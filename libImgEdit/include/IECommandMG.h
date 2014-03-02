#ifndef _IECOMMANDMG_H_
#define _IECOMMANDMG_H_

#include <string>
#include <map>

#include "ImgEdit.h"
#include "IIECommand.h"

typedef IEMODULE_ID (*_GetIEModule_ID)();
typedef void (*_GetIECommandName)(char*);
typedef void (*_GetIECommandDisplayName)(char*);
typedef IIECommand* (*_CreateIECommand)();
typedef void (*_ReleaseIECommand)(IIECommand**);

typedef struct _CMD_DATA{
	std::string dll_path;
	std::string display_name;
	IIECommand* extrn_cmd;
}CMD_DATA;

typedef std::map<std::string, CMD_DATA*> IECommand_Map; //(command_name, cmd_data)
typedef std::map<IIECommand*, HANDLE> CreateIECommand_Map;

class _EXPORTCPP IECommandMG
{
public:
	static void SetStartDir(const char* path);

	static void Init(const char* plugin_dir_path);
	static void End();

	static bool AddExtrnCommandToTable(const char* cmd_name, IIECommand* pCommand);
	static bool RemoveExtrnCommandToTable(const char* cmd_name);

	//コマンド名からidを得る。
	static int GetCommandID(const char* cmd_name);
	
	static int GetMinID();
	static int GetMaxID();

	static bool GetCommandName(int id, char* dst);
	static bool GetDisplayName(const char* cmd_name, char* dst);
	static bool GetDisplayName(int id, char* dst);
	
	//コマンドの生成
	static IIECommand* CreateIECommand(const char* cmd_name);
	static IIECommand* CreateIECommand(int id);

	//コマンド開放
	static void ReleaseIECommand(IIECommand** ppCommand);

private:
	static IIECommand* CallDLL(const char* dll_path);

	static std::string m_IEStartDir;
	static std::string m_IECommandDir;

	static IECommand_Map m_IECommand_Map;
	static CreateIECommand_Map m_CreateIECommand_Map; //生成したコマンドとモジュールのマップ
};

#endif //_IECOMMANDMG_H_