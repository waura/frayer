// ColorFill.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//
#include "stdafx.h"
#include "ColorFill.h"
#include "IEStringHandle.h"
#include "resource.h"

static HMODULE g_hModule;
static IEColor g_SelectColor;
static char g_alpha_txt[4]; //数字半角最大3桁+NULL文字

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_hModule = hModule;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

// for detect memory leak
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <cstdlib>
	#include <crtdbg.h>
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#define DISPLAY_NAME_JA "塗りつぶし"
#define DISPLAY_NAME DISPLAY_NAME_JA

IECOMMAND_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::COMMAND;
}

IECOMMAND_EXPORT void GetIECommandName(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, "ColorFill");
}

IECOMMAND_EXPORT void GetIECommandDisplayNameJa(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, DISPLAY_NAME_JA);
}

LRESULT CALLBACK ColorFillDlgProc(HWND, UINT, WPARAM, LPARAM);

IECOMMAND_EXPORT IIECommand* CreateIECommand()
{
	return new ColorFill;
}

IECOMMAND_EXPORT void ReleaseIECommand(IIECommand** ppCommand)
{
	if(*ppCommand){
		delete (*ppCommand);
		(*ppCommand) = NULL;
	}
}

ColorFill::ColorFill()
{
  	strcpy_s(m_name, MAX_IE_MODULE_NAME, "ColorFill");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

BOOL ColorFill::Run(ImgEdit* pEdit, void* pvoid)
{
	int ret = ::DialogBoxParam((HINSTANCE)g_hModule, MAKEINTRESOURCE(IDD_COLOR_FILL),
		::GetForegroundWindow(), (DLGPROC)ColorFillDlgProc, (LPARAM)pEdit);

	if(ret == IDOK){		
		ImgFile_Ptr f = pEdit->GetActiveFile();
		if(!f) return FALSE;

		IImgLayer_weakPtr _sl = f->GetSelectLayer();
		if (_sl.expired()) {
			OutputError::Alert(IE_ERROR_SELECT_LAYER_DONT_EXIST);
			return FALSE;
		}

		IImgLayer_Ptr sl = _sl.lock();
		if (sl->GetLayerType() != IE_LAYER_TYPE::NORMAL_LAYER) {
			OutputError::Alert(IE_ERROR_CANT_EDIT_LAYER);
			return FALSE;
		}
		ImgLayer_Ptr l = std::dynamic_pointer_cast<ImgLayer>(sl);

		//選択マスクの範囲を取得
		RECT mask_rect;
		bool is_fill_zero_mask = (f->GetMaskState() == IE_MASK_STATE_ID::INVALID);
		if (is_fill_zero_mask) {
			CvSize size = f->GetImgSize();
			mask_rect.top = mask_rect.left = 0;
			mask_rect.right = size.width;
			mask_rect.bottom = size.height;
		}
		else {
			RECT rect;
			l->GetLayerRect(&rect);

			f->GetMaskRect(&mask_rect);
			//
			AndRect(&mask_rect, &mask_rect, &rect);
		}

		int width = mask_rect.right - mask_rect.left;
		int height = mask_rect.bottom - mask_rect.top;

		m_editHandle = (EditLayerHandle*)f->CreateImgFileHandle(IFH_EDIT_LAYER);
		m_editHandle->SetHandleName( DISPLAY_NAME );
		f->DoImgFileHandle( m_editHandle );

		EditNode* pNode = m_editHandle->CreateEditNode(&mask_rect);
		pNode->fourcc = LayerSynthesizerMG::GetDefaultLayerSynthFourCC();

		//fill color
		UCvPixel pixel;
		pixel.value = g_SelectColor.value;
		pixel.a = 255*(atoi(g_alpha_txt)/100.0);

		CvSize size = pNode->edit_img.GetSize();
		pNode->edit_img.RectFillColor(
			0, 0,
			size.width,
			size.height,
			&pixel);
		

		//update layer image
		m_editHandle->Update(&mask_rect);
		m_editHandle->EndEdit();
	}

	return FALSE;
}

LRESULT CALLBACK ColorFillDlgProc(HWND hDlgWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static ImgEdit* pEdit;
	static HWND hCombo;

	switch(msg){
		case WM_INITDIALOG:
			{
				pEdit = (ImgEdit*) lp;
				//描画モードのコンボボックスのハンドルを得る
				hCombo = ::GetDlgItem(hDlgWnd, IDC_COLOR_COMBO);
				//コンボボックスにアイテム追加
				::SendMessage(hCombo, CB_INSERTSTRING, 0, (LPARAM)"現在の選択色");
				::SendMessage(hCombo, CB_INSERTSTRING, 1, (LPARAM)"色選択");
				//コンボボックス初期選択
				::SendMessage(hCombo, CB_SETCURSEL, 0, 0);
				g_SelectColor = pEdit->GetSelectFGColor();
				//不透明度を100に
				HWND hSpin = ::GetDlgItem(hDlgWnd, IDC_SPIN1);
				::SendMessage(hSpin, UDM_SETRANGE, 0, MAKELONG(100, 0));
				::SendMessage(hSpin, UDM_SETPOS, 0, MAKELONG(100, 0));
			}
			return TRUE;
		case WM_COMMAND:
			switch(LOWORD(wp)){
				case IDOK:
					::EndDialog(hDlgWnd, IDOK);
					break;
				case IDCANCEL:
					::EndDialog(hDlgWnd, IDCANCEL);
					break;
				case IDC_COLOR_COMBO:
					if(HIWORD(wp) == CBN_SELCHANGE){//選択アイテム変更時
						//選択アイテムのインデックス取得
						int intIndex = ::SendMessage(hCombo, CB_GETCURSEL, 0, 0);
						switch(intIndex){
							case 0:
								g_SelectColor = pEdit->GetSelectFGColor();
								break;
							case 1:
								//not implemnet
								//g_SelectColor = pEdit->GetSelectFGColor();
								//IEColorDialog ccDialog(g_SelectColor);
								//if(ccDialog.DoModal() == IDOK){
								//	g_SelectColor = ccDialog.GetColor();
								//}
								break;
						}
					}
					break;
				case IDC_ALPHA_EDIT:
					if(HIWORD(wp) == EN_CHANGE){
						char str[6];
						::GetDlgItemText(hDlgWnd, IDC_ALPHA_EDIT, str, 6);
						if( (!IEStringHandle::IsAllDigitStr(str)) ||
							(strlen(str) > 3) || (atoi(str) > 100) || (atoi(str) < 0)){
							//不正な入力名時はビープをならしてリセット
							::MessageBeep(MB_OK);
							::SetDlgItemText(hDlgWnd, IDC_ALPHA_EDIT, g_alpha_txt);
							break;
						}

						//
						strcpy(g_alpha_txt, str);
					}
					break;
				default:
					return FALSE;
			}
			break;
		default:
			return FALSE;
	}
	return FALSE;
}