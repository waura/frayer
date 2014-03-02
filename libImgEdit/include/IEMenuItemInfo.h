#ifndef _IEMENUITEMINFO_H_
#define _IEMENUITEMINFO_H_

#include "LibIEDllHeader.h"

typedef HMENU IEMENU;
typedef LPMENUITEMINFO LPIEMENUITEMINFO;

class _EXPORTCPP IEMenuItemInfo
{
public:
	IEMenuItemInfo();
	virtual ~IEMenuItemInfo();

	void SetMask(UINT fMask);
	void SetID(UINT wID);
	void SetType(UINT fType);
	void SetState(UINT fState);
	void SetSubMenu(IEMENU ieMenu);
	void SetTypeData(LPTSTR dwTypeData);
	void SetCch(UINT cch);

	LPIEMENUITEMINFO GetIEMenuItemInfo();
private:
	MENUITEMINFO mii;
};

#endif //_IEMENUITEMINFO_H_
