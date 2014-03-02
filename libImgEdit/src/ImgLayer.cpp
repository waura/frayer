#include "stdafx.h"

#include "ImgLayer.h"

#include "ImgFile.h"
#include "ImgFileHandleMG.h"
#include "LayerSynthesizerMG.h"
#include "OutputError.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


_EXPORTCPP ImgLayer_Ptr CreateIEImgLayer(ImgFile_Ptr parent_file)
{
	return ImgLayer_Ptr(new ImgLayer(parent_file));
}

//_EXPORT void ReleaseIEImgLayer(ImgLayer_Ptr* ppLayer)
//{
//	if (*ppLayer) {
//		delete (*ppLayer);
//		(*ppLayer) = NULL;
//	}
//}

void ImgLayer::Init(const LPRECT lprc)
{
	IImgLayer::Init(lprc);

	//create new edit node queue
	ResourceLock( IE_LAYER_RESOURCE_ID::EDITNODE_QUEUE );

	m_pEditQueueForeGrnd = new EditQueue;
	m_pEditQueueBackGrnd = new EditQueue;

	ResourceUnlock( IE_LAYER_RESOURCE_ID::EDITNODE_QUEUE );

	m_IsLockPixelAlpha = false;
	m_IsLockVisiblePixel = false;
	m_IsLockLayerPosition = false;
	m_IsClipWithUnderLayer = false;

}

void ImgLayer::End()
{
	assert( _CrtCheckMemory() );

	//delete edit node queue
	ResourceLock( IE_LAYER_RESOURCE_ID::EDITNODE_QUEUE );

	if (m_pEditQueueForeGrnd) {
		EditQueue::iterator itr = m_pEditQueueForeGrnd->begin();
		for (; itr != m_pEditQueueForeGrnd->end(); itr++) {
			if ((*itr).pNode->node_flag == NODE_FLAG::MOVE_IMG) {
				delete (*itr).pNode;
			}
		}
		m_pEditQueueForeGrnd->clear();
		delete m_pEditQueueForeGrnd;
		m_pEditQueueForeGrnd = NULL;
	}
	if (m_pEditQueueBackGrnd) {
		EditQueue::iterator itr = m_pEditQueueBackGrnd->begin();
		for (; itr != m_pEditQueueBackGrnd->end(); itr++) {
			if ((*itr).pNode->node_flag == NODE_FLAG::MOVE_IMG) {
				delete (*itr).pNode;
			}
		}
		m_pEditQueueBackGrnd->clear();
		delete m_pEditQueueBackGrnd;
		m_pEditQueueBackGrnd = NULL;
	}

	ResourceUnlock( IE_LAYER_RESOURCE_ID::EDITNODE_QUEUE );

	IImgLayer::End();

	assert( _CrtCheckMemory() );
}

bool ImgLayer::isAlphaMask() const
{
	return (m_LayerSynthFourCC == IE_LAYER_SYNTH_FOURCC('a','m','s','k'));
}

EditNode* ImgLayer::CreateEditNode(const LPRECT lprc)
{
	CvSize size = m_Img->GetSize();
	EditNode* ret = new EditNode;
	
	ret->edit_img.InitToEqualSize(m_Img, IPL_DEPTH_8U, 4);
	ret->undo_img.InitToEqualSize(m_Img, IPL_DEPTH_8U, 4);
	ret->write_map.InitToEqualSize(m_Img, IPL_DEPTH_8U, 1);

	ret->node_rect = m_LayerRect;
	ret->blt_rect = *lprc;
	AndRect(&m_LayerRect, lprc, &(ret->blt_rect));

	ret->fourcc = LayerSynthesizerMG::GetDefaultLayerSynthFourCC();
	ret->isBakedWriteMap = false;

	return ret;
}

void ImgLayer::DestroyEditNode(EditNode *pEditNode)
{
	if (pEditNode) {
		delete pEditNode;
	}
}

