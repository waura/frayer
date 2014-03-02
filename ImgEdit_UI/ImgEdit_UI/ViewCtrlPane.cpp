#include "stdafx.h"
#include "../resource1.h"

#include "ViewCtrlPane.h"

#define ID_RIGHT_ROT_BTN 1
#define ID_LEFT_ROT_BTN 2
#define ID_FLIP_H_BTN 3
#define ID_FLIP_V_BTN 4


#define LEFT_ROT_BTN_X 0
#define LEFT_ROT_BTN_Y 0
#define LEFT_ROT_BTN_W 24
#define LEFT_ROT_BTN_H 24

#define RIGHT_ROT_BTN_X ((24 + 1) * 1)
#define RIGHT_ROT_BTN_Y 0
#define RIGHT_ROT_BTN_W 24
#define RIGHT_ROT_BTN_H 24

#define FLIP_H_BTN_X ((24 + 1) * 2)
#define FLIP_H_BTN_Y 0
#define FLIP_H_BTN_W 24
#define FLIP_H_BTN_H 24

#define FLIP_V_BTN_X ((24 + 1) * 3)
#define FLIP_V_BTN_Y 0
#define FLIP_V_BTN_W 24
#define FLIP_V_BTN_H 24

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern ImgEdit g_ImgEdit;

CViewCtrlPane::CViewCtrlPane()
{
	m_pEditFile = NULL;
}

CViewCtrlPane::~CViewCtrlPane()
{
}

void CViewCtrlPane::OnChangeActiveFile(ImgFile_Ptr pNewFile, ImgFile_Ptr pOldFile)
{
	if (m_pEditFile) {
		m_pEditFile->DeleteEventListener(this);
	}

	if (pNewFile) {
		pNewFile->AddEventListener(this);
	}

	m_pEditFile = pNewFile;
	if (m_pEditFile) {
		m_FlipHBtn.SetToggle(m_pEditFile->IsViewFlipH());
		m_FlipVBtn.SetToggle(m_pEditFile->IsViewFlipV());
	} else {
		m_FlipHBtn.SetToggle(false);
		m_FlipVBtn.SetToggle(false);
	}
}

void CViewCtrlPane::OnChangeViewFlipH(bool state)
{
	m_FlipHBtn.SetToggle(state);
}

void CViewCtrlPane::OnChangeViewFlipV(bool state)
{
	m_FlipVBtn.SetToggle(state);
}

BOOL CViewCtrlPane::PreTranslateMessage(MSG *pMsg)
{
	m_ToolTip.RelayEvent( pMsg );
	return CWnd::PreTranslateMessage( pMsg );
}

