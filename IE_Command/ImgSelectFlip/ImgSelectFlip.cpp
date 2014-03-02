// ImgSelectFlip.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//
#include "stdafx.h"
#include "ImgSelectFlip.h"


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

#define DISPLAY_NAME_JA "選択範囲を反転する"
#define DISPLAY_NAME DISPLAY_NAME_JA


IECOMMAND_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::COMMAND;
}

IECOMMAND_EXPORT void GetIECommandName(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, "ImgSelectFlip");
}

IECOMMAND_EXPORT void GetIECommandDisplayNameJa(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, DISPLAY_NAME);
}

IECOMMAND_EXPORT IIECommand* CreateIECommand()
{
	return new ImgSelectFlip;
}

IECOMMAND_EXPORT void ReleaseIECommand(IIECommand** ppCommand)
{
	if (*ppCommand) {
		delete (*ppCommand);
		(*ppCommand) = NULL;
	}
}

ImgSelectFlip::ImgSelectFlip()
{
  	strcpy_s(m_name, MAX_IE_MODULE_NAME, "ImgSelectFlip");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

BOOL ImgSelectFlip::Run(ImgEdit* pEdit, void* pvoid)
{
	ImgFile_Ptr f = pEdit->GetActiveFile();
	if(f){
		ImgMask_Ptr pMask = f->GetSelectMask();
		CvSize size = f->GetImgSize();

		EditMaskHandle* pEditMaskHandle = (EditMaskHandle*)f->CreateImgFileHandle(IFH_EDIT_MASK);
		strcpy_s(pEditMaskHandle->handle_name, MAX_IMG_FILE_HANDLE_NAME, DISPLAY_NAME);
		f->DoImgFileHandle( pEditMaskHandle );
	
		EditNode* pEditNode;
		pEditNode = pEditMaskHandle->CreateEditNode();
		pEditNode->fourcc = IE_MASK_SYNTH_FOURCC('c', 'o', 'p', 'y');

		//int x,y;
		//int8_t mask_data;
		//for(y = pEditNode->blt_rect.top; y < pEditNode->blt_rect.bottom; y++){
		//	for(x = pEditNode->blt_rect.left; x < pEditNode->blt_rect.right; x++){
		//		pMask->GetMaskData(x, y, &mask_data);
		//		//pEditNode->undo_img.SetMaskData(
		//		//	x - pEditNode->node_rect.left,
		//		//	y - pEditNode->node_rect.top,
		//		//	mask_data);
		//		pEditNode->edit_img.SetMaskData(
		//			x - pEditNode->node_rect.left,
		//			y - pEditNode->node_rect.top,
		//			255-mask_data);
		//	}
		//}

		FlipBltOp flipBltOp;
		pEditNode->edit_img.OperateBlt(
			flipBltOp,
			0,
			0,
			pEditNode->blt_rect.right - pEditNode->blt_rect.left,
			pEditNode->blt_rect.bottom - pEditNode->blt_rect.top,
			pMask->GetMaskImage(),
			pEditNode->blt_rect.left,
			pEditNode->blt_rect.top);

		//
		pEditMaskHandle->Update( &(pEditNode->blt_rect) );
		pEditMaskHandle->EndEdit();
	}

	return FALSE;
}