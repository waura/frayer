#include "stdafx.h"

#include "IEToolMG.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


class IEToolXmlVisitor : public TiXmlVisitor
{
public:
	void SetIEToolMG(IEToolMG* pToolMG){
		m_pIEToolMG = pToolMG;
	}

	virtual bool VisitEnter(const TiXmlElement& element, const TiXmlAttribute* firstAttribute)
	{
		if (element.ValueStr() == "ietool") {
		}
		else if (element.ValueStr() == "tool_group") {
			m_pNowToolGroup = CreateIEToolGroup();
			VisitIEToolGroupAttr(firstAttribute, m_pNowToolGroup);

			m_pIEToolMG->AddToolGroup( m_pNowToolGroup );
		}
		else if (element.ValueStr() == "tool") {
			m_pNowTool = VisitIEToolAttr( firstAttribute );
			if (m_pNowTool) {
				m_pNowToolGroup->AddTool( m_pNowTool );
			}
		}
		else {
			OutputError::PushLog(
				LOG_LEVEL::_WARN, "tool config xml: unkown tag ", element.ValueStr().c_str());
		}

		return true;
	}

	virtual bool VisitExit(const TiXmlDocument& doc)
	{
		return true;
	}

private:
	void VisitIEToolGroupAttr(const TiXmlAttribute* attr, IEToolGroup* pTGroup)
	{
		while (attr) {
			if (strcmp(attr->Name(), "name") == 0) {
				pTGroup->SetName( attr->ValueStr().c_str() );
			}
			attr = attr->Next();
		}
	}

	IIETool* VisitIEToolAttr (const TiXmlAttribute* attr)
	{
		IIETool* pTool = NULL;
		while (attr) {
			if (strcmp(attr->Name(), "path") == 0) {
				HANDLE hLibrary = ::LoadLibrary( attr->ValueStr().c_str() );
				_CreateIETool pCreateIETool = (_CreateIETool)::GetProcAddress((HMODULE)hLibrary, "CreateIETool");
				if (!pCreateIETool) {
					OutputError::PushLog(
						LOG_LEVEL::_ERROR, "error: faild to load dll ", attr->ValueStr().c_str());
					return NULL;
				}
				
				//craete tool from dll
				pTool = pCreateIETool();		
				if (pTool) {
					pTool->SetDllHandle( hLibrary );
				}
			}
			attr = attr->Next();
		}
		return pTool;
	}

	IEToolMG* m_pIEToolMG;
	IEToolGroup* m_pNowToolGroup;
	IIETool* m_pNowTool;
};

IEToolMG::IEToolMG()
{
	m_pImgEdit = NULL;
	m_pSelectTool = NULL;
	m_pPrevSelectTool = NULL;
	m_pIToolEditWndHandle = NULL;

	m_isInit = false;
	m_LockResourceID = 0;
}

IEToolMG::~IEToolMG()
{
}

void IEToolMG::SetStartDir(const char* path)
{
	m_IEStartDir = path;
}

void IEToolMG::Init(const char* path)
{
	m_isInit = true;

	m_ToolXmlFilePath = path;

	char data_file_path[_MAX_PATH];
	if (m_IEStartDir == "") {
		wsprintf(data_file_path, "%s", m_ToolXmlFilePath.c_str());
	}
	else{
		wsprintf(data_file_path, "%s\\%s", m_IEStartDir.c_str(), m_ToolXmlFilePath.c_str());
	}

	//
	if (!PathFileExists(data_file_path)) {
		MakeDefaultToolXmlFile(data_file_path);
	}

	TiXmlDocument doc( data_file_path );
	if (doc.LoadFile()) {
		IEToolXmlVisitor visitor;
		visitor.SetIEToolMG( this );
		doc.Accept( &visitor );
	}
	else{
		OutputError::PushLog(LOG_LEVEL::_WARN, "faild load tool config xml file");
	}
}

void IEToolMG::End()
{
	if (m_isInit) {
		IEToolGroup_Vec::iterator itr = m_IEToolGroup_Vec.begin();
		for (; itr != m_IEToolGroup_Vec.end(); itr++) {
			IEToolGroup *pgroup = (*itr);
			ReleaseIEToolGroup(&pgroup);
		}
		m_IEToolGroup_Vec.clear();

		m_isInit = false;
	}
}

/////////////////////////////
/*!
	イベントリスナーを登録する。
	@param[in] pListener 登録するリスナー
*/
void IEToolMG::AddEventListener(IEToolMGEventListener*  pListener)
{
	m_IEToolMGEventListener_List.push_back( pListener );
}

