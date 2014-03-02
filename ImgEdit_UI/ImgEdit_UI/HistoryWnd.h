
#pragma once

#include "HistoryFrame.h"

class CHistoryWnd : public CDockablePane
{
public:
	CHistoryWnd();
	~CHistoryWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP();

private:
	CHistoryFrame m_wndHistoryFrame;
};