void ImgLayer::MoveLayer(int move_x, int move_y)
{
	MoveNode* ret = new MoveNode;

	ret->move_x = move_x;
	ret->move_y = move_y;

	PushAddEditNodeQueue(ret);
}

void ImgLayer::PushAddEditNodeQueue(IEditNode* pNode)
{
	ResourceLock( IE_LAYER_RESOURCE_ID::EDITNODE_QUEUE );

	if (!m_pEditQueueBackGrnd->empty()) {
		EditQueue::iterator itr = m_pEditQueueBackGrnd->begin();
		for (; itr != m_pEditQueueBackGrnd->end(); itr++) {
			if (((*itr).pNode == pNode) && ((*itr).flag == EDIT_FLAG::ADD_EDIT)) {
				ResourceUnlock( IE_LAYER_RESOURCE_ID::EDITNODE_QUEUE );
				return;
			}
		}
	}
	EditQueueNode qnode;
	qnode.pNode = pNode;
	qnode.flag = EDIT_FLAG::ADD_EDIT;
	m_pEditQueueBackGrnd->push_back(qnode);

	ResourceUnlock( IE_LAYER_RESOURCE_ID::EDITNODE_QUEUE );
}

void ImgLayer::PushSubEditNodeQueue(IEditNode* pNode)
{
	ResourceLock( IE_LAYER_RESOURCE_ID::EDITNODE_QUEUE );

	if (!m_pEditQueueBackGrnd->empty()) {
		EditQueue::iterator itr = m_pEditQueueBackGrnd->begin();
		for (; itr != m_pEditQueueBackGrnd->end(); itr++) {
			if (((*itr).pNode == pNode) && ((*itr).flag == EDIT_FLAG::SUB_EDIT)) {
				ResourceUnlock( IE_LAYER_RESOURCE_ID::EDITNODE_QUEUE );
				return;
			}
		}
	}
	EditQueueNode qnode;
	qnode.pNode = pNode;
	qnode.flag = EDIT_FLAG::SUB_EDIT;
	m_pEditQueueBackGrnd->push_back(qnode);

	ResourceUnlock( IE_LAYER_RESOURCE_ID::EDITNODE_QUEUE );
}

bool ImgLayer::Update(const LPRECT enable_lprc, LPRECT updated_lprc)
{
	if (m_pUpdateDataForeGrnd_List->empty()) {
		return false;
	}

	UPDATE_DATA udLayer;
	bool isLayerRect = false;
	udLayer.isAll = false;

	//更新情報の縮小化
	ResourceLock( IE_LAYER_RESOURCE_ID::UPDATE_DATA_QUEUE );

	LPUpdateData_List::iterator itr = m_pUpdateDataForeGrnd_List->begin();
	for (; itr != m_pUpdateDataForeGrnd_List->end(); itr++) {
		bool isAll = (*itr)->isAll;
		
		switch ((*itr)->update_flag) {
			case UPDATE_FLAG::UPDATE_LAYER_IMAGE:
				if (isAll) {//全体更新
					udLayer.isAll = isAll;
				}
				else if (udLayer.isAll == false) {//部分更新
					if (isLayerRect == false) {//範囲の初期値がないなら
						udLayer.rect = (*itr)->rect;
						isLayerRect = true;
					}
					else {//範囲がすでに存在しているなら
						OrRect(&((*itr)->rect), &(udLayer.rect), &(udLayer.rect));
					}
				}
				break;
			case UPDATE_FLAG::NO_UPDATE:
				break;
			default:
				OutputError::PushLog( LOG_LEVEL::_WARN, "ImgLayer::Update() unknown update_flag");
				break;
		}

		//最大範囲の更新だったら抜ける。
		if ((*itr)->update_flag == UPDATE_FLAG::UPDATE_LAYER_IMAGE &&
			isAll)
		{
			break;
		}
	}
	ResourceUnlock( IE_LAYER_RESOURCE_ID::UPDATE_DATA_QUEUE );

	if (udLayer.isAll) {
		udLayer.rect = m_LayerRect;
		isLayerRect = true;
	}
	if (enable_lprc) {
		AndRect(&(udLayer.rect), enable_lprc, &(udLayer.rect));
	}
	
	//update
	if (isLayerRect) {
		UpdateEditNode(&udLayer.rect);
	}
	(*updated_lprc) = udLayer.rect;

	return IImgLayer::Update(enable_lprc, updated_lprc);
}

