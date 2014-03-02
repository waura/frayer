// ImgSelectRelease.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//
#include "stdafx.h"
#include "ImgSelectRelease.h"


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

#define DISPLAY_NAME_JA "選択を解除する"
#define DISPLAY_NAME DISPLAY_NAME_JA


IECOMMAND_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::COMMAND;
}

IECOMMAND_EXPORT void GetIECommandName(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, "ImgSelectRelease");
}

IECOMMAND_EXPORT void GetIECommandDisplayNameJa(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, DISPLAY_NAME);
}

IECOMMAND_EXPORT IIECommand* CreateIECommand()
{
	return new ImgSelectRelease;
}

IECOMMAND_EXPORT void ReleaseIECommand(IIECommand** ppCommand)
{
	if (*ppCommand) {
		delete (*ppCommand);
		(*ppCommand) = NULL;
	}
}

ImgSelectRelease::ImgSelectRelease()
{
  	strcpy_s(m_name, MAX_IE_MODULE_NAME, "ImgSelectRelease");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

BOOL ImgSelectRelease::Run(ImgEdit* pEdit, void* pvoid)
{
	ImgFile_Ptr f = pEdit->GetActiveFile();
	if(f){
		RECT rc;
		ImgMask_Ptr pMask = f->GetSelectMask();
		f->GetMaskRect(&rc);

		EditMaskHandle* pEditMaskHandle = (EditMaskHandle*)f->CreateImgFileHandle(IFH_EDIT_MASK);
		strcpy_s(pEditMaskHandle->handle_name, MAX_IMG_FILE_HANDLE_NAME, DISPLAY_NAME);
		f->DoImgFileHandle( pEditMaskHandle );
	
		EditNode* pEditNode;
		pEditNode = pEditMaskHandle->CreateEditNode(&rc);
		pEditNode->fourcc = IE_MASK_SYNTH_FOURCC('s','u','b',' ');
		
		pEditNode->edit_img.FillMask(255);

		//
		pEditMaskHandle->Update(&rc);
		pEditMaskHandle->EndEdit();
	}

	return FALSE;
}