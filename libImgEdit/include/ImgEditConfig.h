#ifndef _IMGEDITCONFIG_H_
#define _IMGEDITCONFIG_H_

#include "IEColor.h"

class ImgEditConfig
{
public:
	ImgEditConfig(){
		edit_wnd_bg_color.value = 0xff000000;
	}
	  ~ImgEditConfig();

	void LoadConfigFile(const char* path);
	void SaveConfigFile(const char* path);

	inline const char* GetEffectXml() const {
		return effect_xml;
	}

	inline const char* GetToolXml() const {
		return tool_xml;
	}

	inline const char* GetBrushDir() const {
		return brush_dir;
	}

	inline const char* GetBrushXml() const {
		return brush_xml;
	}

	inline const char* GetPaletteXml() const {
		return palette_xml;
	}

	inline const char* GetKeymapDat() const {
		return keymap_dat;
	}

	inline const char* GetPrmRegDat() const {
		return prm_reg_dat;
	}

	//
	inline unsigned int GetNumOfHistory() const {
		return num_of_history;
	}

	inline void SetNumOfHistory(unsigned int num){
		num_of_history = num;
	}

	//
	inline const char* GetInputRevise() const {
		return input_revise;
	}

	inline void SetInputRevise(const char* name){
		strcpy_s(input_revise, _MAX_PATH, name);
	}

	//
	inline IEColor GetEditWndBGColor() const {
		return edit_wnd_bg_color;
	}

	inline void SetEditWndBGColor(IEColor bg_color){
		edit_wnd_bg_color = bg_color;
	}

	//
	inline unsigned int GetToolShiftTime() const {
		return tool_shift_time;
	}

	inline void SetToolShiftTime(unsigned int time){
		tool_shift_time = time;
	}

private:
	char effect_xml[_MAX_PATH];
	char tool_xml[_MAX_PATH];
	char brush_dir[_MAX_PATH];
	char brush_xml[_MAX_PATH];
	char palette_xml[_MAX_PATH];
	char keymap_dat[_MAX_PATH];
	char prm_reg_dat[_MAX_PATH];

	unsigned int num_of_history;
	char input_revise[_MAX_PATH];
	IEColor edit_wnd_bg_color;
	unsigned int tool_shift_time;
};

#endif //_IMGEDITCONFIG_H_