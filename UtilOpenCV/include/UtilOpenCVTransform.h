#ifndef _UTILOPENCVTRANSFORM_H_
#define _UTILOPENCVTRANSFORM_H_

#include "LibDllHeader.h"
#include "stdafx.h"

class IplImageExt;

/////////////////////////////////////
/*!
	アフィン変換行列の逆変換行列をもとめる
	@param[in] src 2x3のアフィン変換行列
	@param[out] dst 2x3の逆行列出力先
*/
_EXPORT inline void InvertAffineMat(const CvMat* src, CvMat* dst)
{
	double det_src = cvmGet(src, 0, 0)*cvmGet(src, 1, 1) - cvmGet(src, 0, 1)*cvmGet(src, 1, 0);
	double inv_det_src = 1.0/det_src;

#ifdef _DEBUG
	double tmp;
	tmp = cvmGet(src, 1, 1)*inv_det_src;
	cvmSet(dst, 0, 0, tmp);
	tmp = -cvmGet(src, 0, 1)*inv_det_src;
	cvmSet(dst, 0, 1, tmp);
	tmp = (cvmGet(src, 0, 1)*cvmGet(src, 1, 2) - cvmGet(src, 0, 2)*cvmGet(src, 1, 1))*inv_det_src;
	cvmSet(dst, 0, 2, tmp);

	tmp = -cvmGet(src, 1, 0)*inv_det_src;
	cvmSet(dst, 1, 0, tmp);
	tmp = cvmGet(src, 0, 0)*inv_det_src;
	cvmSet(dst, 1, 1, tmp);
	tmp = (cvmGet(src, 0, 2)*cvmGet(src, 1, 0) - cvmGet(src, 0, 0)*cvmGet(src, 1, 2))*inv_det_src;
	cvmSet(dst, 1, 2, tmp);
#else
	cvmSet(dst, 0, 0, cvmGet(src, 1, 1)*inv_det_src);
	cvmSet(dst, 0, 1, -cvmGet(src, 0, 1)*inv_det_src);
	cvmSet(dst, 0, 2,(cvmGet(src, 0, 1)*cvmGet(src, 1, 2) - cvmGet(src, 0, 2)*cvmGet(src, 1, 1))*inv_det_src);
	
	cvmSet(dst, 1, 0, -cvmGet(src, 1, 0)*inv_det_src);
	cvmSet(dst, 1, 1, cvmGet(src, 0, 0)*inv_det_src);
	cvmSet(dst, 1, 2, (cvmGet(src, 0, 2)*cvmGet(src, 1, 0) - cvmGet(src, 0, 0)*cvmGet(src, 1, 2))*inv_det_src);
#endif //_DEBUG
}

/////////////////////////////////////
/*!
	比率による自由変形
	@param[out] dst 出力画像
	@param[in] src 入力画像
	@param[in] d_pts 出力先変形点(時計回りの順に4点を指定する)
*/
_EXPORT bool FreeDeformationByRatio(IplImage* dst, const IplImage* src, const CvPoint2D64f* d_pts);

/////////////////////////////////////
/*!
	アフィン変換
	@param[out] dst 出力先画像
	@param[in] src 入力画像
	@param[in] matrix 変換行列
	@param[in] fillval 
*/
_EXPORT void WarpAffine(IplImage* dst, const IplImageExt* src, const CvMat* matrix, int flag, const UCvPixel* fillval);
_EXPORT void WarpAffine_NN(IplImage* dst, const IplImageExt* src, const CvMat* matrix, const UCvPixel* fillval);
//_EXPORT void WarpAffine_Bilinear(IplImage* dst, const IplImageExt* src, const CvMat* matrix, const UCvPixel* fillval);

#endif //_UTILOPENCVTRANSFORM_H_