// NewPaletteColorDilalog.cpp : 実装ファイル
//

#include "stdafx.h"
#include "ImgEdit_UI.h"
#include "NewPaletteColorDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern ImgEdit g_ImgEdit;

// CNewPaletteColorDialog ダイアログ

IMPLEMENT_DYNAMIC(CNewPaletteColorDialog, CDialog)

CNewPaletteColorDialog::CNewPaletteColorDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CNewPaletteColorDialog::IDD, pParent)
{

}

CNewPaletteColorDialog::~CNewPaletteColorDialog()
{
}

void CNewPaletteColorDialog::PostNcDestroy()
{
	//set hook
	CImgEdit_UIApp* pApp = (CImgEdit_UIApp*)AfxGetApp();
	pApp->HookKeyboard();
	delete this;
}

void CNewPaletteColorDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CNewPaletteColorDialog::OnInitDialog()
{
	CEdit* pEdit = (CEdit*) GetDlgItem(IDC_PALETTE_COLOR_EDIT);
	pEdit->SetWindowText(TEXT("noname"));

	return TRUE;
}

void CNewPaletteColorDialog::OnOK()
{
	CEdit* pEdit = (CEdit*) GetDlgItem(IDC_PALETTE_COLOR_EDIT);

	TCHAR name[MAX_IE_PALETTE_NODE_NAME];
	pEdit->GetWindowText(name, MAX_IE_PALETTE_NODE_NAME);
	g_ImgEdit.AddColorToPalette(name, m_color );

	CDialog::OnOK();
}

BEGIN_MESSAGE_MAP(CNewPaletteColorDialog, CDialog)
END_MESSAGE_MAP()


// CNewPaletteColorDialog メッセージ ハンドラ
