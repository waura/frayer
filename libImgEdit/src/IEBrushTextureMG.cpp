#include "stdafx.h"

#include "IEBrushTextureMG.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

IEBrushTextureMG::IEBrushTextureMG()
{
}

IEBrushTextureMG::~IEBrushTextureMG()
{
	m_FileName_Vec.clear();
}

void IEBrushTextureMG::SetStartDir(const char* path)
{
	m_IEStartDir = path;
}

bool IEBrushTextureMG::Open(const char* dir_path)
{
	m_DirectoryPath = dir_path;
	std::string path = m_IEStartDir + m_DirectoryPath + "\\";

	//
	if (!PathFileExists(path.c_str())) {
		BOOL ret = MakeSureDirectoryPathExists(path.c_str());
		DWORD last_error = GetLastError();

		char log[256];
		wsprintf(log, "last_error = %d\n", last_error);
		::OutputDebugString(log);
	}

	//search in directory
	HANDLE hFind;
	WIN32_FIND_DATA fd;

	char fpath[_MAX_PATH];
	wsprintf(fpath, "%s\\*.*", path.c_str());
	hFind = ::FindFirstFile(fpath, &fd);

	if (hFind == INVALID_HANDLE_VALUE) {
		return false;
	}

	do {
		if (strcmp(fd.cFileName, ".") == 0)
			continue;
		if (strcmp(fd.cFileName, "..") == 0)
			continue;
		m_FileName_Vec.push_back(fd.cFileName);
	} while(::FindNextFile(hFind, &fd));

	::FindClose(hFind);
}

int IEBrushTextureMG::GetBrushTextureIndex(const char* texture_name) const
{
	//
	int index = 0;
	FileName_Vec::const_iterator itr = m_FileName_Vec.begin();
	for(; itr != m_FileName_Vec.end(); itr++){
		if (strcmp((*itr).c_str(), texture_name) == 0){
			return index;
		}
		index++;
	}
	return -1;
}

void IEBrushTextureMG::GetBrushTexturePath(char* dst_path, int max_path_size, int index) const
{
	assert(0 <= index && index < m_FileName_Vec.size());

	if (0 <= index && index < m_FileName_Vec.size()) {
		std::string name = m_FileName_Vec[index];
		std::string ret_str = m_DirectoryPath + "\\" + name;
		strcpy_s(dst_path, max_path_size, ret_str.c_str());
	}
}

void IEBrushTextureMG::SetBrushTextureComboBox(HWND hCombo) const
{
	//
	FileName_Vec::const_iterator itr = m_FileName_Vec.begin();
	for(; itr != m_FileName_Vec.end(); itr++){
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)(*itr).c_str());
	}

	return;
}
