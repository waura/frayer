#include "stdafx.h"
#include "LayerFrame.h"

#define LAYER_PANEL_H 45
#define LAYER_PANEL_V_MARGIN 1
#define LAYER_PANEL_H_MARGIN 2
#define TAB_LAYER_PANEL_W 15 //

#define INVISIBLE_PANEL_TOP 0
#define INVISIBLE_PANEL_BOTTOM 0
#define INVISIBLE_PANEL_LEFT 1000
#define INVISIBLE_PANEL_RIGHT 1000

#define ID_LF_ON_MOUSE_PANEL_UPDATE_TIMER 1

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern ImgEdit g_ImgEdit;

CLayerFrame::CLayerFrame()
{
	m_OnMousePanel = NULL;
	m_SelectLayerPanel = NULL;
	m_DragLayerPanelInsertIndex = -1;
	m_DragLayerPanelIndex = -1;
	m_DragLayerPanel   = NULL;
	m_PopupLayerPanel  = NULL;
	m_pActiveFile = NULL;
	isPanelDrag = false;

	//scrollbar init
	SetScrollSizes(MM_TEXT, CSize(0,0));
}

CLayerFrame::~CLayerFrame()
{
	ClearLayerPanel();
	_ASSERTE(_CrtCheckMemory());
}

////////////////////////////////////////////////////
/*!
	アクティブになったファイルが変わったときに呼び出されるイベント
	@param file アクティブなファイルオブジェクト
*/
void CLayerFrame::OnChangeActiveFile(ImgFile_Ptr pNewFile, ImgFile_Ptr pOldFile)
{
	if (m_pActiveFile != pNewFile) {
		if (pNewFile) {
			pNewFile->AddEventListener( this );
		}
		if (m_pActiveFile) {
			m_pActiveFile->DeleteEventListener( this );
		}
		m_pActiveFile = pNewFile; //ファイルへのポインタを保持
	
		ReSetAllPanel( pNewFile );
		AdjastScrollSizes();
	}
}

/////////////////////////////////////////////////////
/*!
	レイヤーが追加されたときに呼び出されるイベント
	@param pLayer 追加されたレイヤー
*/
void CLayerFrame::OnAddLayer(IImgLayer_Ptr pLayer, int index)
{
	this->ReSetAllPanel( m_pActiveFile );
	
	AdjastPanel();
	AdjastScrollSizes();
}

/////////////////////////////////////////////////////
/*!
	レイヤーが削除されるときに呼び出されるイベント
	@param[in] pLayer 削除されるレイヤー
	@param[in] index レイヤーインデックス
*/
void CLayerFrame::OnDeleteLayer(IImgLayer_Ptr pLayer, int index)
{
	this->ReSetAllPanel( m_pActiveFile );
	
	AdjastPanel();
	AdjastScrollSizes();
}

void CLayerFrame::OnChangeLayerLine(int from_index, int to_index)
{
	this->ReSetAllPanel( m_pActiveFile );
	//InsertLayerPanel(from_index, to_index);

	//AdjastPanel();
	//AdjastScrollSizes();
}

//////////////////////////////
/*!
	選択レイヤーが変わったときに呼び出す
	@param[in] pLayer 選択されたレイヤー
*/
void CLayerFrame::OnSelectLayer(IImgLayer_weakPtr pLayer)
{
	AdjastPanel();
}

void CLayerFrame::OnDraw(CDC *pDC)
{
	CRect rect;
	GetWindowRect(rect);
	ScreenToClient(rect);

	rect.InflateRect(1, 1);
	pDC->FillSolidRect(rect, ::GetSysColor(COLOR_WINDOW));

	//
	if (m_DragLayerPanelIndex != -1) {
		pDC->FillSolidRect(&m_InsertCursorRect, RGB(255, 0, 0));
	}

	//draw all layer panel
	LayerPanel_Vec::iterator itr = m_LayerPanel_Vec.begin();
	for (; itr != m_LayerPanel_Vec.end(); itr++) {
		if (*itr) {
			(*itr)->DrawPanel(pDC);
		}
	}
}

void CLayerFrame::OnInitialUpdate()
{
}

