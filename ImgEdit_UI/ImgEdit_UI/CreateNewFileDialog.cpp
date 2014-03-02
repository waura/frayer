// _CreateNewFileDialog.cpp : 実装ファイル
//

#include "stdafx.h"
#include "ImgEdit_UI.h"
#include "CreateNewFileDialog.h"

#define SIZE_INI_FILE_PATH ".\\size_template.ini"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCreateNewFileDialog ダイアログ

IMPLEMENT_DYNAMIC(CCreateNewFileDialog, CDialog)

CCreateNewFileDialog::CCreateNewFileDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateNewFileDialog::IDD, pParent)
{
	m_pImgEdit = NULL;
	m_pCreateFile = NULL;

	//to use keyboard input this dialog
	CImgEdit_UIApp* pApp = (CImgEdit_UIApp*)AfxGetApp();
	pApp->UnhookKeyBoard();
}

CCreateNewFileDialog::~CCreateNewFileDialog()
{
	//reset hook
	CImgEdit_UIApp* pApp = (CImgEdit_UIApp*)AfxGetApp();
	pApp->HookKeyboard();
}

void CCreateNewFileDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCreateNewFileDialog, CDialog)
	ON_BN_CLICKED(IDC_HORIZONTAL_RADIO, &CCreateNewFileDialog::OnBnClickedHorizontalRadio)
	ON_BN_CLICKED(IDC_VERTICAL_RADIO, &CCreateNewFileDialog::OnBnClickedVerticalRadio)
END_MESSAGE_MAP()

// CCreateNewFileDialog メッセージ ハンドラ
BOOL CCreateNewFileDialog::OnInitDialog()
{
	CComboBox* combo = NULL;

	//サイズ単位コンボボックス初期化
	combo = (CComboBox*) this->GetDlgItem(IDC_SIZE_UNIT_COMBO);
	combo->InsertString(0, "pixel");
	combo->InsertString(1, "inch");
	combo->InsertString(2, "mm");
	combo->SetCurSel(0);
	m_size_unit = 0;

	//背景指定コンボボックス初期化
	combo = (CComboBox*) this->GetDlgItem(IDC_BG_COLOR_COMBO);
	combo->InsertString(0, "白");
	combo->InsertString(1, "黒");
	combo->InsertString(2, "透明");
	combo->SetCurSel(0);
	m_select_gb_color = 0;

	//
	CFileFind find;
	if (!find.FindFile(SIZE_INI_FILE_PATH)) {
		//if template size ini file don't exist, create new file
		CreateNewTemplateSizeFile();
	}

	//load template size file
	combo = (CComboBox*) this->GetDlgItem(IDC_FILE_SIZE_TEMPLATE_COMBO);
	int num_of_template = ::GetPrivateProfileInt("general", "num_of_template", 0, SIZE_INI_FILE_PATH);

	int i;
	for (i=0; i<num_of_template; i++) {
		char app_name[_MAX_PATH];
		wsprintf(app_name, "template%d", i);

		char template_name[_MAX_PATH];
		::GetPrivateProfileString(app_name, "name", "", template_name, _MAX_PATH, SIZE_INI_FILE_PATH);

		FILE_SIZE_TEMPLATE fstemp;
		fstemp.width = ::GetPrivateProfileInt(app_name, "width", 1000, SIZE_INI_FILE_PATH);
		fstemp.height = ::GetPrivateProfileInt(app_name, "height", 1000, SIZE_INI_FILE_PATH);
		char unit_name[_MAX_PATH];
		::GetPrivateProfileString(app_name, "unit_name", "", unit_name, _MAX_PATH, SIZE_INI_FILE_PATH);
		if (strcmp(unit_name, "pixel") == 0) {
			fstemp.unit_id = 0;
		} else if (strcmp(unit_name, "inch") == 0) {
			fstemp.unit_id = 1;
		} else if (strcmp(unit_name, "mm") == 0) {
			fstemp.unit_id = 2;
		} else {
			char log[_MAX_PATH];
			wsprintf("faild load size template: [%s] invalid unit name, %s", template_name, unit_name);
			OutputError::PushLog(LOG_LEVEL::_WARN, log);
			continue;
		}
		fstemp.resolution = ::GetPrivateProfileInt(app_name, "resolution", 100, SIZE_INI_FILE_PATH);

		//add tmeplate
		combo->InsertString(i, template_name);
		m_fileSizeTemplate_Vec.push_back(fstemp);
	}
	if (num_of_template > 0) {
		combo->SetCurSel(0);
		SetFileSizeTemplate(0);
	}

	return TRUE;
}

void CCreateNewFileDialog::OnOK()
{
	assert(m_pImgEdit);

	int pixel_width = this->GetImgPixelWidth();
	int pixel_height = this->GetImgPixelHeight();

	UCvPixel bg_color;
	switch( m_select_gb_color ){
		case 0:
			bg_color.b = 255;
			bg_color.g = 255;
			bg_color.r = 255;
			bg_color.a = 255;
			break;
		case 1:
			bg_color.b = 0;
			bg_color.g = 0;
			bg_color.r = 0;
			bg_color.a = 255;
			break;
		case 2:
			bg_color.b = 0;
			bg_color.g = 0;
			bg_color.r = 0;
			bg_color.a = 0;
			break;
	}

	m_pCreateFile = m_pImgEdit->CreateNewFile(pixel_width, pixel_height, &bg_color);

	this->EndDialog(IDOK);
	CDialog::OnOK();
}

