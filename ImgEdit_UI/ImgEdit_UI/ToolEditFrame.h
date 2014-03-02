#pragma once

#include "IToolEditWndHandle.h"
#include "ScrollWnd.h"

#include "EditSliderCtrl.h"
#include "LabelComboBox.h"

typedef struct _TE_CTRL{
	CWnd* pWnd;
	int width;
	int height;
	int CtrlType;
} TE_CTRL;

typedef std::list<TE_CTRL> TE_CTRL_LIST;

class CToolEditFrame :
	public CScrollWnd,
	public IToolEditWndHandle
{
public:
	CToolEditFrame();
	~CToolEditFrame();

	virtual void OnDraw(CDC* pDC);
	virtual void OnInitialUpdate();

	VOID ClearAllCtrl();

	//////////////////////////////////
	/*!
		トラックバーの追加
		@param[in] name トラックバーの名前
		@param[in] min 最小値
		@param[in] max 最大値
		@param[in] start 開始時の値
		@param[in] change_val 値を変更する変数
	*/
	VOID AddTrackbar(const char* name, int min, int max, int start, int* change_val);

	//////////////////////////////////
	/*!
		レイヤー合成コンボボックスの追加
		@param[in] name トラックバーの名前
		@param[in] start_index 開始時のインデックス
		@param[in] change_val 値を変更する変数
	*/
	VOID AddLayerDrawModeComboBox(const char* name, int start_index, int* change_val);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	DECLARE_MESSAGE_MAP();

private:

	//////////////////////////////////////
	/*!
		スクロールバーサイズ調節
	*/
	void AdjastScrollSizes();

	//////////////////////////////////////
	/*!
		コントロールの再配置
	*/
	void AdjastControll();

	SCROLLINFO m_FrameScrInfo;

	int m_ctrl_count;
	TE_CTRL_LIST m_te_ctrl_list;
};