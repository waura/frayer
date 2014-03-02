// ChangeCanvasSize.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include "resource.h"
#include "ChangeCanvasSize.h"

// for detect memory leak
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <cstdlib>
	#include <crtdbg.h>
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#define DISPLAY_NAME_JA "キャンバスサイズ変更"
#define DISPLAY_NAME DISPLAY_NAME_JA

extern HMODULE g_hModule;
static CvSize g_new_canvas_size;

IECOMMAND_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::COMMAND;
}

IECOMMAND_EXPORT void GetIECommandName(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, "ChangeCanvasSize");
}

IECOMMAND_EXPORT void GetIECommandDisplayNameJa(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, DISPLAY_NAME_JA);
}

LRESULT CALLBACK ChangeCanvasSizeDlgProc(HWND, UINT, WPARAM, LPARAM);

IECOMMAND_EXPORT IIECommand* CreateIECommand()
{
	return new ChangeCanvasSize;
}

IECOMMAND_EXPORT void ReleaseIECommand(IIECommand** ppCommand)
{
	if (*ppCommand) {
		delete (*ppCommand);
		(*ppCommand) = NULL;
	}
}

ChangeCanvasSize::ChangeCanvasSize()
{
  	strcpy_s(m_name, MAX_IE_MODULE_NAME, "ChangeCanvasSize");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

BOOL ChangeCanvasSize::Run(ImgEdit* pEdit, void* pvoid)
{
	ImgFile_Ptr f = pEdit->GetActiveFile();
	if(!f) return FALSE;

	g_new_canvas_size = f->GetImgSize();

	int ret = ::DialogBoxParam((HINSTANCE)g_hModule, MAKEINTRESOURCE(IDD_CHANGE_CANVAS_SIZE),
		::GetForegroundWindow(), (DLGPROC)ChangeCanvasSizeDlgProc, (LPARAM)pEdit);

	if(ret == IDOK){
		ChangeCanvasSizeHandle* handle = (ChangeCanvasSizeHandle*) f->CreateImgFileHandle(IFH_CHANGE_CANVAS_SIZE);
		strcpy_s(handle->handle_name, MAX_IMG_FILE_HANDLE_NAME, DISPLAY_NAME);
		handle->SetNewCanvasSize( g_new_canvas_size );
		f->DoImgFileHandle( handle );
	}

	return FALSE;
}

LRESULT CALLBACK ChangeCanvasSizeDlgProc(HWND hDlgWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg){
		case WM_INITDIALOG:
			{
				HWND hWEdit = ::GetDlgItem(hDlgWnd, IDC_WIDTH_EDIT);
				HWND hHEdit = ::GetDlgItem(hDlgWnd, IDC_HEIGHT_EDIT);

				char str[256];
				wsprintf(str, "%d", g_new_canvas_size.width);
				::SetWindowText(hWEdit, str);

				wsprintf(str, "%d", g_new_canvas_size.height);
				::SetWindowText(hHEdit, str);
			}
			return TRUE;
		case WM_COMMAND:
			switch(LOWORD(wp)){
				case IDOK:
					{
						HWND hWEdit = ::GetDlgItem(hDlgWnd, IDC_WIDTH_EDIT);
						HWND hHEdit = ::GetDlgItem(hDlgWnd, IDC_HEIGHT_EDIT);

						char str[256];
						::GetWindowText(hWEdit, str, 256);
						g_new_canvas_size.width = atoi(str);

						::GetWindowText(hHEdit, str, 256);
						g_new_canvas_size.height = atoi(str);

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