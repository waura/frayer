#include "stdafx.h"

#include "AddNewLayerHandle.h"

#include "ImgLayer.h"
#include "ImgLayerGroup.h"
#include "ImgFile.h"
#include "LayerSynthesizerMG.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


AddNewLayerHandle::~AddNewLayerHandle()
{
	//if (!m_isAddedLayer) {
	//	if (m_addLayer) {
	//		ReleaseIEImgLayer(&m_addLayer);
	//	}
	//}
}

bool AddNewLayerHandle::Do(ImgFile_weakPtr wpFile)
{
	ImgFile_Ptr pFile = wpFile.lock();
	CvSize img_size = pFile->GetImgSize();

	ImgLayer_Ptr pLayer;
	m_addLayer = pLayer = CreateIEImgLayer(pFile);

	if ((m_layer_rect.top == 0) && (m_layer_rect.bottom == 0)) {
		m_layer_rect.top = 0;
		m_layer_rect.left = 0;
		m_layer_rect.right = img_size.width;
		m_layer_rect.bottom = img_size.height;
	}
	pLayer->Init(&m_layer_rect);


	char layer_name[256];
	wsprintf(layer_name, "ƒŒƒCƒ„[%d", pFile->GetLayerSize());

	pLayer->SetName(layer_name);
	pLayer->SetLayerSynthFourCC( LayerSynthesizerMG::GetDefaultLayerSynthFourCC() );

	if (m_addLayerIndex != -1) {
		pFile->AddLayer(pLayer, m_addLayerIndex);
	} else {
		pFile->AddLayer(pLayer);
	}
	m_isAddedLayer = true;

	pFile->SetSelectLayer(pLayer);
	m_addLayer_lock = NULL;

	return true;
}

void AddNewLayerHandle::Redo(ImgFile_weakPtr wpFile)
{
	ImgFile_Ptr pFile = wpFile.lock();

	if (m_addLayerIndex != -1) {
		pFile->AddLayer(m_addLayer.lock(), m_addLayerIndex);
	} else {
		pFile->AddLayer(m_addLayer.lock());
	}
	m_isAddedLayer = true;
	m_addLayer_lock = NULL;

	LPUPDATE_DATA pData = pFile->CreateUpdateData();
	pData->isAll = true;
	pFile->PushUpdateData(pData);
}

void AddNewLayerHandle::Undo(ImgFile_weakPtr wpFile)
{
	ImgFile_Ptr pFile = wpFile.lock();

	m_addLayer_lock = m_addLayer.lock();
	pFile->RemoveLayer(m_addLayer_lock);
	m_isAddedLayer = false;

	IImgLayer_Ptr pParentLayer = m_addLayer_lock->GetParentLayer().lock();
	LPUPDATE_DATA pData1 = pParentLayer->CreateUpdateData();
	pData1->isAll = true;
	pParentLayer->PushUpdateData(pData1);

	LPUPDATE_DATA pData2 = pFile->CreateUpdateData();
	pData2->isAll = true;
	pFile->PushUpdateData(pData2);
}