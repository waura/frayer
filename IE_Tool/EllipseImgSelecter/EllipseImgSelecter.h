#ifndef _ELIPSEIMGSELECTER_H_
#define _ELIPSEIMGSELECTER_H_

#include "IMakeRectImgSelecter.h"


//‘È‰~Œ`”ÍˆÍ‘I‘ð
class EllipseImgSelecter :
	public IMakeRectImgSelecter
{
public:
	EllipseImgSelecter();
	~EllipseImgSelecter();

	HBITMAP GetButtonImg();

	void OnDraw(HDC hdc);
	
	void OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd);
	void OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd);
	void OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd);

	void OnKeyboardDown(UINT nChar, UINT optChar);
};

#endif