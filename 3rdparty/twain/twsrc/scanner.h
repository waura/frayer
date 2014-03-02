/*
* File: scanner.h
* Company: TWAIN Working Group, JFL Peripheral Solutions Inc.
* Author: Jon Harju
* Description:
*		Simulates basic scanner operations
*
*	Copyright © 1998 TWAIN Working Group: Adobe Systems Incorporated, 
*	Canon Information Systems, Eastman Kodak Company, 
*	Fujitsu Computer Products of America, Genoa Technology, 
*	Hewlett-Packard Company, Intel Corporation, Kofax Image Products, 
*	JFL Peripheral Solutions Inc., Ricoh Corporation, and Xerox Corporation.  
*	All rights reserved.
*/

#ifndef _inc_scanner_h
#define _inc_scanner_h

/*
*	Real scanner type of functions
*/
BOOL IsDocumentInFeeder();
BOOL StartScan(DWORD *pdwLineSize);
BOOL ReadDataBlock(BYTE *pBuffer, DWORD dwMaxSize, DWORD *pdwBytesWritten, BOOL *bLastBuffer);
void StopScan();

/*
*	Additional functions for our simulation
*/
int GetNumPages();
void SetNumPages(int iNumPages);
void LoadFeeder();
void ClearFeeder();

/*
*	Optimized scan to DIB function
*/
HGLOBAL ScanToDIB();

#endif //_inc_scanner_h
