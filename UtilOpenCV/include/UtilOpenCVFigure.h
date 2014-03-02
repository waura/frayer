#ifndef _UTILOPENCVFIGURE_H_
#define _UTILOPENCVFIGURE_H_

#include "LibDllHeader.h"
#include "UtilOpenCV.h"
#include "UtilOpenCV2D.h"
#include "UtilOpenCVPixel.h"
#include "IplImageExt.h"

class IplImageExt;


///////////////////////////////
/*!
	線分のマスクを作る。
	@param[out] mask マスクの出力先
	@param[in] setnum 埋める値
	@param[in] start 開始位置
	@param[in] end 終了位置
*/
_EXPORT void LineMask(IplImage* mask, uint8_t setnum, const CvPoint2D64f *start, const CvPoint2D64f *end);


_EXPORT void DrawLine(CvArr* img, CvPoint pt1, CvPoint pt2,
              CvScalar color, int thickness=1,
              int line_type=8, int shift=0);

_EXPORT void DrawDotLine(CvArr* img, CvPoint pt1, CvPoint pt2,
				 CvScalar color, int dot_length, int thickness = 1,
				 int line_type = 8, int shift = 0);

//四角形のマスクを作る。
_EXPORT void ucvFillRect(IplImage* dst, int num, int x, int y, int width, int height);
_EXPORT void FillRectMask(IplImage* mask, uint8_t setnum, int x, int y, int width, int height);
_EXPORT void IplExtFillRectMask(IplImageExt* mask, uint8_t setnum, int x, int y, int width, int height);

//楕円のマスクを作る。
_EXPORT void FillEllipseMask(IplImage* mask, uint8_t setnum, int x, int y, int width, int height);
_EXPORT void IplExtFillEllipseMask(IplImageExt* mask, uint8_t setnum, int x, int y, int width, int height);

//////////////////////////////////
/*!
	正円のマスクを作る。
	@param[out] mask マスクの出力先
	@param[in] setnum 埋める値
	@param[in] x0
	@param[in] y0
	@param[in] r
*/
_EXPORT void FillCircleMask(IplImage* mask, uint8_t setnum, int x0, int y0, int r);
///////////////////////////////////
/*!
	create antialiasing circle mask
	@param[out] dst
	@param[in] center_x
	@param[in] center_y
	@param[in] r
	@param[in] div
	@param[in] uint8_t setnum
	@param[in] alapha
*/
_EXPORT void FillCircleMaskAA(IplImage* dst, double center_x, double center_y, 
							 double r, int div, uint8_t setnum, uint8_t alpha);
_EXPORT void AddFillCircleMaskAA(IplImage* dst, double center_x, double center_y, 
							 double r, int div, uint8_t setnum, uint8_t alpha);

_EXPORT void FillCircleMaskAA2(IplImage* dst, double center_x, double center_y,
					  double r,  double aa_d, uint8_t setnum, uint8_t alpha);
_EXPORT void FillCircleMaskAA2e(IplImage* dst, double center_x, double center_y,
					  double r,  double aa_d, uint8_t setnum, uint8_t alpha);
_EXPORT void AddFillCircleMaskAA2(IplImage* dst, double center_x, double center_y,
					  double r,  double aa_d, uint8_t setnum, uint8_t alpha);

_EXPORT void FillCircleMaskAA3(IplImage* dst, double center_x, double center_y,
					  double r,  double aa_d, uint8_t setnum, uint8_t alpha);
_EXPORT void AddFillCircleMaskAA3(IplImage* dst, double center_x, double center_y,
					  double r,  double aa_d, uint8_t setnum, uint8_t alpha);

_EXPORT void SoftCircleMask(IplImage* dst, double center_x, double center_y,
					  double r, uint8_t setnum, uint8_t alpha);
_EXPORT void AddSoftCircleMask(IplImage* dst, double center_x, double center_y,
					  double r, uint8_t setnum, uint8_t alpha);
//floatにしてみたけど、遅いなぁ…
_EXPORT void AddSoftCircleMaskf(IplImage* dst, float center_x, float center_y,
					  float r, uint8_t setnum, uint8_t alpha);

////////////////////////////////////
/*!
	市松模様を書き込む
	@param[in,out] img 書き込み先 3or4 ch
	@param[in] color1 色1 rgb
	@parma[in] color2 色2 rgb
	@parma[in] block_size 模様のブロックサイズ
*/
_EXPORT void CheckerImage(IplImage* img, const UCvPixel* color1, const UCvPixel* color2, int block_size);

/////////////////////////////////////
/*!
	市松模様を書き込む
	@param[in,out] img 書き込み先 3or4 ch
	@param[in] color1 色1 rgb
	@param[in] color2 色2 rgb
	@param[in] block_size 模様のブロックサイズ
	@param[in] lprc 書き込み先範囲
*/
_EXPORT void CheckerImageRect(IplImage* img, const UCvPixel* color1, const UCvPixel* color2, int block_size, LPRECT lprc);
_EXPORT void CheckerImageRect2(IplImage* img, const UCvPixel* color1, const UCvPixel* color2, int block_size, LPRECT lprc);

/////////////////////////////////////
/*!
	市松模様を書き込む
	@param[in,out] img 書き込み先 3or4 ch
	@param[in] color1 色1 rgb
	@param[in] color2 色2 rgb
	@param[in] block_size 模様のブロックサイズ
	@param[in] lprc 書き込み先範囲
*/
_EXPORT void CheckerImageExtRect(IplImageExt* img, const UCvPixel* color1, const UCvPixel* color2, int block_size, LPRECT lprc);

/////////////////////////////////////
/*!
	画像に太さline_weight、色colorの枠を書き込む
	@param[in,out] img 書き込み先3or4 ch
	@param[in] line_weight 枠の太さ
	@param[in] color 枠の色 rgb
*/
_EXPORT void AddRectToImage(IplImage* img, int line_weight, const UCvPixel* color);



#endif // _UTILOPNECVFIGURE_H_