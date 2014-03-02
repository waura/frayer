#pragma once

#include "HoverBitmapButton.h"

class CLayerCtrl :
	public CWnd
{
public:
	CLayerCtrl();
	~CLayerCtrl();

protected:
	BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	
	void OnBnClickedNewLayer();
	void OnBnClickedNewLayerGroup();
	void OnBnClickedDeleteLayer();
	
	DECLARE_MESSAGE_MAP();

private:
	CToolTipCtrl m_ToolTip;

	CHoverBitmapButton m_newLayerBtn;
	CHoverBitmapButton m_newLayerGroupBtn;
	CHoverBitmapButton m_deleteLayerBtn;
};