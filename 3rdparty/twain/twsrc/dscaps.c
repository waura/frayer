/*
* File: dscaps.c
* Author: Jon Harju / JFL Peripheral Solutions Inc.
* Date: May 1998
* Comments:
*	Contains the implementation of capability state management for
*	this Sample TWAIN Data Source
*	(Note: this is a single instance implementation)
*
*	Copyright © 1998 TWAIN Working Group: Adobe Systems Incorporated, 
*	Canon Information Systems, Eastman Kodak Company, 
*	Fujitsu Computer Products of America, Genoa Technology, 
*	Hewlett-Packard Company, Intel Corporation, Kofax Image Products, 
*	JFL Peripheral Solutions Inc., Ricoh Corporation, and Xerox Corporation.  
*	All rights reserved.
*/

/*
*	Standard windows includes
*/
#include <windows.h>
#include <windowsx.h>
//#include "c:\..\Vc\include\windef.h"	 TRUE
/*
*	Local includes
*/
#include "..\\twcommon\\twndebug.h"
#include "..\\twcommon\\twain.h"
#include "twd_type.h"
#include "twd_com.h"
#include "dscaps.h"
#include "scanner.h"

#pragma message("****Notice: dscaps.c is a work in progress.")
#pragma message("This is only one of many ways to implement a basic Capability Engine.")
#pragma message("TODO: proper capability constraint implementation.")
#pragma message("TODO: add additional options.")
#pragma message("TODO: change float/fix32 comparissons, memcmp is not accurate for these, not suitable for production data sources.")

/*
*	external global variables
*/
extern TW_IDENTITY appIdentity;

/*
*	All our special case capability bases are here
*/
#define CAP_BITDEPTHBASE			0x9000

/*
*	Module private globals variables
*/

/*
*	Each capability has read only place holders supported and default
* values
*/
static TW_UINT16 HandleQuerySupportCap(pCAP_STATESTORAGEITEM pCapData, pTW_CAPABILITY pCap, TW_UINT16 *pStatus);
static TW_UINT16 HandleGetCap(pCAP_STATESTORAGEITEM pCapData, pTW_CAPABILITY pCap, TW_UINT16 *pStatus);
static TW_UINT16 HandleGetCurrentCap(pCAP_STATESTORAGEITEM pCapData, pTW_CAPABILITY pCap, TW_UINT16 *pStatus);
static TW_UINT16 HandleGetDefaultCap(pCAP_STATESTORAGEITEM pCapData, pTW_CAPABILITY pCap, TW_UINT16 *pStatus);
static TW_UINT16 HandleReSetCap(pCAP_STATESTORAGEITEM pCapData, pTW_CAPABILITY pCap, TW_UINT16 *pStatus);
static TW_UINT16 HandleSetCap(pCAP_STATESTORAGEITEM pCapData, pTW_CAPABILITY pCap, TW_UINT16 *pStatus);

static TW_UINT32 IndexFromItem(pCAP_ITEMVALUE, void *, size_t )	;

static BOOL IsItemInList(pCAP_ITEMVALUE pCapSupported, void *pItem, size_t ItemSize);
static BOOL IsItemInRange(pCAP_ITEMVALUE pCapSupported, void *pItem, TW_UINT16 ItemType);

static void AssignLongToValue(TW_UINT32 LongValue, void *pDestValue, TW_UINT16 ItemType);
static void IncrementValue(void *pItem, void *pStepSize, TW_UINT16 ItemType);
static BOOL IsItemLessOrEqual(void *pItem1, void *pItem2, TW_UINT16 ItemType);

static CAP_ITEMVALUE AllocateUnionRangeToEnum( pTW_CAPABILITY pCap, pCAP_ITEMVALUE pSupported);
static CAP_ITEMVALUE AllocateUnionEnumToRange( pTW_CAPABILITY pCap, pCAP_ITEMVALUE pSupported);
static CAP_ITEMVALUE AllocateUnionEnumToEnum( pTW_CAPABILITY pCap, pCAP_ITEMVALUE pSupported);

static int CompareEnumToSupported( pTW_CAPABILITY pCap, pCAP_ITEMVALUE pSupported, TW_UINT16 GetContainerType);
static int CompareRangeToSupportedRange( pTW_CAPABILITY pCap, pCAP_ITEMVALUE pSupported);
static int CompareRangeToSupportedArray( pTW_CAPABILITY pCap, pCAP_ITEMVALUE pSupported);

#define SUPPORTEDCAPS 40/*41 update when AUTOSCAN is added*/
static  TW_UINT16 gSupportedCaps[SUPPORTEDCAPS] = 
				{CAP_SUPPORTEDCAPS, CAP_FEEDERENABLED, ICAP_XFERMECH, ICAP_PIXELFLAVOR, ICAP_PLANARCHUNKY, 
				ICAP_COMPRESSION, ICAP_PHYSICALWIDTH, ICAP_PHYSICALHEIGHT,ICAP_MINIMUMHEIGHT,ICAP_MINIMUMWIDTH,ICAP_UNITS, CAP_DEVICEONLINE, 
				/*CAP_AUTOSCAN,*/ CAP_AUTOFEED,CAP_FEEDERLOADED, ICAP_AUTOBRIGHT, ICAP_BRIGHTNESS, ICAP_CONTRAST, ICAP_HIGHLIGHT,
				ICAP_ORIENTATION, ICAP_SHADOW, ICAP_XSCALING, ICAP_YSCALING, ICAP_PIXELTYPE, ICAP_BITDEPTH, 
				ICAP_BITDEPTHREDUCTION,ICAP_BITORDER, ICAP_HALFTONES, ICAP_THRESHOLD, ICAP_FRAMES,
				ICAP_MAXFRAMES, ICAP_SUPPORTEDSIZES,ICAP_XNATIVERESOLUTION, ICAP_YNATIVERESOLUTION,
				ICAP_XRESOLUTION, ICAP_YRESOLUTION, CAP_XFERCOUNT,ICAP_IMAGEFILEFORMAT, ICAP_UNDEFINEDIMAGESIZE,
				CAP_INDICATORS, CAP_UICONTROLLABLE };

//ICAP_PIXELFLAVOR
#define PIXELFLAVORS 2
static TW_UINT16 gSupportedPixelFlavors[PIXELFLAVORS] = {TWPF_CHOCOLATE, TWPF_VANILLA};
static TW_UINT16 gDefaultPixelFlavor = TWPF_CHOCOLATE;
static TW_UINT16 gCurrentPixelFlavor = TWPF_CHOCOLATE;

//ICAP_XFERMECH
#define XFERMECHS 3
static TW_UINT16 gSupportedXferMechs[XFERMECHS] = {TWSX_NATIVE, TWSX_FILE, TWSX_MEMORY};
static TW_UINT16 gDefaultXferMech = TWSX_NATIVE;
static TW_UINT16 gCurrentXferMech = TWSX_NATIVE;

//CAP_FEEDERENABLED
#define FEEDERENABLED 2
static TW_BOOL gSupportedFeederEnabled[FEEDERENABLED] = {TRUE, FALSE};
static TW_BOOL gDefaultFeederEnabled = FALSE;
static TW_BOOL gCurrentFeederEnabled = FALSE;

//ICAP_PLANARCHUNKY
#define PLANARCHUNKY 2
static TW_UINT16 gSupportedPlanarChunky[PLANARCHUNKY] = {TWPC_CHUNKY, TWPC_PLANAR};
static TW_UINT16 gDefaultPlanarChunky = TWPC_CHUNKY;
static TW_UINT16 gCurrentPlanarChunky = TWPC_CHUNKY;

//ICAP_COMPRESSION 
#define COMPRESSION	1
static TW_UINT16 gSupportedCompression[COMPRESSION] =	{TWCP_NONE};
static TW_UINT16 gDefaultCompression = TWCP_NONE;
static TW_UINT16 gCurrentCompression = TWCP_NONE;

//ICAP_MINIMUMHEIGHT 	
#define TWMINIMUMWIDTH 1
static float gSupportedMinimumWidth[TWMINIMUMWIDTH] =	{1.0F};
static float gDefaultMinimumWidth = 1.0F;
static float gCurrentMinimumWidth = 1.0F;	

//ICAP_MINIMUMWIDTH
#define TWMINIMUMHEIGHT 1
static float gSupportedMinimumHeight[TWMINIMUMHEIGHT] =	{1.0F};
static float gDefaultMinimumHeight = 1.0F;
static float gCurrentMinimumHeight = 1.0F;	

//ICAP_PHYSICALWIDTH 	
#define TWPHYSICALWIDTH	 1
static float gSupportedPhysicalWidth[TWPHYSICALWIDTH] =	{8.5F};
static float gDefaultPhysicalWidth = 8.5F;
static float gCurrentPhysicalWidth = 8.5F;	

//ICAP_PHYSICALHEIGHT 					 
#define TWPHYSICALHEIGHT 1
static float gSupportedPhysicalHeight[TWPHYSICALHEIGHT] ={11.0F};	
static float gDefaultPhysicalHeight = 11.0F;
static float gCurrentPhysicalHeight = 11.0F; 

//ICAP_UNITS 
#define UNITS	3
static TW_UINT16 gSupportedUnits[UNITS] =	{ TWUN_INCHES, TWUN_CENTIMETERS, TWUN_PIXELS};
static TW_UINT16 gDefaultUnits = TWUN_INCHES;
static TW_UINT16 gCurrentUnits = TWUN_INCHES;

//CAP_DEVICEONLINE 
#define DEVICEONLINE	2
static TW_BOOL gSupportedDeviceOnLine[DEVICEONLINE] =	{ TRUE, FALSE }	;
static TW_BOOL gDefaultDeviceOnLine = TRUE;
static TW_BOOL gCurrentDeviceOnLine = TRUE;

//CAP_AUTOFEED 
#define AUTOFEED	2
static TW_BOOL gSupportedAutoFeed[AUTOFEED] =	{ TRUE, FALSE};
static TW_BOOL gDefaultAutoFeed = FALSE;
static TW_BOOL gCurrentAutoFeed = FALSE;

#if 0
//CAP_AUTOSCAN 
#define AUTOSCAN	2
static TW_BOOL gSupportedAutoScan[AUTOSCAN] =	{ TRUE, FALSE};
static TW_BOOL gDefaultAutoScan = FALSE;
static TW_BOOL gCurrentAutoScan = FALSE;
#endif //#if 0

//CAP_FEEDERLOADED
#define FEEDERLOADED 	2
static TW_BOOL gSupportedFeederLoaded[FEEDERLOADED] =	{ FALSE, TRUE};
static TW_BOOL gDefaultFeederLoaded = FALSE;
static TW_BOOL gCurrentFeederLoaded = FALSE;

//ICAP_AUTOBRIGHT 
#define AUTOBRIGHT	2
static TW_BOOL gSupportedAutoBright[AUTOBRIGHT] =	{ TRUE, FALSE};
static TW_BOOL gDefaultAutoBright = FALSE;
static TW_BOOL gCurrentAutoBright = FALSE;

//ICAP_BRIGHTNESS 						
#define BRIGHTNESS	3
static float gSupportedBrightness[BRIGHTNESS] =	{-1000.0F, 1000.0F, 1000.0F};
static float gDefaultBrightness = 0.0F;
static float gCurrentBrightness = 0.0F;

