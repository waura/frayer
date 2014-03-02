#ifndef _IEBRUSH_TEXTURE_MG_H_
#define _IEBRUSH_TEXTURE_MG_H_

#include "LibIEDllHeader.h"

#include <vector>
#include <string>

typedef std::vector<std::string> FileName_Vec;

class _EXPORTCPP IEBrushTextureMG
{
public:
	IEBrushTextureMG();
	~IEBrushTextureMG();

	void SetStartDir(const char* path);

	bool Open(const char* dir_path);

	void GetBrushTextureDirectoryPath(char* dst_path, int max_path_size) const;

	int GetBrushTextureIndex(const char* texture_name) const;
	void GetBrushTexturePath(char* dst_path, int max_path_size, int index) const;
	void SetBrushTextureComboBox(HWND hCombo) const;

private:
	std::string m_IEStartDir;
	std::string m_DirectoryPath;
	FileName_Vec m_FileName_Vec;
};

#endif //_IEBRUSH_TEXTURE_MG_H_