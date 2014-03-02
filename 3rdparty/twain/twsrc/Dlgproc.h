/* 
* File: Dlgproc.h
* Company: JFL Peripheral Solutions Inc.
* Date: March 1998
* Descripton:
*   TWAIN Dialog Procedures
*
*	Copyright © 1998 TWAIN Working Group: Adobe Systems Incorporated, 
*	Canon Information Systems, Eastman Kodak Company, 
*	Fujitsu Computer Products of America, Genoa Technology, 
*	Hewlett-Packard Company, Intel Corporation, Kofax Image Products, 
*	JFL Peripheral Solutions Inc., Ricoh Corporation, and Xerox Corporation.  
*	All rights reserved.
*/

#ifndef _inc_dlgproc_h
#define _inc_dlgproc_h

LRESULT CALLBACK SettingsDlgProc (HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);

VOID NotifyXferReady(VOID)	;
VOID NotifyCloseDSReq (VOID);

#endif //_inc_dlgproc_h
