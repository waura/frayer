#ifndef _DIFFERENCELAYERSYNTHESIZER_H_
#define _DIFFERENCELAYERSYNTHESIZER_H_

#include "ILayerSynthesizer.h"


//·‚Ìâ‘Î’l
class DifferenceLayerSynthesizer : public ILayerSynthesizer
{
public:
	DifferenceLayerSynthesizer(){};
	~DifferenceLayerSynthesizer(){};

	inline void operator()(UCvPixel& over, const UCvPixel& under) const
	{
		over.b = abs(under.b - over.b);
		over.g = abs(under.g - over.g);
		over.r = abs(under.r - over.r);
	}

	inline __m128i operator()(__m128i xmo16, const __m128i xmu16) const
	{
		UCvPixel over0, under0;
		UCvPixel over1, under1;

		__m128i xmzero = _mm_setzero_si128();

		over0.b = _mm_extract_epi16(xmo16, 0);
		over0.g = _mm_extract_epi16(xmo16, 1);
		over0.r = _mm_extract_epi16(xmo16, 2);
		over0.a = _mm_extract_epi16(xmo16, 3);
		under0.b = _mm_extract_epi16(xmu16, 0);
		under0.g = _mm_extract_epi16(xmu16, 1);
		under0.r = _mm_extract_epi16(xmu16, 2);
		under0.a = _mm_extract_epi16(xmu16, 3);
		this->operator()(over0, under0);

		over1.b = _mm_extract_epi16(xmo16, 4);
		over1.g = _mm_extract_epi16(xmo16, 5);
		over1.r = _mm_extract_epi16(xmo16, 6);
		over1.a = _mm_extract_epi16(xmo16, 7);
		under1.b = _mm_extract_epi16(xmu16, 4);
		under1.g = _mm_extract_epi16(xmu16, 5);
		under1.r = _mm_extract_epi16(xmu16, 6);
		under1.a = _mm_extract_epi16(xmu16, 7);
		this->operator()(over1, under1);

		return _mm_set_epi16(over1.a, over1.r, over1.g, over1.b,
							over0.a, over0.r, over0.g, over0.b);
	}
};

#endif //_DIFFERENCELAYERSYNTHESIZER_H_