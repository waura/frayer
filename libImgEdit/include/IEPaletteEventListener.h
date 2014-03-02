#ifndef _IE_PALETTE_EVENT_LISTENER_H_
#define _IE_PALETTE_EVENT_LISTENER_H_

class IEPaletteEventListener
{
public:
	IEPaletteEventListener():
		m_isLockEvent(false),
		m_isCalledIEPalette(false) {};
	virtual ~IEPaletteEventListener(){};

	//!	イベント呼び出しを行わないようにする
	void LockIEPaletteEvent(){ m_isLockEvent = true; };

	//! イベント呼び出しを行うようにする
	void UnlockIEPaletteEvent(){ m_isLockEvent = false; };

	//! イベント呼び出しを行うかどうか
	bool IsLockIEPaletteEvent(){ return m_isLockEvent; };

	////////////////////////////////////
	/*!
		選択している色情報が変わったときに呼び出し
		@param[in] new_select_node_index
	*/
	virtual void OnChangeSelectColor(int new_select_node_index){};

	////////////////////////////////////
	/*!
		色情報が追加されたときに呼び出し
		@param[in] pNode
	*/
	virtual void OnAddColor(IEPaletteNode_Ptr pNode){};

	////////////////////////////////////
	/*!
		色情報が削除されたときに呼び出し
		@parma[in] index
		@param[in] pNode
	*/
	virtual void OnDeleteColor(int index, IEPaletteNode_Ptr pNode){};

	bool IsCalledIEPalette(){ return m_isCalledIEPalette; };
	void SetCalledIEPalette(bool bl){ m_isCalledIEPalette = bl; };
private:
	bool m_isLockEvent;
	bool m_isCalledIEPalette;
};

#endif //_IE_PALETTE_EVENT_LISTENER_H_