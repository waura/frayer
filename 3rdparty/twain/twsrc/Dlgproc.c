/**File:         AppDllUI
* Description:   To Enable Sample data Source user to select suitable parameters
*                for his purpose of its output and as expected by the application.  
* Owner:         JFL Peripheral Solutions.
* Author:        Berhane Russom
* Created:       May  1998
* Language:      windows C
* Package:       N/A
* Status:        Test tool
*
*	Copyright © 1998 TWAIN Working Group: Adobe Systems Incorporated, 
*	Canon Information Systems, Eastman Kodak Company, 
*	Fujitsu Computer Products of America, Genoa Technology, 
*	Hewlett-Packard Company, Intel Corporation, Kofax Image Products, 
*	JFL Peripheral Solutions Inc., Ricoh Corporation, and Xerox Corporation.  
*	All rights reserved.
*/
#include<windows.h>
#include<WindowsX.h>
#include<commctrl.h>
#include <stdio.h>
#include "resource.h"
#include "Dlgproc.h"

#include "..\twcommon\twain.h"
#include "twd_type.h"
#include "dscaps.h"

#include "scanner.h"


void ResizePaper(HWND hDlg);

const char *StringFromPixelTypeBitDepth(TW_UINT16 PixelType, TW_UINT16 BitDepth)
{
	switch(PixelType)
	{
	case TWPT_BW:
		return "1 bit B&W";
		break;
	case TWPT_GRAY:
		if(BitDepth==4)
		{
			return "4 bit Gray";
		}
		else
		{
			return "8 bit Gray";
		}
		break;
	case TWPT_RGB:
		return "24 bit Color";
		break;
	case TWPT_PALETTE:
		break;
	case TWPT_CMY:
		break;
	case TWPT_CMYK:
		break;
	case TWPT_YUV:
		break;
	case TWPT_YUVK:
		break;
	case TWPT_CIEXYZ:
		break;
	}
	return NULL;
}

const char *StringFromPaperSize(TW_UINT16 PaperSize)
{
	switch(PaperSize)
	{
		case TWSS_NONE:
			return "Custom";
			break;
		case TWSS_A4LETTER:
			return "A4 Letter";
			break;
		case TWSS_USLETTER:
			return "US Letter";
			break;
		case TWSS_USLEGAL:
			return "US Legal";
			break;
	}
	return NULL;
}

