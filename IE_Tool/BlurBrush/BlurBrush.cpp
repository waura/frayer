// BlurBrush.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include "resource.h"
#include "BlurBrush.h"

// for detect memory leak
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <cstdlib>
	#include <crtdbg.h>
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

extern HMODULE g_hLibrary;

#define DISPLAY_NAME_JA "ぼかし"
#define DISPLAY_NAME DISPLAY_NAME_JA


IETOOL_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::TOOL;
}

IETOOL_EXPORT IIETool* CreateIETool()
{
	return new BlurBrush;
}

IETOOL_EXPORT void ReleaseIETool(IIETool** ppTool)
{
	if (*ppTool) {
		delete (*ppTool);
		(*ppTool) = NULL;
	}
}

BlurBrush::BlurBrush()
{
	strcpy_s(m_name, MAX_IE_MODULE_NAME, "BlurBrush");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

BlurBrush::~BlurBrush()
{
	if (m_buttonImg) {
		::DeleteObject(m_buttonImg);
		m_buttonImg = NULL;
	}
}

HBITMAP BlurBrush::GetButtonImg()
{
	if (m_buttonImg) {
		::DeleteObject(m_buttonImg);
		m_buttonImg = NULL;
	}

	m_buttonImg = ::LoadBitmap((HINSTANCE)g_hLibrary, MAKEINTRESOURCE(IDB_BUTTON_BMP));
	return m_buttonImg;
}

void BlurBrush::OnSetToolEditWnd(IToolEditWndHandle *pHandle)
{
}

void BlurBrush::OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	m_pEditLayer = NULL;
	m_pEditLayerHandle = NULL;
	m_pEditNode = NULL;

	ImgFile_Ptr pFile = m_pImgEdit->GetActiveFile();
	if(pFile) return;

	IImgLayer_weakPtr _sl = pFile->GetSelectLayer();
	if (_sl.expired()) {
		OutputError::Alert(IE_ERROR_SELECT_LAYER_DONT_EXIST);
	}

	IImgLayer_Ptr sl = _sl.lock();
	if (sl->GetLayerType() != IE_LAYER_TYPE::NORMAL_LAYER ){
		OutputError::Alert(IE_ERROR_CANT_EDIT_LAYER);
		return;
	}
	m_pEditLayer = std::dynamic_pointer_cast<ImgLayer>(sl);

	IEBrush_Ptr pBrush = m_pImgEdit->GetSelectBrush();
	if(!pBrush){
		OutputError::Alert(IE_ERROR_SELECT_BRUSH_DONT_EXIST);
		return;
	}

	m_pEditLayerHandle = (EditLayerHandle*) pFile->CreateImgFileHandle(IFH_EDIT_LAYER);
	m_pEditLayerHandle->SetHandleName( DISPLAY_NAME );
	if(pFile->DoImgFileHandle( m_pEditLayerHandle )){
		m_pEditNode = m_pEditLayerHandle->CreateEditNode();

		m_blur_alpha_map = cvCreateImage(
			cvSize(m_pEditNode->blt_rect.right - m_pEditNode->blt_rect.left,
			m_pEditNode->blt_rect.bottom - m_pEditNode->blt_rect.top), IPL_DEPTH_8U, 1);
		cvSet(m_blur_alpha_map, cvScalar(0));

		//init blt rect
		m_pEditNode->blt_rect.right = m_pEditNode->blt_rect.left = 0;
		m_pEditNode->blt_rect.bottom = m_pEditNode->blt_rect.top = 0;

		Blur( lpd );
	}
}

void BlurBrush::OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	if(m_pEditLayerHandle){
		Blur( lpd );
	}
}

void BlurBrush::OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	if(m_pEditLayerHandle){
		m_pEditLayerHandle->EndEdit();
	}

	if(m_blur_alpha_map){
		cvReleaseImage(&m_blur_alpha_map);
	}
}

void BlurBrush::Blur(const LPIE_INPUT_DATA lpd)
{
	IEBrush_Ptr brush = m_pImgEdit->GetSelectBrush();
	if(!brush) return;

	RECT draw_rc;
	brush->BltBrushData(m_blur_alpha_map, lpd, &draw_rc);

	//add draw rect
	if(m_pEditNode->blt_rect.top == m_pEditNode->blt_rect.bottom){
		//if blt_rect is empy, init
		m_pEditNode->blt_rect = draw_rc;
	}
	else{
		OrRect(&draw_rc, &(m_pEditNode->blt_rect), &(m_pEditNode->blt_rect));
	}

	//
}