#include "stdafx.h"

#include "BrushSelectFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern ImgEdit g_ImgEdit;

CBrushSelectFrame::CBrushSelectFrame()
{
	m_pSelectBrushPanel = NULL;
	m_pViewBrushGroup = NULL;
	m_panel_x_index = 0;
	m_panel_col = 1;

	//init
	SetScrollSizes(MM_TEXT, CSize(0, 0));
}

CBrushSelectFrame::~CBrushSelectFrame()
{
	//
	if (m_pViewBrushGroup) {
		m_pViewBrushGroup->DeleteEventListener( this );
	}

	//
	BrushPanel_Vec::iterator itr = m_BrushPanel_Vec.begin();
	for (; itr != m_BrushPanel_Vec.end(); itr++) {
		if (*itr) {
			//(*itr)->DestroyWindow();
			delete (*itr);
		}
	}
	m_BrushPanel_Vec.clear();

	_ASSERTE(_CrtCheckMemory());
}

void CBrushSelectFrame::OnChangeSelectBrush(IEBrush_Ptr pNewBrush, IEBrush_Ptr pOldBrush)
{
	LockIEBrushGroupEvent();
	CBrushPanel* pPanel = GetBrushPanelFromIEBrush(pNewBrush);
	ChangeSelectPanel(pPanel);
	UnlockIEBrushGroupEvent();
}

void CBrushSelectFrame::OnAddBrush(IEBrush_Ptr pBrush)
{
	AddBrushPanel( pBrush );
	//スクロールバー調節
	AdjastScrollSizes();
}

void CBrushSelectFrame::OnDeleteBrush(IEBrush_Ptr pBrush)
{
	CBrushPanel* pane = GetBrushPanelFromIEBrush( pBrush );
	if (pane) {
		//unregist tool tip
		m_ToolTip.DelTool(pane);
		//
		DeleteBrushPanel(pane);

		//パネル再配置
		AdjastPanel();
		//スクロールバー調節
		AdjastScrollSizes();
	}
}

void CBrushSelectFrame::SetBrushGroup(IEBrushGroup_Ptr pBGroup)
{
	if (m_pViewBrushGroup) {
		m_pViewBrushGroup->DeleteEventListener( this );
	}
	pBGroup->AddEventListener( this );

	m_pViewBrushGroup = pBGroup;
	m_pSelectBrushPanel = NULL;

	//delete all panel
	BrushPanel_Vec::iterator itr = m_BrushPanel_Vec.begin();
	for(; itr != m_BrushPanel_Vec.end(); itr++){
		if(*itr){
			(*itr)->DestroyWindow();
			delete (*itr);
		}
	}
	m_BrushPanel_Vec.clear();

	//
	int i;
	int brush_size = m_pViewBrushGroup->GetBrushSize();
	for (i=0; i<brush_size; i++) {
		IEBrush_Ptr pBrush = m_pViewBrushGroup->GetBrushAtIndex( i );
		AddBrushPanel( pBrush );
	}

	//set select brush
	IEBrush_Ptr pBrush = m_pViewBrushGroup->GetSelectBrush();
	CBrushPanel* pPanel = GetBrushPanelFromIEBrush(pBrush);

	LockIEBrushGroupEvent();
	ChangeSelectPanel(pPanel);
	UnlockIEBrushGroupEvent();

	AdjastPanel();
}

BOOL CBrushSelectFrame::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent( pMsg );
	return CWnd::PreTranslateMessage( pMsg );
}

BEGIN_MESSAGE_MAP(CBrushSelectFrame, CScrollWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()

int CBrushSelectFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (0 != CScrollWnd::OnCreate(lpCreateStruct)) {
		return -1;
	}

	m_ToolTip.Create(this, TTS_ALWAYSTIP);

	//
	int i;
	int size = g_ImgEdit.GetBrushSize();
	for (i=0; i<size; i++) {
		IEBrush_Ptr pBrush = g_ImgEdit.GetBrushAtIndex(i);
		this->OnAddBrush( pBrush );
	}

	IEBrush_Ptr pBrush = g_ImgEdit.GetSelectBrush();
	OnChangeSelectBrush(pBrush, NULL);

	return 0;
}

