/*
	*	TWAIN Source code:
	*	Copyright (C) '91-'93 TWAIN Working Group: 
	*	Aldus, Caere, Eastman-Kodak, Logitech,
	*	Hewlett-Packard Corporations.
	*	All rights reserved.
*
*	Copyright © 1998 TWAIN Working Group: Adobe Systems Incorporated, 
*	Canon Information Systems, Eastman Kodak Company, 
*	Fujitsu Computer Products of America, Genoa Technology, 
*	Hewlett-Packard Company, Intel Corporation, Kofax Image Products, 
*	JFL Peripheral Solutions Inc., Ricoh Corporation, and Xerox Corporation.  
*	All rights reserved.
*
*/ 
            
#include <windows.h>        // req. windows types and for dc.h  
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "..\\twcommon\\twain.h"  // req. for TWAIN types
#include "..\\twcommon\\twndebug.h"

#include "twd_type.h"       // function prototypes for Source
#include "twd_com.h"        // common lib of container routines
//#include "caps.h"
#include "dscaps.h"
#include "msgbox.h"
#include "Dlgproc.h"
#include "resource.h"

#include "scanner.h"


void LogMessage(char msg[]);

// huge is invalid under WIN32.
#ifdef WIN32
	#define TW_HUGE
#else
	#define TW_HUGE huge
#endif

// to allow compatibility between 32 and 16 bit _llseek
#ifndef WIN32
	#define FILE_BEGIN      0
	#define FILE_CURRENT    1
	#define FILE_END        2
#endif
// HandleCapMessage(PTWMSG, TW_UINT16 );
extern UINT TimerOut;
extern HANDLE hImageDlg;
// Local function prototypes -- need to move these to a header
BOOL CopyFileEx(char source[],char dest[],BOOL notify);
BOOL SetSourceControl();
//void LogMessage(char msg[]);
extern char SourceControl[MAX_TWPATH];    
        
// Global of the app's window handle 
HWND hAppWnd;               // global handle to applications window
int DSState= STATE3;        // global state machine place holder
TW_STATUS    DSStatus;      // status structure for errors

char filename[256];
BOOL FeederEnabled = FALSE;
TW_PALETTE8 hPal;


MAPPER ControlMap[] =
{
	{DAT_IDENTITY,      IdentityMsg},
	{DAT_USERINTERFACE, InterfaceMsg},
	{DAT_CAPABILITY,    CapabilityMsg},
	{DAT_STATUS,        StatusMsg},
	{DAT_PENDINGXFERS,  PendingXferMsg},
	{DAT_SETUPMEMXFER,  SetupMemXferMsg},
	{DAT_SETUPFILEXFER, SetupFileXferMsg},
	{DAT_XFERGROUP,     XferGroupMsg},
	// THIS MUST BE LAST
	{ 0,                NULL }
};

MAPPER ImageMap[] =
{
	{DAT_IMAGEINFO,          ImageInfoMsg},
	{DAT_IMAGELAYOUT,        ImageLayoutMsg},
	{DAT_IMAGEMEMXFER,       ImageMemXferMsg},
	{DAT_IMAGENATIVEXFER,    ImageNativeXferMsg},
	{DAT_IMAGEFILEXFER,      ImageFileXferMsg},
	{DAT_PALETTE8,           Palette8Msg},
	{DAT_GRAYRESPONSE,		   GrayResponseMsg},
	{DAT_RGBRESPONSE,		     RGBResponseMsg},
	{DAT_CIECOLOR,			     CIEColorMsg},
	{DAT_JPEGCOMPRESSION,	   JPEGCompressionMsg},
	// THIS MUST BE LAST
	{0,                      NULL}
};

// Global variables 
extern HANDLE          hDSInst;             // Keep track of who we are
char            DisplayFileName[80]; // file select by user


// Global variables - Source Environment 
extern TW_IDENTITY     dsIdentity;          // Who we are
extern TW_IDENTITY     appIdentity;         // To keep track of who's driving us

// Global variables - Image Info 
HANDLE      hImageDlg;                      // Handle to the Data source dialog
FARPROC     lpfnopenfileLB;
FARPROC     lpfnSettingDlgProc ;

/*
	*	FUNCTION: Dispatch
	*	ARGS:    pTWMsg  ptr to the message struct passed to DS_ENTRY
	*					 pMapper ptr to a struct containing ptr's to functions
	*					 DAT     DAT_xxxx that you are 'switching' on 
	*	RETURNS: condition code
	*	NOTES:   This function will dispatch the correct function based on the
	*					 struct identifier.  Searches down through the given Map for the
	*					 matching DAT_xxxx type.
*/

TW_UINT16 Dispatch (PTWMSG pTWMsg, PMAPPER pMapper, TW_UINT16 DAT)
{
	while (pMapper->pFunc != NULL)
	{
		if (pMapper->ID == DAT)
		{
			return(*pMapper->pFunc)(pTWMsg);
		}
		++pMapper;
	}
	DSStatus.ConditionCode = TWCC_BADPROTOCOL;
	return (TWRC_FAILURE);
} // Dispatch

/*
	*	FUNCTION: IdentityMsg
	*	ARGS:    pTWMsg  message struct, same as list of parms in DS_ENTRY call
	*	RETURNS: twRC TWAIN status response
	*	NOTES:   Handle the parse of: (from DAT_IDENTITY msg's)
	*							 - MSG_OPENDS    open the indicated Source
	*							 - MSG_CLOSEDS   close the indicated Source
	*							 - MSG_GET       get identity struct of Source
	*							 - error         error message
*/

TW_UINT16 IdentityMsg  (PTWMSG pTWMsg)
{
	TW_UINT16 twRc = TWRC_SUCCESS;


	switch (pTWMsg->MSG)
	{
		case MSG_OPENDS:
			OpenDS(pTWMsg);
			break;

		case MSG_CLOSEDS:
			CloseDS();
			break;

		case MSG_GET:  // tell the SM who you are valid 3--7 states
			*(pTW_IDENTITY)pTWMsg->pData = dsIdentity;
			break;

		default:
			DSStatus.ConditionCode = TWCC_BADPROTOCOL;
			twRc = TWRC_FAILURE;
			DS_Error (E_IDENTITYMSG, DSStatus.ConditionCode);
			break;
	}
	return twRc;
} // IdentityMsg



/*
	*	FUNCTION: InterfaceMsg
	*	ARGS:    pTWMsg  message struct, same as list of parms in DS_ENTRY call
	*	RETURNS: twRC TWAIN status response
	*	NOTES:   Handle the parse of: (from DAT_USERINTERFACE msg's)
	*							- MSG_ENABLEDS  enable the open Source
	*						  - MSG_DISABLEDS disable the open Source
	*							- errors        error message
*/

