#ifndef _ADDNEWLAYERHANDLE_H_
#define _ADDNEWLAYERHANDLE_H_

#include "ImgFileHandle.h"


class _EXPORTCPP AddNewLayerHandle
	: public ImgFileHandle
{
public:
	AddNewLayerHandle(){
		m_isAddedLayer = false;
		m_addLayer_lock = NULL;
		m_addLayerIndex = -1;
		m_layer_rect.top = m_layer_rect.bottom = 0;
		m_layer_rect.left = m_layer_rect.right = 0;
	}
	~AddNewLayerHandle();

	inline void SetNewLayerRect(const LPRECT lprc){
		m_layer_rect = (*lprc);
	}

	inline void SetAddLayerIndex(int index){
		m_addLayerIndex = index;
	}

	ImgLayer_weakPtr GetNewLayer() const{
		return m_addLayer;
	}

	bool Do(ImgFile_weakPtr wpFile);
	void Redo(ImgFile_weakPtr wpFile);
	void Undo(ImgFile_weakPtr wpFile);

private:
	RECT m_layer_rect;

	bool m_isAddedLayer;
	ImgLayer_weakPtr m_addLayer;
	ImgLayer_Ptr m_addLayer_lock;
	int m_addLayerIndex;
};

#endif //_ADDNEWLAYERHANDLE_H_