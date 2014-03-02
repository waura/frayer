// E_Diffuse.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//
#include "stdafx.h"
#include "E_Diffuse.h"

#define DEFAULT_SEED 1
#define DEFAULT_RANGE 1

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

// for detect memory leak
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <cstdlib>
	#include <crtdbg.h>
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#define DISPLAY_NAME "ディフューズ"

EFFECT_EXPORT IEffect* CreateIEEffect()
{
	return new E_Diffuse;
}

EFFECT_EXPORT void ReleaseIEEffect(IEffect** ppEffect)
{
	if (*ppEffect) {
		delete (*ppEffect);
		(*ppEffect) = NULL;
	}
}

EFFECT_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::EFFECT;
}

EFFECT_EXPORT void GetEffectDisplayNameJa(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

E_Diffuse::E_Diffuse()
{
}

E_Diffuse::~E_Diffuse()
{
}

bool E_Diffuse::Init(ImgFile_Ptr pFile)
{
	m_seed = DEFAULT_SEED;
	m_range = DEFAULT_RANGE;

	IImgLayer_weakPtr _sl = pFile->GetSelectLayer();
	if (_sl.expired()) {
		OutputError::Alert(IE_ERROR_SELECT_LAYER_DONT_EXIST);
		return false;
	}

	IImgLayer_Ptr sl = _sl.lock();
	if (sl->GetLayerType() != IE_LAYER_TYPE::NORMAL_LAYER) {
		OutputError::Alert(IE_ERROR_CANT_EDIT_LAYER);
		return false;
	}
	m_pEditLayer = std::dynamic_pointer_cast<ImgLayer>(sl);

	RECT rect, mask_rect;
	m_pEditLayer->GetLayerRect(&rect);

	//マスクの範囲取得
	if (pFile->GetMaskState() == IE_MASK_STATE_ID::INVALID) {
		m_mask = NULL;
	}
	else {
		pFile->GetMaskRect(&mask_rect);
		//範囲を計算
		AndRect(&rect, &mask_rect, &rect);

		//マスク切り出し
		IplImage* tmp_mask = cvCreateImage(
			cvSize(rect.right - rect.left, rect.bottom - rect.top),
			IPL_DEPTH_8U,
			1);

		pFile->CopyFileMask(
			rect.left,
			rect.top,
			rect.right - rect.left,
			rect.bottom - rect.top,
			tmp_mask,
			0, 0);

		m_mask = tmp_mask;
	}

	CvSize sizeOfImage;
	sizeOfImage.width = rect.right - rect.left;
	sizeOfImage.height = rect.bottom - rect.top;

	m_pEditLayerHandle = (EditLayerHandle*) pFile->CreateImgFileHandle(IFH_EDIT_LAYER);
	m_pEditLayerHandle->SetHandleName( DISPLAY_NAME );
	pFile->DoImgFileHandle( m_pEditLayerHandle );

	m_pEditNode = m_pEditLayerHandle->CreateEditNode(&rect);
	m_pEditNode->blt_rect = rect;

	m_editImg = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 4);
	m_editedImg = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 4);

	//inImgから有効範囲部分のデータを取得
	m_pEditLayer->CopyLayerImage(
		m_editImg,
		0,
		0,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top);

	return true;
}

void E_Diffuse::End(ImgFile_Ptr pFile)
{
	m_pEditLayerHandle->EndEdit();

	if(m_editImg){
		cvReleaseImage(&m_editImg);
	}
	if(m_editedImg){
		cvReleaseImage(&m_editedImg);
	}

	if(m_mask){
		cvReleaseImage(&m_mask);
	}
}

void E_Diffuse::Edit(ImgFile_Ptr pFile)
{
	m_pEditDialog->SetProgPos(0);

	int x,y;
	int sx, sy;
	UCvPixel* src_pixel;
	UCvPixel* dst_pixel;
	int nnn = m_range*2+1;
	for(y=0; y<m_editImg->height; y++){
		dst_pixel = GetPixelAddress32(m_editedImg, 0, y);
		for(x=0; x<m_editImg->width; x++){
			sx = x+rnoise(nnn, m_range);
			sy = y+rnoise(nnn, m_range);
			if(((0 <= sx) && (sx < m_editImg->width)) &&
				((0 <= sy) && (sy < m_editImg->height))){
				src_pixel = GetPixelAddress32(m_editImg, sx, sy);
			}
			else{
				src_pixel = GetPixelAddress32(m_editImg, x, y);
			}

			dst_pixel->value = src_pixel->value;
			dst_pixel++;
		}
	}
	
	m_pEditDialog->SetProgPos(25);

	//copy
	m_pEditNode->edit_img.ImgBlt(
		m_pEditNode->blt_rect.left,
		m_pEditNode->blt_rect.top,
		m_pEditNode->blt_rect.right - m_pEditNode->blt_rect.left,
		m_pEditNode->blt_rect.bottom - m_pEditNode->blt_rect.top,
		m_editedImg,
		0, 0,
		IPLEXT_RASTER_CODE::COPY,
		m_mask,
		0, 0);
	m_pEditDialog->SetProgPos(75);

	//
	m_pEditLayerHandle->Update(&(m_pEditNode->blt_rect));
	m_pEditDialog->SetProgPos(100);
}

void E_Diffuse::SetEditDialog(IEffectEditDialog *pEffectEdit)
{
	m_pEditDialog = pEffectEdit;
	m_pEditDialog->SetProgRange(0, 100);
	m_pEditDialog->SetProgPos(0);

	pEffectEdit->AddTrackbar("ノイズ範囲", 1, 200, DEFAULT_RANGE, &m_range);
}

int E_Diffuse::rnoise(int a, int b)
{
	return rand()%a-b;
}