TW_UINT16 InterfaceMsg (PTWMSG pTWMsg)
{
	TW_UINT16 twRc = TWRC_SUCCESS;

	switch (pTWMsg->MSG)
	{
		case MSG_ENABLEDS:
			twRc = EnableDS((pTW_USERINTERFACE)pTWMsg->pData);
			break;

		case MSG_DISABLEDS:
			twRc = DisableDS((pTW_USERINTERFACE)pTWMsg->pData);
			break;

		default:
			DSStatus.ConditionCode = TWCC_BADPROTOCOL;
			twRc = TWRC_FAILURE;
			DS_Error (E_INTERFACEMSG, DSStatus.ConditionCode);     
			break;
	}
	return twRc;
}  



/*
	*	FUNCTION: EnableDS
	*	ARGS:    pUI ptr to user interface struct
	*	RETURNS: twRC TWAIN status response
	*							- TWRC_SUCCESS
	*								- TWRC_FAILURE
	*										- TWCC_SEQERROR
	*											- TWCC_LOWMEMORY
	*	NOTES:   
	*	COMMENTS:    Valid states: 4, transitions to state 5 iff successful.
	*							ShowUI bit 1=Source user interface; 0=app supplied
	*							hParent field should contain a valid windows handle iff
	*							you are implementing a modal user interface were the hparent
	*							window will be disabled during access to the user IF dialog
	*							via EnableWindow(hParent, False).  Else use hParent == NULL
	*							for Modeless dialogs, see chapter 5.
*/    

TW_UINT16 EnableDS (pTW_USERINTERFACE pUI)
{
	TW_UINT16 twRc = TWRC_SUCCESS;

	if (DSState==STATE4)
	{
		int nError = 0;
		// make this a global for later
		hAppWnd = pUI->hParent;  // need to know the app window's handle
		// Make a thunk for windows call back function

		hImageDlg = CreateDialog(hDSInst, MAKEINTRESOURCE(IDD_SETTINGSDLG), pUI->hParent, SettingsDlgProc);
		nError = GetLastError();
		TRACE("Last error is %d.\n", nError);
		 
		if(!hImageDlg)
		{
			DSStatus.ConditionCode = TWCC_LOWMEMORY;
			twRc = TWRC_FAILURE;
			DS_Error (E_ENABLEDS, DSStatus.ConditionCode);
		}
		// iff both operations succeed
	  else 
		{
			// Show the window, IFF user asks for it!
			if (pUI->ShowUI)
			{
				ShowWindow(hImageDlg, SW_SHOW);
				DSState=STATE5;
			}
			else
			{
				NotifyXferReady();
				/*
				*	Change to NotifyXferReady later
				*/
				DSState=STATE6;
			}
		}
	}
	else
	{
		DSStatus.ConditionCode = TWCC_SEQERROR;
		twRc = TWRC_FAILURE;
		DS_Error (E_ENABLEDS, DSStatus.ConditionCode);
	}

	return twRc;
} 



/*
	*	FUNCTION: DisableDS
	*	ARGS:    pUI     ptr to user interface structure
	*	RETURNS: TWRC_SUCCESS
	*					TWRC_FAILURE
	*						 TWCC_SEQERROR
	*	NOTES:   Valid states: 5 through 7, transitions to 4 if successful.  The 
	*	App sends down this call when the Source returns the TWMessage "MSG_CLOSEDSREQ".
	*	ShowUI bit 1=Source user interface; 0=app supplied
	*	hParent field should contain a valid windows handle
*/

TW_UINT16 DisableDS (pTW_USERINTERFACE pUI)
{
	TW_UINT16 twRc = TWRC_SUCCESS;

	switch (DSState)
	{
		case STATE5:
		case STATE6:
		case STATE7:
			// Free up the window.
			if (hImageDlg)
			{
				DestroyWindow(hImageDlg);
			}

			// transition to new state
			DSState=STATE4; 
			break;

		default:
			DSStatus.ConditionCode = TWCC_SEQERROR;
			twRc = TWRC_FAILURE;
			DS_Error (E_DISABLEDS, DSStatus.ConditionCode);
	}
	return twRc;
} 


/*
	*	FUNCTION: OpenDS
	*	ARGS:    pTWMsg  message struct, same as list of parms in DS_ENTRY call
	*	RETURNS: status code
	*	NOTES:   This function will 'open' the Source, register it with the SM
	*					 Valid state: 3 transitions to state 4 on success.
	*	TODO: to support simultaneous connections to more than one app. the Source
	*	should make a copy of it's states especially the identity structure of the 
	*	calling app.
*/

TW_UINT16 OpenDS (PTWMSG pTWMsg)
{
	TW_UINT16 twRc = TWRC_SUCCESS;
	OFSTRUCT of;

	// The log is only good for one session of twaining to this source before
	// it is reset
	OpenFile("c:\\twsrc.log", &of, OF_DELETE);


	if (DSState==STATE3)
	{
		// Only allow one connection at a time.  If AppIdentity.Id is set,
		// then there is already a connection.  If not, then remember who
		// we're connected to so we can send messages to it.
		if (appIdentity.Id)
		{
			twRc = TWRC_FAILURE;
			DSStatus.ConditionCode = TWCC_MAXCONNECTIONS;
			DS_Error (E_OPENDS, DSStatus.ConditionCode);    
		}
		else 
		{
			// This is to ensure that how we identify ourselves to the
			// SM is consistent with what it is keeping around.
			appIdentity = *pTWMsg->pSrc;
			dsIdentity = *(pTW_IDENTITY)pTWMsg->pData;
		}

		DSState = STATE4;
		InitializeCaps();
	}
	else 
	{
		twRc = TWRC_FAILURE;
		DSStatus.ConditionCode = TWCC_SEQERROR;
		DS_Error (E_OPENDS, DSStatus.ConditionCode);    
	}

	return twRc;
} // end OpenDS()



/*
	*	FUNCTION: CloseDS
	*	ARGS:    none
	*	RETURNS: status code 
	*	NOTES:   This function will close the Source.
*/

