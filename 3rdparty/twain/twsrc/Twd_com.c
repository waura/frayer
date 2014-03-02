/*
*	TWAIN Sample Source 
*
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
*	This module contains routines to manipulate containers.  Containers are
*	used in CAPabilty negotiations to exchange information about the input
*	device between the Source and the App.  The information may define data
*	exchange or input device characteristics.
*/

#include <windows.h>
#include "..\\twcommon\\twain.h"
#include "twd_com.h"
#include "..\\twcommon\\twndebug.h"

#include "twd_type.h"
TW_FIX32 FloatToFIX32 (float floater);
float FIX32ToFloat (TW_FIX32 fix32);

/*
	* FUNCTION: ExtractEnumerationValue
	*
	* ARGS:    pData   pointer to a capability structure, details about container
	*          pVoid   ptr will be set to point to the item on return
	*          j       requested index into the enumeration
	*
	* RETURNS: pVoid   is set to pointer to itemtype
	*
	* NOTES:   This routine will open a container and extract the Item.  The 
	* Item will be returned to the caller in pVoid.  I will type cast the returned 
	* value to that of ItemType.
	*
	* COMMENTS: only a single value is returned.  It is referred to by the 'j' 
	*  index.
	*
	* BACKGROUND:  Protocol: used by MSG_SET calls were Source empties then App frees 
	* upon return.  It is assumed that the APP allocates and fills the container 
	* prior to this call.
 */
VOID ExtractEnumerationValue (pTW_CAPABILITY pData, LPVOID pVoid, int j)
{
	pTW_ENUMERATION pEnumeration = NULL;
	LPVOID pItemList = NULL;

	ASSERT(pVoid);
	ASSERT(pData);
	ASSERT(pData->ConType == TWON_ENUMERATION);

	if ((pEnumeration = (pTW_ENUMERATION)GlobalLock(pData->hContainer)) != NULL)
	{
		// assign base address of ItemList array to 'generic' pointer
		pItemList = (LPVOID)pEnumeration->ItemList;
	} // CAST to type of var caller wants

	switch (pEnumeration->ItemType)
	{
		case TWTY_INT8:
			*(pTW_INT8)pVoid = *((pTW_INT8)pItemList+j);
			break;

		case TWTY_UINT8:
			*(pTW_UINT8)pVoid = *((pTW_UINT8)pItemList+j);
			break;

		case TWTY_INT16:
			*(pTW_INT16)pVoid = *((pTW_INT16)pItemList+j);
			break;

		case TWTY_UINT16:
			*(pTW_UINT16)pVoid = *((pTW_UINT16)pItemList+j);
			break;

		case TWTY_INT32:
			*(pTW_INT32)pVoid = *((pTW_INT32)pItemList+j);
			break;

		case TWTY_UINT32:
			*(pTW_UINT32)pVoid = *((pTW_UINT32)pItemList+j);
			break;

		case TWTY_BOOL:
			*(pTW_BOOL)pVoid = *((pTW_BOOL)pItemList+j);
			break;


		case TWTY_FIX32:
			*(float *)pVoid = FIX32ToFloat(*((pTW_FIX32)pItemList+j));
			break;

		case TWTY_FRAME:
			((pINTERNALFRAMESTRUCT)pVoid)->Left = FIX32ToFloat(((pTW_FRAME)pItemList+j)->Left);
			((pINTERNALFRAMESTRUCT)pVoid)->Top = FIX32ToFloat(((pTW_FRAME)pItemList+j)->Top);
			((pINTERNALFRAMESTRUCT)pVoid)->Right = FIX32ToFloat(((pTW_FRAME)pItemList+j)->Right);
			((pINTERNALFRAMESTRUCT)pVoid)->Bottom = FIX32ToFloat(((pTW_FRAME)pItemList+j)->Bottom);
			break;

		case TWTY_STR32:
			*(pTW_STR32)pVoid = *((pTW_STR32)pItemList+j);
			break;

		case TWTY_STR64:
			*(pTW_STR64)pVoid = *((pTW_STR64)pItemList+j);
			break;

		case TWTY_STR128:
			*(pTW_STR128)pVoid = *((pTW_STR128)pItemList+j);
			break;

		case TWTY_STR255:
			*(pTW_STR255)pVoid = *((pTW_STR255)pItemList+j);
			break;

		default:
			break;
	}

	GlobalUnlock(pData->hContainer);
	// free is by the App
	return;
} // ExtractEnumerationValue


