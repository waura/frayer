
// ImgEdit_UIView.cpp : CImgEdit_UIView クラスの実装
//

#include "stdafx.h"
#include "ImgEdit_UI.h"

#include "ImgEdit_UIDoc.h"
#include "ImgEdit_UIView.h"

#include <ctype.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define _PRINT_TIME

#define RAD2DEG(x) ((x/3.14159265)*180.0)

#define UPDATE_IMAGE_TIMER 1

extern ImgEdit g_ImgEdit;


DWORD WINAPI UpdateThread(LPVOID data)
{
	LPDRAW_DATA draw_data = (LPDRAW_DATA)data;
	RECT draw_disp_rect = draw_data->draw_disp_rect;
	RECT draw_img_rect = draw_data->draw_img_rect;

	ImgFile_Ptr f = draw_data->imgFile;
	if(!f) return 0;

#ifdef _PRINT_TIME
	char str_blt_lap[256];
	IETimer timer;
#endif

	RECT updated_rc;
	f->Update(&draw_img_rect, &updated_rc);

#ifdef _PRINT_TIME
	sprintf(str_blt_lap, "ImgFile::Update() %f msec\n", timer.elapsed());
	::OutputDebugString(str_blt_lap);
#endif
	return 0;
}

DWORD WINAPI DrawThread(LPVOID data)
{
	LPDRAW_DATA draw_data = (LPDRAW_DATA)data;
	CDC* pDC = draw_data->draw_view->GetDC();
	HDC hdcMem = draw_data->hBackDC;
	HBITMAP hbmp = draw_data->hBackBmp;
	RECT draw_disp_rect = draw_data->draw_disp_rect;
	RECT draw_img_rect = draw_data->draw_img_rect;

	ImgFile_Ptr f = draw_data->imgFile;
	if(!f) return 0;

	const IplImage* dImg = f->GetDisplayImg();

	//バックバッファに切り替え
	HBITMAP hbmp_old = (HBITMAP)::SelectObject(hdcMem, hbmp);
	//バックバッファに画像を転送
	IPLImageToDevice(
		hdcMem,
		0,
		0,
		draw_disp_rect.right - draw_disp_rect.left,
		draw_disp_rect.bottom - draw_disp_rect.top,
		dImg,
		draw_disp_rect.left,
		draw_disp_rect.top);

	//
	g_ImgEdit.OnDraw( hdcMem );

	//
#ifdef _PRINT_TIME
	char str_blt_lap[256];
	IETimer timer;
#endif

	BitBlt(
		pDC->GetSafeHdc(),
		draw_disp_rect.left,
		draw_disp_rect.top,
		draw_disp_rect.right - draw_disp_rect.left,
		draw_disp_rect.bottom - draw_disp_rect.top,
		hdcMem,
		0,
		0,
		SRCCOPY);

#ifdef _PRINT_TIME
	sprintf(str_blt_lap, "BitBlt() %f msec\n", timer.elapsed());
	::OutputDebugString(str_blt_lap);
#endif

	draw_data->draw_view->ReleaseDC( pDC );
	return 0;
}

unsigned _stdcall UpdateDrawThread(void* data)
{
	TRY{
		LPDRAW_DATA draw_data = (LPDRAW_DATA)data;
		CDC* pDC = draw_data->draw_view->GetDC();
		HDC hdcMem = draw_data->hBackDC;
		HBITMAP hbmp = draw_data->hBackBmp;
		RECT draw_disp_rect = draw_data->draw_disp_rect;
		RECT draw_img_rect = draw_data->draw_img_rect;

		ImgFile_Ptr f = draw_data->imgFile;
		if (!f) return 0;

		RECT updated_rc;

#ifdef _PRINT_TIME
		char str_blt_lap[256];
		IETimer timer;
#endif

		f->LockUpdateData();
		f->Update(NULL, &updated_rc);
		//f->Update(&draw_img_rect, &updated_rc);
		f->ClearUpdateData();
		f->UnlockUpdateData();

#ifdef _PRINT_TIME
		sprintf(str_blt_lap, "ImgFile::Update() %f msec\n", timer.elapsed());
		::OutputDebugString(str_blt_lap);
		timer.restart();
#endif
		//switch to back buffer
		HBITMAP hbmp_old = (HBITMAP)::SelectObject(hdcMem, hbmp);

		const IplImage* dImg = f->GetDisplayImg();
		if (dImg) {
			//blt to back buffer
			IPLImageToDevice(
				hdcMem,
				0,
				0,
				draw_disp_rect.right - draw_disp_rect.left,
				draw_disp_rect.bottom - draw_disp_rect.top,
				dImg,
				draw_disp_rect.left,
				draw_disp_rect.top);

			//
			g_ImgEdit.OnDraw( hdcMem );
		}

		//blt back buffer to display
		BitBlt(
			pDC->GetSafeHdc(),
			draw_disp_rect.left,
			draw_disp_rect.top,
			draw_disp_rect.right - draw_disp_rect.left,
			draw_disp_rect.bottom - draw_disp_rect.top,
			hdcMem,
			0,
			0,
			SRCCOPY);

	#ifdef _PRINT_TIME
		sprintf(str_blt_lap, "BitBlt() %f msec\n", timer.elapsed());
		::OutputDebugString(str_blt_lap);
	#endif

		draw_data->draw_view->ReleaseDC( pDC );
#ifdef IE_MULTI_THREAD_DRAW
		(*draw_data->num_of_running_thread)--;
#endif //IE_MULTI_THREAD_DRAW
	}
	CATCH(CException, e)
	{
		::OutputDebugString("UpdateDrawThread() exception\n");
	}
	END_CATCH

	return 0;
}

