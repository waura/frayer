#include "stdafx.h"

#include "ImgMask.h"
#include "MaskSynthesizerMG.h"
#include "OutputError.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


ImgMask::ImgMask(ImgFile_weakPtr parent_file)
{
	m_IsInited = false;
	m_IsVisible = true;
	m_name = "";
	m_pParentFile = parent_file;

	m_Img = NULL;

	m_pEditQueueForeGrnd = NULL;
	m_pEditQueueBackGrnd = NULL;

	m_pUpdateDataForeGrnd_List = NULL;
	m_pUpdateDataBackGrnd_List = NULL;

#ifdef _WIN32
	::InitializeCriticalSection( &m_csUpdateDataQueue );
	::InitializeCriticalSection( &m_csEditNodeQueue );
#else
	m_LockResourceID = 0;
#endif
}

ImgMask::~ImgMask()
{
	assert( _CrtCheckMemory() );

	if(m_IsInited){
		End();
	}

#ifdef _WIN32
	::DeleteCriticalSection( &m_csUpdateDataQueue );
	::DeleteCriticalSection( &m_csEditNodeQueue );
#else
	m_LockResourceID = 0;
#endif

	assert( _CrtCheckMemory() );
}

void ImgMask::Init(const LPRECT lprc)
{
	assert(m_IsInited == false);

	if (m_IsInited == false) {
		//create new edit node queue
		ResourceLock( IE_MASK_RESOURCE_ID::EDITNODE_QUEUE );

		m_pEditQueueForeGrnd = new EditQueue;
		m_pEditQueueBackGrnd = new EditQueue;

		ResourceUnlock( IE_MASK_RESOURCE_ID::EDITNODE_QUEUE );

		//create new update data queue
		ResourceLock( IE_MASK_RESOURCE_ID::UPDATE_DATA_QUEUE );
		if(m_pUpdateDataForeGrnd_List){
			delete m_pUpdateDataForeGrnd_List;
			m_pUpdateDataForeGrnd_List = NULL;
		}
		if(m_pUpdateDataBackGrnd_List){
			delete m_pUpdateDataBackGrnd_List;
			m_pUpdateDataBackGrnd_List = NULL;
		}
		m_pUpdateDataForeGrnd_List = new LPUpdateData_List;
		m_pUpdateDataBackGrnd_List = new LPUpdateData_List;
		ResourceUnlock( IE_MASK_RESOURCE_ID::UPDATE_DATA_QUEUE );

		m_MaskRect = (*lprc);

		if(m_Img){
			delete m_Img;
			m_Img = NULL;
		}
		m_Img = new IplImageExt();
		m_Img->Init(
			lprc->right - lprc->left,
			lprc->bottom - lprc->top,
			IPL_DEPTH_8U,
			1);
		m_MaskStateID = IE_MASK_STATE_ID::INVALID;

		m_MaskRect = (*lprc);

		m_IsInited = true;
	}
}

void ImgMask::End()
{
	assert( _CrtCheckMemory() );
	assert(m_IsInited == true);

	if (m_IsInited) {
		//delete edit node queue
		ResourceLock( IE_MASK_RESOURCE_ID::EDITNODE_QUEUE );

		if(m_pEditQueueForeGrnd){
			m_pEditQueueForeGrnd->clear();
			delete m_pEditQueueForeGrnd;
			m_pEditQueueBackGrnd = NULL;
		}
		if(m_pEditQueueBackGrnd){
			m_pEditQueueBackGrnd->clear();
			delete m_pEditQueueBackGrnd;
			m_pEditQueueBackGrnd = NULL;
		}

		ResourceUnlock( IE_MASK_RESOURCE_ID::EDITNODE_QUEUE );

		//delete update data queue
		ResourceLock( IE_MASK_RESOURCE_ID::UPDATE_DATA_QUEUE );
		if(m_pUpdateDataForeGrnd_List){
			delete m_pUpdateDataForeGrnd_List;
			m_pUpdateDataForeGrnd_List = NULL;
		}
		if(m_pUpdateDataBackGrnd_List){
			delete m_pUpdateDataBackGrnd_List;
			m_pUpdateDataBackGrnd_List = NULL;
		}
		ResourceUnlock( IE_MASK_RESOURCE_ID::UPDATE_DATA_QUEUE );

		if(m_Img){
			delete m_Img;
			m_Img = NULL;
		}

		m_IsInited = false;
	}

	assert( _CrtCheckMemory() );
}

