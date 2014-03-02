/***********************************************************************
 TWAIN Sample Source 
 Copyright (C) '91-'92 TWAIN Working Group: 
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

#ifndef _inc_twd_com_h
#define _inc_twd_com_h

// To be included in the common container handling module TWD_COM.C
TW_UINT16 BuildUpArrayType (pTW_CAPABILITY, pTW_ARRAY, void *, TW_UINT16 msg);
TW_UINT16 BuildUpEnumerationType (pTW_CAPABILITY, pTW_ENUMERATION, void *, TW_UINT16 msg);
TW_UINT16 BuildUpRangeType(pTW_CAPABILITY pData, pTW_RANGE pr, TW_UINT16 msg);
VOID ExtractEnumerationValue (pTW_CAPABILITY, LPVOID, int);
TW_UINT16 BuildUpOneValue (pTW_CAPABILITY, TW_UINT16, void *,TW_UINT16 msg);
VOID ExtractOneValue (pTW_CAPABILITY, LPVOID);
TW_UINT16 AltTWItemSize (TW_INT16 ItemType);

#endif //_inc_twd_com_h
