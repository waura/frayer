#include "stdafx.h"

#include "RLE.h"
#include "Base64.h"
#include "IEBrushMG.h"
#include "IEBrushIO.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

class IEBrushXmlVisitor : public TiXmlVisitor
{
public:
	void SetIEBrushMG(IEBrushMG* pBrushMG){
		m_pIEBrushMG = pBrushMG;
	}

	virtual bool VisitEnter(const TiXmlElement& element, const TiXmlAttribute* firstAttribute)
	{
		if (element.ValueStr() == "iebrush") {
		}
		else if (element.ValueStr() == "brush_group") {
			m_pNowBrushGroup = CreateIEBrushGroup();
			VisitIEBrushGroupAttr(firstAttribute, m_pNowBrushGroup);

			m_pIEBrushMG->AddBrushGroup( m_pNowBrushGroup );
		}
		else if (element.ValueStr() == "brush") {
			m_pNowBrush = VisitIEBrushAttr( firstAttribute );
			m_pNowBrushGroup->AddBrush( m_pNowBrush );
		}
		else if (element.ValueStr() == "radius_ctrl") {
			int val = GetIntValAttr( firstAttribute );
			m_pNowBrush->SetRadiusCtrlID( (IE_BRUSH_CTRL_ID)val );
		}
		else if (element.ValueStr() == "alpha_ctrl") {
			int val = GetIntValAttr( firstAttribute );
			m_pNowBrush->SetAlphaCtrlID( (IE_BRUSH_CTRL_ID)val );
		}
		else if (element.ValueStr() == "min_radius") {
			int val = GetIntValAttr( firstAttribute );
			m_pNowBrush->SetMinRadius( val );
		}
		else if (element.ValueStr() == "min_alpha") {
			int val = GetIntValAttr( firstAttribute );
			m_pNowBrush->SetMinAlpha( val );
		}
		else if (element.ValueStr() == "interval") {
			int val = GetIntValAttr( firstAttribute );
			m_pNowBrush->SetInterval( val );
		}
		else if (element.ValueStr() == "texture") {
			const TiXmlAttribute* attr = firstAttribute;
			while (attr) {
				if (strcmp(attr->Name(), "path") == 0) {
					m_pNowBrush->AddTexture(attr->ValueStr().c_str());
				}
				attr = attr->Next();
			}
		}
		else if (element.ValueStr() == "hardness") {
			if (m_pNowBrush && m_pNowBrush->GetBrushType() == IE_EM_BRUSH) {
				int val = GetIntValAttr( firstAttribute );
				IEEMBrush_Ptr pbrush = std::dynamic_pointer_cast<IEEMBrush>(m_pNowBrush);
				pbrush->SetHardness( val );
			}
		}
		else if (element.ValueStr() == "brush_data") {
			if (m_pNowBrush && m_pNowBrush->GetBrushType() == IE_IMG_BRUSH) {
				int width = 0;
				int height = 0;
				int depth = 0;
				const TiXmlAttribute* attr = firstAttribute;
				while (attr) {
					if (strcmp(attr->Name(), "width") == 0) {
						int ival;
						if (attr->QueryIntValue( &ival ) == TIXML_SUCCESS)
							width = ival;
					}
					else if (strcmp(attr->Name(), "height") == 0) {
						int ival;
						if (attr->QueryIntValue( &ival ) == TIXML_SUCCESS)
							height = ival;
					}
					else if (strcmp(attr->Name(), "depth") == 0) {
						int ival;
						if (attr->QueryIntValue( &ival ) == TIXML_SUCCESS)
							depth = ival;
					}
					attr = attr->Next();
				}
				if ((width == 0) || (height == 0) || (depth == 0)) return true;

				const char* base64_data = element.FirstChild()->ToText()->Value();
				size_t rle_brush_data_size = BASE64_DECODE_SIZE(strlen(base64_data));
				std::shared_ptr<char> rle_brush_data = std::shared_ptr<char>(new char[rle_brush_data_size]);
				decode_base64(base64_data, rle_brush_data.get(), &rle_brush_data_size);

				size_t brush_data_size = width * height * depth;
				std::shared_ptr<uchar> brush_data = std::shared_ptr<uchar>(new uchar[brush_data_size]);

				decode_rle(
					(const int8_t*)rle_brush_data.get(),
					rle_brush_data_size,
					brush_data_size,
					(int8_t*)brush_data.get());

				IEImgBrush_Ptr pbrush = std::dynamic_pointer_cast<IEImgBrush>(m_pNowBrush);
				pbrush->CreateBrush(brush_data.get(), width, height, depth);
			}
		}
		else if (element.ValueStr() == "brush_img") {
			if (m_pNowBrush && m_pNowBrush->GetBrushType() == IE_IMG_BRUSH) {
				const char* val = GetTxtValAttr( firstAttribute );
				IEImgBrush_Ptr pbrush = std::dynamic_pointer_cast<IEImgBrush>(m_pNowBrush);
				if (!(pbrush->LoadBrushImg( val ))) {
					TCHAR log[512];
					wsprintf(log, "brush config xml: brush_img load faild %s", val); 
					OutputError::PushLog(LOG_LEVEL::_WARN, log);

					m_pNowBrushGroup->DeleteBrush( m_pNowBrush );
				}
			}
		}
		else {
			TCHAR log[512];
			wsprintf(log, "brush config xml: unkown tag %s", element.ValueStr().c_str());
			OutputError::PushLog(LOG_LEVEL::_WARN, log);
		}

		return true;
	}

