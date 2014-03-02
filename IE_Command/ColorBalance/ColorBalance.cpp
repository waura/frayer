// ColorBalance.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include "resource.h"
#include "ColorBalance.h"

// for detect memory leak
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <cstdlib>
	#include <crtdbg.h>
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#define DISPLAY_NAME_JA "カラーバランス"
#define DISPLAY_NAME DISPLAY_NAME_JA

extern HMODULE g_hModule;


IECOMMAND_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::COMMAND;
}

IECOMMAND_EXPORT  void GetIECommandName(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, "ColorBalance");
}

IECOMMAND_EXPORT void GetIECommandDisplayNameJa(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, DISPLAY_NAME);
}

LRESULT CALLBACK ColorBalanceDlgProc(HWND, UINT, WPARAM, LPARAM);

IECOMMAND_EXPORT IIECommand* CreateIECommand()
{
	return new ColorBalance;
}

IECOMMAND_EXPORT void ReleaseIECommand(IIECommand** ppCommand)
{
	if (*ppCommand) {
		delete (*ppCommand);
		(*ppCommand) = NULL;
	}
}

ColorBalance::ColorBalance()
{
	m_pEditNode = NULL;
	m_pEditFile = NULL;
	
	int i;
	for (i=0; i<3; i++) {
		m_r_edit_vals[i] = 0;
		m_g_edit_vals[i] = 0;
		m_b_edit_vals[i] = 0;
	}

  	strcpy_s(m_name, MAX_IE_MODULE_NAME, "ColorBalance");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

bool ColorBalance::Init(ImgEdit *pEdit)
{
	m_BGRA_LUT = cvCreateMat(1, 256, CV_8UC4);

	//init transfer coffecient
	int i;
	for(i=0; i <= 255; i++){
		double low = (1.075 - 1 / ((double) i / 16.0 + 1));
		double mid = 0.667 * (1 - (((double) i - 127.0) / 127.0)*(((double) i - 127.0) / 127.0));

		m_shadows_add[i]		= low;
		m_shadows_sub[255 - i]	= low;
		//
		m_midtones_add[i]		= mid;
		m_midtones_sub[i]		= mid;
		//
		m_highlights_add[255 - i]	= low;
		m_highlights_sub[i]			= low;
	}

	m_pEditFile = pEdit->GetActiveFile();
	if(!m_pEditFile) return false;

	IImgLayer_weakPtr _sl = m_pEditFile->GetSelectLayer();
	if (_sl.expired()) {
		OutputError::Alert(IE_ERROR_SELECT_LAYER_DONT_EXIST);
		return false;
	}

	IImgLayer_Ptr sl = _sl.lock();
	if (sl->GetLayerType() != IE_LAYER_TYPE::NORMAL_LAYER) {
		OutputError::Alert(IE_ERROR_CANT_EDIT_LAYER);
		return false;
	}
	ImgLayer_Ptr l = std::dynamic_pointer_cast<ImgLayer>(sl);

	IplImage* mask;
	//選択マスクの範囲を取得
	bool is_invalid_mask = (m_pEditFile->GetMaskState() == IE_MASK_STATE_ID::INVALID);
	if(is_invalid_mask){
		mask = NULL;
		CvSize size = m_pEditFile->GetImgSize();
		m_mask_rect.top = m_mask_rect.left = 0;
		m_mask_rect.right = size.width;
		m_mask_rect.bottom = size.height;
	}
	else{
		RECT rect;
		l->GetLayerRect(&rect);

		m_pEditFile->GetMaskRect(&m_mask_rect);
		//
		AndRect(&m_mask_rect, &m_mask_rect, &rect);

		//マスク切り出し
		mask = cvCreateImage(
			cvSize(m_mask_rect.right - m_mask_rect.left, m_mask_rect.bottom - m_mask_rect.top),
			IPL_DEPTH_8U,
			1);

		m_pEditFile->CopyFileMask(
			m_mask_rect.left,
			m_mask_rect.top,
			m_mask_rect.right - m_mask_rect.left,
			m_mask_rect.bottom - m_mask_rect.top,
			mask,
			0, 0);
	}

	int width = m_mask_rect.right - m_mask_rect.left;
	int height = m_mask_rect.bottom - m_mask_rect.top;

	m_pEditLayerHandle = (EditLayerHandle*) m_pEditFile->CreateImgFileHandle( IFH_EDIT_LAYER );
	m_pEditLayerHandle->SetHandleName( DISPLAY_NAME );
	m_pEditFile->DoImgFileHandle( m_pEditLayerHandle );

	m_pEditNode = m_pEditLayerHandle->CreateEditNode(&m_mask_rect);

	//get undo image
	l->CopyLayerImage(
		&(m_pEditNode->undo_img),
		0,
		0,
		m_mask_rect.left,
		m_mask_rect.top,
		m_mask_rect.right - m_mask_rect.left,
		m_mask_rect.bottom - m_mask_rect.top);

	//
	if(is_invalid_mask){
		CvSize size = m_pEditNode->edit_img.GetSize();
		m_pEditNode->edit_img.RectFillAlpha(
			0,
			0,
			size.width,
			size.height,
			255);
	}
	else{
		m_pEditNode->edit_img.RectFillAlpha(
			m_mask_rect.left,
			m_mask_rect.top,
			m_mask_rect.right - m_mask_rect.left,
			m_mask_rect.bottom - m_mask_rect.top,
			255,
			NULL);
	}

	if(mask){
		cvReleaseImage(&mask);
	}

	return true;
}

void ColorBalance::End(ImgEdit* pEdit)
{
	m_pEditLayerHandle->EndEdit();

	if(m_BGRA_LUT){
		cvReleaseMat(&m_BGRA_LUT);
	}
}

BOOL ColorBalance::Run(ImgEdit* pEdit, void* pvoid)
{
	if(m_pEditNode == NULL) return FALSE;

	int ret = ::DialogBoxParam((HINSTANCE)g_hModule, MAKEINTRESOURCE(IDD_COLOR_BALANCE_DLG),
		::GetForegroundWindow(), (DLGPROC)ColorBalanceDlgProc, (LPARAM)this);

	if(ret == IDCANCEL){
		ImgFile_Ptr f = pEdit->GetActiveFile();
		if(f){
			m_pEditLayerHandle->EndEdit();
			f->Undo(1);
		}
	}

	return FALSE;
}

#define CLAMP(x,l,u) ((x)<(l)?(l):((x)>(u)?(u):(x)))
#define CLAMP0255(a)  CLAMP(a,0,255)

unsigned _stdcall UpdateImageThread(void* data)
{
	LPCOLOR_BALANCE_DATA pCBData = (LPCOLOR_BALANCE_DATA)data;
	RECT mask_rect = pCBData->mask_rect;
	CvMat* BGRA_LUT = pCBData->BGRA_LUT;
	EditNode* pEditNode = pCBData->pEditNode;
	EditLayerHandle* pEditLayerHandle = pCBData->pEditLayerHandle;

	int i;
	int x;
	int r,g,b;
	for(x=0; x<=255; x++){
		r = g = b = x;

		r += pCBData->r_edit_vals[IE_SHADOWS] * pCBData->r_transfer[IE_SHADOWS][x];
		CLAMP0255(r);
		r += pCBData->r_edit_vals[IE_MIDTONES] * pCBData->r_transfer[IE_MIDTONES][x];
		CLAMP0255(r);
		r += pCBData->r_edit_vals[IE_HIGHLIGHTS] * pCBData->r_transfer[IE_HIGHLIGHTS][x];
		CLAMP0255(r);

		g += pCBData->g_edit_vals[IE_SHADOWS] * pCBData->g_transfer[IE_SHADOWS][x];
		CLAMP0255(r);
		g += pCBData->g_edit_vals[IE_MIDTONES] * pCBData->g_transfer[IE_MIDTONES][x];
		CLAMP0255(r);
		g += pCBData->g_edit_vals[IE_HIGHLIGHTS] * pCBData->g_transfer[IE_HIGHLIGHTS][x];
		CLAMP0255(r);

		b += pCBData->b_edit_vals[IE_SHADOWS] * pCBData->b_transfer[IE_SHADOWS][x];
		CLAMP0255(r);
		b += pCBData->b_edit_vals[IE_MIDTONES] * pCBData->b_transfer[IE_MIDTONES][x];
		CLAMP0255(r);
		b += pCBData->b_edit_vals[IE_HIGHLIGHTS] * pCBData->b_transfer[IE_HIGHLIGHTS][x];
		CLAMP0255(r);

		cvSet1D(BGRA_LUT, x, cvScalar(b, g, r, x));
	}

	if(pCBData->is_preserve_luminosity){
		pEditNode->edit_img.PL_LUT(
			mask_rect.left,
			mask_rect.top,
			mask_rect.right - mask_rect.left,
			mask_rect.bottom - mask_rect.top,
			&(pEditNode->undo_img),
			mask_rect.left,
			mask_rect.top,
			BGRA_LUT);
	}
	else{
		pEditNode->edit_img.LUT(
			mask_rect.left,
			mask_rect.top,
			mask_rect.right - mask_rect.left,
			mask_rect.bottom - mask_rect.top,
			&(pEditNode->undo_img),
			mask_rect.left,
			mask_rect.top,
			BGRA_LUT);
	}

	pEditLayerHandle->Update(&mask_rect);
	return 0;
}

void ColorBalance::UpdateImage()
{
	DWORD dwExCode;
	::GetExitCodeThread(m_hUpdateThread, &dwExCode);
	if(dwExCode == STILL_ACTIVE){
	}
	else{
		unsigned int thId;
		int i;
		for(i=0; i<3; i++){
			m_CBData.r_edit_vals[i] = m_r_edit_vals[i];
			m_CBData.g_edit_vals[i] = m_g_edit_vals[i];
			m_CBData.b_edit_vals[i] = m_b_edit_vals[i];
		}
		m_CBData.r_transfer[IE_SHADOWS] = (m_r_edit_vals[IE_SHADOWS] > 0) ? m_shadows_add : m_shadows_sub;
		m_CBData.g_transfer[IE_SHADOWS] = (m_g_edit_vals[IE_SHADOWS] > 0) ? m_shadows_add : m_shadows_sub;
		m_CBData.b_transfer[IE_SHADOWS] = (m_b_edit_vals[IE_SHADOWS] > 0) ? m_shadows_add : m_shadows_sub;

		m_CBData.r_transfer[IE_MIDTONES] = (m_r_edit_vals[IE_MIDTONES] > 0) ? m_midtones_add : m_midtones_sub;
		m_CBData.g_transfer[IE_MIDTONES] = (m_g_edit_vals[IE_MIDTONES] > 0) ? m_midtones_add : m_midtones_sub;
		m_CBData.b_transfer[IE_MIDTONES] = (m_b_edit_vals[IE_MIDTONES] > 0) ? m_midtones_add : m_midtones_sub;

		m_CBData.r_transfer[IE_HIGHLIGHTS] = (m_r_edit_vals[IE_HIGHLIGHTS] > 0) ? m_highlights_add : m_highlights_sub;
		m_CBData.g_transfer[IE_HIGHLIGHTS] = (m_g_edit_vals[IE_HIGHLIGHTS] > 0) ? m_highlights_add : m_highlights_sub;
		m_CBData.b_transfer[IE_HIGHLIGHTS] = (m_b_edit_vals[IE_HIGHLIGHTS] > 0) ? m_highlights_add : m_highlights_sub;

		m_CBData.is_preserve_luminosity = m_is_preserve_luminosity;
		m_CBData.mask_rect = m_mask_rect;
		m_CBData.BGRA_LUT = m_BGRA_LUT;
		m_CBData.pEditNode = m_pEditNode;
		m_CBData.pEditLayerHandle = m_pEditLayerHandle;

		m_hUpdateThread = (HANDLE)_beginthreadex(
			NULL,
			0,
			UpdateImageThread,
			(void*)&m_CBData,
			0,
			&thId);
	}
}

LRESULT CALLBACK ColorBalanceDlgProc(HWND hDlgWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static ColorBalance* pColorBalance;
	static int trans_index;;
	static HWND hRSlider;
	static HWND hGSlider;
	static HWND hBSlider;

	switch(msg){
		case WM_INITDIALOG:
			{
				pColorBalance = (ColorBalance*)lp;

				//slider init
				hRSlider = ::GetDlgItem(hDlgWnd, IDC_CB_R_SLIDER);
				hGSlider = ::GetDlgItem(hDlgWnd, IDC_CB_G_SLIDER);
				hBSlider = ::GetDlgItem(hDlgWnd, IDC_CB_B_SLIDER);

				::SendMessage(hRSlider, TBM_SETRANGE, 0, (LPARAM)MAKELONG(0, 200));
				::SendMessage(hRSlider, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)100);
				::SendMessage(hGSlider, TBM_SETRANGE, 0, (LPARAM)MAKELONG(0, 200));
				::SendMessage(hGSlider, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)100);
				::SendMessage(hBSlider, TBM_SETRANGE, 0, (LPARAM)MAKELONG(0, 200));
				::SendMessage(hBSlider, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)100);

				//editbox init
				HWND hEdit;
				hEdit = ::GetDlgItem(hDlgWnd, IDC_CB_R_EDIT);
				::SetWindowText(hEdit, TEXT("0"));
				hEdit = ::GetDlgItem(hDlgWnd, IDC_CB_G_EDIT);
				::SetWindowText(hEdit, TEXT("0"));
				hEdit = ::GetDlgItem(hDlgWnd, IDC_CB_B_EDIT);
				::SetWindowText(hEdit, TEXT("0"));

				//radio btn init
				trans_index = 1;
				HWND hRad = ::GetDlgItem(hDlgWnd, IDC_RADIO_SH);
				::SendMessage(hRad, BM_SETCHECK, BST_UNCHECKED, 0);
				hRad = ::GetDlgItem(hDlgWnd, IDC_RADIO_MD);
				::SendMessage(hRad, BM_SETCHECK, BST_CHECKED, 0);
				hRad = ::GetDlgItem(hDlgWnd, IDC_RADIO_HL);
				::SendMessage(hRad, BM_SETCHECK, BST_UNCHECKED, 0);

				//preserve luminosity
				HWND hCHCK = ::GetDlgItem(hDlgWnd, IDC_CHECK_PL);
				::SendMessage(hCHCK, BM_SETCHECK, BST_CHECKED, 0);

				pColorBalance->UpdateImage();
			}
			return TRUE;
		case WM_HSCROLL:
			{
				UINT nSBCode = LOWORD(wp);
				if((nSBCode == SB_THUMBPOSITION) ||
					(nSBCode == SB_THUMBTRACK))
				{
					//get slider value
					HWND hEdit;
					int nPos = HIWORD(wp);
					HWND hSlider = (HWND)lp;
					if(hSlider == hRSlider){
						hEdit = ::GetDlgItem(hDlgWnd, IDC_CB_R_EDIT);
						pColorBalance->m_r_edit_vals[trans_index] = nPos-100;
					}
					else if(hSlider == hGSlider){
						hEdit = ::GetDlgItem(hDlgWnd, IDC_CB_G_EDIT);
						pColorBalance->m_g_edit_vals[trans_index] = nPos-100;
					}
					else if(hSlider == hBSlider){
						hEdit = ::GetDlgItem(hDlgWnd, IDC_CB_B_EDIT);
						pColorBalance->m_b_edit_vals[trans_index] = nPos-100;
					}
					else{
					}

					char buf[256];
					sprintf(buf, "%+d", nPos-100);
					::SetWindowText(hEdit, buf);
					pColorBalance->UpdateImage();
				}
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
				case IDC_CHECK_PL:
					if(BST_CHECKED == ::IsDlgButtonChecked(hDlgWnd, IDC_CHECK_PL)){
						pColorBalance->m_is_preserve_luminosity = true;
					}
					else{
						pColorBalance->m_is_preserve_luminosity = false;
					}
					pColorBalance->UpdateImage();
					break;
				case IDC_RADIO_SH:
				case IDC_RADIO_MD:
				case IDC_RADIO_HL:
					//get transfer mode
					if(BST_CHECKED == ::IsDlgButtonChecked(hDlgWnd, IDC_RADIO_SH)){
						trans_index = 0;
					}
					else if(BST_CHECKED == ::IsDlgButtonChecked(hDlgWnd, IDC_RADIO_MD)){
						trans_index = 1;
					}
					else if(BST_CHECKED == ::IsDlgButtonChecked(hDlgWnd, IDC_RADIO_HL)){
						trans_index = 2;
					}
					else{
						assert(0);
					}

					//set slider pos and edit box
					{
						char buf[256];					

						int val;
						HWND hEdit;
						val = pColorBalance->m_r_edit_vals[trans_index] + 100;
						::SendMessage(hRSlider, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)val);
						hEdit = ::GetDlgItem(hDlgWnd, IDC_CB_R_EDIT);
						sprintf(buf, "%+d", pColorBalance->m_r_edit_vals[trans_index]);
						::SetWindowText(hEdit, buf);

						val = pColorBalance->m_g_edit_vals[trans_index] + 100;
						::SendMessage(hGSlider, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)val);
						hEdit = ::GetDlgItem(hDlgWnd, IDC_CB_G_EDIT);
						sprintf(buf, "%+d", pColorBalance->m_g_edit_vals[trans_index]);
						::SetWindowText(hEdit, buf);

						val = pColorBalance->m_b_edit_vals[trans_index] + 100;
						::SendMessage(hBSlider, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)val);
						hEdit = ::GetDlgItem(hDlgWnd, IDC_CB_B_EDIT);
						sprintf(buf, "%+d", pColorBalance->m_b_edit_vals[trans_index]);
						::SetWindowText(hEdit, buf);
					}
					break;
				case IDC_CB_R_EDIT:
				case IDC_CB_G_EDIT:
				case IDC_CB_B_EDIT:
					if(HIWORD(wp) == EN_UPDATE){
						char buf[256];
						HWND hEdit = ::GetDlgItem(hDlgWnd, LOWORD(wp));
						::GetWindowText(hEdit, buf, 256);

						int val = atoi(buf);

						int id = LOWORD(wp);
						HWND hSlider;
						if(id == IDC_CB_R_EDIT){
							hSlider = ::GetDlgItem(hDlgWnd, IDC_CB_R_EDIT);
						}
						else if(id == IDC_CB_G_EDIT){
							hSlider = ::GetDlgItem(hDlgWnd, IDC_CB_G_EDIT);
						}
						else if(id == IDC_CB_B_EDIT){
							hSlider = ::GetDlgItem(hDlgWnd, IDC_CB_B_EDIT);
						}
						::SendMessage(hSlider, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)(val+100));
					}
					break;
				default:
					return FALSE;
			}
		default:
			return FALSE;
	}
	return TRUE;
}