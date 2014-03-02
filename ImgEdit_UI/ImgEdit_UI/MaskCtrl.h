#pragma once

#include "HoverBitmapButton.h"

class CMaskCtrl :
	public CWnd
{
public:
	CMaskCtrl();
	~CMaskCtrl();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();

	void OnBnClickedNewMask();
	void OnBnClickedDeleteMask();

	DECLARE_MESSAGE_MAP();

private:
	CHoverBitmapButton m_newMaskBtn;
	CHoverBitmapButton m_deleteMaskBtn;
};