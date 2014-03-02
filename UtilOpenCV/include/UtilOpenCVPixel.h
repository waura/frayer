#ifndef _UTILOPENCVPIXEL_H_
#define _UTILOPENCVPIXEL_H_

#include "LibDllHeader.h"
#include "UtilOpenCVType.h"

_EXPORT inline void NotPixel(const UCvPixel* src, UCvPixel* dst)
{
	dst->value = ~src->value;

	/*
	dst->b = ~src->b;
	dst->g = ~src->g;
	dst->r = ~src->r;
	dst->a = ~src->a;
	*/
}

////////////////////////////////////////
/*!
	アルファブレンド
	@param[out] out 出力先ピクセル位置
	@param[in] over 上になるピクセル位置
	@param[in] under 下になるピクセル位置
*/
//_EXPORT inline void AlphaBlendBGRA(uchar* out, uchar* over, uchar* under)
//{
//	//b = out[0]; g = out[1]; r = out[2]; a = out[3];
//
//	if(over[3] == 255){
//		out[0] = over[0];
//		out[1] = over[1];
//		out[2] = over[2];
//		out[3] = 255;
//	}
//	else if(over[3] == 0){
//		//out[0] = under[0];
//		//out[1] = under[1];
//		//out[2] = under[2];
//		//out[3] = under[3];
//		memcpy(out, under, sizeof(uchar)*4);
//	}
//	else if(under[3] == 255){
//		uchar ra = 255 - over[3];
//		out[0] = (over[0]*out[3] + under[0]*ra)>>8;
//		out[1] = (over[1]*out[3] + under[1]*ra)>>8;
//		out[2] = (over[2]*out[3] + under[2]*ra)>>8;
//		out[3] = 255;
//	}
//	else{
//		uchar ra = 255 - over[3];
//		uchar w_alpha = 255 - (ra*(255 - under[3])>>8);
//		out[0] = (w_alpha != 0) ? (over[0]*over[3] + ((under[0]*under[3]*ra)>>8))/w_alpha : 0;
//		out[1] = (w_alpha != 0) ? (over[1]*over[3] + ((under[1]*under[3]*ra)>>8))/w_alpha : 0;
//		out[2] = (w_alpha != 0) ? (over[2]*over[3] + ((under[2]*under[3]*ra)>>8))/w_alpha : 0;
//		out[3] = w_alpha;
//	}
//}

////////////////////////////////////////
/*!
	アルファブレンド
	@param[in] under 下になるピクセル位置
	@param[in] under_alpha
	@param[in] over 上になるピクセル位置
	@param[in] over_alpha
*/
_EXPORT inline void AlphaBlendPixel(
	 const UCvPixel* under,
	 const UCvPixel* over,
	 const uint8_t mask,
	 UCvPixel* dst)
{
	if(mask == 255){
		if(over->a == 255){
			if(dst != over)
				dst->value = over->value;
		}
		else if(under->a == 0){
			if(dst != over)
				dst->value = over->value;
		}
		else if(under->a == 255){
			uint8_t ra = ~over->a;
			dst->b = (over->b*over->a + under->b*ra)>>8;
			dst->g = (over->g*over->a +	under->g*ra)>>8;
			dst->r = (over->r*over->a +	under->r*ra)>>8;
			dst->a = 255;
		}
		else{
			uint8_t ra = ~over->a;
			uint8_t alpha = 255 - (ra*(255 - under->a)>>8);
			dst->b = (over->b*over->a + ((under->b*under->a*ra)>>8))/alpha;
			dst->g = (over->g*over->a + ((under->g*under->a*ra)>>8))/alpha;
			dst->r = (over->r*over->a + ((under->r*under->a*ra)>>8))/alpha;
			dst->a = alpha;
		}
	}
	else if(mask != 0){
		uint8_t over_alpha = (over->a * mask) >> 8;
		if(over_alpha == 255){
			if(dst != over){
				dst->value = over->value;
			}
		}
		else if(under->a == 0){
			if(dst != over){
				dst->b = over->b;
				dst->g = over->g;
				dst->r = over->r;
				dst->a = over_alpha;
			}
		}
		else if(under->a == 255){
			uint8_t ra = ~over_alpha;
			dst->b = (over->b*over_alpha + under->b*ra)>>8;
			dst->g = (over->g*over_alpha + under->g*ra)>>8;
			dst->r = (over->r*over_alpha + under->r*ra)>>8;
			dst->a = 255;
		}
		else{
			uint8_t ra = ~over_alpha;
			uint8_t alpha = 255 - (ra*(255 - under->a)>>8);
			dst->b = (over->b*over_alpha + ((under->b*under->a*ra)>>8))/alpha;
			dst->g = (over->g*over_alpha + ((under->g*under->a*ra)>>8))/alpha;
			dst->r = (over->r*over_alpha + ((under->r*under->a*ra)>>8))/alpha;
			dst->a = alpha;
		}
	}
}

