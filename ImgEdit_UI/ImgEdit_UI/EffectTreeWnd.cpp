#include "stdafx.h"

#include "EffectTreeWnd.h"
#include "EffectMG.h"
#include "IE_EffectEditDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern ImgEdit g_ImgEdit;

CEffectTreeWnd::CEffectTreeWnd()
{
	m_isDragging = false;
}

CEffectTreeWnd::~CEffectTreeWnd()
{
	_ASSERTE(_CrtCheckMemory());
}

BEGIN_MESSAGE_MAP(CEffectTreeWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

int CEffectTreeWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (0 != CDockablePane::OnCreate(lpCreateStruct))
		return -1;

	m_hInst = lpCreateStruct->hInstance;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	//ツリービューの作成
	m_hTree = CreateWindowEx(0, WC_TREEVIEW, "",
		WS_CHILD | WS_BORDER | WS_VISIBLE | TVS_HASLINES |
		TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_TRACKSELECT | TVS_LINESATROOT,
		0,
		0,
		0,
		0,
		m_hWnd, (HMENU) 101, lpCreateStruct->hInstance, NULL);

	//ドラッグイメージリストの作成
	//m_hImLt = ImageList_Create(16, 16, ILC_COLOR4 | ILC_MASK, 0, 0);
	//if (m_hImLt == NULL)
	//	OutputError::Alert("イメージリスト作成失敗");

	::DragAcceptFiles(m_hTree,TRUE);

	//エフェクトツーリーアイテムの追加
	g_ImgEdit.SetEffectTree( m_hTree );

	return 0;
}

void CEffectTreeWnd::OnSize(UINT nType, int cx, int cy)
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	//m_effectTree.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + 1, rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
	::SetWindowPos(m_hTree, NULL, rectClient.left + 1, rectClient.top + 1, rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
	CDockablePane::OnSize(nType, cx, cy);
}

void CEffectTreeWnd::OnPaint()
{
	CDockablePane::OnPaint();

	CPaintDC dc(this); // 描画のデバイス コンテキスト

	RECT rect;
	::GetWindowRect(m_hTree, &rect);
	CRect rectTree(&rect);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.FillSolidRect(rectTree, ::GetSysColor(COLOR_WINDOW));
}

BOOL CEffectTreeWnd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR){
		if(pNMHDR->code == TVN_BEGINDRAG){
			//OnBegindrag((LPNMTREEVIEW)lParam);
			return TRUE;
		}
		else if(pNMHDR->code == NM_DBLCLK){
			HTREEITEM hItem = TreeView_GetSelection(m_hTree);
			char pszBuf[256];
			TVITEM ti;
			ti.mask = TVIF_TEXT;
			ti.hItem = hItem;
			ti.pszText = pszBuf;
			ti.cchTextMax = 256;

			if(!TreeView_GetItem(m_hTree, &ti)){
				OutputError::PushLog( LOG_LEVEL::_ERROR, "CEffectTreeWnd::OnNotify() ツリーの文字列が長すぎます。");
				return TRUE;
			}
			//追加するエフェクトを生成
			IEffect *ef = g_ImgEdit.CreateEffect(pszBuf);
			if(!ef){
				OutputError::PushLog( LOG_LEVEL::_WARN, "EffectMG::CreateEffect() return NULL");
				return TRUE;
			}
			
			ImgFile_Ptr pFile = g_ImgEdit.GetActiveFile();
			if(!pFile) return TRUE;

			//マスクの描画方法を点線表示に変更
			int old_mask_draw_id = pFile->GetMaskDrawType();
			pFile->SetMaskDrawType(MASK_DOT_LINE_DRAW);

			////エフェクトをかける
			//g_ImgEdit.AddEffect( pFile, ef );
			//エフェクトダイアログを表示させる
			IE_EffectEditDialog *pDialog = new IE_EffectEditDialog();

			if(ef->Init( pFile )){
				int ret = pDialog->CreateDialogBox(m_hInst,
					this->GetParent()->GetSafeHwnd(), pFile, ef);
				ef->End( pFile );
				g_ImgEdit.ReleaseEffect(ef);
				ef = NULL;
			
				if(ret == IDOK){
				}
				else if(ret == IDCANCEL){
					//エフェクトの消去
					pFile->Undo(1);
				}
			}
			delete pDialog;
			pDialog = NULL;

			//マスクの表示を元に戻す
			pFile->SetMaskDrawType(old_mask_draw_id);

			return TRUE;
		}
	}

	return CDockablePane::OnNotify(wParam, lParam, pResult);;
}