	virtual bool VisitExit(const TiXmlDocument& doc)
	{
		return true;
	}

private:
	void VisitIEBrushGroupAttr(const TiXmlAttribute* attr, IEBrushGroup_Ptr pBGroup)
	{
		while( attr ){
			if(strcmp(attr->Name(), "name") == 0){
				pBGroup->SetName( attr->ValueStr().c_str() );
			}
			attr = attr->Next();
		}
	}

	IEBrush_Ptr VisitIEBrushAttr(const TiXmlAttribute* attr)
	{
		int type = -1;
		int radius = 1;
		TCHAR name[MAX_IE_BRUSH_NAME];
		while (attr) {
			if (strcmp(attr->Name(), "type") == 0) {
				if (attr->ValueStr() == "em") {
					type = IE_EM_BRUSH;
				}
				else if (attr->ValueStr() == "img") {
					type = IE_IMG_BRUSH;
				}
			}
			else if (strcmp(attr->Name(), "radius") == 0) {
				int ival;
				if(attr->QueryIntValue( &ival ) == TIXML_SUCCESS)
					radius = ival;
			}
			else if (strcmp(attr->Name(), "name") == 0) {
				strcpy_s(name, MAX_IE_BRUSH_NAME, attr->ValueStr().c_str());
			}
			attr = attr->Next();
		}

		IEBrush_Ptr pBrush=NULL;
		if (type == IE_EM_BRUSH) {
			pBrush = CreateIEEMBrush();
		}
		else if (type == IE_IMG_BRUSH) {
			pBrush = CreateIEImgBrush();
		}

		if (pBrush) {
			pBrush->SetName( name );
			pBrush->SetOrgRadius( radius );
		}

		return pBrush;
	}

	int GetIntValAttr(const TiXmlAttribute* attr)
	{
		while (attr) {
			if (strcmp(attr->Name(), "val") == 0) {
				int ival;
				if (attr->QueryIntValue( &ival ) == TIXML_SUCCESS)
					return ival;
			}
			attr = attr->Next();
		}

		return -1;
	}

	const char* GetTxtValAttr(const TiXmlAttribute* attr)
	{
		while (attr) {
			if (strcmp(attr->Name(), "val") == 0) {
				return attr->ValueStr().c_str();
			}
			attr = attr->Next();
		}

		return NULL;
	}

	IEBrushMG* m_pIEBrushMG;
	IEBrushGroup_Ptr m_pNowBrushGroup;
	IEBrush_Ptr m_pNowBrush;
};


IEBrushMG::IEBrushMG()
{
	m_pSelectBrushGroup = NULL;
	m_LockResourceID = 0;
}

IEBrushMG::~IEBrushMG()
{
	End();
}

void IEBrushMG::SetStartDir(const char* path)
{
	m_IEStartDir = path;
	m_TextureMG.SetStartDir(path);
}


/////////////////////////////
/*!
	イベントリスナーを登録する。
	@param[in] pListener 登録するリスナー
*/
void IEBrushMG::AddEventListener(IEBrushMGEventListener*  pListener)
{
	m_IEBrushMGEventListener_List.push_back( pListener );
}

/////////////////////////////
/*!
	イベントリスナーを削除する。
	@param[in] pListener 削除するリスナー
*/
void IEBrushMG::DeleteEventListener(IEBrushMGEventListener*  pListener)
{
	IEBrushMGEventListener_List::iterator itr = m_IEBrushMGEventListener_List.begin();
	for (; itr != m_IEBrushMGEventListener_List.end(); itr++) {
		if ((*itr) == pListener) {
			m_IEBrushMGEventListener_List.erase(itr);
			return;
		}
	}
}

