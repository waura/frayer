// E_Sepia.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "E_Sepia.h"

#include <libImgEdit.h>

static const int DEFAULT_H = 22;
static const int DEFAULT_S = 90;


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

#define DISPLAY_NAME "セピア"

EFFECT_EXPORT IEffect* CreateIEEffect()
{
	return new E_Sepia;
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

E_Sepia::E_Sepia()
{
}

E_Sepia::~E_Sepia()
{
}

bool E_Sepia::Init(ImgFile_Ptr pFile)
{
	h_ = DEFAULT_H;
	s_ = DEFAULT_S;

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

	m_editImage = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 4);
	m_editedImage = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 4);
	m_hsvImage        = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 3); // HSV用
	m_mergeImage      = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 3); //マージ用
	m_hueImage        = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 1); //
	m_saturationImage = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 1); //
	m_valueImage      = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 1); //
	m_alphaCh         = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 1);

	//layerから有効範囲部分のデータを取得
	m_pEditLayer->CopyLayerImage(
		m_editImage,
		0,
		0,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top);

	return true;
}

void E_Sepia::End(ImgFile_Ptr pFile)
{
	m_pEditLayerHandle->EndEdit();

	//
	cvReleaseImage(&m_editImage);
	cvReleaseImage(&m_editedImage);
	cvReleaseImage(&m_hsvImage);
	cvReleaseImage(&m_mergeImage);
	cvReleaseImage(&m_hueImage);
	cvReleaseImage(&m_saturationImage);
	cvReleaseImage(&m_valueImage);
	cvReleaseImage(&m_alphaCh);

	if(m_mask){
		cvReleaseImage(&m_mask);
	}
}

void E_Sepia::Edit(ImgFile_Ptr pFile)
{
	m_pEditDialog->SetProgPos( 0 );

	//BGRからHSVに変換
	cvCvtColor(m_editImage, m_hsvImage, CV_BGR2HSV);
	m_pEditDialog->SetProgPos( 10 );

	//HSV画像をH,S,Vに分割
	cvSplit(m_hsvImage, m_hueImage, m_saturationImage, m_valueImage, NULL);
	m_pEditDialog->SetProgPos( 20 );

	//HとSの値を変更
	CvScalar valueH = cvScalar(h_);
	CvScalar valueS = cvScalar(s_);
	cvSet(m_hueImage, valueH, NULL);
	cvSet(m_saturationImage, valueS, NULL);
	m_pEditDialog->SetProgPos( 40 );

	//マージ
	cvMerge(m_hueImage, m_saturationImage, m_valueImage, NULL, m_mergeImage);
	m_pEditDialog->SetProgPos( 50 );

	//HSVからBGR
	cvCvtColor(m_mergeImage, m_mergeImage, CV_HSV2BGR);
	m_pEditDialog->SetProgPos( 60 );
	
	ucvCvtColor(m_mergeImage, m_editedImage, CV_BGR2BGRA);
	m_pEditDialog->SetProgPos( 70 );

	//コピー
	m_pEditNode->edit_img.ImgBlt(
		m_pEditNode->blt_rect.left - m_pEditNode->node_rect.left,
		m_pEditNode->blt_rect.top - m_pEditNode->node_rect.top,
		m_pEditNode->blt_rect.right - m_pEditNode->blt_rect.left,
		m_pEditNode->blt_rect.bottom - m_pEditNode->blt_rect.top,
		m_editedImage,
		0, 0,
		IPLEXT_RASTER_CODE::COPY,
		m_mask,
		0, 0);
	m_pEditDialog->SetProgPos( 90 );

	//
	m_pEditLayerHandle->Update( &(m_pEditNode->blt_rect) );
	m_pEditDialog->SetProgPos( 100 );
}

void E_Sepia::SetEditDialog(IEffectEditDialog *pEditDialog)
{
	m_pEditDialog = pEditDialog;
	m_pEditDialog->SetProgRange(0, 100);
	m_pEditDialog->SetProgPos( 0 );

	pEditDialog->AddTrackbar("彩度(S)", 0, 255, DEFAULT_S, &s_);
}