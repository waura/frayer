
// ImgEdit_UI.h : ImgEdit_UI アプリケーションのメイン ヘッダー ファイル
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"       // メイン シンボル
#include "ImgEdit.h"
#include "LayerSynthesizerMG.h"
#include "EffectMG.h"
#include "IEToolMG.h"
#include "IECommandMG.h"
#include "ImgFileIOMG.h"
#include "BrushPopupWnd.h"
#include "ImgEdit_UIDoc.h"
#include "LoadFromTWAIN.h"

// CImgEdit_UIApp:
// このクラスの実装については、ImgEdit_UI.cpp を参照してください。
//

class CImgEdit_UIApp : public CWinAppEx
{
public:
	CImgEdit_UIApp();
	~CImgEdit_UIApp();

// オーバーライド
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	void HookKeyboard() {
		m_isHookKeyboard = true;
	}
	void UnhookKeyBoard(){
		m_isHookKeyboard = false;
	}

	void HookMouse() {
		m_isHookMouse = true;
	}
	void UnhookMouse() {
		m_isHookMouse = false;
	}

// 実装
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	void OnFileOpenFromScaner();

	BOOL DoOrgPromptFileName(CString& fileName, UINT nIDSTitle,
		DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate);


	afx_msg BOOL OnIdle(LONG lCount);
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	DECLARE_MESSAGE_MAP()
//
private:
	static LRESULT CALLBACK GetMessageHookProc(int nCode, WPARAM wParam, LPARAM lParam);
	//static LRESULT CALLBACK CallWndHookProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT KeyboardHookProc(UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT MouseWheelHookProc(UINT message, WPARAM wParam, LPARAM lParam);

	CBrushPopupWnd m_BrushPopupWnd;
	LoadFromTWAIN* m_pLoadFromTWAIN;

	static bool m_isHookKeyboard;
	static bool m_isHookMouse;
	static HHOOK m_hGetMessageHook;
	//static HHOOK m_hCallWndHook;
};

extern CImgEdit_UIApp theApp;
