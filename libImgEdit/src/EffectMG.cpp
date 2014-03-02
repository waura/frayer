#include "stdafx.h"

#include "EffectMG.h"
#include "OutputError.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


class EffectXmlVisitor : public TiXmlVisitor
{
public:

	void SetEffectGroupMap(EffectGroupMap* pEffectGroupMap){
		m_pEffectGroupMap = pEffectGroupMap;
	}

	virtual bool VisitEnter(const TiXmlElement& element, const TiXmlAttribute* firstAttribute)
	{
		if (element.ValueStr() == "ieeffect") {
		}
		else if (element.ValueStr() == "effect_group") {
			m_pNowEffectMap = EffectMap_Ptr(new EffectMap);
			VisitIEToolGroupAttr(firstAttribute, m_pNowEffectMap);
		}
		else if (element.ValueStr() == "effect") {
			VisitIEToolAttr(firstAttribute);
		}
		else {
			OutputError::PushLog(
				LOG_LEVEL::_WARN, "effect config xml: unkown tag ", element.ValueStr().c_str());
		}
		return true;
	}

	virtual bool VisitExit(const TiXmlDocument& doc)
	{
		return true;
	}

private:
	void VisitIEToolGroupAttr(const TiXmlAttribute* attr, EffectMap_Ptr pEffectMap)
	{
		while (attr) {
			if (strcmp(attr->Name(), "name") == 0) {
				std::string name_s = babel::auto_translate<>(attr->ValueStr().c_str());
				m_pEffectGroupMap->insert(EffectGroupMap::value_type(name_s.c_str(), pEffectMap));
				return;
			}
			attr = attr->Next();
		}
	}

	void VisitIEToolAttr(const TiXmlAttribute* attr)
	{
		std::string path_s;
		while (attr) {
			if (strcmp(attr->Name(), "path") == 0) {
				path_s = attr->ValueStr();
			}
			attr = attr->Next();
		}

		if (m_pNowEffectMap != NULL) {
			HANDLE hLibrary = ::LoadLibrary(path_s.c_str());
			if (!hLibrary) {
				OutputError::Alert("error: EffectMG::Init() LoadLibrary(), ", path_s.c_str());
				return;
			}

			//dllから関数呼び出し
			_GetEffectDisplayName pGetEffectDisplayName;
			pGetEffectDisplayName = (_GetEffectDisplayName)::GetProcAddress((HMODULE)hLibrary, "GetEffectDisplayNameJa");
			if (!pGetEffectDisplayName) {
				OutputError::Alert("error: EffectMG::Init() GetProcAddress(), GetEffectDisplayNameJa in ", path_s.c_str());
				::FreeLibrary((HMODULE)hLibrary);
				return;
			}

			char display_name[MAX_IE_MODULE_DISPLAY_NAME];
			pGetEffectDisplayName(display_name);

			::FreeLibrary((HMODULE)hLibrary);

			m_pNowEffectMap->insert(EffectMap::value_type(display_name, path_s));
		}
	}

	EffectMap_Ptr m_pNowEffectMap;
	EffectGroupMap* m_pEffectGroupMap;
};


void EffectMG::SetStartDir(const char* path)
{
	m_IEStartDir = path;
}

void EffectMG::Init(const char* path)
{
	m_EffectXmlFilePath = path;

	char data_file_path[_MAX_PATH];
	if (m_IEStartDir == "") {
		wsprintf(data_file_path, "%s", m_EffectXmlFilePath.c_str());
	}
	else {
		wsprintf(data_file_path, "%s\\%s", m_IEStartDir.c_str(), m_EffectXmlFilePath.c_str());
	}

	//
	if (!PathFileExists(data_file_path)) {
		MakeDefaultEffectlXmlFile(data_file_path);
	}

	TiXmlDocument doc( data_file_path );
	if (doc.LoadFile()) {
		EffectXmlVisitor visitor;
		visitor.SetEffectGroupMap(&m_EffectGroupMap);
		doc.Accept( &visitor );
	}
	else {
		OutputError::PushLog(LOG_LEVEL::_WARN, "faild load effect config xml file");
	}
}

