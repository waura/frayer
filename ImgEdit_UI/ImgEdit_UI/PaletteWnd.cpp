#include "stdafx.h"

#include "PaletteWnd.h"

#define PALETTE_COMBO_H 20
#define PALETTE_COMBO_DROP_H 80
#define PALETTE_COMBO_V_MARGIN 2
#define PALETTE_COMBO_MAX_W 160

#define PALETTE_CTRL_PANE_W 80
#define PALETTE_CTRL_PANE_H (PALETTE_COMBO_H + PALETTE_COMBO_V_MARGIN*2)

#define PALETTE_COMBO_ID 1
#define PALETTE_CTRL_PANE_ID 2
#define PALETTE_FRAME_ID 3

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern ImgEdit g_ImgEdit;


CPaletteWnd::CPaletteWnd()
{
}

CPaletteWnd::~CPaletteWnd()
{
}

void CPaletteWnd::OnChangeSelectPalette(
	IEPalette_Ptr pNewPalette,
	IEPalette_Ptr pOldPalette)
{
	m_PaletteFrame.SetPalette( pNewPalette );
	m_PaletteCtrlPane.SetPalette( pNewPalette );
}

void CPaletteWnd::OnAddPalette(IEPalette_Ptr pPalette)
{
	UpdatePaletteComboBox();
}

void CPaletteWnd::OnDeletePalette(IEPalette_Ptr pPalette)
{
	UpdatePaletteComboBox();
}

BEGIN_MESSAGE_MAP(CPaletteWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_MOUSEWHEEL()
	ON_CBN_SELCHANGE(PALETTE_COMBO_ID, OnSelChangePaletteGroupCombo)
	ON_NOTIFY(TTN_GETDISPINFO, 0, OnTipsDispInfo)
	ON_MESSAGE(WM_APP_MOUSEWHEEL, CPaletteWnd::OnAppMouseWheel)
END_MESSAGE_MAP()

int CPaletteWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CDockablePane::OnCreate(lpCreateStruct)){
		return -1;
	}

	RECT rc;
	//craete palette group combo box
	rc.top = 0;	rc.left = 0;
	rc.bottom = PALETTE_COMBO_DROP_H; rc.right = 0;
	if(!m_PaletteCmb.Create(
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
		rc,
		this,
		PALETTE_COMBO_ID))
	{
		return -1;
	}

	//create palette ctrl panel
	if(!m_PaletteCtrlPane.CreateEx(
		0, 0, "",
		WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
		rc,
		this,
		PALETTE_CTRL_PANE_ID))
	{
		return -1;
	}

	//create palette frame
	if(!m_PaletteFrame.Create(
		this,
		rc,
		WS_CHILD | WS_CLIPCHILDREN | WS_BORDER | WS_VISIBLE,
		PALETTE_FRAME_ID))
	{
		return -1;
	}

	g_ImgEdit.AddIEPaletteMGEventListener( this );
	//
	UpdatePaletteComboBox();
	m_PaletteCmb.SetCurSel( 0 );
	//if(g_ImgEdit.GetSelectPaletteSize() > 0){
		g_ImgEdit.SetSelectPalette( 0 );
	//}

	return 0;
}

void CPaletteWnd::OnSize(UINT nType, int cx, int cy)
{
	RECT rc;
	this->GetClientRect(&rc);

	int combo_w = PALETTE_COMBO_MAX_W;
	if(cx < (PALETTE_CTRL_PANE_W + PALETTE_COMBO_MAX_W)){
		combo_w = cx - PALETTE_CTRL_PANE_W;
	}

	m_PaletteCmb.SetWindowPos(
		NULL,
		cx - combo_w,
		PALETTE_COMBO_V_MARGIN,
		combo_w,
		PALETTE_COMBO_H,
		SWP_NOZORDER);

	m_PaletteCtrlPane.SetWindowPos(
		NULL,
		0, 0,
		PALETTE_CTRL_PANE_W,
		PALETTE_CTRL_PANE_H,
		SWP_NOZORDER);

	int select_frame_y = PALETTE_COMBO_H + PALETTE_COMBO_V_MARGIN*2 + 4;
	m_PaletteFrame.SetWindowPos(
		NULL,
		0,
		select_frame_y,
		cx,
		cy - select_frame_y,
		SWP_NOZORDER);

	CDockablePane::OnSize(nType, cx, cy);
}

void CPaletteWnd::OnPaint()
{
	CPaintDC dc(this);

	CRect rectWnd;
	GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);

	rectWnd.InflateRect(1, 1);
	dc.FillSolidRect(rectWnd, ::GetSysColor(COLOR_WINDOW));
}

BOOL CPaletteWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	m_PaletteFrame.DoMouseWheel(nFlags, zDelta, pt);
	return TRUE;
}

void CPaletteWnd::OnSelChangePaletteGroupCombo()
{
	int nIndex = m_PaletteCmb.GetCurSel();
	if(nIndex != CB_ERR){
		g_ImgEdit.SetSelectPalette( nIndex );
	}
}

void CPaletteWnd::OnTipsDispInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTTDISPINFO lpttd;

	if(m_PaletteFrame.m_tool_tip_txt != ""){
		lpttd = (LPNMTTDISPINFO)pNMHDR;
		lpttd->lpszText = (LPSTR)(LPCSTR)m_PaletteFrame.m_tool_tip_txt;
	}

	*pResult = 0;
}

LRESULT CPaletteWnd::OnAppMouseWheel(WPARAM wParam, LPARAM lParam)
{
	UINT nFlags = GET_KEYSTATE_WPARAM(wParam);
	short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	CPoint pt;
	pt.x = GET_X_LPARAM(lParam); 
	pt.y = GET_Y_LPARAM(lParam);
	m_PaletteFrame.DoMouseWheel(nFlags, zDelta, pt);
	return TRUE;
}

void CPaletteWnd::UpdatePaletteComboBox()
{
	// Delete all item
	for(int i = m_PaletteCmb.GetCount() - 1; i >= 0; i--){
		m_PaletteCmb.DeleteString( i );
	}

	g_ImgEdit.SetPaletteComboBox( m_PaletteCmb.GetSafeHwnd() );
}