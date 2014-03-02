#include "stdafx.h"

#include "../resource1.h"
#include "LayerGroupPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CLayerGroupPanel::CLayerGroupPanel(HWND hParent) :
	CLayerPanel(hParent)
{
	m_OpenAicon.LoadBitmap(MAKEINTRESOURCE(IDB_OPEN_LAYER_GROUP_PANEL));
	m_CloseAicon.LoadBitmap(MAKEINTRESOURCE(IDB_CLOSE_LAYER_GROUP_PANEL));
}

void CLayerGroupPanel::PopupMenu(HWND hParent, const LPPOINT lpt){
	//create and display menu
	IEMENU hmenu = IEMenuMG::CreateIEPopupMenu();	

	IEMenuItemInfo ieii;
	ieii.SetMask(MIIM_STRING | MIIM_ID | MIIM_STATE );
	ieii.SetState( MFS_ENABLED );

	ieii.SetTypeData(TEXT("レイヤーを複製する"));
	ieii.SetID( LW_CM_COPY_LAYER );
	IEMenuMG::InsertIEMenuItem(hmenu, 0, TRUE, &ieii);

	ieii.SetTypeData(TEXT("レイヤーを削除する"));
	ieii.SetID( LW_CM_DELETE_LAYER );
	IEMenuMG::InsertIEMenuItem(hmenu, 1, TRUE, &ieii);

	//
	ieii.SetTypeData(TEXT("レイヤーグループを結合"));
	ieii.SetID( LW_CM_MERGE_LAYER_GROUP );
	IEMenuMG::InsertIEMenuItem(hmenu, 2, TRUE, &ieii);

	//
	ieii.SetTypeData(TEXT("表示レイヤーを結合"));
	ieii.SetID( LW_CM_MERGE_DISPLAY_LAYER );
	IEMenuMG::InsertIEMenuItem(hmenu, 3, TRUE, &ieii);

	IEMenuMG::TrackPopupIEMenu(hmenu, TPM_LEFTALIGN|TPM_TOPALIGN, lpt->x, lpt->y, 0, hParent, NULL);
	IEMenuMG::DestroyIEMenu(hmenu);
}

void CLayerGroupPanel::DrawPanel(CDC* pDC)
{
	CLayerPanel::DrawPanel(pDC);

	CBitmap* cOldBmp = NULL;
	CDC cMemDC;
	cMemDC.CreateCompatibleDC(NULL);

	ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(m_Layer);
	if (pLGroup->IsOpen()) {
		cOldBmp = cMemDC.SelectObject(&m_OpenAicon);
	} else {
		cOldBmp = cMemDC.SelectObject(&m_CloseAicon);
	}

	pDC->BitBlt(
		m_LayerThmRect.left,
		m_LayerThmRect.top,
		m_LayerThmRect.right - m_LayerThmRect.left,
		m_LayerThmRect.bottom - m_LayerThmRect.top,
		&cMemDC,
		0,
		0,
		SRCCOPY);
	cMemDC.SelectObject(cOldBmp);
}

void CLayerGroupPanel::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (isInRect(point.x, point.y, &m_LayerThmRect)) {
		assert(m_Layer);
		assert(m_Layer->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER);

		ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(m_Layer);
		bool bl = pLGroup->IsOpen();
		if (bl == true) bl = false;
		else bl = true;
		pLGroup->SetOpen(bl);

		::SendMessage(m_hParentWnd, WM_COMMAND, LW_CM_ADJUST_PANEL, NULL);
		return;
	}
	CLayerPanel::OnLButtonDown(nFlags, point);
}

void CLayerGroupPanel::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (isInRect(point.x, point.y, &m_LayerNameTxtRect)) {
		CLayerPanel::OnLButtonDblClk(nFlags, point);
	}
}