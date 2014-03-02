#ifndef _EDITMASKHANDLE_H_
#define _EDITMASKHANDLE_H_

#include "ImgFileHandle.h"
#include "ImgUpdateData.h"
#include "ImgMask.h"
#include "ImgFile.h"

class EditMaskHandle : public ImgFileHandle
{
public:
	EditMaskHandle() : 
		m_editNode(NULL){}
	~EditMaskHandle()
	{
		assert( _CrtCheckMemory() );

		if (m_editNode) {
			m_pEditMask.lock()->DestroyEditNode( m_editNode );
			m_editNode = NULL;
		}

		assert( _CrtCheckMemory() );
	}

	inline void SetHandleName(const char* src) {
		strcpy_s(handle_name, MAX_IMG_FILE_HANDLE_NAME, src);
	}

	inline void SetEditMask(ImgMask_Ptr pMask) {
		m_pEditMask = pMask;
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
		assert(m_pEditMask.lock());

		if (m_editNode) {
			m_pEditMask.lock()->DestroyEditNode(m_editNode);
		}

		m_editNode = m_pEditMask.lock()->CreateEditNode( lprc );
		return m_editNode;
	}

	void Update(const LPRECT lprc)
	{
		assert( m_pEditMask.lock() );
		assert( m_editNode );

		m_pEditMask.lock()->PushAddEditNodeQueue( m_editNode );

		LPUPDATE_DATA pData = m_pEditMask.lock()->CreateUpdateData();
		pData->isAll = false;
		pData->rect = (*lprc);
		m_pEditMask.lock()->PushUpdateData( pData );

		ImgFile_Ptr pFile = m_pEditFile.lock();
		LPUPDATE_DATA pData2 = pFile->CreateUpdateData();
		pData2->isAll = false;
		pData2->rect = (*lprc);
		pFile->PushUpdateData( pData2 );
	}

	void EndEdit(){
		assert( m_pEditMask.lock() );
		assert( m_editNode );

		if( m_pEditMask.lock() ){

			ImgFile_Ptr pFile = m_pEditFile.lock();
			if (pFile) {
				RECT rc;
				pFile->LockUpdateData();
				pFile->Update(NULL, &rc);
				pFile->ClearUpdateData();
				pFile->UnlockUpdateData();

				m_editNode->isBakedWriteMap = true;
				//m_pEditMask.lock()->UpdateMaskState();

				LPUPDATE_DATA pData = pFile->CreateUpdateData();
				pData->isAll = true;
				pFile->PushUpdateData( pData );
			}
		}
	}

	bool Init(ImgFile_weakPtr wpFile)
	{
		m_pEditFile = wpFile;
		m_pEditMask = wpFile.lock()->GetSelectMask();
		return true;
	}

	bool Do(ImgFile_weakPtr wpFile){
		return true;
	}
	void Redo(ImgFile_weakPtr wpFile){
		Update(&(m_editNode->blt_rect));
		//EndEdit();
	}
	void Undo(ImgFile_weakPtr wpFile){
		m_pEditMask.lock()->PushSubEditNodeQueue(m_editNode);

		LPUPDATE_DATA data = m_pEditMask.lock()->CreateUpdateData();
		data->isAll = false;
		data->rect = m_editNode->blt_rect;
		m_pEditMask.lock()->PushUpdateData(data);

		ImgFile_Ptr pFile = m_pEditFile.lock();
		LPUPDATE_DATA data2 = pFile->CreateUpdateData();
		data2->isAll = false;
		data2->rect = m_editNode->blt_rect;
		pFile->PushUpdateData(data2);

		//EndEdit();
	}

private:
	ImgFile_weakPtr m_pEditFile;
	ImgMask_weakPtr m_pEditMask;
	EditNode* m_editNode;
};

#endif //_EDITMASKHANDLE_H_