#pragma once

#include "IIEConfig_DialogComponent.h"
#include "IEColor.h"

#define IEENV_PROP_GRID_X 180
#define IEENV_PROP_GRID_Y 10
#define IEENV_PROP_GRID_W 340
#define IEENV_PROP_GRID_H 400

class IEConfig_Environment : public IIEConfig_DialogComponent
{
public:
	IEConfig_Environment();
	virtual ~IEConfig_Environment();

	virtual void InitDialog(HWND hDialog, HINSTANCE hInst, IE_CONFIG_DIALOG_DATA *pConfigData);
	virtual void Submit();
	virtual BOOL OnMessage(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
private:
	IE_CONFIG_DIALOG_DATA *m_pConfigData;

	CMFCPropertyGridCtrl m_wndEnvPropGrid;

	CMFCPropertyGridColorProperty* m_pBGColorProp;
	CMFCPropertyGridProperty* m_pNumOfHistoryProp;
};
