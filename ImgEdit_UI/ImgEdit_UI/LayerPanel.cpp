#include "stdafx.h"

#include "LayerPanel.h"

#include "IEStringHandle.h"
#include "LayerSynthesizerMG.h"
#include "LayerStyleDialog.h"
#include "..\resource1.h"

#define ID_LP_THM_UPDATE_TIMER 1
#define ID_LP_OFF_MOUSE_TIMER 2

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static HWND g_hWnd;


CLayerPanel::CLayerPanel(HWND hParent)
{
	m_hParentWnd = hParent;
	m_IsSelect = false;
	m_IsOnMouse = false;
	m_LayerThm = cvCreateImage(cvSize(LP_LAYER_THM_W, LP_LAYER_THM_H), IPL_DEPTH_8U, 3);
	cvSet(m_LayerThm, cvScalar(255,255,255));

	m_Layer = NULL;
	m_nUpdateThmTimer = 0;
	m_nOffMouseTimer = 0;

	m_cEye.LoadBitmap(MAKEINTRESOURCE(IDB_EYE));
	m_cNEye.LoadBitmap(MAKEINTRESOURCE(IDB_NEYE));

	//set font for name edit box
	LOGFONT FontInfo;
	memset(&FontInfo, 0, sizeof(FontInfo));
	strcpy(FontInfo.lfFaceName, LP_FONT_NAME);
	FontInfo.lfWeight = FW_BOLD;
	FontInfo.lfHeight = LAYER_NAME_FONT_SIZE;
	FontInfo.lfCharSet = SHIFTJIS_CHARSET;
	m_LayerNameFont.CreateFontIndirect( &FontInfo );
}

CLayerPanel::~CLayerPanel()
{
	m_LayerNameFont.DeleteObject();

	if (m_LayerThm) {
		cvReleaseImage(&m_LayerThm);
	}

	m_Layer->DeleteEventListener(this);
}

void CLayerPanel::SetLayer(IImgLayer_Ptr pLayer)
{
	m_Layer = pLayer;

	//m_nUpdateThmTimer = this->SetTimer(ID_LP_THM_UPDATE_TIMER, 1000, 0);

	//RECT rc;
	//this->GetClientRect(&rc);
	//AdjastLayout(rc.right - rc.left, rc.bottom - rc.top);

	//regist event listener
	m_Layer->AddEventLilstener(this);
}

void CLayerPanel::SetLayerNameEdit(CLayerNameEdit* pEdit)
{
	m_pLayerNameEdit = pEdit;
}

////////////////////////////////////////////
/*!
	レイヤー更新イベント時に呼び出される。
*/
void CLayerPanel::OnUpdateLayer()
{
	//if(m_nUpdateThmTimer != 0){
	//	this->KillTimer(m_nUpdateThmTimer);
	//}
	//m_nUpdateThmTimer = this->SetTimer(ID_LP_THM_UPDATE_TIMER, 1000, 0);
	::InvalidateRect(m_hParentWnd, &m_LayerThmRect, FALSE);
}

void CLayerPanel::OnChangedLayerName(const char* new_name)
{
	::InvalidateRect(m_hParentWnd, &m_LayerPanelRect, FALSE);
}

void CLayerPanel::OnChangedLayerOpacity(uint8_t new_opacity)
{
	::InvalidateRect(m_hParentWnd, &m_LayerPanelRect, FALSE);
}

void CLayerPanel::OnChangedLayerSynthFourCC(uint32_t new_fourcc)
{
	::InvalidateRect(m_hParentWnd, &m_LayerPanelRect, FALSE);
}

void CLayerPanel::Select()
{
	if (m_IsSelect == false) {
		m_IsSelect = true;
		ImgFile_Ptr pFile = m_Layer->GetParentFile().lock();
		if(pFile){
			pFile->SetSelectLayer( m_Layer );
		}
		::InvalidateRect(m_hParentWnd, &m_LayerPanelRect, FALSE);
	}
}

