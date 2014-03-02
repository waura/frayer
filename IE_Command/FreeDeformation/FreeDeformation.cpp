// FreeDeformation.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//
#include "stdafx.h"
#include "FreeDeformation.h"

#define FD_POINT_RECT_SIZE 4

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

#define DISPLAY_NAME_JA "自由変形"
#define DISPLAY_NAME DISPLAY_NAME_JA

IECOMMAND_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::COMMAND;
}

IECOMMAND_EXPORT void GetIECommandName(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, "FreeDeformation");
}

IECOMMAND_EXPORT void GetIECommandDisplayNameJa(char* dst)
{
	strcpy_s(dst, MAX_IE_MODULE_NAME, DISPLAY_NAME);
}

IECOMMAND_EXPORT IIECommand* CreateIECommand()
{
	return new FreeDeformation;
}

IECOMMAND_EXPORT void ReleaseIECommand(IIECommand** ppCommand)
{
	if (*ppCommand) {
		delete (*ppCommand);
		(*ppCommand) = NULL;
	}
}

//自由変形スレッド
unsigned _stdcall FreeDeformationThread(void* data)
{
	LPDEFORM_DATA deform_data = (LPDEFORM_DATA)data;

	int dst_width = deform_data->to_rect.right - deform_data->to_rect.left;
	int dst_height = deform_data->to_rect.bottom - deform_data->to_rect.top;

	if (dst_width <= 0 || dst_height <= 0) return 0;

	IplImage* dst_img = cvCreateImage(cvSize(dst_width, dst_height), IPL_DEPTH_8U, 4);
	IplImage* dst_mask = cvCreateImage(cvSize(dst_width, dst_height), IPL_DEPTH_8U, 1);

	FreeDeformationByRatio(dst_mask, deform_data->src_mask, deform_data->cpoints);
	FreeDeformationByRatio(dst_img, deform_data->src_img, deform_data->cpoints);

	//reset before defomation
	deform_data->pEditMaskNode->edit_img.RectFillZero(
		deform_data->pre_to_rect.left,
		deform_data->pre_to_rect.top,
		deform_data->pre_to_rect.right - deform_data->pre_to_rect.left,
		deform_data->pre_to_rect.bottom - deform_data->pre_to_rect.top);

	//reset before defomation
	deform_data->pEditNode->edit_img.RectFillZero(
		deform_data->pre_to_rect.left,
		deform_data->pre_to_rect.top,
		deform_data->pre_to_rect.right - deform_data->pre_to_rect.left,
		deform_data->pre_to_rect.bottom - deform_data->pre_to_rect.top);

	//blt to mask edit node
	deform_data->pEditMaskNode->edit_img.ImgBlt(
		deform_data->to_rect.left,
		deform_data->to_rect.top,
		dst_mask->width,
		dst_mask->height,
		dst_mask,
		0,
		0,
		IPLEXT_RASTER_CODE::COPY);

	//blt to layer edit node
	deform_data->pEditNode->edit_img.ImgBlt(
		deform_data->to_rect.left,
		deform_data->to_rect.top,
		dst_img->width,
		dst_img->height,
		dst_img,
		0,
		0,
		IPLEXT_RASTER_CODE::COPY);

	RECT rc;
	OrRect(&(deform_data->to_rect), &(deform_data->pre_to_rect), &rc);

	//update mask image
	deform_data->pCutMaskHandle->Update(&rc);
	deform_data->pEditMaskHandle->Update(&rc);
	//update layer image
	deform_data->pCutLayerHandle->Update(&rc);
	deform_data->pEditLayerHandle->Update(&rc);

	deform_data->pre_to_rect = deform_data->to_rect;

	cvReleaseImage(&dst_img);
	cvReleaseImage(&dst_mask);
	return 0;
}

