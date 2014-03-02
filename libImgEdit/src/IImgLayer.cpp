#include "stdafx.h"

#include "IImgLayer.h"
#include "ImgFile.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

//_EXPORT void ReleaseIEIImgLayer(IImgLayer_Ptr* ppLayer)
//{
//	if (*ppLayer) {
//		delete (*ppLayer);
//		(*ppLayer) = NULL;
//	}
//}

IImgLayer::IImgLayer(ImgFile_weakPtr parent_file)
{
	m_Alpha = 1.0;
	m_IsInited = false;
	m_IsVisible = true;
	m_name = "";
	m_pBottomChildLayer = NULL;
	m_pParentFile = parent_file;

	m_LayerRect.top = m_LayerRect.left = 0;
	m_LayerRect.bottom = m_LayerRect.right = 0;

	m_Img = NULL;

	m_pUpdateDataForeGrnd_List = NULL;
	m_pUpdateDataBackGrnd_List = NULL;

#ifdef _WIN32
	::InitializeCriticalSection( &m_csUpdateDataQueue );
	::InitializeCriticalSection( &m_csEditNodeQueue );
#else
	m_LockResourceID = 0;
#endif
}

IImgLayer::~IImgLayer()
{
	if(m_IsInited){
		this->End();
	}

#ifdef _WIN32
	::DeleteCriticalSection( &m_csUpdateDataQueue );
	::DeleteCriticalSection( &m_csEditNodeQueue );
#else
	m_LockResourceID = 0;
#endif
}

void IImgLayer::Init(const LPRECT lprc)
{
	assert(m_IsInited == false);

	if (m_IsInited == false) {
		//create new update data queue
		ResourceLock( IE_LAYER_RESOURCE_ID::UPDATE_DATA_QUEUE );
		if(m_pUpdateDataForeGrnd_List){
			delete m_pUpdateDataForeGrnd_List;
			m_pUpdateDataForeGrnd_List = NULL;
		}
		if(m_pUpdateDataBackGrnd_List){
			delete m_pUpdateDataBackGrnd_List;
			m_pUpdateDataBackGrnd_List = NULL;
		}
		m_pUpdateDataForeGrnd_List = new LPUpdateData_List;
		m_pUpdateDataBackGrnd_List = new LPUpdateData_List;
		ResourceUnlock( IE_LAYER_RESOURCE_ID::UPDATE_DATA_QUEUE );

		m_LayerRect = (*lprc);

		if(m_Img){
			delete m_Img;
			m_Img = NULL;
		}
		m_Img = new IplImageExt();
		m_Img->Init(
			lprc->right - lprc->left,
			lprc->bottom - lprc->top,
			IPL_DEPTH_8U,
			4);

		m_IsInited = true;
	}
}

void IImgLayer::End()
{
	assert(m_IsInited == true);

	if (m_IsInited) {
		//delete update data queue
		ResourceLock( IE_LAYER_RESOURCE_ID::UPDATE_DATA_QUEUE );
		if(m_pUpdateDataForeGrnd_List){
			delete m_pUpdateDataForeGrnd_List;
			m_pUpdateDataForeGrnd_List = NULL;
		}
		if(m_pUpdateDataBackGrnd_List){
			delete m_pUpdateDataBackGrnd_List;
			m_pUpdateDataBackGrnd_List = NULL;
		}
		ResourceUnlock( IE_LAYER_RESOURCE_ID::UPDATE_DATA_QUEUE );

		//release image
		if(m_Img){
			delete m_Img;
			m_Img = NULL;
		}

		m_IsInited = false;
	}
}

////////////////////////////////////
/*!
	@param[in] enable_lprc
	@param[in] updated_lprc
*/
bool IImgLayer::Update(const LPRECT enable_lprc, LPRECT updated_lprc)
{
	//ここでメモリアクセス違反が起きてる？
	//call event
	ImgLayerEventListener_List::iterator el_itr = m_ImgLayerEventListener_List.begin();
	for(; el_itr != m_ImgLayerEventListener_List.end(); el_itr++){
		if((*el_itr)->IsLockLayerEvent() == false && (*el_itr)->IsCalledImgLayer() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*el_itr)->SetCalledImgLayer(true);
			//
			(*el_itr)->OnUpdateLayer();
		}

		(*el_itr)->SetCalledImgLayer(false);
	}
	return true;
}

