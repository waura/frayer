#include "stdafx.h"

#include "IEColorDialog.h"
#include "..\resource.h"

extern HMODULE g_hModule;

IEColorDialog::IEColorDialog(IEColor defColor)
{
	m_SelectColor = defColor;
}

INT_PTR IEColorDialog::DoModal()
{
	::MessageBox(NULL, "‚Ü‚¾–¢ŽÀ‘•", "", MB_OK);
	return ::DialogBoxParam((HINSTANCE)g_hModule, MAKEINTRESOURCE(IDD_IE_COLOR_DIALOG),
		::GetForegroundWindow(), (DLGPROC)IEColorDialog::IEColorDialogProc, (LPARAM)m_SelectColor);
}

IEColor IEColorDialog::GetColor()
{
	return m_SelectColor;
}

LRESULT CALLBACK IEColorDialog::IEColorDialogProc(HWND hDlgWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static IEColor select_color;
	switch(msg){
		case WM_INITDIALOG:
			{
				select_color = (IEColor) lp;
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
				default:
					return FALSE;
			}
		default:
			return FALSE;
	}
	return TRUE;
}