FreeDeformation::FreeDeformation()
{
	m_hPen = ::CreatePen(PS_SOLID, 2, RGB(255,0,0));

  	strcpy_s(m_name, MAX_IE_MODULE_NAME, "FreeDeformation");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

FreeDeformation::~FreeDeformation()
{
	if (m_hPen) {
		::DeleteObject(m_hPen);
		m_hPen = NULL;
	}
}

bool FreeDeformation::Init(ImgEdit* pEdit)
{
	m_srcMask = NULL;
	m_srcImg = NULL;

	m_pEditNode = NULL;
	m_pEditLayerHandle = NULL;
	m_pEditMaskNode = NULL;
	m_pEditMaskHandle = NULL;
	m_pCutLayerHandle = NULL;
	m_pCutMaskHandle = NULL;
	m_pStackHandle = NULL;

	m_pEditLayer = NULL;
	m_pEditFile  = NULL;
	m_SelectPointIndex.col = m_SelectPointIndex.row = -1;
	m_isFinish = false;

	ImgFile_Ptr f = pEdit->GetActiveFile();
	if(!f){
		//if select file don't exist, this command will be finish
		FinishCommand();
		return false;
	}
	m_pEditFile = f;
	
	IImgLayer_weakPtr _sl = f->GetSelectLayer();
	if (_sl.expired()) {
		OutputError::Alert(IE_ERROR_SELECT_LAYER_DONT_EXIST);
		FinishCommand();
		return false;
	}

	IImgLayer_Ptr sl = _sl.lock();
	if (sl->GetLayerType() != IE_LAYER_TYPE::NORMAL_LAYER) {
		OutputError::Alert(IE_ERROR_CANT_EDIT_LAYER);
		return false;
	}
	m_pEditLayer = std::dynamic_pointer_cast<ImgLayer>(sl);

	if (f->GetMaskState() == IE_MASK_STATE_ID::INVALID) {
		//if don't exist select mask, this command will be finish
		FinishCommand();
		return false;
	}

	int x,y;
	RECT mask_rect;
	f->GetMaskRect(&mask_rect);

	CvSize file_img_size = m_pEditFile->GetImgSize();
	RECT file_img_rc;
	file_img_rc.top = 0; file_img_rc.left = 0;
	file_img_rc.right = file_img_size.width;
	file_img_rc.bottom = file_img_size.height;

	//copy image in mask to m_srcImg
	m_srcMask = cvCreateImage(cvSize(mask_rect.right-mask_rect.left, mask_rect.bottom-mask_rect.top), IPL_DEPTH_8U, 1);
	f->CopyFileMask(
		mask_rect.left,
		mask_rect.top,
		mask_rect.right - mask_rect.left,
		mask_rect.bottom - mask_rect.top,
		m_srcMask,
		0, 0);

	f->AllMaskInvisible();
	m_srcImg = cvCreateImage(
		cvSize(mask_rect.right-mask_rect.left, mask_rect.bottom-mask_rect.top),
		IPL_DEPTH_8U,
		4);
	m_pEditLayer->CopyLayerImage(
		m_srcImg,
		0, 0,
		mask_rect.left,
		mask_rect.top,
		mask_rect.right - mask_rect.left,
		mask_rect.bottom - mask_rect.top);
		//m_srcMask);

	//create stack handle
	m_pStackHandle = (StackHandle*) m_pEditFile->CreateImgFileHandle(IFH_STACK);
	m_pStackHandle->SetHandleName( DISPLAY_NAME );

	//cut mask
	m_pCutMaskHandle = (EditMaskHandle*) m_pEditFile->CreateImgFileHandle(IFH_EDIT_MASK);
	if (!m_pCutMaskHandle) {
		FinishCommand();
		return false;
	}
	m_pCutMaskHandle->SetHandleName("マスク切り抜き");
	m_pStackHandle->Push( m_pCutMaskHandle );

	EditNode* pCutMaskNode = m_pCutMaskHandle->CreateEditNode(&mask_rect);
	pCutMaskNode->fourcc = IE_MASK_SYNTH_FOURCC('s','u','b',' ');
	pCutMaskNode->edit_img.ImgBlt(
		mask_rect.left,
		mask_rect.top,
		mask_rect.right - mask_rect.left,
		mask_rect.bottom - mask_rect.top,
		m_srcMask,
		0, 0,
		IPLEXT_RASTER_CODE::COPY);
	m_pCutMaskHandle->Update(&mask_rect);

	//cut layer
	m_pCutLayerHandle = (EditLayerHandle*) m_pEditFile->CreateImgFileHandle(IFH_EDIT_LAYER);
	if (!m_pCutLayerHandle) {
		FinishCommand();
		return false;
	}
	m_pCutLayerHandle->SetHandleName("レイヤー切り抜き");
	m_pStackHandle->Push( m_pCutLayerHandle );

	EditNode* pCutLayerNode = m_pCutLayerHandle->CreateEditNode();
	pCutLayerNode->fourcc = IE_LAYER_SYNTH_FOURCC('s','u','b','A');
	pCutLayerNode->edit_img.ImgBlt(
		mask_rect.left,
		mask_rect.top,
		mask_rect.right - mask_rect.left,
		mask_rect.bottom - mask_rect.top,
		m_srcImg,
		0, 0,
		IPLEXT_RASTER_CODE::COPY,
		m_srcMask,
		0, 0);
	m_pCutLayerHandle->Update(&mask_rect);

	//create EditMaskHandle
	m_pEditMaskHandle = (EditMaskHandle*) m_pEditFile->CreateImgFileHandle(IFH_EDIT_MASK);
	if (!m_pEditMaskHandle) {
		FinishCommand();
		return false;
	}
	m_pEditMaskHandle->SetHandleName("マスク自由変形");
	m_pStackHandle->Push( m_pEditMaskHandle );

	//create EditMaskNode which has same size file image
	m_pEditMaskNode = m_pEditMaskHandle->CreateEditNode();
	m_pEditMaskNode->fourcc = IE_MASK_SYNTH_FOURCC('c','o','p','y');
	m_pEditMaskNode->edit_img.ImgBlt(
		mask_rect.left,
		mask_rect.top,
		mask_rect.right - mask_rect.left,
		mask_rect.bottom - mask_rect.top,
		m_srcMask,
		0, 0,
		IPLEXT_RASTER_CODE::COPY);
	m_pEditMaskHandle->Update(&mask_rect);

	//create EditLayerHandle
	m_pEditLayerHandle = (EditLayerHandle*) m_pEditFile->CreateImgFileHandle(IFH_EDIT_LAYER);
	if (!m_pEditLayerHandle) {
		FinishCommand();
		return false;
	}
	m_pEditLayerHandle->SetHandleName("レイヤー自由変形");
	m_pStackHandle->Push( m_pEditLayerHandle );

	//create EditNode which has same size file image
	m_pEditNode = m_pEditLayerHandle->CreateEditNode();
	m_pEditNode->edit_img.ImgBlt(
		mask_rect.left,
		mask_rect.top,
		mask_rect.right - mask_rect.left,
		mask_rect.bottom - mask_rect.top,
		m_srcImg,
		0, 0,
		IPLEXT_RASTER_CODE::COPY,
		m_srcMask,
		0, 0);
	m_pEditLayerHandle->Update(&mask_rect);

	//
	m_pEditFile->DoImgFileHandle( m_pStackHandle );

	//
	m_DeformData.pre_to_rect.top = m_DeformData.pre_to_rect.bottom = 0;
	m_DeformData.pre_to_rect.left = m_DeformData.pre_to_rect.right = 0;
	m_DeformData.pEditFile = f;
	m_DeformData.src_img = m_srcImg;
	m_DeformData.src_mask = m_srcMask;
	m_DeformData.pEditNode = m_pEditNode;
	m_DeformData.pEditLayerHandle = m_pEditLayerHandle;
	m_DeformData.pEditMaskNode = m_pEditMaskNode;
	m_DeformData.pEditMaskHandle = m_pEditMaskHandle;
	m_DeformData.pCutLayerHandle = m_pCutLayerHandle;
	m_DeformData.pCutMaskHandle = m_pCutMaskHandle;
	
	//get dragpoint position from mask_rect
	m_DragPoint[0][0].x = mask_rect.left;
	m_DragPoint[0][0].y = mask_rect.top;
	m_DragPoint[1][0].x = mask_rect.left;
	m_DragPoint[1][0].y = mask_rect.top + (mask_rect.bottom - mask_rect.top)/2.0;
	m_DragPoint[2][0].x = mask_rect.left;
	m_DragPoint[2][0].y = mask_rect.bottom;

	m_DragPoint[0][1].x = mask_rect.left + (mask_rect.right - mask_rect.left)/2.0;
	m_DragPoint[0][1].y = mask_rect.top;
	m_DragPoint[1][1].x = mask_rect.left + (mask_rect.right - mask_rect.left)/2.0;
	m_DragPoint[1][1].y = mask_rect.top + (mask_rect.bottom - mask_rect.top)/2.0;
	m_DragPoint[2][1].x = mask_rect.left + (mask_rect.right - mask_rect.left)/2.0;
	m_DragPoint[2][1].y = mask_rect.bottom;

	m_DragPoint[0][2].x = mask_rect.right;
	m_DragPoint[0][2].y = mask_rect.top;
	m_DragPoint[1][2].x = mask_rect.right;
	m_DragPoint[1][2].y = mask_rect.top + (mask_rect.bottom - mask_rect.top)/2.0;
	m_DragPoint[2][2].x = mask_rect.right;
	m_DragPoint[2][2].y = mask_rect.bottom;

	//
	Deformation();

	return true;
}

//終了処理
void FreeDeformation::End(ImgEdit* pEdit)
{
	DWORD dwExCode;
	::GetExitCodeThread(m_hDeformThread, &dwExCode);
	if (dwExCode == STILL_ACTIVE){
		::CloseHandle(m_hDeformThread);
	}

	m_pCutMaskHandle->EndEdit();
	m_pCutLayerHandle->EndEdit();
	m_pEditMaskHandle->EndEdit();
	m_pEditLayerHandle->EndEdit();

	if (m_srcMask) {
		cvReleaseImage(&m_srcMask);
	}
	if (m_srcImg) {
		cvReleaseImage(&m_srcImg);
	}
}

BOOL FreeDeformation::Run(ImgEdit* pEdit, void* pvoid)
{
	return (!m_isFinish);
}

void FreeDeformation::OnDraw(HDC hdc)
{
	int i,j;
	HPEN hOldPen;

	//set pen
	hOldPen = (HPEN)::SelectObject(hdc, m_hPen);
	//
	::SelectObject(hdc, GetStockObject(NULL_BRUSH));

	CvPoint2D64f draw_pt;
	//draw rect frame
	m_pEditFile->ConvertPosImgToWnd(&(m_DragPoint[0][0]), &(draw_pt));
	::MoveToEx(hdc, draw_pt.x, draw_pt.y, NULL);
	m_pEditFile->ConvertPosImgToWnd(&(m_DragPoint[1][0]), &(draw_pt));
	::LineTo(hdc, draw_pt.x, draw_pt.y);
	m_pEditFile->ConvertPosImgToWnd(&(m_DragPoint[2][0]), &(draw_pt));
	::LineTo(hdc, draw_pt.x, draw_pt.y);
	m_pEditFile->ConvertPosImgToWnd(&(m_DragPoint[2][1]), &(draw_pt));
	::LineTo(hdc, draw_pt.x, draw_pt.y);
	m_pEditFile->ConvertPosImgToWnd(&(m_DragPoint[2][2]), &(draw_pt));
	::LineTo(hdc, draw_pt.x, draw_pt.y);
	m_pEditFile->ConvertPosImgToWnd(&(m_DragPoint[1][2]), &(draw_pt));
	::LineTo(hdc, draw_pt.x, draw_pt.y);
	m_pEditFile->ConvertPosImgToWnd(&(m_DragPoint[0][2]), &(draw_pt));
	::LineTo(hdc, draw_pt.x, draw_pt.y);
	m_pEditFile->ConvertPosImgToWnd(&(m_DragPoint[0][1]), &(draw_pt));
	::LineTo(hdc, draw_pt.x, draw_pt.y);
	m_pEditFile->ConvertPosImgToWnd(&(m_DragPoint[0][0]), &(draw_pt));
	::LineTo(hdc, draw_pt.x, draw_pt.y);

	//
	for(i=0; i<3; i++){
		for(j=0; j<3; j++){
			m_pEditFile->ConvertPosImgToWnd(&(m_DragPoint[i][j]), &(draw_pt));
			DrawPointRect(hdc, &(draw_pt));
		}
	}

	//
	::SelectObject(hdc, hOldPen);
}

void FreeDeformation::OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	int i,j;
	CvPoint2D64f mouse;
	m_before_mouse_pt.x = mouse.x = lpd->x;
	m_before_mouse_pt.y = mouse.y = lpd->y;
	m_start_drag_mouse_pt.x = lpd->x;
	m_start_drag_mouse_pt.y = lpd->y;
	m_isClickOnInsideArea = false;
	m_isClickOnExteriorArea = false;
	m_SelectPointIndex.col = m_SelectPointIndex.row = -1;
	m_DragFlags = nFlags;
	
	//check hit on corner point
	POINT_INDEX cpoint_index_list[4] = {
		{0,0},
		{0,2},
		{2,0},
		{2,2}};

	for (i=0; i<4; i++) {
		int c = cpoint_index_list[i].col;
		int r = cpoint_index_list[i].row;
		if (isOnPointRect(&(m_DragPoint[r][c]), &mouse)) {
			m_SelectPointIndex.row = r;
			m_SelectPointIndex.col = c;
			return;
		}
	}

	//check hit on middle point
	POINT_INDEX mpoint_index_list[4] = {
		{1, 0},
		{0, 1},
		{2, 1},
		{1, 2}};

	for (i=0; i<4; i++) {
		int c = mpoint_index_list[i].col;
		int r = mpoint_index_list[i].row;
		if (isOnPointRect(&(m_DragPoint[r][c]), &mouse)) {
			m_SelectPointIndex.row = r;
			m_SelectPointIndex.col = c;
			return;
		}
	}

	//check hit on center point
	if (isOnPointRect(&(m_DragPoint[1][1]), &mouse)) {
		m_SelectPointIndex.row = 1;
		m_SelectPointIndex.col = 1;
		return ;
	}

	//check hit on freedeformation area
	POINT_INDEX point_index_list[8][2] = {
		{{0,0}, {0,1}},
		{{0,1}, {0,2}},
		{{0,2}, {1,2}},
		{{1,2}, {2,2}},
		{{2,2}, {2,1}},
		{{2,1}, {2,0}},
		{{2,0}, {1,0}},
		{{1,0}, {0,0}},
	};

	int cnt_intersect_x = 0;
	int cnt_intersect_y = 0;
	for (i=0; i<8; i++) {
		int c0 = point_index_list[i][0].col;
		int r0 = point_index_list[i][0].row;
		int c1 = point_index_list[i][1].col;
		int r1 = point_index_list[i][1].row;

		CvLine2D64f line;
		line.start = m_DragPoint[r0][c0];
		line.end = m_DragPoint[r1][c1];

		if (IsIntersectX(&line, mouse.x)) {
			cnt_intersect_x++;
		}
		if (IsIntersectY(&line, mouse.y)) {
			cnt_intersect_y++;
		}
	}

	if ((cnt_intersect_x > 0 && (cnt_intersect_x & 0x1) == 0) &&
		(cnt_intersect_y > 0 && (cnt_intersect_y & 0x1) == 0))
	{
		m_isClickOnInsideArea = true;
	}

	//hit on external freedeformation area
	m_isClickOnExteriorArea = true;
}