void IImgLayer::PushUpdateData(LPUPDATE_DATA data)
{
	ResourceLock( IE_LAYER_RESOURCE_ID::UPDATE_DATA_QUEUE );
	m_pUpdateDataBackGrnd_List->push_back( data );
	ResourceUnlock( IE_LAYER_RESOURCE_ID::UPDATE_DATA_QUEUE );

	//call event
	ImgLayerEventListener_List::iterator el_itr = m_ImgLayerEventListener_List.begin();
	for(; el_itr != m_ImgLayerEventListener_List.end(); el_itr++){
		if((*el_itr)->IsLockLayerEvent() == false && (*el_itr)->IsCalledImgLayer() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*el_itr)->SetCalledImgLayer(true);
			//
			(*el_itr)->OnImgLayerPushUpdateData(data);
		}

		(*el_itr)->SetCalledImgLayer(false);
	}
}

LPUPDATE_DATA IImgLayer::CreateUpdateData()
{
	LPUPDATE_DATA pData = new UPDATE_DATA;
	pData->update_flag = UPDATE_FLAG::UPDATE_LAYER_IMAGE;
	return pData;
}

void IImgLayer::ClearUpdateData()
{
	ResourceLock( IE_LAYER_RESOURCE_ID::UPDATE_DATA_QUEUE );

	LPUpdateData_List::iterator itr = m_pUpdateDataForeGrnd_List->begin();
	for(; itr != m_pUpdateDataForeGrnd_List->end(); itr++){
		if(*itr){
			delete (*itr);
		}
	}
	m_pUpdateDataForeGrnd_List->clear();

	ResourceUnlock( IE_LAYER_RESOURCE_ID::UPDATE_DATA_QUEUE );
}

void IImgLayer::LockUpdateData()
{
	ResourceLock( IE_LAYER_RESOURCE_ID::UPDATE_DATA_QUEUE );

	LPUpdateData_List* utmp = m_pUpdateDataForeGrnd_List;
	m_pUpdateDataForeGrnd_List = m_pUpdateDataBackGrnd_List;
	m_pUpdateDataBackGrnd_List = utmp;

	ResourceUnlock( IE_LAYER_RESOURCE_ID::UPDATE_DATA_QUEUE );
}

void IImgLayer::UnlockUpdateData()
{
}

/////////////////////////////////
/*!
	regist event listener
	@param[in] pListener  to regist event listener
*/
void IImgLayer::AddEventLilstener(ImgLayerEventListener*  pListener){
	m_ImgLayerEventListener_List.push_back( pListener );
}

//////////////////////////////////
/*!
	delete event listener
	@param[in] pListener to delete event listener
*/
void IImgLayer::DeleteEventListener(ImgLayerEventListener*  pListener)
{
	ImgLayerEventListener_List::iterator itr = m_ImgLayerEventListener_List.begin();
	for(; itr != m_ImgLayerEventListener_List.end(); itr++){
		if((*itr) == pListener){
			m_ImgLayerEventListener_List.erase(itr);
			return;
		}
	}
}

void IImgLayer::SetName(const char* layer_name)
{
	if (m_name != layer_name) {
		m_name = layer_name;

		//send event
		ImgLayerEventListener_List::iterator el_itr = m_ImgLayerEventListener_List.begin();
		for(; el_itr != m_ImgLayerEventListener_List.end(); el_itr++){
			if((*el_itr)->IsLockLayerEvent() == false && (*el_itr)->IsCalledImgLayer() == false){
				//無限にイベント呼び出しが続かないようにフラグを立てる
				(*el_itr)->SetCalledImgLayer(true);
				//
				(*el_itr)->OnChangedLayerName(layer_name);
			}

			(*el_itr)->SetCalledImgLayer(false);
		}
	}
}

int IImgLayer::GetNameSize() const
{
	if(m_name.empty()){
		return 0;
	}
	else{
		return m_name.size();
	}
}

void IImgLayer::GetName(char* layer_name) const
{
	strcpy(layer_name, m_name.c_str());
}

