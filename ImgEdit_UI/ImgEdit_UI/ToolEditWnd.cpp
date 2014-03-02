#include "stdafx.h"

#include "ToolEditWnd.h"

#define ID_TOOL_EDIT_FRAME 111

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CToolEditWnd::CToolEditWnd()
{
}

CToolEditWnd::~CToolEditWnd()
{
}

BEGIN_MESSAGE_MAP(CToolEditWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int CToolEditWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CDockablePane::OnCreate(lpCreateStruct)){
		return -1;
	}

	
	RECT rc;
	rc.top = rc.left = 0;
	rc.right = lpCreateStruct->cx;
	rc.bottom = lpCreateStruct->cy;
	if(!m_wndToolEditFrame.Create(this, rc, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, ID_TOOL_EDIT_FRAME)){
		return -1;
	}

	return 0;
}

void CToolEditWnd::OnSize(UINT nType, int cx, int cy)
{
	RECT rc;
	this->GetClientRect(&rc);

	m_wndToolEditFrame.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER);

	CDockablePane::OnSize(nType, cx, cy);
}