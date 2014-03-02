#ifndef _MOVE_SELECT_IMAGE_H_
#define _MOVE_SELECT_IMAGE_H_

#include "IIETool.h"
#include <libImgEdit.h>


#ifdef __cplusplus
#define IETOOL_EXPORT extern "C" __declspec (dllexport)
#else
#define IETOOL_EXPORT __declspec (dllexport)
#endif

IETOOL_EXPORT IIETool* CreateIETool();
IETOOL_EXPORT void ReleaseIETool(IIETool* tool);


class MoveSelectImage : public IIETool
{
public:
	MoveSelectImage();
	virtual ~MoveSelectImage();

	//入力情報を画像上の座標にする。
	virtual INPUT_DATA_FLAG GetInputDataFlag(){ return INPUT_DATA_FLAG::IMAGE_POS; }

	//ボタン画像
	virtual HBITMAP GetButtonImg();

	virtual void OnMouseLButtonDown(LPINPUT_DATA lpd);
	virtual void OnMouseLDrag(LPINPUT_DATA lpd);
	virtual void OnMouseLButtonUp(LPINPUT_DATA lpd);

private:
	virtual void MoveImage(LPINPUT_DATA lpd, LPPOINT beforPt);

	int move_X;
	int move_Y;
	IplImage *move_mask;
	RECT maskRc;
	RECT layer_Rc;
	RECT beforRc;
	POINT startPt;
	POINT beforPt;

	Edit_Node* m_editNode;
};

#endif //_MOVE_SELECT_IMAGE_H_