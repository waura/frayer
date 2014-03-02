#include "stdafx.h"

//#include "MaskWnd.h"
//
//#define MASK_CTRL_H 30
//
//CMaskWnd::CMaskWnd()
//{
//}
//
//CMaskWnd::~CMaskWnd()
//{
//}
//
//BEGIN_MESSAGE_MAP(CMaskWnd, CDockablePane)
//	ON_WM_CREATE()
//	ON_WM_SIZE()
//END_MESSAGE_MAP()
//
//int CMaskWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//	if(0 != CDockablePane::OnCreate(lpCreateStruct)){
//		return -1;
//	}
//
//	RECT rc;
//
//	//マスクフレーム作成
//	rc.top = 0;
//	rc.left = 0;
//	rc.right = lpCreateStruct->cx;
//	rc.bottom = lpCreateStruct->cy - MASK_CTRL_H;
//	if (!m_wndMaskFrame.Create(this, rc, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 133)){
//		return -1;
//	}
//
//	//マスクコントロール作成
//	rc.top = rc.bottom;
//	rc.bottom = lpCreateStruct->cy;
//	if (!m_wndMaskCtrl.Create(NULL, "", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, rc, this, 135)){
//		return -1;
//	}
//
//	return 0;
//}
//
//void CMaskWnd::OnSize(UINT nType, int cx, int cy)
//{
//	RECT rc;
//	this->GetClientRect(&rc);
//
//	m_wndMaskFrame.SetWindowPos(NULL, 0, 0, cx, cy - MASK_CTRL_H, SWP_NOZORDER);
//	m_wndMaskCtrl.SetWindowPos(NULL, 0, cy - MASK_CTRL_H, cx, MASK_CTRL_H, SWP_NOZORDER);
//
//	CDockablePane::OnSize(nType, cx, cy);
//}