#ifndef _INPUTTEXT_H_
#define _INPUTTEXT_H_

#include "IIETool.h"
#include <libImgEdit.h>


IETOOL_EXPORT IIETool* CreateIETool();
IETOOL_EXPORT void ReleaseIETool(IIETool** ppTool);


class InputText : public IIETool
{
public:
	InputText();
	~InputText();

	//
	IE_INPUT_DATA_FLAG GetInputDataFlag(){ return IE_INPUT_DATA_FLAG::WINDOW_POS; }

	//ƒ{ƒ^ƒ“‰æ‘œ
	HBITMAP GetButtonImg();

	void OnMouseLButtonDown(LPIE_INPUT_DATA lpd);
	void OnMouseLDrag(LPIE_INPUT_DATA lpd);
	void OnMouseLButtonUp(LPIE_INPUT_DATA lpd);
private:

	EditNode* m_editNode;
};

#endif //_INPUTTEXT_H_