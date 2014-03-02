
// MainFrm.cpp : CMainFrame クラスの実装
//

#include "stdafx.h"
#include "ImgEdit_UI.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//test nann dayo
#define OPPAI_IPPAI 1
#define OPPAI_BARRAY 3
//
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	//ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_VS_2005, &CMainFrame::OnApplicationLook)
	//ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_VS_2005, &CMainFrame::OnUpdateApplicationLook)
	ON_MESSAGE(WM_APP_MOUSEWHEEL, CMainFrame::OnAppMouseWheel)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ステータス ライン インジケータ
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame コンストラクション/デストラクション

CMainFrame::CMainFrame()
{
	// TODO: メンバ初期化コードをここに追加してください。
	//theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2005);

	//
	m_bAutoMenuEnable = FALSE;
}

CMainFrame::~CMainFrame()
{
	m_hMenuDefault = m_hOldMenu;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_SYSKEYDOWN || pMsg->message == WM_SYSKEYUP) {
		if (pMsg->wParam == VK_MENU) {
			return TRUE;
		}
	}
	if (pMsg->message == WM_MOUSEWHEEL) {
		return TRUE;
	}
	return CMDIFrameWndEx::PreTranslateMessage(pMsg);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	//ウィンドウタイトル
	char wtxt[256];
#ifdef WIN64
	sprintf(wtxt, "Frayer (x64) (build %s)", __DATE__);
#else
	sprintf(wtxt, "Frayer (build %s)", __DATE__);
