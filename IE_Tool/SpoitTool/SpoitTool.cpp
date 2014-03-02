// SpoitTool.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "resource.h"
#include "SpoitTool.h"

// for detect memory leak
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <cstdlib>
	#include <crtdbg.h>
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

static HMODULE g_hLibrary;

#define DISPLAY_NAME_JA "スポイトツール"
#define DISPLAY_NAME DISPLAY_NAME_JA

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

IETOOL_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::TOOL;
}

IETOOL_EXPORT IIETool* CreateIETool()
{
	return new SpoitTool;
}

IETOOL_EXPORT void ReleaseIETool(IIETool** ppTool)
{
	if (*ppTool) {
		delete (*ppTool);
		(*ppTool) = NULL;
	}
}

SpoitTool::SpoitTool()
{
	strcpy_s(m_name, MAX_IE_MODULE_NAME, "SpoitTool");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

SpoitTool::~SpoitTool()
{
	if (m_buttonImg) {
		::DeleteObject(m_buttonImg);
		m_buttonImg = NULL;
	}
}

HBITMAP SpoitTool::GetButtonImg()
{
	if (m_buttonImg) {
		::DeleteObject(m_buttonImg);
		m_buttonImg = NULL;
	}

	m_buttonImg = ::LoadBitmap((HINSTANCE)g_hLibrary, MAKEINTRESOURCE(IDB_BUTTON_BMP));
	return m_buttonImg;
}

void SpoitTool::OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	SpoitColor(lpd);
}

void SpoitTool::OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	SpoitColor(lpd);
}

void SpoitTool::OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	SpoitColor(lpd);
}

void SpoitTool::OnMouseRButtonDown(UINT nFlags, const LPPOINT lpt)
{
}

void SpoitTool::OnMouseRButtonUp(UINT nFlags, const LPPOINT lpt)
{
}

void SpoitTool::SpoitColor(const LPIE_INPUT_DATA lpd)
{
	ImgFile_Ptr file = m_pImgEdit->GetActiveFile();
	if(!file) return;

	file->ConvertPosWndToImg(lpd);
	
	CvSize f_size = file->GetImgSize();
	if(((0 <= lpd->x) && (lpd->x < f_size.width)) &&
		((0 <= lpd->y) && (lpd->y < f_size.height)))
	{
		UCvPixel pixel;
		file->GetDrawImgPixel(lpd->x, lpd->y, &pixel);

		//選択中の色を変える。
		m_pImgEdit->SetSelectFGColor(pixel);
	}
}