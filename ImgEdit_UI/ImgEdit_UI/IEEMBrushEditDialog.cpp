// IEEMBrushEditDialog.cpp : 実装ファイル
//

#include "stdafx.h"
#include "ImgEdit_UI.h"
#include "IEEMBrushEditDialog.h"

#define PREV_IMG_X 25
#define PREV_IMG_Y 30
#define PREV_IMG_WIDTH 360
#define PREV_IMG_HEIGHT 64

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CIEEMBrushEditDialog ダイアログ

IMPLEMENT_DYNAMIC(CIEEMBrushEditDialog, CDialog)

CIEEMBrushEditDialog::CIEEMBrushEditDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CIEEMBrushEditDialog::IDD, pParent)
{

}

CIEEMBrushEditDialog::~CIEEMBrushEditDialog()
{
}

void CIEEMBrushEditDialog::UpdateBrushPreview()
{
	RECT rc;
	rc.left = PREV_IMG_X;
	rc.top = PREV_IMG_Y;
	rc.right = rc.left + PREV_IMG_WIDTH;
	rc.bottom = rc.top + PREV_IMG_HEIGHT;

	this->InvalidateRect(&rc, FALSE);
}

void CIEEMBrushEditDialog::PostNcDestroy()
{
	delete this;
}

void CIEEMBrushEditDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PARAM_GROUP_TAB, m_ParamGroupTab);
}

BOOL CIEEMBrushEditDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ParamGroupTab.InsertItem(0, _T("基本"));
	m_ParamGroupTab.InsertItem(1, _T("テクスチャ"));

	RECT rect;
	m_ParamGroupTab.GetClientRect(&rect);
	m_ParamGroupTab.AdjustRect(FALSE, &rect);
	m_ParamGroupTab.ClientToScreen(&rect);
	this->ScreenToClient(&rect);

	BOOL ret;

	//tab 1
	m_BrushBaseParamTabDialog.SetEditBrush(m_pEditBrush);
	ret = m_BrushBaseParamTabDialog.Create(this);
	m_BrushBaseParamTabDialog.MoveWindow(&rect, FALSE);

	//tab 2
	m_BrushTextureParamTabDialog.SetEditBrush(m_pEditBrush);
	ret = m_BrushTextureParamTabDialog.Create(this);
	m_BrushTextureParamTabDialog.MoveWindow(&rect, FALSE);

	RECT rc;
	m_BrushBaseParamTabDialog.GetWindowRect(&rc);
	m_BrushTextureParamTabDialog.GetWindowRect(&rc);

	OnTcnSelchangeParamGroupTab(0, 0);

	return TRUE;
}

void CIEEMBrushEditDialog::OnOK()
{
	CDialog::OnOK();
}

void CIEEMBrushEditDialog::OnCancel()
{
	m_BrushBaseParamTabDialog.Cancel();
	m_BrushTextureParamTabDialog.Cancel();

	CDialog::OnCancel();
}

BEGIN_MESSAGE_MAP(CIEEMBrushEditDialog, CDialog)
	ON_WM_PAINT()
	ON_NOTIFY(TCN_SELCHANGE, IDC_PARAM_GROUP_TAB, &CIEEMBrushEditDialog::OnTcnSelchangeParamGroupTab)
END_MESSAGE_MAP()

void CIEEMBrushEditDialog::OnPaint()
{
	CPaintDC dc(this);

	IplImage* prev_img = cvCreateImage(cvSize(PREV_IMG_WIDTH, PREV_IMG_HEIGHT), IPL_DEPTH_8U, 3);
	cvSet(prev_img, cvScalar(255,255,255));

	m_pEditBrush->DrawBrushPreview(prev_img);
	IPLImageToDevice(
		dc.GetSafeHdc(),
		PREV_IMG_X,
		PREV_IMG_Y,
		PREV_IMG_WIDTH,
		PREV_IMG_HEIGHT,
		prev_img,
		0, 0);

	cvReleaseImage(&prev_img);
}

void CIEEMBrushEditDialog::OnTcnSelchangeParamGroupTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int sel = m_ParamGroupTab.GetCurSel();
	m_BrushBaseParamTabDialog.ShowWindow((sel == 0) ? SW_SHOW : SW_HIDE);
	m_BrushTextureParamTabDialog.ShowWindow((sel == 1) ? SW_SHOW : SW_HIDE);

	if (pResult) {
		*pResult = 0;
	}
}
