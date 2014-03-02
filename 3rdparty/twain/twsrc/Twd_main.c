/*
	* TWAIN Sample Source 
	* Copyright (C) '91-'93 TWAIN Working Group:
	* Aldus, Caere, Eastman-Kodak, Logitech,
	* Hewlett-Packard Corporations.
	* All rights reserved.
*
*	Copyright © 1998 TWAIN Working Group: Adobe Systems Incorporated, 
*	Canon Information Systems, Eastman Kodak Company, 
*	Fujitsu Computer Products of America, Genoa Technology, 
*	Hewlett-Packard Company, Intel Corporation, Kofax Image Products, 
*	JFL Peripheral Solutions Inc., Ricoh Corporation, and Xerox Corporation.  
*	All rights reserved.
*
*/

#include <windows.h>        // req. windows types and for twain.h
#include "..\\twcommon\\twain.h"             // req. for TWAIN types

#include "twd_type.h"       // function prototypes for Source
#include "twd_res.h"
#include "..\\twcommon\\twndebug.h"
#include<commctrl.h>


// Extern Function Prototypes -------
extern void LogMessage(char msg[]);	//located at prot.c simply registers 
                                   //complaints	in twsrc.log BR

#ifdef WIN32

/*
	*extern "C" {
	*
	*BOOL WINAPI LibMain (HANDLE hModule,
	*					  ULONG  ulReasonCalled,
	*					  LPVOID lpReserved);
*/

TW_UINT16 CALLBACK DS_Entry(pTW_IDENTITY pSrc, TW_UINT32    DG,
			      TW_UINT16    DAT,  TW_UINT16    MSG,
			      TW_MEMREF    pData);

//} //extern "C"

#endif


#define NUM_PIXTYPES    4   // size of number of pixel types array


// Global variables 
HANDLE          hDSInst;    		// Keep track of who we are
HPALETTE        hDibPal;    		// Color palette handle
UINT TimerOut = 999;        		// timeout value for message boxes
char SourceControl[MAX_TWPATH] = {'\0'}; 	

// Support for pixel types TW_ARRAY container
TW_ARRAY    PixelTypesTWArray={TWTY_UINT16, NUM_PIXTYPES, {57}};
TW_UINT16   PixelTypesList[NUM_PIXTYPES]={TWPT_BW, TWPT_GRAY, TWPT_RGB,
					   TWPT_PALETTE};

// Global variables - Source Environment 
TW_IDENTITY     dsIdentity;     // Who we are
TW_IDENTITY     appIdentity;    // To keep track of who's driving us
TW_STATUS       twStatus = {TWCC_SUCCESS, FALSE};


// External stuff 
extern MAPPER ControlMap[];
extern MAPPER ImageMap[];
extern HANDLE hImageDlg;
extern HANDLE hDlg;

#ifdef WIN32

/*
	* FUNCTION: LibMain
	*
	* 
	*			hModule  		handle to current instance
	*          ulReasonCalled
	*          lpReserved
	*
	* RETURNS: 1
	*
	* NOTES:   This function intializes the Source DLL
*/

BOOL WINAPI DllMain (HINSTANCE hModule, DWORD  ulReasonCalled, LPVOID lpReserved)
{
	if (!hModule)
	{
		return(0);
	}

	switch (ulReasonCalled) 
	{
		case DLL_PROCESS_ATTACH:
			// Initialize any necessary globals
			hDSInst = hModule;
			// state 3
			// Clear out calling app's identity
			appIdentity.Id = 0L;

			// Setup Source's identity and group supported
			dsIdentity.Id = 0L;
			
			/*
			*	Indicate that this is a TWAIN 1.6 compliant Data Source
			*/
			dsIdentity.ProtocolMajor    = 1;//TWON_PROTOCOLMAJOR;
			dsIdentity.ProtocolMinor    = 6;//TWON_PROTOCOLMINOR;
			dsIdentity.Version.MajorNum = 1;
			dsIdentity.Version.MinorNum = 603;
			dsIdentity.Version.Language = TWLG_USA;
			dsIdentity.Version.Country  = TWCY_USA;
			dsIdentity.SupportedGroups  = DG_CONTROL | DG_IMAGE;

			LoadString(hDSInst, VERSION_INFO, dsIdentity.Version.Info, sizeof(TW_STR32));
			LoadString(hDSInst, MANUFACTURER, dsIdentity.Manufacturer, sizeof(TW_STR32));
			LoadString(hDSInst, PRODFAMILY,  dsIdentity.ProductFamily, sizeof(TW_STR32));
			LoadString(hDSInst, PRODNAME, dsIdentity.ProductName, sizeof(TW_STR32));
			InitCommonControls();
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
	}	// switch off
	return (1);
} /* end DllMain() */

#else		//GR: 16 bit version
/*
	* FUNCTION: LibMain
	*
	* ARGS:    hInstance   handle to current instance
	*          wDataSeg
	*          wHeapSize
	*          lpCmdLine
	*
	* RETURNS: 1
	*
	* NOTES:   This function intializes the Source DLL
*/

