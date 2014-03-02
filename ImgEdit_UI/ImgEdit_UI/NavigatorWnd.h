
#pragma once

#include "PreviewFrame.h"
#include "FileSizeScroll.h"
#include "ViewCtrlPane.h"

class CNavigatorWnd : public CDockablePane
{
public:
	CNavigatorWnd();
	~CNavigatorWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	LRESULT OnAppMouseWheel(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP();

private:
	CViewCtrlPane m_wndViewCtrlPane;
	CPreviewFrame m_wndPreviewFrame;
	CFileSizeScroll m_wndFileSizeScroll;
};