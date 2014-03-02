#pragma once

#include "LibIEDllHeader.h"


class IEffect;

class _EXPORTCPP IEffectEditDialog
{
public:
	IEffectEditDialog(){};
	virtual ~IEffectEditDialog(){};

	//////////////////////////////////
	/*!
		ダイアログの表示
		@param[in] hInst
		@param[in] hwndParent
		@param[in] file
		@param[in] effect
	*/
	virtual INT_PTR CreateDialogBox(HINSTANCE hInst, HWND hwndParent, ImgFile_Ptr file, IEffect* effect) =0;

	//////////////////////////////////
	/*!
		トラックバーの追加
		@param[in] name トラックバーの名前
		@param[in] min 最小値
		@param[in] max 最大値
		@param[in] start 開始時の値
		@param[in] change_val 値を変更する変数
	*/
	virtual VOID AddTrackbar(const char* name, int min, int max, int start, int *change_val) =0;

	///////////////////////////////////
	/*!
		プログレスバーの範囲指定
	*/
	virtual VOID SetProgRange(int min, int max){};

	virtual VOID SetProgStep(int step){};
	virtual VOID ProgStepIt(){};
	virtual VOID SetProgPos(int pos){};
};
