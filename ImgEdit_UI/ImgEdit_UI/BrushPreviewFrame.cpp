#include "stdafx.h"

#include "BrushPreviewFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern ImgEdit g_ImgEdit;

CBrushPreviewFrame::CBrushPreviewFrame()
{
	m_pViewBrush = NULL;
	m_pViewBrushGroup = NULL;
	m_PreviewImg = NULL;
}

CBrushPreviewFrame::~CBrushPreviewFrame()
{
	if (m_pViewBrush) {
		m_pViewBrush->DeleteEventListener( this );
	}

	if (m_pViewBrushGroup) {
		m_pViewBrushGroup->DeleteEventListener( this );
	}

	if (m_PreviewImg) {
		cvReleaseImage(&m_PreviewImg);
	}

	_ASSERTE(_CrtCheckMemory());
}

void CBrushPreviewFrame::OnChangeOrgRadius(int old_rad, int new_rad)
{
	UpdatePreviewImg(m_pViewBrush);
	this->Invalidate(FALSE);
}

void CBrushPreviewFrame::OnChangeBrushShape()
{
	UpdatePreviewImg(m_pViewBrush);
	this->Invalidate(FALSE);
}

void CBrushPreviewFrame::OnChangeBrushStroke()
{
	UpdatePreviewImg(m_pViewBrush);
	this->Invalidate(FALSE);
}

////////////////////////////////
/*!
	選択ブラシが変わったときに呼び出す。
	@param[in] pNewBrush 変更後のブラシ
	@param[in] pOldBrush 変更前のブラシ
*/
void CBrushPreviewFrame::OnChangeSelectBrush(IEBrush_Ptr pNewBrush, IEBrush_Ptr pOldBrush)
{
	if (pOldBrush) {
		pOldBrush->DeleteEventListener( this );
	}

	if (pNewBrush) {
		pNewBrush->AddEventListener( this );
	}

	m_pViewBrush = pNewBrush;
	UpdatePreviewImg(pNewBrush);
	this->Invalidate(FALSE);
}

void CBrushPreviewFrame::SetBrushGroup(IEBrushGroup_Ptr pBGroup)
{
	if (m_pViewBrushGroup) {
		m_pViewBrushGroup->DeleteEventListener( this );
	}
	pBGroup->AddEventListener( this );
	m_pViewBrushGroup = pBGroup;

	//
	m_pViewBrush = m_pViewBrushGroup->GetSelectBrush();
	UpdatePreviewImg(m_pViewBrush);
	this->Invalidate(FALSE);
}

BEGIN_MESSAGE_MAP(CBrushPreviewFrame, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

int CBrushPreviewFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (0 != CWnd::OnCreate(lpCreateStruct)) {
		return -1;
	}

	//
	UpdatePreviewImg( g_ImgEdit.GetSelectBrush() );
	return 0;
}

void CBrushPreviewFrame::OnSize(UINT nType, int cx, int cy)
{
	if(m_PreviewImg){
		cvReleaseImage(&m_PreviewImg);
	}
	m_PreviewImg = cvCreateImage(cvSize(cx, cy), IPL_DEPTH_8U, 3);
	UpdatePreviewImg( g_ImgEdit.GetSelectBrush() );
	
	CWnd::OnSize(nType, cx, cy);
}

void CBrushPreviewFrame::OnPaint()
{
	if(m_PreviewImg){
		CPaintDC dc(this);
	
		IPLImageToDevice(
			dc.GetSafeHdc(),
			0, 0,
			m_PreviewImg->width,
			m_PreviewImg->height,
			m_PreviewImg,
			0, 0);
	}
}

void CBrushPreviewFrame::UpdatePreviewImg(IEBrush_Ptr pBrush)
{
	if(m_PreviewImg == NULL) return;
	if(pBrush){
		pBrush->DrawBrushPreview( m_PreviewImg );
	}
	else{
		cvSet(m_PreviewImg, cvScalar(255,255,255));
	}
}