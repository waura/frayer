#include "stdafx.h"

#include "IEToolGroup.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


_EXPORT IEToolGroup* CreateIEToolGroup()
{
	return new IEToolGroup;
}

_EXPORT void ReleaseIEToolGroup(IEToolGroup** ppTGroup)
{
	if (*ppTGroup) {
		delete (*ppTGroup);
		(*ppTGroup) = NULL;
	}
}

IEToolGroup::IEToolGroup()
{
}

IEToolGroup::~IEToolGroup()
{
}

void IEToolGroup::AddTool(IIETool* pTool)
{
	m_IIETool_Vec.push_back( pTool );
}

IIETool* IEToolGroup::GetIEToolFromName(const char *tool_name)
{
	IIETool_Vec::iterator itr = m_IIETool_Vec.begin();
	for(; itr != m_IIETool_Vec.end(); itr++){
		if(strcmp((*itr)->GetName(), tool_name) == 0){
			return (*itr);
		}
	}
	return NULL;
}

bool IEToolGroup::GetToolName(int tnode_index, char *dst)
{
	if(0 <= tnode_index && tnode_index < m_IIETool_Vec.size()){
		m_IIETool_Vec[tnode_index]->GetName( dst );
		return true;
	}
	return false;
}

bool IEToolGroup::GetToolName(const char *display_name, char *dst)
{
	IIETool_Vec::iterator itr = m_IIETool_Vec.begin();
	for(; itr != m_IIETool_Vec.end(); itr++){
		if(strcmp((*itr)->GetDisplayName(), display_name) == 0){
			(*itr)->GetName( dst );
			return true;
		}
	}
	return false;
}

bool IEToolGroup::GetDisplayName(int tnode_index, char *dst)
{
	if(0 <= tnode_index && tnode_index < m_IIETool_Vec.size()){
		m_IIETool_Vec[tnode_index]->GetDisplayName( dst );
		return true;
	}
	return false;
}

bool IEToolGroup::GetDisplayName(const char* tool_name, char *dst)
{
	IIETool_Vec::iterator itr = m_IIETool_Vec.begin();
	for(; itr != m_IIETool_Vec.end(); itr++){
		if(strcmp((*itr)->GetName(), tool_name) == 0){
			(*itr)->GetDisplayName( dst );
			return true;
		}
	}
	return false;
}