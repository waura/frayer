#ifndef _MOVE_LAYER_H_
#define _MOVE_LAYER_H_

#include "IIETool.h"
#include <libImgEdit.h>


class MoveLayer : public IIETool
{
public:
	MoveLayer();
	~MoveLayer();

	//入力情報を画像上の座標にする。
	IE_INPUT_DATA_FLAG GetInputDataFlag(){ return IE_INPUT_DATA_FLAG::IMAGE_POS; }

	//ボタン画像
	HBITMAP GetButtonImg();

	void OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd);
	void OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd);
	void OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd);

private:
	void fMoveLayer(LPIE_INPUT_DATA lpd, LPPOINT beforPt);

	int move_X;
	int move_Y;
	IplImage *move_mask;
	RECT maskRc;
	RECT layer_Rc;
	RECT beforRc;
	POINT startPt;
	POINT beforPt;

	IImgLayer_Ptr m_pEditLayer;
	MoveLayerHandle*  m_pMoveLayerHandle;
};

#endif //_MOVE_SELECT_IMAGE_H_