bool ImgMask::Update(const LPRECT enable_lprc, LPRECT updated_lprc)
{
	if(m_pUpdateDataForeGrnd_List->empty()){
		return false;
	}

	UPDATE_DATA udMask;
	bool isMaskRect = false;
	udMask.isAll = false;

	//更新情報の縮小化
	ResourceLock( IE_MASK_RESOURCE_ID::UPDATE_DATA_QUEUE );

	LPUpdateData_List::iterator itr = m_pUpdateDataForeGrnd_List->begin();
	for(; itr != m_pUpdateDataForeGrnd_List->end(); itr++){
		bool isAll = (*itr)->isAll;

		switch((*itr)->update_flag){
			case UPDATE_FLAG::UPDATE_MASK_IMAGE:
				
				if(isAll){//全体更新
					udMask.isAll = isAll;
				}
				else if(udMask.isAll == false){//部分更新
					if(isMaskRect == false){//範囲の初期値がないなら
						udMask.rect = (*itr)->rect;
						isMaskRect = true;
					}
					else{//範囲が既に存在しているなら
						OrRect(&((*itr)->rect), &(udMask.rect), &(udMask.rect));
					}
				}
				break;
			case UPDATE_FLAG::NO_UPDATE:
				break;
			default:
				OutputError::PushLog( LOG_LEVEL::_WARN, "ImgMask::Update() unknown update_flag");
				break;
		}
		//最大の更新だったら抜ける
		if((*itr)->update_flag == UPDATE_FLAG::UPDATE_MASK_IMAGE &&
			isAll)
		{
			break;
		}
	}
	ResourceUnlock( IE_MASK_RESOURCE_ID::UPDATE_DATA_QUEUE );

	if(udMask.isAll){
		udMask.rect = m_MaskRect;
		isMaskRect = true;
	}
	if(enable_lprc){
		AndRect((&udMask.rect), enable_lprc, &(udMask.rect));
	}

	//update
	if(isMaskRect){
		UpdateEditNode(&udMask.rect);
		UpdateMaskState();
	}
	(*updated_lprc) = udMask.rect;

	return true;
}


void ImgMask::PushUpdateData(LPUPDATE_DATA data)
{
	ResourceLock( IE_MASK_RESOURCE_ID::UPDATE_DATA_QUEUE );
	m_pUpdateDataBackGrnd_List->push_back( data );
	ResourceUnlock( IE_MASK_RESOURCE_ID::UPDATE_DATA_QUEUE );
}

LPUPDATE_DATA ImgMask::CreateUpdateData()
{
	LPUPDATE_DATA pData = new UPDATE_DATA;
	pData->update_flag = UPDATE_FLAG::UPDATE_MASK_IMAGE;
	return pData;
}

void ImgMask::ClearUpdateData()
{
	ResourceLock( IE_MASK_RESOURCE_ID::EDITNODE_QUEUE );
	m_pEditQueueForeGrnd->clear();
	ResourceUnlock( IE_MASK_RESOURCE_ID::EDITNODE_QUEUE );

	//
	ResourceLock( IE_MASK_RESOURCE_ID::UPDATE_DATA_QUEUE );

	LPUpdateData_List::iterator itr = m_pUpdateDataForeGrnd_List->begin();
	for(; itr != m_pUpdateDataForeGrnd_List->end(); itr++){
		if(*itr){
			delete (*itr);
		}
	}
	m_pUpdateDataForeGrnd_List->clear();

	ResourceUnlock( IE_MASK_RESOURCE_ID::UPDATE_DATA_QUEUE );
}

