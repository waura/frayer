#ifndef _UTILOPENCVBLT_H_
#define _UTILOPENCVBLT_H_

#include "LibDllHeader.h"
#include "UtilOpenCVPixel.h"

///////////////////////////////////////
/*!
	SSEを使用したメモリコピーを行う
	@param[out] _Dst コピー先先頭アドレス
	@param[in] _Src コピー元先頭アドレス
	@param[in] size コピーするサイズ
*/
_EXPORT void ssememcpy(void* _Dst, const void* _Src, size_t size);

_EXPORT inline void BltAlphaBlend(
	IplImage* dst_img,
	int dst_startX,
	int dst_startY,
	int width,
	int height,
	const IplImage* under_img,
	int ui_startX,
	int ui_startY,
	const IplImage* over_img,
	int oi_startX,
	int oi_startY,
	const  IplImage* mask_img,
	int mi_startX,
	int mi_startY)
{
	assert( dst_img->nChannels == 4 );
	assert( under_img->nChannels == 4 );
	assert( over_img->nChannels == 4);

	int x,y;
	UCvPixel* dst;
	UCvPixel* over;
	UCvPixel* under;
	if(mask_img == NULL){
		for(y=0; y<height; y++){
			dst   = GetPixelAddress32(dst_img, dst_startX, dst_startY + y);
			under = GetPixelAddress32(under_img, ui_startX, ui_startY + y);
			over  = GetPixelAddress32(over_img, oi_startX, oi_startY + y);
			for(x=0; x<width; x++){
				AlphaBlendPixel(under, over, 255, dst);
				dst++;
				under++;
				over++;
			}
		}
	}
	else if(mask_img->nChannels == 4){
		UCvPixel* mask;
		for(y=0; y<height; y++){
			dst   = GetPixelAddress32(dst_img, dst_startX, dst_startY + y);
			under = GetPixelAddress32(under_img, ui_startX, ui_startY + y);
			over  = GetPixelAddress32(over_img, oi_startX, oi_startY + y);
			mask  = GetPixelAddress32(mask_img, mi_startX, mi_startY + y);
			for(x=0; x<width; x++){
				if(mask->a == 0){
					dst++;
					under++;
					over++;
					mask++;
					continue;
				}

				AlphaBlendPixel(under, over, mask->a, dst);
				dst++;
				under++;
				over++;
				mask++;
			}
		}
	}
	else if(mask_img->nChannels == 1){
		uint8_t* mask;
		for(y=0; y<height; y++){
			dst   = GetPixelAddress32(dst_img, dst_startX, dst_startY + y);
			under = GetPixelAddress32(under_img, ui_startX, ui_startY + y);
			over  = GetPixelAddress32(over_img, oi_startX, oi_startY + y);
			mask  = GetPixelAddress(mask_img, mi_startX, mi_startY + y);
			for(x=0; x<width; x++){
				if((*mask) == 0){
					dst++;
					under++;
					over++;
					mask++;
					continue;
				}

				AlphaBlendPixel(under, over, (*mask), dst);
				dst++;
				under++;
				over++;
				mask++;
			}
		}
	}
	else{
		assert(0);
	}
}

_EXPORT inline void BltCopy8(
	IplImage* dst_img,
	int dst_startX,
	int dst_startY,
	int width,
	int height,
	const IplImage* src_img,
	int src_startX,
	int src_startY,
	const  IplImage* mask_img,
	int mask_startX,
	int mask_startY)
{
	assert(dst_img->nChannels == 1);
	assert(src_img->nChannels == 1);

	int x,y;
	uint8_t* src;
	uint8_t* dst;
	uint8_t* mask;
	for(y=0; y<height; y++){
		dst   = GetPixelAddress(dst_img, dst_startX, dst_startY + y);
		src = GetPixelAddress(src_img, src_startX, src_startY + y);
		mask  = GetPixelAddress(mask_img, mask_startX, mask_startY + y);
		for(x=0; x<width; x++){
			if((*mask) == 0){
				dst++;
				src++;
				mask++;
				continue;
			}

			dst = src;
			dst++;
			src++;
			mask++;
		}
	}
}