// CImgEdit_UIView

IMPLEMENT_DYNCREATE(CImgEdit_UIView, CView)

BEGIN_MESSAGE_MAP(CImgEdit_UIView, CView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ACTIVATE()
	ON_WM_MDIACTIVATE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_NCHITTEST()
	ON_MESSAGE(WT_PACKET, CImgEdit_UIView::OnWtPacket)
	ON_MESSAGE(WT_PROXIMITY, CImgEdit_UIView::OnWtProximity)
	//ON_MESSAGE(WM_USER_PUSH_IE_INPUT_DATA, CImgEdit_UIView::OnPushInputData)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_APP_MOUSEWHEEL, CImgEdit_UIView::OnAppMouseWheel)
END_MESSAGE_MAP()

// CImgEdit_UIView コンストラクション/デストラクション

CImgEdit_UIView::CImgEdit_UIView()
{
	m_isDrag = false;
	m_hBrushCursor = NULL;
	m_numOfProcessors = 1;
	m_numOfRunningThread = 0;
	m_DrawDatas = NULL;
	m_isRegistEventListener = false;
	m_LockResourceID = 0;

}

CImgEdit_UIView::~CImgEdit_UIView()
{
	this->SetNormalCorsor();
	
	if (m_hBrushCursor) {
		::DestroyIcon(m_hBrushCursor);
	}

	_ASSERTE(_CrtCheckMemory());
}

BOOL CImgEdit_UIView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

void CImgEdit_UIView::OnImgLayerPushUpdateData(const LPUPDATE_DATA data)
{
	this->Invalidate(FALSE);
}

void CImgEdit_UIView::OnImgFilePushUpdateData(const LPUPDATE_DATA data)
{
	this->Invalidate(FALSE);
}

void CImgEdit_UIView::OnChangeViewPos(const CvPoint2D64f *pos)
{
	AdjastScrollPos();
}

void CImgEdit_UIView::OnChangeViewSize(int size)
{
	AdjastScrollSizes();
	AdjastScrollPos();
}

void CImgEdit_UIView::OnChangeViewRot(int rot)
{
	AdjastScrollSizes();
	AdjastScrollPos();
}

void CImgEdit_UIView::OnInitialUpdate()
{
	CImgEdit_UIDoc* pDoc = GetDocument();
	if (!pDoc)
		return;

	CString path = pDoc->GetPathName();

	//on first time of view
	ImgFile_Ptr f = pDoc->GetImgFile();
	CvSize img_size = f->GetImgSize();

	AdjastScrollSizes();

	SetScrollPos(SB_HORZ, 0);
	SetScrollPos(SB_VERT, 0);

	ShowScrollBar(SB_BOTH, TRUE);

	//for load from scaner
	CString title = pDoc->GetTitle();
	if (title == "??scaner??") {
		pDoc->SetEmptyPath();
	}

	//regist event listener
	if (f) {
		f->AddEventListener( this );
		f->GetRootLayerGroup()->AddEventLilstener( this );
	}

	//set update image timer
	this->SetTimer(UPDATE_IMAGE_TIMER, 100, NULL);
}

// CImgEdit_UIView 描画
void CImgEdit_UIView::OnDraw(CDC* pDC)
{
	TRY
	{
		CImgEdit_UIDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		if (!pDoc)
			return;

		ImgFile_Ptr f = pDoc->GetImgFile();
		if (!f)
			return;
		if (!f->IsInit())
			return;
		if (g_ImgEdit.GetActiveFile() != f)
			return;
		if (m_DrawDatas == NULL)
			return;

		//
#ifdef IE_MULTI_THREAD_DRAW
		//マルチスレッド
		if (m_numOfProcessors > 1) {
#ifdef _DEBUG
			char str_blt_lap[256];
			IETimer timer;
#endif //_DEBUG
			//遅い!
			//m_numOfRunningThread = m_numOfProcessors;

			//DWORD thId;
			//int i;
			//f->LockUpdateData();

			////create update thread
			//for(i=0; i<m_numOfProcessors; i++){
			//	m_DrawDatas[i].imgFile = f;
			//	m_DrawDatas[0].num_of_running_thread = (&m_numOfRunningThread);
			//	//m_hDrawThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DrawThread, (LPVOID)&m_DrawData, 0, &thId); 
			//	m_DrawDatas[i].pThread = ::AfxBeginThread((AFX_THREADPROC)UpdateThread, (LPVOID)&(m_DrawDatas[i]));
			//}

			//DWORD ret;
			//for(i=0; i<m_numOfProcessors; i++){
			//	ret = ::WaitForSingleObject((*m_DrawDatas[i].pThread), INFINITE);
			//	m_DrawDatas[i].pThread = NULL;
			//}

			////create draw thread
			//for(i=0; i<m_numOfProcessors; i++){
			//	m_DrawDatas[i].imgFile = f;
			//	m_DrawDatas[0].num_of_running_thread = (&m_numOfRunningThread);
			//	//m_hDrawThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DrawThread, (LPVOID)&m_DrawData, 0, &thId); 
			//	m_DrawDatas[i].pThread = ::AfxBeginThread((AFX_THREADPROC)DrawThread, (LPVOID)&(m_DrawDatas[i]));
			//}

			//for(i=0; i<m_numOfProcessors; i++){
			//	ret = ::WaitForSingleObject((*m_DrawDatas[i].pThread), INFINITE);
			//	m_DrawDatas[i].pThread = NULL;
			//}

			//f->ClearUpdateData();
			//f->UnlockUpdateData();

#ifdef _DEBUG
			sprintf(str_blt_lap, "Update Draw Thread %f msec\n", timer.elapsed());
			::OutputDebugString(str_blt_lap);
#endif //_DEBUG
		} else {
			DWORD dwExCode = 0;
			if (m_DrawDatas[0].pThread) {
				::GetExitCodeThread(m_DrawDatas[0].pThread, &dwExCode);
			}
			if ((m_DrawDatas[0].pThread == NULL) || (dwExCode != STILL_ACTIVE)) {
				unsigned int thId;
				m_numOfRunningThread = m_numOfProcessors;
				m_DrawDatas[0].imgFile = f;
				m_DrawDatas[0].num_of_running_thread = (&m_numOfRunningThread);
				m_DrawDatas[0].pThread = (HANDLE)_beginthreadex(
					NULL,
					0,
					UpdateDrawThread,
					(void*)&(m_DrawDatas[0]),
					0,
					&thId);
			}
		}
#else
		//シングルスレッド
		m_DrawDatas[0].imgFile = f;
		UpdateDrawThread((LPVOID)&(m_DrawDatas[0]));
#endif //IE_MULTI_THREAD_DRAW

	}
	CATCH(CException, e)
	{
		::OutputDebugString("CImgEdit_UIView::OnDraw excepiton\n");
	}
	END_CATCH
}

int CImgEdit_UIView::OnCreate(LPCREATESTRUCT lpcs)
{
	if(0 != CView::OnCreate(lpcs)){
		return -1;
	}

	if(m_ptDevice.Initialize( GetSafeHwnd() , &g_ImgEdit)){
		OutputError::PushLog(LOG_LEVEL::_INFO, "succeed initialize wintab");
	}
	else{
		OutputError::PushLog(LOG_LEVEL::_WARN, "failed initialize wintab");
	}

	//RECT rc;
	//GetClientRect(&rc);

	//CDC* cdc = this->GetDC();
	//m_DrawData.hBackBmp = ::CreateCompatibleBitmap(cdc->GetSafeHdc(), rc.right - rc.left, rc.bottom - rc.top);
	//m_DrawData.hBackDC = ::CreateCompatibleDC(cdc->GetSafeHdc());
	//this->ReleaseDC(cdc);

	//SYSTEM_INFO si;
	//::GetSystemInfo(&si);
	//m_numOfProcessors = si.dwNumberOfProcessors;
	m_numOfProcessors = 1;
	m_numOfRunningThread = 0;

	m_DrawDatas = new DRAW_DATA[m_numOfProcessors];
	memset(m_DrawDatas, 0, sizeof(DRAW_DATA)*m_numOfProcessors);
	m_DrawDatas[0].draw_view = this;

	return 0;
}

void CImgEdit_UIView::OnDestroy()
{
	//
	CImgEdit_UIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	ImgFile_Ptr f = pDoc->GetImgFile();
	if (f) {
		//unregist event listener
		f->DeleteEventListener( this );
		f->GetRootLayerGroup()->DeleteEventListener( this );
	}

	//
	this->KillTimer(UPDATE_IMAGE_TIMER);

	//
	if (m_DrawDatas) {
		int i;
		for (i=0; i<m_numOfProcessors; i++ ) {
			if (m_DrawDatas[i].pThread) {
				DWORD dwExCode;
				::GetExitCodeThread(m_DrawDatas[i].pThread, &dwExCode);
				if (dwExCode == STILL_ACTIVE) {
					::OutputDebugString("draw thread still exist\n");
					::WaitForSingleObject(m_DrawDatas[i].pThread, INFINITE);
				}
			}
			if (m_DrawDatas[i].hBackBmp) {
				::DeleteObject(m_DrawDatas[i].hBackBmp);
			}
			if (m_DrawDatas[i].hBackDC) {
				::DeleteDC(m_DrawDatas[i].hBackDC);
			}
		}
		delete[] m_DrawDatas;
		m_DrawDatas = NULL;
	}

	CView::OnDestroy();
}

//
void CImgEdit_UIView::OnActivateView(BOOL bActivate, CView *pActivateView, CView *pDeactiveView)
{
	OutputError::PushLog(LOG_LEVEL::_INFO, "CImgEdit_UIView::OnActivateView()");

	CImgEdit_UIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	ImgFile_Ptr f = pDoc->GetImgFile();

	if (bActivate) {
		//set timer for update image
		this->SetTimer(UPDATE_IMAGE_TIMER, 100, NULL);

		if (f) {
			g_ImgEdit.SetActiveFile(f);
		}
	} else {
		this->KillTimer(UPDATE_IMAGE_TIMER);

		int i;
		for (i = 0; i < m_numOfProcessors; i++) {
			if (m_DrawDatas && m_DrawDatas[i].pThread) {
				DWORD dwExCode;
				::GetExitCodeThread(m_DrawDatas[i].pThread, &dwExCode);
				if (dwExCode == STILL_ACTIVE) {
					::OutputDebugString("draw thread still exist\n");
					//::WaitForSingleObject(m_DrawDatas[i].pThread, INFINITE);
				}
			}
		}
	}
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CImgEdit_UIView::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_ptDevice.OnRButtonDown(nFlags, point);

	//this->SendMessage(WM_USER_PUSH_IE_INPUT_DATA, 0, 0);
	this->Invalidate(FALSE);

	CView::OnRButtonDown(nFlags, point);
}

void CImgEdit_UIView::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_ptDevice.OnRButtonUp(nFlags, point);

	//this->SendMessage(WM_USER_PUSH_IE_INPUT_DATA, 0, 0);
	this->Invalidate(FALSE);

	CView::OnRButtonUp(nFlags, point);
}

