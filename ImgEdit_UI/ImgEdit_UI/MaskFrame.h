#pragma once 

#include "ImgFile.h"
#include "MaskPanel.h"
#include "ImgEditEventListener.h"
#include "ImgFileEventListener.h"
#include "ScrollWnd.h"

typedef std::vector<CMaskPanel*> MaskPanel_Vec;

#define MASK_PANEL_H 40

class CMaskFrame :
	public CScrollWnd,
	public ImgEditEventListener,
	public ImgFileEventListener
{
public:
	CMaskFrame();
	~CMaskFrame();

	////////////////////////////////////////////////////
	/*!
		アクティブになったファイルが変わったときに呼び出されるイベント
		@param[in] file アクティブなファイルオブジェクト
	*/
	void OnChangeActiveFile(ImgFile_Ptr pNewFile, ImgFile_Ptr pOldFile);

	/////////////////////////////////////////////////////
	/*!
		ファイルが追加されたときに呼び出すイベント
		@param[in] pFile 追加されたファイル
	*/
	void OnAddImgFile(ImgFile_Ptr pFile);

	/////////////////////////////////////////////////////
	/*!
		マスクが追加されたときに呼び出されるイベント
		@param[in] pMask 追加されたマスク
		@param[in] index マスクインデックス
	*/
	void OnAddMask(ImgMask *pMask, int index);

	/////////////////////////////////////////////////////
	/*!
		マスクが削除されるときに呼び出されるイベント
		@param[in] pMask 削除されるマスク
		@param[in] index マスクインデックス
	*/
	void OnDeleteMask(ImgMask *pMask, int index);

	virtual void OnDraw(CDC* pDC);
	virtual void OnInitialUpdate();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnCommand(WPARAM wp, LPARAM lp);

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
	VOID ReSetAllPanel(ImgFile_Ptr pFile);

	////////////////////////////////////////////////////
	/*!
		indexの位置にpMaskのマスクパネルを追加する。
		@param[in] pMask
		@param[in] index
	*/
	VOID AddMaskPanel(ImgMask* pMask, int index);

	////////////////////////////////////////////////////
	/*!
		indexの位置のマスクパネルを削除する
		@param[in] index
	*/
	VOID DeleteMaskPanel(int index);

	////////////////////////////////////////////////////
	/*!
	*/
	VOID AdjastPanel();

	///////////////////////////////////////////////////
	/*!
		すべてのマスクパネルを再描画する。
	*/
	void AllPanelRePaint();

	///////////////////////////////////////////////////
	/*!
		ウィンドウハンドルからマスクパネルを探し返す。
		@param[in] hWnd 探すマスクパネルのハンドル
		@return マスクパネルのポインタ
	*/
	//LayerPanel* GetLayerPanelFromHWND(HWND hWnd);

	////////////////////////////////////////////////////
	/*!
		マウスの下にあるパネルのポインタを返す。
		@param[in] point クライアントマウス座標
		@return マスクパネルのポインタ
	*/
	CMaskPanel* GetMaskPanelFromPos(CPoint point);

	////////////////////////////////////////////////////
	/*!
		マスクパネルを削除する。
		@param[in,out] lp 削除するマスクパネル
	*/
	VOID DeleteMaskPanel(CMaskPanel* lp);

	////////////////////////////////////////////////////
	/*!
		マスクパネルをすべて削除メモリー開放
	*/
	VOID ClearMaskPanel();

	HINSTANCE m_hInst;
	MaskPanel_Vec m_MaskPanel_Vec;	//マスクパネル配列
	CMaskPanel* m_SelectMaskPanel;	//選択中マスクパネル
	CMaskPanel* m_DragMaskPanel;	//ドラッグ中のマスクパネル
	CMaskPanel* m_UnderMaskPanel;	//ドラッグ中マウスカーソルの下にあるマスクパネル
	CMaskPanel* m_PopupMaskPanel;
	
	HIMAGELIST m_hPanelDragImage;		//ドラッグ時のイメージ
	bool isPanelDrag;					//パネルドラッグフラグ

	ImgFile_Ptr m_pActiveFile				//現在編集中のファイルオブジェクト

	SCROLLINFO m_FrameScrInfo;

	int m_width;
};