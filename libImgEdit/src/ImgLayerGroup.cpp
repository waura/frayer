#include "stdafx.h"

#include "ImgLayerGroup.h"

#include "ImgLayer.h"
#include "ImgFile.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


_EXPORTCPP ImgLayerGroup_Ptr CreateIEImgLayerGroup(ImgFile_Ptr parent_file)
{
	return ImgLayerGroup_Ptr(new ImgLayerGroup(parent_file));
}

//_EXPORT void ReleaseIEImgLayerGroup(ImgLayerGroup_Ptr* ppLayerGroup)
//{
//	if (*ppLayerGroup) {
//		delete (*ppLayerGroup);
//		(*ppLayerGroup) = NULL;
//	}
//}


ImgLayerGroup::~ImgLayerGroup()
{
	assert( _CrtCheckMemory() );

	//IImgLayer_Ptr tmp;
	//IImgLayer_Ptr pLayer = this->GetBottomChildLayer();
	//while(pLayer != NULL){
	//	tmp = pLayer->GetOverLayer();
	//	ReleaseIEIImgLayer(&pLayer);
	//	pLayer = tmp;
	//}

	assert( _CrtCheckMemory() );
}

bool ImgLayerGroup::Update(const LPRECT enable_lprc, LPRECT updated_lprc)
{
	RECT updated_rc;
	bool isLayerRect = false;

	//
	IImgLayer_Ptr pLayer = this->GetBottomChildLayer();
	while (pLayer != NULL) {
		if (pLayer->Update(enable_lprc, &updated_rc)) {
			if (isLayerRect == false) {
				(*updated_lprc) = updated_rc;
				isLayerRect = true;
			} else {
				OrRect(updated_lprc, &updated_rc, updated_lprc);
			}
		}
		pLayer = pLayer->GetOverLayer().lock();
	}

	UPDATE_DATA udLayerGroup;
	bool isLayerGroupRect = false;
	udLayerGroup.isAll = false;

	//更新情報の縮小化
	ResourceLock( IE_LAYER_RESOURCE_ID::UPDATE_DATA_QUEUE );

	LPUpdateData_List::iterator itr = m_pUpdateDataForeGrnd_List->begin();
	for (; itr != m_pUpdateDataForeGrnd_List->end(); itr++) {
		bool isAll = (*itr)->isAll;
		
		switch ((*itr)->update_flag) {
			case UPDATE_FLAG::UPDATE_LAYER_IMAGE:
				
				if (isAll) {
					//全体更新
					udLayerGroup.isAll = isAll;
				} else if (udLayerGroup.isAll == false) {
					//部分更新
					if (isLayerGroupRect == false) {
						//範囲の初期値がないなら
						udLayerGroup.rect = (*itr)->rect;
						isLayerGroupRect = true;
					} else {
						//範囲がすでに存在しているなら
						OrRect(&((*itr)->rect), &(udLayerGroup.rect), &(udLayerGroup.rect));
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
		if ((*itr)->update_flag == UPDATE_FLAG::UPDATE_LAYER_IMAGE && isAll) {
			break;
		}
	}

	ResourceUnlock( IE_LAYER_RESOURCE_ID::UPDATE_DATA_QUEUE );

	if (udLayerGroup.isAll) {
		udLayerGroup.rect = m_LayerRect;
		isLayerGroupRect = true;
	}


	if (isLayerGroupRect == false) {
		if (isLayerRect == false) {
			return false;
		}
	} else {
		if (isLayerRect) {
			OrRect(&(udLayerGroup.rect), updated_lprc, updated_lprc);
		} else {
			(*updated_lprc) = udLayerGroup.rect;
		}
	}
	AndRect(&m_LayerRect, updated_lprc, updated_lprc);

	if (enable_lprc) {
		AndRect(enable_lprc, updated_lprc, updated_lprc);
	}

	//print debug
	//char log[256];
	//wsprintf(log, "update_group top=%d bottom=%d left=%d right=%d\n",
	//	updated_lprc->top, updated_lprc->bottom, updated_lprc->left, updated_lprc->right);
	//::OutputDebugString(log);

	//zero clear
	m_Img->RectFillZero(
		updated_lprc->left - m_LayerRect.left,
		updated_lprc->top - m_LayerRect.top,
		updated_lprc->right - updated_lprc->left,
		updated_lprc->bottom - updated_lprc->top);

	//layer synth
	pLayer = this->GetBottomChildLayer();
	while (pLayer != NULL) {
		if (pLayer->IsVisible() != false) {
			pLayer->BltRectSynthPixel(
				m_Img,
				updated_lprc->left - m_LayerRect.left,
				updated_lprc->top - m_LayerRect.top,
				updated_lprc->left,
				updated_lprc->top,
				updated_lprc->right - updated_lprc->left,
				updated_lprc->bottom - updated_lprc->top);
		}
		pLayer = pLayer->GetOverLayer().lock();
	}

	return IImgLayer::Update(enable_lprc, updated_lprc);
}

void ImgLayerGroup::ClearUpdateData()
{
	IImgLayer_Ptr pLayer = this->GetBottomChildLayer();
	while (pLayer != NULL) {
		pLayer->ClearUpdateData();
		pLayer = pLayer->GetOverLayer().lock();
	}

	IImgLayer::ClearUpdateData();
}

void ImgLayerGroup::LockUpdateData()
{
	IImgLayer_Ptr pLayer = this->GetBottomChildLayer();
	while (pLayer != NULL) {
		pLayer->LockUpdateData();
		pLayer = pLayer->GetOverLayer().lock();
	}

	IImgLayer::LockUpdateData();
}

void ImgLayerGroup::UnlockUpdateData()
{
	IImgLayer::UnlockUpdateData();

	IImgLayer_Ptr pLayer = this->GetBottomChildLayer();
	while (pLayer != NULL) {
		pLayer->UnlockUpdateData();
		pLayer = pLayer->GetOverLayer().lock();
	}

}

void ImgLayerGroup::PushUpdateDataToAllLayer(const LPUPDATE_DATA data)
{
	LPUPDATE_DATA lud;
	IImgLayer_Ptr pLayer = this->GetBottomChildLayer();
	while (pLayer != NULL) {
		if (pLayer->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER) {
			ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(pLayer);
			pLGroup->PushUpdateData(data);
		} else {
			lud = pLayer->CreateUpdateData();
			(*lud) = (*data);
			pLayer->PushUpdateData(lud);
		}
		pLayer = pLayer->GetOverLayer().lock();
	}
}

///////////////////////////////////////
/*!
	add layer
	@param[in] add_layer 追加するレイヤーのポインタ
*/
void ImgLayerGroup::AddLayer(IImgLayer_Ptr add_layer)
{
	m_IsNeedCountChildLayers = true;
	m_IsNeedCountMaxLayerIndex = true;

	if (GetBottomChildLayer() == NULL) {
		add_layer->SetOverLayer( NULL );
		add_layer->SetUnderLayer( NULL );
		add_layer->SetParentLayer( this->shared_from_this() );
		SetBottomChildLayer( add_layer );
		return;
	} else {
		IImgLayer_Ptr pLayer = this->GetBottomChildLayer();
		while (pLayer != NULL) {
			if (pLayer->GetOverLayer().lock() == NULL) {
				add_layer->SetOverLayer( NULL );
				add_layer->SetUnderLayer( pLayer );
				add_layer->SetParentLayer( this->shared_from_this() );
				pLayer->SetOverLayer( add_layer );
				return;
			}
			pLayer = pLayer->GetOverLayer().lock();
		}
	}
}

void ImgLayerGroup::AddLayer(IImgLayer_Ptr add_layer, int index)
{
	assert( 0 <= index && index <= (GetMaxLayerIndex()+1) );

	m_IsNeedCountChildLayers = true;
	m_IsNeedCountMaxLayerIndex = true;

	if (GetBottomChildLayer() == NULL) {
		add_layer->SetOverLayer( NULL );
		add_layer->SetUnderLayer( NULL );
		add_layer->SetParentLayer( this->shared_from_this() );
		SetBottomChildLayer( add_layer );
	} else {
		int cnt = 0;
		IImgLayer_Ptr pLayer = this->GetBottomChildLayer();
		while (pLayer != NULL) {
			if (pLayer->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER) {
				if (cnt == index) { //hit virtual layer
					IImgLayer_Ptr under_layer = pLayer->GetUnderLayer().lock();
					pLayer->SetUnderLayer( add_layer );
					add_layer->SetOverLayer( pLayer );
					add_layer->SetUnderLayer( under_layer );
					if (under_layer) {
						under_layer->SetOverLayer( add_layer );
					} else {
						SetBottomChildLayer( add_layer );
					}
					add_layer->SetParentLayer( this->shared_from_this() );
					return;
				}

				cnt++;
				ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(pLayer);
				int lg_size = pLGroup->GetMaxLayerIndex();
				if (index <= (cnt + lg_size + 1)) {
					return pLGroup->AddLayer(add_layer, index - cnt);
				}
				cnt += (lg_size + 1);
			}
			if (cnt == index) {
				IImgLayer_Ptr under_layer = pLayer->GetUnderLayer().lock();
				pLayer->SetUnderLayer( add_layer );
				add_layer->SetOverLayer( pLayer );
				add_layer->SetUnderLayer( under_layer );
				if (under_layer) {
					under_layer->SetOverLayer( add_layer );
				} else {
					SetBottomChildLayer( add_layer );
				}
				add_layer->SetParentLayer( this->shared_from_this() );
				return;
			}

			cnt++;
			if (pLayer->GetOverLayer().lock() == NULL) {
				assert(cnt == index);
				add_layer->SetOverLayer( NULL );
				add_layer->SetUnderLayer( pLayer );
				add_layer->SetParentLayer( this->shared_from_this() );
				pLayer->SetOverLayer( add_layer );
				return;
			}
			pLayer = pLayer->GetOverLayer().lock();
		}
	}
}

///////////////////////////////////////
/*!
	delete layer
	@param[in,out] remove_layer 削除するレイヤーのポインタ
	@preturn if find remove layer return true,
*/
bool ImgLayerGroup::RemoveLayer(IImgLayer_Ptr remove_layer)
{
	m_IsNeedCountChildLayers = true;
	m_IsNeedCountMaxLayerIndex = true;

	IImgLayer_Ptr pLayer = this->GetBottomChildLayer();
	while (pLayer != NULL) {
		if (pLayer == remove_layer) {
			IImgLayer_Ptr under_layer = remove_layer->GetUnderLayer().lock();
			IImgLayer_Ptr over_layer = remove_layer->GetOverLayer().lock();
			IImgLayer_Ptr parent_layer = remove_layer->GetParentLayer().lock();

			if (under_layer) {
				under_layer->SetOverLayer( over_layer );
			}
			if (over_layer) {
				over_layer->SetUnderLayer( under_layer );
			}
			if (parent_layer->GetBottomChildLayer() == remove_layer) {
				parent_layer->SetBottomChildLayer( over_layer );
			}
			return true;
		}

		if (pLayer->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER) {
			ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(pLayer);
			if (pLGroup->RemoveLayer( remove_layer )) {
				return true;
			}
		}
		pLayer = pLayer->GetOverLayer().lock();
	}

	return false;
}

///////////////////////////////////////
/*!
	レイヤーの並びを変える
*/
void ImgLayerGroup::ChangeLayerLine(int from_index, int to_index)
{
	if (from_index == to_index) return;

	IImgLayer_Ptr from_layer = GetLayerAtIndex(from_index);
	ChangeLayerLine(from_layer, to_index);
}

void ImgLayerGroup::ChangeLayerLine(IImgLayer_Ptr from_layer, int to_index)
{
	m_IsNeedCountChildLayers = true;
	m_IsNeedCountMaxLayerIndex = true;

	IImgLayer_Ptr to_layer = GetLayerAtIndex(to_index);

	if (GetBottomChildLayer() == NULL) {
		//remove from_layer
		IImgLayer_Ptr from_under_layer = from_layer->GetUnderLayer().lock();
		IImgLayer_Ptr from_over_layer = from_layer->GetOverLayer().lock();
		IImgLayer_Ptr from_parent_layer = from_layer->GetParentLayer().lock();
		if (from_under_layer) {
			from_under_layer->SetOverLayer( from_over_layer );
		}
		if (from_over_layer) {
			from_over_layer->SetUnderLayer( from_under_layer );
		}
		if (from_parent_layer->GetBottomChildLayer() == from_layer) {
			from_parent_layer->SetBottomChildLayer( from_over_layer );
		}

		//insert from_layer
		from_layer->SetOverLayer( NULL );
		from_layer->SetUnderLayer( NULL );
		from_layer->SetParentLayer( this->shared_from_this() );
		this->SetBottomChildLayer( from_layer );
	} else {
		int cnt = 0;
		IImgLayer_Ptr pLayer = this->GetBottomChildLayer();
		while (pLayer != NULL) {
			if (pLayer->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER) {
				if (cnt == to_index) { //hit virtual layer
					//remove from_layer
					IImgLayer_Ptr from_under_layer = from_layer->GetUnderLayer().lock();
					IImgLayer_Ptr from_over_layer = from_layer->GetOverLayer().lock();
					IImgLayer_Ptr from_parent_layer = from_layer->GetParentLayer().lock();
					if (from_under_layer) {
						from_under_layer->SetOverLayer( from_over_layer );
					}
					if (from_over_layer) {
						from_over_layer->SetUnderLayer( from_under_layer );
					}
					if (from_parent_layer->GetBottomChildLayer() == from_layer) {
						from_parent_layer->SetBottomChildLayer( from_over_layer );
					}

					//insert from_layer
					IImgLayer_Ptr to_under_layer = pLayer->GetUnderLayer().lock();
					from_layer->SetOverLayer( pLayer );
					from_layer->SetUnderLayer( to_under_layer );
					if (to_under_layer) {
						to_under_layer->SetOverLayer( from_layer );
					} else {
						SetBottomChildLayer( from_layer );
					}
					from_layer->SetParentLayer( this->shared_from_this() );
					pLayer->SetUnderLayer( from_layer );
					return;
				}

				cnt++;
				ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(pLayer);
				int lg_size = pLGroup->GetMaxLayerIndex();
				if (to_index <= (cnt + lg_size + 1)) {
					return pLGroup->ChangeLayerLine(from_layer, to_index - cnt);
				}
				cnt += (lg_size + 1);
			}
			if (cnt == to_index) {
				//remove from_layer
				IImgLayer_Ptr from_under_layer = from_layer->GetUnderLayer().lock();
				IImgLayer_Ptr from_over_layer = from_layer->GetOverLayer().lock();
				IImgLayer_Ptr from_parent_layer = from_layer->GetParentLayer().lock();
				if (from_under_layer) {
					from_under_layer->SetOverLayer( from_over_layer );
				}
				if (from_over_layer) {
					from_over_layer->SetUnderLayer( from_under_layer );
				}
				if (from_parent_layer->GetBottomChildLayer() == from_layer) {
					from_parent_layer->SetBottomChildLayer( from_over_layer );
				}

				//insert from_layer
				IImgLayer_Ptr to_under_layer = pLayer->GetUnderLayer().lock();
				from_layer->SetOverLayer( pLayer );
				from_layer->SetUnderLayer( to_under_layer );
				if (to_under_layer) {
					to_under_layer->SetOverLayer( from_layer );
				} else {
					SetBottomChildLayer( from_layer );
				}
				from_layer->SetParentLayer( this->shared_from_this() );
				pLayer->SetUnderLayer( from_layer );
				return;
			}

			cnt++;
			if (pLayer->GetOverLayer().lock() == NULL) {
				assert(cnt == to_index); //if hit this assert not found to_index layer
				//remove from_layer
				IImgLayer_Ptr from_under_layer = from_layer->GetUnderLayer().lock();
				IImgLayer_Ptr from_over_layer = from_layer->GetOverLayer().lock();
				IImgLayer_Ptr from_parent_layer = from_layer->GetParentLayer().lock();
				if (from_under_layer) {
					from_under_layer->SetOverLayer( from_over_layer );
				}
				if (from_over_layer) {
					from_over_layer->SetUnderLayer( from_under_layer );
				}
				if (from_parent_layer->GetBottomChildLayer() == from_layer) {
					from_parent_layer->SetBottomChildLayer( from_over_layer );
				}

				//insert from_layer
				IImgLayer_Ptr to_under_layer = pLayer->GetUnderLayer().lock();
				from_layer->SetOverLayer( NULL );
				from_layer->SetUnderLayer( pLayer );
				from_layer->SetParentLayer( this->shared_from_this() );
				pLayer->SetOverLayer( from_layer );
				return;
			}
			pLayer = pLayer->GetOverLayer().lock();
		}
	}
}

///////////////////////////////////////
/*!
	Count layer group
	@return レイヤー枚数
*/
int ImgLayerGroup::CountNumOfChildLayers()
{
	int cnt_layer=0;
	int cnt_index=0;
	IImgLayer_Ptr pLayer = this->GetBottomChildLayer();
	while (pLayer != NULL) {
		if (pLayer->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER) {
			ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(pLayer);
			cnt_layer += pLGroup->CountNumOfChildLayers();
		}
	
		cnt_layer++;
		pLayer = pLayer->GetOverLayer().lock();
	}
	m_NumOfChildLayers = cnt_layer;
	m_IsNeedCountChildLayers = false;
	return cnt_layer;
}

int ImgLayerGroup::CountMaxLayerIndex()
{
	int cnt_index=-1;
	IImgLayer_Ptr pLayer = this->GetBottomChildLayer();
	while (pLayer != NULL) {
		if (pLayer->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER) {
			cnt_index++; //virtual layer
			ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(pLayer);
			cnt_index += (pLGroup->CountMaxLayerIndex() + 1);
		}
	
		cnt_index++;
		pLayer = pLayer->GetOverLayer().lock();
	}
	m_MaxLayerIndex= cnt_index;
	m_IsNeedCountMaxLayerIndex = false;
	return cnt_index;
}

int ImgLayerGroup::GetLayerIndex(const IImgLayer_weakPtr find_layer) const
{
	int cnt=0;
	IImgLayer_Ptr pLayer = this->GetBottomChildLayer();
	while (pLayer != NULL) {
		if (pLayer->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER) {
			cnt++;
			ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(pLayer);
			int index = pLGroup->GetLayerIndex( find_layer );
			if (index != -1) {
				return index + cnt;
			} else {
				cnt += (pLGroup->GetMaxLayerIndex() + 1);
			}
		}
		if (find_layer.lock() == pLayer) {
			return cnt;
		}
		cnt++;
		pLayer = pLayer->GetOverLayer().lock();
	}
	return -1;
}

///////////////////////////////////////
/*!
	インデックスを指定してレイヤーオブジェクトのポインタを返す
	@return レイヤーオブジェクトのポインタ
*/
IImgLayer_Ptr const ImgLayerGroup::GetLayerAtIndex(int index) const
{
	int cnt = 0;
	IImgLayer_Ptr pLayer = this->GetBottomChildLayer();
	while(pLayer != NULL){
		if(pLayer->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER){	
			if(cnt == index) //hit virtual layer
				return NULL;
			
			cnt++;
			ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(pLayer);
			int lg_size = pLGroup->GetMaxLayerIndex();
			if(index <= (cnt + lg_size)){
				return pLGroup->GetLayerAtIndex(index - cnt);
			}
			cnt += (lg_size + 1);
		}
		if(cnt == index)
			return pLayer;
		cnt++;
		pLayer = pLayer->GetOverLayer().lock();
	}
	return NULL;
}

#ifdef _DEBUG
void ImgLayerGroup::DumpChildLayers(int tab_num)
{
	char log[MAX_IMG_LAYER_NAME+64];
	char layer_name[MAX_IMG_LAYER_NAME];
	IImgLayer_Ptr pLayer = this->GetBottomChildLayer();
	while(pLayer != NULL){
		pLayer->GetName(layer_name);
		wsprintf(log, "[%s]\n", layer_name);
		for(int i=0; i<tab_num; i++){
			::OutputDebugString("  ");
		}
		::OutputDebugString(log);

		if(pLayer->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER){	
			ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(pLayer);
			pLGroup->DumpChildLayers(tab_num+1);
		}
		IImgLayer_Ptr over_layer = pLayer->GetOverLayer().lock();
		if(over_layer){
			assert(over_layer->GetUnderLayer().lock() == pLayer);
		}
		pLayer = over_layer;
	}
}
#endif //_DEBUG