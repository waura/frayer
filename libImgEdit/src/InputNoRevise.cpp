#include "stdafx.h"
#include "InputNoRevise.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


InputNoRevise::InputNoRevise()
{
	beforData.x = 0;
	beforData.y = 0;
	beforData.p = 0;
}

InputNoRevise::~InputNoRevise()
{
}

void InputNoRevise::OnBeginDrag(LPIE_INPUT_DATA lpd, ImgEdit *pImgEdit, InputData_Vec &inputData_Vec)
{
	beforData = *lpd;
	inputData_Vec.push_back(*lpd);
}

void InputNoRevise::OnMouseDrag(LPIE_INPUT_DATA lpd, ImgEdit *pImgEdit, InputData_Vec &inputData_Vec)
{
	LineRevise(&beforData, lpd, inputData_Vec);
	beforData = *lpd;
}

void InputNoRevise::OnEndDrag(LPIE_INPUT_DATA lpd, ImgEdit *pImgEdit, InputData_Vec &inputData_Vec)
{
	LineRevise(&beforData, lpd, inputData_Vec);
}
