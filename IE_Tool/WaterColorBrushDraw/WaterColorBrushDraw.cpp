// WaterColorBrushDraw.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//
#include "stdafx.h"
#include "resource.h"
#include "WaterColorBrushDraw.h"

// for detect memory leak
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <cstdlib>
	#include <crtdbg.h>
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

static HMODULE g_hLibrary;


#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	g_hLibrary = hModule;
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

#define DISPLAY_NAME_JA "水彩ブラシ"
#define DISPLAY_NAME DISPLAY_NAME_JA

IETOOL_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::TOOL;
}

IETOOL_EXPORT IIETool* CreateIETool()
{
	return new WaterColorBrushDraw;
}

IETOOL_EXPORT void ReleaseIETool(IIETool** ppTool)
{
	if (*ppTool) {
		delete (*ppTool);
		(*ppTool) = NULL;
	}
}


WaterColorBrushDraw::WaterColorBrushDraw()
{
	m_org_color_rate    = 50;
	m_canvas_color_rate = 80;
	m_delay_color_rate  = 80;

	strcpy_s(m_name, MAX_IE_MODULE_NAME, "WaterColorBrushDraw");
	strcpy_s(m_disp_name, MAX_IE_MODULE_DISPLAY_NAME, DISPLAY_NAME);
}

WaterColorBrushDraw::~WaterColorBrushDraw()
{
	if (m_buttonImg) {
		::DeleteObject(m_buttonImg);
		m_buttonImg = NULL;
	}
}

HBITMAP WaterColorBrushDraw::GetButtonImg()
{
	if (m_buttonImg) {
		::DeleteObject(m_buttonImg);
		m_buttonImg = NULL;
	}

	m_buttonImg = ::LoadBitmap((HINSTANCE)g_hLibrary, MAKEINTRESOURCE(IDB_BUTTON_BMP));
	return m_buttonImg;
}

void WaterColorBrushDraw::OnSetToolEditWnd(IToolEditWndHandle* pHandle)
{
	pHandle->AddTrackbar("混色", 0, 100, 50, &m_org_color_rate);
	pHandle->AddTrackbar("canvas", 0, 100, 80, &m_canvas_color_rate);
	pHandle->AddTrackbar("delay", 0, 100, 80, &m_delay_color_rate);
}

void WaterColorBrushDraw::OnSelect()
{
	IBrushDraw::OnSelect();

	char buf[MAX_IE_PARAM_REGISTER_NAME];
	if (m_pImgEdit->ReadFromParamRegister(this, "org_color_rate", buf)) {
		m_org_color_rate = atoi(buf);
	}
	if (m_pImgEdit->ReadFromParamRegister(this, "canvas_color_rate", buf)) {
		m_canvas_color_rate = atoi(buf);
	}
	if (m_pImgEdit->ReadFromParamRegister(this, "delay_color_rate", buf)) {
		m_delay_color_rate = atoi(buf);
	}
}

void WaterColorBrushDraw::OnNeutral()
{
	char buf[MAX_IE_PARAM_REGISTER_NAME];

	wsprintf(buf, "%d", m_org_color_rate);
	m_pImgEdit->WriteToParamRegister(this, "org_color_rate", buf);
	wsprintf(buf, "%d", m_canvas_color_rate);
	m_pImgEdit->WriteToParamRegister(this, "canvas_color_rate", buf);
	wsprintf(buf, "%d", m_delay_color_rate);
	m_pImgEdit->WriteToParamRegister(this, "delay_color_rate", buf);

	IBrushDraw::OnNeutral();
}

void WaterColorBrushDraw::OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	m_pEditLayer = NULL;
	m_pEditLayerHandle = NULL;
	m_editNode = NULL;

	ImgFile_Ptr file = m_pImgEdit->GetActiveFile();
	if (!file) return;

	IImgLayer_weakPtr _sl = file->GetSelectLayer();
	if (_sl.expired()) {
		OutputError::Alert(IE_ERROR_SELECT_LAYER_DONT_EXIST);
		return;
	}

	IImgLayer_Ptr sl = _sl.lock();
	if (sl->GetLayerType() != IE_LAYER_TYPE::NORMAL_LAYER) {
		OutputError::Alert(IE_ERROR_CANT_EDIT_LAYER);
		return;
	}
	m_pEditLayer = std::dynamic_pointer_cast<ImgLayer>(sl);

	IEBrush_Ptr brush = m_pImgEdit->GetSelectBrush();
	if (!brush) {
		OutputError::Alert(IE_ERROR_SELECT_BRUSH_DONT_EXIST);
		return;
	}

	m_orgColor = m_pImgEdit->GetSelectFGColor();
	m_nowColor.value = m_delayColor.value = m_orgColor.value;
	m_pEditLayerHandle = (EditLayerHandle*)file->CreateImgFileHandle(IFH_EDIT_LAYER );
	m_pEditLayerHandle->SetHandleName( DISPLAY_NAME );
	if (file->DoImgFileHandle( m_pEditLayerHandle )) {
		m_editNode = m_pEditLayerHandle->CreateEditNode();

		//init blt rect
		m_editNode->blt_rect.top = m_editNode->blt_rect.left = 0;
		m_editNode->blt_rect.right = m_editNode->blt_rect.bottom = 0;

		DrawWaterColor(lpd);
	}
}

