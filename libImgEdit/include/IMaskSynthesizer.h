#ifndef _IMASKSYNTHESIZER_H_
#define _IMASKSYNTHESIZER_H_

#include "LibIEDllHeader.h"

class _EXPORTCPP IMaskSynthesizer
{
public:
	IMaskSynthesizer(){};
	virtual ~IMaskSynthesizer(){};
};

template<class _COp>
class LineMaskBlender
{
public:
	LineMaskBlender(){};
	~LineMaskBlender(){};

	inline void operator()(void* dst, const void* under, const void* over, const void* mask, int width) const
	{
		if(over == NULL)
			return;

		uint8_t* pdst = (uint8_t*) dst;
		uint8_t* punder = (uint8_t*) under;
		uint8_t* pover = (uint8_t*) over;

		uint8_t dst_pix;
		uint8_t over_pix;
		uint8_t under_pix;

		_COp Op;

		int i;
		for(i=0; i<width; i++){
			under_pix = (*punder);
			over_pix = (*pover);

			Op(over_pix, under_pix);

			(*pdst) = over_pix;

			pdst++;
			punder++;
			pover++;
		}

	}
};

#endif //_IMASKSYNTHESIZER_H_