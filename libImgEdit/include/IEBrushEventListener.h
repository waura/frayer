#ifndef _IEBRUSHEVENTLISTENER_H_
#define _IEBRUSHEVENTLISTENER_H_

class IEBrushEventListener {
public:
	IEBrushEventListener():
	  m_isLockBrushEvent(false),
	  m_isCalledIEBrush(false) {};
	virtual ~IEBrushEventListener(){};

	//! イベント呼び出しを行わないようにする
	void LockBrushEvent(){ m_isLockBrushEvent = true; };

	//! イベント呼び出しを行うようにする
	void UnlockBrushEvent(){ m_isLockBrushEvent = false; };

	//! イベント呼び出しを行うかどうか
	bool IsLockBrushEvent(){ return m_isLockBrushEvent; };

	bool IsCalledIEBrush(){ return m_isCalledIEBrush; };
	void SetCalledIEBrush(bool bl){ m_isCalledIEBrush = bl; };

	////////////////////////
	/*!
		ブラシサイズが変わったときに呼び出す
	*/
	virtual void OnChangeRadius(int old_rad, int new_rad){};

	virtual void OnChangeOrgRadius(int old_rad, int new_rad){};

	virtual void OnChangeBrushShape(){};
	virtual void OnChangeBrushStroke(){};
private:
	bool m_isLockBrushEvent;
	bool m_isCalledIEBrush;
};

#endif //_IEBRUSHEVENTLISTENER_H_