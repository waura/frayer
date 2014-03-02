// E_Glow.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "E_Glow.h"

#include <libImgEdit.h>

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

#define DISPLAY_NAME "グロー"

EFFECT_EXPORT IEffect* CreateIEEffect()
{
	return new E_Glow;
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

E_Glow::E_Glow()
{
	//m_bs = DEFAULT_BS;
}

E_Glow::~E_Glow()
{
}

bool E_Glow::Init(ImgFile_Ptr pFile)
{
	return true;
}

void E_Glow::End(ImgFile_Ptr pFile)
{
}

void E_Glow::Edit(ImgFile_Ptr pFile)
{
	//RECT rect;
	//inImg->GetMaxRangeRect(&rect);
	//CvSize sizeOfImage;
	//sizeOfImage.width = rect.right - rect.left;
	//sizeOfImage.height = rect.bottom - rect.top;
	//IplImage *editImage = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 3);
	//IplImage *editedImage = cvCreateImage(sizeOfImage, IPL_DEPTH_8U, 3);

	////inImgから有効部分のデータを取得
	//inImg->GetRangeIplImage(editImage, &rect);

	////ガウシアンフィルター
	//cvSmooth(editImage, editedImage, CV_GAUSSIAN, 2*m_bs-1, 0, 0, 0);

	////コピー
	//outImg->ImgBlt(rect.left, 
	//			rect.top,
	//			rect.right - rect.left,
	//			rect.bottom - rect.top,
	//			editedImage,
	//			0,
	//			0,
	//			m_mask);

	////
	//cvReleaseImage(&editImage);
	//cvReleaseImage(&editedImage);
}

void E_Glow::SetEditDialog(IEffectEditDialog *pEffectEdit)
{
	//pEffectEdit->AddTrackbar("適用半径", 1, 250, DEFAULT_BS, &m_bs);
}