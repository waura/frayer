
#pragma once

class CPreviewFrame : 
	public CWnd,
	public ImgEditEventListener,
	public ImgFileEventListener
{
public:
	CPreviewFrame();
	~CPreviewFrame();

	void OnChangeActiveFile(ImgFile_Ptr pNewFile, ImgFile_Ptr pOldFile);

	void OnUpdate();

	void UpdatePreviewThm();
	void UpdateDragRect();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR uIDEvent);

	DECLARE_MESSAGE_MAP();

private:
	bool m_isDragRect;
	double m_resize_ration;

	UINT_PTR m_nTimer;

	CvPoint m_DispRectPt[4];
	RECT m_ImgDragRect;
	CvPoint2D64f m_beforPos;
	IplImage* m_PreviewThm;
	
	ImgFile_Ptr m_pEditFile;
};