// E_EdgeLaplacian.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//
#include "stdafx.h"
#include "E_EdgeLaplacian.h"

static const int LAPLACIAN_APERTURE_SIZE = 3;
static const int SCALE = 1;
static const int SHIFT = 0;


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

#define DISPLAY_NAME "エッジ抽出(Laplacian)"

EFFECT_EXPORT IEffect* CreateIEEffect()
{
	return new E_EdgeLaplacian;
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

E_EdgeLaplacian::E_EdgeLaplacian()
{
}

E_EdgeLaplacian::~E_EdgeLaplacian()
{
}

bool E_EdgeLaplacian::Init(ImgFile_Ptr pFile)
{
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

	m_editImage = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 4);
	m_grayImage = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 1);
	m_laplaceImage16 = cvCreateImage(sizeOfImage, IPL_DEPTH_16S, 1);
	m_laplaceImage8  = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 1);
	m_editedImage = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 4);

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

void E_EdgeLaplacian::End(ImgFile_Ptr pFile)
{
	m_pEditLayerHandle->EndEdit();

	if(m_mask){
		cvReleaseImage(&m_mask);
	}

	cvReleaseImage(&m_editImage);
	cvReleaseImage(&m_grayImage);
	cvReleaseImage(&m_laplaceImage16);
	cvReleaseImage(&m_laplaceImage8);
	cvReleaseImage(&m_editedImage);
}

void E_EdgeLaplacian::Edit(ImgFile_Ptr pFile)
{
	//グレースケール作成
	cvCvtColor(m_editImage, m_grayImage, CV_BGRA2GRAY);
	//Laplacianオペレーターをかける
	cvLaplace(m_grayImage, m_laplaceImage16, LAPLACIAN_APERTURE_SIZE);
	//符号あり16bitから符号なし8bit値へ
	cvConvertScaleAbs(m_laplaceImage16, m_laplaceImage8, SCALE, SHIFT);
	//グレースケールからBGRへ
	ucvCvtColor(m_laplaceImage8, m_editedImage, CV_GRAY2BGRA);
	
	//blt edited image
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

	//
	m_pEditLayerHandle->Update(&(m_pEditNode->blt_rect));
}

void E_EdgeLaplacian::SetEditDialog(IEffectEditDialog *pEditDialog)
{
}