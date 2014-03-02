#include "stdafx.h"

#include "MaskCtrl.h"
#include "..\resource1.h"

extern ImgEdit g_ImgEdit;

#define BTN_W 42
#define BTN_H 26

#define ID_NEW_MASK_BTN 1
#define ID_DELETE_MASK_BTN 2

CMaskCtrl::CMaskCtrl()
{
}

CMaskCtrl::~CMaskCtrl()
{
}

BEGIN_MESSAGE_MAP(CMaskCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_BN_CLICKED(ID_NEW_MASK_BTN, OnBnClickedNewMask)
	ON_BN_CLICKED(ID_DELETE_MASK_BTN, OnBnClickedDeleteMask)
END_MESSAGE_MAP()

int CMaskCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CWnd::OnCreate(lpCreateStruct)){
		return -1;
	}

	RECT rc;
	return 0;
}

void CMaskCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
}

void CMaskCtrl::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetWindowRect(rect);
	ScreenToClient(rect);

	rect.InflateRect(1, 1);
	dc.FillSolidRect(rect, ::GetSysColor(COLOR_WINDOW));
}

void CMaskCtrl::OnBnClickedNewMask()
{
	////add new layer
	//ImgFile_Ptr f = g_ImgEdit.GetActiveFile();
	//if(f){
	//	AddNewLayerHandle* handle = (AddNewLayerHandle*)ImgFileHandleMG::CreateImgFileHandle( f, IFH_ADD_NEW_LAYER );
	//	int index = f->GetSelectLayerIndex();
	//	handle->SetAddToIndex( index + 1 );
	//	f->DoImgFileHandle( handle );

	//	f->SetSelectLayer( index + 1 );
	//}
}

void CMaskCtrl::OnBnClickedDeleteMask()
{
	////delete select layer
	//ImgFile_Ptr f = g_ImgEdit.GetActiveFile();
	//if(f){
	//	ImgLayer_Ptr select_layer = f->GetSelectLayer();
	//	f->RemoveLayer( select_layer );
	//}
}