void CBrushSelectFrame::OnSize(UINT nType, int cx, int cy)
{
	m_panel_col = cx / BRUSH_PANEL_W;
	m_panel_col = (m_panel_col == 0) ? 1 : m_panel_col;

	//ブラシパネル再配置
	AdjastPanel();
	//スクロールバー調整
	AdjastScrollSizes();

	CScrollWnd::OnSize(nType, cx, cy);
}

void CBrushSelectFrame::OnPaint()
{
	CPaintDC dc(this); // 描画のデバイス コンテキスト

	CRect rectWnd;
	GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);

	rectWnd.InflateRect(1, 1);
	dc.FillSolidRect(rectWnd, ::GetSysColor(COLOR_WINDOW));
}

void CBrushSelectFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	//select brush under mouse 
	CBrushPanel* under_panel = GetBrushPanelFromPos(point);
	if (under_panel) {
		ChangeSelectPanel(under_panel);
	}

	CScrollWnd::OnLButtonDown(nFlags, point);
}

void CBrushSelectFrame::OnMouseMove(UINT nFlags, CPoint point)
{
	CScrollWnd::OnMouseMove(nFlags, point);
}

void CBrushSelectFrame::OnLButtonUp(UINT nFlags, CPoint point)
{
	CScrollWnd::OnLButtonUp(nFlags, point);
}

void CBrushSelectFrame::OnRButtonDown(UINT nFlags, CPoint point)
{
	//select brush under mouse 
	CBrushPanel* under_panel = GetBrushPanelFromPos(point);
	if (under_panel) {
		ChangeSelectPanel(under_panel);
	}

	CScrollWnd::OnRButtonDown(nFlags, point);
}

void CBrushSelectFrame::OnRButtonUp(UINT nFlags, CPoint point)
{
	CScrollWnd::OnRButtonUp(nFlags, point);
}

void CBrushSelectFrame::OnMouseLeave()
{
	this->SendMessage(WM_KILLFOCUS, 0, 0);
	CScrollWnd::OnMouseLeave();
}

//BOOL CBrushSelectFrame::OnCommand(WPARAM wParam, LPARAM lParam)
//{
//	switch(LOWORD(wParam)){
//		case CM_ID_BRUSH_DELETE:
//			if(m_UnderBrushPanel){
//				m_UnderBrushPanel->DeleteBrush();
//				m_UnderBrushPanel = NULL;
//			}
//			return TRUE;
//	}
//
//	return CScrollWnd::OnCommand(wParam, lParam);
//}

void CBrushSelectFrame::PostNcDestroy()
{
	//BrushPanel_Vec::iterator itr = m_BrushPanel_Vec.begin();
	//for(; itr != m_BrushPanel_Vec.end(); itr++){
	//	if(*itr){
	//		(*itr)->DestroyWindow();
	//		//delete (*itr);
	//	}
	//}
	//m_BrushPanel_Vec.clear();

	CWnd::PostNcDestroy();
}

void CBrushSelectFrame::ChangeSelectPanel(CBrushPanel* pPanel)
{
	//選択ブラシを変更する
	if (m_pSelectBrushPanel) {
		//前の選択パネルをニュートラルに
		m_pSelectBrushPanel->NewtralPanel();
		m_pSelectBrushPanel->RePaint(NULL);
	}
	//現在の選択パネルを選択状態に
	m_pSelectBrushPanel = pPanel;
	if (m_pSelectBrushPanel) {
		m_pSelectBrushPanel->SelectPanel();
		m_pSelectBrushPanel->RePaint(NULL);
	}
}

void CBrushSelectFrame::AddBrushPanel(IEBrush_Ptr pBrush)
{
	int size = m_BrushPanel_Vec.size();

	RECT rc;
	rc.top  = (size/m_panel_col) * BRUSH_PANEL_H;
	rc.left = m_panel_x_index * BRUSH_PANEL_W;
	rc.bottom = rc.top + BRUSH_PANEL_H;
	rc.right  = rc.left + BRUSH_PANEL_W;
	
	//パネルの作成
	CBrushPanel *p = new CBrushPanel();
	p->CreateEx(0, 0, "",
		WS_CHILD | WS_CLIPCHILDREN | WS_BORDER | WS_VISIBLE,
		rc,
		this,
		m_panel_x_index);
	p->SetBrush( pBrush );
	m_BrushPanel_Vec.push_back(p);

	//regist to tool tip
	char brush_name[MAX_IE_BRUSH_NAME];
	pBrush->GetName(brush_name);
	m_ToolTip.AddTool(p, brush_name);

	m_panel_x_index++;
	if (m_panel_x_index >= m_panel_col) {
		m_panel_x_index = 0;
	}

	//再描画
	this->InvalidateRect(&rc, FALSE);
}