void ImgLayer::ClearUpdateData()
{
	ResourceLock( IE_LAYER_RESOURCE_ID::EDITNODE_QUEUE );
	m_pEditQueueForeGrnd->clear();
	ResourceUnlock( IE_LAYER_RESOURCE_ID::EDITNODE_QUEUE );

	IImgLayer::ClearUpdateData();
}

void ImgLayer::LockUpdateData()
{
	//swap back groud qeue to fore ground qeue
	ResourceLock( IE_LAYER_RESOURCE_ID::EDITNODE_QUEUE );

	EditQueue* etmp = m_pEditQueueForeGrnd;
	m_pEditQueueForeGrnd = m_pEditQueueBackGrnd;
	m_pEditQueueBackGrnd = etmp;

	ResourceUnlock( IE_LAYER_RESOURCE_ID::EDITNODE_QUEUE );

	IImgLayer::LockUpdateData();
}

void ImgLayer::UnlockUpdateData()
{
	IImgLayer::UnlockUpdateData();
}

void ImgLayer::SetPixel(int x, int y, const UCvPixel* pixel)
{
	m_Img->SetPixel(
		x - m_LayerRect.left,
		y - m_LayerRect.top,
		pixel);
}

void ImgLayer::RectFillColor(
	 int posX,
	 int posY,
	 int width,
	 int height,
	 const UCvPixel* fillcolor)
{
	m_Img->RectFillColor(
		posX - m_LayerRect.left,
		posY - m_LayerRect.top,
		width,
		height,
		fillcolor);
}

void ImgLayer::RectFillAlpha(
	int posX,
	int posY,
	int width,
	int height,
	uint8_t alpha)
{
	m_Img->RectFillAlpha(
		posX - m_LayerRect.left,
		posY - m_LayerRect.top,
		width,
		height,
		alpha);
}

void ImgLayer::ImgBltToLayer(
	int posX,
	int posY,
	int width,
	int height,
	const IplImage *src,
	int startX,
	int startY)
{
	m_Img->ImgBlt(
		posX - m_LayerRect.left,
		posY - m_LayerRect.top,
		width,
		height,
		src,
		startX,
		startY);
}

//////////////////////////////////
/*!
	pLayerに自身をコピーする。
	@return 成功すればtrue、そうでなければfalseを返す。
*/
bool ImgLayer::CopyImgLayer(ImgLayer_Ptr pLayer) const
{
	//画像のコピー
	RECT rc;
	this->GetLayerRect(&rc);
	pLayer->Init(&rc);
	m_Img->Copy(pLayer->m_Img);

	//レイヤー名のコピー
	char name[MAX_IMG_LAYER_NAME];
	this->GetName( name );
	pLayer->SetName( name );

	return true;
}

void ImgLayer::Split(
	IplImage* dst0,
	IplImage* dst1,
	IplImage* dst2,
	IplImage* dst3) const
{
	m_Img->Split(
		0, 0,
		m_LayerRect.right - m_LayerRect.left,
		m_LayerRect.bottom - m_LayerRect.top,
		dst0, dst1, dst2, dst3);
}

void ImgLayer::Merge(
	int posX,
	int posY,
	int width,
	int height,
	const IplImage* src0,
	const IplImage* src1,
	const IplImage* src2,
	const IplImage* src3)
{
	m_Img->Merge(
		posX - m_LayerRect.left,
		posY - m_LayerRect.top,
		width,
		height,
		src0,
		src1,
		src2,
		src3);
}

