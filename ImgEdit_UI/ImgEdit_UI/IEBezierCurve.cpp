#include "StdAfx.h"

#include "IEBezierCurve.h"

///////////////////////////////////////
/*!
	3次ベジェ曲線の座標リストを取得
	@param[in] p0 始点
	@param[in] p1 制御点1
	@param[in] p2 制御点2
	@param[in] p3 終点
	@param[in] DIVIDE 分解度
	@return 座標のvector配列
*/
vector<POINT> IEBezierCurve::GetBezier3CurverPointList(LPPOINT p0, LPPOINT p1, LPPOINT p2, LPPOINT p3, int DIVIDE)
{
	vector<POINT> pList;
	for(int i=0; i < DIVIDE; i++){
		float u = i*1.0/(DIVIDE-1);
		float mP0 = (1-u)*(1-u)*(1-u);
		float mP1 = 3 * u * (1-u) * (1-u);
		float mP2 = 3 * u * u * (1-u);
		float mP3 = u * u * u;
		
		POINT pt;
		pt.x = p0->x*mP0 + p1->x*mP1 + p2->x*mP2 + p3->x*mP3;
		pt.y = p0->y*mP0 + p1->y*mP1 + p2->y*mP2 + p3->y*mP3;
		pList.push_back(pt);
	}

	return pList;
}

//////////////////////////////////////
/*!
	2次ベジェ曲線の座標リストを取得
	@param[in] p0 始点
	@param[in] p1 制御点
	@param[in] p2 終点
	@parma[in] DIVIDE 分解度
	@return 座標のvector配列
*/
vector<POINT> IEBezierCurve::GetBezier2CurverPointList(LPPOINT p0, LPPOINT p1, LPPOINT p2, int DIVIDE)
{
	vector<POINT> pList;
	for(int i=0; i < DIVIDE; i++){
		float u = i*1.0/(DIVIDE-1);
		float mP0 = (1-u)*(1-u);
		float mP1 = 2 * u * (1-u);
		float mP2 = u * u;

		POINT pt;
		pt.x = p0->x*mP0 + p1->x*mP1 + p2->x*mP2;
		pt.y = p0->y*mP0 + p1->y*mP1 + p2->y*mP2;
		pList.push_back(pt);
	}

	return pList;
}