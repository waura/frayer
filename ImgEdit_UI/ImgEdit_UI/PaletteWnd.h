#pragma once

#include "PaletteFrame.h"
#include "PaletteCtrlPane.h"

class CPaletteWnd :
	public CDockablePane,
	public IEPaletteMGEventListener
{
public:
	CPaletteWnd();
	~CPaletteWnd();

	void OnChangeSelectPalette(IEPalette_Ptr pNewPalette, IEPalette_Ptr pOldPalette);
	void OnAddPalette(IEPalette_Ptr pPalette);
	void OnDeletePalette(IEPalette_Ptr pPalette);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelChangePaletteGroupCombo();
	afx_msg void OnPaint();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnTipsDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	LRESULT OnAppMouseWheel(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP();

private:
	void UpdatePaletteComboBox();

	CFont m_Font;

	CComboBox m_PaletteCmb;
	CPaletteCtrlPane m_PaletteCtrlPane;
	CPaletteFrame m_PaletteFrame;
};