void ImgLayer::BltRectSynthPixel(
	IplImageExt* dst,
	int d_x,
	int d_y,
	int s_x,
	int s_y,
	int width,
	int height) const
{
	if (m_Alpha == 0.0)
		return;

	IImgLayer_Ptr mask_layer = GetUnderLayer().lock();
	if (IsClipWithUnderLayer() && mask_layer) {
		RECT mask_rc;
		mask_layer->GetLayerRect(&mask_rc);
		LayerSynthesizerMG::BltSynth(
			m_LayerSynthFourCC,
			m_Alpha,
			dst,
			d_x, d_y,
			width, height,
			m_Img,
			s_x - m_LayerRect.left,
			s_y - m_LayerRect.top,
			mask_layer->GetLayerImage(),
			s_x - mask_rc.left,
			s_y - mask_rc.top);
	} else {
		LayerSynthesizerMG::BltSynth(
			m_LayerSynthFourCC,
			m_Alpha,
			dst,
			d_x, d_y,
			width, height,
			m_Img,
			s_x - m_LayerRect.left,
			s_y - m_LayerRect.top);
	}
}

/////////////////////////
/*
	描画イメージのアップデート
	@param[in] lprc アップデートする範囲
*/
void ImgLayer::UpdateEditNode(LPRECT lprc)
{
	ResourceLock( IE_LAYER_RESOURCE_ID::EDITNODE_QUEUE );

	if (!m_pEditQueueForeGrnd->empty()) {
		EditQueue::iterator itr = m_pEditQueueForeGrnd->begin();
		for (; itr != m_pEditQueueForeGrnd->end(); itr++) {
			if ((*itr).flag == EDIT_FLAG::ADD_EDIT) {
				if ((*itr).pNode->node_flag == NODE_FLAG::EDIT_DRAW) {
					AddEditNode(dynamic_cast<EditNode*>((*itr).pNode), lprc);
				}
				else if ((*itr).pNode->node_flag == NODE_FLAG::MOVE_IMG) {
					MoveNode* pNode = dynamic_cast<MoveNode*>((*itr).pNode);
					assert(pNode);
	
					m_LayerRect.left += pNode->move_x;
					m_LayerRect.right += pNode->move_x;
					m_LayerRect.top += pNode->move_y;
					m_LayerRect.bottom += pNode->move_y;

					delete pNode;
					(*itr).pNode = NULL;
				}
			}
			else if ((*itr).flag == EDIT_FLAG::SUB_EDIT) {
				if ((*itr).pNode->node_flag == NODE_FLAG::EDIT_DRAW) {
					SubEditNode(dynamic_cast<EditNode*>((*itr).pNode), lprc);
				}
				else if ((*itr).pNode->node_flag == NODE_FLAG::MOVE_IMG) {
					assert(0);
					//MoveNode* pNode = dynamic_cast<MoveNode*>((*itr).pNode);
					//assert(pNode);
	
					//m_LayerRect.left -= pNode->move_x;
					//m_LayerRect.right -= pNode->move_x;
					//m_LayerRect.top -= pNode->move_y;
					//m_LayerRect.bottom -= pNode->move_y;
				}
			}
		}
	}

	ResourceUnlock( IE_LAYER_RESOURCE_ID::EDITNODE_QUEUE );
}

