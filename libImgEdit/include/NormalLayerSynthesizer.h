#ifndef _NORMALLAYERSYNTHESIZER_H_
#define _NORMALLAYERSYNTHESIZER_H_

#include "ILayerSynthesizer.h"

class NormalLayerSynthesizer :
	public ILayerSynthesizer
{
public:
	NormalLayerSynthesizer(){};
	~NormalLayerSynthesizer(){};

	inline void operator()(UCvPixel& over, const UCvPixel& under) const
	{
	}

	inline __m128i operator()(__m128i xmo16, const __m128i xmu16) const
	{
		return xmo16;
	}
};

#endif //_NORMALLAYERSYNTHESIZER_H_