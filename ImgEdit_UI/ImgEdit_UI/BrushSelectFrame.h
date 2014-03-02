#pragma once

#include "ScrollWnd.h"
#include "BrushPanel.h"
#include "ImgEditEventListener.h"


#define BRUSH_PANEL_W 50
#define BRUSH_PANEL_H 50

typedef std::vector<CBrushPanel*> BrushPanel_Vec;


class CBrushSelectFrame :
	public CScrollWnd,
	public IEBrushGroupEventListener
{
public:
	CBrushSelectFrame();
	~CBrushSelectFrame();

	//////////////////////////////////
	/*!
		洗濯ブラシ変更イベント
		@param[in] pNewBrush
		@param[in] pOldBrush
	*/
	void OnChangeSelectBrush(IEBrush_Ptr pNewBrush, IEBrush_Ptr pOldBrush);

	//////////////////////////////////
	/*!
		ブラシ追加イベント
		@param[in] pBrush 追加されるブラシ
	*/
	void OnAddBrush(IEBrush_Ptr pBrush);

	///////////////////////////////////
	/*!
		ブラシ削除イベント
	*/
	void OnDeleteBrush(IEBrush_Ptr pBrush);

	void SetBrushGroup(IEBrushGroup_Ptr pBGroup);

	////////////////////////////////////
	/*!
	*/
	virtual void OnDraw(CDC* pDC){
	}
	virtual void OnInitialUpdate(){
	}

protected:
	//DECLARE_DYNCREATE(CBrushSelectFrame)

	BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	//afx_msg BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	void PostNcDestroy();

	DECLARE_MESSAGE_MAP()

private:
	//////////////////////////////////////
	/*!
		選択パネルを変更する
		@param[in,out] pPanel 選択するパネル
	*/
	void ChangeSelectPanel(CBrushPanel* pPanel);

	//
	void AddBrushPanel(IEBrush_Ptr pBrush);

	//////////////////////////////////////
	/*!
		スクロールバーサイズ調節
	*/
	void AdjastScrollSizes();

	//////////////////////////////////////
	/*!
		パネルを再配置する。
	*/
	void AdjastPanel();

	//////////////////////////////////////
	/*!
		マウスの下にあるパネルのポインタを返す。
		@param[in] point クライアントマウス座標
		@return ブラシパネルのポインタ
	*/
	CBrushPanel* GetBrushPanelFromPos(CPoint point);

	//////////////////////////////////////
	/*!
		IEBrushポインタを保持しているパネルのポインタを返す。
		@param[in] pBrush ブラシポインタ
		@return ブラシパネルのポインタ
	*/
	CBrushPanel* GetBrushPanelFromIEBrush(const IEBrush_Ptr pBrush);

	//////////////////////////////////////
	/*!
		パネル配列からの除去しブラシパネルを削除する
	*/
	void DeleteBrushPanel(CBrushPanel* pPanel);


	BrushPanel_Vec m_BrushPanel_Vec;	//表示しているパネルの配列
	CBrushPanel *m_pSelectBrushPanel;	//選択中のパネル

	CToolTipCtrl m_ToolTip;

	IEBrushGroup_Ptr m_pViewBrushGroup;

	int m_panel_col; //
	int m_panel_x_index;
};