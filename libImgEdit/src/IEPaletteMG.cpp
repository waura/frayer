#include "stdafx.h"

#include "IEPaletteMG.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


class IEPaletteXmlVisitor : public TiXmlVisitor
{
public:
	IEPaletteXmlVisitor()
	{
		m_pIEPaletteMG = NULL;
		m_pNowPalette = NULL;
	}
	void SetIEPaletteMG(IEPaletteMG* pPaletteMG){
		m_pIEPaletteMG = pPaletteMG;
	}

	virtual bool VisitEnter(const TiXmlElement& element, const TiXmlAttribute* firstAttribute)
	{
		if (element.ValueStr() == "iepalette") {
		}
		else if (element.ValueStr() == "palette") {
			m_pNowPalette = CreateIEPalette();
			VisitIEPaletteAttr(firstAttribute, m_pNowPalette);

			if (m_pIEPaletteMG) {
				m_pIEPaletteMG->AddPalette( m_pNowPalette );
			}
		}
		else if (element.ValueStr() == "color") {
			if (m_pNowPalette) {
				VisitIEPaletteNodeAttr(firstAttribute, m_pNowPalette);
			}
		}
		else{
			TCHAR log[512];
			wsprintf(log, "palette config xml: unkown tag %s", element.ValueStr().c_str());
			OutputError::PushLog(LOG_LEVEL::_WARN, log);
		}
		return true;
	}

private:
	void VisitIEPaletteAttr(const TiXmlAttribute* attr, IEPalette_Ptr pPalette)
	{
		while( attr ){
			if(strcmp(attr->Name(), "name") == 0){
				pPalette->SetName( attr->ValueStr().c_str() );
			}
			else{
				TCHAR log[512];
				wsprintf(log, "palette config xml: unkown attribute %s", attr->ValueStr().c_str());
				OutputError::PushLog(LOG_LEVEL::_WARN, log);
			}
			attr = attr->Next();
		}
	}

	void VisitIEPaletteNodeAttr(const TiXmlAttribute* attr, IEPalette_Ptr pPalette)
	{
		int nattr = 0;
		std::string name;
		IEColor color;
		while( attr ){
			if (strcmp(attr->Name(), "name") == 0) {
				name = attr->ValueStr();
				nattr++;
			}
			else if (strcmp(attr->Name(), "value") == 0) {
				int val;
				if (attr->QueryIntValue(&val) == TIXML_SUCCESS) {
					color.value = val;
					nattr++;
				}
			}
			else {
				TCHAR log[512];
				wsprintf(log, "palette config xml: unkown attribute %s", attr->ValueStr().c_str());
				OutputError::PushLog(LOG_LEVEL::_WARN, log);
			}
			attr = attr->Next();
		}

		if (nattr == 2) {
			pPalette->AddColor(name.c_str(), color);
		} else {
			OutputError::PushLog(LOG_LEVEL::_WARN, "palette config xml: invalid attribute");
		}
	}

	IEPaletteMG* m_pIEPaletteMG;
	IEPalette_Ptr m_pNowPalette;
};

IEPaletteMG::IEPaletteMG()
{
	m_pSelectPalette = NULL;
	m_LockResourceID = 0;
}

IEPaletteMG::~IEPaletteMG()
{
	End();
}

void IEPaletteMG::Init(const char* path)
{
	m_isInit = true;
	m_PaletteXmlFilePath = path;
	TiXmlDocument doc( path );
	if( doc.LoadFile() ){
		IEPaletteXmlVisitor visitor;
		visitor.SetIEPaletteMG( this );
		doc.Accept( &visitor );
	}
	else{
		OutputError::PushLog(LOG_LEVEL::_INFO, "faild load palette config xml file");

		IEPalette_Ptr pPalette = CreateIEPalette();
		pPalette->SetName("プリセット");
		AddPalette(pPalette);
	}
}

void IEPaletteMG::End()
{
	if(m_isInit == true){
		FILE* fp;
		if(fp = fopen(m_PaletteXmlFilePath.c_str(), "w")){
			fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
			fprintf(fp, "<iepalette>\n");
			IEPalette_Vec::const_iterator itr = m_IEPalette_Vec.begin();
			for(; itr != m_IEPalette_Vec.end(); itr++){
				(*itr)->WritePaletteToXml(fp, 1);
			}
			fprintf(fp, "</iepalette>\n");
			fclose( fp );
		}
		else{
			OutputError::PushLog(LOG_LEVEL::_ERROR, IE_ERROR_PALETTE_CONFIG_FILE_CANT_WRITE);
		}

		//IEPalette_Vec::iterator itr = m_IEPalette_Vec.begin();
		//for(; itr != m_IEPalette_Vec.end(); itr++){
		//	IEPalette_Ptr pPalette = (*itr);
		//	ReleaseIEPalette( &pPalette );
		//}
		m_IEPalette_Vec.clear();

		m_isInit = false;
	}
}

