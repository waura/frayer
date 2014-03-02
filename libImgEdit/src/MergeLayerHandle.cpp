#include "stdafx.h"

#include "MergeLayerHandle.h"

#include "ImgLayer.h"
#include "ImgLayerGroup.h"
#include "ImgFile.h"

#include "AddNewLayerHandle.h"
#include "ImgFileHandleMG.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


class MLayerSortOp {
public:
	bool operator()(const MergingLayer& left, const MergingLayer& right) const {
		return left.LayerIndex < right.LayerIndex;
	}
};

class MLayerFindOp {
public:
	MLayerFindOp(IImgLayer_Ptr _pLayer){
		pLayer = _pLayer;
	}

	bool operator()(const MergingLayer& mlayer) const {
		return mlayer.pLayer == pLayer;
	}
private:
	IImgLayer_Ptr pLayer;
};

static int GetMLayerVecSize(MergingLayer_Vec& vector)
{
	int size = 0;
	MergingLayer_Vec::iterator itr = vector.begin();
	for(; itr != vector.end(); itr++){
		if((*itr).pLayer->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER){
			ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>((*itr).pLayer);
			size += pLGroup->GetMaxLayerIndex() + 2; //2 = layer group + close virtual layer
		}
		else{
			size++;
		}
	}
	return size;
}

static bool FindLayerFromMLayerVec(MergingLayer_Vec& vector, IImgLayer_Ptr pLayer)
{
	MergingLayer_Vec::iterator itr = vector.begin();
	for(; itr != vector.end(); itr++){
		if((*itr).pLayer->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER){
			ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>((*itr).pLayer);
			if(pLGroup->GetLayerIndex(pLayer) != -1){
				return true;
			}
		}
		else{
			if((*itr).pLayer == pLayer){
				return true;
			}
		}
	}
	return false;
}

MergeLayerHandle::~MergeLayerHandle()
{
	int cnt = m_addLayer.lock().use_count();
	MergingLayer_Vec::iterator itr = m_MergingLayer_Vec.begin();
	for (; itr != m_MergingLayer_Vec.end(); itr++) {
		int cnt = (*itr).pLayer.use_count();
	}
	//if (m_isAddedLayer) {
	//	//delete merging layer
	//	MergingLayer_Vec::iterator itr = m_MergingLayer_Vec.begin();
	//	for (; itr != m_MergingLayer_Vec.end(); itr++) {
	//		IImgLayer_Ptr pLayer = (*itr).pLayer;
	//		ReleaseIEIImgLayer(&pLayer);
	//	}
	//}
	//else {
	//	//delete merged layer
	//	if (m_addLayer) {
	//		ReleaseIEImgLayer(&m_addLayer);
	//	}
	//}
}

void MergeLayerHandle::AddMergingLayer(IImgLayer_Ptr pLayer)
{
	if(!FindLayerFromMLayerVec(m_MergingLayer_Vec, pLayer)){
		ImgFile_Ptr pFile = m_pEditFile.lock();
		ImgLayerGroup_Ptr pGLayer = pFile->GetRootLayerGroup();
		MergingLayer mdata;
		mdata.pLayer = pLayer;
		mdata.LayerIndex = pFile->GetLayerIndex(pLayer);
		m_MergingLayer_Vec.push_back(mdata);
	}
}