//ICAP_CONTRAST 				
#define CONTRAST	3
static float gSupportedContrast[CONTRAST] =	{-1000.0F, 1000.0F, 1000.0F};
static float gDefaultContrast = 0.0F;
static float gCurrentContrast = 0.0F;

//ICAP_HIGHLIGHT 			
#define HIGHLIGHT	3
static float gSupportedHighLight[HIGHLIGHT] =	{0.0F, 255.0F, 1.0F};
static float gDefaultHighLight = 128.0F;
static float gCurrentHighLight = 128.0F;

//ICAP_ORIENTATION
#define ORIENTATION	4
static TW_UINT16 gSupportedOrientation[ORIENTATION] =	{TWOR_PORTRAIT, TWOR_ROT90, TWOR_ROT180, TWOR_LANDSCAPE};
static TW_UINT16 gDefaultOrientation = TWOR_PORTRAIT;
static TW_UINT16 gCurrentOrientation = TWOR_PORTRAIT;

//ICAP_SHADOW 					 
#define SHADOW 	3
static float gSupportedShadow[SHADOW] ={0.0F, 255.0F, 1.0F};
static float gDefaultShadow = 128.0F;
static float gCurrentShadow = 128.0F;

//ICAP_XSCALING					
#define XSCALING	1//3
static float gSupportedXScaling[XSCALING] =	{/*75.0F, 90.0F,*/ 100.0F};
static float gDefaultXScaling = 100.0F;
static float gCurrentXScaling = 100.0F;

/*
*	Bit depth is more complicated, there is a bitdepth for each pixeltype
*/
//ICAP_BITDEPTH
#define BITDEPTHBW 1
static TW_UINT16 gSupportedBitDepthBW[BITDEPTHBW] =	{1};
static TW_UINT16 gDefaultBitDepthBW = 1;
static TW_UINT16 gCurrentBitDepthBW =	1;

#define BITDEPTHGRAY 1
static TW_UINT16 gSupportedBitDepthGray[BITDEPTHGRAY] =	{/*4,*/ 8};
static TW_UINT16 gDefaultBitDepthGray = 8;
static TW_UINT16 gCurrentBitDepthGray =	8;

#define BITDEPTHRGB 1
static TW_UINT16 gSupportedBitDepthRGB[BITDEPTHRGB] =	{24};
static TW_UINT16 gDefaultBitDepthRGB = 24;
static TW_UINT16 gCurrentBitDepthRGB =	24;

//ICAP_PIXELTYPE
#define PIXELTYPE	3
static TW_UINT16 gSupportedPixelType[PIXELTYPE] =	{ TWPT_BW, TWPT_GRAY, TWPT_RGB };
static TW_UINT16 gDefaultPixelType = TWPT_BW;
static TW_UINT16 gCurrentPixelType = TWPT_GRAY;


//ICAP_BITDEPTHREDUCTION
#define BITDEPTHREDUCTION 3
static TW_UINT16 gSupportedBitDepthReduction[BITDEPTHREDUCTION] =	{TWBR_THRESHOLD, TWBR_HALFTONE, TWBR_DIFFUSION};
static TW_UINT16 gDefaultBitDepthReduction = TWBR_THRESHOLD;
static TW_UINT16 gCurrentBitDepthReduction = TWBR_THRESHOLD;

//ICAP_BITORDER
#define BITORDER 2
static TW_UINT16 gSupportedBitOrder[BITORDER] =	{TWBO_MSBFIRST, TWBO_LSBFIRST};
static TW_UINT16 gDefaultBitOrder =  TWBO_MSBFIRST;
static TW_UINT16 gCurrentBitOrder =  TWBO_MSBFIRST;

//ICAP_HALFTONES
#define HALFTONES	 2
static TW_STR32 gSupportedHalfTones[HALFTONES] = {"FLOYD/STEINBURG", "ORDERED"};
static TW_STR32 gDefaultHalfTones = "FLOYD/STEINBURG";
static TW_STR32 gCurrentHalfTones = "FLOYD/STEINBURG";

//ICAP_THRESHOLD
#define THRESHOLD	 5
static float gSupportedThreshold[THRESHOLD] =	{0.0F, 255.0F, 1.0F};
static float gDefaultThreshold = 128.0F;
static float gCurrentThreshold = 128.0F;

//ICAP_FRAMES					
#define FR 1
static INTERNALFRAMESTRUCT gSupportedFrames[FR] = {0.0F, 0.0F, 8.5F, 11.0F};
static INTERNALFRAMESTRUCT gDefaultFrames = {0.0F, 0.0F, 8.5F, 11.0F};
static INTERNALFRAMESTRUCT gCurrentFrames = {0.0F, 0.0F, 8.5F, 11.0F};

//ICAP_MAXFRAMES
#define MAXFRAMES 1
static TW_UINT16 gSupportedMaxFrames[MAXFRAMES] =	{1};
static TW_UINT16 gDefaultMaxFrames = 1;
static TW_UINT16 gCurrentMaxFrames = 1;

//ICAP_SUPPORTEDSIZES
#define SUPPORTEDSIZES 1
static TW_UINT16	gSupportedSupportedSizes[SUPPORTEDSIZES]=
{ /*TWSS_NONE, TWSS_A4LETTER,*/	TWSS_USLETTER /*, TWSS_USLEGAL */};
static TW_UINT16 gDefaultSupportedSizes = TWSS_USLETTER;
static TW_UINT16 gCurrentSupportedSizes = TWSS_USLETTER;

//ICAP_XNATIVERESOLUTION
#define XNATIVERESOLUTION 1
static float	gSupportedXNativeResolution[XNATIVERESOLUTION] =	{ 100.0F };
static float gDefaultXNativeResolution = 100.0F;
static float gCurrentXNativeResolution = 100.0F;

//ICAP_YNATIVERESOLUTION
#define YNATIVERESOLUTION 1
static float gSupportedYNativeResolution[YNATIVERESOLUTION]  =	{100.0F };
static float gDefaultYNativeResolution = 100.0F;
static float gCurrentYNativeResolution = 100.0F;

//ICAP_XRESOLUTION
#define XRESOLUTION 1
static float gSupportedXResolution[XRESOLUTION]  =	{100.0F/*, 200.0F, 300.0F*/ };
static float gDefaultXResolution = 100.0F;
static float gCurrentXResolution = 100.0F;

//CAP_XFERCOUNT
#define XFERCOUNT 26
static TW_INT16 gSupportedXferCount[XFERCOUNT] =	{-1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25};
static TW_INT16 gDefaultXferCount =  -1;
static TW_INT16 gCurrentXferCount  = -1;

//ICAP_IMAGEFILEFORMAT
#define IMAGEFILEFORMAT 2
static TW_UINT16 gSupportedImageFileFormat[IMAGEFILEFORMAT] =	{TWFF_TIFF, TWFF_BMP};
static TW_UINT16 gDefaultImageFileFormat = TWFF_BMP ;
static TW_UINT16 gCurrentImageFileFormat = TWFF_BMP ;

//ICAP_UNDEFINEDIMAGESIZE
#define UNDEFINEDIMAGESIZE 2
static TW_BOOL gSupportedUndefinedImageSize[UNDEFINEDIMAGESIZE] =	{ FALSE, TRUE };
static TW_BOOL gDefaultUndefinedImageSize = FALSE;
static TW_BOOL gCurrentUndefinedImageSize = FALSE;

//CAP_INDICATORS
#define INDICATORS 2
static TW_BOOL gSupportedIndicators[INDICATORS] =	{ TRUE, FALSE }	;
static TW_BOOL gDefaultIndicators=  TRUE;
static TW_BOOL gCurrentIndicators = TRUE;

//CAP_UICONTROLLABLE
#define UICONTROLLABLE 2
static TW_BOOL gSupportedUIControllable[UICONTROLLABLE] =	{TRUE , FALSE };
static TW_BOOL gDefaultUIControllable= TRUE;
static TW_BOOL gCurrentUIControllable= TRUE;

/*
*	Capability table, contains reference to all capabilities
*/
static CAP_STATESTORAGEITEM gAllCaps[SUPPORTEDCAPS+PIXELTYPE];

/*
*	Module private functions
*/
static pCAP_STATESTORAGEITEM FindCapStorageItem(TW_UINT16 CapId);

/*
*	Initialization and shutdown of this module
*/

/*
*	Function: InitializeCaps
* Author: Jon Harju / J.F.L. Peripheral Solutions Inc.
* Date: May 1998
* Inputs: none
* Outputs: none
* Comments:
*/
void InitializeCaps()
{
	/*
	*	Initialize all the capability table entries, take care of
	* any special cases here
	*/
	int iCapIndex = 0;
	/*
	*	Verify before manipulating each item verify that there is enough space
	*/
	// CAP_SUPPORTEDCAPS

	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);	//Evaluates the given condition and to continue must success.

	gAllCaps[iCapIndex].Header.CapId = CAP_SUPPORTEDCAPS;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_UINT16;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT;
	gAllCaps[iCapIndex].Header.GetContainerType = TWON_ARRAY;
	gAllCaps[iCapIndex].Header.CurrentContainerType = TWON_ARRAY;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = SUPPORTEDCAPS;
	gAllCaps[iCapIndex].DefaultSupported.pItems = gSupportedCaps;	//an Array no need for &
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = SUPPORTEDCAPS;
	gAllCaps[iCapIndex].DefaultValue.pItems = gSupportedCaps;

	gAllCaps[iCapIndex].CurrentValue.NumItems = SUPPORTEDCAPS;
	gAllCaps[iCapIndex].CurrentValue.pItems = gSupportedCaps;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;
	
	/*
	*	Increment iCapIndex untill last item
	*/
	iCapIndex++;
	// CAP_FEEDERENABLE
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = CAP_FEEDERENABLED;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_BOOL;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_RESET|TWQC_SET;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ONEVALUE;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = 1;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedFeederEnabled;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultFeederEnabled;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentFeederEnabled;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;
	
	
	iCapIndex++;
	// ICAP_PIXELFLAVOR
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_PIXELFLAVOR;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_UINT16;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_SET|TWQC_RESET;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ENUMERATION;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = PIXELFLAVORS;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedPixelFlavors;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultPixelFlavor;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentPixelFlavor;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;
	
	
	iCapIndex++;
	//ICAP_XFERMECH	
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_XFERMECH;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_UINT16;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_SET|TWQC_RESET;
	gAllCaps[iCapIndex].Header.GetContainerType = TWON_ENUMERATION;
	gAllCaps[iCapIndex].Header.CurrentContainerType = TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = XFERMECHS;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedXferMechs;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultXferMech;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentXferMech;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;

	iCapIndex++;
	// ICAP_PLANARCHUNKY
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_PLANARCHUNKY;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_UINT16;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ENUMERATION;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = PLANARCHUNKY;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedPlanarChunky;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultPlanarChunky;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentPlanarChunky;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;
	

	iCapIndex++;
	// ICAP_COMPRESSION
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_COMPRESSION;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_UINT16;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_RESET|TWQC_SET;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ENUMERATION;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = COMPRESSION;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedCompression;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultCompression;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentCompression;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;
	
 	iCapIndex++;

	//ICAP_MINIMUMWIDTH
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_MINIMUMWIDTH;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_FIX32;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ONEVALUE;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = TWMINIMUMWIDTH;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedMinimumWidth;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultMinimumWidth;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentMinimumWidth;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL; 
	
  iCapIndex++;

	//ICAP_MINIMUMHEIGHT
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_MINIMUMHEIGHT;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_FIX32;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ONEVALUE;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = TWPHYSICALHEIGHT;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedMinimumHeight;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultMinimumHeight;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentMinimumHeight;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL; 
	
  iCapIndex++;

	//ICAP_PHYSICALWIDTH
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_PHYSICALWIDTH;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_FIX32;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ONEVALUE;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = TWPHYSICALWIDTH;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedPhysicalWidth;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultPhysicalWidth;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentPhysicalWidth;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL; 
	
  iCapIndex++;
	//ICAP_PHYSICALHEIGHT
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_PHYSICALHEIGHT;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_FIX32;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ONEVALUE;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = TWPHYSICALHEIGHT;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedPhysicalHeight;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultPhysicalHeight;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentPhysicalHeight;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;
																										
	iCapIndex++;
	//ICAP_UNITS
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_UNITS;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_UINT16;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_SET|TWQC_RESET;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ENUMERATION;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;


	gAllCaps[iCapIndex].DefaultSupported.NumItems = UNITS;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedUnits;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultUnits;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentUnits;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;
	
		
	iCapIndex++;
	//CAP_DEVICEONLINE
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = CAP_DEVICEONLINE;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_BOOL;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ONEVALUE;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = DEVICEONLINE;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedDeviceOnLine;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultDeviceOnLine;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentDeviceOnLine;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;
	
  
	iCapIndex++;
	//CAP_AUTOFEED
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = CAP_AUTOFEED;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_BOOL;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT;
  gAllCaps[iCapIndex].Header.GetContainerType=TWON_ONEVALUE;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = AUTOFEED ;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedAutoFeed;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultAutoFeed;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentAutoFeed;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;

	iCapIndex++;
	//CAP_FEEDERLOADED
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = CAP_FEEDERLOADED;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_BOOL;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ONEVALUE;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = FEEDERLOADED;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedFeederLoaded;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultFeederLoaded;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentFeederLoaded;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;
	
