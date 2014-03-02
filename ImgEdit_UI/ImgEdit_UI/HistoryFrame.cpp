#include "stdafx.h"

#include "HistoryFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern ImgEdit g_ImgEdit;


void CListCtrlOD::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	CRect rect(lpDrawItemStruct->rcItem);
	int iItem = lpDrawItemStruct->itemID;

	char szBuff[MAX_IMG_FILE_HANDLE_NAME];

	LVITEM lvItem;
	CRect iRect;

	//
	BOOL isForcus = GetItemState(iItem, LVIS_FOCUSED) == LVIS_FOCUSED;

	int iIndentPix;
	int iIndent;

	//出力座標の初期化
	iIndentPix = 0;

	//アイテム取得のための設定
	lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_INDENT;
	lvItem.iItem = iItem;
	lvItem.iSubItem = 0;
	lvItem.pszText = szBuff;
	lvItem.cchTextMax = sizeof(szBuff);
	lvItem.stateMask = LVIS_ACTIVATING | LVIS_FOCUSED | LVIS_SELECTED | LVIS_STATEIMAGEMASK;


	if (GetItem(&lvItem) == TRUE){
		iIndent = lvItem.iIndent;

		//アイテムの場合
		GetItemRect(iItem, &iRect, LVIR_BOUNDS);

		//インデント分の座標計算
		iIndentPix = (iIndent > 0) ? iIndent * 16 : 0;

		//選択状態か
		DWORD txtColor = ::GetSysColor(COLOR_WINDOWTEXT);
		BOOL isSelect = FALSE;
		if (GetItemState(iItem, LVIS_SELECTED) == LVIS_SELECTED) {
			pDC->FillRect(iRect, &CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
			txtColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
			isSelect = TRUE;
		} else {
			pDC->FillRect(iRect, &CBrush(::GetSysColor(COLOR_WINDOW)));
		}

		//フォーカスがあるか
		if (GetItemState(iItem, LVIS_FOCUSED) == LVIS_FOCUSED) {
			CPen pen(PS_DOT, 1, ::GetSysColor(COLOR_BTNFACE));
			pDC->SelectObject(pen);
			CPoint pos[5];
			pos[0].x = iRect.left + 1;
			pos[0].y = iRect.top;

			pos[1].x = iRect.right - 2;
			pos[1].y = iRect.top;

			pos[2].x = iRect.right - 2;
			pos[2].y = iRect.bottom - 2;

			pos[3].x = iRect.left + 1;
			pos[3].y = iRect.bottom - 2;

			pos[4].x = iRect.left + 1;
			pos[4].y = iRect.top;
			pDC->Polyline(&pos[0], 5);
			pen.DeleteObject();
		}


		pDC->SetTextColor(txtColor);
		DWORD uFlag = DT_WORD_ELLIPSIS | DT_SINGLELINE | DT_LEFT;

		CRect strRect;
		strRect = iRect;
		strRect.left += 2;

		CString outText = lvItem.pszText;

		if (m_nowHandleItem >= iItem) {
			//set text color undo handle
			pDC->SetTextColor(RGB(0, 0, 0));
		} else {
			//set text color redo handle
			pDC->SetTextColor(RGB(150, 150, 150));
		}
		pDC->DrawText(outText, strRect, uFlag);
	}
}


CHistoryFrame::CHistoryFrame()
{
	m_pEditFile = NULL;
}

CHistoryFrame::~CHistoryFrame()
{
	_ASSERTE(_CrtCheckMemory());
}

void CHistoryFrame::OnChangeActiveFile(ImgFile_Ptr pNewFile, ImgFile_Ptr pOldFile)
{
	if (m_pEditFile) {
		m_pEditFile->DeleteEventListener( this );
	}
	if (pNewFile) {
		pNewFile->AddEventListener( this );
	}
	m_pEditFile = pNewFile;
	this->ResetList();
}

void CHistoryFrame::OnAddUndoImgFileHandle(const ImgFileHandle *pHandle)
{
	if(m_pEditFile){
		int undo_size = m_pEditFile->GetUndoHandleSize();
		int redo_size = m_pEditFile->GetRedoHandleSize();

		//add new handle
		LV_ITEM lvitem;
		lvitem.mask = LVIF_TEXT;
		lvitem.iItem = undo_size-1;
		lvitem.iSubItem = 0;
		lvitem.pszText = (LPSTR)pHandle->handle_name;
		m_handleListCtrl.InsertItem( &lvitem );

		//delete redo handle
		int i;
		int list_item_cnt = m_handleListCtrl.GetItemCount();
		for(i=undo_size; i<list_item_cnt; i++){
			m_handleListCtrl.DeleteItem(undo_size);
		}

		m_handleListCtrl.SetNowHandleItem( undo_size-1 );
		m_handleListCtrl.Invalidate(FALSE);
	}
}

