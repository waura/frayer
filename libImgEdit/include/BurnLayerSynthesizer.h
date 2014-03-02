#ifndef _BURNSYNTHESIZER_H_
#define _BURNSYNTHESIZER_H_

#include "ILayerSynthesizer.h"

//èƒÇ´Ç±Ç›ÉåÉCÉÑÅ[çáê¨
class BurnLayerSynthesizer : public ILayerSynthesizer
{
public:
	BurnLayerSynthesizer(){};
	~BurnLayerSynthesizer(){};

	inline void operator()(UCvPixel& over, const UCvPixel& under) const
	{
		if(over.b != 0){
			int val = 255 - (255 * (255 - under.b) / over.b);

			if(val < 0) val = 0;
			over.b = val;
		}
		if(over.g != 0){
			int val = 255 - (255 * (255 - under.g) / over.g);

			if(val < 0) val = 0;
			over.g = val;
		}
		if(over.r != 0){
			int val = 255 - (255 * (255 - under.r) / over.r);

			if(val < 0) val = 0;
			over.r = val;
		}
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

#endif //_BURNSYNTHESIZER_H_