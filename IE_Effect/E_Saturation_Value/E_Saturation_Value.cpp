// E_Saturation_Value.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "E_Saturation_Value.h"

static const int DEFAULT_S = 0;
static const int DEFAULT_V = 0;
static const int MAX_VALUE = 255;


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

#define DISPLAY_NAME "明度・彩度"

EFFECT_EXPORT IEffect* CreateIEEffect()
{
	return new E_Saturation_Value;
}

EFFECT_EXPORT void ReleaseIEEffect(IEffect** ppEffect)
{
	if (*ppEffect){
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

E_Saturation_Value::E_Saturation_Value()
{
	s_ = DEFAULT_S;
	v_ = DEFAULT_V;
}

E_Saturation_Value::~E_Saturation_Value()
{
}

bool E_Saturation_Value::Init(ImgFile_Ptr pFile)
{
	s_ = DEFAULT_S;
	v_ = DEFAULT_V;

	IImgLayer_weakPtr _sl = pFile->GetSelectLayer();
	if (!_sl.lock()) {
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

	m_pEditLayerHandle = (EditLayerHandle*)pFile->CreateImgFileHandle( IFH_EDIT_LAYER );
	m_pEditLayerHandle->SetHandleName( DISPLAY_NAME );
	pFile->DoImgFileHandle( m_pEditLayerHandle );

	m_pEditNode = m_pEditLayerHandle->CreateEditNode(&rect);
	m_pEditNode->blt_rect = rect;

	m_editImage = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 4);
	m_editedImage = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 4);
	m_hsvImage = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 3);
	m_hueImage = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 1);
	m_saturationImage = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 1);
	m_valueImage = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 1);
	m_addData = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 1);

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

void E_Saturation_Value::End(ImgFile_Ptr pFile)
{
	m_pEditLayerHandle->EndEdit();

	cvReleaseImage(&m_editImage);
	cvReleaseImage(&m_editedImage);
	cvReleaseImage(&m_hsvImage);
	cvReleaseImage(&m_hueImage);
	cvReleaseImage(&m_saturationImage);
	cvReleaseImage(&m_valueImage);
	cvReleaseImage(&m_addData);

	if(m_mask){
		cvReleaseImage(&m_mask);
	}
}

void E_Saturation_Value::Edit(ImgFile_Ptr pFile)
{
	m_pEditDialog->SetProgPos(0);

	//hsvに変換
	cvCvtColor( m_editImage, m_hsvImage, CV_BGR2HSV );
	m_pEditDialog->SetProgPos(10);

	//分割
	cvSplit( m_hsvImage, m_hueImage, m_saturationImage, m_valueImage, NULL );
	m_pEditDialog->SetProgPos(20);

	//彩度を加算
	cvSet( m_addData, cvScalar(abs(s_)), NULL );
	m_pEditDialog->SetProgPos(25);
	if(s_ >=0){
		cvAdd( m_saturationImage, m_addData, m_saturationImage );
	}
	else{
		cvSub( m_saturationImage, m_addData, m_saturationImage );
	}
	m_pEditDialog->SetProgPos(35);

	//明度を加算
	cvSet( m_addData, cvScalar(abs(v_)), NULL );
	m_pEditDialog->SetProgPos(45);
	if(v_ >= 0){
		cvAdd( m_valueImage, m_addData, m_valueImage );
	}
	else{
		cvSub( m_valueImage, m_addData, m_valueImage );
	}
	m_pEditDialog->SetProgPos(55);

	//合成
	cvMerge( m_hueImage, m_saturationImage, m_valueImage, NULL, m_hsvImage);
	m_pEditDialog->SetProgPos(65);

	//hsvからBGRに変換
	cvCvtColor( m_hsvImage, m_hsvImage, CV_HSV2BGR );
	m_pEditDialog->SetProgPos(75);

	ucvCvtColor(m_hsvImage, m_editedImage, CV_BGR2BGRA);

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
	m_pEditDialog->SetProgPos(85);

	//
	m_pEditLayerHandle->Update( &(m_pEditNode->blt_rect ));
	m_pEditDialog->SetProgPos(100);
}

void E_Saturation_Value::SetEditDialog(IEffectEditDialog *pEditDialog)
{
	m_pEditDialog = pEditDialog;
	pEditDialog->SetProgRange(0, 100);
	pEditDialog->SetProgPos(0);

	pEditDialog->AddTrackbar("彩度", -MAX_VALUE/2, MAX_VALUE/2, DEFAULT_S, &s_);
	pEditDialog->AddTrackbar("明度", -MAX_VALUE/2, MAX_VALUE/2, DEFAULT_V, &v_);
}