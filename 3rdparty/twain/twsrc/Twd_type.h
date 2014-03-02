/***********************************************************************
 TWAIN Sample Source 
 Copyright (C) '91-'93 TWAIN Working Group: 
 Aldus, Caere, Eastman-Kodak, Logitech,
 Hewlett-Packard Corporations.
 All rights reserved.
*
*	Copyright © 1998 TWAIN Working Group: Adobe Systems Incorporated, 
*	Canon Information Systems, Eastman Kodak Company, 
*	Fujitsu Computer Products of America, Genoa Technology, 
*	Hewlett-Packard Company, Intel Corporation, Kofax Image Products, 
*	JFL Peripheral Solutions Inc., Ricoh Corporation, and Xerox Corporation.  
*	All rights reserved.
*
*************************************************************************/

#ifndef _inc_twd_type_h
#define _inc_twd_type_h

/* This header contains the function prototypes and constants required for
the TWD... source routines used to generate the toolkit sample Source.
*/

// Constants
/* Source Version */
#define MAX_TWPATH			260
#define VERSION_MAJOR       0
#define VERSION_MINOR       1

/* Source State defines */
#define STATE1            100
#define STATE2            200
#define STATE3            300
#define STATE4            400
#define STATE5            500
#define STATE6            600
#define STATE7            700

#define E_IDENTITYMSG   (char *)"General failure in response to DAT_IDENTITY.  "
#define E_INTERFACEMSG  (char *)"General failure in response to DAT_USERINTERFACE.  "

#define E_CONTROLMSG    (char *)"General failure in response to DAT_NULL.  "
#define E_ENABLEDS      (char *)"General failure in response to MSG_ENABLEDS.  "
#define E_DISABLEDS     (char *)"General failure in response to MSG_DISABLEDS.  "
#define E_OPENDS        (char *)"General failure in response to MSG_OPENDS.  "
#define E_CLOSEDS       (char *)"General failure in response to MSG_CLOSEDS.  "

#define E_CAPABILITYMSG     (char *)"General failure in response to DAT_CAPS.  "
#define E_STATUSMSG         (char *)"General failure in response to DAT_STATUS.  "
#define E_PENDINGXFERMSG    (char *)"General failure in response to DAT_PENDINGXFER.  "

#define E_SETUPXFERMECHMSG  (char *)"General failure in response to DAT_XFERMECH.  "
#define E_SETUPXFERMECHSET  (char *)"General failure in response to DAT_XFERMECH, MSG_SET, non-supported type.  "

#define E_SETUPMEMXFER      (char *)"General failure in response to DAT_SETUPMEMXFER, not supported.  "
#define E_SETUPFILEXFER     (char *)"General failure in response to DAT_SETUPFILEXFER, not supported.  "

#define E_XFERGROUP         (char *)"General failure in response to DAT_XFERGROUP, not supported.  "

#define E_SETUPNATIVEXFERSET (char *)"General failure in response to DAT_SETUPNATIVEXFER, non-supported type.  "
#define E_SETUPNATIVEXFERMSG (char *)"General failure in response to DAT_SETUPNATIVEXFER.  "

#define E_PALETTE8           (char *)"General failure in response to DAT_PALETTE8.  "

#define E_IMAGEINFOFOPEN     (char *)"General failure in transfer file open.  "
#define E_IMAGEINFORHEAD     (char *)"General failure in transfer reading file header.  "
#define E_IMAGEINFOFTYPE     (char *)"General failure in transfer file is wrong type.  "
#define E_IMAGEINFOIHEAD     (char *)"General failure in transfer reading file info header.  "
#define E_IMAGEINFOBTYPE     (char *)"General failure in transfer bad info header info.  "
#define E_IMAGEINFOMSG       (char *)"General failure in transfer.  "

#define E_IMAGELAYOUTMSG     (char *)"General failure in response to DAT_IMAGEINFO.  "
#define E_IMAGEMEMXFERWTYPE  (char *)"General failure in response to DAT_IMAGEMEMXFER, wrong setup type.  "
#define E_IMAGEMEMXFERMSG    (char *)"General failure in response to DAT_IMAGEMEMXFER.  "
#define E_IMAGENATIVEXFERMSG (char *)"General failure in response to DAT_IMAGENATIVEXFER.  "

#define E_CONTAINER          (char *)"General failure creating a container.  "


// This struct is used to dispatch to functions to handle the
// various structures that need to be filled in.  It could be
// extended to check the Message field as well.
typedef struct {
   TW_UINT16   ID;               // Either the struct id or msg id
   TW_UINT16   (*pFunc)();       // Pointer to fnt which processes msg
} MAPPER, * PMAPPER;

