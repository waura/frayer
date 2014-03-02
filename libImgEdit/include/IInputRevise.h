#ifndef _IINPUTREVISE_H_
#define _IINPUTREVISE_H_

#include "InputData.h"
#include "ImgEdit.h"

#define IIR_MIN_STEP_SIZE 0.1

class IInputRevise
{
public:
	IInputRevise():
	  m_stepSize(1.0){};
	virtual ~IInputRevise(){};

	void SetStepSize(double size){
		m_stepSize = (size > IIR_MIN_STEP_SIZE) ? size : IIR_MIN_STEP_SIZE;
	}

	//////////////////////////////
	/*!
		入力開始
		@param[in] lpd 入力データ
		@param[in] pImgEdit
		@param[out] inputData_Vec 出力座標配列
	*/
	virtual void OnBeginDrag(LPIE_INPUT_DATA lpd, ImgEdit *pImgEdit, InputData_Vec &inputData_Vec) = 0;
	
	//////////////////////////////
	/*!
		入力
		@param[in] lpd 入力データ
		@param[in] pImgEdit
		@param[out] inputData_Vec 出力座標配列
	*/
	virtual void OnMouseDrag(LPIE_INPUT_DATA lpd, ImgEdit *pImgEdit, InputData_Vec &inputData_Vec) = 0;
	
	//////////////////////////////
	/*!
		入力終了
		@param[in] lpd 入力データ
		@param[in] pImgEdit
		@param[out] inputData_Vec 出力座標配列
	*/
	virtual void OnEndDrag(LPIE_INPUT_DATA lpd, ImgEdit *pImgEdit, InputData_Vec &inputData_Vec) = 0;

protected:
	void LineRevise(LPIE_INPUT_DATA startPt, LPIE_INPUT_DATA endPt, InputData_Vec &inputData_Vec);

	double m_stepSize;
};

#endif //_IINPUTREVISE_H_