BEGIN_MESSAGE_MAP(CLayerFrame, CScrollWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	ON_EN_KILLFOCUS(ID_LP_NAME_EDIT, OnLayerNameEditKillFocus)
END_MESSAGE_MAP()

BOOL CLayerFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	if ( !CScrollWnd::PreCreateWindow(cs) )
		return FALSE;
	
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, NULL, NULL);
	return TRUE;
}

int CLayerFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (0 != CScrollWnd::OnCreate(lpCreateStruct)) {
		return -1;
	}

	//create layer name edit box
	RECT rc;
	rc.top = rc.bottom = rc.left = rc.right = 0;
	m_LayerNameEdit.Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
		rc,
		this,
		ID_LP_NAME_EDIT);
	m_LayerNameEdit.ShowWindow(SW_HIDE);

	//set font for name edit box
	LOGFONT FontInfo;
	memset(&FontInfo, 0, sizeof(FontInfo));
	strcpy(FontInfo.lfFaceName, LP_FONT_NAME);
	FontInfo.lfWeight = FW_BOLD;
	FontInfo.lfHeight = LAYER_NAME_FONT_SIZE;
	FontInfo.lfCharSet = SHIFTJIS_CHARSET;
	m_LayerNameFont.CreateFontIndirect( &FontInfo );
	m_LayerNameEdit.SetFont( &m_LayerNameFont );

	//
	this->SetTimer(ID_LF_ON_MOUSE_PANEL_UPDATE_TIMER, 1000, 0);

	//register event listener
	g_ImgEdit.AddEventListener( this );

	return 0;
}

void CLayerFrame::OnDestroy()
{
	//unregister event listener
	if (m_pActiveFile) {
		m_pActiveFile->DeleteEventListener( this );
	}
	g_ImgEdit.DeleteEventListener( this );

	m_LayerNameFont.DeleteObject();

	CWnd::OnDestroy();
}

void CLayerFrame::OnSize(UINT nType, int cx, int cy)
{
	AdjastPanel();
	AdjastScrollSizes();
	CScrollWnd::OnSize(nType, cx, cy);
}

void CLayerFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_DragLayerPanel = GetLayerPanelFromPos(point);
	if (m_DragLayerPanel) {
		m_DragLayerPanel->OnLButtonDown(nFlags, point);

		isPanelDrag = true;
		m_DragLayerPanelIndex = GetLayerPanelIndexFromPos(point);
		m_DragLayerPanelInsertIndex = m_DragLayerPanelIndex;

		//change select layer panel
		if (m_SelectLayerPanel) {
			m_SelectLayerPanel->Newtral();
		}
		m_SelectLayerPanel = m_DragLayerPanel;
		m_SelectLayerPanel->Select();
	}

	CScrollWnd::OnLButtonDown(nFlags, point);
}

