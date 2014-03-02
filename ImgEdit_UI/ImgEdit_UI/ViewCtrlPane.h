
#pragma once 

#include "HoverBitmapButton.h"

class CViewCtrlPane :
	public CWnd,
	public ImgEditEventListener,
	public ImgFileEventListener
	//public std::enable_shared_from_this<CViewCtrlPane>
{
public:
	CViewCtrlPane();
	~CViewCtrlPane();

	//ImgEdit event
	void OnChangeActiveFile(ImgFile_Ptr pNewFile, ImgFile_Ptr pOldFile);

	//ImgFile event
	void OnChangeViewFlipH(bool state);
	void OnChangeViewFlipV(bool state);

protected:
	BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();

	void OnClickedRightRotBtn();
	void OnClickedLeftRotBtn();
	void OnClickedFlipHBtn();
	void OnClickedFlipVBtn();

	DECLARE_MESSAGE_MAP();

private:

	CToolTipCtrl m_ToolTip;

	CHoverBitmapButton m_RightRotBtn;
	CHoverBitmapButton m_LeftRotBtn;
	CHoverBitmapButton m_FlipHBtn;
	CHoverBitmapButton m_FlipVBtn;

	ImgFile_Ptr m_pEditFile;
};