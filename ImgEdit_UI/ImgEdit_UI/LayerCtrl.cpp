#include "stdafx.h"

#include "LayerCtrl.h"
#include "..\resource1.h"

#define BTN_W 42
#define BTN_H 26

#define ID_NEW_LAYER_BTN 1
#define ID_NEW_LAYER_GROUP_BTN 2
#define ID_DELETE_LAYER_BTN 3

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern ImgEdit g_ImgEdit;


CLayerCtrl::CLayerCtrl()
{
}

CLayerCtrl::~CLayerCtrl()
{
}

BOOL CLayerCtrl::PreTranslateMessage(MSG *pMsg)
{
	m_ToolTip.RelayEvent( pMsg );
	return CWnd::PreTranslateMessage( pMsg );
}

BEGIN_MESSAGE_MAP(CLayerCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_BN_CLICKED(ID_NEW_LAYER_BTN, OnBnClickedNewLayer)
	ON_BN_CLICKED(ID_NEW_LAYER_GROUP_BTN, OnBnClickedNewLayerGroup)
	ON_BN_CLICKED(ID_DELETE_LAYER_BTN, OnBnClickedDeleteLayer)
END_MESSAGE_MAP()

int CLayerCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CWnd::OnCreate(lpCreateStruct)){
		return -1;
	}

	RECT rc;

	//create new layer btn
	rc.top = 2;
	rc.left = 3;
	rc.right = rc.left + BTN_W;
	rc.bottom = rc.top + BTN_H;
	m_newLayerBtn.Create(
		NULL,
		WS_VISIBLE | WS_CHILD | BS_OWNERDRAW | BS_PUSHBUTTON,
		rc,
		this,
		ID_NEW_LAYER_BTN);
	m_newLayerBtn.LoadBitmaps(
		IDB_NEW_LAYER_A,
		IDB_NEW_LAYER_B,
		IDB_NEW_LAYER_C);

	//create new layer btn
	rc.left = rc.right + 1;
	rc.right = rc.left + BTN_W;
	m_newLayerGroupBtn.Create(
		NULL,
		WS_VISIBLE | WS_CHILD | BS_OWNERDRAW | BS_PUSHBUTTON,
		rc,
		this,
		ID_NEW_LAYER_GROUP_BTN);
	m_newLayerGroupBtn.LoadBitmaps(
		IDB_NEW_LAYER_GROUP_A,
		IDB_NEW_LAYER_GROUP_B,
		IDB_NEW_LAYER_GROUP_C);

	//create delete layer btn
	rc.left = rc.right + 1;
	rc.right = rc.left + BTN_W;
	m_deleteLayerBtn.Create(
		NULL,
		WS_VISIBLE | WS_CHILD | BS_OWNERDRAW | BS_PUSHBUTTON,
		rc,
		this,
		ID_DELETE_LAYER_BTN);
	m_deleteLayerBtn.LoadBitmaps(
		IDB_DELETE_LAYER_A,
		IDB_DELETE_LAYER_B,
		IDB_DELETE_LAYER_C);

	m_ToolTip.Create( this );
	m_ToolTip.AddTool(&m_newLayerBtn, TEXT("新規レイヤー作成"));
	m_ToolTip.AddTool(&m_newLayerGroupBtn, TEXT("新規レイヤーグループ作成"));
	m_ToolTip.AddTool(&m_deleteLayerBtn, TEXT("選択レイヤー削除"));

	return 0;
}

void CLayerCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
}

void CLayerCtrl::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetWindowRect(rect);
	ScreenToClient(rect);

	rect.InflateRect(1, 1);
	dc.FillSolidRect(rect, ::GetSysColor(COLOR_WINDOW));
}

void CLayerCtrl::OnBnClickedNewLayer()
{
	//add new layer
	ImgFile_Ptr f = g_ImgEdit.GetActiveFile();
	if (f) {
		AddNewLayerHandle* handle = (AddNewLayerHandle*)f->CreateImgFileHandle(IFH_ADD_NEW_LAYER);
		f->DoImgFileHandle( handle );

		f->SetSelectLayer( handle->GetNewLayer().lock() );
	}
}

void CLayerCtrl::OnBnClickedNewLayerGroup()
{
	//add new layer group
	ImgFile_Ptr f = g_ImgEdit.GetActiveFile();
	if (f) {
		AddNewLayerGroupHandle* handle = (AddNewLayerGroupHandle*)f->CreateImgFileHandle(IFH_ADD_NEW_LAYER_GROUP);
		f->DoImgFileHandle( handle );

		f->SetSelectLayer( handle->GetNewLayeyGroup().lock() );
	}
}

void CLayerCtrl::OnBnClickedDeleteLayer()
{
	//delete select layer
	ImgFile_Ptr f = g_ImgEdit.GetActiveFile();
	if (f) {
		IImgLayer_weakPtr select_layer = f->GetSelectLayer();
		if (!select_layer.expired()) {
			RemoveLayerHandle* handle = (RemoveLayerHandle*) f->CreateImgFileHandle(IFH_REMOVE_LAYER);
			handle->SetRemoveLayer( select_layer );
			f->DoImgFileHandle( handle );
		}
	}
}