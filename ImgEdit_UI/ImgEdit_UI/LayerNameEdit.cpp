#include "stdafx.h"

#include "ImgEdit_UI.h"
#include "LayerNameEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CLayerNameEdit::EndEdit()
{
	if (m_pEditLayer) {
		char new_name[MAX_IMG_LAYER_NAME];
		char old_name[MAX_IMG_LAYER_NAME];
		this->GetWindowText(new_name, MAX_IMG_LAYER_NAME);
		m_pEditLayer->GetName( old_name );

		if (strcmp(old_name, new_name) != 0) {
			ImgFile_Ptr pFile = m_pEditLayer->GetParentFile().lock();
			SetLayerNameHandle* set_layer_name_handle = (SetLayerNameHandle*)pFile->CreateImgFileHandle( IFH_SET_LAYER_NAME );
			set_layer_name_handle->SetEditLayer( m_pEditLayer );
			set_layer_name_handle->SetEditName( new_name );
			pFile->DoImgFileHandle( set_layer_name_handle );
		}

		m_pEditLayer = NULL;
		this->ShowWindow( SW_HIDE );

		//set hook
		CImgEdit_UIApp* pApp = (CImgEdit_UIApp*)AfxGetApp();
		pApp->HookKeyboard();
	}
}

BOOL CLayerNameEdit::PreTranslateMessage(MSG* pMsg){
	if (pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_RETURN) {
			EndEdit();
			return TRUE;
		}
	}
	else if (pMsg->message == WM_ACTIVATE) {
		int i;
		i = 0;
	}
	return CEdit::PreTranslateMessage(pMsg);
}

BOOL CLayerNameEdit::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (HIWORD(wParam)) {
		case EN_KILLFOCUS:
			{
				::OutputDebugString("en_killfocus \n");
			}
			return TRUE;
	}
	return CEdit::OnCommand(wParam, lParam);
}

BEGIN_MESSAGE_MAP(CLayerNameEdit, CEdit)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()

void CLayerNameEdit::OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized)
{
	CEdit::OnActivate(nState, pWndOther, bMinimized);

	if (nState == WA_INACTIVE) {
		EndEdit();
	}
}
