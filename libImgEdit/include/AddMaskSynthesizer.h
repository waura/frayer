#ifndef _ADDMASKSYNTHESIZER_H_
#define _ADDMASKSYNTHESIZER_H_

#include "IMaskSynthesizer.h"

class AddMaskSynthesizer : public IMaskSynthesizer
{
public:
	AddMaskSynthesizer(){};
	~AddMaskSynthesizer(){};

	inline void operator()(uint8_t& over, const uint8_t under)
	{
		int val = under + over;
		if(val > 255) val = 255;
		over = val;
	}
};

#endif //_ADDMASKSYNTHESIZER_H_