#ifndef _IMGLAYEREVENTLISTENER_H_
#define _IMGLAYEREVENTLISTENER_H_

#include "ImgUpdateData.h"

class ImgLayerEventListener {
public:
	ImgLayerEventListener():
	  m_isLockEvent(false),
	  m_isCalledImgLayer(false) {};
	virtual ~ImgLayerEventListener() {};

	//!	イベント呼び出しを行わないようにする
	void LockLayerEvent(){ m_isLockEvent = true; };

	//! イベント呼び出しを行うようにする
	void UnlockLayerEvent(){ m_isLockEvent = false; };

	//! イベント呼び出しを行うかどうか
	bool IsLockLayerEvent(){ return m_isLockEvent; };

	bool IsCalledImgLayer(){ return m_isCalledImgLayer; };
	void SetCalledImgLayer(bool bl){ m_isCalledImgLayer = bl; };

	//////////////////////////////
	/*!
		レイヤーが更新されたときに呼び出す。
	*/
	virtual void OnUpdateLayer(){};

	///////////////////////////////
	/*!
	*/
	virtual void OnImgLayerPushUpdateData(const LPUPDATE_DATA data){};

	virtual void OnChangedLayerName(const char* new_name){}
	virtual void OnChangedLayerOpacity(uint8_t new_opacity){}
	virtual void OnChangedLayerSynthFourCC(uint32_t new_fourcc){}
private:
	bool m_isLockEvent;
	bool m_isCalledImgLayer;
};

#endif //_IMGLAYEREVENTLISTENER_H_