#include "stdafx.h"

#include "ToolWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CToolWnd::CToolWnd()
{
}

CToolWnd::~CToolWnd()
{
}

BEGIN_MESSAGE_MAP(CToolWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

int CToolWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CDockablePane::OnCreate(lpCreateStruct)){
		return -1;
	}

	RECT rc;
	rc.left = rc.top = 0;
	rc.right = rc.bottom = 0;
	m_ToolFrame.CreateEx(
		0, 0, "",
		WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
		rc,
		this,
		1);

	return 0;
}

void CToolWnd::OnSize(UINT nType, int cx, int cy)
{
	m_ToolFrame.SetWindowPos(
		NULL,
		0, 0,
		cx, cy,
		SWP_NOZORDER);
	return CDockablePane::OnSize(nType, cx, cy);
}

void CToolWnd::OnPaint()
{
	TRY
	{
		CPaintDC dc(this);

		CRect rectWnd;
		GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);

		rectWnd.InflateRect(1, 1);
		dc.FillSolidRect(rectWnd, ::GetSysColor(COLOR_WINDOW));
		
		CDockablePane::OnPaint();
	}
	CATCH(CException, e)
	{
		::OutputDebugString("CToolWnd::OnPaint() exception\n");
	}
	END_CATCH
}