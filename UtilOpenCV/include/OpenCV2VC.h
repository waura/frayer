#ifndef _OPENCV2VC_H_
#define _OPENCV2VC_H_

#include "LibDllHeader.h"
#include "UtilOpenCVType.h"

//---------------------------------------------------------------------------
#define GetBMIHEAD(a) ((LPBITMAPINFOHEADER)(a + sizeof(BITMAPFILEHEADER)))
#define GetRGBQUAD(a) ((RGBQUAD*)(a + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)))
#define GetDIBDATA(a) (a + ((LPBITMAPFILEHEADER)a)->bfOffBits)

//---------------------------------------------------------------------------
//    ピクセルのCOLOR取得
//---------------------------------------------------------------------------
#define CL15TO24(a)    (((a&0x1F)*0xFF/0x1F)+((((a>>5)&0x1F)*0xFF/0x1F)<<8)+((((a>>10)&0x1F)*0xFF/0x1F)<<16))
#define CL16TO24(a)    (((a&0x1F)*0xFF/0x1F)+((((a>>5)&0x3F)*0xFF/0x3F)<<8)+((((a>>11)&0x1F)*0xFF/0x1F)<<16))

//---------------------------------------------------------------------------
//    ピクセルのCOLOR設定
//---------------------------------------------------------------------------
#define CL24TO15(a) (((a>>3)&0x1F)+((a>>6)&0x3E0)+((a>>9)&0x7C00))
#define CL24TO16(a)    (((a>>19)&0x1F)+((a>>5)&0x7E0)+((a<<8)&0xF800))


/////////////////////////////////////////////////////
/*!
	IplImageからDIBへ変換
	IplImageのメモリ領域をそのまま使用するので注意する。
	@param[in] image 変換するIplImage
	@param[out] lpbmi bitmapヘッダー 
	@param[out] lpBit bitmapデータ
*/
_EXPORT BOOL ConvertIPLImageToDIB(const IplImage* image, BITMAPINFO *lpbmi, VOID **lpBit);

//////////////////////////////////////////////////////
/*!
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
				 const IplImage* src_img, int x1, int y1);


_EXPORT inline void* GetLpBits(void* lpDib)
{
   BITMAPINFOHEADER* lpHeader = (BITMAPINFOHEADER*)lpDib ;

   // biClrUsedに数値が入っていれば、それがパレットの数
   DWORD pal =  lpHeader->biClrUsed ;

   // biClrUsedが0でもパレットがある場合がある。
   if(pal == 0 )
   {
      switch(lpHeader->biBitCount)
      {
      case 1:    pal=  2;    break;
      case 4:    pal= 16;    break;
      case 8:    pal=256;    break;
      default :    pal= 0;
      }
   }

   // BI_BITFIELDSの場合はビットフィールドを持っている
   DWORD fields = 0 ;
   if( lpHeader->biCompression == BI_BITFIELDS && 
    (lpHeader->biBitCount == 32 || lpHeader->biBitCount == 16 ) )
      fields = 3 * sizeof(DWORD) ;

   return  (BYTE*)lpDib + sizeof(BITMAPINFOHEADER)
        + pal * sizeof(RGBQUAD) + fields;
}

_EXPORT inline void GetDIBPixel(LPBITMAPINFOHEADER lpbmih, const BYTE *buff, const int X, const int Y, UCvPixel* dst)
{
	switch(lpbmih->biBitCount){
		//case 1:
		//	dst->val = 0xFFFFFF*(Line[(X)/8] & (1 << (7-(X)%8))!=0);
		//case 4:
		//	return ((Line[(X)/2] >> ((1-(X)%2)*4)) & 0x0F);
		//case 8:
		//	return Line[X];
		//case 15:
		//	return CL15TO24(*((WORD*)&Line[(X)*2]));
		//case 16:
		//	return CL16TO24(*((WORD*)&Line[(X)*2]));
		case 24:
			memcpy(&(dst->value),
				buff + 3*lpbmih->biWidth*Y + 3*X,
				sizeof(uint32_t));
			return;
		case 32:
			memcpy(&(dst->value),
				buff + 4*lpbmih->biWidth*Y + 4*X,
				sizeof(uint32_t));
			return;
	}
	return;
}

#endif //_OPENCV2VC_H_