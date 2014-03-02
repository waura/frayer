// RectImgSelecter.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "resource.h"
#include "RectImgSelecter.h"

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

#define DISPLAY_NAME_JA "画像矩形選択"
#define DISPLAY_NAME DISPLAY_NAME_JA

IETOOL_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::TOOL;
}

IETOOL_EXPORT IIETool* CreateIETool()
{
	return new RectImgSelecter;
}

IETOOL_EXPORT void ReleaseIETool(IIETool** ppTool)
{
	if (*ppTool) {
		delete (*ppTool);
		(*ppTool) = NULL;
	}
}


RectImgSelecter::RectImgSelecter()
{
	m_pEditMask = NULL;
	strcpy_s(m_name, MAX_IE_MODULE_NAME, "RectImgSelecter");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

RectImgSelecter::~RectImgSelecter()
{
	if (m_buttonImg) {
		::DeleteObject(m_buttonImg);
		m_buttonImg = NULL;
	}
}

HBITMAP RectImgSelecter::GetButtonImg()
{
	if (m_buttonImg) {
		::DeleteObject(m_buttonImg);
		m_buttonImg = NULL;
	}

	m_buttonImg = ::LoadBitmap((HINSTANCE)g_hLibrary, MAKEINTRESOURCE(IDB_BUTTON_BMP));
	return m_buttonImg;
}

void RectImgSelecter::OnDraw(HDC hdc)
{
	IMakeRectImgSelecter::OnDraw(hdc);
}

void RectImgSelecter::OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	IMakeRectImgSelecter::OnMouseLButtonDown(nFlags, lpd);
}

void RectImgSelecter::OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	IMakeRectImgSelecter::OnMouseLDrag(nFlags, lpd);
}

void RectImgSelecter::OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	IMakeRectImgSelecter::OnMouseLButtonUp(nFlags, lpd);
}

void RectImgSelecter::OnKeyboardDown(UINT nChar, UINT optChar)
{
	if(nChar == IE_KEY_RETURN){
		if(IsMakedRect()){
			ImgFile_Ptr file = m_pImgEdit->GetActiveFile();
			if(!file) return;
			ImgMask_Ptr mask = file->GetSelectMask();
			if(!mask) return;


			EditMaskHandle* pEditMaskHandle = (EditMaskHandle*)file->CreateImgFileHandle( IFH_EDIT_MASK );
			strcpy_s(pEditMaskHandle->handle_name, MAX_IMG_FILE_HANDLE_NAME, DISPLAY_NAME);
			file->DoImgFileHandle( pEditMaskHandle );

			RECT mask_rect;
			GetDragPointRect(&mask_rect);

			EditNode* editNode = pEditMaskHandle->CreateEditNode(&mask_rect);

			IplExtFillRectMask(
				&(editNode->edit_img),
				255,
				editNode->blt_rect.left - editNode->node_rect.left,
				editNode->blt_rect.top - editNode->node_rect.top,
				editNode->blt_rect.right - editNode->blt_rect.left,
				editNode->blt_rect.bottom - editNode->blt_rect.top);

			//update mask image
			pEditMaskHandle->Update( &mask_rect );

			ClearDragPoint();

			//
			pEditMaskHandle->EndEdit();
		}
	}
}
