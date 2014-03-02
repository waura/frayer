#ifndef _IEMODULES_H_
#define _IEMODULES_H_

enum IEMODULE_ID {
	COMMAND = 0x1,
	TOOL = 0x2,
	EFFECT = 0x4,
	IMG_FILE_IO = 0x10,
};

class _EXPORTCPP IIEModule
{
public:
	IIEModule(){};
	virtual ~IIEModule(){};

	//////////////////////////////
	/*!
		tool name
	*/
	inline const char* GetName() const {
		return m_name;
	}
	inline void GetName(char* dst) const {
		strcpy_s(dst, MAX_IE_MODULE_NAME, m_name);
	}

	//////////////////////////////
	/*!
		display name
	*/
	inline const char* GetDisplayName() const {
		return m_disp_name;
	}
	inline void GetDisplayName(char* dst) const {
		strcpy_s(dst, MAX_IE_MODULE_DISPLAY_NAME, m_disp_name);
	}

protected:
	char m_name[MAX_IE_MODULE_NAME];
	char m_disp_name[MAX_IE_MODULE_DISPLAY_NAME];
};

#endif