#pragma once

#include <libImgEdit.h>

#include "afxcmn.h"
#include "../resource1.h"
#include "BrushBaseParamTabDialog.h"
#include "BrushTextureParamTabDialog.h"

class CIEEMBrushEditDialog : public CDialog
{
	DECLARE_DYNAMIC(CIEEMBrushEditDialog)

public:
	CIEEMBrushEditDialog(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CIEEMBrushEditDialog();

	void SetEditBrush(IEEMBrush_Ptr pBrush){
		m_pEditBrush = pBrush;
	}

	void UpdateBrushPreview();

	BOOL Create(CWnd* pParentWnd){
		return CDialog::Create(IDD, pParentWnd);
	}

// ダイアログ データ
	enum { IDD = IDD_BRUSH_EDIT };

protected:
	//
	virtual void PostNcDestroy();
	//
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	BOOL OnInitDialog();
	void OnOK();
	void OnCancel();
	void OnPaint();
	void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	//
	DECLARE_MESSAGE_MAP()

	afx_msg void OnTcnSelchangeParamGroupTab(NMHDR *pNMHDR, LRESULT *pResult);

private:


	IEEMBrush_Ptr m_pEditBrush;

	CTabCtrl m_ParamGroupTab;
	CBrushBaseParamTabDialog m_BrushBaseParamTabDialog;
	CBrushTextureParamTabDialog m_BrushTextureParamTabDialog;
};