VOID ExtractArrayValue (pTW_CAPABILITY pData, LPVOID pVoid, int j)
{
	pTW_ARRAY pArray = NULL;
	LPVOID pItemList = NULL;

	ASSERT(pVoid);
	ASSERT(pData);
	ASSERT(pData->ConType == TWON_ARRAY);

	if ((pArray = (pTW_ARRAY)GlobalLock(pData->hContainer)) != NULL)
	{
		// assign base address of ItemList array to 'generic' pointer
		pItemList = (LPVOID)pArray->ItemList;
	} // CAST to type of var caller wants

	switch (pArray->ItemType)
	{
		case TWTY_INT8:
			*(pTW_INT8)pVoid = *((pTW_INT8)pItemList+j);
			break;

		case TWTY_UINT8:
			*(pTW_UINT8)pVoid = *((pTW_UINT8)pItemList+j);
			break;

		case TWTY_INT16:
			*(pTW_INT16)pVoid = *((pTW_INT16)pItemList+j);
			break;

		case TWTY_UINT16:
			*(pTW_UINT16)pVoid = *((pTW_UINT16)pItemList+j);
			break;

		case TWTY_INT32:
			*(pTW_INT32)pVoid = *((pTW_INT32)pItemList+j);
			break;

		case TWTY_UINT32:
			*(pTW_UINT32)pVoid = *((pTW_UINT32)pItemList+j);
			break;

		case TWTY_BOOL:
			*(pTW_BOOL)pVoid = *((pTW_BOOL)pItemList+j);
			break;


		case TWTY_FIX32:
			*(float *)pVoid = FIX32ToFloat(*((pTW_FIX32)pItemList+j));
			break;

		case TWTY_FRAME:
			((pINTERNALFRAMESTRUCT)pVoid)->Left = FIX32ToFloat(((pTW_FRAME)pItemList+j)->Left);
			((pINTERNALFRAMESTRUCT)pVoid)->Top = FIX32ToFloat(((pTW_FRAME)pItemList+j)->Top);
			((pINTERNALFRAMESTRUCT)pVoid)->Right = FIX32ToFloat(((pTW_FRAME)pItemList+j)->Right);
			((pINTERNALFRAMESTRUCT)pVoid)->Bottom = FIX32ToFloat(((pTW_FRAME)pItemList+j)->Bottom);
			break;

		case TWTY_STR32:
			*(pTW_STR32)pVoid = *((pTW_STR32)pItemList+j);
			break;

		case TWTY_STR64:
			*(pTW_STR64)pVoid = *((pTW_STR64)pItemList+j);
			break;

		case TWTY_STR128:
			*(pTW_STR128)pVoid = *((pTW_STR128)pItemList+j);
			break;

		case TWTY_STR255:
			*(pTW_STR255)pVoid = *((pTW_STR255)pItemList+j);
			break;

		default:
			break;
	}

	GlobalUnlock(pData->hContainer);
	// free is by the App
	return;
} // ExtractArrayValue

/*
	* FUNCTION: BuildUpEnumerationType 
	* 
	* ARGS:    pData   pointer to a capability structure, details about container
	*          pE      ptr to struct that contains the other fields of ENUM struct
	*          *pList  ptr to array of elements to put into the ENUM array
	* 
	* RETURNS: pData->hContainer set to address of the container handle, ptr is 
	*          returned here
	* 
	* NOTES:   The routine dynamically allocates a chunk of memory large enough 
	* to contain all the struct pTW_ENUMERATION as well as store it's ItemList
	* array INTERNAL to the struct.  The array itself and it's elements must be
	* type cast to ItemType.  I do not know how to dynamically cast elements
	* of an array to ItemType so it is time for a big static switch.>>>
	* 
	* Protocol: Used by MSG_GET.. calls were Source allocates the container and the
	* APP uses and then frees the container.
*/ 

