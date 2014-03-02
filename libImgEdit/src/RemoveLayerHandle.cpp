#include "stdafx.h"

#include "RemoveLayerHandle.h"

#include "ImgLayer.h"
#include "ImgLayerGroup.h"
#include "ImgFile.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


RemoveLayerHandle::~RemoveLayerHandle()
{
//	if (m_isRemovedLayer) {
//		if (m_removeLayer) {
//#ifdef _DEBUG
//			ImgLayerGroup_Ptr pLGroup = (ImgLayerGroup_Ptr)m_removeLayer->GetParentLayer();
//			assert(pLGroup->GetLayerIndex(m_removeLayer) == -1);
//#endif //_DEBUG
//			ReleaseIEIImgLayer(&m_removeLayer);
//		}
//	}
}

bool RemoveLayerHandle::Do(ImgFile_weakPtr wpFile)
{
	assert(m_removeLayer_lock == NULL);

	if (m_removeLayer.lock()) {
		ImgFile_Ptr pFile = wpFile.lock();
		m_removeLayerIndex = pFile->GetLayerIndex(m_removeLayer);
		Redo(pFile);
		return true;
	}
	return false;
}

void RemoveLayerHandle::Redo(ImgFile_weakPtr wpFile)
{
	assert(m_removeLayer_lock == NULL);

	m_removeLayer_lock = m_removeLayer.lock();
	if (m_removeLayer_lock) {
		wpFile.lock()->RemoveLayer(m_removeLayer_lock);
	}

	m_isRemovedLayer = true;
}

void RemoveLayerHandle::Undo(ImgFile_weakPtr wpFile)
{
	assert(m_removeLayer_lock);

	wpFile.lock()->AddLayer(m_removeLayer_lock, m_removeLayerIndex);
	m_removeLayer_lock = NULL;

	m_isRemovedLayer = false;
}