void CCreateNewFileDialog::OnCancel()
{
	this->EndDialog(IDCANCEL);
	CDialog::OnOK();
}

BOOL CCreateNewFileDialog::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam)) {
		case IDC_FILE_SIZE_TEMPLATE_COMBO:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				CComboBox* combo = (CComboBox*) this->GetDlgItem(IDC_FILE_SIZE_TEMPLATE_COMBO);
				int index = combo->GetCurSel();
				SetFileSizeTemplate(index);
			}
			return TRUE;
		case IDC_SIZE_UNIT_COMBO:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				CComboBox* combo = (CComboBox*) this->GetDlgItem(IDC_SIZE_UNIT_COMBO);
				m_size_unit = combo->GetCurSel();
			}
			return TRUE;
		case IDC_BG_COLOR_COMBO:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				CComboBox* combo = (CComboBox*) this->GetDlgItem(IDC_BG_COLOR_COMBO);
				m_select_gb_color = combo->GetCurSel();
			}
			return TRUE;
		case IDC_WIDTH_EDIT:
			if(HIWORD(wParam) == EN_CHANGE){
				char str[256];
				this->GetDlgItemText(IDC_WIDTH_EDIT, str, 256);
				m_file_width = atoi(str);
				if (m_file_width < 0) {
					m_file_width = 0;
					wsprintf(str, "%d", m_file_width);
					this->SetDlgItemText(IDC_WIDTH_EDIT, str);
				}
				else if (m_file_width > IMG_EDIT_MAX_FILE_WIDTH) {
					m_file_width = IMG_EDIT_MAX_FILE_WIDTH;
					wsprintf(str, "%d", m_file_width);
					this->SetDlgItemText(IDC_WIDTH_EDIT, str);
				}
			}
			return TRUE;
		case IDC_HEIGHT_EDIT:
			if (HIWORD(wParam) == EN_CHANGE) {
				char str[256];
				this->GetDlgItemText(IDC_HEIGHT_EDIT, str, 256);
				m_file_height = atoi(str);
				if(m_file_height < 0){
					m_file_height = 0;
					wsprintf(str, "%d", m_file_height);
					this->SetDlgItemText(IDC_WIDTH_EDIT, str);
				}
				else if(m_file_height > IMG_EDIT_MAX_FILE_HEIGHT){
					m_file_height = IMG_EDIT_MAX_FILE_HEIGHT;
					wsprintf(str, "%d", m_file_height);
					this->SetDlgItemText(IDC_HEIGHT_EDIT, str);
				}
			}
			return TRUE;
	}
	return CDialog::OnCommand(wParam, lParam);
}

void CCreateNewFileDialog::CreateNewTemplateSizeFile()
{
	::WritePrivateProfileString("general", "num_of_template", "4", SIZE_INI_FILE_PATH);

	//A4
	::WritePrivateProfileString("template0", "name", "A4", SIZE_INI_FILE_PATH);
	::WritePrivateProfileString("template0", "width", "210", SIZE_INI_FILE_PATH);
	::WritePrivateProfileString("template0", "height", "297", SIZE_INI_FILE_PATH);
	::WritePrivateProfileString("template0", "unit_name", "mm", SIZE_INI_FILE_PATH);
	::WritePrivateProfileString("template0", "resolution", "300", SIZE_INI_FILE_PATH);

	//A3
	::WritePrivateProfileString("template1", "name", "A3", SIZE_INI_FILE_PATH);
	::WritePrivateProfileString("template1", "width", "297", SIZE_INI_FILE_PATH);
	::WritePrivateProfileString("template1", "height", "420", SIZE_INI_FILE_PATH);
	::WritePrivateProfileString("template1", "unit_name", "mm", SIZE_INI_FILE_PATH);
	::WritePrivateProfileString("template1", "resolution", "300", SIZE_INI_FILE_PATH);

	//B5(JIS)
	::WritePrivateProfileString("template2", "name", "B5(JIS)", SIZE_INI_FILE_PATH);
	::WritePrivateProfileString("template2", "width", "182", SIZE_INI_FILE_PATH);
	::WritePrivateProfileString("template2", "height", "257", SIZE_INI_FILE_PATH);
	::WritePrivateProfileString("template2", "unit_name", "mm", SIZE_INI_FILE_PATH);
	::WritePrivateProfileString("template2", "resolution", "300", SIZE_INI_FILE_PATH);

	//B4(JIS)
	::WritePrivateProfileString("template3", "name", "B4(JIS)", SIZE_INI_FILE_PATH);
	::WritePrivateProfileString("template3", "width", "254", SIZE_INI_FILE_PATH);
	::WritePrivateProfileString("template3", "height", "364", SIZE_INI_FILE_PATH);
	::WritePrivateProfileString("template3", "unit_name", "mm", SIZE_INI_FILE_PATH);
	::WritePrivateProfileString("template3", "resolution", "300", SIZE_INI_FILE_PATH);
}