void CImgEdit_UIView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_isDrag = true;
	m_ptDevice.OnLButtonDown(nFlags, point);

	//this->SendMessage(WM_USER_PUSH_IE_INPUT_DATA, 0, 0);
	this->Invalidate(FALSE);

	CView::OnLButtonDown(nFlags, point);
}

void CImgEdit_UIView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_isDrag = false;
	m_ptDevice.OnLButtonUp(nFlags, point);

	//this->SendMessage(WM_USER_PUSH_IE_INPUT_DATA, 0, 0);
	this->Invalidate(FALSE);

	CView::OnLButtonUp(nFlags, point);
}

void CImgEdit_UIView::OnMouseMove(UINT nFlags, CPoint point)
{
	TRY
	{
		m_ptDevice.OnMouseMove(nFlags, point);
		if (m_isDrag) {
			this->Invalidate(FALSE);
		}
		else {
			if (g_ImgEdit.GetIECursor() == IE_CURSOR_ID::BRUSH_CURSOR) {
				CreateBrushCursor(g_ImgEdit.GetSelectBrush(), point);
			}
		}
	}
	CATCH(CException, e)
	{
		::OutputDebugString("CImgEdit_UIView::OnMouseMove excepiton\n");
	}
	END_CATCH

	CView::OnMouseMove(nFlags, point);
}

