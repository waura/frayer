#pragma once

#include "IIEConfig_DialogComponent.h"

#include <list>
#include <string>

enum EDIT_KEY_ID {
	E_ADD = 1,
	E_DELETE = 2,
};

typedef struct _EDIT_KEY{
	uint32_t key_code;
	std::string cmd_name;
	EDIT_KEY_ID id;
}EDIT_KEY;

typedef std::list<EDIT_KEY> EDIT_KEY_LIST;

class CShortCutKeyEdit : public CEdit
{
public:
	CShortCutKeyEdit(){};
	~CShortCutKeyEdit(){};

	void SetIEKeyMap(IEKeyMap* pKeyMap){
		m_pIEKeyMap = pKeyMap;
	}
	void SetEditShortCutList(EDIT_KEY_LIST* plist){
		m_copyEditShortCut_List = plist;
	}

	uint32_t key_code;
protected:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_MESSAGE_MAP();

private:
	bool FindEditKeyList(uint32_t key_code, char* cmd_name);
	uint32_t GetKeyCodeEditKeyList(const char* cmd_name);

	EDIT_KEY_LIST* m_pEditShortCutList;
	IEKeyMap* m_pIEKeyMap;
	EDIT_KEY_LIST* m_copyEditShortCut_List;
};

class IEConfig_Shortcut : public IIEConfig_DialogComponent
{
public:
	IEConfig_Shortcut();
	~IEConfig_Shortcut();

	void InitDialog(HWND hDialog, HINSTANCE hInst, IE_CONFIG_DIALOG_DATA *pConfigData);
	void Submit();
	BOOL OnMessage(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

private:
	void InsertKeyEdit(EDIT_KEY& edit_key);

	IE_CONFIG_DIALOG_DATA *m_pConfigData;

	LV_HITTESTINFO m_selectItem;
	EDIT_KEY_LIST m_editShortCut_List;
	CListCtrl m_listCtrl;
	CShortCutKeyEdit m_KeyEdit;
};