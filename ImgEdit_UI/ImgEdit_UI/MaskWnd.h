
#pragma once 

#include "MaskFrame.h"
#include "MaskCtrl.h"

#define ID_MASK_FRAME 101

class CMaskWnd : public CDockablePane
{
public:
	CMaskWnd();
	~CMaskWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP();

private:

	int m_width;
	CMaskFrame m_wndMaskFrame;
	CMaskCtrl m_wndMaskCtrl;
};