void CLayerPanel::Newtral()
{
	if (m_IsSelect == true) {
		m_IsSelect = false;
		::InvalidateRect(m_hParentWnd, &m_LayerPanelRect, FALSE);
	}
}

void CLayerPanel::OnMouse()
{
	if (m_IsOnMouse == false) {
		m_IsOnMouse = true;
		::InvalidateRect(m_hParentWnd, &m_LayerPanelRect, FALSE);
	}
}

void CLayerPanel::LeaveMouse()
{
	if (m_IsOnMouse == true) {
		m_IsOnMouse = false;
		::InvalidateRect(m_hParentWnd, &m_LayerPanelRect, FALSE);
	}
}

//////////////////////////////////////////////////
///*!
//	不透明度をセットする
//	@param[in] alpha 不透明度
//*/
//void CLayerPanel::SetOpacity(int alpha)
//{
//	m_Layer->SetOpacity(alpha);
//}

//////////////////////////////////////////////
/*!
	レイヤースタイルダイアログを開く
*/
void CLayerPanel::OpenLayerStyleDialog(HWND hParent)
{
	if (m_Layer->GetLayerType() == IE_LAYER_TYPE::NORMAL_LAYER) {
		//CLayerStyleDialog dialog(CWnd::FromHandle(hParent));
		CLayerStyleDialog dialog;
		dialog.SetEditImgLayer(std::dynamic_pointer_cast<ImgLayer>(m_Layer));
		INT_PTR ret = dialog.DoModal();
		if (ret != IDOK) {
			char sret[256];
			wsprintf(sret, "%d", ret);
			OutputError::PushLog(LOG_LEVEL::_ERROR, "can't open layer style dialog, ", sret);
		}
	}
}

//////////////////////////////////////////////
/*!
	レイヤー削除
*/
void CLayerPanel::RemoveLayer()
{
	ImgFile_Ptr pFile = m_Layer->GetParentFile().lock();
	if(pFile){
		pFile->RemoveLayer( m_Layer );

		//イメージを再生成
		LPUPDATE_DATA data = pFile->CreateUpdateData();
		data->isAll = true;
		pFile->PushUpdateData( data );
	}
}

//////////////////////////////////////////////
/*!
	ポップアップメニュー表示
*/
void CLayerPanel::PopupMenu(HWND hParent, const LPPOINT lpt){
	//create and display menu
	IEMENU hmenu = IEMenuMG::CreateIEPopupMenu();	

	IEMenuItemInfo ieii;
	ieii.SetMask(MIIM_STRING | MIIM_ID | MIIM_STATE );
	ieii.SetState( MFS_ENABLED );

	ieii.SetTypeData(TEXT("レイヤーを複製する"));
	ieii.SetID( LW_CM_COPY_LAYER );
	IEMenuMG::InsertIEMenuItem(hmenu, 0, TRUE, &ieii);

	ieii.SetTypeData(TEXT("レイヤーを削除する"));
	ieii.SetID( LW_CM_DELETE_LAYER );
	IEMenuMG::InsertIEMenuItem(hmenu, 1, TRUE, &ieii);

	ImgLayer_Ptr pLayer = std::dynamic_pointer_cast<ImgLayer>(m_Layer);
	//
	if (pLayer->IsLockPixelAlpha()) {
		ieii.SetTypeData(TEXT("不透明度を保護しない"));
		ieii.SetID( LW_CM_UNLOCK_PIXEL_ALPHA );
	} else {
		ieii.SetTypeData(TEXT("不透明度を保護する"));
		ieii.SetID( LW_CM_LOCK_PIXEL_ALPHA );
	}
	IEMenuMG::InsertIEMenuItem(hmenu, 2, TRUE, &ieii);

	//
	if (pLayer->IsClipWithUnderLayer()) {
		ieii.SetTypeData(TEXT("下のレイヤーでクリッピングしない"));
		ieii.SetID( LW_CM_UNCLIP_WITH_UNDER_LAYER );
	} else {
		ieii.SetTypeData(TEXT("下のレイヤーでクリッピングする"));
		ieii.SetID( LW_CM_CLIP_WITH_UNDER_LAYER );
	}
	IEMenuMG::InsertIEMenuItem(hmenu, 3, TRUE, &ieii);

	//
	IImgLayer_Ptr pUnderLayer = m_Layer->GetUnderLayer().lock();
	if ((pUnderLayer == NULL) || (pUnderLayer->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER)) {
		ieii.SetState( MFS_GRAYED );
	}
	ieii.SetTypeData(TEXT("下のレイヤーと結合"));
	ieii.SetID( LW_CM_MERGE_UNDER_LAYER );
	IEMenuMG::InsertIEMenuItem(hmenu, 4, TRUE, &ieii);
	ieii.SetState( MFS_ENABLED );

	//
	ieii.SetTypeData(TEXT("表示レイヤーを結合"));
	ieii.SetID( LW_CM_MERGE_DISPLAY_LAYER );
	IEMenuMG::InsertIEMenuItem(hmenu, 5, TRUE, &ieii);

	IEMenuMG::TrackPopupIEMenu(hmenu, TPM_LEFTALIGN|TPM_TOPALIGN, lpt->x, lpt->y, 0, hParent, NULL);
	IEMenuMG::DestroyIEMenu(hmenu);
}