TW_UINT16 CloseDS(VOID)
{
	TW_UINT16 twRc = TWRC_SUCCESS;

	switch (DSState)
	{
		case STATE4:
		case STATE5:
		case STATE6:
		case STATE7:
			// clear out current connection
			appIdentity.Id = 0;

			// Set the DS state
			DSState = STATE3;
			FreeCaps();
			break;

		default:
			twRc = TWRC_FAILURE;
			DSStatus.ConditionCode = TWCC_SEQERROR;
			DS_Error (E_CLOSEDS, DSStatus.ConditionCode);
			break;
	}   

	return twRc;
} // end CloseDS()



/*
*	FUNCTION: CapabilityMsg
*	ARGS:    pTWMsg  message struct, same as list of parms in DS_ENTRY call
*	RETURNS: status code
*	NOTES: for the MSG_SET side of Source
*	The application will create the container and destroy the container.
*	The Source will extract the value from the container 1st and use this value
*	to modify it's local value for the CAP.
*	The Source needs to be able to extract values from all of the different
*	container types and load as many as possible into it's local containers
*	for the particular capabilty.
*	NOTES: for the MSG_GET side of Source
*	The Source will create the container and the App will destroy container.
*	The App will extract the value from the container 1st and use this value
*	to modify it's local state or value for this CAP.
*	The App extracts values from all different container types and also
*	matches up these cap's when appropriate to find the value or values which
*	best suit the current mode of the application.
*	NOTES: for both App and Source in general:
*	You don't know what type of container you will be getting for any
*	particular cap until you get it.  
*/
 
TW_UINT16 CapabilityMsg (PTWMSG pTWMsg)
{
	ASSERT(pTWMsg);
	return HandleCapMessage( pTWMsg, &DSStatus.ConditionCode);
}

/*
	*	FUNCTION: StatusMsg
	*	ARGS:    pTWMsg  message struct, same as list of parms in DS_ENTRY call
	*	RETURNS: status code
	*	NOTES:   Valid states 3 through 7.  Source fills in all fields of pDSMStatus 
	*	and clears it's condition code.
*/   

TW_UINT16 StatusMsg (PTWMSG pTWMsg)
{
	TW_UINT16 twRc = TWRC_SUCCESS;

	switch (pTWMsg->MSG)
	{
		case MSG_GET:
			*(pTW_STATUS)pTWMsg->pData = DSStatus;        // copy all fields
			DSStatus.ConditionCode = TWCC_SUCCESS;        // clear condition code
			break;

			// Invalid triplet
		default:
			DSStatus.ConditionCode = TWCC_BADPROTOCOL;
			twRc = TWRC_FAILURE;
			DS_Error (E_STATUSMSG, DSStatus.ConditionCode);
			break;
	}

	return twRc;
} // StatusMsg()



/*
	*	FUNCTION: Msg
	*	ARGS:    pTWMsg  message struct, same as list of parms in DS_ENTRY call
	*	RETURNS: TWRC_SUCCESS
	*				    TWRC_FAILURE
	*							TWCC_SEQERROR
	*							 TWCC_BADPROTOCOL        // debug ONLY
	*	NOTES:   Valid states: 4 -- 7.
*/

TW_UINT16 PendingXferMsg (PTWMSG pTWMsg)
{
	TW_UINT16 twRc = TWRC_SUCCESS;
	TW_INT16 PendingImages = 0;

	switch (pTWMsg->MSG)
	{
		case MSG_GET:                    // valid states 4 -- 7
			switch (DSState)
			{
				case STATE4:
				case STATE5:
				case STATE6:
					if(GetCurrentFeederEnabled())
					{
						((pTW_PENDINGXFERS)pTWMsg->pData)->Count=IsDocumentInFeeder();
					}
					else
					{
						((pTW_PENDINGXFERS)pTWMsg->pData)->Count=1;
					}
					break;
				case STATE7:
					if(GetCurrentFeederEnabled())
					{
						((pTW_PENDINGXFERS)pTWMsg->pData)->Count=IsDocumentInFeeder();
					}
					else
					{
						((pTW_PENDINGXFERS)pTWMsg->pData)->Count=0;
					}
					break;

				default:
					// fail get due to  seqerror
					DSStatus.ConditionCode = TWCC_SEQERROR;
					twRc = TWRC_FAILURE;
					DS_Error (E_PENDINGXFERMSG, DSStatus.ConditionCode);
					break; //break for any of the above states.
			}
			break;	//break for MSG_GET

		case MSG_ENDXFER: // clears ONLY current transfer, valid 6 or 7
			StopScan();
			/*
			* Assume there are no more images to scan
			*/
			((pTW_PENDINGXFERS)pTWMsg->pData)->Count = 0;
			if(GetCurrentFeederEnabled())
			{
				((pTW_PENDINGXFERS)pTWMsg->pData)->Count = (TW_UINT16)(IsDocumentInFeeder()?-1:0);
			}

			if((DSState==STATE7))
			{
				DSState = STATE6;
			}
			else
			{
				DSStatus.ConditionCode = TWCC_SEQERROR;
				twRc = TWRC_FAILURE;
				DS_Error (E_PENDINGXFERMSG, DSStatus.ConditionCode);
			}
			break;

		case MSG_RESET: // clears ALL transfers, valid 6 or 7
			if ((DSState==STATE6) || (DSState==STATE7))
			{
				/*
				* Stop the current scan, clear the rest of the pages, return no images left
				* Return to state 5
				*/
				StopScan();
				ClearFeeder();
				DSState = STATE5;
				((pTW_PENDINGXFERS)pTWMsg->pData)->Count = 0;
			}
			else 
			{
				DSStatus.ConditionCode = TWCC_SEQERROR;
				twRc = TWRC_FAILURE;
				DS_Error (E_PENDINGXFERMSG, DSStatus.ConditionCode);
			}
			break;

		default:
			DSStatus.ConditionCode = TWCC_BADPROTOCOL;
			twRc = TWRC_FAILURE;
			DS_Error (E_PENDINGXFERMSG, DSStatus.ConditionCode);
			break;
	}
	return twRc;
} // PendingXferMsg


/*
	*	FUNCTION: SetupMemXferMsg
	*	ARGS:    pTWMsg  message struct, same as list of parms in DS_ENTRY call
	*	RETURNS: TWRC_SUCCESS
	*			TWRC_FAILURE
	*				TWCC_SEQERROR
	*					TWCC_BADPROTOCOL        // debug ONLY
	*	NOTES:   Valid states: 4 -- 6.
	*	Source fills in the following:
*/