TW_UINT16 BuildUpEnumerationType (pTW_CAPABILITY pData, pTW_ENUMERATION pE, void *pList, TW_UINT16 msg)
{
	pTW_ENUMERATION pEnumeration;   // template for ENUM fields
	int j;                          // anyone with more than 32K array elements
                          // should crash.  Could type on NumItems.
	LPVOID pVoid;

	if (msg != MSG_SET)
	{
		// allocate a block large enough for struct and complete enumeration array
		if ((pData->hContainer =
		(TW_HANDLE)GlobalAlloc(GMEM_MOVEABLE,
		(sizeof(TW_ENUMERATION)-sizeof(TW_UINT8))+
		pE->NumItems*AltTWItemSize(pE->ItemType))) == NULL)
		{
			return TWCC_LOWMEMORY;
		}
	}

	// fill in contype
	pData->ConType = TWON_ENUMERATION;
	if ((pEnumeration = (pTW_ENUMERATION)GlobalLock(pData->hContainer)) != NULL)
	{
		pEnumeration->ItemType     = pE->ItemType;        	// TWTY_XXXX
		pEnumeration->NumItems     = pE->NumItems;        	// TWPT_XXXX...
		pEnumeration->CurrentIndex = pE->CurrentIndex;	// current index setting
		pEnumeration->DefaultIndex = pE->DefaultIndex;  // default index setting

		// assign base address of ItemList array to 'generic' pointer
		// i.e. reposition the struct pointer to overlay the allocated block
		pVoid = (LPVOID)pEnumeration->ItemList;

		for (j=0; j<(int)pE->NumItems; j++)
		{
			// dynamic cast to ItemType of each array element
			switch (pE->ItemType)
			{
				case TWTY_INT8:
					*((pTW_INT8)pVoid+j) = *((pTW_INT8)pList+j);
					break;

				case TWTY_INT16:
					*((pTW_INT16)pVoid+j) = *((pTW_INT16)pList+j);
					break;

				case TWTY_INT32:
					*((pTW_INT32)pVoid+j) = *((pTW_INT32)pList+j);
					break;

				case TWTY_UINT8:
					*((pTW_UINT8)pVoid+j) = *((pTW_UINT8)pList+j);
					break;

				case TWTY_UINT16:
					*((pTW_UINT16)pVoid+j) = *((pTW_UINT16)pList+j);
					break;

				case TWTY_UINT32:
					*((pTW_UINT32)pVoid+j) = *((pTW_UINT32)pList+j);
					break;

				case TWTY_BOOL:
					*((pTW_BOOL)pVoid+j) = *((pTW_BOOL)pList+j);
					break;

				case TWTY_FIX32:
					*((pTW_FIX32)pVoid+j) = FloatToFIX32(*((float *)pList+j));
					break;

				case TWTY_FRAME:
					((pTW_FRAME)pVoid+j)->Left = FloatToFIX32(((pINTERNALFRAMESTRUCT)pList+j)->Left);
					((pTW_FRAME)pVoid+j)->Top = FloatToFIX32(((pINTERNALFRAMESTRUCT)pList+j)->Top);
					((pTW_FRAME)pVoid+j)->Right = FloatToFIX32(((pINTERNALFRAMESTRUCT)pList+j)->Right);
					((pTW_FRAME)pVoid+j)->Bottom = FloatToFIX32(((pINTERNALFRAMESTRUCT)pList+j)->Bottom);
					break;

				case TWTY_STR32:
					strncpy(((pTW_STR32)pVoid+(j*sizeof(TW_STR32))), ((pTW_STR32)pList+(j*sizeof(TW_STR32))), 32);
					break;

				case TWTY_STR64:
					strncpy(((pTW_STR64)pVoid+(j*sizeof(TW_STR64))), ((pTW_STR64)pList+(j*sizeof(TW_STR64))), 64);
					break;

				case TWTY_STR128:
					strncpy(((pTW_STR128)pVoid+(j*sizeof(TW_STR128))), ((pTW_STR128)pList+(j*sizeof(TW_STR128))), 128);
					break;

				case TWTY_STR255:
					strncpy(((pTW_STR255)pVoid+(j*sizeof(TW_STR255))), ((pTW_STR255)pList+(j*sizeof(TW_STR255))), 255);
					break;

				default:
					// bad ItemType
					break;
			} // end of switch
																														
		} // end of for loop

    GlobalUnlock(pData->hContainer);
 }	 //if closed


  // now you have a dynamically sized enumeration array WITHIN a struct
  return TWCC_SUCCESS;
} 


