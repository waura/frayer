#include "stdafx.h"
#include "ChangeResolutionHandle.h"

#include "ImgFile.h"
#include "ImgLayer.h"
#include "ImgLayerGroup.h"

#include "AddNewLayerHandle.h"
#include "AddNewLayerGroupHandle.h"
#include "ImgFileHandleMG.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


ChangeResolutionHandle::ChangeResolutionHandle()
{
	m_interpolation = CV_INTER_LINEAR;
	m_pOldRootLayerGroup_lock = NULL;
	m_pNewRootLayerGroup_lock = NULL;
}

ChangeResolutionHandle::~ChangeResolutionHandle()
{
	//if (m_isChangedResolution) {
	//	if (m_pOldRootLayerGroup) {
	//		ReleaseIEImgLayerGroup( &m_pOldRootLayerGroup );
	//	}
	//} else {
	//	if (m_pNewRootLayerGroup) {
	//		ReleaseIEImgLayerGroup( &m_pNewRootLayerGroup );
	//	}
	//}
	m_pOldRootLayerGroup_lock = NULL;
	m_pNewRootLayerGroup_lock = NULL;
}

bool ChangeResolutionHandle::Do(ImgFile_weakPtr wpFile)
{
	assert(m_pOldRootLayerGroup_lock == NULL);
	assert(m_pNewRootLayerGroup_lock == NULL);

	m_pEditFile = wpFile;
	ImgFile_Ptr pFile = wpFile.lock();
	assert(pFile);

	m_old_resolution = pFile->GetImgSize();

	//change canvas size
	pFile->InitCanvasSize( m_new_resolution );

	RECT new_rc;
	new_rc.left = new_rc.top = 0;
	new_rc.right = m_new_resolution.width;
	new_rc.bottom = m_new_resolution.height;

	m_pOldRootLayerGroup = m_pOldRootLayerGroup_lock = pFile->GetRootLayerGroup();
	m_pNewRootLayerGroup = m_pNewRootLayerGroup_lock = CreateIEImgLayerGroup(pFile);
	m_pNewRootLayerGroup_lock->Init(&new_rc);
	m_pNewRootLayerGroup_lock->SetParentLayer( NULL );
	m_pNewRootLayerGroup_lock->SetUnderLayer( NULL );
	m_pNewRootLayerGroup_lock->SetOverLayer( NULL );
	m_pNewRootLayerGroup_lock->SetLayerSynthFourCC( LayerSynthesizerMG::GetDefaultLayerSynthFourCC() );

	pFile->RemoveLayer(m_pOldRootLayerGroup_lock);
	pFile->SetRootLayerGroup(m_pNewRootLayerGroup_lock);
	pFile->SetSelectLayer(m_pNewRootLayerGroup_lock);
	CreateNewResolutionLayerGroup(m_pOldRootLayerGroup_lock, m_pNewRootLayerGroup_lock);

	m_isChangedResolution = true;
	m_pNewRootLayerGroup_lock = NULL;

	//update
	LPUPDATE_DATA pData = pFile->CreateUpdateData();
	pData->isAll = true;
	pFile->PushUpdateData( pData );

	ImgLayerGroup_Ptr pRootLayerGroup = pFile->GetRootLayerGroup();
	LPUPDATE_DATA pData2 = pRootLayerGroup->CreateUpdateData();
	pData2->isAll = true;
	pRootLayerGroup->PushUpdateData( pData2 );

	return true;
}

void ChangeResolutionHandle::Redo(ImgFile_weakPtr wpFile)
{
	assert(m_pOldRootLayerGroup_lock == NULL);
	assert(m_pNewRootLayerGroup_lock != NULL);

	ImgFile_Ptr pFile = wpFile.lock();

	m_isChangedResolution = true;
	m_pOldRootLayerGroup_lock = m_pOldRootLayerGroup.lock();;
	//
	pFile->RemoveLayer(m_pOldRootLayerGroup_lock);
	pFile->SetRootLayerGroup(m_pNewRootLayerGroup_lock);
	pFile->UnsetSelectLayer();

	m_pNewRootLayerGroup_lock = NULL;

	//change canvas size
	pFile->InitCanvasSize( m_new_resolution );
	//update
	LPUPDATE_DATA pData = pFile->CreateUpdateData();
	pData->isAll = true;
	pFile->PushUpdateData( pData );

	ImgLayerGroup_Ptr pRootLayerGroup = pFile->GetRootLayerGroup();
	LPUPDATE_DATA pData2 = pRootLayerGroup->CreateUpdateData();
	pData2->isAll = true;
	pRootLayerGroup->PushUpdateData( pData2 );
}

