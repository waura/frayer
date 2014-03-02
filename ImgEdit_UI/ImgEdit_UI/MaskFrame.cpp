#include "stdafx.h"
//
//#include "MaskFrame.h"
//
//extern ImgEdit g_ImgEdit;
//
//CMaskFrame::CMaskFrame()
//{
//	m_hInst = NULL;
//	m_SelectMaskPanel = NULL;
//	m_DragMaskPanel   = NULL;
//	m_UnderMaskPanel  = NULL;
//	m_PopupMaskPanel  = NULL;
//	m_pActiveFile = NULL;
//	m_hPanelDragImage = NULL;
//	isPanelDrag = false;
//
//	//scrollbar init
//	SetScrollSizes(MM_TEXT, CSize(0,0));
//}
//
//CMaskFrame::~CMaskFrame()
//{
//}
//
//////////////////////////////////////////////////////
///*!
//	アクティブになったファイルが変わったときに呼び出されるイベント
//	@param file アクティブなファイルオブジェクト
//*/
//void CMaskFrame::OnChangeActiveFile(ImgFile_Ptr pNewFile, ImgFile_Ptr pOldFile)
//{
//	m_pActiveFile = pFile; //ファイルへのポインタを保持
//	
//	ReSetAllPanel( pFile );
//	AdjastScrollSizes();
//}
//
///////////////////////////////////////////////////////
///*!
//	ファイルが追加されたときに呼び出すイベント
//	@param[in] pFile 追加されたファイル
//*/
//void CMaskFrame::OnAddImgFile(ImgFile_Ptr pFile)
//{
//	pFile->AddEventListener( this );
//}
//
///////////////////////////////////////////////////////
///*!
//	マスクが追加されたときに呼び出されるイベント
//	@param pMask 追加されたマスク
//*/
//void CMaskFrame::OnAddMask(ImgMask* pMask, int index)
//{
//	AddMaskPanel(pMask, index);
//	
//	AdjastPanel();
//	AdjastScrollSizes();
//}
//
///////////////////////////////////////////////////////
///*!
//	マスクが削除されるときに呼び出されるイベント
//	@param[in] pLayer 削除されるマスク
//	@param[in] index マスクインデックス
//*/
//void CMaskFrame::OnDeleteMask(ImgMask *pMask, int index)
//{
//	DeleteMaskPanel(index);
//	
//	AdjastPanel();
//	AdjastScrollSizes();
//}
//
//void CMaskFrame::OnDraw(CDC *pDC)
//{
//}
//
//void CMaskFrame::OnInitialUpdate()
//{
//}
//
//BEGIN_MESSAGE_MAP(CMaskFrame, CScrollWnd)
//	ON_WM_CREATE()
//	ON_WM_SIZE()
//	ON_WM_PAINT()
//	ON_WM_LBUTTONDOWN()
//	ON_WM_MOUSEMOVE()
//	ON_WM_LBUTTONUP()
//	ON_WM_RBUTTONUP()
//END_MESSAGE_MAP()
//
//int CMaskFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//	if(0 != CScrollWnd::OnCreate(lpCreateStruct)){
//		return -1;
//	}
//
//	m_width = lpCreateStruct->cx;
//	m_hInst = lpCreateStruct->hInstance;
//
//	//イベントリスナー登録
//	g_ImgEdit.AddEventListener( this );
//
//	return 0;
//}
//
//void CMaskFrame::OnSize(UINT nType, int cx, int cy)
//{
//	AdjastPanel();
//	AdjastScrollSizes();
//	CScrollWnd::OnSize(nType, cx, cy);
//}
//
//void CMaskFrame::OnPaint()
//{
//	CPaintDC dc(this);
//	CRect rect;
//	GetWindowRect(rect);
//	ScreenToClient(rect);
//
//	rect.InflateRect(1, 1);
//	dc.FillSolidRect(rect, ::GetSysColor(COLOR_WINDOW));
//}
//
//void CMaskFrame::OnLButtonDown(UINT nFlags, CPoint point)
//{
//	m_UnderMaskPanel = NULL;
//
//	m_DragMaskPanel = GetMaskPanelFromPos(point);
//	if(m_DragMaskPanel){
//		isPanelDrag = true;
//		//イメージリストの作成
//		m_hPanelDragImage = ImageList_Create(m_width, MASK_PANEL_H, ILC_COLOR4, 1,1);
//		m_DragMaskPanel->CreateDragImage( m_hPanelDragImage );
//		
//		//イメージのドラッグ開始
//		//レイヤーパネルの範囲を取得して、ドラッグイメージの位置調整
//		RECT rc;
//		m_DragMaskPanel->GetClientRect(&rc);
//		POINT pt;
//		pt.x = point.x; pt.y = point.y;
//		ClientToScreen(&pt);
//		ImageList_BeginDrag( m_hPanelDragImage, 0, pt.x - rc.left, pt.y - rc.top);
//		ImageList_DragEnter(NULL, point.x, point.y);
//	}
//
//	CScrollWnd::OnLButtonDown(nFlags, point);
//}
//
//void CMaskFrame::OnMouseMove(UINT nFlags, CPoint point)
//{
//	if(isPanelDrag){
//		POINT pt;
//		GetCursorPos(&pt);
//		ImageList_DragMove(pt.x, pt.y);
//
//		//マウスの下にあるレイヤーパネルを呼び出し
//		CMaskPanel* lp = GetMaskPanelFromPos(point);
//		if(lp){
//			lp->OnDragMouse(&pt);
//			if(lp != m_UnderMaskPanel){	
//				m_UnderMaskPanel = lp;
//			}
//		}
//		AllPanelRePaint();
//	}
//
//	CScrollWnd::OnMouseMove(nFlags, point);
//}
//
//void CMaskFrame::OnLButtonUp(UINT nFlags, CPoint point)
//{
//	if(isPanelDrag){
//		m_UnderMaskPanel = NULL;
//
//		isPanelDrag = false;
//		ImageList_EndDrag();
//		ImageList_DragLeave( GetSafeHwnd() );
//		ImageList_Destroy(m_hPanelDragImage);
//
//		//マウスの座標から下にあるウィンドウのハンドルを得る
//		POINT pt;
//		GetCursorPos(&pt);
//		CWnd* wnd = WindowFromPoint(pt);
//
//		//ドロップするウィンドウを取得し、それが自分のファイルのウィンドウ出なければ
//		//メッセージを送り、ドロップしたレイヤを追加させる。
//		if(wnd && wnd != this){
//			//wnd->SendMessage(WM_USER, UM_DROPLAYER, (LPARAM)m_DragMaskPanel->GetMask());
//			//
//			////表示画像の再生成
//			//UPDATE_DATA data;
//			//data.isAll = true;
//			//data.update_flag = UPDATE_FLAG::UPDATE_MASK_IMAGE;
//			//m_pActiveFile->PushUpdateData(&data);
//		}
//
//	}
//
//	//カーソルがドラッグ開始時のパネルの上にあるならそのパネルを選択状態へ
//	CMaskPanel *mp = GetMaskPanelFromPos(point);
//	if(mp && mp == m_DragMaskPanel){
//		//選択レイヤーを変更する
//		if(m_SelectMaskPanel){
//			m_SelectMaskPanel->NewtralPanel();
//		}
//		m_SelectMaskPanel = mp;
//		m_SelectMaskPanel->SelectPanel();
//	}
//
//	CScrollWnd::OnLButtonUp(nFlags, point);
//}
//
//void CMaskFrame::OnRButtonUp(UINT nFlags, CPoint point)
//{
//	m_PopupMaskPanel = GetMaskPanelFromPos(point);
//	if(m_PopupMaskPanel){
//		POINT spt;
//		spt.x = point.x;
//		spt.y = point.y;
//		ClientToScreen(&spt);
//		m_PopupMaskPanel->PopupMenu( GetSafeHwnd(), &spt);
//	}
//}
//
/////////////////////////////////////////////////////
///*!
//	コマンド処理
//*/
//BOOL CMaskFrame::OnCommand(WPARAM wp, LPARAM lp)
//{
//	//switch(LOWORD(wp)){
//	//	case LW_CM_COPY_LAYER:
//	//		{
//	//			int index = m_pActiveFile->GetLayerSize();
//	//			AddCopyLayerHandle* handle = (AddCopyLayerHandle*)ImgFileHandleMG::CreateImgFileHandle( m_pActiveFile, IFH_ADD_COPY_LAYER );
//	//			handle->SetAddToIndex( index );
//	//			m_pActiveFile->DoImgFileHandle( handle );
//
//	//			m_pActiveFile->SetSelectLayer( index );
//	//		}
//	//		return TRUE;
//	//	case LW_CM_DELETE_LAYER:
//	//		{
//	//			DeleteLayerPanel( m_PopupLayerPanel );
//	//			m_PopupLayerPanel = NULL;
//	//		}
//	//		return TRUE;
//	//}
//
//	return CScrollWnd::OnCommand(wp, lp);
//}
//
////////////////////////////////////////
///*!
//	スクロールバーサイズ調節
//*/
//void CMaskFrame::AdjastScrollSizes()
//{
//	RECT rc;
//	this->GetClientRect(&rc);
//	size_t size = m_MaskPanel_Vec.size();
//
//	CClientDC dc(NULL);
//	OnPrepareDC(&dc);
//	CSize sizeDoc;
//	sizeDoc.cx = rc.right - rc.left;
//	sizeDoc.cy = MASK_PANEL_H * size;
//	dc.LPtoDP(&sizeDoc);
//	SetScrollSizes(MM_TEXT, sizeDoc);
//}
//
//////////////////////////////////////////////////////
///*!
//*/
//VOID CMaskFrame::ReSetAllPanel(ImgFile_Ptr pFile)
//{
//	//レイヤーパネル全削除
//	ClearMaskPanel();
//
//	if(pFile){
//		RECT rc;
//		this->GetClientRect(&rc);
//
//		ImgMask *select_mask = pFile->GetSelectMask();
//		int lSize = pFile->GetMaskSize();
//		for(int i=0; i<lSize; i++){
//			//マスクオブジェクトを取得
//			ImgMask *mask = pFile->GetMaskAtIndex(i);
//			//レイヤーパネルの作成
//			RECT tmp_rc;
//			tmp_rc.top = rc.bottom - MASK_PANEL_H * (i+1);
//			tmp_rc.left = 0;
//			tmp_rc.bottom = tmp_rc.top + MASK_PANEL_H;
//			tmp_rc.right = tmp_rc.left + rc.right - rc.left;
//			CMaskPanel *mp = new CMaskPanel();
//			mp->CreateEx(0,0,"",
//				WS_CHILD | WS_CLIPCHILDREN | WS_BORDER | WS_VISIBLE,
//				tmp_rc,
//				this,
//				i);
//			mp->SetMask(mask);
//
//			m_MaskPanel_Vec.push_back(mp);
//			
//			if(mask == select_mask){//選択レイヤーならパネルを選択状態に
//				mp->SelectPanel();
//				m_SelectMaskPanel = mp;
//			}
//		}
//
//		InvalidateRect(NULL, FALSE);
//	}
//}
//
//////////////////////////////////////////////////////
///*!
//	indexの位置にpMaskのマスクパネルを追加する。
//	@param[in] pMask
//	@param[in] index
//*/
//VOID CMaskFrame::AddMaskPanel(ImgMask *pMask, int index)
//{
//	assert(0 <= index && index <= m_MaskPanel_Vec.size() );
//
//	CMaskPanel *mp = new CMaskPanel();
//	mp->CreateEx(0, 0, "",
//		WS_CHILD | WS_CLIPCHILDREN | WS_BORDER | WS_VISIBLE,
//		CRect(0,0,0,0),
//		this,
//		index);
//	mp->SetMask( pMask );
//
//	MaskPanel_Vec::iterator itr;
//	if(index < m_MaskPanel_Vec.size()/2){
//		itr = m_MaskPanel_Vec.begin();
//		for(int i=0; i<index; i++){
//			itr++;
//		}
//	}
//	else{
//		itr = m_MaskPanel_Vec.end();
//		for(int i=m_MaskPanel_Vec.size(); i > index; i--){
//			itr--;
//		}
//	}
//
//	m_MaskPanel_Vec.insert(itr, mp);
//}
//
//////////////////////////////////////////////////////
///*!
//	indexの位置のマスクパネルを削除する
//	@param[in] index
//*/
//VOID CMaskFrame::DeleteMaskPanel(int index)
//{
//	assert(0 <= index && index <= m_MaskPanel_Vec.size() );
//
//	int size = m_MaskPanel_Vec.size();
//	MaskPanel_Vec::iterator itr;
//	if(index < m_MaskPanel_Vec.size()/2){
//		itr = m_MaskPanel_Vec.begin();
//		for(int i=0; i<index; i++){
//			itr++;
//		}
//	}
//	else{
//		itr = m_MaskPanel_Vec.end();
//		for(int i=m_MaskPanel_Vec.size(); i > index; i--){
//			itr--;
//		}
//	}
//
//	CMaskPanel* mp = (*itr);
//	m_MaskPanel_Vec.erase(itr);
//
//	m_SelectMaskPanel = (mp == m_SelectMaskPanel) ? NULL : m_SelectMaskPanel;
//	m_DragMaskPanel   = (mp == m_DragMaskPanel)   ? NULL : m_DragMaskPanel;
//	m_UnderMaskPanel  = (mp == m_UnderMaskPanel)  ? NULL : m_UnderMaskPanel;
//	m_PopupMaskPanel  = (mp == m_PopupMaskPanel)  ? NULL : m_PopupMaskPanel;
//
//	delete mp;
//}
//
//////////////////////////////////////////////////////
///*!
//*/
//VOID CMaskFrame::AdjastPanel()
//{
//	if(!m_pActiveFile) return;
//
//	RECT rc;
//	this->GetClientRect(&rc);
//	
//	//スクロール位置を取得
//	CPoint cpt = GetScrollPosition();
//
//	int mSize = m_pActiveFile->GetMaskSize();
//	ImgMask *select_mask = m_pActiveFile->GetSelectMask();
//	
//	MaskPanel_Vec::iterator itr = m_MaskPanel_Vec.begin();
//
//	int i;
//	for(i=0; itr != m_MaskPanel_Vec.end(); itr++, i++){
//		(*itr)->SetWindowPos(
//			NULL,
//			0,
//			(rc.bottom-rc.top > mSize*MASK_PANEL_H) ? rc.bottom - MASK_PANEL_H * (i+1) : mSize*MASK_PANEL_H - MASK_PANEL_H*(i+1) - cpt.y,
//			rc.right - rc.left,
//			MASK_PANEL_H,
//			SWP_NOZORDER);
//
//		if((*itr)->GetMask() == select_mask){//選択マスクならパネルを選択状態に
//			(*itr)->SelectPanel();
//			m_SelectMaskPanel = (*itr);
//		}else{
//			(*itr)->NewtralPanel();
//		}
//	}
//
//	InvalidateRect(NULL, FALSE);
//}
//
/////////////////////////////////////////////////////
///*!
//	すべてのレイヤーパネルを再描画する。
//*/
//void CMaskFrame::AllPanelRePaint()
//{
//	this->Invalidate(FALSE);
//}
//
//////////////////////////////////////////////////////
///*!
//	マウスの下にあるパネルのポインタを返す。
//	@param[in] point クライアントマウス座標
//	@return マスクパネルのポインタ
//*/
//CMaskPanel* CMaskFrame::GetMaskPanelFromPos(CPoint point)
//{
//	HWND hwnd;
//	RECT rc;
//	POINT lt, rb;
//	MaskPanel_Vec::iterator itr = m_MaskPanel_Vec.begin();
//	for(; itr != m_MaskPanel_Vec.end(); itr++){
//		//パネルの位置を求める
//		(*itr)->GetWindowRect(&rc);
//		lt.x = rc.left; lt.y = rc.top;
//		rb.x = rc.right; rb.y = rc.bottom;
//		this->ScreenToClient(&lt);
//		this->ScreenToClient(&rb);
//		//判定
//		if((lt.x <= point.x && point.x <= rb.x) &&
//			(lt.y <= point.y && point.y <= rb.y))
//		{
//			return (*itr);
//		}
//	}
//	return NULL;
//}
//
//////////////////////////////////////////////////////
///*!
//	マスクパネルを削除する。
//	@param[in,out] lp 削除するマスクパネル
//*/
//VOID CMaskFrame::DeleteMaskPanel(CMaskPanel *mp)
//{
//	if(mp){
//		mp->DeleteMask();
//		if(mp == m_SelectMaskPanel) m_SelectMaskPanel = NULL;
//
//		LPUPDATE_DATA data = m_pActiveFile->CreateUpdateData(UPDATE_MASK_IMAGE);
//		data->isAll = true;
//		m_pActiveFile->PushUpdateData( data );
//	}
//}
//
//////////////////////////////////////////////////////
///*!
//	マスクパネルをすべて削除メモリー開放
//*/
//VOID CMaskFrame::ClearMaskPanel()
//{
//	MaskPanel_Vec::iterator itr = m_MaskPanel_Vec.begin();
//	for(; itr != m_MaskPanel_Vec.end(); itr++){
//		delete (*itr);
//	}
//	m_MaskPanel_Vec.clear();
//}
