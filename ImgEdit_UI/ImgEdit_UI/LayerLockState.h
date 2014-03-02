#pragma once

#include "HoverBitmapButton.h"
#include "ImgFileEventListener.h"

class CLayerLockState :
	public CWnd,
	public ImgEditEventListener,
	public ImgFileEventListener
{
public:
	CLayerLockState();
	~CLayerLockState();

	void OnChangeActiveFile(ImgFile_Ptr pNewFile, ImgFile_Ptr pOldFile);

	//////////////////////////////////
	/*!
		レイヤー選択時イベント
	*/
	void OnSelectLayer(IImgLayer_weakPtr pLayer);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();

	void OnBnClickedLockAlphaBtn();

	DECLARE_MESSAGE_MAP();

private:

	ImgFile_Ptr m_pSelectFile;
	IImgLayer_Ptr m_pSelectLayer;

	CToolTipCtrl m_ToolTip;

	CStatic m_Label;
	CHoverBitmapButton m_LockAlphaBtn;
};