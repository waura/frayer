#ifndef _IECOMMAND_H_
#define _IECOMMAND_H_

#include "LibIEDllHeader.h"
#include "IIEGuiFrame.h"

class ImgEdit;

//class _EXPORTCPP IIECommand : public IIEModule
class _EXPORTCPP IIECommand : public IIEGuiFrame
{
public:
	IIECommand(){};
	virtual ~IIECommand(){};


	////////////////////////////////
	/*!
		コマンド開始時に一度呼ばれる
	*/
	virtual bool Init(ImgEdit* pEdit){
		return true;
	}

	////////////////////////////////
	/*!
		コマンド終了時に一度呼ばれる
	*/
	virtual void End(ImgEdit* pEdit){}

	////////////////////////////////
	/*!
		アイドル時毎回呼び出し
		@param[in] pEdit
		@return FALSEを返すとコマンドを終了する、TRUEの場合は継続する
	*/
	virtual BOOL Run(ImgEdit* pEdit, void* pvoid){ return FALSE; };

	//////////////////////////////////
	/*!
		編集中の画像ウィンドウ描画時に呼び出し
		@param[in] hdc 描画するデバイスコンテキスト
	*/
	virtual void OnDraw(HDC hdc){};
};


#ifdef __cplusplus
#define IECOMMAND_EXPORT extern "C" __declspec (dllexport)
#else
#define IECOMMAND_EXPORT __declspec (dllexport)
#endif

IECOMMAND_EXPORT IEMODULE_ID GetIEModule_ID();

IECOMMAND_EXPORT void GetIECommandName(char* dst);
IECOMMAND_EXPORT void GetIECommandDisplayNameJa(char* dst);

IECOMMAND_EXPORT IIECommand* CreateIECommand();
IECOMMAND_EXPORT void ReleaseIECommand(IIECommand** ppCommand);

#endif //_IECOMMAND_H_