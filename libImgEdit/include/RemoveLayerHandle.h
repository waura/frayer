#ifndef _REMOVE_LAYER_HANDLE_H_
#define _REMOVE_LAYER_HANDLE_H_

#include "ImgFileHandle.h"

#include "IImgLayer.h"

class _EXPORTCPP RemoveLayerHandle
	: public ImgFileHandle
{
public:
	RemoveLayerHandle()
	{
		m_isRemovedLayer = false;
		m_removeLayerIndex = -1;
		m_removeLayer_lock = NULL;
	}
	~RemoveLayerHandle();

	void SetRemoveLayer(IImgLayer_weakPtr pLayer) {
		m_removeLayer = pLayer;
	}

	bool Do(ImgFile_weakPtr wpFile);
	void Redo(ImgFile_weakPtr wpFile);
	void Undo(ImgFile_weakPtr wpFile);

private:
	
	bool m_isRemovedLayer;

	int m_removeLayerIndex;
	IImgLayer_weakPtr m_removeLayer;
	IImgLayer_Ptr m_removeLayer_lock;
};

#endif //_REMOVE_LAYER_HANDLE_H_