bool MergeLayerHandle::Do(ImgFile_weakPtr wpFile)
{
	if(m_MergingLayer_Vec.empty()) return false;

	ImgFile_Ptr pFile = wpFile.lock();
	//sort by layer index in ascending order
	std::sort(m_MergingLayer_Vec.begin(), m_MergingLayer_Vec.end(), MLayerSortOp());
	//remove merging layersfrom file
	MergingLayer_Vec::reverse_iterator ritr = m_MergingLayer_Vec.rbegin();
	for(; ritr != m_MergingLayer_Vec.rend(); ritr++){
		pFile->RemoveLayer( (*ritr).pLayer );
	}
	//get insert index
	m_addLayerIndex = (*(m_MergingLayer_Vec.rbegin())).LayerIndex - GetMLayerVecSize(m_MergingLayer_Vec);

	//create new layer
	AddNewLayerHandle* addNewLayerHdl = (AddNewLayerHandle*)pFile->CreateImgFileHandle(IFH_ADD_NEW_LAYER);
	addNewLayerHdl->SetAddLayerIndex( m_addLayerIndex );
	addNewLayerHdl->Do( wpFile );
	m_addLayer = addNewLayerHdl->GetNewLayer();
	pFile->ReleaseImgFileHandle(addNewLayerHdl);
	addNewLayerHdl = NULL;

	ImgLayer_Ptr addLayer = m_addLayer.lock();
	//merge layers
	RECT rc;
	addLayer->GetLayerRect(&rc);
	IplImage* merge_img = cvCreateImage(cvSize(rc.right - rc.left, rc.bottom - rc.top), IPL_DEPTH_8U, 4);
	cvSet(merge_img, cvScalar(0,0,0,0));
	MergingLayer_Vec::iterator itr = m_MergingLayer_Vec.begin();
	for(; itr != m_MergingLayer_Vec.end(); itr++){
		(*itr).pLayer->BltRectSynthPixel(
			merge_img,
			0, 0,
			rc.left,
			rc.top,
			rc.right - rc.left,
			rc.bottom - rc.top);
	}
	addLayer->ImgBltToLayer(
		rc.left,
		rc.top,
		rc.right - rc.left,
		rc.bottom - rc.top,
		merge_img,
		0, 0);

	cvReleaseImage(&merge_img);


	m_isAddedLayer = true;

	//update display
	LPUPDATE_DATA pData1 = pFile->GetRootLayerGroup()->CreateUpdateData();
	pData1->isAll = true;
	pFile->GetRootLayerGroup()->PushUpdateData(pData1);

	LPUPDATE_DATA pData2 = pFile->CreateUpdateData();
	pData2->isAll = true;
	pFile->PushUpdateData(pData2);

	return true;
}

void MergeLayerHandle::Redo(ImgFile_weakPtr wpFile)
{
	ImgFile_Ptr pFile = wpFile.lock();
	ImgLayer_Ptr addLayer = m_addLayer.lock();

	//remove merging layersfrom file
	MergingLayer_Vec::reverse_iterator ritr = m_MergingLayer_Vec.rbegin();
	for(; ritr != m_MergingLayer_Vec.rend(); ritr++){
		pFile->RemoveLayer( (*ritr).pLayer );
	}
	//insert marged layer
	if (m_addLayerIndex != -1) {
		pFile->AddLayer(addLayer, m_addLayerIndex);
	} else {
		pFile->AddLayer(addLayer);
	}
	m_isAddedLayer = true;

	//update display
	LPUPDATE_DATA pData1 = pFile->GetRootLayerGroup()->CreateUpdateData();
	pData1->isAll = true;
	pFile->GetRootLayerGroup()->PushUpdateData(pData1);

	LPUPDATE_DATA pData2 = pFile->CreateUpdateData();
	pData2->isAll = true;
	pFile->PushUpdateData(pData2);
}

void MergeLayerHandle::Undo(ImgFile_weakPtr wpFile)
{
	ImgFile_Ptr pFile = wpFile.lock();
	ImgLayer_Ptr addLayer = m_addLayer.lock();

	//remove marged layer
	pFile->RemoveLayer(addLayer);
	//insert margind layer
	MergingLayer_Vec::iterator itr = m_MergingLayer_Vec.begin();
	for(; itr != m_MergingLayer_Vec.end(); itr++){
		pFile->AddLayer((*itr).pLayer, (*itr).LayerIndex);
	}
	m_isAddedLayer = false;

	//update display
	LPUPDATE_DATA pData1 = pFile->GetRootLayerGroup()->CreateUpdateData();
	pData1->isAll = true;
	pFile->GetRootLayerGroup()->PushUpdateData(pData1);

	LPUPDATE_DATA pData2 = pFile->CreateUpdateData();
	pData2->isAll = true;
	pFile->PushUpdateData(pData2);
}