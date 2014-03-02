// ViewRot.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "resource.h"
#include "ViewRot.h"
#include "math.h"

// for detect memory leak
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <cstdlib>
	#include <crtdbg.h>
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

static HMODULE g_hLibrary;

#define DISPLAY_NAME_JA "表示回転"
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
	return new ViewRot;
}

IETOOL_EXPORT void ReleaseIETool(IIETool** ppTool)
{
	if (*ppTool) {
		delete (*ppTool);
		(*ppTool) = NULL;
	}
}

ViewRot::ViewRot()
{
	strcpy_s(m_name, MAX_IE_MODULE_NAME, "ViewRot");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

ViewRot::~ViewRot()
{
	if (m_buttonImg) {
		::DeleteObject(m_buttonImg);
	}
}

HBITMAP ViewRot::GetButtonImg()
{
	if (m_buttonImg) {
		::DeleteObject(m_buttonImg);
		m_buttonImg = NULL;
	}

	m_buttonImg = ::LoadBitmap((HINSTANCE)g_hLibrary, MAKEINTRESOURCE(IDB_BUTTON_BMP));
	return m_buttonImg;
}

void ViewRot::OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	startPt.x = lpd->x;
	startPt.y = lpd->y;

	ImgFile_Ptr f = m_pImgEdit->GetActiveFile();
	if(f){
		start_rot = f->GetViewRot();
		CvPoint2D64f tmp = f->GetViewCenterPos();
		f->ConvertPosImgToWnd(&tmp, &center);
	}
}

void ViewRot::OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	double rot = GetRot(&startPt, lpd);

	ImgFile_Ptr f = m_pImgEdit->GetActiveFile();
	if(f){
		f->SetViewRot(rot + start_rot);

		//表示画像に適用
		LPUPDATE_DATA data = f->CreateUpdateData();
		data->isAll = true;
		f->PushUpdateData(data);
	}
}

void ViewRot::OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	double rot = GetRot(&startPt, lpd);

	ImgFile_Ptr f = m_pImgEdit->GetActiveFile();
	if(f){
		f->SetViewRot(rot + start_rot);

		//表示画像に適用
		LPUPDATE_DATA data = f->CreateUpdateData();
		data->isAll = true;
		f->PushUpdateData(data);
	}

	m_isLDrag = false;
}

void ViewRot::OnMouseRButtonDown(UINT nFlags, const LPPOINT lpt)
{
}

void ViewRot::OnMouseRButtonUp(UINT nFlags, const LPPOINT lpt)
{
}

double ViewRot::GetRot(LPPOINT start_lpt, LPIE_INPUT_DATA end_lpd)
{
	ImgFile_Ptr f = m_pImgEdit->GetActiveFile();
	if(f){
		if((start_lpt->x - center.x == 0) || (end_lpd->x - center.x == 0)){
			return 0.0;
		}

		double rad_s = atan2((double)start_lpt->y - center.y, (double)start_lpt->x - center.x);
		double rad_e = atan2((double)end_lpd->y - center.y, (double)end_lpd->x - center.x);
		double rad = -(rad_e - rad_s);

		//if(rad > CV_PI || rad < -CV_PI){
		//	int i = rad / fabs(rad);
		//	rad = 2.0*CV_PI - fabs(rad);
		//	rad *= i;
		//}
		
		double rot = 360.0*rad/(2.0*CV_PI);

		return rot;
	}
	return 0.0;
}