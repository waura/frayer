#include "stdafx.h"

#include "UtilOpenCVEffect.h"


#define _PI_ 3.14159265
#define NN 17
#define NF 8

_EXPORT void RadiallyBlur(const IplImage* src, IplImage* dst, double ef)
{
	assert(src->nChannels == dst->nChannels);

	int center_x = src->width/2;
	int center_y = src->height/2;

	int i,x,y;
	int xx, yy;
	int dx, dy;
	int pat_sum, pat;
	double rate;
	double rad, dis, disI;
	double disMax = sqrt((double)src->height*src->height + src->width*src->width);

	int r, g, b, a=255;
	int sampling_r, sampling_g, sampling_b, sampling_a;
	for(y=0; y<src->height; y++){
		for(x=0; x<src->width; x++){
			dx = x - center_x;
			dy = y - center_y;

			if(dx != 0) rad = atan((double)dy/dx);
			else rad = _PI_/2.0;

			dis = sqrt((double)dx*dx + dy*dy); //distance of from center to (x, y)
			rate = ef * dis / disMax;
			rate /= (double)NN;
			pat_sum = 0;

			for(i=0; i<NN; i++){
				if(i == NF) pat = 3;
				else pat = 1;

				disI = (double)(i-NF)*rate;
				xx = (int)(disI*cos(rad)) + x;
				yy = (int)(disI*sin(rad)) + y;
			
				//read pixel
				switch(src->nChannels){
					case 1:
						a = GetMaskDataPos(src, x, y);
						sampling_a = a * pat;
						break;
					case 3:
						break;
					case 4:
						break;
				}
				pat_sum += pat;
			}

			//write pixel
			switch(dst->nChannels){
				case 1:
					SetMaskDataPos(dst, x, y, a/pat_sum);
					break;
				case 3:
					break;
				case 4:
					break;
			}
		}


	}
}