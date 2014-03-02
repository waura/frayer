#include "stdafx.h"

#include "ToolEditFrame.h"

#define TE_ESC_LABEL_WIDTH 50
#define TE_ESC_LABEL_HEIGHT 12
#define TE_ESC_SLIDER_WIDTH 90
#define TE_ESC_SLIDER_HEIGHT 30
#define TE_ESC_EDITBOX_WIDTH 20
#define TE_ESC_EDITBOX_HEIGHT 12

#define TE_ESC_WIDTH (TE_ESC_LABEL_WIDTH + TE_ESC_SLIDER_WIDTH + TE_ESC_EDITBOX_WIDTH + 84)

#define TE_LC_LABEL_WIDTH 50
#define TE_LC_LABEL_HEIGHT 12
#define TE_LC_COMBO_WIDTH 120
#define TE_LC_COMBO_HEIGHT 260
#define TE_LC_WIDTH (TE_LC_LABEL_WIDTH + TE_LC_COMBO_WIDTH + 30)

#define TE_CTRL_HEIGHT 48

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern ImgEdit g_ImgEdit;

CToolEditFrame::CToolEditFrame()
{
	m_ctrl_count = 0;
	//scrollbar init
	SetScrollSizes(MM_TEXT, CSize(0, 0));
}

CToolEditFrame::~CToolEditFrame()
{
	ClearAllCtrl();
}

void CToolEditFrame::OnDraw(CDC *pDC)
{
}

void CToolEditFrame::OnInitialUpdate()
{
}

VOID CToolEditFrame::ClearAllCtrl()
{
	TE_CTRL_LIST::iterator itr = m_te_ctrl_list.begin();
	for(; itr != m_te_ctrl_list.end(); itr++){
		if((*itr).pWnd){
			//(*itr).pWnd->DestroyWindow();
			delete (*itr).pWnd;
		}
	}
	m_te_ctrl_list.clear();
}

//////////////////////////////////
/*
	トラックバーの追加
	@param[in] name トラックバーの名前
	@param[in] min 最小値
	@param[in] max 最大値
	@param[in] start 開始時の値
	@param[in] change_val 値を変更する変数
*/
VOID CToolEditFrame::AddTrackbar(const char* name, int min, int max, int start, int *change_val)
{
	CEditSliderCtrl* pSlider = new CEditSliderCtrl;

	RECT rc;
	this->GetClientRect(&rc);

	BOOL ret;
	rc.left = 0;
	rc.right = rc.left + TE_ESC_LABEL_WIDTH + TE_ESC_SLIDER_WIDTH +
		TE_ESC_EDITBOX_WIDTH + pSlider->GetUseWidth();
	ret = pSlider->CreateEditSliderCtrl(
		TE_ESC_LABEL_WIDTH, TE_ESC_LABEL_HEIGHT,
		TE_ESC_SLIDER_WIDTH, TE_ESC_SLIDER_HEIGHT,
		TE_ESC_EDITBOX_WIDTH, TE_ESC_EDITBOX_HEIGHT,
		rc,
		this,
		m_te_ctrl_list.size());
	if (ret == FALSE) {
		OutputError::PushLog(LOG_LEVEL::_WARN, "faild create edit slider in tool edit frame");
	}

	pSlider->SetLabel( name );
	pSlider->SetRange(min, max, TRUE);
	pSlider->SetPos( start );
	pSlider->SetChangeVal( change_val );

	TE_CTRL te_ctrl;
	te_ctrl.pWnd = pSlider;
	te_ctrl.width = TE_ESC_WIDTH;
	te_ctrl.height = TE_CTRL_HEIGHT;
	m_te_ctrl_list.push_back(te_ctrl);

	AdjastControll();
}