void FreeDeformation::OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	int i,j;
	int spi_c = m_SelectPointIndex.col;
	int spi_r = m_SelectPointIndex.row;

	if (m_SelectPointIndex.col == 1 && m_SelectPointIndex.row == 1) {
		m_DragPoint[1][1].x = lpd->x;
		m_DragPoint[1][1].y = lpd->y;
	} else if (m_SelectPointIndex.col != -1 && m_SelectPointIndex.row != -1) {
		if (IE_MK_CONTROL == (m_DragFlags & IE_MK_CONTROL)) {
			ControlDrag(lpd);
		} else {
			NormalDrag(lpd);
		}
	} else if (m_isClickOnInsideArea) {
		//
		int shiftX = lpd->x - m_before_mouse_pt.x;
		int shiftY = lpd->y - m_before_mouse_pt.y;

		for (i=0; i<3; i++) {
			for (j=0; j<3; j++) {
				m_DragPoint[i][j].x = m_DragPoint[i][j].x + shiftX;
				m_DragPoint[i][j].y = m_DragPoint[i][j].y + shiftY;
			}
		}
	} else if (m_isClickOnExteriorArea) {
		RotationDrag(lpd);
	}

	m_before_mouse_pt.x = lpd->x;
	m_before_mouse_pt.y = lpd->y;

	//変形
	Deformation();
}

