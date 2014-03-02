#ifndef _ADD_NEW_LAYER_GROUP_HANDLE_H_
#define _ADD_NEW_LAYER_GROUP_HANDLE_H_

#include "ImgFileHandle.h"


class _EXPORTCPP AddNewLayerGroupHandle
	: public ImgFileHandle
{
public:
	AddNewLayerGroupHandle()
	{
		m_isAddedLayer = false;
		m_addLayerGroup_lock = NULL;
		m_addLayerIndex =  -1;
	}
	~AddNewLayerGroupHandle();

	inline void SetAddLayerIndex(int index) {
		m_addLayerIndex = index;
	}

	ImgLayerGroup_weakPtr GetNewLayeyGroup() const {
		return m_addLayerGroup;
	}

	bool Do(ImgFile_weakPtr wpFile);
	void Redo(ImgFile_weakPtr wpFile);
	void Undo(ImgFile_weakPtr wpFile);

private:
	bool m_isAddedLayer;
	ImgLayerGroup_weakPtr m_addLayerGroup;
	ImgLayerGroup_Ptr m_addLayerGroup_lock;
	int m_addLayerIndex;
};

#endif //_ADD_NEW_LAYER_GROUP_HANDLE_H_