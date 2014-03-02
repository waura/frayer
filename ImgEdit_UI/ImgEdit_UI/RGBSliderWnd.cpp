#include "stdafx.h"

#include "RGBSliderWnd.h"

#define CS_COLOR_CIRCLE_ID 1

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CRGBSliderWnd::CRGBSliderWnd()
{
}

CRGBSliderWnd::~CRGBSliderWnd()
{
}

BEGIN_MESSAGE_MAP(CRGBSliderWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()


int CRGBSliderWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CDockablePane::OnCreate(lpCreateStruct)){
		return -1;
	}

	RECT rc;
	rc.top = 0; rc.left = 0;
	rc.bottom = 0; rc.right = 0;

	if (!m_RGBSliderFrame.CreateEx(
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

void CRGBSliderWnd::OnSize(UINT nType, int cx, int cy)
{
	m_RGBSliderFrame.SetWindowPos(
		NULL,
		0, 0,
		cx, cy,
		SWP_NOZORDER);

	CDockablePane::OnSize(nType, cx, cy);
}

void CRGBSliderWnd::OnPaint()
{
	CPaintDC dc(this);

	CRect rectWnd;
	GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);

	rectWnd.InflateRect(1, 1);
	dc.FillSolidRect(rectWnd, ::GetSysColor(COLOR_WINDOW));
}