///////////////////////////////////
/*!
	pNodeを加算する。
	@param[in] pNode
	@param[in] lprc 更新範囲
*/
void ImgLayer::AddEditNode(EditNode *pNode, const LPRECT lprc)
{
	if(!pNode) return;

	RECT rc; 
	if (lprc) {
		//更新範囲とコピー先範囲の論理積
		AndRect(lprc, &(pNode->blt_rect), &rc);
	}
	else {
		rc = pNode->blt_rect;
	}

	if (!pNode->isBakedWriteMap) {
		//blt undo image
		BltLayerUndoOp bltLayerUndoOp;
		AllMaskOp allMaskOp;
		pNode->undo_img.OperateBlt<BltLayerUndoOp, AllMaskOp>(
			bltLayerUndoOp,
			allMaskOp,
			rc.left - pNode->node_rect.left,
			rc.top - pNode->node_rect.top,
			rc.right - rc.left,
			rc.bottom - rc.top,
			m_Img,
			rc.left - m_LayerRect.left,
			rc.top - m_LayerRect.top,
			&(pNode->write_map),
			rc.left - pNode->node_rect.left,
			rc.top - pNode->node_rect.top);
		
		//write pixel mark
		WritePixelMark writePixelMark;
		NormMaskOp normMaskOp;
		pNode->write_map.OperateBlt<WritePixelMark, NormMaskOp>(
			writePixelMark,
			normMaskOp,
			rc.left - pNode->node_rect.left,
			rc.top - pNode->node_rect.top,
			rc.right - rc.left,
			rc.bottom - rc.top,
			NULL,
			0,
			0,
			&(pNode->edit_img),
			rc.left - pNode->node_rect.left,
			rc.top - pNode->node_rect.top);
	}

	//blt edit image
	const IplImage* mask = NULL;
	ImgFile_Ptr pParentFile = this->GetParentFile().lock();
	if (pParentFile->GetMaskState() == IE_MASK_STATE_ID::VALID) {
		mask = pParentFile->GetDrawMaskImg();
	}

	if (!m_IsLockPixelAlpha) {
		LayerSynthesizerMG::BlendSynth<NormalCopy>(
			pNode->fourcc,
			1.0,
			m_Img,
			rc.left - m_LayerRect.left,
			rc.top - m_LayerRect.top,
			rc.right - rc.left,
			rc.bottom - rc.top,
			&(pNode->undo_img),
			rc.left - pNode->node_rect.left,
			rc.top - pNode->node_rect.top,
			&(pNode->edit_img),
			rc.left - pNode->node_rect.left,
			rc.top - pNode->node_rect.top,
			mask,
			rc.left,
			rc.top);
	} else {
		LayerSynthesizerMG::BlendSynth<AlphaLockCopy>(
			pNode->fourcc,
			1.0,
			m_Img,
			rc.left - m_LayerRect.left,
			rc.top - m_LayerRect.top,
			rc.right - rc.left,
			rc.bottom - rc.top,
			&(pNode->undo_img),
			rc.left - pNode->node_rect.left,
			rc.top - pNode->node_rect.top,
			&(pNode->edit_img),
			rc.left - pNode->node_rect.left,
			rc.top - pNode->node_rect.top,
			mask,
			rc.left,
			rc.top);
	}
}

///////////////////////////////////
/*!
	pNodeを減算する。
	@param[in] pNode
	@param[in] lprc 更新範囲
*/
void ImgLayer::SubEditNode(EditNode *pNode, const LPRECT lprc)
{
	if (!pNode) return;

	RECT rc; 
	if (lprc) {
		//更新範囲とコピー先範囲の論理積
		AndRect(lprc, &(pNode->blt_rect), &rc);
	}
	else {
		rc = pNode->blt_rect;
	}

	m_Img->ImgBlt(
		rc.left - m_LayerRect.left,
		rc.top - m_LayerRect.top,
		rc.right - rc.left,
		rc.bottom - rc.top,
		&(pNode->undo_img),
		rc.left - pNode->node_rect.left,
		rc.top - pNode->node_rect.top,
		IPLEXT_RASTER_CODE::COPY_BY_BINARY_MASK,
		&(pNode->write_map),
		rc.left - pNode->node_rect.left,
		rc.top - pNode->node_rect.top);
}
