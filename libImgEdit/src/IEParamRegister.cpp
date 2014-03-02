#include "stdafx.h"

#include "IEParamRegister.h"
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

void IEParamRegister::SetStartDir(const char* path)
{
	m_IEStartDir = path;
}

void IEParamRegister::SetIEParamRegisterDat(const char* path)
{
	m_IEParamRegisterDat = path;
}

void IEParamRegister::Init()
{
	char path[_MAX_PATH];
	wsprintf(path, "%s\\%s", m_IEStartDir.c_str(), m_IEParamRegisterDat.c_str());

	//read dat file
	FILE* fp;
	if ((fp = fopen(path, "rb")) == NULL) {
		OutputError::PushLog(LOG_LEVEL::_WARN, "can't open param register dat file");
		return;
	}

	try {
		uint32_t i,j;
		uint32_t num_of_section = Read4ByteLE(fp);
		for (i=0; i<num_of_section; i++) {
			//read section name
			uint16_t section_name_len = Read2ByteLE(fp);
			char* buf = new char[section_name_len + 1];
			ReadString(fp, section_name_len, buf);

			//add section to map
			IEParamMap *pParamMap = new IEParamMap;
			m_ieSectionMap.insert(IESectionMap::value_type(buf, pParamMap));

			delete[] buf;
			buf = NULL;

			//read param in section
			uint32_t section_size = Read4ByteLE(fp);
			for (j=0; j<section_size; j++) {
				uint16_t name_len = Read2ByteLE(fp);
				char* name = new char[name_len+1];
				ReadString(fp, name_len, name);

				uint16_t val_len = Read2ByteLE(fp);
				char* val = new char[val_len+1];
				ReadString(fp, val_len, val);

				pParamMap->insert(IEParamMap::value_type(name, val));
				delete[] val; val = NULL;
				delete[] name; name = NULL;
			}
		}

		fclose(fp);

	} catch (std::ios_base::failure& e) {
		fclose(fp);
		OutputError::PushLog(LOG_LEVEL::_ERROR, "read param register file exception: ", e.what());
	}
}

void IEParamRegister::End()
{
	uint32_t i,j;

	char path[_MAX_PATH];
	wsprintf(path, "%s\\%s", m_IEStartDir.c_str(), m_IEParamRegisterDat.c_str());

	//write dat file
	FILE* fp;
	if ((fp = fopen(path, "wb")) == NULL) {
		OutputError::PushLog(LOG_LEVEL::_WARN, "can't open param register dat file");
		return;
	}

	try {
		uint32_t num_of_section = m_ieSectionMap.size();
		Write4ByteLE(fp, num_of_section);

		//write section map
		IESectionMap::iterator itr = m_ieSectionMap.begin();
		for (; itr != m_ieSectionMap.end(); itr++) {
			//write section name
			uint16_t section_name_len = (*itr).first.size();
			Write2ByteLE(fp, section_name_len);
			WriteString(fp, section_name_len, (*itr).first.c_str());

			//write param in section
			IEParamMap* pParamMap = (*itr).second;
			uint32_t section_size = pParamMap->size();
			Write4ByteLE(fp, section_size);

			IEParamMap::iterator itr = pParamMap->begin();
			for (; itr != pParamMap->end(); itr++) {
				//write name
				uint16_t name_len = (*itr).first.size();
				Write2ByteLE(fp, name_len);
				WriteString(fp, name_len, (*itr).first.c_str());

				//write value
				uint16_t val_len = (*itr).second.size();
				Write2ByteLE(fp, val_len);
				WriteString(fp, val_len, (*itr).second.c_str());
			}
		}

		fclose(fp);

	} catch (std::ios_base::failure& e) {
		fclose(fp);
		OutputError::PushLog(LOG_LEVEL::_ERROR, "write param register file exception: ", e.what());
	}
}

bool IEParamRegister::ReadParam(const char* section, const char* name, char* value)
{
	IESectionMap::iterator itr_s = m_ieSectionMap.find(section);
	if (itr_s == m_ieSectionMap.end()) {
		return false;
	}

	IEParamMap *pParamMap = (*itr_s).second;
	IEParamMap::iterator itr_p = pParamMap->find(name);
	if (itr_p == pParamMap->end()) {
		return false;
	}

	strncpy(value, (*itr_p).second.c_str(), (*itr_p).second.size() + 1);
	return true;
}

void IEParamRegister::WriteParam(const char* section, const char* name, const char* value)
{
	IESectionMap::iterator itr_s = m_ieSectionMap.find(section);
	if (itr_s == m_ieSectionMap.end()) {
		//if don't exist section, add section
		IEParamMap *pParamMap = new IEParamMap;
		m_ieSectionMap.insert(IESectionMap::value_type(section, pParamMap));

		//add param in section
		pParamMap->insert(IEParamMap::value_type(name, value));
		return;
	}

	IEParamMap* pParamMap = (*itr_s).second;
	IEParamMap::iterator itr_p = pParamMap->find(name);
	if (itr_p == pParamMap->end()) {
		//add param in section
		pParamMap->insert(IEParamMap::value_type(name, value));
		return;
	}
	//over write param;
	(*itr_p).second = value;
}