/////////////////////////////
/*!
	イベントリスナーを削除する。
	@param[in] pListener 削除するリスナー
*/
void IEToolMG::DeleteEventListener(IEToolMGEventListener*  pListener)
{
	IEToolMGEventListener_List::iterator itr = m_IEToolMGEventListener_List.begin();
	for (; itr != m_IEToolMGEventListener_List.end(); itr++) {
		if ((*itr) == pListener) {
			m_IEToolMGEventListener_List.erase(itr);
			return;
		}
	}
}

void IEToolMG::SetSelectTool(IIETool* pTool)
{
	//change tool
	if (m_pSelectTool != pTool) {

		//end old tool
		if (m_pSelectTool) {
			m_pSelectTool->OnNeutral();
			m_pPrevSelectTool = m_pSelectTool;
		}

		//init select tool
		m_pSelectTool = pTool;
		if (m_pSelectTool) {
			m_pSelectTool->SetImgEdit( m_pImgEdit );
			m_pSelectTool->OnSelect();
		}

		//set tool edit window
		if (m_pIToolEditWndHandle) {
			m_pIToolEditWndHandle->ClearAllCtrl();
			if(m_pSelectTool){
				m_pSelectTool->OnSetToolEditWnd( m_pIToolEditWndHandle );
			}
		}

		//call event
		IEToolMGEventListener_List::iterator itr = m_IEToolMGEventListener_List.begin();
		for (; itr != m_IEToolMGEventListener_List.end(); itr++) {
			if ((*itr)->IsLockIEToolMGEvent() == false && (*itr)->IsCalledIEToolMG() == false) {
				//無限にイベント呼び出しが続かないようにフラグを立てる
				(*itr)->SetCalledIEToolMG(true);
				//
				(*itr)->OnChangeSelectTool(m_pSelectTool, m_pPrevSelectTool);
			}
			(*itr)->SetCalledIEToolMG(false);
		}
	}
}

void IEToolMG::SetToolEditWndHandle(IToolEditWndHandle* pHandle) {
	m_pIToolEditWndHandle = pHandle;
	if (m_pIToolEditWndHandle) {
		//set tool edit window
		if (m_pIToolEditWndHandle) {
			m_pIToolEditWndHandle->ClearAllCtrl();
			if (m_pSelectTool) {
				m_pSelectTool->OnSetToolEditWnd( m_pIToolEditWndHandle );
			}
		}
	}
}

////////////////////////////////
///*!
//	@return ツールオブジェクト
//*/
//IIETool* IEToolMG::GetIEToolFromID(int id)
//{
//}

IIETool* IEToolMG::GetIEToolFromName(const char *tool_name)
{
	IIETool* pTool=NULL;
	IEToolGroup_Vec::iterator itr = m_IEToolGroup_Vec.begin();
	for (; itr != m_IEToolGroup_Vec.end(); itr++) {
		pTool = (*itr)->GetIEToolFromName( tool_name );
		if (pTool) {
			return pTool;
		}
	}
	return NULL;
}

////////////////////////////////
///*!
//	ツール名に対応するツールオブジェクトのポインタを返す。
//	@param[in] name ツール名
//	@return ツールオブジェクト
//*/
//IIETool* IEToolMG::GetIEToolFromDisplayName(const char* disp_name)
//{
//	IETOOL_NODE_GROUP_VEC::iterator ng_itr = m_ieToolNodeGroup_Vec.begin();
//	for(; ng_itr != m_ieToolNodeGroup_Vec.end(); ng_itr++){
//		IETOOL_NODE_VEC::iterator itr = (*ng_itr)->ieToolNode_Vec.begin();
//		for(; itr != (*ng_itr)->ieToolNode_Vec.end(); itr++){
//			if(strcmp((*itr)->display_name.c_str(), disp_name) == 0){
//				return (*itr)->pIETool;
//			}
//		}
//	}
//	return NULL;
//}

void IEToolMG::AddToolGroup(IEToolGroup* pTGroup)
{
	m_IEToolGroup_Vec.push_back( pTGroup );
}

///////////////////////////////
/*!
	指定したグループに含まれるツールの数を返す。
	@param[in] group_index グループのインデックス
	@return 
*/
int IEToolMG::GetSizeOfToolNode(int group_index)
{
	assert(0 <= group_index && group_index < m_IEToolGroup_Vec.size());
	return m_IEToolGroup_Vec[group_index]->GetToolSize();
}

