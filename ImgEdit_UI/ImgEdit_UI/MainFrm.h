
// MainFrm.h : CMainFrame クラスのインターフェイス
//

#pragma once

#include "ToolWnd.h"
#include "EffectTreeWnd.h"
#include "HSVCircleWnd.h"
#include "RGBSliderWnd.h"
#include "LayerWnd.h"
#include "BrushSelectWnd.h"
#include "NavigatorWnd.h"
#include "HistoryWnd.h"
#include "ToolEditWnd.h"
#include "PaletteWnd.h"
//#include "MaskWnd.h"
#include "IEConfigDialog.h"

enum {
	ID_TOOL_WND = 11,
	ID_EFFECT_TREE,
	ID_HSV_CIRCLE_WND,
	ID_RGB_SLIDER_WND,
	ID_LAYER_WND,
	ID_BRUSH_SELECT,
	ID_NAVIGATOR_WND,
	ID_TOOL_EDIT_WND,
	ID_HISTORY_WND,
	ID_PALETTE_WND,
	ID_MASK_WND,
	ID_BRUSH_POPUP_WND,
};

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// 属性
public:

// 操作
public:

// オーバーライド
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// 実装
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // コントロール バー用メンバ
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;

	//ドッキングウィンドウ
	CToolWnd			m_wndTool;
	CEffectTreeWnd		m_wndEffectTree;
	CHSVCircleWnd		m_wndHSVCircle;
	CRGBSliderWnd		m_wndRGBSlider;
	CLayerWnd			m_wndLayer;
	CBrushSelectWnd		m_wndBrushSelect;
	CNavigatorWnd		m_wndNavigator;
	CHistoryWnd			m_wndHistory;
	CToolEditWnd		m_wndToolEdit;
	CPaletteWnd			m_wndPalette;
	//CMaskWnd			m_wndMask;

// 生成された、メッセージ割り当て関数
protected:

	BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnAppMouseWheel(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);

private:
	void InitMenu();
	void InitIEMenu();

	HMENU m_hOldMenu;
	HMENU m_hIEMenu;
	HMENU m_hMenuFile;
	HMENU m_hMenuEdit;
	HMENU m_hMenuImage;
	HMENU m_hMenuDisplay;
	HMENU m_hMenuWindow;
	HMENU m_hMenuImgSelect;
	HMENU m_hMenuTool;
	HMENU m_hMenuHelp;
};


