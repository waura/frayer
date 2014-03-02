#include "stdafx.h"

#include "BrushSelectWnd.h"

#define BRUSH_GROUP_COMBO_H 20
#define BRUSH_GROUP_COMBO_DROP_H 80
#define BRUSH_GROUP_COMBO_V_MARGIN 2
#define BRUSH_GROUP_COMBO_MAX_W 160

#define BRUSH_GROUP_COMBO_LABEL_W 80
#define BRUSH_GROUP_COMBO_LABEL_H (BRUSH_GROUP_COMBO_H + BRUSH_GROUP_COMBO_V_MARGIN*2)

#define BRUSH_PREVIEW_H 50

#define BRUSH_GROUP_COMBO_ID 1
#define BRUSH_SELECT_FRAME_ID 2
#define BRUSH_PREVIEW_FRAME_ID 3

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern ImgEdit g_ImgEdit;


CBrushSelectWnd::CBrushSelectWnd()
{
}

CBrushSelectWnd::~CBrushSelectWnd()
{
	_ASSERTE(_CrtCheckMemory());
}

void CBrushSelectWnd::OnChangeSelectBrushGroup(
	IEBrushGroup_Ptr pNewBrushGroup,
	IEBrushGroup_Ptr pOldBrushGroup)
{
	m_BrushSelectFrame.SetBrushGroup( pNewBrushGroup );
	m_BrushPreviewFrame.SetBrushGroup( pNewBrushGroup );
}

void CBrushSelectWnd::OnAddBrushGrup(IEBrushGroup_Ptr pBGroup)
{
	UpdateBrushGroupComboBox();
}

void CBrushSelectWnd::OnDeleteBrushGroup(IEBrushGroup_Ptr pBGroup)
{
	UpdateBrushGroupComboBox();
}

BEGIN_MESSAGE_MAP(CBrushSelectWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(BRUSH_GROUP_COMBO_ID, OnSelChangeBrushGroupCombo)
	ON_MESSAGE(WM_APP_MOUSEWHEEL, CBrushSelectWnd::OnAppMouseWheel)
END_MESSAGE_MAP()

int CBrushSelectWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CDockablePane::OnCreate(lpCreateStruct)){
		return -1;
	}

	if(!m_Font.CreateFont(
		12,
		0,
		0,
		0,
		FW_NORMAL,
		FALSE,
		FALSE,
		0,
		SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FF_DONTCARE | DEFAULT_PITCH,
		"MS ゴシック")){
			return -1;
	}

	RECT rc;
	//create brush group combo box
	rc.top = BRUSH_GROUP_COMBO_V_MARGIN;
	rc.left = 0;
	rc.bottom = BRUSH_GROUP_COMBO_DROP_H;
	rc.right = lpCreateStruct->cx;
	if(!m_BrushGroupCmb.Create(
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
		rc,
		this,
		BRUSH_GROUP_COMBO_ID))
	{
		return -1;
	}

	//create brush select frame
	rc.top = BRUSH_GROUP_COMBO_H + BRUSH_GROUP_COMBO_V_MARGIN*2;
	rc.left = 0;
	rc.bottom = lpCreateStruct->cy - BRUSH_PREVIEW_H;
	rc.right = lpCreateStruct->cx;
	if(!m_BrushSelectFrame.Create(
		this,
		rc,
		WS_CHILD | WS_CLIPCHILDREN | WS_BORDER | WS_VISIBLE,
		BRUSH_SELECT_FRAME_ID))
	{
		return -1;
	}

	//craete brush preview
	rc.top = rc.bottom;
	rc.left = 0;
	rc.bottom = rc.top + BRUSH_PREVIEW_H;
	rc.right = lpCreateStruct->cx;
	if(!m_BrushPreviewFrame.Create(
		NULL,
		"",
		WS_CHILD | WS_CLIPCHILDREN | WS_BORDER | WS_VISIBLE,
		rc,
		this,
		BRUSH_PREVIEW_FRAME_ID))
	{
		return -1;
	}

	g_ImgEdit.AddIEBrushMGEventListener( this );
	//
	UpdateBrushGroupComboBox();
	m_BrushGroupCmb.SetCurSel( 0 );
	if(g_ImgEdit.GetBrushSize() > 0){
		g_ImgEdit.SetSelectBrushGroup( 0 );	
	}

	return 0;
}

