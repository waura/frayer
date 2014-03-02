#pragma once

#include "../resource1.h"
#include <libImgEdit.h>
#include "afxcmn.h"

class CBrushTextureParamTabDialog : public CDialog
{
	DECLARE_DYNAMIC(CBrushTextureParamTabDialog)

public:
	CBrushTextureParamTabDialog(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CBrushTextureParamTabDialog();

	BOOL Create(CWnd* pParentWnd) {
		return CDialog::Create(IDD, pParentWnd);
	}

	void Cancel();

	void SetEditBrush(IEBrush_Ptr pBrush) {
		m_pEditBrush = pBrush;
	}

// ダイアログ データ
	enum { IDD = IDD_BRUSH_TEXTURE_PARAM_TAB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	BOOL OnInitDialog();
	void OnPaint();

	DECLARE_MESSAGE_MAP()

private:
	bool m_prev_is_valid_texture;
	char m_prev_texture_path[_MAX_PATH];

	IEBrush_Ptr m_pEditBrush;

public:
	afx_msg void OnCbnSelchangeBrushTextureNameCombo();
	afx_msg void OnBnClickedIsUseBrushTextureCheck();
};
