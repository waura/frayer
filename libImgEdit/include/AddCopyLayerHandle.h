#ifndef _ADDCOPYLAYERHANDLE_H_
#define _ADDCOPYLAYERHANDLE_H_

#include "ImgFileHandle.h"

#include "ImgLayer.h"
#include "ImgFile.h"
#include "LayerSynthesizerMG.h"

class _EXPORTCPP AddCopyLayerHandle : public ImgFileHandle
{
public:
	AddCopyLayerHandle() :
		m_is_added_layer(false),
		m_addLayer_lock(NULL)
	{}
	~AddCopyLayerHandle();

	inline ImgLayer_weakPtr GetAddLayer(){
		return m_addLayer;
	}

	inline void SetCopyLayer(const ImgLayer_Ptr layer){
		m_copyLayer = layer;
	}

	bool Do(ImgFile_weakPtr wpFile);
	void Redo(ImgFile_weakPtr wpFile);
	void Undo(ImgFile_weakPtr wpFile);

private:
	bool m_is_added_layer;
	ImgLayer_weakPtr m_addLayer;
	ImgLayer_Ptr m_addLayer_lock;
	const_ImgLayer_weakPtr m_copyLayer;
};

#endif //_ADDCOPYLAYERHANDLE_H_