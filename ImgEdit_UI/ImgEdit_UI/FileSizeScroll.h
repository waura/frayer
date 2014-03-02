
#pragma once

#include "EditSliderCtrl.h"

class CFileSizeScroll :
	public CWnd,
	public ImgEditEventListener,
	public ImgFileEventListener
{
public:
	CFileSizeScroll();
	~CFileSizeScroll();

	void OnChangeActiveFile(ImgFile_Ptr pNewFile, ImgFile_Ptr pOldFile);

	/////////////////////////////////////////////////////
	/*!
	*/
	void OnChangeViewSize(int size);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nTyp, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	DECLARE_MESSAGE_MAP();

private:
	CEditSliderCtrl m_FileSizeSlider;
	ImgFile_Ptr m_pActiveFile;
};