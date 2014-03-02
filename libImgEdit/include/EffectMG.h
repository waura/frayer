#ifndef _EFFECTMG_H_
#define _EFFECTMG_H_

#include <map>

#include "IEffect.h"

typedef std::map<std::string, std::string> EffectMap;
typedef std::shared_ptr<EffectMap> EffectMap_Ptr;
typedef std::map<std::string, EffectMap_Ptr> EffectGroupMap;
typedef std::map<IEffect*, HANDLE> CreateEffectMap;

typedef IEffect* (*_CreateIEEffect)();
typedef void (*_ReleaseIEEffect) (IEffect**);
typedef void (*_GetEffectDisplayName)(char*);

class _EXPORTCPP EffectMG {
public:

	void SetEffectDir(const char* path);
	void SetEffectDat(const char* path);

	void SetStartDir(const char* path);

	void Init(const char* path);
	void End();

	//////////////////////////////
	/*!
		ツリービューにアイテムを追加する。
		@param[in] hTree ツリービューのハンドル
	*/
	void SetEditTree(HWND hTree);

	///////////////////////////////
	/*!
		ツリービューにアイテムを追加する。
		@param[in] pTree ツリービューコントロールクラス
	*/
	//static void SetEditTree(CTreeCtrl *pCTree);

	///////////////////////////////
	/*!
		エフェクトの名前を与え、エフェクトオブジェクトを生成しポインタを返す。
		生成したオブジェクトは EffectMG::ReleaseEffect() で開放する
		@param[in] effect_name エフェクト名
	*/
	IEffect* const CreateEffect(const char* effect_name);

	///////////////////////////////
	/*!
		生成したエフェクトの開放
		@param[in] pEffect 開放するエフェクト
	*/
	void ReleaseEffect(IEffect *pEffect);

private:

	void MakeDefaultEffectlXmlFile(const char* effect_xml_path);

	std::string m_IEStartDir;
	std::string m_EffectXmlFilePath;

	EffectGroupMap m_EffectGroupMap;
	CreateEffectMap m_CreateEffectMap; //生成したエフェクトとモジュールのマップ
};

#endif
