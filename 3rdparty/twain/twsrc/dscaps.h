/*
* File: dscaps.h
* Author: Jon Harju / JFL Peripheral Solutions Inc.
* Date: May 1998
* Comments:
*		Contains declarations for all functions related to managing
* Capabilities in this data source
*
*	Copyright © 1998 TWAIN Working Group: Adobe Systems Incorporated, 
*	Canon Information Systems, Eastman Kodak Company, 
*	Fujitsu Computer Products of America, Genoa Technology, 
*	Hewlett-Packard Company, Intel Corporation, Kofax Image Products, 
*	JFL Peripheral Solutions Inc., Ricoh Corporation, and Xerox Corporation.  
*	All rights reserved.
*
*/

#ifndef _inc_gencaps_h
#define _inc_gencaps_h

/*
*	Structures and defines used for manipulating capabilities
* with this Data Source
*/

/*
*	Generic header used for all capability state storage items
*/
typedef struct
{
	TW_UINT16 CapId;
	TW_UINT16 ItemType;
	TW_UINT16 GetContainerType;
	TW_UINT16 CurrentContainerType;
	TW_UINT32 SupportedMsg;
} CAP_STATEHEADER, *pCAPSTATEHEADER;

/*
*	Generic value storage item, used for all capabilities regardless
* of ItemType
*/
typedef struct
{
	TW_UINT32 NumItems;
	void *pItems;		 
} CAP_ITEMVALUE, *pCAP_ITEMVALUE;

/*
*	A single complete capability table entry
*/
typedef struct
{
	CAP_STATEHEADER Header;
	CAP_ITEMVALUE DefaultSupported;
	CAP_ITEMVALUE DefaultValue;
	CAP_ITEMVALUE CurrentValue;
	TW_UINT16 ConstraintContainerType;
	CAP_ITEMVALUE Constraints;
} CAP_STATESTORAGEITEM, *pCAP_STATESTORAGEITEM;

/*typedef struct
{
	float Left;
	float Top ;
	float Right; 
	float Bottom; 
}frame, *pframe; */
 
/*struct new_frame
{
	float Left;
	float Top ;
	float Right; 
	float Bottom; 
}frame={10,10,10,10};*/
/*
*	Functions for getting initializing and freeing capabilities
*/
void InitializeCaps();
void FreeCaps();

/*
* Function for negotiating all supported Capability requests through 
*	the TWAIN Interface
*/
TW_UINT16 HandleCapMessage(PTWMSG pTWMsg, TW_UINT16 *pStatus);
static pCAP_STATESTORAGEITEM FindCapStorageItem(TW_UINT16 CapId);
static TW_UINT16 HandleQuerySupportCap(pCAP_STATESTORAGEITEM pCapData, pTW_CAPABILITY pCap, TW_UINT16 *pStatus);
static TW_UINT16 HandleGetCap(pCAP_STATESTORAGEITEM pCapData, pTW_CAPABILITY pCap, TW_UINT16 *pStatus);
static TW_UINT16 HandleGetCurrentCap(pCAP_STATESTORAGEITEM pCapData, pTW_CAPABILITY pCap, TW_UINT16 *pStatus);
static TW_UINT16 HandleGetDefaultCap(pCAP_STATESTORAGEITEM pCapData, pTW_CAPABILITY pCap, TW_UINT16 *pStatus);
static TW_UINT16 HandleReSetCap(pCAP_STATESTORAGEITEM pCapData, pTW_CAPABILITY pCap, TW_UINT16 *pStatus);
static TW_UINT16 HandleSetCap(pCAP_STATESTORAGEITEM pCapData, pTW_CAPABILITY pCap, TW_UINT16 *pStatus);


/*
*	Functions for storing and restoring state during serialization of
* capabilities (1.7 feature)
*/
TW_UINT16 GetAllSettings(PTWMSG pTWMsg, TW_UINT16 *pStatus);
TW_UINT16 SetAllSettings(PTWMSG pTWMsg, TW_UINT16 *pStatus);

/*
*	High bandwidth internal capability interface
*/
TW_UINT16 GetCurrentPixelFlavor();
TW_UINT16 GetCurrentXferMech();
TW_INT16 GetXferCount();

TW_BOOL GetFeederEnabled();
void SetFeederEnabled(TW_BOOL);

float *GetSupportedXResolution(int *pNumItems);
float GetCurrentXResolution();
void SetCurrentXResolution(float value);

TW_UINT16 *GetSupportedBitDepth(int *pNumItems, TW_UINT16 PixelType);
TW_UINT16 GetCurrentBitDepth(TW_UINT16 PixelType);
void SetCurrentBitDepth(TW_UINT16 value, TW_UINT16 PixelType);

TW_UINT16 *GetSupportedPixelType(int *pNumItems);
TW_UINT16 GetCurrentPixelType();
void SetCurrentPixelType(TW_UINT16 value);

TW_BOOL *GetSupportedFeederEnabled(int *pNumItems);
TW_BOOL GetCurrentFeederEnabled();
void SetCurrentFeederEnabled(TW_BOOL value);

float *GetSupportedXScaling(int *pNumItems);
float GetCurrentXScaling();
void SetCurrentXScaling(float value);

TW_UINT16 *GetSupportedSupportedSizes(int *pNumItems);
TW_UINT16 GetCurrentSupportedSizes();
void SetCurrentSupportedSizes(TW_UINT16 value);

TW_FIX32 FloatToFIX32 (float floater);
float FIX32ToFloat (TW_FIX32 fix32);
static TW_UINT32 IndexFromItem(pCAP_ITEMVALUE, void *, size_t);

#endif //_inc_gencaps_h
