
#pragma once

#include "BrushSelectFrame.h"
#include "BrushPreviewFrame.h"


class CBrushSelectWnd :
	public CDockablePane,
	public IEBrushMGEventListener
{
public:
	CBrushSelectWnd();
	~CBrushSelectWnd();

	void OnChangeSelectBrushGroup(
		IEBrushGroup_Ptr pNewGrushGroup, IEBrushGroup_Ptr pOldBrushGroup);

	void OnAddBrushGrup(IEBrushGroup_Ptr pBGroup);
	void OnDeleteBrushGroup(IEBrushGroup_Ptr pBGroup);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSelChangeBrushGroupCombo();
	LRESULT OnAppMouseWheel(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP();

private:
	void UpdateBrushGroupComboBox();
	
	CFont m_Font;

	CComboBox m_BrushGroupCmb;
	CBrushSelectFrame m_BrushSelectFrame;
	CBrushPreviewFrame m_BrushPreviewFrame;
};