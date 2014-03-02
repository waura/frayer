
#include "stdafx.h"
#include "LoadFromTWAIN.h"

#pragma data_seg(.LoadFromTWAIN)
	HHOOK LoadFromTWAIN::m_hHook = NULL;
#pragma data_seg()

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static TW_IDENTITY g_tw_appID;
static TW_IDENTITY g_tw_dsID;
static DSMENTRYPROC g_fpDSM_Entry;
static bool g_isStage5;
static bool g_isFinish;
static ImgFile_Ptr g_img_file = NULL;

bool LoadFromTWAIN::Init(ImgEdit* pEdit)
{
	m_pImgEdit = pEdit;

	//load twain dll
	m_hDLL = LoadLibrary("twain_32.dll");
	if(m_hDLL == NULL){
		OutputError::Alert("cannot load twain dll");
		return false;
	}
	g_fpDSM_Entry = (DSMENTRYPROC)GetProcAddress( m_hDLL, MAKEINTRESOURCE(1) );

	TW_UINT16 twRC;
	//Open Data Source Manager
	g_tw_appID.Id = 0;
	g_tw_appID.Version.MajorNum = 1;
	g_tw_appID.Version.MinorNum = 0;
	g_tw_appID.Version.Language = TWLG_JAPANESE;
	g_tw_appID.Version.Country = TWCY_JAPAN;
	lstrcpy( g_tw_appID.Version.Info, "CTTwain 2001/01/19" );
	g_tw_appID.ProtocolMajor = TWON_PROTOCOLMAJOR;
	g_tw_appID.ProtocolMinor = TWON_PROTOCOLMINOR;
	g_tw_appID.SupportedGroups = DG_IMAGE | DG_CONTROL;
	lstrcpy( g_tw_appID.Manufacturer, "TSUGU" );
	lstrcpy( g_tw_appID.ProductFamily, "TGL" );
	lstrcpy( g_tw_appID.ProductName, "TGL" );

	m_hWnd = ::GetForegroundWindow();
	twRC = g_fpDSM_Entry( &g_tw_appID, NULL, DG_CONTROL, DAT_PARENT, MSG_OPENDSM, (TW_MEMREF)&m_hWnd );
	if(twRC != TWRC_SUCCESS){
		OutputError::Alert("error: cant open data source manager");
		::FreeLibrary( m_hDLL );
		return false;
	}

	//Select Data Source
	TW_IDENTITY new_dsID;
	memset(&new_dsID, 0, sizeof(TW_IDENTITY));
	//Get Default Data Source
	twRC = g_fpDSM_Entry(&g_tw_appID, NULL, DG_CONTROL, DAT_IDENTITY, MSG_GETDEFAULT, (TW_MEMREF)&new_dsID);
	//Select Data Source
	twRC = g_fpDSM_Entry(&g_tw_appID, NULL, DG_CONTROL, DAT_IDENTITY, MSG_USERSELECT, (TW_MEMREF)&new_dsID);
	if(twRC == TWRC_SUCCESS){
		g_tw_dsID = new_dsID; 
	}

	//Open Data Source
	twRC = g_fpDSM_Entry( &g_tw_appID, NULL, DG_CONTROL, DAT_IDENTITY, MSG_OPENDS, &g_tw_dsID );
	if(twRC != TWRC_SUCCESS){
		::FreeLibrary( m_hDLL );
		return false;
	}

	//Data Source Enable
	TW_USERINTERFACE twUI;
	twUI.ShowUI = TRUE;
	twUI.ModalUI = TRUE;
	twUI.hParent = m_hWnd;
	g_fpDSM_Entry( &g_tw_appID, &g_tw_dsID, DG_CONTROL, DAT_USERINTERFACE, MSG_ENABLEDS, (TW_MEMREF)&twUI );
	if(twRC == TWRC_SUCCESS){
		::EnableWindow(m_hWnd, FALSE);
	}

	g_isStage5 = true;
	g_isFinish = false;

	//hook window message
	m_hHook = ::SetWindowsHookEx(
		WH_GETMESSAGE,
		(HOOKPROC)LoadFromTWAIN::HookProc,
		NULL,
		::GetCurrentThreadId());

	if( !m_hHook ){
		::FreeLibrary( m_hDLL );
		g_isFinish = true;
		return false;
	}

	return true;
}

