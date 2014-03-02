#ifndef _INPUTNOREVISE_H_
#define _INPUTNOREVISE_H_

#include "IInputRevise.h"

class InputNoRevise : public IInputRevise
{
public:
	InputNoRevise();
	~InputNoRevise();

	//////////////////////////////
	/*!
		入力開始
		@param[in] lpd 入力データ
		@param[in] pImgEdit
		@param[out] inputData_Vec 出力座標配列
	*/
	void OnBeginDrag(LPIE_INPUT_DATA lpd, ImgEdit *pImgEdit, InputData_Vec &inputData_Vec);

	//////////////////////////////
	/*!
		入力
		@param[in] lpd 入力データ
		@param[in] pImgEdit
		@param[out] inputData_Vec 出力座標配列
	*/
	void OnMouseDrag(LPIE_INPUT_DATA lpd, ImgEdit *pImgEdit, InputData_Vec &inputData_Vec);

	//////////////////////////////
	/*!
		入力終了
		@param[in] lpd 入力データ
		@param[in] pImgEdit
		@param[out] inputData_Vec 出力座標配列
	*/
	void OnEndDrag(LPIE_INPUT_DATA lpd, ImgEdit *pImgEdit, InputData_Vec &inputData_Vec);

private:

	IE_INPUT_DATA beforData;
};

#endif //_INPUTNOREVISE_H_