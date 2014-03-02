#ifndef _IEKEYMAP_H_
#define _IEKEYMAP_H_

#include <map>
#include <string>

#include "IEKeyCord.h"
#include "LibIEDllHeader.h"
#include "IEFileIOCommon.h"

#define IE_EXT_KEY_CONTROL 0x8000
#define IE_EXT_KEY_SHIFT   0x4000
#define IE_EXT_KEY_ALT 0x2000
#define IE_KEY_CODE_MASK   0x00ff

typedef std::map<uint32_t, std::string> IEShortCutMap;
typedef std::map<uint32_t, std::string> IEToolShiftMap;

/*!
	@brief this class have map of short cut key and tool shift key
*/
class _EXPORTCPP IEKeyMap
{
public:
	IEKeyMap(){};
	~IEKeyMap(){};

	void SetStartDir(const char* path);
	void SetIEKeyMapDat(const char* path);

	void Init();
	void End();

	void AddShortCut(uint32_t keycode, const char* cmd);
	void DeleteShortCut(uint32_t keycode);
	void DeleteShortCut(const char* cmd);
	bool GetShortCutCmd(uint32_t keycode, char* dst);
	uint32_t GetShortCutKey(const char* cmd);

	void AddToolShift(uint32_t keycode, const char* tool_name);
	void DeleteToolShift(uint32_t keycode);
	void DeleteToolShift(const char* tool_name);
	bool GetToolShiftName(uint32_t keycode, char* dst);
	uint32_t GetToolShiftKey(const char* tool_name);

private:

	void MakeDefaultKeyMap();

	bool m_isChanged;
	std::string m_IEStartDir;
	std::string m_IEKeyMapDat;

	IEShortCutMap m_ieShortCutMap;
	IEToolShiftMap m_ieToolShiftMap;
};

#endif //_IEKEYMAP_H_