void FreeDeformation::OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	m_SelectPointIndex.col = m_SelectPointIndex.row = -1;
}

void FreeDeformation::OnKeyboardDown(UINT nChar, UINT optChar)
{
	if (nChar == IE_KEY_RETURN) { //エンターで終了
		FinishCommand();
	}
}

void FreeDeformation::FinishCommand()
{
	m_isFinish = true;
}

//
void FreeDeformation::Deformation()
{
	DWORD dwExCode;
	::GetExitCodeThread(m_hDeformThread, &dwExCode);
	if (dwExCode == STILL_ACTIVE){
	} else {
		int i,j;
		unsigned int thId;
		POINT_INDEX cpoint_index_list[] = {
			{0,0},
			{0,2},
			{2,2},
			{2,0}};

		//変形先の範囲
		int max_x = 0;
		int max_y = 0;
		int min_x = 0x7fff;
		int min_y = 0x7fff;

		//コーナーの点取り出し
		for (i=0; i<4; i++) {
			int c = cpoint_index_list[i].col;
			int r = cpoint_index_list[i].row;
			if (max_x < m_DragPoint[r][c].x) {
				max_x = m_DragPoint[r][c].x;
			}
			if (max_y < m_DragPoint[r][c].y) {
				max_y = m_DragPoint[r][c].y;
			}
			if (min_x > m_DragPoint[r][c].x) {
				min_x = m_DragPoint[r][c].x;
			}
			if (min_y > m_DragPoint[r][c].y) {
				min_y = m_DragPoint[r][c].y;
			}
		}

		for (i=0; i<4; i++) {
			int c = cpoint_index_list[i].col;
			int r = cpoint_index_list[i].row;
			m_DeformData.cpoints[i].x = m_DragPoint[r][c].x - min_x;
			m_DeformData.cpoints[i].y = m_DragPoint[r][c].y - min_y;
		}

		m_DeformData.to_rect.top = min_y;
		m_DeformData.to_rect.left = min_x;
		m_DeformData.to_rect.bottom = max_y;
		m_DeformData.to_rect.right = max_x;

		m_hDeformThread = (HANDLE)_beginthreadex(
			NULL,
			0,
			FreeDeformationThread,
			(void*)&m_DeformData,
			0,
			&thId);
	}
}

