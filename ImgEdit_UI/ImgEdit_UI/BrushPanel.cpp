#include "StdAfx.h"

#include "BrushPanel.h"
#include "IEEMBrushEditDialog.h"

#define ID_BP_LABEL 1
#define LABEL_H 15

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern ImgEdit g_ImgEdit;


CBrushPanel::CBrushPanel()
{
	m_pBrush = NULL;
	m_isSelectBrush = false;
}

CBrushPanel::~CBrushPanel()
{
	if (m_pBrush) {
		m_pBrush->DeleteEventListener( this );
	}
}

void CBrushPanel::OnChangeOrgRadius(int old_rad, int new_rad)
{
	this->Invalidate(FALSE);
}

void CBrushPanel::OnChangeBrushShape()
{
	this->Invalidate(FALSE);
}

void CBrushPanel::SetBrush(IEBrush_Ptr pBrush)
{
	m_pBrush = pBrush;
	m_pBrush->AddEventListener( this );
	this->Invalidate(FALSE);
}

///////////////////////////////////
/*!
	保持しているブラシを削除する。
*/
void CBrushPanel::DeleteBrush()
{
	m_pBrush->DeleteEventListener( this );
	g_ImgEdit.DeleteBrush( m_pBrush );
	m_pBrush = NULL;
}

///////////////////////////////////
/*!
	非選択状態にする
*/
void CBrushPanel::NewtralPanel(){
	m_isSelectBrush = false;
}

///////////////////////////////////
/*!
	選択状態にする
*/
void CBrushPanel::SelectPanel(){
	m_isSelectBrush = true;
	g_ImgEdit.SetSelectBrush(m_pBrush);
}

///////////////////////////////////
/*!
	再描画
	@param[in: lprect 再描画する範囲 全体のときはNULLを指定する。
*/
void CBrushPanel::RePaint(LPRECT lprect)
{
	this->InvalidateRect(lprect, FALSE);
}

//////////////////////////////////////////////
/*!
	管理しているブラシのポインタを返す
*/
IEBrush_Ptr CBrushPanel::GetBrush()
{
	return m_pBrush;
}

BEGIN_MESSAGE_MAP(CBrushPanel, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

int CBrushPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CWnd::OnCreate(lpCreateStruct)){
		return -1;
	}

	if(!m_Font.CreateFont(
		LABEL_H - 2,
		0,
		0,
		0,
		FW_NORMAL,
		FALSE,
		FALSE,
		0,
		SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FF_DONTCARE | DEFAULT_PITCH,
		"MS ゴシック")){
			return -1;
	}

	return 0;
}

void CBrushPanel::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
}

void CBrushPanel::OnPaint()
{
	CPaintDC dc(this); // 描画のデバイス コンテキスト

	BITMAPINFO bmpi;
	LPVOID pBit;

	CRect rectWnd;
	GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);

	rectWnd.InflateRect(1, 1);
	dc.FillSolidRect(rectWnd, ::GetSysColor(COLOR_WINDOW));

	if(!m_pBrush) return;

	RECT rc;
	this->GetClientRect(&rc);

	int c_w = rc.right - rc.left;
	int c_h = rc.bottom - rc.top;

	//ブラシのサムネイルの表示
	IplImage* sumne = m_pBrush->CreateBrushSampleImg(c_w, c_h - LABEL_H);
	if(sumne == NULL) return;

	//IplImageをDIBに転送
	IPLImageToDevice(dc.m_hDC, 0, 0, sumne->width, sumne->height, sumne, 0, 0);

	//draw brush radius
	CFont* old_font = dc.SelectObject(&m_Font);

	RECT rad_txt_rc;
	rad_txt_rc.left = 0; rad_txt_rc.right = c_w;
	rad_txt_rc.top = c_h - LABEL_H, rad_txt_rc.bottom = c_h;

	char rad_txt[256];
	wsprintf(rad_txt, "%d", m_pBrush->GetOrgRadius());
	dc.DrawText(rad_txt, strlen(rad_txt), &rad_txt_rc, DT_CENTER);

	dc.SelectObject(old_font);

	//選択中なら枠の表示
	if(m_isSelectBrush){
		HPEN hPen, hOldPen;

		hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
		hOldPen = (HPEN)SelectObject(dc.m_hDC, hPen);
		SelectObject(dc.m_hDC, GetStockObject(NULL_BRUSH));
		Rectangle(dc.m_hDC, 1, 1, (rc.right-rc.left), (rc.bottom-rc.top));
		SelectObject(dc.m_hDC, hOldPen);
		DeleteObject(hPen);
	}

	//サムネイルイメージの開放
	m_pBrush->ReleaseBrushSampleImg(&sumne);
}

