#include "stdafx.h"

#include "RGBSliderFrame.h"

#define SLIDER_LEFT_MARGIN 24
#define SLIDER_TOP_MARGIN 5
#define SLIDER_RIGHT_MARGIN 48
#define SLIDER_BOTTOM_MARGIN 5

#define SLIDER_BAR_H 12
#define SLIDER_POINTER_W 16
#define SLIDER_POINTER_H 10
#define SLIDER_H (SLIDER_BAR_H + SLIDER_POINTER_H)

#define SLIDER_LABEL_H 14
#define SLIDER_LABEL_W 14

#define SLIDER_VAL_TXT_H 14
#define SLIDER_VAL_TXT_W 28

#define SLIDER_SPIN_BTN_H SLIDER_H
#define SLIDER_SPIN_BTN_W SLIDER_POINTER_W

#define R_MAX 255
#define R_MIN 0
#define G_MAX 255
#define G_MIN 0
#define B_MAX 255
#define B_MIN 0

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern ImgEdit g_ImgEdit;

CRGBSliderFrame::CRGBSliderFrame()
{
	m_isRDrag = false;
	m_isGDrag = false;
	m_isBDrag = false;
}

CRGBSliderFrame::~CRGBSliderFrame()
{
}

//////////////////////////////
/*
	選択色が変わったときに呼び出す。
	@param[in] color 変更後の色
*/
void CRGBSliderFrame::OnChangeSelectColor(IEColor color)
{
	if(m_SelectColor.value != color.value){
		m_SelectColor.value = color.value;
		RedrawWindow();
	}
}

BEGIN_MESSAGE_MAP(CRGBSliderFrame, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

int CRGBSliderFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (0 != CWnd::OnCreate(lpCreateStruct)) {
		return -1;
	}

	if(!m_Font.CreateFont(
		SLIDER_LABEL_H - 2,
		0,
		0,
		0,
		FW_NORMAL,
		FALSE,
		FALSE,
		0,
		SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FF_DONTCARE | DEFAULT_PITCH,
		"MS ゴシック")){
			return -1;
	}

	m_SelectColor = g_ImgEdit.GetSelectFGColor();

	//register event listener
	g_ImgEdit.AddEventListener( this );

	return 0;
}

void CRGBSliderFrame::OnDestroy()
{
	//unregister event listener
	g_ImgEdit.DeleteEventListener( this );

	CWnd::OnDestroy();
}

