#ifndef _NORMALBRUSHDRAW_H_
#define _NORMALBRUSHDRAW_H_

#include <libImgEdit.h>
#include "IBrushDraw.h"

//ブラシツール
class NormalBrushDraw : public IBrushDraw
{
public:
	NormalBrushDraw();
	~NormalBrushDraw();

	HBITMAP GetButtonImg();

	void OnSetToolEditWnd(IToolEditWndHandle* pHandle);

	void OnSelect();
	void OnNeutral();

	void OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd) ;
	void OnMouseLDrag(UINT nFlags,  const LPIE_INPUT_DATA lpd) ;
	void OnMouseLButtonUp(UINT nFlags,  const LPIE_INPUT_DATA lpd) ;
private:

	void DrawBrush(LPIE_INPUT_DATA lpd);

	int m_brush_alpha;
	int m_brend_index;

	ImgLayer_Ptr m_pEditLayer;
	IplImage* m_brush_alpha_map;
	EditNode* m_editNode;
	EditLayerHandle* m_pEditLayerHandle;
};

class NormalBrushBltOp
{
public:

	inline void SetColor(const UCvPixel* color){
		m_color = (*color);
		//m_opacity = color->a / 255.0;
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
			pdst->a = brush_alpha;

		next_loop:
			pdst++;
			psrc++;
		}
	}

private:
	double m_opacity;
	UCvPixel m_color;
};

class NormalBrushCopyOp
{
public:

	inline void SetColor(const UCvPixel* color){
		m_color = (*color);
	}

	inline void operator()(void* dst, uint8_t pix) const
	{
		UCvPixel* pdst = (UCvPixel*) dst;

		if (pix == 0) return;

		if (pdst->a == 0) {
			pdst->value = m_color.value;
			pdst->a = pix;
		} else if (pdst->a == 255) {
			pdst->value = m_color.value;
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
	UCvPixel m_color;
};

#endif //_NORMALBRUSHDRAW_H_