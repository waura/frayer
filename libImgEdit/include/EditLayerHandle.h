#ifndef _EDITLAYERHANDLE_H_
#define _EDITLAYERHANDLE_H_

#include "ImgFileHandle.h"
#include "ImgUpdateData.h"
#include "ImgLayer.h"
#include "ImgFile.h"

#include "OutputError.h"

class EditLayerHandle : public ImgFileHandle
{
public:
	EditLayerHandle() :
	  m_editLayerIndex(-1),
		m_editNode(NULL){}
	~EditLayerHandle()
	{
		if (m_editNode) {
			m_pEditLayer.lock()->DestroyEditNode( m_editNode );
			m_editNode = NULL;
		}

		assert( _CrtCheckMemory() );
	}

	inline void SetHandleName(const char* src)
	{
		ImgLayer_Ptr pLayer = m_pEditLayer.lock();
		if (pLayer) {
			char layer_name[MAX_IMG_LAYER_NAME];
			pLayer->GetName(layer_name);
			wsprintf(handle_name, "%s (%s)", src, layer_name);
		} else {
			strcpy_s(handle_name, MAX_IMG_FILE_HANDLE_NAME, src);
		}
	}

	inline void SetEditLayerIndex(int index)
	{
		assert(m_editLayerIndex == -1);
		m_editLayerIndex = index;
	}

	void UpdateLayerPointer()
	{
	}

	EditNode* CreateEditNode()
	{
		CvSize size = m_pEditFile.lock()->GetImgSize();

		RECT rc;
		rc.left = rc.top = 0;
		rc.right = size.width;
		rc.bottom = size.height;
		return this->CreateEditNode(&rc);
	}
	EditNode* CreateEditNode(const LPRECT lprc)
	{
		ImgLayer_Ptr pEditLayer = m_pEditLayer.lock();
		assert(pEditLayer);

		if (m_editNode) {
			pEditLayer->DestroyEditNode(m_editNode);
		}

		m_editNode = pEditLayer->CreateEditNode( lprc );
		return m_editNode;
	}

	void Update(const LPRECT lprc)
	{
		ImgLayer_Ptr pEditLayer = m_pEditLayer.lock();
		assert( pEditLayer );
		assert( m_editNode );

		pEditLayer->PushAddEditNodeQueue( m_editNode );

		LPUPDATE_DATA pData = pEditLayer->CreateUpdateData();
		pData->isAll = false;
		pData->rect = (*lprc);
		pEditLayer->PushUpdateData( pData );

		ImgFile_Ptr pFile = m_pEditFile.lock();
		LPUPDATE_DATA pData2 = pFile->CreateUpdateData();
		pData2->isAll = false;
		pData2->rect = (*lprc);
		pFile->PushUpdateData(pData2);
	}

	void EndEdit()
	{
		assert( m_editNode );

		ImgFile_Ptr pFile = m_pEditFile.lock();
		if (pFile) {
			RECT rc;
			pFile->LockUpdateData();
			pFile->Update(NULL, &rc);
			pFile->ClearUpdateData();
			pFile->UnlockUpdateData();
		}

		if (m_editNode) {
			m_editNode->isBakedWriteMap = true;
		}
	}

	bool Init(ImgFile_weakPtr wpFile)
	{
		m_pEditFile = wpFile;
		IImgLayer_weakPtr _l = wpFile.lock()->GetSelectLayer();
		if (_l.expired()) return false;

		IImgLayer_Ptr l = _l.lock();
		if (l->GetLayerType() != IE_LAYER_TYPE::NORMAL_LAYER) {
			OutputError::Alert(IE_ERROR_CANT_EDIT_LAYER);
			return false;
		}

		m_pEditLayer = std::dynamic_pointer_cast<ImgLayer>(l);
		return true;
	}

	bool Do(ImgFile_weakPtr wpFile);
	void Redo(ImgFile_weakPtr wpFile);
	void Undo(ImgFile_weakPtr wpFile);

private:
	int m_editLayerIndex;
	ImgFile_weakPtr m_pEditFile;
	ImgLayer_weakPtr m_pEditLayer;
	EditNode* m_editNode;
};

#endif //_EDITLAYERHANDLE_H_