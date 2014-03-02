#ifndef _IIMGLAYER_H_
#define _IIMGLAYER_H_

#include "LibIEDllHeader.h"
#include "ImgLayerEventListener.h"
#include "ImgUpdateData.h"
#include "ILayerSynthesizer.h"
#include "LayerSynthesizerMG.h"

#include <string>
#include <list>

typedef std::list<ImgLayerEventListener* > ImgLayerEventListener_List;

enum IE_LAYER_RESOURCE_ID{
	UPDATE_DATA_QUEUE=0x01,
	EDITNODE_QUEUE=0x02,
};

enum IE_LAYER_TYPE{
	NORMAL_LAYER,
	GROUP_LAYER,
};

class IImgLayer;
//_EXPORT void ReleaseIEIImgLayer(IImgLayer_Ptr* ppLayer);

//レイヤークラス インターフェース
class _EXPORTCPP IImgLayer
{
public:
	IImgLayer(ImgFile_weakPtr parent_file);
	virtual ~IImgLayer();

	virtual int GetLayerType()=0;

	////////////////////////////////////
	/*!
		layer initialize
		@param[in] lprc layer rect
	*/
	virtual void Init(const LPRECT lprc);

	////////////////////////////////////
	/*!
	*/
	virtual void End();

	////////////////////////////////////
	/*!
		@param[in] enable_lprc
		@param[in] updated_lprc
	*/
	virtual bool Update(const LPRECT enable_lprc, LPRECT updated_lprc);

	LPUPDATE_DATA CreateUpdateData();
	void PushUpdateData(LPUPDATE_DATA data);
	virtual void ClearUpdateData();
	virtual void LockUpdateData();
	virtual void UnlockUpdateData();

	/////////////////////////////////
	/*!
		regist event listener
		@param[in] pListener  to regist event listener
	*/
	void AddEventLilstener(ImgLayerEventListener*  pListener);

	//////////////////////////////////
	/*!
		delete event listener
		@param[in] pListener to delete event listener
	*/
	void DeleteEventListener(ImgLayerEventListener*  pListener);	

	//////////////////////////////////
	/*!
		set layer name
		@param[in] layer_name
	*/
	void SetName(const char* layer_name);
	int GetNameSize() const;
	void GetName(char* layer_name) const;

	///////////////////////////////////
	//virtual void MoveLayer(int move_x, int move_y) {
	//	m_LayerRect.top += move_y;
	//	m_LayerRect.bottom += move_y;
	//	m_LayerRect.left += move_x;
	//	m_LayerRect.right += move_x;
	//}

	/////////////////////////////////
	void GetLayerRect(LPRECT lprc) const {
		(*lprc) = m_LayerRect;
	}

	//////////////////////////////////
	/*!
		set opacity value
		@param[in] opacity [0.0, 1.0]
	*/
	void SetOpacity(double opacity);

	//////////////////////////////////
	/*!
		set opacity value
		@param[in] opacity [0.0, 255]
	*/
	inline void SetOpacity(uint8_t opacity)
	{
		assert(0 <= opacity && opacity <= 255);
		if (0 <= opacity && opacity <= 255) {
			SetOpacity( ((double)opacity)/255.0 );
		}
	}

	//////////////////////////////////
	/*!
		set opacity percent value
		@param[in] alpha opacity(0~100)
	*/
	inline void SetOpacityPer(int opacity_per)
	{
		assert(0 <= opacity_per && opacity_per <= 100);
		if (0 <= opacity_per && opacity_per <= 100) {
			SetOpacity( (opacity_per / 100.0) );
		}
	}

	//////////////////////////////////
	/*!
		return opacity value(0~255)
		@return
	*/
	inline uint8_t GetOpacity() const {
		return (int)(255.0 * m_Alpha + 0.5);
	}

	//////////////////////////////////
	/*!
		return opacity percent value(0~100)
		@return
	*/
	inline int GetOpacityPer() const {
		return (int)(100.0 * m_Alpha + 0.5);
	}

	/////////////////////////////////
	/*!
		set layer index
	*/
	inline void SetIndex(int index){
		m_Index = index;
	}
	inline int GetIndex(){
		return m_Index;
	}

	/////////////////////////////////
	/*!
	*/
	inline void SetParentLayer(IImgLayer_Ptr parent_layer){
		m_pParentLayer = parent_layer;
	}
	inline IImgLayer_weakPtr GetParentLayer() const {
		return m_pParentLayer;
	}

	/////////////////////////////////
	/*!
	*/
	inline void SetBottomChildLayer(IImgLayer_Ptr child_layer){
		m_pBottomChildLayer = child_layer;
	}
	inline IImgLayer_Ptr GetBottomChildLayer() const {
		return m_pBottomChildLayer;
	}

	/////////////////////////////////
	/*!
		上になるレイヤーへのポインタを指定する。
		@param[in] over_layer
	*/
	inline void SetOverLayer(IImgLayer_Ptr over_layer){
		m_pOverLayer = over_layer;
	}
	inline IImgLayer_weakPtr GetOverLayer() const {
		return m_pOverLayer;
	}

	/////////////////////////////////
	/*!
		下になるレイヤーへのポインタを指定する。
		@param[in] under_layer
	*/
	inline void SetUnderLayer(IImgLayer_Ptr under_layer){
		m_pUnderLayer = under_layer;
	}
	inline IImgLayer_weakPtr GetUnderLayer() const {
		return m_pUnderLayer;
	}

	//////////////////////////////////
	/*!
		レイヤーが所属しているファイルへのポインタを返す
		@return  レイヤーが所属しているファイルへのポインタ
	*/
	inline ImgFile_weakPtr GetParentFile() const {
		return m_pParentFile;
	}

