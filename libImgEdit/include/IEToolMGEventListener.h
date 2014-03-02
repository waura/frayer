#ifndef _IE_TOOL_MG_EVENT_LISTENER_H_
#define _IE_TOOL_MG_EVENT_LISTENER_H_

class IEToolMGEventListener
{
public:
	IEToolMGEventListener():
		m_isLockEvent(false),
		m_isCalledIEToolMG(false){}
	virtual ~IEToolMGEventListener(){}

	//!	イベント呼び出しを行わないようにする
	void LockIEToolMGEvent(){ m_isLockEvent = true; };

	//! イベント呼び出しを行うようにする
	void UnlockIEToolMGEvent(){ m_isLockEvent = false; };

	//! イベント呼び出しを行うかどうか
	bool IsLockIEToolMGEvent(){ return m_isLockEvent; };

	////////////////////////////////
	/*!
		選択ツールが変わったときに呼び出す。
		@param[in] pNewTool 変更後のツール
		@param[in] pOldTool 変更前のツール
	*/
	virtual void OnChangeSelectTool(
		IIETool* pNewTool, IIETool* pOldTool){};

	bool IsCalledIEToolMG(){ return m_isCalledIEToolMG; };
	void SetCalledIEToolMG(bool bl){ m_isCalledIEToolMG = bl; };

private:
	bool m_isLockEvent;
	bool m_isCalledIEToolMG;
};

#endif //_IE_BURHS_MG_EVENT_LISTENER_H_