void CHistoryFrame::OnRemoveOldUndoImgFileHandle(int num)
{
	assert(num > 0);

	int i;
	for(i=0; i<num; i++){
		m_handleListCtrl.DeleteItem(0);
	}
	m_handleListCtrl.Invalidate(FALSE);
}

void CHistoryFrame::OnUndoImgFileHandle(const ImgFileHandle* pHandle)
{
	if(m_pEditFile){
		int undo_size = m_pEditFile->GetUndoHandleSize();
		m_handleListCtrl.SetNowHandleItem( undo_size-1 );
		m_handleListCtrl.Invalidate(FALSE);
	}
}

void CHistoryFrame::OnRedoImgFileHandle(const ImgFileHandle* pHandle)
{
	if(m_pEditFile){
		int undo_size = m_pEditFile->GetUndoHandleSize();
		m_handleListCtrl.SetNowHandleItem( undo_size-1 );
		m_handleListCtrl.Invalidate(FALSE);
	}
}

BEGIN_MESSAGE_MAP(CHistoryFrame, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGED, 1, CHistoryFrame::OnListChange)
END_MESSAGE_MAP()

int CHistoryFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CWnd::OnCreate(lpCreateStruct)){
		return -1;
	}

	RECT rc;
	rc.top = rc.left = 0;
	rc.right = lpCreateStruct->cx;
	rc.bottom = lpCreateStruct->cy;
	m_handleListCtrl.Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT |
		LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_SINGLESEL | LVS_NOSORTHEADER | LVS_OWNERDRAWFIXED,
		rc,
		this,
		1);
	m_handleListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	LV_COLUMN listcol;
	listcol.pszText = "";
	listcol.cx = lpCreateStruct->cx;
	m_handleListCtrl.InsertColumn(0, &listcol);
	m_handleListCtrl.SetNowHandleItem( -1 );

	//register event listener
	g_ImgEdit.AddEventListener( this );

	return 0;
}

void CHistoryFrame::OnDestroy()
{
	//unregister event listener
	if (m_pEditFile) {
		m_pEditFile->DeleteEventListener( this );
	}
	g_ImgEdit.DeleteEventListener( this );

	CWnd::OnDestroy();
}

void CHistoryFrame::OnSize(UINT nType, int cx, int cy)
{
	m_handleListCtrl.SetColumnWidth(0, cx);
	m_handleListCtrl.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER);

	CWnd::OnSize(nType, cx, cy);
}

void CHistoryFrame::OnListChange(NMHDR* pNotifyStruct, LRESULT* result)
{
	int select_index = m_handleListCtrl.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (select_index != -1) {
		int dif = m_handleListCtrl.GetNowHandleItem() - select_index;
		if (dif > 0) {
			m_pEditFile->Undo(dif);
		} else if (dif < 0) {
			m_pEditFile->Redo(-dif);
		}
	}
}

void CHistoryFrame::ResetList()
{
	m_handleListCtrl.DeleteAllItems();

	if (m_pEditFile) {
		int undo_size = m_pEditFile->GetUndoHandleSize();
		int redo_size = m_pEditFile->GetRedoHandleSize();
	

		LV_ITEM lvitem;

		int i;
		for (i=0; i<undo_size; i++) {
			const ImgFileHandle* pHandle = m_pEditFile->GetUndoHandleAt(i);

			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = i;
			lvitem.iSubItem = 0;
			lvitem.pszText = (LPSTR)pHandle->handle_name;
			m_handleListCtrl.InsertItem( &lvitem );
		}

		for (i=redo_size - 1; i >= 0; i++) {
			const ImgFileHandle* pHandle = m_pEditFile->GetRedoHandleAt(i);

			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = redo_size - i - 1 + undo_size;
			lvitem.iSubItem = 0;
			lvitem.pszText = (LPSTR)pHandle->handle_name;
			m_handleListCtrl.InsertItem( &lvitem );
		}

		m_handleListCtrl.SetNowHandleItem(undo_size-1);
	}
}