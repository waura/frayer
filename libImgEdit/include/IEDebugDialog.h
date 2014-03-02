#pragma once

#include "LibIEDllHeader.h"
#include "OutputError.h"

class _EXPORTCPP IEDebugDialog
{
public:
	IEDebugDialog();
	~IEDebugDialog();

	//////////////////////////////////
	/*!
		ダイアログの表示
		@param[in] hInst
		@param[in] hwndParent
		@param[in] image 
	*/
	INT_PTR CreateDialogBox(HINSTANCE hInst, HWND hwndParent, IplImageExt* image);

	static BOOL CALLBACK DialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
};