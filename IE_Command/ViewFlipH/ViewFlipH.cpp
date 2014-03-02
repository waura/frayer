// ViewFlipH.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include "ViewFlipH.h"

// for detect memory leak
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <cstdlib>
	#include <crtdbg.h>
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#define DISPLAY_NAME_JA "表示左右反転"
#define DISPLAY_NAME DISPLAY_NAME_JA

IECOMMAND_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::COMMAND;
}

IECOMMAND_EXPORT void GetIECommandName(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, "ViewFlipH");
}

IECOMMAND_EXPORT void GetIECommandDisplayNameJa(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, DISPLAY_NAME_JA);
}

IECOMMAND_EXPORT IIECommand* CreateIECommand()
{
	return new ViewFlipH;
}

IECOMMAND_EXPORT void ReleaseIECommand(IIECommand** ppCommand)
{
	if (*ppCommand) {
		delete (*ppCommand);
		(*ppCommand) = NULL;
	}
}

ViewFlipH::ViewFlipH()
{
  	strcpy_s(m_name, MAX_IE_MODULE_NAME, "ViewFlipH");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

BOOL ViewFlipH::Run(ImgEdit* pEdit, void* pvoid)
{
	ImgFile_Ptr f = pEdit->GetActiveFile();
	if (!f) return FALSE;

	if (f->IsViewFlipH()) {
		f->SetViewFlipH(false);
	} else {
		f->SetViewFlipH(true);
	}

	//update display
	LPUPDATE_DATA data = f->CreateUpdateData();
	data->isAll = true;
	f->PushUpdateData(data);

	return FALSE;
}