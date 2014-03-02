#pragma once

#include "ImgFileEventListener.h"
#include "EditSliderCtrl.h"

class CLayerAlphaEdit :
	public CWnd,
	public ImgEditEventListener,
	public ImgFileEventListener
{
public:
	CLayerAlphaEdit();
	~CLayerAlphaEdit();

	void OnChangeActiveFile(ImgFile_Ptr pNewFile, ImgFile_Ptr pOldFile);

	//////////////////////////////////
	/*!
		レイヤー選択時イベント
	*/
	void OnSelectLayer(IImgLayer_weakPtr pLayer);
	void OnChangedLayerOpacity(uint8_t new_opacity);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nTyp,int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	DECLARE_MESSAGE_MAP();

private:
	CEditSliderCtrl m_LayerAlphaSlider;

	IImgLayer_Ptr m_pSelectLayer;
	ImgFile_Ptr m_pActiveFile;

	ChangeLayerOpacityHandle* m_pChangeOpacityHandle;
};