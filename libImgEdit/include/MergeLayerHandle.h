#ifndef _MERGELAYERHANDLE_H_
#define _MERGELAYERHANDLE_H_

#include "ImgFileHandle.h"

class IImgLayer;

typedef struct _MergingLayer{
	IImgLayer_Ptr pLayer;
	int LayerIndex;
} MergingLayer;

typedef std::vector<MergingLayer> MergingLayer_Vec;

class _EXPORTCPP MergeLayerHandle
	: public ImgFileHandle
{
public:
	MergeLayerHandle(){
		//m_addLayer = NULL;
	}
	~MergeLayerHandle();

	void AddMergingLayer(IImgLayer_Ptr pLayer);
	ImgLayer_weakPtr GetMergedLayer(){
		return m_addLayer;
	}

	bool Init(ImgFile_weakPtr wpFile){
		m_pEditFile = wpFile;
		return true;
	}

	bool Do(ImgFile_weakPtr wpFile);
	void Redo(ImgFile_weakPtr wpFile);
	void Undo(ImgFile_weakPtr wpFile);

private:
	ImgFile_weakPtr m_pEditFile;

	bool m_isAddedLayer;
	ImgLayer_weakPtr m_addLayer;
	int m_addLayerIndex;
	MergingLayer_Vec m_MergingLayer_Vec;
};

#endif //_MERGELAYERHANDLE_H_