////////////////////////////////////////
/*!
	位置(x,y)に値をセットする
	@param[in] image マスク
	@param[in] x セットするX座標
	@param[in] y セットするY座標
	@param[in] dpeth セットする画素深度位置
	@param[in] data セットする値
*/
_EXPORT inline void SetDataPos(IplImage* image, int x, int y, int depth, uint8_t data)
{
	assert(image);
	assert(depth < image->nChannels);
	assert((0 <= x && x <= image->width-1) &&
		(0 <= y && y <= image->height-1));

	*(image->imageData + y*image->widthStep + x*image->nChannels + depth) = data;
}

/////////////////////////////////////////
/*!
	位置(x,y)の値を返す
	@param[in] image マスク
	@param[in] x X座標
	@param[in] y Y座標
	@param[in] depth セットする画素深度位置
	@return 位置(x,y)の値
*/
_EXPORT inline uint8_t GetDataPos(const IplImage* image, int x, int y, int depth)
{
	assert(image != NULL);
	assert(depth < image->nChannels);
	assert((0 <= x && x <= image->width-1) &&
		(0 <= y && y <= image->height-1));

	return *(image->imageData + y*image->widthStep + x*image->nChannels + depth);
}

////////////////////////////////////////
/*!
	位置(x,y)に値をセットする
	@param[in] image マスク
	@param[in] x セットするX座標
	@param[in] y セットするY座標
	@param[in] depth セットする画素深度位置
	@param[in] data セットする値
*/
_EXPORT inline void SetMaskDataPos(IplImage* image, int x, int y, uint8_t data)
{
	assert(image);
	assert(image->nChannels == 1);
	assert((0 <= x && x <= image->width-1) &&
		(0 <= y && y <= image->height-1));

	*(image->imageData + y*image->widthStep + x) = data;
}

/////////////////////////////////////////
/*!
	マスクの位置(x,y)の値を返す
	@param[in] image マスク
	@param[in] x X座標
	@param[in] y Y座標
	@return マスクの位置(x,y)の値
*/
_EXPORT inline uint8_t GetMaskDataPos(const IplImage* image, int x, int y)
{
	assert(image);
	assert(image->nChannels == 1);
	assert((0 <= x && x <= image->width-1) &&
		(0 <= y && y <= image->height-1));

	return *(image->imageData + y*image->widthStep + x);
}

/////////////////////////////////////////////
/*!
	指定位置のRGB値を取得する
	@param[in] image 画像
	@param[in] x X座標
	@param[in] y Y座標
	@param[out] dst 取得した値
*/
_EXPORT inline void GetPixelFromBGR(const IplImage *image, int x, int y, UCvPixel *dst)
{
	assert(image);
	assert(image->nChannels == 3);
	assert((0 <= x && x <= image->width-1) &&
		(0 <= y && y <= image->height-1));

	memcpy(&(dst->value), image->imageData + y*image->widthStep + x*image->nChannels, sizeof(uint8_t)*3);

	//*r = (uint8_t) image->imageData[y *image->widthStep+ x * image->nChannels + 2];
	//*g = (uint8_t) image->imageData[y *image->widthStep+ x * image->nChannels + 1];
	//*b = (uint8_t) image->imageData[y *image->widthStep+ x * image->nChannels];
}

_EXPORT inline uint8_t* GetPixelAddress(const IplImage* image, int x, int y)
{
	assert(image);
	assert(image->depth == IPL_DEPTH_8U);
	assert((0 <= x && x <= image->width-1) &&
		(0 <= y && y <= image->height-1));

	return (uint8_t*)(image->imageData + y*image->widthStep + x*image->nChannels);
}

_EXPORT inline uint8_t* GetNextLineAddress(const IplImage* image, uint8_t* addr)
{
	return addr + image->widthStep;
}

_EXPORT inline UCvBGR24* GetPixelAddress24(const IplImage* image, int x, int y)
{
	assert(image);
	assert(image->depth == IPL_DEPTH_8U);
	assert((0 <= x && x <= image->width-1) &&
		(0 <= y && y <= image->height-1));

	return (UCvBGR24*)(image->imageData + y*image->widthStep + x*image->nChannels);
}

_EXPORT inline UCvBGR24* GetNextLineAddress24(const IplImage* image, UCvBGR24* addr)
{
	return (UCvBGR24*)((uint8_t*)addr + image->widthStep);
}

_EXPORT inline UCvPixel32* GetPixelAddress32(const IplImage* image, int x, int y)
{
	assert(image);
	assert(image->depth == IPL_DEPTH_8U);
	assert((0 <= x && x <= image->width-1) &&
		(0 <= y && y <= image->height-1));

	return (UCvPixel32*)(image->imageData + y*image->widthStep + x*image->nChannels);
}

_EXPORT inline UCvPixel32* GetNextLineAddress32(const IplImage* image, UCvPixel32* addr)
{
	return (UCvPixel32*)((uint8_t*)addr + image->widthStep);
}

