#ifndef _SOFTLIGHTLAYERSYNTHESIZER_H_
#define _SOFTLIGHTLAYERSYNTHESIZER_H_

#include "ILayerSynthesizer.h"


//ソフトライトレイヤー合成
class SoftlightLayerSynthesizer : public ILayerSynthesizer
{
public:
	SoftlightLayerSynthesizer(){};
	~SoftlightLayerSynthesizer(){};

	inline void operator()(UCvPixel& over, const UCvPixel& under) const
	{
		if(over.b < 128) over.b = pow((under.b / 255.0), ((255.0 - over.b)/128.0)) * 255;
		else over.b = pow((under.b / 255.0), (128.0 / over.b)) * 255;

		if(over.g < 128) over.g = pow((under.g / 255.0), ((255.0 - over.g)/128.0)) * 255;
		else over.g = pow((under.g / 255.0), (128.0 / over.g)) * 255;

		if(over.r < 128) over.r = pow((under.r / 255.0), ((255.0 - over.r)/128.0)) * 255;
		else over.r = pow((under.r / 255.0), (128.0 / over.r)) * 255;
	}

	inline __m128i operator()(__m128i xmo16, const __m128i xmu16)
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


#endif //_SOFTLIGHTLAYERSYNTHESIZER_H_