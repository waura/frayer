#ifndef _IMGFILEIOMG_H_
#define _IMGFILEIOMG_H_

#include <map>

#include "ImgFile.h"

#ifdef __cplusplus
#define IEIO_EXPORT extern "C" __declspec (dllexport)
#else
#define IEIO_EXPORT __declspec (dllexport)
#endif

typedef void (*_IEIO_GetFilter)(char filter[], size_t buf_size);
typedef bool (*_IEIO_LoadImgFile)(const char *filePath, ImgFile_Ptr pFile);
typedef bool (*_IEIO_SaveAsImgFile)(const char *filePath, const ImgFile_Ptr pFile);


typedef std::map<std::string, std::string> IEImgFileIOModule_Map;

class _EXPORTCPP ImgFileIOMG
{
public:
	static void SetStartDir(const char* path);

	static bool Init(const char* plugin_dir_path);
	static void End();

	static ImgFile_Ptr LoadImgFile(const char *filePath);
	static bool SaveAsImgFile(const char *filePath, const ImgFile_Ptr pFile);

	static void GetLoadExtFilters(char* str_filters);
	static int GetLoadExtFiltersLength();

	static void GetSaveExtFilters(char* str_filters);
	static int GetSaveExtFiltersLength();

	static int GetNFilterIndex(const char* ext);

private:
	static std::string m_IEStartDir;
	static std::string m_IEFileIODir;

	static IEImgFileIOModule_Map m_ImgFileIOModule_Map;
	static std::string m_IELoadFilter;
	static std::string m_IESaveFilter;
};

#endif //_IMGFILEIOMG_H_