void CImgEdit_UIView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	if(nSBCode == SB_THUMBPOSITION || nSBCode == SB_THUMBTRACK){
		CImgEdit_UIDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		if(!pDoc)
			return;
		ImgFile_Ptr f = pDoc->GetImgFile();
		double rot = f->GetViewRot() * CV_PI/180.0;
		double scale (f->GetViewSize() / 100.0);

		CvPoint2D64f pos;

		CvSize img_size = f->GetImgSize();
		int spt_x = (int) nPos;
		int spt_y = GetScrollPos(SB_VERT);
		double dif_x = spt_x / scale;
		double dif_y = spt_y / scale;


		if( f->IsViewFlipH() )
			pos.x = cos(rot) * dif_x - sin(rot) * dif_y;
		else
			pos.x = -cos(rot) * dif_x + sin(rot) * dif_y;

		if( f->IsViewFlipV() )
			pos.y = sin(rot) * dif_x + cos(rot) * dif_y;
		else
			pos.y = -sin(rot) * dif_x - cos(rot) * dif_y;

		f->SetViewPos( &pos );
		LPUPDATE_DATA data = f->CreateUpdateData();
		data->isAll = true;
		f->PushUpdateData( data );

		//if(nSBCode == SB_THUMBPOSITION){
		//	SetScrollPos(SB_HORZ, nPos);
		//}
	}

	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CImgEdit_UIView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	if(nSBCode == SB_THUMBPOSITION ||
		nSBCode == SB_THUMBTRACK){
		CImgEdit_UIDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		if(!pDoc)
			return;
		ImgFile_Ptr f = pDoc->GetImgFile();
		double rot = f->GetViewRot() * CV_PI/180.0;
		double scale (f->GetViewSize() / 100.0);

		CvPoint2D64f pos;

		CvSize img_size = f->GetImgSize();
		int spt_x = GetScrollPos(SB_HORZ);
		int spt_y = (int) nPos;
		double dif_x = spt_x / scale;
		double dif_y = spt_y / scale;

		if( f->IsViewFlipH() )
			pos.x = cos(rot) * dif_x - sin(rot) * dif_y;
		else
			pos.x = -cos(rot) * dif_x + sin(rot) * dif_y;

		if( f->IsViewFlipV() )
			pos.y = sin(rot) * dif_x + cos(rot) * dif_y;
		else
			pos.y = -sin(rot) * dif_x - cos(rot) * dif_y;

		f->SetViewPos( &pos );
		LPUPDATE_DATA data = f->CreateUpdateData();
		data->isAll = true;
		f->PushUpdateData( data );

		//if(nSBCode == SB_THUMBPOSITION){
		//	SetScrollPos(SB_VERT, nPos);
		//}
	}
	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

