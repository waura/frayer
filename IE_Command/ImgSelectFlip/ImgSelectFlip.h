#ifndef _IMGSELECTFLIP_H_
#define _IMGSELECTFLIP_H_

#include <libImgEdit.h>


class ImgSelectFlip : public IIECommand {
public:
	ImgSelectFlip();
	~ImgSelectFlip(){};

	//
	BOOL Run(ImgEdit* pEdit, void* pvoid);
};

class FlipBltOp
{
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst != NULL);
		assert(mask == NULL);

		uint8_t* pdst = (uint8_t*) dst;
		uint8_t* psrc = (uint8_t*) src;

		int i;
		if(src == NULL){
			for(i=0; i<width; i++){
				(*pdst++) = 255;
			}
			return;
		}

		for(i=0; i<width; i++){
			(*pdst++) = 255 - (*psrc++);
		}
	}
};

#endif //_IMGSELECTFLIP_H_