#pragma once

#include "ToolButton.h"

#include <vector>


typedef std::vector<CToolButton*> CToolButton_Vec;


class CToolPopupPanel : public CWnd
{
public:
	CToolPopupPanel();
	virtual ~CToolPopupPanel();

	void SetIEToolGroup(IEToolGroup* pTGroup);

	inline int GetButtonSize() const {
		return m_ToolButton_Vec.size();
	}
	inline CToolButton* GetButtonAt(int i){
		if(0 <= i && i < m_ToolButton_Vec.size()){
			return m_ToolButton_Vec[i];
		}
		return NULL;
	}

	int GetHeight(int width);
	void AllButtonNewtral();
	void SelectToolButton(const IIETool* pTool);

	CString m_tool_tip_txt;

protected:
	BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint poiont);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint poitn);
	afx_msg LRESULT OnMouseLeave(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP();

private:
	CToolButton* GetToolButtonFromPos(CPoint point);

	IEToolGroup* m_pViewToolGroup;
	CToolButton_Vec m_ToolButton_Vec;
	CToolTipCtrl m_ToolTip;

	BOOL m_MouseEnterdCtrl;

	int m_btn_col;
};