TW_UINT16 SetupMemXferMsg (PTWMSG pTWMsg)
{
	TW_UINT16 twRc = TWRC_SUCCESS;

	switch (DSState)
	{
		case STATE4:
		case STATE5:
		case STATE6:
			switch (pTWMsg->MSG)
			{
				case MSG_GET:
					((pTW_SETUPMEMXFER)pTWMsg->pData)->MinBufSize = 16 * 1024;
					
					/*
					*	We use 64k - 1 since a 16bit application may call us and
					* have some problems allocating such a buffer with malloc
					*/
					((pTW_SETUPMEMXFER)pTWMsg->pData)->MaxBufSize = (64 * 1024) - 1;
					((pTW_SETUPMEMXFER)pTWMsg->pData)->Preferred = (64 * 1024) - 1;
					break;
				default:
					DSStatus.ConditionCode = TWCC_BADPROTOCOL;
					twRc = TWRC_FAILURE;
					DS_Error (E_SETUPMEMXFER, DSStatus.ConditionCode);
					break;
			}
			break; 

		default: //case 4,5,6
			DSStatus.ConditionCode = TWCC_SEQERROR;
			twRc = TWRC_FAILURE;
			DS_Error (E_SETUPMEMXFER, DSStatus.ConditionCode);
			break;
	}
	return twRc;
} 


/*
	*	FUNCTION: SetupFileXferMsg
	*	ARGS:    pTWMsg  message struct, same as list of parms in DS_ENTRY call
	*	RETURNS: TWRC_SUCCESS
	*		TWRC_FAILURE
	*			TWCC_SEQERROR
	*				TWCC_BADVALUE
	*					TWCC_BADPROTOCOL        // not supportted
	*	NOTES:  Valid states: 4 -- 6.
*/ 

TW_UINT16 SetupFileXferMsg (PTWMSG pTWMsg)
{
	pTW_SETUPFILEXFER pSetup;
	TW_UINT16 twRc = TWRC_SUCCESS;
	HFILE hFile;
	OFSTRUCT         of;

	switch (DSState)
	{
		case STATE4:
		case STATE5:
		case STATE6:           
			switch (pTWMsg->MSG)
			{
				case MSG_SET:
					pSetup = (pTW_SETUPFILEXFER)(pTWMsg->pData);
					lstrcpyn(filename,pSetup->FileName,sizeof(filename));
            
					// check that the file is valid
					if ((hFile = OpenFile(filename, &of, OF_EXIST)) == HFILE_ERROR)
					{			
						if ((hFile = OpenFile(filename,&of,OF_CREATE))==HFILE_ERROR)
						{
							DSStatus.ConditionCode = TWCC_BADVALUE;
							twRc = TWRC_FAILURE;    
							DS_Error ("Unable to create file <<DAT_SETUPFILEXFER>>", DSStatus.ConditionCode);
						}
						else
						{
							_lclose(hFile);
						}
					}    				
					break; 

				case MSG_GETDEFAULT:
				case MSG_GETCURRENT:
				case MSG_GET:
					pSetup = (pTW_SETUPFILEXFER)(pTWMsg->pData);
					lstrcpyn(pSetup->FileName,"c:\\twain.bmp",sizeof(pSetup->FileName));
					pSetup->Format = TWFF_BMP;
					pSetup->VRefNum = 0;
					break;
				case MSG_RESET:
				default:
					DSStatus.ConditionCode = TWCC_BADPROTOCOL;
					twRc = TWRC_FAILURE;
					DS_Error (E_SETUPFILEXFER, DSStatus.ConditionCode);
					break;
			}	//inside switch ends here
			break;

		default:
			DSStatus.ConditionCode = TWCC_SEQERROR;
			twRc = TWRC_FAILURE;
			DS_Error (E_SETUPFILEXFER, DSStatus.ConditionCode);
	}//external switch ends here 

	return twRc;
} 


/*
	*	FUNCTION: XferGroupMsg
	*	ARGS:    pTWMsg  message struct, same as list of parms in DS_ENTRY call 
	*	RETURNS: status code
	*	NOTES:   Valid states: 4 -- 6.  Since only the Data Group is supported 
	*	under Version 1.0 of the specification the application should only use 
	*	MSG_GET.
*/ 

TW_UINT16 XferGroupMsg (PTWMSG pTWMsg)
{
	TW_UINT16 twRc = TWRC_SUCCESS;

	switch (DSState)
	{
		case STATE4:
		case STATE5:
		case STATE6:   
			switch (pTWMsg->MSG)
			{
				case MSG_GET:
				case MSG_GETDEFAULT:
				case MSG_GETCURRENT:
				case MSG_RESET:
					*(pTW_UINT16)pTWMsg->pData = DG_IMAGE;
					break;

		    case MSG_SET:
			    // ONLY images are define in current spec so you can not change DG_ type
			    break;

		    default:
		    	DSStatus.ConditionCode = TWCC_BADPROTOCOL;
					twRc = TWRC_FAILURE;
					DS_Error (E_XFERGROUP, DSStatus.ConditionCode);
					break;
				}	//Inside switch
				break;

		default:  
			DSStatus.ConditionCode = TWCC_SEQERROR;
			twRc = TWRC_FAILURE;
			DS_Error (E_XFERGROUP, DSStatus.ConditionCode);
			break;
	}//outside switch 
	return twRc;
} // XferGroupMsg()


/*
	*	FUNCTION: Palette8Msg
	*	ARGS:    pTWMsg  message struct, same as list of parms in DS_ENTRY call
	*	RETURNS: status code
	*	NOTES:   
*/ 

TW_UINT16 Palette8Msg (PTWMSG pTWMsg)
{
	TW_UINT16 twRc = TWRC_SUCCESS;
	HANDLE bmp;
	LPBITMAPINFO pdib;
	int index;

	switch (DSState)
	{
		case STATE4:
		case STATE5:
		case STATE6:   
			switch (pTWMsg->MSG)
			{
				case MSG_GET:
					bmp = ScanToDIB();
					pdib = (LPBITMAPINFO)GlobalLock(bmp);

					((pTW_PALETTE8)pTWMsg->pData)->NumColors = (TW_UINT16)pdib->bmiHeader.biClrUsed;
					((pTW_PALETTE8)pTWMsg->pData)->PaletteType = TWPA_RGB;
                
					for (index=0;index<(int)pdib->bmiHeader.biClrUsed;index++)
					{
						((pTW_PALETTE8)pTWMsg->pData)->Colors[index].Channel1 = pdib->bmiColors[index].rgbRed;
						((pTW_PALETTE8)pTWMsg->pData)->Colors[index].Channel2 = pdib->bmiColors[index].rgbGreen;
						((pTW_PALETTE8)pTWMsg->pData)->Colors[index].Channel3 = pdib->bmiColors[index].rgbBlue;
						((pTW_PALETTE8)pTWMsg->pData)->Colors[index].Index = index;
					}

					GlobalUnlock(bmp);
					GlobalFree(bmp);
					break;

				case MSG_GETDEFAULT:
				case MSG_RESET:
				case MSG_SET:
					break;
				default:
					DSStatus.ConditionCode = TWCC_BADPROTOCOL;
					twRc = TWRC_FAILURE;
					DS_Error (E_PALETTE8, DSStatus.ConditionCode);
					break;
			}//internal switch
			break;

		default:  
			DSStatus.ConditionCode = TWCC_SEQERROR;
			twRc = TWRC_FAILURE;
			DS_Error (E_PALETTE8, DSStatus.ConditionCode);
			break;
	} 

	return twRc;
} // Palette8Msg()


