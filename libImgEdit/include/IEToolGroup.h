#ifndef _IE_TOOL_GROUP_H_
#define _IE_TOOL_GROUP_H_

#include "IIETool.h"

typedef std::vector<IIETool*> IIETool_Vec;

class _EXPORTCPP IEToolGroup
{
public:
	IEToolGroup();
	~IEToolGroup();

	void SetSelectTool(int index){
		m_SelectToolIndex = index;
	}
	const IIETool* GetSelectTool() const {
		return m_IIETool_Vec[ m_SelectToolIndex ];
	}

	int GetToolSize() const {
		return m_IIETool_Vec.size();
	}

	IIETool* GetToolAtIndex(int index) const {
		if(0 <= index && index < m_IIETool_Vec.size()){
			return m_IIETool_Vec[index];
		}
		return NULL;
	}

	void AddTool(IIETool* pTool);

	IIETool* GetIEToolFromName(const char *tool_name);

	bool GetToolName(int tnode_index, char *dst);
	bool GetToolName(const char *display_name, char *dst);

	bool GetDisplayName(int tnode_index, char *dst);
	bool GetDisplayName(const char *tool_name, char *dst);

	//////////////////////////////
	/*!
		tool group name
	*/
	inline const char* GetName() const {
		return m_name;
	}
	inline void GetName(char* dst) const {
		strcpy_s(dst, MAX_IE_TOOL_GROUP_NAME, m_name);
	}
	inline void SetName(const char* src){
		strcpy_s(m_name, MAX_IE_TOOL_GROUP_NAME, src);
	}

private:
	char m_name[MAX_IE_TOOL_GROUP_NAME];
	IIETool_Vec m_IIETool_Vec;
	int m_SelectToolIndex;
};

_EXPORT IEToolGroup* CreateIEToolGroup();
_EXPORT void ReleaseIEToolGroup(IEToolGroup** ppTGroup);

#endif //_IE_TOOL_GROUP_H_