#if 0
	iCapIndex++;
	//CAP_AUTOSCAN
	gAllCaps[iCapIndex].Header.CapId = CAP_AUTOSCAN;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_BOOL;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_SET|TWQC_RESET;
  gAllCaps[iCapIndex].Header.GetContainerType=TWON_ONEVALUE;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = AUTOSCAN;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedAutoScan;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultAutoScan;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentAutoScan;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;
#endif //#if 0

	iCapIndex++;
	//ICAP_AUTOBRIGHT
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_AUTOBRIGHT;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_BOOL;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_RESET|TWQC_SET;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ONEVALUE;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = AUTOBRIGHT;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedAutoBright;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultAutoBright;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentAutoBright;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;
	
	
	iCapIndex++;
	//ICAP_BRIGHTNESS	
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_BRIGHTNESS;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_FIX32;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_RESET|TWQC_SET;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_RANGE;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = BRIGHTNESS;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedBrightness;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultBrightness;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentBrightness;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;
	
	iCapIndex++;
	//ICAP_CONTRAST
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_CONTRAST;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_FIX32;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_RESET|TWQC_SET;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_RANGE;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = CONTRAST;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedContrast;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultContrast;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentContrast;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;

	iCapIndex++;
	//ICAP_HighLight
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_HIGHLIGHT;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_FIX32;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_RESET|TWQC_SET;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_RANGE;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = HIGHLIGHT;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedHighLight;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultHighLight;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentHighLight;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;
	
	iCapIndex++;
	//ICAP_ORIENTATION
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_ORIENTATION;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_UINT16;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_SET|TWQC_RESET|TWQC_RESET|TWQC_SET;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ENUMERATION;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = ORIENTATION;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedOrientation;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultOrientation;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentOrientation;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;
	

	iCapIndex++;
	//ICAP_SHADOW
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_SHADOW;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_FIX32;
  gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_SET|TWQC_RESET;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_RANGE;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = SHADOW;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedShadow;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultShadow;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentShadow;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;


	iCapIndex++;
	//ICAP_XSCALING	
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_XSCALING;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_FIX32;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_RESET|TWQC_SET;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ENUMERATION;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = XSCALING;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedXScaling;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultXScaling;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentXScaling;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;
	iCapIndex++;

	//ICAP_YSCALING
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_YSCALING;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_FIX32;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_RESET|TWQC_SET;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ENUMERATION;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = XSCALING;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedXScaling;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultXScaling;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentXScaling;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;

	iCapIndex++;
	//ICAP_BITDEPTH (BW)
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = CAP_BITDEPTHBASE+TWPT_BW/*ICAP_BITDEPTH*/;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_UINT16;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_RESET|TWQC_SET;
	gAllCaps[iCapIndex].Header.GetContainerType =TWON_ENUMERATION;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = BITDEPTHBW;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedBitDepthBW;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultBitDepthBW;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentBitDepthBW;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;
	
	iCapIndex++;
	//ICAP_BITDEPTH (Gray)
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = CAP_BITDEPTHBASE+TWPT_GRAY/*ICAP_BITDEPTH*/;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_UINT16;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_RESET|TWQC_SET;
	gAllCaps[iCapIndex].Header.GetContainerType =TWON_ENUMERATION;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = BITDEPTHGRAY;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedBitDepthGray;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultBitDepthGray;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentBitDepthGray;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;

	iCapIndex++;
	//ICAP_BITDEPTH (RGB)
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = CAP_BITDEPTHBASE+TWPT_RGB/*ICAP_BITDEPTH*/;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_UINT16;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_RESET|TWQC_SET;
	gAllCaps[iCapIndex].Header.GetContainerType =TWON_ENUMERATION;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = BITDEPTHRGB;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedBitDepthRGB;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultBitDepthRGB;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentBitDepthRGB;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;

	iCapIndex++;
	//ICAP_PIXELTYPE
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_PIXELTYPE;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_UINT16;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_SET|TWQC_RESET;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ENUMERATION;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = PIXELTYPE;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedPixelType;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultPixelType;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentPixelType;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;
	
	iCapIndex++;
	//ICAP_BITDEPTHREDUCTION
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_BITDEPTHREDUCTION;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_UINT16;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_RESET|TWQC_SET;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ENUMERATION;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = BITDEPTHREDUCTION;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedBitDepthReduction;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultBitDepthReduction;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentBitDepthReduction;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;
	
	iCapIndex++;
	//ICAP_BITORDER
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_BITORDER;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_UINT16;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ENUMERATION;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = BITORDER;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedBitOrder;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultBitOrder;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentBitOrder;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;

	
	iCapIndex++;
	//ICAP_THRESHOLD
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_THRESHOLD;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_FIX32;
  gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_RESET|TWQC_SET;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_RANGE;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = THRESHOLD;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedThreshold;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultThreshold;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentThreshold;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;

	iCapIndex++;
	//ICAP_HALFTONES
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_HALFTONES;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_STR32;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_RESET|TWQC_SET;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ENUMERATION;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = HALFTONES;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedHalfTones;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems =&gDefaultHalfTones;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentHalfTones;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;

	
	iCapIndex++;
	//ICAP_FRAMES
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE); 	 

	gAllCaps[iCapIndex].Header.CapId = ICAP_FRAMES;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_FRAME;
  gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_RESET|TWQC_SET;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ONEVALUE;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = FR;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedFrames;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultFrames;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentFrames;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;


	iCapIndex++;
	//ICAP_MAXFRAMES
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_MAXFRAMES;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_UINT16;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ONEVALUE;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = MAXFRAMES;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedMaxFrames;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultMaxFrames;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentMaxFrames;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;

	
	iCapIndex++;
	//ICAP_SUPPORTEDSIZES
  ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_SUPPORTEDSIZES;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_UINT16;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_SET|TWQC_RESET;
	gAllCaps[iCapIndex].Header.GetContainerType = TWON_ENUMERATION;
	gAllCaps[iCapIndex].Header.CurrentContainerType = TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = SUPPORTEDSIZES;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedSupportedSizes;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultSupportedSizes;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems =&gCurrentSupportedSizes; 

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;

	
	iCapIndex++;
	//ICAP_XNATIVERESOLUTION
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_XNATIVERESOLUTION;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_FIX32;
  gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ONEVALUE;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = XNATIVERESOLUTION;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedXNativeResolution;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultXNativeResolution;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentXNativeResolution;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;

	
	iCapIndex++;
	//ICAP_YNATIVERESOLUTION
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_YNATIVERESOLUTION;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_FIX32;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ONEVALUE;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = YNATIVERESOLUTION;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedYNativeResolution;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultYNativeResolution;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentYNativeResolution;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;

	iCapIndex++;
	//ICAP_XRESOLUTION	
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_XRESOLUTION;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_FIX32;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_SET|TWQC_RESET;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ENUMERATION;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = XRESOLUTION;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedXResolution;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems =  &gDefaultXResolution;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems =  &gCurrentXResolution;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;

 	
	iCapIndex++;

	//ICAP_YRESOLUTION
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_YRESOLUTION;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_FIX32;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_SET|TWQC_RESET;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ENUMERATION;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = XRESOLUTION;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedXResolution;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultXResolution;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentXResolution;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;

	iCapIndex++;
	//CAP_XFERCOUNT
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = CAP_XFERCOUNT;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_INT16;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_SET|TWQC_RESET;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ONEVALUE;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = XFERCOUNT;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedXferCount;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultXferCount;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentXferCount;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;
	
	iCapIndex++;
	//ICAP_IMAGEFILEFORMAT
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_IMAGEFILEFORMAT;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_UINT16;
  gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_RESET|TWQC_SET;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ENUMERATION;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = IMAGEFILEFORMAT;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedImageFileFormat;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultImageFileFormat;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentImageFileFormat;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;

	iCapIndex++;
	//ICAP_UNDEFINEDIMAGESIZE
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = ICAP_UNDEFINEDIMAGESIZE;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_BOOL;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_RESET|TWQC_SET;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ONEVALUE;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = UNDEFINEDIMAGESIZE;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedUndefinedImageSize;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultUndefinedImageSize;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentUndefinedImageSize;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;
	

	iCapIndex++;
	//CAP_INDICATORS
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = CAP_INDICATORS;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_BOOL;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT|TWQC_RESET|TWQC_SET;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ONEVALUE;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = INDICATORS;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedIndicators;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultIndicators;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentIndicators;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;

	iCapIndex++;
	//CAP_UICONTROLLABLE
	ASSERT(iCapIndex < SUPPORTEDCAPS+PIXELTYPE);

	gAllCaps[iCapIndex].Header.CapId = CAP_UICONTROLLABLE;
	gAllCaps[iCapIndex].Header.ItemType = TWTY_BOOL;
	gAllCaps[iCapIndex].Header.SupportedMsg = TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT;
	gAllCaps[iCapIndex].Header.GetContainerType=TWON_ONEVALUE;
	gAllCaps[iCapIndex].Header.CurrentContainerType=TWON_ONEVALUE;

	gAllCaps[iCapIndex].DefaultSupported.NumItems = UICONTROLLABLE;
	gAllCaps[iCapIndex].DefaultSupported.pItems = &gSupportedUIControllable;
					 
	gAllCaps[iCapIndex].DefaultValue.NumItems = 1;
	gAllCaps[iCapIndex].DefaultValue.pItems = &gDefaultUIControllable;

	gAllCaps[iCapIndex].CurrentValue.NumItems = 1;
	gAllCaps[iCapIndex].CurrentValue.pItems = &gCurrentUIControllable;

	gAllCaps[iCapIndex].Constraints.NumItems = 0;
	gAllCaps[iCapIndex].Constraints.pItems = NULL;

	/*
	* Finished initialized capabilities
	*/
	TRACE("InitializeCaps: initialized %d capabilities.\n", iCapIndex);

	return;
}

