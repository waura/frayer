#include "stdafx.h"

#include "ImgFileIOMG.h"
#include "IEIOfyd.h"
#include "OutputError.h"

#define DEFAULT_FILTER  "Frayer iamge data(*.fyd)|*.fyd;*.FYD|" \
						"Windows bitmaps(*.bmp)|*.bmp;*.BMP|" \
						"Portable image format(*.pbm)|*.pbm;*.PBM|" \
						"Sun rasters(*.sr;*ras)|*.sr;*.SR;*.ras;*.RAS|" \
						"JPEG 2000(*.jp2)|*.jp2|" \
						"TIFF(*.tif;*tiff)|*.tif;*.TIF;*.tiff;*.TIFF|" \
						"OpenEXR HDR images(*.exr)|*.exr;*.EXR|" \

#define DEFAULT_EXT "*.fyd;*.FYD;*.bmp;*.BMP;*.pbm;*.PBM;*.sr;*.SR;*.ras;*.RAS;*.jp2;*.tif;*.TIF;*.tiff;*.TIFF;*.exr;*.EXR;"


std::string ImgFileIOMG::m_IEStartDir;
std::string ImgFileIOMG::m_IEFileIODir;

IEImgFileIOModule_Map ImgFileIOMG::m_ImgFileIOModule_Map;
std::string ImgFileIOMG::m_IELoadFilter;
std::string ImgFileIOMG::m_IESaveFilter;

void ImgFileIOMG::SetStartDir(const char* path)
{
	m_IEStartDir = path;
}

bool ImgFileIOMG::Init(const char* plugin_dir_path)
{
	m_IEFileIODir = plugin_dir_path;
	ImgFileIOMG::m_IELoadFilter = DEFAULT_FILTER;
	ImgFileIOMG::m_IESaveFilter = DEFAULT_FILTER;

	std::string readable_ext = DEFAULT_EXT;

	//dll検索
	HANDLE hFind;
	WIN32_FIND_DATA fd;

	char dll_find_path[_MAX_PATH];
	wsprintf(dll_find_path, "%s\\%s\\*.dll", m_IEStartDir.c_str(), m_IEFileIODir.c_str());
	hFind = ::FindFirstFile(dll_find_path, &fd);

	if (hFind == INVALID_HANDLE_VALUE) {//検索失敗
		OutputError::PushLog( LOG_LEVEL::_ERROR, "can't find command dll dir");
		return false;
	}

	do {
		char dll_path[_MAX_PATH];
		char dll_all_path[_MAX_PATH];
		wsprintf(dll_path, "%s\\%s", m_IEFileIODir.c_str(), fd.cFileName);
		wsprintf(dll_all_path, "%s\\%s\\%s", m_IEStartDir.c_str(), m_IEFileIODir.c_str(), fd.cFileName);

		//load dll
		HANDLE hLibrary = ::LoadLibrary(dll_all_path);
		if(!hLibrary){
			DWORD code = ::GetLastError();
			OutputError::Alert("error: ImgFileIOMG::Init() LoadLibrary()");
			OutputError::PushLog(LOG_LEVEL::_INFO, "LoadLibrary last error = ", code);
			continue;
		}

		//dllから関数呼び出し
		_IEIO_GetFilter pIEIO_GetFilter;
		pIEIO_GetFilter = (_IEIO_GetFilter)::GetProcAddress((HMODULE)hLibrary, "IEIO_GetFilter");
		if(!pIEIO_GetFilter){
			OutputError::Alert("error: ImgFileIOMG::Init() GetProcAdderss(), IEIO_GetFilter");
			continue;
		}

		char filter_c_str[_MAX_PATH];
		pIEIO_GetFilter(filter_c_str, _MAX_PATH);
		::FreeLibrary((HMODULE)hLibrary);

		std::string mod_s = dll_all_path;
		std::string filter = filter_c_str;
		int p = filter.find_first_of('|');
		int p2 = filter.find_last_of('|');
		std::string last_exts_s = filter.substr(p+1, p2-p-1);

		readable_ext += ";";
		readable_ext += last_exts_s;

		std::string ext_s;
		for(;;){
			p = last_exts_s.find_first_of(';');
			if(p == std::string::npos){
				ext_s = last_exts_s.substr(1, last_exts_s.size()-1);
				m_ImgFileIOModule_Map.insert(IEImgFileIOModule_Map::value_type(ext_s, mod_s));
				break;
			}
				
			ext_s = last_exts_s.substr(1, p-1);
			last_exts_s = last_exts_s.substr(p+1, last_exts_s.size()-p);
			//
			m_ImgFileIOModule_Map.insert(IEImgFileIOModule_Map::value_type(ext_s, mod_s));
		}

		ImgFileIOMG::m_IELoadFilter += filter;
		ImgFileIOMG::m_IESaveFilter += filter;

		//
		OutputError::PushLog( LOG_LEVEL::_INFO, "load img file IO dll, ", dll_path);
	} while (::FindNextFile(hFind, &fd));
	::FindClose(hFind);

	//add all image filter
	ImgFileIOMG::m_IELoadFilter += "|\0";
	std::string tmp = "All Image File (*.*)|"; 
	tmp += readable_ext;
	tmp += "|";
	ImgFileIOMG::m_IELoadFilter = tmp + ImgFileIOMG::m_IELoadFilter;

	//
	ImgFileIOMG::m_IESaveFilter += "|\0";

	return true;
}