void FreeDeformation::DrawPointRect(HDC hdc, const CvPoint2D64f* pcvPoint)
{
	::MoveToEx(hdc, pcvPoint->x - FD_POINT_RECT_SIZE,
		pcvPoint->y - FD_POINT_RECT_SIZE, NULL);
	::LineTo(hdc, pcvPoint->x + FD_POINT_RECT_SIZE,
		pcvPoint->y - FD_POINT_RECT_SIZE);
	::LineTo(hdc, pcvPoint->x + FD_POINT_RECT_SIZE,
		pcvPoint->y + FD_POINT_RECT_SIZE);
	::LineTo(hdc, pcvPoint->x - FD_POINT_RECT_SIZE,
		pcvPoint->y + FD_POINT_RECT_SIZE);
	::LineTo(hdc, pcvPoint->x - FD_POINT_RECT_SIZE,
		pcvPoint->y - FD_POINT_RECT_SIZE);
}

bool FreeDeformation::isOnPointRect(const CvPoint2D64f* pcvPoint, const CvPoint2D64f* pMouse)
{
	if((pcvPoint->x - FD_POINT_RECT_SIZE <= pMouse->x && pMouse->x <= pcvPoint->x + FD_POINT_RECT_SIZE) &&
		(pcvPoint->y - FD_POINT_RECT_SIZE <= pMouse->y && pMouse->y <= pcvPoint->y + FD_POINT_RECT_SIZE))
	{
		return true;
	}

	return false;
}


