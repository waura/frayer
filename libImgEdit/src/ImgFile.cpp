#include "stdafx.h"

#include "ImgFile.h"

#include "ImgLayer.h"
#include "ImgLayerGroup.h"
#include "ImgFileEventListener.h"
#include "LayerSynthesizerMG.h"
#include "ImgFileHandle.h"
#include "ImgFileHandleMG.h"
#include "OutputError.h"
#include "IETimer.h"


#define DEG2RAD(x) ((x/(2.0*3.14159265))*360.0)

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


const int IE_NUM_OF_FILE_SIZE_TIC = 23;
int IE_FILE_SIZE_TIC [IE_NUM_OF_FILE_SIZE_TIC] = {
	1,
	2,
	3,
	4,
	6,
	8,
	12,
	16,
	25,
	33,
	50,
	66,
	100,
	150,
	200,
	300,
	400,
	600,
	800,
	1200,
	1600,
	2400,
	3200
};

_EXPORTCPP ImgFile_Ptr CreateIEImgFile()
{
	return ImgFile_Ptr(new ImgFile());
}

//_EXPORT void ReleaseIEImgFile(ImgFile_Ptr* ppFile)
//{
//	if (*ppFile) {
//		delete (*ppFile);
//		(*ppFile) = NULL;
//	}
//}

static void AddARGB(int* b, int* g, int*r, UCvPixel* add_color, int m)
{
	*b += add_color->b * m;
	*g += add_color->g * m;
	*r += add_color->r * m;
}

//16:16の固定小数点演算
//img
static void GetPixelAAFF(IplImage* img, int fixx, int fixy, int idu, int idv, UCvPixel *dst)
{
	//ピクセル小数点位置
	int fx = (fixx >> 8) & 0xFF;
	int fy = (fixy >> 8) & 0xFF;

	//占有率
	int f[4];
	f[0] = (255 - fx) * (255 - fy) >> 8;
	f[1] = fx * (255 - fy) >> 8;
	f[2] = (255 - fx) * fy >> 8;
	f[3] = fx * fy >> 8;

	//取得するべきピクセル位置
	int r,g,b;
	r=g=b=0;

	UCvPixel add_pixel;
	//4点を占有率でまぜる
	int sx = fixx >> 16;
	int sy = fixy >> 16;
	//クリッピング
	if(sx < 0) sx = 0;
	if(sy < 0) sy = 0;
	if(sx >= img->width) sx = img->width-1;
	if(sy >= img->height) sy = img->height-1;
	//
	GetPixelFromBGRA(img, sx, sy, &add_pixel);
	AddARGB(&b, &g, &r, &add_pixel, f[0]);

	sx = (fixx + idu) >> 16;
	//クリッピング
	if(sx < 0) sx = 0;
	if(sy < 0) sy = 0;
	if(sx >= img->width) sx = img->width-1;
	if(sy >= img->height) sy = img->height-1;
	//
	GetPixelFromBGRA(img, sx, sy, &add_pixel);
	AddARGB(&b, &g, &r, &add_pixel, f[1]);

	sy = (fixy + idv) >> 16;
	//クリッピング
	if(sx < 0) sx = 0;
	if(sy < 0) sy = 0;
	if(sx >= img->width) sx = img->width-1;
	if(sy >= img->height) sy = img->height-1;
	//
	GetPixelFromBGRA(img, sx, sy, &add_pixel);
	AddARGB(&b, &g, &r, &add_pixel, f[3]);

	sx = fixx >> 16;
	//クリッピング
	if(sx < 0) sx = 0;
	if(sy < 0) sy = 0;
	if(sx >= img->width) sx = img->width-1;
	if(sy >= img->height) sy = img->height-1;
	//
	GetPixelFromBGRA(img, sx, sy, &add_pixel);
	AddARGB(&b, &g, &r, &add_pixel, f[2]);

	dst->b = b >> 8;
	dst->g = g >> 8;
	dst->r = r >> 8;
	dst->a = 0;
}

static void GetPixel2x2(IplImage* img, int x, int y, UCvPixel *dst)
{
	//取得するべきピクセル位置
	int r,g,b;
	r=g=b=0;

	UCvPixel* add_pixel;

	int sx,sy;
	int i,j;
	for(j=0; j<2; j++){
		sy = y+j;
		if(sy >= img->height) sy = img->height-1;

		add_pixel = GetPixelAddress32(img, x, sy);
		for(i=0; i<2; i++){
			sx = x+i;

			b += add_pixel->b;
			g += add_pixel->g;
			r += add_pixel->r;

			if(sx >= img->width) continue;
			add_pixel++;
		}
	}

	dst->b = b >> 2;
	dst->g = g >> 2;
	dst->r = r >> 2;
	dst->a = 255;
}

static void GetPixel4x4(IplImage* img, int x, int y, UCvPixel *dst)
{
	//取得するべきピクセル位置
	int r,g,b;
	r=g=b=0;

	UCvPixel* add_pixel;

	int sx,sy;
	int i,j;
	for(j=0; j<4; j++){
		sy = y+j;
		if(sy >= img->height) sy = img->height-1;

		add_pixel = GetPixelAddress32(img, x, sy);
		for(i=0; i<4; i++){
			sx = x+i;

			b += add_pixel->b;
			g += add_pixel->g;
			r += add_pixel->r;

			if(sx >= img->width) continue;
			add_pixel++;
		}
	}

	dst->b = b >> 4;
	dst->g = g >> 4;
	dst->r = r >> 4;
	dst->a = 255;
}

ImgFile::ImgFile()
{
	isInit = false;
	m_MaskDrawType = IE_MASK_DRAW_ID::MASK_DOT_LINE_DRAW;
	m_DrawImg      = NULL;
	m_DrawMaskImg  = NULL;
	m_DisplayImg   = NULL;
	m_TransformMat = NULL;
	m_BackGroundColor.value = 0xff000000;
	m_NumOfHistory = 100;
	m_path = NULL;

	m_isViewFlipH = false;
	m_isViewFlipV = false;

	m_pUpdateDataForeGrnd_List = new LPUpdateData_List;
	m_pUpdateDataBackGrnd_List = new LPUpdateData_List;

#ifdef _WIN32
	//::InitializeCriticalSection( &m_csDrawImg );
	//::InitializeCriticalSection( &m_csDrawMaskImg );
	//::InitializeCriticalSection( &m_csDisplayImg );
	//::InitializeCriticalSection( &m_csUpdateData );
	//::InitializeCriticalSection( &m_csIELayer );
	//::InitializeCriticalSection( &m_csIEMask );
	//::InitializeCriticalSection( &m_csIEEventListener );
	m_csDrawImg = ::CreateMutex(NULL, FALSE, NULL);
	m_csDrawMaskImg = ::CreateMutex(NULL, FALSE, NULL);
	m_csDisplayImg = ::CreateMutex(NULL, FALSE, NULL);
	m_csUpdateData= ::CreateMutex(NULL, FALSE, NULL);
	m_csIELayer = ::CreateMutex(NULL, FALSE, NULL);
	m_csIEMask = ::CreateMutex(NULL, FALSE, NULL);
	m_csIEEventListener = ::CreateMutex(NULL, FALSE, NULL);
#else
	m_LockResourceID = 0;
#endif //_WIN32
}

ImgFile::~ImgFile()
{
	assert( _CrtCheckMemory() );

	if (isInit) {
		Close();
	}

	if (m_path) {
		delete[] m_path;
		m_path = NULL;
	}

#ifdef _WIN32
	//::DeleteCriticalSection( &m_csDrawImg );
	//::DeleteCriticalSection( &m_csDrawMaskImg );
	//::DeleteCriticalSection( &m_csDisplayImg );
	//::DeleteCriticalSection( &m_csUpdateData );
	//::DeleteCriticalSection( &m_csIELayer );
	//::DeleteCriticalSection( &m_csIEMask );
	//::DeleteCriticalSection( &m_csIEEventListener );
	::CloseHandle(m_csDrawImg);
	::CloseHandle(m_csDrawMaskImg);
	::CloseHandle(m_csDisplayImg);
	::CloseHandle(m_csUpdateData);
	::CloseHandle(m_csIELayer);
	::CloseHandle(m_csIEMask);
	::CloseHandle(m_csIEEventListener);
#endif //_WIN32

	assert( _CrtCheckMemory() );
}

/////////////////////////////////
/*!
	イベントリスナーを登録する。
	@param[in] pListener 登録するリスナー
*/
void ImgFile::AddEventListener(ImgFileEventListener*  pListener)
{
	ResourceLock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);
	m_ImgFileEventListener_List.push_back( pListener );
	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);
}

/////////////////////////////
/*!
	イベントリスナーを削除する。
	@param[in] pListener 削除するリスナー
*/
void ImgFile::DeleteEventListener(ImgFileEventListener*  pListener)
{
	ResourceLock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);

	ImgFileEventListener_List::iterator itr = m_ImgFileEventListener_List.begin();
	for(; itr != m_ImgFileEventListener_List.end(); itr++){
		if((*itr) == pListener){
			m_ImgFileEventListener_List.erase(itr);
			break;
		}
	}

	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);
}


