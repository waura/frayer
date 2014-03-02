#ifndef _IMGFILE_H_
#define _IMGFILE_H_

#include "LibIEDllHeader.h"

#include "IEffect.h"
#include "IIETool.h"
#include "IEColor.h"
#include "ImgUpdateData.h"
#include "ImgLayer.h"
#include "ImgLayerGroup.h"
#include "ImgMask.h"
#include "ImgFileHandleMG.h"

#include <list>
#include <string>
#include <vector>

#define IMG_FILE_COUNT_TIME //

//typedef std::vector<ImgMask*> ImgMask_Vec;
typedef std::list<ImgFileHandle*> ImgFileHandle_List;
typedef std::list<ImgFileEventListener* > ImgFileEventListener_List;

_EXPORTCPP ImgFile_Ptr CreateIEImgFile();
//_EXPORT void ReleaseIEImgFile(ImgFile_Ptr* ppFile);

enum{
	MASK_DOT_LINE_UPDATE_TIME = 1000,
	MASK_DOT_LINE_SIZE = 3,
};

enum IE_MASK_DRAW_ID {
	MASK_SOLID_DRAW,
	MASK_DOT_LINE_DRAW,
};

enum IE_FILE_RESOURCE_ID {
	IE_DRAW_IMG=0x01,
	IE_DRAW_MASK_IMG=0x02,
	IE_DISPLAY_IMG=0x04,
	IE_UPDATE_DATA=0x08,
	IE_LAYER=0x010,
	IE_MASK=0x020,
	IE_EVENTLISTENER=0x040,
};


