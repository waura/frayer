#include "stdafx.h"

#include "IEBrush.h"
#include "OutputError.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

//_EXPORT void ReleaseIEBrush(IEBrush_Ptr* ppBrush)
//{
//	if(*ppBrush){
//		delete (*ppBrush);
//		(*ppBrush) = NULL;
//	}
//}

IEBrush::IEBrush()
{
	m_brush_type_id = -1;
	m_brush_buf_map_size = 0;
	m_radius = 1;
	m_min_radius_per = 0.0;
	m_min_alpha_per = 1.0;
	m_interval = 0.2;

	m_texture = NULL;

	m_radius_ctrl_id = IE_BRUSH_CTRL_ID::PRESSURE;
	m_alpha_ctrl_id = IE_BRUSH_CTRL_ID::PRESSURE;

	SetName("");
	ClearPrevDrawPt();
}

IEBrush::~IEBrush()
{
	m_IEBrushEventListener_List.clear();
	DeleteTexture();
	ClearBrushImgBufMap();
}

void IEBrush::WriteBrushToXml(FILE* fp, int indt)
{
	int i;
	for(i=0; i<(indt+1); i++) fprintf(fp, "  ");
	fprintf(fp, "<radius_ctrl val=\"%d\"/>\n", GetRadiusCtrlID());
	for(i=0; i<(indt+1); i++) fprintf(fp, "  ");
	fprintf(fp, "<alpha_ctrl val=\"%d\"/>\n", GetAlphaCtrlID());
	for(i=0; i<(indt+1); i++) fprintf(fp, "  ");
	fprintf(fp, "<min_radius val=\"%d\"/>\n", GetMinRadius());
	for(i=0; i<(indt+1); i++) fprintf(fp, "  ");
	fprintf(fp, "<min_alpha val=\"%d\"/>\n", GetMinAlpha());
	for(i=0; i<(indt+1); i++) fprintf(fp, "  ");
	fprintf(fp, "<interval val=\"%d\"/>\n", GetInterval());
	if (IsValidTexture()) {
		for(i=0; i<(indt+1); i++) fprintf(fp, "  ");
		fprintf(fp, "<texture path=\"%s\"/>\n", m_texture_path.c_str());
	}
}

void IEBrush::AddEventListener(IEBrushEventListener*  pListener)
{
	m_IEBrushEventListener_List.push_back( pListener );
}

void IEBrush::DeleteEventListener(IEBrushEventListener*  pListener)
{
	IEBrushEventListener_List::iterator itr = m_IEBrushEventListener_List.begin();
	for (; itr != m_IEBrushEventListener_List.end(); itr++) {
		if ((*itr) == pListener) {
			m_IEBrushEventListener_List.erase(itr);
			return;
		}
	}
}

void IEBrush::ClearBrushImgBufMap()
{
	if (m_brushImgBuf_Map.empty()) return;


	IEBrushImgBuffer_Map::iterator itr = m_brushImgBuf_Map.begin();
	for (; itr != m_brushImgBuf_Map.end(); itr++) {
		cvReleaseImage(&((*itr).second));
	}
	m_brushImgBuf_Map.clear();
	m_brush_buf_map_size = 0;
}

void IEBrush::ManageBufMap()
{
	if (m_brush_buf_map_size > _MAX_BRUSH_IMG_BUF_SIZE_) {
		ClearBrushImgBufMap();
	}
}

void IEBrush::ReleaseBrushSampleImg(IplImage** img)
{
	cvReleaseImage(img);
}

void IEBrush::SetOrgRadius(int r)
{
	if (m_orgRadius == r) return;

	int prev_r = m_orgRadius;
	m_orgRadius = r;

	//call event listener
	IEBrushEventListener_List::iterator itr = m_IEBrushEventListener_List.begin();
	for (; itr != m_IEBrushEventListener_List.end(); itr++) {
		if ((*itr)->IsLockBrushEvent() == false && (*itr)->IsCalledIEBrush() == false) {
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledIEBrush(true);
			//
			(*itr)->OnChangeOrgRadius(prev_r, m_orgRadius);
		}

		(*itr)->SetCalledIEBrush(false);
	}

	SetRadius( m_orgRadius );
}

void IEBrush::SetRadius(int r)
{
	if(m_radius == r) return;

	int prev_r = m_radius;
	m_radius = r;

	//call event listener
	IEBrushEventListener_List::iterator itr = m_IEBrushEventListener_List.begin();
	for (; itr != m_IEBrushEventListener_List.end(); itr++) {
		if ((*itr)->IsLockBrushEvent() == false && (*itr)->IsCalledIEBrush() == false) {
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledIEBrush(true);
			//
			(*itr)->OnChangeRadius(prev_r, m_radius);
		}

		(*itr)->SetCalledIEBrush(false);
	}
}

void IEBrush::SetRadiusCtrlID(IE_BRUSH_CTRL_ID id)
{
	assert(0 <= id && id < IE_BRUSH_CTRL_ID::_MAX_INDEX_);
	if (m_radius_ctrl_id == id) return;

	m_radius_ctrl_id = id;

	//call event listener
	IEBrushEventListener_List::iterator itr = m_IEBrushEventListener_List.begin();
	for (; itr != m_IEBrushEventListener_List.end(); itr++) {
		if ((*itr)->IsLockBrushEvent() == false && (*itr)->IsCalledIEBrush() == false) {
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledIEBrush(true);
			//
			(*itr)->OnChangeBrushStroke();
		}

		(*itr)->SetCalledIEBrush(false);
	}
}

