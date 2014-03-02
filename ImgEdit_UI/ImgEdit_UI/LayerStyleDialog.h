#pragma once

#include "../resource1.h"
#include <libImgEdit.h>

// CLayerStyleDialog ダイアログ

class CLayerStyleDialog : public CDialog
{
	DECLARE_DYNAMIC(CLayerStyleDialog)

public:
	CLayerStyleDialog(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CLayerStyleDialog();

	void SetEditImgLayer(ImgLayer_Ptr pLayer) {
		m_pEditLayer = pLayer;
	}

// ダイアログ データ
	enum { IDD = IDD_LAYER_STYLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	BOOL OnInitDialog();
	void OnOK();
	void OnCancel();

	BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

private:
	char m_alpha_txt[4];
	int m_start_alpha;

	int m_blend_mode_index;
	int m_start_blend_mode_index;

	ImgLayer_Ptr m_pEditLayer;

	ChangeLayerOpacityHandle* m_pChangeOpacityHandle;
	ChangeLayerSynthFourCCHandle* m_pChangeFourCCHandle;
};