/***********************************************
 Shouldn't be some comments in here (Berhane)?
	That is why inconsistent look fo comments.
************************************************/
typedef struct 
{
	TW_ELEMENT8 Response[256];
}BetterGrayResponseStruct, *pBetterGrayResponseStruct;

static BetterGrayResponseStruct GrayResponse;

TW_UINT16 GrayResponseMsg (PTWMSG pTWMsg)
{
	TW_UINT8 x = 0;

	if(pTWMsg->MSG == MSG_RESET)
	{
		for(x=0;x<256;x++)
		{
			/*
			*	Reset to our identity 8bit response curve
			*/
			GrayResponse.Response[x].Index = x;
			GrayResponse.Response[x].Channel1 = /* tristimulous for 8bit gray is useless */
				GrayResponse.Response[x].Channel2 =
					GrayResponse.Response[x].Channel3 = x;
		}
	}
	else if(pTWMsg->MSG == MSG_SET)
	{
		/*
		*	Set to the requested response curve
		*/
		GrayResponse = *((BetterGrayResponseStruct*)pTWMsg->pData);
	}
	return(TWRC_SUCCESS);
}

/*
*	RGBResponse -- Note:  Uses the GrayResponse structure which is identical to the
*	RGBRESPONSE structure in TWAIN 1.5.4
*/
typedef struct 
{
	TW_ELEMENT8 Response[256];
}BetterRGBResponseStruct, *pBetterRGBResponseStruct;

static BetterRGBResponseStruct RGBResponse;

TW_UINT16 RGBResponseMsg (PTWMSG pTWMsg)
{	
	TW_UINT8 x = 0;

	if(pTWMsg->MSG == MSG_RESET)
	{
		/*
		*	Reset to our 24bit identity reponse curve
		*/
		for(x=0;x<256;x++)
		{
			RGBResponse.Response[x].Index = x;
			RGBResponse.Response[x].Channel1 = /* Red */
				RGBResponse.Response[x].Channel2 =	/* Green */
					RGBResponse.Response[x].Channel3 = x; /* Blue */
		}
	}
	else if(pTWMsg->MSG == MSG_SET)
	{
		/*
		* Set to the requested response curve
		*/
		RGBResponse = *((BetterRGBResponseStruct*)pTWMsg->pData);
	}
	
	return(TWRC_SUCCESS);
}


/*
	* CIEColorMsg --
*/

TW_UINT16 CIEColorMsg (PTWMSG pTWMsg)
{	
	TW_HANDLE phandle;

	((pTW_CIECOLOR)pTWMsg->pData)->ColorSpace = TWPT_RGB;
	((pTW_CIECOLOR)pTWMsg->pData)->LowEndian = 0;
	((pTW_CIECOLOR)pTWMsg->pData)->DeviceDependent = TRUE;
	((pTW_CIECOLOR)pTWMsg->pData)->VersionNumber = 0;

	// Transform stage ABC
	((pTW_CIECOLOR)pTWMsg->pData)->StageABC.Decode[0].StartIn = FloatToFIX32((float)0.1);

	((pTW_CIECOLOR)pTWMsg->pData)->StageABC.Mix[0][0] = FloatToFIX32((float)0.0);
	((pTW_CIECOLOR)pTWMsg->pData)->StageABC.Mix[0][1] = FloatToFIX32((float)0.1);
	((pTW_CIECOLOR)pTWMsg->pData)->StageABC.Mix[0][2] = FloatToFIX32((float)0.2);
	((pTW_CIECOLOR)pTWMsg->pData)->StageABC.Mix[1][0] = FloatToFIX32((float)1.0);
	((pTW_CIECOLOR)pTWMsg->pData)->StageABC.Mix[1][1] = FloatToFIX32((float)1.1);
	((pTW_CIECOLOR)pTWMsg->pData)->StageABC.Mix[1][2] = FloatToFIX32((float)1.2);
	((pTW_CIECOLOR)pTWMsg->pData)->StageABC.Mix[2][0] = FloatToFIX32((float)2.0);
	((pTW_CIECOLOR)pTWMsg->pData)->StageABC.Mix[2][1] = FloatToFIX32((float)2.1);
	((pTW_CIECOLOR)pTWMsg->pData)->StageABC.Mix[2][2] = FloatToFIX32((float)2.2);

	//Transform Stage LMN
	((pTW_CIECOLOR)pTWMsg->pData)->StageLMN.Decode[0].StartIn = FloatToFIX32((float)1.1);

	((pTW_CIECOLOR)pTWMsg->pData)->StageLMN.Mix[0][0] = FloatToFIX32((float)0.0);
	((pTW_CIECOLOR)pTWMsg->pData)->StageLMN.Mix[0][1] = FloatToFIX32((float)0.1);
	((pTW_CIECOLOR)pTWMsg->pData)->StageLMN.Mix[0][2] = FloatToFIX32((float)0.2);
	((pTW_CIECOLOR)pTWMsg->pData)->StageLMN.Mix[1][0] = FloatToFIX32((float)1.0);
	((pTW_CIECOLOR)pTWMsg->pData)->StageLMN.Mix[1][1] = FloatToFIX32((float)1.1);
	((pTW_CIECOLOR)pTWMsg->pData)->StageLMN.Mix[1][2] = FloatToFIX32((float)1.2);
	((pTW_CIECOLOR)pTWMsg->pData)->StageLMN.Mix[2][0] = FloatToFIX32((float)2.0);
	((pTW_CIECOLOR)pTWMsg->pData)->StageLMN.Mix[2][1] = FloatToFIX32((float)2.1);
	((pTW_CIECOLOR)pTWMsg->pData)->StageLMN.Mix[2][2] = FloatToFIX32((float)2.2);

	// CIEPoint
	((pTW_CIECOLOR)pTWMsg->pData)->WhitePoint.X = FloatToFIX32((float)4.4);
	((pTW_CIECOLOR)pTWMsg->pData)->WhitePoint.Y = FloatToFIX32((float)5.5);
	((pTW_CIECOLOR)pTWMsg->pData)->WhitePoint.Z = FloatToFIX32((float)6.6);

	// CIEPoint
	((pTW_CIECOLOR)pTWMsg->pData)->BlackPoint.X = FloatToFIX32((float)1.1);
	((pTW_CIECOLOR)pTWMsg->pData)->BlackPoint.Y = FloatToFIX32((float)5.5);
	((pTW_CIECOLOR)pTWMsg->pData)->BlackPoint.Z = FloatToFIX32((float)6.6);

	// CIEPoint
	((pTW_CIECOLOR)pTWMsg->pData)->WhitePaper.X = FloatToFIX32((float)1.1);
	((pTW_CIECOLOR)pTWMsg->pData)->WhitePaper.Y = FloatToFIX32((float)5.5);
	((pTW_CIECOLOR)pTWMsg->pData)->WhitePaper.Z = FloatToFIX32((float)6.6);

	// CIEPoint
	((pTW_CIECOLOR)pTWMsg->pData)->BlackInk.X = FloatToFIX32((float)1.1);
	((pTW_CIECOLOR)pTWMsg->pData)->BlackInk.Y = FloatToFIX32((float)5.5);
	((pTW_CIECOLOR)pTWMsg->pData)->BlackInk.Z = FloatToFIX32((float)6.6);

	phandle = GlobalAlloc(GHND,sizeof(TW_FIX32)*4);
	//	((pTW_CIECOLOR)pTWMsg->pData)->Samples[0] = (pTW_FIX32)GlobalLock(phandle);

	((pTW_CIECOLOR)pTWMsg->pData)->Samples[0] = FloatToFIX32((float)99.99);
	((pTW_CIECOLOR)pTWMsg->pData)->Samples[1] = FloatToFIX32((float)98.98);
	((pTW_CIECOLOR)pTWMsg->pData)->Samples[2] = FloatToFIX32((float)97.97);
	((pTW_CIECOLOR)pTWMsg->pData)->Samples[3] = FloatToFIX32((float)96.96);

	//	GlobalUnlock(phandle);

	return(TWRC_SUCCESS);
}


