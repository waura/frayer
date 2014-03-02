#ifndef _UTILOPENCV2D_H_
#define _UTILOPENCV2D_H_

#include "LibDllHeader.h"

////////////////////////////////
/*!
	ベクトルの足し算 dst = p1 + p2を行う
	@param[out] dst 出力結果
	@param[in] p1 ベクトル1
	@param[in] p2 ベクトル2
*/
_EXPORT inline void AddVec2D(CvPoint2D64f *dst, const CvPoint2D64f *p1, const CvPoint2D64f *p2)
{
	dst->x = p1->x + p2->x;
	dst->y = p1->y + p2->y;
}

////////////////////////////////
/*!
	ベクトルの引き算 dst = p1 - p2を行う
	@param[out] dst 出力結果
	@param[in] p1 ベクトル1
	@param[in] p2 ベクトル2
*/
_EXPORT inline void SubVec2D(CvPoint2D64f *dst, const CvPoint2D64f *p1, const CvPoint2D64f *p2)
{
	dst->x = p1->x - p2->x;
	dst->y = p1->y - p2->y;
}

////////////////////////////////
/*!
	ベクトルとスカラの積 dst = t*pを行う
	@param[out] dst 出力結果
	@param[in] t スカラt
	@param[in] p ベクトル
*/
_EXPORT inline void ScaleVec2D(CvPoint2D64f *dst, float t, const CvPoint2D64f *p)
{
	dst->x = t * p->x;
	dst->y = t * p->y;
}

////////////////////////////////
/*!
	ベクトルの大きさを返す
	@param[in] vec
	@return |vec|
*/
_EXPORT inline float Vec2DLength(const CvPoint2D64f *vec)
{
	return sqrt(vec->x * vec->x + vec->y * vec->y);
}

////////////////////////////////
/*!
	2次元ベクトルの外積の大きさを返す。
	@param[in] p1 ベクトル1
	@param[in] p2 ベクトル2
*/
_EXPORT inline float CrossVec2DLength(const CvPoint2D64f *p1, const CvPoint2D64f *p2)
{
	return (p1->x * p2->y - p1->y * p2->x);
}

#endif //_UTILOPENCV2D_H_