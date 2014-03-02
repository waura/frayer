#include "IMakeRect.h"

#define FD_POINT_RECT_SIZE 4


IMakeRect::IMakeRect()
{
	m_isMakedRect = false;
	m_hPen = ::CreatePen(PS_SOLID, 2, RGB(255,0,0));
}

IMakeRect::~IMakeRect()
{
	if (m_hPen) {
		::DeleteObject(m_hPen);
		m_hPen = NULL;
	}
}

void IMakeRect::OnSelect()
{
	ClearDragPoint();
}

void IMakeRect::OnNeutral()
{
	ClearDragPoint();
}

void IMakeRect::OnDraw(HDC hdc)
{
	if(m_isMakedRect){
		int i,j;
		HPEN hOldPen;

		//set pen
		hOldPen = (HPEN)::SelectObject(hdc, m_hPen);
		//
		::SelectObject(hdc, GetStockObject(NULL_BRUSH));	

		CvPoint2D64f draw_pt;
		ImgFile_Ptr pEditFile = m_pImgEdit->GetActiveFile();
		if(pEditFile == NULL) return;

		//draw rect frame
		pEditFile->ConvertPosImgToWnd(&(m_DragPoint[0][0]), &(draw_pt));
		::MoveToEx(hdc, draw_pt.x, draw_pt.y, NULL);
		pEditFile->ConvertPosImgToWnd(&(m_DragPoint[1][0]), &(draw_pt));
		::LineTo(hdc, draw_pt.x, draw_pt.y);
		pEditFile->ConvertPosImgToWnd(&(m_DragPoint[2][0]), &(draw_pt));
		::LineTo(hdc, draw_pt.x, draw_pt.y);
		pEditFile->ConvertPosImgToWnd(&(m_DragPoint[2][1]), &(draw_pt));
		::LineTo(hdc, draw_pt.x, draw_pt.y);
		pEditFile->ConvertPosImgToWnd(&(m_DragPoint[2][2]), &(draw_pt));
		::LineTo(hdc, draw_pt.x, draw_pt.y);
		pEditFile->ConvertPosImgToWnd(&(m_DragPoint[1][2]), &(draw_pt));
		::LineTo(hdc, draw_pt.x, draw_pt.y);
		pEditFile->ConvertPosImgToWnd(&(m_DragPoint[0][2]), &(draw_pt));
		::LineTo(hdc, draw_pt.x, draw_pt.y);
		pEditFile->ConvertPosImgToWnd(&(m_DragPoint[0][1]), &(draw_pt));
		::LineTo(hdc, draw_pt.x, draw_pt.y);
		pEditFile->ConvertPosImgToWnd(&(m_DragPoint[0][0]), &(draw_pt));
		::LineTo(hdc, draw_pt.x, draw_pt.y);

		//
		for(i=0; i<3; i++){
			for(j=0; j<3; j++){
				pEditFile->ConvertPosImgToWnd(&(m_DragPoint[i][j]), &(draw_pt));
				DrawPointRect(hdc, &(draw_pt));
			}
		}

		//
		::SelectObject(hdc, hOldPen);
	}
}

void IMakeRect::OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	int i,j;

	if(m_isMakedRect){
		CvPoint2D64f mouse;
		m_befor_mouse_pt.x = mouse.x = lpd->x;
		m_befor_mouse_pt.y = mouse.y = lpd->y;
		m_isClickOnDragPointArea = false;
		m_SelectPointIndex.col = m_SelectPointIndex.row = -1;
		m_DragFlags = nFlags;
		
		//check hit on corner point
		POINT_INDEX cpoint_index_list[] = {
			{0,0},
			{0,2},
			{2,0},
			{2,2}};

		for(i=0; i<4; i++){
			int c = cpoint_index_list[i].col;
			int r = cpoint_index_list[i].row;
			if( isOnPointRect(&(m_DragPoint[r][c]), &mouse) ){
				m_SelectPointIndex.row = r;
				m_SelectPointIndex.col = c;
				return;
			}
		}

		//check hit on middle point
		POINT_INDEX mpoint_index_list[] = {
			{1, 0},
			{0, 1},
			{2, 1},
			{1, 2}};

		for(i=0; i<4; i++){
			int c = mpoint_index_list[i].col;
			int r = mpoint_index_list[i].row;
			if( isOnPointRect(&(m_DragPoint[r][c]), &mouse) ){
				m_SelectPointIndex.row = r;
				m_SelectPointIndex.col = c;
				return;
			}
		}

		//check hit on freedeformation area
		RECT rc;
		GetDragPointRect(&rc);
		if((rc.left <= mouse.x && mouse.x <= rc.right) &&
			(rc.top <= mouse.y && mouse.y <= rc.bottom)){
				m_isClickOnDragPointArea = true;
				return;
		}

	}

	//make new rect
	m_isMakedRect = true;
	
	for(i=0; i<3; i++){
		for(j=0; j<3; j++){
			m_DragPoint[i][j].x = lpd->x;
			m_DragPoint[i][j].y = lpd->y;
		}
	}
	m_befor_mouse_pt.x = lpd->x;
	m_befor_mouse_pt.y = lpd->y;

	m_SelectPointIndex.row = 2;
	m_SelectPointIndex.col = 2;
}