void CLayerFrame::OnMouseMove(UINT nFlags, CPoint point)
{
	if (isPanelDrag == false) {
		if (m_OnMousePanel) {
			m_OnMousePanel->LeaveMouse();
			m_OnMousePanel = NULL;
		}

		CLayerPanel* lp = GetLayerPanelFromPos(point);
		if (lp && (lp != m_SelectLayerPanel)) {
			//change on mosue layer panel
			m_OnMousePanel = lp;
			m_OnMousePanel->OnMouse();
		}
	}
	else if (isPanelDrag && m_DragLayerPanel) {
		//get layer panel under mouse
		int under_index = GetLayerPanelIndexFromPos(point);
		CLayerPanel* lp = NULL;
		if ((under_index != -1) && (under_index != m_DragLayerPanelIndex)) {
			lp = m_LayerPanel_Vec[under_index];
		}

		if (lp) {
			if (lp->GetLayer()->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER) {
				ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(lp->GetLayer());
				RECT rc;
				lp->GetPanelRect(&rc);

				int line1 = (rc.bottom - rc.top)/3 + rc.top;
				int line2 = (rc.bottom - rc.top)/3 + line1;

				int index;
				int location_num;
				if (point.y < line1) {
					location_num = 0; //上
					index = under_index + 1;
				} else if (point.y < line2) {
					location_num = 1; //中
					index = under_index;
				} else {
					location_num = 2; //下
					//index = under_index - pLGroup->GetNumOfChildLayer() - 1;
					index = under_index;
				}

				if (index != m_DragLayerPanelInsertIndex) {
					m_DragLayerPanelInsertIndex = index;
					this->InvalidateRect(&m_InsertCursorRect, FALSE);

					switch (location_num) {
						case 0:
							{
								m_InsertCursorRect.top = rc.top - LAYER_PANEL_V_MARGIN*2;
								m_InsertCursorRect.bottom = m_InsertCursorRect.top + LAYER_PANEL_V_MARGIN*2;
								m_InsertCursorRect.left = rc.left;
								m_InsertCursorRect.right = rc.right;
							}
							break;
						case 1:
							{
								m_InsertCursorRect.top = rc.top - LAYER_PANEL_V_MARGIN;
								m_InsertCursorRect.bottom = rc.bottom + LAYER_PANEL_V_MARGIN;
								m_InsertCursorRect.left = rc.left - LAYER_PANEL_H_MARGIN;
								m_InsertCursorRect.right = rc.right + LAYER_PANEL_H_MARGIN;
							}
							break;
						case 2:
							{
								CLayerPanel* under_lp = m_LayerPanel_Vec[ index ];
								under_lp->GetPanelRect( &rc );
								m_InsertCursorRect.top = rc.top - LAYER_PANEL_V_MARGIN*2;
								m_InsertCursorRect.bottom = m_InsertCursorRect.top + LAYER_PANEL_V_MARGIN*2;
								m_InsertCursorRect.left = rc.left;
								m_InsertCursorRect.right = rc.right;
							}
							break;
						default:
							assert(0);
							break;
					}
					this->InvalidateRect(&m_InsertCursorRect, FALSE);
				}
			} else {
				RECT rc;
				POINT pt;
				bool isOver = false;
				lp->GetPanelRect(&rc);
				int line = (rc.bottom - rc.top)/2 + rc.top;
				
				int index = under_index;
				if (point.y < line) {
					isOver = true;
					index++;
				}

				if (index != m_DragLayerPanelInsertIndex) {
					m_DragLayerPanelInsertIndex = index;

					this->InvalidateRect(&m_InsertCursorRect, FALSE);

					if (isOver) {
						m_InsertCursorRect.top = rc.top - LAYER_PANEL_V_MARGIN*2;
					} else {
						m_InsertCursorRect.top = rc.bottom;
					}
					m_InsertCursorRect.bottom = m_InsertCursorRect.top + LAYER_PANEL_V_MARGIN*2;
					m_InsertCursorRect.left = rc.left;
					m_InsertCursorRect.right = rc.right;

					this->InvalidateRect(&m_InsertCursorRect, FALSE);
				}
			}
		}
	}

	CScrollWnd::OnMouseMove(nFlags, point);
}

void CLayerFrame::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (isPanelDrag && m_DragLayerPanel) {
		isPanelDrag = false;

		//layer change position
		if (this->m_DragLayerPanelIndex != this->m_DragLayerPanelInsertIndex) {
			ChangeLayerLineHandle* handle;
			handle = (ChangeLayerLineHandle*)m_pActiveFile->CreateImgFileHandle(IFH_CHANGE_LAYER_LINE);
			handle->SetChangeIndex(this->m_DragLayerPanelIndex, this->m_DragLayerPanelInsertIndex);
			m_pActiveFile->DoImgFileHandle( handle );
		}

		m_DragLayerPanelIndex = -1;
		m_DragLayerPanelInsertIndex = -1;
		this->InvalidateRect(&m_InsertCursorRect, FALSE);


		////マウスの座標から下にあるウィンドウのハンドルを得る
		//POINT pt;
		//GetCursorPos(&pt);
		//CWnd* wnd = WindowFromPoint(pt);

		////ドロップするウィンドウを取得し、それが自分のファイルのウィンドウ出なければ
		////メッセージを送り、ドロップしたレイヤを追加させる。
		//if(wnd && wnd != this){
		//	wnd->SendMessage(WM_USER, UM_DROPLAYER, (LPARAM)m_DragLayerPanel->GetLayer());
		//}
	}

	CScrollWnd::OnLButtonUp(nFlags, point);
}

void CLayerFrame::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CLayerPanel* pPanel = GetLayerPanelFromPos(point);
	if (pPanel) {
		pPanel->OnLButtonDblClk(nFlags, point);
	}

	CScrollWnd::OnLButtonDblClk(nFlags, point);
}

