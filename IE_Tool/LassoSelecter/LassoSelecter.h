#ifndef _LASSOSELECTER_H_
#define _LASSOSELECTER_H_

#include "PolygonSelecter_Base.h"


//投げ縄選択
class LassoSelecter : public PolygonSelecter_Base
{
public:
	LassoSelecter();
	virtual ~LassoSelecter();

	//入力情報、補間、補正有り
	virtual bool isRevise(){ return true; }

	//ボタン画像
	virtual HBITMAP GetButtonImg();

	virtual void OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd);
	virtual void OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd);
	virtual void OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd);

private:
	///////////////////////////
	/*!
	*/
	virtual void Push_Line(LPIE_INPUT_DATA lpd);

	bool m_isSelected;
};

#endif //_LASSOSELECTER_H_