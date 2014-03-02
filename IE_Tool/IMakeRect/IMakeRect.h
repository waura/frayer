#ifndef _IMAKERECT_H_
#define _IMAKERECT_H_

#include "IIETool.h"
#include <libImgEdit.h>

typedef struct _POINT_INDEX {
	int row;
	int col;
} POINT_INDEX;


class IMakeRect : public IIETool
{
public:	
	IMakeRect();
	virtual ~IMakeRect();

	//マウス座標補正無し
	bool isRevise(){
		return false;
	}

	//マウス座標系は画像座標
	virtual IE_INPUT_DATA_FLAG GetInputDataFlag(){
		return IE_INPUT_DATA_FLAG::IMAGE_POS;
	}

	virtual void OnSelect();
	virtual void OnNeutral();

	virtual void OnDraw(HDC hdc);

	virtual void OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd);
	virtual void OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd);
	virtual void OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd);

protected:

	void GetDragPointRect(LPRECT lprc);
	void ClearDragPoint();
	bool IsMakedRect(){
		return m_isMakedRect;
	}
	HPEN m_hPen;

private:
	void NormalDrag(LPIE_INPUT_DATA lpd);
	void DrawPointRect(HDC hdc, const CvPoint2D64f* pcvPoint);
	bool isOnPointRect(const CvPoint2D64f* pcvPoint, const CvPoint2D64f* pMouse);

	bool m_isMakedRect;
	bool m_isClickOnDragPointArea;
	UINT m_DragFlags;
	POINT_INDEX m_SelectPointIndex;
	POINT m_befor_mouse_pt;

	CvPoint2D64f m_DragPoint[3][3];
};

#endif //_IMAKERECT_H_