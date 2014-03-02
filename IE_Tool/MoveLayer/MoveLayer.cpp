#include "resource.h"
#include "MoveLayer.h"

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

#define DISPLAY_NAME_JA "レイヤー平行移動"
#define DISPLAY_NAME DISPLAY_NAME_JA

IETOOL_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::TOOL;
}

IETOOL_EXPORT IIETool* CreateIETool()
{
	return new MoveLayer;
}

IETOOL_EXPORT void ReleaseIETool(IIETool** ppTool)
{
	if (*ppTool) {
		delete (*ppTool);
		(*ppTool) = NULL;
	}
}


MoveLayer::MoveLayer()
{
	strcpy_s(m_name, MAX_IE_MODULE_NAME, "MoveLayer");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

MoveLayer::~MoveLayer()
{
	if (m_buttonImg) {
		::DeleteObject(m_buttonImg);
	}
}

HBITMAP MoveLayer::GetButtonImg()
{
	if (m_buttonImg) {
		::DeleteObject(m_buttonImg);
		m_buttonImg = NULL;
	}

	m_buttonImg = ::LoadBitmap((HINSTANCE)g_hLibrary, MAKEINTRESOURCE(IDB_BUTTON_BMP));
	return m_buttonImg;
}

void MoveLayer::OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	move_X = 0;
	move_Y = 0;

	startPt.x = beforPt.x = lpd->x;
	startPt.y = beforPt.y = lpd->y;

	ImgFile_Ptr f = m_pImgEdit->GetActiveFile();
	if(!f) return;
	m_pEditLayer = f->GetSelectLayer().lock();
	if(!m_pEditLayer) return;

	//
	m_pMoveLayerHandle = (MoveLayerHandle*) f->CreateImgFileHandle(IFH_MOVE_LAYER);
	strcpy_s(m_pMoveLayerHandle->handle_name, MAX_IMG_FILE_HANDLE_NAME, DISPLAY_NAME);
	f->DoImgFileHandle( m_pMoveLayerHandle );

	//
	m_pEditLayer->GetLayerRect(&beforRc);
}

void MoveLayer::OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	ImgFile_Ptr f = m_pImgEdit->GetActiveFile();
	if(!f) return;
	m_pEditLayer = f->GetSelectLayer().lock();
	if(!m_pEditLayer) return;

	fMoveLayer(lpd, &beforPt);

	beforPt.x = lpd->x;
	beforPt.y = lpd->y;
}

void MoveLayer::OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	ImgFile_Ptr f = m_pImgEdit->GetActiveFile();
	if(!f) return;
	m_pEditLayer = f->GetSelectLayer().lock();
	if(!m_pEditLayer) return;

	fMoveLayer(lpd, &beforPt);
	
	//レイヤー領域拡張
	m_pEditLayer->ExtendLayer();
	
	m_pMoveLayerHandle->Update();
}

void MoveLayer::fMoveLayer(LPIE_INPUT_DATA lpd, LPPOINT beforPt)
{
	ImgFile_Ptr f = m_pImgEdit->GetActiveFile();
	if(f){
		int shiftX = lpd->x - beforPt->x;
		int shiftY = lpd->y - beforPt->y;

		move_X += shiftX;
		move_Y += shiftY;

		//レイヤーの移動
		m_pMoveLayerHandle->SetMoveXY(move_X, move_Y);

		//画像の更新
		m_pMoveLayerHandle->Update();
	}
}