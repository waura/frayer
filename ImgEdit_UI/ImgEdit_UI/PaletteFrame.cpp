#include "stdafx.h"

#include "PaletteFrame.h"

#define COLOR_PANEL_W 20
#define COLOR_PANEL_H 20

#define COLOR_PANEL_H_MARGIN 1
#define COLOR_PANEL_V_MARGIN 1

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern ImgEdit g_ImgEdit;


CPaletteFrame::CPaletteFrame()
{
	m_pViewPalette = NULL;

	m_select_panel_index = IE_PALETTE_NON_SELECT;
	m_panel_col = 1;

	//init
	SetScrollSizes(MM_TEXT, CSize(0, 0));
}

CPaletteFrame::~CPaletteFrame()
{
	PanelImg_List::iterator itr = m_PanelImg_List.begin();
	for (; itr != m_PanelImg_List.end(); itr++) {
		IplImage* tmp = (*itr);
		cvReleaseImage( &tmp );
	}
	m_PanelImg_List.clear();
}

////////////////////////////////////
/*!
	選択色情報変更イベント
*/
void CPaletteFrame::OnChangeSelectColor(int new_select_color_index)
{
	m_select_panel_index = new_select_color_index;

	if (m_select_panel_index != IE_PALETTE_NON_SELECT) {
		const_IEPaletteNode_Ptr pNode = m_pViewPalette->GetColorAtIndex( new_select_color_index );
		g_ImgEdit.SetSelectFGColor( pNode->color );
	}

	this->Invalidate(FALSE);
}

///////////////////////////////////
/*!
	色情報追加イベント
*/
void CPaletteFrame::OnAddColor(IEPaletteNode_Ptr pNode)
{
	UCvPixel32 color1;
	color1.b = 255;
	color1.g = 255;
	color1.r = 255;
	color1.a = 255;

	UCvPixel32 color2;
	color2.b = 100;
	color2.g = 100;
	color2.r = 100;
	color2.a = 255;

	//AddColorPanel( pNode );
	IplImage* img = cvCreateImage(cvSize(COLOR_PANEL_W, COLOR_PANEL_H), IPL_DEPTH_8U, 4);

	RECT rc;
	rc.left = 0; rc.right = img->width;
	rc.top = 0; rc.bottom = img->height;
	CheckerImageRect(img, &color1, &color2, 2, &rc);
	FillAlphaBlendColor(img, 0, 0, img->width, img->height, &(pNode->color));

	m_PanelImg_List.push_back( img );
	//
	AdjastScrollSizes();
	//
	this->Invalidate(FALSE);
}

///////////////////////////////////
/*!
	色情報削除イベント
*/
void CPaletteFrame::OnDeleteColor(int index, IEPaletteNode_Ptr pNode)
{
	ResetAllPanel();
}

void CPaletteFrame::SetPalette(IEPalette_Ptr pPalette)
{
	if( m_pViewPalette ){
		m_pViewPalette->DeleteEventListener( this );
	}
	pPalette->AddEventListener( this );

	m_pViewPalette = pPalette;

	ResetAllPanel();
}

BOOL CPaletteFrame::PreTranslateMessage(MSG *pMsg)
{
	m_ToolTip.RelayEvent( pMsg );
	return CWnd::PreTranslateMessage( pMsg );
}

BEGIN_MESSAGE_MAP(CPaletteFrame, CScrollWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()

int CPaletteFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CScrollWnd::OnCreate(lpCreateStruct)){
		return -1;
	}

	//m_ToolTip.Create(this, TTS_ALWAYSTIP);
	m_ToolTip.Create( this );
	m_ToolTip.AddTool(this, LPSTR_TEXTCALLBACK);

	return 0;
}

void CPaletteFrame::OnSize(UINT nType, int cx, int cy)
{
	m_panel_col = cx / (COLOR_PANEL_W + COLOR_PANEL_H_MARGIN*2);
	m_panel_col = (m_panel_col == 0) ? 1 : m_panel_col;

	//スクロールバー調整
	AdjastScrollSizes();

	CScrollWnd::OnSize(nType, cx, cy);
}

