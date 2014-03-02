#include "stdafx.h"

#include "IE_EffectEditDialog.h"
#include "ImgFile.h"
#include "IEStringHandle.h"
#include "OutputError.h"
#include "../resource1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const int DIALOG_ITEM_WIDTH = 190;

static const int EW_MARGIN = 5;
static const int EW_LABEL_H = 20;
static const int EW_TRACKBAR_H = 30;


IE_EffectEditDialog::IE_EffectEditDialog()
{
	m_Ctrl_Num = 0;
	m_IdIndex = 0;
}

IE_EffectEditDialog::~IE_EffectEditDialog()
{
	ClearDialog();
}

INT_PTR IE_EffectEditDialog::CreateDialogBox(HINSTANCE hInst, HWND hwndParent, ImgFile_Ptr file, IEffect* effect)
{
	ClearDialog();

	m_pEditFile = file;
	m_pEditEffect = effect;
	
	int ret = DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_EFFECT_EDIT), hwndParent, IE_EffectEditDialog::DialogProc, (LPARAM)this);
	if(ret == -1){
		char str[256];
		wsprintf(str, "IEffectEditDialog::CreateDialogBox() error code %d", GetLastError());
		OutputError::Alert(str);
	}

	return ret;
}

//////////////////////////////////
/*!
	トラックバーの追加
	@param[in] name トラックバーの名前
	@param[in] min 最小値
	@param[in] max 最大値
	@param[in] start 開始時の値
	@param[in] change_val 値を変更する変数
*/
void IE_EffectEditDialog::AddTrackbar(const char *name, int min, int max, int start, int *change_val)
{
	TRACKBAR_DATA *pTData = new TRACKBAR_DATA;
	pTData->id = m_IdIndex;
	pTData->change_val = change_val;
	pTData->file = m_pEditFile;

	HWND hLabel;
	hLabel = CreateWindowEx(
		0,
		"static",
		name,
		SS_LEFT | WS_CHILD | WS_VISIBLE,
		EW_MARGIN, (EW_LABEL_H + EW_TRACKBAR_H)*m_Ctrl_Num + EW_MARGIN,
		DIALOG_ITEM_WIDTH/2, EW_LABEL_H,
		m_hWnd,
		(HMENU)pTData->id+1,
		m_hInst,
		NULL);


	//表示する文字数を求め領域確保
	int dig = IEStringHandle::GetDigit(start);
	char* val_name = new char[dig+2];
	wsprintf(val_name, "%d", start);

	HWND hValLabel;
	hValLabel = CreateWindowEx(
		0,
		"static",
		val_name,
		SS_RIGHT | WS_CHILD | WS_VISIBLE,
		DIALOG_ITEM_WIDTH - EW_MARGIN - 50, 
		(EW_LABEL_H + EW_TRACKBAR_H)*m_Ctrl_Num + EW_MARGIN,
		50, EW_LABEL_H,
		m_hWnd,
		(HMENU)pTData->id+2,
		m_hInst,
		NULL);
	delete[] val_name;
	val_name = NULL;

	HWND hTrack;
	hTrack = CreateWindowEx(
		0,
		TRACKBAR_CLASS,
		"",
		WS_CHILD | WS_VISIBLE |
		TBS_AUTOTICKS,
		EW_MARGIN, (EW_LABEL_H + EW_TRACKBAR_H)*m_Ctrl_Num + (EW_LABEL_H + EW_MARGIN),
		DIALOG_ITEM_WIDTH, EW_LABEL_H,
		m_hWnd,
		(HMENU)pTData->id,
		m_hInst,
		NULL);
	
	pTData->hWnd = hTrack;
	pTData->hLabel = hLabel;
	pTData->hValLabel = hValLabel;

	//スライダーの範囲を設定
	SendMessage( hTrack, TBM_SETRANGE, TRUE, MAKELPARAM(min, max));
	//スライダーの現在位置を変更
	SendMessage( hTrack, TBM_SETPOS, TRUE, start);

	m_TrackBar_DataVec.push_back(pTData);
	m_IdIndex += 3;
	m_Ctrl_Num++;
}

