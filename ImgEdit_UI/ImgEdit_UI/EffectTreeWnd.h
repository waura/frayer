
#pragma once

#define UM_DROPITEM 0x000F //ツリービューアイテムが画像表示ウィンドウにドロップされたときWPARAMとして送る。

class CEffectTreeWnd : public CDockablePane
{
public:
	CEffectTreeWnd();
	~CEffectTreeWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult);
	afx_msg void OnBegindrag(LPNMTREEVIEW lpnmtv);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP();

private:
	HINSTANCE m_hInst;
	HWND m_hTree;

	HIMAGELIST m_hImLt;
	HTREEITEM m_hDragItem;
	bool m_isDragging;
};