void CCreateNewFileDialog::SetFileSizeTemplate(int index)
{
	assert(0 <= index && index < m_fileSizeTemplate_Vec.size());
	FILE_SIZE_TEMPLATE fstemp = m_fileSizeTemplate_Vec[index];

	char str[256];

	sprintf(str, "%f", fstemp.width);
	this->SetDlgItemText(IDC_WIDTH_EDIT, str);

	sprintf(str, "%f", fstemp.height);
	this->SetDlgItemText(IDC_HEIGHT_EDIT, str);

	CComboBox* combo = (CComboBox*) this->GetDlgItem(IDC_SIZE_UNIT_COMBO);
	combo->SetCurSel(fstemp.unit_id);
	m_size_unit = fstemp.unit_id;

	wsprintf(str, "%d", fstemp.resolution);
	this->SetDlgItemText(IDC_RESOLUTION_EDIT, str);

	//
	CButton* btn = (CButton*)this->GetDlgItem(IDC_VERTICAL_RADIO);
	btn->SetCheck(BST_CHECKED);
}

int CCreateNewFileDialog::GetImgPixelWidth()
{
	char str[256];
	this->GetDlgItemText(IDC_WIDTH_EDIT, str, 256);
	int width = atoi(str);

	this->GetDlgItemText(IDC_RESOLUTION_EDIT, str, 256);
	int resolution = atoi(str);

	switch (m_size_unit) {
	case 0: //pixel
		return width;
		break;
	case 1: //inch
		return width * resolution;
		break;
	case 2: //mm
		return ((double)width / 25.4) * resolution;
		break;
	default:
		assert(0);
		break;
	}
}

void CCreateNewFileDialog::SetImgPixelWidth(int pixel)
{
	char str[256];

	this->GetDlgItemText(IDC_RESOLUTION_EDIT, str, 256);
	int resolution = atoi(str);

	double width = 0;
	switch (m_size_unit) {
	case 0: //pixel;
		width = pixel;
		break;
	case 1: //inch
		width = (double)pixel / resolution;
		break;
	case 2: //mm
		width = ((double)pixel * 25.4) / resolution;
		break;
	default:
		assert(0);
		break;
	}

	wsprintf(str, "%f", width);
	this->SetDlgItemText(IDC_WIDTH_EDIT, str);
}

int CCreateNewFileDialog::GetImgPixelHeight()
{
	char str[256];
	this->GetDlgItemText(IDC_HEIGHT_EDIT, str, 256);
	int height = atoi(str);

	this->GetDlgItemText(IDC_RESOLUTION_EDIT, str, 256);
	int resolution = atoi(str);

	switch (m_size_unit) {
	case 0: //pixel
		return height;
		break;
	case 1: //inch
		return height * resolution;
		break;
	case 2: //mm
		return ((double)height / 25.4) * resolution;
		break;
	}
}

void CCreateNewFileDialog::SetImgPixelHeight(int pixel)
{
	char str[256];

	this->GetDlgItemText(IDC_RESOLUTION_EDIT, str, 256);
	int resolution = atoi(str);

	double height = 0;
	switch (m_size_unit) {
	case 0: //pixel;
		height = pixel;
		break;
	case 1: //inch
		height = (double)pixel / resolution;
		break;
	case 2: //mm
		height = ((double)pixel * 25.4) / resolution;
	default:
		assert(0);
		break;
	}

	wsprintf(str, "%f", height);
	this->SetDlgItemText(IDC_HEIGHT_EDIT, str);
}

void CCreateNewFileDialog::OnBnClickedHorizontalRadio()
{
	CButton* btn = (CButton*)this->GetDlgItem(IDC_HORIZONTAL_RADIO);
	if (btn->GetCheck() == BST_CHECKED) {
		//exchange width for height
		char buf1[_MAX_PATH];
		char buf2[_MAX_PATH];
		this->GetDlgItemText(IDC_WIDTH_EDIT, buf1, _MAX_PATH);
		this->GetDlgItemText(IDC_HEIGHT_EDIT, buf2, _MAX_PATH);
		this->SetDlgItemText(IDC_WIDTH_EDIT, buf2);
		this->SetDlgItemText(IDC_HEIGHT_EDIT, buf1);
	}
}


void CCreateNewFileDialog::OnBnClickedVerticalRadio()
{
	CButton* btn = (CButton*)this->GetDlgItem(IDC_VERTICAL_RADIO);
	if (btn->GetCheck() == BST_CHECKED) {
		//exchange width for height
		char buf1[_MAX_PATH];
		char buf2[_MAX_PATH];
		this->GetDlgItemText(IDC_WIDTH_EDIT, buf1, _MAX_PATH);
		this->GetDlgItemText(IDC_HEIGHT_EDIT, buf2, _MAX_PATH);
		this->SetDlgItemText(IDC_WIDTH_EDIT, buf2);
		this->SetDlgItemText(IDC_HEIGHT_EDIT, buf1);
	}
}
