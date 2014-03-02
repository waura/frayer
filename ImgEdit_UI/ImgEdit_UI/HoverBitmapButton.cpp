#include "stdafx.h"
#include "HoverBitmapButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CHoverBitmapButton::LoadBitmaps(
	UINT nIDBitmapResource,
	UINT nIDBitmapResourceHov,
	UINT nIDBitmapResourceSel,
	UINT nIDBitmapResourceFocus,
	UINT nIDBitmapResourceDisabled)
{
	m_bitmapHov.DeleteObject();
	if(!m_bitmapHov.LoadBitmap(nIDBitmapResourceHov)){
		return FALSE;
	}

	return CBitmapButton::LoadBitmaps(
		nIDBitmapResource,
		nIDBitmapResourceSel,
		nIDBitmapResourceFocus);
}

void CHoverBitmapButton::DrawItem(LPDRAWITEMSTRUCT lpDis)
{
	if(m_isToggle){
		//í‚Éselected
		lpDis->itemState |= ODS_SELECTED;
	}

	if(m_isHover && !(lpDis->itemState & ODS_SELECTED)){
		CDC* pDC = CDC::FromHandle(lpDis->hDC);
		CDC memDC;
		if (memDC.CreateCompatibleDC(pDC) == FALSE)
			return;
		CBitmap* pOld = memDC.SelectObject(&m_bitmapHov);
		if (pOld == NULL)
			return; //destructors will clean up

		CRect rect;
		rect.CopyRect(&lpDis->rcItem);
		pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),
			&memDC, 0, 0, SRCCOPY);
		memDC.SelectObject(pOld);
	}
	else{
		CBitmapButton::DrawItem(lpDis);
	}
}

BEGIN_MESSAGE_MAP(CHoverBitmapButton, CBitmapButton)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

void CHoverBitmapButton::OnMouseMove(UINT nFlags, CPoint point)
{
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_HOVER | TME_LEAVE;
	tme.hwndTrack = this->GetSafeHwnd();
	tme.dwHoverTime = 1;
	::TrackMouseEvent(&tme);
	CBitmapButton::OnMouseMove(nFlags, point);
}

LRESULT CHoverBitmapButton::OnMouseHover(WPARAM wp, LPARAM lp)
{
	m_isHover = true;
	this->Invalidate(FALSE);
	return FALSE;
}

LRESULT CHoverBitmapButton::OnMouseLeave(WPARAM wp, LPARAM lp)
{
	m_isHover = false;
	this->Invalidate(FALSE);
	return FALSE;
}