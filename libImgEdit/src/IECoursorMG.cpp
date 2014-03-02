#include "stdafx.h"

#include "IECoursorMG.h"
#include "OutputError.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

/*
bool IECoursorMG::SetBrushCoursor(HWND hWnd, const IEBrush_Ptr pBrush, double scale)
{
	int x,y;
	HBITMAP hBitmap;
	HBITMAP hMonoBitmap;
	HCURSOR hAlphaCursor = NULL;
	CvSize size = pBrush->GetBrushSize();

	IplImage *mask = cvCreateImage(size, IPL_DEPTH_8U, 3);
	IplImage *brush_cur = cvCreateImage(size, IPL_DEPTH_8U, 3);

	//ブラシデータからカーソルイメージ作成
	bool checkEdge;
	bool pre_;
	//X軸方向走査
	checkEdge = false;
	UCvPixel zero_px = ucvPixel(0,0,0);
	UCvPixel fill_px = ucvPixel(0xFF, 0xFF, 0xFF); 
	for(y=0; y<size.height; y++){
		pre_ = false;
		for(x=0; x<size.width; x++){
			int dat = pBrush->GetBrushDataPos(x, y);
			if(dat == 0){
				checkEdge = false;
			}
			else{
				checkEdge = true;
			}

			if(pre_ != checkEdge || (x == size.width-1 && checkEdge)){
				SetPixelToBGR(brush_cur, x, y, &zero_px);
				SetPixelToBGR(mask, x, y, &zero_px);
			}
			else{
				SetPixelToBGR(brush_cur, x, y, &zero_px);
				SetPixelToBGR(mask, x, y, &fill_px);
			}

			pre_ = checkEdge;
		}
	}
	//Y軸方向走査
	checkEdge = false;
	for(x=0; x<size.width; x++){
		pre_ = false;
		for(y=0; y<size.height; y++){
			int dat = pBrush->GetBrushDataPos(x, y);
			if(dat == 0){
				checkEdge = false;
			}
			else{
				checkEdge = true;
			}

			if(pre_ != checkEdge || (y == size.height-1 && checkEdge)){
				SetPixelToBGR(brush_cur, x, y, &zero_px);
				SetPixelToBGR(mask, x, y, &zero_px);
			}

			pre_ = checkEdge;
		}
	}

	HDC hdc;
	hdc = GetDC(NULL);

	//HBITMAPへ変換
	BITMAPINFO maskBmi;
	BITMAPINFO brush_curBmi;
	void *pMaskBit;
	void *pBrush_curBit;
	ConvertIPLImageToDIB(mask, &maskBmi, &pMaskBit);
	ConvertIPLImageToDIB(brush_cur, &brush_curBmi, &pBrush_curBit);
	hMonoBitmap = CreateDIBitmap(hdc, &(maskBmi.bmiHeader), CBM_INIT, pMaskBit, &maskBmi, DIB_RGB_COLORS);
	hBitmap = CreateDIBitmap(hdc, &(brush_curBmi.bmiHeader), CBM_INIT, pBrush_curBit, &brush_curBmi, DIB_RGB_COLORS);

	::ReleaseDC(NULL, hdc);

	ICONINFO ii;
	ii.fIcon = FALSE; //カーソル
	ii.xHotspot = size.width/2;
	ii.yHotspot = size.height/2;
	ii.hbmMask = hMonoBitmap;
	ii.hbmColor = hBitmap;

	hAlphaCursor = CreateIconIndirect(&ii);

	HCURSOR hOldCursor = (HCURSOR)SetClassLongPtr(hWnd, GCLP_HCURSOR, (LONG)hAlphaCursor);
	if(hOldCursor == NULL){
		OutputError::Alert("IECoursorMG::SetBrushCoursor() カーソル設定エラー");
		return false;
	}
	DestroyCursor(hOldCursor);

	DeleteObject(hBitmap);
	DeleteObject(hMonoBitmap);

	return true;
}
*/