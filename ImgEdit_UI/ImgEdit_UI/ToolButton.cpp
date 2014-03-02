#include "StdAfx.h"

#include "ToolButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern ImgEdit g_ImgEdit;

CToolButton::CToolButton()
{
	m_buttonState = BUTTONSTATE::NEWTRAL;
	m_hBmp = NULL;
}

CToolButton::~CToolButton()
{
}

void CToolButton::SetButton(IIETool* pTool)
{
	m_pTool = pTool;
	m_hBmp = pTool->GetButtonImg();
}

bool CToolButton::isClicked()
{
	return (m_buttonState == BUTTONSTATE::CLICK);
}

void CToolButton::Draw(HDC hdc, int x, int y)
{
	if(!m_hBmp) return;

	int x0 = TOOL_BUTTON_W * m_buttonState;
	int y0 = 0;

	HGDIOBJ hOldBmp;
	HDC hMemDC = CreateCompatibleDC(NULL);
	hOldBmp = SelectObject(hMemDC, m_hBmp);

	BitBlt(
		hdc,
		x,
		y,
		TOOL_BUTTON_W,
		TOOL_BUTTON_H,
		hMemDC,
		x0, y0,
		SRCCOPY);
	SelectObject(hMemDC, hOldBmp);

	DeleteDC(hMemDC);
}

BUTTONSTATE CToolButton::GetButtonState()
{
	return m_buttonState;
}

void CToolButton::OnNewtral()
{
	m_buttonState = BUTTONSTATE::NEWTRAL;
}

void CToolButton::OnMouseOn()
{
	m_buttonState = BUTTONSTATE::MOUSEON;
}

void CToolButton::OnClick()
{
	m_buttonState = BUTTONSTATE::CLICK;
	g_ImgEdit.SetSelectTool( m_pTool );
}