void CPaletteFrame::OnPaint()
{
	CPaintDC dc(this); //

	//スクロール位置を取得
	CPoint cpt = GetScrollPosition();

	CRect rectWnd;
	GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);

	rectWnd.InflateRect(1, 1);
	dc.FillSolidRect(rectWnd, ::GetSysColor(COLOR_WINDOW));

	if (m_select_panel_index != IE_PALETTE_NON_SELECT) {
		int row = m_select_panel_index / m_panel_col;
		int col = m_select_panel_index - (row * m_panel_col);
		RECT rc;
		rc.top = row * (COLOR_PANEL_H + COLOR_PANEL_V_MARGIN*2);
		rc.left = col * (COLOR_PANEL_W + COLOR_PANEL_H_MARGIN*2);
		rc.bottom = rc.top + COLOR_PANEL_H + COLOR_PANEL_V_MARGIN*2;
		rc.right = rc.left + COLOR_PANEL_W + COLOR_PANEL_H_MARGIN*2;

		CBrush brush(RGB(255,0,0));
		dc.FillRect(&rc, &brush);
	}

	int panel_cnt = 0;
	int col_index = 0;
	PanelImg_List::iterator itr = m_PanelImg_List.begin();
	for (; itr != m_PanelImg_List.end(); itr++) {
		IPLImageToDevice(
			dc.GetSafeHdc(),
			(COLOR_PANEL_W + COLOR_PANEL_H_MARGIN*2) * col_index + COLOR_PANEL_H_MARGIN,
			(COLOR_PANEL_H + COLOR_PANEL_V_MARGIN*2) * (panel_cnt/m_panel_col) + COLOR_PANEL_V_MARGIN - cpt.y,
			COLOR_PANEL_W,
			COLOR_PANEL_H,
			(*itr),
			0, 0);


		col_index++;
		if (col_index >= m_panel_col) {
			col_index = 0;
		}

		panel_cnt++;
	}

}

void CPaletteFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	int row = point.y / (COLOR_PANEL_H + COLOR_PANEL_V_MARGIN*2);
	int col = point.x / (COLOR_PANEL_W + COLOR_PANEL_H_MARGIN*2);
	int click_panel_index = row * m_panel_col + col;

	if (m_pViewPalette) {
		m_pViewPalette->SetSelectColor( click_panel_index );
	}

	CScrollWnd::OnLButtonDown(nFlags, point);
}

void CPaletteFrame::OnMouseMove(UINT nFlags, CPoint point)
{
	int row = point.y / (COLOR_PANEL_H + COLOR_PANEL_V_MARGIN*2);
	int col = point.x / (COLOR_PANEL_W + COLOR_PANEL_H_MARGIN*2);
	int panel_index = row * m_panel_col + col;

	if (m_pViewPalette) {
		int palette_size = m_pViewPalette->GetPaletteSize();
		if (0 <= panel_index && panel_index < palette_size) {
			const_IEPaletteNode_Ptr pNode = m_pViewPalette->GetColorAtIndex( panel_index );
			m_tool_tip_txt = pNode->name;
			m_ToolTip.Update();
		}
		else {
			m_tool_tip_txt = "";
			m_ToolTip.Pop();
		}
	}
}

void CPaletteFrame::OnMouseLeave()
{
	this->SendMessage(WM_KILLFOCUS, 0, 0);
}

//////////////////////////////////////
/*!
	スクロールバーサイズ調節
*/
void CPaletteFrame::AdjastScrollSizes()
{
	int size = g_ImgEdit.GetSelectPaletteSize();

	CClientDC dc(NULL);
	OnPrepareDC(&dc);
	CSize sizeDoc;
	sizeDoc.cx = m_panel_col * (COLOR_PANEL_W + COLOR_PANEL_H_MARGIN*2);
	sizeDoc.cy = (size/m_panel_col + 1) * (COLOR_PANEL_H + COLOR_PANEL_V_MARGIN*2);
	dc.LPtoDP(&sizeDoc);
	SetScrollSizes(MM_TEXT, sizeDoc);
}

void CPaletteFrame::ResetAllPanel()
{
	//clear all panel
	PanelImg_List::iterator itr = m_PanelImg_List.begin();
	for (; itr != m_PanelImg_List.end(); itr++) {
		IplImage* tmp = (*itr);
		cvReleaseImage( &tmp );
	}
	m_PanelImg_List.clear();

	//reset panel
	UCvPixel32 color1;
	color1.b = 255;
	color1.g = 255;
	color1.r = 255;
	color1.a = 255;

	UCvPixel32 color2;
	color2.b = 100;
	color2.g = 100;
	color2.r = 100;
	color2.a = 255;

	int palette_size = m_pViewPalette->GetPaletteSize();
	for (int i=0; i<palette_size; i++) {
		const_IEPaletteNode_Ptr pNode = m_pViewPalette->GetColorAtIndex( i );
		IplImage* img = cvCreateImage(cvSize(COLOR_PANEL_W, COLOR_PANEL_H), IPL_DEPTH_8U, 4);

		RECT rc;
		rc.left = 0; rc.right = img->width;
		rc.top = 0; rc.bottom = img->height;
		CheckerImageRect(img, &color1, &color2, 2, &rc);
		FillAlphaBlendColor(img, 0, 0, img->width, img->height, &(pNode->color));

		m_PanelImg_List.push_back( img );
	}

	this->Invalidate(FALSE);
}