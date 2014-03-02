#pragma once

#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <cstdlib>
	#include <crtdbg.h>
#endif

#define _UOCV_USE_SSE_

#include <math.h>

#include <vector>

//OpenCV 1.x
//#include <cv.h>
//#include <cxcore.h>
//#include <highgui.h>

//OpenCV 2.x
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

#include <windows.h>
#include "UtilOpenCV.h"

#ifndef max
    #define max(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef min
    #define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef IS_16BYTE_ALIGNMENT(x)
	#define IS_16BYTE_ALIGNMENT(x) (!((int)(x) & 0xf))
#endif
