// BrushTextureParamTabDialog.cpp : 実装ファイル
//

#include "stdafx.h"
#include "ImgEdit_UI.h"
#include "BrushTextureParamTabDialog.h"
#include "IEEMBrushEditDialog.h"
#include "afxdialogex.h"

#define PREV_IMG_X 38
#define PREV_IMG_Y 95
#define PREV_IMG_WIDTH 256
#define PREV_IMG_HEIGHT 256

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern ImgEdit g_ImgEdit;

// CBrushTextureParamTabDialog ダイアログ

IMPLEMENT_DYNAMIC(CBrushTextureParamTabDialog, CDialog)

CBrushTextureParamTabDialog::CBrushTextureParamTabDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CBrushTextureParamTabDialog::IDD, pParent)
{
	m_pEditBrush = NULL;
}

CBrushTextureParamTabDialog::~CBrushTextureParamTabDialog()
{
}

void CBrushTextureParamTabDialog::Cancel()
{
	if (m_prev_is_valid_texture) {
		m_pEditBrush->AddTexture(m_prev_texture_path);
	} else {
		m_pEditBrush->DeleteTexture();
	}
}

void CBrushTextureParamTabDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CBrushTextureParamTabDialog::OnInitDialog()
{
	assert(m_pEditBrush);

	if (CDialog::OnInitDialog() == FALSE) {
		return FALSE;
	}

	int combo_cursel = 0;
	m_prev_is_valid_texture = m_pEditBrush->IsValidTexture();
	if (m_prev_is_valid_texture) {
		m_pEditBrush->GetTexturePath(m_prev_texture_path, _MAX_PATH);

		TCHAR drive[_MAX_DRIVE];
		TCHAR dir[_MAX_DIR];
		TCHAR fname[_MAX_FNAME];
		TCHAR ext[_MAX_EXT];
		_splitpath_s(
			m_prev_texture_path,
			drive,
			_MAX_DRIVE,
			dir,
			_MAX_DIR,
			fname,
			_MAX_FNAME,
			ext,
			_MAX_EXT);
		std::string texture_name = fname;
		texture_name += ext;
		combo_cursel = g_ImgEdit.GetBrushTextureIndex(texture_name.c_str());
	}

	CButton* pBtn = (CButton*) GetDlgItem(IDC_IS_USE_BRUSH_TEXTURE_CHECK);
	if (pBtn) {
		if (m_prev_is_valid_texture) {
			pBtn->SetCheck(BST_CHECKED);
		} else {
			pBtn->SetCheck(BST_UNCHECKED);
		}
	}

	CComboBox* pFileNameCombo = (CComboBox*) GetDlgItem(IDC_BRUSH_TEXTURE_NAME_COMBO);
	if (pFileNameCombo) {
		g_ImgEdit.SetBrushTextureComboBox(pFileNameCombo->GetSafeHwnd());
		pFileNameCombo->SetCurSel(combo_cursel);
	}

	if (!m_prev_is_valid_texture) {
		pFileNameCombo->EnableWindow(FALSE);
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(CBrushTextureParamTabDialog, CDialog)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_BRUSH_TEXTURE_NAME_COMBO, &CBrushTextureParamTabDialog::OnCbnSelchangeBrushTextureNameCombo)
	ON_BN_CLICKED(IDC_IS_USE_BRUSH_TEXTURE_CHECK, &CBrushTextureParamTabDialog::OnBnClickedIsUseBrushTextureCheck)
END_MESSAGE_MAP()


// CBrushTextureParamTabDialog メッセージ ハンドラー

void CBrushTextureParamTabDialog::OnPaint()
{
	CPaintDC dc(this);

	CButton* pBtn = (CButton*) GetDlgItem(IDC_IS_USE_BRUSH_TEXTURE_CHECK);
	if (pBtn == NULL)
		return;
	if (pBtn->GetCheck() == BST_UNCHECKED)
		return;

	CComboBox* pFileNameCombo = (CComboBox*) GetDlgItem(IDC_BRUSH_TEXTURE_NAME_COMBO);
	if (pFileNameCombo == NULL)
		return;

	int cursel = pFileNameCombo->GetCurSel();
	if (cursel == -1)
		return;

	char path[_MAX_PATH];
	g_ImgEdit.GetBrushTexturePath(path, _MAX_PATH, cursel);

	IplImage* prev_org_img = cvLoadImage(path);
	if (prev_org_img) {
		IplImage* prev_img = cvCreateImage(cvSize(PREV_IMG_WIDTH, PREV_IMG_HEIGHT), IPL_DEPTH_8U, 3);
		cvResize(prev_org_img, prev_img, CV_INTER_NN);
		IPLImageToDevice(
			dc.GetSafeHdc(),
			PREV_IMG_X,
			PREV_IMG_Y,
			PREV_IMG_WIDTH,
			PREV_IMG_HEIGHT,
			prev_img,
			0, 0);
		cvReleaseImage(&prev_img);
		cvReleaseImage(&prev_org_img);
	}
}

void CBrushTextureParamTabDialog::OnCbnSelchangeBrushTextureNameCombo()
{
	CComboBox* pFileNameCombo = (CComboBox*) GetDlgItem(IDC_BRUSH_TEXTURE_NAME_COMBO);
	if (pFileNameCombo) {
		int cursel = pFileNameCombo->GetCurSel();
		if (cursel != -1) {
			char path[_MAX_PATH];
			g_ImgEdit.GetBrushTexturePath(path, _MAX_PATH, cursel);
			m_pEditBrush->AddTexture(path);
		}
	}

	this->Invalidate(TRUE);
	CIEEMBrushEditDialog* pParent = (CIEEMBrushEditDialog*) this->GetParent();
	pParent->UpdateBrushPreview();
}


void CBrushTextureParamTabDialog::OnBnClickedIsUseBrushTextureCheck()
{
	CButton* pBtn = (CButton*) GetDlgItem(IDC_IS_USE_BRUSH_TEXTURE_CHECK);
	if (pBtn) {
		if (pBtn->GetCheck() == BST_UNCHECKED) {
			m_pEditBrush->DeleteTexture();
		} else {
			OnCbnSelchangeBrushTextureNameCombo();
		}

		CComboBox* pFileNameCombo = (CComboBox*) GetDlgItem(IDC_BRUSH_TEXTURE_NAME_COMBO);
		if (pFileNameCombo) {
			pFileNameCombo->EnableWindow(pBtn->GetCheck() == BST_CHECKED);
		}
	}

	this->Invalidate(TRUE);
	CIEEMBrushEditDialog* pParent = (CIEEMBrushEditDialog*) this->GetParent();
	pParent->UpdateBrushPreview();
}