//////////////////////////////////
/*
	レイヤー合成コンボボックスの追加
	@param[in] name トラックバーの名前
	@param[in] start_index 開始時のインデックス
	@param[in] change_val 値を変更する変数
*/
VOID CToolEditFrame::AddLayerDrawModeComboBox(
	const char* name,
	int start_index,
	int* change_val)
{
	CLabelComboBox* pCombo = new CLabelComboBox;

	RECT rc;
	this->GetClientRect(&rc);

	BOOL ret;
	rc.left = 0;
	rc.right = rc.left + TE_LC_WIDTH;
	ret = pCombo->CreateLabelComboBox(
		TE_LC_LABEL_WIDTH, TE_LC_LABEL_HEIGHT,
		TE_LC_COMBO_WIDTH, TE_LC_COMBO_HEIGHT,
		rc,
		this,
		m_te_ctrl_list.size());
	if (ret == FALSE) {
		OutputError::PushLog(LOG_LEVEL::_WARN, "faild create label combobox in tool edit frame");
	}

	LayerSynthesizerMG::SetDrawModeComboBox(pCombo->GetComboBoxHandle());

	pCombo->SetLabel( name );
	pCombo->SetChangeVal( change_val );
	pCombo->SetCurSel( start_index );

	(*change_val) = start_index;

	TE_CTRL te_ctrl;
	te_ctrl.pWnd = pCombo;
	te_ctrl.width = TE_LC_WIDTH;
	te_ctrl.height = TE_CTRL_HEIGHT;
	m_te_ctrl_list.push_back(te_ctrl);

	AdjastControll();
}

BEGIN_MESSAGE_MAP(CToolEditFrame, CScrollWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

int CToolEditFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CScrollWnd::OnCreate(lpCreateStruct)){
		return -1;
	}

	g_ImgEdit.SetToolEditWndHandle( this );

	return 0;
}

void CToolEditFrame::OnDestroy()
{
	g_ImgEdit.SetToolEditWndHandle( NULL );

	CScrollWnd::OnDestroy();
}

void CToolEditFrame::OnSize(UINT nType, int cx, int cy)
{
	AdjastScrollSizes();
	AdjastControll();
	CScrollWnd::OnSize(nType, cx, cy);
}

void CToolEditFrame::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetWindowRect(rect);
	ScreenToClient(rect);

	rect.InflateRect(1, 1);
	dc.FillSolidRect(rect, ::GetSysColor(COLOR_WINDOW));
}

//////////////////////////////////////
/*!
	スクロールバーサイズ調節
*/
void CToolEditFrame::AdjastScrollSizes()
{
	RECT rc;
	this->GetClientRect(&rc);
	int all_size = rc.bottom - rc.top;

	CClientDC dc(NULL);
	OnPrepareDC(&dc);
	CSize sizeDoc;
	sizeDoc.cx = rc.right - rc.left;
	sizeDoc.cy = all_size;
	dc.LPtoDP(&sizeDoc);
	SetScrollSizes(MM_TEXT, sizeDoc);
}

//////////////////////////////////////
/*!
	コントロールの再配置
*/
void CToolEditFrame::AdjastControll()
{
	RECT rc;
	this->GetClientRect(&rc);

	int start_x=0;
	int start_y=0;
	int client_width = rc.right - rc.left;

	TE_CTRL_LIST::iterator itr = m_te_ctrl_list.begin();
	for (; itr != m_te_ctrl_list.end(); itr++) {
		if (start_x != 0 && (start_x + (*itr).width) > client_width) {
			start_x = 0;
			start_y += TE_CTRL_HEIGHT;
		}

		BOOL ret;
		ret = (*itr).pWnd->SetWindowPos(
			NULL,
			start_x,
			start_y,
			start_x + (*itr).width,
			start_y + TE_CTRL_HEIGHT,
			SWP_NOZORDER | SWP_SHOWWINDOW);
		if (ret == FALSE) {
			OutputError::PushLog(LOG_LEVEL::_WARN, "faild SetWindowPos in tool edit frame");
		}
		
		ret = (*itr).pWnd->ShowWindow(SW_SHOW);
		if (ret == FALSE) {
			OutputError::PushLog(LOG_LEVEL::_WARN, "faild ShowWindow in tool edit frame");
		}
		(*itr).pWnd->UpdateWindow();

		start_x += (*itr).width;
	}

	this->ShowWindow(SW_SHOW);
	this->UpdateWindow();
}