void ImgMask::LockUpdateData()
{
	//swap back ground to fore ground
	ResourceLock( IE_MASK_RESOURCE_ID::EDITNODE_QUEUE );

	EditQueue* etmp = m_pEditQueueForeGrnd;
	m_pEditQueueForeGrnd = m_pEditQueueBackGrnd;
	m_pEditQueueBackGrnd = etmp;

	ResourceUnlock( IE_MASK_RESOURCE_ID::EDITNODE_QUEUE );

	//swap back ground to fore ground
	ResourceLock( IE_MASK_RESOURCE_ID::UPDATE_DATA_QUEUE );

	LPUpdateData_List* utmp = m_pUpdateDataForeGrnd_List;
	m_pUpdateDataForeGrnd_List = m_pUpdateDataBackGrnd_List;
	m_pUpdateDataBackGrnd_List = utmp;

	ResourceUnlock( IE_MASK_RESOURCE_ID::UPDATE_DATA_QUEUE );
}

void ImgMask::UnlockUpdateData()
{
}

EditNode* ImgMask::CreateEditNode(const LPRECT lprc)
{
	CvSize size = m_Img->GetSize();
	EditNode* ret = new EditNode;

	ret->edit_img.InitToEqualSize(m_Img, IPL_DEPTH_8U, 1);
	ret->undo_img.InitToEqualSize(m_Img, IPL_DEPTH_8U, 1);
	ret->write_map.InitToEqualSize(m_Img, IPL_DEPTH_8U, 1);

	ret->node_rect = m_MaskRect;
	ret->blt_rect = *lprc;
	AndRect(&m_MaskRect, lprc, &(ret->blt_rect));

	ret->fourcc = MaskSynthesizerMG::GetDefaultMaskSynthFourCC();	
	ret->isBakedWriteMap = false;

	return ret;
}

void ImgMask::DestroyEditNode(EditNode* pEditNode)
{
	if (pEditNode) {
		delete pEditNode;
	}
}

void ImgMask::PushAddEditNodeQueue(EditNode* pNode)
{
	ResourceLock( IE_MASK_RESOURCE_ID::EDITNODE_QUEUE );

	if (!m_pEditQueueBackGrnd->empty()) {
		EditQueue::iterator itr = m_pEditQueueBackGrnd->begin();
		for (; itr != m_pEditQueueBackGrnd->end(); itr++) {
			if (((*itr).pNode == pNode) && ((*itr).flag == EDIT_FLAG::ADD_EDIT)) {
				ResourceUnlock( IE_MASK_RESOURCE_ID::EDITNODE_QUEUE );
				return;
			}
		}
	}
	EditQueueNode qnode;
	qnode.pNode = pNode;
	qnode.flag = EDIT_FLAG::ADD_EDIT;
	m_pEditQueueBackGrnd->push_back(qnode);

	ResourceUnlock( IE_MASK_RESOURCE_ID::EDITNODE_QUEUE );
}

void ImgMask::PushSubEditNodeQueue(EditNode* pNode)
{
	ResourceLock( IE_MASK_RESOURCE_ID::EDITNODE_QUEUE );

	if (!m_pEditQueueBackGrnd->empty()) {
		EditQueue::iterator itr = m_pEditQueueBackGrnd->begin();
		for (; itr != m_pEditQueueBackGrnd->end(); itr++) {
			if (((*itr).pNode == pNode) && ((*itr).flag == EDIT_FLAG::SUB_EDIT)) {
				ResourceUnlock( IE_MASK_RESOURCE_ID::EDITNODE_QUEUE );
				return;
			}
		}
	}
	EditQueueNode qnode;
	qnode.pNode = pNode;
	qnode.flag = EDIT_FLAG::SUB_EDIT;
	m_pEditQueueBackGrnd->push_back(qnode);

	ResourceUnlock( IE_MASK_RESOURCE_ID::EDITNODE_QUEUE );
}

void ImgMask::SetName(const char* mask_name)
{
	m_name = mask_name;
}

