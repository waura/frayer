#ifndef _IETOOLMG_H_
#define _IETOOLMG_H_

#include "IEToolGroup.h"
#include "IToolEditWndHandle.h"
#include "IEToolMGEventListener.h"

class ImgEdit;

typedef std::vector<IEToolGroup*> IEToolGroup_Vec;
typedef std::list<IEToolMGEventListener* > IEToolMGEventListener_List;

typedef IIETool* (*_CreateIETool)();
typedef void (*_ReleaseIETool)(IIETool*);

enum IE_TOOL_MG_RESOURCE_ID {
	IE_SELECT_TOOL=0x01,
};

class _EXPORTCPP IEToolMG
{
public:
	IEToolMG();
	~IEToolMG();

	void SetImgEdit(ImgEdit* pImgEdit){
		m_pImgEdit = pImgEdit;
	}

	void SetStartDir(const char* path);

	//////////////////////////////
	/*!
		初期化処理
	*/
	void Init(const char* path);

	//////////////////////////////
	/*!
		終了処理
	*/
	void End();

	/////////////////////////////////
	/*!
		イベントリスナーを登録する。
		@param[in] pListener 登録するリスナー
	*/
	void AddEventListener(IEToolMGEventListener*  pListener);

	//////////////////////////////////
	/*!
		イベントリスナーを削除する
		@param[in] pListener 削除するリスナー
	*/
	void DeleteEventListener(IEToolMGEventListener*  pListener);

	////////////////////////////////
	/*!
		ツールを指定する
		@param[in] id 指定するツールのポインタ
	*/
	void SetSelectTool(IIETool* pTool);

	////////////////////////////////
	/*!
		選択されているツールを返す
		@return 選択されているツールへのポインタ
	*/
	IIETool* GetSelectTool(){
		return m_pSelectTool;
	}

	void ReturnPrevSelectTool(){
		if(m_pPrevSelectTool){
			SetSelectTool( m_pPrevSelectTool );
		}
	}

	////////////////////////////////
	/*!
		ツールエディットウィンドウハンドルをセット
		ツールエティットウィンドウに追加するツールエディット
		コントロールを送る。
		@param[in,out] pHandle
	*/
	void SetToolEditWndHandle(IToolEditWndHandle* pHandle);

	//////////////////////////////
	/*!
		idに対応するツールオブジェクトのポインタを返す。
		@param[in] id
		@return ツールオブジェクト
	*/
	//IIETool* GetIEToolFromIndex(int group_index, int tnode_index);

	//////////////////////////////
	/*!
		ツール名に対応するツールオブジェクトのポインタを返す。
		@param[in] tool_name ツール名
		@return ツールオブジェクト
	*/
	IIETool* GetIEToolFromName(const char* tool_name);

	////////////////////////////////
	///*!
	//	ツール表示名に対応するツールオブジェクトのポインタを返す。
	//	@param[in] disp_name ツール表示名
	//	@return ツールオブジェクト
	//*/
	//IIETool* GetIEToolFromDisplayName(const char* disp_name);

	void AddToolGroup(IEToolGroup* pTGroup);

	///////////////////////////////
	/*!
		総ツールグループ数を返す。
		@return 
	*/
	int GetSizeOfToolGroup(){
		return m_IEToolGroup_Vec.size();
	}

	////////////////////////////////
	/*!
		インデックスを指定してツールグループを返す
	*/
	IEToolGroup* GetToolGroupAtIndex(int index){
		return m_IEToolGroup_Vec[index];
	}

	///////////////////////////////
	/*!
		指定したグループに含まれるツールの数を返す。
		@param[in] group_index グループのインデックス
		@return 
	*/
	int GetSizeOfToolNode(int group_index);

	/////////////////////////////////
	///*!
	//	グループインデックス,ツールノードインデックスを指定し
	//	そのツールのIDを返す。
	//	@param[in] group_index グループのインデックス
	//	@param[in] tnode_index ツールのインデックス
	//	@return
	//*/
	//int GetToolID(int group_index, int tnode_index);

	///////////////////////////////
	/*!
		グループインデックス,ツールノードインデックスを指定し
		そのツールの表示名をdstに書き込む
		@param[in] group_index グループのインデックス
		@param[in] tnode_index ツールのインデックス
		@param[in] dst 出力先アドレス
		@return
	*/
	bool GetToolName(int group_index, int tnode_index, char* dst);

	///////////////////////////////
	/*!
		ツール名を取得する
		@param[in] group_index グループのインデックス
		@param[in] tnode_index ツールのインデックス
		@param[in] dst 出力先アドレス
		@return
	*/
	bool GetToolName(const char* display_name, char* dst);

	///////////////////////////////
	/*!
		グループインデックス,ツールノードインデックスを指定し
		そのツールの表示名をdstに書き込む
		@param[in] group_index グループのインデックス
		@param[in] tnode_index ツールのインデックス
		@param[in] dst 出力先アドレス
		@return
	*/
	bool GetDisplayName(int group_index, int tnode_index, char* dst);

	/////////////////////////////////
	/*!
		ツールの表示名を取得する
		@param[in] tool_name 取得するツールの名前を指定する
		@param[in] dst 出力先アドレス
	*/
	bool GetDisplayName(const char* tool_name, char* dst);

	///////////////////////////////////
	inline void ResourceLock(unsigned int id){
		while(IsResourceLock( id ));
		m_LockResourceID |= id;
	}
	inline void ResourceUnlock(unsigned int id){
		m_LockResourceID &= (~id);
	}
	inline bool IsResourceLock(unsigned int id) const {
		return ((m_LockResourceID & id) == id);
	}
private:

	void MakeDefaultToolXmlFile(const char* tool_xml_path);

	IEToolMGEventListener_List m_IEToolMGEventListener_List;

	std::string m_IEStartDir;
	std::string m_ToolXmlFilePath;

	ImgEdit* m_pImgEdit;
	IIETool* m_pSelectTool;
	IIETool* m_pPrevSelectTool;

	IToolEditWndHandle* m_pIToolEditWndHandle;

	IEToolGroup_Vec m_IEToolGroup_Vec;

	bool m_isInit;
	unsigned int m_LockResourceID;
};

#endif //_IETOOLMG_H_