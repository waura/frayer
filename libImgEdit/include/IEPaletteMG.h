#ifndef _IE_PALETTE_MG_H_
#define _IE_PALETTE_MG_H_

#include "IEPalette.h"
#include "IEPaletteMGEventListener.h"

typedef std::vector<IEPalette_Ptr> IEPalette_Vec;
typedef std::list<IEPaletteMGEventListener* > IEPaletteMGEventListener_List;

enum IE_PALETTE_MG_RESOURCE_ID {
	IE_PALETTE=0x01,
};


class _EXPORTCPP IEPaletteMG
{
public:
	IEPaletteMG();
	~IEPaletteMG();

	void Init(const char* path);
	void End();

	/////////////////////////////////
	/*!
		イベントリスナーを登録する。
		@param[in] pListener 登録するリスナー
	*/
	void AddEventListener(IEPaletteMGEventListener*  pListener);

	//////////////////////////////////
	/*!
		イベントリスナーを削除する
		@param[in] pListener 削除するリスナー
	*/
	void DeleteEventListener(IEPaletteMGEventListener*  pListener);

	void AddPalette(IEPalette_Ptr pPalette);
	void DeletePalette(IEPalette_Ptr pPalette);

	void SetSelectPalette(IEPalette_Ptr pPalette);

	///////////////////////////////
	/*!
		indexを指定してパレットを指定する
		@param[in] index
	*/
	void SetSelectPalette(int index);

	////////////////////////////////
	/*!
		選択されているパレットが保持している色数を返す
		@return 保持している色数
	*/
	int GetSelectPaletteSize() const {
		if( m_pSelectPalette ){
			return m_pSelectPalette->GetPaletteSize();
		}
		return -1;
	}

	//////////////////////////////
	/*!
		色情報を追加する。
		@param[in] pNode 追加する色情報
	*/
	void AddColor(const char* name, IEColor color){
		if( m_pSelectPalette ){
			m_pSelectPalette->AddColor(name, color);
		}
	}

	//////////////////////////////
	/*!
		色情報を削除する。
		@param[in] index　削除する色のインデックス
	*/
	void DeleteColor(int index){
		if( m_pSelectPalette ){
			m_pSelectPalette->DeleteColor( index );
		}
	}

	///////////////////////////////
	/*!
		すべてのパレット名をコンボボックスに追加する
		@param[in] hCombo
	*/
	void SetPaletteComboBox(HWND hCombo);

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
	IEPaletteMGEventListener_List m_IEPaletteMGEventListener_List;

	IEPalette_Vec m_IEPalette_Vec;
	IEPalette_Ptr m_pSelectPalette;

	std::string m_PaletteXmlFilePath;

	bool m_isInit;
	unsigned int m_LockResourceID;
};

#endif //_IE_PALETTE_MG_H_