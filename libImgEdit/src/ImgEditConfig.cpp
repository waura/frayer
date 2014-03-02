#include "stdafx.h"

#include "ImgEditConfig.h"

#define SECTION_NAME TEXT("imagedit")

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


ImgEditConfig::~ImgEditConfig()
{
}

void ImgEditConfig::LoadConfigFile(const char* path)
{
	//
	::GetPrivateProfileString(SECTION_NAME, "effect_xml", ".\\config\\effect.xml", effect_xml, _MAX_PATH, path);
	//
	::GetPrivateProfileString(SECTION_NAME, "tool_xml", ".\\config\\tool.xml", tool_xml, _MAX_PATH, path);
	//
	::GetPrivateProfileString(SECTION_NAME, "brush_dir", "", brush_dir, _MAX_PATH, path);
	//
	::GetPrivateProfileString(SECTION_NAME, "brush_xml", ".\\config\\brush.xml", brush_xml, _MAX_PATH, path);
	//
	::GetPrivateProfileString(SECTION_NAME, "palette_xml", ".\\config\\palette.xml", palette_xml, _MAX_PATH, path);
	//
	::GetPrivateProfileString(SECTION_NAME, "keymap_dat", ".\\config\\keymap.dat", keymap_dat, _MAX_PATH, path);
	//
	::GetPrivateProfileString(SECTION_NAME, "prm_reg_dat", ".\\config\\prm_reg.dat", prm_reg_dat, _MAX_PATH, path);
	//
	num_of_history = ::GetPrivateProfileInt(SECTION_NAME, "num_of_history", 100, path);
	//
	::GetPrivateProfileString(SECTION_NAME, "input_revise", "Gï‚ê≥8", input_revise, _MAX_PATH, path);
	//
	tool_shift_time = ::GetPrivateProfileInt(SECTION_NAME, "toolshift_time", 1000, path);
	//
	edit_wnd_bg_color.value = ::GetPrivateProfileInt(SECTION_NAME, "bg_color", 8355712, path);
}

void ImgEditConfig::SaveConfigFile(const char* path) 
{
	TCHAR buf[_MAX_PATH];

	//
	::WritePrivateProfileString(SECTION_NAME, "effect_xml", effect_xml, path);
	//
	::WritePrivateProfileString(SECTION_NAME, "tool_xml", tool_xml, path);
	//
	::WritePrivateProfileString(SECTION_NAME, "brush_dir", brush_dir, path);
	//
	::WritePrivateProfileString(SECTION_NAME, "brush_xml", brush_xml, path);
	//
	::WritePrivateProfileString(SECTION_NAME, "palette_xml", palette_xml, path);
	//
	::WritePrivateProfileString(SECTION_NAME, "keymap_dat", keymap_dat, path);
	//
	::WritePrivateProfileString(SECTION_NAME, "prm_reg_dat", prm_reg_dat, path);
	//
	wsprintf(buf, "%d", GetNumOfHistory());
	::WritePrivateProfileString(SECTION_NAME, "num_of_history", buf, path);
	//
	::WritePrivateProfileString(SECTION_NAME, "input_revise", input_revise, path);
	//
	wsprintf(buf, "%d", GetToolShiftTime());
	::WritePrivateProfileString(SECTION_NAME, "toolshift_time", buf, path);
	//
	wsprintf(buf, "%d", GetEditWndBGColor().value);
	::WritePrivateProfileString(SECTION_NAME, "bg_color", buf, path);
}