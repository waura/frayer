
#pragma once

#include "ImgLayer.h"

class CLayerNameEdit :
	public CEdit
{
public:
	CLayerNameEdit(){
	}

	void SetEditLayer(IImgLayer_Ptr pLayer){
		m_pEditLayer = pLayer;
	}

	void EndEdit();

protected:
	BOOL PreTranslateMessage(MSG* pMsg);
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	afx_msg void OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized);

	DECLARE_MESSAGE_MAP();

private:
	IImgLayer_Ptr m_pEditLayer;
};