void CLayerFrame::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_PopupLayerPanel = GetLayerPanelFromPos(point);
	if (m_PopupLayerPanel) {
		POINT spt;
		spt.x = point.x;
		spt.y = point.y;
		ClientToScreen(&spt);
		m_PopupLayerPanel->PopupMenu( GetSafeHwnd(), &spt);
	}
}

void CLayerFrame::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent) {
		case ID_LF_ON_MOUSE_PANEL_UPDATE_TIMER:
			{
				POINT pt;
				::GetCursorPos(&pt);
				this->ScreenToClient(&pt);

				CLayerPanel* lp = GetLayerPanelFromPos(CPoint(pt));
				if (lp != m_OnMousePanel) {
					if (m_OnMousePanel) {
						m_OnMousePanel->LeaveMouse();
						m_OnMousePanel = NULL;
					}
				}
			}
			return;
		default:
			assert(0); //unknown timer
			break;
	}
	CScrollWnd::OnTimer(nIDEvent);
}

///////////////////////////////////////////////////
/*!
	コマンド処理
*/
BOOL CLayerFrame::OnCommand(WPARAM wp, LPARAM lp)
{
	switch (LOWORD(wp)) {
		case LW_CM_COPY_LAYER:
			{
				AddCopyLayerHandle* handle = (AddCopyLayerHandle*)m_pActiveFile->CreateImgFileHandle(IFH_ADD_COPY_LAYER);
				m_pActiveFile->DoImgFileHandle( handle );

				m_pActiveFile->SetSelectLayer( handle->GetAddLayer().lock() );
			}
			return TRUE;
		case LW_CM_DELETE_LAYER:
			{
				RemoveLayerHandle* handle = (RemoveLayerHandle*)m_pActiveFile->CreateImgFileHandle(IFH_REMOVE_LAYER);
				handle->SetRemoveLayer(m_PopupLayerPanel->GetLayer());
				m_pActiveFile->DoImgFileHandle( handle );

				m_PopupLayerPanel = NULL;
				//DeleteLayerPanel( m_PopupLayerPanel );
			}
			return TRUE;
		case LW_CM_LOCK_PIXEL_ALPHA:
			if (m_PopupLayerPanel) {
				assert(m_PopupLayerPanel->GetLayer()->GetLayerType() == IE_LAYER_TYPE::NORMAL_LAYER);

				ImgLayer_Ptr pLayer = std::dynamic_pointer_cast<ImgLayer>(m_PopupLayerPanel->GetLayer());
				pLayer->SetLockPixelAlpha(true);
			}
			return TRUE;
		case LW_CM_UNLOCK_PIXEL_ALPHA:
			{
				if (m_PopupLayerPanel) {
					assert(m_PopupLayerPanel->GetLayer()->GetLayerType() == IE_LAYER_TYPE::NORMAL_LAYER);

					ImgLayer_Ptr pLayer = std::dynamic_pointer_cast<ImgLayer>(m_PopupLayerPanel->GetLayer());
					pLayer->SetLockPixelAlpha(false);
				}
			}
			return TRUE;
		case LW_CM_CLIP_WITH_UNDER_LAYER:
			{
				if (m_PopupLayerPanel) {
					assert(m_PopupLayerPanel->GetLayer()->GetLayerType() == IE_LAYER_TYPE::NORMAL_LAYER);

					ImgLayer_Ptr pLayer = std::dynamic_pointer_cast<ImgLayer>(m_PopupLayerPanel->GetLayer());
					pLayer->SetClipWithUnderLayer(true);

					//update draw image
					LPUPDATE_DATA data = pLayer->CreateUpdateData();
					data->isAll = true;
					pLayer->PushUpdateData(data);

					LPUPDATE_DATA data2 = m_pActiveFile->CreateUpdateData();
					data2->isAll = true;
					m_pActiveFile->PushUpdateData(data2);
				}
			}
			return TRUE;
		case LW_CM_UNCLIP_WITH_UNDER_LAYER:
			{
				if (m_PopupLayerPanel) {
					assert(m_PopupLayerPanel->GetLayer()->GetLayerType() == IE_LAYER_TYPE::NORMAL_LAYER);

					ImgLayer_Ptr pLayer = std::dynamic_pointer_cast<ImgLayer>(m_PopupLayerPanel->GetLayer());
					pLayer->SetClipWithUnderLayer(false);

					//update draw image
					LPUPDATE_DATA data = pLayer->CreateUpdateData();
					data->isAll = true;
					pLayer->PushUpdateData(data);

					LPUPDATE_DATA data2 = m_pActiveFile->CreateUpdateData();
					data2->isAll = true;
					m_pActiveFile->PushUpdateData(data2);
				}

			}
			return TRUE;
		case LW_CM_MERGE_UNDER_LAYER:
			if (m_PopupLayerPanel) {
				ImgLayer_Ptr pLayer = std::dynamic_pointer_cast<ImgLayer>(m_PopupLayerPanel->GetLayer());
				assert(pLayer->GetLayerType() != IE_LAYER_TYPE::GROUP_LAYER);

				IImgLayer_Ptr pUnderLayer = pLayer->GetUnderLayer().lock();
				assert(pUnderLayer);
				assert(pUnderLayer->GetLayerType() != IE_LAYER_TYPE::GROUP_LAYER);

				MergeLayerHandle* handle = (MergeLayerHandle*) m_pActiveFile->CreateImgFileHandle(IFH_MERGE_LAYER);
				handle->AddMergingLayer(pLayer);
				handle->AddMergingLayer(pUnderLayer);
				m_pActiveFile->DoImgFileHandle( handle );

				m_pActiveFile->SetSelectLayer( handle->GetMergedLayer().lock() );
			}
			return TRUE;
		case LW_CM_MERGE_LAYER_GROUP:
			if (m_PopupLayerPanel) {
				ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(m_PopupLayerPanel->GetLayer());
				assert(pLGroup->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER);

				MergeLayerHandle* handle = (MergeLayerHandle*) m_pActiveFile->CreateImgFileHandle(IFH_MERGE_LAYER);
				handle->AddMergingLayer(pLGroup);
				m_pActiveFile->DoImgFileHandle( handle );

				m_pActiveFile->SetSelectLayer( handle->GetMergedLayer().lock() );
			}
			return TRUE;
		case LW_CM_MERGE_DISPLAY_LAYER:
			{
				ImgLayerGroup_Ptr pLGroup = (ImgLayerGroup_Ptr) m_pActiveFile->GetRootLayerGroup();
				MergeLayerHandle* handle = (MergeLayerHandle*) m_pActiveFile->CreateImgFileHandle(IFH_MERGE_LAYER);

				//add visible layers to handle
				IImgLayer_Ptr pLayer = pLGroup->GetBottomChildLayer();
				while (pLayer) {
					if (pLayer->IsVisible()) {
						handle->AddMergingLayer(pLayer);
					}
					pLayer = pLayer->GetOverLayer().lock();
				}
				m_pActiveFile->DoImgFileHandle(handle);

				m_pActiveFile->SetSelectLayer(handle->GetMergedLayer().lock());
			}
			return TRUE;
		case LW_CM_ADJUST_PANEL:
			this->AdjastPanel();
			return TRUE;
	}

	return CScrollWnd::OnCommand(wp, lp);
}