bool ImgFile::Init(CvSize size)
{
	assert( !isInit );
	isInit = true;

	//
	m_pRootLayerGroup = CreateIEImgLayerGroup( this->shared_from_this() );	
	m_pMask = ImgMask_Ptr(new ImgMask( this->shared_from_this() ));

	//表示変換行列
	//m_TransformMat = cvCreateMat(2, 3, CV_64FC1);
	m_TransformMat = cvCreateMat(3, 3, CV_64FC1);
	//init matrix
	cvmSet(m_TransformMat, 0, 0, 1.0);
	cvmSet(m_TransformMat, 0, 1, 0.0);
	cvmSet(m_TransformMat, 0, 2, 0.0);
	cvmSet(m_TransformMat, 1, 0, 0.0);
	cvmSet(m_TransformMat, 1, 1, 1.0);
	cvmSet(m_TransformMat, 1, 2, 0.0);

	//表示倍率100%
	SetViewSize( 100 );
	//表示回転角0.0
	SetViewRot( 0.0 );
	//表示開始位置
	CvPoint2D64f pos;
	pos.x = pos.y = 0.0;
	SetViewPos( &pos );

	ResourceLock( IE_FILE_RESOURCE_ID::IE_LAYER );
	RECT rc;
	rc.left = rc.top = 0;
	rc.right = size.width;
	rc.bottom = size.height;
	m_pRootLayerGroup->Init( &rc );
	m_pRootLayerGroup->SetOpen( true );
	m_pRootLayerGroup->SetParentLayer( NULL );
	m_pRootLayerGroup->SetUnderLayer( NULL );
	m_pRootLayerGroup->SetOverLayer( NULL );
	m_pRootLayerGroup->SetLayerSynthFourCC( LayerSynthesizerMG::GetDefaultLayerSynthFourCC() );
	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_LAYER );

	ResourceLock( IE_FILE_RESOURCE_ID::IE_MASK );
	m_pMask->Init( &rc );
	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_MASK );

	//
	InitCanvasSize(size);

	return true;
}

bool ImgFile::InitCanvasSize(CvSize size)
{
	m_size = size;
	//
	ResourceLock(IE_FILE_RESOURCE_ID::IE_DRAW_IMG);
	if(m_DrawImg){
		cvReleaseImage(&m_DrawImg);
	}
	m_DrawImg = cvCreateImage(size, IPL_DEPTH_8U, 4);
	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_DRAW_IMG);

	ResourceLock(IE_FILE_RESOURCE_ID::IE_DRAW_MASK_IMG);
	if(m_DrawMaskImg){
		cvReleaseImage(&m_DrawMaskImg);
	}
	m_DrawMaskImg = cvCreateImage(size, IPL_DEPTH_8U, 1);
	cvSet(m_DrawMaskImg, cvScalar(0));
	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_DRAW_MASK_IMG);

	return true;
}

void ImgFile::Close()
{
	isInit = false;

	//
	ResourceLock( IE_FILE_RESOURCE_ID::IE_LAYER );
	UnsetSelectLayer();
	//if (m_pRootLayerGroup) {
	//	ReleaseIEImgLayerGroup(&m_pRootLayerGroup);
	//}
	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_LAYER );

	////
	//ResourceLock( IE_FILE_RESOURCE_ID::IE_MASK );
	//if (m_pMask) {
	//	delete m_pMask;
	//	m_pMask = NULL;
	//}
	//ResourceUnlock( IE_FILE_RESOURCE_ID::IE_MASK );

	ImgFileHandle_List::reverse_iterator ritr_u = m_UndoHandle_List.rbegin();
	for(; ritr_u != m_UndoHandle_List.rend(); ritr_u++){
		ImgFileHandleMG::ReleaseImgFileHandle( (*ritr_u) );
	}

	ImgFileHandle_List::reverse_iterator ritr_r = m_RedoHandle_List.rbegin();
	for(;ritr_r != m_RedoHandle_List.rend(); ritr_r++){
		ImgFileHandleMG::ReleaseImgFileHandle( (*ritr_r) );
	}

	ResourceLock(IE_FILE_RESOURCE_ID::IE_DRAW_IMG);
	if(m_DrawImg){
		cvReleaseImage(&m_DrawImg);
	}
	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_DRAW_IMG);

	ResourceLock(IE_FILE_RESOURCE_ID::IE_DRAW_MASK_IMG);
	if(m_DrawMaskImg){
		cvReleaseImage(&m_DrawMaskImg);
	}
	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_DRAW_MASK_IMG);

	ResourceLock(IE_FILE_RESOURCE_ID::IE_DISPLAY_IMG);
	if(m_DisplayImg){
		cvReleaseImage(&m_DisplayImg);
	}
	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_DISPLAY_IMG);

	if (m_TransformMat) {
		cvReleaseMat(&m_TransformMat);
	}

	ResourceLock( IE_FILE_RESOURCE_ID::IE_UPDATE_DATA );
	if (m_pUpdateDataForeGrnd_List) {
		delete m_pUpdateDataForeGrnd_List;
		m_pUpdateDataForeGrnd_List = NULL;
	}
	if (m_pUpdateDataBackGrnd_List) {
		delete m_pUpdateDataBackGrnd_List;
		m_pUpdateDataBackGrnd_List;
	}
	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_UPDATE_DATA );
}

//////////////////////////////////////
/*!
	ヒストリ数を設定する。
	@param[in] num ヒストリ数
*/
void ImgFile::SetNumOfHistory(unsigned int num)
{
	assert(num >= 0);

	m_NumOfHistory = num;
	
	int remove_num = m_UndoHandle_List.size() - num;
	if(remove_num > 0){
		//remove handles
		ImgFileHandle_List::iterator itr = m_UndoHandle_List.begin();
		for(int i=0; i < remove_num; i++){
			ImgFileHandleMG::ReleaseImgFileHandle(*itr);
			itr = m_UndoHandle_List.erase( itr );
		}

		//call event
		ResourceLock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);

		ImgFileEventListener_List::iterator el_itr = m_ImgFileEventListener_List.begin();
		for(; el_itr != m_ImgFileEventListener_List.end(); el_itr++){
			if((*el_itr)->IsLockFileEvent() == false && (*el_itr)->IsCalledImgFile() == false){
				//無限にイベント呼び出しが続かないようにフラグを立てる
				(*el_itr)->SetCalledImgFile(true);
				(*el_itr)->OnRemoveOldUndoImgFileHandle(remove_num);
			}
			(*el_itr)->SetCalledImgFile(false);
		}

		ResourceUnlock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);
	}
}

void ImgFile::SetPath(const char* path)
{
	if (m_path) {
		delete[] m_path;
		m_path = NULL;
	}

	int len = strlen(path) + 1;
	m_path = new char[ len ];
	strcpy_s(m_path, len*sizeof(char), path);
}

void ImgFile::GetDrawWndClientRect(LPRECT lrect) const
{
	*lrect = m_ClientRect;
}

bool ImgFile::DoImgFileHandle(ImgFileHandle* pHandle)
{
	bool result = pHandle->Do(this->shared_from_this());
	pHandle->isValid = result;

	bool isRemoveUndoHandle = false;
	if (m_NumOfHistory <= m_UndoHandle_List.size()) {
		//erase front handle
		ImgFileHandle_List::iterator itr = m_UndoHandle_List.begin();
		ImgFileHandle* handle = (*itr);
		m_UndoHandle_List.erase( itr );
		ImgFileHandleMG::ReleaseImgFileHandle( handle );
		isRemoveUndoHandle = true;
	}			
	m_UndoHandle_List.push_back( pHandle );

	//RedoListに中身が残っていればすべて消去
	if (!m_RedoHandle_List.empty()) {
		ImgFileHandle_List::iterator itr_r = m_RedoHandle_List.begin();
		for(; itr_r != m_RedoHandle_List.end(); itr_r++){
			ImgFileHandleMG::ReleaseImgFileHandle(*itr_r);
		}
		m_RedoHandle_List.clear();
	}

	//call event
	ResourceLock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);

	ImgFileEventListener_List::iterator el_itr = m_ImgFileEventListener_List.begin();
	for (; el_itr != m_ImgFileEventListener_List.end(); el_itr++) {
		if ((*el_itr)->IsLockFileEvent() == false && (*el_itr)->IsCalledImgFile() == false) {
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*el_itr)->SetCalledImgFile(true);
			if (isRemoveUndoHandle) {
				(*el_itr)->OnRemoveOldUndoImgFileHandle(1);
			}
			//
			(*el_itr)->OnAddUndoImgFileHandle(pHandle);
		}

		(*el_itr)->SetCalledImgFile(false);
	}

	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);

	return result;
}

void ImgFile::Undo(int i)
{
	if (i <= 0) return;
	if (m_UndoHandle_List.size() < i) {
		return;
	}

	ImgFileHandle_List::iterator itr = m_UndoHandle_List.end();
	itr--;

	ImgFileHandle* pHandle = (*itr);
	if (pHandle && pHandle->isValid) {
		pHandle->Undo( this->shared_from_this() );
	}

	m_RedoHandle_List.push_back(*itr);
	m_UndoHandle_List.erase(itr);

	//イベント通知
	ResourceLock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);

	ImgFileEventListener_List::iterator el_itr = m_ImgFileEventListener_List.begin();
	for (; el_itr != m_ImgFileEventListener_List.end(); el_itr++) {
		if ((*el_itr)->IsLockFileEvent() == false && (*el_itr)->IsCalledImgFile() == false) {
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*el_itr)->SetCalledImgFile(true);
			//
			(*el_itr)->OnUndoImgFileHandle(pHandle);
		}

		(*el_itr)->SetCalledImgFile(false);
	}

	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);

	Undo(i-1);
}

void ImgFile::Redo(int i)
{
	if (i <= 0) return;
	if (m_RedoHandle_List.empty()) {
		return;
	}

	ImgFileHandle_List::iterator itr = m_RedoHandle_List.end();
	itr--;

	ImgFileHandle* pHandle = (*itr);
	if (pHandle && pHandle->isValid) {
		pHandle->Redo( this->shared_from_this() );
	}

	m_UndoHandle_List.push_back(*itr);
	m_RedoHandle_List.erase(itr);

	//イベント通知
	ResourceLock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);

	ImgFileEventListener_List::iterator el_itr = m_ImgFileEventListener_List.begin();
	for (; el_itr != m_ImgFileEventListener_List.end(); el_itr++) {
		if ((*el_itr)->IsLockFileEvent() == false && (*el_itr)->IsCalledImgFile() == false) {
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*el_itr)->SetCalledImgFile(true);
			//
			(*el_itr)->OnRedoImgFileHandle(pHandle);
		}

		(*el_itr)->SetCalledImgFile(false);
	}
	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);

	Redo(i - 1);
}