void IImgLayer::SetOpacity(double opacity)
{
	assert(0.0 <= opacity && opacity <= 1.0);
	if (0.0 <= opacity && opacity <= 1.0) {
		if (m_Alpha != opacity) {
			m_Alpha = opacity;

			//send event
			ImgLayerEventListener_List::iterator el_itr = m_ImgLayerEventListener_List.begin();
			for(; el_itr != m_ImgLayerEventListener_List.end(); el_itr++){
				if((*el_itr)->IsLockLayerEvent() == false && (*el_itr)->IsCalledImgLayer() == false){
					//無限にイベント呼び出しが続かないようにフラグを立てる
					(*el_itr)->SetCalledImgLayer(true);
					//
					(*el_itr)->OnChangedLayerOpacity(this->GetOpacity());
				}

				(*el_itr)->SetCalledImgLayer(false);
			}
		}
	}
}

void IImgLayer::SetLayerSynthFourCC(uint32_t fourcc)
{
	if (m_LayerSynthFourCC != fourcc) {
		m_LayerSynthFourCC = fourcc;

		//send event
		ImgLayerEventListener_List::iterator el_itr = m_ImgLayerEventListener_List.begin();
		for(; el_itr != m_ImgLayerEventListener_List.end(); el_itr++){
			if((*el_itr)->IsLockLayerEvent() == false && (*el_itr)->IsCalledImgLayer() == false){
				//無限にイベント呼び出しが続かないようにフラグを立てる
				(*el_itr)->SetCalledImgLayer(true);
				//
				(*el_itr)->OnChangedLayerSynthFourCC(m_LayerSynthFourCC);
			}

			(*el_itr)->SetCalledImgLayer(false);
		}
	}
}

//////////////////////////////
/*!
	フレーム内に不足分がなくなるようにレイヤーイメージを拡張する。
*/
void IImgLayer::ExtendLayer()
{
	{//update layer data
		RECT updated_rc;
		LockUpdateData();
		Update(NULL, &updated_rc);
		ClearUpdateData();
		UnlockUpdateData();
	}

	unsigned int extend_code;

	//左上方向
	extend_code = 0;
	int extend_left = m_LayerRect.left;
	int extend_top = m_LayerRect.top;
	if(extend_left > 0){
		extend_code |= TO_LEFT;
		m_LayerRect.left = 0;
	}
	if(extend_top > 0){
		extend_code |= TO_TOP;
		m_LayerRect.top = 0;
	}
	m_Img->ExtendImage(extend_left, extend_top, extend_code);

	//右下方向
	extend_code = 0;
	CvSize frame_size = GetParentFile().lock()->GetImgSize();
	int extend_right =  frame_size.width - m_LayerRect.right;
	int extend_bottom = frame_size.height - m_LayerRect.bottom;
	if(extend_right > 0){
		extend_code |= TO_RIGHT;
		m_LayerRect.right = frame_size.width;
	}
	if(extend_bottom > 0){
		extend_code |= TO_BOTTOM;
		m_LayerRect.bottom = frame_size.height;
	}
	m_Img->ExtendImage(extend_right, extend_bottom, extend_code);
}

void IImgLayer::CopyLayerImage(
	IplImage* dst,
	int dst_startX,
	int dst_startY,
	int src_startX,
	int src_startY,
	int width,
	int height,
	const IplImage* mask) const
{
	m_Img->GetRangeIplImage(
		dst,
		dst_startX,
		dst_startY,
		width,
		height,
		src_startX - m_LayerRect.left,
		src_startY - m_LayerRect.top,
		mask);
}


void IImgLayer::CopyLayerImage(
	IplImageExt* dst,
	int dst_startX,
	int dst_startY,
	int src_startX,
	int src_startY,
	int width,
	int height,
	const IplImage* mask,
	int mask_startX,
	int mask_startY) const
{
	assert(dst->GetImgNChannel() == 4);

	dst->ImgBlt(
		dst_startX,
		dst_startY,
		width,
		height,
		m_Img,
		src_startX - m_LayerRect.left,
		src_startY - m_LayerRect.top,
		IPLEXT_RASTER_CODE::COPY,
		mask,
		mask_startX,
		mask_startY);
}