/*
*	Function: FreeCaps
* Author: Jon Harju / J.F.L. Peripheral Solutions Inc.
* Date: May 1998
* Inputs: none
* Outputs: none
* Comments:
*/
void FreeCaps()
{
	int iIndex = 0;

	/*
	*	Search through the capability array and clean out any constraint
	* allocations
	*/
	for(iIndex = 0; iIndex < (SUPPORTEDCAPS+PIXELTYPE); iIndex++)
	{
		if(gAllCaps[iIndex].Constraints.pItems)
		{
			/*
			*	a contraint pointer was found, was allocated with malloc
			*/
			free(gAllCaps[iIndex].Constraints.pItems);
			gAllCaps[iIndex].Constraints.pItems = NULL;
		}
		gAllCaps[iIndex].Constraints.NumItems = 0;
	}

	/*
	*	Make sure capability array is un-usable without another InitializeCaps
	* call
	*/
	memset(gAllCaps, 0, sizeof(CAP_STATESTORAGEITEM) * (SUPPORTEDCAPS+PIXELTYPE));
	
	TRACE("FreeCaps: finished cleanup.\n");
	return;
}

/***
*	TWAIN Capability interface
***/

/*
*	Function: HandleCapMessage
* Author: Jon Harju / J.F.L. Peripheral Solutions Inc.
* Date: May 1998
*		pTWMsg - complete message leading to this negotiation
*		pStatus - place to record appropriate condition code depending upon response
* Outputs:
*		TW_UINT16 - responds with appropriate TWRC_ defined values for capabilities 
* Comments:
*/
TW_UINT16 HandleCapMessage(PTWMSG pTWMsg, TW_UINT16 *pStatus)
{
	TW_UINT16 twRc = TWRC_FAILURE;
	pCAP_STATESTORAGEITEM pCapData = NULL;
	TW_UINT16 CapId = ((pTW_CAPABILITY)pTWMsg->pData)->Cap;

	ASSERT(pTWMsg);
	ASSERT(pStatus); 

	*pStatus = TWCC_CAPUNSUPPORTED;

	/*
	*	Check for special cased caps here
	*/
	switch(CapId)
	{
		case ICAP_BITDEPTH:
			TRACE("HandleCapMessage: detected BitDepth special case.\n");
			CapId = CAP_BITDEPTHBASE + GetCurrentPixelType();
			break;
		default:
			/*
			*	By default, do nothing
			*/
			break;
	}


	if(pCapData = FindCapStorageItem(CapId))
	{
		switch(pTWMsg->MSG)
		{
			case MSG_QUERYSUPPORT:
				twRc = HandleQuerySupportCap(pCapData, (pTW_CAPABILITY)pTWMsg->pData, pStatus);
				break;
			case MSG_GET:					 
				if(pCapData->Header.SupportedMsg & TWQC_GET)
				{
					twRc = HandleGetCap(pCapData, (pTW_CAPABILITY)pTWMsg->pData, pStatus);
				}
				else
				{
					twRc = TWRC_FAILURE;
					*pStatus = TWCC_CAPBADOPERATION;
				}
				break;
			case MSG_GETCURRENT:
				if(pCapData->Header.SupportedMsg & TWQC_GETCURRENT)
				{
					twRc = HandleGetCurrentCap(pCapData, (pTW_CAPABILITY)pTWMsg->pData, pStatus);
				}
				else
				{
					twRc = TWRC_FAILURE;
					*pStatus = TWCC_CAPBADOPERATION;
				}
				break;
			case MSG_GETDEFAULT:
				if(pCapData->Header.SupportedMsg & TWQC_GETDEFAULT)
				{
					twRc = HandleGetDefaultCap(pCapData, (pTW_CAPABILITY)pTWMsg->pData, pStatus);
				}
				else
				{
					twRc = TWRC_FAILURE;
					*pStatus = TWCC_CAPBADOPERATION;
				}
				break;
			case MSG_RESET: 	
				if(pCapData->Header.SupportedMsg & TWQC_RESET)
				{
					twRc = HandleReSetCap(pCapData, (pTW_CAPABILITY)pTWMsg->pData, pStatus);
				}
				else
				{
					twRc = TWRC_FAILURE;
					*pStatus = TWCC_CAPBADOPERATION;
				}
				break;
			case MSG_SET:
				if(pCapData->Header.SupportedMsg & TWQC_SET)
				{
					twRc = HandleSetCap(pCapData, (pTW_CAPABILITY)pTWMsg->pData, pStatus);
				}
				else
				{
					twRc = TWRC_FAILURE;
					*pStatus = TWCC_CAPBADOPERATION;
				}
				break;
		}
	}

	/*
	*	If the calling application is old, do not use the 1.6 condition codes
	*/
	if((appIdentity.ProtocolMajor <= 1)&&(appIdentity.ProtocolMinor <= 5))
	{
		if(*pStatus > TWCC_BADDEST)
		{
			*pStatus = TWCC_BADCAP;
		}
	}
	return twRc;
}

/*
*	Function: GetAllSettings
* Author: Jon Harju / J.F.L. Peripheral Solutions Inc.
* Date: May 1998
*		pTWMsg - complete message leading to this negotiation
*		pStatus - place to record appropriate condition code depending upon response
* Outputs:
*		TW_UINT16 - responds with appropriate TWRC_ defined values for capabilities 
* Comments:
*/
TW_UINT16 GetAllSettings(PTWMSG pTWMsg, TW_UINT16 *pStatus)
{
	TW_UINT16 twRc = TWRC_FAILURE;
	ASSERT(pStatus);
	*pStatus = TWCC_CAPUNSUPPORTED;
	
	ASSERT(FALSE); // not yet implemented

	return twRc;
}
/*
*	Function: SetAllSettings
* Author: Jon Harju / J.F.L. Peripheral Solutions Inc.
* Date: May 1998
* Inputs:
*		pTWMsg - complete message leading to this negotiation
*		pStatus - place to record appropriate condition code depending upon response
* Outputs:
*		TW_UINT16 - responds with appropriate TWRC_ defined values for capabilities 
* Comments:
*/
TW_UINT16 SetAllSettings(PTWMSG pTWMsg, TW_UINT16 *pStatus)
{
	TW_UINT16 twRc = TWRC_FAILURE;
	ASSERT(pStatus);
	*pStatus = TWCC_CAPUNSUPPORTED;
	ASSERT(FALSE); // not yet implemented	
	return twRc;
}

/***
*	High bandwidth internal capability functions
***/

/*
*	Function: GetCurrentPixelFlavor
* Author: Jon Harju / J.F.L. Peripheral Solutions Inc.
* Date: May 1998
* Inputs: none
* Outputs:
*		TW_UINT16 - current pixel flavor
* Comments:
*/
TW_UINT16 GetCurrentPixelFlavor()
{
	ASSERT(FALSE); // not yet implemented
	return 0;
}

/*
*	Function: GetCurrentXferMech
* Author: Jon Harju / J.F.L. Peripheral Solutions Inc.
* Date: May 1998
* Inputs: none
* Outputs:
*		TW_UINT16 - Current XferMech
* Comments:
*/
TW_UINT16 GetCurrentXferMech()
{
	ASSERT(FALSE); // not yet implemented
	return 0;
}


/***
*	Implementation of all module private functions
***/

/*
*	Function: FindCapStorageItem
* Author: Jon Harju / J.F.L. Peripheral Solutions Inc.
* Date: May 1998
* Inputs:
*		CapId - TWAIN Id of the capability state to find
* Outputs:
*		pCAP_STATESTORAGEITEM - NULL if no item found
* Comments:
*/
pCAP_STATESTORAGEITEM FindCapStorageItem(TW_UINT16 CapId)
{
	pCAP_STATESTORAGEITEM pStorageItem = NULL;
	int iCapIndex = 0;

	/*
	*	Search through the capability storage table and find the appropriate entry
	*/
	for(iCapIndex = 0; iCapIndex < (SUPPORTEDCAPS+PIXELTYPE); iCapIndex++)
	{
		if(gAllCaps[iCapIndex].Header.CapId == CapId)
		{
			/*
			*	found the appropriate entry
			*/
			pStorageItem = &gAllCaps[iCapIndex];
			break;
		}
	}

	#ifdef _DEBUG
	if(!pStorageItem)
	{
		TRACE("FindCapStorageItem: unable to find support for cap 0x%X.\n", CapId);
	}
	#endif //_DEBUG

	return pStorageItem;
}

/*
*	Function: 
* Author: / J.F.L. Peripheral Solutions Inc.
* Date: 1998
* Inputs:
* Outputs:
* Comments:
*/
TW_UINT16 HandleQuerySupportCap(pCAP_STATESTORAGEITEM pCapData, pTW_CAPABILITY pCap, TW_UINT16 *pStatus)
{
	pCap->ConType = TWON_ONEVALUE;

	*pStatus = BuildUpOneValue (pCap, TWTY_UINT32, &pCapData->Header.SupportedMsg, MSG_QUERYSUPPORT);

	return (*pStatus==TWCC_SUCCESS)?TWRC_SUCCESS:TWRC_FAILURE;
}
				
