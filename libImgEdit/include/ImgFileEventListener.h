#ifndef _IMGFILEEVENTLISTENER_H_
#define _IMGFILEEVENTLISTENER_H_

#include "IImgLayer.h"

/*!
	ImgFileのイベント時に呼び出されるクラスインターフェイス
	継承して使用する。
*/
class ImgFileEventListener {
public:
	ImgFileEventListener():
	  m_isLockEvent(false),
	  m_isCalledImgFile(false) {};
	virtual ~ImgFileEventListener(){};

	//!	イベント呼び出しを行わないようにする
	void LockFileEvent(){ m_isLockEvent = true; };

	//! イベント呼び出しを行うようにする
	void UnlockFileEvent(){ m_isLockEvent = false; };

	//! イベント呼び出しを行うかどうか
	bool IsLockFileEvent(){ return m_isLockEvent; };

	bool IsCalledImgFile(){ return m_isCalledImgFile; };
	void SetCalledImgFile(bool bl){ m_isCalledImgFile = bl; };

	////////////////////////////
	/*!
		レイヤーが追加された後に呼び出す。
		@param[in] pLayer 追加されるレイヤー
		@param[in] index 追加されたレイヤーのインデックス
	*/
	virtual void OnAddLayer(IImgLayer_Ptr pLayer, int index){};

	/////////////////////////////
	/*!
		レイヤーを削除する前に呼び出す
		@param[in] pLyaer 削除されるレイヤー
		@param[in] index 削除されたレイヤーのインデックス
	*/
	virtual void OnDeleteLayer(IImgLayer_Ptr pLayer, int index){};

	virtual void OnChangeLayerLine(int from_index, int to_index){};

	//////////////////////////////
	/*!
		選択レイヤーが変わったときに呼び出す
		@param[in] pLayer 選択されたレイヤー
	*/
	virtual void OnSelectLayer(IImgLayer_weakPtr pLayer){};

	////////////////////////////
	/*!
		マスクが追加された後に呼び出す。
		@param[in] pMask 追加されるマスク
		@param[in] index 追加されたマスクのインデックス
	*/
	virtual void OnAddMask(ImgMask* pMask, int index){};

	/////////////////////////////
	/*!
		マスクを削除する前に呼び出す
		@param[in] pMask 削除されるマスク
		@param[in] index 削除されたマスクのインデックス
	*/
	virtual void OnDeleteMask(ImgMask* pMask, int index){};

	//////////////////////////////
	/*!
		選択マスクが変わったときに呼び出す
		@param[in] pMask 選択されたマスク
	*/
	virtual void OnSelectMask(ImgMask* pMask){};

	//////////////////////////////
	/*!
		表示位置が変わったときに呼び出す
		@param[in] size 表示位置
	*/
	virtual void OnChangeViewPos(const CvPoint2D64f *pos){};

	//////////////////////////////
	/*!
		表示サイズが変わったときに呼び出す
		@param[in] size 表示サイズ
	*/
	virtual void OnChangeViewSize(int size){};

	///////////////////////////////
	/*!
		表示角度が変わったときに呼び出す
		@param[in] rot 表示角度
	*/
	virtual void OnChangeViewRot(int rot){};

	///////////////////////////////
	/*!
		表示の左右反転が切り替わったときに呼び出す
		@param[in] state
	*/
	virtual void OnChangeViewFlipH(double state){};

	///////////////////////////////
	/*!
		表示角度が変わったときに呼び出す
		@param[in] state
	*/
	virtual void OnChangeViewFlipV(double state){};

	///////////////////////////////
	/*!
	*/
	virtual void OnUpdate(){};

	///////////////////////////////
	/*!
		ファイルハンドル、追加時に呼び出し
	*/
	virtual void OnAddUndoImgFileHandle(const ImgFileHandle* pHandle){};
	///////////////////////////////
	/*!
		古いファイルハンドル、削除時に呼び出し
	*/
	virtual void OnRemoveOldUndoImgFileHandle(int num){};

	virtual void OnUndoImgFileHandle(const ImgFileHandle* pHandle){};
	virtual void OnRedoImgFileHandle(const ImgFileHandle* pHandle){};

	///////////////////////////////
	/*!
	*/
	virtual void OnImgFilePushUpdateData(const LPUPDATE_DATA data){};

private:
	bool m_isLockEvent;
	bool m_isCalledImgFile;
};

#endif //_IMGFILEEVENTLISTENER_H_