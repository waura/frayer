#ifndef _UTILOPENCVINTERPLATE_H_
#define _UTILOPENCVINTERPLATE_H_

#include "LibDllHeader.h"
#include "stdafx.h"


/*!
	@param[in] n ì_êî
	@param[in] src_points
	@param[in] dst åãâ äiî[çsóÒ
*/
_EXPORT void SolveSpline3D(int n, const CvPoint2D64f* src_points, CvMat* dst);

/*!
	@param[in] n
	@param[in] x
	@param[in] src_points
	@param[in] c_mat
*/
_EXPORT float FuncSpline3D(int n, float x, const CvPoint2D64f* src_points, const CvMat* c_mat);


#endif //_UTILOPENCVINTERPLATE_H_