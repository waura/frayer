#ifndef _OVERLAYLAYERSYNTHESIZER_H_
#define _OVERLAYLAYERSYNTHESIZER_H_

#include "ILayerSynthesizer.h"


//オーバーレイレイヤー合成
class OverlayLayerSynthesizer : public ILayerSynthesizer
{
public:
	OverlayLayerSynthesizer(){};
	~OverlayLayerSynthesizer(){};

	inline void operator()(UCvPixel& over, const UCvPixel& under) const
	{
		int val;

		//
		if(under.b < 128)
			val = 2 * over.b * under.b / 255;
		else
			val = 2 * (under.b + over.b - under.b * over.b / 255) - 255;
		if(val > 255) val = 255;
		over.b = val;

		//
		if(under.g < 128)
			val = 2 * over.g * under.g / 255;
		else
			val = 2 * (under.g + over.g - under.g * over.g / 255) - 255;
		if(val > 255) val = 255;
		over.g = val;

		//
		if(under.r < 128)
			over.r = 2 * over.r * under.r / 255;
		else
			val = 2 * (under.r + over.r - under.r * over.r / 255) - 255;
		if(val > 255) val = 255;
		over.r = val;
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

#endif //_OVERLAYLAYERSYNTHESIZER_H_