#include "stdafx.h"
#include "InputGaussianRevise.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


InputGaussianRevise::InputGaussianRevise(int N, double sigma)
{
	assert(N > 1);
	assert(!(N & (N-1))); //is power of 2?

	m_N = N;
	m_sigma = sigma;
	m_indexMask = ~(N | (-N));
	m_inputData = new IE_INPUT_DATA[N];
}

InputGaussianRevise::~InputGaussianRevise()
{
	if(m_inputData){
		delete[] m_inputData;
		m_inputData = NULL;
	}
}

//////////////////////////////
/*!
	入力開始
	@param[in] lpd 入力データ
	@param[out] inputData_Vec 出力座標配列
*/
void InputGaussianRevise::OnBeginDrag(LPIE_INPUT_DATA lpd, ImgEdit *pImgEdit, InputData_Vec &inputData_Vec)
{
	InitInputData(lpd);

	IE_INPUT_DATA idata;
	GetAverageData(&idata);
	inputData_Vec.push_back(idata);
}

//////////////////////////////
/*!
	入力
	@param[in] lpd 入力データ
	@param[out] inputData_Vec 出力座標配列
*/
void InputGaussianRevise::OnMouseDrag(LPIE_INPUT_DATA lpd, ImgEdit *pImgEdit, InputData_Vec &inputData_Vec)
{
	AddInputData(lpd);

	IE_INPUT_DATA start;
	IE_INPUT_DATA end;
	GetAverageData(&end);
	GetBeforeData(&start);
	LineRevise(&start, &end, inputData_Vec);
}

//////////////////////////////
/*!
	入力終了
	@param[in] lpd 入力データ
	@param[out] inputData_Vec 出力座標配列
*/
void InputGaussianRevise::OnEndDrag(LPIE_INPUT_DATA lpd, ImgEdit *pImgEdit, InputData_Vec &inputData_Vec)
{
	AddInputData(lpd);

	IE_INPUT_DATA start;
	IE_INPUT_DATA end;
	GetAverageData(&end);
	GetBeforeData(&start);
	LineRevise(&start, &end, inputData_Vec);
	LineRevise(&end, lpd, inputData_Vec);
}

void InputGaussianRevise::InitInputData(LPIE_INPUT_DATA lpd)
{
	int i;
	for(i=0; i<m_N; i++){
		*(m_inputData + i) = *lpd;
	}
	m_avrgData = (*lpd);
	m_bufferIndex = m_N;
}

static double Gaussian(double x, double sigma){
	// 1/((sqrt(2 * pi) * sigma) * exp(- x^2 / (2 * sigma^2))
	double inv_sigma = (1.0 / sigma);
	return (0.225079079) * inv_sigma * exp(-0.5 * x * x * inv_sigma * inv_sigma);
}

void InputGaussianRevise::AddInputData(LPIE_INPUT_DATA lpd)
{
	m_beforeData = m_avrgData;

	m_bufferIndex++;
	*(m_inputData + (m_bufferIndex & m_indexMask)) = (*lpd);

	unsigned int index = m_bufferIndex;
	m_avrgData.x = 0.0;
	m_avrgData.y = 0.0;
	m_avrgData.p = 0.0;
	m_avrgData.azimuth_rad = 0.0;
	m_avrgData.altitude_rad = 0.0;
	m_avrgData.random = 0.0;

	int i;
	double g;
	double sum_g = 0.0;
	IE_INPUT_DATA data;
	for(i=0; i<m_N; i++){
		g = Gaussian(i, m_sigma);
		sum_g += g;
		data = *(m_inputData + (m_bufferIndex & m_indexMask));
		m_avrgData.x += data.x * g;
		m_avrgData.y += data.y * g;
		m_avrgData.p += data.p * g;
		m_avrgData.azimuth_rad += data.azimuth_rad * g;
		m_avrgData.altitude_rad += data.altitude_rad * g;
		m_avrgData.random += data.random * g;
	}
	m_avrgData.x /= sum_g;
	m_avrgData.y /= sum_g;
	m_avrgData.p /= sum_g;
	m_avrgData.azimuth_rad /= sum_g;
	m_avrgData.altitude_rad /= sum_g;
	m_avrgData.random /= sum_g;
}