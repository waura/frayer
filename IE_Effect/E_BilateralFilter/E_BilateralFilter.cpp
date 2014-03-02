// E_BilateralFilter.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//
#include "stdafx.h"
#include "E_BilateralFilter.h"


#define DEFAULT_V_SIG 3
#define DEFAULT_DIS_SIG 0

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
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

#define DISPLAY_NAME "エッジ保持平滑化"

EFFECT_EXPORT IEffect* CreateIEEffect()
{
	return new E_BilateralFilter;
}

EFFECT_EXPORT void ReleaseIEEffect(IEffect** ppEffect)
{
	if(*ppEffect){
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

E_BilateralFilter::E_BilateralFilter()
{
}

E_BilateralFilter::~E_BilateralFilter()
{
}

bool E_BilateralFilter::Init(ImgFile_Ptr pFile)
{
	m_v_sig = DEFAULT_V_SIG;
	m_dis_sig = DEFAULT_DIS_SIG;

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
	if(pFile->GetMaskState() == IE_MASK_STATE_ID::INVALID){
		m_mask = NULL;
	}
	else{
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

	m_pEditLayerHandle = (EditLayerHandle*)pFile->CreateImgFileHandle( IFH_EDIT_LAYER );
	m_pEditLayerHandle->SetHandleName( DISPLAY_NAME );
	pFile->DoImgFileHandle( m_pEditLayerHandle );

	m_pEditNode = m_pEditLayerHandle->CreateEditNode(&rect);
	m_pEditNode->blt_rect = rect;

	m_editImg = cvCreateImage(
		cvSize(rect.right - rect.left, rect.bottom - rect.top),
		IPL_DEPTH_8U,
		3);

	m_editedImg = cvCreateImage(
		cvSize(rect.right - rect.left, rect.bottom - rect.top),
		IPL_DEPTH_8U,
		3);

	m_editedBGRAImg = cvCreateImage(
		cvSize(rect.right - rect.left, rect.bottom - rect.top),
		IPL_DEPTH_8U,
		4);

	//レイヤーから有効範囲部分のデータを取得
	m_pEditLayer->CopyLayerImage(
		m_editedBGRAImg,
		0,
		0,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top);

	ucvCvtColor(m_editedBGRAImg, m_editImg, CV_BGRA2BGR);

	return true;
}

void E_BilateralFilter::End(ImgFile_Ptr pFile)
{
	m_pEditLayerHandle->EndEdit();

	cvReleaseImage(&m_editImg);
	cvReleaseImage(&m_editedImg);
	cvReleaseImage(&m_editedBGRAImg);

	if(m_mask){
		cvReleaseImage(&m_mask);
	}
}

void E_BilateralFilter::Edit(ImgFile_Ptr pFile)
{
	m_pEditDialog->SetProgPos( 0 );

	//bilateral filter
	cvSmooth(m_editImg, m_editedImg, CV_BILATERAL, m_v_sig, m_dis_sig, 0, 0);
	m_pEditDialog->SetProgPos( 25 );

	//to BGRA
	ucvCvtColor(m_editedImg, m_editedBGRAImg, CV_BGR2BGRA);

	//copy
	m_pEditNode->edit_img.ImgBlt(
		m_pEditNode->blt_rect.left - m_pEditNode->node_rect.left,
		m_pEditNode->blt_rect.top - m_pEditNode->node_rect.top,
		m_pEditNode->blt_rect.right - m_pEditNode->blt_rect.left,
		m_pEditNode->blt_rect.bottom - m_pEditNode->blt_rect.top,
		m_editedBGRAImg,
		0,
		0,
		IPLEXT_RASTER_CODE::COPY);

	m_pEditDialog->SetProgPos( 75 );

	//
	m_pEditLayerHandle->Update( &(m_pEditNode->blt_rect) );
	m_pEditDialog->SetProgPos( 100 );
}

void E_BilateralFilter::SetEditDialog(IEffectEditDialog *pEffectEdit)
{
	m_pEditDialog = pEffectEdit;
	m_pEditDialog->SetProgRange(0, 100);
	m_pEditDialog->SetProgPos(0);

	m_pEditDialog->AddTrackbar("輝度Σ", 0, 16, DEFAULT_V_SIG, &m_v_sig);
	m_pEditDialog->AddTrackbar("領域Σ", 0, 16, DEFAULT_DIS_SIG, &m_dis_sig);
}