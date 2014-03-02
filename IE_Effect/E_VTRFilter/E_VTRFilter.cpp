// E_VTRFilter.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "E_VTRFilter.h"

#include <libImgEdit.h>

#define DEFAULT_L_WID 2
#define DEFAULT_L_DET 4
#define DEFAULT_CST 100
#define DEFAULT_GST 3


#ifdef _MANAGED
#pragma managed(push, off)
#endif

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

#ifdef _MANAGED
#pragma managed(pop)
#endif

// for detect memory leak
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <cstdlib>
	#include <crtdbg.h>
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#define DISPLAY_NAME "VTR調変換"

EFFECT_EXPORT IEffect* CreateIEEffect()
{
	return new E_VTRFilter;
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

E_VTRFilter::E_VTRFilter()
{
}

E_VTRFilter::~E_VTRFilter()
{
}

bool E_VTRFilter::Init(ImgFile_Ptr pFile)
{
	m_l_wid = DEFAULT_L_WID;
	m_l_det = DEFAULT_L_DET;
	m_cst = DEFAULT_CST;
	m_gst = DEFAULT_GST;

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

	//layerから有効範囲部分のデータを取得
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

void E_VTRFilter::End(ImgFile_Ptr pFile)
{
	m_pEditLayerHandle->EndEdit();

	cvReleaseImage(&m_editImg);
	cvReleaseImage(&m_editedImg);

	if(m_mask){
		cvReleaseImage(&m_mask);
	}
}

void E_VTRFilter::Edit(ImgFile_Ptr pFile)
{
	m_pEditDialog->SetProgPos(0);

	int x,y;
	UCvPixel pixel1;
	UCvPixel pixel2;
	BOOL flag;
	for(y=0; y<m_editImg->height; y++){
		if((y%m_l_det)<m_l_wid) flag = TRUE;
		else flag = FALSE;
		for(x=0; x<m_editImg->width; x++){
			GetPixelFromBGRA(m_editImg, x, y, &pixel1);

			int src_x = x+m_gst;
			if(src_x >= m_editImg->width) src_x = m_editImg->width-1;
			GetPixelFromBGRA(m_editImg, src_x, y, &pixel2);
			
			pixel1.b = ((int)pixel1.b+pixel2.b) >> 1;
			pixel1.g = ((int)pixel1.g+pixel2.g) >> 1;
			pixel1.r = ((int)pixel1.r+pixel2.r) >> 1;

			if(flag){
				int add_cst = m_cst - 127;
				if(pixel1.b + add_cst > 0xff)
					pixel1.b = 0xff;
				else
					pixel1.b += add_cst;

				if(pixel1.g + add_cst > 0xff)
					pixel1.g = 0xff;
				else
					pixel1.g += add_cst;

				if(pixel1.r + add_cst > 0xff)
					pixel1.r = 0xff;
				else
					pixel1.r += add_cst;
			}

			SetPixelToBGRA(m_editedImg, x, y, &pixel1);
		}
	}
	m_pEditDialog->SetProgPos(40);

	//copy
	m_pEditNode->edit_img.ImgBlt(
		m_pEditNode->blt_rect.left - m_pEditNode->node_rect.left,
		m_pEditNode->blt_rect.top - m_pEditNode->node_rect.top,
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

void E_VTRFilter::SetEditDialog(IEffectEditDialog *pEffectEdit)
{
	m_pEditDialog = pEffectEdit;
	m_pEditDialog->SetProgRange(0, 100);
	m_pEditDialog->SetProgPos(0);

	pEffectEdit->AddTrackbar("帯幅", 1, 20, DEFAULT_L_WID, &m_l_wid);
	pEffectEdit->AddTrackbar("帯数", 1, 50, DEFAULT_L_DET, &m_l_det);
	pEffectEdit->AddTrackbar("彩度", 0, 255, DEFAULT_CST, &m_cst);
	pEffectEdit->AddTrackbar("ぶれ幅", 1, 50, DEFAULT_GST, &m_gst);
}