/*
	* FUNCTION: BuildUpArrayType
	* 
	* ARGS:    pData   pointer to a capability structure, details about container
	*          pA      ptr to struct that contains the other fields of ARRAY struct
	*          *pList  ptr to array of elements to put into the ARRAY struct
	* 
	* RETURNS: pData->hContainer set to address of the container handle, ptr is 
	*          returned here
	* 
	* NOTES: The routine dynamically allocates a chunk of memory large enough to
	* contain all the struct pTW_ARRAY as well as store it's ItemList array
	* INTERNAL to the struct.  The array itself and it's elements must be
	* type cast to ItemType.  I do not know how to dynamically cast elements
	* of an array to ItemType so it is time for a big static switch.>>>
	* 
	* Protocol: Used by MSG_GET.. calls were Source allocates the container and the 
	* APP uses and then frees the container.
*/ 

TW_UINT16 BuildUpArrayType (pTW_CAPABILITY pData, pTW_ARRAY pA, void *pList, TW_UINT16 msg)
{
	pTW_ARRAY pArray;
	int j;              // anyone with more than 32K array elements
                    // should crash.  Could type on NumItems.
	LPVOID pVoid;

	if (msg != MSG_SET)
	{
		// allocate a block large enough for struct and complete array
		if ((pData->hContainer = 
		(TW_HANDLE)GlobalAlloc(GMEM_MOVEABLE,
		(sizeof(TW_ARRAY)-sizeof(TW_UINT8))+
		pA->NumItems*AltTWItemSize(pA->ItemType))) == NULL)
		{
			return TWCC_LOWMEMORY;
		}
	}

	pData->ConType = TWON_ARRAY;  	
	if ((pArray = (pTW_ARRAY)GlobalLock(pData->hContainer)) != NULL)
	{
		pArray->ItemType = pA->ItemType;    // TWTY_XXXX
		pArray->NumItems = pA->NumItems;    // TWPT_XXXX...

		// assign base address of ItemList array to 'generic' pointer
		// i.e. reposition the struct pointer to overlay the allocated block
		pVoid = (LPVOID)pArray->ItemList;

		for (j=0; j<(int)pA->NumItems; j++)
		{
			// dynamic cast to ItemType of each array element
			switch (pA->ItemType)
			{
				case TWTY_INT8:
					*((pTW_INT8)pVoid+j) = *((pTW_INT8)pList+j);
					break;

				case TWTY_INT16:
					*((pTW_INT16)pVoid+j) = *((pTW_INT16)pList+j);
					break;

				case TWTY_INT32:
					*((pTW_INT32)pVoid+j) = *((pTW_INT32)pList+j);
					break;

				case TWTY_UINT8:
					*((pTW_UINT8)pVoid+j) = *((pTW_UINT8)pList+j);
					break;

				case TWTY_UINT16:
					*((pTW_UINT16)pVoid+j) = *((pTW_UINT16)pList+j);
					break;

				case TWTY_UINT32:
					*((pTW_UINT32)pVoid+j) = *((pTW_UINT32)pList+j);
					break;

				case TWTY_BOOL:
					*((pTW_BOOL)pVoid+j) = *((pTW_BOOL)pList+j);
					break;

				case TWTY_FIX32:
					*((pTW_FIX32)pVoid+j) = *((pTW_FIX32)pList+j);
					break;

				case TWTY_FRAME:
					((pTW_FRAME)pVoid+j)->Left = FloatToFIX32(((pINTERNALFRAMESTRUCT)pList+j)->Left);
					((pTW_FRAME)pVoid+j)->Top = FloatToFIX32(((pINTERNALFRAMESTRUCT)pList+j)->Top);
					((pTW_FRAME)pVoid+j)->Right = FloatToFIX32(((pINTERNALFRAMESTRUCT)pList+j)->Right);
					((pTW_FRAME)pVoid+j)->Bottom = FloatToFIX32(((pINTERNALFRAMESTRUCT)pList+j)->Bottom);
					break;

				
				case TWTY_STR32:
					strncpy(((pTW_STR32)pVoid+(j*sizeof(TW_STR32))), ((pTW_STR32)pList+(j*sizeof(TW_STR32))), 32);
					break;

				case TWTY_STR64:
					strncpy(((pTW_STR64)pVoid+(j*sizeof(TW_STR64))), ((pTW_STR64)pList+(j*sizeof(TW_STR64))), 64);
					break;

				case TWTY_STR128:
					strncpy(((pTW_STR128)pVoid+(j*sizeof(TW_STR128))), ((pTW_STR128)pList+(j*sizeof(TW_STR128))), 128);
					break;

				case TWTY_STR255:
					strncpy(((pTW_STR255)pVoid+(j*sizeof(TW_STR255))), ((pTW_STR255)pList+(j*sizeof(TW_STR255))), 255);
					break;

				default:
					// bad ItemType
					break;
			} // end of switch
		} // end of for loop

		GlobalUnlock(pData->hContainer);
	}

    // now you have a dynamically sized array WITHIN a struct
	return TWCC_SUCCESS;
}  // BuildUpArrayType