/*********************************************
 Shouldn't be some comments in here?
 That is why it is inconsistent comments look.
**********************************************/

TW_UINT16 JPEGCompressionMsg (PTWMSG pTWMsg)
{
	return(TWRC_SUCCESS);
}


/*
	*	FUNCTION: ImageInfoMsg
	*	ARGS:    pTWMsg  message struct, same as list of parms in DS_ENTRY call
	*	RETURNS: 
	*		TWRC_SUCCESS      
	*			TWRC_FAILURE
	*				TWCC_SEQERROR
	*	NOTES:  Valid states 6.  Fills in all fields of imageinfo struct.  Get 
	*	info on pending image transfer.
	*	Fix the return codes, suppose to be pending, in which case it has already 
	*	been cleared for take off an none of the other message should occur at 
	*	this time !!!
*/

TW_UINT16 ImageInfoMsg (PTWMSG pTWMsg)
{
	TW_UINT16           twRC = TWRC_SUCCESS;
	pTW_IMAGEINFO       pII;
	short               i;

	if (DSState==STATE6)
	{
		switch (pTWMsg->MSG)
		{
			case MSG_GET:
				// set up local pointer to IMAGEINFO struct
				pII = (pTW_IMAGEINFO)pTWMsg->pData;

				// use data in .BMP file to best fill the ImageInfo struct
				pII->XResolution = FloatToFIX32(GetCurrentXResolution());
				pII->YResolution = FloatToFIX32(GetCurrentXResolution());

				pII->ImageWidth   = (TW_UINT16)(8.5F * GetCurrentXResolution());
				
				if(GetCurrentSupportedSizes()==TWSS_USLETTER)
				{
					pII->ImageLength  = (TW_UINT16)(11.0F * GetCurrentXResolution());
				}
				else
				{
					pII->ImageLength  = (TW_UINT16)(14.0F * GetCurrentXResolution());
				}

				pII->PixelType = GetCurrentPixelType(); 
				pII->BitsPerPixel = GetCurrentBitDepth(pII->PixelType);
				pII->Planar = FALSE;
				pII->Compression  = TWCP_NONE;

				// clear first
				for (i = 0; i <= 7; i++)
				{
					pII->BitsPerSample[i] = 0;
				}

				// assumes color .BMP are 1, 4, 8, 24 as spec. by MS
				if (pII->BitsPerPixel<24)
				{
					pII->SamplesPerPixel = 1;
					pII->BitsPerSample[0] = pII->BitsPerPixel;
				}
			  else 
				{
					pII->SamplesPerPixel = 3;
					for (i = 0; i <= 3; i++)
					{
						pII->BitsPerSample[i] = 8;
					}
				}

				break;

			default:
				DSStatus.ConditionCode = TWCC_BADPROTOCOL;
				twRC = TWRC_FAILURE;
				DS_Error (E_IMAGEINFOMSG, DSStatus.ConditionCode);
				break;
		}	//switch closed
	}	//#first if
	else
	{
		DSStatus.ConditionCode = TWCC_SEQERROR;
		twRC = TWRC_FAILURE;
		DS_Error (E_IMAGEINFOMSG, DSStatus.ConditionCode);
	}

	return twRC;
} 


/*
	*	FUNCTION: ImageLayoutMsg
	*	ARGS:    pTWMsg  message struct, same as list of parms in DS_ENTRY call 
	*	RETURNS: status code 
	*	NOTES:   This function is is supported to the extent that it will
	*	return success for each call.
*/ 

TW_UINT16 ImageLayoutMsg (PTWMSG pTWMsg)
{
	TW_UINT16 twRc = TWRC_SUCCESS;
	TW_FIX32 fix32;

	switch (pTWMsg->MSG)
	{
		case MSG_GET:
		case MSG_GETDEFAULT:
		case MSG_GETCURRENT:
			fix32.Whole = 22;
			fix32.Frac = 33;

			((pTW_IMAGELAYOUT)pTWMsg->pData)->Frame.Top = fix32;
			((pTW_IMAGELAYOUT)pTWMsg->pData)->Frame.Left = fix32;
			((pTW_IMAGELAYOUT)pTWMsg->pData)->Frame.Right = fix32;
			((pTW_IMAGELAYOUT)pTWMsg->pData)->Frame.Bottom = fix32;

			((pTW_IMAGELAYOUT)pTWMsg->pData)->DocumentNumber = 4;  
			((pTW_IMAGELAYOUT)pTWMsg->pData)->PageNumber = 88;
			((pTW_IMAGELAYOUT)pTWMsg->pData)->FrameNumber = 1;
			break;

		case MSG_SET:
			break;

		case MSG_RESET:
			break;

		default:
			DSStatus.ConditionCode = TWCC_BADPROTOCOL;
			twRc = TWRC_FAILURE;
			DS_Error (E_IMAGELAYOUTMSG, DSStatus.ConditionCode);
			break;
	}

	return twRc;
} 



