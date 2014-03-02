#ifndef _IMGLAYERGROUP_H_
#define _IMGLAYERGROUP_H_

#include "IImgLayer.h"

typedef std::vector<IImgLayer_Ptr> IImgLayer_Vec;

_EXPORTCPP ImgLayerGroup_Ptr CreateIEImgLayerGroup(ImgFile_Ptr parent_file);
//_EXPORT void ReleaseIEImgLayerGroup(ImgLayerGroup_Ptr* ppLayerGroup);

//
class _EXPORTCPP ImgLayerGroup :
	public IImgLayer,
	public std::enable_shared_from_this<ImgLayerGroup>
{
public:
	ImgLayerGroup(ImgFile_Ptr parent_file)
		:IImgLayer(parent_file)
	{
		m_NumOfChildLayers = 0;
		m_MaxLayerIndex = 0;
		m_IsNeedCountChildLayers = false;
		m_IsNeedCountMaxLayerIndex = false;
		m_IsOpen = true;
	}
	~ImgLayerGroup();

	inline int GetLayerType(){
		return IE_LAYER_TYPE::GROUP_LAYER;
	}

	bool Update(const LPRECT enable_lprc, LPRECT updated_lprc);

	void ClearUpdateData();
	void LockUpdateData();
	void UnlockUpdateData();

	void PushUpdateDataToAllLayer(const LPUPDATE_DATA data);

	///////////////////////////////////////
	/*!
		add layer
		@param[in] add_layer 追加するレイヤーのポインタ
		@param[in] index 追加先インデックス
	*/
	void AddLayer(IImgLayer_Ptr add_layer); //最後尾に追加
	void AddLayer(IImgLayer_Ptr add_layer, int index);

	///////////////////////////////////////
	/*!
		remove layer
		@param[in,out] delete_layer 取り除くレイヤーのポインタ
		@preturn if find remove layer return true,
	*/
	bool RemoveLayer(IImgLayer_Ptr remove_layer);

	///////////////////////////////////////
	/*!
		レイヤーの並びを変える
	*/
	void ChangeLayerLine(int from_index, int to_index);
	void ChangeLayerLine(IImgLayer_Ptr from_layer, int to_index);

	///////////////////////////////////////
	/*!
		ファイルが保持しているレイヤーの数を返す
		@return レイヤー枚数
	*/
	inline int GetNumOfChildLayers() const {
		return m_NumOfChildLayers;
	}

	inline int GetMaxLayerIndex() const {
		return m_MaxLayerIndex;
	}

	int CountNumOfChildLayers();
	int CountMaxLayerIndex();

	///////////////////////////////////////
	/*!
		レイヤーのインデックスを得る
		見つからない場合は-1を返す
		@param[in] find_layer
	*/
	int GetLayerIndex(const IImgLayer_weakPtr find_layer) const;

	///////////////////////////////////////
	/*!
		インデックスを指定してレイヤーオブジェクトのポインタを返す
		@return レイヤーオブジェクトのポインタ
	*/
	IImgLayer_Ptr const GetLayerAtIndex(int index) const;

	//////////////////////////////////
	/*!
		return layer group state
		@return display state
	*/
	inline bool IsOpen() const {
		return m_IsOpen;
	}

	/////////////////////////////////
	/*!
		set display state
		@param[in] bl display state
	*/
	inline void SetOpen(bool bl){
		m_IsOpen = bl;
	}

#ifdef _DEBUG
	//////////////////////////////////
	/*!
		dump child layer data
	*/
	void DumpChildLayers(int tab_num=0);
#endif //_DEBUG

private:
	bool m_IsNeedCountChildLayers;
	bool m_IsNeedCountMaxLayerIndex;
	int m_NumOfChildLayers;	//
	int m_MaxLayerIndex; //
	bool m_IsOpen;			//下の階層のレイヤーを見せるか?
};

#endif //_IMGLAYERGROUP_H_