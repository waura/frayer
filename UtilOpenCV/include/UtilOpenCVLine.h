#ifndef _UTILOPENCVLINE_H_
#define _UTILOPENCVLINE_H_

#include "LibDllHeader.h"
#include "stdafx.h"

typedef struct _CvLine2D32f {
	CvPoint2D32f start;
	CvPoint2D32f end;
} CvLine2D32f;

typedef struct _CvLine2D64f {
	CvPoint2D64f start;
	CvPoint2D64f end;
} CvLine2D64f;

typedef struct _AfPoint2D64f{
	CvPoint2D64f src;
	CvPoint2D64f dst;
}AfPoint2D64f;

typedef std::vector<CvLine2D32f> CvLine2D32f_Vec;
typedef std::vector<CvLine2D64f> CvLine2D64f_Vec;

//////////////////////////
/*!
	線分line1と線分line2の交点pを求める。
	@param[out] p 交点
	@param[in] line1 線分1
	@param[in] line2 線分2
*/
_EXPORT void IntersectPoint(CvPoint2D64f *p, const CvLine2D64f *line1, const CvLine2D64f *line2);

//////////////////////////
/*!
	線分line1と線分line2の交差判定
	交差している場合はtrueを返す。そうでない場合はfalseを返す
	@param[in] line1 線分1
	@param[in] line2 線分2
	@return 交差しているならtrueを、そうでないならfalseを返す。
*/
_EXPORT bool IsIntersect(const CvLine2D64f *line1, const CvLine2D64f *line2);

//////////////////////////
/*!
	線分lineとx=cの交差判定をする。
	交差している場合はtrueを返す。そうでない場合はfalseを返す。
	@param[in] line 線分
	@param[in] c 
	@return 交差しているならtrueを、そうでないならfalseを返す。
*/
_EXPORT bool IsIntersectX(const CvLine2D64f *line, double c);

//////////////////////////
/*!
	線分lineとy=cの交差判定をする。
	交差している場合はtrueを返す。そうでない場合はfalseを返す。
	@param[in] line 線分
	@param[in] c 
	@return 交差しているならtrueを、そうでないならfalseを返す。
*/
_EXPORT bool IsIntersectY(const CvLine2D64f *line, double c);

//////////////////////////
/*!
	スキャンラインと辺(v1,v2)の交点を求める
*/
_EXPORT bool AfIntersect(const AfPoint2D64f *v1, const AfPoint2D64f *v2,
			   int y, double* x, double* sx, double* sy);

#endif //_UTILOPENCVLINE_H_