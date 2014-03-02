#include "stdafx.h"

#include "NavigatorWnd.h"
#include "MainFrm.h"
#include "ImgEdit_UI.h"

#define VIEW_CTRL_H 26
#define SCROLL_BAR_H 40

#define ID_VIEW_CTRL_PANE 1
#define ID_PREVIEW_FRAME 2
#define ID_FILE_SIZE_SCROLL 3

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern ImgEdit g_ImgEdit;


CNavigatorWnd::CNavigatorWnd()
{
}

CNavigatorWnd::~CNavigatorWnd()
{
	_ASSERTE(_CrtCheckMemory());
}

BEGIN_MESSAGE_MAP(CNavigatorWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(WM_APP_MOUSEWHEEL, CNavigatorWnd::OnAppMouseWheel)
END_MESSAGE_MAP()

int CNavigatorWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CDockablePane::OnCreate(lpCreateStruct)){
		return -1;
	}

	RECT rc;
	BOOL ret;

	rc.left = 0;
	rc.right = lpCreateStruct->cx;

	//
	rc.top = 0;	
	rc.bottom = VIEW_CTRL_H;
	ret = m_wndViewCtrlPane.Create(
		NULL,
		"",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		rc,
		this,
		ID_VIEW_CTRL_PANE);
	if (!ret) {
		return -1;
	}

	//ファイルイメージプレビューワー
	rc.top = VIEW_CTRL_H;
	rc.bottom = lpCreateStruct->cy - SCROLL_BAR_H;
	ret = m_wndPreviewFrame.Create(
		NULL,
		"",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		rc,
		this,
		ID_PREVIEW_FRAME);
	if (!ret) {
		return -1;
	}

	//ファイルイメージサイズスクロールバー
	rc.top = lpCreateStruct->cy - SCROLL_BAR_H;
	rc.bottom = lpCreateStruct->cy;
	ret = m_wndFileSizeScroll.Create(
		NULL,
		"",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		rc,
		this,
		ID_FILE_SIZE_SCROLL);
	if(!ret) {
		return -1;
	}

	return 0;
}

void CNavigatorWnd::OnSize(UINT nType, int cx, int cy)
{
	RECT rc;
	this->GetClientRect(&rc);

	m_wndViewCtrlPane.SetWindowPos(NULL, 0, 0, cx, VIEW_CTRL_H, SWP_NOZORDER);
	m_wndPreviewFrame.SetWindowPos(NULL, 0, VIEW_CTRL_H, cx, cy - (VIEW_CTRL_H + SCROLL_BAR_H), SWP_NOZORDER);
	m_wndFileSizeScroll.SetWindowPos(NULL, 0, cy-SCROLL_BAR_H, cx, SCROLL_BAR_H, SWP_NOZORDER);

	CDockablePane::OnSize(nType, cx, cy);
}

LRESULT CNavigatorWnd::OnAppMouseWheel(WPARAM wParam, LPARAM lParam)
{
	CImgEdit_UIApp* pApp = (CImgEdit_UIApp*)AfxGetApp();
	assert(pApp);
	CMainFrame* pMainWnd = dynamic_cast<CMainFrame*>(pApp->m_pMainWnd);
	assert(pMainWnd);
	CMDIChildWnd* pMDIChildWnd = pMainWnd->MDIGetActive();
	if (pMDIChildWnd) {
		CView* pView = pMDIChildWnd->GetActiveView();
		if (pView) {
			RECT viewrc;
			pView->GetWindowRect(&viewrc);
			pView->SendMessage(WM_APP_MOUSEWHEEL, wParam, lParam);
		}
	}

	return TRUE;
}