/*
	* BuildUpRangeType --
*/

TW_UINT16 BuildUpRangeType(pTW_CAPABILITY pData, pTW_RANGE pr, TW_UINT16 msg)
{
	pTW_RANGE pRange;

  if (msg != MSG_SET)	// MSG_SET allocates the structure at the app
	{
		if ((pData->hContainer = (TW_HANDLE)GlobalAlloc(GMEM_MOVEABLE,
	  sizeof(TW_RANGE))) == NULL)
		{
				return TWCC_LOWMEMORY;
		}
	}	 
	pData->ConType = TWON_RANGE;
	if ((pRange = (pTW_RANGE)GlobalLock(pData->hContainer))!=NULL)
	{
		pRange->ItemType = pr->ItemType;
		pRange->MinValue = pr->MinValue; 
		pRange->MaxValue = pr->MaxValue;
		pRange->StepSize = pr->StepSize;
		pRange->DefaultValue = pr->DefaultValue;
		pRange->CurrentValue = pr->CurrentValue;

		GlobalUnlock(pData->hContainer);	
	}
	return TWCC_SUCCESS;
}


/*
	 * FUNCTION: BuildOneValue 
	 *
	 * ARGS:    pData   pointer to a capability structure, details about container
	 *          ItemType constant that defines the type of the Item to follow
	 *          Item    the data to put into the OneValue container
	 *
	 * RETURNS: pData->hContainer set to address of the container handle, ptr is 
	 *          returned here
	 *
	 * NOTES:   This routine responds to a CAP_ call by creating a container of 
	 * type OneValue and returning with the hContainer value (excuse me) "pointing"
	 * to the container.  The container is filled with the values for ItemType
	 * and Item requested by the caller.
	 *
	 * NOTE: be sure to tell the APP the container type you have built, ConType.
	 *
	 * Protocol: Used by MSG_GET.. calls were Source allocates the container and the
	 * APP uses and then frees the container.
	 *
	 * For generalization value of ItemType does not affect Item in anyway.
	 * Caller should cast Item to TW_UINT32.
 */

