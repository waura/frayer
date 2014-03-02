#include "stdafx.h"

#include "IEConfig_InputRevise.h"
#include "ImgEdit.h"
#include "InputReviseMG.h"

#define IEIR_COMBO_X 200
#define IEIR_COMBO_Y 10
#define IEIR_COMBO_W 300
#define IEIR_COMBO_H 200 //ドロップダウン時の高さ

#define IEIR_COMBO_ID 1

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IEConfig_InputRevise::IEConfig_InputRevise()
{
}

IEConfig_InputRevise::~IEConfig_InputRevise()
{
	::DestroyWindow(m_hCombo);
}

void IEConfig_InputRevise::InitDialog(HWND hDialog, HINSTANCE hInst, IE_CONFIG_DIALOG_DATA *pConfigData)
{
	m_pConfigData = pConfigData;

	m_hCombo = ::CreateWindow(
		"COMBOBOX", "",
		ES_LEFT | WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWNLIST,
		IEIR_COMBO_X,
		IEIR_COMBO_Y,
		IEIR_COMBO_W,
		IEIR_COMBO_H,
		hDialog,
		(HMENU) IEIR_COMBO_ID,
		hInst, NULL);

	m_pConfigData->pImgEdit->SetInputReviseComboBox(m_hCombo);
	m_pConfigData->pImgEdit->GetInputReviseName(select_revise_name);

	::UpdateWindow(hDialog);
}

void IEConfig_InputRevise::Submit()
{
	m_pConfigData->pImgEdit->SelectInputRevise(select_revise_name);
}

BOOL IEConfig_InputRevise::OnMessage(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg){
		case WM_COMMAND:
			if(LOWORD(wp) == IEIR_COMBO_ID){
				if(HIWORD(wp) == CBN_SELCHANGE){
					int select_index = SendMessage(m_hCombo, CB_GETCURSEL, 0, 0); 
					PSTR strText = (PSTR)malloc(
									SendMessage(m_hCombo, CB_GETLBTEXTLEN,
									select_index, 0 ) + 1);
					SendMessage(m_hCombo, CB_GETLBTEXT,
						select_index,
						(LPARAM)strText);

					strcpy(select_revise_name, strText);
					free(strText);
					return TRUE;
				}
			}
			break;
	}

	return FALSE;
}