LRESULT CImgEdit_UIView::OnNcHitTest(CPoint point)
{
	LRESULT lr;

	TRY
	{
		lr = CView::OnNcHitTest(point);

		//ウィンドウの領域内
		if(lr == HTCLIENT){
			if(g_ImgEdit.GetIECursor() == IE_CURSOR_ID::BRUSH_CURSOR){
				SetBrushCorsor();
				return lr;
			}
		}

		SetNormalCorsor();
	}
	CATCH(CException, e)
	{
		::OutputDebugString("CImgEdit_UIView::OnNcHitTest exception\n");
	}
	END_CATCH
	
	return lr;
}

LRESULT CImgEdit_UIView::OnWtPacket(WPARAM wParam, LPARAM lParam)
{
	m_ptDevice.OnWtPacket(wParam, lParam);
	//this->SendMessage(WM_USER_PUSH_IE_INPUT_DATA, 0, 0);
	this->Invalidate(FALSE);

	return TRUE;
}

LRESULT CImgEdit_UIView::OnWtProximity(WPARAM wParam, LPARAM lParam)
{
	m_ptDevice.OnWtProximity(wParam, lParam);

	return TRUE;
}

void CImgEdit_UIView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}

void CImgEdit_UIView::OnSize(UINT nType, int cx, int cy)
{
	CImgEdit_UIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if(!pDoc)
		return;

	//
	RECT rc;
	GetClientRect(&rc);
	ImgFile_Ptr f = pDoc->GetImgFile();
	if(f){
		f->OnWindowReSize(&rc);

		int pre_rc_y=-1;
		int rc_height = (rc.bottom - rc.top)/m_numOfProcessors;

		int i;
		CDC* cdc = this->GetDC();
		for (i=0; i<m_numOfProcessors; i++) {
			if (m_DrawDatas[i].hBackBmp) {
				::DeleteObject(m_DrawDatas[i].hBackBmp);
			}
			if (m_DrawDatas[i].hBackDC) {
				::DeleteDC(m_DrawDatas[i].hBackDC);
			}

			m_DrawDatas[i].draw_disp_rect.left = rc.left;
			m_DrawDatas[i].draw_disp_rect.right = rc.right;
			m_DrawDatas[i].draw_disp_rect.top = pre_rc_y + 1;
			m_DrawDatas[i].draw_disp_rect.bottom = m_DrawDatas[i].draw_disp_rect.top + rc_height;
			pre_rc_y = m_DrawDatas[i].draw_disp_rect.bottom;
			if (i == (m_numOfRunningThread - 1)) {
				m_DrawDatas[i].draw_disp_rect.bottom = rc.bottom;
			}

			m_DrawDatas[i].hBackBmp = ::CreateCompatibleBitmap(
				cdc->GetSafeHdc(),
				m_DrawDatas[i].draw_disp_rect.right - m_DrawDatas[i].draw_disp_rect.left,
				m_DrawDatas[i].draw_disp_rect.bottom - m_DrawDatas[i].draw_disp_rect.top);
			m_DrawDatas[i].hBackDC = ::CreateCompatibleDC(cdc->GetSafeHdc());
		}
		this->ReleaseDC(cdc);


		CvSize img_size = f->GetImgSize();
		pre_rc_y=-1;
		rc_height = img_size.height / m_numOfProcessors;
		for (i=0; i<m_numOfProcessors; i++) {
			m_DrawDatas[i].draw_img_rect.left = 0;
			m_DrawDatas[i].draw_img_rect.right = img_size.width;
			m_DrawDatas[i].draw_img_rect.top = pre_rc_y + 1;
			m_DrawDatas[i].draw_img_rect.bottom = m_DrawDatas[i].draw_img_rect.top + rc_height;
			pre_rc_y = m_DrawDatas[i].draw_img_rect.bottom;
			if (i == (m_numOfRunningThread - 1)) {
				m_DrawDatas[i].draw_img_rect.bottom = rc.bottom;
			}
		}
	}

	//
	AdjastScrollSizes();

	CView::OnSize(nType, cx, cy);
}

void CImgEdit_UIView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == UPDATE_IMAGE_TIMER) {
		this->Invalidate(FALSE);
	}
}

LRESULT CImgEdit_UIView::OnAppMouseWheel(WPARAM wParam, LPARAM lParam)
{
	UINT nFlags = GET_KEYSTATE_WPARAM(wParam);
	short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	CPoint pt;
	pt.x = GET_X_LPARAM(lParam); 
	pt.y = GET_Y_LPARAM(lParam);
	DoMouseWheel(nFlags, zDelta, pt);
	return TRUE;
}

void CImgEdit_UIView::DoMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CImgEdit_UIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if(pDoc){
		ImgFile_Ptr f = pDoc->GetImgFile();
		if(f){
			if(zDelta > 0){
				f->UpViewSize();
			}
			else{
				f->DownViewSize();
			}
		}
	}
}