void FreeDeformation::NormalDrag(const LPIE_INPUT_DATA lpd)
{
	int i;
	int spi_c = m_SelectPointIndex.col;
	int spi_r = m_SelectPointIndex.row;

	if(m_SelectPointIndex.col == 1){
		//middle point, move x pos
		for(i=0; i<3; i++){
			m_DragPoint[spi_r][i].y += lpd->y - m_before_mouse_pt.y;
		}

		//recalc middle point
		for(i=0; i<3; i++){
			m_DragPoint[1][i].y = abs(m_DragPoint[0][i].y - m_DragPoint[2][i].y)/2.0 
				+ min(m_DragPoint[0][i].y, m_DragPoint[2][i].y);
		}
	}
	else if(m_SelectPointIndex.row == 1){
		//middle point, move y pos
		for(i=0; i<3; i++){
			m_DragPoint[i][spi_c].x += lpd->x - m_before_mouse_pt.x;
		}

		//recalc middle point
		for(i=0; i<3; i++){
			m_DragPoint[i][1].x = abs(m_DragPoint[i][0].x - m_DragPoint[i][2].x)/2.0 
				+ min(m_DragPoint[i][0].x, m_DragPoint[i][2].x);
		}
	}
	else{
		//corner
		//set corner point
		for(i=0; i<3; i++){
			m_DragPoint[spi_r][i].y += lpd->y - m_before_mouse_pt.y;
			m_DragPoint[i][spi_c].x += lpd->x - m_before_mouse_pt.x;
		}

		//recalc middle point
		POINT_INDEX xpoint_index_list[] = {
			{0,1},
			{1,1},
			{2,1}};
		POINT_INDEX ypoint_index_list[] = {
			{1, 0},
			{1, 1},
			{1, 2}};

		for(i=0; i<3; i++){
			int c = xpoint_index_list[i].col;
			int r = xpoint_index_list[i].row;
			m_DragPoint[r][c].x = abs(m_DragPoint[r][0].x - m_DragPoint[r][2].x)/2.0
				+ min(m_DragPoint[r][0].x, m_DragPoint[r][2].x);
		}

		for(i=0; i<3; i++){
			int c = ypoint_index_list[i].col;
			int r = ypoint_index_list[i].row;
			m_DragPoint[r][c].y = abs(m_DragPoint[0][c].y - m_DragPoint[2][c].y)/2.0
				+ min(m_DragPoint[0][c].y, m_DragPoint[2][c].y);
		}
	}
}