void EffectMG::End()
{
	CreateEffectMap::iterator itr = m_CreateEffectMap.begin();
	for(; itr != m_CreateEffectMap.end(); itr++){
		IEffect* pEffect = (*itr).first;
		//エフェクトの開放
		_ReleaseIEEffect pReleaseIEEffect;
		pReleaseIEEffect = (_ReleaseIEEffect)::GetProcAddress((HMODULE)((*itr).second), "ReleaseIEEffect");
		if(!pReleaseIEEffect){
			OutputError::Alert("error: EffectMG::ReleaseEffect() GetProcAddress()");
			break;
		}
		pReleaseIEEffect(&pEffect);

		//dll開放
		::FreeLibrary((HMODULE)((*itr).second));
	}
	m_CreateEffectMap.clear();
}

//////////////////////////////
/*!
	ツリービューにアイテムを追加する。
	@param[in] hTree ツリービューのハンドル
*/
void EffectMG::SetEditTree(HWND hTree)
{
	//
	HTREEITEM hParent=TVI_ROOT;
	TV_INSERTSTRUCT tv;
	memset((char *)&tv, '\0', sizeof(tv));
	tv.hInsertAfter = TVI_LAST;
	tv.item.mask = TVIF_TEXT;
	tv.hParent = TVI_ROOT;

	EffectGroupMap::iterator gitr = m_EffectGroupMap.begin();
	for (; gitr != m_EffectGroupMap.end(); gitr++) {
		char gname[MAX_PATH];
		wsprintf(gname, "%s", (*gitr).first.c_str());
		tv.hParent = TVI_ROOT;
		tv.item.pszText = gname;
		hParent = TreeView_InsertItem(hTree, &tv);

		EffectMap_Ptr pEffectMap = (*gitr).second;
		EffectMap::iterator itr = pEffectMap->begin();
		for (; itr != pEffectMap->end(); itr++) {
			char name[MAX_PATH];
			wsprintf(name, "%s", (*itr).first.c_str());
			tv.hParent = hParent;
			tv.item.pszText = name;
			TreeView_InsertItem(hTree, &tv);
		}
	}

	return;
}

///////////////////////////////
/*!
	ツリービューにアイテムを追加する。
	@param[in] pTree ツリービューコントロールクラス
*/
//void EffectMG::SetEditTree(CTreeCtrl *pCTree)
//{
//	//
//	HTREEITEM hParent=TVI_ROOT;
//	TV_INSERTSTRUCT tv;
//	memset((char *)&tv, '\0', sizeof(tv));
//	tv.hInsertAfter = TVI_LAST;
//	tv.item.mask = TVIF_TEXT;
//	tv.hParent = TVI_ROOT;
//
//	//ファイルからツリー情報読み出し
//	std::ifstream ifs(EFFECT_TREE_DATA);
//	std::string buf;
//	while(ifs && getline(ifs, buf)){
//		if(buf[0] == '-' && buf[1] == '-'){
//			int p;
//			char name[MAX_PATH];
//
//			//buf.erase(buf.size()-1, 1); //改行文字消去
//			p = buf.find_first_of("=");
//			std::string name_s = buf.substr(2, p - 2);
//			std::string mod_s = buf.substr(p+1, buf.size()-p);
//
//			//エフェクトマップに追加
//			m_EffectMap.insert(EffectMap::value_type(name_s, mod_s));
//
//			wsprintf(name, "%s", name_s.c_str());
//			tv.hParent = hParent;
//			tv.item.pszText = name;
//			pCTree->InsertItem(&tv);
//		}
//		else{
//			if(buf.size() != 0){
//				char name[MAX_PATH];
//				wsprintf(name, "%s", buf.c_str());
//				//buf.erase(buf.size()-1, 1); //改行文字消去
//				tv.hParent = TVI_ROOT;
//				tv.item.pszText = name;
//				hParent = pCTree->InsertItem(&tv);
//			}
//		}
//	}
//
//	ifs.close();
//
//	return;
//}