/*
	*	FUNCTION: ImageMemXferMsg 
	*	ARGS:    pTWMsg  message struct, same as list of parms in DS_ENTRY call
	*	RETURNS: TWRC_SUCCESS
	*		TWRC_FAILURE
	*			TWCC_BADVALUE
	*				TWCC_SEQERROR
	*					TWRC_XFERDONE
	*						TWRC_CANCEL
*/ 
static DWORD dwLineSize = 0l;
static DWORD dwRowCount = 0l;

TW_UINT16 ImageMemXferMsg (PTWMSG pTWMsg)
{
	TW_UINT16 twRc = TWRC_SUCCESS;
	pTW_IMAGEMEMXFER pMem = (pTW_IMAGEMEMXFER)pTWMsg->pData;


	switch (pTWMsg->MSG)
	{
		case MSG_GET:
			if(DSState==STATE6)
			{
				/*
				*	Initiate the scan on the first read
				*/
				if(!StartScan(&dwLineSize))
				{
					/*
					*	There was a problem starting the scan
					*/
					DSStatus.ConditionCode = TWCC_OPERATIONERROR;
					twRc = TWRC_FAILURE;
					DS_Error (E_IMAGEMEMXFERMSG, DSStatus.ConditionCode);
					break;
				}
			}

			pMem->BytesPerRow = dwLineSize;
			pMem->YOffset = dwRowCount;

			pMem->Columns = (TW_UINT16)(8.5F * GetCurrentXResolution());
			pMem->XOffset = 0;

			//pMem->BytesWritten
			/*
			*	Really should check the memory flags here in case this is a
			* handle, not a pointer
			*/
			{
				BOOL bLastBuffer = FALSE;
				if(!ReadDataBlock(pMem->Memory.TheMem, pMem->Memory.Length, &pMem->BytesWritten, &bLastBuffer))
				{
					DSStatus.ConditionCode = TWCC_OPERATIONERROR;
					twRc = TWRC_FAILURE;
					DS_Error (E_IMAGEMEMXFERMSG, DSStatus.ConditionCode);
					break;
				}

				if(bLastBuffer)
				{
					twRc = TWRC_XFERDONE;
				}
			}

			pMem->Rows = pMem->BytesWritten/pMem->BytesPerRow;
			dwRowCount += pMem->Rows;
			break;
		default:
			DSStatus.ConditionCode = TWCC_BADPROTOCOL;
			twRc = TWRC_FAILURE;
			DS_Error (E_IMAGEMEMXFERMSG, DSStatus.ConditionCode);
			break;
	}	//turn off the switch


	/*
	*	Success means we move to state 7
	*/
	if(twRc != TWRC_FAILURE)
	{
		DSState=STATE7;
	}
	return twRc;
} 

/*
	*	FUNCTION: ImageFileXferMsg 
	*	ARGS:    pTWMsg  message struct, same as list of parms in DS_ENTRY call 
	*	RETURNS: 
	*		TWRC_DONE
	*			TWRC_FAILURE
	*				TWCC_BADVALUE
	*					TWCC_SEQERROR
	*						TWRC_XFERDONE
	*							TWCC_CANCEL 
	*	NOTES:   Valid states: 6 -- 7.  I do not support file transfers.
	*/          

TW_UINT16 ImageFileXferMsg (PTWMSG pTWMsg)
{
	TW_UINT16 twRc = TWRC_FAILURE;
	DWORD dwLineSize = 0l;
	HGLOBAL hDIB = NULL;
	DSStatus.ConditionCode = TWCC_OPERATIONERROR;

	switch (pTWMsg->MSG)
	{
		case MSG_GET:
			StartScan(&dwLineSize);

			/*
			*	File transfer is simply saving a DIB to BMP file
			* (since we are only supporting BMP files now)
			*/
			hDIB = ScanToDIB();
			if(hDIB)
			{
				SaveBitmap(NULL, hDIB, filename);
				GlobalFree(hDIB);
				twRc = TWRC_XFERDONE;
				DSStatus.ConditionCode = TWCC_SUCCESS;
			}
			break;

		default:
			DSStatus.ConditionCode = TWCC_BADPROTOCOL;
			twRc = TWRC_FAILURE;
			DS_Error (E_IMAGELAYOUTMSG, DSStatus.ConditionCode);
			break;
	}

	/*
	*	Success means we move to state 7
	*/
	if(twRc != TWRC_FAILURE)
	{
		DSState=STATE7;
	}
	return twRc;
} 

/*
*	FUNCTION: ImageNativeXferMsg 
*	ARGS:    pTWMsg  message struct, same as list of parms in DS_ENTRY call 
*	RETURNS: TWRC_SUCCESS
*		TWRC_FAILURE
*			TWCC_BADVALUE
*				TWCC_SEQERROR
*					TWRC_XFERDONE
*						TWCC_CANCEL
*							TWCC_BADPROTOCOL         // debug only
*	NOTES:  Valid states: 6 -- 7.  I only support native transfers.
*/          

TW_UINT16 ImageNativeXferMsg (PTWMSG pTWMsg)
{
	TW_UINT16        twRc = TWRC_SUCCESS;
	DWORD dwLineSize = 0l;

	if(DSState==STATE6)
	{
		switch (pTWMsg->MSG)
		{
			case MSG_GET:
				StartScan(&dwLineSize);

				// return hDib in low word
				*(pTW_UINT32)pTWMsg->pData = (TW_UINT32)ScanToDIB();//OpenDIB((LPSTR)DisplayFileName);
				twRc = TWRC_XFERDONE;
				break;

			default:
				DSStatus.ConditionCode = TWCC_BADPROTOCOL;
				twRc = TWRC_FAILURE;
				DS_Error (E_IMAGENATIVEXFERMSG, DSStatus.ConditionCode);
				break;
		}
	}
	else
	{
		DSStatus.ConditionCode = TWCC_SEQERROR;
		twRc = TWRC_FAILURE;
		DS_Error (E_IMAGENATIVEXFERMSG, DSStatus.ConditionCode);
	}

	/*
	*	Success means we move to state 7
	*/
	if(twRc != TWRC_FAILURE)
	{
		DSState=STATE7;
	}
	return twRc;
} 