_EXPORT inline UCvBGR24* GetBGRAddress(const IplImage* image, int x, int y)
{
	assert(image);
	assert(image->depth == IPL_DEPTH_8U);
	assert((0 <= x && x <= image->width-1) &&
		(0 <= y && y <= image->height-1));

	return (UCvBGR24*)(image->imageData + y*image->widthStep + x*image->nChannels);
} 

//////////////////////////////////////////////
/*!
	指定位置のRGB値を取得する (4チャンネルBGRA画像用)
	@param[in] image 画像
	@param[in] x X座標
	@param[in] y Y座標
	@param[out] dst 取得した値
*/
_EXPORT inline void GetPixelFromBGRA(
	const IplImage *image,
	int x,
	int y,
	UCvPixel* dst)
{
	assert(image);
	assert(image->nChannels == 4);
	assert((0 <= x && x <= image->width-1) &&
		(0 <= y && y <= image->height-1));

	//dst->value = *(image->imageData + y*image->widthStep + x*image->nChannels);

	memcpy(&(dst->value), image->imageData + y*image->widthStep + x*image->nChannels, sizeof(uint32_t));

	//*a = (uint8_t) image->imageData[y *image->widthStep +x *image->nChannels + 3];
	//*r = (uint8_t) image->imageData[y *image->widthStep +x *image->nChannels + 2];
	//*g = (uint8_t) image->imageData[y *image->widthStep +x *image->nChannels + 1];
	//*b = (uint8_t) image->imageData[y *image->widthStep +x *image->nChannels];
}

//////////////////////////////////////////////
/*!
	指定位置のRGB値を取得する (4チャンネルBGRA画像用)
	@param[in] image 画像
	@param[in] x X座標
	@param[in] y Y座標
	@param[out] a 取得したアルファ値
*/
_EXPORT inline void GetAlphaFromBGRA(
	const IplImage *image,
	int x,
	int y,
	uint8_t *a)
{
	assert(image);
	assert(image->nChannels == 4);
	assert((0 <= x && x <= image->width-1) &&
		(0 <= y && y <= image->height-1));

	*a = *(image->imageData + y*image->widthStep + x*image->nChannels + 3);
}

//////////////////////////////////////////////
/*!
	指定位置にRGB値をセットする
	@param[in] image 画像
	@param[in] x X座標
	@param[in] y Y座標
	@param[in] src セットする値
*/
_EXPORT inline void SetPixelToBGR(
	IplImage *image,
	int x,
	int y,
	const UCvPixel* src
)
{
	assert(image);
	assert(image->nChannels == 3);
	assert((0 <= x && x <= image->width-1) &&
		(0 <= y && y <= image->height-1));

	memcpy(image->imageData + y*image->widthStep + x*image->nChannels, &(src->value), sizeof(uint8_t)*3);

	//image->imageData[y *image->widthStep+ x * image->nChannels + 2] = r;
	//image->imageData[y *image->widthStep+ x * image->nChannels + 1] = g;
	//image->imageData[y *image->widthStep+ x * image->nChannels] = b;
}

//////////////////////////////////////////////
/*!
	指定位置にRGB値をセットする (4チャンネルBGRA画像用)
	@param[in] image 画像
	@param[in] x X座標
	@param[in] y Y座標
	@param[in] src セットする値
*/
_EXPORT inline void SetPixelToBGRA(
	IplImage *image,
	int x,
	int y,
	const UCvPixel* src)
{
	assert(image);
	assert(image->nChannels == 4);
	assert((0 <= x && x <= image->width-1) &&
		(0 <= y && y <= image->height-1));

	//*(UCvPixel*)(image->imageData + y*image->widthStep + x*image->nChannels) = src->value;

	memcpy(image->imageData + y*image->widthStep + x*image->nChannels, &(src->value), sizeof(uint32_t));

	//image->imageData[y *image->widthStep + x *image->nChannels + 3] = a;
	//image->imageData[y *image->widthStep + x *image->nChannels + 2] = r;
	//image->imageData[y *image->widthStep + x *image->nChannels + 1] = g;
	//image->imageData[y *image->widthStep + x *image->nChannels] = b;
}

//////////////////////////////////////////////
/*!
	指定位置にRGB値をセットする (4チャンネルBGRA画像用)
	@param[in] image 画像
	@param[in] x X座標
	@param[in] y Y座標
	@param[in] a セットするアルファー値
*/
_EXPORT inline void SetAlphaToBGRA(
	IplImage *image,
	int x,
	int y,
	uint8_t a)
{
	assert(image);
	assert(image->nChannels == 4);
	assert((0 <= x && x <= image->width-1) &&
		(0 <= y && y <= image->height-1));

	*(image->imageData + y*image->widthStep + x*image->nChannels + 3) = a;
}

#endif //_UTILOPENCVPIXEL_H_
