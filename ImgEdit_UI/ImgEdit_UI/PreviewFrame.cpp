#include "stdafx.h"

#include "PreviewFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern ImgEdit g_ImgEdit;

CPreviewFrame::CPreviewFrame()
{
	m_ImgDragRect.top = m_ImgDragRect.left = 0;
	m_ImgDragRect.right = m_ImgDragRect.bottom = 0;
	m_isDragRect = false;
	m_resize_ration = 1.0;
	m_PreviewThm = NULL;
	m_nTimer = 0;
	m_pEditFile = NULL;
}

CPreviewFrame::~CPreviewFrame()
{
	_ASSERTE(_CrtCheckMemory());
}

void CPreviewFrame::OnChangeActiveFile(ImgFile_Ptr pNewFile, ImgFile_Ptr pOldFile)
{
	if (m_pEditFile) {
		m_pEditFile->DeleteEventListener( this );
	}
	if (pNewFile) {
		pNewFile->AddEventListener( this );
	}

	m_pEditFile = pNewFile;
	this->Invalidate(FALSE);
}

void CPreviewFrame::OnUpdate()
{
	if(m_nTimer != 0){
		this->KillTimer(m_nTimer);
	}
	m_nTimer = this->SetTimer(1, 1000, 0);

	this->Invalidate(FALSE);
}

void CPreviewFrame::UpdatePreviewThm()
{
	if(m_PreviewThm){
		cvReleaseImage(&m_PreviewThm);
	}

	CRect rectWnd;
	GetWindowRect(rectWnd);

	ImgFile_Ptr pFile = g_ImgEdit.GetActiveFile();
	if(pFile){
		CvSize file_img_size = pFile->GetImgSize();

		CvSize Resize_size;
		int frame_width = rectWnd.right - rectWnd.left;
		int frame_height = rectWnd.bottom - rectWnd.top;
		if(file_img_size.width > file_img_size.height){
			Resize_size.width = frame_width;
			Resize_size.height = file_img_size.height * ((double)frame_width / file_img_size.width);

			if(Resize_size.height > frame_height){
				Resize_size.width = Resize_size.width * ((double) frame_height / Resize_size.height);
				Resize_size.height = frame_height;
			}

			m_PreviewThm = cvCreateImage(Resize_size, IPL_DEPTH_8U, 4);
		}
		else{
			Resize_size.height = frame_height;
			Resize_size.width = file_img_size.width * ((double)frame_height / file_img_size.height);

			if(Resize_size.width > frame_width){
				Resize_size.height = Resize_size.height * ((double) frame_width / Resize_size.width);
				Resize_size.width = frame_width;
			}


			m_PreviewThm = cvCreateImage(Resize_size, IPL_DEPTH_8U, 4);
		}

		if(m_PreviewThm == NULL) return;
		pFile->CopyFileImageThumbnail(m_PreviewThm);

		//
		CvSize display_size = pFile->GetDisplaySize();
		m_DispRectPt[0].x = 0; m_DispRectPt[0].y = 0;
		m_DispRectPt[1].x = display_size.width-1; m_DispRectPt[1].y = 0;
		m_DispRectPt[2].x = display_size.width-1; m_DispRectPt[2].y = display_size.height-1;
		m_DispRectPt[3].x = 0; m_DispRectPt[3].y = display_size.height-1;

		m_resize_ration = (double) Resize_size.width / file_img_size.width;
		for(int i=0; i<4; i++){
			pFile->ConvertPosWndToImg(&(m_DispRectPt[i]), &(m_DispRectPt[i]));
			m_DispRectPt[i].x *= m_resize_ration;
			m_DispRectPt[i].y *= m_resize_ration;
			m_DispRectPt[i].x += (frame_width - Resize_size.width)/2;
			m_DispRectPt[i].y += (frame_height - Resize_size.height)/2;
		}

		//calclate darg rect
   		m_ImgDragRect.left   = m_DispRectPt[0].x;
		m_ImgDragRect.top    = m_DispRectPt[0].y;
		m_ImgDragRect.right  = m_DispRectPt[0].x;
		m_ImgDragRect.bottom = m_DispRectPt[0].y;
		for(int i=1; i<4; i++){
			if(m_ImgDragRect.left > m_DispRectPt[i].x) m_ImgDragRect.left = m_DispRectPt[i].x;
			if(m_ImgDragRect.top > m_DispRectPt[i].y) m_ImgDragRect.top = m_DispRectPt[i].y;
			if(m_ImgDragRect.right < m_DispRectPt[i].x) m_ImgDragRect.right = m_DispRectPt[i].x; 
			if(m_ImgDragRect.bottom < m_DispRectPt[i].y) m_ImgDragRect.bottom = m_DispRectPt[i].y;
		}
	}
}

