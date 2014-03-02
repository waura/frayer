#include "stdafx.h"

#include "IEConfigDialog.h"

#include "../resource1.h"

#include "IEConfig_Environment.h"
#include "IEConfig_InputRevise.h"
#include "IEConfig_Shortcut.h"
#include "IEConfig_ToolShift.h"

#include "ImgEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IEConfigDialog::IEConfigDialog()
{
	m_pDialogCmpnt = NULL;
}

IEConfigDialog::~IEConfigDialog()
{
	if(m_pDialogCmpnt){
		delete m_pDialogCmpnt;
		m_pDialogCmpnt = NULL;
	}
}

void IEConfigDialog::ShowDialog(HINSTANCE hInst, HWND howner, ImgEdit *pImgEdit)
{
	m_hInst = hInst;

	IE_CONFIG_DIALOG_DATA data;
	data.pIEConfigDialog = this;
	data.pImgEdit = pImgEdit;
	data.pDialogFont = &m_DialogFont;

	int ret = DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_IE_CONFIG), howner, IEConfigDialog::DialogProc, (LPARAM)(&data));
	if(ret == -1){
		char str[256];
		wsprintf(str, "IEConfigDialog::ShowDialog() error code %d", GetLastError());
		OutputError::Alert(str);
	}
}

//////////////////////////////////
/*!
	ダイアログのプロシージャ
*/
INT_PTR CALLBACK IEConfigDialog::DialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static IE_CONFIG_DIALOG_DATA ieData;

	if(ieData.pIEConfigDialog){
		if(ieData.pIEConfigDialog->DialogCmpntOnMessage(hWnd, msg, wp, lp)){
			return TRUE;
		}
	}

	switch(msg){
		case WM_INITDIALOG:
			{
				ieData = *((IE_CONFIG_DIALOG_DATA*) lp);
				ieData.pIEConfigDialog->InitDialog(hWnd);
			}
			return TRUE;
		case WM_NOTIFY:
			if(LOWORD(wp) == IDC_CONFIG_TREE){
				ieData.pIEConfigDialog->DialogTreeEvent(hWnd, lp, &ieData);
			}
			return TRUE;
		case WM_COMMAND:
			switch(LOWORD(wp)){
				case ID_IEOK:
					ieData.pIEConfigDialog->DialogCmpntSubmit();
					EndDialog(hWnd, IDOK);
					return TRUE;
				case ID_IECANCEL:
					EndDialog(hWnd, IDCANCEL);
					return TRUE;
				case ID_IEAPPLY:
					ieData.pIEConfigDialog->DialogCmpntSubmit();
					return TRUE;
			}
			break;
		case WM_CLOSE:
			EndDialog(hWnd, IDCANCEL);
			return TRUE;
	}

	return FALSE;
}

void IEConfigDialog::InitDialog(HWND hDlg)
{
	if(!m_DialogFont.CreateFont(
		12,
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
		"MS ゴシック")){
			OutputError::PushLog(LOG_LEVEL::_ERROR, "IEConfigDialog::InitDialog() faild create font");
	}

	TVINSERTSTRUCT tvi;
	HWND hTree;
	HTREEITEM hParent=TVI_ROOT;

	hTree = ::GetDlgItem(hDlg, IDC_CONFIG_TREE);

	//環境設定
	tvi.hInsertAfter = TVI_LAST;
	tvi.hParent = TVI_ROOT;
	tvi.item.mask = TVIF_TEXT;
	tvi.item.pszText = IE_CONFIG_TXT_ENV;
	TreeView_InsertItem(hTree, &tvi);

	//入力補正
	tvi.hInsertAfter = TVI_LAST;
	tvi.hParent = TVI_ROOT;
	tvi.item.mask = TVIF_TEXT;
	tvi.item.pszText = IE_CONFIG_TXT_INPUT_REVISE;
	TreeView_InsertItem(hTree, &tvi);

	//キーコンフィグ
	tvi.hInsertAfter = TVI_LAST;
	tvi.hParent = TVI_ROOT;
	tvi.item.mask = TVIF_TEXT;
	tvi.item.pszText = IE_CONFIG_TXT_KEY_CONFIG;
	hParent = TreeView_InsertItem(hTree, &tvi);

		//ショートカット
		tvi.hInsertAfter = TVI_LAST;
		tvi.hParent = hParent;
		tvi.item.mask = TVIF_TEXT;
		tvi.item.pszText = IE_CONFIG_TXT_SHORTCUT;
		TreeView_InsertItem(hTree, &tvi);

		//ツールシフト
		tvi.hInsertAfter = TVI_LAST;
		tvi.hParent = hParent;
		tvi.item.mask = TVIF_TEXT;
		tvi.item.pszText = IE_CONFIG_TXT_TOOL_SHIFT;
		TreeView_InsertItem(hTree, &tvi);
}

void IEConfigDialog::DialogTreeEvent(HWND hDlg, LPARAM lp, IE_CONFIG_DIALOG_DATA* piecData)
{
	HWND hTree;
	hTree = ::GetDlgItem(hDlg, IDC_CONFIG_TREE);

	LPNMTREEVIEW lpnmtview = (LPNMTREEVIEW)lp;
	if(lpnmtview->hdr.idFrom != IDC_CONFIG_TREE) return;

	switch(lpnmtview->hdr.code){
		case TVN_SELCHANGED: //ツリーアイテム選択変更時
			{
				HTREEITEM hItem = lpnmtview->itemNew.hItem;
				char pszText[256];
				TVITEM ti;
				ti.mask = TVIF_TEXT;
				ti.hItem = hItem;
				ti.pszText = pszText;
				ti.cchTextMax = 256;
				
				if(TreeView_GetItem(hTree, &ti)){
					//delete old compornent
					if(m_pDialogCmpnt){
						delete m_pDialogCmpnt;
						m_pDialogCmpnt = NULL;
					}

					//create dialog compornent
					if(strcmp(IE_CONFIG_TXT_ENV, pszText) == 0){
						m_pDialogCmpnt = new IEConfig_Environment;
					}
					else if(strcmp(IE_CONFIG_TXT_INPUT_REVISE, pszText) == 0){
						m_pDialogCmpnt = new IEConfig_InputRevise;
					}
					else if(strcmp(IE_CONFIG_TXT_SHORTCUT, pszText) == 0){
						m_pDialogCmpnt = new IEConfig_Shortcut;
					}
					else if(strcmp(IE_CONFIG_TXT_TOOL_SHIFT, pszText) == 0){
						m_pDialogCmpnt = new IEConfig_ToolShift;
					}
					else{
						OutputError::PushLog( LOG_LEVEL::_WARN, "IEConfigDialog::DialogTreeEvent() selected unknown tree item");
					}

					if(m_pDialogCmpnt){
						m_pDialogCmpnt->InitDialog(hDlg, m_hInst, piecData);
					}
				}
				else{
					OutputError::PushLog( LOG_LEVEL::_WARN, "IEConfigDialog::DialogTreeEvent() can't get tree item");
				}
			}
			break;
	}
}

void IEConfigDialog::DialogCmpntSubmit()
{
	if(m_pDialogCmpnt){
		m_pDialogCmpnt->Submit();
	}
}

BOOL IEConfigDialog::DialogCmpntOnMessage(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if(m_pDialogCmpnt){
		return m_pDialogCmpnt->OnMessage(hWnd, msg, wp, lp);
	}

	return FALSE;
}