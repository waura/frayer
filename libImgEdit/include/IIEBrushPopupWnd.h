#ifndef _IIEBRUSHPOPUPWND_H_
#define _IIEBRUSHPOPUPWND_H_

#include "IEBrush.h"

class IIEBrushPopupWnd
{
public:
	IIEBrushPopupWnd(){};
	virtual ~IIEBrushPopupWnd(){};

	virtual void ShowPopup(){};
	virtual void SetEditBrush(IEBrush_Ptr pBrush){};
private:
};

#endif //_IIEBRUSHPOPUPWND_H_