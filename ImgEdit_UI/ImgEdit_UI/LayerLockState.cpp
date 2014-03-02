#include "stdafx.h"

#include "LayerLockState.h"
#include "..\resource1.h"

#define LABEL_ID 1
#define LABEL_X 5
#define LABEL_W 60
#define LABEL_H 24

#define LOCK_ALPHA_BTN_ID 2
#define LOCK_ALPHA_BTN_X 70
#define LOCK_ALPHA_BTN_W 24
#define LOCK_ALPHA_BTN_H 24

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern ImgEdit g_ImgEdit;


CLayerLockState::CLayerLockState()
{
	m_pSelectFile = NULL;
}

CLayerLockState::~CLayerLockState()
{
}

void CLayerLockState::OnChangeActiveFile(ImgFile_Ptr pNewFile, ImgFile_Ptr pOldFile)
{
	if (m_pSelectFile) {
		m_pSelectFile->DeleteEventListener( this );
	}
	if (pNewFile) {
		pNewFile->AddEventListener( this );
	}

	m_pSelectFile = pNewFile;
	if (m_pSelectFile) {
		IImgLayer_weakPtr _wpLayer = m_pSelectFile->GetSelectLayer();
		if (!_wpLayer.expired()) {
			IImgLayer_Ptr _pLayer = _wpLayer.lock();
			if (_pLayer->GetLayerType() == IE_LAYER_TYPE::NORMAL_LAYER) {
				ImgLayer_Ptr pLayer = std::dynamic_pointer_cast<ImgLayer>(_pLayer);
				if (pLayer->IsLockPixelAlpha()) {
					m_LockAlphaBtn.SetToggle(true);
					return;
				}
			}
		}
	}
	m_LockAlphaBtn.SetToggle(false);
}

//////////////////////////////////
/*!
	レイヤー選択時イベント
*/
void CLayerLockState::OnSelectLayer(IImgLayer_weakPtr _wpLayer)
{
	IImgLayer_Ptr _pLayer = _wpLayer.lock();
	if(_pLayer){
		if(_pLayer->GetLayerType() == IE_LAYER_TYPE::NORMAL_LAYER){
			ImgLayer_Ptr pLayer = std::dynamic_pointer_cast<ImgLayer>(_pLayer);
			if(pLayer->IsLockPixelAlpha()){
				m_LockAlphaBtn.SetToggle(true);
				return;
			}
		}
	}
	m_LockAlphaBtn.SetToggle(false);
}

BEGIN_MESSAGE_MAP(CLayerLockState, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_BN_CLICKED(LOCK_ALPHA_BTN_ID, OnBnClickedLockAlphaBtn)
END_MESSAGE_MAP()

int CLayerLockState::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CWnd::OnCreate(lpCreateStruct)){
		return -1;
	}

	//regist event listener
	g_ImgEdit.AddEventListener( this);

	//
	RECT rc;
	rc.top = (lpCreateStruct->cy - LABEL_H)/2;
	rc.bottom = rc.top + LABEL_H;
	rc.left = LABEL_X;
	rc.right = rc.left + LABEL_W;
	if(!m_Label.Create("ロック:", WS_CHILD | WS_VISIBLE | SS_LEFT, rc, this, LABEL_ID)){
		return -1;
	}

	//
	rc.top = (lpCreateStruct->cy - LOCK_ALPHA_BTN_H)/2;
	rc.bottom = rc.top + LOCK_ALPHA_BTN_H;
	rc.left = LOCK_ALPHA_BTN_X;
	rc.right = rc.left + LOCK_ALPHA_BTN_W;
	if(!m_LockAlphaBtn.Create("",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		BS_OWNERDRAW | BS_PUSHBUTTON,
		rc, this, LOCK_ALPHA_BTN_ID))
	{
		return -1;
	}
	m_LockAlphaBtn.LoadBitmaps(
		IDB_LOCK_LAYER_ALPHA_A,
		IDB_LOCK_LAYER_ALPHA_B,
		IDB_LOCK_LAYER_ALPHA_C);

	//regist tool tip
	m_ToolTip.Create( this );
	m_ToolTip.AddTool(&m_LockAlphaBtn, TEXT("不透明度保護"));
	return 0;
}

void CLayerLockState::OnDestroy()
{
	//unregist event listener
	if (m_pSelectFile) {
		m_pSelectFile->DeleteEventListener( this );
	}
	g_ImgEdit.DeleteEventListener( this );

	CWnd::OnDestroy();
}

void CLayerLockState::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
}

void CLayerLockState::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetWindowRect(rect);
	ScreenToClient(rect);

	rect.InflateRect(1, 1);
	dc.FillSolidRect(rect, ::GetSysColor(COLOR_WINDOW));
}

void CLayerLockState::OnBnClickedLockAlphaBtn()
{
	IImgLayer_weakPtr _wpLayer = m_pSelectFile->GetSelectLayer();
	if (!_wpLayer.expired()) {
		IImgLayer_Ptr _pLayer = _wpLayer.lock();
		if (m_pSelectLayer->GetLayerType() == IE_LAYER_TYPE::NORMAL_LAYER) {
			ImgLayer_Ptr pLayer = std::dynamic_pointer_cast<ImgLayer>(_pLayer);

			if (m_LockAlphaBtn.GetToggle()) {
				m_LockAlphaBtn.SetToggle(false);
				pLayer->SetLockPixelAlpha(false);
			}
			else {
				m_LockAlphaBtn.SetToggle(true);
				pLayer->SetLockPixelAlpha(true);
			}
		}
	}
}