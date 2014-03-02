#include "stdafx.h"
#include "IInputRevise.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


void IInputRevise::LineRevise(LPIE_INPUT_DATA startPt, LPIE_INPUT_DATA endPt, InputData_Vec &inputData_Vec)
{
	double length = sqrt((endPt->x - startPt->x) * (endPt->x - startPt->x) +
					(endPt->y - startPt->y) * (endPt->y - startPt->y));

	double add = m_stepSize/length;
	if (add < FLT_EPSILON) return;

	double t=0.0;
	IE_INPUT_DATA data;
	while(t < 1.0){
		data.x = startPt->x + (endPt->x - startPt->x)*t;
		data.y = startPt->y + (endPt->y - startPt->y)*t;
		data.p = startPt->p + (endPt->p - startPt->p)*t;
		data.azimuth_rad = startPt->azimuth_rad + (endPt->azimuth_rad - startPt->azimuth_rad)*t;
		data.altitude_rad = startPt->altitude_rad + (endPt->altitude_rad - startPt->altitude_rad)*t;
		data.random = startPt->random + (endPt->random - startPt->random)*t;

		inputData_Vec.push_back(data);
		t += add;
	}

}