BEGIN_MESSAGE_MAP(CViewCtrlPane, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_BN_CLICKED(ID_RIGHT_ROT_BTN, OnClickedRightRotBtn)
	ON_BN_CLICKED(ID_LEFT_ROT_BTN, OnClickedLeftRotBtn)
	ON_BN_CLICKED(ID_FLIP_H_BTN, OnClickedFlipHBtn)
	ON_BN_CLICKED(ID_FLIP_V_BTN, OnClickedFlipVBtn)
END_MESSAGE_MAP()

int CViewCtrlPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(0 != CWnd::OnCreate( lpCreateStruct )){
		return -1;
	}

	//register event listener
	g_ImgEdit.AddEventListener(this);

	RECT rc;

	//init view left button
	rc.top = LEFT_ROT_BTN_Y;
	rc.left = LEFT_ROT_BTN_X;
	rc.bottom = rc.top + LEFT_ROT_BTN_H;
	rc.right = rc.left + LEFT_ROT_BTN_W;
	if(!m_LeftRotBtn.Create("",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		BS_OWNERDRAW | BS_PUSHBUTTON,
		rc, this, ID_LEFT_ROT_BTN))
	{
			return -1;
	}
	m_LeftRotBtn.LoadBitmaps(
		IDB_VIEW_LEFT_ROT_A,
		IDB_VIEW_LEFT_ROT_B,
		IDB_VIEW_LEFT_ROT_C);

	//init view right button
	rc.top = RIGHT_ROT_BTN_Y;
	rc.left = RIGHT_ROT_BTN_X;
	rc.bottom = rc.top + RIGHT_ROT_BTN_H;
	rc.right = rc.left + RIGHT_ROT_BTN_W;
	if(!m_RightRotBtn.Create("",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		BS_OWNERDRAW | BS_PUSHBUTTON,
		rc, this, ID_RIGHT_ROT_BTN))
	{
			return -1;
	}
	m_RightRotBtn.LoadBitmaps(
		IDB_VIEW_RIGHT_ROT_A,
		IDB_VIEW_RIGHT_ROT_B,
		IDB_VIEW_RIGHT_ROT_C);

	//init view flip H button
	rc.top = FLIP_H_BTN_Y;
	rc.left = FLIP_H_BTN_X;
	rc.bottom = rc.top + FLIP_H_BTN_H;
	rc.right = rc.left + FLIP_H_BTN_W;
	if(!m_FlipHBtn.Create("",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		BS_OWNERDRAW | BS_PUSHBUTTON,
		rc, this, ID_FLIP_H_BTN))
	{
			return -1;
	}
	m_FlipHBtn.LoadBitmaps(
		IDB_VIEW_FLIP_H_A,
		IDB_VIEW_FLIP_H_B,
		IDB_VIEW_FLIP_H_C);

	//init view flip V button
	rc.top = FLIP_V_BTN_Y;
	rc.left = FLIP_V_BTN_X;
	rc.bottom = rc.top + FLIP_V_BTN_H;
	rc.right = rc.left + FLIP_V_BTN_W;
	if(!m_FlipVBtn.Create("",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		BS_OWNERDRAW | BS_PUSHBUTTON,
		rc, this, ID_FLIP_V_BTN))
	{
			return -1;
	}
	m_FlipVBtn.LoadBitmaps(
		IDB_VIEW_FLIP_V_A,
		IDB_VIEW_FLIP_V_B,
		IDB_VIEW_FLIP_V_C);

	//regist tool tip
	m_ToolTip.Create(this);
	m_ToolTip.AddTool(&m_LeftRotBtn, TEXT("ç∂âÒì]"));
	m_ToolTip.AddTool(&m_RightRotBtn, TEXT("âEâÒì]"));
	m_ToolTip.AddTool(&m_FlipHBtn, TEXT("ç∂âEîΩì]"));
	m_ToolTip.AddTool(&m_FlipVBtn, TEXT("è„â∫îΩì]"));

	return 0;
}

void CViewCtrlPane::OnSize(UINT nType, int cx, int cy)
{
	return CWnd::OnSize(nType, cx, cy);
}

void CViewCtrlPane::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetWindowRect(rect);
	ScreenToClient(rect);

	rect.InflateRect(1, 1);
	dc.FillSolidRect(rect, RGB(230, 230, 230));
}

void CViewCtrlPane::OnClickedRightRotBtn()
{
	if (m_pEditFile) {
		int rot = static_cast<int>(m_pEditFile->GetViewRot());
		int next_rot = ((rot / 45) - 1) * 45;
		m_pEditFile->SetViewRot(static_cast<double>(next_rot));

		//update display image
		LPUPDATE_DATA data = m_pEditFile->CreateUpdateData();
		data->isAll = true;
		m_pEditFile->PushUpdateData(data);
	}
}

void CViewCtrlPane::OnClickedLeftRotBtn()
{
	if (m_pEditFile) {
		int rot = static_cast<int>(m_pEditFile->GetViewRot());
		int next_rot = ((rot / 45) + 1) * 45;
		m_pEditFile->SetViewRot(static_cast<double>(next_rot));

		//update display image
		LPUPDATE_DATA data = m_pEditFile->CreateUpdateData();
		data->isAll = true;
		m_pEditFile->PushUpdateData(data);
	}
}

void CViewCtrlPane::OnClickedFlipHBtn()
{
	//change button state
	if (m_FlipHBtn.GetToggle()) {
		m_FlipHBtn.SetToggle(false);
	} else {
		m_FlipHBtn.SetToggle(true);
	}

	//
	if (m_pEditFile) {
		m_pEditFile->SetViewFlipH(m_FlipHBtn.GetToggle());

		//update display image
		LPUPDATE_DATA data = m_pEditFile->CreateUpdateData();
		data->isAll = true;
		m_pEditFile->PushUpdateData(data);
	}
}

void CViewCtrlPane::OnClickedFlipVBtn()
{
	//change button state
	if (m_FlipVBtn.GetToggle()) {
		m_FlipVBtn.SetToggle(false);
	} else {
		m_FlipVBtn.SetToggle(true);
	}

	//
	if (m_pEditFile) {
		m_pEditFile->SetViewFlipV(m_FlipVBtn.GetToggle());

		//update display image
		LPUPDATE_DATA data = m_pEditFile->CreateUpdateData();
		data->isAll = true;
		m_pEditFile->PushUpdateData(data);
	}
}