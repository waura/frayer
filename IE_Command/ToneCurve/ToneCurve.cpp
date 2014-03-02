#include "stdafx.h"
#include "ToneCurve.h"
#include "resource.h"

static HMODULE g_hModule;

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_hModule = hModule;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

// for detect memory leak
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <cstdlib>
	#include <crtdbg.h>
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#define DISPLAY_NAME_JA "トーンカーブ"
#define DISPLAY_NAME DISPLAY_NAME_JA

#define TC_GRAPH_X 60
#define TC_GRAPH_Y 50
#define TC_GRAPH_W 256
#define TC_GRAPH_H 256

#define TC_SRC_POINT_RAD 7

#define TC_RGB 0
#define TC_R 1
#define TC_G 2
#define TC_B 3
#define TC_A 4

IECOMMAND_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::COMMAND;
}

IECOMMAND_EXPORT void GetIECommandName(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, "ToneCurve");
}

IECOMMAND_EXPORT void GetIECommandDisplayNameJa(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, DISPLAY_NAME);
}


LRESULT CALLBACK ToneCurveDlgProc(HWND, UINT, WPARAM, LPARAM);

IECOMMAND_EXPORT IIECommand* CreateIECommand()
{
	return new ToneCurve;
}

IECOMMAND_EXPORT void ReleaseIECommand(IIECommand** ppCommand)
{
	if (*ppCommand) {
		delete (*ppCommand);
		(*ppCommand) = NULL;
	}
}