VOID IE_EffectEditDialog::SetProgRange(int min, int max)
{
	::SendMessage(::GetDlgItem(m_hWnd, IDC_EDIT_PROGRESS), PBM_SETRANGE, 0, MAKELPARAM(min, max));
}

VOID IE_EffectEditDialog::SetProgStep(int step)
{
	::SendMessage(::GetDlgItem(m_hWnd, IDC_EDIT_PROGRESS), PBM_SETSTEP, step, 0);
}

VOID IE_EffectEditDialog::ProgStepIt()
{
	::SendMessage(::GetDlgItem(m_hWnd, IDC_EDIT_PROGRESS), PBM_STEPIT, 0, 0);
}

VOID IE_EffectEditDialog::SetProgPos(int pos)
{
	::SendMessage(::GetDlgItem(m_hWnd, IDC_EDIT_PROGRESS), PBM_SETPOS, pos, 0);
}

//////////////////////////////////
/*!
	ダイアログのプロシージャ
*/
INT_PTR CALLBACK IE_EffectEditDialog::DialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg){
		case WM_INITDIALOG:
			{
				IE_EffectEditDialog *ed = (IE_EffectEditDialog*) lp;
				SetProp(hWnd, PROP_EED_WINPROC, ed);
				ed->SetHWND(hWnd);
				ed->SetDialogToEffect();
				ed->EffectEdit();
			}
			return TRUE;
		case WM_HSCROLL:
			{
				int val = SendMessage((HWND)lp, TBM_GETPOS, 0, 0);
				IE_EffectEditDialog *ew = (IE_EffectEditDialog*)GetProp(hWnd, PROP_EED_WINPROC);
				ew->CallTrackbar((HWND)lp, wp, val);
			}
			return TRUE;
		case WM_COMMAND:
			switch(LOWORD(wp)){
				case IDOK:
					::RemoveProp(hWnd, PROP_EED_WINPROC);
					EndDialog(hWnd, IDOK);
					return TRUE;
				case IDCANCEL:
					::RemoveProp(hWnd, PROP_EED_WINPROC);
					EndDialog(hWnd, IDCANCEL);
					return TRUE;
			}
	}
	return FALSE;
}

VOID IE_EffectEditDialog::SetHWND(HWND hWnd)
{
	m_hWnd = hWnd;
}

VOID IE_EffectEditDialog::SetDialogToEffect()
{
	m_pEditEffect->SetEditDialog(this);
}

VOID IE_EffectEditDialog::EffectEdit()
{
	m_pEditEffect->Edit( m_pEditFile );
}

//////////////////////////////////
/*!
	トラックバーのイベント時
	@param[in] hTrac トラックバーのハンドル
	@param[in] wp
	@param[in] val トラックバーの値
*/
VOID IE_EffectEditDialog::CallTrackbar(HWND hTrack, WPARAM wp, int val)
{
	TRACKBAR_DATA_VEC::iterator itr = m_TrackBar_DataVec.begin();
	for(; itr != m_TrackBar_DataVec.end(); itr++){
		if((*itr)->hWnd == hTrack){
			*((*itr)->change_val) = val;

			//表示する文字数を求め領域確保
			int dig = IEStringHandle::GetDigit(val);
			char* val_name = new char[dig+2];
			wsprintf(val_name, "%d", val);

			//表示値の更新
			::SetWindowText((*itr)->hValLabel, val_name);
			delete[] val_name;
			val_name = NULL;
			::InvalidateRect((*itr)->hValLabel, NULL, true);

			if(LOWORD(wp) == SB_ENDSCROLL){
				////表示画像の更新
				m_pEditEffect->Edit( m_pEditFile );
			}

			return;
		}

	}
}

VOID IE_EffectEditDialog::ClearDialog()
{
	TRACKBAR_DATA_VEC::iterator itr = m_TrackBar_DataVec.begin();
	for(; itr != m_TrackBar_DataVec.end(); itr++){
		//DestroyWindow((*itr)->hWnd);
		//DestroyWindow((*itr)->hLabel);
		//DestroyWindow((*itr)->hValLabel);
		delete (*itr);
		(*itr) = NULL;
	}
	m_TrackBar_DataVec.clear();

	m_Ctrl_Num = 0;
	m_IdIndex = 0;
}