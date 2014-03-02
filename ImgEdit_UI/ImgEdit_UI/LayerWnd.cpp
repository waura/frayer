#include "stdafx.h"

#include "LayerWnd.h"

#define LAYER_ALPHA_EDIT_ID 133
#define LAYER_ALPHA_EDIT_H 30

#define LAYER_LOCK_STATE_ID 134
#define LAYER_LOCK_STATE_H 30

#define LAYER_FRAME_ID 135

#define LAYER_CTRL_ID 136
#define LAYER_CTRL_H 30

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLayerWnd::CLayerWnd()
{
}

CLayerWnd::~CLayerWnd()
{
}

BEGIN_MESSAGE_MAP(CLayerWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(WM_APP_MOUSEWHEEL, CLayerWnd::OnAppMouseWheel)
END_MESSAGE_MAP()

int CLayerWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CDockablePane::OnCreate(lpCreateStruct)){
		return -1;
	}

	RECT rc;
	
	//レイヤーアルファエディットウィンドウ作成
	rc.top = 0;
	rc.left = 0;
	rc.right = lpCreateStruct->cx;
	rc.bottom = LAYER_ALPHA_EDIT_H;
	if(!m_wndLayerAlphaEdit.Create(
			NULL,
			"",
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			rc,
			this,
			LAYER_ALPHA_EDIT_ID))
	{
		return -1;
	}

	//
	rc.top = rc.bottom;
	rc.bottom = rc.top + LAYER_LOCK_STATE_H;
	if(!m_wndLayerLockState.Create(
			NULL,
			"",
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			rc,
			this,
			LAYER_LOCK_STATE_ID))
	{
		return -1;
	}


	//レイヤーフレーム作成
	rc.top = rc.bottom;
	rc.bottom = lpCreateStruct->cy - LAYER_CTRL_H;
	if(!m_wndLayerFrame.Create(
			this,
			rc,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			LAYER_FRAME_ID))
	{
		return -1;
	}

	//レイヤーコントロールフレーム作成
	rc.top = rc.bottom;
	rc.bottom = lpCreateStruct->cy;
	if(!m_wndLayerCtrl.Create(
			NULL,
			"",
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			rc,
			this,
			LAYER_CTRL_ID))
	{
		return -1;
	}

	return 0;
}

void CLayerWnd::OnSize(UINT nType, int cx, int cy)
{
	RECT rc;
	this->GetClientRect(&rc);

	int y = 0;
	m_wndLayerAlphaEdit.SetWindowPos(NULL, 0, y, cx, LAYER_ALPHA_EDIT_H, SWP_NOZORDER);
	y += LAYER_ALPHA_EDIT_H;

	m_wndLayerLockState.SetWindowPos(NULL, 0, y, cx, LAYER_LOCK_STATE_H, SWP_NOZORDER);
	y += LAYER_LOCK_STATE_H;

	m_wndLayerFrame.SetWindowPos(NULL, 0, y, cx, cy - y - LAYER_CTRL_H, SWP_NOZORDER);
	
	m_wndLayerCtrl.SetWindowPos(NULL, 0, cy - LAYER_CTRL_H, cx, LAYER_CTRL_H, SWP_NOZORDER);

	CDockablePane::OnSize(nType, cx, cy);
}

LRESULT CLayerWnd::OnAppMouseWheel(WPARAM wParam, LPARAM lParam)
{
	UINT nFlags = GET_KEYSTATE_WPARAM(wParam);
	short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	CPoint pt;
	pt.x = GET_X_LPARAM(lParam); 
	pt.y = GET_Y_LPARAM(lParam);
	m_wndLayerFrame.DoMouseWheel(nFlags, zDelta, pt);
	return TRUE;
}