ToneCurve::ToneCurve()
{
	m_graph_img = NULL;
	m_src_points = NULL;
	m_pEditNode = NULL;
	m_pEditFile = NULL;

  	strcpy_s(m_name, MAX_IE_MODULE_NAME, "ToneCurve");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

bool ToneCurve::Init(ImgEdit* pEdit)
{
	m_graph_img = cvCreateImage(cvSize(TC_GRAPH_W, TC_GRAPH_H), IPL_DEPTH_8U, 3);

	m_num_of_src_points = 2;
	m_src_points = new CvPoint2D64f[ m_num_of_src_points ];
	m_src_points[0].x = 0; m_src_points[0].y = 0;
	m_src_points[1].x = 255; m_src_points[1].y = 255;

	m_BGRA_LUT = cvCreateMat(1, 256, CV_8UC4);

	m_pEditFile = pEdit->GetActiveFile();
	if (!m_pEditFile) return false;

	IImgLayer_weakPtr _sl = m_pEditFile->GetSelectLayer();
	if (_sl.expired()) {
		OutputError::Alert(IE_ERROR_SELECT_LAYER_DONT_EXIST);
		return false;
	}

	IImgLayer_Ptr sl = _sl.lock();
	if (sl->GetLayerType() != IE_LAYER_TYPE::NORMAL_LAYER) {
		OutputError::Alert(IE_ERROR_CANT_EDIT_LAYER);
		return false;
	}
	ImgLayer_Ptr l = std::dynamic_pointer_cast<ImgLayer>(sl);

	IplImage* mask = NULL;
	//選択マスクの範囲を取得
	bool is_invalid_mask = (m_pEditFile->GetMaskState() == IE_MASK_STATE_ID::INVALID);
	if (is_invalid_mask) {
		CvSize size = m_pEditFile->GetImgSize();
		m_mask_rect.top = m_mask_rect.left = 0;
		m_mask_rect.right = size.width;
		m_mask_rect.bottom = size.height;
	}
	else {
		RECT rect;
		l->GetLayerRect(&rect);

		m_pEditFile->GetMaskRect(&m_mask_rect);
		//
		AndRect(&m_mask_rect, &m_mask_rect, &rect);

		//マスク切り出し
		mask = cvCreateImage(
			cvSize(m_mask_rect.right - m_mask_rect.left, m_mask_rect.bottom - m_mask_rect.top),
			IPL_DEPTH_8U,
			1);

		m_pEditFile->CopyFileMask(
			m_mask_rect.left,
			m_mask_rect.top,
			m_mask_rect.right - m_mask_rect.left,
			m_mask_rect.bottom - m_mask_rect.top,
			mask,
			0, 0);
	}

	int widht = m_mask_rect.right - m_mask_rect.left;
	int height = m_mask_rect.bottom - m_mask_rect.top;

	m_pEditLayerHandle = (EditLayerHandle*) m_pEditFile->CreateImgFileHandle( IFH_EDIT_LAYER );
	m_pEditLayerHandle->SetHandleName( DISPLAY_NAME );
	m_pEditFile->DoImgFileHandle( m_pEditLayerHandle );

	m_pEditNode = m_pEditLayerHandle->CreateEditNode(&m_mask_rect);

	//get undo image
	l->CopyLayerImage(
		&(m_pEditNode->undo_img),
		0,
		0,
		m_mask_rect.left,
		m_mask_rect.top,
		m_mask_rect.right - m_mask_rect.left,
		m_mask_rect.bottom - m_mask_rect.top);

	//
	if(is_invalid_mask){
		CvSize size = m_pEditNode->edit_img.GetSize();
		m_pEditNode->edit_img.RectFillAlpha(
			0,
			0,
			size.width,
			size.height,
			255);
	}
	else{
		m_pEditNode->edit_img.RectFillAlpha(
			m_mask_rect.left,
			m_mask_rect.top,
			m_mask_rect.right - m_mask_rect.left,
			m_mask_rect.bottom - m_mask_rect.top,
			255,
			NULL);
	}

	if(mask){
		cvReleaseImage(&mask);
	}

	return true;
}

void ToneCurve::End(ImgEdit* pEdit)
{
	m_pEditLayerHandle->EndEdit();

	if(m_graph_img){
		cvReleaseImage(&m_graph_img);
	}
	if(m_BGRA_LUT){
		cvReleaseMat(&m_BGRA_LUT);
	}
	if(m_src_points){
		delete[] m_src_points;
		m_src_points = NULL;
	}
}

BOOL ToneCurve::Run(ImgEdit* pEdit, void* pvoid)
{
	if(m_pEditNode == NULL) return FALSE;

	int ret = ::DialogBoxParam((HINSTANCE)g_hModule, MAKEINTRESOURCE(IDD_TONE_CURVE_DIALOG),
		::GetForegroundWindow(), (DLGPROC)ToneCurveDlgProc, (LPARAM)this);

	if(ret == IDCANCEL){
		ImgFile_Ptr f = pEdit->GetActiveFile();
		if(f){
			m_pEditLayerHandle->EndEdit();
			f->Undo(1);
		}
	}

	return FALSE;
}

int ToneCurve::GetSrcPoint(int x, int y)
{
	int i;
	for(i=1; i<m_num_of_src_points-1; i++){
		if(((m_src_points[i].x - TC_SRC_POINT_RAD) <= x && x <= (m_src_points[i].x + TC_SRC_POINT_RAD)) &&
			((m_src_points[i].y - TC_SRC_POINT_RAD) <= y && y <= (m_src_points[i].y + TC_SRC_POINT_RAD))){
				return i;
		}
	}

	return -1;
}

int ToneCurve::AddPoint(int x, int y)
{
	CvPoint2D64f* new_points = new CvPoint2D64f[m_num_of_src_points+1];

	int i,j;
	int ret_index;
	for(i=0,j=0; i<	m_num_of_src_points; i++){

		new_points[j++] = m_src_points[i];

		if(m_src_points[i].x == x){
			delete[] new_points;
			new_points = NULL;

			m_src_points[i].y = y;
			return i;
		}
		else if(i != m_num_of_src_points){
			if((m_src_points[i].x < x) && (x < m_src_points[i+1].x)){
				ret_index = j;
				new_points[j].x = x;
				new_points[j].y = y;
				j++;
			}
		}
	}

	//
	delete[] m_src_points;
	m_src_points = new_points;

	m_num_of_src_points++;
	return ret_index;
}

int ToneCurve::EditPoint(int point_index, int x, int y)
{
	assert((0 <= point_index) && (point_index < m_num_of_src_points));

	if((0 <= point_index) && (point_index < m_num_of_src_points)){
		int i,j;
		for(i=0; i<m_num_of_src_points; i++){
			if(m_src_points[i].x == x){
				return point_index;
			}
		}

		m_src_points[ point_index ].x = x;
		m_src_points[ point_index ].y = y;

		//sort
		int ret_index = point_index;
		for(i=0; i<m_num_of_src_points-1; i++){
			for(j=i+1; j<m_num_of_src_points; j++){
				if(m_src_points[i].x > m_src_points[j].x){
					//swap src point
					CvPoint2D64f tmp = m_src_points[j];
					m_src_points[j] = m_src_points[i];
					m_src_points[i] = tmp;

					if(j == ret_index) ret_index = i;
					else if(i == ret_index) ret_index =j;
				}
			}
		}

		return ret_index;
	}

	return point_index;
}

void ToneCurve::RebuildGraph()
{
	int i;

	//clear graph image
	cvSet(m_graph_img, cvScalar(255, 255, 255));

	//draw grid
	double dd = 255 /4;
	for(i=1; i<=3; i++){
		DrawLine(
			m_graph_img,
			cvPoint(dd*i, 0),
			cvPoint(dd*i, 255),
			CV_RGB(200, 200, 200),
			1);
		DrawLine(
			m_graph_img,
			cvPoint(0, dd*i),
			cvPoint(255, dd*i),
			CV_RGB(200, 200, 200),
			1);
	}
	DrawLine(
		m_graph_img,
		cvPoint(0, 0),
		cvPoint(0, 255),
		CV_RGB(200, 200, 200),
		1);
	DrawLine(
		m_graph_img,
		cvPoint(0, 0),
		cvPoint(255, 0),
		CV_RGB(200, 200, 200),
		1);
	DrawLine(
		m_graph_img,
		cvPoint(255, 0),
		cvPoint(255, 255),
		CV_RGB(200, 200, 200),
		1);
	DrawLine(
		m_graph_img,
		cvPoint(0, 255),
		cvPoint(255, 255),
		CV_RGB(200, 200, 200),
		1);

	//draw spline
	CvMat* c_mat = cvCreateMat(m_num_of_src_points, 1, CV_32F);
	SolveSpline3D( m_num_of_src_points, m_src_points, c_mat);

	int x,y;
	for(x=0; x<256; x++){
		y = this->FuncToneCurve( m_num_of_src_points, x, m_src_points, c_mat);
		//set look up table
		switch(m_edit_channel_index){
			case TC_RGB:
				cvSet1D(m_BGRA_LUT, x, cvScalar(y, y, y, x));
				break;
			case TC_R:
				cvSet1D(m_BGRA_LUT, x, cvScalar(x, x, y, x));
				break;
			case TC_G:
				cvSet1D(m_BGRA_LUT, x, cvScalar(x, y, x, x));
				break;
			case TC_B:
				cvSet1D(m_BGRA_LUT, x, cvScalar(y, x, x, x));
				break;
			case TC_A:
				cvSet1D(m_BGRA_LUT, x, cvScalar(x, x, x, y));
				break;
		}
	}

	uint8_t b1, g1, r1, a1;
	uint8_t b2, g2, r2, a2;
	for(x=0; x<255; x++){

		CvScalar cs1 = cvGet1D(m_BGRA_LUT, x);
		CvScalar cs2 = cvGet1D(m_BGRA_LUT, x+1);

		//get look up table
		switch(m_edit_channel_index){
			case TC_RGB:
				DrawLine(
					m_graph_img,
					cvPoint(x, 255 - cs1.val[0]),
					cvPoint(x+1, 255 - cs2.val[0]),
					CV_RGB(0, 0, 0),
					1,
					8);
				break;
			case TC_R:
				DrawLine(
					m_graph_img,
					cvPoint(x, 255 - cs1.val[2]),
					cvPoint(x+1, 255 - cs2.val[2]),
					CV_RGB(255, 0, 0),
					1,
					8);
				break;
			case TC_G:
				DrawLine(
					m_graph_img,
					cvPoint(x, 255 - cs1.val[1]),
					cvPoint(x+1, 255 - cs2.val[1]),
					CV_RGB(0, 255, 0),
					1,
					8);
				break;
			case TC_B:
				DrawLine(
					m_graph_img,
					cvPoint(x, 255 - cs1.val[0]),
					cvPoint(x+1, 255 - cs2.val[0]),
					CV_RGB(0, 0, 255),
					1,
					8);
				break;
			case TC_A:
				DrawLine(
					m_graph_img,
					cvPoint(x, 255 - cs1.val[3]),
					cvPoint(x+1, 255 - cs2.val[3]),
					CV_RGB(0, 0, 0),
					1,
					8);
				break;
		}
	}

	//draw src point
	for(i=0; i<m_num_of_src_points; i++){
		x = (int)m_src_points[i].x;
		y = (int)m_src_points[i].y;
		cvCircle(m_graph_img, cvPoint(x, 255-y), 3, CV_RGB(255, 0, 0), 1, CV_AA);
	}
}

int ToneCurve::FuncToneCurve(int n, int x, const CvPoint2D64f* src_points, const CvMat* c_mat)
{
	int y;
	y = FuncSpline3D( n, x, src_points, c_mat);
	y = (y > 255) ? 255 : y;
	y = (y < 0) ? 0 : y;
	return y;
}

unsigned _stdcall UpdateImageThread(void* data)
{
	LPTONE_CURVE_DATA pTCData = (LPTONE_CURVE_DATA)data;
	RECT mask_rect = pTCData->mask_rect;
	CvMat* BGRA_LUT = pTCData->BGRA_LUT;
	EditNode* pEditNode = pTCData->pEditNode;
	EditLayerHandle* pEditLayerHandle = pTCData->pEditLayerHandle;

	pEditNode->edit_img.LUT(
		mask_rect.left,
		mask_rect.top,
		mask_rect.right - mask_rect.left,
		mask_rect.bottom - mask_rect.top,
		&(pEditNode->undo_img),
		mask_rect.left,
		mask_rect.top,
		BGRA_LUT);

	pEditLayerHandle->Update(&mask_rect);
	return 0;
}

void ToneCurve::UpdateImage()
{
	DWORD dwExCode;
	::GetExitCodeThread(m_hUpdateThread, &dwExCode);
	if(dwExCode == STILL_ACTIVE){
	}
	else{
		unsigned int thId;
		m_TCData.mask_rect = m_mask_rect;
		m_TCData.BGRA_LUT = m_BGRA_LUT;
		m_TCData.pEditNode = m_pEditNode;
		m_TCData.pEditLayerHandle = m_pEditLayerHandle;

		m_hUpdateThread = (HANDLE)_beginthreadex(
			NULL,
			0,
			UpdateImageThread,
			(void*)&m_TCData,
			0,
			&thId);
	}
}

LRESULT CALLBACK ToneCurveDlgProc(HWND hDlgWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static ToneCurve* pToneCurve;
	static bool isLDrag;
	static int point_index;
	static HWND hCombo;

	switch(msg){
		case WM_INITDIALOG:
			{
				pToneCurve = (ToneCurve*)lp;
				hCombo = ::GetDlgItem(hDlgWnd, IDC_TONE_CURVE_COMBO);
				//
				::SendMessage(hCombo, CB_INSERTSTRING, TC_RGB, (LPARAM)"RGB");
				::SendMessage(hCombo, CB_INSERTSTRING, TC_R, (LPARAM)"R");
				::SendMessage(hCombo, CB_INSERTSTRING, TC_G, (LPARAM)"G");
				::SendMessage(hCombo, CB_INSERTSTRING, TC_B, (LPARAM)"B");
				::SendMessage(hCombo, CB_INSERTSTRING, TC_A, (LPARAM)"A");
				//
				::SendMessage(hCombo, CB_SETCURSEL, TC_RGB, 0);
				pToneCurve->ChangeEditChannel(TC_RGB);
				pToneCurve->RebuildGraph();


				::InvalidateRect(hDlgWnd, NULL, FALSE);
			}
			return TRUE;
		case WM_PAINT:
			{
				IplImage* graph_img = pToneCurve->GetGraphImg();

				PAINTSTRUCT ps;
				HDC hdc = ::BeginPaint(hDlgWnd, &ps);
				IPLImageToDevice(
					hdc,
					TC_GRAPH_X,
					TC_GRAPH_Y,
					TC_GRAPH_W,
					TC_GRAPH_H,
					graph_img,
					0,
					0);
				::EndPaint(hDlgWnd, &ps);
			}
			return TRUE;
		case WM_LBUTTONDOWN:
			{
				::SetCapture(hDlgWnd);

				int x = LOWORD(lp);
				int y = HIWORD(lp);
				int graph_x = x - TC_GRAPH_X;
				int graph_y = TC_GRAPH_H - (y - TC_GRAPH_Y);

				graph_x = (graph_x < 0) ? 0 : graph_x;
				graph_x = (graph_x > TC_GRAPH_W) ? TC_GRAPH_W : graph_x; 
				graph_y = (graph_y < 0) ? 0 : graph_y;
				graph_y = (graph_y > TC_GRAPH_H) ? TC_GRAPH_H : graph_y;

				if((point_index = pToneCurve->GetSrcPoint(graph_x, graph_y)) == -1){
					point_index = pToneCurve->AddPoint(graph_x, graph_y);
					pToneCurve->RebuildGraph();
				}

				isLDrag = true;

				::InvalidateRect(hDlgWnd, NULL, FALSE);
			}
			return TRUE;
		case WM_MOUSEMOVE:
			{
				if(isLDrag){
					if(point_index != -1){
						int x = LOWORD(lp);
						int y = HIWORD(lp);
						int graph_x = x - TC_GRAPH_X;
						int graph_y = TC_GRAPH_H - (y - TC_GRAPH_Y);

						graph_x = (graph_x < 0) ? 0 : graph_x;
						graph_x = (graph_x > TC_GRAPH_W) ? TC_GRAPH_W : graph_x; 
						graph_y = (graph_y < 0) ? 0 : graph_y;
						graph_y = (graph_y > TC_GRAPH_H) ? TC_GRAPH_H : graph_y;

						point_index = pToneCurve->EditPoint(point_index, graph_x, graph_y);
						pToneCurve->RebuildGraph();
						::InvalidateRect(hDlgWnd, NULL, FALSE);

						pToneCurve->UpdateImage();
					}
				}
			}
			return TRUE;
		case WM_LBUTTONUP:
			{
				::ReleaseCapture();

				if(isLDrag){
					if(point_index != -1){
						int x = LOWORD(lp);
						int y = HIWORD(lp);
						int graph_x = x - TC_GRAPH_X;
						int graph_y = TC_GRAPH_H - (y - TC_GRAPH_Y);

						graph_x = (graph_x < 0) ? 0 : graph_x;
						graph_x = (graph_x > TC_GRAPH_W) ? TC_GRAPH_W : graph_x; 
						graph_y = (graph_y < 0) ? 0 : graph_y;
						graph_y = (graph_y > TC_GRAPH_H) ? TC_GRAPH_H : graph_y;					

						point_index = pToneCurve->EditPoint(point_index, graph_x, graph_y);
						pToneCurve->RebuildGraph();

						isLDrag = false;
						::InvalidateRect(hDlgWnd, NULL, FALSE);

						pToneCurve->UpdateImage();
					}
				}
			}
			return TRUE;
		case WM_COMMAND:
			switch(LOWORD(wp)){
				case IDOK:
					::EndDialog(hDlgWnd, IDOK);
					break;
				case IDCANCEL:
					::EndDialog(hDlgWnd, IDCANCEL);
					break;
				case IDC_TONE_CURVE_COMBO:
					if(HIWORD(wp) == CBN_SELCHANGE){
						int select_index = ::SendMessage(hCombo, CB_GETCURSEL, 0, 0);
						pToneCurve->ChangeEditChannel(select_index);
						pToneCurve->RebuildGraph();

						::InvalidateRect(hDlgWnd, NULL, FALSE);
					}
					break;
				default:
					return FALSE;
			}
			break;
		default:
			return FALSE;
	}
	return TRUE;
}