void LoadFromTWAIN::End(ImgEdit* pEdit)
{
	//Data Source Disable
	TW_USERINTERFACE twUI;
	g_fpDSM_Entry( &g_tw_appID, &g_tw_dsID, DG_CONTROL, DAT_USERINTERFACE, MSG_DISABLEDS, (TW_MEMREF)&twUI );
	::EnableWindow(m_hWnd, TRUE);

	//Close Data Source
	g_fpDSM_Entry( &g_tw_appID, NULL, DG_CONTROL, DAT_IDENTITY, MSG_CLOSEDS, &g_tw_dsID );

	//Close Data Source Manager
	g_fpDSM_Entry( &g_tw_appID, NULL, DG_CONTROL, DAT_PARENT, MSG_CLOSEDSM, &m_hWnd );

	//close twain dll
	FreeLibrary( m_hDLL );
	m_hDLL = NULL;

	//unhook window message
	BOOL res;
	if( m_hHook )
		res = ::UnhookWindowsHookEx( m_hHook );
	m_hHook = NULL;
}

BOOL LoadFromTWAIN::Run(ImgEdit* pEdit, void* pvoid)
{
	if(g_isFinish){
		return FALSE;
	}
	else{
		g_img_file = *((ImgFile_Ptr*)pvoid);
		::Sleep(200);
		return TRUE;
	}
}

static void DoNativeTransfer()
{
	TW_PENDINGXFERS twPendingXfer;
	TW_IMAGEINFO twImageInfo;
	TW_UINT16 twRC;
	HANDLE hBitmap;
	LPBITMAPINFOHEADER pDib;

	twPendingXfer.Count = -1;
	while( twPendingXfer.Count != 0 ) {
		//イメージ情報の取得
		g_fpDSM_Entry( &g_tw_appID, &g_tw_dsID, DG_IMAGE, DAT_IMAGEINFO, MSG_GET, &twImageInfo );
		::OutputDebugString("get scaner image\n");

		//get dib
		twRC = g_fpDSM_Entry( &g_tw_appID, &g_tw_dsID, DG_IMAGE, DAT_IMAGENATIVEXFER, MSG_GET, (TW_MEMREF)&hBitmap );
		if( twRC == TWRC_XFERDONE ) {
			//complete transform
			g_fpDSM_Entry( &g_tw_appID, &g_tw_dsID, DG_CONTROL, DAT_PENDINGXFERS, MSG_ENDXFER, (TW_MEMREF)&twPendingXfer );
		}
		else { //TWRC_CANCEL / TWRC_FAILURE
			//cansel dib transform
			g_fpDSM_Entry( &g_tw_appID, &g_tw_dsID, DG_CONTROL, DAT_PENDINGXFERS, MSG_RESET, (TW_MEMREF)&twPendingXfer );
			break;
		}

		void* lpDib = (void*) GlobalLock( hBitmap );
		LPBITMAPINFOHEADER lpbmih = (LPBITMAPINFOHEADER)lpDib;
		BYTE* lpvBits = (BYTE*)GetLpBits(lpDib);

		int height = lpbmih->biHeight;
		int width = lpbmih->biWidth;

		while( g_img_file == NULL ){
			::Sleep(100);
		}
		g_img_file->Init(cvSize(width, height));

		AddNewLayerHandle* handle = (AddNewLayerHandle*)g_img_file->CreateImgFileHandle(IFH_ADD_NEW_LAYER);
		handle->Do( g_img_file );

		ImgLayer_Ptr new_layer = handle->GetNewLayer().lock();
		g_img_file->SetSelectLayer( new_layer );

		new_layer->ExtendLayer(); //不足分があれば拡張

		UCvPixel pix;
		for(int y=0; y<height; y++){
			for(int x=0; x<width; x++){
				GetDIBPixel(lpbmih, lpvBits, x, y, &pix);
				pix.a = 255;
				//dib reverse y axis
				new_layer->SetPixel(x, height - y - 1, &pix);
			}
		}

		//
		LPUPDATE_DATA pData = new_layer->CreateUpdateData();
		pData->isAll = true;
		new_layer->PushUpdateData( pData );

		//
		g_img_file->ReleaseImgFileHandle(handle);
		handle = NULL;

		//
		NullHandle* null_handle = (NullHandle*)g_img_file->CreateImgFileHandle(IFH_NULL);
		strcpy_s(null_handle->handle_name, MAX_IMG_FILE_HANDLE_NAME, "スキャナから読み込み");
		g_img_file->DoImgFileHandle( null_handle );

		GlobalUnlock( hBitmap );
		GlobalFree( hBitmap );
	}
}