/*
*	Function: 
* Author: / J.F.L. Peripheral Solutions Inc.
* Date: 1998
* Inputs:
* Outputs:
* Comments:
*/
TW_UINT16 HandleGetCap(pCAP_STATESTORAGEITEM pCapData, pTW_CAPABILITY pCap, TW_UINT16 *pStatus)
{
  TW_UINT16 twRc=TWRC_FAILURE;

	/*
	*	special case feeder loaded, this is supposed to give real time feedback
	* so the current state may need to be updated
	*/
	if(pCap->Cap == CAP_FEEDERLOADED)
	{
		gCurrentFeederLoaded = IsDocumentInFeeder();
	}

	switch(pCapData->Header.GetContainerType)
	{
		case TWON_ONEVALUE:
			{
				TW_ONEVALUE twOneValue;
				memset(&twOneValue, 0, sizeof(TW_ONEVALUE));
				twOneValue.ItemType = pCapData->Header.ItemType;								
				twOneValue.ItemType = pCapData->Header.ItemType;							 					

				/*
				* if we are performing a MSG_GET and using a ONEVALUE then we are reporting
				* the "current" value, instead of reporting the "supported"
				*/
				*pStatus = BuildUpOneValue(pCap, pCapData->Header.ItemType, pCapData->CurrentValue.pItems, MSG_GET);
				twRc= (*pStatus==TWRC_SUCCESS)?TWRC_SUCCESS:TWRC_FAILURE;
			}
			break;
		case TWON_ENUMERATION:
			{
				TW_ENUMERATION twEnum;
				memset(&twEnum, 0, sizeof(TW_ENUMERATION));
				twEnum.ItemType = pCapData->Header.ItemType; 
				twEnum.NumItems = pCapData->DefaultSupported.NumItems;
				twEnum.CurrentIndex=	(int)IndexFromItem(&pCapData->DefaultSupported, pCapData->CurrentValue.pItems, AltTWItemSize(pCapData->Header.ItemType)) ;
    		twEnum.DefaultIndex=  (int) IndexFromItem(&pCapData->DefaultSupported, pCapData->DefaultValue.pItems, AltTWItemSize(pCapData->Header.ItemType)) ;
				*pStatus = BuildUpEnumerationType(pCap, &twEnum, pCapData->DefaultSupported.pItems, MSG_GET);
				twRc= (*pStatus==TWCC_SUCCESS)?TWRC_SUCCESS:TWRC_FAILURE;
			}
			break;		
		case TWON_ARRAY: 			 	
			{
				TW_ARRAY twArray;
				memset(&twArray, 0, sizeof(TW_ARRAY));
				twArray.ItemType=  pCapData->Header.ItemType;
				twArray.NumItems=	 pCapData->DefaultSupported.NumItems; 			  
		  	*pStatus = BuildUpArrayType(pCap, &twArray, pCapData->DefaultSupported.pItems, MSG_GET);
				twRc= (*pStatus==TWCC_SUCCESS)?TWRC_SUCCESS:TWRC_FAILURE;
			}
			break;
		case TWON_RANGE:
		    {				  
					TW_RANGE twRange;
					memset(&twRange, 0, sizeof(TW_RANGE));
				  twRange.ItemType     = pCapData->Header.ItemType;					
					if(twRange.ItemType == TWTY_FIX32)
					{
						*((TW_FIX32*)&twRange.MinValue) = FloatToFIX32(((float*)pCapData->DefaultSupported.pItems)[0]);
						*((TW_FIX32*)&twRange.MaxValue) = FloatToFIX32(((float*)pCapData->DefaultSupported.pItems)[1]);
						*((TW_FIX32*)&twRange.StepSize) = FloatToFIX32(((float*)pCapData->DefaultSupported.pItems)[2]);	
						*((TW_FIX32*)&twRange.CurrentValue) = FloatToFIX32(*(float*)pCapData->CurrentValue.pItems);
						*((TW_FIX32*)&twRange.DefaultValue) = FloatToFIX32(*(float*)pCapData->DefaultValue.pItems);
					}
					else if((twRange.ItemType == TWTY_UINT32)||(twRange.ItemType == TWTY_INT32))
					{
						twRange.MinValue = ((TW_UINT32*)pCapData->DefaultSupported.pItems)[0];
						twRange.MaxValue = ((TW_UINT32*)pCapData->DefaultSupported.pItems)[1];
						twRange.MinValue = ((TW_UINT32*)pCapData->DefaultSupported.pItems)[2];
						*((TW_UINT32*)&twRange.CurrentValue) = *(TW_UINT32*)pCapData->CurrentValue.pItems;
						*((TW_UINT32*)&twRange.DefaultValue) = *(TW_UINT32*)pCapData->DefaultValue.pItems;
					}
					else if((twRange.ItemType == TWTY_UINT16)||(twRange.ItemType == TWTY_INT16))
					{
						twRange.MinValue = ((TW_UINT16*)pCapData->DefaultSupported.pItems)[0];
						twRange.MaxValue = ((TW_UINT16*)pCapData->DefaultSupported.pItems)[1];
            twRange.StepSize = ((TW_UINT16*)pCapData->DefaultSupported.pItems)[2];
						*((TW_UINT16*)&twRange.CurrentValue) = *(TW_UINT16*)pCapData->CurrentValue.pItems;
						*((TW_UINT16*)&twRange.DefaultValue) = *(TW_UINT16*)pCapData->DefaultValue.pItems;
				
					}
					else
					{
						/*
						*	Unexpected value type for Range container
						*/
						ASSERT(FALSE);
					}

   			  *pStatus = BuildUpRangeType(pCap, &twRange,  MSG_GET);
					twRc= (*pStatus==TWCC_SUCCESS)?TWRC_SUCCESS:TWRC_FAILURE;
				}
			  break;
	}
	return twRc;
}

/*
*	Function: 
* Author: / J.F.L. Peripheral Solutions Inc.
* Date: 1998
* Inputs:
* Outputs:
* Comments:
*/
TW_UINT16 HandleGetCurrentCap(pCAP_STATESTORAGEITEM pCapData, pTW_CAPABILITY pCap, TW_UINT16 *pStatus)
{
	TW_UINT16 twRc=TWRC_FAILURE;
	
	switch(pCapData->Header.CurrentContainerType)
	{
		case TWON_ONEVALUE:
			 {
				 TW_ONEVALUE twOneValue;
				 memset(&twOneValue, 0, sizeof(TW_ONEVALUE));

				twOneValue.ItemType     = pCapData->Header.ItemType;		
				*pStatus = BuildUpOneValue(pCap, pCapData->Header.ItemType, pCapData->CurrentValue.pItems, MSG_GETCURRENT);
				 twRc= (*pStatus==TWCC_SUCCESS)?TWRC_SUCCESS:TWRC_FAILURE;

				 }
			break;

  	case TWON_ENUMERATION:
			{
				TW_ENUMERATION twEnum;
				if(memset(&twEnum, 0, sizeof(TW_ENUMERATION)))
				{
					twEnum.ItemType = pCapData->Header.ItemType;
					twEnum.NumItems = pCapData->CurrentValue.NumItems;
					twEnum.CurrentIndex=	(int)IndexFromItem(&pCapData->DefaultSupported, pCapData->CurrentValue.pItems, AltTWItemSize(pCapData->Header.ItemType)) ;
      		twEnum.DefaultIndex=  (int) IndexFromItem(&pCapData->DefaultSupported, pCapData->DefaultValue.pItems, AltTWItemSize(pCapData->Header.ItemType)) ;
					*pStatus = BuildUpEnumerationType(pCap, &twEnum, pCapData->CurrentValue.pItems, MSG_GETCURRENT);
					twRc= (*pStatus==TWCC_SUCCESS)?TWRC_SUCCESS:TWRC_FAILURE;
				}
				else
				{
				 return TWCC_LOWMEMORY;
				}
			}
			break;
					
		case TWON_ARRAY: 
			{
				TW_ARRAY twArray;
				if(memset(&twArray, 0, sizeof(TW_ARRAY)))
				{
					twArray.ItemType=  pCapData->Header.ItemType;
					twArray.NumItems=	 pCapData->CurrentValue.NumItems;
 					*pStatus = BuildUpArrayType(pCap, &twArray, pCapData->CurrentValue.pItems, MSG_GETCURRENT);
					twRc= (*pStatus==TWCC_SUCCESS)?TWRC_SUCCESS:TWRC_FAILURE;

				}
				else
				{
				 return TWCC_LOWMEMORY;
				}
			}
			break;
		case TWON_RANGE:
		    {				  
					TW_RANGE twRange;
				if(memset(&twRange, 0, sizeof(TW_RANGE)))
				{
				  twRange.ItemType     = pCapData->Header.ItemType;					
					if(twRange.ItemType == TWTY_FIX32)
					{
						*((TW_FIX32*)&twRange.MinValue)     = FloatToFIX32(((float*)pCapData->CurrentValue.pItems)[0]);
						*((TW_FIX32*)&twRange.MaxValue)     = FloatToFIX32(((float*)pCapData->CurrentValue.pItems)[1]);
						*((TW_FIX32*)&twRange.StepSize)     = FloatToFIX32(((float*)pCapData->CurrentValue.pItems)[2]);			
						*((TW_FIX32*)&twRange.CurrentValue) =  	*((TW_FIX32*)&twRange.MinValue) ;		
						*((TW_FIX32*)&twRange.DefaultValue) = 	*((TW_FIX32*)&twRange.MinValue)	;							
					}
					else if((twRange.ItemType == TWTY_UINT32)||(twRange.ItemType == TWTY_INT32))
					{
						twRange.MinValue         = ((TW_UINT32*)pCapData->CurrentValue.pItems)[0];
						twRange.MaxValue         = ((TW_UINT32*)pCapData->CurrentValue.pItems)[1];
						twRange.MinValue         = ((TW_UINT32*)pCapData->CurrentValue.pItems)[2];
						*((TW_UINT32*)&twRange.CurrentValue) =  *((TW_UINT32*)&twRange.MinValue) ;		
						*((TW_UINT32*)&twRange.DefaultValue) = 	*((TW_UINT32*)&twRange.MinValue)	;							
					}
					else if((twRange.ItemType == TWTY_UINT16)||(twRange.ItemType == TWTY_INT16))
					{
						twRange.MinValue         = ((TW_UINT16*)pCapData->CurrentValue.pItems)[0];
						twRange.MaxValue         = ((TW_UINT16*)pCapData->CurrentValue.pItems)[1];
            twRange.StepSize         = ((TW_UINT16*)pCapData->CurrentValue.pItems)[2];
						*((TW_UINT16*)&twRange.CurrentValue) =  	*((TW_UINT16*)&twRange.MinValue) ;		
						*((TW_UINT16*)&twRange.DefaultValue) = 	*((TW_UINT16*)&twRange.MinValue)	;							
					}
					else
					{
						/*
						*	Unexpected value type for Range container
						*/
						ASSERT(FALSE);
					}

   			*pStatus = BuildUpRangeType(pCap, &twRange,  MSG_GETCURRENT);
				twRc= (*pStatus==TWCC_SUCCESS)?TWRC_SUCCESS:TWRC_FAILURE;
				}
				else
				{
					return TWCC_LOWMEMORY;
				}
			}
			break;


	 }
	 return twRc;
}

