#ifndef _IECOURSORMG_H_
#define _IECOURSORMG_H_

#include "IEBrush.h"

class IECoursorMG
{
public:
	/////////////////////////////////////
	/*!
		マウスカーソルをブラシの形状に変更する。
		@param[in] IEBrush ブラシのポインタ
		@param[in] scale ブラシのサイズ倍率
	*/
	static bool SetBrushCoursor(HWND hWnd, const IEBrush_Ptr pBrush, double scale);
};

#endif //_IECOURSORMG_H_