//ひとつの画像ファイルを表すクラス
class _EXPORTCPP ImgFile :
	public std::enable_shared_from_this<ImgFile>
{
public:
	ImgFile();
	virtual ~ImgFile();

	/////////////////////////////////
	/*!
		イベントリスナーを登録する。
		@param[in] pListener 登録するリスナー
	*/
	void AddEventListener(ImgFileEventListener*  pListener);

	//////////////////////////////////
	/*!
		イベントリスナーを削除する
		@param[in] pListener 削除するリスナー
	*/
	void DeleteEventListener(ImgFileEventListener*  pListener);

	//
	inline bool IsInit(){
		return isInit;
	}

	//////////////////////////////////////
	/*
		初期化処理
		画像サイズを与えて初期化
	*/
	bool Init(CvSize size);

	//////////////////////////////////////
	/*
		初期化処理
		画像サイズを与えて初期化
	*/
	bool InitCanvasSize(CvSize size);

	void Close();

	//////////////////////////////////////
	/*!
		背景色を設定する。
		@param[in] color 設定する色
	*/
	void SetBGColor(IEColor color){
		m_BackGroundColor = color;
	}

	void SetNumOfHistory(unsigned int num);

	//////////////////////////////////////
	/*!
		ファイルのパスをセットする。
		@param[in] path パス
	*/
	void SetPath(const char* path);

	//////////////////////////////////////
	/*!
		ファイルのパスを返す。
		@return パスを返す
	*/
	std::string GetPath() const
	{
		return m_path;
	}

	///////////////////////////////////////
	/*!	
		マスクの表示法を設定する。
		@param[in] id 表示法を現すid
	*/
	void SetMaskDrawType(int id)
	{
		m_MaskDrawType = id;
	}

	///////////////////////////////////////
	/*!	
		マスクの表示法のIDを返す。
		@return id 表示法を現すid
	*/
	int GetMaskDrawType() const
	{
		return m_MaskDrawType;
	}

	///////////////////////////////////////
	/*!
		画像のサイズを返す
		@return サイズを返す
	*/
	CvSize GetImgSize() const
	{
		return m_size;
	}

	//////////////////////////////////////
	/*!
		画像が描画されているウィンドウのクライアントの範囲を
		スクリーン座標で渡す。
		@param[out] lrect
	*/
	void GetDrawWndClientRect(LPRECT lrect) const;

	ImgFileHandle* CreateImgFileHandle(int handle_id) {
		return ImgFileHandleMG::CreateImgFileHandle(this->shared_from_this(), handle_id);
	}
	void ReleaseImgFileHandle(ImgFileHandle* pHandle) {
		ImgFileHandleMG::ReleaseImgFileHandle(pHandle);
	}

	/////////////////////////////////////
	/*!
		ファイル操作クラスを適用し、リストに追加
		@param[in] pHandle ファイル操作クラス
		@return
	*/
	bool DoImgFileHandle(ImgFileHandle* pHandle);

	/////////////////////////////////////
	/*!
		処理をi個戻す
		@param[in] i 戻す処理の個数
	*/
	void Undo(int i);

	//////////////////////////////////////
	/*!
		処理をi個進める
		@param[in] i 進める処理の個数
	*/
	void Redo(int i);

	size_t GetUndoHandleSize();
	const ImgFileHandle* GetUndoHandleAt(int index);

	size_t GetRedoHandleSize();
	const ImgFileHandle* GetRedoHandleAt(int index);

	/////////////////////////////////////
	/*!
		画像更新データを記憶
		@param[in] data 画像更新データフラグ
	*/
	LPUPDATE_DATA CreateUpdateData();

	/////////////////////////////////////
	/*!
		画像更新データを記憶
		@param[in] data 画像更新データフラグ
	*/
	void PushUpdateData(LPUPDATE_DATA data);

	//////////////////////////////////////
	/*!
		画像更新データを削除
	*/
	void ClearUpdateData();

	/////////////////////////////////////
	/*!
		フラグがたっているもののイメージを再生成する
		@param[in] enable_lprc 更新できる範囲、NULLなら全範囲更新
		@param[out] updated_lprc 更新した範囲
		@return 更新があるならtrue,そうでないならfalse
	*/
	bool Update(const LPRECT enable_lprc, LPRECT updated_lprc);

	void LockUpdateData();
	void UnlockUpdateData();

	///////////////////////////////////////
	/*!
		出力画像をコピーする。
		@param[in] image コピー先 4ch BGRA
	*/
	void CopyFileImage(IplImage *image);
	void CopyFileImageThumbnail(IplImage *image);

	///////////////////////////////////////
	/*!
		マスクをコピーする。
		@param[in] start_x
		@param[in] start_y
		@param[in] width
		@param[in] height
		@param[in] dst
		@param[in] dst_x
		@param[in] dst_y
	*/
	void CopyFileMask(int start_x, int start_y, int width, int height,
		IplImage* dst, int dst_x, int dst_y);
	void CopyFileMask(int start_x, int start_y, int width, int height,
		IplImageExt* dst, int dst_x, int dst_y);

	/////////////////////////////////
	/*!
		表示サイズをセットする(1～800%)
		@param[in] size 表示サイズ
		@param[in] lpt 中心座標
	*/
	void SetViewSize(int size);
	void SetViewSize(int size, LPPOINT lpt);
	void UpViewSize();
	void DownViewSize();

	/////////////////////////////////
	/*!
		表示サイズを返す
		@return 表示サイズ
	*/
	int GetViewSize() const {
		return m_view_size;
	}

	/////////////////////////////////
	/*!
		表示画像回転角をセットする。
		@param[in] rot 回転角 (0 ~ 360)
	*/
	void SetViewRot(double rot);

	//////////////////////////////////
	/*!
		表示画像の回転角を返す。
		@return 回転角 (0 ~ 360)
	*/
	double GetViewRot() const {
		return m_view_rot;
	}

	///////////////////////////////////
	/*!
		表示開始位置をセット　左上原点
	*/
	void SetViewPos(const CvPoint2D64f *viewPos);

	///////////////////////////////////
	/*!
		表示開始位置を返す。
	*/
	void GetViewPos(CvPoint2D64f* viewPos) const;

	///////////////////////////////////
	/*!
		表示中心位置(画像座標)を返す
	*/
	CvPoint2D64f GetViewCenterPos();

	///////////////////////////////////////
	/*!
		ウィンドウサイズが変更されたとき呼び出す
		@param[in] lrect ウィンドウの範囲
	*/
	void OnWindowReSize(LPRECT lrect);

	CvSize GetDisplaySize();

	///////////////////////////////////////
	/*!
		display image flip horizontal
	*/
	inline bool IsViewFlipH() {
		return m_isViewFlipH;
	}

	void SetViewFlipH(bool bl);

	///////////////////////////////////////
	/*!
		display image flip vertical
	*/
	inline bool IsViewFlipV() {
		return m_isViewFlipV;
	}

	void SetViewFlipV(bool bl);

	///////////////////////////////////////
	/*!
		add layer
		@param[in] pLayer 追加するレイヤーのポインタ
		@param[in] index 追加先インデックス
	*/
	void AddLayer(IImgLayer_Ptr pLayer);
	void AddLayer(IImgLayer_Ptr pLayer, int index);
	void AddLayer(ImgLayerGroup_Ptr pLayerGroup);

	///////////////////////////////////////
	/*!
		remove layer
		@param[in,out] pLayer 取り除くレイヤーのポインタ
		@param[in] index 取り除くレイヤーのインデックス
	*/
	void RemoveLayer(IImgLayer_Ptr pLayer);

	///////////////////////////////////////
	/*!
		レイヤーの並びを変える
	*/
	void ChangeLayerLine(int from_index, int to_index);

	///////////////////////////////////////
	/*!
		ファイルが保持しているレイヤーの数を返す
		@return number of layer
	*/
	int GetLayerSize() const;

	///////////////////////////////////////
	/*!
		get layer index
		@param[in] pLayer to get index of layer
		@return layer index, if don't exist layer return -1
	*/
	inline int GetLayerIndex(IImgLayer_weakPtr pLayer) const
	{
		assert(m_pRootLayerGroup);
		return m_pRootLayerGroup->GetLayerIndex(pLayer);
	}

	///////////////////////////////////////
	/*!
		get layer from layer index
		@param[in] index of get layer
		@return layer pointer, if don't exist layer return NULL
	*/
	inline IImgLayer_Ptr GetLayerAtIndex(int index) const
	{
		assert(m_pRootLayerGroup);
		return m_pRootLayerGroup->GetLayerAtIndex(index);
	}

	////////////////////////////////////////
	/*!
	*/
	inline ImgLayerGroup_Ptr const GetRootLayerGroup() const {
		return m_pRootLayerGroup;
	}

	/////////////////////////////////////////
	/*!
	*/
	void SetRootLayerGroup(ImgLayerGroup_Ptr pRootLayerGroup);

	///////////////////////////////////////
	/*!
		選択中のレイヤーオブジェクトのポインターを返す
		@return レイヤーオブジェクトのポインタ
	*/
	inline IImgLayer_weakPtr const GetSelectLayer() const {
		return m_pSelectLayer;
	}

	///////////////////////////////////////
	/*!
		選択レイヤーをセットする。
		@param[in] layer 選択するレイヤー
	*/
	void SetSelectLayer(IImgLayer_weakPtr layer);
	void UnsetSelectLayer() {
		m_pSelectLayer.expired();
	}

	/////////////////////////////////////////
	///*!
	//	マスクを追加する。
	//	@param[in] pMask 追加するマスクのポインタ
	//	@param[in] index 追加先インデックス
	//*/
	//void AddMask(ImgMask *pMask);
	//void AddMask(ImgMask *pMask, int index);

	/////////////////////////////////////////
	///*!
	//	マスクを削除する。
	//	@param[in,out] pMask 削除するマスクのポインタ
	//	@param[in] index 削除するマスクのインデックス
	//*/
	//void DeleteMask(ImgMask *pMask);
	//void DeleteMask(int index);

	/////////////////////////////////////////
	///*!
	//	ファイルが保持しているマスクの数を返す
	//	@return マスク枚数
	//*/
	//int GetMaskSize() const;

	/////////////////////////////////////////
	///*!
	//	インデックスを指定してマスクオブジェクトへのポインタを返す
	//	@return マスクオブジェクトのポインタ
	//*/
	//ImgMask* const GetMaskAtIndex(int index) const;

	/////////////////////////////////////////
	///*!
	//	指定したマスクオブジェクトのインデックスを返す
	//	@param[in] mask 指定するマスクへのポインタ
	//	@return 指定したマスクのインデックス
	//*/
	//int GetMaskIndex(const ImgMask* mask) const;

	///////////////////////////////////////
	/*!
		選択中のマスクオブジェクトのポインターを返す
		@return マスクオブジェクトのポインタ
	*/
	ImgMask_Ptr GetSelectMask() const {
		return m_pMask;
	}
	//int GetSelectMaskIndex() const;

	/////////////////////////////////////////
	///*!
	//	選択マスクをセットする
	//	@param[in] mask 選択するマスク
	//*/
	//void SetSelectMask(ImgMask* mask);
	//void SetSelectMask(int index);

	void GetDrawImgPixel(int x, int y, UCvPixel* dst) const {
		if( ((0 <= x) && (x < m_DrawImg->width)) &&
			((0 <= y) && (y < m_DrawImg->height)) ){
			GetPixelFromBGRA(m_DrawImg, x, y, dst);
		}
		else{
			dst->value = 0;
		}
	}
	void GetMaskData(int x, int y, uint8_t* dat) const {
		if( ((0 <= x) && (x < m_DrawImg->width)) &&
			((0 <= y) && (y < m_DrawImg->height)) ){
			(*dat) = GetMaskDataPos(m_DrawMaskImg, x, y);
		}
		else{
			dat = 0;
		}
	}

	///////////////////////////////////////
	/*!
		return draw image
		@return draw image
	*/
	const IplImage* GetDrawImg() const {
		assert( isInit );
		return m_DrawImg;
	}

	///////////////////////////////////////
	/*!
		return draw mask image
		@return draw mask image
	*/
	const IplImage* GetDrawMaskImg() const {
		assert( isInit );
		return m_DrawMaskImg;
	}

	///////////////////////////////////////
	/*!
		return display image
		@return display image
	*/
	const IplImage* GetDisplayImg() const {
		assert( isInit );
		return m_DisplayImg;
	}



	///////////////////////////////////////
	/*!
		マスクの状態を返す
		@return
	*/
	inline unsigned int GetMaskState() const {
		return m_pMask->GetMaskState();
	}

	///////////////////////////////////////
	/*!
		マスクの範囲を返す
		@param[out] lprc マスクの範囲
	*/
	inline void GetMaskRect(LPRECT lprc) {
		ResourceLock(IE_FILE_RESOURCE_ID::IE_MASK);
		ucvGetMaskRect(m_DrawMaskImg, lprc);
		ResourceUnlock(IE_FILE_RESOURCE_ID::IE_MASK);
	}

	///////////////////////////////////////
	/*!
		全てのマスクを不可視にする。
	*/
	void AllMaskInvisible() const {
	}

	////////////////////////////////
	/*!
		選択領域の上にマウスがあるかどうか？
		@param[in] lpt マウスの入力情報
		@return マウスが選択領域の上ならtrue そうでないならfalse
	*/
	bool IsOnSelectImage(LPPOINT lpt);

	//////////////////////////////////////
	/*!
		ウィンドウクライアント座標を画像の座標に変換する。
		@param[in,out] lpd 入力、出力
	*/
	void ConvertPosWndToImg(LPIE_INPUT_DATA lpd) const;
	void ConvertPosWndToImg(const LPPOINT src, LPPOINT dst) const;
	void ConvertPosWndToImg(const CvPoint* src, CvPoint* dst) const;
	void ConvertPosWndToImg(const CvPoint2D64f* src, CvPoint2D64f* dst) const;
	void ConvertPosWndToImg(const LPRECT src_rc, LPRECT dst_rc) const;

	//////////////////////////////////////
	/*!
		画像の座標をウィンドウクライアント座標に変換する。
		@param[in,out] lpd 入力、出力
	*/
	void ConvertPosImgToWnd(LPIE_INPUT_DATA lpd) const;
	void ConvertPosImgToWnd(const LPPOINT src, LPPOINT dst) const;
	void ConvertPosImgToWnd(const CvPoint* src, CvPoint* dst) const;
	void ConvertPosImgToWnd(const CvPoint2D64f* src, CvPoint2D64f* dst) const;
	void ConvertPosImgToWnd(const CvPoint2D64f* src, CvPoint2D64f* dst, int size) const;
	void ConvertPosImgToWnd(const LPRECT src_rc, LPRECT dst_rc) const;

private:
	////////////////////////////////////////
	inline void ResourceLock(unsigned int id){
#ifdef _WIN32
		//switch(id){
		//	case IE_FILE_RESOURCE_ID::IE_DRAW_IMG:
		//		::EnterCriticalSection( &m_csDrawImg );
		//		break;
		//	case IE_FILE_RESOURCE_ID::IE_DRAW_MASK_IMG:
		//		::EnterCriticalSection( &m_csDrawMaskImg );
		//		break;
		//	case IE_FILE_RESOURCE_ID::IE_DISPLAY_IMG:
		//		::EnterCriticalSection( &m_csDisplayImg );
		//		break;
		//	case IE_FILE_RESOURCE_ID::IE_UPDATE_DATA:
		//		::EnterCriticalSection( &m_csUpdateData );
		//		break;
		//	case IE_FILE_RESOURCE_ID::IE_LAYER:
		//		::EnterCriticalSection( &m_csIELayer );
		//		break;
		//	case IE_FILE_RESOURCE_ID::IE_MASK:
		//		::EnterCriticalSection( &m_csIEMask );
		//		break;
		//	case IE_FILE_RESOURCE_ID::IE_EVENTLISTENER:
		//		::EnterCriticalSection( &m_csIEEventListener );
		//		break;
		//	default:
		//		assert( 0 );
		//		break;
		//}
		switch(id){
			case IE_FILE_RESOURCE_ID::IE_DRAW_IMG:
				::WaitForSingleObject(m_csDrawImg, INFINITE);
				break;
			case IE_FILE_RESOURCE_ID::IE_DRAW_MASK_IMG:
				::WaitForSingleObject(m_csDrawMaskImg, INFINITE);
				break;
			case IE_FILE_RESOURCE_ID::IE_DISPLAY_IMG:
				::WaitForSingleObject(m_csDisplayImg, INFINITE);
				break;
			case IE_FILE_RESOURCE_ID::IE_UPDATE_DATA:
				::WaitForSingleObject(m_csUpdateData, INFINITE);
				break;
			case IE_FILE_RESOURCE_ID::IE_LAYER:
				::WaitForSingleObject(m_csIELayer, INFINITE);
				break;
			case IE_FILE_RESOURCE_ID::IE_MASK:
				::WaitForSingleObject(m_csIEMask, INFINITE);
				break;
			case IE_FILE_RESOURCE_ID::IE_EVENTLISTENER:
				::WaitForSingleObject(m_csIEEventListener, INFINITE);
				break;
			default:
				assert( 0 );
				break;
		}
#else
		while((m_LockResourceID & id) == id);
		m_LockResourceID |= id;
#endif
	}
	inline void ResourceUnlock(unsigned int id){
#ifdef _WIN32
		//switch(id){
		//	case IE_FILE_RESOURCE_ID::IE_DRAW_IMG:
		//		::LeaveCriticalSection( &m_csDrawImg );
		//		break;
		//	case IE_FILE_RESOURCE_ID::IE_DRAW_MASK_IMG:
		//		::LeaveCriticalSection( &m_csDrawMaskImg );
		//		break;
		//	case IE_FILE_RESOURCE_ID::IE_DISPLAY_IMG:
		//		::LeaveCriticalSection( &m_csDisplayImg );
		//		break;
		//	case IE_FILE_RESOURCE_ID::IE_UPDATE_DATA:
		//		::LeaveCriticalSection( &m_csUpdateData );
		//		break;
		//	case IE_FILE_RESOURCE_ID::IE_LAYER:
		//		::LeaveCriticalSection( &m_csIELayer );
		//		break;
		//	case IE_FILE_RESOURCE_ID::IE_MASK:
		//		::LeaveCriticalSection( &m_csIEMask );
		//		break;
		//	case IE_FILE_RESOURCE_ID::IE_EVENTLISTENER:
		//		::LeaveCriticalSection( &m_csIEEventListener );
		//		break;
		//	default:
		//		assert( 0 );
		//		break;
		//}
		switch(id){
			case IE_FILE_RESOURCE_ID::IE_DRAW_IMG:
				::ReleaseMutex(m_csDrawImg);
				break;
			case IE_FILE_RESOURCE_ID::IE_DRAW_MASK_IMG:
				::ReleaseMutex(m_csDrawMaskImg);
				break;
			case IE_FILE_RESOURCE_ID::IE_DISPLAY_IMG:
				::ReleaseMutex(m_csDisplayImg);
				break;
			case IE_FILE_RESOURCE_ID::IE_UPDATE_DATA:
				::ReleaseMutex(m_csUpdateData);
				break;
			case IE_FILE_RESOURCE_ID::IE_LAYER:
				::ReleaseMutex(m_csIELayer);
				break;
			case IE_FILE_RESOURCE_ID::IE_MASK:
				::ReleaseMutex(m_csIEMask);
				break;
			case IE_FILE_RESOURCE_ID::IE_EVENTLISTENER:
				::ReleaseMutex(m_csIEEventListener);
				break;
			default:
				assert( 0 );
				break;
		}
#else
		m_LockResourceID &= (~id);
#endif
	}

	/////////////////////////////////////
	/*!
		最終表示画像の再生成
		@param[in] lprc 更新範囲 画像座標
	*/
	void UpdateDisplayImage(const LPRECT lprc);

	/////////////////////////////////////
	/*!
		描画イメージの再生成
		@param[in] lprc 更新範囲 画像座標
	*/
	void UpdateDrawImage(const LPRECT lprc);

	/////////////////////////////////////
	/*!
		描画イメージの再生成
		@param[in] lprc 更新範囲 画像座標
	*/
	void UpdateDrawImageFromDisplayScan(const LPRECT lprc);

	/////////////////////////////////////
	/*!
		選択範囲も入れた描画イメージの再生成
		@param[in] lprc 更新範囲 NULLなら全範囲
	*/
	void UpdateMaskImage(const LPRECT lprc);

	//////////////////////////////////////
	/*!
		平行移動
	*/
	void UpdateViewParallelMove(int d_x, int d_y);

	///////////////////////////////////////
	/*!
		update affin transform matrix
	*/
	void UpdateTransformMat();

	///////////////////////////////////////
	/*!
		オフスクリーンバッファーを作成する。
		@param[in] lprect スクリーンの
	*/
	void ReGenOffScreenBuffer(LPRECT lprect);


	ImgFileHandle_List m_UndoHandle_List; //Undoファイル操作クラスリスト
	ImgFileHandle_List m_RedoHandle_List; //Redoファイル操作クラスリスト

	ImgFileEventListener_List m_ImgFileEventListener_List; //イベントリスナーリスト
	
	char* m_path;				//ファイルのパス
	CvSize m_size;				//画像サイズ
	RECT m_ClientRect;			//

	int m_view_size;				//画像表示サイズ(1～800%)
	double m_view_rot;				//画像表示角　度数
	CvPoint2D64f m_view_startPt;	//画像表示開始左上原点
	CvPoint2D64f m_view_centerPt;	//ディスプレイの中心点の座標 画像座標
	CvMat* m_TransformMat;
	CvMat* m_TransformInvMat;

	bool m_isViewFlipH;	//左右反転
	bool m_isViewFlipV;	//上下反転
	int m_MaskDrawType;

	IplImage* m_DrawImg;	//表示画像	4ch BGR
	IplImage* m_DrawMaskImg;//表示マスク    1ch
	IplImage* m_DisplayImg; //最終表示画像 (選択範囲＋表示元画像) 3ch BGR
	
	LPUpdateData_List* m_pUpdateDataForeGrnd_List; //更新情報を格納する
	LPUpdateData_List* m_pUpdateDataBackGrnd_List; //

	ImgLayerGroup_Ptr m_pRootLayerGroup;

	//ImgMask_Vec m_Mask_Vec;	//選択範囲配列
	ImgMask_Ptr m_pMask;	//

	IImgLayer_weakPtr m_pSelectLayer;	//選択中のレイヤー
	//ImgMask* m_pSelectMask;

	IEColor m_BackGroundColor;
	unsigned int m_NumOfHistory;

	bool isInit;

#ifdef _WIN32
	//CRITICAL_SECTION m_csDrawImg;
	//CRITICAL_SECTION m_csDrawMaskImg;
	//CRITICAL_SECTION m_csDisplayImg;
	//CRITICAL_SECTION m_csUpdateData;
	//CRITICAL_SECTION m_csIELayer;
	//CRITICAL_SECTION m_csIEMask;
	//CRITICAL_SECTION m_csIEEventListener;
	HANDLE m_csDrawImg;
	HANDLE m_csDrawMaskImg;
	HANDLE m_csDisplayImg;
	HANDLE m_csUpdateData;
	HANDLE m_csIELayer;
	HANDLE m_csIEMask;
	HANDLE m_csIEEventListener;
#else
	unsigned int m_LockResourceID;
#endif //_WIN32

#ifdef IMG_FILE_COUNT_TIME
public:
	double m_UpdateMaskTime;
	double m_UpdateLayerTime;
	double m_UpdateDrawImageTime;
	double m_UpdateMaskImageTime;
	double m_UpdateDisplayImageTime;
#endif //_IMG_FILE_COUNT_TIME
};

#endif
