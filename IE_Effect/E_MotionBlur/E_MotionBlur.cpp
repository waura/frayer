// E_MotionBlur.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "E_MotionBlur.h"

#define _PI 3.14159265

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

#define DISPLAY_NAME "メディアンフィルター"

EFFECT_EXPORT IEffect* CreateIEEffect()
{
	return new E_MotionBlur;
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

E_MotionBlur::E_MotionBlur()
{
	m_rot = 0;
	m_size = 2;
}

E_MotionBlur::~E_MotionBlur()
{
}

bool E_MotionBlur::Init(ImgFile_Ptr pFile)
{
	IImgLayer_weakPtr _sl = pFile->GetSelectLayer();
	if(!_sl.expired()){
		OutputError::Alert(IE_ERROR_SELECT_LAYER_DONT_EXIST);
		return false;
	}

	IImgLayer_Ptr sl = _sl.lock();
	if(sl->GetLayerType() != IE_LAYER_TYPE::NORMAL_LAYER){
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

void E_MotionBlur::End(ImgFile_Ptr pFile)
{
	m_pEditLayerHandle->EndEdit();

	cvReleaseImage(&m_editImage);
	cvReleaseImage(&m_editedImage);

	if(m_mask){
		cvReleaseImage(&m_mask);
	}
}

void E_MotionBlur::Edit(ImgFile_Ptr pFile)
{
	m_pEditDialog->SetProgPos(0);

	//モーションブラー
	const DWORD fixfloat_bit = 16;
	const DWORD fixfloat_constant = (1 << fixfloat_bit);

	m_blur_vec_theta = 2.0*_PI*(m_rot/360.0);
	double v_x = cos(m_blur_vec_theta);
	double v_y = sin(m_blur_vec_theta);
	int delta_x = v_x / 3.0 * fixfloat_constant;
	int delta_y = v_y / 3.0 * fixfloat_constant;

	int i,j;
	int r,g,b,a;
	UCvPixel* dst;
	UCvPixel pixel1;
	UCvPixel pixel2;
	int count;
	for(j=0; j<m_editImage->height; j++){
		dst = GetPixelAddress32(m_editedImage, 0, j);
		for(i=0; i<m_editImage->width; i++){
			r=g=b=a=count=0;

			//始点と、サンプリング数を求める
			int dx = ((double)i - v_x * m_size) * fixfloat_constant;
			int dy = ((double)j - v_y * m_size) * fixfloat_constant;
			int max = m_size * 2 * 3.0;

			int m;
			int8_t r_buf, g_buf, b_buf, a_buf;
			for(m=0; m<max; m++){
				int xx = dx >> fixfloat_bit;
				int yy = dy >> fixfloat_bit;
				if((0 <= xx && xx < m_editImage->width) &&
					(0 <= yy && yy < m_editImage->height))
				{
					GetPixelFromBGRA(m_editImage, xx, yy, &pixel2);

					a += pixel2.a;
					r += pixel2.r * pixel2.a;
					g += pixel2.g * pixel2.a;
					b += pixel2.b * pixel2.a;
					count++;
				}

				dx += delta_x;
				dy += delta_y;
			}

			if(a != 0){
				pixel1.a = a / count;
				pixel1.r = r / a;
				pixel1.g = g / a;
				pixel1.b = b / a;
			}
			else{
				pixel1.value = 0;
			}

			(*dst++).value = pixel1.value;
		}
	}
	m_pEditDialog->SetProgPos(50);

	//コピー
	m_pEditNode->edit_img.ImgBlt(
		m_pEditNode->blt_rect.left - m_pEditNode->node_rect.left,
		m_pEditNode->blt_rect.top - m_pEditNode->node_rect.top,
		m_pEditNode->blt_rect.right - m_pEditNode->blt_rect.left,
		m_pEditNode->blt_rect.bottom - m_pEditNode->blt_rect.top,
		m_editedImage,
		0, 0,
		IPLEXT_RASTER_CODE::ALPHA_BLEND,
		m_mask,
		0, 0);
	m_pEditDialog->SetProgPos(75);

	//
	m_pEditLayerHandle->Update( &(m_pEditNode->blt_rect) );
	m_pEditDialog->SetProgPos(100);
}

void E_MotionBlur::SetEditDialog(IEffectEditDialog *pEffectEdit)
{
	m_pEditDialog = pEffectEdit;
	pEffectEdit->AddTrackbar("角度", 0, 360, 0, &m_rot);
	pEffectEdit->AddTrackbar("距離", 1, 10, 2, &m_size);
}