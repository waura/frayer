#include "stdafx.h"

#include "../resource1.h"
#include "IEConfig_Shortcut.h"

#include <ctype.h>

#define IEIR_LIST_CTRL_ID 1
#define IEIR_KEY_EDIT_ID 2

#define IEIR_LIST_CTRL_X 180
#define IEIR_LIST_CTRL_Y 10
#define IEIR_LIST_CTRL_W 340
#define IEIR_LIST_CTRL_H 400

#define SHORTCUT_STR_LEN 256

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CShortCutKeyEdit, CEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

void CShortCutKeyEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}

void CShortCutKeyEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == IE_KEY_BACK ||
		nChar == IE_KEY_DELETE ||
		nChar == IE_KEY_TAB ||
		nChar == IE_KEY_CONTROL){
			this->SetWindowText("");
			return;
	}

	uint32_t tmp_key_code;
	char key_name[64];
	char str[SHORTCUT_STR_LEN];

	if(nChar == IE_KEY_SPACE){
		wsprintf(key_name, "Space");
	}
	else{
		int c = tolower(nChar);
		wsprintf(key_name, "%c", c);
	}

	if((::GetKeyState(VK_SHIFT) < 0) && (::GetKeyState(VK_CONTROL) < 0)){
		wsprintf(str, "Ctrl+Shift+%s", key_name);
		tmp_key_code = tolower(nChar);
		tmp_key_code |= IE_EXT_KEY_CONTROL;
		tmp_key_code |= IE_EXT_KEY_SHIFT;
	}
	else if(::GetKeyState(VK_SHIFT) < 0){
		wsprintf(str, "Shift+%s", key_name);
		tmp_key_code = tolower(nChar);
		tmp_key_code |= IE_EXT_KEY_SHIFT;
	}
	else if(::GetKeyState(VK_CONTROL) < 0){
		wsprintf(str, "Ctrl+%s", key_name);
		tmp_key_code = tolower(nChar);
		tmp_key_code |= IE_EXT_KEY_CONTROL;
	}
	else{
		wsprintf(str, "%s", key_name);
		tmp_key_code = tolower(nChar);
	}

	char cmd[MAX_IE_MODULE_NAME];
	char tool_name[MAX_IE_MODULE_NAME];
	if(FindEditKeyList(tmp_key_code, cmd)){
		char display_name[MAX_IE_MODULE_DISPLAY_NAME];
		if(!IECommandMG::GetDisplayName(cmd, display_name)){
			return;
		}

		char buf[MAX_IE_MODULE_DISPLAY_NAME + 64];
		wsprintf(buf, "ショートカット %s と重複しています。", display_name);
		OutputError::Alert(buf);
		return;
	}
	else{
		if(m_pIEKeyMap->GetShortCutCmd(tmp_key_code, cmd)){
			if(GetKeyCodeEditKeyList(cmd) == tmp_key_code){
				this->key_code = -1;
				char display_name[MAX_IE_MODULE_DISPLAY_NAME];
				if(!IECommandMG::GetDisplayName(cmd, display_name)){
					return;
				}

				char buf[MAX_IE_MODULE_DISPLAY_NAME + 64];
				wsprintf(buf, "ショートカット %s と重複しています。", display_name);
				OutputError::Alert(buf);
				return;
			}
		}
		else if(m_pIEKeyMap->GetToolShiftName(tmp_key_code, tool_name)){
			this->key_code = -1;
			char display_name[MAX_IE_MODULE_DISPLAY_NAME];
			if(!g_ImgEdit.GetToolDisplayName(tool_name, display_name)){
				return;
			}

			char buf[MAX_IE_MODULE_DISPLAY_NAME + 64];
			wsprintf(buf, "ツールシフト %s と重複しています。", display_name);
			OutputError::Alert(buf);
			return;
		}
	}

	this->key_code = tmp_key_code;
	this->SetWindowText(str);
}

bool CShortCutKeyEdit::FindEditKeyList(uint32_t key_code, char* cmd_name)
{
	EDIT_KEY_LIST::reverse_iterator itr = m_copyEditShortCut_List->rbegin();
	for(; itr != m_copyEditShortCut_List->rend(); itr++){
		if((*itr).key_code == key_code){
			strcpy(cmd_name, (*itr).cmd_name.c_str());
			return true;
		}
	}
	return false;
}

