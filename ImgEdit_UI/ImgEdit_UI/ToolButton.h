#pragma once

#define TOOL_BUTTON_W 42
#define TOOL_BUTTON_H 26

enum BUTTONSTATE{
	NEWTRAL=0,
	MOUSEON=1,
	CLICK=2,
};

class CToolButton
{
public:
	CToolButton();
	virtual ~CToolButton();

	void SetButton(IIETool* pTool);
	IIETool* GetTool(){
		return m_pTool;
	}

	const char* GetToolDisplayName(){
		return m_pTool->GetDisplayName();
	}

	bool isClicked();

	void Draw(HDC hdc, int x, int y);
	
	BUTTONSTATE GetButtonState();

	void OnNewtral();
	void OnMouseOn();
	void OnClick();

private:
	BUTTONSTATE m_buttonState;

	IIETool* m_pTool;
	HBITMAP m_hBmp;
};