void CLayerFrame::OnLayerNameEditKillFocus()
{
	m_LayerNameEdit.EndEdit();
}

void CLayerFrame::AdjastScrollSizes()
{
	RECT rc;
	this->GetClientRect(&rc);
	size_t size = m_LayerPanel_Vec.size();

	CClientDC dc(NULL);
	OnPrepareDC(&dc);
	CSize sizeDoc;
	sizeDoc.cx = rc.right - rc.left;
	sizeDoc.cy = LAYER_PANEL_H * size;
	dc.LPtoDP(&sizeDoc);
	SetScrollSizes(MM_TEXT, sizeDoc);
}

////////////////////////////////////////////////////
VOID CLayerFrame::CreatePanel(
	ImgLayerGroup_Ptr pLGroup)
{
	CLayerPanel *lp;
	IImgLayer_Ptr pLayer = pLGroup->GetBottomChildLayer();
	while (pLayer != NULL) {
		if (pLayer->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER) {
			ImgLayerGroup_Ptr lgroup = std::dynamic_pointer_cast<ImgLayerGroup>(pLayer);
			m_LayerPanel_Vec.push_back( NULL );
			CreatePanel(std::dynamic_pointer_cast<ImgLayerGroup>(pLayer));

			lp = new CLayerGroupPanel(this->GetSafeHwnd());
		} else {
			lp = new CLayerPanel(this->GetSafeHwnd());
		}
		lp->SetLayer(pLayer);
		lp->SetLayerNameEdit(&m_LayerNameEdit);

		m_LayerPanel_Vec.push_back( lp );
		pLayer = pLayer->GetOverLayer().lock();
	}
}

