#include "stdafx.h"

#include "UtilOpenCVFilter.h"

void ucvGaussianFilter(
	const IplImage* src,
	int src_x,
	int src_y,
	int width,
	int height,
	IplImage* dst,
	int dst_x,
	int dst_y)
{
	assert(dst->nChannels == 4);
	assert(src->nChannels == 4);

	int kernel_size = 7;
	int ker_r = (kernel_size-1)/2;
	double sig = (kernel_size/2 - 1)*0.3 + 0.8;
	double s = 2*sig*sig;
	double div = sqrt(s*CV_PI);

	if(src_x < ker_r) src_x = ker_r;
	if(src_y < ker_r) src_y = ker_r;
	if((src_x + width + ker_r) > src->width) width = src->width - src_x - ker_r;
	if((src_y + height + ker_r) > src->height) height = src->height - src_y - ker_r;

	double* mask = (double*) malloc(sizeof(double)*kernel_size);
	double* mask_ptr = mask;

	//create mask
	for(int i=0; i<kernel_size; i++){
		int p = (i - ker_r)*(i - ker_r);
		(*mask_ptr) = exp(-p/s)/div;
		mask_ptr++;
	}

	int i,x,y;
	double b,g,r,a;
	UCvPixel32* src_pix;
	UCvPixel32* dst_pix;
	UCvPixel32* s_ker_pix;

	//x
	for(x=0; x<width; x++){
		src_pix = GetPixelAddress32(src, src_x + x, src_y);
		dst_pix = GetPixelAddress32(dst, dst_x + x, dst_y);
		for(y=0; y<height; y++){
			b = g = r = a = 0.0;
			mask_ptr = mask;
			s_ker_pix = src_pix - (src->width * ker_r);
			for(i=0; i<kernel_size; i++){
				b += (*mask_ptr) * s_ker_pix->b;
				g += (*mask_ptr) * s_ker_pix->g;
				r += (*mask_ptr) * s_ker_pix->r;
				//a += (*mask_ptr) * s_ker_pix->a;
				s_ker_pix += src->width;
				mask_ptr++;
			}
			dst_pix->b = b;
			dst_pix->g = g;
			dst_pix->r = r;
			//dst_pix->a = a;

			src_pix += src->width;
			dst_pix += dst->width;
		}
	}

	//y
	for(y=0; y<height; y++){
		dst_pix = GetPixelAddress32(dst, dst_x, dst_y + y);
		for(x=0; x<width; x++){
			b = g = r = a = 0.0;
			mask_ptr = mask;
			s_ker_pix = dst_pix;
			s_ker_pix -= ker_r;
			for(i=0; i<kernel_size; i++){
				b += (*mask_ptr) * s_ker_pix->b;
				g += (*mask_ptr) * s_ker_pix->g;
				r += (*mask_ptr) * s_ker_pix->r;
				//a += (*mask_ptr) * s_ker_pix->a;
				s_ker_pix++;
				mask_ptr++;
			}
			dst_pix->b = b;
			dst_pix->g = g;
			dst_pix->r = r;
			//dst_pix->a = a;
			dst_pix++;
		}
	}

	free(mask);
}