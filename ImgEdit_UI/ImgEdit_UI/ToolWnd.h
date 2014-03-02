#pragma once

#include "ToolFrame.h"

class CToolWnd :
	public CDockablePane
{
public:
	CToolWnd();
	~CToolWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP();

private:
	CToolFrame m_ToolFrame;
};