////////////////////////////////////////////////////
/*!
*/
VOID CLayerFrame::ReSetAllPanel(ImgFile_Ptr pFile)
{
	//delete all layer panel
	ClearLayerPanel();

	if (pFile) {	

		ImgLayerGroup_Ptr pGLayer = pFile->GetRootLayerGroup();
		if (pGLayer) {
			CreatePanel(pGLayer);

			//set panel position and state
			AdjastPanel();

			InvalidateRect(NULL, FALSE);
		}
	}
}

////////////////////////////////////////////////////
/*!
*/
VOID CLayerFrame::AdjastPanel()
{
	if (!m_pActiveFile) return;

	RECT rc;
	this->GetClientRect(&rc);

	int panel_width = rc.right - rc.left;
	
	//
	IImgLayer_weakPtr select_layer = m_pActiveFile->GetSelectLayer();
	
	int tab_num=0;
	int draw_panel_cnt=0;
	bool is_close_layer = false;
	int close_layer_depth = 0;

	LayerPanel_Vec::reverse_iterator itr = m_LayerPanel_Vec.rbegin();
	for (; itr != m_LayerPanel_Vec.rend(); itr++) {
		if ((*itr) == NULL) { //is close dummy panel
			if (is_close_layer) {
				if (close_layer_depth == tab_num) {
					is_close_layer = false;
				}
			}

			tab_num--;
			continue;
		}

		RECT rc;
		if (is_close_layer) {
			rc.top    = INVISIBLE_PANEL_TOP;
			rc.left   = INVISIBLE_PANEL_LEFT;
			rc.bottom = INVISIBLE_PANEL_BOTTOM;
			rc.right  = INVISIBLE_PANEL_RIGHT;
		} else {
			rc.top = LAYER_PANEL_H*draw_panel_cnt + LAYER_PANEL_V_MARGIN;
			rc.left = TAB_LAYER_PANEL_W * tab_num + LAYER_PANEL_H_MARGIN;
			rc.bottom = rc.top + LAYER_PANEL_H - LAYER_PANEL_V_MARGIN*2;
			rc.right = rc.left + panel_width - (TAB_LAYER_PANEL_W * tab_num) - LAYER_PANEL_H_MARGIN*2;
			draw_panel_cnt++; //
		}
		(*itr)->SetPanelRect(&rc);

		if ((*itr)->GetLayer()->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER) {
			tab_num++;

			if (is_close_layer == false) {
				ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>((*itr)->GetLayer());
				if (!pLGroup->IsOpen()) {
					is_close_layer = true;
					close_layer_depth = tab_num;
				}
			}
		}

		//set panel state
		if ((*itr)->GetLayer() == select_layer.lock()) {
			(*itr)->Select();
			m_SelectLayerPanel = (*itr);
		} else {
			(*itr)->Newtral();
		}
	}

	InvalidateRect(NULL, FALSE);
}

////////////////////////////////////////////////////
/*!
	indexの位置のレイヤーパネルを削除する
	@param[in] index
*/
VOID CLayerFrame::DeleteLayerPanel(int index)
{
	assert(0 <= index && index < m_LayerPanel_Vec.size() );

	int size = m_LayerPanel_Vec.size();
	LayerPanel_Vec::iterator itr;
	if (index < m_LayerPanel_Vec.size()/2) {
		itr = m_LayerPanel_Vec.begin();
		for (int i=0; i<index; i++) {
			itr++;
		}
	} else {
		itr = m_LayerPanel_Vec.end();
		for (int i=m_LayerPanel_Vec.size(); i > index; i--) {
			itr--;
		}
	}

	CLayerPanel* lp = (*itr);
	m_LayerPanel_Vec.erase(itr);

	if (lp) {
		m_SelectLayerPanel = (lp == m_SelectLayerPanel) ? NULL : m_SelectLayerPanel;
		m_DragLayerPanel   = (lp == m_DragLayerPanel)   ? NULL : m_DragLayerPanel;
		m_PopupLayerPanel  = (lp == m_PopupLayerPanel)  ? NULL : m_PopupLayerPanel;
		delete lp;
	}
}

