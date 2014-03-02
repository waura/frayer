#ifndef _IE_PALETTE_H_
#define _IE_PALETTE_H_

#include "LibIEDllHeader.h"

#include "IEColor.h"

#define IE_PALETTE_NON_SELECT (-1)

typedef struct _IEPaletteNode {
	IEColor color;
	char name[ MAX_IE_PALETTE_NODE_NAME ];
} IEPaletteNode;

typedef std::shared_ptr<IEPaletteNode> IEPaletteNode_Ptr;
typedef std::shared_ptr<const IEPaletteNode> const_IEPaletteNode_Ptr;

#include "IEPaletteEventListener.h"

typedef std::vector<IEPaletteNode_Ptr> IEPaletteNode_Vec;
typedef std::list<IEPaletteEventListener* > IEPaletteEventListener_List;

enum IE_PALETTE_RESOURCE_ID {
	IE_PALETTE_NODE=0x01,
};

class _EXPORTCPP IEPalette
{
public:
	IEPalette();
	~IEPalette();

	/////////////////////////////////////
	/*!
		XML形式でパレットデータ書き込み
	*/
	void WritePaletteToXml(FILE* fp, int indt);

	/////////////////////////////////
	/*!
		イベントリスナーを登録する。
		@param[in] pListener 登録するリスナー
	*/
	void AddEventListener(IEPaletteEventListener*  pListener);

	//////////////////////////////////
	/*!
		イベントリスナーを削除する
		@param[in] pListener 削除するリスナー
	*/
	void DeleteEventListener(IEPaletteEventListener*  pListener);

	///////////////////////////////////////
	///*!
	//	パレットデータを読み込み
	//*/
	//bool LoadPalette(const char* path);

	int GetPaletteSize() const {
		return m_IEPaletteNode_Vec.size();
	}

	const_IEPaletteNode_Ptr GetColorAtIndex(int index) const {
		assert(0 <= index && index < m_IEPaletteNode_Vec.size());
		return m_IEPaletteNode_Vec[index];
	}

	//////////////////////////////////////
	/*!
		インデックスを指定して選択ノードを設定
	*/
	void SetSelectColor(int index);
	int GetSelectColorIndex(){
		return m_SelectPaletteNodeIndex;
	}

	//////////////////////////////////////
	/*!
		パレットに色情報を追加
		@param[in] name 色の名前
		@param[in] color 色データ
	*/
	void AddColor(const char* name, IEColor color);

	//////////////////////////////////////
	/*!
		インデックスを指定して色情報を削除
	*/
	void DeleteColor(int index);

	/////////////////////////////////////
	/*!
		palette name
	*/
	inline const char* GetName() const {
		return m_name;
	}
	inline void GetName(char* dst) const {
		strcpy_s(dst, MAX_IE_PALETTE_NAME, m_name);
	}
	inline void SetName(const char* src){
		strcpy_s(m_name, MAX_IE_PALETTE_NAME, src);
	}



private:
	///////////////////////////////////
	inline void ResourceLock(unsigned int id){
#ifdef _WIN32
		switch( id ){
			case IE_PALETTE_RESOURCE_ID::IE_PALETTE_NODE:
				::EnterCriticalSection( &m_csPaletteNode );
				break;
			default:
				assert( 0 );
				break;
		}
#else
		while((m_LockResourceID & id) == id);
		m_LockResourceID |= id;
#endif //_WIN32
	}
	inline void ResourceUnlock(unsigned int id){
#ifdef _WIN32
		switch( id ){
			case IE_PALETTE_RESOURCE_ID::IE_PALETTE_NODE:
				::EnterCriticalSection( &m_csPaletteNode );
				break;
			default:
				assert( 0 );
				break;
		}
#else
		m_LockResourceID &= (~id);
#endif //_WIN32
	}
	
	//void SavePalette();

	IEPaletteEventListener_List m_IEPaletteEventListener_List;

	char m_name[MAX_IE_PALETTE_NAME];

	int m_SelectPaletteNodeIndex;
	IEPaletteNode_Ptr m_pSelectPaletteNode;

	IEPaletteNode_Vec m_IEPaletteNode_Vec;

#ifdef _WIN32
	CRITICAL_SECTION m_csPaletteNode;
#else
	unsigned int m_LockResourceID;
#endif
};

_EXPORTCPP IEPalette_Ptr CreateIEPalette();

#endif //_IE_PALETTE_H_