/////////////////////////////
/*!
	イベントリスナーを登録する。
	@param[in] pListener 登録するリスナー
*/
void IEPaletteMG::AddEventListener(IEPaletteMGEventListener*  pListener)
{
	m_IEPaletteMGEventListener_List.push_back( pListener );
}

/////////////////////////////
/*!
	イベントリスナーを削除する。
	@param[in] pListener 削除するリスナー
*/
void IEPaletteMG::DeleteEventListener(IEPaletteMGEventListener*  pListener)
{
	IEPaletteMGEventListener_List::iterator itr = m_IEPaletteMGEventListener_List.begin();
	for(; itr != m_IEPaletteMGEventListener_List.end(); itr++){
		if((*itr) == pListener){
			m_IEPaletteMGEventListener_List.erase(itr);
			return;
		}
	}
}

void IEPaletteMG::AddPalette(IEPalette_Ptr pPalette)
{
	ResourceLock( IE_PALETTE_MG_RESOURCE_ID::IE_PALETTE );
	m_IEPalette_Vec.push_back( pPalette );
	ResourceUnlock( IE_PALETTE_MG_RESOURCE_ID::IE_PALETTE );

	//call event
	IEPaletteMGEventListener_List::iterator itr = m_IEPaletteMGEventListener_List.begin();
	for(; itr != m_IEPaletteMGEventListener_List.end(); itr++){
		if((*itr)->IsLockIEPaletteMGEvent() == false && (*itr)->IsCalledIEPaletteMG() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledIEPaletteMG(true);
			//
			(*itr)->OnAddPalette( pPalette );
		}

		(*itr)->SetCalledIEPaletteMG(false);
	}

	//
	if(m_pSelectPalette == NULL){
		SetSelectPalette( pPalette );
	}
}

void IEPaletteMG::DeletePalette(IEPalette_Ptr pPalette)
{
	if(pPalette == NULL) return;

	IEPaletteMGEventListener_List::iterator itr = m_IEPaletteMGEventListener_List.begin();
	for(; itr != m_IEPaletteMGEventListener_List.end(); itr++){
		if((*itr)->IsLockIEPaletteMGEvent() == false && (*itr)->IsCalledIEPaletteMG() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledIEPaletteMG(true);
			//
			(*itr)->OnDeletePalette( pPalette );
		}

		(*itr)->SetCalledIEPaletteMG(false);
	}

	if(m_pSelectPalette == pPalette){
	}

	ResourceLock( IE_PALETTE_MG_RESOURCE_ID::IE_PALETTE );

	//ブラシグループ配列から削除
	IEPalette_Vec::iterator itr_br = m_IEPalette_Vec.begin();
	for(; itr_br != m_IEPalette_Vec.end(); itr_br++){
		if(pPalette == (*itr_br)){
			m_IEPalette_Vec.erase(itr_br);
			break;
		}
	}

	//ReleaseIEPalette( &pPalette );

	ResourceUnlock( IE_PALETTE_MG_RESOURCE_ID::IE_PALETTE );
}

void IEPaletteMG::SetSelectPalette(IEPalette_Ptr pPalette)
{
	//call event
	IEPaletteMGEventListener_List::iterator itr = m_IEPaletteMGEventListener_List.begin();
	for(; itr != m_IEPaletteMGEventListener_List.end(); itr++){
		if((*itr)->IsLockIEPaletteMGEvent() == false && (*itr)->IsCalledIEPaletteMG() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledIEPaletteMG(true);
			//
			(*itr)->OnChangeSelectPalette(pPalette, m_pSelectPalette);
		}

		(*itr)->SetCalledIEPaletteMG(false);
	}

	ResourceLock( IE_PALETTE_MG_RESOURCE_ID::IE_PALETTE );
	m_pSelectPalette = pPalette;
	ResourceUnlock( IE_PALETTE_MG_RESOURCE_ID::IE_PALETTE );
}

///////////////////////////////
/*!
	indexを指定してパレットを指定する
	@param[in] index
*/
void IEPaletteMG::SetSelectPalette(int index)
{
	assert(0 <= index && index < m_IEPalette_Vec.size());
	if (0 <= index && index < m_IEPalette_Vec.size()) {
		SetSelectPalette( m_IEPalette_Vec[index] );
	}
}

///////////////////////////////
/*!
	すべてのパレット名をコンボボックスに追加する
	@param[in] hCombo
*/
void IEPaletteMG::SetPaletteComboBox(HWND hCombo)
{
	//
	IEPalette_Vec::const_iterator itr = m_IEPalette_Vec.begin();
	for(; itr != m_IEPalette_Vec.end(); itr++){
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)(*itr)->GetName());
	}

	return;
}