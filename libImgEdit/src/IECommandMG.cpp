#include "stdafx.h"

#include "IECommandMG.h"
#include "OutputError.h"

std::string IECommandMG::m_IEStartDir;
std::string IECommandMG::m_IECommandDir;
IECommand_Map IECommandMG::m_IECommand_Map;
CreateIECommand_Map IECommandMG::m_CreateIECommand_Map;

#define IE_CMD_ID_OFFSET 50000

void IECommandMG::SetStartDir(const char* path)
{
	m_IEStartDir = path;
}

void IECommandMG::Init(const char* plugin_dir_path)
{
	m_IECommandDir = plugin_dir_path;

	//dll検索
	HANDLE hFind;
	WIN32_FIND_DATA fd;

	char dll_find_path[_MAX_PATH];
	wsprintf(dll_find_path, "%s\\%s\\*.dll", m_IEStartDir.c_str(), m_IECommandDir.c_str());
	hFind = ::FindFirstFile(dll_find_path, &fd);

	if (hFind == INVALID_HANDLE_VALUE) {//検索失敗
		OutputError::PushLog( LOG_LEVEL::_ERROR, "can't find command dll dir");
		return;
	}

	do {
		char dll_path[_MAX_PATH];
		char dll_all_path[_MAX_PATH];
		wsprintf(dll_path, "%s\\%s", m_IECommandDir.c_str(), fd.cFileName);
		wsprintf(dll_all_path, "%s\\%s\\%s", m_IEStartDir.c_str(), m_IECommandDir.c_str(), fd.cFileName);

		//dll読み込み
		HANDLE hLibrary = ::LoadLibrary(dll_all_path);
		if (!hLibrary) {
			DWORD code = ::GetLastError();
			OutputError::Alert("error: IECommandMG::Init() LoadLibrary(), ", dll_all_path);
			OutputError::PushLog(LOG_LEVEL::_INFO, "LoadLibrary last error = ", code);
			continue;
		}

		//dllから関数呼び出し
		_GetIECommandDisplayName pGetIECommandDisplayName;
		pGetIECommandDisplayName = (_GetIECommandDisplayName)::GetProcAddress((HMODULE)hLibrary, "GetIECommandDisplayNameJa");
		if (!pGetIECommandDisplayName) {
			OutputError::Alert("error: IECommandMG::Init() GetProcAddress(), GetIECommandDisplayNameJa");
			::FreeLibrary((HMODULE)hLibrary);
			continue;
		}

		char display_name[MAX_IE_MODULE_DISPLAY_NAME];
		pGetIECommandDisplayName(display_name);

		::FreeLibrary((HMODULE)hLibrary);

		TCHAR drive[_MAX_DRIVE];
		TCHAR dir[_MAX_DIR];
		TCHAR fname[_MAX_FNAME];
		TCHAR ext[_MAX_EXT];
		_splitpath_s(
			dll_all_path,
			drive,
			_MAX_DRIVE,
			dir,
			_MAX_DIR,
			fname,
			_MAX_FNAME,
			ext,
			_MAX_EXT);

		std::string name_s = fname;

		CMD_DATA* cmd_data = new CMD_DATA;
		cmd_data->dll_path = dll_all_path;
		cmd_data->extrn_cmd = NULL;
		cmd_data->display_name = display_name;

		//
		m_IECommand_Map.insert(IECommand_Map::value_type(name_s, cmd_data));

		//
		OutputError::PushLog( LOG_LEVEL::_INFO, "load command dll, ", dll_path);
	} while (::FindNextFile(hFind, &fd));

	::FindClose(hFind);
}

void IECommandMG::End()
{
	//free command map
	IECommand_Map::iterator mitr = m_IECommand_Map.begin();
	for (; mitr != m_IECommand_Map.end(); mitr++) {
		delete (*mitr).second;
	}
	m_IECommand_Map.clear();

	//free dll module
	CreateIECommand_Map::iterator itr = m_CreateIECommand_Map.begin();
	for (; itr != m_CreateIECommand_Map.end(); itr++) {
		_ReleaseIECommand pReleaseIECommand;
		pReleaseIECommand = (_ReleaseIECommand)::GetProcAddress((HMODULE)((*itr).second), "ReleaseIECommand");
		if (!pReleaseIECommand) {
			OutputError::Alert("error: IECommandMG::ReleaseIECommand() GetProcAddress()");
			break;
		}

		pReleaseIECommand( (IIECommand**)(&(*itr).first) );
		::FreeLibrary((HMODULE)((*itr).second));
	}
	m_CreateIECommand_Map.clear();
}

bool IECommandMG::AddExtrnCommandToTable(const char* cmd_name, IIECommand* pCommand)
{
	std::string cmd_name_s = cmd_name;
	IECommand_Map::iterator itr = m_IECommand_Map.find(cmd_name_s);
	if(itr != m_IECommand_Map.end()){
		return false;
	}

	CMD_DATA* cmd_data = new CMD_DATA;
	cmd_data->display_name = cmd_name;
	cmd_data->extrn_cmd = pCommand;

	//add to command map
	m_IECommand_Map.insert(IECommand_Map::value_type(cmd_name, cmd_data));
	return true;
}

bool IECommandMG::RemoveExtrnCommandToTable(const char* cmd_name)
{
	std::string cmd_name_s = cmd_name;
	IECommand_Map::iterator itr = m_IECommand_Map.find(cmd_name_s);
	if(itr != m_IECommand_Map.end()){
		m_IECommand_Map.erase( itr );
		return true;
	}
	return false;
}