size_t ImgFile::GetUndoHandleSize()
{
	return m_UndoHandle_List.size();
}

const ImgFileHandle* ImgFile::GetUndoHandleAt(int index)
{
	assert((0 <= index) && (index < m_UndoHandle_List.size()));

	int i;
	ImgFileHandle_List::iterator itr = m_UndoHandle_List.begin();
	for(i=0; i<index; i++){
		itr++;
	}

	return (*itr);
}

size_t ImgFile::GetRedoHandleSize()
{
	return m_RedoHandle_List.size();
}

const ImgFileHandle* ImgFile::GetRedoHandleAt(int index)
{
	assert((0 <= index) && (index < m_RedoHandle_List.size()));

	int i;
	ImgFileHandle_List::iterator itr = m_RedoHandle_List.begin();
	for(i=0; i<index; i++){
		itr++;
	}

	return (*itr);
}

LPUPDATE_DATA ImgFile::CreateUpdateData()
{
	LPUPDATE_DATA lud = new UPDATE_DATA;
	if(lud){
		lud->update_flag = UPDATE_FLAG::UPDATE_DISPLAY_IMAGE;
	}
	return lud;
}

/////////////////////////////////////
/*!
	画像更新データを記憶
	@param[in] data 画像更新データフラグ
*/
void ImgFile::PushUpdateData(LPUPDATE_DATA data)
{
	if(data->update_flag == UPDATE_FLAG::UPDATE_MASK_IMAGE){
		assert(0);
	}
	//check update data lock
	ResourceLock( IE_FILE_RESOURCE_ID::IE_UPDATE_DATA );
	m_pUpdateDataBackGrnd_List->push_back( data );
	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_UPDATE_DATA );

	//call event
	ResourceLock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);

	ImgFileEventListener_List::iterator el_itr = m_ImgFileEventListener_List.begin();
	for(; el_itr != m_ImgFileEventListener_List.end(); el_itr++){
		if((*el_itr)->IsLockFileEvent() == false && (*el_itr)->IsCalledImgFile() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*el_itr)->SetCalledImgFile(true);
			//
			(*el_itr)->OnImgFilePushUpdateData(data);
		}

		(*el_itr)->SetCalledImgFile(false);
	}

	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);
}

//////////////////////////////////////
/*!
	画像更新データを削除
*/
void ImgFile::ClearUpdateData()
{
	//clear update data each layer
	ResourceLock( IE_FILE_RESOURCE_ID::IE_LAYER );
	m_pRootLayerGroup->ClearUpdateData();
	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_LAYER );

	//clear update data each mask layer
	ResourceLock( IE_FILE_RESOURCE_ID::IE_MASK );
	m_pMask->ClearUpdateData();
	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_MASK );
	//ImgMask_Vec::iterator itr_m = m_Mask_Vec.begin();
	//for(; itr_m != m_Mask_Vec.end(); itr_m++){
	//	(*itr_m)->ClearUpdateData();
	//}

	//clear update data 
	ResourceLock( IE_FILE_RESOURCE_ID::IE_UPDATE_DATA );

	LPUpdateData_List::iterator itr_u = m_pUpdateDataForeGrnd_List->begin();
	for(; itr_u != m_pUpdateDataForeGrnd_List->end(); itr_u++){
		if(*itr_u){
			delete (*itr_u);
		}
	}
	m_pUpdateDataForeGrnd_List->clear();

	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_UPDATE_DATA );
}

/////////////////////////////////////
/*!
	フラグがたっているもののイメージを再生成する。
*/
bool ImgFile::Update(const LPRECT enable_lprc, LPRECT updated_lprc)
{
	int i;

	UPDATE_DATA udMask;
	bool isMaskRect = false;
	udMask.isAll = false;

	UPDATE_DATA udDrawImage;
	bool isDrawImageRect = false;
	udDrawImage.isAll = false;

	UPDATE_DATA udDisplay;
	bool isDisplayRect = false;
	udDisplay.isAll = false;

	//更新情報の縮小化
	ResourceLock( IE_FILE_RESOURCE_ID::IE_UPDATE_DATA );

	LPUpdateData_List::iterator itr = m_pUpdateDataForeGrnd_List->begin();
	for(; itr != m_pUpdateDataForeGrnd_List->end(); itr++){
		bool isAll = (*itr)->isAll;

		if((*itr)->rect.top < 0) (*itr)->rect.top = 0;
		if((*itr)->rect.left < 0) (*itr)->rect.left = 0;
		if((*itr)->rect.bottom >= m_size.height) (*itr)->rect.bottom = m_size.height;
		if((*itr)->rect.right >= m_size.width) (*itr)->rect.right = m_size.width;
		
		switch((*itr)->update_flag){
			case UPDATE_FLAG::UPDATE_DISPLAY_IMAGE:
				if(isAll){//全体更新
					udDisplay.isAll = isAll;
				}
				else if(udDisplay.isAll == false){//部分更新

					if(isDisplayRect == false){//範囲の初期値が無いなら
						udDisplay.rect = (*itr)->rect;
						isDisplayRect = true;
					}
					else{//範囲がすでに存在しているなら
						OrRect(&((*itr)->rect), &(udDisplay.rect), &(udDisplay.rect));
					}
				}
				break;
			case UPDATE_FLAG::NO_UPDATE:
				break;
			default:
				OutputError::PushLog( LOG_LEVEL::_WARN, "ImgFile::Update() unknown update_flag");
				break;
		}
	}

	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_UPDATE_DATA );


	RECT img_rect;
	img_rect.left = img_rect.top = 0;
	img_rect.right = m_size.width;
	img_rect.bottom = m_size.height;

	if (udDisplay.isAll) {
		isDisplayRect = true;
		udDisplay.rect.top = udDisplay.rect.left = 0;
		udDisplay.rect.right = m_size.width;
		udDisplay.rect.bottom = m_size.height;

		ResourceLock( IE_FILE_RESOURCE_ID::IE_DISPLAY_IMG );
		cvSet(
			m_DisplayImg,
			cvScalar(
				m_BackGroundColor.b,
				m_BackGroundColor.g,
				m_BackGroundColor.r,
				m_BackGroundColor.a)
				);
		ResourceUnlock( IE_FILE_RESOURCE_ID::IE_DISPLAY_IMG );
	}

	if (enable_lprc) {
		AndRect(&(udDrawImage.rect), enable_lprc, &(udDrawImage.rect));
		AndRect(&(udMask.rect), enable_lprc, &(udMask.rect));
		AndRect(&(udDisplay.rect), enable_lprc, &(udDisplay.rect));
	}

#ifdef IMG_FILE_COUNT_TIME
	IETimer timer;
#endif
	
	ResourceLock( IE_FILE_RESOURCE_ID::IE_MASK );
	//update mask
	isMaskRect = m_pMask->Update(enable_lprc, &(udMask.rect));
	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_MASK );

	if (isMaskRect) {
		AndRect(&(udMask.rect), &img_rect, &(udMask.rect));
		if (isDisplayRect) {
			OrRect((&udMask.rect), &(udDisplay.rect), &(udDisplay.rect));
		}
		else {
			udDisplay.rect = udMask.rect;
		}
		isDisplayRect = true;
	}

	if (enable_lprc) {
		AndRect(&(udMask.rect), enable_lprc, &(udMask.rect));
	}

#ifdef IMG_FILE_COUNT_TIME
	m_UpdateMaskTime = timer.elapsed();
	timer.restart();
#endif

	if (isMaskRect) {
		UpdateMaskImage( &udMask.rect );
	}

#ifdef IMG_FILE_COUNT_TIME
	m_UpdateMaskImageTime = timer.elapsed();
	timer.restart();
#endif

	ResourceLock( IE_FILE_RESOURCE_ID::IE_LAYER );
	//update each layer
	isDrawImageRect = m_pRootLayerGroup->Update(enable_lprc, &(udDrawImage.rect));
	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_LAYER );

	if (isDrawImageRect) {
		AndRect(&(udDrawImage.rect), &img_rect, &(udDrawImage.rect));
		if (isDisplayRect) {
			OrRect(&(udDrawImage.rect), &(udDisplay.rect), &(udDisplay.rect));
		}
		else {
			udDisplay.rect = udDrawImage.rect;
		}
		isDisplayRect = true;
	}

	if (enable_lprc) {
		AndRect(&(udDrawImage.rect), enable_lprc, &(udDrawImage.rect));
	}

#ifdef IMG_FILE_COUNT_TIME
	m_UpdateLayerTime = timer.elapsed();
	timer.restart();
#endif

	if (isDrawImageRect) {
		UpdateDrawImage( &udDrawImage.rect );
	}

#ifdef IMG_FILE_COUNT_TIME
	m_UpdateDrawImageTime = timer.elapsed();
	timer.restart();
#endif


#ifdef IMG_FILE_COUNT_TIME
	timer.restart();
#endif

	if (enable_lprc) {
		AndRect(&(udDisplay.rect), enable_lprc, &(udDisplay.rect));
	}

	if (isDisplayRect || udDisplay.isAll) {
		UpdateDisplayImage( &udDisplay.rect );
	}
	else {
		//non update data
		return false;
	}

#ifdef IMG_FILE_COUNT_TIME
	m_UpdateDisplayImageTime = timer.elapsed();
	timer.restart();
#endif

	//イベント通知
	ResourceLock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);

	ImgFileEventListener_List::iterator el_itr = m_ImgFileEventListener_List.begin();
	for(; el_itr != m_ImgFileEventListener_List.end(); el_itr++){
		if((*el_itr)->IsLockFileEvent() == false && (*el_itr)->IsCalledImgFile() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*el_itr)->SetCalledImgFile(true);
			//
			(*el_itr)->OnUpdate();
		}

		(*el_itr)->SetCalledImgFile(false);
	}

	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);

	return true;
}

