#ifndef _IIEGUIFRAME_H_
#define _IIEGUIFRAME_H_

#include "LibIEDllHeader.h"

#include "InputData.h"
#include "IEKeyCord.h"
#include "IEModules.h"

enum IE_INPUT_DATA_FLAG{
	IMAGE_POS,			//画像上の座標
	WINDOW_POS,			//ウィンドウクライアント座標
};

//GUIイベント呼び出し用インターフェース
class _EXPORTCPP IIEGuiFrame : public IIEModule
{
public:
	IIEGuiFrame(){};
	virtual ~IIEGuiFrame(){};

	///////////////////////////////
	/*!
		入力の補間、補正をするかしないかを返す。
		trueの場合はする、falseの場合はしない
		デフォルトではfalse
	*/
	virtual bool isRevise(){ return false; }

	////////////////////////////////
	/*!
		入力を補間する場合の点の間隔(pixel単位で)
		デフォルトでは1.0
	*/
	virtual double GetReviseStepSize(const LPIE_INPUT_DATA){ return 1.0; }

	////////////////////////////////
	/*!
		マウスの入力座標として送られてくるもの座標系を決める。
		デフォルトではウィンドウのクライアント座標
	*/
	virtual IE_INPUT_DATA_FLAG GetInputDataFlag(){ return WINDOW_POS; }

	//////////////////////////////////
	/*!
		編集中の画像ウィンドウ描画時に呼び出し
		@param[in] hdc 描画するデバイスコンテキスト
	*/
	virtual void OnDraw(HDC hdc){};

	//////////////////////////////////
	/*!
		マウスが動いたときに呼び出し
		@param[in] lpd マウス情報
	*/
	virtual void OnMouseMove(UINT nFlags, const LPIE_INPUT_DATA lpd){};

	//////////////////////////////////
	/*!
		マウス左ボタン押下時に呼び出し。
		@param[in] lpd マウス情報
	*/
	virtual void OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd){};
	
	//////////////////////////////////
	/*!
		マウス左ボタンを離した時に呼び出し。
		@param[in] lpd マウス情報
	*/
	virtual void OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd){};

	///////////////////////////////////
	/*!
		マウス左ボタンドラッグ時に呼び出し。
		@param[in] lpd マウス情報
	*/
	virtual void OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd){};
	
	///////////////////////////////////
	/*!
		マウス右ボタン押下時に呼び出し
		@param[in] lpd マウス情報
	*/
	virtual void OnMouseRButtonDown(UINT nFlags, const LPPOINT lpt){};

	//////////////////////////////////
	/*!
		マウス右ボタンドラッグ時に呼び出し。
	*/
	virtual void OnMouseRDrag(UINT nFlags, const LPPOINT lpt){};

	//////////////////////////////////
	/*!
		マウス左ボタンを離した時に呼び出し。
		@param[in] lpd マウス情報
	*/
	virtual void OnMouseRButtonUp(UINT nFlags, const LPPOINT lpt){};

	////////////////////////////////
	/*!
		キーボードのキーが押されたときに呼び出し
		@param[in] nChar 押されたキーの仮想コード
		@param[in] optChar 同時に押されているCtrl,Shiftキーなどの情報
	*/
	virtual void OnKeyboardDown(UINT nChar, UINT optChar){};
};

#endif //_IIEGUIFRAME_H_