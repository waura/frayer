#ifndef _ITOOLEDITWNDHANDLE_H_
#define _ITOOLEDITWNDHANDLE_H_

#include "LibIEDllHeader.h"

class _EXPORTCPP IToolEditWndHandle
{
public:
	IToolEditWndHandle(){};
	virtual ~IToolEditWndHandle(){};

	virtual VOID ClearAllCtrl(){};

	//////////////////////////////////
	/*!
		トラックバーの追加
		@param[in] name トラックバーの名前
		@param[in] min 最小値
		@param[in] max 最大値
		@param[in] start 開始時の値
		@param[in,out] change_val 値を変更する変数
	*/
	virtual VOID AddTrackbar(const char* name, int min, int max, int start, int *change_val){};

	//////////////////////////////////
	/*!
		レイヤー合成コンボボックスの追加
		@param[in] name トラックバーの名前
		@param[in] start_index 開始時のインデックス
		@param[in,out] change_val 値を変更する変数
	*/
	virtual VOID AddLayerDrawModeComboBox(const char* name, int start_index, int* change_val){};
};

#endif //ITOOLEDITWNDHANDLE_H_