#include "E_SoftForcus.h"

#include <libImgEdit.h>

#define DEFAULT_EF 1
#define DEFAULT_RATE 128

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

#define DISPLAY_NAME "ソフトフォーカス"

EFFECT_EXPORT IEffect* CreateIEEffect()
{
	return new E_SoftForcus;
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

E_SoftForcus::E_SoftForcus()
{
}

E_SoftForcus::~E_SoftForcus()
{
}

bool E_SoftForcus::Init(ImgFile_Ptr pFile)
{
	m_ef = DEFAULT_EF;
	m_rate = DEFAULT_RATE;

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

void E_SoftForcus::End(ImgFile_Ptr pFile)
{
	m_pEditLayerHandle->EndEdit();

	//
	cvReleaseImage(&m_editImg);
	cvReleaseImage(&m_editedImg);

	if(m_mask){
		cvReleaseImage(&m_mask);
	}
}

void E_SoftForcus::Edit(ImgFile_Ptr pFile)
{
	m_pEditDialog->SetProgPos(0);

	int x,y;
	int sumr[20], sumg[20], sumb[20];
	int sq = (2*m_ef+1)*(2*m_ef+1);

	UCvPixel* dst;
	UCvPixel* src;
	UCvPixel pixel;

	for(y=0; y<m_editImg->height; y++){
		dst = GetPixelAddress32(m_editedImg, 0, y);
		src = GetPixelAddress32(m_editImg, 0, y);

		int xx, yy;
		int smx;
		//画素の周りの平均をだし、閾値と比較
		for(xx = -m_ef; xx<m_ef; xx++){
			smx = xx + m_ef;
			sumr[smx] = sumg[smx] = sumb[smx] = 0;
			for(yy = -m_ef; yy<= m_ef; yy++){
				int src_x = xx;
				int src_y = y+yy;
				if(src_x < 0) src_x = 0;
				if(src_y < 0) src_y = 0;
				if(src_x >= m_editImg->width) src_x = m_editImg->width - 1;
				if(src_y >= m_editImg->height) src_y = m_editImg->height - 1;

				GetPixelFromBGRA(m_editImg, src_x, src_y, &pixel);
				int gray = (pixel.b*28 +
					pixel.r*77 +
					pixel.g*151)>>8;
				if(gray > m_rate){
					sumr[smx] += pixel.r;
					sumg[smx] += pixel.g;
					sumb[smx] += pixel.b;
				}
			}
		}
		for(x=0; x<m_editImg->width; x++){
			int r, g, b;
			r = g = b = 0;
			smx = m_ef + m_ef;
			sumr[smx] = sumg[smx] = sumb[smx] = 0;
			for(yy = -m_ef; yy<=m_ef; yy++){
				int src_x = x+m_ef;
				int src_y = y+yy;
				if(src_x < 0) src_x = 0;
				if(src_y < 0) src_y = 0;
				if(src_x >= m_editImg->width) src_x = m_editImg->width - 1;
				if(src_y >= m_editImg->height) src_y = m_editImg->height - 1;

				GetPixelFromBGRA(m_editImg, src_x, src_y, &pixel);
				int gray = (pixel.b*28 +
					pixel.r*77 +
					pixel.g*151)>>8;
				if(gray > m_rate){
					sumr[smx] += pixel.r;
					sumg[smx] += pixel.g;
					sumb[smx] += pixel.b;
				}
			}
			for(xx=0; xx<=smx; xx++){
				r += sumr[xx];
				g += sumg[xx];
				b += sumb[xx];
				sumr[xx] = sumr[xx+1];
				sumg[xx] = sumg[xx+1];
				sumb[xx] = sumb[xx+1];
			}

			r = r / sq;
			g = g / sq;
			b = b / sq;
			r = r*255 / (3*src->r + 255);
			g = g*255 / (3*src->g + 255);
			b = b*255 / (3*src->b + 255);

			r = r + (255 - r) * src->r / 255;
			g = g + (255 - g) * src->g / 255;
			b = b + (255 - b) * src->b / 255;

			dst->b = b;
			dst->g = g;
			dst->r = r;
			dst->a = src->a;

			dst++;
			src++;
		}
	}
	m_pEditDialog->SetProgPos(25);

	//Copy
	m_pEditNode->edit_img.ImgBlt(
		m_pEditNode->blt_rect.left - m_pEditNode->node_rect.left,
		m_pEditNode->blt_rect.top - m_pEditNode->node_rect.top,
		m_pEditNode->blt_rect.right - m_pEditNode->blt_rect.left,
		m_pEditNode->blt_rect.bottom - m_pEditNode->blt_rect.top,
		m_editedImg,
		0,
		0,
		IPLEXT_RASTER_CODE::COPY);

	m_pEditDialog->SetProgPos(75);

	//
	m_pEditLayerHandle->Update(&(m_pEditNode->blt_rect));
	m_pEditDialog->SetProgPos(100);
}

void E_SoftForcus::SetEditDialog(IEffectEditDialog *pEffectEdit)
{
	m_pEditDialog = pEffectEdit;
	m_pEditDialog->SetProgRange(0, 100);
	m_pEditDialog->SetProgPos(0);

	pEffectEdit->AddTrackbar("適用半径", 1, 10, DEFAULT_EF, &m_ef);
	pEffectEdit->AddTrackbar("閾値", 0, 255, DEFAULT_RATE, &m_rate);
}