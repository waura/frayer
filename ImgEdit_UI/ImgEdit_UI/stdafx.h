
// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Windows ヘッダーから使用されていない部分を除外します。
#endif



#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 一部の CString コンストラクタは明示的です。

// 一般的で無視しても安全な MFC の警告メッセージの一部の非表示を解除します。
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC のコアおよび標準コンポーネント
#include <afxext.h>         // MFC の拡張部分


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC の Internet Explorer 4 コモン コントロール サポート
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC の Windows コモン コントロール サポート
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC におけるリボンとコントロール バーのサポート

#include <mmsystem.h>
//
#include <stdio.h>
#include <math.h>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <twain.h>

#include <UtilOpenCV.h>
#include <libImgEdit.h>

//for detect memory leak
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <cstdlib>
	#include <crtdbg.h>
#endif


#define IDIE_BRUSHLOAD     1000
#define IDIE_TOOLCONFIG    1001
#define IDIE_OPEN_FILE_FROM_SCANER 1003

#define WM_APP_MOUSEWHEEL	(WM_APP + 1)
#define WM_APP_SYSKEYDOWN	(WM_APP + 2) //for alt key event
#define WM_APP_SYSKEYUP		(WM_APP + 3) //for alt key event


extern ImgEdit g_ImgEdit;
