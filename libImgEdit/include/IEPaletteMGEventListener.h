#ifndef _IE_PALETTE_MG_EVENT_LISTENER_H_
#define _IE_PALETTE_MG_EVENT_LISTENER_H_

class IEPaletteMGEventListener
{
public:
	IEPaletteMGEventListener():
		m_isLockEvent(false),
		m_isCalledIEPaletteMG(false) {};
	virtual ~IEPaletteMGEventListener(){};

	//!	イベント呼び出しを行わないようにする
	void LockIEPaletteMGEvent(){ m_isLockEvent = true; };

	//! イベント呼び出しを行うようにする
	void UnlockIEPaletteMGEvent(){ m_isLockEvent = false; };

	//! イベント呼び出しを行うかどうか
	bool IsLockIEPaletteMGEvent(){ return m_isLockEvent; };

	////////////////////////////////////
	/*!
		パレットが追加されたときに呼び出し
		@param[in] pPalette
	*/
	virtual void OnAddPalette(IEPalette_Ptr pPalette){};

	////////////////////////////////////
	/*!
		パレットが削除されたときに呼び出し
		@param[in] pPalette
	*/
	virtual void OnDeletePalette(IEPalette_Ptr pPalette){};

	/////////////////////////////////////
	/*!
		選択パレットが変更されたときに呼び出し
		@param[in] pNewPalette 変更後のパレット
		@param[in] pOldPalette 変更前のパレット
	*/
	virtual void OnChangeSelectPalette(IEPalette_Ptr pNewPalette, IEPalette_Ptr pOldPalette){};

	bool IsCalledIEPaletteMG(){ return m_isCalledIEPaletteMG; };
	void SetCalledIEPaletteMG(bool bl){ m_isCalledIEPaletteMG = bl; };
private:
	bool m_isLockEvent;
	bool m_isCalledIEPaletteMG;
};

#endif //_IE_PALETTE_EVENT_LISTENER_H_