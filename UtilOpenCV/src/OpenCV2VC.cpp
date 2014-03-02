#include "stdafx.h"

#include "OpenCV2VC.h"



/////////////////////////////////////////////////////
/*!
	IplImageからDIBへ変換
	IplImageのメモリ領域をそのまま使用するので注意する。
	@param[in] image 変換するIplImage
	@param[out] lpbmi bitmapヘッダー 
	@param[out] lpBit bitmapデータ
*/
_EXPORT BOOL ConvertIPLImageToDIB(const IplImage* image, BITMAPINFO *lpbmi, VOID **lpBit)
{
	if(!image) return FALSE;

	lpbmi->bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
	lpbmi->bmiHeader.biWidth  = image->width;
	lpbmi->bmiHeader.biHeight = -image->height;
	lpbmi->bmiHeader.biPlanes = 1;
	lpbmi->bmiHeader.biBitCount			= 8*image->nChannels;		//1ピクセルあたりのビット数
	lpbmi->bmiHeader.biCompression		= BI_RGB;					//圧縮形式
	lpbmi->bmiHeader.biSizeImage		= 0;
	lpbmi->bmiHeader.biXPelsPerMeter	= 1;
	lpbmi->bmiHeader.biYPelsPerMeter	= 1;
	lpbmi->bmiHeader.biClrUsed			= 0;
	lpbmi->bmiHeader.biClrImportant		= 0;

	(*lpBit) = image->imageData;
	return TRUE;
}

//////////////////////////////////////////////////////
/*
	デバイスコンテキストにIplImageを書き込み
	@param[in] hdc 書き込み先デバイスコンテキスト
	@param[in] x 書き込み開始位置
	@param[in] y 書き込み開始位置
	@param[in] cx 書き込み幅
	@param[in] cy 書き込み高さ
	@param[in] src_img 書き込む画像データ
	@param[in] x1 画像データ書き込み開始位置
	@param[in] y1 画像データ書き込み開始位置
*/
_EXPORT VOID IPLImageToDevice(HDC hdc, int x, int y, int cx, int cy,
				 const IplImage* src_img, int x1, int y1)
{
	BITMAPINFO bmpi;
	LPVOID pBit;

	ConvertIPLImageToDIB(src_img, &bmpi, &pBit);
	::SetDIBitsToDevice(hdc, x, y, cx, cy, x1, y1, 0, src_img->height, pBit, &bmpi, DIB_RGB_COLORS);
}