void CBrushSelectWnd::OnDestroy()
{
	g_ImgEdit.DeleteIEBrushMGEventListener( this );
	CDockablePane::OnDestroy();
}

void CBrushSelectWnd::OnSize(UINT nType, int cx, int cy)
{
	RECT rc;
	this->GetClientRect(&rc);

	int combo_w = BRUSH_GROUP_COMBO_MAX_W;
	if(cx < (BRUSH_GROUP_COMBO_LABEL_W + BRUSH_GROUP_COMBO_MAX_W)){
		combo_w = cx - BRUSH_GROUP_COMBO_LABEL_W;
	}
	
	m_BrushGroupCmb.SetWindowPos(
		NULL,
		cx - combo_w,
		BRUSH_GROUP_COMBO_V_MARGIN,
		combo_w,
		BRUSH_GROUP_COMBO_H,
		SWP_NOZORDER);

	int select_frame_y = BRUSH_GROUP_COMBO_H + BRUSH_GROUP_COMBO_V_MARGIN*2 + 4;
	m_BrushSelectFrame.SetWindowPos(
		NULL,
		0,
		select_frame_y,
		cx,
		cy - BRUSH_PREVIEW_H - select_frame_y,
		SWP_NOZORDER);

	m_BrushPreviewFrame.SetWindowPos(
		NULL,
		0,
		cy - BRUSH_PREVIEW_H,
		cx,
		BRUSH_PREVIEW_H,
		SWP_NOZORDER);

	CDockablePane::OnSize(nType, cx, cy);
}

void CBrushSelectWnd::OnPaint()
{
	CPaintDC dc(this); // 描画のデバイス コンテキスト

	CRect rectWnd;
	GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);

	rectWnd.InflateRect(1, 1);
	dc.FillSolidRect(rectWnd, ::GetSysColor(COLOR_WINDOW));

	CFont* old_font = dc.SelectObject( &m_Font );

	RECT rc;
	rc.left = 4; rc.top = 8;
	rc.right = BRUSH_GROUP_COMBO_LABEL_W;
	rc.bottom = BRUSH_GROUP_COMBO_LABEL_H;
	TCHAR txt[] = "レイヤーセット:";
	dc.DrawText(txt, strlen(txt), &rc, DT_CENTER);

	dc.SelectObject( old_font );
}

void CBrushSelectWnd::OnSelChangeBrushGroupCombo()
{
	int nIndex = m_BrushGroupCmb.GetCurSel();
	if(nIndex != CB_ERR){
		g_ImgEdit.SetSelectBrushGroup( nIndex );
	}
}

LRESULT CBrushSelectWnd::OnAppMouseWheel(WPARAM wParam, LPARAM lParam)
{
	UINT nFlags = GET_KEYSTATE_WPARAM(wParam);
	short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	CPoint pt;
	pt.x = GET_X_LPARAM(lParam); 
	pt.y = GET_Y_LPARAM(lParam);
	m_BrushSelectFrame.DoMouseWheel(nFlags, zDelta, pt);
	return TRUE;
}

void CBrushSelectWnd::UpdateBrushGroupComboBox()
{
	// Delete all item
	for(int i = m_BrushGroupCmb.GetCount()-1; i >= 0; i--){
		m_BrushGroupCmb.DeleteString( i );
	}

	g_ImgEdit.SetBrushGroupComboBox( m_BrushGroupCmb.GetSafeHwnd() );
}