void IEBrushMG::Init(const char* brushXmlFilePath)
{
	m_isInit = true;
	if (!m_TextureMG.Open(".\\brush_texture")) {
		OutputError::PushLog(LOG_LEVEL::_ERROR, "can't open brush texture dir");
	}

	m_BrushXmlFilePath = brushXmlFilePath;
	TiXmlDocument doc( brushXmlFilePath );
	if (doc.LoadFile()) {
		IEBrushXmlVisitor visitor;
		visitor.SetIEBrushMG( this );
		doc.Accept( &visitor );
	}
	else {
		OutputError::PushLog(LOG_LEVEL::_INFO, "don't exist brush config xml file");

		//set priset brush
		IEBrushGroup_Ptr pBrushGroup = CreateIEBrushGroup();
		pBrushGroup->SetName("プリセット");

		int rad_set[] = {1, 3, 7, 15, 31, 62};
		int nset = sizeof(rad_set) / sizeof(rad_set[0]);

		for (int i = 0; i < nset; i++) {
			char name[256];
			wsprintf(name, "ハード円ブラシ%d", rad_set[i]);
			IEEMBrush_Ptr pBrush = CreateIEEMBrush();
			pBrush->SetName(name);
			pBrush->SetOrgRadius(rad_set[i]);
			pBrush->SetMinRadius(0);
			pBrush->SetMinAlpha(100);
			pBrush->SetHardness(100);
			pBrushGroup->AddBrush(pBrush);
		}

		for (int i = 0; i < nset; i++) {
			char name[256];
			wsprintf(name, "ソフト円ブラシ%d", rad_set[i]);
			IEEMBrush_Ptr pBrush = CreateIEEMBrush();
			pBrush->SetName(name);
			pBrush->SetOrgRadius(rad_set[i]);
			pBrush->SetMinRadius(0);
			pBrush->SetMinAlpha(100);
			pBrush->SetHardness(0);
			pBrushGroup->AddBrush(pBrush);
		}

		for (int i = 0; i < nset; i++) {
			char name[256];
			wsprintf(name, "ハードエアーブラシ%d", rad_set[i]);
			IEEMBrush_Ptr pBrush = CreateIEEMBrush();
			pBrush->SetName(name);
			pBrush->SetOrgRadius(rad_set[i]);
			pBrush->SetMinRadius(50);
			pBrush->SetMinAlpha(0);
			pBrush->SetHardness(100);
			pBrushGroup->AddBrush(pBrush);
		}

		for (int i = 0; i < nset; i++) {
			char name[256];
			wsprintf(name, "ソフトエアーブラシ%d", rad_set[i]);
			IEEMBrush_Ptr pBrush = CreateIEEMBrush();
			pBrush->SetName(name);
			pBrush->SetOrgRadius(rad_set[i]);
			pBrush->SetMinRadius(50);
			pBrush->SetMinAlpha(0);
			pBrush->SetHardness(0);
			pBrushGroup->AddBrush(pBrush);
		}

		AddBrushGroup(pBrushGroup);
	}
}

void IEBrushMG::End()
{
	if (m_isInit == true) {
		FILE* fp;
		if (fp = fopen(m_BrushXmlFilePath.c_str(), "w")) {
			fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");

			fprintf(fp, "<iebrush>\n");
			IEBrushGroup_Vec::const_iterator itr = m_IEBrushGroup_Vec.begin();
			for (; itr != m_IEBrushGroup_Vec.end(); itr++) {
				(*itr)->WriteBrushGroupToXml(fp, 1);
			}
			fprintf(fp, "</iebrush>\n");
			fclose( fp );
		}
		else {
			OutputError::Alert(IE_ERROR_BRUSH_CONFIG_FILE_CANT_WRITE);
		}

		//IEBrushGroup_Vec::iterator itr = m_IEBrushGroup_Vec.begin();
		//for(; itr != m_IEBrushGroup_Vec.end(); itr++){
		//	IEBrushGroup_Ptr pBGroup = (*itr);
		//	ReleaseIEBrushGroup(&pBGroup);
		//}
		m_IEBrushGroup_Vec.clear();

		m_isInit = false;
	}
}

void IEBrushMG::AddBrushGroup(IEBrushGroup_Ptr pBGroup)
{
	ResourceLock( IE_BRUSH_MG_RESOURCE_ID::IE_BRUSH_GROUP );
	//ブラシ配列に追加
	m_IEBrushGroup_Vec.push_back( pBGroup );
	ResourceUnlock( IE_BRUSH_MG_RESOURCE_ID::IE_BRUSH_GROUP );

	//call event
	IEBrushMGEventListener_List::iterator itr = m_IEBrushMGEventListener_List.begin();
	for(; itr != m_IEBrushMGEventListener_List.end(); itr++){
		if((*itr)->IsLockIEBrushMGEvent() == false && (*itr)->IsCalledIEBrushMG() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledIEBrushMG(true);
			//
			(*itr)->OnAddBrushGroup(pBGroup);
		}

		(*itr)->SetCalledIEBrushMG(false);
	}

	//
	if(m_pSelectBrushGroup == NULL){
		SetSelectBrushGroup( pBGroup );
	}
}

