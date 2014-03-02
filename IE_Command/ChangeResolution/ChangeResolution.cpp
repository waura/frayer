// ChangeResolution.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include "resource.h"
#include "ChangeResolution.h"

// for detect memory leak
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <cstdlib>
	#include <crtdbg.h>
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#define DISPLAY_NAME_JA "画像解像度変更"
#define DISPLAY_NAME DISPLAY_NAME_JA

extern HMODULE g_hModule;
static CvSize g_new_resolution;
static int g_interpolation;

IECOMMAND_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::COMMAND;
}

IECOMMAND_EXPORT void GetIECommandName(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, "ChangeResolution");
}

IECOMMAND_EXPORT void GetIECommandDisplayNameJa(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, DISPLAY_NAME_JA);
}

LRESULT CALLBACK ChangeResolutionDlgProc(HWND, UINT, WPARAM, LPARAM);

IECOMMAND_EXPORT IIECommand* CreateIECommand()
{
	return new ChangeResolution();
}

IECOMMAND_EXPORT void ReleaseIECommand(IIECommand** ppCommand)
{
	if (*ppCommand) {
		delete (*ppCommand);
		(*ppCommand) = NULL;
	}
}

ChangeResolution::ChangeResolution()
{
  	strcpy_s(m_name, MAX_IE_MODULE_NAME, "ChangeResolution");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

BOOL ChangeResolution::Run(ImgEdit *pEdit, void *pvoid)
{
	ImgFile_Ptr f = pEdit->GetActiveFile();
	if(!f) return FALSE;

	g_new_resolution = f->GetImgSize();

	int ret = ::DialogBoxParam((HINSTANCE)g_hModule, MAKEINTRESOURCE(IDD_CHANGE_RESOLUTION),
		::GetForegroundWindow(), (DLGPROC)ChangeResolutionDlgProc, (LPARAM)pEdit);

	if(ret == IDOK){
		ChangeResolutionHandle* handle = (ChangeResolutionHandle*) f->CreateImgFileHandle(IFH_CHANGE_RESOLUTION);
		strcpy_s(handle->handle_name, MAX_IMG_FILE_HANDLE_NAME, DISPLAY_NAME);
		handle->SetNewResolution( g_new_resolution );
		handle->SetInterpolation( g_interpolation );
		f->DoImgFileHandle( handle );
	}

	return FALSE;
}

LRESULT CALLBACK ChangeResolutionDlgProc(HWND hDlgWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static bool is_write_edit_box;

	switch(msg){
		case WM_INITDIALOG:
			{
				//resolution edit
				HWND hWEdit = ::GetDlgItem(hDlgWnd, IDC_WIDTH_EDIT);
				HWND hHEdit = ::GetDlgItem(hDlgWnd, IDC_HEIGHT_EDIT);

				char str[256];
				wsprintf(str, "%d", g_new_resolution.width);
				::SetWindowText(hWEdit, str);

				wsprintf(str, "%d", g_new_resolution.height);
				::SetWindowText(hHEdit, str);

				//interpolation combo
				HWND hIntrpCmb = ::GetDlgItem(hDlgWnd, IDC_INTRP_COMBO);
				::SendMessage(hIntrpCmb, CB_INSERTSTRING, 0, (LPARAM)"ニアレストネイバー");
				::SendMessage(hIntrpCmb, CB_INSERTSTRING, 1, (LPARAM)"バイリニア");
				::SendMessage(hIntrpCmb, CB_INSERTSTRING, 2, (LPARAM)"ピクセル領域による補間");
				::SendMessage(hIntrpCmb, CB_INSERTSTRING, 3, (LPARAM)"バイキュービック");
				::SendMessage(hIntrpCmb, CB_SETCURSEL, 3, 0);

				//fix aspect check
				HWND hAsChk = ::GetDlgItem(hDlgWnd, IDC_FIX_ASPECT_CHECK);
				::SendMessage(hAsChk, BM_SETCHECK, 1, 0);
			}
			return FALSE;
		case WM_COMMAND:
			switch(LOWORD(wp)){
				case IDC_WIDTH_EDIT:
					{
						HWND hAsChk = ::GetDlgItem(hDlgWnd, IDC_FIX_ASPECT_CHECK);
						int bchk = (int)SendMessage(hAsChk, BM_GETCHECK, 0, 0);
						if(is_write_edit_box == false && 
							bchk == 1 && HIWORD(wp) == EN_UPDATE)
						{
							HWND hWEdit = ::GetDlgItem(hDlgWnd, IDC_WIDTH_EDIT);
							HWND hHEdit = ::GetDlgItem(hDlgWnd, IDC_HEIGHT_EDIT);

							char str[256];
							::GetWindowText(hWEdit, str, 256);
							int width = atoi(str);
							int height = g_new_resolution.height * ((double)width / g_new_resolution.width);

							wsprintf(str, "%d", height);
							is_write_edit_box = true;
							::SetWindowText(hHEdit, str);
							is_write_edit_box = false;
						}
					}
					break;
				case IDC_HEIGHT_EDIT:
					{
						HWND hAsChk = ::GetDlgItem(hDlgWnd, IDC_FIX_ASPECT_CHECK);
						int bchk = (int)SendMessage(hAsChk, BM_GETCHECK, 0, 0);
						if(is_write_edit_box == false &&
							bchk == 1 && HIWORD(wp) == EN_UPDATE)
						{
							HWND hWEdit = ::GetDlgItem(hDlgWnd, IDC_WIDTH_EDIT);
							HWND hHEdit = ::GetDlgItem(hDlgWnd, IDC_HEIGHT_EDIT);

							char str[256];
							::GetWindowText(hHEdit, str, 256);
							int height = atoi(str);
							int width = g_new_resolution.width * ((double)height / g_new_resolution.height);

							wsprintf(str, "%d", width);
							is_write_edit_box = true;
							::SetWindowText(hWEdit, str);
							is_write_edit_box = false;
						}
					}
					break;
				case IDOK:
					{
						//set resolution
						HWND hWEdit = ::GetDlgItem(hDlgWnd, IDC_WIDTH_EDIT);
						HWND hHEdit = ::GetDlgItem(hDlgWnd, IDC_HEIGHT_EDIT);

						char str[256];
						::GetWindowText(hWEdit, str, 256);
						g_new_resolution.width = atoi(str);

						::GetWindowText(hHEdit, str, 256);
						g_new_resolution.height = atoi(str);

						//set interpolation
						HWND hIntrpCmb = ::GetDlgItem(hDlgWnd, IDC_INTRP_COMBO);
						int index = ::SendMessage(hIntrpCmb, CB_GETCURSEL, 0, 0);
						assert(index != -1);
						switch( index ){
							case 0:
								g_interpolation = CV_INTER_NN;
								break;
							case 1:
								g_interpolation = CV_INTER_LINEAR;
								break;
							case 2:
								g_interpolation = CV_INTER_AREA;
								break;
							case 3:
								g_interpolation = CV_INTER_CUBIC;
								break;
							default:
								assert(0);
								break;
						}

						::EndDialog(hDlgWnd, IDOK);
					}
					break;
				case IDCANCEL:
					::EndDialog(hDlgWnd, IDCANCEL);					
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