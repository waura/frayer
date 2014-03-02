#include "stdafx.h"
#include "BrushBaseParamTabDialog.h"
#include "IEEMBrushEditDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CBrushBaseParamTabDialog, CDialog)

CBrushBaseParamTabDialog::CBrushBaseParamTabDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CBrushBaseParamTabDialog::IDD, pParent)
{
	m_pEditBrush = NULL;
}

CBrushBaseParamTabDialog::~CBrushBaseParamTabDialog()
{
}

void CBrushBaseParamTabDialog::Cancel()
{
	assert(m_pEditBrush);

	m_pEditBrush->SetOrgRadius(m_prev_org_rad);
	m_pEditBrush->SetRadiusCtrlID(m_prev_rad_ctrl_id);
	m_pEditBrush->SetAlphaCtrlID(m_prev_alpha_ctrl_id);
	m_pEditBrush->SetHardness(m_prev_hardness);
	m_pEditBrush->SetMinRadius(m_prev_min_rad);
	m_pEditBrush->SetMinAlpha(m_prev_min_alpha);
}

void CBrushBaseParamTabDialog::PostNcDestroy()
{
}

void CBrushBaseParamTabDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CBrushBaseParamTabDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CEdit* pEdit;
	CComboBox* pCombo;
	CSliderCtrl* pSld;

	//base radius
	m_prev_org_rad = m_pEditBrush->GetOrgRadius();
	pSld = (CSliderCtrl*) GetDlgItem(IDC_BASE_RAD_SLD);
	if (pSld) {
		pSld->SetRange(
			IEBrush::MIN_RADIUS,
			IEBrush::MAX_RADIUS);
		pSld->SetPos( m_prev_org_rad );
	}
	else {
		::OutputDebugString("CBrushBaseParamTabDialog::OnInitDialog(), not found IDC_BASE_RAD_SLD\n");
	}
	pEdit = (CEdit*) GetDlgItem(IDC_BASE_RAD_EDIT);
	if (pEdit) {
		TCHAR txt[256];
		wsprintf(txt, "%d", m_prev_org_rad);
		pEdit->SetWindowText(txt);
	}
	else {
		::OutputDebugString("CBrushBaseParamTabDialog::OnInitDialog(), not found IDC_BASE_RAD_EDIT\n");
	}
	m_prev_rad_ctrl_id = m_pEditBrush->GetRadiusCtrlID();
	pCombo = (CComboBox*) GetDlgItem(IDC_RAD_CTRL_COMBO);
	if (pCombo) {
		pCombo->InsertString(IE_BRUSH_CTRL_ID::OFF, "ƒIƒt");
		pCombo->InsertString(IE_BRUSH_CTRL_ID::PRESSURE, "•Mˆ³");
		pCombo->InsertString(IE_BRUSH_CTRL_ID::AZIMUTH, "‰ñ“]");
		pCombo->InsertString(IE_BRUSH_CTRL_ID::ALTITUDE, "ŒX‚«");
		pCombo->InsertString(IE_BRUSH_CTRL_ID::RANDOM, "ƒ‰ƒ“ƒ_ƒ€");
		pCombo->SetCurSel(m_prev_rad_ctrl_id);
	}

	//alpha
	m_prev_alpha_ctrl_id = m_pEditBrush->GetAlphaCtrlID();
	pCombo = (CComboBox*) GetDlgItem(IDC_ALPHA_CTRL_COMBO);
	if (pCombo) {
		pCombo->InsertString(IE_BRUSH_CTRL_ID::OFF, "ƒIƒt");
		pCombo->InsertString(IE_BRUSH_CTRL_ID::PRESSURE, "•Mˆ³");
		pCombo->InsertString(IE_BRUSH_CTRL_ID::AZIMUTH, "‰ñ“]");
		pCombo->InsertString(IE_BRUSH_CTRL_ID::ALTITUDE, "ŒX‚«");
		pCombo->InsertString(IE_BRUSH_CTRL_ID::RANDOM, "ƒ‰ƒ“ƒ_ƒ€");
		pCombo->SetCurSel(m_prev_alpha_ctrl_id);
	}

	//hardness
	m_prev_hardness = m_pEditBrush->GetHardness();
	pSld = (CSliderCtrl*) GetDlgItem(IDC_HARDNESS_SLD);
	if (pSld) {
		pSld->SetRange(0, 100);
		pSld->SetPos( m_prev_hardness );
	}
	else {
		::OutputDebugString("CBrushBaseParamTabDialog::OnInitDialog(), not found IDC_HARDNESS_SLD\n");
	}
	pEdit = (CEdit*) GetDlgItem(IDC_HARDNESS_EDIT);
	if (pEdit) {
		TCHAR txt[256];
		wsprintf(txt, "%d", m_prev_hardness);
		pEdit->SetWindowText(txt);
	}
	else {
		::OutputDebugString("CBrushBaseParamTabDialog::OnInitDialog(), not found IDC_HARDNESS_EDIT\n");
	}

	//min radius
	m_prev_min_rad = m_pEditBrush->GetMinRadius();
	pSld = (CSliderCtrl*) GetDlgItem(IDC_MIN_RAD_SLD);
	if (pSld) {
		pSld->SetRange(0, 100);
		pSld->SetPos( m_prev_min_rad );
	}
	else {
		::OutputDebugString("CBrushBaseParamTabDialog::OnInitDialog(), not found IDC_MIN_RAD_SLD\n");
	}
	pEdit = (CEdit*) GetDlgItem(IDC_MIN_RAD_EDIT);
	if (pEdit) {
		TCHAR txt[256];
		wsprintf(txt, "%d", m_prev_min_rad);
		pEdit->SetWindowText(txt);
	}
	else {
		::OutputDebugString("CBrushBaseParamTabDialog::OnInitDialog(), not found IDC_MIN_RAD_EDIT\n");
	}

	//min alpha
	m_prev_min_alpha = m_pEditBrush->GetMinAlpha();
	pSld = (CSliderCtrl*) GetDlgItem(IDC_MIN_ALPHA_SLD);
	if (pSld) {
		pSld->SetRange(0, 100);
		pSld->SetPos( m_prev_min_alpha );
	}
	else {
		::OutputDebugString("CBrushBaseParamTabDialog::OnInitDialog(), not found IDC_MIN_ALPHA_SLD\n");
	}
	pEdit = (CEdit*) GetDlgItem(IDC_MIN_ALPHA_EDIT);
	if (pEdit) {
		TCHAR txt[256];
		wsprintf(txt, "%d", m_prev_min_alpha);
		pEdit->SetWindowText(txt);
	}
	else {
		::OutputDebugString("CBrushBaseParamTabDialog::OnInitDialog(), not found IDC_MIN_ALPHA_EDIT\n");
	}

	//interval
	m_prev_interval = m_pEditBrush->GetInterval();
	pSld = (CSliderCtrl*) GetDlgItem(IDC_INTERVAL_SLD);
	if (pSld) {
		pSld->SetRange(
			IEBrush::MIN_INTERVAL,
			IEBrush::MAX_INTERVAL);
		pSld->SetPos( m_prev_interval );
	}
	else {
		::OutputDebugString("CBrushBaseParamTabDialog::OnInitDialog(), not found IDC_INTERVAL_SLD\n");
	}
	pEdit = (CEdit*) GetDlgItem(IDC_INTERVAL_EDIT);
	if (pEdit) {
		TCHAR txt[256];
		wsprintf(txt, "%d", m_prev_interval);
		pEdit->SetWindowText(txt);
	}
	else {
		::OutputDebugString("CBrushBaseParamTabDialog::OnInitDialog(), not found IDC_INTERVAL_EDIT\n");
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(CBrushBaseParamTabDialog, CDialog)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_RAD_CTRL_COMBO, &CBrushBaseParamTabDialog::OnCbnSelchangeRadCtrlCombo)
	ON_CBN_SELCHANGE(IDC_ALPHA_CTRL_COMBO, &CBrushBaseParamTabDialog::OnCbnSelchangeAlphaCtrlCombo)
