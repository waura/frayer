#ifndef _IEBRUSHGROUPEVENTLISTENRE_H_
#define _IEBRUSHGROUPEVENTLISTENRE_H_

class IEBrushGroupEventListener
{
public:
	IEBrushGroupEventListener():
		m_isLockEvent(false),
		m_isCalledIEBrushGroup(false) {};

	virtual ~IEBrushGroupEventListener(){};

	//!	イベント呼び出しを行わないようにする
	void LockIEBrushGroupEvent(){ m_isLockEvent = true; };

	//! イベント呼び出しを行うようにする
	void UnlockIEBrushGroupEvent(){ m_isLockEvent = false; };

	//! イベント呼び出しを行うかどうか
	bool IsLockIEBrushGroupEvent(){ return m_isLockEvent; };

	////////////////////////////////
	/*!
		選択ブラシが変わったときに呼び出す。
		@param[in] pNewBrush 変更後のブラシ
		@param[in] pOldBrush 変更前のブラシ
	*/
	virtual void OnChangeSelectBrush(IEBrush_Ptr pNewBrush, IEBrush_Ptr pOldBrush){};

	/////////////////////////////////
	/*!
		ブラシが追加されたときに呼び出し
		@param[in] pBrush 追加されたブラシ
	*/
	virtual void OnAddBrush(IEBrush_Ptr pBrush){};

	//////////////////////////////////
	/*!
		ブラシが削除されたときに呼び出し
		@param[in] pBrish 削除されるブラシ
	*/
	virtual void OnDeleteBrush(IEBrush_Ptr pBrush){};

	bool IsCalledIEBrushGroup(){ return m_isCalledIEBrushGroup; };
	void SetCalledIEBrushGroup(bool bl){ m_isCalledIEBrushGroup = bl; };
private:
	bool m_isLockEvent;
	bool m_isCalledIEBrushGroup;
};

#endif //_IEBRUSHGROUPEVENTLISTENER_H_