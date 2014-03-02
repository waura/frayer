#ifndef _LOADFROMTWAIN_H_
#define _LOADFROMTWAIN_H_

#include <libImgEdit.h>

class LoadFromTWAIN : public IIECommand{
public:
	LoadFromTWAIN(){}
	~LoadFromTWAIN(){}

	bool Init(ImgEdit* pEdit);
	void End(ImgEdit* pEdit);

	BOOL Run(ImgEdit* pEdit, void* pvoid);

	static LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam);

private:
	static HHOOK m_hHook;
	HMODULE m_hDLL;
	HWND m_hWnd;

	ImgEdit* m_pImgEdit;
};

#endif //_LOADFROMTWAIN_H_