void CLayerPanel::UpdateLayerThm()
{
	if(m_Layer && m_LayerThm){
		int x,y;
		int n,m;
		UCvPixel out_pixel;
		UCvPixel buf1_pixel;
		UCvPixel buf2_pixel;

		buf1_pixel.r = 255;
		buf1_pixel.g = 255;
		buf1_pixel.b = 255;
		buf1_pixel.a = 255;

		buf2_pixel.r = 100;
		buf2_pixel.g = 100;
		buf2_pixel.b = 100;
		buf2_pixel.a = 255;

		CvSize file_img_size = m_Layer->GetParentFile().lock()->GetImgSize();
		double x_scale = (double)file_img_size.width/m_LayerThm->width;
		double y_scale = (double)file_img_size.height/m_LayerThm->height;

		double src_x, src_y;
		src_y=0.0;
		for(y=0; y<m_LayerThm->height; y++){
			src_x=0.0;
			for(x=0; x<m_LayerThm->width; x++){
				src_x += x_scale;

				if((0 <= src_x && src_x < file_img_size.width) &&
					(0 <= src_y && src_y < file_img_size.height))
				{
					out_pixel.value = 0;
					m_Layer->GetPixel(src_x, src_y, &out_pixel);

					if(out_pixel.a != 255){
						n = y >> 3;
						m = x >> 3;
						if((n & 0x1) == (m & 0x1)){
							AlphaBlendPixel(&buf1_pixel, &out_pixel, 255, &out_pixel);
						}
						else{
							AlphaBlendPixel(&buf2_pixel, &out_pixel, 255, &out_pixel);
						}
					}
					SetPixelToBGR(m_LayerThm, x, y, &out_pixel);
				}
			}
			src_y += y_scale;
		}
	}
}

