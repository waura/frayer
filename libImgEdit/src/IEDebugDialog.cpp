#include "stdafx.h"

#include "IEDebugDialog.h"
#include "..\resource.h"

IEDebugDialog::IEDebugDialog()
{
}

IEDebugDialog::~IEDebugDialog()
{
}

INT_PTR IEDebugDialog::CreateDialogBox(HINSTANCE hInst, HWND hwndParent, IplImageExt* image)
{
	int ret = ::DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_IE_DEBUG_DIALOG), hwndParent, IEDebugDialog::DialogProc, (LPARAM)image);
	if(ret == -1){
		char str[256];
		wsprintf(str, "IEDebugDialog::CreateDialogBox() error code %d", GetLastError());
		OutputError::Alert(str);
	}

	return ret;
}

//////////////////////////////////
/*!
	ダイアログのプロシージャ
*/
BOOL CALLBACK IEDebugDialog::DialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static IplImageExt* image;

	switch(msg){
		case WM_INITDIALOG:
			{
				image = (IplImageExt*) lp;
			}
			return TRUE;
		case WM_CREATE:
			{
				CvSize size = image->GetSize();
				::SetWindowPos(hWnd, NULL, NULL, NULL, size.width, size.height, SWP_NOZORDER | SWP_NOMOVE); 
			}
			return TRUE;
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = ::BeginPaint(hWnd, &ps);
				//
				BITMAPINFO bmpi;
				LPVOID pBit;

				CvSize size = image->GetSize();
				RECT rc;
				rc.left = 0; rc.top =0;
				rc.right = size.width; rc.bottom = size.height;
				IplImage* i = cvCreateImage(size, IPL_DEPTH_8U, 1);
				UCvPixel color1 = ucvPixel(255,255,255,255);
				UCvPixel color2 = ucvPixel(100,100,100,255);
				CheckerImageRect(i, &color1, &color2, 25, &rc);
				
				int x,y;

				UCvPixel pixel1;
				UCvPixel pixel2;
				for(y=0; y<size.height; y++){
					for(x=0; x<size.width; x++){
						image->GetPixel(x, y, &pixel1);
						GetPixelFromBGR(i, x, y, &pixel2);
						pixel2.val[A_CHANNEL] = pixel1.val[A_CHANNEL] = 255;

						AlphaBlendPixel(&pixel2, &pixel1, &pixel1);
						SetPixelToBGR(i, x, y, &pixel1);
					}
				}

				//IplImageをDIBに
				ConvertIPLImageToDIB(i, &bmpi, &pBit);
				//
				::SetDIBitsToDevice(hdc, 0, 0, i->width, i->height,
					0, 0, 0, i->height, pBit, &bmpi, DIB_RGB_COLORS);

				::EndPaint(hWnd, &ps);
			}
			return TRUE;
		case WM_COMMAND:
			switch(LOWORD(wp)){
				case IDOK:
					EndDialog(hWnd, IDOK);
					return TRUE;
				case IDCANCEL:
					EndDialog(hWnd, IDCANCEL);
					return TRUE;
			}
	}
	return FALSE;
}

