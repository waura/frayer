// EraserSelecter.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "resource.h"
#include "EraserSelecter.h"

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

#define DISPLAY_NAME_JA "画像選択消しゴム"
#define DISPLAY_NAME DISPLAY_NAME_JA

IETOOL_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::TOOL;
}

IETOOL_EXPORT IIETool* CreateIETool()
{
	return new EraserSelecter;
}

IETOOL_EXPORT void ReleaseIETool(IIETool** ppTool)
{
	if (*ppTool) {
		delete (*ppTool);
		(*ppTool) = NULL;
	}
}


EraserSelecter::EraserSelecter()
{
	m_brush_alpha = 100;
	m_pEditMask = NULL;

	strcpy_s(m_name, MAX_IE_MODULE_NAME, "EraserSelecter");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

EraserSelecter::~EraserSelecter()
{
	if (m_buttonImg) {
		::DeleteObject(m_buttonImg);
		m_buttonImg = NULL;
	}
}

HBITMAP EraserSelecter::GetButtonImg()
{
	if (m_buttonImg) {
		::DeleteObject(m_buttonImg);
		m_buttonImg = NULL;
	}

	m_buttonImg = ::LoadBitmap((HINSTANCE)g_hLibrary, MAKEINTRESOURCE(IDB_BUTTON_BMP));
	return m_buttonImg;
}

void EraserSelecter::OnSetToolEdtiWnd(IToolEditWndHandle* pHandle)
{
	pHandle->AddTrackbar("不透明度", 0, 100, 100, &m_brush_alpha);
}

void EraserSelecter::OnSelect()
{
	m_pImgEdit->SetIECursor(IE_CURSOR_ID::BRUSH_CURSOR);

	IImgSelecter::OnSelect();
}

void EraserSelecter::OnNeutral()
{
	m_pImgEdit->SetIECursor(IE_CURSOR_ID::NORMAL_CURSOR);

	IImgSelecter::OnNeutral();
}

void EraserSelecter::OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	ImgFile_Ptr file = m_pImgEdit->GetActiveFile();
	if (!file) return;
	m_pEditMask = file->GetSelectMask();
	if (!m_pEditMask) return;

	m_pEditMaskHandle = (EditMaskHandle*)file->CreateImgFileHandle(IFH_EDIT_MASK);
	strcpy_s(m_pEditMaskHandle->handle_name, MAX_IMG_FILE_HANDLE_NAME, DISPLAY_NAME);
	if (file->DoImgFileHandle( m_pEditMaskHandle )) {
		m_editNode = m_pEditMaskHandle->CreateEditNode();
		m_editNode->fourcc = IE_MASK_SYNTH_FOURCC('s', 'u', 'b', ' ');

		m_brush_alpha_map = cvCreateImage(
			cvSize(m_editNode->blt_rect.right - m_editNode->blt_rect.left, 
			m_editNode->blt_rect.bottom - m_editNode->blt_rect.top), IPL_DEPTH_8U, 1);
		cvSet(m_brush_alpha_map, cvScalar(0));

		//init blt rect
		m_editNode->blt_rect.top = m_editNode->blt_rect.left = 0;
		m_editNode->blt_rect.right = m_editNode->blt_rect.bottom = 0;

		Select(lpd);
	}
	else {
		file->ReleaseImgFileHandle(m_pEditMaskHandle);
		m_pEditMaskHandle = NULL;
	}
}

void EraserSelecter::OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	if (m_pEditMaskHandle) {
		Select(lpd);
	}
}

void EraserSelecter::OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	if (m_pEditMaskHandle) {
		m_pEditMaskHandle->EndEdit();
	}
	
	if (m_brush_alpha_map) {
		cvReleaseImage(&m_brush_alpha_map);
	}
}

void EraserSelecter::Select(LPIE_INPUT_DATA lpd)
{
	if(m_brush_alpha_map == NULL) return;

	ImgFile_Ptr file = m_pImgEdit->GetActiveFile();
	if(!file) return;
	ImgMask_Ptr mask = file->GetSelectMask();
	if(!mask) return;
	IEBrush_Ptr brush = m_pImgEdit->GetSelectBrush();
	if(!brush) return;

	//convert from screen axis to image axis
	IE_INPUT_DATA input_data = (*lpd);
	input_data.x -= m_editNode->node_rect.left;
	input_data.y -= m_editNode->node_rect.top;

	RECT draw_rc;
	if (brush->GetBrushType() == IE_BRUSH_TYPE::IE_EM_BRUSH) {
		IEEMBrush_Ptr _brush = std::dynamic_pointer_cast<IEEMBrush>(brush);

		EraserSelecterCopyOp copyOp;
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
	m_pEditMaskHandle->Update( &draw_rc );
}