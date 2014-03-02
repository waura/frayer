#pragma once

#include "IIEConfig_DialogComponent.h"
#include "IEConfig_Shortcut.h"


class CToolShiftKeyEdit : public CEdit
{
public:
	CToolShiftKeyEdit(){};
	~CToolShiftKeyEdit(){};

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
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_MESSAGE_MAP();

private:
	bool FindEditKeyList(uint32_t key_code, char* cmd_name);
	uint32_t GetKeyCodeEditKeyList(const char* cmd_name);

	EDIT_KEY_LIST* m_pEditShortCutList;
	IEKeyMap* m_pIEKeyMap;
	EDIT_KEY_LIST* m_copyEditShortCut_List;
};


class IEConfig_ToolShift : public IIEConfig_DialogComponent
{
public:
	IEConfig_ToolShift();
	~IEConfig_ToolShift();

	void InitDialog(HWND hDialog, HINSTANCE hInst, IE_CONFIG_DIALOG_DATA *pConfigData);
	void Submit();
	BOOL OnMessage(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

private:
	void InsertKeyEdit(EDIT_KEY& edit_key);

	IE_CONFIG_DIALOG_DATA *m_pConfigData;

	LV_HITTESTINFO m_selectItem;
	EDIT_KEY_LIST m_editToolShift_List;

	CListCtrl m_listCtrl;
	CToolShiftKeyEdit m_KeyEdit;
	CEdit m_EditToolShiftTime;
	CStatic m_ToolShiftTimeLabel;
};