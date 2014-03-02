// LevelCorrection.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include "LevelCorrection.h"
#include "resource.h"

// for detect memory leak
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <cstdlib>
	#include <crtdbg.h>
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#define DISPLAY_NAME_JA "レベル補正"
#define DISPLAY_NAME DISPLAY_NAME_JA

extern HMODULE g_hModule;

#define LC_RGB 0
#define LC_R 1
#define LC_G 2
#define LC_B 3

#define LC_HISTOGRAM_X 60
#define LC_HISTOGRAM_Y 50
#define LC_HISTOGRAM_W 256
#define LC_HISTOGRAM_H 128

#define LC_LEVEL_SLD_X 60
#define LC_LEVEL_SLD_Y 180
#define LC_LEVEL_SLD_W 256
#define LC_LEVEL_SLD_H 16

#define LC_LEVEL_GAM_SLD_COLOR CV_RGB(100,100,100)
#define LC_LEVEL_MAX_SLD_COLOR CV_RGB(255,255,255)
#define LC_LEVEL_MIN_SLD_COLOR CV_RGB(0,0,0)

#define LC_OUTPUT_SLD_X 60
#define LC_OUTPUT_SLD_Y 220
#define LC_OUTPUT_SLD_W 256
#define LC_OUTPUT_SLD_H 16

#define LC_OUTPUT_MAX_SLD_COLOR CV_RGB(255,255,255)
#define LC_OUTPUT_MIN_SLD_COLOR CV_RGB(0,0,0)

#define HISTOGRAM_MIN 0
#define HISTOGRAM_MAX 255

#define GAMMA_MIN (0.0)
#define GAMMA_MAX (10.0)

#define LC_SLD_RAD 7

#define FUNC_GAMMA(x) (10.0*pow(x, 3.321928))

IECOMMAND_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::COMMAND;
}

IECOMMAND_EXPORT  void GetIECommandName(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, "LevelCorrection");
}

IECOMMAND_EXPORT void GetIECommandDisplayNameJa(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, DISPLAY_NAME);
}

LRESULT CALLBACK LevelCorrectionDlgProc(HWND, UINT, WPARAM, LPARAM);

IECOMMAND_EXPORT IIECommand* CreateIECommand()
{
	return new LevelCorrection;
}

IECOMMAND_EXPORT void ReleaseIECommand(IIECommand** ppCommand)
{
	if (*ppCommand) {
		delete (*ppCommand);
		(*ppCommand) = NULL;
	}
}

LevelCorrection::LevelCorrection()
{
	m_max_level = 255;
	m_min_level = 0;
	m_max_out_val = 255;
	m_min_out_val = 0;
	m_level_sld_img = NULL;
	m_output_sld_img = NULL;
	m_BGRA_LUT = NULL;
	m_pEditNode = NULL;
	m_pEditFile = NULL;
	
	strcpy_s(m_name, MAX_IE_MODULE_NAME, "LevelCorrection");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, "レベル補正");
}