/*
LRESULT CImgEdit_UIView::OnPushInputData(WPARAM wParam, LPARAM lParam)
{
	IE_INPUT_DATA_EVENT_LIST::iterator itr = m_ptDevice.m_InputDataEvent_List.begin();
	for(; itr != m_ptDevice.m_InputDataEvent_List.end(); itr++){
		switch( (*itr).ievent ){
			case IE_INPUT_DOWN:
				g_ImgEdit.OnMouseLButtonDown( IE_MK_LBUTTON, &((*itr).data) );
				break;
			case IE_INPUT_MOVE:
				g_ImgEdit.OnMouseMove( IE_MK_LBUTTON, &((*itr).data) );
				break;
			case IE_INPUT_UP:
				g_ImgEdit.OnMouseLButtonUp( IE_MK_LBUTTON, &((*itr).data ) );
				break;
			default:
				assert(0);
				break;
		}
	}
	m_ptDevice.m_InputDataEvent_List.clear();
	return 0;
}
*/

// CImgEdit_UIView 診断

#ifdef _DEBUG
void CImgEdit_UIView::AssertValid() const
{
	CView::AssertValid();
}

void CImgEdit_UIView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CImgEdit_UIDoc* CImgEdit_UIView::GetDocument() const // デバッグ以外のバージョンはインラインです。
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImgEdit_UIDoc)));
	return (CImgEdit_UIDoc*)m_pDocument;
}
#endif //_DEBUG


