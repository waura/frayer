#ifndef _SUBMASKSYNTHESIZER_H_
#define _SUBMASKSYNTHESIZER_H_

#include "IMaskSynthesizer.h"

class SubMaskSynthesizer : public IMaskSynthesizer
{
public:
	SubMaskSynthesizer(){};
	~SubMaskSynthesizer(){};

	inline void operator()(uint8_t& over, const uint8_t under)
	{
		int val = under - over;
		if(val < 0) val = 0;
		over = val;
	}
};

#endif //_SUBMASKSYNTHESIZER_H_