/*
*	Function: 
* Author: / J.F.L. Peripheral Solutions Inc.
* Date: 1998
* Inputs:
* Outputs:
* Comments:
*/
TW_UINT16 HandleGetDefaultCap(pCAP_STATESTORAGEITEM pCapData, pTW_CAPABILITY pCap, TW_UINT16 *pStatus)
{
	TW_UINT16 twRc=TWRC_FAILURE;
	
	switch(pCapData->Header.CurrentContainerType)
	{
		case TWON_ONEVALUE:
		  {
				TW_ONEVALUE twOneValue;
				memset(&twOneValue, 0, sizeof(TW_ONEVALUE));
				twOneValue.ItemType     = pCapData->Header.ItemType;	
			  *pStatus = BuildUpOneValue(pCap, pCapData->Header.ItemType, pCapData->DefaultValue.pItems, MSG_GETDEFAULT);
				twRc= (*pStatus==TWCC_SUCCESS)?TWRC_SUCCESS:TWRC_FAILURE;
			}
			break;
	  case TWON_ENUMERATION:
			{
				TW_ENUMERATION twEnum;
				memset(&twEnum, 0, sizeof(TW_ENUMERATION));
				twEnum.ItemType = pCapData->Header.ItemType;
				twEnum.NumItems = pCapData->DefaultValue.NumItems;
				twEnum.CurrentIndex=	(int)IndexFromItem(&pCapData->DefaultSupported, pCapData->CurrentValue.pItems, AltTWItemSize(pCapData->Header.ItemType)) ;
    		twEnum.DefaultIndex=  (int) IndexFromItem(&pCapData->DefaultSupported, pCapData->DefaultValue.pItems, AltTWItemSize(pCapData->Header.ItemType)) ;
				*pStatus = BuildUpEnumerationType(pCap, &twEnum, pCapData->DefaultValue.pItems, MSG_GET);
				twRc= (*pStatus==TWCC_SUCCESS)?TWRC_SUCCESS:TWRC_FAILURE;
			}
			break;
		
		case TWON_ARRAY: 
			{
				TW_ARRAY twArray;
				memset(&twArray, 0, sizeof(TW_ARRAY));
				twArray.ItemType=  pCapData->Header.ItemType;
				twArray.NumItems=	 pCapData->DefaultValue.NumItems;
 			  *pStatus = BuildUpArrayType(pCap, &twArray, pCapData->DefaultValue.pItems, MSG_GET);
				twRc= (*pStatus==TWCC_SUCCESS)?TWRC_SUCCESS:TWRC_FAILURE;
			}
			break;
		case TWON_RANGE:
			{				  
				TW_RANGE twRange;
				memset(&twRange, 0, sizeof(TW_RANGE));
				twRange.ItemType     = pCapData->Header.ItemType;

				if(twRange.ItemType == TWTY_FIX32)
				{
					*((TW_FIX32*)&twRange.MinValue)     = FloatToFIX32(((float*)pCapData->DefaultValue.pItems)[0]);
					*((TW_FIX32*)&twRange.MaxValue)     = FloatToFIX32(((float*)pCapData->DefaultValue.pItems)[1]);
					*((TW_FIX32*)&twRange.StepSize)     = FloatToFIX32(((float*)pCapData->DefaultValue.pItems)[2]);			
					*((TW_FIX32*)&twRange.CurrentValue) = *((TW_FIX32*)&twRange.MinValue) ;		
					*((TW_FIX32*)&twRange.DefaultValue)= *((TW_FIX32*)&twRange.MinValue)	;			
				}
				else if((twRange.ItemType == TWTY_UINT32)||(twRange.ItemType == TWTY_INT32))
				{
					twRange.MinValue         = ((TW_UINT32*)pCapData->DefaultValue.pItems)[0];
					twRange.MaxValue         = ((TW_UINT32*)pCapData->DefaultValue.pItems)[1];
					twRange.MinValue         = ((TW_UINT32*)pCapData->DefaultValue.pItems)[2];
					*((TW_UINT32*)&twRange.CurrentValue) =  	*((TW_UINT32*)&twRange.MinValue) ;		
					*((TW_UINT32*)&twRange.DefaultValue) = 	*((TW_UINT32*)&twRange.MinValue)	;							
				}
				else if((twRange.ItemType == TWTY_UINT16)||(twRange.ItemType == TWTY_INT16))
				{
					twRange.MinValue         = ((TW_UINT16*)pCapData->DefaultValue.pItems)[0];
					twRange.MaxValue         = ((TW_UINT16*)pCapData->DefaultValue.pItems)[1];
					twRange.StepSize         = ((TW_UINT16*)pCapData->DefaultValue.pItems)[2];
					*((TW_UINT16*)&twRange.CurrentValue) =  	*((TW_UINT16*)&twRange.MinValue) ;		
					*((TW_UINT16*)&twRange.DefaultValue) = 	*((TW_UINT16*)&twRange.MinValue)	;							
				}
				else
				{
					/*
					*	Unexpected value type for Range container
					*/
					ASSERT(FALSE);
				}

				*pStatus = BuildUpRangeType(pCap, &twRange,  MSG_GETDEFAULT);
				twRc= (*pStatus==TWCC_SUCCESS)?TWRC_SUCCESS:TWRC_FAILURE;

			}
		  break;
	}
	return twRc;
}

/*
*	Function: HandleReSetCap
* Author: / J.F.L. Peripheral Solutions Inc.
* Date: 1998
* Inputs:
* Outputs:
* Comments:
*/

TW_UINT16 HandleReSetCap(pCAP_STATESTORAGEITEM pCapData, pTW_CAPABILITY pCap, TW_UINT16 *pStatus)
{  
	/*
	*	initialize current to 0
	* validate number of items in current == number of items in default
	* set current = default
	*/
	memset(pCapData->CurrentValue.pItems, 0, pCapData->CurrentValue.NumItems*AltTWItemSize(pCapData->Header.ItemType));
	ASSERT(pCapData->CurrentValue.NumItems == pCapData->DefaultValue.NumItems);
	memcpy(pCapData->CurrentValue.pItems, pCapData->DefaultValue.pItems, pCapData->CurrentValue.NumItems*AltTWItemSize(pCapData->Header.ItemType));

	return HandleGetDefaultCap(pCapData, pCap, pStatus);
}

/*
*	Function: HandleSetCap
* Author: / J.F.L. Peripheral Solutions Inc.
* Date: 1998
* Inputs:
* Outputs:
* Comments:
*/
TW_UINT16 HandleSetCap(pCAP_STATESTORAGEITEM pCapData, pTW_CAPABILITY pCap, TW_UINT16 *pStatus)
{	//1
  int i=0;
	TW_UINT16 twRc = TWRC_FAILURE;
	*pStatus = TWCC_BADVALUE;

	switch(pCap->ConType)
	{
		/*
		* This is easiest one, we are noly setting "current" state
		*/
		case TWON_ONEVALUE:	//itemType, itemValue
		{
			pTW_ONEVALUE pOne=(pTW_ONEVALUE)GlobalLock(pCap->hContainer);				
			if(pOne)
			{
				if(pOne->ItemType == pCapData->Header.ItemType)	 // set at find time for pCapdata....
				{
					void *pAppValue = malloc(AltTWItemSize(pCapData->Header.ItemType));	
					//only one value allocated	at pointer pAppVAlue
					if(pAppValue)
					{
						TW_UINT16 twSize = AltTWItemSize(pCapData->Header.ItemType); //return would be sizeof item
						ExtractOneValue(pCap, pAppValue);	//determined at memory level the type requested

						if(pOne->ItemType==TWTY_FIX32)
						{
							/*
							*	Special case, our storage contains float values, not TW_FIX32
							* Values have to be converted before copied.  Size much also change
							*/
							*(float*)pAppValue = FIX32ToFloat(*(pTW_FIX32)pAppValue);
							twSize = sizeof(float);
						}
					
						/*
						*	make a loop to compare all DefaultSupported items with the pAppValue
						*	memcmp for each item
						* if you find a match, then set current to pAppValue
						* else do NOT CHANGE Current value
						*/
						if(IsItemInList(&pCapData->DefaultSupported, pAppValue, twSize))
						{
							memcpy(pCapData->CurrentValue.pItems, pAppValue, twSize); //casting problem

							twRc = TWRC_SUCCESS;
							*pStatus = TWCC_SUCCESS;

							free(pAppValue);
							pAppValue = NULL;

							GlobalUnlock(pCap->hContainer);
							break;
						}
						free(pAppValue);
						pAppValue = NULL;
					}	//if pAppValue
				}	//if poneitem
				GlobalUnlock(pCap->hContainer);
			}	//if pone
		}	 //case onevalue
		break;
		/*
		*	Constraints are being imposed
		*/
#if 0
	case TWON_ENUMERATION:
			{
				memset(pCapData->CurrentValue.pItems, 0, pCapData->CurrentValue.NumItems*AltTWItemSize(pCapData->Header.ItemType));
				ASSERT(pCapData->CurrentValue.NumItems == pCapData->CurrentValue.NumItems*AltTWItemSize(pCapData->Header.ItemType));			
				memcpy(pCapData->CurrentValue.pItems,  pCapData->CurrentValue.NumItems*AltTWItemSize(pCapData->Header.ItemType));
			
				memset(pCapData->DefaultSupported.pItems, 0, pCapData->DefaultSupported.NumItems*AltTWItemSize(pCapData->Header.ItemType));
				ASSERT(pCapData->DefaultSupported.NumItems == pCapData->DefaultSupported.NumItems*AltTWItemSize(pCapData->Header.ItemType));							
				memcpy(pCapData->DefaultSupported.pItems,  pCapData->DefaultSupported.NumItems*AltTWItemSize(pCapData->Header.ItemType));


				TW_ENUMERATION twEnum;
				memset(&twEnum, 0, sizeof(TW_ENUMERATION));
				twEnum.ItemType = pCapData->Header.ItemType;
				twEnum.NumItems = pCapData->CurrentValue.NumItems;

				*pStatus = BuildUpEnumerationType(pCap, &twEnum, pCapData->CurrentValue.pItems, MSG_GET);
			}
		  break;	
#endif //#if 0
		/*
		*	Arrays may simply not be supported in our model (only EXTENDEDCAPS is documented as allowing it
		* and it has limited use)
		*/
/*	case TWON_ARRAY: 
			{  		  
				TW_ARRAY twArray;
				memset(&twArray, 0, sizeof(TW_ARRAY)); 
  			memset(pCapData->CurrentValue.pItems, 0, pCapData->CurrentValue.NumItems*AltTWItemSize(pCapData->Header.ItemType));
	  		ASSERT(pCapData->CurrentValue.NumItems = pCapData->CurrentValue.NumItems*AltTWItemSize(pCapData->Header.ItemType));
		  	memcpy(pCapData->CurrentValue.pItems,    (pTW_ONEVALUE)GlobalLock(pCap->hContainer), pCapData->CurrentValue.NumItems*AltTWItemSize(pCapData->Header.ItemType));
			  memcmp(pCapData->CurrentValue.pItems,  (pTW_ONEVALUE)HandleGetCurrentCap(pCapData, pCap, pStatus),	pCapData->CurrentValue.NumItems*AltTWItemSize(pCapData->Header.ItemType));
				twArray.ItemType=  pCapData->Header.ItemType;
				twArray.NumItems=	 pCapData->CurrentValue.NumItems;

 			  *pStatus = BuildUpArrayType(pCap, &twArray, pCapData->CurrentValue.pItems, MSG_GETCURRENT);
			}
			break;
		/*
		*	Constraints are being imposed
		*/
/*  case TWON_RANGE:
			{				  
				memset(pCapData->CurrentValue.pItems, 0, pCapData->CurrentValue.NumItems*AltTWItemSize(pCapData->Header.ItemType));
				ASSERT(pCapData->CurrentValue.NumItems == pCapData->CurrentValue.NumItems*AltTWItemSize(pCapData->Header.ItemType));			
				memcpy(pCapData->CurrentValue.pItems,  pCapData->CurrentValue.NumItems*AltTWItemSize(pCapData->Header.ItemType));
			
				memset(pCapData->DefaultSupported.pItems, 0, pCapData->DefaultSupported.NumItems*AltTWItemSize(pCapData->Header.ItemType));
				ASSERT(pCapData->DefaultSupported.NumItems == pCapData->DefaultSupported.NumItems*AltTWItemSize(pCapData->Header.ItemType));							
				memcpy(pCapData->DefaultSupported.pItems,  pCapData->DefaultSupported.NumItems*AltTWItemSize(pCapData->Header.ItemType));

				TW_RANGE twRange;
				memset(&twRange, 0, sizeof(TW_RANGE));
				twRange.ItemType     = pCapData->Header.ItemType;
				
				if(twRange.ItemType == TWTY_FIX32)
				{
					*((TW_FIX32*)&twRange.MinValue)     = FloatToFIX32(((float*)pCapData->CurrentValue.pItems)[0]);
					*((TW_FIX32*)&twRange.MaxValue)     = FloatToFIX32(((float*)pCapData->CurrentValue.pItems)[1]);
					*((TW_FIX32*)&twRange.StepSize)     = FloatToFIX32(((float*)pCapData->CurrentValue.pItems)[2]);			
				}
				else if((twRange.ItemType == TWTY_UINT32)||(twRange.ItemType == TWTY_INT32))
				{
					twRange.MinValue         = ((TW_UINT32*)pCapData->CurrentValue.pItems)[0];
					twRange.MaxValue         = ((TW_UINT32*)pCapData->CurrentValue.pItems)[1];
					twRange.MinValue         = ((TW_UINT32*)pCapData->CurrentValue.pItems)[2];
				}
				else if((twRange.ItemType == TWTY_UINT16)||(twRange.ItemType == TWTY_INT16))
				{
					twRange.MinValue         = ((TW_UINT16*)pCapData->CurrentValue.pItems)[0];
					twRange.MaxValue         = ((TW_UINT16*)pCapData->CurrentValue.pItems)[1];
					twRange.StepSize         = ((TW_UINT16*)pCapData->CurrentValue.pItems)[2];
				}
				else
				{
					/*
					*	Unexpected value type for Range container
					*/
				/*	ASSERT(FALSE);
				}

		*pStatus = BuildUpRangeType(pCap, &twRange,  MSG_GET);
		}
		break;
*/
	}				
  return twRc;
}		 

