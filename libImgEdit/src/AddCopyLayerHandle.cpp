#include "stdafx.h"

#include "AddCopyLayerHandle.h"
#include "ImgLayerGroup.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

AddCopyLayerHandle::~AddCopyLayerHandle()
{
//	if (!m_is_added_layer) {
//		if(m_addLayer) {
//#ifdef _DEBUG
//			ImgLayerGroup_Ptr pLGroup = (ImgLayerGroup_Ptr)m_addLayer->GetParentLayer();
//			assert(pLGroup->GetLayerIndex(m_addLayer) == -1);
//#endif //_DEBUG
//			ReleaseIEImgLayer(&m_addLayer);
//		}
//	}
}

bool AddCopyLayerHandle::Do(ImgFile_weakPtr wpFile)
{
	ImgFile_Ptr pFile = wpFile.lock();

	m_addLayer = m_addLayer_lock = CreateIEImgLayer( pFile );

	IImgLayer_weakPtr wlayer = pFile->GetSelectLayer();
	if (wlayer.expired()) {
		OutputError::PushLog(LOG_LEVEL::_ERROR, "AddCopyLayerHandle::Do() select layer is expired");
		return false;
	}

	IImgLayer_Ptr layer = wlayer.lock();
	if(layer->GetLayerType() != IE_LAYER_TYPE::NORMAL_LAYER){
		OutputError::PushLog(LOG_LEVEL::_ERROR, "AddCopyLayerHandle::Do() select layer is not normal layer");
		return false;
	}

	if(m_copyLayer.lock() == NULL){
		m_copyLayer = std::dynamic_pointer_cast<ImgLayer>(layer);
	}

	const_ImgLayer_Ptr copy_layer = m_copyLayer.lock();

	copy_layer->CopyImgLayer( m_addLayer_lock );
	
	char layer_name[MAX_IMG_LAYER_NAME];
	copy_layer->GetName(layer_name);
	sprintf(layer_name,  "%s‚ÌƒRƒs[", layer_name);
	m_addLayer_lock->SetName(layer_name);
	m_addLayer_lock->SetLayerSynthFourCC( LayerSynthesizerMG::GetDefaultLayerSynthFourCC() );

	pFile->AddLayer(m_addLayer_lock);
	m_is_added_layer = true;
	m_addLayer_lock = NULL;

	LPUPDATE_DATA data = pFile->CreateUpdateData();
	data->isAll = true;
	pFile->PushUpdateData(data);
	return true;
}

void AddCopyLayerHandle::Redo(ImgFile_weakPtr wpFile){
	ImgFile_Ptr pFile = wpFile.lock();

	pFile->AddLayer(m_addLayer.lock());
	m_is_added_layer = true;
	m_addLayer_lock = NULL;

	LPUPDATE_DATA data = pFile->CreateUpdateData();
	data->isAll = true;
	pFile->PushUpdateData(data);
}

void AddCopyLayerHandle::Undo(ImgFile_weakPtr wpFile){
	ImgFile_Ptr pFile = wpFile.lock();

	m_addLayer_lock = m_addLayer.lock();
	pFile->RemoveLayer(m_addLayer_lock);
	m_is_added_layer = false;

	LPUPDATE_DATA data=pFile->CreateUpdateData();
	data->isAll = true;
	pFile->PushUpdateData(data);
}