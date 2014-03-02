#pragma once

#include "ImgFile.h"
#include "LayerPanel.h"
#include "LayerGroupPanel.h"
#include "ImgEditEventListener.h"
#include "ImgFileEventListener.h"
#include "ScrollWnd.h"

typedef std::vector<CLayerPanel*> LayerPanel_Vec;

#define UM_DROPLAYER 10 //レイヤーパネルがドロップしたときのメッセージ


class CLayerFrame :
	public CScrollWnd,
	public ImgEditEventListener,
	public ImgFileEventListener
{
public:
	CLayerFrame();
	~CLayerFrame();

	////////////////////////////////////////////////////
	/*!
		アクティブになったファイルが変わったときに呼び出されるイベント
		@param[in] file アクティブなファイルオブジェクト
	*/
	void OnChangeActiveFile(ImgFile_Ptr pNewFile, ImgFile_Ptr pOldFile);

	/////////////////////////////////////////////////////
	/*!
		レイヤーが追加されたときに呼び出されるイベント
		@param[in] pLayer 追加されたレイヤー
		@param[in] index レイヤーインデックス
	*/
	void OnAddLayer(IImgLayer_Ptr pLayer, int index);

	/////////////////////////////////////////////////////
	/*!
		レイヤーが削除されるときに呼び出されるイベント
		@param[in] pLayer 削除されるレイヤー
		@param[in] index レイヤーインデックス
	*/
	void OnDeleteLayer(IImgLayer_Ptr pLayer, int index);

	void OnChangeLayerLine(int from_index, int to_index);

	//////////////////////////////
	/*!
		選択レイヤーが変わったときに呼び出す
		@param[in] pLayer 選択されたレイヤー
	*/
	void OnSelectLayer(IImgLayer_weakPtr pLayer);

	virtual void OnDraw(CDC* pDC);
	virtual void OnInitialUpdate();

protected:

	BOOL PreCreateWindow(CREATESTRUCT& cs);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnCommand(WPARAM wp, LPARAM lp);

	afx_msg void OnLayerNameEditKillFocus();

	DECLARE_MESSAGE_MAP();

private:

	//////////////////////////////////////
	/*!
		スクロールバーサイズ調節
	*/
	void AdjastScrollSizes();

	////////////////////////////////////////////////////
	/*!
	*/
	VOID CreatePanel(ImgLayerGroup_Ptr pLGroup);

	////////////////////////////////////////////////////
	/*!
	*/
	VOID ReSetAllPanel(ImgFile_Ptr pFile);

	////////////////////////////////////////////////////
	/*!
		reset panel position annd state
	*/
	VOID AdjastPanel();

	////////////////////////////////////////////////////
	/*!
		indexの位置のレイヤーパネルを削除する
		@param[in] index
	*/
	VOID DeleteLayerPanel(int index);

	////////////////////////////////////////////////////
	/*!
		@param[in] from_index
		@param[in] to_index
	*/
	VOID InsertLayerPanel(int from_index, int to_index);

	///////////////////////////////////////////////////
	/*!
		すべてのレイヤーパネルを再描画する。
	*/
	void AllPanelRePaint();

	////////////////////////////////////////////////////
	/*!
		マウスの下にあるパネルのポインタを返す。
		@param[in] point クライアントマウス座標
		@return レイヤーパネルのポインタ
	*/
	CLayerPanel* GetLayerPanelFromPos(CPoint point);
	
	////////////////////////////////////////////////////
	/*!
		マウスの下にあるパネルのインデックスを返す。
		@param[in] point クライアントマウス座標
		@return レイヤーパネルのインデックス、見つからなければ(-1)
	*/
	int GetLayerPanelIndexFromPos(CPoint point);

	////////////////////////////////////////////////////
	/*!
		レイヤーパネルを削除する。
		@param[in,out] lp 削除するレイヤーパネル
	*/
	VOID DeleteLayerPanel(CLayerPanel* lp);

	////////////////////////////////////////////////////
	/*!
		レイヤーパネルをすべて削除メモリー開放
	*/
	VOID ClearLayerPanel();

	bool isPanelDrag;					//パネルドラッグフラグ
	LayerPanel_Vec m_LayerPanel_Vec;	//レイヤーパネルリスト
	CLayerPanel* m_OnMousePanel;		//マウスの下にあるパネル
	CLayerPanel* m_SelectLayerPanel;	//選択中レイヤーパネル

	int m_DragLayerPanelInsertIndex;
	int m_DragLayerPanelIndex;
	CLayerPanel* m_DragLayerPanel;		//ドラッグ中のレイヤーパネル
	CLayerPanel* m_PopupLayerPanel;

	RECT m_InsertCursorRect;

	CFont m_LayerNameFont;
	CLayerNameEdit m_LayerNameEdit;

	ImgFile_Ptr m_pActiveFile;				//現在編集中のファイルオブジェクト

	SCROLLINFO m_FrameScrInfo;
};