#ifndef _IE_BRUSH_MG_EVENT_LISTENER_H_
#define _IE_BRUSH_MG_EVENT_LISTENER_H_

class IEBrushMGEventListener
{
public:
	IEBrushMGEventListener():
		m_isLockEvent(false),
		m_isCalledIEBrushMG(false){}
	virtual ~IEBrushMGEventListener(){}

	//!	イベント呼び出しを行わないようにする
	void LockIEBrushMGEvent(){ m_isLockEvent = true; };

	//! イベント呼び出しを行うようにする
	void UnlockIEBrushMGEvent(){ m_isLockEvent = false; };

	//! イベント呼び出しを行うかどうか
	bool IsLockIEBrushMGEvent(){ return m_isLockEvent; };

	////////////////////////////////
	/*!
		選択ブラシグループが変わったときに呼び出す。
		@param[in] pNewBrushGruop 変更後のブラシグループ
		@param[in] pOldBrushGroup 変更前のブラシグループ
	*/
	virtual void OnChangeSelectBrushGroup(
		IEBrushGroup_Ptr pNewBrushGroup, IEBrushGroup_Ptr pOldBrushGroup){};

	/////////////////////////////////
	/*!
		ブラシグループが追加されたときに呼び出し
		@param[in] pBGroup to add brush group
	*/
	virtual void OnAddBrushGroup(IEBrushGroup_Ptr pBGroup){};

	//////////////////////////////////
	/*!
		ブラシグループが削除されたときに呼び出し
		@param[in] pBGroup to delete brush group
	*/
	virtual void OnDeleteBrushGroup(IEBrushGroup_Ptr pBGroup){};

	bool IsCalledIEBrushMG(){ return m_isCalledIEBrushMG; };
	void SetCalledIEBrushMG(bool bl){ m_isCalledIEBrushMG = bl; };

private:
	bool m_isLockEvent;
	bool m_isCalledIEBrushMG;
};

#endif //_IE_BURHS_MG_EVENT_LISTENER_H_