////////////////////////
/*!
	ブラシカーソルを作成する
*/
void CImgEdit_UIView::CreateBrushCursor(IEBrush_Ptr pBrush, CPoint point)
{
	/*
	int i;
	int x,y;
	HBITMAP hBrushCorsorMask = NULL;
	HBITMAP hBrushCorsorBmp = NULL;

	if(!pBrush) return;

	//
	CImgEdit_UIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if(!pDoc)
		return;
	ImgFile_Ptr f = pDoc->GetImgFile();

	CvSize size = pBrush->GetBrushSize();
	IE_INPUT_DATA input_data;
	input_data.x=0;
	input_data.y=0;
	input_data.p=1.0;
	POINT startPt;
	const IplImage* brush_data = pBrush->GetBrushData();
	CvPoint2D64f b_ipoint[4];
	CvPoint2D64f b_wpoint[4];
	b_ipoint[0].x = 0; b_ipoint[0].y = 0;
	b_ipoint[1].x = brush_data->width; b_ipoint[1].y = 0;
	b_ipoint[2].x = brush_data->width; b_ipoint[2].y = brush_data->height;
	b_ipoint[3].x = 0; b_ipoint[3].y = brush_data->height;
	f->ConvertPosImgToWnd(b_ipoint, b_wpoint, 4);
	CvPoint2D64f max_bw;
	CvPoint2D64f min_bw;
	max_bw = min_bw = b_wpoint[0];
	for(i=1; i<4; i++){
		if(min_bw.x > b_wpoint[i].x){
			min_bw.x = b_wpoint[i].x;
		}
		if(min_bw.y > b_wpoint[i].y){
			min_bw.y = b_wpoint[i].y;
		}
		if(max_bw.x < b_wpoint[i].x){
			max_bw.x = b_wpoint[i].x;
		}
		if(max_bw.y < b_wpoint[i].y){
			max_bw.y = b_wpoint[i].y;
		}
	}

	CvSize brush_wsize;
	brush_wsize.width = max_bw.x - min_bw.x;
	brush_wsize.height = max_bw.y - min_bw.y;

	CvSize img_wsize;
	img_wsize.width = (brush_wsize.width < 15*2) ? 15*2 : brush_wsize.width;
	img_wsize.height = (brush_wsize.height < 12*2) ? 12*2 : brush_wsize.height;

	IplImage *brush_wdata = cvCreateImage(img_wsize, IPL_DEPTH_8U, 1);
	IplImage *mask = cvCreateImage(img_wsize, IPL_DEPTH_8U, 3);
	IplImage *brush_cur = cvCreateImage(img_wsize, IPL_DEPTH_8U, 3);

	//
	CvMat* trans_mat = cvCreateMat(2, 3, CV_32FC1);
	int view_size = f->GetViewSize();
	double view_rot = f->GetViewRot();
	CvPoint2D32f brush_center;
	brush_center.x = brush_data->width / 2.0;
	brush_center.y = brush_data->height / 2.0;
	//rot
	cv2DRotationMatrix(brush_center, RAD2DEG(view_rot), 1.0, trans_mat);
	//scale
	cvmSet(trans_mat, 0, 0, (view_size/100.0)*cvmGet(trans_mat, 0, 0));
	cvmSet(trans_mat, 0, 1, (view_size/100.0)*cvmGet(trans_mat, 0, 1));
	cvmSet(trans_mat, 0, 2, (view_size/100.0)*cvmGet(trans_mat, 0, 2));
	cvmSet(trans_mat, 1, 0, (view_size/100.0)*cvmGet(trans_mat, 1, 0));
	cvmSet(trans_mat, 1, 1, (view_size/100.0)*cvmGet(trans_mat, 1, 1));
	cvmSet(trans_mat, 1, 2, (view_size/100.0)*cvmGet(trans_mat, 1, 2));

	cvWarpAffine(brush_data, brush_wdata, trans_mat,
		 CV_INTER_NN | CV_WARP_FILL_OUTLIERS,
		 cvScalar(0));

	//convert mouse cursor from window pos to image pos
	int line_x, line_y;

	UCvPixel pixel;
	UCvPixel zero_px;
	UCvPixel fill_px;

	zero_px.value = 0;
	fill_px.value = 0xffffffff;

	cvSet(brush_cur, cvScalar(0,0,0));
	cvSet(mask, cvScalar(255, 255,255));

	//create cursor image from brush data
	bool checkEdge;
	bool pre_;
	//X軸方向走査
	checkEdge = false;
	for(y=0; y<brush_wsize.height; y++){
		pre_ = false;
		line_y = y + point.y - brush_wsize.height/2;
		for(x=0; x<brush_wsize.width; x++){
			int dat = GetMaskDataPos(brush_wdata, x, y);
			if(dat == 0){
				checkEdge = false;
			}
			else{
				checkEdge = true;
			}

			if(pre_ != checkEdge || (x == brush_wsize.width-1 && checkEdge)){
				//エッジ書き込み
				f->GetDrawImgPixel(x + point.x - brush_wsize.width/2, line_y, &pixel);
				NotPixel(&pixel, &pixel);
				SetPixelToBGR(brush_cur, x, y, &pixel);
				SetPixelToBGR(mask, x, y, &zero_px);
			}

			pre_ = checkEdge;
		}
	}
	//Y軸方向走査
	checkEdge = false;
	for(x=0; x<brush_wsize.width; x++){
		pre_ = false;
		line_x = x + point.x - brush_wsize.width/2;
		for(y=0; y<brush_wsize.height; y++){
			int dat = GetMaskDataPos(brush_wdata, x, y);
			if(dat == 0){
				checkEdge = false;
			}
			else{
				checkEdge = true;
			}

			if(pre_ != checkEdge || (y == brush_wsize.height-1 && checkEdge)){
				//エッジ書き込み
				f->GetDrawImgPixel(line_x, y + point.y - brush_wsize.height/2, &pixel);
				NotPixel(&pixel, &pixel);
				SetPixelToBGR(brush_cur, x, y, &pixel);
				SetPixelToBGR(mask, x, y, &zero_px);
			}

			pre_ = checkEdge;
		}
	}

	CDC* cdc;
	cdc = this->GetDC();
	if( cdc == NULL ) return;

	CvPoint2D64f new_brush_center;
	new_brush_center.x = brush_center.x * cvmGet(trans_mat, 0, 0)
					+ brush_center.y * cvmGet(trans_mat, 0, 1)
					+ cvmGet(trans_mat, 0, 2);
	new_brush_center.y = brush_center.y * cvmGet(trans_mat, 1, 0)
					+ brush_center.y * cvmGet(trans_mat, 1, 1)
					+ cvmGet(trans_mat, 1, 2);
	//add cursor
	CvPoint **pts;
	int npts[] = {4};
	pts = (CvPoint**) cvAlloc(sizeof(CvPoint*) * 1);
	pts[0] = (CvPoint*) cvAlloc(sizeof(CvPoint) * 4);
	pts[0][0].x = new_brush_center.x + 0; pts[0][0].y = new_brush_center.y + 0;
	pts[0][1].x = new_brush_center.x + 0; pts[0][1].y = new_brush_center.y + 14;
	pts[0][2].x = new_brush_center.x + 6; pts[0][2].y = new_brush_center.y + 11;
	pts[0][3].x = new_brush_center.x + 11; pts[0][3].y = new_brush_center.y + 11;
	cvFillPoly(brush_cur, pts, npts, 1, CV_RGB (255,255,255));
	cvPolyLine(brush_cur, pts, npts, 1, 1, CV_RGB (0, 0, 0));
	cvFillPoly(mask, pts, npts, 1, CV_RGB (0,0,0));
	cvFree(&pts[0]);
	cvFree(pts);

	//HBITMAPへ変換
	BITMAPINFO maskBmi;
	BITMAPINFO brush_curBmi;
	void *pMaskBit;
	void *pBrush_curBit;
	ConvertIPLImageToDIB(mask, &maskBmi, &pMaskBit);
	ConvertIPLImageToDIB(brush_cur, &brush_curBmi, &pBrush_curBit);
	hBrushCorsorMask = CreateDIBitmap(cdc->GetSafeHdc(), &(maskBmi.bmiHeader), CBM_INIT, pMaskBit, &maskBmi, DIB_RGB_COLORS);
	hBrushCorsorBmp = CreateDIBitmap(cdc->GetSafeHdc(), &(brush_curBmi.bmiHeader), CBM_INIT, pBrush_curBit, &brush_curBmi, DIB_RGB_COLORS);



	HCURSOR hAlphaCursor = NULL;
	ICONINFO ii;
	ii.fIcon = FALSE; //カーソル
	ii.xHotspot = new_brush_center.x;
	ii.yHotspot = new_brush_center.y;
	ii.hbmMask = hBrushCorsorMask;
	ii.hbmColor = hBrushCorsorBmp;

	ResourceLock( IE_UI_VIEW_RESOURCE_ID::IE_BRUSH_CORSOR );
	if(m_hBrushCursor){
		HCURSOR hOldCursor = (HCURSOR)SetClassLongPtr(this->GetSafeHwnd(), GCLP_HCURSOR, (LONG)::LoadCursor(NULL, IDC_ARROW));
		::DestroyIcon(m_hBrushCursor);
	}
	m_hBrushCursor = CreateIconIndirect(&ii);
	ResourceUnlock( IE_UI_VIEW_RESOURCE_ID::IE_BRUSH_CORSOR );

	this->ReleaseDC(cdc);

	DeleteObject(hBrushCorsorMask);
	DeleteObject(hBrushCorsorBmp);

	cvReleaseMat(&trans_mat);
	cvReleaseImage(&brush_wdata);
	cvReleaseImage(&mask);
	cvReleaseImage(&brush_cur);
	*/
}

