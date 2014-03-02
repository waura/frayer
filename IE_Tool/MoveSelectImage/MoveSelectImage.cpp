// MoveSelectImage.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "resource.h"
#include "MoveSelectImage.h"

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


IETOOL_EXPORT IIETool* CreateIETool()
{
	return new MoveSelectImage;
}

IETOOL_EXPORT void ReleaseIETool(IIETool* tool)
{
	if(tool){
		delete tool;
	}
}


MoveSelectImage::MoveSelectImage()
{
}

MoveSelectImage::~MoveSelectImage()
{
	::DeleteObject(m_buttonImg);
}

HBITMAP MoveSelectImage::GetButtonImg()
{
	m_buttonImg = ::LoadBitmap((HINSTANCE)g_hLibrary, MAKEINTRESOURCE(IDB_BUTTON_BMP));
	return m_buttonImg;
}

void MoveSelectImage::OnMouseLButtonDown(LPINPUT_DATA lpd)
{
	move_X = 0;
	move_Y = 0;

	startPt.x = beforPt.x = lpd->x;
	startPt.y = beforPt.y = lpd->y;

	ImgFile *f = m_pImgEdit->GetActiveFile();
	if(f){
		int g,b,r,a;
		ImgLayer* l = f->GetSelectLayer();
		if(!l){
			IEMessageBox("レイヤーが選択されていません。");
		}

		const IplImageExt *layer_img_ext =  l->GetDrawImage();
		
		IplImage *mask = f->GetSelectMask();
		
		//選択範囲が収まる矩形を求める
		GetMaskRect(mask, &maskRc);
		//レイヤーの有効範囲を求める
		layer_img_ext->GetMaxRangeRect(&layer_Rc);
		//
		m_editNode = l->CreateEditNode(&maskRc);
		m_editNode->raster_code = IPLEXT_RASTER_CODE::ALPHA_BLEND;
		l->PushEditNode(m_editNode);
		//
		move_mask = cvCreateImage(
			cvSize(maskRc.right - maskRc.left, maskRc.bottom - maskRc.top),
			IPL_DEPTH_8U,
			1);
		//

		_I8 md;
		for(int y=maskRc.top; y <= maskRc.bottom; y++){
			for(int x=maskRc.left; x <= maskRc.right; x++){
				//選択レイヤの画像をコピー
				layer_img_ext->GetPixel(x, y, &r, &g, &b, &a);

				//動かす部分のコピーを得る
				if(md = GetMaskDataPos(mask, x, y)){
					layer_img_ext->GetPixel(x, y, &r, &g, &b, &a);
					m_editNode->edit_img.SetPixel(x-maskRc.left, y-maskRc.top, r, g, b, a);
					//画像の動かす部分は透明に
				}
				else{
					//選択されていないところは透明に
					m_editNode->edit_img.SetPixel(x-maskRc.left, y-maskRc.top, 0, 0, 0, 0);
				}
				
				//mask
				SetMaskDataPos(move_mask, x-maskRc.left, y-maskRc.top, md);
			}
		}

		beforRc = maskRc;

		////画像の更新
		//UPDATE_DATA data;
		//data.isAll = false;
		//data.rect = maskRc;
		//data.update_flag = UPDATE_FLAG::UPDATE_SELECT_LAYER;
		//f->PushUpdateData(&data);
	}
}

void MoveSelectImage::OnMouseLDrag(LPINPUT_DATA lpd)
{
	MoveImage(lpd, &beforPt);
	beforPt.x = lpd->x;
	beforPt.y = lpd->y;
}

void MoveSelectImage::OnMouseLButtonUp(LPINPUT_DATA lpd)
{
	MoveImage(lpd, &beforPt);

	cvReleaseImage(&move_mask);
}

void MoveSelectImage::MoveImage(LPINPUT_DATA lpd, LPPOINT beforPt)
{
	ImgFile *f = m_pImgEdit->GetActiveFile();
	if(f){
		int shiftX = lpd->x - beforPt->x;
		int shiftY = lpd->y - beforPt->y;
		ImgLayer *l = f->GetSelectLayer();		
		if(!l){
			IEMessageBox("レイヤーが選択されていません。");
		}
		const IplImageExt *ImgExt = l->GetDrawImage();
		IplImage* mask = f->GetSelectMask();


		move_X += shiftX;
		move_Y += shiftY;
		
		RECT newRc;
		newRc.top  = beforRc.top + shiftY;  newRc.bottom = beforRc.bottom + shiftY;
		newRc.left = beforRc.left + shiftX; newRc.right  = beforRc.right + shiftX;
		
		//
		RECT rc;
		OrRect(&beforRc, &newRc, &rc);

		//画像の移動
		m_editNode->to_rect = newRc;

		//mask zero clear
		cvSet(mask, cvScalar(0));

		//マスクの移動
		for(int y=0; y < move_mask->height; y++){
			for(int x=0; x < move_mask->width; x++){
				SetMaskDataPos(mask,
					x + move_X + maskRc.left,
					y + move_Y + maskRc.top,
					GetMaskDataPos(move_mask, x, y));
			}
		}


		//画像の更新
		UPDATE_DATA data;
		data.isAll = false;
		data.rect = rc;
		data.update_flag = UPDATE_FLAG::UPDATE_SELECT_LAYER_ALL_NODE;
		f->PushUpdateData(&data);

		beforRc = newRc;
	}
}