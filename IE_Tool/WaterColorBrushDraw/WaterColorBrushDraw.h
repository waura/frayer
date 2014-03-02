#ifndef _WATERCOLORBRUSHDRAW_H_
#define _WATERCOLORBRUSHDRAW_H_

#include "IBrushDraw.h"


//…Êƒuƒ‰ƒV
class WaterColorBrushDraw : public IBrushDraw
{
public:
	WaterColorBrushDraw();
	~WaterColorBrushDraw();

	HBITMAP GetButtonImg();

	void OnSetToolEditWnd(IToolEditWndHandle* pHandle);

	void OnSelect();
	void OnNeutral();

	void OnMouseLButtonDown(UINT nFlags,  const LPIE_INPUT_DATA lpd );
	void OnMouseLDrag(UINT nFlags,  const LPIE_INPUT_DATA lpd );
	void OnMouseLButtonUp(UINT nFlags,  const LPIE_INPUT_DATA lpd );
private:

	void DrawWaterColor(LPIE_INPUT_DATA lpd);
	void ColorMix(UCvPixel* dst, const UCvPixel* src, double src_rate);
	
	int m_org_color_rate;
	int m_delay_color_rate;
	int m_canvas_color_rate;

	ImgLayer_Ptr m_pEditLayer;
	UCvPixel m_nowColor;
	UCvPixel m_orgColor;
	UCvPixel m_delayColor;
	EditNode* m_editNode;
	EditLayerHandle* m_pEditLayerHandle;
};

class WaterColorBrushBltOp
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
		UCvPixel tmp_color = m_color;
		for(i=0; i<width; i++){
			brush_alpha = (*psrc);
			if(brush_alpha == 0) goto next_loop;
			if((brush_alpha != 255) && (m_color.a != 255))
				brush_alpha = brush_alpha * m_opacity;

			tmp_color.a = brush_alpha;
			AlphaBlendPixel(pdst, &tmp_color, 255, pdst);

		next_loop:
			pdst++;
			psrc++;
		}
	}

private:
	double m_opacity;
	UCvPixel m_color;
};

class WaterColorBrushCopyOp
{
public:

	inline void SetColor(const UCvPixel* color){
		m_color = (*color);
	}

	inline void operator()(void* dst, uint8_t pix) const
	{
		UCvPixel* pdst = (UCvPixel*) dst;

		if (pix == 0) return;

		UCvPixel tmp_color = m_color;
		tmp_color.a = pix;
		AlphaBlendPixel(pdst, &tmp_color, 255, pdst);
	}

	inline void operator()(void* dst, __m128i mmpix16) const
	{

	}

private:
	UCvPixel m_color;
};

#endif //_WATERCOLORBRUSHDRAW_H_