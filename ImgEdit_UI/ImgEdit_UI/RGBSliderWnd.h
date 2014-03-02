
#pragma once

#include "RGBSliderFrame.h"

class CRGBSliderWnd :
	public CDockablePane
{
public:
	CRGBSliderWnd();
	~CRGBSliderWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP();

private:

	CRGBSliderFrame m_RGBSliderFrame;
};