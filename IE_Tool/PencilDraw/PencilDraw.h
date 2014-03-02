#ifndef _PENCILDRAW_H_
#define _PENCILDRAW_H_

#include "IBrushDraw.h"


//‰”•Mƒc[ƒ‹
class PencilDraw : public IBrushDraw
{
public:
	PencilDraw();
	~PencilDraw();

	HBITMAP GetButtonImg();

	void OnSetToolEditWnd(IToolEditWndHandle* pHandle);

	void OnSelect();
	void OnNeutral();

	void OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd );
	void OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd );
	void OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd );
private:

	void DrawPencil(LPIE_INPUT_DATA lpd);

	int m_brush_alpha;

	ImgLayer_Ptr m_pEditLayer;
	EditNode* m_editNode;
	EditLayerHandle* m_pEditLayerHandle;
};

class PencilDrawBltOp
{
public:

	inline void SetColor(const UCvPixel* color){
		m_color = (*color);
		//m_opacity = color->a / 255.0;
		m_color.a = 255;
		m_opacity = 1.0;
	}

	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		UCvPixel* pdst = (UCvPixel*) dst;
		uint8_t* psrc = (uint8_t*) src;

		int i;
		uint8_t brush_alpha;
		for(i=0; i<width; i++){
			brush_alpha = (*psrc);
			if(brush_alpha == 0) goto next_loop;
			if((brush_alpha != 255) && (m_color.a != 255))
				brush_alpha = brush_alpha * m_opacity;

			pdst->value = m_color.value;

		next_loop:
			pdst++;
			psrc++;
		}
	}

private:
	double m_opacity;
	UCvPixel m_color;
};

class PencilDrawCopyOp
{
public:

	inline void SetColor(const UCvPixel* color){
		m_color = (*color);
	}

	inline void operator()(void* dst, uint8_t pix) const
	{
		UCvPixel* pdst = (UCvPixel*) dst;

		if (pix == 0) return;

		pdst->value = m_color.value;
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
	UCvPixel m_color;
};

#endif //_PENCILDRAW_H_