_EXPORT inline void BltCopy32(
	IplImage* dst_img,
	int dst_startX,
	int dst_startY,
	int width,
	int height,
	const IplImage* src_img,
	int src_startX,
	int src_startY,
	const  IplImage* mask_img,
	int mask_startX,
	int mask_startY)
{
	assert(dst_img->nChannels == 4);
	assert(src_img->nChannels == 4);

	int x,y;
	UCvPixel* dst;
	UCvPixel* src;

	if(mask_img->nChannels == 4){
		UCvPixel* mask;
		for(y=0; y<height; y++){
			dst   = GetPixelAddress32(dst_img, dst_startX, dst_startY + y);
			src = GetPixelAddress32(src_img, src_startX, src_startY + y);
			mask  = GetPixelAddress32(mask_img, mask_startX, mask_startY + y);
			for(x=0; x<width; x++){
				if(mask->a == 0){
					dst++;
					src++;
					mask++;
					continue;
				}

				dst->value = src->value;
				dst++;
				src++;
				mask++;
			}
		}
	}
	else if(mask_img->nChannels == 1){
		uint8_t* mask;
		for(y=0; y<height; y++){
			dst   = GetPixelAddress32(dst_img, dst_startX, dst_startY + y);
			src = GetPixelAddress32(src_img, src_startX, src_startY + y);
			mask  = GetPixelAddress(mask_img, mask_startX, mask_startY + y);
			for(x=0; x<width; x++){
				if((*mask) == 0){
					dst++;
					src++;
					mask++;
					continue;
				}

				dst->value = src->value;
				dst++;
				src++;
				mask++;
			}
		}
	}
	else{
		assert(0);
	}
}

_EXPORT inline void BltCopy(
	IplImage* dst_img,
	int dst_startX,
	int dst_startY,
	int width,
	int height,
	const IplImage* src_img,
	int src_startX,
	int src_startY,
	const  IplImage* mask_img,
	int mask_startX,
	int mask_startY)
{
	assert(dst_img->nChannels == src_img->nChannels);

	if(mask_img == NULL){
		int y;
		uint8_t* dst;
		uint8_t* src;
		int byte_width = width * dst_img->nChannels * (dst_img->depth >> 3);
		for(y=0; y<height; y++){
			dst = GetPixelAddress(dst_img, dst_startX, dst_startY + y);
			src = GetPixelAddress(src_img, src_startX, src_startY + y);
			memcpy(dst, src, byte_width);
		}
	}
	else if(dst_img->nChannels == 4){
		BltCopy32(
			dst_img,
			dst_startX,
			dst_startY,
			width,
			height,
			src_img,
			src_startX,
			src_startY,
			mask_img,
			mask_startX,
			mask_startY);
	}
	else if(dst_img->nChannels == 1){
		BltCopy32(
			dst_img,
			dst_startX,
			dst_startY,
			width,
			height,
			src_img,
			src_startX,
			src_startY,
			mask_img,
			mask_startX,
			mask_startY);
	}
	else{
		assert(0);
	}
}

_EXPORT inline void FillColor(
	IplImage* dst_img,
	int dst_startX,
	int dst_startY,
	int width,
	int height,
	const UCvPixel* color)
{
	assert(dst_img->nChannels == 4);

	int x,y;
	UCvPixel* dst;
	for(y=0; y<height; y++){
		dst   = GetPixelAddress32(dst_img, dst_startX, dst_startY + y);
		for(x=0; x<width; x++){
			dst->value = color->value;
			dst++;
		}
	}
}

_EXPORT inline void FillMask(
	IplImage* dst_img,
	int dst_startX,
	int dst_startY,
	int width,
	int height,
	uint8_t val)
{
	assert(dst_img->nChannels == 1);

	int x,y;
	uint8_t* dst;
	for(y=0; y<height; y++){
		dst   = GetPixelAddress(dst_img, dst_startX, dst_startY + y);
		for(x=0; x<width; x++){
			(*dst) = val;
			dst++;
		}
	}
}

_EXPORT inline void FillZero(
	IplImage* dst_img,
	int dst_startX,
	int dst_startY,
	int width,
	int height)
{
	if(dst_img->nChannels == 4){
		UCvPixel zero;
		zero.value = 0;
		FillColor(
			dst_img,
			dst_startX,
			dst_startY,
			width,
			height,
			&zero);
	}
	else if(dst_img->nChannels == 1){
		FillMask(
			dst_img,
			dst_startX,
			dst_startY,
			width,
			height,
			0);
	}
	else{
		assert(0);
	}
}

_EXPORT inline void FillAlphaBlendColor(
	IplImage* dst_img,
	int dst_startX,
	int dst_startY,
	int width,
	int height,
	const UCvPixel* color)
{
	assert(dst_img->nChannels == 4);

	int x,y;
	UCvPixel* dst;
	for(y=0; y<height; y++){
		dst   = GetPixelAddress32(dst_img, dst_startX, dst_startY + y);
		for(x=0; x<width; x++){
			AlphaBlendPixel(dst, color, 255, dst);
			dst++;
		}
	}
}



#endif //_UTILOPENCVBLT_H_