void IMakeRect::OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	if(m_isMakedRect){
		int i,j;
		int spi_c = m_SelectPointIndex.col;
		int spi_r = m_SelectPointIndex.row;

		if(m_SelectPointIndex.col != -1 && m_SelectPointIndex.row != -1){
			//if(IE_MK_CONTROL == (m_DragFlags & IE_MK_CONTROL)){
			//	ControlDrag(lpd);
			//}
			//else{
				NormalDrag(lpd);
			//}
		}
		else if(m_isClickOnDragPointArea){
			//
			int shiftX = lpd->x - m_befor_mouse_pt.x;
			int shiftY = lpd->y - m_befor_mouse_pt.y;

			for(i=0; i<3; i++){
				for(j=0; j<3; j++){
					m_DragPoint[i][j].x = m_DragPoint[i][j].x + shiftX;
					m_DragPoint[i][j].y = m_DragPoint[i][j].y + shiftY;
				}
			}
		}

		m_befor_mouse_pt.x = lpd->x;
		m_befor_mouse_pt.y = lpd->y;
	}
}

void IMakeRect::OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	if(m_isMakedRect){
		m_SelectPointIndex.col = m_SelectPointIndex.row = -1;
	}
}

void IMakeRect::GetDragPointRect(LPRECT lprc)
{
	//変形先の範囲
	int max_x = 0;
	int max_y = 0;
	int min_x = 0x7fff;
	int min_y = 0x7fff;

	POINT_INDEX cpoint_index_list[] = {
		{0,0},
		{0,2},
		{2,2},
		{2,0}};

	//コーナーの点取り出し
	int i;
	for(i=0; i<4; i++){
		int c = cpoint_index_list[i].col;
		int r = cpoint_index_list[i].row;

		if(max_x < m_DragPoint[r][c].x){
			max_x = m_DragPoint[r][c].x;
		}
		if(max_y < m_DragPoint[r][c].y){
			max_y = m_DragPoint[r][c].y;
		}
		if(min_x > m_DragPoint[r][c].x){
			min_x = m_DragPoint[r][c].x;
		}
		if(min_y > m_DragPoint[r][c].y){
			min_y = m_DragPoint[r][c].y;
		}
	}

	lprc->top = min_y;
	lprc->left = min_x;
	lprc->bottom = max_y;
	lprc->right = max_x;
}

void IMakeRect::ClearDragPoint()
{
	m_isMakedRect = false;
}

void IMakeRect::NormalDrag(LPIE_INPUT_DATA lpd)
{
	int i;
	int spi_c = m_SelectPointIndex.col;
	int spi_r = m_SelectPointIndex.row;

	if(m_SelectPointIndex.col == 1){
		//middle point, move x pos
		for(i=0; i<3; i++){
			m_DragPoint[spi_r][i].y += lpd->y - m_befor_mouse_pt.y;
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
			m_DragPoint[i][spi_c].x += lpd->x - m_befor_mouse_pt.x;
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
			m_DragPoint[spi_r][i].y += lpd->y - m_befor_mouse_pt.y;
			m_DragPoint[i][spi_c].x += lpd->x - m_befor_mouse_pt.x;
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

void IMakeRect::DrawPointRect(HDC hdc, const CvPoint2D64f* pcvPoint)
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

bool IMakeRect::isOnPointRect(const CvPoint2D64f* pcvPoint, const CvPoint2D64f* pMouse)
{
	if((pcvPoint->x - FD_POINT_RECT_SIZE <= pMouse->x && pMouse->x <= pcvPoint->x + FD_POINT_RECT_SIZE) &&
		(pcvPoint->y - FD_POINT_RECT_SIZE <= pMouse->y && pMouse->y <= pcvPoint->y + FD_POINT_RECT_SIZE))
	{
		return true;
	}

	return false;
}