/////////////////////////////////
///*!
//	グループインデックス,ツールノードインデックスを指定し
//	そのツールのIDを返す。
//	@param[in] group_index グループのインデックス
//	@param[in] tnode_index ツールのインデックス
//	@return
//*/
//int IEToolMG::GetToolID(int group_index, int tnode_index)
//{
//	assert(0 <= group_index && group_index < m_IEToolGroup_Vec.size());
//	return m_IEToolGroup_Vec[group_index]->GetToolAtIndex( tnode_index );
//}

bool IEToolMG::GetToolName(int group_index, int tnode_index, char* dst)
{
	if (0 <= group_index && group_index < m_IEToolGroup_Vec.size()) {
		const IIETool* pTool = m_IEToolGroup_Vec[group_index]->GetToolAtIndex( tnode_index );
		if (pTool) {
			pTool->GetName(dst);
			return true;
		}
	}
	return false;
}

bool IEToolMG::GetToolName(const char* display_name, char* dst)
{
	IEToolGroup_Vec::iterator itr = m_IEToolGroup_Vec.begin();
	for (; itr != m_IEToolGroup_Vec.end(); itr++) {
		if ((*itr)->GetToolName(display_name, dst)) {
			return true;
		}
	}
	return false;
}

///////////////////////////////
/*!
	グループインデックス,ツールノードインデックスを指定し
	そのツールの表示名をdstに書き込む
	@param[in] group_index グループのインデックス
	@param[in] tnode_index ツールのインデックス
	@param[in] dst 出力先アドレス
	@return
*/
bool IEToolMG::GetDisplayName(int group_index, int tnode_index, char *dst)
{
	if (0 <= group_index && group_index < m_IEToolGroup_Vec.size()) {
		return m_IEToolGroup_Vec[ group_index ]->GetDisplayName(tnode_index, dst);
	}
	return false;
}

bool IEToolMG::GetDisplayName(const char *tool_name, char* dst)
{
	IEToolGroup_Vec::iterator itr = m_IEToolGroup_Vec.begin();
	for (; itr != m_IEToolGroup_Vec.end(); itr++) {
		if ((*itr)->GetDisplayName(tool_name, dst)) {
			return true;
		}
	}
	return false;
}

void IEToolMG::MakeDefaultToolXmlFile(const char* tool_xml_path)
{
	FILE* fp = NULL;
	if (fp = fopen(tool_xml_path, "w")) {
		fprintf(fp,
			"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
			"<ietool>\n"
			"  <tool_group name=\"movelayer\">\n"
			"    <tool path=\".\\data\\tool_dll\\MoveLayer.dll\" />\n"
			"  </tool_group>\n"
			"\n"
			"  <tool_group name=\"ImageSlecter\">\n"
			"    <tool path=\".\\data\\tool_dll\\RectImgSelecter.dll\" />\n"
			"    <tool path=\".\\data\\tool_dll\\EllipseImgSelecter.dll\" />\n"
			"    <tool path=\".\\data\\tool_dll\\PenSelecter.dll\" />\n"
			"    <tool path=\".\\data\\tool_dll\\EraserSelecter.dll\" />\n"
			"    <tool path=\".\\data\\tool_dll\\PolygonSelecter.dll\" />\n"
			"    <tool path=\".\\data\\tool_dll\\LassoSelecter.dll\" />\n"
			"  </tool_group>\n"
			"\n"
			"  <tool_group name=\"brush\">\n"
			"    <tool path=\".\\data\\tool_dll\\NormalBrushDraw.dll\" />\n"
			"    <tool path=\".\\data\\tool_dll\\PencilDraw.dll\" />\n"
			"    <tool path=\".\\data\\tool_dll\\WaterColorBrushDraw.dll\" />\n"
			"  </tool_group>\n"
			"\n"
			"  <tool_group name=\"eraser\">\n"
			"    <tool path=\".\\data\\tool_dll\\Eraser.dll\" />\n"
			"  </tool_group>\n"
			"\n"
			"  <tool_group name=\"spoit\">\n"
			"    <tool path=\".\\data\\tool_dll\\SpoitTool.dll\" />\n"
			"  </tool_group>\n"
			"\n"
			"  <tool_group name=\"display\">\n"
			"    <tool path=\".\\data\\tool_dll\\ViewParallelTrans.dll\" />\n"
			"    <tool path=\".\\data\\tool_dll\\ViewRot.dll\" />\n"
			"  </tool_group>\n"
			"</ietool>\n");
		fclose(fp);
	}
	else {
		OutputError::Alert(IE_ERROR_TOOL_CONFIG_FILE_CANT_WRITE);
	}
}