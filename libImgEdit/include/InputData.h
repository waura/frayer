#ifndef _IE_INPUT_DATA_H_
#define _IE_INPUT_DATA_H_

#include <vector>

typedef struct _IE_INPUT_DATA {
	double x;
	double y;
	double p; //•Mˆ³[0.0, 1.0]
	double azimuth_rad; //[0, 2pi]
	double altitude_rad; //[-pi, pi]
	double random; //[0, 1.0]
} IE_INPUT_DATA, *LPIE_INPUT_DATA;

typedef std::vector<IE_INPUT_DATA> InputData_Vec;


#endif