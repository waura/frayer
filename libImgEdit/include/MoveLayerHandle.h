#ifndef _MOVELAYERHANDLE_H_
#define _MOVELAYERHANDLE_H_

#include "ImgFileHandle.h"
#include "ImgUpdateData.h"
#include "ImgLayerGroup.h"
#include "ImgFile.h"

class MoveLayerHandle : public ImgFileHandle
{
public:
	MoveLayerHandle() {
		m_move_x = 0;
		m_move_y = 0;
	}
	~MoveLayerHandle(){};

	void SetMoveXY(int move_x, int move_y){
		IImgLayer_Ptr _pEditLayer = m_pEditLayer.lock();
		ImgLayer_Ptr pEditLayer = std::dynamic_pointer_cast<ImgLayer>(_pEditLayer);
		pEditLayer->MoveLayer(-m_move_x, -m_move_y);
		pEditLayer->MoveLayer(move_x, move_y);

		m_move_x = move_x;
		m_move_y = move_y;
	}

	inline void SetEditLayer(IImgLayer_Ptr pLayer){
		m_pEditLayer = pLayer;
	}

	void Update(){
		IImgLayer_Ptr _pEditLayer = m_pEditLayer.lock();
		ImgLayer_Ptr pEditLayer = std::dynamic_pointer_cast<ImgLayer>(_pEditLayer);

		LPUPDATE_DATA pData = pEditLayer->CreateUpdateData();
		pData->isAll = true;
		pEditLayer->PushUpdateData( pData );

		ImgFile_Ptr pFile = m_pEditFile.lock();
		LPUPDATE_DATA pData2 = pFile->CreateUpdateData();
		pData2->isAll = true;
		pFile->PushUpdateData( pData2 );
	}

	bool Init(ImgFile_weakPtr wpFile){
		m_pEditFile = wpFile;
		return true;
	}

	bool Do(ImgFile_weakPtr wpFile){
		m_pEditLayer = m_pEditFile.lock()->GetSelectLayer();
		IImgLayer_Ptr _pEditLayer = m_pEditLayer.lock();
		assert(_pEditLayer);

		this->Update();

		return true;
	}
	void Redo(ImgFile_weakPtr wpFile){
		IImgLayer_Ptr _pEditLayer = m_pEditLayer.lock();
		ImgLayer_Ptr pEditLayer = std::dynamic_pointer_cast<ImgLayer>(_pEditLayer);
		pEditLayer->MoveLayer(m_move_x, m_move_y);

		this->Update();
	}
	void Undo(ImgFile_weakPtr wpFile){
		IImgLayer_Ptr _pEditLayer = m_pEditLayer.lock();
		ImgLayer_Ptr pEditLayer = std::dynamic_pointer_cast<ImgLayer>(_pEditLayer);
		pEditLayer->MoveLayer(-m_move_x, -m_move_y);

		this->Update();
	}

private:
	ImgFile_weakPtr m_pEditFile;
	IImgLayer_weakPtr m_pEditLayer;

	int m_move_x;
	int m_move_y;
};

#endif //_MOVELAYERHANDLE_H_