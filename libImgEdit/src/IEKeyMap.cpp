#include "stdafx.h"

#include "IEKeyMap.h"
#include "OutputError.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


static void ReadString(FILE* fp, int len, char* dst)
{
	int i;
	for (i=0; i<len; i++) {
		(*dst++) = Read1ByteLE(fp);
	}
	(*dst) = '\0';
}

static void WriteString(FILE* fp, int len, const char* src)
{
	int i;
	for (i=0; i<len; i++) {
		Write1ByteLE(fp, (*src++));
	}
}

void IEKeyMap::SetStartDir(const char *path)
{
	m_IEStartDir = path;
}

void IEKeyMap::SetIEKeyMapDat(const char *path)
{
	m_IEKeyMapDat = path;
}

void IEKeyMap::Init()
{
	m_isChanged = false;

	char path[_MAX_PATH];
	wsprintf(path, "%s\\%s", m_IEStartDir.c_str(), m_IEKeyMapDat.c_str());

	//read dat file
	FILE* fp;
	if ((fp = fopen(path, "rb")) == NULL) {
		m_isChanged = true;
		OutputError::PushLog(LOG_LEVEL::_INFO, "can't open keymap dat file. create default keymap");
		MakeDefaultKeyMap();
		return;
	}

	uint32_t i,j;
	uint32_t category_size = Read2ByteLE(fp);
	if (category_size != 2) {
		m_isChanged = true;
		OutputError::PushLog(LOG_LEVEL::_WARN, "invalid format keymap dat file");
		return;
	}

	uint32_t shortcut_size = Read4ByteLE(fp);
	uint32_t toolshift_size = Read4ByteLE(fp);
	
	for (i=0; i<shortcut_size; i++) {
		uint32_t key_code = Read4ByteLE(fp);
		uint32_t cmd_length = Read2ByteLE(fp);

		char* cmd = new char[cmd_length+1];
		ReadString(fp, cmd_length, cmd);

		m_ieShortCutMap.insert(IEShortCutMap::value_type(key_code, cmd));
		delete[] cmd;
	}

	for (i=0; i<toolshift_size; i++) {
		uint32_t key_code = Read4ByteLE(fp);
		uint32_t name_length = Read2ByteLE(fp);

		char* tool_name = new char[name_length+1];
		ReadString(fp, name_length, tool_name);

		m_ieToolShiftMap.insert(IEToolShiftMap::value_type(key_code, tool_name));
		delete[] tool_name;
	}
}

void IEKeyMap::End()
{
	if (m_isChanged) {
		uint32_t i,j;

		char path[_MAX_PATH];
		wsprintf(path, "%s\\%s", m_IEStartDir.c_str(), m_IEKeyMapDat.c_str());

		//read dat file
		FILE* fp;
		if ((fp = fopen(path, "wb")) == NULL) {
			OutputError::PushLog(LOG_LEVEL::_WARN, "can't open keymap dat file");
			return;
		}

		//write dat file
		Write2ByteLE(fp, 2); //category_size
		uint32_t shortcut_size  = m_ieShortCutMap.size();
		uint32_t toolshift_size = m_ieToolShiftMap.size();
		Write4ByteLE(fp, shortcut_size);
		Write4ByteLE(fp, toolshift_size);

		//write shortcut key map
		IEShortCutMap::iterator itr = m_ieShortCutMap.begin();
		for (; itr != m_ieShortCutMap.end(); itr++) {
			Write4ByteLE(fp, (*itr).first ); //keycode
			uint16_t cmd_length = (*itr).second.size();
			Write2ByteLE(fp, cmd_length ); //cmd length
			WriteString(fp, cmd_length, (*itr).second.c_str());
		}

		//write toolshift key map
		IEToolShiftMap::iterator itr_ts = m_ieToolShiftMap.begin();
		for (; itr_ts != m_ieToolShiftMap.end(); itr_ts++) {
			Write4ByteLE(fp, (*itr_ts).first ); //keycode
			uint16_t name_length = (*itr_ts).second.size();
			Write2ByteLE( fp, name_length );
			WriteString(fp, name_length, (*itr_ts).second.c_str());
		}
	}
}

