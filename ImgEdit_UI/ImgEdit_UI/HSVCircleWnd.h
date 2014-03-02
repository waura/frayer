
#pragma once

#include "HSVCircleFrame.h"

class CHSVCircleWnd :
	public CDockablePane
{
public:
	CHSVCircleWnd();
	~CHSVCircleWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP();

private:

	CHSVCircleFrame m_HSVCircleFrame;
};