bool LevelCorrection::Init(ImgEdit* pEdit)
{
	m_is_gamma_dlg = false;
	m_is_max_level_dlg = false;
	m_is_min_level_dlg = false;
	m_is_max_out_dlg = false;
	m_is_min_out_dlg = false;

	m_gamma = 0.5;
	m_max_level = 255;
	m_min_level = 0;
	m_max_out_val = 255;
	m_min_out_val = 0;

	for(int i=0; i<4; i++){
		m_histogram_img[i] = cvCreateImage(cvSize(LC_HISTOGRAM_W, LC_HISTOGRAM_H), IPL_DEPTH_8U, 3);
		cvSet(m_histogram_img[i], cvScalar(255, 255, 255));
	}
	m_level_sld_img = cvCreateImage(cvSize(LC_LEVEL_SLD_W, LC_LEVEL_SLD_H), IPL_DEPTH_8U, 3);
	m_output_sld_img = cvCreateImage(cvSize(LC_OUTPUT_SLD_W, LC_OUTPUT_SLD_H), IPL_DEPTH_8U, 3);
	m_BGRA_LUT = cvCreateMat(1, 256, CV_8UC4);

	m_pEditFile = pEdit->GetActiveFile();
	if(!m_pEditFile) return false;

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

	//選択マスクの範囲を取得
	bool is_invalid_mask = (m_pEditFile->GetMaskState() == IE_MASK_STATE_ID::INVALID);
	if (is_invalid_mask) {
		CvSize size = m_pEditFile->GetImgSize();
		m_mask_rect.top = m_mask_rect.left = 0;
		m_mask_rect.right = size.width;
		m_mask_rect.bottom = size.height;
	}
	else {
		m_pEditFile->GetMaskRect(&m_mask_rect);
	}

	RECT layer_rect;
	l->GetLayerRect(&layer_rect);
	//
	AndRect(&m_mask_rect, &layer_rect, &m_mask_rect);

	int width = m_mask_rect.right - m_mask_rect.left;
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


	//create histogram img
	int bin_w;
	int hist_size = HISTOGRAM_MAX - HISTOGRAM_MIN;
	float max_value = 0;
	float range_0[] = { HISTOGRAM_MIN, HISTOGRAM_MAX };
	float *ranges[] = { range_0 };
	IplImage* ch_img[4];
	for(int i=0; i<4; i++){
		ch_img[i] = cvCreateImage(
			cvSize(m_mask_rect.right - m_mask_rect.left, m_mask_rect.bottom - m_mask_rect.top),
			IPL_DEPTH_8U,
			1);
	}
	CvHistogram* ch_hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
	m_pEditNode->undo_img.Split(
		m_mask_rect.left, m_mask_rect.top,
		m_mask_rect.right - m_mask_rect.left,
		m_mask_rect.bottom - m_mask_rect.top,
		ch_img[0], ch_img[1], ch_img[2], NULL);

	for(int i=0; i<3; i++){
		cvCalcHist(&(ch_img[i]), ch_hist, 0, NULL);
		cvGetMinMaxHistValue(ch_hist, 0, &max_value, 0, 0);
		cvScale (ch_hist->bins, ch_hist->bins, ((double) LC_HISTOGRAM_H) / max_value, 0);
		bin_w = cvRound((double) LC_HISTOGRAM_W/hist_size);
		for(int j=0; j<hist_size; j++){
			cvRectangle(
				m_histogram_img[i],
				cvPoint(j*bin_w, m_histogram_img[i]->height),
				cvPoint((j+1)*bin_w, m_histogram_img[i]->height - cvRound(cvGetReal1D(ch_hist->bins, j))),
				cvScalar(0, 0, 0),
				-1, 8, 0);
		}
	}

	RebuildLevelDlg();
	RebuildOutputDlg();

	return true;
}

void LevelCorrection::End(ImgEdit* pEdit)
{
	m_pEditLayerHandle->EndEdit();

	for(int i=0; i<4; i++){
		if(m_histogram_img[i]){
			cvReleaseImage(&m_histogram_img[i]);
		}
	}
	if(m_level_sld_img){
		cvReleaseImage(&m_level_sld_img);
	}
	if(m_output_sld_img){
		cvReleaseImage(&m_output_sld_img);
	}
	if(m_BGRA_LUT){
		cvReleaseMat(&m_BGRA_LUT);
	}
}

BOOL LevelCorrection::Run(ImgEdit* pEdit, void* pvoid)
{
	if(m_pEditNode == NULL) return FALSE;

	int ret = ::DialogBoxParam((HINSTANCE)g_hModule, MAKEINTRESOURCE(IDD_LEVEL_CORRECTION_DLG),
		::GetForegroundWindow(), (DLGPROC)LevelCorrectionDlgProc, (LPARAM)this);

	if(ret == IDCANCEL){
		ImgFile_Ptr f = pEdit->GetActiveFile();
		if(f){
			m_pEditLayerHandle->EndEdit();
			f->Undo(1);
		}
	}

	return FALSE;
}

