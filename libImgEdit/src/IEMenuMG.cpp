#include "stdafx.h"

#include "IEMenuMG.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


////////////////////////////////////////////
/*!
	メニューを作成する。
	@return メニューハンドル
*/
IEMENU IEMenuMG::CreateIEMenu()
{
	return CreateMenu();
}

IEMENU IEMenuMG::CreateIEPopupMenu()
{
	return CreatePopupMenu();
}

/////////////////////////////////////////////
/*!
	メニューを破棄する。
	@param[in] ieMenu 破棄するメニュー
*/
void IEMenuMG::DestroyIEMenu(IEMENU ieMenu)
{
	DestroyMenu(ieMenu);
}

////////////////////////////////////////////
/*!
	メニューにアイテムを追加する。
	@param[in] ieMenu メニューハンドル
	@param[in] uItem メニューインデックスまたはID
	@param[in] fByPosition FALSE なら uItem は ID、それ以外ならインデックスであると判断します
	@param[in] lpmii 追加するメニューアイテム
*/
BOOL IEMenuMG::InsertIEMenuItem(IEMENU ieMenu, UINT uItem, BOOL fByPosition, IEMenuItemInfo *lpmii)
{
	return InsertMenuItem(ieMenu, uItem, fByPosition, lpmii->GetIEMenuItemInfo());
}

/////////////////////////////////////////////
/*!
	ポップアップメニューを表示させる

*/
BOOL IEMenuMG::TrackPopupIEMenu(IEMENU ieMenu, UINT uFlags, int x, int y,
								int nReserved, HWND hWnd, CONST RECT *prcRect)
{
	return TrackPopupMenu(ieMenu, uFlags, x, y, nReserved, hWnd, prcRect);
}