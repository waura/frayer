#ifndef _POLYGONSELECTER_H_
#define _POLYGONSELECTER_H_

#include "PolygonSelecter_Base.h"


//多角形選択
class PolygonSelecter : public PolygonSelecter_Base
{
public:
	PolygonSelecter();
	virtual ~PolygonSelecter();

	//ボタン画像
	virtual HBITMAP GetButtonImg();

	virtual void OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd);
	virtual void OnMouseMove(UINT nFlags,  const LPIE_INPUT_DATA lpd);
	virtual void OnMouseLButtonUp(UINT nFlags,  const LPIE_INPUT_DATA lpd);

	virtual void OnMouseRButtonUp(UINT nFlags, const LPPOINT lpt);
};

#endif //_POLYGONSELECTER_H_