TW_UINT16 BuildUpOneValue (pTW_CAPABILITY pData, TW_UINT16 ItemType, void *Item, TW_UINT16 msg)
{
	pTW_FRAME pFrame = NULL;
	pINTERNALFRAMESTRUCT pFrameStruct = NULL;

  pTW_ONEVALUE pOneValue = NULL;
	TW_FIX32 Fix32Value;

	ASSERT(pData);
	ASSERT(Item);

	if (msg != MSG_SET)	// MSG_SET allocates the structure at the app
	{
		if ((pData->hContainer = (TW_HANDLE)GlobalAlloc(GMEM_MOVEABLE, sizeof(TW_ONEVALUE)+AltTWItemSize(ItemType))) == NULL)
		{																				
			return(TWCC_LOWMEMORY);
		}																				
	}

	// tell APP the ConType returning
	pData->ConType = TWON_ONEVALUE;
	if ((pOneValue = (pTW_ONEVALUE)GlobalLock(pData->hContainer)) != NULL)
	{
		pOneValue->ItemType = ItemType; 
		switch(ItemType)
		{
			case TWTY_FRAME:
				/*
				*	Item will be one of our internal frame structures
				*/
				pFrameStruct = (pINTERNALFRAMESTRUCT)Item;
				pFrame = (pTW_FRAME)&pOneValue->Item;

				/*
				*	Convert the output to something TWAIN Recognizes
				*/
				pFrame->Left = FloatToFIX32(pFrameStruct->Left);
				pFrame->Top = FloatToFIX32(pFrameStruct->Top);
				pFrame->Right = FloatToFIX32(pFrameStruct->Right);
				pFrame->Bottom = FloatToFIX32(pFrameStruct->Bottom);

				break;				
			case TWTY_FIX32:
				Fix32Value = FloatToFIX32(*(float*)Item);
				pOneValue->Item = *(TW_UINT32*)&Fix32Value;	 //TW_UINT32 for ONEVALUE Item Value type
				break;	
			case TWTY_BOOL:
			  pOneValue->Item=*(TW_BOOL *)Item;
				break;
		 	case TWTY_STR32:
				memcpy((pTW_STR32)&pOneValue->Item, Item, sizeof(TW_STR32));
				break;
			case TWTY_UINT16:
			case TWTY_INT16:
				pOneValue->Item = *(TW_UINT16*)Item; 
				break;
		 	default:
				pOneValue->Item = *(TW_UINT32*)Item; 
				break; 
		}   
		GlobalUnlock(pData->hContainer);
	}
	return(TWCC_SUCCESS);
} // BuildUpOneValue



/* 
	 * FUNCTION: ExtractOneValue 
	 *
	 * ARGS:    pData   pointer to a capability structure, details about container
	 *          pVoid   ptr will be set to point to the item on return
	 *
	 * RETURNS: pVoid pts to extracted value.
	 *
	 * NOTES:   This routine will open a container and extract the Item.  The Item 
	 * will be returned to the caller in pVoid.  I will type cast the returned 
	 * value to that of ItemType.
	 *  
	 * Protocol: used by MSG_SET calls were Source empties then App frees.  It is 
	 * also assumed that the APP allocates and fills the container BEFORE this
	 * call.
 */   