void ImgFile::LockUpdateData()
{
	ResourceLock( IE_FILE_RESOURCE_ID::IE_LAYER );
	m_pRootLayerGroup->LockUpdateData();
	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_LAYER );

	ResourceLock( IE_FILE_RESOURCE_ID::IE_MASK );
	m_pMask->LockUpdateData();
	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_MASK );
	//ImgMask_Vec::iterator itr_m = m_Mask_Vec.begin();
	//for(; itr_m != m_Mask_Vec.end(); itr_m++){
	//	(*itr_m)->LockUpdateData();
	//}

	ResourceLock( IE_FILE_RESOURCE_ID::IE_UPDATE_DATA );

	LPUpdateData_List* tmp = m_pUpdateDataForeGrnd_List;
	m_pUpdateDataForeGrnd_List = m_pUpdateDataBackGrnd_List;
	m_pUpdateDataBackGrnd_List = tmp;

	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_UPDATE_DATA );
}

void ImgFile::UnlockUpdateData()
{
	ResourceLock( IE_FILE_RESOURCE_ID::IE_LAYER );
	m_pRootLayerGroup->UnlockUpdateData();
	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_LAYER );

	ResourceLock( IE_FILE_RESOURCE_ID::IE_MASK );
	m_pMask->UnlockUpdateData();
	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_MASK );
	//ImgMask_Vec::iterator itr_m = m_Mask_Vec.begin();
	//for(; itr_m != m_Mask_Vec.end(); itr_m++){
	//	(*itr_m)->UnlockUpdateData();
	//}
}

///////////////////////////////////////
/*!
	出力画像をコピーする。
	@param[in] image コピー先 4ch BGRA
*/
void ImgFile::CopyFileImage(IplImage *image)
{
#ifdef _PRINT_TIME
	static char str_blt_lap[256];
	IETimer timer;
#endif

	assert( isInit );
	assert(image->nChannels == 4);

	ResourceLock( IE_FILE_RESOURCE_ID::IE_DRAW_IMG );
	cvCopy(m_DrawImg, image);
	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_DRAW_IMG );

#ifdef _PRINT_TIME
	sprintf(str_blt_lap, "ImgFile::CopyFileImage() %f msec\n", timer.elapsed());
	::OutputDebugString(str_blt_lap);
#endif
}

///////////////////////////////////////
/*!
	出力画像をコピーする。
	@param[in] image コピー先 4ch BGRA
*/
void ImgFile::CopyFileImageThumbnail(IplImage* image)
{
	ResourceLock( IE_FILE_RESOURCE_ID::IE_DRAW_IMG );

	int x,y;
	UCvPixel* out_pixel;
	UCvPixel* src_pixel;
	CvSize file_img_size = GetImgSize();

	double x_scale = (double)file_img_size.width/image->width;
	double y_scale = (double)file_img_size.height/image->height;

	double src_x, src_y;

	src_y = 0.0;
	for(y=0; y<image->height; y++){
		src_x = 0.0;
		out_pixel = GetPixelAddress32(image, 0, y);
		for(x=0; x<image->width; x++){
			src_x += x_scale;
			if((0 <= src_x) && (src_x < m_DrawImg->width) &&
				(0 <= src_y) && (src_y < m_DrawImg->height))
			{
				src_pixel = GetPixelAddress32(m_DrawImg, src_x, src_y);
				(*out_pixel) = (*src_pixel);
			}
			out_pixel++;
		}
		src_y += y_scale;
	}

	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_DRAW_IMG );
}

void ImgFile::CopyFileMask(
   int start_x,
   int start_y,
   int width,
   int height,
   IplImage* dst,
   int dst_x,
   int dst_y)
{
	assert(dst->nChannels == 1);

	ResourceLock( IE_FILE_RESOURCE_ID::IE_DRAW_MASK_IMG );
	BltCopy(
		dst,
		dst_x,
		dst_y,
		width,
		height,
		m_DrawMaskImg,
		start_x,
		start_y,
		NULL,
		0, 0);
	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_DRAW_MASK_IMG );
}

void ImgFile::CopyFileMask(
   int start_x,
   int start_y,
   int width,
   int height,
   IplImageExt* dst,
   int dst_x,
   int dst_y)
{
	assert(dst->GetImgNChannel() == 1);

	ResourceLock( IE_FILE_RESOURCE_ID::IE_DRAW_MASK_IMG );
	dst->ImgBlt(
		dst_x,
		dst_y,
		width,
		height,
		m_DrawMaskImg,
		start_x,
		start_y,
		IPLEXT_RASTER_CODE::COPY);
	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_DRAW_MASK_IMG );
}

////////////////////////////////
/*!
	表示サイズをセットする(1～800%)
	@param[in] 表示サイズ
*/
void ImgFile::SetViewSize(int size)
{
	if(m_view_size == size) return;

	m_view_size = size;

	UpdateTransformMat();

	//call event
	ResourceLock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);

	ImgFileEventListener_List::iterator itr = m_ImgFileEventListener_List.begin();
	for(; itr != m_ImgFileEventListener_List.end(); itr++){
		if((*itr)->IsLockFileEvent() == false && (*itr)->IsCalledImgFile() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledImgFile(true);
			//
			(*itr)->OnChangeViewSize( m_view_size );
		}

		(*itr)->SetCalledImgFile(false);
	}

	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);
}

/////////////////////////////////
/*!
	表示サイズをセットする(1～800%)
	@param[in] size 表示サイズ
	@param[in] lpt 中心座標
*/
void ImgFile::SetViewSize(int size, LPPOINT lpt)
{
	SetViewSize(size);
}

void ImgFile::UpViewSize()
{
	int i;
	for(i = 0; i < IE_NUM_OF_FILE_SIZE_TIC; i++){
		if(m_view_size < IE_FILE_SIZE_TIC[i]){
			SetViewSize( IE_FILE_SIZE_TIC[i] );
			return;
		}
	}
}

void ImgFile::DownViewSize()
{
	int i;
	for(i = IE_NUM_OF_FILE_SIZE_TIC-1; i >= 0; i--){
		if(IE_FILE_SIZE_TIC[i] < m_view_size){
			SetViewSize( IE_FILE_SIZE_TIC[i] );
			return;
		}
	}
}

void ImgFile::SetViewRot(double rot)
{
	if(rot < 0.0)
		rot += 360.0;
	else if(rot > 360.0)
		rot -= 360.0;

	if(m_view_rot == rot) return;

	m_view_rot = rot;
	UpdateTransformMat();

	//call event
	ResourceLock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);

	ImgFileEventListener_List::iterator itr = m_ImgFileEventListener_List.begin();
	for(; itr != m_ImgFileEventListener_List.end(); itr++){
		if((*itr)->IsLockFileEvent() == false && (*itr)->IsCalledImgFile() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledImgFile(true);
			//
			(*itr)->OnChangeViewRot( m_view_rot );
		}

		(*itr)->SetCalledImgFile(false);
	}

	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);
}

///////////////////////////////////
/*!
	表示開始位置をセット　左上原点
*/
void ImgFile::SetViewPos(const CvPoint2D64f *viewPos)
{
	if((m_view_startPt.x == viewPos->x) && (m_view_startPt.y == viewPos->y)) return;

	double dif_x = viewPos->x - m_view_startPt.x;
	double dif_y = viewPos->y - m_view_startPt.y;
	m_view_startPt.x = viewPos->x;
	m_view_startPt.y = viewPos->y;

	m_view_centerPt.x -= dif_x;
	m_view_centerPt.y -= dif_y;

	UpdateTransformMat();

	//call event
	ResourceLock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);

	ImgFileEventListener_List::iterator itr = m_ImgFileEventListener_List.begin();
	for(; itr != m_ImgFileEventListener_List.end(); itr++){
		if((*itr)->IsLockFileEvent() == false && (*itr)->IsCalledImgFile() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledImgFile(true);
			//
			(*itr)->OnChangeViewPos( viewPos );
		}

		(*itr)->SetCalledImgFile(false);
	}

	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);
}

///////////////////////////////////
/*!
	表示開始位置を返す。
*/
void ImgFile::GetViewPos(CvPoint2D64f *viewPos) const
{
	(*viewPos) = m_view_startPt;
}

CvPoint2D64f ImgFile::GetViewCenterPos()
{
	if(m_DisplayImg){
		ResourceLock( IE_DISPLAY_IMG );
		CvSize disp_size = cvGetSize( m_DisplayImg );
		ResourceUnlock( IE_DISPLAY_IMG );

		CvPoint2D64f disp;
		CvPoint2D64f ret;
		disp.x = disp_size.width/2;
		disp.y = disp_size.height/2;

		ConvertPosWndToImg(&disp, &ret);
		return ret;
	}
	else{	
		CvPoint2D64f ret;
		CvSize img_size = this->GetImgSize();
		ret.x = img_size.width/2;
		ret.y = img_size.height/2;
		return ret;
	}
}

///////////////////////////////////////
/*!
	ウィンドウサイズが変更されたとき呼び出す
	@param[in] lrect ウィンドウの範囲
*/
void ImgFile::OnWindowReSize(LPRECT lrect)
{
	m_ClientRect = *lrect;
	if((m_ClientRect.left - m_ClientRect.right) == 0 &&
		(m_ClientRect.top - m_ClientRect.bottom) == 0)
		return;

	//オフスクリーンバッファの再生成
	ReGenOffScreenBuffer(lrect);
}

CvSize ImgFile::GetDisplaySize()
{
	//check lock
	ResourceLock( IE_FILE_RESOURCE_ID::IE_DISPLAY_IMG );
	CvSize size;
	if( m_DisplayImg ){
		size = cvGetSize( m_DisplayImg );
	}
	else{
		size.width = size.height = 0;
	}
	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_DISPLAY_IMG );

	return size;
}

void ImgFile::SetViewFlipH(bool bl)
{
	m_isViewFlipH = bl;
	UpdateTransformMat();

	//call event
	ResourceLock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);

	ImgFileEventListener_List::iterator itr = m_ImgFileEventListener_List.begin();
	for(; itr != m_ImgFileEventListener_List.end(); itr++){
		if((*itr)->IsLockFileEvent() == false && (*itr)->IsCalledImgFile() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledImgFile(true);
			//
			(*itr)->OnChangeViewFlipH( m_isViewFlipH );
		}

		(*itr)->SetCalledImgFile(false);
	}

	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);
}