void CRGBSliderFrame::OnSize(UINT nType, int cx, int cy)
{
	int spin_btn_ctrl_w = SLIDER_VAL_TXT_W + SLIDER_SPIN_BTN_W;

	//r ctrl
	m_RSliderRect.left = SLIDER_LEFT_MARGIN;
	m_RSliderRect.right = cx - SLIDER_RIGHT_MARGIN;
	m_RSliderRect.top = SLIDER_TOP_MARGIN * 2;
	m_RSliderRect.bottom = m_RSliderRect.top + SLIDER_H;

	m_RSliderLabelRect.top = m_RSliderRect.top + (SLIDER_BAR_H / 2) - (SLIDER_LABEL_H / 2);
	m_RSliderLabelRect.bottom = m_RSliderLabelRect.top + SLIDER_LABEL_H;
	m_RSliderLabelRect.left = (SLIDER_LEFT_MARGIN / 2) - (SLIDER_LABEL_W / 2);
	m_RSliderLabelRect.right = m_RSliderLabelRect.left + SLIDER_LABEL_W;

	m_RSliderValRect.top = m_RSliderRect.top + (SLIDER_BAR_H / 2) - (SLIDER_VAL_TXT_H / 2);
	m_RSliderValRect.bottom = m_RSliderValRect.top + SLIDER_VAL_TXT_H;
	m_RSliderValRect.left = m_RSliderRect.right + (SLIDER_RIGHT_MARGIN / 2) - (spin_btn_ctrl_w / 2);
	m_RSliderValRect.right = m_RSliderValRect.left + SLIDER_VAL_TXT_W;

	m_RSliderSpinBtnRect.top = m_RSliderRect.top + (SLIDER_BAR_H / 2) - (SLIDER_SPIN_BTN_H / 2);
	m_RSliderSpinBtnRect.bottom = m_RSliderRect.top + SLIDER_SPIN_BTN_W;
	m_RSliderSpinBtnRect.left = m_RSliderValRect.right;
	m_RSliderSpinBtnRect.right = m_RSliderSpinBtnRect.left + SLIDER_SPIN_BTN_W;

	//g ctrl
	m_GSliderRect.left = SLIDER_LEFT_MARGIN;
	m_GSliderRect.right = cx - SLIDER_RIGHT_MARGIN;
	m_GSliderRect.top = m_RSliderRect.bottom + SLIDER_BOTTOM_MARGIN + SLIDER_TOP_MARGIN;
	m_GSliderRect.bottom = m_GSliderRect.top + SLIDER_H;

	m_GSliderLabelRect.top = m_GSliderRect.top + (SLIDER_BAR_H / 2) - (SLIDER_LABEL_H / 2);
	m_GSliderLabelRect.bottom = m_GSliderLabelRect.top + SLIDER_LABEL_H;
	m_GSliderLabelRect.left = (SLIDER_LEFT_MARGIN / 2) - (SLIDER_LABEL_W / 2);
	m_GSliderLabelRect.right = m_GSliderLabelRect.left + SLIDER_LABEL_W;

	m_GSliderValRect.top = m_GSliderRect.top + (SLIDER_BAR_H / 2) - (SLIDER_VAL_TXT_H / 2);
	m_GSliderValRect.bottom = m_GSliderValRect.top + SLIDER_VAL_TXT_H;
	m_GSliderValRect.left = m_GSliderRect.right + (SLIDER_RIGHT_MARGIN / 2) - (spin_btn_ctrl_w / 2);
	m_GSliderValRect.right = m_GSliderValRect.left + SLIDER_VAL_TXT_W;

	m_GSliderSpinBtnRect.top = m_GSliderRect.top + (SLIDER_BAR_H / 2) - (SLIDER_SPIN_BTN_H / 2);
	m_GSliderSpinBtnRect.bottom = m_GSliderRect.top + SLIDER_SPIN_BTN_W;
	m_GSliderSpinBtnRect.left = m_GSliderValRect.right;
	m_GSliderSpinBtnRect.right = m_GSliderSpinBtnRect.left + SLIDER_SPIN_BTN_W;

	//b ctrl
	m_BSliderRect.left = SLIDER_LEFT_MARGIN;
	m_BSliderRect.right = cx - SLIDER_RIGHT_MARGIN;
	m_BSliderRect.top = m_GSliderRect.bottom + SLIDER_BOTTOM_MARGIN + SLIDER_TOP_MARGIN;
	m_BSliderRect.bottom = m_BSliderRect.top + SLIDER_H;

	m_BSliderLabelRect.top = m_BSliderRect.top + (SLIDER_BAR_H / 2) - (SLIDER_LABEL_H / 2);
	m_BSliderLabelRect.bottom = m_BSliderLabelRect.top + SLIDER_LABEL_H;
	m_BSliderLabelRect.left = (SLIDER_LEFT_MARGIN / 2) - (SLIDER_LABEL_W / 2);
	m_BSliderLabelRect.right = m_BSliderLabelRect.left + SLIDER_LABEL_W;

	m_BSliderValRect.top = m_BSliderRect.top + (SLIDER_BAR_H / 2) - (SLIDER_VAL_TXT_H / 2);
	m_BSliderValRect.bottom = m_BSliderValRect.top + SLIDER_VAL_TXT_H;
	m_BSliderValRect.left = m_BSliderRect.right + (SLIDER_RIGHT_MARGIN / 2) - (spin_btn_ctrl_w / 2);
	m_BSliderValRect.right = m_BSliderValRect.left + SLIDER_VAL_TXT_W;

	m_BSliderSpinBtnRect.top = m_BSliderRect.top + (SLIDER_BAR_H / 2) - (SLIDER_SPIN_BTN_H / 2);
	m_BSliderSpinBtnRect.bottom = m_BSliderRect.top + SLIDER_SPIN_BTN_W;
	m_BSliderSpinBtnRect.left = m_BSliderValRect.right;
	m_BSliderSpinBtnRect.right = m_BSliderSpinBtnRect.left + SLIDER_SPIN_BTN_W;


	CWnd::OnSize(nType, cx, cy);
}

