#include "IImgSelecter.h"

void IImgSelecter::OnSelect()
{
	ImgFile_Ptr file = m_pImgEdit->GetActiveFile();
	if(file){
		file->SetMaskDrawType( MASK_SOLID_DRAW );

		LPUPDATE_DATA data = file->CreateUpdateData();
		data->isAll = false;
		file->GetMaskRect(&(data->rect));
		file->PushUpdateData(data);
	}
}

void IImgSelecter::OnNeutral()
{
	ImgFile_Ptr file = m_pImgEdit->GetActiveFile();
	if(file){
		file->SetMaskDrawType( MASK_DOT_LINE_DRAW );

		LPUPDATE_DATA data = file->CreateUpdateData();
		data->isAll = false;
		file->GetMaskRect(&(data->rect));
		file->PushUpdateData(data);
	}
}

void IImgSelecter::OnMouseRButtonUp(UINT nFlags, const LPPOINT lpt)
{
	ImgFile_Ptr f = m_pImgEdit->GetActiveFile();
	if(f){
		//HWND hWnd = f->GetHWND();
		//if(f->isOnSelectImage(lpt)){//選択領域の上ならコンテキストメニューを出す。
		//	//クライアント座標からスクリーン座標へ
		//	POINT spt = *lpt;
		//	::ClientToScreen(hWnd, &spt);
		
		POINT spt;
		::GetCursorPos(&spt);

		//メニューの作成・表示
		IEMENU hmenu = IEMenuMG::CreateIEPopupMenu();

		IEMenuItemInfo ieii;
		ieii.SetMask(MIIM_STRING | MIIM_ID | MIIM_STATE );
		ieii.SetState( MFS_ENABLED );

		ieii.SetTypeData(TEXT("選択領域をカットしたレイヤ"));
		ieii.SetID( m_pImgEdit->GetCommandID(TEXT("CutLayerSelectImage")) );
		IEMenuMG::InsertIEMenuItem(hmenu, 0, TRUE, &ieii);

		ieii.SetTypeData(TEXT("選択領域をコピーしたレイヤ"));
		ieii.SetID( m_pImgEdit->GetCommandID(TEXT("CopyLayerSelectImage")) );
		IEMenuMG::InsertIEMenuItem(hmenu, 1, TRUE, &ieii);

		ieii.SetTypeData(TEXT("すべてを選択"));
		ieii.SetID( m_pImgEdit->GetCommandID(TEXT("ImgSelectAll")) );
		IEMenuMG::InsertIEMenuItem(hmenu, 3, TRUE, &ieii);

		ieii.SetTypeData(TEXT("選択を解除"));
		ieii.SetID( m_pImgEdit->GetCommandID(TEXT("ImgSelectRelease")) );
		IEMenuMG::InsertIEMenuItem(hmenu, 4, TRUE, &ieii);

		ieii.SetTypeData(TEXT("選択範囲を反転"));
		ieii.SetID( m_pImgEdit->GetCommandID(TEXT("ImgSelectFlip")) );
		IEMenuMG::InsertIEMenuItem(hmenu, 5, TRUE, &ieii);

		ieii.SetTypeData(TEXT("塗りつぶし"));
		ieii.SetID( m_pImgEdit->GetCommandID(TEXT("ColorFill")) );
		IEMenuMG::InsertIEMenuItem(hmenu, 7, TRUE, &ieii);

		ieii.SetTypeData(TEXT("自由変形"));
		ieii.SetID( m_pImgEdit->GetCommandID(TEXT("FreeDeformation")) );
		IEMenuMG::InsertIEMenuItem(hmenu, 8, TRUE, &ieii);

		ieii.SetMask(MIIM_FTYPE);
		ieii.SetType(MFT_SEPARATOR);
		IEMenuMG::InsertIEMenuItem(hmenu, 2, TRUE, &ieii);
		IEMenuMG::InsertIEMenuItem(hmenu, 6, TRUE, &ieii);

		HWND hwnd = ::GetForegroundWindow();
		IEMenuMG::TrackPopupIEMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN,
			spt.x, spt.y, 0, hwnd, NULL);
		IEMenuMG::DestroyIEMenu(hmenu);
	}
}