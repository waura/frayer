#ifndef _COPYMASKSYNTHESIZER_H_
#define _COPYMASKSYNTHESIZER_H_

#include "IMaskSynthesizer.h"

class CopyMaskSynthesizer : public IMaskSynthesizer
{
public:
	CopyMaskSynthesizer(){};
	~CopyMaskSynthesizer(){};

	inline void operator()(uint8_t& over, const uint8_t under)
	{
	}
};

#endif //_COPYMASKSYNTHESIZER_H_