unsigned _stdcall UpdateImageThread(void* data)
{
	LPLEVEL_CORR_DATA pLCorrData = (LPLEVEL_CORR_DATA)data;
	double gamma = pLCorrData->gamma;
	int min_level = pLCorrData->min_level;
	int max_level = pLCorrData->max_level;
	int min_out_val = pLCorrData->min_out_val;
	int max_out_val = pLCorrData->max_out_val;
	int edit_channel_index = pLCorrData->edit_channel_index;
	CvMat* BGRA_LUT = pLCorrData->BGRA_LUT;
	RECT mask_rect = pLCorrData->mask_rect;
	EditNode* pEditNode = pLCorrData->pEditNode;
	EditLayerHandle* pEditLayerHandle = pLCorrData->pEditLayerHandle;

	int x,y;
	double v;
	double d;
	double tmp = FUNC_GAMMA(gamma);
	for(x=0; x<256; x++){
		v = pow((double)x/255, tmp);
		y = v*255.0;
		if(y < min_level) y = min_level;
		if(y > max_level) y = max_level;
		
		y = 255.0 * (y - min_level) / (max_level - min_level);
		d = ((double)y - min_level)/(max_level - min_level);
		y = d*(max_out_val - min_out_val) + min_out_val;
		//set look up table
		switch(edit_channel_index){
			case LC_RGB:
				cvSet1D(BGRA_LUT, x, cvScalar(y, y, y, x));
				break;
			case LC_R:
				cvSet1D(BGRA_LUT, x, cvScalar(x, x, y, x));
				break;
			case LC_G:
				cvSet1D(BGRA_LUT, x, cvScalar(x, y, x, x));
				break;
			case LC_B:
				cvSet1D(BGRA_LUT, x, cvScalar(y, x, x, x));
				break;
		}
	}

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

void LevelCorrection::UpdateImage()
{
	DWORD dwExCode;
	::GetExitCodeThread(m_hUpdateThread, &dwExCode);
	if(dwExCode == STILL_ACTIVE){
	}
	else{
		unsigned int thId;
		m_LCorrData.gamma = m_gamma;
		m_LCorrData.min_level = m_min_level;
		m_LCorrData.max_level = m_max_level;
		m_LCorrData.min_out_val = m_min_level;
		m_LCorrData.max_out_val = m_max_level;
		m_LCorrData.edit_channel_index = m_edit_channel_index;
		m_LCorrData.BGRA_LUT = m_BGRA_LUT;
		m_LCorrData.mask_rect = m_mask_rect;
		m_LCorrData.pEditNode = m_pEditNode;
		m_LCorrData.pEditLayerHandle = m_pEditLayerHandle;

		m_hUpdateThread = (HANDLE)_beginthreadex(
			NULL,
			0,
			UpdateImageThread,
			(void*)&m_LCorrData,
			0,
			&thId);
	}
}

void LevelCorrection::MLButtonDown(HWND hWnd, int x, int y)
{
	int level_sld_x = x - LC_LEVEL_SLD_X;
	int level_sld_y = y - LC_LEVEL_SLD_Y;
	if((0 <= level_sld_x) && (level_sld_x < LC_LEVEL_SLD_W) &&
		(0 <= level_sld_y) && (level_sld_y < LC_LEVEL_SLD_H))
	{
		int gamma_x = m_gamma*(m_max_level - m_min_level) + m_min_level + 0.5;
		int min_dis = abs(level_sld_x - m_min_level);
		int max_dis = abs(level_sld_x - m_max_level);
		int gam_dis = abs(level_sld_x - gamma_x);
		if(min_dis < max_dis){
			if(gam_dis < min_dis){
				gamma_x = level_sld_x;
				gamma_x = (gamma_x <= (m_min_level+1)) ? (m_min_level+1) : gamma_x;
				gamma_x = (gamma_x >= (m_max_level-1)) ? (m_max_level-1) : gamma_x;
				m_is_gamma_dlg = true;
	
				m_gamma = ((double)gamma_x - m_min_level)/(m_max_level - m_min_level);
			}
			else{
				m_min_level = level_sld_x;
				m_min_level = (m_min_level < 0) ? 0 : m_min_level;
				m_min_level = (m_min_level >= (m_max_level-1)) ? (m_max_level-2) : m_min_level;
				m_is_min_level_dlg = true;
			}
		}
		else{
			if(gam_dis < max_dis){
				int gamma_x = level_sld_x;
				gamma_x = (gamma_x <= (m_min_level+1)) ? (m_min_level+1) : gamma_x;
				gamma_x = (gamma_x >= (m_max_level-1)) ? (m_max_level-1) : gamma_x;
				m_is_gamma_dlg = true;

				m_gamma = ((double)gamma_x - m_min_level)/(m_max_level - m_min_level);
			}
			else{
				m_max_level = level_sld_x;
				m_max_level = (m_max_level <= (m_min_level+1)) ? (m_min_level+2) : m_max_level;
				m_max_level = (m_max_level > LC_LEVEL_SLD_W) ? LC_LEVEL_SLD_W : m_max_level;
				m_is_max_level_dlg = true;
			}
		}
		RebuildLevelDlg();
		::InvalidateRect(hWnd, NULL, FALSE);
		return;
	}

	int output_sld_x = x - LC_OUTPUT_SLD_X;
	int output_sld_y = y - LC_OUTPUT_SLD_Y;
	if((0 <= output_sld_x) && (output_sld_x < LC_OUTPUT_SLD_W) &&
		(0 <= output_sld_y) && (output_sld_y < LC_OUTPUT_SLD_H))
	{
		int min_dis = abs(output_sld_x - m_min_out_val);
		int max_dis = abs(output_sld_x - m_max_out_val);
		if(min_dis < max_dis){
			m_min_out_val = output_sld_x;
			m_min_out_val = (m_min_out_val < 0) ? 0 : m_min_out_val;
			m_min_out_val = (m_min_out_val >= (m_max_out_val-1)) ? (m_max_out_val-2) : m_min_out_val;
			m_is_min_out_dlg = true;
		}
		else{
			m_max_out_val = output_sld_x;
			m_max_out_val = (m_max_out_val <= (m_min_out_val+1)) ? (m_min_out_val+2) : m_max_out_val;
			m_max_out_val = (m_max_out_val > LC_OUTPUT_SLD_W) ? LC_OUTPUT_SLD_W : m_max_out_val;
			m_is_max_out_dlg = true;
		}
		RebuildOutputDlg();
		::InvalidateRect(hWnd, NULL, FALSE);
		return;
	}
}

void LevelCorrection::MMove(HWND hWnd, int x, int y)
{
	if(m_is_gamma_dlg){
		int gamma_x = x - LC_LEVEL_SLD_X;
		gamma_x = (gamma_x <= (m_min_level+1)) ? (m_min_level+1) : gamma_x;
		gamma_x = (gamma_x >= (m_max_level-1)) ? (m_max_level-1) : gamma_x;

		m_gamma = ((double)gamma_x - m_min_level)/(m_max_level - m_min_level);

		RebuildLevelDlg();
		::InvalidateRect(hWnd, NULL, FALSE);
	}
	if(m_is_max_level_dlg){
		m_max_level = x - LC_LEVEL_SLD_X;
		m_max_level = (m_max_level <= (m_min_level+1)) ? (m_min_level+2) : m_max_level;
		m_max_level = (m_max_level > LC_LEVEL_SLD_W) ? LC_LEVEL_SLD_W : m_max_level;

		RebuildLevelDlg();
		::InvalidateRect(hWnd, NULL, FALSE);
	}
	if(m_is_min_level_dlg){
		m_min_level = x - LC_LEVEL_SLD_X;
		m_min_level = (m_min_level < 0) ? 0 : m_min_level;
		m_min_level = (m_min_level >= (m_max_level-1)) ? (m_max_level-2) : m_min_level;
		
		RebuildLevelDlg();
		::InvalidateRect(hWnd, NULL, FALSE);
	}
	if(m_is_max_out_dlg){
		m_max_out_val = x - LC_OUTPUT_SLD_X;
		m_max_out_val = (m_max_out_val <= (m_min_out_val+1)) ? (m_min_out_val+2) : m_max_out_val;
		m_max_out_val = (m_max_out_val > LC_OUTPUT_SLD_W) ? LC_OUTPUT_SLD_W : m_max_out_val;
		
		RebuildOutputDlg();
		::InvalidateRect(hWnd, NULL, FALSE);
	}
	if(m_is_min_out_dlg){
		m_min_out_val = x - LC_OUTPUT_SLD_X;
		m_min_out_val = (m_min_out_val < 0) ? 0 : m_min_out_val;
		m_min_out_val = (m_min_out_val >= (m_max_out_val-1)) ? (m_max_out_val-2) : m_min_out_val;
		
		RebuildOutputDlg();
		::InvalidateRect(hWnd, NULL, FALSE);
	}

	if( m_is_gamma_dlg ||
		m_is_max_level_dlg || m_is_min_level_dlg ||
		m_is_max_out_dlg || m_is_min_out_dlg )
	{
		UpdateImage();
	}
}

void LevelCorrection::MLButtonUp(HWND hWnd, int x, int y)
{
	if(m_is_gamma_dlg){
		int gamma_x = x - LC_LEVEL_SLD_X;
		gamma_x = (gamma_x <= (m_min_level+1)) ? (m_min_level+1) : gamma_x;
		gamma_x = (gamma_x >= (m_max_level-1)) ? (m_max_level-1) : gamma_x;

		m_gamma = ((double)gamma_x - m_min_level)/(m_max_level - m_min_level);

		RebuildLevelDlg();
		::InvalidateRect(hWnd, NULL, FALSE);

		m_is_gamma_dlg = false;
	}
	if(m_is_max_level_dlg){
		m_max_level = x - LC_LEVEL_SLD_X;
		m_max_level = (m_max_level <= (m_min_level+1)) ? (m_min_level+2) : m_max_level;
		m_max_level = (m_max_level > LC_LEVEL_SLD_W) ? LC_LEVEL_SLD_W : m_max_level;
		
		RebuildLevelDlg();
		::InvalidateRect(hWnd, NULL, FALSE);
		m_is_max_level_dlg = false;
	}
	if(m_is_min_level_dlg){
		m_min_level = x - LC_LEVEL_SLD_X;
		m_min_level = (m_min_level < 0) ? 0 : m_min_level;
		m_min_level = (m_min_level >= (m_max_level-1)) ? (m_max_level-2) : m_min_level;
		
		RebuildLevelDlg();
		::InvalidateRect(hWnd, NULL, FALSE);
		m_is_min_level_dlg = false;
	}
	if(m_is_max_out_dlg){
		m_max_out_val = x - LC_OUTPUT_SLD_X;
		m_max_out_val = (m_max_out_val <= (m_min_out_val+1)) ? (m_min_out_val+2) : m_max_out_val;
		m_max_out_val = (m_max_out_val > LC_OUTPUT_SLD_W) ? LC_OUTPUT_SLD_W : m_max_out_val;
		
		RebuildOutputDlg();
		::InvalidateRect(hWnd, NULL, FALSE);
		m_is_max_out_dlg = false;
	}
	if(m_is_min_out_dlg){
		m_min_out_val = x - LC_OUTPUT_SLD_X;
		m_min_out_val = (m_min_out_val < 0) ? 0 : m_min_out_val;
		m_min_out_val = (m_min_out_val >= (m_max_out_val-1)) ? (m_max_out_val-2) : m_min_out_val;
		
		RebuildOutputDlg();
		::InvalidateRect(hWnd, NULL, FALSE);
		m_is_min_out_dlg = false;
	}
}

void LevelCorrection::RebuildLevelDlg()
{
	//clear
	cvSet(m_level_sld_img, cvScalar(255,255,255));

	int npts = 3; 
	CvPoint** pts = (CvPoint**) cvAlloc(sizeof(CvPoint*) * 1);
	pts[0] = (CvPoint*) cvAlloc(sizeof(CvPoint) * 3);

	//min slider
	pts[0][0].x = m_min_level - LC_SLD_RAD;
	pts[0][0].y = LC_LEVEL_SLD_H-1;
	pts[0][1].x = m_min_level;
	pts[0][1].y = 0;
	pts[0][2].x = m_min_level + LC_SLD_RAD;
	pts[0][2].y = LC_LEVEL_SLD_H-1;
	cvFillPoly(m_level_sld_img, pts, &npts, 1, LC_LEVEL_MIN_SLD_COLOR, 8, 0);
	cvPolyLine(m_level_sld_img, pts, &npts, 1, 1, CV_RGB(0,0,0), 1, 8, 0);

	//max slider
	pts[0][0].x = m_max_level - LC_SLD_RAD;
	pts[0][1].x = m_max_level;
	pts[0][2].x = m_max_level + LC_SLD_RAD;
	cvFillPoly(m_level_sld_img, pts, &npts, 1, LC_LEVEL_MAX_SLD_COLOR, 8, 0);
	cvPolyLine(m_level_sld_img, pts, &npts, 1, 1, CV_RGB(0,0,0), 1, 8, 0);

	//
	int gamma_x = m_gamma*(m_max_level - m_min_level) + m_min_level + 0.5;
	pts[0][0].x = gamma_x - LC_SLD_RAD;
	pts[0][1].x = gamma_x;
	pts[0][2].x = gamma_x + LC_SLD_RAD;
	cvFillPoly(m_level_sld_img, pts, &npts, 1, LC_LEVEL_GAM_SLD_COLOR, 8, 0);
	cvPolyLine(m_level_sld_img, pts, &npts, 1, 1, CV_RGB(0,0,0), 1, 8, 0);

	cvFree(&(pts[0]));
	cvFree(&pts);
}

void LevelCorrection::RebuildOutputDlg()
{
	//clear
	cvSet(m_output_sld_img, cvScalar(255,255,255));

	int npts = 3; 
	CvPoint** pts = (CvPoint**) cvAlloc(sizeof(CvPoint*) * 1);
	pts[0] = (CvPoint*) cvAlloc(sizeof(CvPoint) * 3);

	//min slider
	pts[0][0].x = m_min_out_val - LC_SLD_RAD;
	pts[0][0].y = LC_OUTPUT_SLD_H-1;
	pts[0][1].x = m_min_out_val;
	pts[0][1].y = 0;
	pts[0][2].x = m_min_out_val + LC_SLD_RAD;
	pts[0][2].y = LC_OUTPUT_SLD_H-1;
	cvFillPoly(m_output_sld_img, pts, &npts, 1, LC_OUTPUT_MIN_SLD_COLOR, CV_AA, 0);
	cvPolyLine(m_output_sld_img, pts, &npts, 1, 1, CV_RGB(0,0,0), 1, CV_AA, 0);

	//max slider
	pts[0][0].x = m_max_out_val - LC_SLD_RAD;
	pts[0][1].x = m_max_out_val;
	pts[0][2].x = m_max_out_val + LC_SLD_RAD;
	cvFillPoly(m_output_sld_img, pts, &npts, 1, LC_OUTPUT_MAX_SLD_COLOR, CV_AA, 0);
	cvPolyLine(m_output_sld_img, pts, &npts, 1, 1, CV_RGB(0,0,0), 1, CV_AA, 0);

	cvFree(&(pts[0]));
	cvFree(&pts);
}

LRESULT CALLBACK LevelCorrectionDlgProc(HWND hDlgWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static LevelCorrection* pLevelCrrc;
	static HWND hCombo;
	
	switch(msg){
		case WM_INITDIALOG:
			{
				pLevelCrrc = (LevelCorrection*)lp;
				hCombo = ::GetDlgItem(hDlgWnd, IDC_EDIT_CHANNEL_COMBO);
				//
				::SendMessage(hCombo, CB_INSERTSTRING, LC_RGB, (LPARAM)"RGB");
				::SendMessage(hCombo, CB_INSERTSTRING, LC_R, (LPARAM)"R");
				::SendMessage(hCombo, CB_INSERTSTRING, LC_G, (LPARAM)"G");
				::SendMessage(hCombo, CB_INSERTSTRING, LC_B, (LPARAM)"B");
				//
				::SendMessage(hCombo, CB_SETCURSEL, LC_RGB, 0);
				pLevelCrrc->ChangeEditChannel(LC_RGB);

				::InvalidateRect(hDlgWnd, NULL, FALSE);
			}
			return TRUE;
		case WM_PAINT:
			{
				IplImage* hist_img = pLevelCrrc->GetHistogramImg();
				IplImage* level_sld_img = pLevelCrrc->GetLevelSldImg();
				IplImage* output_sld_img = pLevelCrrc->GetOutputSldImg();

				PAINTSTRUCT ps;
				HDC hdc = ::BeginPaint(hDlgWnd, &ps);

				IPLImageToDevice(
					hdc,
					LC_HISTOGRAM_X,
					LC_HISTOGRAM_Y,
					LC_HISTOGRAM_W,
					LC_HISTOGRAM_H,
					hist_img,
					0,0);

				IPLImageToDevice(
					hdc,
					LC_LEVEL_SLD_X,
					LC_LEVEL_SLD_Y,
					LC_LEVEL_SLD_W,
					LC_LEVEL_SLD_H,
					level_sld_img,
					0,0);

				IPLImageToDevice(
					hdc,
					LC_OUTPUT_SLD_X,
					LC_OUTPUT_SLD_Y,
					LC_OUTPUT_SLD_W,
					LC_OUTPUT_SLD_H,
					output_sld_img,
					0,0);

				::EndPaint(hDlgWnd, &ps);

			}
			return TRUE;
		case WM_LBUTTONDOWN:
			{
				::SetCapture(hDlgWnd);

				int x = LOWORD(lp);
				int y = HIWORD(lp);
				pLevelCrrc->MLButtonDown(hDlgWnd, x, y);
			}
			return TRUE;
		case WM_MOUSEMOVE:
			{
				int x = LOWORD(lp);
				int y = HIWORD(lp);
				pLevelCrrc->MMove(hDlgWnd, x, y);
			}
			return TRUE;
		case WM_LBUTTONUP:
			{
				::ReleaseCapture();

				int x = LOWORD(lp);
				int y = HIWORD(lp);
				pLevelCrrc->MLButtonUp(hDlgWnd, x, y);
				pLevelCrrc->UpdateImage();
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
				case IDC_EDIT_CHANNEL_COMBO:
					if(HIWORD(wp) == CBN_SELCHANGE){
						int select_index = ::SendMessage(hCombo, CB_GETCURSEL, 0, 0);
						pLevelCrrc->ChangeEditChannel(select_index);

						::InvalidateRect(hDlgWnd, NULL, FALSE);
					}
					break;
				default:
					return FALSE;
			}
		default:
			return FALSE;
	}
	return TRUE;
}