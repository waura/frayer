#include "stdafx.h"

#include "AddNewMaskHandle.h"

#include "ImgMask.h"
#include "ImgFile.h"
#include "MaskSynthesizerMG.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


AddNewMaskHandle::AddNewMaskHandle()
{
	m_new_mask_index = -1;
	m_isAddedMask = false;
	
}

AddNewMaskHandle::~AddNewMaskHandle()
{
	if( !m_isAddedMask ){
		
	}
}

bool AddNewMaskHandle::Do(ImgFile_weakPtr wpFile)
{
	assert(0);
	//CvSize img_size = pFile->GetImgSize();
	//ImgMask* mask = new ImgMask( pFile );

	//m_mask_rect.top = 0;
	//m_mask_rect.left = 0;
	//m_mask_rect.right = img_size.width;
	//m_mask_rect.bottom = img_size.height;

	//if(m_new_mask_index == -1){
	//	m_new_mask_index = pFile->GetMaskSize();
	//}
	//mask->Init( &m_mask_rect );

	//char mask_name[256];
	//wsprintf(mask_name, "ƒ}ƒXƒN%d", m_new_mask_index);

	//mask->SetName(mask_name);
	//mask->SetMaskSynth(DEFAULT_MASK_SYNTH_SELECT_INDEX, MaskSynthesizerMG::GetDefaultMaskSynth());

	//pFile->AddMask(mask, m_new_mask_index);

	//LPUPDATE_DATA data = pFile->CreateUpdateData(UPDATE_MASK_IMAGE);
	//data->isAll = true;
	//pFile->PushUpdateData(data);
	return true;
}

void AddNewMaskHandle::Redo(ImgFile_weakPtr wpFile)
{
	/*Do(pFile);*/
}

void AddNewMaskHandle::Undo(ImgFile_weakPtr wpFile)
{
	//pFile->DeleteMask(m_new_mask_index);

	//LPUPDATE_DATA data = pFile->CreateUpdateData(UPDATE_MASK_IMAGE);
	//data->isAll = true;
	//pFile->PushUpdateData(data);
}