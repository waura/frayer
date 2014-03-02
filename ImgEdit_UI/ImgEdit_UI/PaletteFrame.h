
#pragma once

#include "ScrollWnd.h"

typedef std::list<IplImage*> PanelImg_List;

class CPaletteFrame :
	public CScrollWnd,
	public IEPaletteEventListener
{
public:
	CPaletteFrame();
	~CPaletteFrame();

	////////////////////////////////////
	/*!
		選択色情報変更イベント
	*/
	void OnChangeSelectColor(int new_select_color_index);

	///////////////////////////////////
	/*!
		色情報追加イベント
	*/
	void OnAddColor(IEPaletteNode_Ptr pNode);

	///////////////////////////////////
	/*!
		色情報削除イベント
	*/
	void OnDeleteColor(int index, IEPaletteNode_Ptr pNode);

	void SetPalette(IEPalette_Ptr pPalette);

	virtual void OnDraw(CDC* pDC){};
	virtual void OnInitialUpdate(){};

	CString m_tool_tip_txt;

protected:

	BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	DECLARE_MESSAGE_MAP()

private:

	void ResetAllPanel();

	//////////////////////////////////////
	/*!
		スクロールバーサイズ調節
	*/
	void AdjastScrollSizes();

	IEPalette_Ptr m_pViewPalette;
	PanelImg_List m_PanelImg_List;

	CToolTipCtrl m_ToolTip;

	int m_select_panel_index;
	int m_panel_col; //
};