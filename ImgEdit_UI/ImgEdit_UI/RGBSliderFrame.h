
#pragma once

class CRGBSliderFrame :
	public CWnd,
	public ImgEditEventListener
{
public:
	CRGBSliderFrame();
	~CRGBSliderFrame();

	//////////////////////////////
	/*!
		選択色が変わったときに呼びだされる
		@param[in] color 変更後の色
	*/
	void OnChangeSelectColor(IEColor color);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP();

private:

	void DrawRSlider(HDC hdc);
	void DrawGSlider(HDC hdc);
	void DrawBSlider(HDC hdc);
	void DrawSpinBtn(HDC hdc, LPRECT lprc);
	void DrawSliderPointer(HDC hdc, int x, int y, int cx, int cy);

	void DrawUTriangle(HDC hdc, int x, int y, int cx, int cy);
	void DrawDTriangle(HDC hdc, int x, int y, int cx, int cy);

	bool m_isRDrag;
	bool m_isGDrag;
	bool m_isBDrag;

	IEColor m_SelectColor;

	CFont m_Font;

	RECT m_RSliderRect;
	RECT m_GSliderRect;
	RECT m_BSliderRect;

	RECT m_RSliderLabelRect;
	RECT m_GSliderLabelRect;
	RECT m_BSliderLabelRect;

	RECT m_RSliderValRect;
	RECT m_GSliderValRect;
	RECT m_BSliderValRect;

	RECT m_RSliderSpinBtnRect;
	RECT m_GSliderSpinBtnRect;
	RECT m_BSliderSpinBtnRect;
};