#include "stdafx.h"

#include "UtilOpenCVIO.h"
#include "IplImageExt.h"

_EXPORT void SaveIplImageExt(const char* path, const IplImageExt* image)
{
	CvSize size = image->GetSize();
	RECT rc;
	rc.left = 0; rc.top =0;
	rc.right = size.width; rc.bottom = size.height;
	IplImage* i = cvCreateImage(size, IPL_DEPTH_8U, 3);
	UCvPixel color1 = ucvPixel(255,255,255,255);
	UCvPixel color2 = ucvPixel(100,100,100,255);
	CheckerImageRect(i, &color1, &color2, 25, &rc);

	int x,y;
	UCvPixel pixel1;
	UCvPixel pixel2;
	for(y=0; y<size.height; y++){
		for(x=0; x<size.width; x++){
			switch(image->GetImgNChannel()){
				case 4:
					image->GetPixel(x, y, &pixel1);
					break;
				case 1:
					image->GetMaskData(x, y, &(pixel1.r));
					pixel1.b = pixel1.g = pixel1.r;
					pixel1.a = 255;
					break;
				default:
					assert(0);
					break;
			}
			GetPixelFromBGR(i, x, y, &pixel2);

			pixel1.b = pixel1.b*(pixel1.a/255.0) + (1.0 - pixel1.a/255.0)*pixel2.b;
			pixel1.g = pixel1.g*(pixel1.a/255.0) + (1.0 - pixel1.a/255.0)*pixel2.g;
			pixel1.r = pixel1.r*(pixel1.a/255.0) + (1.0 - pixel1.a/255.0)*pixel2.r;
			SetPixelToBGR(i, x, y, &pixel1);
		}
	}

	cvSaveImage(path, i);
	cvReleaseImage(&i);
}

_EXPORT bool LoadIplImageExt(const char* path, IplImageExt* image)
{
	IplImage* i = cvLoadImage(path);
	if (i == NULL) return false;

	image->Init(i->width, i->height, i->depth, 4);
	
	int x,y;
	UCvPixel pixel;
	for(y=0; y<i->height; y++){
		for(x=0; x<i->width; x++){
			GetPixelFromBGR(i, x, y, &pixel);
			pixel.a = 255;
			image->SetPixel(x, y, &pixel);
		}
	}

	cvReleaseImage(&i);
	return true;
}