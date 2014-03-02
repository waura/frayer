#ifndef _IEPARAMREGISTER_H_
#define _IEPARAMREGISTER_H_

#include <map>
#include <string>

#include "LibIEDllHeader.h"
#include "IEFileIOCommon.h"

typedef std::map<std::string, std::string> IEParamMap;
typedef std::map<std::string, IEParamMap*> IESectionMap;

/*!
*/
class _EXPORTCPP IEParamRegister
{
public:
	IEParamRegister(){};
	~IEParamRegister(){};

	void SetStartDir(const char* path);
	void SetIEParamRegisterDat(const char* path);

	void Init();
	void End();

	bool ReadParam(const char* section, const char* name, char* value);
	void WriteParam(const char* section, const char* name, const char* value);

private:
	std::string m_IEStartDir;
	std::string m_IEParamRegisterDat;

	IESectionMap m_ieSectionMap;
};

#endif //_IEPARAMREGISTER_H_