void CLayerPanel::SetPanelRect(const LPRECT lprc)
{
	m_LayerPanelRect = (*lprc);
	int orgx = m_LayerPanelRect.left;
	int orgy = m_LayerPanelRect.top;
	int panel_width = m_LayerPanelRect.right - m_LayerPanelRect.left;
	int panel_height = m_LayerPanelRect.bottom - m_LayerPanelRect.top;

	m_EyeRect.left = 10 + orgx;
	m_EyeRect.top = panel_height/2 - LP_EYE_HEIGHT/2 + orgy; //パネルY軸方向中央
	m_EyeRect.right = m_EyeRect.left + LP_EYE_WIDTH;
	m_EyeRect.bottom = m_EyeRect.top + LP_EYE_HEIGHT;

	m_LayerThmRect.top = panel_height/2 - LP_LAYER_THM_H/2 + orgy;
	m_LayerThmRect.left = 38 + orgx;
	m_LayerThmRect.right = m_LayerThmRect.left + m_LayerThm->width;
	m_LayerThmRect.bottom = m_LayerThmRect.top + m_LayerThm->height;

	m_LayerStyleTxtRect.top = panel_height/4 - LAYER_STYLE_FONT_SIZE/2 + orgy;
	m_LayerStyleTxtRect.left = 88 + orgx;
	m_LayerStyleTxtRect.bottom = m_LayerStyleTxtRect.top + LAYER_STYLE_FONT_SIZE;
	m_LayerStyleTxtRect.right = 320 + orgy;

	if(m_Layer){
		int name_size = m_Layer->GetNameSize();
		m_LayerNameTxtRect.top = 3*panel_height/4 - LAYER_NAME_FONT_SIZE/2 + orgy;
		m_LayerNameTxtRect.left = 80 + orgx;
		m_LayerNameTxtRect.bottom = m_LayerNameTxtRect.top + LAYER_NAME_FONT_SIZE;
		m_LayerNameTxtRect.right = m_LayerNameTxtRect.left + name_size * LAYER_NAME_FONT_WIDTH;
	}
}

void CLayerPanel::GetPanelRect(LPRECT lprc)
{
	(*lprc) = m_LayerPanelRect;
}

void CLayerPanel::DrawPanel(CDC* pDC)
{
	if(!m_Layer) return;

	int panel_width = m_LayerPanelRect.right - m_LayerPanelRect.left;
	int panel_height = m_LayerPanelRect.bottom - m_LayerPanelRect.top;
	
	DrawBackGround(pDC);

	//set font
	HFONT hOldFont = (HFONT)::SelectObject(pDC->GetSafeHdc(), m_LayerNameFont.GetSafeHandle());

	RECT text_rect;

	//レイヤースタイル、不透明度表示
	char output_txt[MAX_IELAYER_SYNTH_DISPLAY_NAME*2];
	char layer_style[MAX_IELAYER_SYNTH_DISPLAY_NAME];
	LayerSynthesizerMG::GetLayerSynthDisplayName(m_Layer->GetLayerSynthFourCC(), layer_style);
	wsprintf(output_txt, "[%s] %d%", layer_style, m_Layer->GetOpacityPer());
	pDC->DrawText(output_txt, strlen(output_txt), &m_LayerStyleTxtRect, DT_LEFT);

	::SelectObject(pDC->GetSafeHdc(), hOldFont);

	//select font
	CFont* oldFont = pDC->SelectObject( &m_LayerNameFont );

	//レイヤー名表示
	char layer_name[MAX_IMG_LAYER_NAME];
	m_Layer->GetName(layer_name);
	pDC->DrawText(layer_name, strlen(layer_name), &m_LayerNameTxtRect, DT_LEFT);

	//フォントを元に戻す
	pDC->SelectObject( oldFont );

	//可視・不可視アイコン表示
	CBitmap* cOldBmp = NULL;
	CDC cMemDC;
	cMemDC.CreateCompatibleDC(NULL);
	if (m_Layer->IsVisible()) {
		cOldBmp = cMemDC.SelectObject(&m_cEye);
	} else {
		cOldBmp = cMemDC.SelectObject(&m_cNEye);
	}
	pDC->BitBlt(
		m_EyeRect.left,
		m_EyeRect.top,
		m_EyeRect.right - m_EyeRect.left,
		m_EyeRect.bottom - m_EyeRect.top,
		&cMemDC,
		0,
		0,
		SRCCOPY);
	cMemDC.SelectObject(cOldBmp);

	//blt layer thmune
	if(m_LayerThm){
		UpdateLayerThm(); //
		IPLImageToDevice(
			pDC->GetSafeHdc(),
			m_LayerThmRect.left,
			m_LayerThmRect.top,
			m_LayerThm->width,
			m_LayerThm->height,
			m_LayerThm,
			0, 0);
	}
}

