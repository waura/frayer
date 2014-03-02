#include "stdafx.h"

#include "../resource1.h"
#include "IEConfig_ToolShift.h"

#define IEIR_LIST_CTRL_ID 1
#define IEIR_KEY_EDIT_ID 2
#define IEIR_EDIT_TSTIME_ID 3
#define IEIR_LABEL_TSTIME_ID 4

#define IEIR_LABEL_TSTIME_X 180
#define IEIR_LABEL_TSTIME_Y 10
#define IEIR_LABEL_TSTIME_W 100
#define IEIR_LABEL_TSTIME_H 17

#define IEIR_EDIT_TSTIME_X 285
#define IEIR_EDIT_TSTIME_Y 8
#define IEIR_EDIT_TSTIME_W 60
#define IEIR_EDIT_TSTIME_H 17

#define IEIR_LIST_CTRL_X 180
#define IEIR_LIST_CTRL_Y 32
#define IEIR_LIST_CTRL_W 340
#define IEIR_LIST_CTRL_H 378

#define SHORTCUT_STR_LEN 256

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CToolShiftKeyEdit, CEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
END_MESSAGE_MAP()

void CToolShiftKeyEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}

void CToolShiftKeyEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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
	if(FindEditKeyList(tmp_key_code, tool_name)){
		char display_name[MAX_IE_MODULE_DISPLAY_NAME];
		if(!g_ImgEdit.GetToolDisplayName(tool_name, display_name)){
			return;
		}

		char buf[MAX_IE_MODULE_DISPLAY_NAME + 64];
		wsprintf(buf, "ツールシフト %s と重複しています。", display_name);
		OutputError::Alert(buf);
		return;
	}
	else{
		if(m_pIEKeyMap->GetShortCutCmd(tmp_key_code, cmd)){
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
		else if(m_pIEKeyMap->GetToolShiftName(tmp_key_code, tool_name)){
			if(GetKeyCodeEditKeyList(tool_name) == tmp_key_code){
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
	}

	this->key_code = tmp_key_code;
	this->SetWindowText(str);
}

void CToolShiftKeyEdit::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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
	else if(nChar == IE_KEY_MENU){
		wsprintf(key_name, "Alt");
	}
	else{
		int c = tolower(nChar);
		wsprintf(key_name, "%c", c);
	}

	if(nChar == IE_KEY_MENU){
		wsprintf(str, "%s", key_name);
		tmp_key_code = tolower(nChar);
	}
	else if((::GetKeyState(VK_SHIFT) < 0) && (::GetKeyState(VK_CONTROL) < 0)){
		wsprintf(str, "Alt+Ctrl+Shift+%s", key_name);
		tmp_key_code = tolower(nChar);
		tmp_key_code |= IE_EXT_KEY_CONTROL;
		tmp_key_code |= IE_EXT_KEY_SHIFT;
	}
	else if(::GetKeyState(VK_SHIFT) < 0){
		wsprintf(str, "Alt+Shift+%s", key_name);
		tmp_key_code = tolower(nChar);
		tmp_key_code |= IE_EXT_KEY_SHIFT;
	}
	else if(::GetKeyState(VK_CONTROL) < 0){
		wsprintf(str, "Alt+Ctrl+%s", key_name);
		tmp_key_code = tolower(nChar);
		tmp_key_code |= IE_EXT_KEY_CONTROL;
	}
	else{
		wsprintf(str, "Alt+%s", key_name);
		tmp_key_code = tolower(nChar);
	}
	tmp_key_code |= IE_EXT_KEY_ALT;

	char cmd[MAX_IE_MODULE_NAME];
	char tool_name[MAX_IE_MODULE_NAME];
	if(FindEditKeyList(tmp_key_code, tool_name)){
		char display_name[MAX_IE_MODULE_DISPLAY_NAME];
		if(!g_ImgEdit.GetToolDisplayName(tool_name, display_name)){
			return;
		}

		char buf[MAX_IE_MODULE_DISPLAY_NAME + 64];
		wsprintf(buf, "ツールシフト %s と重複しています。", display_name);
		OutputError::Alert(buf);
		return;
	}
	else{
		if(m_pIEKeyMap->GetShortCutCmd(tmp_key_code, cmd)){
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
		else if(m_pIEKeyMap->GetToolShiftName(tmp_key_code, tool_name)){
			if(GetKeyCodeEditKeyList(tool_name) == tmp_key_code){
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
	}

	this->key_code = tmp_key_code;
	this->SetWindowText(str);
}

bool CToolShiftKeyEdit::FindEditKeyList(uint32_t key_code, char* cmd_name)
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

uint32_t CToolShiftKeyEdit::GetKeyCodeEditKeyList(const char* cmd_name)
{
	EDIT_KEY_LIST::reverse_iterator itr = m_copyEditShortCut_List->rbegin();
	for(; itr != m_copyEditShortCut_List->rend(); itr++){
		if(strcmp((*itr).cmd_name.c_str(), cmd_name) == 0){
			return (*itr).key_code;
		}
	}
	return -1;
}



IEConfig_ToolShift::IEConfig_ToolShift()
{
}

IEConfig_ToolShift::~IEConfig_ToolShift()
{
}

void IEConfig_ToolShift::InitDialog(HWND hDialog, HINSTANCE hInst, IE_CONFIG_DIALOG_DATA *pConfigData)
{
	m_pConfigData = pConfigData;
	IEKeyMap* pKeyMap = pConfigData->pImgEdit->GetKeyMap();
	CWnd* pParent = CWnd::FromHandle( hDialog);

	RECT rc;

	//create toolshift time label
	rc.top = IEIR_LABEL_TSTIME_Y;
	rc.left = IEIR_LABEL_TSTIME_X;
	rc.bottom = rc.top + IEIR_LABEL_TSTIME_H;
	rc.right = rc.left + IEIR_LABEL_TSTIME_W;
	m_ToolShiftTimeLabel.Create(
		"ツールシフトタイム",
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		rc,
		pParent,
		IEIR_LABEL_TSTIME_ID);
	m_ToolShiftTimeLabel.SetFont(m_pConfigData->pDialogFont, FALSE);

	//create edit box for toolshift time
	rc.top = IEIR_EDIT_TSTIME_Y;
	rc.left = IEIR_EDIT_TSTIME_X;
	rc.bottom = rc.top + IEIR_EDIT_TSTIME_H;
	rc.right = rc.left + IEIR_EDIT_TSTIME_W;
	m_EditToolShiftTime.Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_RIGHT,
		rc,
		pParent,
		IEIR_EDIT_TSTIME_ID);
	char buf[256];
	int time = m_pConfigData->pImgEdit->GetIEConfig()->GetToolShiftTime();
	wsprintf(buf, "%d", time);
	m_EditToolShiftTime.SetWindowText(buf);
	m_EditToolShiftTime.SetFont(m_pConfigData->pDialogFont, FALSE);


	//create toolshift list
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
	m_KeyEdit.SetEditShortCutList( &m_editToolShift_List );

	LV_COLUMN listcol;
	char name[MAX_IE_MODULE_NAME];
	char disp_name[MAX_IE_MODULE_DISPLAY_NAME];

	//insert column
	wsprintf(name, "コマンド名");
	listcol.mask = LVCF_TEXT | LVCF_WIDTH;
	listcol.pszText = name;
	listcol.cx = 200;
	m_listCtrl.InsertColumn(1, &listcol);

	wsprintf(name, "キー");
	listcol.cx = 100;
	m_listCtrl.InsertColumn(2, &listcol);

	//inser list data
	LV_ITEM lvitem;
	int i,j;
	int group_size = g_ImgEdit.GetSizeOfToolGroup();
	uint32_t key_code;
	int count = 0;
	for(i=0; i<group_size; i++){
		IEToolGroup* pTGroup = g_ImgEdit.GetToolGroupAtIndex( i );
		int tool_node_size = pTGroup->GetToolSize();
		for(j=0; j<tool_node_size; j++){
		
			//insert row data
			if(pTGroup->GetDisplayName(j, disp_name) && pTGroup->GetToolName(j, name))
			{
				lvitem.mask = LVIF_TEXT;
				lvitem.iItem = count;
				lvitem.iSubItem = 0;
				lvitem.pszText = disp_name;
				m_listCtrl.InsertItem(&lvitem);

				//insert col data
				char str[SHORTCUT_STR_LEN];
				char s_ctrl[256];
				char s_shift[256];
				char s_alt[256];
				wsprintf(s_ctrl, "");
				wsprintf(s_shift, "");
				wsprintf(s_alt, "");

				key_code = pKeyMap->GetToolShiftKey(name);
				if(key_code != -1){
					UINT nChar = key_code & IE_KEY_CODE_MASK;
					if( (key_code & IE_EXT_KEY_CONTROL) ){
						wsprintf(s_ctrl, "Ctrl+");
					}
					if( (key_code & IE_EXT_KEY_SHIFT) ){
						wsprintf(s_shift, "Shift"); 
					}
					if( (key_code & IE_EXT_KEY_ALT) &&
						(nChar != IE_KEY_MENU) )
					{
						wsprintf(s_alt, "Alt+");
					}

					if(nChar == IE_KEY_SPACE){
						wsprintf(str, "%s%s%sSpace", s_ctrl, s_shift, s_alt);
					}
					else if(nChar == IE_KEY_MENU){
						wsprintf(str, "%s%s%sAlt", s_ctrl, s_shift, s_alt);
					}
					else{
						wsprintf(str, "%s%s%s%c", s_ctrl, s_shift, s_alt, nChar);
					}
					lvitem.pszText = str;
				}
				else{
					lvitem.pszText = "";		
				}
				lvitem.iSubItem = 1;
				m_listCtrl.SetItem(&lvitem);
			}

			count++;
		}
	}
}

void IEConfig_ToolShift::Submit()
{
	EDIT_KEY_LIST::iterator itr = m_editToolShift_List.begin();
	for(; itr != m_editToolShift_List.end(); itr++){
		switch( (*itr).id ){
			case EDIT_KEY_ID::E_ADD:
				{
					IEKeyMap* pIEKeyMap = m_pConfigData->pImgEdit->GetKeyMap();
					if(pIEKeyMap){
						pIEKeyMap->DeleteToolShift( (*itr).cmd_name.c_str() );
						pIEKeyMap->AddToolShift( (*itr).key_code, (*itr).cmd_name.c_str() );
					}
				}
				break;
			case EDIT_KEY_ID::E_DELETE:
				{
					IEKeyMap* pIEKeyMap = m_pConfigData->pImgEdit->GetKeyMap();
					if(pIEKeyMap){
						pIEKeyMap->DeleteToolShift( (*itr).cmd_name.c_str() );
					}
				}
				break;
		}
	}

	char buf[256];
	m_EditToolShiftTime.GetWindowText(buf, 256);
	m_pConfigData->pImgEdit->GetIEConfig()->SetToolShiftTime( atoi(buf) );
}

BOOL IEConfig_ToolShift::OnMessage(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg){
		case WM_COMMAND:
			switch(LOWORD(wp)){
				case ID_IEOK:
					if(m_KeyEdit.IsWindowVisible()){
						char disp_name[MAX_IE_MODULE_DISPLAY_NAME];
						char tool_name[MAX_IE_MODULE_NAME];
						m_listCtrl.GetItemText(m_selectItem.iItem, 0, disp_name, MAX_IE_MODULE_NAME);
						g_ImgEdit.GetToolName(disp_name, tool_name);

						EDIT_KEY es;
						es.key_code = m_KeyEdit.key_code;
						es.cmd_name = tool_name;
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

void IEConfig_ToolShift::InsertKeyEdit(EDIT_KEY &edit_key)
{
	EDIT_KEY_LIST::iterator itr = m_editToolShift_List.begin();
	for(; itr != m_editToolShift_List.end(); itr++){
		if((*itr).cmd_name == edit_key.cmd_name){
			(*itr) = edit_key;
			return;
		}
	}

	m_editToolShift_List.push_back(edit_key);
}