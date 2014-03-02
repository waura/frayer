#ifndef _UTILOPENCVFILTER_H_
#define _UTILOPENCVFILTER_H_

#include "LibDllHeader.h"
#include "UtilOpenCVType.h"

_EXPORT void ucvGaussianFilter(const IplImage* src, int src_x, int src_y, int width, int height,
					IplImage* dst, int dst_x, int dst_y);

#endif //_UTILOPENCVFILTER_H_