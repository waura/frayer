#include "stdafx.h"

#include "IEPalette.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


_EXPORTCPP IEPalette_Ptr CreateIEPalette()
{
	return IEPalette_Ptr(new IEPalette);
}

static IEPaletteNode_Ptr CreateIEPaletteNode(const char* name, IEColor color)
{
	IEPaletteNode_Ptr ret = IEPaletteNode_Ptr(new IEPaletteNode);
	strcpy_s(ret->name, MAX_IE_PALETTE_NODE_NAME, name);
	ret->color = color;
	return ret;
}

static void write_4byte(FILE* fp, uint32_t val)
{
	fwrite(&val, 1, 4, fp);
}

static void write_2byte(FILE* fp, uint16_t val)
{
	fwrite(&val, 1, 2, fp);
}

static void write_1byte(FILE* fp, uint8_t val)
{
	fwrite(&val, 1, 1, fp);
}

static void write_string(FILE* fp, const char* val)
{
	int len = strlen(val);
	write_1byte(fp, len);

	if(len == 0) return;

	fwrite(val, len, 1, fp);
}

static uint32_t read_4byte(FILE* fp)
{
	if(fp == NULL) return 0;
	uint32_t val; 
	fread(&val, sizeof(uint32_t), 1, fp);
	return val;
}

static uint16_t read_2byte(FILE* fp)
{
	if(fp == NULL) return 0;
	uint16_t val;
	fread(&val, sizeof(uint16_t), 1, fp);
	return val;
}

static uint8_t read_1byte(FILE* fp)
{
	if(fp == NULL) return 0;
	return fgetc(fp);
}

static void read_string(FILE* fp, char* dst)
{
	int len = read_1byte(fp);

	if(len == 0) return;

	fread(dst, len, 1, fp);
}

IEPalette::IEPalette()
{
	memset(m_name, 0, sizeof(char)*MAX_IE_PALETTE_NAME);

	m_SelectPaletteNodeIndex = IE_PALETTE_NON_SELECT;
	m_pSelectPaletteNode = NULL;

#ifdef _WIN32
	::InitializeCriticalSection( &m_csPaletteNode );
#else
	m_LockResourceID = 0;
#endif //_WIN32
}

IEPalette::~IEPalette()
{
	//
	if( !m_IEPaletteNode_Vec.empty() ){
		m_IEPaletteNode_Vec.clear();
	}

#ifdef _WIN32
	::DeleteCriticalSection( &m_csPaletteNode );
#else
#endif //_WIN32
}

///////////////////////////////////////////////
/*!
	XML形式でパレットのデータを書き込み
*/
void IEPalette::WritePaletteToXml(FILE *fp, int indt)
{
	assert(indt >= 0);

	int i;
	for (i=0; i<indt; i++) fprintf(fp, "  ");
	fprintf(fp, "<palette name=\"%s\">\n", m_name);

	IEPaletteNode_Vec::iterator itr = m_IEPaletteNode_Vec.begin();
	for(; itr != m_IEPaletteNode_Vec.end(); itr++){
		IEPaletteNode_Ptr pNode = (*itr);
		for(i=0; i<indt+1; i++) fprintf(fp, "  ");
		fprintf(fp, "<color name=\"%s\" value=\"%d\"/>\n", pNode->name, pNode->color.value);
	}

	for (i=0; i<indt; i++) fprintf(fp, "  ");
	fprintf(fp, "</palette>\n", m_name);
}

//bool IEPalette::LoadPalette(const char *path)
//{
//	m_palettePath = path;
//
//	FILE* fp;
//	if(fp = fopen(path, "rb")){
//		char signature[4];
//		if(!fread(signature, 4, 1, fp)) return false;
//		if(memcmp(signature, "PFRY", 4) != 0) return false;
//
//		int num_of_node = read_2byte(fp);
//		for(int i=0; i<num_of_node; i++){
//			IEPaletteNode* pNode = new IEPaletteNode;
//			memset(pNode, 0, sizeof(IEPaletteNode));
//
//			pNode->color.value = read_4byte(fp);
//			read_string(fp, pNode->name);
//			this->AddColor( pNode );
//		}
//	}
//	else{
//		TCHAR log[256];
//		wsprintf(log, "LoadPalette(): cant open palette file %s", path);
//		OutputError::PushLog(LOG_LEVEL::_WARN, log);
//	}
//
//	return true;
//}

/////////////////////////////
/*!
	イベントリスナーを登録する。
	@param[in] pListener 登録するリスナー
*/
void IEPalette::AddEventListener(IEPaletteEventListener*  pListener)
{
	m_IEPaletteEventListener_List.push_back( pListener );
}

