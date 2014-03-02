// E_Binary_Threshold.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//
#include "stdafx.h"
#include "E_Binary_Threshold.h"


static const int DEFAULT_THRESHOLD = 128;
static const int THRESHOLD_MAX_VALUE = 255; 


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

#define DISPLAY_NAME "2値化(Thresold)"

EFFECT_EXPORT IEffect* CreateIEEffect()
{
	return new E_Binary_Threshold;
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

E_Binary_Threshold::E_Binary_Threshold()
{
}

E_Binary_Threshold::~E_Binary_Threshold()
{
}

bool E_Binary_Threshold::Init(ImgFile_Ptr pFile)
{
	m_threshold = DEFAULT_THRESHOLD;

	IImgLayer_weakPtr _sl = pFile->GetSelectLayer();
	if(_sl.expired()){
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
	m_grayImg = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 1);
	m_binaryImg = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 1);
	m_editedImg = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 4);

	//レイヤーから有効範囲部分のデータを取得
	m_pEditLayer->CopyLayerImage(
		m_editImg,
		0,
		0,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom  - rect.top);

	return true;
}

void E_Binary_Threshold::End(ImgFile_Ptr pFile)
{
	m_pEditLayerHandle->EndEdit();

	cvReleaseImage(&m_editImg);
	cvReleaseImage(&m_grayImg);
	cvReleaseImage(&m_binaryImg);
	cvReleaseImage(&m_editedImg);

	if(m_mask){
		cvReleaseImage(&m_mask);
	}
}

void E_Binary_Threshold::Edit(ImgFile_Ptr pFile)
{
	m_pEditDialog->SetProgPos(0);

	//covert to gray scale
	cvCvtColor(m_editImg, m_grayImg, CV_BGRA2GRAY);
	m_pEditDialog->SetProgPos(10);

	//gray scale to binary
	cvThreshold(m_grayImg, m_binaryImg, (double)m_threshold, THRESHOLD_MAX_VALUE, CV_THRESH_BINARY);
	m_pEditDialog->SetProgPos(25);

	//to BGRA
	ucvCvtColor(m_binaryImg, m_editedImg, CV_GRAY2BGRA);

	m_pEditDialog->SetProgPos(40);

	//copy to edit_img
	m_pEditNode->edit_img.ImgBlt(
		m_pEditNode->blt_rect.left - m_pEditNode->node_rect.left,
		m_pEditNode->blt_rect.top - m_pEditNode->node_rect.top,
		m_pEditNode->blt_rect.right - m_pEditNode->blt_rect.left,
		m_pEditNode->blt_rect.bottom - m_pEditNode->blt_rect.top,
		m_editedImg,
		0,
		0,
		IPLEXT_RASTER_CODE::COPY);

	m_pEditDialog->SetProgPos(80);

	//
	m_pEditLayerHandle->Update( &(m_pEditNode->blt_rect) );
	m_pEditDialog->SetProgPos(100);

}

void E_Binary_Threshold::SetEditDialog(IEffectEditDialog *pEditDialog)
{
	m_pEditDialog = pEditDialog;
	pEditDialog->SetProgRange(0, 100);
	pEditDialog->SetProgPos(0);

	pEditDialog->AddTrackbar("2値化閾値", 0, 255, DEFAULT_THRESHOLD, &m_threshold);
}
