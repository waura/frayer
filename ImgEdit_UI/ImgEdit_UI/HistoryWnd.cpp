#include "stdafx.h"

#include "HistoryWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CHistoryWnd::CHistoryWnd()
{
}

CHistoryWnd::~CHistoryWnd()
{
}

BEGIN_MESSAGE_MAP(CHistoryWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int CHistoryWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CDockablePane::OnCreate(lpCreateStruct)){
		return -1;
	}

	RECT rc;
	rc.top = rc.left = 0;
	rc.right = lpCreateStruct->cx;
	rc.bottom = lpCreateStruct->cy;
	if(!m_wndHistoryFrame.Create(NULL, "", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, rc, this, 1)){
		return -1;
	}

	return 0;
}

void CHistoryWnd::OnSize(UINT nType, int cx, int cy)
{
	m_wndHistoryFrame.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER);

	CDockablePane::OnSize(nType, cx, cy);
}