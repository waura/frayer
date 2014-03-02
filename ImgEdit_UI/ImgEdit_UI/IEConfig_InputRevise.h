#pragma once

#include "IIEConfig_DialogComponent.h"

class IEConfig_InputRevise : public IIEConfig_DialogComponent
{
public:
	IEConfig_InputRevise();
	~IEConfig_InputRevise();

	void InitDialog(HWND hDialog, HINSTANCE hInst, IE_CONFIG_DIALOG_DATA *pConfigData);
	void Submit();
	BOOL OnMessage(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

private:
	HWND m_hCombo;
	IE_CONFIG_DIALOG_DATA *m_pConfigData;

	char select_revise_name[256];
};
