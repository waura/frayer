#ifndef _IEBEZIERCURVE_H_
#define _IEBEZIERCURVE_H_

#include "StdAfx.h"

using namespace std;

//ベジェ曲線クラス
class IEBezierCurve {
public:
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
	static vector<POINT> GetBezier3CurverPointList(LPPOINT p0, LPPOINT p1, 
										LPPOINT p2, LPPOINT p3, int DIVIDE=16);

	//////////////////////////////////////
	/*!
		2次ベジェ曲線の座標リストを取得
		@param[in] p0 始点
		@param[in] p1 制御点
		@param[in] p2 終点
		@parma[in] DIVIDE 分解度
		@return 座標のvector配列
	*/
	static vector<POINT> GetBezier2CurverPointList(LPPOINT p0, LPPOINT p1,
										LPPOINT p2, int DIVIDE=16);
};

#endif