
#pragma once

class CHSVCircleFrame :
	public CWnd,
	public ImgEditEventListener,
	public std::enable_shared_from_this<CHSVCircleFrame>
{
public:
	CHSVCircleFrame();
	~CHSVCircleFrame();

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
	//Hサークルを描画
	void DrawHCircle(IplImage* dst, int center_x, int center_y, int radius, int circle_width);
	//SV矩形
	void DrawSVRect(IplImage* dst, int start_x, int start_y, int width, int height);
	//
	void DrawPoint(IplImage* dst, int center_x, int center_y, int radius, int circle_width);

	///////////////////////////////////////////
	/*!
		色選択円の中心の位置を返す。
		@param[out] point 中心のクライアント座標
	*/
	void GetCircleCenterPos(CPoint& point);

	//////////////////////////////////
	/*!
		色選択円の半径を返す。
	*/
	double GetCircleRadius();

	//////////////////////////////////
	/*!
		SV四角形の開始位置を返す。
		@param[out] point
	*/
	void GetBeginSVRectPos(CPoint& point);

	//////////////////////////////////
	/*!
		SV四角形の終了位置を返す。
		@param[out] point
	*/
	void GetEndSVRectPos(CPoint& point);

	//////////////////////////////////
	/*!
		lptがH選択円に乗っているか判定する。
		@param[in] point マウスクライアント座標
		@return 乗っていたらH値 そうでないなら-1.0
	*/
	double isOnColorCircle(CPoint point);

	///////////////////////////////////
	/*!
		lptがSV選択四角に乗っているか判定する。
		@param[in] lpt マウスクライアント座標
		@param[out] s 選択されたs値
		@param[out] v 選択されたv値
		@return 乗っているたらtrue そうでないならfalseを返す。
	*/
	double isOnColorRect(CPoint point, double *s, double *v);

	bool m_isHDrag;
	bool m_isSVDrag;
	IEColor m_SelectColor;
	double m_Select_H;
	double m_Select_S;
	double m_Select_V;
	RECT m_ColorCircleRect;
	RECT m_FGColorPanelRect;
	RECT m_BGColorPanelRect;
};