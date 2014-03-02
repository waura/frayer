#include "StdAfx.h"

#include "BrushPopupWnd.h"
#include "IEStringHandle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const int IDC_BRUSH_SIZE_LABEL = 1;
static const int IDC_BRUSH_SIZE_SLIDER = 2;
static const int BPOPUP_WIDTH = 200;
static const int BPOPUP_HEIGHT = 200;

CBrushPopupWnd::CBrushPopupWnd()
{
	m_pEditBrush = NULL;
}

CBrushPopupWnd::~CBrushPopupWnd()
{
}

BOOL CBrushPopupWnd::CreatePopupWnd(CWnd* pOwner, UINT nID)
{
	LPCTSTR lpszWndClassName = ::AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW);
	if(this->CreateEx(
		WS_EX_TOOLWINDOW,
		lpszWndClassName,
		"",
		WS_POPUP | WS_VISIBLE,
		CRect(0, 0, BPOPUP_WIDTH, BPOPUP_HEIGHT),
		pOwner,
		nID))
	{
		this->HidePopup();
		return TRUE;
	}
	return FALSE;
}

//////////////////////////////////
/*!
	座標を指定してポップアップを表示
	@param[in] lpt スクリーン座標
*/
void CBrushPopupWnd::ShowPopup()
{
	POINT pt;
	::GetCursorPos(&pt);
	this->MoveWindow(pt.x, pt.y, BPOPUP_WIDTH,BPOPUP_HEIGHT, TRUE);

	int r=1;
	int h=0;
	if(m_pEditBrush){
		r = m_pEditBrush->GetRadius();
		//h = m_pEditBrush->GetHardness();
	}

	//スライダーの範囲を設定
	m_BrushSizeSlider.SetRange(1, 800);

	//スライダーの現在位置を変更
	m_BrushSizeSlider.SetPos( r );

	this->ShowWindow(SW_SHOWNA);
	this->SetActiveWindow();
	this->UpdateWindow();
}

//////////////////////////////////
/*!
	ポップアップを非表示
*/
void CBrushPopupWnd::HidePopup()
{
	this->ShowWindow(SW_HIDE);
	this->UpdateWindow();
}

///////////////////////////////////
/*!
	編集するブラシをセットする。
*/
void CBrushPopupWnd::SetEditBrush(IEBrush_Ptr pBrush)
{
	m_pEditBrush = pBrush;
}

BOOL CBrushPopupWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	//cs.style |= WS_EX_TOOLWINDOW;
	return CWnd::PreCreateWindow(cs);
}

BEGIN_MESSAGE_MAP(CBrushPopupWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_ACTIVATE()
	ON_WM_HSCROLL()
	ON_WM_PAINT()
END_MESSAGE_MAP()


afx_msg int CBrushPopupWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CWnd::OnCreate(lpCreateStruct)){
		return -1;
	}

	RECT rc;
	//ブラシサイズラベル追加
	rc.top = 0;
	rc.left = 0;
	rc.right = lpCreateStruct->cx;
	rc.bottom = 15;
	if(!m_BrushSizeLabel.Create("ブラシサイズ", WS_VISIBLE | WS_CHILD, rc, this, IDC_BRUSH_SIZE_LABEL)){
		return -1;
	}
	m_font.CreateFont(12, 0, 0, 0,
		FW_DONTCARE,
		FALSE,
		FALSE,
		FALSE,
		SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DRAFT_QUALITY,
		DEFAULT_PITCH,
		"ＭＳ ゴシック");
	m_BrushSizeLabel.SetFont(&m_font);
	//ブラシサイズスライダー追加
	rc.top = 15;
	rc.left = 0;
	rc.right = lpCreateStruct->cx;
	rc.bottom = 30;
	if(!m_BrushSizeSlider.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_BORDER | TBS_HORZ, rc , this, IDC_BRUSH_SIZE_SLIDER)){
		return -1;
	}

	return 0;
}

afx_msg void CBrushPopupWnd::OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized)
{
	if(nState == WA_INACTIVE){
		this->HidePopup();
	}

	CWnd::OnActivate(nState, pWndOther, bMinimized);
}

afx_msg void CBrushPopupWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	if(nSBCode == SB_ENDSCROLL){
		CSliderCtrl* pSliderCtrl = (CSliderCtrl*) pScrollBar;
		if(pSliderCtrl == (&m_BrushSizeSlider)){
			int r = m_BrushSizeSlider.GetPos();
			if(m_pEditBrush){
				m_pEditBrush->SetRadius( r );
			}
		}
	}
}

afx_msg void CBrushPopupWnd::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetWindowRect(rect);
	ScreenToClient(rect);

	rect.InflateRect(1, 1);
	dc.FillSolidRect(rect, ::GetSysColor(COLOR_WINDOW));
}
