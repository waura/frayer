#pragma once

#include "IIEConfig_DialogComponent.h"

#define IE_CONFIG_TXT_ENV "環境設定"
#define IE_CONFIG_TXT_INPUT_REVISE "入力補正"
#define IE_CONFIG_TXT_KEY_CONFIG "キーコンフィグ"
#define IE_CONFIG_TXT_SHORTCUT "ショートカット"
#define IE_CONFIG_TXT_TOOL_SHIFT "ツールシフト"

class IEConfigDialog
{
public:
	IEConfigDialog();
	virtual ~IEConfigDialog();

	void ShowDialog(HINSTANCE hInst, HWND howner, ImgEdit *pImgEdit);

	//////////////////////////////////
	/*!
		ダイアログのプロシージャ
	*/
	static INT_PTR CALLBACK DialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

private:

	///////////////////////////////////
	/*!
		ダイアログの初期化
	*/
	void InitDialog(HWND hDlg);

	///////////////////////////////////
	/*!
		設定項目選択ツリービューのイベント
	*/
	void DialogTreeEvent(HWND hDlg, LPARAM lp, IE_CONFIG_DIALOG_DATA* piecData);

	////////////////////////////////////
	/*!
	*/
	void DialogCmpntSubmit();

	BOOL DialogCmpntOnMessage(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);


	HINSTANCE m_hInst;
	CFont m_DialogFont;
	IIEConfig_DialogComponent *m_pDialogCmpnt;
};
