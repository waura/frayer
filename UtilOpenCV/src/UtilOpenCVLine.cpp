#include "stdafx.h"

#include "UtilOpenCVLine.h"
#include "UtilOpenCV2D.h"


//////////////////////////
/*!
	線分line1と線分line2の交点pを求める。
	@param[out] p 交点
	@param[in] line1 線分1
	@param[in] line2 線分2
*/
_EXPORT void IntersectPoint(CvPoint2D64f *p, const CvLine2D64f *line1, const CvLine2D64f *line2)
{
	//a1 = line1->start
	//a2 = line1->end
	//b1 = line2->start
	//b2 = line2->end;

	//線分1の終点と線分2の始点が異なる場合での端点での交差判定
	if(!((line1->end.x == line2->start.x) && (line1->end.y == line2->end.y))){
		if((line1->end.x == line2->end.x) && (line1->end.y == line2->end.y)){
			*p = line1->end;
			return;
		}
		else if((line1->start.x == line2->end.x) && (line1->start.y == line2->end.y)){
			*p = line1->start;
			return;
		}
	}

	CvPoint2D64f b2b1;
	CvPoint2D64f a2a1;
	CvPoint2D64f a1b1;
	CvPoint2D64f a2b1;

	SubVec2D(&b2b1, &(line2->end), &(line2->start));
	SubVec2D(&a2a1, &(line1->end), &(line1->start));
	SubVec2D(&a1b1, &(line1->start), &(line2->start));
	SubVec2D(&a2b1, &(line1->end), &(line2->start));

	double d1 = fabs(CrossVec2DLength(&b2b1, &a1b1));
	double d2 = fabs(CrossVec2DLength(&b2b1, &a2b1));

	if(d1 + d2 != 0){
		double t = d1 / (d1 + d2);

		// p = a1 + (a2 - a1)*t
		ScaleVec2D(p, t, &a2a1);
		AddVec2D(p, p, &(line1->start));
	}
	else{
		::MessageBox(NULL, "t", "", MB_OK);
	}
}

//////////////////////////
/*!
	線分line1と線分line2の交差判定
	交差している場合はtrueを返す。そうでない場合はfalseを返す
	@param[in] line1 線分1
	@param[in] line2 線分2
*/
_EXPORT bool IsIntersect(const CvLine2D64f *line1, const CvLine2D64f *line2)
{
	//a1 = line1->start
	//a2 = line1->end
	//b1 = line2->start
	//b2 = line2->end;

	//線分1の終点と線分2の始点が異なる場合での端点での交差判定
	if(!((line1->end.x == line2->start.x) && (line1->end.y == line2->end.y))){
		if((line1->end.x == line2->end.x) && (line1->end.y == line2->end.y)){
			return true;
		}
		else if((line1->start.x == line2->end.x) && (line1->start.y == line2->end.y)){
			return true;
		}
	}

	CvPoint2D64f a2a1;
	CvPoint2D64f b2b1;
	CvPoint2D64f b2a1;
	CvPoint2D64f a2b1;
	CvPoint2D64f b1a1;
	CvPoint2D64f a1b1;

	SubVec2D(&a2a1, &(line1->end), &(line1->start));
	SubVec2D(&b2b1, &(line2->end), &(line2->start));
	SubVec2D(&b2a1, &(line2->end), &(line1->start));
	SubVec2D(&a2b1, &(line1->end), &(line2->start));
	SubVec2D(&b1a1, &(line2->start), &(line1->start));
	SubVec2D(&a1b1, &(line1->start), &(line2->start));

	//外積の大きさの符号の違いを利用して判定
	if( (CrossVec2DLength(&a2a1, &b1a1) * CrossVec2DLength(&a2a1, &b2a1) < 0) &&
		(CrossVec2DLength(&b2b1, &a1b1) * CrossVec2DLength(&b2b1, &a2b1) <= 0) ){
			return true;
	}

	return false;
}

_EXPORT bool IsIntersectX(const CvLine2D64f *line, double c)
{
	double min_x = min(line->start.x, line->end.x);
	double max_x = max(line->start.x, line->end.x);
	if (min_x <= c && c <= max_x) {
		return true;
	}
	return false;
}

_EXPORT bool IsIntersectY(const CvLine2D64f *line, double c)
{
	double min_y = min(line->start.y, line->end.y);
	double max_y = max(line->start.y, line->end.y);
	if (min_y <= c && c <= max_y) {
		return true;
	}
	return false;
}

_EXPORT bool AfIntersect(
	const AfPoint2D64f *v1,
	const AfPoint2D64f* v2,
	int y,
	double* x,
	double* sx,
	double* sy)
{
	y += 0.001; //
	if( ((y <= v1->dst.y) && (y >= v2->dst.y)) ||
		((y >= v1->dst.y) &&( y <= v2->dst.y)) )
	{
		if(y == v1->dst.y){
			*x = v1->dst.x;
			*sx = v1->src.x;
			*sy = v1->src.y;
			return true;;
		}
		if(y== v2->dst.y){
			*x = v2->dst.x;
			*sx = v2->src.x;
			*sy = v2->src.y;
			return true;
		}

		double m = (y - v1->dst.y) / (v2->dst.y - v1->dst.y);
	    *x  = v1->dst.x + m*(v2->dst.x - v1->dst.x);
	    *sx = v1->src.x + m*(v2->src.x - v1->src.x);
	    *sy = v1->src.y + m*(v2->src.y - v1->src.y);
		return true;
	}

	return false;
}