////////////////////////
/*!
	ブラシカーソルに変更する
*/
bool CImgEdit_UIView::SetBrushCorsor()
{
	if(!m_hBrushCursor) return false;

	ResourceLock( IE_UI_VIEW_RESOURCE_ID::IE_BRUSH_CORSOR );
	HCURSOR hOldCursor = (HCURSOR)SetClassLongPtr(this->GetSafeHwnd(), GCLP_HCURSOR, (LONG)m_hBrushCursor);
	ResourceUnlock( IE_UI_VIEW_RESOURCE_ID::IE_BRUSH_CORSOR );
	if(hOldCursor == NULL){
		//OutputError::Alert("IECoursorMG::SetBrushCoursor() カーソル設定エラー");
		return false;
	}

	return true;
}

void CImgEdit_UIView::SetNormalCorsor()
{
	HCURSOR hOldCursor = (HCURSOR)SetClassLongPtr(this->GetSafeHwnd(), GCLP_HCURSOR, (LONG)::LoadCursor(NULL, IDC_ARROW));
}

void CImgEdit_UIView::AdjastScrollSizes()
{
	CImgEdit_UIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if(!pDoc)
		return;
	ImgFile_Ptr f = pDoc->GetImgFile();
	if(f == NULL) return;

	CvSize img_size = f->GetImgSize();
	CvPoint2D64f i_pt[4];
	CvPoint2D64f w_pt[4];
	i_pt[0].x = 0;				i_pt[0].y = 0;
	i_pt[1].x = img_size.width;	i_pt[1].y = 0;
	i_pt[2].x = img_size.width;	i_pt[2].y = img_size.height;
	i_pt[3].x = 0;				i_pt[3].y = img_size.height;
	f->ConvertPosImgToWnd(i_pt, w_pt, 4);

	//find minimum maximum value
	int i;
	int min_y = w_pt[0].y, max_y = w_pt[0].y;
	int min_x = w_pt[0].x, max_x = w_pt[0].x;
	for(i=1; i<4; i++){
		if(w_pt[i].x < min_x){
			min_x = w_pt[i].x;
		}
		if(w_pt[i].x > max_x){
			max_x = w_pt[i].x;
		}
		if(w_pt[i].y < min_y){
			min_y = w_pt[i].y;
		}
		if(w_pt[i].y > max_y){
			max_y = w_pt[i].y;
		}
	}

	//

	//CClientDC dc(NULL);
	//OnPrepareDC(&dc);
	//CSize sizeDoc;
	//sizeDoc.cx = (max_x - min_x)*3;
	//sizeDoc.cy = (max_y - min_y)*3;
	//dc.LPtoDP(&sizeDoc);
	//SetScrollSizes(MM_TEXT, sizeDoc);
	//SetScrollRange(SB_HORZ, 0, (max_x - min_x)*3, FALSE);
	//SetScrollRange(SB_VERT, 0, (max_y - min_y)*3, FALSE);

	RECT rc;
	GetClientRect(&rc);

	int d_img_w = max_x - min_x;
	int d_img_h = max_y - min_y;

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_DISABLENOSCROLL | SIF_RANGE | SIF_PAGE;

	si.nPage = rc.right - rc.left;
	si.nMin = -d_img_w;
	si.nMax = 2 * d_img_w;
	SetScrollInfo(SB_HORZ, &si, FALSE);

	si.nPage = rc.bottom - rc.top;
	si.nMin = -d_img_h;
	si.nMax = 2 * d_img_h;
	SetScrollInfo(SB_VERT, &si, FALSE);
}

void CImgEdit_UIView::AdjastScrollPos()
{
	CImgEdit_UIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if(!pDoc)
		return;

	ImgFile_Ptr f = pDoc->GetImgFile();
	double scale = f->GetViewSize() / 100.0;
	double inv_scale = 100.0 / f->GetViewSize();
	double inv_rot = -f->GetViewRot() * CV_PI/180.0;

	CvPoint2D64f pos;
	f->GetViewPos(&pos);
	double pos_x,pos_y;
	if( f->IsViewFlipH() )
		pos_x = cos(inv_rot) * pos.x - sin(inv_rot) * pos.y;
	else
		pos_x = -cos(inv_rot) * pos.x + sin(inv_rot) * pos.y;

	if( f->IsViewFlipV() )
		pos_y = sin(inv_rot) * pos.x + cos(inv_rot) * pos.y;
	else
		pos_y = -sin(inv_rot) * pos.x - cos(inv_rot) * pos.y;

	//SetScrollPos(SB_HORZ, pos_x*inv_scale);
	//SetScrollPos(SB_VERT, pos_y*inv_scale);
	SetScrollPos(SB_HORZ, pos_x * scale);
	SetScrollPos(SB_VERT, pos_y * scale);
	//SetScrollPos(SB_HORZ, pos_x);
	//SetScrollPos(SB_VERT, pos_y);
}