void IEBrushMG::SetSelectBrushGroup(IEBrushGroup_Ptr pBGroup)
{
	//call event
	IEBrushMGEventListener_List::iterator itr = m_IEBrushMGEventListener_List.begin();
	for(; itr != m_IEBrushMGEventListener_List.end(); itr++){
		if((*itr)->IsLockIEBrushMGEvent() == false && (*itr)->IsCalledIEBrushMG() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledIEBrushMG(true);
			//
			(*itr)->OnChangeSelectBrushGroup(pBGroup, m_pSelectBrushGroup);
		}

		(*itr)->SetCalledIEBrushMG(false);
	}

	ResourceLock( IE_BRUSH_MG_RESOURCE_ID::IE_BRUSH_GROUP );
	//選択ブラシ切り替え
	m_pSelectBrushGroup = pBGroup;
	ResourceUnlock( IE_BRUSH_MG_RESOURCE_ID::IE_BRUSH_GROUP );
}

void IEBrushMG::SetSelectBrushGroup(int index)
{
	assert(0 <= index && index < m_IEBrushGroup_Vec.size());
	SetSelectBrushGroup( m_IEBrushGroup_Vec[index] );
}

int IEBrushMG::GetSelectBrushGroupIndex()
{
	int index = 0;
	IEBrushGroup_Vec::iterator itr = m_IEBrushGroup_Vec.begin();
	for(; itr != m_IEBrushGroup_Vec.end(); itr++){
		if(m_pSelectBrushGroup == (*itr)){
			return index;
			break;
		}
		index++;
	}
	return -1;
}

void IEBrushMG::DeleteBrushGroup(IEBrushGroup_Ptr pBGroup)
{
	if(pBGroup == NULL) return;

	IEBrushMGEventListener_List::iterator itr = m_IEBrushMGEventListener_List.begin();
	for(; itr != m_IEBrushMGEventListener_List.end(); itr++){
		if((*itr)->IsLockIEBrushMGEvent() == false && (*itr)->IsCalledIEBrushMG() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledIEBrushMG(true);
			//
			(*itr)->OnDeleteBrushGroup(pBGroup);
		}

		(*itr)->SetCalledIEBrushMG(false);
	}

	if(m_pSelectBrushGroup == (pBGroup)){
		SetSelectBrushGroup(NULL);
	}

	ResourceLock( IE_BRUSH_MG_RESOURCE_ID::IE_BRUSH_GROUP );

	//delete from brush group vector
	IEBrushGroup_Vec::iterator itr_br = m_IEBrushGroup_Vec.begin();
	for(; itr_br != m_IEBrushGroup_Vec.end(); itr_br++){
		if(pBGroup == (*itr_br)){
			m_IEBrushGroup_Vec.erase(itr_br);
			break;
		}
	}
	//ReleaseIEBrushGroup(pBGroup);

	ResourceUnlock( IE_BRUSH_MG_RESOURCE_ID::IE_BRUSH_GROUP );
}

void IEBrushMG::LoadBrush(const char* filename)
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	_splitpath_s(
		filename,
		drive,
		_MAX_DRIVE,
		dir,
		_MAX_DIR,
		fname,
		_MAX_FNAME,
		ext,
		_MAX_EXT);

	if (strcmp(ext, ".ieb") == 0) {//ImgEdtiブラシファイル
		IEBrush_Ptr pBrush = IEBrushIO::LoadIEBrush(filename);
		AddBrush( pBrush );
	}
	else if (strcmp(ext, ".abr") == 0) {//abrブラシファイル
		int brush_num;
		if((brush_num = IEBrushIO::StartReadAbrBrush(filename)) > 0){
	
			for (int i=0; i < brush_num; i++) {
				IEBrush_Ptr pBrush = IEBrushIO::GetBrush();
				AddBrush( pBrush );
			}
	
			IEBrushIO::EndReadAbrBrush();
		}
	}
}

////////////////////////////////
/*!
	ブラシグループ名をコンボボックスに追加する。
*/
void IEBrushMG::SetBrushGroupComboBox(HWND hCombo) const
{
	//
	IEBrushGroup_Vec::const_iterator itr = m_IEBrushGroup_Vec.begin();
	for(; itr != m_IEBrushGroup_Vec.end(); itr++){
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)(*itr)->GetName());
	}

	return;
}

//void IEBrushMG::SetBrushGroupComboBox(CComboBox combo)
//{
//	//
//	IEBrushGroup_Vec::const_iterator itr = m_IEBrushGroup_Vec.begin();
//	for(; itr != m_IEBrushGroup_Vec.end(); itr++){
//		combo.AddString( (*itr)->GetName() );
//	}
//
//	return;
//}
