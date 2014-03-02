#pragma once

#include "ToolEditFrame.h"

class CToolEditWnd : public CDockablePane
{
public:
	CToolEditWnd();
	~CToolEditWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP();

private:

	CToolEditFrame m_wndToolEditFrame;
};