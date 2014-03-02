#ifndef _UTILOPENCV_COLOR_H_
#define _UTILOPNECV_COLOR_H_

#include "LibDllHeader.h"
#include "UtilOpenCVType.h"


//////////////////////////////////////////////
//  RGB <==> HSV
//	H = 0.0 ~ 359.0
//	S = 0.0 ~ 1.0
//	V = 0.0 ~ 1.0
//////////////////////////////////////////////
_EXPORT UCvPixel32 ucvHSV2RGB(double h, double s, double v);
_EXPORT double ucvRGB2V(UCvPixel32 color);
_EXPORT void ucvRGB2HSV(UCvPixel32 color, double *h, double *s, double *v);

//////////////////////////////////////////////
//  RGB <==> HLS
//////////////////////////////////////////////
_EXPORT UCvPixel32 ucvHLS2RGB(int h, int l, int s);
_EXPORT int ucvRGB2L(UCvPixel32 color);
_EXPORT void ucvRGB2HLS(UCvPixel32 color, int* h, int* l, int* s);

#endif //_UTILOPENCV_COLOR_H_