//////////////////////////////////////////////////////////////////////////////
// FloatToFIX32 -- Convert a floating point value into a FIX32.
//
TW_FIX32 FloatToFIX32 (float floater)
{
  TW_FIX32 Fix32_value;

  TW_INT32 value = (TW_INT32) (floater * 65536.0 + 0.5);
  Fix32_value.Whole = LOWORD(value >> 16);
  Fix32_value.Frac = LOWORD(value & 0x0000ffffL);

  return Fix32_value;
}

//////////////////////////////////////////////////////////////////////////////
// FIX32ToFloat -- Convert a FIX32 value into a floating point value
//
float FIX32ToFloat (TW_FIX32 fix32)
{
	float   floater;
	floater = (float) fix32.Whole + (float) (fix32.Frac / 65536.0);
	return(floater);
}

BOOL IsItemInList(pCAP_ITEMVALUE pCapSupported, void *pItem, size_t ItemSize)
{
	int i = 0;
	
	BYTE *pList = NULL;
	BYTE *pCompareItem = NULL;

	pList = (BYTE *)pCapSupported->pItems;
	pCompareItem = (BYTE *)pItem;
	for (i=0; i < (int)pCapSupported->NumItems; i++)
	{
	  if( memcmp( pList,pCompareItem, ItemSize )==0)			  										 
		{
		  return TRUE;
		}
		pList += ItemSize;	//Itemsize is the result of size of an Item
	}						
	return FALSE;
}

BOOL IsItemInRange(pCAP_ITEMVALUE pCapSupported, void *pItem, TW_UINT16 ItemType)
{
	BOOL bRC = FALSE;
	int i = 0;
	
	/*
	*	Setup for general type "free" inspection of the range
	*/
	TW_UINT16 twItemSize = AltTWItemSize(ItemType);
	void *pCurrValue = malloc(twItemSize);
	void *pMaxValue = malloc(twItemSize);
	void *pStepValue = malloc(twItemSize);

	/*
	*	Consistent with range type internal storage
	*/
	ASSERT(pCapSupported->NumItems == 3);

	if(pCurrValue && pMaxValue && pStepValue)
	{
		if(ItemType == TWTY_FIX32)
		{
			TW_FIX32 Convert = FloatToFIX32(((float*)pCapSupported->pItems)[0]);
			AssignLongToValue(*(pTW_UINT32)&Convert, pCurrValue, ItemType);

			Convert = FloatToFIX32(((float*)pCapSupported->pItems)[1]);
			AssignLongToValue(*(pTW_UINT32)&Convert, pMaxValue, ItemType);

			Convert = FloatToFIX32(((float*)pCapSupported->pItems)[2]);
			AssignLongToValue(*(pTW_UINT32)&Convert, pStepValue, ItemType);
		}
		else
		{
			AssignLongToValue(*(pTW_UINT32)pCapSupported->pItems, pCurrValue, ItemType);
			AssignLongToValue(*(pTW_UINT32)(((BYTE*)pCapSupported->pItems)+twItemSize), pMaxValue, ItemType);
			AssignLongToValue(*(pTW_UINT32)(((BYTE*)pCapSupported->pItems)+(twItemSize*2)), pStepValue, ItemType);
		}		

		for(; IsItemLessOrEqual(pCurrValue, pMaxValue, ItemType);
					IncrementValue(pCurrValue, pStepValue, ItemType))
		{
			if( memcmp( pCurrValue, pItem, twItemSize )==0)
			{
				bRC = TRUE;
				break;
			}
		}
	}

	/*
	*	Cleanup
	*/
	if(pCurrValue)
	{
		free(pCurrValue);
	}

	if(pMaxValue)
	{
		free(pMaxValue);
	}

	if(pStepValue)
	{
		free(pStepValue);
	}

	return bRC;
}

TW_UINT32 IndexFromItem(pCAP_ITEMVALUE pCapSupported, void *pItem, size_t ItemSize)
{
	int i = 0;	
	BYTE *pList = NULL;
	BYTE *pCompareItem = NULL;

	pList =   (BYTE*)pCapSupported->pItems;
	pCompareItem = (BYTE*)pItem;
	for (i=0; i < (int)pCapSupported->NumItems; i++)
	{
	  if( memcmp( pList,pCompareItem, ItemSize )==0)			  										 
		{
		  return (TW_UINT32)i;
		}
		pList += ItemSize;	//Itemsize is the result of size of an Item
	}						
	return (TW_UINT32)i;//(TW_UINT32)-1;
}


pCAP_ITEMVALUE AllocateUnion(pCAP_ITEMVALUE pList1, pCAP_ITEMVALUE pList2, size_t ItemSize)
{
	// allocate a CAP_ITEMVALUE
	// count # of shared items
	// allocate pItems for new set
	// copy shared items to new set
	// return address of new CAP_ITEMVALUE
	pCAP_ITEMVALUE pCapItemValue= NULL;
	int iNumShared = 0;
	BYTE *pSource = NULL;
	int i = 0;
	pSource = (BYTE*)pList1->pItems;
	/*
	*	Count how many matches
	*/
	
	for (i=0; i < (int)pList1->NumItems; i++)
	{
		if(IsItemInList(pList2, pSource, ItemSize))
		{
			iNumShared++;
		}
		pSource+=ItemSize;
	}


	if(iNumShared)
	{
		BYTE *pDestination = NULL;

		BYTE *pArrayByte   = NULL;
		/*
		*	Allocate a CAP_ITEMVALUE                        
		*/
		pCapItemValue = (pCAP_ITEMVALUE)malloc(sizeof(CAP_ITEMVALUE));
		/*
		*	Allocate an array of bytes of size iNumShared * ItemSize
		*/
		pArrayByte    = (BYTE*)malloc(iNumShared * ItemSize);

		/*
		*	Assign this to pDestination & also to pList1->pItems
		*/
		pDestination   =pArrayByte;
		pSource = (BYTE*)pList1->pItems;

		/*
		*	Set pList12->NumItems = iNumShared;
		*/
    pCapItemValue->NumItems = iNumShared;
	  pCapItemValue->pItems = pArrayByte;
		/*

		*	Same loop from above, but instead of iNumShared++, Copy current item to destination
		* increment destination pointer to point to next item
		*/
		 for (i=0; i < iNumShared; i++)
  	{
	     	if(IsItemInList(pList2, pSource, ItemSize))
				{
					memcpy(pDestination, pSource,ItemSize);
    			pDestination += ItemSize;
				 }
	  	    pSource+= ItemSize;
	   }						
   
	 }
	 return  pCapItemValue; 
}

float *GetSupportedXResolution(int *pNumItems)
{
 *pNumItems = XRESOLUTION;
 return gSupportedXResolution;
}

float GetCurrentXResolution()
{
 return gCurrentXResolution;
}

void SetCurrentXResolution(float value)
{
 gCurrentXResolution=value;
}


TW_UINT16 *GetSupportedBitDepth(int *pNumItems, TW_UINT16 PixelType)
{
	switch(PixelType)
	{
		case TWPT_BW:
			*pNumItems = BITDEPTHBW;
			return gSupportedBitDepthBW;
			break;
		case TWPT_GRAY:
			*pNumItems = BITDEPTHGRAY;
			return gSupportedBitDepthGray;
			break;
		case TWPT_RGB:
			*pNumItems = BITDEPTHRGB;
			return gSupportedBitDepthRGB;
			break;
	}

	TRACE("GetSupportedBitDepth: unsupported pixeltype\n");
	*pNumItems = 0;
	return NULL;
}

TW_UINT16 GetCurrentBitDepth(TW_UINT16 PixelType)
{
	switch(PixelType)
	{
		case TWPT_BW:
			return gCurrentBitDepthBW;
			break;
		case TWPT_GRAY:
			return gCurrentBitDepthGray;
			break;
		case TWPT_RGB:
			return gCurrentBitDepthRGB;
			break;
	}
	TRACE("GetCurrentBitDepth: unsupported pixeltype\n");
	return 0;
}

void SetCurrentBitDepth(TW_UINT16 value, TW_UINT16 PixelType)
{
	switch(PixelType)
	{
		case TWPT_BW:
			gCurrentBitDepthBW=value;
			return;
			break;
		case TWPT_GRAY:
			gCurrentBitDepthGray=value;
			return;
			break;
		case TWPT_RGB:
			gCurrentBitDepthRGB=value;
			return;
			break;
	}
	TRACE("SetCurrentBitDepth: unsupported pixeltype\n");
	return;
}

TW_UINT16 *GetSupportedPixelType(int *pNumItems)
{
 *pNumItems = PIXELTYPE;
 return gSupportedPixelType;
}

TW_UINT16 GetCurrentPixelType()
{
  return gCurrentPixelType;
}

void SetCurrentPixelType(TW_UINT16 value)
{
  gCurrentPixelType=value;
}

TW_BOOL *GetSupportedFeederEnabled(int *pNumItems)
{
 *pNumItems = FEEDERENABLED;
 return gSupportedFeederEnabled;
}

TW_BOOL GetCurrentFeederEnabled()
{
	return gCurrentFeederEnabled;
}

void SetCurrentFeederEnabled(TW_BOOL value)
{
	gCurrentFeederEnabled=value;
	return;
}

float *GetSupportedXScaling(int *pNumItems)
{
 *pNumItems = XSCALING;
 return gSupportedXScaling;
}