void CLayerPanel::OnLButtonDown(UINT nFlags, CPoint point)
{
	//目画像とのクリック判定
	if (isInRect(point.x, point.y, &m_EyeRect)) {
		bool bl = m_Layer->IsVisible();
		if (bl) bl = false;
		else   bl = true;
		m_Layer->SetVisible( bl );

		ImgFile_Ptr pFile = m_Layer->GetParentFile().lock();
		if (pFile) {
			//表示画像の再生成
			IImgLayer_Ptr _pLGroup = m_Layer->GetParentLayer().lock();
			ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(_pLGroup);
			LPUPDATE_DATA data = pLGroup->CreateUpdateData();
			data->isAll = true;
			pLGroup->PushUpdateData(data);

			LPUPDATE_DATA data2 = pFile->CreateUpdateData();
			data2->isAll = true;
			pFile->PushUpdateData(data2);
		}
		::InvalidateRect(m_hParentWnd, &m_EyeRect, FALSE);
	}
	return;
}

void CLayerPanel::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (isInRect(point.x, point.y, &m_LayerNameTxtRect)) {
		assert(m_pLayerNameEdit);
		if (m_pLayerNameEdit) {
			m_pLayerNameEdit->SetEditLayer( m_Layer );
			m_pLayerNameEdit->SetWindowPos(
				NULL,
				m_LayerNameTxtRect.left,
				m_LayerNameTxtRect.top,
				m_LayerNameTxtRect.right - m_LayerNameTxtRect.left,
				m_LayerNameTxtRect.bottom - m_LayerNameTxtRect.top,
				SWP_NOZORDER);

			//
			char layer_name[MAX_IMG_LAYER_NAME];
			m_Layer->GetName(layer_name);
			m_pLayerNameEdit->SetWindowText( layer_name );
			m_pLayerNameEdit->SetEditLayer( m_Layer );
			m_pLayerNameEdit->ShowWindow( SW_SHOW );
			m_pLayerNameEdit->SetActiveWindow();
			m_pLayerNameEdit->SetFocus();
			m_pLayerNameEdit->SetSel(0, -1); //all text select
			m_pLayerNameEdit->UpdateWindow();
		}

		//unhook
		CImgEdit_UIApp* pApp = (CImgEdit_UIApp*)AfxGetApp();
		pApp->UnhookKeyBoard();
	}
	else if (isInRect(point.x, point.y, &m_EyeRect)) {
		return;
	}
	else {
		OpenLayerStyleDialog(m_hParentWnd);
	}
	//
	::InvalidateRect(m_hParentWnd, &m_LayerPanelRect, TRUE);
}

void CLayerPanel::OnTimer(UINT_PTR uIDEvent)
{
	//switch( uIDEvent ){
	//	case ID_LP_THM_UPDATE_TIMER:
	//		m_nUpdateThmTimer = 0;
	//		UpdateLayerThm();
	//		Invalidate(FALSE);
	//		this->KillTimer(m_nUpdateThmTimer);
	//		break;
	//	case ID_LP_OFF_MOUSE_TIMER:
	//		if(isOnMouse == true){
	//			isOnMouse = false;
	//			Invalidate(FALSE);
	//		}
	//		break;
	//	default:
	//		assert(0);
	//		break;
	//}
}

void CLayerPanel::DrawBackGround(CDC* pDC)
{
	//draw back graund
	if (m_IsSelect) {
		pDC->FillSolidRect(&m_LayerPanelRect, RGB(100, 100, 200));
	}
	else {
		if (m_IsOnMouse) {
			pDC->FillSolidRect(&m_LayerPanelRect, RGB(120, 150, 200));
		}
		else {
			pDC->FillSolidRect(&m_LayerPanelRect, RGB(200, 200, 200));
		}
	}
}