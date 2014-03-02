#ifndef _PENSELECTER_H_
#define _PENSELECTER_H_


#include "IImgSelecter.h"


//ペン選択
class PenSelecter : public IImgSelecter 
{
public:
	PenSelecter();
	~PenSelecter();

	//マウス座標補正有り
	bool isRevise(){
		return true;
	}

	//補間点の間隔
	double GetReviseStepSize(const LPIE_INPUT_DATA lpd){
		if(m_pImgEdit){
			IEBrush_Ptr pBrush = m_pImgEdit->GetSelectBrush();
			if(pBrush){
				return pBrush->GetStepSize(lpd);
			}
		}

		return 1.0;
	}

	//ボタン画像
	HBITMAP GetButtonImg();

	void OnSelect();
	void OnNeutral();

	void OnSetToolEditWnd(IToolEditWndHandle* pHandle);

	void OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd);
	void OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd);
	void OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd);

private:

	void Select(LPIE_INPUT_DATA lpd);

	int m_brush_alpha;
	IplImage* m_brush_alpha_map;

	ImgMask_Ptr m_pEditMask;

	EditNode* m_editNode;
	EditMaskHandle* m_pEditMaskHandle;
};

class PenSelecterCopyOp
{
public:
	inline void operator()(void* dst, uint8_t pix) const
	{
		uint8_t* pdst = (uint8_t*) dst;

		if (pix == 0) return;

		if ((*pdst) == 0) {
			(*pdst) = pix;
		} else if ((*pdst) == 255) {
			//
		} else {
			uint8_t ra = ~pix;
			uint8_t alpha = 255 - (ra*(255 - (*pdst))>>8);
			(*pdst) = alpha;
		}
	}
};

#endif