//void IImgLayer::BltLineSynthPixel(
//	UCvPixel* dst,
//	int start_x,
//	int start_y,
//	int width) const
//{
//	if(m_Alpha == 0.0) return;
//
//	int x;
//	UCvPixel* over_pixel;
//
//	int i;
//	int next_x;
//	int block_width;
//	for(x=0; x<width; x++){
//
//		next_x = m_Img->GetNextBlockPosX(x + start_x - m_LayerRect.left);
//		block_width = next_x - (x + start_x - m_LayerRect.left) + 1;
//
//		over_pixel = m_Img->GetPixelAddr32(x + start_x - m_LayerRect.left, start_y - m_LayerRect.top);
//		if(over_pixel){
//			if((x + block_width) >= width){
//				block_width = width - x;
//			}		
//			for(i=0; i<block_width; i++){
//				m_pLayerSynth->Synth(dst, over_pixel, over_pixel->a * m_Alpha);
//				over_pixel++;
//				dst++;
//			}
//		}
//		else{
//			dst += block_width;
//		}
//
//		x += block_width-1;
//	}
//}

////////////////////////////////////////////////////
/*!
	@param[out] dst bltted image
	@param[in] d_x dst image start x (image axis)
	@param[in] d_y dst image start y (image axis)
	@param[in] s_x src image start x (frame axis)
	@param[in] s_y src image start y (frame axis)
	@param[in] width blt width
*/
//void IImgLayer::BltLineSynthPixel(
//	IplImageExt* dst,
//	int d_x,
//	int d_y,
//	int s_x,
//	int s_y,
//	int width) const
//{
//	if(m_Alpha == 0.0) return;
//
//	int i,j,k;
//	int out_next_x;
//	int out_block_width;
//	int over_next_x;
//	int over_block_width;
//	UCvPixel* out_pixel;
//	UCvPixel* over_pixel;
//
//	for(k=0; k<width; k++){
//		out_next_x = dst->GetNextBlockPosX(k + d_x);
//		out_block_width = out_next_x - (k + d_x) +1;
//
//		out_pixel = dst->GetPixelAddr32(k + d_x, d_y);
//		if((k + out_block_width) >= width){
//			out_block_width = width - k;
//		}
//
//		for(j=0; j<out_block_width; j++){
//			over_next_x = m_Img->GetNextBlockPosX(k + j + s_x - m_LayerRect.left);
//			over_block_width = over_next_x - (k + j + s_x - m_LayerRect.left) + 1;
//
//			over_pixel = m_Img->GetPixelAddr32(k + j + s_x - m_LayerRect.left, s_y - m_LayerRect.top);
//			if(over_pixel){
//				if(out_pixel == NULL){
//					dst->CreateBlockFromPos(k + d_x, d_y);
//					out_pixel = dst->GetPixelAddr32(k + d_x, d_y);
//				}
//
//				for(i=0; i<over_block_width; i++){
//					m_pLayerSynth->Synth(out_pixel, over_pixel, over_pixel->a * m_Alpha);
//					over_pixel++;
//					out_pixel++;
//				}
//			}
//			else{
//				out_pixel += over_block_width;
//			}
//			j += over_block_width-1;
//		}
//		k += out_block_width-1;
//	}
//}

////////////////////////////////////////////////////
/*
	@param[out] dst bltted image
	@param[in] d_x dst image start x (image axis)
	@param[in] d_y dst image start y (image axis)
	@param[in] s_x src image start x (frame axis)
	@param[in] s_y src image start y (frame axis)
	@param[in] width blt width
	@param[in] height blt height
*/
void IImgLayer::BltRectSynthPixel(
	IplImage* dst,
	int d_x,
	int d_y,
	int s_x,
	int s_y,
	int width,
	int height) const
{
	if(m_Alpha == 0.0) return;

	LayerSynthesizerMG::BltSynth(
		m_LayerSynthFourCC,
		m_Alpha,
		dst,
		d_x, d_y,
		width, height,
		m_Img,
		s_x - m_LayerRect.left,
		s_y - m_LayerRect.top);
}

////////////////////////////////////////////////////
/*
	@param[out] dst bltted image
	@param[in] d_x dst image start x (image axis)
	@param[in] d_y dst image start y (image axis)
	@param[in] s_x src image start x (frame axis)
	@param[in] s_y src image start y (frame axis)
	@param[in] width blt width
	@param[in] height blt height
*/
void IImgLayer::BltRectSynthPixel(
	IplImageExt* dst,
	int d_x,
	int d_y,
	int s_x,
	int s_y,
	int width,
	int height) const
{
	if (m_Alpha == 0.0)
		return;

	LayerSynthesizerMG::BltSynth(
		m_LayerSynthFourCC,
		m_Alpha,
		dst,
		d_x, d_y,
		width, height,
		m_Img,
		s_x - m_LayerRect.left,
		s_y - m_LayerRect.top);
}