BOOL FAR PASCAL LibMain(HANDLE hInstance, WORD wDataSeg,
                        WORD wHeapSize, LPSTR lpCmdLine)
{
	if (!hInstance)
	{
		return(0);
	}

/*
	* LocalInit() adds one lock to the Data Segment.
	* We call UnlockSegment() here to remove this lock and
	* allow the Source to move freely.
	*
	* WARNING 1:
	* Any function call outside the current code segment may move the Source.
	* WARNING 2:
	* LocalAlloc() and LocalRealloc() will unlock and move the Source, if
	* need be.
*/

	if (wHeapSize)
	{
		UnlockSegment(wDataSeg);
	}

	// Initialize any necessary globals
	hDSInst = hInstance;
	// state 3

	// Clear out calling app's identity
	appIdentity.Id = 0L;

	// Setup Source's identity and group supported
	dsIdentity.Id = 0L;
	dsIdentity.ProtocolMajor   = TWON_PROTOCOLMAJOR;
	dsIdentity.ProtocolMinor   = TWON_PROTOCOLMINOR;
	dsIdentity.Version.MajorNum = 1;
	dsIdentity.Version.MinorNum = 603;
	dsIdentity.Version.Language = TWLG_USA;
	dsIdentity.Version.Country  = TWCY_USA;
	dsIdentity.SupportedGroups = DG_CONTROL | DG_IMAGE;

	LoadString(hDSInst, VERSION_INFO, dsIdentity.Version.Info, sizeof(TW_STR32));
	LoadString(hDSInst, MANUFACTURER, dsIdentity.Manufacturer, sizeof(TW_STR32));
	LoadString(hDSInst, PRODFAMILY,  dsIdentity.ProductFamily, sizeof(TW_STR32));
	LoadString(hDSInst, PRODNAME, dsIdentity.ProductName, sizeof(TW_STR32));

	return (1);
} // end LibMain() 

#endif   //WIN32


//extern "C" {

/*
	*  FUNCTION: DS_Entry
	*
	* ARGS:    pSrc    ptr to caller, source of message
	*          DG      data group ID, DG_IMAGE, DG_xxxx
	*          DAT     data argument type, DAT_xxxx
	*          MSG     message ID, MSG_xxxx
	*          pData   pointer to the data
	*
	* RETURNS: twRC    status code
	* NOTES:   This function is declared as the single entry point to the Source.
	*          It should be exported as Ordinal function 1 (@1).
*/

TW_UINT16 CALLBACK DS_Entry(pTW_IDENTITY pSrc, TW_UINT32  DG,
			      TW_UINT16    DAT,  TW_UINT16    MSG,
			      TW_MEMREF    pData)
{
	TWMSG     twMsg;
	TW_UINT16 twRc;

/* 
	* TW apps are sending down all their messages prior to process them
	*to give us a chance to extract messages such as dialog keyboard
	*handling, accelerator translation or MDI processing in a Source
	*while the Source is enabled. We are doing this filtering prior to process
	*real Source messages to minimize overhead.
	*Relay windows messages down to the Source via the SM.  The SM will send
	*a MSG_PROCESSEVENT to the Source to flag the arrival of a message relayed
	*from the APP that was originally a windows message.
	*
	*Valid states: 5 -- 7.  As soon as the application has enabled the Source it
	*must begin sending the Source events.  This allows the Source to recieve events
	*to update it's user interface and to return messages to the application.
	*The app sends down ALL message, the Source decides which ones apply to it.
	*
	*added to keep stop Apps from talking to us until properly connected
	*NOTE: Since SM peeks into Sources pSrc may be invalid (NULL) on some calls
	*so the order of evaluation is key. 
*/

	if ((!appIdentity.Id) || (appIdentity.Id == pSrc->Id))
	{
		if (MSG == MSG_PROCESSEVENT)
		{
			LogMessage("Process event message at source\r\n");

			if (hImageDlg && IsDialogMessage(hImageDlg, (LPMSG)(((pTW_EVENT)pData)->pEvent)))
			{
				LogMessage("Process event Message directed to data source dialog\r\n");

				twRc = TWRC_DSEVENT;
/*
	*you should, for proper form, return
	*a MSG_NULL for all windows messages processed by the Data Source
	*NOTE: could slip by with a default case in the App and no MSG_NULL
	*but that would be cheating!!
*/
				((pTW_EVENT)pData)->TWMessage = MSG_NULL;
			}
			else
			{
				LogMessage("Process event not ds event\r\n");

				twRc = TWRC_NOTDSEVENT;
			}
		}
		else
		{
			// Reset the condition code
			if (DG != DG_CONTROL && DAT != DAT_STATUS)
			{
				twStatus.ConditionCode = TWCC_SUCCESS;
			}

			// Build the message for the dispatch routine
			twMsg.pSrc  = pSrc;
			twMsg.DG    = DG;
			twMsg.DAT   = DAT;
			twMsg.MSG   = MSG;
			twMsg.pData = pData;

			// Route the Source message.
			if (DG == DG_CONTROL)
			{
				twRc = Dispatch(&twMsg, ControlMap, DAT);
			}
			else
			{
				if (DG ==  DG_IMAGE)
				{
					twRc = Dispatch(&twMsg, ImageMap, DAT);
				}
				else
				{
					twStatus.ConditionCode = TWCC_BADPROTOCOL;
					twRc = TWRC_FAILURE;
				}
		  }
	  }   

/*
	*else
	*{
	*    twStatus.ConditionCode = TWCC_BADPROTOCOL;
	*    twRc = TWRC_FAILURE;
	*}
*/

 }
	return twRc;
} 



