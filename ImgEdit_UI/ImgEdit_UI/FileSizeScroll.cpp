#include "stdafx.h"

#include "FileSizeScroll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern ImgEdit g_ImgEdit;

CFileSizeScroll::CFileSizeScroll()
{
	m_pActiveFile = NULL;
}

CFileSizeScroll::~CFileSizeScroll()
{
	_ASSERTE(_CrtCheckMemory());
}

void CFileSizeScroll::OnChangeActiveFile(ImgFile_Ptr pNewFile, ImgFile_Ptr pOldFile)
{
	if (m_pActiveFile) {
		m_pActiveFile->DeleteEventListener( this );
	}
	if (pNewFile) {
		pNewFile->AddEventListener( this );
	}

	m_pActiveFile = pNewFile;
	if(m_pActiveFile){
		m_FileSizeSlider.SetPos( m_pActiveFile->GetViewSize() );
	}
}

void CFileSizeScroll::OnChangeViewSize(int size)
{
	m_FileSizeSlider.SetPos(size);
}

BEGIN_MESSAGE_MAP(CFileSizeScroll, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

int CFileSizeScroll::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CWnd::OnCreate(lpCreateStruct)){
		return -1;
	}

	//register event listener
	g_ImgEdit.AddEventListener( this );

	//スライダー追加
	RECT rc;
	rc.top = 0;
	rc.left = 0;
	rc.right = lpCreateStruct->cx;
	rc.bottom = lpCreateStruct->cy;
	int width = lpCreateStruct->cx - m_FileSizeSlider.GetUseWidth();
	if(!m_FileSizeSlider.CreateEditSliderCtrl(
		width*0.20, 12,
		width*0.65, lpCreateStruct->cy,
		width*0.15, 10, rc, this, 1)){
		return -1;
	}
	m_FileSizeSlider.SetLabel("表示サイズ");
	m_FileSizeSlider.SetRange(1, 3200);
	m_FileSizeSlider.SetTic(1);
	m_FileSizeSlider.SetTic(25);
	m_FileSizeSlider.SetTic(33);
	m_FileSizeSlider.SetTic(50);
	m_FileSizeSlider.SetTic(100);
	m_FileSizeSlider.SetTic(150);
	m_FileSizeSlider.SetTic(200);
	m_FileSizeSlider.SetTic(300);
	m_FileSizeSlider.SetTic(400);
	m_FileSizeSlider.SetTic(600);
	m_FileSizeSlider.SetTic(800);
	m_FileSizeSlider.SetTic(1200);
	m_FileSizeSlider.SetTic(1600);
	m_FileSizeSlider.SetTic(2400);
	m_FileSizeSlider.SetTic(3200);
	return 0;
}

void CFileSizeScroll::OnDestroy()
{
	//unregister event listener
	if (m_pActiveFile) {
		m_pActiveFile->DeleteEventListener( this );
	}

	g_ImgEdit.DeleteEventListener( this );

	CWnd::OnDestroy();
}

void CFileSizeScroll::OnSize(UINT nType, int cx, int cy)
{
	int width = cx - m_FileSizeSlider.GetUseWidth();
	m_FileSizeSlider.SetSize(width*0.20, 12, width*0.65, cy, width*0.15, 10);
	CWnd::OnSize(nType, cx, cy);
}

void CFileSizeScroll::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetWindowRect(rect);
	ScreenToClient(rect);

	rect.InflateRect(1, 1);
	dc.FillSolidRect(rect, ::GetSysColor(COLOR_WINDOW));
}

void CFileSizeScroll::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	if(nSBCode == TB_THUMBTRACK){
		if((void*)pScrollBar == (void*)(&m_FileSizeSlider)){
			if(m_pActiveFile){
				m_pActiveFile->SetViewSize(nPos);
			
				LPUPDATE_DATA data = m_pActiveFile->CreateUpdateData();
				data->isAll = true;
				m_pActiveFile->PushUpdateData(data);
			}
		}
	}
}