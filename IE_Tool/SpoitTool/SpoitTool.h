#ifndef _SPOITTOOL_H_
#define _SPOITTOOL_H_

#include "IIETool.h"
#include <libImgEdit.h>


//スポイトツール
//画像から色を得る
class SpoitTool : public IIETool
{
public:
	SpoitTool();
	virtual ~SpoitTool();

	//マウス座標入力情はウィンドウクライアント座標
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

	void SpoitColor(const LPIE_INPUT_DATA lpd);
};

#endif