int IECommandMG::GetCommandID(const char* cmd_name)
{
	int index = 0;
	IECommand_Map::iterator itr = m_IECommand_Map.begin();
	for(; itr != m_IECommand_Map.end(); itr++){
		
		if(strcmp((*itr).first.c_str(), cmd_name) == 0){//hitするコマンド名を見つける
			return index + IE_CMD_ID_OFFSET;
		}

		index++;
	}

	return -1;
}

int IECommandMG::GetMinID()
{
	return IE_CMD_ID_OFFSET;
}

int IECommandMG::GetMaxID()
{
	return m_IECommand_Map.size() + IE_CMD_ID_OFFSET;
}

bool IECommandMG::GetCommandName(int id, char* dst)
{
	int offset_id = id - IE_CMD_ID_OFFSET;

	IECommand_Map::iterator itr;
	size_t size = m_IECommand_Map.size();
	if(0 <= offset_id && offset_id < size/2){
		itr = m_IECommand_Map.begin();
		for(int i=0; i<offset_id; i++)
			itr++;

		strcpy(dst, (*itr).first.c_str());
		return true;
	}
	else if(size/2 <= offset_id && offset_id < size){
		itr = m_IECommand_Map.end();
		for(int i=offset_id; i<size; i++){
			itr--;
		}

		strcpy(dst, (*itr).first.c_str());
		return true;
	}
	return false;
}

bool IECommandMG::GetDisplayName(const char* cmd_name, char* dst)
{
	std::string cmd_name_s = cmd_name;
	IECommand_Map::iterator itr = m_IECommand_Map.find(cmd_name_s);
	if(itr != m_IECommand_Map.end()){
		strcpy(dst, (*itr).second->display_name.c_str());
		return true;
	}

	return false;
}

bool IECommandMG::GetDisplayName(int id, char* dst)
{
	int offset_id = id - IE_CMD_ID_OFFSET;

	IECommand_Map::iterator itr;
	size_t size = m_IECommand_Map.size();
	if(0 <= offset_id && offset_id < size/2){
		itr = m_IECommand_Map.begin();
		for(int i=0; i<offset_id; i++)
			itr++;

		strcpy(dst, (*itr).second->display_name.c_str());
		return true;
	}
	else if(size/2 <= offset_id && offset_id < size){
		itr = m_IECommand_Map.end();
		for(int i=offset_id; i<size; i++){
			itr--;
		}

		strcpy(dst, (*itr).second->display_name.c_str());
		return true;
	}
	return false;
}

IIECommand* IECommandMG::CreateIECommand(const char* cmd_name)
{
	std::string cmd_name_s = cmd_name;
	IECommand_Map::iterator itr = m_IECommand_Map.find(cmd_name_s);
	if(itr != m_IECommand_Map.end()){
		if((*itr).second->extrn_cmd){
			return (*itr).second->extrn_cmd;
		}

		//dll呼び出し
		return CallDLL((*itr).second->dll_path.c_str());
	}

	return false;
}

IIECommand* IECommandMG::CreateIECommand(int id)
{
	int offset_id = id - IE_CMD_ID_OFFSET;

	IECommand_Map::iterator itr;
	size_t size = m_IECommand_Map.size();
	if(0 <= offset_id && offset_id < size/2){
		itr = m_IECommand_Map.begin();
		for(int i=0; i<offset_id; i++)
			itr++;
		
		if((*itr).second->extrn_cmd){
			return (*itr).second->extrn_cmd;
		}
		return CallDLL((*itr).second->dll_path.c_str());
	}
	else if(size/2 <= offset_id && offset_id < size){
		itr = m_IECommand_Map.end();
		for(int i=offset_id; i<size; i++){
			itr--;
		}

		if((*itr).second->extrn_cmd){
			return (*itr).second->extrn_cmd;
		}
		return CallDLL((*itr).second->dll_path.c_str());
	}

	return false;
}

//
void IECommandMG::ReleaseIECommand(IIECommand **ppCommand)
{
	CreateIECommand_Map::const_iterator itr = m_CreateIECommand_Map.find((*ppCommand));
	if (itr != m_CreateIECommand_Map.end()) {
		//
		_ReleaseIECommand pReleaseIECommand;
		pReleaseIECommand = (_ReleaseIECommand)::GetProcAddress((HMODULE)((*itr).second), "ReleaseIECommand");
		if (!pReleaseIECommand) {
			OutputError::Alert("error: IECommandMG::ReleaseIECommand() GetProcAddress()");
		}
		else {
			pReleaseIECommand( ppCommand );
		}
		::FreeLibrary((HMODULE)((*itr).second));

		m_CreateIECommand_Map.erase(itr);
	}
}

IIECommand* IECommandMG::CallDLL(const char *dll_path)
{
		//dll読み込み
		HANDLE hLibrary = ::LoadLibrary(dll_path);
		if (!hLibrary) {
			OutputError::Alert("error: IECommandMG::CallDLL() LoadLibrary()");
			return NULL;
		}

		//dllから関数呼び出し
		_CreateIECommand pCreateIECommand;
		pCreateIECommand = (_CreateIECommand)::GetProcAddress((HMODULE)hLibrary, "CreateIECommand");
		if (!pCreateIECommand) {
			OutputError::Alert("error: IECommandMG::CallDLL() GetProcAddress()");
			::FreeLibrary((HMODULE)hLibrary);
			return NULL;
		}

		IIECommand* pCommand = pCreateIECommand();
		if (!pCommand) {
			OutputError::PushLog(LOG_LEVEL::_ERROR, "faild Create IECommand in ", dll_path);
			return NULL;
		}

		m_CreateIECommand_Map.insert(CreateIECommand_Map::value_type(pCommand, hLibrary));

		return pCommand;
}