float GetCurrentXScaling()
{
   return gCurrentXScaling;
}

void SetCurrentXScaling(float value)		 
{
  gCurrentXScaling=value;
}

TW_UINT16 *GetSupportedSupportedSizes(int *pNumItems)
{
 *pNumItems = SUPPORTEDSIZES ;
 return gSupportedSupportedSizes;
}

TW_UINT16 GetCurrentSupportedSizes()
{
   return gCurrentSupportedSizes;
}

void SetCurrentSupportedSizes(TW_UINT16 value)
{
   gCurrentSupportedSizes=value;
}

TW_INT16 GetXferCount()
{
	return gCurrentXferCount;
}

int CompareEnumToSupported(pTW_CAPABILITY pCap, pCAP_ITEMVALUE pSupported, TW_UINT16 GetContainerType)
{
	int iRet = -1;
	int iIndex = 0;
	int iMatches = 0;
	TW_UINT16 twSize = 0;
	void *pAppValue = NULL;
	pTW_ENUMERATION pEnum = NULL;

	/*
	*	Various assumptions made by this function
	*/
	ASSERT(pCap);
	ASSERT(pSupported);
	ASSERT(pCap->ConType == TWON_ENUMERATION);
	
	/*
	*	Lock a pointer to the container
	*/
	pEnum = (pTW_ENUMERATION)GlobalLock(pCap->hContainer);
	if(pEnum)
	{
		/*
		*	Compute the size of the type, allocate local storage
		*/
		twSize = AltTWItemSize(pEnum->ItemType); //return would be sizeof item
		pAppValue = malloc(twSize);
		
		if(pAppValue)
		{
			for(iIndex = 0; iIndex < (int)pEnum->NumItems; iIndex++)
			{
				ExtractEnumerationValue(pCap, pAppValue, iIndex);	//determined at memory level the type requested

				if(pEnum->ItemType==TWTY_FIX32)
				{
					/*
					*	Special case, our storage contains float values, not TW_FIX32
					* Values have to be converted before copied.  Size much also change
					*/
					ASSERT(sizeof(float) <= sizeof(TW_FIX32)); // oops
					*(float*)pAppValue = FIX32ToFloat(*(pTW_FIX32)pAppValue);
					twSize = sizeof(float);
				}
				
				if((GetContainerType==TWON_RANGE)&&(IsItemInRange(pSupported, pAppValue, twSize)))
				{
					/*
					* We have a match in a supported Range
					*/
					iMatches++;
				}
				else if((GetContainerType!=TWON_RANGE)&&(IsItemInList(pSupported, pAppValue, twSize)))
				{
					/*
					* We have a match in a supported array
					*/
					iMatches++;
				}
			}
			free(pAppValue);
		}

		/*
		*	Unlock container
		*/
		GlobalUnlock(pCap->hContainer);
		pEnum = NULL;
	}

	/*
	*	Set the correct return code, defaults to "none" of the values found
	*/
	if(iMatches == (int)pEnum->NumItems)
	{
		/*
		*	"all" the values were found
		*/
		iRet = 1;
	}
	else if(iMatches == 0)
	{
		/*
		*	"some" of the values were found
		*/
		iRet = 0;
	}
	return iRet;
}

int CompareRangeToSupported( pTW_CAPABILITY pCap, pCAP_ITEMVALUE pSupported, TW_UINT16 GetContainerType)
{
	float TmpFloat = 0.0F;
	pTW_RANGE pRange = NULL;
	int iRet = -1;
	int iMatches = 0;
	int iNumRangeItems = 0;

	/*
	*	Various assumptions made by this function
	*/
	ASSERT(pCap);
	ASSERT(pSupported);
	ASSERT(pCap->ConType == TWON_RANGE);
	
	/*
	*	Lock a pointer to the container
	*/
	pRange = (pTW_RANGE)GlobalLock(pCap->hContainer);
	if(pRange)
	{
		/*
		*	Compute the size of the type, allocate local storage
		*/
		TW_UINT16 twSize = AltTWItemSize(pRange->ItemType); //return would be sizeof item
		
		/*
		*	Setup for general type "free" inspection of the range
		*/
		void *pCurrValue = malloc(twSize);
		void *pMaxValue = malloc(twSize);
		void *pStepValue = malloc(twSize);

		if(pCurrValue && pMaxValue && pStepValue)
		{
			AssignLongToValue(pRange->MinValue, pCurrValue, pRange->ItemType);
			AssignLongToValue(pRange->MaxValue, pMaxValue, pRange->ItemType);
			AssignLongToValue(pRange->StepSize, pStepValue, pRange->ItemType);

			for(AssignLongToValue(pRange->MinValue, pCurrValue, pRange->ItemType); 
				IsItemLessOrEqual(pCurrValue, pMaxValue, pRange->ItemType); 
					IncrementValue(pCurrValue, pStepValue, pRange->ItemType))
			{
				iNumRangeItems++;
				if(pRange->ItemType==TWTY_FIX32&&GetContainerType!=TWON_RANGE)
				{
					/*
					*	Special case, our storage contains float values, not TW_FIX32
					* Values have to be converted before copied.  Size much also change
					*/
					TmpFloat = FIX32ToFloat(*(pTW_FIX32)pCurrValue);
					twSize = sizeof(float);

					if(IsItemInList(pSupported, &TmpFloat, twSize))
					{
						/*
						* We have a match
						*/
						iMatches++;
					}
				}
				else
				{
					if(IsItemInList(pSupported, pCurrValue, twSize))
					{
						/*
						* We have a match
						*/
						iMatches++;
					}
				}
			}
		}

		/*
		*	Cleanup
		*/
		if(pCurrValue)
		{
			free(pCurrValue);
		}

		if(pMaxValue)
		{
			free(pMaxValue);
		}

		if(pStepValue)
		{
			free(pStepValue);
		}

		/*
		*	Unlock container
		*/
		GlobalUnlock(pCap->hContainer);
		pRange = NULL;
	}

	/*
	*	Set the correct return code, defaults to "none" of the values found
	*/
	if(iMatches == iNumRangeItems)
	{
		/*
		*	"all" the values were found
		*/
		iRet = 1;
	}
	else if(iMatches == 0)
	{
		/*
		*	"some" of the values were found
		*/
		iRet = 0;
	}
	return iRet;
}

int CompareRangeToSupportedRange( pTW_CAPABILITY pCap, pCAP_ITEMVALUE pSupported)
{
	int iRet = -1;
	ASSERT(pCap);
	ASSERT(pSupported);
	return iRet;
}

/*
*	Following functions are used for creating Union Lists
*/
CAP_ITEMVALUE AllocateUnionEnumToEnum( pTW_CAPABILITY pCap, pCAP_ITEMVALUE pSupported)
{
	CAP_ITEMVALUE Item = {0l, NULL};
	ASSERT(pCap);
	ASSERT(pSupported);
	return Item;
}

CAP_ITEMVALUE AllocateUnionEnumToRange( pTW_CAPABILITY pCap, pCAP_ITEMVALUE pSupported)
{
	CAP_ITEMVALUE Item = {0l, NULL};
	ASSERT(pCap);
	ASSERT(pSupported);
	return Item;
}

CAP_ITEMVALUE AllocateUnionRangeToEnum( pTW_CAPABILITY pCap, pCAP_ITEMVALUE pSupported)
{
	CAP_ITEMVALUE Item = {0l, NULL};
	ASSERT(pCap);
	ASSERT(pSupported);
	return Item;
}

BOOL IsItemLessOrEqual(void *pItem1, void *pItem2, TW_UINT16 ItemType)
{
	BOOL bRc = FALSE;
	switch(ItemType)
	{
		case TWTY_UINT8:
			if((*(pTW_UINT8)pItem1)<=(*(pTW_UINT8)pItem2))
			{
				bRc = TRUE;
			}
			break;
		case TWTY_UINT16:
			if((*(pTW_UINT16)pItem1)<=(*(pTW_UINT16)pItem2))
			{
				bRc = TRUE;
			}
			break;
		case TWTY_UINT32:
			if((*(pTW_UINT32)pItem1)<=(*(pTW_UINT32)pItem2))
			{
				bRc = TRUE;
			}
			break;
		case TWTY_INT8:
			if((*(pTW_INT8)pItem1)<=(*(pTW_INT8)pItem2))
			{
				bRc = TRUE;
			}
			break;
		case TWTY_INT16:
			if((*(pTW_INT16)pItem1)<=(*(pTW_INT16)pItem2))
			{
				bRc = TRUE;
			}
			break;
		case TWTY_INT32:
			if((*(pTW_INT32)pItem1)<=(*(pTW_INT32)pItem2))
			{
				bRc = TRUE;
			}
			break;
		case TWTY_FIX32:
			if(FIX32ToFloat((*(pTW_FIX32)pItem1))<=FIX32ToFloat(((*(pTW_FIX32)pItem2))))
			{
				bRc = TRUE;
			}
			break;
		case TWTY_STR32:
		case TWTY_STR64:
		case TWTY_STR128:
		case TWTY_STR255:
			/*
			*	Invalid type for this operation
			*/
			ASSERT(FALSE);
			break;
	}
	return bRc;
}

void IncrementValue(void *pItem, void *pStepSize, TW_UINT16 ItemType)
{
	switch(ItemType)
	{
		case TWTY_UINT8:
			(*(pTW_UINT8)pItem) += (*(pTW_UINT8)pStepSize);
			break;
		case TWTY_UINT16:
			(*(pTW_UINT16)pItem) += (*(pTW_UINT16)pStepSize);
			break;
		case TWTY_UINT32:
			(*(pTW_UINT32)pItem) += (*(pTW_UINT32)pStepSize);
			break;
		case TWTY_INT8:
			(*(pTW_INT8)pItem) += (*(pTW_INT8)pStepSize);
			break;
		case TWTY_INT16:
			(*(pTW_INT16)pItem) += (*(pTW_INT16)pStepSize);
			break;
		case TWTY_INT32:
			(*(pTW_INT32)pItem) += (*(pTW_INT32)pStepSize);
			break;
		case TWTY_FIX32:
			(*(pTW_FIX32)pItem) = FloatToFIX32( FIX32ToFloat(*(pTW_FIX32)pItem) + FIX32ToFloat(*(pTW_FIX32)pStepSize) );
			break;
		default:
			/*
			*	Invalid type for this operation
			*/
			ASSERT(FALSE);
			break;
	}
	return;
}

void AssignLongToValue(TW_UINT32 LongValue, void *pDestValue, TW_UINT16 ItemType)
{
	switch(ItemType)
	{
		case TWTY_INT8:
		case TWTY_UINT8:
			(*(pTW_UINT8)pDestValue) += (TW_UINT8)LongValue;
			break;
		case TWTY_INT16:
		case TWTY_UINT16:
			(*(pTW_UINT16)pDestValue) += (TW_UINT16)LongValue;
			break;
		case TWTY_FIX32:
		case TWTY_UINT32:
		case TWTY_INT32:
			(*(pTW_UINT32)pDestValue) += (TW_UINT32)LongValue;
			break;
		default:
			/*
			*	Invalid type for this operation
			*/
			ASSERT(FALSE);
			break;
	}
	return;
}
