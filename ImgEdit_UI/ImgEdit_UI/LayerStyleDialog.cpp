// D:\Documents\Visual Studio 2008\Projects\frayer\ImgEdit_UI\ImgEdit_UI\LayerStyleDialog.cpp : 実装ファイル
//

#include "stdafx.h"
#include "ImgEdit_UI.h"
#include "LayerStyleDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLayerStyleDialog ダイアログ

IMPLEMENT_DYNAMIC(CLayerStyleDialog, CDialog)

CLayerStyleDialog::CLayerStyleDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CLayerStyleDialog::IDD, pParent)
{

}

CLayerStyleDialog::~CLayerStyleDialog()
{
}

void CLayerStyleDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLayerStyleDialog, CDialog)
END_MESSAGE_MAP()


// CLayerStyleDialog メッセージ ハンドラ

BOOL CLayerStyleDialog::OnInitDialog()
{
	assert(m_pEditLayer);

	//描画モードのコンボボックスのハンドルを得る
	HWND hCombo = NULL;
	this->GetDlgItem(IDC_DRAW_MODE_COMBO, &hCombo);
	//コンボボックスにアイテムを追加
	LayerSynthesizerMG::SetDrawModeComboBox(hCombo);
	//現在のレイヤーの描画モードを選択状態に
	m_start_blend_mode_index = LayerSynthesizerMG::GetLayerSynthIndex(m_pEditLayer->GetLayerSynthFourCC());
	::SendMessage(hCombo, CB_SETCURSEL, m_start_blend_mode_index, 0);	//
	
	char a_str[4];
	m_start_alpha = m_pEditLayer->GetOpacityPer();
	wsprintf(a_str, "%d", m_start_alpha);
	this->SetDlgItemText(IDC_LS_ALPHA_EDIT, a_str);

	//
	ImgFile_Ptr f = m_pEditLayer->GetParentFile().lock();
	assert(f);

	//create handle to change layer opacity
	m_pChangeOpacityHandle = (ChangeLayerOpacityHandle*) f->CreateImgFileHandle(IFH_CHANGE_LAYER_OPACITY);
	m_pChangeOpacityHandle->SetEditLayer(m_pEditLayer);

	//create handle to change layer blend mode
	m_pChangeFourCCHandle = (ChangeLayerSynthFourCCHandle*) f->CreateImgFileHandle(IFH_CHANGE_LAYER_SYNTH_FOURCC);
	m_pChangeFourCCHandle->SetEditLayer((ImgLayer_Ptr)m_pEditLayer);

	return TRUE;
}

void CLayerStyleDialog::OnOK()
{
	assert(m_pEditLayer);

	//
	ImgFile_Ptr f = m_pEditLayer->GetParentFile().lock();
	assert(f);

	f->DoImgFileHandle(m_pChangeOpacityHandle);
	f->DoImgFileHandle(m_pChangeFourCCHandle);

	this->EndDialog(IDOK);

	CDialog::OnOK();
}

void CLayerStyleDialog::OnCancel()
{
	assert(m_pEditLayer);

	m_pEditLayer->SetOpacity((uint8_t)((m_start_alpha/100.0) * 255));

	ImgFile_Ptr f = m_pEditLayer->GetParentFile().lock();
	assert(f);

	//
	f->ReleaseImgFileHandle(m_pChangeOpacityHandle);
	m_pChangeOpacityHandle = NULL;

	//
	f->ReleaseImgFileHandle(m_pChangeFourCCHandle);
	m_pChangeFourCCHandle = NULL;

	this->EndDialog(IDCANCEL);

	CDialog::OnCancel();
}

BOOL CLayerStyleDialog::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam)){
		case IDC_DRAW_MODE_COMBO:
			if(HIWORD(wParam) == CBN_SELCHANGE){
				//描画モードのコンボボックスのハンドルを得る
				HWND hCombo = NULL;
				this->GetDlgItem(IDC_DRAW_MODE_COMBO, &hCombo);
				//選択されているアイテムのインデックスを得る
				m_blend_mode_index = ::SendMessage(hCombo, CB_GETCURSEL, 0, 0);

				if (m_blend_mode_index != m_start_blend_mode_index) {
					uint32_t fourcc = LayerSynthesizerMG::GetLayerSynthFourCC(m_blend_mode_index);
					m_pChangeFourCCHandle->SetChangeFourCC(fourcc);
					m_pChangeFourCCHandle->Do(m_pEditLayer->GetParentFile());
				}
			}
			return TRUE;
		case IDC_LS_ALPHA_EDIT:
			if(HIWORD(wParam) == EN_CHANGE){
				char str[6];
				this->GetDlgItemText(IDC_LS_ALPHA_EDIT, str, 6);

				int alpha = atoi(str);
				if(alpha < 0 || alpha > 100){
					//不正な入力時はビープを鳴らしてリセット
					::MessageBeep(MB_OK);
					this->SetDlgItemText(IDC_LS_ALPHA_EDIT, m_alpha_txt);
					break;
				}
				strcpy_s(m_alpha_txt, sizeof(char)*4, str);

				//update layer opacity
				int opacity = atoi(m_alpha_txt);
				m_pChangeOpacityHandle->SetChangeOpacity((alpha/100.0) * 255);
				m_pChangeOpacityHandle->Do(m_pEditLayer->GetParentFile());
			}
			return TRUE;
	}
	return CDialog::OnCommand(wParam, lParam);
}