void CEffectTreeWnd::OnBegindrag(LPNMTREEVIEW lpnmtv)
{
	m_hDragItem = lpnmtv->itemNew.hItem;
	LPSTR pstr = lpnmtv->itemNew.pszText;
	//ドラッグイメージ作成
	//m_hImLt = TreeView_CreateDragImage(m_hTree, lpnmtv->itemNew.hItem);
	////TreeView_GetItemRect(m_hTree, lpnmtv->itemNew.hItem, &rcItem, TRUE); 
	//::ImageList_BeginDrag(m_hImLt, 0, 0, 0);
	//カーソルをキャプチャー
	CWnd *wnd = this->SetCapture();
	//::SetCapture( this->GetSafeHwnd() );
	//カーソルを消去
	//::ShowCursor(FALSE);

	POINT pt;
	pt.x = lpnmtv->ptDrag.x;
	pt.y = lpnmtv->ptDrag.y;	
	
	ClientToScreen(&(lpnmtv->ptDrag));
	//ImageList_DragEnter(NULL, lpnmtv->ptDrag.x, lpnmtv->ptDrag.y);

	m_isDragging = true;
	return;
}

void CEffectTreeWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDockablePane::OnLButtonDown(nFlags, point);
}

void CEffectTreeWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_isDragging){
		TVHITTESTINFO tvht;
		POINT pt;

		//マウスのスクリーン座標取得
		GetCursorPos(&pt);

		ZeroMemory(&tvht, sizeof(TVHITTESTINFO));
		tvht.flags = TVHT_ONITEM;
		tvht.pt.x = pt.x;
		tvht.pt.y = pt.y;
		ScreenToClient(&(tvht.pt));
		
		ImageList_DragMove(pt.x, pt.y);
	}

	CDockablePane::OnMouseMove(nFlags, point);
}

void CEffectTreeWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_isDragging){
		//ImageList_EndDrag();
		//ImageList_DragLeave(m_hTree);
		//::ShowCursor(TRUE);
		::ReleaseCapture();
		m_isDragging = FALSE;

		//マウスの座標から下にあるウィンドウのハンドルを得る
		POINT pt;
		GetCursorPos(&pt);
		//HWND hSendWnd = ::WindowFromPoint(pt);
		CWnd* pwnd = WindowFromPoint(pt);
		//ドロップするウィンドウを取得していればドラッグしているアイテムのテキストを送る。
		//if(hSendWnd){
		if(pwnd){
			char pszBuf[256];
			TVITEM ti;
			ti.mask = TVIF_TEXT;
			ti.hItem = m_hDragItem;
			ti.pszText = pszBuf;
			ti.cchTextMax = 256;

			if(TreeView_GetItem(m_hTree, &ti)){
				//SendMessage(hSendWnd, WM_USER, UM_DROPITEM, (LPARAM)pszBuf);
				pwnd->SendMessage(WM_USER, UM_DROPITEM, (LPARAM)pszBuf);
			}
			else{
				OutputError::PushLog( LOG_LEVEL::_ERROR, "EffectTreeWindow::OnEndDrag() ツリーの文字列が長すぎます。");
			}
		}
	}
	
	CDockablePane::OnLButtonUp(nFlags, point);
}