END_MESSAGE_MAP()

void CBrushBaseParamTabDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	if ((nSBCode == SB_THUMBPOSITION) || (nSBCode == SB_THUMBTRACK)) {
		CEdit* pEdit = NULL;
		CSliderCtrl* pSld = (CSliderCtrl*) pScrollBar;
		if (pScrollBar->GetDlgCtrlID() == IDC_BASE_RAD_SLD) {
			pEdit = (CEdit*) GetDlgItem(IDC_BASE_RAD_EDIT);

			TCHAR txt[256];
			wsprintf(txt, "%d", nPos);
			pEdit->SetWindowText(txt);
		}
		else {
			switch (pScrollBar->GetDlgCtrlID()) {
				case IDC_HARDNESS_SLD:
					pEdit = (CEdit*) GetDlgItem(IDC_HARDNESS_EDIT);
					break;
				case IDC_MIN_RAD_SLD:
					pEdit = (CEdit*) GetDlgItem(IDC_MIN_RAD_EDIT);
					break;
				case IDC_MIN_ALPHA_SLD:
					pEdit = (CEdit*) GetDlgItem(IDC_MIN_ALPHA_EDIT);
					break;
				case IDC_INTERVAL_SLD:
					pEdit = (CEdit*) GetDlgItem(IDC_INTERVAL_EDIT);
					break;
			}

			if (pEdit) {
				TCHAR txt[256];
				wsprintf(txt, "%d", nPos);
				pEdit->SetWindowText(txt);
			}
		}

		UpdateBrushParam();
		CIEEMBrushEditDialog* pParent = (CIEEMBrushEditDialog*) this->GetParent();
		pParent->UpdateBrushPreview();
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CBrushBaseParamTabDialog::UpdateBrushParam()
{
	CComboBox* pCombo;
	CSliderCtrl* pSld;

	//base radius
	pSld = (CSliderCtrl*) GetDlgItem(IDC_BASE_RAD_SLD);
	if (pSld) {
		m_pEditBrush->SetOrgRadius( pSld->GetPos() );
	}
	else{
		::OutputDebugString("CBrushBaseParamTabDialog::UpdateBrushParam(), not found IDC_BASE_RAD_SLD\n");
	}
	pCombo = (CComboBox*) GetDlgItem(IDC_RAD_CTRL_COMBO);
	if (pCombo) {
		m_pEditBrush->SetRadiusCtrlID( (IE_BRUSH_CTRL_ID)pCombo->GetCurSel() );
	}

	//alpha
	pCombo = (CComboBox*) GetDlgItem(IDC_ALPHA_CTRL_COMBO);
	if (pCombo) {
		m_pEditBrush->SetAlphaCtrlID( (IE_BRUSH_CTRL_ID)pCombo->GetCurSel() );
	}

	//hardness
	pSld = (CSliderCtrl*) GetDlgItem(IDC_HARDNESS_SLD);
	if (pSld) {
		m_pEditBrush->SetHardness( pSld->GetPos() );
	}
	else{
		::OutputDebugString("CBrushBaseParamTabDialog::UpdateBrushParam(), not found IDC_HARDNESS_SLD\n");
	}

	//min radius
	pSld = (CSliderCtrl*) GetDlgItem(IDC_MIN_RAD_SLD);
	if (pSld) {
		m_pEditBrush->SetMinRadius( pSld->GetPos() );
	}
	else {
		::OutputDebugString("CBrushBaseParamTabDialog::UpdateBrushParam(), not found IDC_MIN_RAD_SLD\n");
	}

	//min alpha
	pSld = (CSliderCtrl*) GetDlgItem(IDC_MIN_ALPHA_SLD);
	if (pSld) {
		m_pEditBrush->SetMinAlpha( pSld->GetPos() );
	}
	else {
		::OutputDebugString("CBrushBaseParamTabDialog::UpdateBrushParam(), not found IDC_MIN_ALPHA_SLD\n");
	}

	//interval
	pSld = (CSliderCtrl*) GetDlgItem(IDC_INTERVAL_SLD);
	if (pSld) {
		m_pEditBrush->SetInterval( pSld->GetPos() );
	}
	else {
		::OutputDebugString("CBrushBaseParamTabDialog::UpdateBrushParam(), not found IDC_INTERVAL_SLD\n");
	}
}

void CBrushBaseParamTabDialog::OnCbnSelchangeRadCtrlCombo()
{
	UpdateBrushParam();
	CIEEMBrushEditDialog* pParent = (CIEEMBrushEditDialog*) this->GetParent();
	pParent->UpdateBrushPreview();
}


void CBrushBaseParamTabDialog::OnCbnSelchangeAlphaCtrlCombo()
{
	UpdateBrushParam();
	CIEEMBrushEditDialog* pParent = (CIEEMBrushEditDialog*) this->GetParent();
	pParent->UpdateBrushPreview();
}
