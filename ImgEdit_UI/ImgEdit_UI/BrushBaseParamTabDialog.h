#pragma once

#include "../resource1.h"
#include <libImgEdit.h>
#include "afxcmn.h"

class CBrushBaseParamTabDialog : public CDialog
{
	DECLARE_DYNAMIC(CBrushBaseParamTabDialog)

public:
	CBrushBaseParamTabDialog(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CBrushBaseParamTabDialog();

	BOOL Create(CWnd* pParentWnd){
		return CDialog::Create(IDD, pParentWnd);
	}

	void Cancel();

	void SetEditBrush(IEEMBrush_Ptr pBrush) {
		m_pEditBrush = pBrush;
	}

// ダイアログ データ
	enum { IDD = IDD_BRUSH_BASE_PARAM_TAB };

protected:
	//
	virtual void PostNcDestroy();
	//
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	BOOL OnInitDialog();
	void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	//
	DECLARE_MESSAGE_MAP()

	afx_msg void OnTcnSelchangeParamGroupTab(NMHDR *pNMHDR, LRESULT *pResult);

private:

	void UpdateBrushParam();

	int m_prev_org_rad;
	int m_prev_hardness;
	int m_prev_min_rad;
	int m_prev_min_alpha;
	int m_prev_interval;
	IE_BRUSH_CTRL_ID m_prev_rad_ctrl_id;
	IE_BRUSH_CTRL_ID m_prev_alpha_ctrl_id;
	IEEMBrush_Ptr m_pEditBrush;
public:
	afx_msg void OnCbnSelchangeRadCtrlCombo();
	afx_msg void OnCbnSelchangeAlphaCtrlCombo();
};
