#include "stdafx.h"
#include "ChangeCanvasSizeHandle.h"

#include "IImgLayer.h"
#include "ImgLayerGroup.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


static void ExtendLayers(IImgLayer_Ptr pRoot)
{
	IImgLayer_Ptr pLayer = pRoot->GetBottomChildLayer();
	while(pLayer){
		if(pLayer->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER){
			ExtendLayers( pLayer );
		}
		pLayer->ExtendLayer();
		pLayer = pLayer->GetOverLayer().lock();
	}
}

bool ChangeCanvasSizeHandle::Do(ImgFile_weakPtr wpFile)
{
	m_pEditFile = wpFile;
	ImgFile_Ptr pFile = wpFile.lock();

	if((m_toSize.width <= 0) || (m_toSize.height <= 0)){
		char elog[256];
		wsprintf(elog, "ChangeCanvasSize::Do() to change size is invalid, width:%d height:%d");
		OutputError::PushLog(LOG_LEVEL::_ERROR, elog);
		return false;
	}

	m_fromSize = pFile->GetImgSize();

	//change size
	pFile->InitCanvasSize( m_toSize );

	ImgLayerGroup_Ptr pRootLayerGroup = pFile->GetRootLayerGroup();
	ExtendLayers((IImgLayer_Ptr) pRootLayerGroup);
	pRootLayerGroup->ExtendLayer();

	//update
	LPUPDATE_DATA pData = NULL;
	pData = pFile->CreateUpdateData();
	pData->isAll = true;
	pFile->PushUpdateData( pData );

	pData = pRootLayerGroup->CreateUpdateData();
	pData->isAll = true;
	pRootLayerGroup->PushUpdateData( pData );

	ImgMask_Ptr pMask = pFile->GetSelectMask();
	if (pMask) {
		pData = pMask->CreateUpdateData();
		pData->isAll = true;
		pMask->PushUpdateData( pData );
	}

	return true;
}

void ChangeCanvasSizeHandle::Redo(ImgFile_weakPtr wpFile)
{
	ImgFile_Ptr pFile = m_pEditFile.lock();

	//change size
	pFile->InitCanvasSize( m_toSize );
	//update
	LPUPDATE_DATA pData = NULL;
	pData = pFile->CreateUpdateData();
	pData->isAll = true;
	pFile->PushUpdateData( pData );

	ImgLayerGroup_Ptr pRootLayerGroup = pFile->GetRootLayerGroup();
	pData = pRootLayerGroup->CreateUpdateData();
	pData->isAll = true;
	pRootLayerGroup->PushUpdateData( pData );

	ImgMask_Ptr pMask = pFile->GetSelectMask();
	if (pMask) {
		pData = pMask->CreateUpdateData();
		pData->isAll = true;
		pMask->PushUpdateData( pData );
	}
}

void ChangeCanvasSizeHandle::Undo(ImgFile_weakPtr wpFile)
{
	ImgFile_Ptr pFile = m_pEditFile.lock();

	//change size
	pFile->InitCanvasSize( m_fromSize );
	//update
	LPUPDATE_DATA pData = NULL;
	pData = pFile->CreateUpdateData();
	pData->isAll = true;
	pFile->PushUpdateData( pData );

	ImgLayerGroup_Ptr pRootLayerGroup = pFile->GetRootLayerGroup();
	pData = pRootLayerGroup->CreateUpdateData();
	pData->isAll = true;
	pRootLayerGroup->PushUpdateData( pData );

	ImgMask_Ptr pMask = pFile->GetSelectMask();
	if (pMask) {
		pData = pMask->CreateUpdateData();
		pData->isAll = true;
		pMask->PushUpdateData( pData );
	}
}