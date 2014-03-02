#ifndef _CHANGELAYERLINEHANDLE_H_
#define _CHANGELAYERLINEHANDLE_H_

#include "ImgFileHandle.h"
#include "ImgUpdateData.h"
#include "ImgLayer.h"
#include "ImgLayerGroup.h"
#include "ImgFile.h"

class _EXPORTCPP ChangeLayerLineHandle
	: public ImgFileHandle
{
public:
	ChangeLayerLineHandle() : 
		from_index(-1),
		to_index(-1){}
	~ChangeLayerLineHandle(){}

	void SetChangeIndex(int from, int to)
	{
		assert(from != to);

		from_index = from;
		to_index = to;
	}

	bool Do(ImgFile_weakPtr wpFile)
	{
		assert(from_index != to_index);

		ImgFile_Ptr pFile = wpFile.lock();

		ImgLayerGroup_Ptr pLGroup = pFile->GetRootLayerGroup();
		IImgLayer_Ptr from_layer = pLGroup->GetLayerAtIndex(from_index);
		IImgLayer_Ptr from_parent_layer = from_layer->GetParentLayer().lock();

		pFile->ChangeLayerLine(from_index, to_index);

		//update image
		LPUPDATE_DATA pData;

		pData = from_layer->CreateUpdateData();
		pData->isAll = true;
		from_layer->PushUpdateData(pData);

		pData = from_parent_layer->CreateUpdateData();
		pData->isAll = true;
		from_parent_layer->PushUpdateData(pData);

		pData = pLGroup->CreateUpdateData();
		pData->isAll = true;
		pLGroup->PushUpdateData(pData);
		return true;
	}
	void Redo(ImgFile_weakPtr wpFile)
	{
		Do(wpFile);
	}
	void Undo(ImgFile_weakPtr wpFile)
	{
		assert(from_index != to_index);

		ImgFile_Ptr pFile = wpFile.lock();

		ImgLayerGroup_Ptr pLGroup = pFile->GetRootLayerGroup();
		IImgLayer_Ptr from_layer = NULL;
		IImgLayer_Ptr from_parent_layer = NULL;

		if(from_index > to_index){
			from_layer = pLGroup->GetLayerAtIndex(to_index);
			from_parent_layer = from_layer->GetParentLayer().lock();
			pFile->ChangeLayerLine(to_index, from_index+1);
		}
		else{
			from_layer = pLGroup->GetLayerAtIndex(to_index-1);
			from_parent_layer = from_layer->GetParentLayer().lock();
			pFile->ChangeLayerLine(to_index-1, from_index);
		}

		//update image
		LPUPDATE_DATA pData;

		pData = from_layer->CreateUpdateData();
		pData->isAll = true;
		from_layer->PushUpdateData(pData);

		pData = from_parent_layer->CreateUpdateData();
		pData->isAll = true;
		from_parent_layer->PushUpdateData(pData);

		pData = pLGroup->CreateUpdateData();
		pData->isAll = true;
		pLGroup->PushUpdateData(pData);
	}

private:

	int from_index;
	int to_index;
};

#endif //_CHANGELAYERLINEHANDLE_H_