VOID CLayerFrame::InsertLayerPanel(int from_index, int to_index)
{
	if (from_index == to_index) return;

	//get insert_panel
	LayerPanel_Vec::iterator from_itr;
	if (from_index < m_LayerPanel_Vec.size()/2) {
		from_itr = m_LayerPanel_Vec.begin();
		for (int i=0; i<from_index; i++) {
			from_itr++;
		}
	} else {
		from_itr = m_LayerPanel_Vec.end();
		for (int i=m_LayerPanel_Vec.size(); i > from_index; i--) {
			from_itr--;
		}
	}
	CLayerPanel* insert_panel = (*from_itr);
	m_LayerPanel_Vec.erase(from_itr);

	if(from_index < to_index)
		to_index--;

	//insert
	LayerPanel_Vec::iterator to_itr;
	if (to_index < m_LayerPanel_Vec.size()/2) {
		to_itr = m_LayerPanel_Vec.begin();
		for (int i=0; i<to_index; i++) {
			to_itr++;
		}
	} else {
		to_itr = m_LayerPanel_Vec.end();
		for (int i=m_LayerPanel_Vec.size(); i > to_index; i--) {
			to_itr--;
		}
	}
	m_LayerPanel_Vec.insert(to_itr, insert_panel);
}

///////////////////////////////////////////////////
/*!
	すべてのレイヤーパネルを再描画する。
*/
void CLayerFrame::AllPanelRePaint()
{
	this->Invalidate(FALSE);
}

////////////////////////////////////////////////////
/*!
	マウスの下にあるパネルのポインタを返す。
	@param[in] point クライアントマウス座標
	@return レイヤーパネルのポインタ
*/
CLayerPanel* CLayerFrame::GetLayerPanelFromPos(CPoint point)
{
	CPoint pt = GetScrollPosition();
	pt.x += point.x;
	pt.y += point.y;

	RECT rc;
	POINT lt, rb;
	LayerPanel_Vec::iterator itr = m_LayerPanel_Vec.begin();
	for (; itr != m_LayerPanel_Vec.end(); itr++) {
		if ((*itr) == NULL){
			continue;
		}

		//
		(*itr)->GetPanelRect(&rc);
		if ((rc.left <= pt.x && pt.x <= rc.right) &&
			(rc.top <= pt.y && pt.y <= rc.bottom))
		{
			return (*itr);
		}
	}
	return NULL;
}

////////////////////////////////////////////////////
/*!
	マウスの下にあるパネルのインデックスを返す。
	@param[in] point クライアントマウス座標
	@return レイヤーパネルのインデックス、見つからなければ(-1)
*/
int CLayerFrame::GetLayerPanelIndexFromPos(CPoint point)
{
	CPoint pt = GetScrollPosition();
	pt.x += point.x;
	pt.y += point.y;

	int cnt=0;
	RECT rc;
	LayerPanel_Vec::iterator itr = m_LayerPanel_Vec.begin();
	for (; itr != m_LayerPanel_Vec.end(); itr++) {
		if (*itr) {
			//パネルの位置を求める
			(*itr)->GetPanelRect(&rc);
			if ((rc.left <= pt.x && pt.x <= rc.right) &&
				(rc.top <= pt.y && pt.y <= rc.bottom))
			{
				return cnt;
			}
		}
		cnt++;
	}
	return -1;
}

////////////////////////////////////////////////////
/*!
	レイヤーパネルを削除する。
	@param[in,out] lp 削除するレイヤーパネル
*/
VOID CLayerFrame::DeleteLayerPanel(CLayerPanel *lp)
{
	if (lp) {
		lp->RemoveLayer();
		if (lp == m_SelectLayerPanel) {
			m_SelectLayerPanel = NULL;
		}

		LPUPDATE_DATA data = m_pActiveFile->CreateUpdateData();
		data->isAll = true;
		m_pActiveFile->PushUpdateData( data );
	}
}

////////////////////////////////////////////////////
/*!
	レイヤーパネルをすべて削除メモリー開放
*/
VOID CLayerFrame::ClearLayerPanel()
{
	LayerPanel_Vec::iterator itr = m_LayerPanel_Vec.begin();
	for (; itr != m_LayerPanel_Vec.end(); itr++) {
		if (*itr) {
			delete (*itr);
		}
	}
	m_LayerPanel_Vec.clear();
}