/*
*	FUNCTION: NotifyCloseDSReq
*	ARGS:    none
*	RETURNS: Indirectly returns a MSG_CLOSEDSREQ to the Application, thru
*	DSM_Entry call.
*/ 
VOID NotifyCloseDSReq (VOID)
{
	HANDLE hDSMDLL = NULL;
	DSMENTRYPROC lpDSM_Entry = NULL;         // entry point to the SM

	#ifdef WIN32
	//SDH - 02/03/95 - New Twain32 dll name for thunker.
	if((hDSMDLL = LoadLibrary("TWAIN_32.DLL")) != NULL)
	#else   // GR: 16 bit version
	if ((hDSMDLL = LoadLibrary("TWAIN.DLL")) != NULL)
	#endif
	{
		if((lpDSM_Entry = (DSMENTRYPROC)GetProcAddress(hDSMDLL, "DSM_Entry")) != NULL)
		{
			(*lpDSM_Entry)(&dsIdentity, &appIdentity, (TW_UINT32)DG_CONTROL, DAT_NULL, MSG_CLOSEDSREQ, (TW_MEMREF) NULL);
		}
		FreeLibrary (hDSMDLL);
	}
	return;
} // NotifyCloseReq()

/*
*  FUNCTION: NotifyXferReady 
*  ARGS:    none 
*  RETURNS: Indirectly returns a MSG_XFERREADY to the Application, thru
*          DSM_Entry call.
*/ 
VOID NotifyXferReady(VOID)
{
	HANDLE hDSMDLL = NULL;
	DSMENTRYPROC lpDSM_Entry = NULL;         // entry point to the SM

	LogMessage("function  NotifyXferReady\r\n");

	// go to xferready state
	DSState=STATE6;

	#ifdef WIN32
  //  SDH - 02/03/95 - New Twain32 dll name for thunker.
  if((hDSMDLL = LoadLibrary("TWAIN_32.DLL")) != NULL)
	#else   // GR: 16 bit version
  if((hDSMDLL = LoadLibrary("TWAIN.DLL")) != NULL)
	#endif
	{
		if((lpDSM_Entry = (DSMENTRYPROC)GetProcAddress(hDSMDLL, "DSM_Entry")) != NULL)
		{
			(*lpDSM_Entry)(&dsIdentity, &appIdentity, (TW_UINT32)DG_CONTROL, DAT_NULL, MSG_XFERREADY, (TW_MEMREF) NULL);
		}
		FreeLibrary (hDSMDLL);
	}
	return;
} // NotifyXferReady()


/*
	*	FUNCTION: DS_Error
	*	ARGS:    *state  ptr to char string of error message
	*		code    condition code TWCC_xxxx 
	*	RETURNS: none, does output a message box
*/

VOID DS_Error(char *state, int code)
{
	char details [180];

	// Put in detail of current operation
	lstrcpy (details, state);

	switch (code)
	{
		case TWCC_BADCAP:
			lstrcat (details, "Bad capabilities error.");
			break;

		case TWCC_BADPROTOCOL:
			lstrcat (details, "Bad protocol error.");
			break;

		case TWCC_BADVALUE:
			lstrcat (details, "Bad value error.");
			break;

		case TWCC_BUMMER:
			lstrcat (details, "Bummer error.");
			break;

		case TWCC_OPERATIONERROR:
			lstrcat (details, "An operation error just occurred.");
			break;

		case TWCC_LOWMEMORY:
			lstrcat (details, "Low Memory Condition encountered, free up some memory and try again.");
			break;

		case TWCC_MAXCONNECTIONS:
			lstrcat (details, "Source is connected to max possible apps.");
			break;

		case TWCC_NODS:
			lstrcat (details, "No Data Source found error.");
			break;

		case TWCC_SEQERROR:
			lstrcat (details, "DG DAT MSG is out of expected sequence.");
			break;

		case TWCC_SUCCESS:
			lstrcat (details, "Success message from detailed error request ??");
			break;

		default:
			lstrcat (details, "Unknown Error value in condition code.");
			break;
	}

	// timerout is set to 999 initially and only reset with the autotests
	if (TimerOut == 999)
	{ 
		MessageBox (hAppWnd, details, "Sample Source Error", MB_OK);
	}
	else
	{
		TimedMsgBox(hDSInst, hAppWnd, details, "Sample Source Error", MB_OK, MB_OK, TimerOut);
	}
	// Rules say to return a CLOSEDSREQ for fatal errors
	return;
} 


/*
	*	CopyFileEx -- copies file1 to file2.  This function is provided for backwards
	*	compatibility to 16 bit Windows.  The CopyFile function is not supported at 
	*	16 bits
*/

BOOL CopyFileEx(char source[],char dest[],BOOL notify)
{               
	//HANDLE buf;
	HFILE hSource,hDest;
	UINT count,out;
	OFSTRUCT of;
	//char *phdl;
	char buffer[2040];
    
	if ((hSource = OpenFile(source,&of,OF_READ))==HFILE_ERROR)
	{
		return(FALSE);
	}
  if ((hDest = OpenFile(dest,&of,OF_CREATE))==HFILE_ERROR)
	{
		return(FALSE);
	}
    
	//    buf = GlobalAlloc(GHND,10240);
	//    phdl = GlobalLock(buf);
	count = 2040;
  while((out = _lread(hSource,buffer,count)) > 0)
	{
		_lwrite(hDest,buffer,out);
  }      
   // Close files and release memory. 
   _lclose(hSource);
   _lclose(hDest);
    
	//    GlobalUnlock(buf);
	//    GlobalFree(buf);     

	return(TRUE);
}

/*
* LogMessage -- 
*/
void LogMessage(char msg[])
{ 
	#ifdef _DEBUG

	HFILE file;
	OFSTRUCT of;
                
	if (OpenFile("c:\\twsrc.log",&of,OF_EXIST)==HFILE_ERROR)
	{
		file = OpenFile("c:\\twsrc.log",&of,OF_CREATE|OF_WRITE);
	}
	else
	{
		file = OpenFile("c:\\twsrc.log",&of,OF_WRITE);
	}

	_llseek(file,0,FILE_END);
	_lwrite(file,msg,lstrlen(msg));
	_lclose(file); 

	#endif
}
