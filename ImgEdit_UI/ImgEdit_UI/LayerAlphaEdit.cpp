#include "stdafx.h"
#include "LayerAlphaEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern ImgEdit g_ImgEdit;

CLayerAlphaEdit::CLayerAlphaEdit()
{
	m_pActiveFile = NULL;
	m_pChangeOpacityHandle = NULL;
}

CLayerAlphaEdit::~CLayerAlphaEdit()
{
}

void CLayerAlphaEdit::OnChangeActiveFile(ImgFile_Ptr pNewFile, ImgFile_Ptr pOldFile)
{
	if (m_pActiveFile) {
		m_pActiveFile->DeleteEventListener( this );
	}
	if (pNewFile) {
		pNewFile->AddEventListener( this );
	}

	m_pActiveFile = pNewFile;
	if (m_pActiveFile) {
		IImgLayer_weakPtr pLayer = m_pActiveFile->GetSelectLayer();
		if (!pLayer.expired()) {
			m_LayerAlphaSlider.SetPos(pLayer.lock()->GetOpacityPer());
		}
	}
}

//////////////////////////////////
/*!
	レイヤー選択時イベント
*/
void CLayerAlphaEdit::OnSelectLayer(IImgLayer_weakPtr pLayer)
{
	assert(m_pChangeOpacityHandle == NULL);
	if (m_pChangeOpacityHandle) {
		OutputError::PushLog(LOG_LEVEL::_ERROR, "change opacity handle, CLayerAlphaEdit");
	}

	if (!pLayer.expired()) {
		m_LayerAlphaSlider.SetPos( pLayer.lock()->GetOpacityPer() );
	}
}

void CLayerAlphaEdit::OnChangedLayerOpacity(uint8_t new_opacity)
{
	IImgLayer_weakPtr pLayer = m_pActiveFile->GetSelectLayer();
	if (!pLayer.expired()) {
		m_LayerAlphaSlider.SetPos( pLayer.lock()->GetOpacityPer() );
	}
}

BEGIN_MESSAGE_MAP(CLayerAlphaEdit, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

int CLayerAlphaEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CWnd::OnCreate(lpCreateStruct)){
		return -1;
	}

	//register event listener
	g_ImgEdit.AddEventListener( this );

	//スライダー追加
	RECT rc;
	rc.top = rc.left = 0;
	rc.right = lpCreateStruct->cx;
	rc.bottom = lpCreateStruct->cy;
	int width = lpCreateStruct->cx - m_LayerAlphaSlider.GetUseWidth();
	if(!m_LayerAlphaSlider.CreateEditSliderCtrl(
		width*0.20, 12,
		width*0.65, lpCreateStruct->cy,
		width*0.15, 10, rc, this, 1)){
		return -1;
	}
	m_LayerAlphaSlider.SetLabel("不透明度");
	m_LayerAlphaSlider.SetRange(0, 100);
	return 0;
}

void CLayerAlphaEdit::OnDestroy()
{
	//unregister event listener
	if (m_pActiveFile) {
		m_pActiveFile->DeleteEventListener( this );
	}
	g_ImgEdit.DeleteEventListener( this );

	CWnd::OnDestroy();
}

void CLayerAlphaEdit::OnSize(UINT nType, int cx, int cy)
{
	int width = cx - m_LayerAlphaSlider.GetUseWidth();
	m_LayerAlphaSlider.SetSize(width*0.20, 12, width*0.65, cy, width*0.15, 10);
	CWnd::OnSize(nType, cx, cy);
}

void CLayerAlphaEdit::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetWindowRect(rect);
	ScreenToClient(rect);

	rect.InflateRect(1, 1);
	dc.FillSolidRect(rect, ::GetSysColor(COLOR_WINDOW));
}

void CLayerAlphaEdit::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	if ((void*)pScrollBar == (void*)(&m_LayerAlphaSlider)) {
		if (m_pSelectLayer == NULL) return;

		ImgFile_Ptr pFile = m_pSelectLayer->GetParentFile().lock();
		assert(pFile != NULL);
		if (pFile == NULL) return;

		if (nSBCode == TB_THUMBTRACK) {
			if (m_pChangeOpacityHandle == NULL) {
				//init handle
				m_pChangeOpacityHandle = (ChangeLayerOpacityHandle*) pFile->CreateImgFileHandle( IFH_CHANGE_LAYER_OPACITY);
				m_pChangeOpacityHandle->SetEditLayer(m_pSelectLayer);
			}

			uint8_t opacity = (nPos/100.0) * 255;
			m_pSelectLayer->SetOpacity(opacity);
			if (m_pChangeOpacityHandle) {
				m_pChangeOpacityHandle->SetChangeOpacity(opacity);
				m_pChangeOpacityHandle->Do(pFile);
			}
		}
		else if (nSBCode == TB_ENDTRACK) {
			if (m_pChangeOpacityHandle) {
				//set handle to history
				pFile->DoImgFileHandle(m_pChangeOpacityHandle);
				m_pChangeOpacityHandle = NULL;
			}
		}
	}
}