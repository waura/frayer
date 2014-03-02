#include "stdafx.h"
#include "InputMovingAverageRevise.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


InputMovingAverageRevise::InputMovingAverageRevise(int N)
{
	assert(N > 1);
	assert(!(N & (N-1))); //is power of 2?
	
	m_N = N;
	m_indexMask = ~(N | (-N));
	m_inputData = new IE_INPUT_DATA[N];
}

InputMovingAverageRevise::~InputMovingAverageRevise()
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
void InputMovingAverageRevise::OnBeginDrag(LPIE_INPUT_DATA lpd, ImgEdit *pImgEdit, InputData_Vec &inputData_Vec)
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
void InputMovingAverageRevise::OnMouseDrag(LPIE_INPUT_DATA lpd, ImgEdit *pImgEdit, InputData_Vec &inputData_Vec)
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
void InputMovingAverageRevise::OnEndDrag(LPIE_INPUT_DATA lpd, ImgEdit *pImgEdit, InputData_Vec &inputData_Vec)
{
	AddInputData(lpd);

	IE_INPUT_DATA start;
	IE_INPUT_DATA end;
	GetAverageData(&end);
	GetBeforeData(&start);
	LineRevise(&start, &end, inputData_Vec);
	LineRevise(&end, lpd, inputData_Vec);
}

void InputMovingAverageRevise::InitInputData(LPIE_INPUT_DATA lpd)
{
	int i;
	for(i=0; i<m_N; i++){
		m_inputData[i] = *lpd;
	}
	m_avrgData = (*lpd);
	m_bufferIndex = m_N;
}

void InputMovingAverageRevise::AddInputData(LPIE_INPUT_DATA lpd)
{
	m_beforeData = m_avrgData;

	m_bufferIndex++;
	IE_INPUT_DATA delete_val = *(m_inputData + (m_bufferIndex & m_indexMask));
	m_avrgData.x += (lpd->x - delete_val.x)/m_N;
	m_avrgData.y += (lpd->y - delete_val.y)/m_N;
	m_avrgData.p += (lpd->p - delete_val.p)/m_N;
	m_avrgData.azimuth_rad += (lpd->azimuth_rad - delete_val.azimuth_rad)/m_N;
	m_avrgData.altitude_rad += (lpd->altitude_rad - delete_val.altitude_rad)/m_N;
	m_avrgData.random += (lpd->random - delete_val.random)/m_N;
	*(m_inputData + (m_bufferIndex & m_indexMask)) = (*lpd);
}