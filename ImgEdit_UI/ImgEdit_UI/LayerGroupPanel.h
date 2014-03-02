#pragma once 

#include "LayerPanel.h"

class CLayerGroupPanel :
	public CLayerPanel
{
public:
	CLayerGroupPanel(HWND hParent);
	~CLayerGroupPanel()
	{
	}

	void SetLayer(IImgLayer_Ptr pLayer)
	{
		assert(pLayer->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER);
		CLayerPanel::SetLayer(pLayer);
	}

	void PopupMenu(HWND hParent, const LPPOINT lpt);

	void DrawPanel(CDC* pDC);

	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonDblClk(UINT nFlags, CPoint point);

private:
	CBitmap m_OpenAicon;
	CBitmap m_CloseAicon;
};