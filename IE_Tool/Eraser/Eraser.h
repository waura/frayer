#ifndef _ERASER_H_
#define _ERASER_H_

#include "IBrushDraw.h"

class Eraser : public IBrushDraw
{
public:
	Eraser();
	~Eraser();


	HBITMAP GetButtonImg();

	void OnSetToolEditWnd(IToolEditWndHandle* pHandle);

	void OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd);
	void OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd);
	void OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd);

private:
	
	void Erase(const LPIE_INPUT_DATA lpd);

	int m_eraser_alpha;

	ImgLayer_Ptr m_pEditLayer;
	EditNode* m_editNode;
	EditLayerHandle* m_pEditLayerHandle;
};


class EraserBltOp
{
public:

	//inline void SetColor(const UCvPixel* color){
	//	m_opacity = 1.0;
	//}

	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(mask == NULL);

		UCvPixel* pdst = (UCvPixel*) dst;
		uint8_t* psrc = (uint8_t*) src;

		int i;
		for(i=0; i<width; i++){
			pdst->a = (*psrc);

			pdst++;
			psrc++;
		}
	}

private:
	//double m_opacity;
};

class EraserCopyOp
{
public:

	inline void operator()(void* dst, uint8_t pix) const
	{
		UCvPixel* pdst = (UCvPixel*) dst;

		if (pix == 0) return;

		if (pdst->a == 0) {
			pdst->a = pix;
		} else if (pdst->a == 255) {
			//
		} else {
			uint8_t ra = ~pix;
			uint8_t alpha = 255 - (ra*(255 - pdst->a)>>8);
			pdst->a = alpha;
		}

		//if (pdst->a < pix) {
		//	pdst->value = m_color.value;
		//	pdst->a = pix;
		//}
	}

	inline void operator()(void* dst, __m128i mmpix16) const
	{
		//UCvPixel* pdst = (UCvPixel*) dst;

		//if (pix == 0) return;

		////int a;
		////if (pdst->a == 255) {
		////	a = 255;
		////}
		////else if (pdst->a < 255) {
		////	a = pdst->a + pix - ((pdst->a * pix) >> 8);
		////}
		//if (pdst->a < pix) {
		//	pdst->value = m_color.value;
		//	pdst->a = pix;
		//}
	}

private:
	//double m_opacity;
};

#endif //_ERASER_H_