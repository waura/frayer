#include "StdAfx.h"

#include "ToolPopupPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CToolPopupPanel::CToolPopupPanel()
{
	m_pViewToolGroup = NULL;
	m_MouseEnterdCtrl = FALSE;
}

CToolPopupPanel::~CToolPopupPanel()
{
	CToolButton_Vec::iterator itr = m_ToolButton_Vec.begin();
	for(; itr != m_ToolButton_Vec.end(); itr++){
		delete (*itr);
	}
	m_ToolButton_Vec.clear();
}

void CToolPopupPanel::SetIEToolGroup(IEToolGroup *pTGroup)
{
	int size = pTGroup->GetToolSize();
	int i;
	for(i=0; i<size; i++){
		IIETool* pTool = pTGroup->GetToolAtIndex( i );

		CToolButton* pBtn = new CToolButton();
		pBtn->SetButton( pTool );

		m_ToolButton_Vec.push_back( pBtn );
	}
}

int CToolPopupPanel::GetHeight(int width)
{
	m_btn_col = width / TOOL_BUTTON_W;
	m_btn_col = (m_btn_col == 0) ? 1 : m_btn_col;

	int num_of_btn = GetButtonSize();
	int row = num_of_btn / m_btn_col;
	if((num_of_btn % m_btn_col) != 0)
		row++;
	return row * TOOL_BUTTON_H;
}

void CToolPopupPanel::AllButtonNewtral()
{
	CToolButton_Vec::iterator itr = m_ToolButton_Vec.begin();
	for(; itr != m_ToolButton_Vec.end(); itr++){
		(*itr)->OnNewtral();
	}

	this->Invalidate(FALSE);
}

void CToolPopupPanel::SelectToolButton(const IIETool* pTool)
{
	CToolButton_Vec::iterator itr = m_ToolButton_Vec.begin();
	for(; itr != m_ToolButton_Vec.end(); itr++){
		if(pTool == (*itr)->GetTool()){
			(*itr)->OnClick();
		}
	}

	this->Invalidate(FALSE);
}

BOOL CToolPopupPanel::PreTranslateMessage(MSG *pMsg)
{
	m_ToolTip.RelayEvent( pMsg );
	return CWnd::PreTranslateMessage( pMsg );
}

BEGIN_MESSAGE_MAP(CToolPopupPanel, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

int CToolPopupPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CWnd::OnCreate(lpCreateStruct)){
		return -1;
	}

	m_ToolTip.Create( this );
	m_ToolTip.AddTool(this, LPSTR_TEXTCALLBACK);

	return 0;
}

void CToolPopupPanel::OnPaint()
{
	CPaintDC dc(this);

	CRect rectWnd;
	GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);

	rectWnd.InflateRect(1, 1);
	dc.FillSolidRect(rectWnd, ::GetSysColor(COLOR_WINDOW));

	int col=0;
	int x=0;
	int y=0;
	CToolButton_Vec::iterator itr = m_ToolButton_Vec.begin();
	for(; itr != m_ToolButton_Vec.end(); itr++){
		(*itr)->Draw(dc.GetSafeHdc(), x, y);
		col++;

		x += TOOL_BUTTON_W;
		if(col >= m_btn_col){
			col = 0;
			x = 0;
			y += TOOL_BUTTON_H;
		}
	}
}

void CToolPopupPanel::OnLButtonDown(UINT nFlags, CPoint point)
{
	CToolButton* btn = GetToolButtonFromPos( point );
	if( btn ){
		btn->OnClick();
	}
	this->Invalidate (FALSE );

	CWnd::OnLButtonDown(nFlags, point);
}

void CToolPopupPanel::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_MouseEnterdCtrl == FALSE){
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = this->GetSafeHwnd();
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_MouseEnterdCtrl = _TrackMouseEvent( &tme );
	}

	//change all btn newtral
	CToolButton_Vec::iterator itr = m_ToolButton_Vec.begin();
	for(; itr != m_ToolButton_Vec.end(); itr++){
		if(!(*itr)->isClicked()){
			(*itr)->OnNewtral();
		}
	}

	//get tool btn under mouse
	CToolButton* btn = GetToolButtonFromPos( point );
	if( btn ){
		if(!btn->isClicked()){
			btn->OnMouseOn();
		}
		m_tool_tip_txt = btn->GetToolDisplayName();
		m_ToolTip.Update();
	}
	else{
		m_tool_tip_txt = "";
		m_ToolTip.Pop();
	}

	this->Invalidate (FALSE );
	CWnd::OnMouseMove(nFlags, point);
}

void CToolPopupPanel::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonUp(nFlags, point);
}

LRESULT CToolPopupPanel::OnMouseLeave(WPARAM wp, LPARAM lp)
{
	m_MouseEnterdCtrl = FALSE;

	CToolButton_Vec::iterator itr = m_ToolButton_Vec.begin();
	for(; itr != m_ToolButton_Vec.end(); itr++){
		if(!(*itr)->isClicked()){
			(*itr)->OnNewtral();
		}
	}

	m_tool_tip_txt = "";
	m_ToolTip.Pop();

	this->Invalidate (FALSE );

	return FALSE;
}

CToolButton* CToolPopupPanel::GetToolButtonFromPos(CPoint point)
{
	int col = point.x / TOOL_BUTTON_W;
	int row = point.y / TOOL_BUTTON_H;
	int index = row * m_btn_col + col;
	if(0 <= index && index <= m_ToolButton_Vec.size() - 1){
		return m_ToolButton_Vec[index];
	}

	return NULL;
}