void ImgFile::SetViewFlipV(bool bl)
{
	m_isViewFlipV = bl;
	UpdateTransformMat();

	//call event
	ResourceLock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);

	ImgFileEventListener_List::iterator itr = m_ImgFileEventListener_List.begin();
	for(; itr != m_ImgFileEventListener_List.end(); itr++){
		if((*itr)->IsLockFileEvent() == false && (*itr)->IsCalledImgFile() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledImgFile(true);
			//
			(*itr)->OnChangeViewFlipV( m_isViewFlipV );
		}

		(*itr)->SetCalledImgFile(false);
	}

	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);
}

///////////////////////////////////////
/*
	add layer
	@param[in] pLayer 追加するレイヤーのポインタ
*/
void ImgFile::AddLayer(IImgLayer_Ptr pLayer)
{
	ResourceLock( IE_FILE_RESOURCE_ID::IE_LAYER );
	IImgLayer_Ptr pSelectLayer = m_pSelectLayer.lock();
	if(pSelectLayer){
		if(pSelectLayer->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER){
			ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(pSelectLayer);
			pLGroup->AddLayer( pLayer );
		}
		else{
			IImgLayer_Ptr _pLGroup = pSelectLayer->GetParentLayer().lock();
			ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(_pLGroup);
			int sub_index = pLGroup->GetLayerIndex( m_pSelectLayer );
			pLGroup->AddLayer(pLayer, sub_index+1);
		}
	}
	else{
		m_pRootLayerGroup->AddLayer( pLayer );
	}
	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_LAYER );

	m_pRootLayerGroup->CountNumOfChildLayers();
	m_pRootLayerGroup->CountMaxLayerIndex();

#ifdef _DEBUG
	m_pRootLayerGroup->DumpChildLayers();
#endif

	int index = m_pRootLayerGroup->GetLayerIndex( pLayer );

	//call event
	ResourceLock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);

	ImgFileEventListener_List::iterator itr_el = m_ImgFileEventListener_List.begin();
	for(; itr_el != m_ImgFileEventListener_List.end(); itr_el++){
		if((*itr_el)->IsLockFileEvent() == false && (*itr_el)->IsCalledImgFile() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr_el)->SetCalledImgFile(true);
			//
			(*itr_el)->OnAddLayer(pLayer, index);
		}

		(*itr_el)->SetCalledImgFile(false);
	}

	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);
}

///////////////////////////////////////
/*
	add layer
	@param[in] pLayer 追加するレイヤーのポインタ
	@param[in] index 追加先インデックス
*/
void ImgFile::AddLayer(IImgLayer_Ptr pLayer, int index)
{
	ResourceLock( IE_FILE_RESOURCE_ID::IE_LAYER );
	m_pRootLayerGroup->AddLayer(pLayer, index);
	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_LAYER );

	m_pRootLayerGroup->CountNumOfChildLayers();
	m_pRootLayerGroup->CountMaxLayerIndex();

#ifdef _DEBUG
	m_pRootLayerGroup->DumpChildLayers();
	int _index = m_pRootLayerGroup->GetLayerIndex( pLayer );
	if (pLayer->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER) {
		ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(pLayer);
		assert(_index == (index + pLGroup->GetNumOfChildLayers() + 1));
	} else {
		assert(_index == index);
	}
#endif

	//call event
	ResourceLock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);

	ImgFileEventListener_List::iterator itr_el = m_ImgFileEventListener_List.begin();
	for(; itr_el != m_ImgFileEventListener_List.end(); itr_el++){
		if((*itr_el)->IsLockFileEvent() == false && (*itr_el)->IsCalledImgFile() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr_el)->SetCalledImgFile(true);
			//
			(*itr_el)->OnAddLayer(pLayer, index);
		}

		(*itr_el)->SetCalledImgFile(false);
	}

	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);
}

//これ↓いるのか?
void ImgFile::AddLayer(ImgLayerGroup_Ptr pLayerGroup)
{
	ResourceLock( IE_FILE_RESOURCE_ID::IE_LAYER );
	IImgLayer_Ptr pSelectLayer = m_pSelectLayer.lock();
	if(pSelectLayer){
		if(pSelectLayer->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER){
			ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(pSelectLayer);
			pLGroup->AddLayer( pLayerGroup );
		}
		else{
			IImgLayer_Ptr _pLGroup = pSelectLayer->GetParentLayer().lock();
			ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(_pLGroup);
			int sub_index = pLGroup->GetLayerIndex( m_pSelectLayer );
			pLGroup->AddLayer(pLayerGroup, sub_index+1);
		}
	}
	else{
		m_pRootLayerGroup->AddLayer( pLayerGroup );
	}
	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_LAYER );

	m_pRootLayerGroup->CountNumOfChildLayers();
	m_pRootLayerGroup->CountMaxLayerIndex();

#ifdef _DEBUG
	m_pRootLayerGroup->DumpChildLayers();
#endif

	int group_index = m_pRootLayerGroup->GetLayerIndex( pLayerGroup );
	int close_index = group_index - (pLayerGroup->GetMaxLayerIndex() + 2);

	//call event
	ResourceLock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);

	ImgFileEventListener_List::iterator itr_el = m_ImgFileEventListener_List.begin();
	for(; itr_el != m_ImgFileEventListener_List.end(); itr_el++){
		if((*itr_el)->IsLockFileEvent() == false && (*itr_el)->IsCalledImgFile() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr_el)->SetCalledImgFile(true);
			//
			(*itr_el)->OnAddLayer(NULL, close_index);
			(*itr_el)->OnAddLayer(pLayerGroup, group_index);
		}

		(*itr_el)->SetCalledImgFile(false);
	}

	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);
}

///////////////////////////////////////
/*!
	remove layer
	@param[in,out] pLayer 取り除くレイヤーのポインタ
*/
void ImgFile::RemoveLayer(IImgLayer_Ptr pLayer)
{
	if (m_pSelectLayer.lock() == pLayer) {
		m_pSelectLayer.reset();
	}

	int index;
	int remove_num_of_layer=0;

	ResourceLock( IE_FILE_RESOURCE_ID::IE_LAYER );
	if(pLayer == m_pRootLayerGroup){
		index = m_pRootLayerGroup->GetMaxLayerIndex() + 1;
		remove_num_of_layer = m_pRootLayerGroup->GetMaxLayerIndex() + 2;
		m_pRootLayerGroup = NULL;
	}
	else{
		index = m_pRootLayerGroup->GetLayerIndex( pLayer );
		if(pLayer->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER){
			ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(pLayer);
			remove_num_of_layer = pLGroup->GetMaxLayerIndex() + 2;
		}

		ResourceLock( IE_FILE_RESOURCE_ID::IE_LAYER );
		m_pRootLayerGroup->RemoveLayer( pLayer );
		ResourceUnlock( IE_FILE_RESOURCE_ID::IE_LAYER );

		m_pRootLayerGroup->CountNumOfChildLayers();
		m_pRootLayerGroup->CountMaxLayerIndex();
	}
	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_LAYER );

#ifdef _DEBUG
	if(m_pRootLayerGroup){
		m_pRootLayerGroup->DumpChildLayers();
	}
#endif

	//call event
	ResourceLock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);

	ImgFileEventListener_List::iterator itr = m_ImgFileEventListener_List.begin();
	for(; itr != m_ImgFileEventListener_List.end(); itr++){
		if((*itr)->IsLockFileEvent() == false && (*itr)->IsCalledImgFile() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledImgFile(true);
			//
			int i;
			for(i = 0; i <= remove_num_of_layer; i++){
				(*itr)->OnDeleteLayer(pLayer, index - i);
			}
		}

		(*itr)->SetCalledImgFile(false);
	}

	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);
}

///////////////////////////////////////
/*!
	レイヤーの並びを変える
*/
void ImgFile::ChangeLayerLine(int from_index, int to_index)
{
	if(from_index == to_index) return;

	ResourceLock( IE_FILE_RESOURCE_ID::IE_LAYER );
	m_pRootLayerGroup->ChangeLayerLine(from_index, to_index);
	m_pRootLayerGroup->CountNumOfChildLayers();
	m_pRootLayerGroup->CountMaxLayerIndex();
#ifdef _DEBUG
	m_pRootLayerGroup->DumpChildLayers();
#endif
	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_LAYER );

	//call event
	ResourceLock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);

	ImgFileEventListener_List::iterator itr = m_ImgFileEventListener_List.begin();
	for(; itr != m_ImgFileEventListener_List.end(); itr++){
		if((*itr)->IsLockFileEvent() == false && (*itr)->IsCalledImgFile() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledImgFile(true);
			//
			(*itr)->OnChangeLayerLine(from_index, to_index);
		}

		(*itr)->SetCalledImgFile(false);
	}

	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);
}

int ImgFile::GetLayerSize() const
{
	int num = 0;
	if(m_pRootLayerGroup){
		num = m_pRootLayerGroup->GetNumOfChildLayers();
	}
	return num;
}

void ImgFile::SetRootLayerGroup(ImgLayerGroup_Ptr pRootLayerGroup)
{
	m_pRootLayerGroup = pRootLayerGroup;
	int index = m_pRootLayerGroup->GetMaxLayerIndex() + 1;

	//call event
	ResourceLock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);

	ImgFileEventListener_List::iterator itr_el = m_ImgFileEventListener_List.begin();
	for(; itr_el != m_ImgFileEventListener_List.end(); itr_el++){
		if((*itr_el)->IsLockFileEvent() == false && (*itr_el)->IsCalledImgFile() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr_el)->SetCalledImgFile(true);
			//
			(*itr_el)->OnAddLayer(m_pRootLayerGroup, index);
		}

		(*itr_el)->SetCalledImgFile(false);
	}

	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);
}

