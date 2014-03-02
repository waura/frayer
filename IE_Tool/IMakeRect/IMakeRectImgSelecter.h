#ifndef _IMAKERECTIMGSELECTER_H_
#define _IMAKERECTIMGSELECTER_H_

#include "IMakeRect.h"

class IMakeRectImgSelecter : public IMakeRect
{
public:	
	IMakeRectImgSelecter();
	virtual ~IMakeRectImgSelecter();

	virtual void OnSelect();
	virtual void OnNeutral();

	virtual void OnMouseRButtonDown(UINT nFlags,  LPPOINT lpt);
	virtual void OnMouseRButtonUp(UINT nFlags,  LPPOINT lpt);
};

#endif 