/////////////////////////////
/*!
	イベントリスナーを削除する。
	@param[in] pListener 削除するリスナー
*/
void IEPalette::DeleteEventListener(IEPaletteEventListener*  pListener)
{
	IEPaletteEventListener_List::iterator itr = m_IEPaletteEventListener_List.begin();
	for(; itr != m_IEPaletteEventListener_List.end(); itr++){
		if((*itr) == pListener){
			m_IEPaletteEventListener_List.erase(itr);
			return;
		}
	}
}

void IEPalette::SetSelectColor(int index)
{
	IEPaletteNode_Ptr pNode = NULL;
	if(0 <= index && index < m_IEPaletteNode_Vec.size()){
		pNode = m_IEPaletteNode_Vec[ index ];
	}
	else{
		index = IE_PALETTE_NON_SELECT;
	}
	m_SelectPaletteNodeIndex = index;

	//call event
	IEPaletteEventListener_List::iterator itr = m_IEPaletteEventListener_List.begin();
	for(; itr != m_IEPaletteEventListener_List.end(); itr++){
		if((*itr)->IsLockIEPaletteEvent() == false && (*itr)->IsCalledIEPalette() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledIEPalette(true);
			//
			(*itr)->OnChangeSelectColor(index);
		}

		(*itr)->SetCalledIEPalette(false);
	}

	ResourceLock( IE_PALETTE_RESOURCE_ID::IE_PALETTE_NODE );
	m_pSelectPaletteNode = pNode;
	ResourceUnlock( IE_PALETTE_RESOURCE_ID::IE_PALETTE_NODE );
}

void IEPalette::AddColor(const char* name, IEColor color)
{
	IEPaletteNode_Ptr pNode = CreateIEPaletteNode(name, color);

	ResourceLock( IE_PALETTE_RESOURCE_ID::IE_PALETTE_NODE );
	m_IEPaletteNode_Vec.push_back( pNode );
	ResourceUnlock( IE_PALETTE_RESOURCE_ID::IE_PALETTE_NODE );

	IEPaletteEventListener_List::iterator itr = m_IEPaletteEventListener_List.begin();
	for(; itr != m_IEPaletteEventListener_List.end(); itr++){
		if((*itr)->IsLockIEPaletteEvent() == false && (*itr)->IsCalledIEPalette() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledIEPalette( true );
			//
			(*itr)->OnAddColor( pNode );
		}

		(*itr)->SetCalledIEPalette( false );
	}
}
void IEPalette::DeleteColor(int index)
{
	IEPaletteNode_Vec::iterator itr_br = m_IEPaletteNode_Vec.begin();
	for(int i=0; i<index; i++){
		itr_br++;
	}
	IEPaletteNode_Ptr pNode = (*itr_br);
	if(pNode == m_pSelectPaletteNode){
		SetSelectColor( IE_PALETTE_NON_SELECT );
	}

	ResourceLock( IE_PALETTE_RESOURCE_ID::IE_PALETTE_NODE );
	//ブラシ配列から削除
	m_IEPaletteNode_Vec.erase( itr_br );
	ResourceUnlock( IE_PALETTE_RESOURCE_ID::IE_PALETTE_NODE );

	IEPaletteEventListener_List::iterator itr = m_IEPaletteEventListener_List.begin();
	for(; itr != m_IEPaletteEventListener_List.end(); itr++){
		if((*itr)->IsLockIEPaletteEvent() == false && (*itr)->IsCalledIEPalette() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledIEPalette( true );
			//
			(*itr)->OnDeleteColor(index, pNode);
		}

		(*itr)->SetCalledIEPalette( false );
	}
}

//void IEPalette::SavePalette()
//{
//	FILE* fp;
//	if(fp = fopen(m_palettePath.c_str(), "wb")){
//		fwrite("PFRY", 1, 4, fp); //signature
//
//		write_2byte(fp, GetPaletteSize());
//		IEPaletteNode_Vec::iterator itr = m_IEPaletteNode_Vec.begin();
//		for(; itr != m_IEPaletteNode_Vec.end(); itr++){
//			write_4byte(fp, (*itr)->color.value);
//			write_string(fp, (*itr)->name);
//		}
//	}
//	else{
//		TCHAR log[256];
//		wsprintf(log, "SavePalette(): cant open palette file %s", m_palettePath.c_str());
//		OutputError::PushLog(LOG_LEVEL::_WARN, log);
//	}
//}