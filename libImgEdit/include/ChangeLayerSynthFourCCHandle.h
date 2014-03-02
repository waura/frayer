#ifndef _CHANGE_LAYER_SYNTH_FOURCC_HANDLE_H_
#define _CHANGE_LAYER_SYNTH_FOURCC_HANDLE_H_

#include "ImgFileHandle.h"
#include "ImgUpdateData.h"
#include "ImgLayer.h"
#include "ImgLayerGroup.h"
#include "ImgFile.h"

class _EXPORTCPP ChangeLayerSynthFourCCHandle
	: public ImgFileHandle
{
public:
	ChangeLayerSynthFourCCHandle()
	{
		from_fourcc = -1;
		to_fourcc = -1;
	}
	~ChangeLayerSynthFourCCHandle(){}

	void SetEditLayer(ImgLayer_Ptr pLayer)
	{
		assert(pLayer);
		m_pEditLayer = pLayer;


		from_fourcc = pLayer->GetLayerSynthFourCC();
		to_fourcc = from_fourcc;

		char layer_name[MAX_IMG_LAYER_NAME];
		pLayer->GetName(layer_name);
		wsprintf(handle_name, "レイヤー合成モード変更 (%s)", layer_name);
	}

	void SetChangeFourCC(int fourcc)
	{		
		to_fourcc = fourcc;
		//char to_disp_name[MAX_IELAYER_SYNTH_DISPLAY_NAME];
		//char from_disp_name[MAX_IELAYER_SYNTH_DISPLAY_NAME];		
		//LayerSynthesizerMG::GerLayerSynthDisplayName(to_fourcc, to_disp_name);
		//LayerSynthesizerMG::GetLayerSynthDisplayName(from_fourcc, from_disp_name);
	}

	void Update()
	{
		ImgLayer_Ptr pEditLayer = m_pEditLayer.lock();
		assert(pEditLayer);

		ImgFile_Ptr pFile = m_pEditFile.lock();
		assert(pFile);

		//update image
		LPUPDATE_DATA pData;

		IImgLayer_Ptr _pLGroup = pEditLayer->GetParentLayer().lock();
		ImgLayerGroup_Ptr pLGroup = std::dynamic_pointer_cast<ImgLayerGroup>(_pLGroup);
		pData = pLGroup->CreateUpdateData();
		pData->isAll = true;
		pLGroup->PushUpdateData(pData);

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
		ImgLayer_Ptr pEditLayer = m_pEditLayer.lock();
		assert(pEditLayer);
		if (from_fourcc != to_fourcc) {
			pEditLayer->SetLayerSynthFourCC(to_fourcc);
			Update();
			return true;
		}
		return false;
	}

	void Redo(ImgFile_weakPtr wpFile)
	{
		ImgLayer_Ptr pEditLayer = m_pEditLayer.lock();
		assert(pEditLayer);
		if (from_fourcc != to_fourcc) {
			pEditLayer->SetLayerSynthFourCC(to_fourcc);
			Update();
		}
	}

	void Undo(ImgFile_weakPtr wpFile)
	{
		ImgLayer_Ptr pEditLayer = m_pEditLayer.lock();
		assert(pEditLayer);

		pEditLayer->SetLayerSynthFourCC(from_fourcc);
		Update();
	}

private:

	int from_fourcc;
	int to_fourcc;

	ImgFile_weakPtr m_pEditFile;
	ImgLayer_weakPtr m_pEditLayer;
};

#endif //_CHANGE_LAYER_SYNTH_FOURCC_HANDLE_H_