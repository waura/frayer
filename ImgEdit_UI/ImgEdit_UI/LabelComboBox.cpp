#include "stdafx.h"

#include "LabelComboBox.h"

#define ID_LC_LABEL 1
#define ID_LC_COMBO 2

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CLabelComboBox::CreateLabelComboBox(
	int label_width,
	int label_height,
	int combo_width,
	int combo_height,
	const RECT& rc,
	CWnd* parentWnd,
	int id)
{
	m_label_width = label_width;
	m_label_height = label_height;
	m_combo_width = combo_width;
	m_combo_height = combo_height;

	if(!this->Create(
		NULL, "",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_BORDER,
		rc,
		parentWnd,
		id)){
		return FALSE;
	}
	return TRUE;
}

VOID CLabelComboBox::SetSize(
	int label_width, int label_height,
	int combo_width, int combo_height)
{
	m_label_width = label_width;
	m_label_height = label_height;
	m_combo_width = combo_width;
	m_combo_height = combo_height;

	RECT rc;
	this->GetClientRect(&rc);
	int cy = rc.bottom  - rc.top;

	int x=0;
	m_Label.SetWindowPos(NULL, x, (cy - m_label_height)/2,
		m_label_width, m_label_height, SWP_NOZORDER);
	x += m_label_width;

	int combo_edit_height = m_Combo.GetItemHeight(-1);
	m_Combo.SetWindowPos(NULL, x, (cy - combo_edit_height)/2,
		m_combo_width, combo_edit_height, SWP_NOZORDER);
	x += m_combo_width;
}

VOID CLabelComboBox::SetLabel(LPCTSTR lpszString)
{
	m_Label.SetWindowText(lpszString);
}

BEGIN_MESSAGE_MAP(CLabelComboBox, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(ID_LC_COMBO, OnSelChangeCombo)
END_MESSAGE_MAP()

int CLabelComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
		"MS ÉSÉVÉbÉN")){
			return -1;
	}

	RECT rc;

	//add label
	rc.top = 0;
	rc.left = 0;
	rc.right = rc.left + m_label_width;
	rc.bottom = m_label_height;	
	if(!m_Label.Create("",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		SS_LEFT,
		rc, this, ID_LC_LABEL))
	{
		return -1;
	}
	m_Label.SetFont(&m_Font, FALSE);

	//add combobox
	rc.top = 0;
	rc.left = rc.right;
	rc.right = rc.left + m_combo_width;
	rc.bottom = m_combo_height;
	if(!m_Combo.Create(
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		CBS_DROPDOWNLIST,
		rc , this, ID_LC_COMBO))
	{
		return -1;
	}

	return 0;
}

void CLabelComboBox::OnSize(UINT nType, int cx, int cy)
{
	SetSize(m_label_width, m_label_height, m_combo_width, m_combo_height);
	return CWnd::OnSize(nType, cx, cy);
}

void CLabelComboBox::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetWindowRect(rect);
	ScreenToClient(rect);

	rect.InflateRect(1, 1);
	dc.FillSolidRect(rect, RGB(230, 230, 230));
}

void CLabelComboBox::OnSelChangeCombo()
{
	int nIndex = m_Combo.GetCurSel();
	if(nIndex != CB_ERR){
		(*m_change_val) = nIndex;
	}
}