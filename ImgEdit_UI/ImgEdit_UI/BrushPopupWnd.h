#pragma once

#include "IEBrush.h"
#include "IIEBrushPopupWnd.h"

#define PROP_BPP_WINPROC "CBrushPopupWnd"

class CBrushPopupWnd : 
	public CWnd,
	public IIEBrushPopupWnd 
{
public:
	CBrushPopupWnd();
	virtual ~CBrushPopupWnd();

	BOOL CreatePopupWnd(CWnd* pOwner, UINT nID);

	//////////////////////////////////
	/*!
		座標を指定してポップアップを表示
		@param[in] lpt スクリーン座標
	*/
	void ShowPopup();

	//////////////////////////////////
	/*!
		ポップアップを非表示
	*/
	void HidePopup();

	///////////////////////////////////
	/*!
		編集するブラシをセットする。
	*/
	void SetEditBrush(IEBrush_Ptr pBrush);

	BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP();

private:

	IEBrush_Ptr m_pEditBrush;

	CFont m_font;
	CStatic m_BrushSizeLabel;
	CSliderCtrl m_BrushSizeSlider;
};