void IEKeyMap::AddShortCut(uint32_t keycode, const char* cmd)
{
	m_ieShortCutMap.insert(IEShortCutMap::value_type(keycode, cmd));

	m_isChanged = true;
}

void IEKeyMap::DeleteShortCut(uint32_t keycode)
{
	IEShortCutMap::iterator itr = m_ieShortCutMap.find(keycode);
	if(itr != m_ieShortCutMap.end()){
		m_ieShortCutMap.erase( itr );
	}

	m_isChanged = true;
}

void IEKeyMap::DeleteShortCut(const char* cmd)
{
	IEShortCutMap::iterator itr = m_ieShortCutMap.begin();
	for(; itr != m_ieShortCutMap.end(); itr++){
		if(strcmp(cmd, (*itr).second.c_str()) == 0){
			m_ieShortCutMap.erase( itr );
			return;
		}
	}
}

bool IEKeyMap::GetShortCutCmd(uint32_t keycode, char* dst)
{
	IEShortCutMap::iterator itr = m_ieShortCutMap.find(keycode);
	if(itr != m_ieShortCutMap.end()){
		strcpy(dst, (*itr).second.c_str());
		return true;
	}
	
	return false;
}

uint32_t IEKeyMap::GetShortCutKey(const char* cmd)
{
	IEShortCutMap::iterator itr = m_ieShortCutMap.begin();
	for(; itr != m_ieShortCutMap.end(); itr++){
		if(strcmp(cmd, (*itr).second.c_str()) == 0){
			return (*itr).first;
		}
	}
	
	return -1;
}

void IEKeyMap::AddToolShift(uint32_t keycode, const char* tool_name)
{
	m_ieToolShiftMap.insert(IEToolShiftMap::value_type(keycode, tool_name));

	m_isChanged = true;
}

void IEKeyMap::DeleteToolShift(uint32_t keycode)
{
	IEToolShiftMap::iterator itr = m_ieToolShiftMap.find(keycode);
	if(itr != m_ieToolShiftMap.end()){
		m_ieToolShiftMap.erase( itr );
	}

	m_isChanged = true;
}

void IEKeyMap::DeleteToolShift(const char* tool_name)
{
	IEToolShiftMap::iterator itr = m_ieToolShiftMap.begin();
	for(; itr != m_ieToolShiftMap.end(); itr++){
		if(strcmp(tool_name, (*itr).second.c_str()) == 0){
			m_ieToolShiftMap.erase( itr );
			return;
		}
	}
}

bool IEKeyMap::GetToolShiftName(uint32_t keycode, char* dst)
{
	IEToolShiftMap::iterator itr = m_ieToolShiftMap.find(keycode);
	if(itr != m_ieToolShiftMap.end()){
		strcpy(dst, (*itr).second.c_str());
		return true;
	}
	
	return false;
}

uint32_t IEKeyMap::GetToolShiftKey(const char* tool_name)
{
	IEToolShiftMap::iterator itr = m_ieToolShiftMap.begin();
	for(; itr != m_ieToolShiftMap.end(); itr++){
		if(strcmp(tool_name, (*itr).second.c_str()) == 0){
			return (*itr).first;
		}
	}
	
	return -1;
}

void IEKeyMap::MakeDefaultKeyMap()
{
	AddShortCut('h', "ViewFlipH");
	AddShortCut('v', "ViewFlipV");
	AddShortCut('z' | IE_EXT_KEY_CONTROL, "Undo");
	AddShortCut('y' | IE_EXT_KEY_CONTROL, "Redo");

	AddToolShift('b', "NormalBrushDraw");
	AddToolShift(IE_EXT_KEY_ALT | IE_KEY_MENU, "SpoitTool");
	AddToolShift(IE_KEY_SPACE, "ViewParallelTrans");
}