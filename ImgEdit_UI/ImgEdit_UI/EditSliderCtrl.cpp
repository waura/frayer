#include "stdafx.h"

#include "EditSliderCtrl.h"
#include "../resource1.h"

#define ID_ESC_LABEL 1
#define ID_ESC_SLIDER 2
#define ID_ESC_EDIT 3
#define ID_ESC_LBTN 4
#define ID_ESC_RBTN 5

#define LRBTN_WIDTH 42
#define LRBTN_HEIGHT 26

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CEditSliderCtrl::CreateEditSliderCtrl(
	int label_width,
	int label_height,
	int slider_width,
	int slider_height,
	int editbox_width,
	int editbox_height,
	const RECT& rc,
	CWnd* parentWnd,
	int id)
{
	m_label_width = label_width;
	m_label_height = label_height;
	m_slider_width = slider_width;
	m_slider_height = slider_height;
	m_editbox_width = editbox_width;
	m_editbox_height = editbox_height;

	if(!this->Create(NULL, "", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_BORDER, rc, parentWnd, id)){
		return FALSE;
	}
	return TRUE;
}

VOID CEditSliderCtrl::SetSize(
	int label_width, int label_height,
	int slider_width, int slider_height,
	int editbox_width, int editbox_height)
{
	m_label_width = label_width;
	m_label_height = label_height;
	m_slider_width = slider_width;
	m_slider_height = slider_height;
	m_editbox_width = editbox_width;
	m_editbox_height = editbox_height;

	RECT rc;
	this->GetClientRect(&rc);
	int cy = rc.bottom  - rc.top;

	int x=0;
	m_LeftBtn.SetWindowPos(NULL, x, (cy - LRBTN_HEIGHT)/2,
		LRBTN_WIDTH, LRBTN_HEIGHT, SWP_NOZORDER);
	x += LRBTN_WIDTH;

	m_Label.SetWindowPos(NULL, x, (cy - m_label_height)/2,
		m_label_width, m_label_height, SWP_NOZORDER);
	x += m_label_width;

	m_Slider.SetWindowPos(NULL, x, (cy - m_slider_height)/2,
		m_slider_width, m_slider_height, SWP_NOZORDER);
	x += m_slider_width;

	m_Edit.SetWindowPos(NULL, x, (cy - m_editbox_height)/2,
		m_editbox_width, m_editbox_height, SWP_NOZORDER);
	x += m_editbox_width;

	m_RightBtn.SetWindowPos(NULL, x, (cy - LRBTN_HEIGHT)/2,
		LRBTN_WIDTH, LRBTN_HEIGHT, SWP_NOZORDER);
}

VOID CEditSliderCtrl::SetLabel(LPCTSTR lpszString)
{
	m_Label.SetWindowText(lpszString);
}

VOID CEditSliderCtrl::SetRange(int min, int max, BOOL bRedraw)
{
	m_min = min;
	m_max = max;
	m_Slider.SetRange(min, max, bRedraw);
}

VOID CEditSliderCtrl::SetPos(int nPos)
{
	m_Slider.SetPos(nPos);

	char num_s[128];
	wsprintf(num_s, "%d", nPos);
	m_Edit.SetWindowText(num_s);

	if(m_change_val){
		(*m_change_val) = nPos;
	}

	this->GetOwner()->SendMessage(
		WM_HSCROLL,
		(WPARAM)MAKELONG((WORD)TB_THUMBTRACK, (WORD)nPos),
		(LPARAM)this->GetSafeHwnd());
}

BOOL CEditSliderCtrl::SetTic(int nTic)
{
	return m_Slider.SetTic( nTic );
}

