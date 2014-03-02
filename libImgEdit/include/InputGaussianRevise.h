#ifndef _INPUTGAUSSIANREVISE_H_
#define _INPUTGAUSSIANREVISE_H_

#include "IInputRevise.h"

class InputGaussianRevise : public IInputRevise
{
public:
	InputGaussianRevise(int N, double sigma);
	~InputGaussianRevise();

	//////////////////////////////
	/*!
		入力開始
		@param[in] lpd 入力データ
		@param[out] inputData_Vec 出力座標配列
	*/
	void OnBeginDrag(LPIE_INPUT_DATA lpd, ImgEdit *pImgEdit, InputData_Vec &inputData_Vec);

	//////////////////////////////
	/*!
		入力
		@param[in] lpd 入力データ
		@param[out] inputData_Vec 出力座標配列
	*/
	void OnMouseDrag(LPIE_INPUT_DATA lpd, ImgEdit *pImgEdit, InputData_Vec &inputData_Vec);

	//////////////////////////////
	/*!
		入力終了
		@param[in] lpd 入力データ
		@param[out] inputData_Vec 出力座標配列
	*/
	void OnEndDrag(LPIE_INPUT_DATA lpd, ImgEdit *pImgEdit, InputData_Vec &inputData_Vec);

private:

	void InitInputData(LPIE_INPUT_DATA lpd);
	void AddInputData(LPIE_INPUT_DATA lpd);
	void GetAverageData(LPIE_INPUT_DATA lpd){
		*lpd = m_avrgData;
	}
	void GetBeforeData(LPIE_INPUT_DATA lpd){
		*lpd = m_beforeData;
	}

	int m_N;
	double m_sigma;
	unsigned int m_bufferIndex;
	unsigned int m_indexMask;
	IE_INPUT_DATA m_avrgData;
	IE_INPUT_DATA m_beforeData;
	IE_INPUT_DATA* m_inputData; //!< buffer
};

#endif //_INPUTGAUSSIANREVISE_H_