int ImgMask::GetNameSize() const
{
	return m_name.size();
}

void ImgMask::GetName(char* mask_name) const
{
	strcpy(mask_name, m_name.c_str());
}

void ImgMask::UpdateEditNode(const LPRECT lprc)
{
	ResourceLock( IE_MASK_RESOURCE_ID::EDITNODE_QUEUE );

	if(!m_pEditQueueForeGrnd->empty()){
		EditQueue::iterator itr = m_pEditQueueForeGrnd->begin();
		for(; itr != m_pEditQueueForeGrnd->end(); itr++){
			if ((*itr).flag == EDIT_FLAG::ADD_EDIT) {
				if ((*itr).pNode->node_flag == NODE_FLAG::EDIT_DRAW) {
					AddEditNode(dynamic_cast<EditNode*>((*itr).pNode), lprc);
				}
			}
			else if ((*itr).flag == EDIT_FLAG::SUB_EDIT) {
				if ((*itr).pNode->node_flag == NODE_FLAG::EDIT_DRAW) {
					SubEditNode(dynamic_cast<EditNode*>((*itr).pNode), lprc);
				}
			}
		}
	}

	ResourceUnlock( IE_MASK_RESOURCE_ID::EDITNODE_QUEUE );
}

void ImgMask::UpdateMaskState()
{
	if( m_Img->IsFillZeroMask() )
		m_MaskStateID = IE_MASK_STATE_ID::INVALID;
	else
		m_MaskStateID = IE_MASK_STATE_ID::VALID;
}

///////////////////////////////////
/*!
	pNodeを加算する。
	@param[in] pNode
	@param[in] lprc 更新範囲
*/
void ImgMask::AddEditNode(EditNode *pNode, const LPRECT lprc)
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

	if (!pNode->isBakedWriteMap) {
		//blt undo image
		BltMaskUndoOp bltMaskUndoOp;
		AllMaskOp allMaskOp;

		//blt undo image
		pNode->undo_img.OperateBlt<BltMaskUndoOp, AllMaskOp>(
			bltMaskUndoOp,
			allMaskOp,
			rc.left - pNode->node_rect.left,
			rc.top - pNode->node_rect.top,
			rc.right - rc.left,
			rc.bottom - rc.top,
			m_Img,
			rc.left - m_MaskRect.left,
			rc.top - m_MaskRect.top,
			&(pNode->write_map),
			rc.left - pNode->node_rect.left,
			rc.top - pNode->node_rect.top);
		//write pixel mark
		pNode->write_map.RectFillMask(
			rc.left - pNode->node_rect.left,
			rc.top - pNode->node_rect.top,
			rc.right - rc.left,
			rc.bottom - rc.top,
			255,
			&(pNode->edit_img),
			rc.left - pNode->node_rect.left,
			rc.top - pNode->node_rect.top);
	}

	MaskSynthesizerMG::BlendSynth(
		pNode->fourcc,
		1.0,
		m_Img,
		rc.left - m_MaskRect.left,
		rc.top - m_MaskRect.top,
		rc.right - rc.left,
		rc.bottom - rc.top,
		&(pNode->undo_img),
		rc.left - pNode->node_rect.left,
		rc.top - pNode->node_rect.top,
		&(pNode->edit_img),
		rc.left - pNode->node_rect.left,
		rc.top - pNode->node_rect.top);
}

///////////////////////////////////
/*!
	pNodeを減算する。
	@param[in] pNode
	@param[in] lprc 更新範囲
*/
void ImgMask::SubEditNode(EditNode *pNode, const LPRECT lprc)
{
	if(!pNode) return;

	RECT rc; 
	if(lprc){
		//更新範囲とコピー先範囲の論理積
		AndRect(lprc, &(pNode->blt_rect), &rc);
	}
	else{
		rc = pNode->blt_rect;
	}

	assert(pNode->isBakedWriteMap == true);

	m_Img->ImgBlt(
		rc.left - m_MaskRect.left,
		rc.top - m_MaskRect.top,
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