void ChangeResolutionHandle::Undo(ImgFile_weakPtr wpFile)
{
	assert(m_pOldRootLayerGroup_lock != NULL);
	assert(m_pNewRootLayerGroup_lock == NULL);

	ImgFile_Ptr pFile = wpFile.lock();

	m_isChangedResolution = false;
	//
	m_pNewRootLayerGroup_lock = m_pNewRootLayerGroup.lock();

	pFile->RemoveLayer(m_pNewRootLayerGroup_lock);
	pFile->SetRootLayerGroup(m_pOldRootLayerGroup_lock);
	pFile->UnsetSelectLayer();

	m_pOldRootLayerGroup_lock = NULL;

	//change canvas size
	pFile->InitCanvasSize( m_old_resolution );
	//update
	LPUPDATE_DATA pData = pFile->CreateUpdateData();
	pData->isAll = true;
	pFile->PushUpdateData( pData );

	ImgLayerGroup_Ptr pRootLayerGroup = pFile->GetRootLayerGroup();
	LPUPDATE_DATA pData2 = pRootLayerGroup->CreateUpdateData();
	pData2->isAll = true;
	pRootLayerGroup->PushUpdateData( pData2 );
}

void ChangeResolutionHandle::CreateNewResolutionLayerGroup(
	ImgLayerGroup_Ptr pSrcGroup,
	ImgLayerGroup_Ptr pDstGroup)
{
	double xscale = (double)m_new_resolution.width / m_old_resolution.width;
	double yscale = (double)m_new_resolution.height / m_old_resolution.height;

	ImgFile_Ptr pFile = m_pEditFile.lock();
	IImgLayer_Ptr pLayer = pSrcGroup->GetBottomChildLayer();
	while(pLayer != NULL){
		if(pLayer->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER){
			AddNewLayerGroupHandle* handle = (AddNewLayerGroupHandle*)pFile->CreateImgFileHandle(IFH_ADD_NEW_LAYER_GROUP);
			handle->Do( m_pEditFile );

			ImgLayerGroup_Ptr srcg = std::dynamic_pointer_cast<ImgLayerGroup>(pLayer);
			ImgLayerGroup_Ptr dstg = handle->GetNewLayeyGroup().lock();

			CreateNewResolutionLayerGroup(srcg, dstg);

			pFile->SetSelectLayer( pDstGroup );
			pFile->ReleaseImgFileHandle(handle);
			handle = NULL;
		}
		else{
			RECT old_rc, new_rc;
			pLayer->GetLayerRect( &old_rc );

			new_rc.left = old_rc.left * xscale;
			new_rc.right = old_rc.right * xscale;
			new_rc.top = old_rc.top * yscale;
			new_rc.bottom = old_rc.bottom * yscale;

			IplImage* old_img = cvCreateImage(
				cvSize(old_rc.right - old_rc.left, old_rc.bottom - old_rc.top),
				IPL_DEPTH_8U,
				4);
			IplImage* new_img = cvCreateImage(
				cvSize(new_rc.right - new_rc.left, new_rc.bottom - new_rc.top),
				IPL_DEPTH_8U,
				4);


			AddNewLayerHandle* handle = (AddNewLayerHandle*)pFile->CreateImgFileHandle(IFH_ADD_NEW_LAYER);
			handle->SetNewLayerRect( &new_rc );
			handle->Do( m_pEditFile );
			ImgLayer_Ptr new_layer = handle->GetNewLayer().lock();

			pFile->SetSelectLayer( new_layer );

			const IplImageExt* old_img_ext = pLayer->GetLayerImage();
			old_img_ext->GetRangeIplImage(
				old_img,
				0, 0,
				old_rc.right - old_rc.left,
				old_rc.bottom - old_rc.top,
				0, 0,
				NULL);

			cvResize(old_img, new_img, m_interpolation);

			new_layer->ImgBltToLayer(
				0, 0,
				new_img->width,
				new_img->height,
				new_img,
				0, 0);

			LPUPDATE_DATA pData = new_layer->CreateUpdateData();
			pData->isAll = true;
			new_layer->PushUpdateData( pData );

			pFile->ReleaseImgFileHandle(handle);
			handle = NULL;
		}
		pLayer->ExtendLayer();
		pLayer = pLayer->GetOverLayer().lock();
	}
}