#endif //WIN64
	this->SetWindowText(wtxt);

	BOOL bNameValid;
	// 固定値に基づいてビジュアル マネージャと visual スタイルを設定します
	OnApplicationLook(theApp.m_nAppLook);

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // 使用可能なその他の視覚スタイル...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // タブ領域の右部に [閉じる] ボタンを配置するには、FALSE に設定します
	mdiTabParams.m_bTabIcons = FALSE;    // MDI タブでドキュメント アイコンを有効にするには、TRUE に設定します
	mdiTabParams.m_bAutoColor = FALSE;    // MDI タブの自動色設定を無効にするには、FALSE に設定します
	mdiTabParams.m_bDocumentMenu = TRUE; // タブ領域の右端にあるドキュメント メニューを有効にします
	EnableMDITabbedGroups(TRUE, mdiTabParams);


	if (!m_wndMenuBar.Create(this)){
		TRACE0("メニュー バーを作成できませんでした\n");
		return -1;      // 作成できませんでした。
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// アクティブになったときメニュー バーにフォーカスを移動しない
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("ツール バーの作成に失敗しました。\n");
		return -1;      // 作成できませんでした。
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// ユーザー定義のツール バーの操作を許可します:
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("ステータス バーの作成に失敗しました。\n");
		return -1;      // 作成できない場合
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: ツール バーおよびメニュー バーをドッキング可能にしない場合は、この 5 つの行を削除します
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// Visual Studio 2005 スタイルのドッキング ウィンドウ動作を有効にします
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 スタイルのドッキング ウィンドウの自動非表示動作を有効にします
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// メニュー項目イメージ (どの標準ツール バーにもないイメージ) を読み込みます:
	//CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// ドッキング ウィンドウを作成します
	if (!CreateDockingWindows())
	{
		TRACE0("ドッキング ウィンドウを作成できませんでした\n");
		return -1;
	}

	//ツールウィンドウ追加
	m_wndTool.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndTool);
	//エフェクトツリーウィンドウ追加
	m_wndEffectTree.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndEffectTree);
	//HSVサークルウィンドウ追加
	m_wndHSVCircle.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndHSVCircle);
	//RGBスライダーウィンドウ追加
	m_wndRGBSlider.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndRGBSlider);
	//ブラシウィンドウ追加
	m_wndBrushSelect.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndBrushSelect);
	//ナビゲーターウィンドウ追加
	m_wndNavigator.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndNavigator);
	//レイヤーウィンドウ追加
	m_wndLayer.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndLayer);
	//ヒストリウィンドウ追加
	m_wndHistory.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndHistory);
	//ツールエディットウィンドウ追加
	m_wndToolEdit.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndToolEdit);
	//パレットウィンドウ追加
	m_wndPalette.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndPalette);
	////マスクウィンドウ追加
	//m_wndMask.EnableDocking(CBRS_ALIGN_ANY);
	//DockPane(&m_wndMask);

	// 拡張ウィンドウ管理ダイアログ ボックスを有効にします
	EnableWindowsDialog(ID_WINDOW_MANAGER, IDS_WINDOWS_MANAGER, TRUE);

	// ツール バーとドッキング ウィンドウ メニューの配置変更を有効にします
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// ツール バーのクイック (Alt キーを押しながらドラッグ) カスタマイズを有効にします
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// ユーザー定義のツール バー イメージを読み込みます
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			m_UserImages.SetImageSize(CSize(16, 16), FALSE);
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// メニューのパーソナル化 (最近使用されたコマンド) を有効にします
	// TODO: ユーザー固有の基本コマンドを定義し、各メニューをクリックしたときに基本コマンドが 1 つ以上表示されるようにします。
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_SAVE_AS);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	//lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	//lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	//lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	//lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	//lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	//lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	//lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_SORTING_SORTALPHABETIC);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYTYPE);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYACCESS);
	lstBasicCommands.AddTail(ID_SORTING_GROUPBYTYPE);

	lstBasicCommands.AddTail( IDIE_BRUSHLOAD );
	lstBasicCommands.AddTail( IDIE_TOOLCONFIG );
	lstBasicCommands.AddTail( IDIE_OPEN_FILE_FROM_SCANER );

	for (int id = g_ImgEdit.GetMinCommandID(); id <= g_ImgEdit.GetMaxCommandID(); id++) {
		lstBasicCommands.AddTail( id );
	}

	CMFCToolBar::SetBasicCommands( lstBasicCommands );

	//スタイル指定
	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	//ツールウィンドウ作成
	if (!m_wndTool.Create(
		"ツール",
		this,
		CRect(0, 0, 42, 300),
		TRUE,
		ID_TOOL_WND,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("ツールウィンドウを作成できませんでした\n");
		return FALSE;
	}
	//ヒストリウィンドウ作成
	if (!m_wndHistory.Create(
		"ヒストリ",
		this,
		CRect(500, 500, 800, 800),
		TRUE,
		ID_HISTORY_WND,
		WS_CHILD | /*WS_VISIBLE |*/ WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("ヒストリウィンドウを作成できませんでした\n");
		return FALSE;
	}
	//ブラシウィンドウ作成
	if (!m_wndBrushSelect.Create(
		"ブラシ",
		this,
		CRect(300, 300, 600, 600),
		TRUE,
		ID_BRUSH_SELECT,
		WS_CHILD | /*WS_VISIBLE |*/ WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("ブラシウィンドウを作成できませんでした\n");
		return FALSE;
	}
	//エフェクトツリーウィンドウ作成
	if (!m_wndEffectTree.Create(
		"エフェクト",
		this,
		CRect(0, 0, 300, 300),
		TRUE,
		ID_EFFECT_TREE,
		WS_CHILD | /*WS_VISIBLE |*/ WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("エフェクトツリーウィンドウを作成できませんでした\n");
		return FALSE;
	}
	//HSVサークルウィンドウ作成
	if (!m_wndHSVCircle.Create(
		"HSVサークル",
		this,
		CRect(100, 100, 400, 400),
		TRUE,
		ID_HSV_CIRCLE_WND,
		WS_CHILD | /*WS_VISIBLE |*/ WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("HSVサークルウィンドウを作成できませんでした\n");
		return FALSE;
	}
	//RGBスライダーウィンドウ作成
	if (!m_wndRGBSlider.Create(
		"RGBスライダー",
		this,
		CRect(0, 0, 300, 300),
		TRUE,
		ID_RGB_SLIDER_WND,
		WS_CHILD | /*WS_VISIBLE |*/ WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("RGBスライダーウィンドウを作成できませんでした\n");
		return FALSE;
	}
	//レイヤーウィンドウ作成
	if (!m_wndLayer.Create(
		"レイヤー",
		this,
		CRect(200, 200, 500, 500),
		TRUE,
		ID_LAYER_WND,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("レイヤーウィンドウを作成できませんでした\n");
		return FALSE;
	}

	//ナビゲーターウィンドウ作成
	if (!m_wndNavigator.Create(
		"ナビゲーター",
		this,
		CRect(400, 400, 700, 700),
		TRUE, ID_NAVIGATOR_WND,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("ナビゲーターウィンドウを作成できませんでした\n");
		return FALSE;
	}

	//ツールエディットウィンドウ作成
	if (!m_wndToolEdit.Create(
		"ツールエディット",
		this,
		CRect(600, 600, 900, 670),
		TRUE,
		ID_TOOL_EDIT_WND,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOP | CBRS_FLOAT_MULTI))
	{
		TRACE0("ツールエディットウィンドウを作成できませんでした\n");
		return FALSE;
	}

	//パレットウィンドウ作成
	if (!m_wndPalette.Create(
		"パレット",
		this,
		CRect(600, 200, 800, 400),
		TRUE,
		ID_PALETTE_WND,
		WS_CHILD | /*WS_VISIBLE |*/ WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("パレットウィンドウを作成出来ませんでした\n");
		return FALSE;
	}
	////マスクウィンドウ作成
	//if (!m_wndMask.Create("マスク", this, CRect(700, 600, 900, 900), TRUE, ID_MASK_WND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	//{
	//	TRACE0("マスクウィンドウを作成できませんでした\n");
	//	return FALSE;
	//}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	UpdateMDITabbedBarsIcons();
}

// CMainFrame 診断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame メッセージ ハンドラ

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* メニューをスキャンします*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	//case ID_VIEW_APPLOOK_WIN_2000:
	//	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
	//	break;

	//case ID_VIEW_APPLOOK_OFF_XP:
	//	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
	//	break;

	//case ID_VIEW_APPLOOK_WIN_XP:
	//	CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
	//	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
	//	break;

	//case ID_VIEW_APPLOOK_OFF_2003:
	//	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
	//	CDockingManager::SetDockingMode(DT_SMART);
	//	break;

	//case ID_VIEW_APPLOOK_VS_2005:
	//	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
	//	CDockingManager::SetDockingMode(DT_SMART);
	//	break;

	default:
		//switch (theApp.m_nAppLook)
		//{
		//case ID_VIEW_APPLOOK_OFF_2007_BLUE:
		//	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
		//	break;

		//case ID_VIEW_APPLOOK_OFF_2007_BLACK:
		//	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
		//	break;

		//case ID_VIEW_APPLOOK_OFF_2007_SILVER:
		//	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
		//	break;

		//case ID_VIEW_APPLOOK_OFF_2007_AQUA:
		//	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
		//	break;
		//}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if(g_ImgEdit.RunCommandAsync(LOWORD(wParam), NULL))
		return TRUE;

	switch (LOWORD(wParam)) {
		case IDIE_BRUSHLOAD:
			{
				TCHAR strFile[MAX_PATH] = "";
				OPENFILENAME ofn = {0};

				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = this->GetSafeHwnd();
				ofn.lpstrFilter = "Adobe PhotoShop Brush (.abr)\0*.abr\0";
				ofn.lpstrFile = strFile;
				ofn.nMaxFile = MAX_PATH;
				ofn.Flags = OFN_FILEMUSTEXIST;

				//ダイアログを開く
				::GetOpenFileName(&ofn);
				if(strcmp(strFile, "") != 0){
					g_ImgEdit.LoadBrush(strFile);
				}
			}
			return TRUE;
		case IDIE_TOOLCONFIG:
			{
				HWND hWnd = this->GetSafeHwnd();
				//HINSTANCE hInst = (HINSTANCE)::GetWindowLong(hWnd, GWL_HINSTANCE);
				HINSTANCE hInst = ::AfxGetInstanceHandle();
				IEConfigDialog dlg;
				dlg.ShowDialog(hInst, hWnd, &g_ImgEdit);
			}
			return TRUE;
		case IDIE_OPEN_FILE_FROM_SCANER:
			{
				CImgEdit_UIApp* pApp = (CImgEdit_UIApp*)AfxGetApp();
				pApp->OnFileOpenFromScaner();
			}
			return TRUE;
	}
	return CMDIFrameWndEx::OnCommand(wParam, lParam);
}

LRESULT CMainFrame::OnAppMouseWheel(WPARAM wParam, LPARAM lParam)
{
	//UINT nFlags = GET_KEYSTATE_WPARAM(wParam);
	//short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	//CPoint pt;
	//pt.x = GET_X_LPARAM(lParam); 
	//pt.y = GET_Y_LPARAM(lParam);
	////m_PaletteFrame.DoMouseWheel(nFlags, zDelta, pt);
	//
	CWnd* pView = this->GetActiveView();
	pView->SendMessage(WM_APP_MOUSEWHEEL, wParam, lParam);
	return TRUE;
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 基本クラスが実際の動作を行います。
	if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	//メニューの変更
	this->InitIEMenu();
	m_wndMenuBar.CreateFromMenu(m_hIEMenu);
	m_hOldMenu = m_hMenuDefault;
	m_hMenuDefault = m_hIEMenu;


	// すべてのユーザー定義ツール バーのボタンのカスタマイズを有効にします
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}

void CMainFrame::InitIEMenu()
{
	m_hIEMenu = IEMenuMG::CreateIEMenu();

	m_hMenuFile      = IEMenuMG::CreateIEMenu();
	m_hMenuEdit      = IEMenuMG::CreateIEMenu();
	m_hMenuImage     = IEMenuMG::CreateIEMenu();
	m_hMenuDisplay   = IEMenuMG::CreateIEMenu();
	m_hMenuWindow    = IEMenuMG::CreateIEMenu();
	m_hMenuImgSelect = IEMenuMG::CreateIEMenu();
	m_hMenuTool      = IEMenuMG::CreateIEMenu();
	m_hMenuHelp      = IEMenuMG::CreateIEMenu();

	IEMenuItemInfo ieii;
	
	//ファイル
	ieii.SetMask(MIIM_STRING | MIIM_ID | MIIM_STATE);
	ieii.SetState(MFS_ENABLED);

	ieii.SetTypeData(TEXT("新規作成"));
	ieii.SetID( ID_FILE_NEW );
	IEMenuMG::InsertIEMenuItem(m_hMenuFile, 0, TRUE, &ieii);

	ieii.SetTypeData(TEXT("ファイルを開く"));
	ieii.SetID( ID_FILE_OPEN );
	IEMenuMG::InsertIEMenuItem(m_hMenuFile, 1, TRUE, &ieii);

	ieii.SetTypeData(TEXT("上書き保存"));
	ieii.SetID( ID_FILE_SAVE );
	IEMenuMG::InsertIEMenuItem(m_hMenuFile, 2, TRUE, &ieii);

	ieii.SetTypeData(TEXT("名前をつけて保存"));
	ieii.SetID( ID_FILE_SAVE_AS );
	IEMenuMG::InsertIEMenuItem(m_hMenuFile, 3, TRUE, &ieii);

	ieii.SetTypeData(TEXT("スキャナから読み込み"));
	ieii.SetID( IDIE_OPEN_FILE_FROM_SCANER );
	IEMenuMG::InsertIEMenuItem(m_hMenuFile, 4, TRUE, &ieii);

#ifdef _DEBUG
	ieii.SetTypeData(TEXT("イベントキャプチャを開始する"));
	ieii.SetID( g_ImgEdit.GetCommandID(TEXT("StartEventCapture")) );
	IEMenuMG::InsertIEMenuItem(m_hMenuFile, 6, TRUE, &ieii);

	ieii.SetTypeData(TEXT("イベントキャプチャを終了する"));
	ieii.SetID( g_ImgEdit.GetCommandID(TEXT("EndEventCapture")) );
	IEMenuMG::InsertIEMenuItem(m_hMenuFile, 7, TRUE, &ieii);

	ieii.SetTypeData(TEXT("イベントキャプチャを再生する"));
	ieii.SetID( g_ImgEdit.GetCommandID(TEXT("PlayEventCapture")) );
	IEMenuMG::InsertIEMenuItem(m_hMenuFile, 8, TRUE, &ieii);

	ieii.SetTypeData(TEXT("終了"));
	ieii.SetID( ID_APP_EXIT );
	IEMenuMG::InsertIEMenuItem(m_hMenuFile, 10, TRUE, &ieii);

	ieii.SetMask(MIIM_FTYPE);
	ieii.SetType(MFT_SEPARATOR);
	IEMenuMG::InsertIEMenuItem(m_hMenuFile, 5, TRUE, &ieii);
	IEMenuMG::InsertIEMenuItem(m_hMenuFile, 9, TRUE, &ieii);
#else
	ieii.SetTypeData(TEXT("終了"));
	ieii.SetID( ID_APP_EXIT );
	IEMenuMG::InsertIEMenuItem(m_hMenuFile, 6, TRUE, &ieii);

	ieii.SetMask(MIIM_FTYPE);
	ieii.SetType(MFT_SEPARATOR);
	IEMenuMG::InsertIEMenuItem(m_hMenuFile, 5, TRUE, &ieii);
#endif //_DEBUG

	//編集
	ieii.SetMask(MIIM_STRING | MIIM_ID | MIIM_STATE);
	ieii.SetState(MFS_ENABLED);

	ieii.SetTypeData(TEXT("やり直し"));
	ieii.SetID( g_ImgEdit.GetCommandID(TEXT("Redo")) );
	IEMenuMG::InsertIEMenuItem(m_hMenuEdit, 0, TRUE, &ieii);

	ieii.SetTypeData(TEXT("元に戻す"));
	ieii.SetID( g_ImgEdit.GetCommandID(TEXT("Undo")) );
	IEMenuMG::InsertIEMenuItem(m_hMenuEdit, 1, TRUE, &ieii);

	//画像
	ieii.SetMask(MIIM_STRING | MIIM_ID | MIIM_STATE);
	ieii.SetState(MFS_ENABLED);

	ieii.SetTypeData(TEXT("トーンカーブ"));
	ieii.SetID( g_ImgEdit.GetCommandID(TEXT("ToneCurve")) );
	IEMenuMG::InsertIEMenuItem(m_hMenuImage, 0, TRUE, &ieii);

	ieii.SetTypeData(TEXT("レベル補正"));
	ieii.SetID( g_ImgEdit.GetCommandID(TEXT("LevelCorrection")) );
	IEMenuMG::InsertIEMenuItem(m_hMenuImage, 1, TRUE, &ieii);

	ieii.SetTypeData(TEXT("カラーバランス"));
	ieii.SetID( g_ImgEdit.GetCommandID(TEXT("ColorBalance")) );
	IEMenuMG::InsertIEMenuItem(m_hMenuImage, 2, TRUE, &ieii);

	ieii.SetTypeData(TEXT("キャンバスサイズ変更"));
	ieii.SetID( g_ImgEdit.GetCommandID(TEXT("ChangeCanvasSize")) );
	IEMenuMG::InsertIEMenuItem(m_hMenuImage, 4, TRUE, &ieii);

	ieii.SetTypeData(TEXT("画像解像度変更"));
	ieii.SetID( g_ImgEdit.GetCommandID(TEXT("ChangeResolution")) );
	IEMenuMG::InsertIEMenuItem(m_hMenuImage, 5, TRUE, &ieii);

	ieii.SetMask(MIIM_FTYPE);
	ieii.SetType(MFT_SEPARATOR);
	IEMenuMG::InsertIEMenuItem(m_hMenuImage, 3, TRUE, &ieii);

	//画像選択
	ieii.SetMask(MIIM_STRING | MIIM_ID | MIIM_STATE);
	ieii.SetState(MFS_ENABLED);

	ieii.SetTypeData(TEXT("すべてを選択"));
	ieii.SetID( g_ImgEdit.GetCommandID(TEXT("ImgSelectAll")) );
	IEMenuMG::InsertIEMenuItem(m_hMenuImgSelect, 0, TRUE, &ieii);

	ieii.SetTypeData(TEXT("選択を解除"));
	ieii.SetID( g_ImgEdit.GetCommandID(TEXT("ImgSelectRelease")) );
	IEMenuMG::InsertIEMenuItem(m_hMenuImgSelect, 1, TRUE, &ieii);

	ieii.SetTypeData(TEXT("選択範囲を反転"));
	ieii.SetID( g_ImgEdit.GetCommandID(TEXT("ImgSelectFlip")) );
	IEMenuMG::InsertIEMenuItem(m_hMenuImgSelect, 2, TRUE, &ieii);

	//表示
	ieii.SetMask(MIIM_STRING | MIIM_ID | MIIM_STATE);
	ieii.SetState(MFS_ENABLED);

	ieii.SetTypeData(TEXT("左右反転"));
	ieii.SetID( g_ImgEdit.GetCommandID(TEXT("ViewFlipH")) );
	IEMenuMG::InsertIEMenuItem(m_hMenuDisplay, 0, TRUE, &ieii);

	ieii.SetTypeData(TEXT("上下反転"));
	ieii.SetID( g_ImgEdit.GetCommandID(TEXT("ViewFlipV")) );
	IEMenuMG::InsertIEMenuItem(m_hMenuDisplay, 1, TRUE, &ieii);

	//ウィンドウ
	ieii.SetMask(MIIM_STRING | MIIM_ID | MIIM_STATE);
	ieii.SetState(MFS_ENABLED);

	ieii.SetTypeData(TEXT("ウィンドウ"));
	ieii.SetID( ID_VIEW_TOOLBAR );
	IEMenuMG::InsertIEMenuItem(m_hMenuWindow, 0, TRUE, &ieii);

	//ツール
	ieii.SetMask(MIIM_STRING | MIIM_ID | MIIM_STATE);
	ieii.SetState(MFS_ENABLED);

	ieii.SetTypeData(TEXT("ブラシの読み込み"));
	ieii.SetID( IDIE_BRUSHLOAD );
	IEMenuMG::InsertIEMenuItem(m_hMenuTool, 0, TRUE, &ieii);

	ieii.SetTypeData(TEXT("設定"));
	ieii.SetID( IDIE_TOOLCONFIG );
	IEMenuMG::InsertIEMenuItem(m_hMenuTool, 1, TRUE, &ieii);

	//各メニューを統合
	ieii.SetMask(MIIM_TYPE | MIIM_SUBMENU);
	ieii.SetType(MFT_STRING);

	ieii.SetSubMenu(m_hMenuFile);
	ieii.SetTypeData(TEXT("ファイル"));
	IEMenuMG::InsertIEMenuItem(m_hIEMenu, 0, TRUE, &ieii);

	ieii.SetSubMenu(m_hMenuEdit);
	ieii.SetTypeData(TEXT("編集"));
	IEMenuMG::InsertIEMenuItem(m_hIEMenu, 1, TRUE, &ieii);

	ieii.SetSubMenu(m_hMenuImage);
	ieii.SetTypeData(TEXT("画像"));
	IEMenuMG::InsertIEMenuItem(m_hIEMenu, 2, TRUE, &ieii);

	ieii.SetSubMenu(m_hMenuImgSelect);
	ieii.SetTypeData(TEXT("範囲選択"));
	IEMenuMG::InsertIEMenuItem(m_hIEMenu, 3, TRUE, &ieii);

	ieii.SetSubMenu(m_hMenuDisplay);
	ieii.SetTypeData(TEXT("表示"));
	IEMenuMG::InsertIEMenuItem(m_hIEMenu, 4, TRUE, &ieii);

	ieii.SetSubMenu(m_hMenuWindow);
	ieii.SetTypeData(TEXT("ウィンドウ"));
	IEMenuMG::InsertIEMenuItem(m_hIEMenu, 5, TRUE, &ieii);

	ieii.SetSubMenu(m_hMenuTool);
	ieii.SetTypeData(TEXT("ツール"));
	IEMenuMG::InsertIEMenuItem(m_hIEMenu, 6, TRUE, &ieii);

	ieii.SetSubMenu(m_hMenuHelp);
	ieii.SetTypeData(TEXT("ヘルプ"));
	IEMenuMG::InsertIEMenuItem(m_hIEMenu, 7, TRUE, &ieii); 
}