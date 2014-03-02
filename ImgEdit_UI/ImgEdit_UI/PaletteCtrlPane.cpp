#include "stdafx.h"

#include "PaletteCtrlPane.h"

#define ID_NEW_BTN 1
#define ID_DELETE_BTN 2

#define NEW_BTN_X 0
#define NEW_BTN_Y 0
#define NEW_BTN_W 24
#define NEW_BTN_H 24

#define DELETE_BTN_X 25
#define DELETE_BTN_Y 0
#define DELETE_BTN_W 24
#define DELETE_BTN_H 24

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPaletteCtrlPane::CPaletteCtrlPane()
{
}

CPaletteCtrlPane::~CPaletteCtrlPane()
{
}

BOOL CPaletteCtrlPane::PreTranslateMessage(MSG *pMsg)
{
	m_ToolTip.RelayEvent( pMsg );
	return CWnd::PreTranslateMessage( pMsg );
}

BEGIN_MESSAGE_MAP(CPaletteCtrlPane, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_BN_CLICKED(ID_NEW_BTN, OnClickedNewBtn)
	ON_BN_CLICKED(ID_DELETE_BTN, OnClickedDeleteBtn)
END_MESSAGE_MAP()

int CPaletteCtrlPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CWnd::OnCreate( lpCreateStruct )){
		return -1;
	}

	RECT rc;
	rc.top = NEW_BTN_Y;
	rc.left = NEW_BTN_X;
	rc.bottom = rc.top + NEW_BTN_H;
	rc.right = rc.left + NEW_BTN_W;
	if(!m_NewBtn.Create("",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		BS_OWNERDRAW | BS_PUSHBUTTON,
		rc, this, ID_NEW_BTN))
	{
			return -1;
	}
	m_NewBtn.LoadBitmaps(
		IDB_NEW_PALETTE_COLOR_A,
		IDB_NEW_PALETTE_COLOR_B,
		IDB_NEW_PALETTE_COLOR_C);

	rc.top = DELETE_BTN_Y;
	rc.left = DELETE_BTN_X;
	rc.bottom = rc.top + DELETE_BTN_H;
	rc.right = rc.left + DELETE_BTN_W;
	if(!m_DeleteBtn.Create("",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		BS_OWNERDRAW | BS_PUSHBUTTON,
		rc, this, ID_DELETE_BTN))
	{
			return -1;
	}
	m_DeleteBtn.LoadBitmaps(
		IDB_DELETE_PALETTE_COLOR_A,
		IDB_DELETE_PALETTE_COLOR_B,
		IDB_DELETE_PALETTE_COLOR_C);

	//regist tool tip
	m_ToolTip.Create( this );
	m_ToolTip.AddTool(&m_NewBtn, TEXT("F“o˜^"));
	m_ToolTip.AddTool(&m_DeleteBtn, TEXT("Fíœ"));

	return 0;
}

void CPaletteCtrlPane::OnSize(UINT nType, int cx, int cy)
{
	return CWnd::OnSize(nType, cx, cy);
}

void CPaletteCtrlPane::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetWindowRect(rect);
	ScreenToClient(rect);

	rect.InflateRect(1, 1);
	dc.FillSolidRect(rect, RGB(230, 230, 230));
}

void CPaletteCtrlPane::OnClickedNewBtn()
{
	CNewPaletteColorDialog* dlg = new CNewPaletteColorDialog();
	dlg->SetColor( g_ImgEdit.GetSelectFGColor() );
	BOOL ret = dlg->Create( this );
	if(ret == TRUE){
		dlg->ShowWindow( SW_SHOW );
	}
	else{
		::OutputDebugString("faild CIEEMBrushEditDialog::Create()\n");
	}
}

void CPaletteCtrlPane::OnClickedDeleteBtn()
{
	if(m_pViewPalette){
		int index = m_pViewPalette->GetSelectColorIndex();
		m_pViewPalette->DeleteColor( index );
	}
}