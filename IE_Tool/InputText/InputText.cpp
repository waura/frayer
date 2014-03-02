#include "InputText.h"

// for detect memory leak
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <cstdlib>
	#include <crtdbg.h>
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

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

IETOOL_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::TOOL;
}

IETOOL_EXPORT void GetIEToolName(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, "InputText");
}

IETOOL_EXPORT void GetIEToolDisplayNameJa(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_DISPLAY_NAME, "テキスト入力");
}


IETOOL_EXPORT IIETool* CreateIETool()
{
	return new InputText;
}

IETOOL_EXPORT void ReleaseIETool(IIETool** ppTool)
{
	if (*ppTool) {
		delete (*ppTool);
		(*ppTool) = NULL;
	}
}

InputText::InputText()
{
}

InputText::~InputText()
{
}

HBITMAP InputText::GetButtonImg()
{
	return m_buttonImg;
}

void InputText::OnMouseLButtonDown(LPIE_INPUT_DATA lpd)
{
}

void InputText::OnMouseLDrag(LPIE_INPUT_DATA lpd)
{
}

void InputText::OnMouseLButtonUp(LPIE_INPUT_DATA lpd)
{
}