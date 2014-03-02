
#pragma once

#include "LayerFrame.h"
#include "LayerAlphaEdit.h"
#include "LayerLockState.h"
#include "LayerCtrl.h"

#define ID_LAYER_FRAME 101


class CLayerWnd : public CDockablePane
{
public:
	CLayerWnd();
	~CLayerWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	LRESULT OnAppMouseWheel(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP();

private:

	CLayerFrame m_wndLayerFrame;
	CLayerAlphaEdit m_wndLayerAlphaEdit;
	CLayerLockState m_wndLayerLockState;
	CLayerCtrl m_wndLayerCtrl;
};