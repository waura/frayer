#ifndef _IEMENUMG_H_
#define _IEMENUMG_H_

#include "IEMenuItemInfo.h"

class _EXPORTCPP IEMenuMG
{
public:
	////////////////////////////////////////////
	/*!
		メニューを作成する。
		@return メニューハンドル
	*/
	static IEMENU CreateIEMenu();

	//////////////////////////////////////////////
	/*!
	
	*/
	static IEMENU CreateIEPopupMenu();

	/////////////////////////////////////////////
	/*!
		メニューを破棄する。
		@param[in] ieMenu 破棄するメニュー
	*/
	static void DestroyIEMenu(IEMENU ieMenu);

	////////////////////////////////////////////
	/*!
		メニューにアイテムを追加する。
		@param[in] ieMenu メニューハンドル
		@param[in] uItem メニューインデックスまたはID
		@param[in] fByPosition FALSE なら uItem は ID、それ以外ならインデックスであると判断します
		@param[in] lpmii 追加するメニューアイテム
	*/
	static BOOL InsertIEMenuItem(IEMENU ieMenu, UINT uItem,
						BOOL fByPosition,  IEMenuItemInfo *lpmii);

	/////////////////////////////////////////////
	/*!
		ポップアップメニューを表示させる

	*/
	static BOOL TrackPopupIEMenu(IEMENU ieMenu, UINT uFlags, int x, int y,
		int nReserved, HWND hWnd, CONST RECT *prcRect);

	static BOOL TrackPopupIEMenuEx(IEMENU ieMenu, UINT uFlags, int x, int y,
		HWND hWnd, LPTPMPARAMS ptpm);
};

#endif