///////////////////////////////////////
/*!
	選択レイヤーをセットする。
	@param[in] layer 選択するレイヤー
*/
void ImgFile::SetSelectLayer(IImgLayer_weakPtr layer)
{
	if (!m_pSelectLayer.expired()) {
		if (m_pSelectLayer.lock() == layer.lock()) {
			return;
		}
	}

	m_pSelectLayer = layer;

	//call event
	ResourceLock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);

	ImgFileEventListener_List::iterator itr = m_ImgFileEventListener_List.begin();
	for(; itr != m_ImgFileEventListener_List.end(); itr++){
		if((*itr)->IsLockFileEvent() == false && (*itr)->IsCalledImgFile() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledImgFile(true);
			//
			(*itr)->OnSelectLayer(m_pSelectLayer);
		}

		(*itr)->SetCalledImgFile(false);
	}

	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_EVENTLISTENER);
}

/////////////////////////////////////////
///*!
//	マスクを追加する。
//	@param[in] pMask 追加するマスクのポインタ
//*/
//void ImgFile::AddMask(ImgMask *pMask)
//{
//	AddMask(pMask, m_Mask_Vec.size());
//}
//
//void ImgFile::AddMask(ImgMask *pMask, int index)
//{
//	assert( 0 <= index && index <= m_Mask_Vec.size() );
//
//	ImgMask_Vec::iterator itr;
//	if( index < m_Mask_Vec.size()/2 ){
//		itr = m_Mask_Vec.begin();
//		for(int i=0; i<index; i++){
//			itr++;
//		}
//	}
//	else{
//		itr = m_Mask_Vec.end();
//		for(int i=m_Mask_Vec.size(); i > index; i--){
//			itr--;
//		}
//	}
//
//	m_Mask_Vec.insert(itr, pMask);
//
//	ImgFileEventListener_List::iterator itr_el = m_ImgFileEventListener_List.begin();
//	for(; itr_el != m_ImgFileEventListener_List.end(); itr_el++){
//		if((*itr_el)->IsLockFileEvent() == false && (*itr_el)->IsCalledImgFile() == false){
//			//無限にイベント呼び出しが続かないようにフラグを立てる
//			(*itr_el)->SetCalledImgFile(true);
//			//
//			(*itr_el)->OnAddMask(pMask, index);
//		}
//
//		(*itr_el)->SetCalledImgFile(false);
//	}
//}
//
/////////////////////////////////////////
///*!
//	マスクを削除する。
//	@param[in,out] pMask 削除するマスクのポインタ
//*/
//void ImgFile::DeleteMask(ImgMask *pMask)
//{
//	int index = 0;
//	ImgMask_Vec::iterator itr_m = m_Mask_Vec.begin();
//	for(; itr_m != m_Mask_Vec.end(); itr_m++){
//		if(pMask == (*itr_m)){			
//			break;
//		}
//		index++;
//	}
//
//	//イベント通知
//	ImgFileEventListener_List::iterator itr = m_ImgFileEventListener_List.begin();
//	for(; itr != m_ImgFileEventListener_List.end(); itr++){
//		if((*itr)->IsLockFileEvent() == false && (*itr)->IsCalledImgFile() == false){
//			//無限にイベント呼び出しが続かないようにフラグを立てる
//			(*itr)->SetCalledImgFile(true);
//			//
//			(*itr)->OnDeleteMask(pMask, index);
//		}
//
//		(*itr)->SetCalledImgFile(false);
//	}
//
//	//削除
//	m_Mask_Vec.erase(itr_m);
//	delete pMask;
//}
//
//void ImgFile::DeleteMask(int index)
//{
//	int i;
//
//	ImgMask_Vec::iterator itr_m = m_Mask_Vec.begin();
//	for(i=0; i<index; itr_m++){}
//
//	//イベント通知
//	ImgFileEventListener_List::iterator itr = m_ImgFileEventListener_List.begin();
//	for(; itr != m_ImgFileEventListener_List.end(); itr++){
//		if((*itr)->IsLockFileEvent() == false && (*itr)->IsCalledImgFile() == false){
//			//無限にイベント呼び出しが続かないようにフラグを立てる
//			(*itr)->SetCalledImgFile(true);
//			//
//			(*itr)->OnDeleteMask((*itr_m), index);
//		}
//
//		(*itr)->SetCalledImgFile(false);
//	}
//
//	//削除
//	delete (*itr_m);
//	m_Mask_Vec.erase(itr_m);
//}
//
/////////////////////////////////////////
///*!
//	ファイルが保持しているマスクの数を返す
//	@return マスク枚数
//*/
//int ImgFile::GetMaskSize() const
//{
//	return m_Mask_Vec.size();
//}
//
//
/////////////////////////////////////////
///*!
//	インデックスを指定してマスクオブジェクトへのポインタを返す
//	@return マスクオブジェクトのポインタ
//*/
//ImgMask* const ImgFile::GetMaskAtIndex(int index) const
//{
//	assert(0 <= index && index < m_Mask_Vec.size());
//
//	if(0 <= index && index < m_Mask_Vec.size()){
//		return m_Mask_Vec[index];
//	}
//
//	return NULL;
//}
//
/////////////////////////////////////////
///*!
//	指定したマスクオブジェクトのインデックスを返す
//	@param[in] mask 指定するマスクへのポインタ
//	@return 指定したマスクのインデックス
//*/
//int ImgFile::GetMaskIndex(const ImgMask* mask) const
//{
//	int i;
//	for(i=0; i < m_Mask_Vec.size(); i++){
//		if(mask == m_Mask_Vec[i]){
//			return i;
//		}
//	}
//
//	OutputError::PushLog( LOG_LEVEL::_WARN, "ImgFile::GetMaskIndex() don't exist mask");
//	return -1;
//}

//int ImgFile::GetSelectMaskIndex() const
//{
//	int i;
//	size_t size = m_Mask_Vec.size();
//	for(i=0; i<size; i++){
//		if(m_Mask_Vec[i] == m_pSelectMask){
//			return i;
//		}
//	}
//
//	return -1;
//}

/////////////////////////////////////////
///*!
//	選択マスクをセットする
//	@param[in] mask 選択するマスク
//*/
//void ImgFile::SetSelectMask(ImgMask* mask)
//{
//	if(m_pSelectMask == mask) return;
//
//	m_pSelectMask = mask;
//
//	//イベント通知
//	ImgFileEventListener_List::iterator itr = m_ImgFileEventListener_List.begin();
//	for(; itr != m_ImgFileEventListener_List.end(); itr++){
//		if((*itr)->IsLockFileEvent() == false && (*itr)->IsCalledImgFile() == false){
//			//無限にイベント呼び出しが続かないようにフラグを立てる
//			(*itr)->SetCalledImgFile(true);
//			//
//			(*itr)->OnSelectMask(m_pSelectMask);
//		}
//
//		(*itr)->SetCalledImgFile(false);
//	}
//}
//
//void ImgFile::SetSelectMask(int index)
//{
//	assert((0 <= index) && (index < m_Mask_Vec.size()));
//
//	if((0 <= index) && (index < m_Mask_Vec.size())){
//		m_pSelectMask = m_Mask_Vec[index];
//
//		//イベント通知
//		ImgFileEventListener_List::iterator itr = m_ImgFileEventListener_List.begin();
//		for(; itr != m_ImgFileEventListener_List.end(); itr++){
//			if((*itr)->IsLockFileEvent() == false && (*itr)->IsCalledImgFile() == false){
//				//無限にイベント呼び出しが続かないようにフラグを立てる
//				(*itr)->SetCalledImgFile(true);
//				//
//				(*itr)->OnSelectMask(m_pSelectMask);
//			}
//
//			(*itr)->SetCalledImgFile(false);
//		}
//	}
//	else{
//		char log[256];
//		wsprintf(log, "ImgFile::SetSelectMask() invalidate mask index %d", index);
//		OutputError::PushLog(LOG_LEVEL::_WARN, log);
//	}
//}

////////////////////////////////
/*!
	選択領域の上にマウスがあるかどうか？
	@param[in] lpt マウスの入力情報
	@return マウスが選択領域の上ならtrue そうでないならfalse
*/
bool ImgFile::IsOnSelectImage(LPPOINT lpt)
{
	if(GetMaskState() == IE_MASK_STATE_ID::INVALID){ 
		//選択領域が無いときはfalse
		return false;
	}

	IE_INPUT_DATA pd;
	pd.x = lpt->x;
	pd.y = lpt->y;
	ConvertPosWndToImg(&pd);
	
	uint8_t data;
	GetMaskData(pd.x, pd.y, &data);
	if(data != 0){
		return true;
	}
	return false;
}

