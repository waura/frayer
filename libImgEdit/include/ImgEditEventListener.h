#ifndef _IMGEDITEVENTLISTENER_H_
#define _IMGEDITEVENTLISTENER_H_

#include "IEColor.h"
#include "ImgFile.h"
#include "IEBrush.h"


/*!
	ImgEditのイベント時に呼び出されるクラスインターフェイス
	継承して使用する。
*/
class ImgEditEventListener {
public:
	ImgEditEventListener():
	  m_isLockEvent(false),
	  m_isCalledImgEdit(false) {};
	virtual ~ImgEditEventListener(){};

	//!	イベント呼び出しを行わないようにする
	void LockImgEditEvent(){ m_isLockEvent = true; };

	//! イベント呼び出しを行うようにする
	void UnlockImgEditEvent(){ m_isLockEvent = false; };

	//! イベント呼び出しを行うかどうか
	bool IsLockImgEditEvent(){ return m_isLockEvent; };

	//////////////////////////////
	/*!
		選択色が変わったときに呼び出す。
		@param[in] color 変更後の色
	*/
	virtual void OnChangeSelectColor(IEColor color){};

	///////////////////////////////
	/*!
		編集中のファイルが切り替わったときに呼び出す。
		@param[in] pNewFile 切り替え後のファイル
		@param[in] pOldFile 切り替え後のファイル
	*/
	virtual void OnChangeActiveFile(ImgFile_Ptr pNewFile, ImgFile_Ptr pOldFile){};

	////////////////////////////////
	/*!
		編集する画像ファイルを追加する
		@param[in] pFile 追加されるファイル
	*/
	virtual void OnAddImgFile(ImgFile_Ptr pFile){};

	//////////////////////////////////
	/*!
		ツールが変更されたときに呼び出し
		@param[in] pNewTool 変更後のツール 
		@param[in] pOldTool 変更前のツール
	*/
	virtual void OnChangeSelectTool(IIETool* pNewTool, IIETool* pOldTool){};

	bool IsCalledImgEdit(){ return m_isCalledImgEdit; };
	void SetCalledImgEdit(bool bl){ m_isCalledImgEdit = bl; };
private:
	bool m_isLockEvent;
	bool m_isCalledImgEdit;
};

#endif //_IMGEDITEVENTLISTENER_H_