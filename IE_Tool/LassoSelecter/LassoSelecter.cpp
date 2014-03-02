// LassoSelecter.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "resource.h"
#include "LassoSelecter.h"

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

#define DISPLAY_NAME_JA "投げ縄選択"
#define DISPLAY_NAME DISPLAY_NAME_JA

IETOOL_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::TOOL;
}

IETOOL_EXPORT IIETool* CreateIETool()
{
	return new LassoSelecter;
}

IETOOL_EXPORT void ReleaseIETool(IIETool** ppTool)
{
	if (*ppTool) {
		delete (*ppTool);
		(*ppTool) = NULL;
	}
}


LassoSelecter::LassoSelecter()
{
	m_isSelected = false;
	strcpy_s(m_name, MAX_IE_MODULE_NAME, "LassoSelecter");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

LassoSelecter::~LassoSelecter()
{
	if (m_buttonImg) {
		::DeleteObject(m_buttonImg);
		m_buttonImg = NULL;
	}
}

HBITMAP LassoSelecter::GetButtonImg()
{
	if (m_buttonImg) {
		::DeleteObject(m_buttonImg);
		m_buttonImg = NULL;
	}

	m_buttonImg = ::LoadBitmap((HINSTANCE)g_hLibrary, MAKEINTRESOURCE(IDB_BUTTON_BMP));
	return m_buttonImg;
}

void LassoSelecter::OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	m_beforePt.x = m_nowPt.x = lpd->x;
	m_beforePt.y = m_nowPt.y = lpd->y;

	m_isSelected = false;
}

void LassoSelecter::OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	if(m_isSelected == false){
		m_nowPt.x = lpd->x;
		m_nowPt.y = lpd->y;
		Push_Line(lpd);
	}
}

void LassoSelecter::OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	if(m_isSelected == false){
		//格納線分の消去
		m_CvLine_Vec.clear();
	}
}

void LassoSelecter::Push_Line(LPIE_INPUT_DATA lpd){
	//現在の点から線分を生成
	CvLine2D64f line;
	CvPoint2D64f pt;

	pt.x = lpd->x;
	pt.y = lpd->y;
	line.start = m_beforePt;
	line.end = m_beforePt = pt;

	//交差判定、交差したら選択領域を生成して終了する
	for(int i=0; i < m_CvLine_Vec.size(); i++){
		if(IsIntersect(&(m_CvLine_Vec[i]), &line)){
			//更新範囲を求める
			RECT rect;
			GetLinesRect(&rect);

			//交差していたらその交点を求める
			CvPoint2D64f p;
			IntersectPoint(&p, &(m_CvLine_Vec[i]), &line);
			//選択領域が交点を含み閉じた形になるようにする
			m_CvLine_Vec[i].start = p;
			line.end = p;
			//最後の線分の追加
			//スピンロック
			while(m_isLineVecLock);
			m_CvLine_Vec.push_back(line);
			
			//閉じた図形にかかわらない線分を消去
			
			for(int j=0; j < i; j++){
				CvLine_Vec::iterator itr = m_CvLine_Vec.begin();
				m_CvLine_Vec.erase(itr);
			}

			//選択領域の作成
			CreateMask(DISPLAY_NAME, &rect);

			//スピンロック
			while(m_isLineVecLock);
			m_CvLine_Vec.clear();
			m_isSelected = true;
			return;
		}
	}

	//スピンロック
	while(m_isLineVecLock);
	m_CvLine_Vec.push_back(line);
}
