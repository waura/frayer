#ifndef _CHANGE_LAYER_OPACITY_HANDLE_H_
#define _CHANGE_LAYER_OPACITY_HANDLE_H_

#include "ImgFileHandle.h"
#include "ImgUpdateData.h"
#include "IImgLayer.h"
#include "ImgFile.h"

class _EXPORTCPP ChangeLayerOpacityHandle
	: public ImgFileHandle
{
public:
	ChangeLayerOpacityHandle()
	{
		from_opacity = -1.0;
		to_opacity = -1.0;
	}
	~ChangeLayerOpacityHandle(){}

	void SetEditLayer(IImgLayer_Ptr pLayer)
	{
		assert(pLayer);

		m_pEditLayer = pLayer;
		from_opacity = pLayer->GetOpacity();
		to_opacity = from_opacity;

		char layer_name[MAX_IMG_LAYER_NAME];
		pLayer->GetName(layer_name);
		wsprintf(handle_name, "レイヤー不透明度変更 (%s)", layer_name);
	}

	void SetChangeOpacity(uint8_t opacity)
	{
		to_opacity = opacity;
	}

	void Update()
	{
		IImgLayer_Ptr pLayer = m_pEditLayer.lock();
		assert(pLayer);

		IImgLayer_Ptr _pLGroup = pLayer->GetParentLayer().lock();
		LPUPDATE_DATA pData;
		ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(_pLGroup);
		pData = pLGroup->CreateUpdateData();
		pData->isAll = true;
		pLayer->PushUpdateData(pData);

		ImgFile_Ptr pFile = m_pEditFile.lock();
		assert(pFile);

		pData = pFile->CreateUpdateData();
		pData->isAll = true;
		pFile->PushUpdateData(pData);
	}

	bool Init(ImgFile_weakPtr wpFile)
	{
		m_pEditFile = wpFile;
		return true;
	}

	bool Do(ImgFile_weakPtr wpFile)
	{
		IImgLayer_Ptr pLayer = m_pEditLayer.lock();
		assert(pLayer);

		if (from_opacity != to_opacity) {
			pLayer->SetOpacity(to_opacity);
			Update();
			return true;
		}
		return false;
	}

	void Redo(ImgFile_weakPtr wpFile)
	{
		IImgLayer_Ptr pLayer = m_pEditLayer.lock();
		assert(pLayer);
		if (from_opacity != to_opacity) {
			pLayer->SetOpacity(to_opacity);
			Update();
		}
	}

	void Undo(ImgFile_weakPtr wpFile)
	{
		IImgLayer_Ptr pLayer = m_pEditLayer.lock();
		assert(pLayer);

		pLayer->SetOpacity(from_opacity);
		Update();
	}

private:

	uint8_t from_opacity;
	uint8_t to_opacity;

	ImgFile_weakPtr m_pEditFile;
	IImgLayer_weakPtr m_pEditLayer;
};

#endif //_CHANGE_LAYER_OPACITY_HANDLE_H_