uint32_t CShortCutKeyEdit::GetKeyCodeEditKeyList(const char* cmd_name)
{
	EDIT_KEY_LIST::reverse_iterator itr = m_copyEditShortCut_List->rbegin();
	for(; itr != m_copyEditShortCut_List->rend(); itr++){
		if(strcmp((*itr).cmd_name.c_str(), cmd_name) == 0){
			return (*itr).key_code;
		}
	}
	return -1;
}

IEConfig_Shortcut::IEConfig_Shortcut()
{
}

IEConfig_Shortcut::~IEConfig_Shortcut()
{
}

void IEConfig_Shortcut::InitDialog(HWND hDialog, HINSTANCE hInst, IE_CONFIG_DIALOG_DATA *pConfigData)
{
	m_pConfigData = pConfigData;
	IEKeyMap* pKeyMap = pConfigData->pImgEdit->GetKeyMap();
	CWnd* pParent = CWnd::FromHandle( hDialog);

	RECT rc;
	rc.top = IEIR_LIST_CTRL_Y;
	rc.left = IEIR_LIST_CTRL_X;
	rc.bottom = rc.top + IEIR_LIST_CTRL_H;
	rc.right = rc.left + IEIR_LIST_CTRL_W;
	m_listCtrl.Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL | LVS_NOSORTHEADER,
		rc,
		pParent,
		IEIR_LIST_CTRL_ID);
	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	rc.top = 0;
	rc.left = IEIR_LIST_CTRL_X;
	rc.bottom = 10;
	rc.right = rc.left + IEIR_LIST_CTRL_W;
	m_KeyEdit.Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
		rc,
		&m_listCtrl,
		IEIR_KEY_EDIT_ID);
	m_KeyEdit.ShowWindow(SW_HIDE);
	m_KeyEdit.SetIEKeyMap( pKeyMap );
	m_KeyEdit.SetEditShortCutList( &m_editShortCut_List );

	LV_COLUMN listcol;
	char cmd[MAX_IE_MODULE_NAME];
	char buf[MAX_IE_MODULE_DISPLAY_NAME];

	//insert column
	wsprintf(buf, "コマンド名");
	listcol.mask = LVCF_TEXT | LVCF_WIDTH;
	listcol.pszText = buf;
	listcol.cx = 200;
	m_listCtrl.InsertColumn(1, &listcol);

	wsprintf(buf, "キー");
	listcol.cx = 100;
	m_listCtrl.InsertColumn(2, &listcol);

	//inser list data
	LV_ITEM lvitem;
	int i;
	int min = IECommandMG::GetMinID();
	int max = IECommandMG::GetMaxID();
	uint32_t key_code;
	for(i=min; i<=max; i++){
		//insert row data
		if(IECommandMG::GetDisplayName(i, buf) && IECommandMG::GetCommandName(i, cmd)){
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = i - min;
			lvitem.iSubItem = 0;
			lvitem.pszText = buf;
			m_listCtrl.InsertItem(&lvitem);

			//insert col data
			char str[SHORTCUT_STR_LEN];
			key_code = pKeyMap->GetShortCutKey(cmd);
			if(key_code != -1){
				UINT nChar = key_code & IE_KEY_CODE_MASK;
				if((key_code & IE_EXT_KEY_CONTROL) && (key_code & IE_EXT_KEY_SHIFT)){
					wsprintf(str, "Ctrl+Shift+%c", nChar);
				}
				else if(key_code & IE_EXT_KEY_CONTROL){
					wsprintf(str, "Ctrl+%c", nChar);
				}
				else if(key_code & IE_EXT_KEY_SHIFT){
					wsprintf(str, "Shift+%c", nChar);
				}
				else{
					wsprintf(str, "%c", nChar);
				}
				lvitem.pszText = str;
			}
			else{
				lvitem.pszText = "";		
			}
			lvitem.iSubItem = 1;
			m_listCtrl.SetItem(&lvitem);
		}
	}
}

