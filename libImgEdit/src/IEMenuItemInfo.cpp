#include "stdafx.h"

#include "IEMenuItemInfo.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


IEMenuItemInfo::IEMenuItemInfo()
{
	memset(&mii, 0, sizeof(MENUITEMINFO)); 
	mii.cbSize = sizeof(MENUITEMINFO);
}

IEMenuItemInfo::~IEMenuItemInfo()
{
}

void IEMenuItemInfo::SetMask(UINT fMask)
{
	mii.fMask = fMask;
}

void IEMenuItemInfo::SetID(UINT wID)
{
	mii.wID = wID;
}

void IEMenuItemInfo::SetType(UINT fType)
{
	mii.fType = fType;
}

void IEMenuItemInfo::SetState(UINT fState)
{
	mii.fState = fState;
}

void IEMenuItemInfo::SetSubMenu(IEMENU ieMenu)
{
	mii.hSubMenu = ieMenu;
}

void IEMenuItemInfo::SetTypeData(LPTSTR dwTypeData)
{
	mii.dwTypeData = dwTypeData;
}

void IEMenuItemInfo::SetCch(UINT cch)
{
	mii.cch = cch;
}

LPIEMENUITEMINFO IEMenuItemInfo::GetIEMenuItemInfo()
{
	return &mii;
}