#pragma once

#include "ImgMask.h"
#include "ImgFile.h"

#undef CImage

#define MASK_NAME_FONT_SIZE 12
#define MASK_NAME_FONT_NAME "ＭＳ ゴシック"


class CMaskPanel :
	public CWnd
{
public:
	CMaskPanel();
	~CMaskPanel();


	void SetMask(ImgMask* pMask);

	////////////////////////////////////////////
	/*!
		マスク更新イベント時に呼び出される。
	*/
	void OnUpdateMask();

	////////////////////////////////////////////
	/*!
		ドラッグ中のマウスがパネルの上に来たときに呼び出す
		@param[in] lpt マウスのスクリーン座標
	*/
	void OnDragMouse(LPPOINT lpt);

	/////////////////////////////////////////////////////
	/*!
		パネルを選択色にする。
	*/
	void SelectPanel();

	////////////////////////////////////////////////
	/*!
		パネルを普通の色にする。
	*/
	void NewtralPanel();

	////////////////////////////////////////////////
	/*!
		ドラッグ時のイメージを作成する
		@param[out] hImageList 出力先のイメージリスト
	*/
	void CreateDragImage(HIMAGELIST hImageList);

	//////////////////////////////////////////////
	/*!
		マスクスタイルダイアログを開く
	*/
	void OpenMaskStyleDialog(HWND hParent);

	//////////////////////////////////////////////
	/*!
		マスク削除
	*/
	void DeleteMask();

	//////////////////////////////////////////////
	/*!
		ポップアップメニュー表示
	*/
	void PopupMenu(HWND hParent, const LPPOINT lpt);

	//////////////////////////////////////////////
	/*!
		マスクオブジェクトへのポインタを返す。
		@return マスクオブジェクトへのポインタ
	*/
	ImgMask* GetMask();

	void UpdateMaskThm();

	///////////////////////////////////////////
	/*!
		パネルの描画
		@param[in] pDC 描画するデバイスコンテキスト
	*/
	void Draw(CDC *pDC);

	//////////////////////////////////////////////
	/*!
		マスクスタイルダイアログのプロシージャ
	*/
	static LRESULT CALLBACK MaskStyleDialogProc(HWND, UINT, WPARAM, LPARAM);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType,int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP();

private:

	/////////////////////////////////////////////
	/*!
		レイヤースタイルダイアログの初期化
	*/
	void InitMaskStyleDialog(HWND hDlgWnd);

	//////////////////////////////////////////////
	/*!
		描画モードの選択項目が変更されたとき呼び出す
	*/
	void OnChangeDrawModeCombo(HWND hDlgWnd);


	HINSTANCE m_hInst;

	ImgMask *m_Mask;

	IplImage* m_MaskThm;

	
	RECT m_EyeRect;
	
	RECT m_MaskThmRect;

	int m_DrawModeCombo_buf;

	bool isSelectMask; //選択表示をするかどうか
	bool isInsertOver;
	bool isInsertUnder;
};