#ifndef _IEFFECT_H_
#define _IEFFECT_H_

#include "LibIEDllHeader.h"

#include "IEModules.h"
#include "IIECommand.h"
#include "IEffectEditDialog.h"
#include "ImgFile.h"

class _EXPORTCPP IEffect
{
public:
	IEffect(){};
	virtual ~IEffect(){};

	////名前を返す
	//virtual std::string GetName() = 0;

	////////////////////////////////
	/*!
		エフェクト開始時に一度呼ばれる
	*/
	virtual bool Init(ImgFile_Ptr wpFile){
		return true;
	}
	////////////////////////////////
	/*!
		エフェクト終了時に一度呼ばれる
	*/
	virtual void End(ImgFile_Ptr pFile){};

	//レイヤーにエフェクトをかける。
	virtual void Edit(ImgFile_Ptr pFile) = 0;
	
	//エディットウィンドウを設定する
	virtual void SetEditDialog(IEffectEditDialog *pEditDialog) = 0;

private:
	std::string name;

};

#ifdef __cplusplus
#define EFFECT_EXPORT extern "C" __declspec (dllexport)
#else
#define EFFECT_EXPORT __declspec (dllexport)
#endif

EFFECT_EXPORT IEffect* CreateIEEffect();
EFFECT_EXPORT void ReleaseIEEffect(IEffect** ppEffect);
EFFECT_EXPORT IEMODULE_ID GetIEModule_ID();

EFFECT_EXPORT void GetEffectDisplayNameJa(char* dst);

#endif //_IEFFECT_H_