BEGIN_MESSAGE_MAP(CEditSliderCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(ID_ESC_LBTN, OnBnClickedLBtn)
	ON_BN_CLICKED(ID_ESC_RBTN, OnBnClickedRBtn)
END_MESSAGE_MAP()

int CEditSliderCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CWnd::OnCreate(lpCreateStruct)){
		return -1;
	}

	if(!m_Font.CreateFont(
		m_label_height - 2,
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
		"MS ƒSƒVƒbƒN")){
			return -1;
	}

	RECT rc;

	//add left botton
	rc.top = 0;
	rc.left = 0;
	rc.right = LRBTN_WIDTH;
	rc.bottom = LRBTN_HEIGHT;
	if(!m_LeftBtn.Create("",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		BS_OWNERDRAW | BS_PUSHBUTTON,
		rc, this, ID_ESC_LBTN))
	{
		return -1;
	}
	m_LeftBtn.LoadBitmaps(IDB_LEFT_A, IDB_LEFT_B, IDB_LEFT_C);

	//add label
	rc.top = 0;
	rc.left = rc.right;
	rc.right = rc.left + m_label_width;
	rc.bottom = m_label_height;	
	if(!m_Label.Create("",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		SS_LEFT,
		rc, this, ID_ESC_LABEL))
	{
		return -1;
	}
	m_Label.SetFont(&m_Font, FALSE);

	//add slider
	rc.top = 0;
	rc.left = rc.right;
	rc.right = rc.left + m_slider_width;
	rc.bottom = m_slider_height;
	if(!m_Slider.Create(
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		TBS_HORZ,
		rc , this, ID_ESC_SLIDER))
	{
		return -1;
	}

	//add edit box
	rc.top = 0;
	rc.left = rc.right;
	rc.right = rc.left + m_editbox_width;
	rc.bottom = m_editbox_height;
	if(!m_Edit.Create(
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_BORDER |
		ES_LEFT | ES_NUMBER,
		rc, this, ID_ESC_EDIT))
	{
		return -1;
	}
	m_Edit.SetFont(&m_Font, FALSE);

	//add right botton
	rc.top = 0;
	rc.left = rc.right;
	rc.right = rc.left + LRBTN_WIDTH;
	rc.bottom = LRBTN_HEIGHT;
	if(!m_RightBtn.Create("",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		BS_OWNERDRAW | BS_PUSHBUTTON,
		rc, this, ID_ESC_RBTN))
	{
		return -1;
	}
	m_RightBtn.LoadBitmaps(IDB_RIGHT_A, IDB_RIGHT_B, IDB_RIGHT_C);

	return 0;
}

void CEditSliderCtrl::OnSize(UINT nType, int cx, int cy)
{
	SetSize(
		m_label_width, m_label_height,
		m_slider_width, m_slider_height,
		m_editbox_width, m_editbox_height);
	return CWnd::OnSize(nType, cx, cy);
}

void CEditSliderCtrl::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetWindowRect(rect);
	ScreenToClient(rect);

	rect.InflateRect(1, 1);
	dc.FillSolidRect(rect, RGB(230, 230, 230));
}

void CEditSliderCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	if((void*)pScrollBar == (void*)(&m_Slider)){
		if(!IsLockEvent()){
			LockEvent();

			if((nSBCode == SB_THUMBPOSITION) ||
				(nSBCode == SB_THUMBTRACK)){
					this->SetPos(nPos);
			}

			this->GetOwner()->SendMessage(
				WM_HSCROLL,
				(WPARAM)MAKELONG((WORD)nSBCode, (WORD)nPos),
				(LPARAM)this->GetSafeHwnd());

			UnlockEvent();
		}
	}

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CEditSliderCtrl::OnBnClickedLBtn()
{
	if(!IsLockEvent()){
		LockEvent();

		int val = m_Slider.GetPos() - 10;
		if(val < m_min) val = m_min;
		if(val > m_max) val = m_max;

		//change value
		this->SetPos(val);

		UnlockEvent();
	}
}

void CEditSliderCtrl::OnBnClickedRBtn()
{
	if(!IsLockEvent()){
		LockEvent();

		int val = m_Slider.GetPos() + 10;
		if(val < m_min) val = m_min;
		if(val > m_max) val = m_max;

		//change value
		this->SetPos(val);

		UnlockEvent();
	}
}

BOOL CEditSliderCtrl::OnCommand(WPARAM wp, LPARAM lp)
{
	if(ID_ESC_EDIT == LOWORD(wp)){
		if(HIWORD(wp) == EN_UPDATE){
			if(!IsLockEvent()){
				LockEvent();

				char buf[256];
				m_Edit.GetWindowText(buf, 256);
			
				int val = atoi(buf);
			
				if(val < m_min) val = m_min;
				if(val > m_max) val = m_max;

				//change value
				this->SetPos(val);

				UnlockEvent();
				return TRUE;
			}
		}
	}

	return CWnd::OnCommand(wp, lp);
}