void ImgFileIOMG::End()
{
	m_ImgFileIOModule_Map.clear();
}

ImgFile_Ptr ImgFileIOMG::LoadImgFile(const char *filePath)
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	_splitpath_s(
		filePath,
		drive,
		_MAX_DRIVE,
		dir,
		_MAX_DIR,
		fname,
		_MAX_FNAME,
		ext,
		_MAX_EXT);

	bool ret;
	ImgFile_Ptr retFile = CreateIEImgFile();

	//fyd形式ならここで読み込み
	if(strcmp(ext, ".fyd") == 0){
		ret = IEIOFyd_LoadImgFile(filePath, retFile);
		if(ret){
			return retFile;
		}
		else{
			return NULL;
		}
	}

	//pluginで対応する形式がないか調べる
	std::string ext_s = ext;
	IEImgFileIOModule_Map::iterator itr;
	itr = m_ImgFileIOModule_Map.find(ext_s);
	if(itr != m_ImgFileIOModule_Map.end()){
		std::string mod_s = itr->second;
		
		//dll読み込み
		HANDLE hLibrary = ::LoadLibrary(mod_s.c_str());
		if(!hLibrary){
			OutputError::Alert("error: ImgFileIOMG::LoadImgFile LoadLibrary()");
			return NULL;
		}

		//dllから関数呼び出し
		_IEIO_LoadImgFile pIEIO_LoadImgFile;
		pIEIO_LoadImgFile = (_IEIO_LoadImgFile)::GetProcAddress((HMODULE)hLibrary, "IEIO_LoadImgFile");
		if(!pIEIO_LoadImgFile){
			OutputError::Alert("error: ImgFileIOMG::LoadImgFile() GetProcAdderss()");
			return NULL;
		}

		bool ret = pIEIO_LoadImgFile(filePath, retFile);

		::FreeLibrary((HMODULE)hLibrary);

		if(ret){
			return retFile;
		}
	}

	return NULL;
}

bool ImgFileIOMG::SaveAsImgFile(const char *filePath, const ImgFile_Ptr pFile)
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	_splitpath_s(
		filePath,
		drive,
		_MAX_DRIVE,
		dir,
		_MAX_DIR,
		fname,
		_MAX_FNAME,
		ext,
		_MAX_EXT);

	bool ret;

	//拡張子に応じたファイルの読み込み
	if(strcmp(ext, ".fyd") == 0){
		return IEIOFyd_SaveAsImgFile(filePath, pFile);
	}

	std::string ext_s = ext;
	IEImgFileIOModule_Map::iterator itr;
	itr = m_ImgFileIOModule_Map.find(ext_s);

	if(itr != m_ImgFileIOModule_Map.end()){
		std::string mod_s = itr->second;
		
		//dll読み込み
		HANDLE hLibrary = ::LoadLibrary(mod_s.c_str());
		if(!hLibrary){
			OutputError::Alert("error: ImgFileIOMG::LoadImgFile LoadLibrary()");
			return NULL;
		}

		//dllから関数呼び出し
		_IEIO_SaveAsImgFile pIEIO_SaveAsImgFile;
		pIEIO_SaveAsImgFile = (_IEIO_SaveAsImgFile)::GetProcAddress((HMODULE)hLibrary, "IEIO_SaveAsImgFile");
		if(!pIEIO_SaveAsImgFile){
			OutputError::Alert("error: ImgFileIOMG::SaveAsImgFile GetProcAdderss()");
			return NULL;
		}

		ret = pIEIO_SaveAsImgFile(filePath, pFile);

		::FreeLibrary((HMODULE)hLibrary);

		return ret;
	}

	return false;
}

void ImgFileIOMG::GetLoadExtFilters(char* str_filters)
{
	strcpy(str_filters, ImgFileIOMG::m_IELoadFilter.c_str());
}

int ImgFileIOMG::GetLoadExtFiltersLength()
{
	return ImgFileIOMG::m_IELoadFilter.size();
}

void ImgFileIOMG::GetSaveExtFilters(char* str_filters)
{
	strcpy(str_filters, ImgFileIOMG::m_IESaveFilter.c_str());
}

int ImgFileIOMG::GetSaveExtFiltersLength()
{
	return ImgFileIOMG::m_IESaveFilter.size();
}

int ImgFileIOMG::GetNFilterIndex(const char* ext)
{
	int sindex = 0;
	int nIndex = 1;

	for (;;) {
		int pos1 = ImgFileIOMG::m_IESaveFilter.find('|', sindex);
		if (std::string::npos == pos1) {
			return -1;
		}
		sindex = pos1 + 1;

		int pos2 = ImgFileIOMG::m_IESaveFilter.find('|', sindex);
		if (std::string::npos == pos2) {
			return -1;
		}

		//cutout extension string
		std::string str_exts = ImgFileIOMG::m_IESaveFilter.substr(sindex, pos2 - pos1 - 1);

		int ext_pos = str_exts.find(ext, 0);
		if (std::string::npos != ext_pos) {
			return nIndex;
		}

		sindex = pos2 + 1;
		nIndex++;
	}
}