void CRGBSliderFrame::OnPaint()
{
	TRY
	{
		CPaintDC dc(this);
		CDC* pDC = CDC::FromHandle(dc.GetSafeHdc());

		CRect rectWnd;
		GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);
		int frame_width = rectWnd.right - rectWnd.left;
		int frame_height = rectWnd.bottom - rectWnd.top;

		//create back buffer
		CDC memDC;
		CBitmap memBmp;
		memDC.CreateCompatibleDC(pDC);
		memBmp.CreateCompatibleBitmap(pDC, frame_width, frame_height);

		CBitmap* memOldBmp = memDC.SelectObject(&memBmp);

		//clear window rect
		rectWnd.InflateRect(1, 1);
		memDC.FillSolidRect(rectWnd, ::GetSysColor(COLOR_WINDOW));

		//draw slider
		DrawRSlider(memDC.GetSafeHdc());
		DrawGSlider(memDC.GetSafeHdc());
		DrawBSlider(memDC.GetSafeHdc());

		//draw spin btn
		DrawSpinBtn(memDC.GetSafeHdc(), &m_RSliderSpinBtnRect);
		DrawSpinBtn(memDC.GetSafeHdc(), &m_GSliderSpinBtnRect);
		DrawSpinBtn(memDC.GetSafeHdc(), &m_BSliderSpinBtnRect);

		CFont* old_font = memDC.SelectObject(&m_Font);
		char* label_txt;

		//draw slider label
		label_txt = TEXT("R");
		memDC.DrawText(label_txt, strlen(label_txt), &m_RSliderLabelRect, DT_CENTER);
		label_txt = TEXT("G");
		memDC.DrawText(label_txt, strlen(label_txt), &m_GSliderLabelRect, DT_CENTER);
		label_txt = TEXT("B");
		memDC.DrawText(label_txt, strlen(label_txt), &m_BSliderLabelRect, DT_CENTER);

		//draw slider val
		char val_txt[256];
		wsprintf(val_txt, "%d", (int)m_SelectColor.r);
		memDC.DrawText(val_txt, strlen(val_txt), &m_RSliderValRect, DT_CENTER);
		wsprintf(val_txt, "%d", (int)m_SelectColor.g);
		memDC.DrawText(val_txt, strlen(val_txt), &m_GSliderValRect, DT_CENTER);
		wsprintf(val_txt, "%d", (int)m_SelectColor.b);
		memDC.DrawText(val_txt, strlen(val_txt), &m_BSliderValRect, DT_CENTER);

		memDC.SelectObject(old_font);

		//blt back buffer to display
		BitBlt(dc.GetSafeHdc(), 0, 0, frame_width, frame_height,
			memDC.GetSafeHdc(), 0, 0, SRCCOPY);

		//
		memDC.SelectObject(memOldBmp);
		memBmp.DeleteObject();
	}
	CATCH(CException, e)
	{
		::OutputDebugString("CRGBSliderFrame::OnPaint() exception\n");
	}
	END_CATCH
}

void CRGBSliderFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	::SetCapture( this->GetSafeHwnd() );

	bool isChangeColor = false;

	//check slider
	if (isInRect(point.x, point.y, &m_RSliderRect)) {
		m_isRDrag = true;
		isChangeColor = true;

		//calc slider value
		int slider_width = m_RSliderRect.right - m_RSliderRect.left;
		if(slider_width > 0){
			double per = (double)(point.x - m_RSliderRect.left) / slider_width;
			m_SelectColor.r = (R_MAX - R_MIN) * per;
		}

	} else if (isInRect(point.x, point.y, &m_GSliderRect)) {
		m_isGDrag = true;
		isChangeColor = true;

		//calc slider value
		int slider_width = m_GSliderRect.right - m_GSliderRect.left;
		if(slider_width > 0){
			double per = (double)(point.x - m_GSliderRect.left) / slider_width;
			m_SelectColor.g = (G_MAX - G_MIN) * per;
		}

	} else if (isInRect(point.x, point.y, &m_BSliderRect)) {
		m_isBDrag = true;
		isChangeColor = true;

		//calc slider value
		int slider_width = m_BSliderRect.right - m_BSliderRect.left;
		if(slider_width > 0){
			double per = (double)(point.x - m_BSliderRect.left) / slider_width;
			m_SelectColor.b = (B_MAX - B_MIN) * per;
		}
	}

	//check spin btn
	if (isInRect(point.x, point.y, &m_RSliderSpinBtnRect)) {
		isChangeColor = true;
		int center_y = m_RSliderSpinBtnRect.top + (m_RSliderSpinBtnRect.bottom - m_RSliderSpinBtnRect.top) / 2;
		if (point.y <  center_y) {
			if (m_SelectColor.r < R_MAX) {
				m_SelectColor.r++;
			}
		} else {
			if (m_SelectColor.r > R_MIN) {
				m_SelectColor.r--;
			}
		}
	} else if (isInRect(point.x, point.y, &m_GSliderSpinBtnRect)) {
		isChangeColor = true;
		int center_y = m_GSliderSpinBtnRect.top + (m_GSliderSpinBtnRect.bottom - m_GSliderSpinBtnRect.top) / 2;
		if (point.y <  center_y) {
			if (m_SelectColor.g < G_MAX) {
				m_SelectColor.g++;
			}
		} else {
			if (m_SelectColor.g > G_MIN) {
				m_SelectColor.g--;
			}
		}
	} else if (isInRect(point.x, point.y, &m_BSliderSpinBtnRect)) {
		isChangeColor = true;
		int center_y = m_BSliderSpinBtnRect.top + (m_BSliderSpinBtnRect.bottom - m_BSliderSpinBtnRect.top) / 2;
		if (point.y <  center_y) {
			if (m_SelectColor.b < B_MAX) {
				m_SelectColor.b++;
			}
		} else {
			if (m_SelectColor.b > B_MIN) {
				m_SelectColor.b--;
			}
		}
	}

	if (isChangeColor) {
		this->Invalidate(FALSE);

		LockImgEditEvent();
		g_ImgEdit.SetSelectFGColor(m_SelectColor);
		UnlockImgEditEvent();
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CRGBSliderFrame::OnMouseMove(UINT nFlags, CPoint point)
{
	bool isChangeColor = false;

	if (m_isRDrag) {
		isChangeColor = true;

		//calc slider value
		if (point.x > m_RSliderRect.right) {
			m_SelectColor.r = R_MAX;
		} else if (point.x < m_RSliderRect.left) {
			m_SelectColor.r = R_MIN;
		} else {
			int slider_width = m_RSliderRect.right - m_RSliderRect.left;
			if(slider_width > 0){
				double per = (double)(point.x - m_RSliderRect.left) / slider_width;
				m_SelectColor.r = (R_MAX - R_MIN) * per;
			}
		}
	} else if (m_isGDrag) {
		isChangeColor = true;

		//calc slider value
		if (point.x > m_GSliderRect.right) {
			m_SelectColor.g = G_MAX;
		} else if (point.x < m_GSliderRect.left) {
			m_SelectColor.g = G_MIN;
		} else {
			int slider_width = m_GSliderRect.right - m_GSliderRect.left;
			if(slider_width > 0){
				double per = (double)(point.x - m_GSliderRect.left) / slider_width;
				m_SelectColor.g = (G_MAX - G_MIN) * per;
			}
		}
	} else if (m_isBDrag) {
		isChangeColor = true;

		//calc slider value
		if (point.x > m_BSliderRect.right) {
			m_SelectColor.b = B_MAX;
		} else if (point.x < m_BSliderRect.left) {
			m_SelectColor.b = B_MIN;
		} else {
			int slider_width = m_BSliderRect.right - m_BSliderRect.left;
			if(slider_width > 0){
				double per = (double)(point.x - m_BSliderRect.left) / slider_width;
				m_SelectColor.b = (B_MAX - B_MIN) * per;
			}
		}
	}

	if (isChangeColor) {
		this->Invalidate(FALSE);

		LockImgEditEvent();
		g_ImgEdit.SetSelectFGColor(m_SelectColor);
		UnlockImgEditEvent();
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CRGBSliderFrame::OnLButtonUp(UINT nFlags, CPoint point)
{
	::ReleaseCapture();

	bool isChangeColor = false;
	if (m_isRDrag) {
		m_isRDrag = false;
		isChangeColor = true;

		//calc slider value
		if (point.x > m_RSliderRect.right) {
			m_SelectColor.r = R_MAX;
		} else if (point.x < m_RSliderRect.left) {
			m_SelectColor.r = R_MIN;
		} else {
			int slider_width = m_RSliderRect.right - m_RSliderRect.left;
			if(slider_width > 0){
				double per = (double)(point.x - m_RSliderRect.left) / slider_width;
				m_SelectColor.r = (R_MAX - R_MIN) * per;
			}
		}
	} else if (m_isGDrag) {
		m_isGDrag = false;
		isChangeColor = true;

		//calc slider value
		if (point.x > m_GSliderRect.right) {
			m_SelectColor.g = G_MAX;
		} else if (point.x < m_GSliderRect.left) {
			m_SelectColor.g = G_MIN;
		} else {
			int slider_width = m_GSliderRect.right - m_GSliderRect.left;
			if(slider_width > 0){
				double per = (double)(point.x - m_GSliderRect.left) / slider_width;
				m_SelectColor.g = (G_MAX - G_MIN) * per;
			}
		}
	} else if (m_isBDrag) {
		m_isBDrag = false;
		isChangeColor = true;

		//calc slider value
		if (point.x > m_BSliderRect.right) {
			m_SelectColor.b = B_MAX;
		} else if (point.x < m_BSliderRect.left) {
			m_SelectColor.b = B_MIN;
		} else {
			int slider_width = m_BSliderRect.right - m_BSliderRect.left;
			if(slider_width > 0){
				double per = (double)(point.x - m_BSliderRect.left) / slider_width;
				m_SelectColor.b = (B_MAX - B_MIN) * per;
			}
		}
	}

	if (isChangeColor) {
		this->Invalidate(FALSE);

		LockImgEditEvent();
		g_ImgEdit.SetSelectFGColor(m_SelectColor);
		UnlockImgEditEvent();
	}
	CWnd::OnLButtonUp(nFlags, point);
}

void CRGBSliderFrame::DrawRSlider(HDC hdc)
{
	int slider_width = m_RSliderRect.right - m_RSliderRect.left;
	double val;
	double step = (double)(R_MAX - R_MIN) / slider_width;

	IEColor color = m_SelectColor;
	color.a = 255;

	IplImage* sldr_img = cvCreateImage(
		cvSize(slider_width, SLIDER_BAR_H),
		IPL_DEPTH_8U,
		4);

	//draw slider
	int x,y;
	UCvPixel32* pix;
	UCvPixel32* line = GetPixelAddress32(sldr_img, 0, 0);
	for (y=0; y<SLIDER_BAR_H; y++) {
		val = 0.0;
		pix = line;
		for (x=0; x<slider_width; x++) {
			color.r = val;
			(*pix++).value = color.value;
			val += step;
		}
		line = GetNextLineAddress32(sldr_img, line);
	}

	//draw slider rectu
	UCvPixel bg = ucvPixel(0,0,0,255);
	AddRectToImage(sldr_img, 1, &bg);

	//blt slider image
	IPLImageToDevice(
		hdc,
		m_RSliderRect.left,
		m_RSliderRect.top,
		sldr_img->width,
		sldr_img->height,
		sldr_img,
		0, 0);

	//
	cvReleaseImage(&sldr_img);

	//
	double per = (double)(m_SelectColor.r - R_MIN) / R_MAX;
	int pt = slider_width * per;
	DrawSliderPointer(
		hdc,
		pt - SLIDER_POINTER_W / 2 + m_RSliderRect.left,
		m_RSliderRect.top + SLIDER_BAR_H,
		SLIDER_POINTER_W,
		SLIDER_POINTER_H);
}

void CRGBSliderFrame::DrawGSlider(HDC hdc)
{
	int slider_width = m_GSliderRect.right - m_GSliderRect.left;
	double val;
	double step = (double)(G_MAX - G_MIN) / slider_width;

	IEColor color = m_SelectColor;
	color.a = 255;

	IplImage* sldr_img = cvCreateImage(
		cvSize(slider_width, SLIDER_BAR_H),
		IPL_DEPTH_8U,
		4);

	//draw slider
	int x,y;
	UCvPixel32* pix;
	UCvPixel32* line = GetPixelAddress32(sldr_img, 0, 0);
	for (y=0; y<SLIDER_BAR_H; y++) {
		val = 0.0;
		pix = line;
		for (x=0; x<slider_width; x++) {
			color.g = val;
			(*pix++).value = color.value;
			val += step;
		}
		line = GetNextLineAddress32(sldr_img, line);
	}

	//draw slider rect
	UCvPixel bg = ucvPixel(0,0,0,255);
	AddRectToImage(sldr_img, 1, &bg);

	//blt slider image
	IPLImageToDevice(
		hdc,
		m_GSliderRect.left,
		m_GSliderRect.top,
		sldr_img->width,
		sldr_img->height,
		sldr_img,
		0, 0);

	//
	cvReleaseImage(&sldr_img);

	//
	double per = (double)(m_SelectColor.g - G_MIN) / G_MAX;
	int pt = slider_width * per;
	DrawSliderPointer(
		hdc,
		pt - SLIDER_POINTER_W / 2 + m_GSliderRect.left,
		m_GSliderRect.top + SLIDER_BAR_H,
		SLIDER_POINTER_W,
		SLIDER_POINTER_H);
}

void CRGBSliderFrame::DrawBSlider(HDC hdc)
{
	int slider_width = m_GSliderRect.right - m_GSliderRect.left;
	double val;
	double step = (double)(G_MAX - G_MIN) / slider_width;

	IEColor color = m_SelectColor;
	color.a = 255;

	IplImage* sldr_img = cvCreateImage(
		cvSize(slider_width, SLIDER_BAR_H),
		IPL_DEPTH_8U,
		4);

	//draw slider
	int x,y;
	UCvPixel32* pix;
	UCvPixel32* line = GetPixelAddress32(sldr_img, 0, 0);
	for (y=0; y<SLIDER_BAR_H; y++) {
		val = 0.0;
		pix = line;
		for (x=0; x<slider_width; x++) {
			color.b = val;
			(*pix++).value = color.value;
			val += step;
		}
		line = GetNextLineAddress32(sldr_img, line);
	}

	//draw slider rect
	UCvPixel bg = ucvPixel(0,0,0,255);
	AddRectToImage(sldr_img, 1, &bg);

	//blt slider image
	IPLImageToDevice(
		hdc,
		m_BSliderRect.left,
		m_BSliderRect.top,
		sldr_img->width,
		sldr_img->height,
		sldr_img,
		0, 0);

	//
	cvReleaseImage(&sldr_img);

	//
	double per = (double)(m_SelectColor.b - B_MIN) / B_MAX;
	int pt = slider_width * per;
	DrawSliderPointer(
		hdc,
		pt - SLIDER_POINTER_W / 2 + m_BSliderRect.left,
		m_BSliderRect.top + SLIDER_BAR_H,
		SLIDER_POINTER_W,
		SLIDER_POINTER_H);
}

void CRGBSliderFrame::DrawSpinBtn(HDC hdc, LPRECT lprc)
{
	//draw up btn
	DrawUTriangle(
		hdc,
		lprc->left,
		lprc->top,
		lprc->right - lprc->left,
		(lprc->bottom - lprc->top)/2 - 1);

	//draw down btn
	DrawDTriangle(
		hdc,
		lprc->left,
		lprc->top + (lprc->bottom - lprc->top) / 2,
		lprc->right - lprc->left,
		(lprc->bottom - lprc->top)/2 - 1);
}

void CRGBSliderFrame::DrawSliderPointer(HDC hdc, int x, int y, int cx, int cy)
{
	DrawUTriangle(hdc, x, y, cx, cy);
}

void CRGBSliderFrame::DrawUTriangle(HDC hdc, int x, int y, int cx, int cy)
{
	IplImage* pt_img = cvCreateImage(
		cvSize(cx, cy),
		IPL_DEPTH_8U,
		3);

	//draw slider pointer
	DWORD bg_color  = ::GetSysColor(COLOR_WINDOW);

	//clear slider pointer image
	cvSet(
		pt_img,
		cvScalar(GetBValue(bg_color), GetGValue(bg_color), GetRValue(bg_color)));

	//draw slider pointer
	int npts = 3; 
	CvPoint** pts = (CvPoint**) cvAlloc(sizeof(CvPoint*) * 1);
	pts[0] = (CvPoint*) cvAlloc(sizeof(CvPoint) * 3);
	pts[0][0].x = 0;
	pts[0][0].y = pt_img->height-1;
	pts[0][1].x = pt_img->width/2;
	pts[0][1].y = 1;
	pts[0][2].x = pt_img->width-1;
	pts[0][2].y = pt_img->height-1;
	cvFillPoly(pt_img, pts, &npts, 1, CV_RGB(80,80,80), CV_AA, 0);
	cvFree(&(pts[0]));
	cvFree(&pts);

	//blt slider image
	IPLImageToDevice(
		hdc,
		x,
		y,
		pt_img->width,
		pt_img->height,
		pt_img,
		0, 0);

	//
	cvReleaseImage(&pt_img);
}

void CRGBSliderFrame::DrawDTriangle(HDC hdc, int x, int y, int cx, int cy)
{
	IplImage* pt_img = cvCreateImage(
		cvSize(cx, cy),
		IPL_DEPTH_8U,
		3);

	//draw slider pointer
	DWORD bg_color  = ::GetSysColor(COLOR_WINDOW);

	//clear slider pointer image
	cvSet(
		pt_img,
		cvScalar(GetBValue(bg_color), GetGValue(bg_color), GetRValue(bg_color)));

	//draw slider pointer
	int npts = 3; 
	CvPoint** pts = (CvPoint**) cvAlloc(sizeof(CvPoint*) * 1);
	pts[0] = (CvPoint*) cvAlloc(sizeof(CvPoint) * 3);
	pts[0][0].x = 1;
	pts[0][0].y = 1;
	pts[0][1].x = pt_img->width/2;
	pts[0][1].y = pt_img->height-1;
	pts[0][2].x = pt_img->width-2;
	pts[0][2].y = 1;
	cvFillPoly(pt_img, pts, &npts, 1, CV_RGB(80,80,80), CV_AA, 0);
	cvFree(&(pts[0]));
	cvFree(&pts);

	//blt slider image
	IPLImageToDevice(
		hdc,
		x,
		y,
		pt_img->width,
		pt_img->height,
		pt_img,
		0, 0);

	//
	cvReleaseImage(&pt_img);
}