void FreeDeformation::ControlDrag(const LPIE_INPUT_DATA lpd)
{
	assert(m_SelectPointIndex.row != 1);
	assert(m_SelectPointIndex.col != 1);

	int spi_c = m_SelectPointIndex.col;
	int spi_r = m_SelectPointIndex.row;

	//corner
	//set corner point
	m_DragPoint[spi_r][spi_c].x = lpd->x;
	m_DragPoint[spi_r][spi_c].y = lpd->y;

	//recalc middle point
	m_DragPoint[1][spi_c].x = abs(m_DragPoint[0][spi_c].x - m_DragPoint[2][spi_c].x)/2.0
								+ min(m_DragPoint[0][spi_c].x, m_DragPoint[2][spi_c].x);
	m_DragPoint[1][spi_c].y = abs(m_DragPoint[0][spi_c].y - m_DragPoint[2][spi_c].y)/2.0
								+ min(m_DragPoint[0][spi_c].y, m_DragPoint[2][spi_c].y);

	m_DragPoint[spi_r][1].x = abs(m_DragPoint[spi_r][0].x - m_DragPoint[spi_r][2].x)/2.0
			+ min(m_DragPoint[spi_r][0].x, m_DragPoint[spi_r][2].x);
	m_DragPoint[spi_r][1].y = abs(m_DragPoint[spi_r][0].y - m_DragPoint[spi_r][2].y)/2.0
			+ min(m_DragPoint[spi_r][0].y, m_DragPoint[spi_r][2].y);
}

