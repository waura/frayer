// ViewParallelTrans.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "resource.h"
#include "ViewParallelTrans.h"

// for detect memory leak
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <cstdlib>
	#include <crtdbg.h>
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

static HMODULE g_hLibrary;

#define DISPLAY_NAME_JA "表示位置平行移動"
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
	return new ViewParallelTrans;
}

IETOOL_EXPORT void ReleaseIETool(IIETool** ppTool)
{
	if (*ppTool) {
		delete (*ppTool);
		(*ppTool) = NULL;
	}
}


ViewParallelTrans::ViewParallelTrans()
{
	strcpy_s(m_name, MAX_IE_MODULE_NAME, "ViewParallelTrans");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

ViewParallelTrans::~ViewParallelTrans()
{
	if (m_buttonImg) {
		::DeleteObject(m_buttonImg);
		m_buttonImg = NULL;
	}
}

HBITMAP ViewParallelTrans::GetButtonImg()
{
	if (m_buttonImg) {
		::DeleteObject(m_buttonImg);
		m_buttonImg = NULL;
	}

	m_buttonImg = ::LoadBitmap((HINSTANCE)g_hLibrary, MAKEINTRESOURCE(IDB_BUTTON_BMP));
	return m_buttonImg;
}

void ViewParallelTrans::OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	beforPt.x = lpd->x;
	beforPt.y = lpd->y;
}

void ViewParallelTrans::OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	ImgFile_Ptr f = m_pImgEdit->GetActiveFile();
	if(f){
		//スクロールを表示画像に適用
		CvPoint2D64f pos;
		f->GetViewPos( &pos );
		int view_size = f->GetViewSize();
		double addx = (lpd->x - beforPt.x)/(view_size/100.0);
		double addy = (lpd->y - beforPt.y)/(view_size/100.0);
		double rot = f->GetViewRot() * CV_PI/180.0;
		
		if( f->IsViewFlipH() )
			pos.x -= cos(rot) * addx - sin(rot) * addy;
		else
			pos.x += cos(rot) * addx - sin(rot) * addy;

		if( f->IsViewFlipV() )
			pos.y -= sin(rot) * addx + cos(rot) * addy;
		else
			pos.y += sin(rot) * addx + cos(rot) * addy;

		f->SetViewPos( &pos );

		beforPt.x = lpd->x;
		beforPt.y = lpd->y;

		LPUPDATE_DATA data = f->CreateUpdateData();
		data->isAll = true;
		f->PushUpdateData(data);
	}
}

void ViewParallelTrans::OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	ImgFile_Ptr f = m_pImgEdit->GetActiveFile();
	if(f){
		LPUPDATE_DATA data = f->CreateUpdateData();
		data->isAll = true;
		f->PushUpdateData(data);
	}
}

void ViewParallelTrans::OnMouseRButtonDown(UINT nFlags, const LPPOINT lpt)
{
}

void ViewParallelTrans::OnMouseRButtonUp(UINT nFlags, const LPPOINT lpt)
{
}