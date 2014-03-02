#pragma once


class ImgEditExt;
class IEWindowMG;
class IEConfigDialog;

typedef struct _IE_CONFIG_DIALOG_DATA {
	IEConfigDialog *pIEConfigDialog;
	ImgEdit *pImgEdit;
	CFont* pDialogFont;
}IE_CONFIG_DIALOG_DATA;


class IIEConfig_DialogComponent
{
public:
	IIEConfig_DialogComponent(){};
	virtual ~IIEConfig_DialogComponent(){};

	virtual void InitDialog(HWND hDialog, HINSTANCE hInst, IE_CONFIG_DIALOG_DATA *pConfigData) = 0;
	virtual void Submit() = 0;
	virtual BOOL OnMessage(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) = 0;
};
