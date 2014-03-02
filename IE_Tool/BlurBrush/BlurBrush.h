#ifndef _BLUR_BRUSH_H_
#define _BLUR_BRUSH_H_

#include "IBrushDraw.h"

class BlurBrush : public IBrushDraw
{
public:
	BlurBrush();
	~BlurBrush();

	HBITMAP GetButtonImg();

	void OnSetToolEditWnd(IToolEditWndHandle* pHandle);

	void OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd);
	void OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd);
	void OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd);

private:
	void Blur(const LPIE_INPUT_DATA lpd);

	IplImage* m_blur_alpha_map;

	ImgLayer_Ptr m_pEditLayer;
	EditNode* m_pEditNode;
	EditLayerHandle* m_pEditLayerHandle;
};

#endif //_BLUR_BRUSH_H_