// EndEventCapture.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include "EndEventCapture.h"

// for detect memory leak
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <cstdlib>
	#include <crtdbg.h>
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#define DISPLAY_NAME "イベントキャプチャを終了する"

IECOMMAND_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::COMMAND;
}

IECOMMAND_EXPORT void GetIECommandName(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, "EndEventCapture");
}

IECOMMAND_EXPORT void GetIECommandDisplayNameJa(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, DISPLAY_NAME);
}

IECOMMAND_EXPORT IIECommand* CreateIECommand()
{
	return new EndEventCapture;
}

IECOMMAND_EXPORT void ReleaseIECommand(IIECommand** ppCommand)
{
	if(*ppCommand){
		delete (*ppCommand);
		(*ppCommand) = NULL;
	}
}

EndEventCapture::EndEventCapture()
{
  	strcpy_s(m_name, MAX_IE_MODULE_NAME, "EndEventCapture");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

EndEventCapture::~EndEventCapture()
{
}

BOOL EndEventCapture::Run(ImgEdit *pEdit, void* pvoid)
{
	pEdit->EndEventCapture();
	return FALSE;
}