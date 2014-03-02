// EllipseImgSelecter.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "resource.h"
#include "EllipseImgSelecter.h"

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

#define DISPLAY_NAME_JA "楕円画像選択"
#define DISPLAY_NAME DISPLAY_NAME_JA


IETOOL_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::TOOL;
}

IETOOL_EXPORT IIETool* CreateIETool()
{
	return new EllipseImgSelecter;
}

IETOOL_EXPORT void ReleaseIETool(IIETool** ppTool)
{
	if (*ppTool) {
		delete (*ppTool);
		(*ppTool) = NULL;
	}
}


EllipseImgSelecter::EllipseImgSelecter()
{
	strcpy_s(m_name, MAX_IE_MODULE_NAME, "EllipseImgSelecter");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

EllipseImgSelecter::~EllipseImgSelecter()
{
	if (m_buttonImg) {
		::DeleteObject(m_buttonImg);
		m_buttonImg = NULL;
	}
}

HBITMAP EllipseImgSelecter::GetButtonImg()
{
	if (m_buttonImg) {
		::DeleteObject(m_buttonImg);
		m_buttonImg = NULL;
	}

	m_buttonImg = ::LoadBitmap((HINSTANCE)g_hLibrary, MAKEINTRESOURCE(IDB_BUTTON_BMP));
	return m_buttonImg;
}

void EllipseImgSelecter::OnDraw(HDC hdc)
{
	IMakeRectImgSelecter::OnDraw(hdc);

	ImgFile_Ptr pEditFile = m_pImgEdit->GetActiveFile();
	if(pEditFile == NULL) return;

	if(IsMakedRect()){
		RECT rc, crc;
		GetDragPointRect(&rc);
		pEditFile->ConvertPosImgToWnd(&rc, &crc);

		//set pen
		HPEN hOldPen = (HPEN)::SelectObject(hdc, m_hPen);
		::SelectObject(hdc, GetStockObject(NULL_BRUSH));

		::Ellipse(hdc, crc.left, crc.top, crc.right, crc.bottom);

		//reset pen
		::SelectObject(hdc, hOldPen);
	}
}

void EllipseImgSelecter::OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	IMakeRectImgSelecter::OnMouseLButtonDown(nFlags, lpd);
}

void EllipseImgSelecter::OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	IMakeRectImgSelecter::OnMouseLDrag(nFlags, lpd);
}

void EllipseImgSelecter::OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	IMakeRectImgSelecter::OnMouseLButtonUp(nFlags, lpd);
}

void EllipseImgSelecter::OnKeyboardDown(UINT nChar, UINT optChar)
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

			IplExtFillEllipseMask(
				&(editNode->edit_img),
				255,
				mask_rect.left - editNode->node_rect.left,
				mask_rect.top - editNode->node_rect.top,
				mask_rect.right - mask_rect.left,
				mask_rect.bottom - mask_rect.top);

			//update mask image
			pEditMaskHandle->Update( &mask_rect );

			ClearDragPoint();

			//
			pEditMaskHandle->EndEdit();
		}
	}
}