/////////////////////////////////////
/*!
	最終表示画像の再生成
	@param[in] lprc 更新範囲 NULLなら全範囲
*/
void ImgFile::UpdateDisplayImage(const LPRECT lprc)
{
	if(m_DisplayImg == NULL) return;

	ResourceLock( IE_FILE_RESOURCE_ID::IE_DISPLAY_IMG );

	int i;
	int x,y;

	int ver_index[4] = {1, 2, 3, 0};
	CvPoint2D64f d_ver[4];
	CvPoint2D64f dd_ver[4];
	CvPoint2D64f update_ver[4];
	d_ver[0].x = 0;				d_ver[0].y = 0;
	d_ver[1].x = m_size.width;	d_ver[1].y = 0;
	d_ver[2].x = m_size.width;	d_ver[2].y = m_size.height;
	d_ver[3].x = 0;				d_ver[3].y = m_size.height;
	ConvertPosImgToWnd(d_ver, dd_ver, 4);

	AfPoint2D64f af_ver[4];
	af_ver[0].src.x = 0;			af_ver[0].src.y = 0;
	af_ver[1].src.x = m_size.width;	af_ver[1].src.y = 0;
	af_ver[2].src.x = m_size.width;	af_ver[2].src.y = m_size.height;
	af_ver[3].src.x = 0;			af_ver[3].src.y = m_size.height;
	af_ver[0].dst = dd_ver[0];
	af_ver[1].dst = dd_ver[1];
	af_ver[2].dst = dd_ver[2];
	af_ver[3].dst = dd_ver[3];

	d_ver[0].x = lprc->left;	d_ver[0].y = lprc->top;
	d_ver[1].x = lprc->right;	d_ver[1].y = lprc->top;
	d_ver[2].x = lprc->right;	d_ver[2].y = lprc->bottom;
	d_ver[3].x = lprc->left;	d_ver[3].y = lprc->bottom;
	ConvertPosImgToWnd(d_ver, update_ver, 4);

	//find minimum maximum value
	int min_y=update_ver[0].y, max_y=update_ver[0].y;
	for (i=1; i<4; i++) {
		if (update_ver[i].y < min_y) {
			min_y = update_ver[i].y;
		}
		if (update_ver[i].y > max_y) {
			max_y = update_ver[i].y;
		}
	}
	int min_x=update_ver[0].x, max_x=update_ver[0].x;
	for (i=1; i<4; i++) {
		if (update_ver[i].x < min_x) {
			min_x = update_ver[i].x;
		}
		if (update_ver[i].x > max_x) {
			max_x = update_ver[i].x;
		}
	}

	if (min_y < 0) min_y = 0;
	if (max_y < 0) max_y = 0;
	if (min_y >= m_DisplayImg->height) min_y = m_DisplayImg->height-1;
	if (max_y >= m_DisplayImg->height) max_y = m_DisplayImg->height-1;

	if (min_x < 0) min_x = 0;
	if (max_x < 0) max_x = 0;
	if (min_x >= m_DisplayImg->width) min_x = m_DisplayImg->width-1;
	if (max_x >= m_DisplayImg->width) max_x = m_DisplayImg->width-1;


	int FF = 16;
	int FIXMUL = 0x01 << FF;
	UCvPixel* dst_pixel;

	UCvPixel mask_color;
	mask_color.b = 255;
	mask_color.g = 0;
	mask_color.r = 0;
	mask_color.a = 100;

	UCvPixel dot_black;
	dot_black.b = 0;
	dot_black.g = 0;
	dot_black.r = 0;
	dot_black.a = 255;

	UCvPixel dot_white;
	dot_white.b = 255;
	dot_white.g = 255;
	dot_white.r = 255;
	dot_white.a = 255;

	double x_, sx_, sy_;
	double edge[2], sx[2], sy[2];

	for (y=min_y; y<max_y; y++) {
		int edgenum = 0;

		//find intersection
		for (i=0; i<4; i++) {
			if (AfIntersect(&(af_ver[i]), &(af_ver[ ver_index[i] ]), y, &x_, &sx_, &sy_)) {
				edge[edgenum] = x_;
				sx[edgenum] = sx_;
				sy[edgenum] = sy_;
				edgenum++;
				if(edgenum == 2)break;
			}
		}
		if( edgenum != 2 ) continue;
		if(edge[0] == edge[1]) continue;

		if(edge[0] > edge[1]){
			double tmp;
			tmp = edge[0]; edge[0] = edge[1]; edge[1] = tmp;
			tmp = sx[0]; sx[0] = sx[1]; sx[1] = tmp;
			tmp = sy[0]; sy[0] = sy[1]; sy[1] = tmp;
		}

		double d_u = (sx[1] - sx[0]) / (edge[1] - edge[0]);
		double d_v = (sy[1] - sy[0]) / (edge[1] - edge[0]);

		//clipping
		if (edge[0] < min_x) {
			sx[0] += d_u * (min_x - edge[0]);
			sy[0] += d_v * (min_x - edge[0]);
			edge[0] = min_x;
		}
		if (edge[1] > max_x) edge[1] = max_x;
		if (edge[0] > edge[1])
			continue;

		int u = sx[0] * FIXMUL + 0.5;
		int v = sy[0] * FIXMUL + 0.5;
		int id_u = d_u * FIXMUL + 0.5;
		int id_v = d_v * FIXMUL + 0.5;

		//
		int sx, sy;
		int left = edge[0];
		int right = edge[1];
		uint8_t mask_data;
		dst_pixel = GetPixelAddress32(m_DisplayImg, left, y);
		for (x=left; x<right; x++) {
			sx = u >> FF;
			sy = v >> FF;
			if (((0 <= sx) && (sx < m_DrawImg->width)) &&
				((0 <= sy) && (sy < m_DrawImg->height)))
			{
				//get from draw image
				if (m_view_size == 25) {
					GetPixel4x4(m_DrawImg, sx, sy, dst_pixel);
				} else if (m_view_size == 50) {
					GetPixel2x2(m_DrawImg, sx, sy, dst_pixel);
				} else {
					GetPixelFromBGRA(m_DrawImg, sx, sy, dst_pixel);
				}
				//GetPixelAAFF(m_DrawImg, u, v, id_u, id_v, dst_pixel);
				
				//draw mask
				if (GetMaskState() == IE_MASK_STATE_ID::VALID) {
					mask_data = GetMaskDataPos(m_DrawMaskImg, sx, sy);
					if (m_MaskDrawType == MASK_SOLID_DRAW) {
						if (mask_data != 0) {
							mask_color.a = (100 * mask_data) >> 8;
							AlphaBlendPixel(dst_pixel, &mask_color, 255, dst_pixel);
						}
					} else if (m_MaskDrawType == MASK_DOT_LINE_DRAW) {
						if (mask_data != 0) {
							do{
								int mx, my;
								uint8_t check;
								//check left pix
								check = 0;
								mx = (u - id_u) >> FF;
								my = v >> FF;
								if ((0 <= mx && mx < m_DrawImg->width) &&
									(0 <= my && my < m_DrawImg->height))
								{
									check = GetMaskDataPos(m_DrawMaskImg, mx, my);
								}
								if (check == 0) {
									if (my & 0x4)
										dst_pixel->value = dot_white.value;
									else
										dst_pixel->value = dot_black.value;
									break;
								}


								//check right pix
								check = 0;
								mx = (u + id_u) >> FF;
								my = v >> FF;
								if ((0 <= mx && mx < m_DrawImg->width) &&
									(0 <= my && my < m_DrawImg->height))
								{
									check = GetMaskDataPos(m_DrawMaskImg, mx, my);
								}
								if (check == 0) {
									if(my & 0x4)
										dst_pixel->value = dot_white.value;
									else
										dst_pixel->value = dot_black.value;
									break;
								}

								//check over pix
								check = 0;
								mx = u >> FF;
								my = (v - id_u) >> FF;
								if ((0 <= mx && mx < m_DrawImg->width) &&
									(0 <= my && my < m_DrawImg->height))
								{
									check = GetMaskDataPos(m_DrawMaskImg, mx, my);
								}
								if (check == 0) {
									if (mx & 0x4)
										dst_pixel->value = dot_white.value;
									else
										dst_pixel->value = dot_black.value;
									break;
								}

								//check under pix
								check = 0;
								mx = u >> FF;
								my = (v + id_u) >> FF;
								if ((0 <= mx && mx < m_DrawImg->width) &&
									(0 <= my && my < m_DrawImg->height))
								{
									check = GetMaskDataPos(m_DrawMaskImg, mx, my);
								}
								if (check == 0) {
									if (mx & 0x4)
										dst_pixel->value = dot_white.value;
									else
										dst_pixel->value = dot_black.value;
									break;
								}
							} while(0);
						}
					}
				}
			}
			dst_pixel++;

			u += id_u;
			v += id_v;
		}
	}

	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_DISPLAY_IMG );
}

/////////////////////////////////////
/*!
	選択範囲も入れた描画イメージの再生成
	@param[in] lprc 更新範囲 NULLなら全範囲
*/
void ImgFile::UpdateMaskImage(const LPRECT lprc)
{
	if((lprc->left == lprc->right) || (lprc->top == lprc->bottom))
		return;

	const IplImageExt* mask = m_pMask->GetMaskImage();

	ResourceLock( IE_FILE_RESOURCE_ID::IE_DRAW_MASK_IMG );
	mask->GetRangeIplImage(
		m_DrawMaskImg,
		lprc->left,
		lprc->top,
		lprc->right - lprc->left,
		lprc->bottom - lprc->top,
		lprc->left,
		lprc->top);
	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_DRAW_MASK_IMG );
}

/////////////////////////////////////
/*!
	描画イメージの再生成
*/
void ImgFile::UpdateDrawImage(const LPRECT lprc)
{
	if((lprc->left == lprc->right) || (lprc->top == lprc->bottom))
		return;

	ResourceLock( IE_FILE_RESOURCE_ID::IE_DRAW_IMG );
	UCvPixel color1;
	UCvPixel color2;
	color1.value = 0xffffffff;
	color2.value = 0xff646464;
	CheckerImageRect(m_DrawImg, &color1, &color2, 5, lprc);

	m_pRootLayerGroup->BltRectSynthPixel(
		m_DrawImg,
		lprc->left,
		lprc->top,
		lprc->left,
		lprc->top,
		lprc->right- lprc->left,
		lprc->bottom - lprc->top);
	ResourceUnlock( IE_FILE_RESOURCE_ID::IE_DRAW_IMG );
}

///////////////////////////////////////
/*!
	アフィン変換行列の更新
*/
void ImgFile::UpdateTransformMat()
{
	if(m_DisplayImg == NULL) return;
	if(m_TransformMat == NULL) return;

	double angle = m_view_rot * (CV_PI/180.0);
	double scale = m_view_size/100.0;
	double m[3][3];
	CvMat M = cvMat(3, 3, CV_64FC1, m);
	double alpha = cos(angle)*scale;
	double beta = sin(angle)*scale;

	double lralpha, lrbeta;
	double udalpha, udbeta;
	
	if( IsViewFlipH() ){
		lralpha = -alpha;
		lrbeta = -beta;
	}
	else{
		lralpha = alpha;
		lrbeta = beta;
	}

	if( IsViewFlipV() ){
		udalpha = -alpha;
		udbeta = -beta;
	}
	else{
		udalpha = alpha;
		udbeta = beta;
	}
    
	m[0][0] = lralpha;
	m[0][1] = udbeta;
	m[0][2] = (1-lralpha)*m_view_centerPt.x - udbeta*m_view_centerPt.y + m_view_startPt.x;
 
	m[1][0] = -lrbeta;
	m[1][1] = udalpha;
    m[1][2] = lrbeta*m_view_centerPt.x + (1-udalpha)*m_view_centerPt.y + m_view_startPt.y;
	m[2][0] = 0;
	m[2][1] = 0;
	m[2][2] = 1;

    cvConvert( &M, m_TransformMat );
}