void CPreviewFrame::UpdateDragRect()
{
	if(m_PreviewThm == NULL) return;

	ImgFile_Ptr pFile = g_ImgEdit.GetActiveFile();
	if( pFile ){
		CRect rectWnd;
		GetWindowRect(rectWnd);

		CvSize file_img_size = pFile->GetImgSize();
		int frame_width = rectWnd.right - rectWnd.left;
		int frame_height = rectWnd.bottom - rectWnd.top;

		//
		CvSize display_size = pFile->GetDisplaySize();
		m_DispRectPt[0].x = 0; m_DispRectPt[0].y = 0;
		m_DispRectPt[1].x = display_size.width-1; m_DispRectPt[1].y = 0;
		m_DispRectPt[2].x = display_size.width-1; m_DispRectPt[2].y = display_size.height-1;
		m_DispRectPt[3].x = 0; m_DispRectPt[3].y = display_size.height-1;

		m_resize_ration = (double) m_PreviewThm->width / file_img_size.width;
		for(int i=0; i<4; i++){
			pFile->ConvertPosWndToImg(&(m_DispRectPt[i]), &(m_DispRectPt[i]));
			m_DispRectPt[i].x *= m_resize_ration;
			m_DispRectPt[i].y *= m_resize_ration;
			m_DispRectPt[i].x += (frame_width - m_PreviewThm->width)/2;
			m_DispRectPt[i].y += (frame_height - m_PreviewThm->height)/2;
		}

		//calclate darg rect
		m_ImgDragRect.left   = m_DispRectPt[0].x;
		m_ImgDragRect.top    = m_DispRectPt[0].y;
		m_ImgDragRect.right  = m_DispRectPt[0].x;
		m_ImgDragRect.bottom = m_DispRectPt[0].y;
		for(int i=1; i<4; i++){
			if(m_ImgDragRect.left > m_DispRectPt[i].x) m_ImgDragRect.left = m_DispRectPt[i].x;
			if(m_ImgDragRect.top > m_DispRectPt[i].y) m_ImgDragRect.top = m_DispRectPt[i].y;
			if(m_ImgDragRect.right < m_DispRectPt[i].x) m_ImgDragRect.right = m_DispRectPt[i].x; 
			if(m_ImgDragRect.bottom < m_DispRectPt[i].y) m_ImgDragRect.bottom = m_DispRectPt[i].y;
		}
	}
}

BEGIN_MESSAGE_MAP(CPreviewFrame, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

int CPreviewFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CWnd::OnCreate(lpCreateStruct)){
		return -1;
	}

	//register event listener
	g_ImgEdit.AddEventListener( this );

	return 0;
}

void CPreviewFrame::OnDestroy()
{
	//unregister event listener
	if (m_pEditFile) {
		m_pEditFile->DeleteEventListener( this );
	}
	g_ImgEdit.DeleteEventListener( this );

	CWnd::OnDestroy();
}

void CPreviewFrame::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
}

void CPreviewFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(isInRect(point.x, point.y, &m_ImgDragRect)){
		//start mouse capture
		this->SetCapture();

		m_isDragRect = true;
		m_beforPos.x = point.x;
		m_beforPos.y = point.y;
	}
}

void CPreviewFrame::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_isDragRect){
		if(!m_pEditFile) return;
		double file_view_size = m_pEditFile->GetViewSize()/100.0;

		CvPoint2D64f pos;
		m_pEditFile->GetViewPos(&pos);
		pos.x -= (point.x - m_beforPos.x)/m_resize_ration;
		pos.y -= (point.y - m_beforPos.y)/m_resize_ration;

		m_pEditFile->SetViewPos(&pos);

		LPUPDATE_DATA data = m_pEditFile->CreateUpdateData();
		data->isAll = true;
		m_pEditFile->PushUpdateData(data);

		int i;
		for(i=0; i<4; i++){
			m_DispRectPt[i].x += point.x - m_beforPos.x;
			m_DispRectPt[i].y += point.y - m_beforPos.y;
		}

		m_beforPos.x = point.x;
		m_beforPos.y = point.y;

		this->Invalidate(FALSE);
	}
}

void CPreviewFrame::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_isDragRect){
		//end mosue capture
		::ReleaseCapture();

		m_isDragRect = false;
	}
}

void CPreviewFrame::OnPaint()
{
	CPaintDC dc(this);
	//
	CRect rectWnd;
	GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	int frame_width = rectWnd.right - rectWnd.left;
	int frame_height = rectWnd.bottom - rectWnd.top;

	CDC* pDC = CDC::FromHandle( dc.GetSafeHdc() );

	//create back buffer
	CDC memDC;
	CBitmap memBmp;
	memDC.CreateCompatibleDC( pDC );
	memBmp.CreateCompatibleBitmap(pDC, frame_width, frame_height);
	
	CBitmap* memOldBmp = memDC.SelectObject( &memBmp );

	rectWnd.InflateRect(1,1);
	memDC.FillSolidRect(rectWnd, ::GetSysColor(COLOR_WINDOW));

	if(m_PreviewThm){
		//
		IPLImageToDevice(
			memDC.GetSafeHdc(),
			(frame_width-m_PreviewThm->width)/2,
			(frame_height-m_PreviewThm->height)/2,
			m_PreviewThm->width,
			m_PreviewThm->height,
			m_PreviewThm,
			0,
			0);

		//
		UpdateDragRect();
		
		//draw drag rect
		CPen pen;
		pen.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
		CPen* oldpen = memDC.SelectObject( &pen );
		memDC.MoveTo(m_DispRectPt[0].x, m_DispRectPt[0].y);
		memDC.LineTo(m_DispRectPt[1].x, m_DispRectPt[1].y);
		memDC.MoveTo(m_DispRectPt[1].x, m_DispRectPt[1].y);
		memDC.LineTo(m_DispRectPt[2].x, m_DispRectPt[2].y);
		memDC.MoveTo(m_DispRectPt[2].x, m_DispRectPt[2].y);
		memDC.LineTo(m_DispRectPt[3].x, m_DispRectPt[3].y);
		memDC.MoveTo(m_DispRectPt[3].x, m_DispRectPt[3].y);
		memDC.LineTo(m_DispRectPt[0].x, m_DispRectPt[0].y);
		memDC.SelectObject( oldpen );
		pen.DeleteObject();
	}

	//blt back buffer to display
	BitBlt(dc.GetSafeHdc(), 0,0, frame_width, frame_height,
		memDC.GetSafeHdc(), 0, 0, SRCCOPY);

	memDC.SelectObject( memOldBmp );
	memBmp.DeleteObject();
}

void CPreviewFrame::OnTimer(UINT_PTR nIDEvent)
{
	this->KillTimer(m_nTimer);
	m_nTimer = 0;
	UpdatePreviewThm();
	this->Invalidate(FALSE);
}