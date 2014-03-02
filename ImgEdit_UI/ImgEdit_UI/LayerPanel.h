#pragma once

#include "ImgLayer.h"
#include "ImgFile.h"
#include "ImgLayerEventListener.h"

#include "ImgEdit_UI.h"
#include "LayerNameEdit.h"

#undef CImage

#define ID_LP_NAME_EDIT 1

#define LP_FONT_NAME "ＭＳ ゴシック"

#define LAYER_NAME_FONT_SIZE 14
#define LAYER_NAME_FONT_WIDTH 17
#define LAYER_STYLE_FONT_SIZE 12

#define LP_EYE_WIDTH 18
#define LP_EYE_HEIGHT 18

#define LP_LAYER_THM_W 40
#define LP_LAYER_THM_H 40

#define LW_CM_COPY_LAYER  10002				//レイヤー複製
#define LW_CM_DELETE_LAYER 10003			//レイヤー削除
#define LW_CM_LOCK_PIXEL_ALPHA	10004		//不透明度保護する
#define LW_CM_UNLOCK_PIXEL_ALPHA 10005		//不透明度を保護しない
#define LW_CM_CLIP_WITH_UNDER_LAYER 10006	//下のレイヤーでクリッピングする
#define LW_CM_UNCLIP_WITH_UNDER_LAYER 10007	//下のレイヤーでクリッピングしない
#define LW_CM_MERGE_UNDER_LAYER 10008		//下のレイヤーと結合
#define LW_CM_MERGE_LAYER_GROUP 10009		//レイヤーグループを結合
#define LW_CM_MERGE_DISPLAY_LAYER 10010		//表示レイヤーを結合
#define LW_CM_ADJUST_PANEL 10100			//レイヤーパネル位置、状態を再表示

class CLayerPanel :
	public ImgLayerEventListener
{
public:
	CLayerPanel(HWND hParent);
	virtual ~CLayerPanel();

	void SetLayer(IImgLayer_Ptr pLayer);
	void SetLayerNameEdit(CLayerNameEdit* pEdit);

	////////////////////////////////////////////
	/*!
		レイヤー更新イベント時に呼び出される。
	*/
	void OnUpdateLayer();

	void OnChangedLayerName(const char* new_name);
	void OnChangedLayerOpacity(uint8_t new_opacity);
	void OnChangedLayerSynthFourCC(uint32_t new_fourcc);

	/////////////////////////////////////////////////////
	/*!
		パネルを選択状態にする。
	*/
	void Select();

	////////////////////////////////////////////////
	/*!
		パネルを非選択状態にする。
	*/
	void Newtral();

	////////////////////////////////////////////////
	/*!
		パネルをマウスオン状態にする。
	*/
	void OnMouse();

	////////////////////////////////////////////////
	/*!
		パネルをマウスオフ状態にする。
	*/
	void LeaveMouse();

	//////////////////////////////////////////////////
	///*!
	//	不透明度をセットする
	//	@param[in] alpha 不透明度
	//*/
	//void SetOpacity(int alpha);

	//////////////////////////////////////////////
	/*!
		レイヤースタイルダイアログを開く
	*/
	void OpenLayerStyleDialog(HWND hParent);

	//////////////////////////////////////////////
	/*!
		レイヤー削除
	*/
	void RemoveLayer();

	//////////////////////////////////////////////
	/*!
		ポップアップメニュー表示
		@param[in] hParent
		@param[in] lpt スクリーン座標
	*/
	virtual void PopupMenu(HWND hParent, const LPPOINT lpt);

	//////////////////////////////////////////////
	/*!
		レイヤーオブジェクトへのポインタを返す。
		@return レイヤーオブジェクトへのポインタ
	*/
	inline IImgLayer_Ptr GetLayer(){
		return m_Layer;
	}

	void UpdateLayerThm();


	void SetPanelRect(const LPRECT lprc);
	void GetPanelRect(LPRECT lprc);
	virtual void DrawPanel(CDC* pDC);

	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);

protected:
	afx_msg void OnTimer(UINT_PTR uIDEvent);

	void DrawBackGround(CDC* pDC);

	HWND m_hParentWnd;
	IImgLayer_Ptr m_Layer;

	RECT m_EyeRect;
	RECT m_LayerThmRect;
	RECT m_LayerStyleTxtRect;
	RECT m_LayerNameTxtRect;
	RECT m_LayerPanelRect;

private: 


	IplImage* m_LayerThm;

	CBitmap m_cEye;	//可視
	CBitmap m_cNEye;//不可視

	CFont m_LayerNameFont;
	CLayerNameEdit* m_pLayerNameEdit;
	
	bool m_IsSelect;
	bool m_IsOnMouse;

	UINT_PTR m_nUpdateThmTimer;
	UINT_PTR m_nOffMouseTimer;
};
