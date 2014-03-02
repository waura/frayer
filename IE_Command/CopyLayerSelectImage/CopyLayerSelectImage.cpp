// CopyLayerSelectImage.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//
#include "stdafx.h"
#include "CopyLayerSelectImage.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

// for detect memory leak
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <cstdlib>
	#include <crtdbg.h>
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#define DISPLAY_NAME "選択領域をコピーしてレイヤー複製"

IECOMMAND_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::COMMAND;
}

IECOMMAND_EXPORT void GetIECommandName(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, "CopyLayerSelectImage");
}

IECOMMAND_EXPORT void GetIECommandDisplayNameJa(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, DISPLAY_NAME);
}

IECOMMAND_EXPORT IIECommand* CreateIECommand()
{
	return new CopyLayerSelectImage;
}

IECOMMAND_EXPORT void ReleaseIECommand(IIECommand** ppCommand)
{
	if(*ppCommand){
		delete (*ppCommand);
		(*ppCommand) = NULL;
	}
}

CopyLayerSelectImage::CopyLayerSelectImage()
{
  	strcpy_s(m_name, MAX_IE_MODULE_NAME, "CopyLayerSelectImage");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

BOOL CopyLayerSelectImage::Run(ImgEdit* pEdit, void* pvoid)
{
	ImgFile_Ptr f = pEdit->GetActiveFile();
	if(f == NULL) return FALSE;

	IImgLayer_weakPtr _sl = f->GetSelectLayer();
	if (_sl.expired()) {
		OutputError::Alert(IE_ERROR_SELECT_LAYER_DONT_EXIST);
		return FALSE;
	}

	IImgLayer_Ptr sl = _sl.lock();
	if (sl->GetLayerType() != IE_LAYER_TYPE::NORMAL_LAYER) {
		OutputError::Alert(IE_ERROR_CANT_EDIT_LAYER);
		return FALSE;
	}
	ImgLayer_Ptr layer = std::dynamic_pointer_cast<ImgLayer>(sl);

	CvSize size = f->GetImgSize();

	RECT mask_rect;
	f->GetMaskRect(&mask_rect);
	bool isNoSelect = (f->GetMaskState() == IE_MASK_STATE_ID::INVALID); //選択領域有りか無しか？
	if (isNoSelect) {
		mask_rect.top = mask_rect.left = 0;
		mask_rect.right = size.width;
		mask_rect.bottom =size.height;
	}

	ImgLayer_Ptr select_layer = layer;

	//create handles
	AddNewLayerHandle* hadd_new_layer = (AddNewLayerHandle*)f->CreateImgFileHandle(IFH_ADD_NEW_LAYER);
	EditLayerHandle* hedit_layer = (EditLayerHandle*)f->CreateImgFileHandle(IFH_EDIT_LAYER);
	StackHandle* hstack = (StackHandle*)f->CreateImgFileHandle(IFH_STACK);

	//set edit layer index
	int select_layer_index = f->GetLayerIndex(select_layer);
	int new_layer_index = select_layer_index + 1;
	hadd_new_layer->SetAddLayerIndex(new_layer_index);
	hedit_layer->SetEditLayerIndex(new_layer_index);

	//push handles to stack
	hstack->SetHandleName( DISPLAY_NAME );
	hstack->Push( hadd_new_layer );
	hstack->Push( hedit_layer );
	f->DoImgFileHandle( hstack );

	//select new layer
	ImgLayer_Ptr new_layer = hadd_new_layer->GetNewLayer().lock();
	f->SetSelectLayer(new_layer);

	hedit_layer->SetHandleName( DISPLAY_NAME );

	EditNode* pEditNode = hedit_layer->CreateEditNode();

	//選択領域部分の画像をコピー
	layer->CopyLayerImage(
		&(pEditNode->edit_img),
		mask_rect.left - pEditNode->node_rect.left,
		mask_rect.top - pEditNode->node_rect.top,
		mask_rect.left,
		mask_rect.top,
		mask_rect.right - mask_rect.left,
		mask_rect.bottom - mask_rect.top,
		f->GetDrawMaskImg(),
		mask_rect.left,
		mask_rect.top);

	//update layer image
	hedit_layer->Update(&mask_rect);
	hedit_layer->EndEdit();

	return FALSE;	
}