static void TransferImage()
{
	TW_CAPABILITY twCap;
	TW_PENDINGXFERS twPendingXfer;
	pTW_ONEVALUE ptwOneValue = NULL;
	TW_UINT16 twRC;
	DWORD dwType;
	memset( &twCap, 0, sizeof(TW_CAPABILITY) );
	twCap.Cap = ICAP_XFERMECH;
	twRC = g_fpDSM_Entry( &g_tw_appID, &g_tw_dsID, DG_CONTROL, DAT_CAPABILITY, MSG_GETCURRENT, (TW_MEMREF)&twCap );

	ptwOneValue = (pTW_ONEVALUE)GlobalLock( twCap.hContainer );
	dwType = ptwOneValue->Item;
	GlobalUnlock( twCap.hContainer );
	GlobalFree( twCap.hContainer );

	if( dwType == TWSX_NATIVE )  //ネイティブ転送の場合…
	    DoNativeTransfer(); //画像の転送
	else  // ネイティブ以外の転送の場合…
		g_fpDSM_Entry( &g_tw_appID, &g_tw_dsID, DG_CONTROL, DAT_PENDINGXFERS, MSG_RESET, &twPendingXfer );
}

LRESULT CALLBACK LoadFromTWAIN::HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	TW_EVENT twEvent;
	memset(&twEvent, 0, sizeof(TW_EVENT));

	TW_INT16 twRC = TWRC_NOTDSEVENT;
	MSG* pMsg = (MSG*)lParam;
	char str[256];
	if(nCode < 0){
		::OutputDebugString("ncode is minas\m");
		return CallNextHookEx(m_hHook, nCode, wParam, lParam);
	}
	if(nCode == HC_ACTION){
		if(wParam & PM_REMOVE){
			wsprintf(str, "message = %d \n", pMsg->message);
			::OutputDebugString( str );
			if( g_isStage5 ){
				twEvent.pEvent = (TW_MEMREF)pMsg;
				twEvent.TWMessage = MSG_NULL;
				twRC = g_fpDSM_Entry( &g_tw_appID, &g_tw_dsID, DG_CONTROL, DAT_EVENT, MSG_PROCESSEVENT, (TW_MEMREF)&twEvent );
				
				wsprintf(str, "twRC = %d \n", pMsg->message);
				::OutputDebugString( str );

				if(twRC == TWRC_DSEVENT){
					switch( twEvent.TWMessage ) {
						case MSG_XFERREADY:
							::OutputDebugString("start transfer image\n");
							TransferImage(); //画像転送
							::OutputDebugString("end transfer image\n");
							g_isStage5 = false;
							g_isFinish = true;
							break;
						case MSG_CLOSEDSREQ:
						case MSG_CLOSEDSOK:
							g_isFinish = true;
							break;
						case MSG_NULL:
							break;
					}
				}
			}
        }
	}

	return CallNextHookEx(m_hHook, nCode, wParam, lParam); 
}