void WaterColorBrushDraw::OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	if(m_pEditLayerHandle){
		DrawWaterColor(lpd);
	}
}

void WaterColorBrushDraw::OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd)
{
	if(m_pEditLayerHandle){
		m_pEditLayerHandle->EndEdit();
	}
}

void WaterColorBrushDraw::DrawWaterColor(LPIE_INPUT_DATA lpd)
{
	IEBrush_Ptr brush = m_pImgEdit->GetSelectBrush();
	if(!brush) return;

	//convert from screen axis to image axis
	IE_INPUT_DATA input_data = (*lpd);
	input_data.x -= m_editNode->node_rect.left;
	input_data.y -= m_editNode->node_rect.top;

	int x,y;
	UCvPixel pixel;
	int8_t alpha;
	int8_t brush_n;

	RECT layer_rc;
	m_pEditLayer->GetLayerRect(&layer_rc);

	//org color mix
	ColorMix(&m_nowColor, &m_orgColor, m_org_color_rate/100.0);
	//get canvas color
	x = input_data.x;
	y = input_data.y;
	int brush_rad = brush->GetRadius();
	if (x < layer_rc.left) x = layer_rc.left;
	if (y < layer_rc.top)  y = layer_rc.top;
	if (x >= layer_rc.right)  x = layer_rc.right-1;
	if (y >= layer_rc.bottom) y = layer_rc.top-1;
	if (x - brush_rad < layer_rc.left)  brush_rad = layer_rc.left - x;
	if (y - brush_rad < layer_rc.top) brush_rad = layer_rc.top - y;
	if (x + brush_rad >= layer_rc.right) brush_rad = layer_rc.right - x - 1;
	if (y + brush_rad >= layer_rc.bottom) brush_rad = layer_rc.bottom - y - 1;


	UCvPixel cnvs_px[4];
	m_editNode->edit_img.GetPixel(x, y - brush_rad, &cnvs_px[0]);
	if(cnvs_px[0].a == 0){
		m_pEditLayer->GetPixel(x, y - brush_rad, &cnvs_px[0]);
	}
	m_editNode->edit_img.GetPixel(x, y + brush_rad, &cnvs_px[1]);
	if(cnvs_px[1].a == 0){
		m_pEditLayer->GetPixel(x, y + brush_rad, &cnvs_px[1]);
	}
	m_editNode->edit_img.GetPixel(x - brush_rad, y, &cnvs_px[2]);
	if(cnvs_px[2].a == 0){
		m_pEditLayer->GetPixel(x - brush_rad, y, &cnvs_px[2]);
	}
	m_editNode->edit_img.GetPixel(x + brush_rad, y, &cnvs_px[3]);
	if(cnvs_px[3].a == 0){
		m_pEditLayer->GetPixel(x + brush_rad, y, &cnvs_px[3]);
	}
	pixel.b = (cnvs_px[0].b + cnvs_px[1].b + cnvs_px[2].b + cnvs_px[3].b) >> 2;
	pixel.g = (cnvs_px[0].g + cnvs_px[1].g + cnvs_px[2].g + cnvs_px[3].g) >> 2;
	pixel.r = (cnvs_px[0].r + cnvs_px[1].r + cnvs_px[2].r + cnvs_px[3].r) >> 2;
	//delay color mix
	ColorMix(&pixel, &m_delayColor, m_delay_color_rate/100.0);
	//canvas color mix
	ColorMix(&m_nowColor, &pixel, m_canvas_color_rate/100.0);
	//
	m_delayColor.value = m_nowColor.value;

	RECT draw_rc;
	if (brush->GetBrushType() == IE_BRUSH_TYPE::IE_EM_BRUSH) {
		IEEMBrush_Ptr _brush = std::dynamic_pointer_cast<IEEMBrush>(brush);

		WaterColorBrushCopyOp copyOp;
		copyOp.SetColor(&m_nowColor);
		_brush->BltBrushData(
			copyOp,
			&(m_editNode->edit_img),
			&input_data,
			&draw_rc);
	}

	if( (draw_rc.left >= draw_rc.right) || (draw_rc.top >= draw_rc.bottom) )
		return;

	//convert from image axis to screen axis
	draw_rc.left += m_editNode->node_rect.left;
	draw_rc.top += m_editNode->node_rect.top;
	draw_rc.right += m_editNode->node_rect.left;
	draw_rc.bottom += m_editNode->node_rect.top;

	//add draw rect
	if(m_editNode->blt_rect.top == m_editNode->blt_rect.bottom){
		//if blt_rect is empty, init
		m_editNode->blt_rect = draw_rc;
	}
	else{
		OrRect(&draw_rc, &(m_editNode->blt_rect), &(m_editNode->blt_rect));
	}

	//update layer image
	m_pEditLayerHandle->Update(&draw_rc);

}

void WaterColorBrushDraw::ColorMix(UCvPixel* dst, const UCvPixel* src, double src_rate)
{
	dst->b = dst->b * (1.0 - src_rate) + src->b * src_rate;
	dst->g = dst->g * (1.0 - src_rate) + src->g * src_rate;
	dst->r = dst->r * (1.0 - src_rate) + src->r * src_rate;
}