///////////////////////////////
/*!
	エフェクトの名前を与え、エフェクトオブジェクトを生成しポインタを返す。
	生成したオブジェクトは EffectMG::ReleaseEffect() で開放する
	@param[in] effect_name エフェクト名
*/
IEffect* const EffectMG::CreateEffect(const char *effect_name)
{
	EffectGroupMap::iterator gitr = m_EffectGroupMap.begin();
	for (; gitr != m_EffectGroupMap.end(); gitr++) {
		EffectMap_Ptr pEffectMap = (*gitr).second;
		EffectMap::iterator itr = pEffectMap->find(effect_name);
		if (itr != pEffectMap->end()) {
			HANDLE hLibrary = ::LoadLibrary((*itr).second.c_str());
			if(!hLibrary){
				OutputError::Alert("error: EffectMG::CreateEffect() LoadLibrary()");
				break;
			}

			//dllからエフェクトインスタンスの生成
			_CreateIEEffect pCreateIEEffect;
			pCreateIEEffect = (_CreateIEEffect)::GetProcAddress((HMODULE)hLibrary, "CreateIEEffect");
			if(!pCreateIEEffect){
				OutputError::Alert("error: EffectMG::CreateEffect() GetProcAddress()");
				break;
			}
			
			IEffect *ie = pCreateIEEffect();
			m_CreateEffectMap.insert(CreateEffectMap::value_type(ie, hLibrary));
			return ie;
		}
	}
	return NULL;
}

///////////////////////////////
/*!
	生成したエフェクトの開放
	@param[in] pEffect 開放するエフェクト
*/
void EffectMG::ReleaseEffect(IEffect *pEffect)
{
	if(!pEffect) return;

	CreateEffectMap::iterator itr = m_CreateEffectMap.begin();
	for(; itr != m_CreateEffectMap.end(); itr++){
		if(pEffect == (*itr).first){
			//エフェクトの開放
			_ReleaseIEEffect pReleaseIEEffect;
			pReleaseIEEffect = (_ReleaseIEEffect)::GetProcAddress((HMODULE)((*itr).second), "ReleaseIEEffect");
			if(!pReleaseIEEffect){
				OutputError::Alert("error: EffectMG::ReleaseEffect() GetProcAddress()");
				break;
			}
			pReleaseIEEffect(&pEffect);

			//dll開放
			::FreeLibrary((HMODULE)((*itr).second));

			m_CreateEffectMap.erase(itr);
			break;
		}
	}
}

void EffectMG::MakeDefaultEffectlXmlFile(const char* effect_xml_path)
{
	FILE* fp = NULL;
	if (fp = fopen(effect_xml_path, "w")) {
		char buffer[] = {
			"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
			"<ieeffect>\n"
			"  <effect_group name=\"2値化\">\n"
			"    <effect path=\".\\data\\effect_dll\\E_Binary_Threshold.dll\" />\n"
			"  </effect_group>\n"
			"\n"
			"  <effect_group name=\"色調補正\">\n"
			"    <effect path=\".\\data\\effect_dll\\E_Sepia.dll\" />\n"
			"    <effect path=\".\\data\\effect_dll\\E_Saturation_Value.dll\" />\n"
			"  </effect_group>\n"
			"\n"
			"  <effect_group name=\"画像効果\">\n"
			"    <effect path=\".\\data\\effect_dll\\E_ColorReverse.dll\" />\n"
			"    <effect path=\".\\data\\effect_dll\\E_SoftForcus.dll\" />\n"
			"    <effect path=\".\\data\\effect_dll\\E_Diffuse.dll\" />\n"
			"    <effect path=\".\\data\\effect_dll\\E_VTRFilter.dll\" />\n"
			"  </effect_group>\n"
			"\n"
			"  <effect_group name=\"エッジ抽出\">\n"
			"    <effect path=\".\\data\\effect_dll\\E_EdgeSobel.dll\" />\n"
			"    <effect path=\".\\data\\effect_dll\\E_EdgeLaplacian.dll\" />\n"
			"  </effect_group>\n"
			"\n"
			"  <effect_group name=\"ぼかし\">\n"
			"    <effect path=\".\\data\\effect_dll\\E_GaussianBlur.dll\" />\n"
			"    <effect path=\".\\data\\effect_dll\\E_MedianFilter.dll\" />\n"
			"    <effect path=\".\\data\\effect_dll\\E_BilateralFilter.dll\" />\n"
			"    <effect path=\".\\data\\effect_dll\\E_MotionBlur.dll\" />\n"
			"  </effect_group>\n"
			"\n"
			"  <effect_group name=\"画像補正\">\n"
			"    <effect path=\".\\data\\effect_dll\\E_Inpaint.dll\" />\n"
			"  </effect_group>\n"
			"</ieeffect>\n"
		};

		std::string buf = babel::auto_translate<>(buffer, babel::base_encoding::utf8);
		fprintf(fp, buf.c_str());
		fclose(fp);
	}
	else {
		OutputError::Alert(IE_ERROR_EFFECT_CONFIG_FILE_CANT_WRITE, effect_xml_path);
	}
}