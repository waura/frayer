#pragma once

#include "HoverBitmapButton.h"

class CEditSliderCtrl :
	public CWnd
{
public:
	CEditSliderCtrl(): 
	  m_change_val(0),
	  m_isLockEvent(false){};
	~CEditSliderCtrl(){};

	BOOL CreateEditSliderCtrl(int label_width, int label_height, int slider_width, int slider_height,
		int editbox_width, int editbox_height, const RECT& rc, CWnd* parentWnd, int id);

	VOID SetSize(int label_width, int label_height, int slider_width, int slider_height,
		int editbox_width, int editbox_height);
	VOID SetLabel(LPCTSTR lpszString);
	VOID SetRange(int min, int max, BOOL bRedraw = 0);
	VOID SetPos(int nPos);
	VOID SetChangeVal(int* change_val){
		m_change_val = change_val;
	}
	BOOL SetTic(int nTic);

	/////////////////////////////////////////////////////
	/*!
		return this ctrl need to use width
	*/
	inline int GetUseWidth(){
		return 42*2;
	}
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedLBtn();
	afx_msg void OnBnClickedRBtn();
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP();

private:
	inline bool IsLockEvent(){ return m_isLockEvent; }
	inline void LockEvent(){ m_isLockEvent = true; }
	inline void UnlockEvent(){ m_isLockEvent = false; }

	int m_label_width;
	int m_label_height;
	int m_slider_width;
	int m_slider_height;
	int m_editbox_width;
	int m_editbox_height;

	int* m_change_val;
	int m_min;
	int m_max;

	bool m_isLockEvent;

	CFont m_Font;
	CStatic m_Label;
	CEdit m_Edit;
	CSliderCtrl m_Slider;
	CHoverBitmapButton m_LeftBtn;
	CHoverBitmapButton m_RightBtn;
};
