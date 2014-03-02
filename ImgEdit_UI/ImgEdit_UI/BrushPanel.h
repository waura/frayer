#pragma once

#include "IEBrush.h"

#define CM_ID_BRUSH_DELETE 1
#define CM_ID_BRUSH_EDIT 2

class CBrushPanel :
	public CWnd,
	public IEBrushEventListener
{
public:
	CBrushPanel();
	virtual ~CBrushPanel();

	void OnChangeOrgRadius(int old_rad, int new_rad);
	void OnChangeBrushShape();

	/////////////////////////////////////////
	/*!
	*/
	void SetBrush(IEBrush_Ptr pBrush);

	///////////////////////////////////
	/*!
		保持しているブラシを削除する。
	*/
	void DeleteBrush();

	///////////////////////////////////
	/*!
		非選択状態にする
	*/
	void NewtralPanel();

	///////////////////////////////////
	/*!
		選択状態にする
	*/
	void SelectPanel();

	///////////////////////////////////
	/*!
		再描画
		@param[in: lprect 再描画する範囲 全体のときはNULLを指定する。
	*/
	void RePaint(LPRECT lprect);

	//////////////////////////////////////////////
	/*!
		管理しているブラシのポインタを返す
	*/
	IEBrush_Ptr GetBrush();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	void PostNcDestroy();

	DECLARE_MESSAGE_MAP();

private:

	IEBrush_Ptr m_pBrush;
	bool m_isSelectBrush; //パネルの選択状態

	CFont m_Font;
};
