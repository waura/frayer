#if !defined(AFX_SCROLLWND_H__99F78BC8_8F4F_4862_B0B3_F42505EAC39D__INCLUDED_)
#define AFX_SCROLLWND_H__99F78BC8_8F4F_4862_B0B3_F42505EAC39D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScrollWnd.h : header file
//
#include <afxpriv.h>
/////////////////////////////////////////////////////////////////////////////
// CScrollWnd window



class CScrollWnd : public CWnd
{
	int	m_nMapMode;
	CSize	m_totalLog;           // total size in logical units (no rounding)
	CSize	m_totalDev;           // total size in device units
	CSize	m_pageDev;            // per page scroll size in device units
	CSize	m_lineDev;            // per line scroll size in device units
	
	BOOL	m_bCenter;             // Center output if larger than total size
	BOOL	m_bInsideUpdate;       // internal state for OnSize callback

	static const SIZE	sizeDefault;
	
	void ScrollToDevicePosition(POINT ptDev); // explicit scrolling no checking
	void UpdateBars();          // adjust scrollbars etc
	BOOL GetTrueClientSize(CSize& size, CSize& sizeSb);
	void GetScrollBarSizes(CSize& sizeSb);
	void GetScrollBarState(CSize sizeClient, CSize& needSb,	CSize& sizeRange, CPoint& ptMove, BOOL bInsideClient);


protected:
	BOOL PreCreateWindow(CREATESTRUCT& cs);
	
	void CenterOnPoint(CPoint ptCenter);
	

		
	virtual void	CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType = adjustBorder);
	virtual void	OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);

	virtual void	OnDraw(CDC* pDC) = 0;
	virtual void	OnInitialUpdate() = 0;
	
	// scrolling implementation support for OLE
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);
	virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE);
	
	//{{AFX_MSG(CScrollView)
	afx_msg void OnPaint();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT fFlags, short zDelta, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CScrollWnd();
	virtual ~CScrollWnd();

	void	DocumentToClient(CPoint& pt);
	void	ClientToDocument(CPoint& pt);

	BOOL	Create(CWnd* pParent, const RECT& rc, DWORD dwStyle, UINT nID = 0);
	
	CPoint	GetScrollPosition() const;       // upper corner of scrolling
	CSize	GetTotalSize() const;             // logical size
	
	// for device units
	CPoint	GetDeviceScrollPosition() const;
	void	GetDeviceScrollSizes(int& nMapMode, SIZE& sizeTotal, SIZE& sizePage, SIZE& sizeLine) const;
	
	void	SetScrollSizes(int nMapMode, SIZE sizeTotal, const SIZE& sizePage = sizeDefault, const SIZE& sizeLine = sizeDefault);
	void	SetScaleToFitSize(SIZE sizeTotal);
	
	void	ScrollToPosition(POINT pt);    // set upper left position
	void	FillOutsideRect(CDC* pDC, CBrush* pBrush);
	void	ResizeParentToFit(BOOL bShrinkOnly = TRUE);
	BOOL	DoMouseWheel(UINT fFlags, short zDelta, CPoint point);
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCROLLWND_H__99F78BC8_8F4F_4862_B0B3_F42505EAC39D__INCLUDED_)
