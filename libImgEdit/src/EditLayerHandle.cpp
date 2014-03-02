#include "stdafx.h"
#include "EditLayerHandle.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


bool EditLayerHandle::Do(ImgFile_weakPtr wpFile)
{
	ImgFile_Ptr pFile = wpFile.lock();
	if (m_editLayerIndex == -1) {
		int select_layer_index = pFile->GetLayerIndex(pFile->GetSelectLayer());
		SetEditLayerIndex(select_layer_index);
	}
	IImgLayer_weakPtr _pLayer = pFile->GetLayerAtIndex(m_editLayerIndex);
	IImgLayer_Ptr pLayer = _pLayer.lock();

	assert(pLayer);
	if (pLayer == NULL) {
		OutputError::PushLog(LOG_LEVEL::_ERROR, "invalid layer pointer in EditLayerHandle");
		return false;
	}
	if (pLayer->GetLayerType() != IE_LAYER_TYPE::NORMAL_LAYER) {
		OutputError::PushLog(LOG_LEVEL::_ERROR, "wrong layer type in EditLayerHandle");
		return false;
	}

	if (pLayer) {
		if (!pLayer->IsVisible()) {
			OutputError::Alert(IE_ERROR_LAYER_INVISIBLE);
			return false;
		}
	} else {
		OutputError::Alert(IE_ERROR_SELECT_LAYER_DONT_EXIST);
		return false;
	}

	m_pEditLayer = std::dynamic_pointer_cast<ImgLayer>(pLayer);
	return true;
}
void EditLayerHandle::Redo(ImgFile_weakPtr wpFile)
{
	//update layer pointer
	IImgLayer_weakPtr _pLayer = wpFile.lock()->GetLayerAtIndex(m_editLayerIndex);
	m_pEditLayer = std::dynamic_pointer_cast<ImgLayer>(_pLayer.lock());
	this->Update(&(m_editNode->blt_rect));
}

void EditLayerHandle::Undo(ImgFile_weakPtr wpFile)
{
	//update layer pointer
	IImgLayer_weakPtr _pLayer = wpFile.lock()->GetLayerAtIndex(m_editLayerIndex);
	m_pEditLayer = std::dynamic_pointer_cast<ImgLayer>(_pLayer.lock());

	ImgLayer_Ptr pEditLayer = m_pEditLayer.lock();
	pEditLayer->PushSubEditNodeQueue(m_editNode);

	LPUPDATE_DATA data = pEditLayer->CreateUpdateData();
	data->isAll = false;
	data->rect = m_editNode->blt_rect;
	pEditLayer->PushUpdateData(data);

	ImgFile_Ptr pFile = wpFile.lock();
	LPUPDATE_DATA data2 = pFile->CreateUpdateData();
	data2->isAll = false;
	data2->rect = m_editNode->blt_rect;
	pFile->PushUpdateData(data2);
}