//////////////////////////////////////
/*!
	マウス座標を実際の画像の座標に変換する。
	@param[in,out] lpd 入力、出力
*/
void ImgFile::ConvertPosWndToImg(LPIE_INPUT_DATA lpd) const 
{
	CvMat* InvCtr = cvCreateMat(2, 3, CV_64FC1);

	InvertAffineMat(m_TransformMat, InvCtr);

	double x = lpd->x;
	double y = lpd->y;

	lpd->x = cvmGet(InvCtr, 0, 0) * x +
		cvmGet(InvCtr, 0, 1) * y +
		cvmGet(InvCtr, 0, 2);
	lpd->y = cvmGet(InvCtr, 1, 0) * x +
		cvmGet(InvCtr, 1, 1) * y +
		cvmGet(InvCtr, 1, 2);

	cvReleaseMat(&InvCtr);
}

void ImgFile::ConvertPosWndToImg(const LPPOINT src, LPPOINT dst) const
{
	CvMat* InvCtr = cvCreateMat(2, 3, CV_32F);

	//逆行列を求める
	InvertAffineMat(m_TransformMat, InvCtr);

	dst->x = cvmGet(InvCtr, 0, 0) * src->x +
		cvmGet(InvCtr, 0, 1) * src->y +
		cvmGet(InvCtr, 0, 2) + 0.5;
	dst->y = cvmGet(InvCtr, 1, 0) * src->x +
		cvmGet(InvCtr, 1, 1) * src->y +
		cvmGet(InvCtr, 1, 2) + 0.5;

	cvReleaseMat(&InvCtr);
}

void ImgFile::ConvertPosWndToImg(const CvPoint* src, CvPoint* dst) const
{
	CvPoint2D64f src_32f, dst_32f;
	src_32f.x = src->x;
	src_32f.y = src->y;
	
	ConvertPosWndToImg(&src_32f, &dst_32f);
	dst->x = dst_32f.x;
	dst->y = dst_32f.y;
}

void ImgFile::ConvertPosWndToImg(const CvPoint2D64f* src, CvPoint2D64f* dst) const
{
	if(m_TransformMat == NULL) return;

	CvMat* InvCtr = cvCreateMat(2, 3, CV_64FC1);
	//calc invert matrix
	InvertAffineMat(m_TransformMat, InvCtr);

	dst->x = cvmGet(InvCtr, 0, 0) * src->x +
		cvmGet(InvCtr, 0, 1) * src->y +
		cvmGet(InvCtr, 0, 2);
	dst->y = cvmGet(InvCtr, 1, 0) * src->x +
		cvmGet(InvCtr, 1, 1) * src->y +
		cvmGet(InvCtr, 1, 2);

	cvReleaseMat(&InvCtr);
}

void ImgFile::ConvertPosWndToImg(const LPRECT src_rc, LPRECT dst_rc) const
{
	if(m_TransformMat == NULL) return;

	POINT sp[4];
	sp[0].x = src_rc->left;  sp[0].y = src_rc->top;
	sp[1].x = src_rc->right; sp[1].y = src_rc->top;
	sp[2].x = src_rc->right; sp[2].y = src_rc->bottom;
	sp[3].x = src_rc->left;  sp[3].y = src_rc->bottom;

	CvMat* InvCtr = cvCreateMat(2, 3, CV_64FC1);

	InvertAffineMat(m_TransformMat, InvCtr);

	int i;
	POINT dp[4];
	for(i=0; i<4; i++){
		dp[i].x = cvmGet(InvCtr, 0, 0) * sp[i].x +
			cvmGet(InvCtr, 0, 1) * sp[i].y +
			cvmGet(InvCtr, 0, 2);
		dp[i].y = cvmGet(InvCtr, 1, 0) * sp[i].x +
			cvmGet(InvCtr, 1, 1) * sp[i].y +
			cvmGet(InvCtr, 1, 2);
	}

	int min_x, max_x;
	int min_y, max_y;
	min_x = max_x = dp[0].x;
	min_y = max_y = dp[0].y;
	for(i=1; i<4; i++){
		if(min_x > dp[i].x){
			min_x = dp[i].x;
		}
		if(min_y > dp[i].y){
			min_y = dp[i].y;
		}
		if(max_x < dp[i].x){
			max_x = dp[i].x;
		}
		if(max_y < dp[i].y){
			max_y = dp[i].y;
		}
	}

	dst_rc->top = min_y;
	dst_rc->left = min_x;
	dst_rc->bottom = max_y;
	dst_rc->right = max_x;

	cvReleaseMat(&InvCtr);
}

//////////////////////////////////////
/*!
	画像の座標をウィンドウクライアント座標に変換する。
	@param[in,out] lpd 入力、出力
*/
void ImgFile::ConvertPosImgToWnd(LPIE_INPUT_DATA lpd) const
{
	if(m_TransformMat == NULL) return;

	double x = lpd->x;
	double y = lpd->y;

	lpd->x = cvmGet(m_TransformMat, 0, 0) * x +
		cvmGet(m_TransformMat, 0, 1) * y +
		cvmGet(m_TransformMat, 0, 2);
	lpd->y = cvmGet(m_TransformMat, 1, 0) * x +
		cvmGet(m_TransformMat, 1, 1) * y +
		cvmGet(m_TransformMat, 1, 2);
}

void ImgFile::ConvertPosImgToWnd(const LPPOINT src, LPPOINT dst) const
{
	if(m_TransformMat == NULL) return;

	dst->x = cvmGet(m_TransformMat, 0, 0) * src->x +
		cvmGet(m_TransformMat, 0, 1) * src->y +
		cvmGet(m_TransformMat, 0, 2) + 0.5;
	dst->y = cvmGet(m_TransformMat, 1, 0) * src->x +
		cvmGet(m_TransformMat, 1, 1) * src->y +
		cvmGet(m_TransformMat, 1, 2) + 0.5;
}

void ImgFile::ConvertPosImgToWnd(const CvPoint2D64f* src, CvPoint2D64f* dst) const
{
	ConvertPosImgToWnd(src, dst, 1);
}

void ImgFile::ConvertPosImgToWnd(const CvPoint2D64f* src, CvPoint2D64f* dst, int size) const
{
	assert(size > 0);
	if(m_TransformMat == NULL) return;

	int i;
	for(i=0; i<size; i++){
		double x = src[i].x;
		double y = src[i].y;
		dst[i].x = cvmGet(m_TransformMat, 0, 0) * x +
			cvmGet(m_TransformMat, 0, 1) * y +
			cvmGet(m_TransformMat, 0, 2);
		dst[i].y = cvmGet(m_TransformMat, 1, 0) * x +
			cvmGet(m_TransformMat, 1, 1) * y +
			cvmGet(m_TransformMat, 1, 2);
	}
}

void ImgFile::ConvertPosImgToWnd(const LPRECT src_rc, LPRECT dst_rc) const
{
	if(m_TransformMat == NULL) return;

	POINT sp[4];
	sp[0].x = src_rc->left;  sp[0].y = src_rc->top;
	sp[1].x = src_rc->right; sp[1].y = src_rc->top;
	sp[2].x = src_rc->right; sp[2].y = src_rc->bottom;
	sp[3].x = src_rc->left;  sp[3].y = src_rc->bottom;

	int i;
	POINT dp[4];
	for(i=0; i<4; i++){
		dp[i].x = cvmGet(m_TransformMat, 0, 0) * sp[i].x +
			cvmGet(m_TransformMat, 0, 1) * sp[i].y +
			cvmGet(m_TransformMat, 0, 2);
		dp[i].y = cvmGet(m_TransformMat, 1, 0) * sp[i].x +
			cvmGet(m_TransformMat, 1, 1) * sp[i].y +
			cvmGet(m_TransformMat, 1, 2);
	}

	int min_x, max_x;
	int min_y, max_y;
	min_x = max_x = dp[0].x;
	min_y = max_y = dp[0].y;
	for(i=1; i<4; i++){
		if(min_x > dp[i].x){
			min_x = dp[i].x;
		}
		if(min_y > dp[i].y){
			min_y = dp[i].y;
		}
		if(max_x < dp[i].x){
			max_x = dp[i].x;
		}
		if(max_y < dp[i].y){
			max_y = dp[i].y;
		}
	}

	dst_rc->top = min_y;
	dst_rc->left = min_x;
	dst_rc->bottom = max_y;
	dst_rc->right = max_x;
}

///////////////////////////////////////
/*!
	オフスクリーンバッファーを作成する。
*/
void ImgFile::ReGenOffScreenBuffer(LPRECT lprect)
{
	if(lprect == NULL) return;

	int width = lprect->right - lprect->left;
	int height = lprect->bottom - lprect->top;

	CvSize displaySize;
	displaySize.width = width;
	displaySize.height = height;

	//re create image
	ResourceLock(IE_FILE_RESOURCE_ID::IE_DISPLAY_IMG);
	if(m_DisplayImg){
		cvReleaseImage(&m_DisplayImg);
	}
	m_DisplayImg = cvCreateImage(displaySize, IPL_DEPTH_8U, 4);
	ResourceUnlock(IE_FILE_RESOURCE_ID::IE_DISPLAY_IMG);

	m_view_centerPt = GetViewCenterPos();
	UpdateTransformMat();
	
	LPUPDATE_DATA data = this->CreateUpdateData();
	data->isAll = true;
	this->PushUpdateData(data);
}