void FreeDeformation::RotationDrag(const LPIE_INPUT_DATA lpd)
{
	assert(m_pEditFile);

	CvPoint2D64f spt,ept;
	spt.x = (double)m_before_mouse_pt.x - m_DragPoint[1][1].x;
	spt.y = (double)m_before_mouse_pt.y - m_DragPoint[1][1].y;
	ept.x = lpd->x - m_DragPoint[1][1].x;
	ept.y = lpd->y - m_DragPoint[1][1].y;

	m_pEditFile->ConvertPosImgToWnd(&spt, &spt);
	m_pEditFile->ConvertPosImgToWnd(&ept, &ept);

	int i,j;
	double rad_s = atan2(spt.y, spt.x);
	double rad_e = atan2(ept.y, ept.x);
	double rad = -(rad_e - rad_s);

	char buf[256];
	sprintf(buf, "rot = %f\n", 360.0*rad/(2.0*CV_PI));
	::OutputDebugString(buf);

	//copy to rotate point
	CvPoint2D64f m_RotPoint[3][3];
	for (i=0; i<3; i++) {
		for (j=0; j<3; j++) {
			m_RotPoint[i][j].x = m_DragPoint[i][j].x - m_DragPoint[1][1].x;
			m_RotPoint[i][j].y = m_DragPoint[i][j].y - m_DragPoint[1][1].y;
		}
	}

	//multiple rotation matrix
	for (i=0; i<3; i++) {
		for (j=0; j<3; j++) {
			double x = m_RotPoint[i][j].x;
			double y = m_RotPoint[i][j].y;
			m_RotPoint[i][j].x = x * cos(rad) + y * sin(rad);
			m_RotPoint[i][j].y = -x * sin(rad) + y * cos(rad);
		}
	}

	//copy rotated point
	for (i=0; i<3; i++) {
		for (j=0; j<3; j++) {
			m_DragPoint[i][j].x = m_RotPoint[i][j].x + m_DragPoint[1][1].x;
			m_DragPoint[i][j].y = m_RotPoint[i][j].y + m_DragPoint[1][1].y;
		}
	}
}