VOID ExtractOneValue (pTW_CAPABILITY pData, LPVOID pVoid)
{
	pTW_ONEVALUE pOneValue = NULL;

	ASSERT(pData);
	ASSERT(pVoid);
	ASSERT(pData->ConType == TWON_ONEVALUE);

	if ((pOneValue = (pTW_ONEVALUE)GlobalLock(pData->hContainer)) != NULL)
	{
		// add a check for valid type
		// CAST to type of var caller wants
		switch (pOneValue->ItemType)
		{
			case TWTY_INT8:
				*(pTW_INT8)pVoid = (TW_INT8)pOneValue->Item;
				break;

			case TWTY_UINT8:
				*(pTW_UINT8)pVoid = (TW_UINT8)pOneValue->Item;
				break;

			case TWTY_INT16:
				*(pTW_INT16)pVoid = (TW_INT16)pOneValue->Item;
				break;

			case TWTY_UINT16:
				*(pTW_UINT16)pVoid = (TW_UINT16)pOneValue->Item;
				break;

			case TWTY_INT32:
				*(pTW_INT32)pVoid = (TW_INT32)pOneValue->Item;
				break;

			case TWTY_UINT32:
				*(pTW_UINT32)pVoid = (TW_UINT32)pOneValue->Item;
				break;

			case TWTY_BOOL:
				*(pTW_BOOL)pVoid = (TW_BOOL)pOneValue->Item;
				break;

			case TWTY_FIX32:
				*(float *)pVoid = FIX32ToFloat(*(pTW_FIX32)&pOneValue->Item);
				break;

			case TWTY_FRAME:
				((pINTERNALFRAMESTRUCT)pVoid)->Left = FIX32ToFloat(((pTW_FRAME)&pOneValue->Item)->Left);
				((pINTERNALFRAMESTRUCT)pVoid)->Top = FIX32ToFloat(((pTW_FRAME)&pOneValue->Item)->Top);
				((pINTERNALFRAMESTRUCT)pVoid)->Right = FIX32ToFloat(((pTW_FRAME)&pOneValue->Item)->Right);
				((pINTERNALFRAMESTRUCT)pVoid)->Bottom = FIX32ToFloat(((pTW_FRAME)&pOneValue->Item)->Bottom);
				break;
	
			case TWTY_STR32:
				strncpy((pTW_STR32)pVoid, (pTW_STR32)&pOneValue->Item, 32);
				break;

			case TWTY_STR64:
				strncpy((pTW_STR64)pVoid, (pTW_STR64)&pOneValue->Item, 64);
				break;

			case TWTY_STR128:
				strncpy((pTW_STR128)pVoid, (pTW_STR128)&pOneValue->Item, 128);
				break;

			case TWTY_STR255:
				strncpy((pTW_STR255)pVoid, (pTW_STR255)&pOneValue->Item, 255);
				break;
	
			default:
				break;
		}

		GlobalUnlock(pData->hContainer);
	}

  // it is assumed that the App will free the container upon return
  return;
} // ExtractOneValue



/*
	* FUNCTION: AltTWItemSize
	*
	* ARGS:    ItemType    constant which serves as an index to appropiate
	*                      data type
	* RETURNS: Result      size of item in bytes
	*
	* NOTES:   The routine provides a look-up table to get actual size in bytes of 
	* a particular data type.  Using the sizeof call should give the correct
	* results on any machine type.  The value for the ItemType parm are found
	* in the twain.h file and are indicated by TWTY_XXXX.. (TWTY_UINT16...)
	* 
	* This routine is provided to allow the Source to use this code intact.  The
	* Source does not link to the TWA_GLUE.C module and thus does not have access
	* to the TWItemSize array.
 */   

TW_UINT16 AltTWItemSize(TW_INT16 ItemType)
{
	TW_UINT16 result=sizeof(TW_UINT16);

	switch (ItemType)
	{
		case TWTY_INT8:
			result = sizeof(TW_INT8);
			break;

		case TWTY_UINT8:
			result = sizeof(TW_UINT8);
			break;

		case TWTY_INT16:
			result = sizeof(TW_INT16);
			break;

		case TWTY_UINT16:
			result = sizeof(TW_UINT16);
			break;

		case TWTY_INT32:
			result = sizeof(TW_INT32);
			break;

		case TWTY_UINT32:
			result = sizeof(TW_UINT32);
			break;
		case TWTY_FIX32:
			result = sizeof(TW_FIX32);
			break;
		case TWTY_FRAME:
			result = sizeof(TW_FRAME);
			break;
		case TWTY_STR32:
			result = sizeof(TW_STR32);
			break;
		case TWTY_STR64:
			result = sizeof(TW_STR32);
			break;
		case TWTY_STR128:
			result = sizeof(TW_STR32);
			break;
		case TWTY_STR255:
			result = sizeof(TW_STR32);
			break;
		default:
			break;
	}

	return(result);
}
