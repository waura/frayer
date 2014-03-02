#include "stdafx.h"

#include "AddNewLayerGroupHandle.h"

#include "ImgLayerGroup.h"
#include "ImgFile.h"
#include "LayerSynthesizerMG.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


AddNewLayerGroupHandle::~AddNewLayerGroupHandle()
{
	//if (!m_isAddedLayer) {
	//	if (m_addLayerGroup){
	//		ReleaseIEImgLayerGroup(&m_addLayerGroup);
	//	}
	//}
}

bool AddNewLayerGroupHandle::Do(ImgFile_weakPtr wpFile)
{
	ImgFile_Ptr pFile = wpFile.lock();
	CvSize img_size = pFile->GetImgSize();

	m_addLayerGroup = m_addLayerGroup_lock = CreateIEImgLayerGroup( pFile );

	RECT layer_rect;
	layer_rect.top = 0;
	layer_rect.left = 0;
	layer_rect.right = img_size.width;
	layer_rect.bottom = img_size.height;
	m_addLayerGroup_lock->Init(&layer_rect);

	char layer_name[256];
	wsprintf(layer_name, "レイヤーグループ%d", pFile->GetLayerSize());

	m_addLayerGroup_lock->SetName(layer_name);
	m_addLayerGroup_lock->SetLayerSynthFourCC( LayerSynthesizerMG::GetDefaultLayerSynthFourCC() );

	if (m_addLayerIndex != -1) {
		pFile->AddLayer(m_addLayerGroup_lock, m_addLayerIndex);
	} else {
		pFile->AddLayer(m_addLayerGroup_lock);
	}
	pFile->SetSelectLayer(m_addLayerGroup_lock);

	m_isAddedLayer = true;
	m_addLayerGroup_lock = NULL;

	//
	LPUPDATE_DATA pData = pFile->CreateUpdateData();
	pData->isAll = true;
	pFile->PushUpdateData(pData);

	return true;	
}

void AddNewLayerGroupHandle::Redo(ImgFile_weakPtr wpFile)
{
	ImgFile_Ptr pFile = wpFile.lock();
	if (m_addLayerIndex != -1) {
		pFile->AddLayer(m_addLayerGroup.lock(), m_addLayerIndex);
	} else {
		pFile->AddLayer(m_addLayerGroup.lock());
	}
	m_isAddedLayer = true;
	m_addLayerGroup_lock = NULL;

	//
	LPUPDATE_DATA pData = pFile->CreateUpdateData();
	pData->isAll = true;
	pFile->PushUpdateData(pData);
}

void AddNewLayerGroupHandle::Undo(ImgFile_weakPtr wpFile)
{
	ImgFile_Ptr pFile = wpFile.lock();

	m_addLayerGroup_lock = m_addLayerGroup.lock();
	pFile->RemoveLayer(m_addLayerGroup_lock);
	m_isAddedLayer = false;

	//
	LPUPDATE_DATA pData = pFile->CreateUpdateData();
	pData->isAll = true;
	pFile->PushUpdateData(pData);
}