void IEConfig_Shortcut::Submit()
{
	EDIT_KEY_LIST::iterator itr = m_editShortCut_List.begin();
	for(; itr != m_editShortCut_List.end(); itr++){
		switch( (*itr).id ){
			case EDIT_KEY_ID::E_ADD:
				{
					IEKeyMap* pIEKeyMap = m_pConfigData->pImgEdit->GetKeyMap();
					if(pIEKeyMap){
						pIEKeyMap->DeleteShortCut( (*itr).cmd_name.c_str() );
						pIEKeyMap->AddShortCut( (*itr).key_code, (*itr).cmd_name.c_str() );
					}
				}
				break;
			case EDIT_KEY_ID::E_DELETE:
				{
					IEKeyMap* pIEKeyMap = m_pConfigData->pImgEdit->GetKeyMap();
					if(pIEKeyMap){
						pIEKeyMap->DeleteShortCut( (*itr).cmd_name.c_str() );
					}
				}
				break;
		}
	}
}

BOOL IEConfig_Shortcut::OnMessage(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg){
		case WM_COMMAND:
			switch(LOWORD(wp)){
				case ID_IEOK:
					if(m_KeyEdit.IsWindowVisible()){
						char cmd[MAX_IE_MODULE_NAME];
						int id = IECommandMG::GetMinID() + m_selectItem.iItem;
						IECommandMG::GetCommandName(id, cmd);

						EDIT_KEY es;
						es.key_code = m_KeyEdit.key_code;
						es.cmd_name = cmd;
						if(es.key_code != -1){
							es.id = EDIT_KEY_ID::E_ADD;
							InsertKeyEdit( es );
	
							//change item text
							char str[SHORTCUT_STR_LEN];
							m_KeyEdit.GetWindowText(str, SHORTCUT_STR_LEN);
							m_listCtrl.SetItemText(
								m_selectItem.iItem,
								m_selectItem.iSubItem,
								str);
						}
						else{
							es.id = EDIT_KEY_ID::E_DELETE;
							InsertKeyEdit( es );

							//change item text
							m_listCtrl.SetItemText(
								m_selectItem.iItem,
								m_selectItem.iSubItem,
								"");
						}

						//
						m_KeyEdit.ShowWindow(SW_HIDE);
						return TRUE;
					}
					break;
			}
			break;
		case WM_NOTIFY:
			if(LOWORD(wp) == IEIR_LIST_CTRL_ID){
				LV_DISPINFO *lvinfo = (LV_DISPINFO*)lp;
				switch(lvinfo->hdr.code){
					case NM_DBLCLK:
						{
							CRect rc;							
							POINT pt;
							::GetCursorPos(&pt);
							m_listCtrl.ScreenToClient(&pt);
							
							LV_HITTESTINFO hittest;
							hittest.pt.x = pt.x;
							hittest.pt.y = pt.y;
							if(m_listCtrl.SubItemHitTest(&hittest) != -1){
								if(hittest.iSubItem != 0){
									m_selectItem = hittest;

									m_listCtrl.GetSubItemRect(hittest.iItem, hittest.iSubItem, LVIR_LABEL, rc);
									m_KeyEdit.SetWindowText("");
									m_KeyEdit.MoveWindow(
										rc.left,
										rc.top,
										rc.right - rc.left,
										rc.bottom - rc.top);
									m_KeyEdit.ShowWindow(SW_SHOW);
									m_KeyEdit.SetActiveWindow();
									m_KeyEdit.key_code = -1;
								}
							}
						}
						return TRUE;
					case LVN_COLUMNCLICK:
						m_KeyEdit.ShowWindow(SW_HIDE);
						return TRUE;
					case LVN_ITEMCHANGING:
						m_KeyEdit.ShowWindow(SW_HIDE);
						return TRUE;
					case LVN_BEGINLABELEDIT:
						{
							int i=0;
						}
						break;
					case LVN_ENDLABELEDIT:
						{
							int i=0;
						}
						break;
				}
			}

			if(LOWORD(wp) == IEIR_KEY_EDIT_ID){
				switch(HIWORD(wp)){
					case EN_CHANGE:
						return TRUE;
					case EN_SETFOCUS:
						{
							int i=0;
						}
						return TRUE;
					case EN_KILLFOCUS:
						m_KeyEdit.ShowWindow(SW_HIDE);
						return TRUE;
				}
			}
			break;
	}

	return FALSE;
}

void IEConfig_Shortcut::InsertKeyEdit(EDIT_KEY &edit_key)
{
	EDIT_KEY_LIST::iterator itr = m_editShortCut_List.begin();
	for(; itr != m_editShortCut_List.end(); itr++){
		if((*itr).cmd_name == edit_key.cmd_name){
			(*itr) = edit_key;
			return;
		}
	}

	m_editShortCut_List.push_back(edit_key);

}