/* Function: SettingsDlgProc 
* Author: Berhane Russom
* Input:
*		hDlg        -Handle to Dailog.
*		iMsg        -Message Identifier.
*	  (l|w)Param	_message parameters
* Output:
*	              _If the function succeeds, the return value is the nResult parameter in the call to the 
*               _EndDialog function used to terminate the dialog box.
*               _If the function fails, the return value is -1.
* Comments:	 		_processes messages sent to a window.
*		
*/
#ifdef WIN32
LRESULT CALLBACK SettingsDlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
#else	//16 bit version
BOOL FAR PASCAL _export SettingsDlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
#endif
{	
	int iIndex = 0;
	switch(iMsg)
	{
		case WM_INITDIALOG:
			{
				int iSubIndex = 0;
				int iNumItems = 0;
				int iNumSubItems = 0;
				float *pFloatTypes = NULL;
				TW_UINT16 *pUintTypes = NULL;
				TW_UINT16 *pUintSubTypes = NULL;
				char TmpString[MAX_PATH];
				HWND dlgItemWnd = NULL;
				short sMaxXfer = 0;

				memset(TmpString, 0, MAX_PATH);

				/*
				*	Initialize the current feeder state
				*/	
				SendDlgItemMessage(hDlg, ID_ADFCHECKBOX, BM_SETCHECK, GetCurrentFeederEnabled()?BST_CHECKED:BST_UNCHECKED, 0);

				/*
				*	Initialize the current feeder state
				*/	
				SendDlgItemMessage(hDlg, ID_BTN_ADFLOADED, BM_SETCHECK, IsDocumentInFeeder()?BST_CHECKED:BST_UNCHECKED, 0);

				/*
				* Spin button for our custom feature of simulating certain
				* number of images transferred
				* One image minimum transfer
				* Max is limited only by XferCount
				*/
				if(GetXferCount()==-1)
				{
					sMaxXfer = 25;
				}
				else
				{
					sMaxXfer = min(25, GetXferCount());
				}

				SendDlgItemMessage(hDlg, IDC_SPIN_IMAGECOUNT, UDM_SETRANGE, (WPARAM)0, MAKELONG((short)sMaxXfer, (short)1));
				SendDlgItemMessage(hDlg, IDC_SPIN_IMAGECOUNT, UDM_SETPOS, (WPARAM)0, MAKELONG((short)GetNumPages(), (short)0));

				/*
				*	Initialize resolution values
				*/
				pFloatTypes = GetSupportedXResolution(&iNumItems);
				for(iIndex=0; iIndex<iNumItems; iIndex++)
				{
					int iInsertIndex = 0;
					sprintf(TmpString, "%.3f", pFloatTypes[iIndex]);
					iInsertIndex = ComboBox_AddString(GetDlgItem(hDlg, ID_COMBO_RESOLUTION), TmpString);
					ComboBox_SetItemData(GetDlgItem(hDlg, ID_COMBO_RESOLUTION), iInsertIndex, (DWORD)pFloatTypes[iIndex]);

					if(pFloatTypes[iIndex] == GetCurrentXResolution())
					{
						/*
						* Make this the current resolution
						*/
						ComboBox_SelectString(GetDlgItem(hDlg, ID_COMBO_RESOLUTION), 0, TmpString);
					}
				}

				/*
				*	Initialize scaling values
				*/
				pFloatTypes = GetSupportedXScaling(&iNumItems);
				for(iIndex=0; iIndex<iNumItems; iIndex++)
				{
					int iInsertIndex = 0;
					sprintf(TmpString, "%.3f%%", pFloatTypes[iIndex]);
					iInsertIndex = ComboBox_AddString(GetDlgItem(hDlg, ID_COMBO_SCALE), TmpString);
					ComboBox_SetItemData(GetDlgItem(hDlg, ID_COMBO_SCALE), iInsertIndex, (DWORD)pFloatTypes[iIndex]);

					if(pFloatTypes[iIndex] == GetCurrentXScaling())
					{
						/*
						* Make this the current scaling factor
						*/
						ComboBox_SelectString(GetDlgItem(hDlg, ID_COMBO_SCALE), 0, TmpString);
					}
				}

				/*
				*	Initialize the paper sizes
				*/
				pUintTypes = GetSupportedSupportedSizes(&iNumItems);
				for(iIndex=0; iIndex<iNumItems; iIndex++)
				{
					int iInsertIndex = 0;
					iInsertIndex = ComboBox_AddString(GetDlgItem(hDlg, ID_COMBO_PAPERSIZE), StringFromPaperSize(pUintTypes[iIndex]));
					ComboBox_SetItemData(GetDlgItem(hDlg, ID_COMBO_PAPERSIZE), iInsertIndex, (DWORD)pUintTypes[iIndex]);

					if(pUintTypes[iIndex] == GetCurrentSupportedSizes())
					{
						/*
						* Make this the current paper size
						*/
						ComboBox_SelectString(GetDlgItem(hDlg, ID_COMBO_PAPERSIZE), 0, StringFromPaperSize(pUintTypes[iIndex]));
					}
				}

				ResizePaper(hDlg);

				/*
				*	Initialize the scan mode
				*/
				pUintTypes = GetSupportedPixelType(&iNumItems);
				for(iIndex=0; iIndex<iNumItems; iIndex++)
				{
					pUintSubTypes = GetSupportedBitDepth(&iNumSubItems, pUintTypes[iIndex]);
					for(iSubIndex = 0; iSubIndex < iNumSubItems; iSubIndex++)
					{
						int iInsertIndex = 0;
						iInsertIndex = ComboBox_AddString(GetDlgItem(hDlg, ID_COMBO_SCANMODE), StringFromPixelTypeBitDepth(pUintTypes[iIndex], pUintSubTypes[iSubIndex]));
						ComboBox_SetItemData(GetDlgItem(hDlg, ID_COMBO_SCANMODE), iInsertIndex, MAKELONG(pUintTypes[iIndex], pUintSubTypes[iSubIndex]));

						if( (pUintTypes[iIndex] == GetCurrentPixelType())&&(pUintSubTypes[iSubIndex]==GetCurrentBitDepth(pUintTypes[iIndex])) )
						{
							/*
							* Make this the current paper size
							*/
							ComboBox_SelectString(GetDlgItem(hDlg, ID_COMBO_SCANMODE), 0, StringFromPixelTypeBitDepth(pUintTypes[iIndex], pUintSubTypes[iSubIndex]));
						}
					}
				}
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{																
				case ID_COMBO_PAPERSIZE:
					if(HIWORD(wParam)==CBN_SELCHANGE)
					{
						ResizePaper(hDlg);
					}
					break;

				case ID_BTN_ADFLOADED:
					if(SendDlgItemMessage(hDlg, ID_BTN_ADFLOADED, BM_GETCHECK, 0, 0)==BST_CHECKED)
					{
						/*
						*	Chose to simulate loading of the document feeder
						*/
						LoadFeeder();
					}
					else
					{
						/*
						*	Chose to simulate the clearning of the document feeder
						*/
						ClearFeeder();
					}
					break;
			  case IDOK:									 	
					/*
					*	Update all our current settings first
					*/
					SetCurrentFeederEnabled((TW_BOOL)((SendDlgItemMessage(hDlg, ID_ADFCHECKBOX, BM_GETCHECK, 0, 0)==BST_CHECKED)?TRUE:FALSE));

					if(GetCurrentFeederEnabled())
					{
						/*
						*	Should not proceed with any document feeder scan if the
						* response to ICAP_FEEDERLOADED is going to be false
						*/
						if(!IsDocumentInFeeder())
						{
							MessageBox(hDlg, "Please load document feeder.\n", "Feeder Problem", MB_OK|MB_ICONEXCLAMATION);
							break;
						}

						/*
						*	Set the specified number of pages to scan
						*/
						SetNumPages(LOWORD(SendDlgItemMessage(hDlg, IDC_SPIN_IMAGECOUNT, UDM_GETPOS, (WPARAM)0, (LPARAM)0)));
						LoadFeeder();
					}
					else
					{
						/*
						*	Simulating a flat bed, there will be only one image to scan
						*/
						SetNumPages(1);
					}

					iIndex = ComboBox_GetCurSel(GetDlgItem(hDlg, ID_COMBO_RESOLUTION));
					SetCurrentXResolution((float)ComboBox_GetItemData(GetDlgItem(hDlg, ID_COMBO_RESOLUTION), iIndex));

					iIndex = ComboBox_GetCurSel(GetDlgItem(hDlg, ID_COMBO_SCALE));
					SetCurrentXScaling((float)ComboBox_GetItemData(GetDlgItem(hDlg, ID_COMBO_SCALE), iIndex));

					iIndex = ComboBox_GetCurSel(GetDlgItem(hDlg, ID_COMBO_PAPERSIZE));
					SetCurrentSupportedSizes((TW_UINT16)ComboBox_GetItemData(GetDlgItem(hDlg, ID_COMBO_PAPERSIZE), iIndex));

					iIndex = ComboBox_GetCurSel(GetDlgItem(hDlg, ID_COMBO_SCANMODE));
					SetCurrentPixelType((TW_UINT16)LOWORD(ComboBox_GetItemData(GetDlgItem(hDlg, ID_COMBO_SCANMODE), iIndex)));
					SetCurrentBitDepth((TW_UINT16)HIWORD(ComboBox_GetItemData(GetDlgItem(hDlg, ID_COMBO_SCANMODE), iIndex)), GetCurrentPixelType());

					/*
					*	For now, just fall through
					* NotifyXferReady comes later
					*/
					NotifyXferReady();
					break;
				case IDCANCEL:
					NotifyCloseDSReq();
					break;
				default:
					break;
			 }
			 break;

		default:
			break;
	}	  //switch
  return FALSE;
}

void ResizePaper(HWND hDlg)
{
	/*
	*	Resize the paper static control
	*/
	float fWidth = 8.5F, fHeight = 11.0F, fRatio = 0.0F;
	int iIndex = ComboBox_GetCurSel(GetDlgItem(hDlg, ID_COMBO_PAPERSIZE));
	RECT rcWndRect;
	int iWidth = 0, iHeight = 0;
	POINT ptTopLeft, ptBottomRight;

	/*
	*	Figure out where the window is now
	*/
	GetWindowRect(GetDlgItem(hDlg, IDC_STC_PAPER), &rcWndRect);
	ptTopLeft.x = rcWndRect.left;
	ptTopLeft.y = rcWndRect.top;

	ptBottomRight.x = rcWndRect.right;
	ptBottomRight.y = rcWndRect.bottom;

	ScreenToClient(hDlg, &ptTopLeft);
	ScreenToClient(hDlg, &ptBottomRight);

	/*
	* Width does not change
	*/
	iWidth = ptBottomRight.x - ptTopLeft.x;
	iHeight = ptBottomRight.y - ptTopLeft.y;

	if(ComboBox_GetItemData(GetDlgItem(hDlg, ID_COMBO_PAPERSIZE), iIndex)==TWSS_USLEGAL)
	{
		/*
		*	Resize our image to 8.5x14
		*/
		fHeight = 14.0F;
	}
	
	iHeight = (int)((fHeight * iWidth) / fWidth);

	/*
	*	Move the window with the new dimensions
	*/
	MoveWindow(GetDlgItem(hDlg, IDC_STC_PAPER),
		ptTopLeft.x, ptTopLeft.y, iWidth, iHeight, TRUE);

	return;
}
