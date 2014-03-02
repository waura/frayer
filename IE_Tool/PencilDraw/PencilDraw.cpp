// PencilDraw.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "resource.h"
#include "PencilDraw.h"

// for detect memory leak
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <cstdlib>
	#include <crtdbg.h>
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

static HMODULE g_hLibrary;


#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	g_hLibrary = hModule;
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

#define DISPLAY_NAME_JA "鉛筆"
#define DISPLAY_NAME DISPLAY_NAME_JA

IETOOL_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::TOOL;
}

IETOOL_EXPORT IIETool* CreateIETool()
{
	return new PencilDraw;
}

IETOOL_EXPORT void ReleaseIETool(IIETool** ppTool)
{
	if (*ppTool) {
		delete (*ppTool);
	}
}


PencilDraw::PencilDraw()
{
	strcpy_s(m_name, MAX_IE_MODULE_NAME, "PencilDraw");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

PencilDraw::~PencilDraw()
{
	if (m_buttonImg) {
		::DeleteObject(m_buttonImg);
		m_buttonImg = NULL;
	}
}

HBITMAP PencilDraw::GetButtonImg()
{
	if (m_buttonImg) {
		::DeleteObject(m_buttonImg);
		m_buttonImg = NULL;
	}

	m_buttonImg = ::LoadBitmap((HINSTANCE)g_hLibrary, MAKEINTRESOURCE(IDB_BUTTON_BMP));
	return m_buttonImg;
}

void PencilDraw::OnSetToolEditWnd(IToolEditWndHandle* pHandle)
{
	m_brush_alpha = 100;
	pHandle->AddTrackbar("不透明度", 0, 100, 100, &m_brush_alpha);
}

void PencilDraw::OnSelect()
{
	IBrushDraw::OnSelect();
}

void PencilDraw::OnNeutral()
{
	IBrushDraw::OnNeutral();
}

void PencilDraw::OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	m_pEditLayer = NULL;
	m_pEditLayerHandle = NULL;
	m_editNode = NULL;

	ImgFile_Ptr file = m_pImgEdit->GetActiveFile();
	if(!file) return;

	IImgLayer_weakPtr _sl = file->GetSelectLayer();
	if (_sl.expired()) {
		OutputError::Alert(IE_ERROR_SELECT_LAYER_DONT_EXIST);
		return;
	}

	IImgLayer_Ptr sl = _sl.lock();
	if (sl->GetLayerType() != IE_LAYER_TYPE::NORMAL_LAYER) {
		OutputError::Alert(IE_ERROR_CANT_EDIT_LAYER);
		return;
	}
	m_pEditLayer = std::dynamic_pointer_cast<ImgLayer>(sl);

	IEBrush_Ptr brush = m_pImgEdit->GetSelectBrush();
	if (!brush) {
		OutputError::Alert(IE_ERROR_SELECT_BRUSH_DONT_EXIST);
		return;
	}

	m_pEditLayerHandle = (EditLayerHandle*)file->CreateImgFileHandle(IFH_EDIT_LAYER );
	m_pEditLayerHandle->SetHandleName( DISPLAY_NAME );
	if (file->DoImgFileHandle( m_pEditLayerHandle )) {
		m_editNode = m_pEditLayerHandle->CreateEditNode();

		//init blt rect
		m_editNode->blt_rect.top = m_editNode->blt_rect.left = 0;
		m_editNode->blt_rect.right = m_editNode->blt_rect.bottom = 0;

		DrawPencil(lpd);
	}
}

void PencilDraw::OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	if(m_pEditLayerHandle){
		DrawPencil(lpd);
	}
}

void PencilDraw::OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	if(m_pEditLayerHandle){
		m_pEditLayerHandle->EndEdit();
	}
}

void PencilDraw::DrawPencil(LPIE_INPUT_DATA lpd)
{
	IEBrush_Ptr brush = m_pImgEdit->GetSelectBrush();
	if(!brush) return;

	IEColor color = m_pImgEdit->GetSelectFGColor();

	//convert from screen axis to image axis
	IE_INPUT_DATA input_data = (*lpd);
	input_data.x -= m_editNode->node_rect.left;
	input_data.y -= m_editNode->node_rect.top;

	RECT draw_rc;
	if (brush->GetBrushType() == IE_BRUSH_TYPE::IE_EM_BRUSH) {
		IEEMBrush_Ptr _brush = std::dynamic_pointer_cast<IEEMBrush>(brush);

		PencilDrawCopyOp copyOp;
		copyOp.SetColor(&color);
		_brush->BltBrushData(
			copyOp,
			&(m_editNode->edit_img),
			&input_data,
			&draw_rc);
	}

	if (draw_rc.top == draw_rc.bottom)
		return;

	//convert from image axis to screen axis
	draw_rc.left += m_editNode->node_rect.left;
	draw_rc.top += m_editNode->node_rect.top;
	draw_rc.right += m_editNode->node_rect.left;
	draw_rc.bottom += m_editNode->node_rect.top;

	//add draw rect
	if(m_editNode->blt_rect.top == m_editNode->blt_rect.bottom){
		//if blt_rect is empty, init
		m_editNode->blt_rect = draw_rc;
	}
	else{
		OrRect(&draw_rc, &(m_editNode->blt_rect), &(m_editNode->blt_rect));
	}

	//update layer image
	m_pEditLayerHandle->Update(&draw_rc);
}