void IEBrush::SetAlphaCtrlID(IE_BRUSH_CTRL_ID id)
{
	assert(0 <= id && id < IE_BRUSH_CTRL_ID::_MAX_INDEX_);
	if (m_alpha_ctrl_id == id) return;

	m_alpha_ctrl_id = id;

	//call event listener
	IEBrushEventListener_List::iterator itr = m_IEBrushEventListener_List.begin();
	for (; itr != m_IEBrushEventListener_List.end(); itr++) {
		if ((*itr)->IsLockBrushEvent() == false && (*itr)->IsCalledIEBrush() == false) {
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledIEBrush(true);
			//
			(*itr)->OnChangeBrushStroke();
		}

		(*itr)->SetCalledIEBrush(false);
	}
}

void IEBrush::SetMinRadius(int percent)
{
	if (m_min_radius_per == (percent/100.0)) return;

	m_min_radius_per = percent/100.0;

	//call event listener
	IEBrushEventListener_List::iterator itr = m_IEBrushEventListener_List.begin();
	for (; itr != m_IEBrushEventListener_List.end(); itr++) {
		if ((*itr)->IsLockBrushEvent() == false && (*itr)->IsCalledIEBrush() == false) {
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledIEBrush(true);
			//
			(*itr)->OnChangeBrushStroke();
		}

		(*itr)->SetCalledIEBrush(false);
	}
}

void IEBrush::SetMinAlpha(int percent)
{
	if(m_min_alpha_per == (percent/100.0)) return;

	m_min_alpha_per = percent/100.0;

	//call event listener
	IEBrushEventListener_List::iterator itr = m_IEBrushEventListener_List.begin();
	for (; itr != m_IEBrushEventListener_List.end(); itr++) {
		if ((*itr)->IsLockBrushEvent() == false && (*itr)->IsCalledIEBrush() == false) {
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledIEBrush(true);
			//
			(*itr)->OnChangeBrushStroke();
		}

		(*itr)->SetCalledIEBrush(false);
	}
}

void IEBrush::SetInterval(int interval)
{
	double interval_per = interval / 100.0;
	if (m_interval == interval_per) return;

	m_interval = interval_per;

	//call event listener
	IEBrushEventListener_List::iterator itr = m_IEBrushEventListener_List.begin();
	for (; itr != m_IEBrushEventListener_List.end(); itr++) {
		if ((*itr)->IsLockBrushEvent() == false && (*itr)->IsCalledIEBrush() == false) {
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledIEBrush(true);
			//
			(*itr)->OnChangeBrushStroke();
		}

		(*itr)->SetCalledIEBrush(false);
	}
}

bool IEBrush::AddTexture(const char* path)
{
	if (m_texture_path == path)
		return false;

	DeleteTexture();

	m_texture_path = path;
	IplImage* texture_bgr = cvLoadImage(path);
	if (texture_bgr == NULL) {
		OutputError::PushLog(LOG_LEVEL::_ERROR, "can't load brush texture ", path);
		OutputError::Alert(IE_ERROR_CANT_LOAD_BRUSH_TEXTURE);
		return false;
	}
	if (texture_bgr->width != texture_bgr->height) {
		OutputError::Alert(IE_ERROR_INVALID_BRUSH_TEXTURE);
		cvReleaseImage(&texture_bgr);
		return false;
	}
	if ((texture_bgr->width != 256) &&
		(texture_bgr->width != 512) &&
		(texture_bgr->width != 1024))
	{
		OutputError::Alert(IE_ERROR_INVALID_BRUSH_TEXTURE);
		cvReleaseImage(&texture_bgr);
		return false;
	}

	m_texture = cvCreateImage(cvSize(texture_bgr->width, texture_bgr->height), IPL_DEPTH_8U, 1);
	if (m_texture == NULL) {
		OutputError::PushLog(LOG_LEVEL::_ERROR, "can't create gray scale brush texture ", path);
		cvReleaseImage(&texture_bgr);
		return false;
	}
	cvCvtColor(texture_bgr, m_texture, CV_BGR2GRAY);
	cvReleaseImage(&texture_bgr);

	//call event listener
	IEBrushEventListener_List::iterator itr = m_IEBrushEventListener_List.begin();
	for (; itr != m_IEBrushEventListener_List.end(); itr++) {
		if ((*itr)->IsLockBrushEvent() == false && (*itr)->IsCalledIEBrush() == false) {
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledIEBrush(true);
			//
			(*itr)->OnChangeBrushStroke();
		}

		(*itr)->SetCalledIEBrush(false);
	}

	return true;
}

void IEBrush::DeleteTexture()
{
	if (m_texture) {
		cvReleaseImage(&m_texture);
		m_texture = NULL;

		//call event listener
		IEBrushEventListener_List::iterator itr = m_IEBrushEventListener_List.begin();
		for (; itr != m_IEBrushEventListener_List.end(); itr++) {
			if ((*itr)->IsLockBrushEvent() == false && (*itr)->IsCalledIEBrush() == false) {
				//無限にイベント呼び出しが続かないようにフラグを立てる
				(*itr)->SetCalledIEBrush(true);
				//
				(*itr)->OnChangeBrushStroke();
			}

			(*itr)->SetCalledIEBrush(false);
		}
	}
}