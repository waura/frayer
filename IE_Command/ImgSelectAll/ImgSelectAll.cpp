// ImgSelectAll.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//
#include "stdafx.h"
#include "ImgSelectAll.h"

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

#define DISPLAY_NAME_JA "全て選択する"
#define DISPLAY_NAME DISPLAY_NAME_JA

IECOMMAND_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::COMMAND;
}

IECOMMAND_EXPORT void GetIECommandName(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, "ImgSelectAll");
}

IECOMMAND_EXPORT void GetIECommandDisplayNameJa(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, DISPLAY_NAME);
}

IECOMMAND_EXPORT IIECommand* CreateIECommand()
{
	return new ImgSelectAll;
}

IECOMMAND_EXPORT void ReleaseIECommand(IIECommand** ppCommand)
{
	if(*ppCommand){
		delete (*ppCommand);
		(*ppCommand) = NULL;
	}
}

ImgSelectAll::ImgSelectAll()
{
  	strcpy_s(m_name, MAX_IE_MODULE_NAME, "ImgSelectAll");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

BOOL ImgSelectAll::Run(ImgEdit* pEdit, void* pvoid)
{
	ImgFile_Ptr f = pEdit->GetActiveFile();
	if(f){
		const IplImage* pMask = f->GetDrawMaskImg();
		CvSize size = f->GetImgSize();

		EditMaskHandle* pEditMaskHandle = (EditMaskHandle*)f->CreateImgFileHandle(IFH_EDIT_MASK);
		strcpy_s(pEditMaskHandle->handle_name, MAX_IMG_FILE_HANDLE_NAME, DISPLAY_NAME);
		f->DoImgFileHandle( pEditMaskHandle );
	
		EditNode* pEditNode;
		pEditNode = pEditMaskHandle->CreateEditNode();

		pEditNode->edit_img.FillMask(255);

		//
		pEditMaskHandle->Update( &(pEditNode->blt_rect) );
		pEditMaskHandle->EndEdit();
	}
	
	return FALSE;
}