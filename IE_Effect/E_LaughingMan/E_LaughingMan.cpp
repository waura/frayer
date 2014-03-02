// E_LaughingMan.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "E_LaughingMan.h"

#include <libImgEdit.h>

#define DISPLAY_NAME "笑い男"
#define CASCADE_FILE_PATH "data\\haarcascades\\haarcascade_frontalface_alt.xml"

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

EFFECT_EXPORT IEffect* CreateIEEffect()
{
	return new E_LaughingMan;
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

E_LaughingMan::E_LaughingMan()
{
}

E_LaughingMan::~E_LaughingMan()
{
}

bool E_LaughingMan::Init(ImgFile_Ptr pFile)
{
	m_faces = NULL;
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
	if(pFile->GetMaskState() != IE_MASK_STATE_ID::INVALID){
		pFile->GetMaskRect(&mask_rect);
		//範囲を計算
		AndRect(&rect, &mask_rect, &rect);
	}

	CvSize sizeOfImage;
	sizeOfImage.width = rect.right - rect.left;
	sizeOfImage.height = rect.bottom - rect.top;

	m_pEditLayerHandle = (EditLayerHandle*)pFile->CreateImgFileHandle( IFH_EDIT_LAYER );
	m_pEditLayerHandle->SetHandleName( DISPLAY_NAME );
	pFile->DoImgFileHandle( m_pEditLayerHandle );

	m_pEditNode = m_pEditLayerHandle->CreateEditNode(&rect);
	m_pEditNode->blt_rect = rect;

	IplImage* bgraImg;
	IplImage* editImg;
	IplImage* grayImg;
	IplImage* editedImg;

	bgraImg = cvCreateImage(
		cvSize(rect.right - rect.left, rect.bottom - rect.top),
		IPL_DEPTH_8U,
		4);
	editImg = cvCreateImage(
		cvSize(rect.right - rect.left, rect.bottom - rect.top),
		IPL_DEPTH_8U,
		3);
	grayImg = cvCreateImage(
		cvSize(rect.right - rect.left, rect.bottom - rect.top),
		IPL_DEPTH_8U,
		1);
	editedImg = cvCreateImage(
		cvSize(rect.right - rect.left, rect.bottom - rect.top),
		IPL_DEPTH_8U,
		3);

	//レイヤーから有効範囲部分のデータを取得
	m_pEditLayer->CopyLayerImage(
		bgraImg,
		0,
		0,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top);

	ucvCvtColor(bgraImg, editImg, CV_BGRA2BGR);
	cvCvtColor(editImg, grayImg, CV_BGRA2GRAY);
	cvEqualizeHist(grayImg, grayImg);

	m_cascade = (CvHaarClassifierCascade*)cvLoad(CASCADE_FILE_PATH, 0, 0, 0);
	if (m_cascade == NULL) {
		return false;
	}

	m_storage = cvCreateMemStorage(0);;
	cvClearMemStorage(m_storage);

    m_faces = cvHaarDetectObjects(grayImg, m_cascade, m_storage,
                                       1.1, 2, 0,//CV_HAAR_DO_CANNY_PRUNING,
                                       cvSize(30, 30) );


	cvReleaseImage(&bgraImg);
	cvReleaseImage(&editImg);
    cvReleaseImage(&grayImg);
    cvReleaseImage(&editedImg);

	return true;
}

void E_LaughingMan::End(ImgFile_Ptr pFile)
{
	cvReleaseMemStorage(&m_storage);
}

void E_LaughingMan::Edit(ImgFile_Ptr pFile)
{
	int i;
    for (i = 0; i < (m_faces ? m_faces->total : 0); i++ ) {
        CvRect* r = (CvRect*)cvGetSeqElem(m_faces, i );
        CvPoint center;
        int radius;
        center.x = cvRound(r->x + r->width*0.5);
        center.y = cvRound(r->y + r->height*0.5);
        radius = cvRound((r->width + r->height)*0.25);
        //cvCircle(outImg, center, radius, colors[i%8], 3, 8, 0 );
		//
    }
}

void E_LaughingMan::SetEditDialog(IEffectEditDialog *pEditDialog)
{
}