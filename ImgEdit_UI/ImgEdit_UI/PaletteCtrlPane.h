
#pragma once

#include "HoverBitmapButton.h"
#include "NewPaletteColorDialog.h"

class CPaletteCtrlPane:
	public CWnd,
	public IEPaletteEventListener
{
public:
	CPaletteCtrlPane();
	~CPaletteCtrlPane();

	void SetPalette(IEPalette_Ptr pPalette){
		m_pViewPalette = pPalette;
	}

protected:
	BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();

	void OnClickedNewBtn();
	void OnClickedDeleteBtn();

	DECLARE_MESSAGE_MAP();

private:

	IEPalette_Ptr m_pViewPalette;

	CToolTipCtrl m_ToolTip;

	CHoverBitmapButton m_NewBtn;
	CHoverBitmapButton m_DeleteBtn;
};