// internal structure to store TW messages
typedef struct
{
   pTW_IDENTITY  pSrc;
   TW_UINT32     DG;
   TW_UINT16     DAT;
   TW_UINT16     MSG;
   TW_MEMREF     pData;
} TWMSG, FAR * PTWMSG;

/***********************************************************************/
/* TWD_MAIN.C Source Functions */
/***********************************************************************/
#ifdef WIN32
INT APIENTRY LibMain (HANDLE hModule,
		      ULONG  ulReasonCalled,
		      LPVOID lpReserved);
#else     //GR: 16 bit version
BOOL FAR PASCAL LibMain (HANDLE hInstance, WORD wDataSeg,
                         WORD wHeapSize, LPSTR lpCmdLine);
#endif

/***********************************************************************/
/* TWD_PROTO.C Source Functions */
/***********************************************************************/
TW_UINT16 Dispatch (PTWMSG pDcMsg, PMAPPER pMapper, TW_UINT16 DAT);
TW_UINT16 IdentityMsg (PTWMSG pDcMsg);
TW_UINT16 InterfaceMsg (PTWMSG pDcMsg);
TW_UINT16 ControlMsg (PTWMSG pDcMsg);
TW_UINT16 CapabilityMsg (PTWMSG pDcMsg);
TW_UINT16 StatusMsg (PTWMSG pDcMsg);
TW_UINT16 PendingXferMsg (PTWMSG pDcMsg);
TW_UINT16 SetupXferMechMsg (PTWMSG pDcMsg);
TW_UINT16 SetupMemXferMsg (PTWMSG pDcMsg);
TW_UINT16 SetupFileXferMsg (PTWMSG pDcMsg);
TW_UINT16 SetupNativeXferMsg (PTWMSG pDcMsg);
TW_UINT16 XferGroupMsg (PTWMSG pDcMsg);
TW_UINT16 Palette8Msg (PTWMSG pDcMsg);
TW_UINT16 GrayResponseMsg (PTWMSG pTWMsg);
TW_UINT16 RGBResponseMsg (PTWMSG pTWMsg);
TW_UINT16 CIEColorMsg (PTWMSG pTWMsg);
TW_UINT16 JPEGCompressionMsg (PTWMSG pTWMsg);
TW_UINT16 ImageInfoMsg (PTWMSG pDcMsg);
TW_UINT16 ImageLayoutMsg (PTWMSG pDcMsg);
TW_UINT16 ImageMemXferMsg (PTWMSG pDcMsg);
TW_UINT16 ImageFileXferMsg (PTWMSG pDcMsg);
TW_UINT16 ImageNativeXferMsg (PTWMSG pDcMsg);
TW_UINT16 EnableDS (pTW_USERINTERFACE pUI);
TW_UINT16 DisableDS (pTW_USERINTERFACE pUI);
TW_UINT16 OpenDS (PTWMSG pDcMsg);
TW_UINT16 CloseDS (VOID);

VOID NotifyXferReady (VOID);
VOID NotifyCloseDSReq (VOID);
VOID DS_Error (char *, int);


/***********************************************************************/
/* TWD_SLB.C Source Functions */
/***********************************************************************/
#ifdef WIN32
LRESULT CALLBACK open_file_LB (HWND hdlg, UINT wMsg, WPARAM wParam, LPARAM lParam);
#else     //GR: 16 bit version
BOOL FAR PASCAL _export open_file_LB (HWND hdlg, WORD wMsg, WORD wParam, LONG lParam);
#endif

/***********************************************************************/
/* TWD_HDIB.C Source Functions */
/***********************************************************************/
HANDLE OpenDIB (LPSTR);
HANDLE ReadDibBitmapInfo (int);
WORD DibNumColors (VOID FAR *);
#ifdef WIN32
UINT PASCAL lread (int, VOID FAR *, UINT);
#else	//GR: 16 bit version
DWORD PASCAL lread (int, VOID FAR *, DWORD);
#endif
BOOL DibInfo (HANDLE, LPBITMAPINFOHEADER);
WORD PaletteSize (VOID FAR *);
void SaveBitmap(HWND hWnd, HGLOBAL _hDIB, LPCSTR pszFileName);

typedef struct
{
	float Top;
	float Left;
	float Right;
	float Bottom;
} INTERNALFRAMESTRUCT, *pINTERNALFRAMESTRUCT;

#endif //_inc_twd_type_h