	//////////////////////////////////
	/*!
		return display state
		@return display state
	*/
	inline bool IsVisible() const {
		return m_IsVisible;
	}

	/////////////////////////////////
	/*!
		set display state
		@param[in] bl display state
	*/
	inline void SetVisible(bool bl){
		m_IsVisible = bl;
	}

	/////////////////////////////////
	/*!
		set layer synth method fourcc
		@param[in] fourcc
	*/
	void SetLayerSynthFourCC(uint32_t fourcc);
	inline uint32_t GetLayerSynthFourCC() const {
		return m_LayerSynthFourCC;
	}

	//////////////////////////////
	/*!
		フレーム内に不足分がなくなるようにレイヤーイメージを拡張する。
	*/
	void ExtendLayer();

	//////////////////////////////////////////////////
	/*!
		レイヤーのピクセル情報を得る
	*/
	inline void GetPixel(int x, int y, UCvPixel* dst) const{
		if( isInRect(x, y, (LPRECT)&m_LayerRect) ){
			m_Img->GetPixel(x - m_LayerRect.left, y - m_LayerRect.top, dst);
		}
	}

	const IplImageExt* GetLayerImage() const {
		return m_Img;
	}

	///////////////////////////
	/*!
		レイヤー画像をdstにコピーする
		@param[out] dst コピー先
		@param[in] dst_startX dstのコピー開始位置Y座標
		@param[in] dst_startY dstのコピー開始位置Y座標
		@param[in] src_startX レイヤー画像コピー開始位置、(ファイル画像座標)
		@param[in] src_startY
		@param[in] width
		@param[in] height
		@param[in] code 転送チャンネルコード
		@praam[in] mask マスク画像
	*/
	void CopyLayerImage(IplImage* dst, int dst_startX, int dst_startY,
		int src_startX, int src_startY, int width, int height,
		const IplImage* mask = NULL) const;
	void CopyLayerImage(IplImageExt* dst, int dst_startX, int dst_startY,
		int src_startX, int src_startY, int width, int height,
		const IplImage* mask = NULL, int mask_startX = 0, int mask_startY = 0) const;

	//void BltLineSynthPixel(UCvPixel* dst, int start_x, int start_y, int width) const;

	////////////////////////////////////////////////////
	/*!
		@param[out] dst bltted image
		@param[in] d_x dst image start x (image axis)
		@param[in] d_y dst image start y (image axis)
		@param[in] s_x src image start x (frame axis)
		@param[in] s_y src image start y (frame axis)
		@param[in] width blt width
	*/
	//void BltLineSynthPixel(IplImageExt* dst, int d_x, int d_y, int s_x, int s_y, int width) const;

	////////////////////////////////////////////////////
	/*!
		@param[out] dst bltted image
		@param[in] d_x dst image start x (image axis)
		@param[in] d_y dst image start y (image axis)
		@param[in] s_x src image start x (frame axis)
		@param[in] s_y src image start y (frame axis)
		@param[in] width blt width
		@param[in] height blt height
	*/
	virtual void BltRectSynthPixel(IplImage* dst, int d_x, int d_y, int s_x, int s_y, int width, int height) const;
	virtual void BltRectSynthPixel(IplImageExt* dst, int d_x, int d_y, int s_x, int s_y, int width, int height) const;


protected:
	///////////////////////////////////
	inline void ResourceLock(unsigned int id){
#ifdef _WIN32
		switch( id ) {
			case IE_LAYER_RESOURCE_ID::UPDATE_DATA_QUEUE:
				::EnterCriticalSection( &m_csUpdateDataQueue );
				break;
			case IE_LAYER_RESOURCE_ID::EDITNODE_QUEUE:
				::EnterCriticalSection( &m_csEditNodeQueue );
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
		switch( id ) {
			case IE_LAYER_RESOURCE_ID::UPDATE_DATA_QUEUE:
				::LeaveCriticalSection( &m_csUpdateDataQueue );
				break;
			case IE_LAYER_RESOURCE_ID::EDITNODE_QUEUE:
				::LeaveCriticalSection( &m_csEditNodeQueue );
				break;
			default:
				assert( 0 );
				break;
		}
#else
		m_LockResourceID &= (~id);
#endif
	}

	int m_Index;				//!< layer index
	double m_Alpha;				//!< opacity(0.0 ~ 1.0)
	bool m_IsVisible;			//!< 可視状態かどうか
	RECT m_LayerRect;			//!< レイヤー範囲 (frame axis)
	IplImageExt* m_Img;			//!< 表示画像  4ch BGRA
	uint32_t m_LayerSynthFourCC;

	LPUpdateData_List* m_pUpdateDataForeGrnd_List;
	LPUpdateData_List* m_pUpdateDataBackGrnd_List;

private:

	ImgLayerEventListener_List m_ImgLayerEventListener_List;

	ImgFile_weakPtr m_pParentFile;		//レイヤーが所属するファイルへのポインタ
	IImgLayer_weakPtr m_pParentLayer;	//自分の親レイヤーへのポインタ
	IImgLayer_Ptr m_pOverLayer;	//自分の上になるレイヤーへのポインタ
	IImgLayer_weakPtr m_pUnderLayer;	//自分の下になるレイヤーへのポインタ
	IImgLayer_Ptr m_pBottomChildLayer;	//
	std::string m_name;			//layer name

	bool m_IsInited;

#ifdef _WIN32
	CRITICAL_SECTION m_csUpdateDataQueue;
	CRITICAL_SECTION m_csEditNodeQueue;
#else
	unsigned int m_LockResourceID;
#endif
};

#endif //_IIMGLAYER_H_