afx_msg void CBrushPanel::OnLButtonDown(UINT nFlags, CPoint point)
{
	POINT pt;
	pt.x = point.x; pt.y = point.y;
	this->ClientToScreen(&pt);
	CWnd* parent = this->GetParent();
	parent->ScreenToClient(&pt);
	LPARAM newLp = pt.x | (pt.y << 16);
	this->GetParent()->SendMessage(WM_LBUTTONDOWN, MK_LBUTTON, newLp);
}

afx_msg void CBrushPanel::OnMouseMove(UINT nFlags, CPoint point)
{
	POINT pt;
	pt.x = point.x; pt.y = point.y;
	this->ClientToScreen(&pt);
	CWnd* parent = this->GetParent();
	parent->ScreenToClient(&pt);
	LPARAM newLp = pt.x | (pt.y << 16);
	this->GetParent()->SendMessage(WM_MOUSEMOVE, 0, newLp);
}

afx_msg void CBrushPanel::OnLButtonUp(UINT nFlags, CPoint point)
{
	POINT pt;
	pt.x = point.x; pt.y = point.y;
	this->ClientToScreen(&pt);
	CWnd* parent = this->GetParent();
	parent->ScreenToClient(&pt);
	LPARAM newLp = pt.x | (pt.y << 16);
	this->GetParent()->SendMessage(WM_LBUTTONUP, MK_LBUTTON, newLp);
}

afx_msg void CBrushPanel::OnRButtonDown(UINT nFlags, CPoint point)
{
	POINT pt;
	pt.x = point.x; pt.y = point.y;
	this->ClientToScreen(&pt);
	CWnd* parent = this->GetParent();
	parent->ScreenToClient(&pt);
	LPARAM newLp = pt.x | (pt.y << 16);
	this->GetParent()->SendMessage(WM_RBUTTONDOWN, MK_RBUTTON, newLp);
}

afx_msg void CBrushPanel::OnRButtonUp(UINT nFlags, CPoint point)
{
	POINT pt;
	pt.x = point.x; pt.y = point.y;
	::ClientToScreen( this->GetSafeHwnd(), &pt );

	//コンテキストメニューの作成・表示
	IEMENU hmenu = IEMenuMG::CreateIEPopupMenu();

	IEMenuItemInfo ieii;
	ieii.SetMask(MIIM_STRING | MIIM_ID | MIIM_STATE);
	ieii.SetState( MFS_ENABLED );

	ieii.SetTypeData(TEXT("削除"));
	ieii.SetID( CM_ID_BRUSH_DELETE );
	IEMenuMG::InsertIEMenuItem(hmenu, 0, TRUE, &ieii);

	ieii.SetTypeData(TEXT("ブラシ編集"));
	ieii.SetID( CM_ID_BRUSH_EDIT );
	IEMenuMG::InsertIEMenuItem(hmenu, 1, TRUE, &ieii);

	IEMenuMG::TrackPopupIEMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN,
		pt.x, pt.y, 0, this->GetSafeHwnd(), NULL);
	IEMenuMG::DestroyIEMenu(hmenu);
}

BOOL CBrushPanel::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam)){
		case CM_ID_BRUSH_DELETE:
			if(m_pBrush){
				g_ImgEdit.DeleteBrush(m_pBrush);
				m_pBrush = NULL;
			}
			return TRUE;
		case CM_ID_BRUSH_EDIT:
			if(m_pBrush){
				switch( m_pBrush->GetBrushType() ){
					case IE_EM_BRUSH:
						{
							CIEEMBrushEditDialog* dlg = new CIEEMBrushEditDialog();
							dlg->SetEditBrush( std::dynamic_pointer_cast<IEEMBrush>(m_pBrush) );
							BOOL ret = dlg->Create( this );
							if(ret == TRUE){
								dlg->ShowWindow( SW_SHOW );
							}
							else{
								::OutputDebugString("faild CIEEMBrushEditDialog::Create()\n");
							}
						}
						break;
					case IE_IMG_BRUSH:
						{
						}
						break;
					default:
						assert(0);
						break;
				}
			}
			return TRUE;
	}

	return CWnd::OnCommand(wParam, lParam);
}

void CBrushPanel::PostNcDestroy()
{
	CWnd::PostNcDestroy();
}