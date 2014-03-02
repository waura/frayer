
#pragma once

#include "ToolPopupPanel.h"
#include <libImgEdit.h>

#define POPUP_TIME 500

typedef std::vector<CToolPopupPanel*> CToolPopupPanel_Vec;

class CToolFrame :
	public CWnd,
	public IEToolMGEventListener
{
public:
	CToolFrame();
	~CToolFrame();

	//
	void OnChangeSelectTool(IIETool* pNewTool, IIETool* pOldTool);
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnTipsDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	//afx_msg void OnLButtonDown(UINT nFlagas, CPoint point);
	//afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP();

private:
	////////////////////////////
	/*!
		ボタンを再配置
	*/
	void AdjastButtonGroup();

	////////////////////////////
	/*!
		位置からボタングループへのポインタを得る
	*/
	CToolPopupPanel* GetToolPanelFromPos(CPoint point);

	bool SelectToolButton(const IIETool* pTool);

	//////////////////////////////////////////////////////
	/*!
		ツールの種類をIDで指定する
		@param[in] id 指定するID
	*/
	void SetIEToolID(int id) const;

	CToolPopupPanel_Vec m_ToolPopupPanel_Vec;
};