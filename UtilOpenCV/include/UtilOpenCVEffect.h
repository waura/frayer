#ifndef _UTILOPENCVEFFECT_H_
#define _UTILOPENCVEFFECT_H_

#include "LibDllHeader.h"
#include "UtilOpenCVPixel.h"

///////////////////////////////////////////////////////
/*!
	放射状ブラー
	@param[in] src 入力画像
	@param[out] dst 出力画像
	@param[in] ef ブラー幅 0~1で指定
*/
_EXPORT void RadiallyBlur(const IplImage* src, IplImage* dst, double ef);

#endif //_UTILOPENCVEFFECT_H_