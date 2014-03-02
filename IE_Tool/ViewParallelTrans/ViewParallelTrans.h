#ifndef _VIEWPARALLELTRANS_H_
#define _VIEWPARALLELTRANS_H_

#include "IIETool.h"
#include <libImgEdit.h>

IETOOL_EXPORT IIETool* CreateIETool();
IETOOL_EXPORT void ReleaseIETool(IIETool** ppTool);


//画面平行移動ツール
class ViewParallelTrans : public IIETool
{
public:
	ViewParallelTrans();
	virtual ~ViewParallelTrans();

	//入力情報をウィンドウクライアント座標にする。
	virtual IE_INPUT_DATA_FLAG GetInputDataFlag(){ return IE_INPUT_DATA_FLAG::WINDOW_POS; }

	//ボタン画像
	virtual HBITMAP GetButtonImg();

	virtual void OnDraw(HDC hdc){};

	virtual void OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd);
	virtual void OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd);
	virtual void OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd);
	virtual void OnMouseRButtonDown(UINT nFlags, const LPPOINT lpt);
	virtual void OnMouseRButtonUp(UINT nFlags, const LPPOINT lpt);
private:

	bool m_isLDrag;
	POINT beforPt;
};

#endif //_VIEWPARALLELTRANS_H_