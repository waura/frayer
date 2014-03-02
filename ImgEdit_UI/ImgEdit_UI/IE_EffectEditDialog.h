#ifndef _IE_EFFECTEDITDIALOG_H_
#define _IE_EFFECTEDITDIALOG_H_

#include <vector>
#include "IEffectEditDialog.h"

#define PROP_EED_WINPROC "EditWindowProc" //クラスを識別する文字列

typedef struct _TRACKBAR_DATA{
	int id;
	HWND hWnd;
	HWND hLabel;
	HWND hValLabel;
	ImgFile_Ptr file;
	int *change_val;
}TRACKBAR_DATA;

typedef std::vector<TRACKBAR_DATA*> TRACKBAR_DATA_VEC;


class IE_EffectEditDialog : public IEffectEditDialog
{
public:
	IE_EffectEditDialog();
	~IE_EffectEditDialog();

	//////////////////////////////////
	/*!
		ダイアログの表示
		@param[in] hInst
		@param[in] hwndParent
	*/
	INT_PTR CreateDialogBox(HINSTANCE hInst, HWND hwndParent, ImgFile_Ptr file, IEffect* effect);

	//////////////////////////////////
	/*!
		トラックバーの追加
		@param[in] name トラックバーの名前
		@param[in] min 最小値
		@param[in] max 最大値
		@param[in] start 開始時の値
		@param[in] change_val 値を変更する変数
	*/
	VOID AddTrackbar(const char* name, int min, int max, int start, int *change_val);

	VOID SetProgRange(int min, int max);

	VOID SetProgStep(int step);
	VOID ProgStepIt();

	VOID SetProgPos(int pos);

	//////////////////////////////////
	/*!
		ダイアログのプロシージャ
	*/
	static INT_PTR CALLBACK DialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

private:
	//////////////////////////////////
	/*!
	*/
	VOID SetHWND(HWND hWnd);

	VOID SetDialogToEffect();

	VOID EffectEdit();

	//////////////////////////////////
	/*!
		トラックバーのイベント時
		@param[in] hTrac トラックバーのハンドル
		@param[in] wp
		@param[in] val トラックバーの値
	*/
	VOID CallTrackbar(HWND hTrack, WPARAM wp, int val);

	//////////////////////////////////
	/*!
	*/
	VOID ClearDialog(); 


	HWND m_hWnd;
	HINSTANCE m_hInst;

	ImgFile_Ptr m_pEditFile;
	IEffect *m_pEditEffect;

	int m_Ctrl_Num;
	int m_IdIndex;

	TRACKBAR_DATA_VEC m_TrackBar_DataVec;

};

#endif