//////////////////////////////////////
/*!
	スクロールバーサイズ調節
*/
void CBrushSelectFrame::AdjastScrollSizes()
{
	size_t size = m_BrushPanel_Vec.size();

	CClientDC dc(NULL);
	OnPrepareDC(&dc);
	CSize sizeDoc;
	sizeDoc.cx = m_panel_col * BRUSH_PANEL_W;
	sizeDoc.cy = (size/m_panel_col + 1) * BRUSH_PANEL_H;
	dc.LPtoDP(&sizeDoc);
	SetScrollSizes(MM_TEXT, sizeDoc);
}

////////////////////////////////////////////////////
/*!
	パネルを再配置する。
*/
void CBrushSelectFrame::AdjastPanel()
{
	//スクロール位置を取得
	CPoint cpt = GetScrollPosition();

	int brush_cnt=0;
	m_panel_x_index = 0;

	BrushPanel_Vec::iterator itr = m_BrushPanel_Vec.begin();
	for (; itr != m_BrushPanel_Vec.end(); itr++, brush_cnt++) {
		(*itr)->SetWindowPos(
			NULL,
			m_panel_x_index * BRUSH_PANEL_W,
			(brush_cnt/m_panel_col) * BRUSH_PANEL_H - cpt.y,
			BRUSH_PANEL_W,
			BRUSH_PANEL_H,
			SWP_NOZORDER | SWP_NOSIZE);
		
		m_panel_x_index++;
		if (m_panel_x_index >= m_panel_col) {
			m_panel_x_index = 0;
		}
	}
}

////////////////////////////////////////////////////
/*!
	マウスの下にあるパネルのポインタを返す。
	@param[in] point クライアントマウス座標
	@return レイヤーパネルのポインタ
*/
CBrushPanel* CBrushSelectFrame::GetBrushPanelFromPos(CPoint point)
{
	RECT rc;
	POINT lt, rb;
	BrushPanel_Vec::iterator itr = m_BrushPanel_Vec.begin();
	for (; itr != m_BrushPanel_Vec.end(); itr++) {
		//パネルの位置を求める
		(*itr)->GetWindowRect(&rc);
		lt.x = rc.left; lt.y = rc.top;
		rb.x = rc.right; rb.y = rc.bottom;
		ScreenToClient(&lt);
		ScreenToClient(&rb);
		if ((lt.x <= point.x && point.x <= rb.x) &&
			(lt.y <= point.y && point.y <= rb.y))
		{
			return (*itr);
		}
	}
	return NULL;
}

/////////////////////////////////////////////////////
/*!
	IEBrushポインタを保持しているパネルのポインタを返す。
	@param[in] pBrush ブラシポインタ
	@return ブラシパネルのポインタ
*/
CBrushPanel* CBrushSelectFrame::GetBrushPanelFromIEBrush(const IEBrush_Ptr pBrush)
{
	BrushPanel_Vec::iterator itr = m_BrushPanel_Vec.begin();
	for (; itr != m_BrushPanel_Vec.end(); itr++) {
		if (pBrush == (*itr)->GetBrush()) {
			return (*itr);
		}
	}
	return NULL;
}

///////////////////////////////////////////////////////
/*!
	パネル配列からの除去しブラシパネルを削除する
*/
void CBrushSelectFrame::DeleteBrushPanel(CBrushPanel *pPanel)
{
	if (!pPanel) return;

	if (m_pSelectBrushPanel == pPanel) m_pSelectBrushPanel = NULL;

	BrushPanel_Vec::iterator itr = m_BrushPanel_Vec.begin();
	for (; itr != m_BrushPanel_Vec.end(); itr++) {
		if (pPanel == (*itr)) {
			m_BrushPanel_Vec.erase(itr);
			break;
		}
	}
	
	pPanel->DestroyWindow();
}