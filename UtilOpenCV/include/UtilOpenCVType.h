#ifndef _UTILOPENCVTYPE_H_
#define _UTILOPENCVTYPE_H_

#include "LibDllHeader.h"
#include "stdafx.h"

#include <stdint.h>
//typedef unsigned char uint8_t;
//typedef unsigned char int8_t;
//typedef unsigned short uint16_t;
//typedef short int16_t;
//typedef unsigned long uint32_t;
//typedef long int32_t;

union UCvBGR24
{
	struct
	{
		uint8_t b,g,r;
	};
};
typedef UCvBGR24 UCvBGR;

union UCvPixel32
{
	uint32_t value;
	struct
	{
		uint8_t b,g,r,a;
	};
};
typedef UCvPixel32 UCvPixel;

_EXPORT inline UCvPixel ucvPixel(uint8_t val0, uint8_t val1=0, uint8_t val2=0, uint8_t val3=0)
{
    UCvPixel pixel;
    pixel.b = val0; pixel.g = val1;
    pixel.r = val2; pixel.a = val3;
    return pixel;
}

#endif //_UITLOPENCVTYPE_H_