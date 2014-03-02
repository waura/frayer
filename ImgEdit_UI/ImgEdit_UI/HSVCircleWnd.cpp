#include "stdafx.h"

#include "HSVCircleWnd.h"

#define CS_COLOR_CIRCLE_ID 1

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CHSVCircleWnd::CHSVCircleWnd()
{
}

CHSVCircleWnd::~CHSVCircleWnd()
{
	_ASSERTE(_CrtCheckMemory());
}

BEGIN_MESSAGE_MAP(CHSVCircleWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()


int CHSVCircleWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CDockablePane::OnCreate(lpCreateStruct)){
		return -1;
	}

	RECT rc;
	rc.top = 0; rc.left = 0;
	rc.bottom = 0; rc.right = 0;

	if (!m_HSVCircleFrame.CreateEx(
		0, 0, "",
		WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
		rc,
		this,
		CS_COLOR_CIRCLE_ID))
	{
		return -1;
	}

	return 0;
}

void CHSVCircleWnd::OnSize(UINT nType, int cx, int cy)
{
	m_HSVCircleFrame.SetWindowPos(
		NULL,
		0, 0,
		cx, cy,
		SWP_NOZORDER);

	CDockablePane::OnSize(nType, cx, cy);
}

void CHSVCircleWnd::OnPaint()
{
	CPaintDC dc(this);

	CRect rectWnd;
	GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);

	rectWnd.InflateRect(1, 1);
	dc.FillSolidRect(rectWnd, ::GetSysColor(COLOR_WINDOW));
}