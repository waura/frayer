#include "stdafx.h"
#include "UtilOpenCVFigure.h"

///////////////////////////////
/*!
	線分のマスクを作る。
	@param[out] mask マスクの出力先
	@param[in] setnum 埋める値
	@param[in] start 開始位置
	@param[in] end 終了位置
*/
void LineMask(IplImage *mask, uint8_t setnum, const CvPoint2D64f *start, const CvPoint2D64f *end)
{
	float t=0.0;
	CvPoint2D64f d; //傾き
	CvPoint2D64f p;

	//end-startの傾きを計算
	SubVec2D(&d, end, start);
	float d_length = Vec2DLength(&d);
	float rev_d_length = 1.0 / d_length;
	do{
		//p = start + t*d
		ScaleVec2D(&p, t, &d);
		AddVec2D(&p, &p , start);
		//位置pにsetnum書き込み
		SetMaskDataPos(mask, p.x, p.y, setnum);

		t += rev_d_length/2.0;
	}while(t <= 1.0);
}

void DrawLine(CvArr* img, CvPoint pt1, CvPoint pt2,
              CvScalar color, int thickness, int line_type, int shift){
	cvLine(img, pt1, pt2, color, thickness, line_type, shift);
}

void DrawDotLine(CvArr* img, CvPoint pt1, CvPoint pt2,
              CvScalar color, int dot_length, int thickness, int line_type, int shift){
	cvLine(img, pt1, pt2, color, thickness, line_type, shift);
}

// a(X-x0)^2 + b(Y-y0)^2 = r^2
static void FillEllipse(IplImage* mask, int setnum, int x0, int y0, int r, int a, int b)
{
  int x,y,F,H;
  CvSize size = cvGetSize(mask);

  x = r / sqrt( (double)a );
  y = 0;
  F = -2 * sqrt( (double)a ) * r + a + 2*b;
  H = -4 * sqrt( (double)a ) * r + 2*a + b;

  while ( x > 0 ) {
	for(int i=x0 - x; i<=x0 + x; i++){
		SetMaskDataPos(mask, i, y0 + y, setnum);	
		SetMaskDataPos(mask, i, y0 - y, setnum);
	}
    if ( F < 0 ) {
      y++;
      F += 4 * b * y + 2 * b;
      H += 4 * b * y;
    } else if ( H >= 0 ) {
      x--;
      F -= 4*a*x;
      H -= 4*a*x - 2*a;
    } else {
      x--;
      y++;
      F += 4*b*y - 4*a*x + 2*b;
      H += 4*b*y - 4*a*x + 2*a;
    }
  }

}

// a(X-x0)^2 + b(Y-y0)^2 = r^2
static void IplExtFillEllipse(IplImageExt* mask, uint8_t setnum, int x0, int y0, int r, int a, int b)
{
  int x,y,F,H;
  CvSize size = mask->GetSize();

  x = r / sqrt( (double)a );
  y = 0;
  F = -2 * sqrt( (double)a ) * r + a + 2*b;
  H = -4 * sqrt( (double)a ) * r + 2*a + b;

  while ( x > 0 ) {
	for(int i=x0 - x; i<x0 + x; i++){
		if(0 <= i && i < size.width){
			if(0 <= (y0 + y) && (y0 + y) < size.height)
				mask->SetMaskData(i, y0 + y, setnum);
			if(0 <= (y0 - y) && (y0 - y) <size.height)
				mask->SetMaskData(i, y0 - y, setnum);
		}
	}
    if ( F < 0 ) {
      y++;
      F += 4 * b * y + 2 * b;
      H += 4 * b * y;
    } else if ( H >= 0 ) {
      x--;
      F -= 4*a*x;
      H -= 4*a*x - 2*a;
    } else {
      x--;
      y++;
      F += 4*b*y - 4*a*x + 2*b;
      H += 4*b*y - 4*a*x + 2*a;
    }
  }

}


//四角形のマスクを作る。
void FillRectMask(IplImage* mask, uint8_t setnum, int x, int y, int width, int height)
{
	int x_ = min(x, x + width);
	int y_ = min(y, y + height);
	int width_ = abs(width);
	int height_ = abs(height);

	for(int iy = y_; iy <= y_ + height_; iy++){
		for(int ix = x_; ix <= x_ + width_; ix++){
			SetMaskDataPos(mask, ix, iy, setnum);
		}
	}
}

void IplExtFillRectMask(IplImageExt* mask, uint8_t setnum, int x, int y, int width, int height)
{
	int x_ = min(x, x + width);
	int y_ = min(y, y + height);
	int width_ = abs(width);
	int height_ = abs(height);

	mask->RectFillMask(x_, y_, width_, height_, (int)setnum);
}

void FillEllipseMask(IplImage* mask, uint8_t setnum, int x, int y, int width, int height)
{
	int x_ = min(x, x + width);
	int y_ = min(y, y + height);
	int width_ = abs(width);
	int height_ = abs(height);
	int a = width_/2;
	int b = height_/2;

	//楕円を描画
	FillEllipse(mask, setnum, x_ + width_/2, y_ + height_/2, a*b, b*b, a*a);
}

void IplExtFillEllipseMask(IplImageExt* mask, uint8_t setnum, int x, int y, int width, int height)
{
	int x_ = min(x, x + width);
	int y_ = min(y, y + height);
	int width_ = abs(width);
	int height_ = abs(height);
	int a = width_/2 - 1;
	int b = height_/2 - 1;

	//楕円を描画
	IplExtFillEllipse(mask, setnum, x_ + width_/2, y_ + height_/2, a*b, b*b, a*a);
}

//////////////////////////////////
/*!
	正円のマスクを作る。
	@param[out] mask マスクの出力先
	@param[in] setnum 埋める値
	@param[in] x0
	@param[in] y0
	@param[in] r
*/
void FillCircleMask(IplImage* mask, uint8_t setnum, int x0, int y0, int  r)
{
 // int x,y,F;

 // x = r;
 // y = 0;
 // F = -2 * r + 3;

 // while ( x >= y ) {
	//for(int i=x0 -x; i<=x0+x; i++){
	//	SetMaskDataPos(mask, i, y0 + y, setnum);
	//	SetMaskDataPos(mask, i, y0 - y, setnum);
	//	SetMaskDataPos(mask, i, y0 + x, setnum);
	//	SetMaskDataPos(mask, i, y0 - x, setnum); 
	//}
 //   if ( F >= 0 ) {
 //     x--;
 //     F -= 4 * x;
 //   }
 //   y++;
 //   F += 4 * y + 2;
 // }

	FillEllipseMask(mask, setnum, x0-r, y0-r, r*2, r*2);
}

///////////////////////////////////
/*!
	create antialiasing circle mask
	@param[out] dst
	@param[in] center_x
	@param[in] center_y
	@param[in] r
	@param[in] div
	@param[in] uint8_t setnum
	@param[in] alapha
*/
void FillCircleMaskAA(
	IplImage* dst,
	double center_x,
	double center_y,
	double r,
	int div,
	uint8_t setnum,
	uint8_t alpha)
{
	assert(dst);
	assert(r > 0);
	assert((center_x + r) > 0.0);
	assert((center_y + r) > 0.0);
	assert((center_x - r) < dst->width);
	assert((center_y - r) < dst->height);

	bool is_small = false;
	if(r <= 3) is_small = true;

	int start_x = (int)(center_x - r);
	int start_y = (int)(center_y - r);
	int end_x = (int)(center_x + r);
	int end_y = (int)(center_y + r);

	//固定小数表現する
	int FIXMUL = 32;
	int ir = (int)(r * FIXMUL);
	int icenter_x = (int)(center_x * FIXMUL);
	int icenter_y = (int)(center_y * FIXMUL);

	int dr = (int)(r * div);
	int dcenter_x = (int)(center_x * div);
	int dcenter_y = (int)(center_y * div);

	double dalpha = alpha / 255.0;
	
	int x,y;
	for(y=start_y; y<=end_y; y++){
		for(x=start_x; x<=end_x; x++){
			int fx = x * FIXMUL - icenter_x;
			int fy = y * FIXMUL - icenter_y;

			bool inside0 = false; //左上
			bool inside1 = false; //右上
			bool inside2 = false; //左下
			bool inside3 = false; //右下

			if(!is_small){
				if((fx*fx + fy*fy) <= ir*ir) inside0 = true;
				if(((fx + FIXMUL)*(fx + FIXMUL) + fy*fy) <= ir*ir) inside1 = true;
				if((fx*fx + (fy + FIXMUL)*(fy + FIXMUL)) <= ir*ir) inside2 = true;
				if(((fx + FIXMUL)*(fx + FIXMUL) + (fy + FIXMUL)*(fy + FIXMUL)) <= ir*ir) inside3 = true;
			}
			else{
				inside0 = true;
			}

			bool outside = !inside0 && !inside1 && !inside2 && !inside3;
			if(outside){
				continue;
			}

			bool notaa = inside0 && inside1 && inside2 && inside3;
			if(notaa){
				SetMaskDataPos(dst, x, y, setnum*dalpha);
			}
			else{
				//
				int c = 0;
				int i,j;
				for(j=0; j<div; j++){
					for(i=0; i<div; i++){
						if((x*div+i - dcenter_x)*(x*div+i - dcenter_x) + (y*div+j - dcenter_y)*(y*div+j -  dcenter_y) < dr*dr)
							c++;
					}
				}

				int d = 255* c / (div*div);
				SetMaskDataPos(dst, x, y, setnum*(d/255.0)*dalpha);
			}
		}
	}
}

void AddFillCircleMaskAA(
	IplImage* dst,
	double center_x,
	double center_y,
	double r,
	int div,
	uint8_t setnum,
	uint8_t alpha)
{
	assert(dst);
	assert(r > 0);

	bool is_small = false;
	if(r <= 3) is_small = true;

	int start_x = (int)(center_x - r);
	int start_y = (int)(center_y - r);
	int end_x = (int)(center_x + r);
	int end_y = (int)(center_y + r);

	if(start_x < 0) start_x=0;
	if(start_y < 0) start_y=0;
	if(end_x >= dst->width) end_x = dst->width-1;
	if(end_y >= dst->height) end_y = dst->height-1;

	//固定小数表現する
	int FIXMUL = 32;
	int ir = (int)(r * FIXMUL);
	int icenter_x = (int)(center_x * FIXMUL);
	int icenter_y = (int)(center_y * FIXMUL);

	int dr = (int)(r * div);
	int dcenter_x = (int)(center_x * div);
	int dcenter_y = (int)(center_y * div);

	double dalpha = alpha / 255.0;
	
	int x,y;
	for(y=start_y; y<=end_y; y++){
		for(x=start_x; x<=end_x; x++){
			int fx = (x << 5) - icenter_x; //x * FIXMUL - icenter_x;
			int fy = (y << 5) - icenter_y; //y * FIXMUL - icenter_y;

			bool inside0 = false; //左上
			bool inside1 = false; //右上
			bool inside2 = false; //左下
			bool inside3 = false; //右下

			if(!is_small){
				if((fx*fx + fy*fy) <= ir*ir) inside0 = true;
				if(((fx + FIXMUL)*(fx + FIXMUL) + fy*fy) <= ir*ir) inside1 = true;
				if((fx*fx + (fy + FIXMUL)*(fy + FIXMUL)) <= ir*ir) inside2 = true;
				if(((fx + FIXMUL)*(fx + FIXMUL) + (fy + FIXMUL)*(fy + FIXMUL)) <= ir*ir) inside3 = true;
			}
			else{
				inside0 = true;
			}

			bool outside = !inside0 && !inside1 && !inside2 && !inside3;
			if(outside){
				continue;
			}

			bool notaa = inside0 && inside1 && inside2 && inside3;
			if(notaa){
				SetMaskDataPos(dst, x, y, setnum*dalpha);
			}
			else{
				//
				int c = 0;
				int i,j;
				for(j=0; j<div; j++){
					for(i=0; i<div; i++){
						if((x*div+i - dcenter_x)*(x*div+i - dcenter_x) + (y*div+j - dcenter_y)*(y*div+j -  dcenter_y) < dr*dr)
							c++;
					}
				}

				int d = 255* c / (div*div);
				uint8_t val = setnum*(d/255.0)*dalpha;
				if(val > GetMaskDataPos(dst, x, y)){
					SetMaskDataPos(dst, x, y, val);
				}
			}
		}
	}
}

void FillCircleMaskAA2(
	IplImage* dst,
	double center_x,
	double center_y,
	double r,
	double aa_d,
	uint8_t setnum,
	uint8_t alpha)
{
	assert(dst);
	assert(r > 0);
	assert(0.0 < aa_d);

	double aa_min_d = r-aa_d;
	double aa_max_d = r+aa_d;
	double aa_min_dd = aa_min_d * aa_min_d;
	double aa_max_dd = aa_max_d * aa_max_d;

	double inv_alpha = alpha / 255.0;

	int start_x = (int)(center_x - aa_max_d);
	int start_y = (int)(center_y - aa_max_d);
	int end_x = (int)(center_x + aa_max_d);
	int end_y = (int)(center_y + aa_max_d);

	assert(start_x >= 0);
	assert(start_y >= 0);
	assert(end_x < dst->width);
	assert(end_y < dst->height);

	int x,y;
	uint8_t pix;
	uint8_t* dst_pix;
	for(y=start_y; y<=end_y; y++){
		dst_pix = GetPixelAddress(dst, start_x, y);
		for(x=start_x; x<=end_x; x++){
			double frr = (double)(x-center_x+0.5)*(x-center_x+0.5)+(y-center_y+0.5)*(y-center_y+0.5);
			if(frr < aa_min_dd){
				(*dst_pix++) = setnum*inv_alpha;
			}
			else if(frr >= aa_max_dd){
				(*dst_pix++) = 0;
			}
			else{
				double fr = sqrt(frr);
				double tmp = 1.0 - (fr - aa_min_d)/(aa_max_d - aa_min_d);
				(*dst_pix++) = (tmp*tmp) * (3.0-2.0*tmp) * setnum*inv_alpha;
			}
		}
	}
}

void FillCircleMaskAA2e(
	IplImage* dst,
	double center_x,
	double center_y,
	double r,
	double aa_d,
	uint8_t setnum,
	uint8_t alpha)
{
	assert(dst);
	assert(r > 0);
	assert(0.0 < aa_d);

	double aa_min_d = r-aa_d;
	double aa_max_d = r+aa_d;
	double aa_min_dd = aa_min_d * aa_min_d;
	double aa_max_dd = aa_max_d * aa_max_d;

	double inv_alpha = alpha / 255.0;

	int start_x = (int)(center_x - aa_max_d);
	int start_y = (int)(center_y - aa_max_d);
	int end_x = (int)(center_x + aa_max_d);
	int end_y = (int)(center_y + aa_max_d);

	assert(start_x >= 0);
	assert(start_y >= 0);
	assert(end_x < dst->width);
	assert(end_y < dst->height);

	int x,y;
	uint8_t pix;
	uint8_t* dst_pix;
	for(y=start_y; y<=end_y; y++){
		dst_pix = GetPixelAddress(dst, start_x, y);
		for(x=start_x; x<=end_x; x++){
			double frr = (double)(x-center_x+0.5)*(x-center_x+0.5)+(y-center_y+0.5)*(y-center_y+0.5);
			if(frr < aa_min_dd){
				(*dst_pix++) = setnum*inv_alpha;
			}
			else if(frr >= aa_max_dd){
				(*dst_pix++) = 0;
			}
			else{
				double fr = sqrt(frr);
				double tmp = 1.0 - (fr - aa_min_d)/(aa_max_d - aa_min_d);
				(*dst_pix++) = (tmp*tmp) * (3.0-2.0*tmp) * setnum*inv_alpha;
			}
		}
	}
}

void AddFillCircleMaskAA2(
	IplImage* dst,
	double center_x,
	double center_y,
	double r,
	double aa_d,
	uint8_t setnum,
	uint8_t alpha)
{
	assert(dst);
	assert(r > 0);
	assert(0.0 < aa_d);

	double aa_min_d = r-aa_d;
	double aa_max_d = r+aa_d;
	double aa_min_dd = aa_min_d * aa_min_d;
	double aa_max_dd = aa_max_d * aa_max_d;

	double inv_alpha = alpha / 255.0;

	int start_x = (int)(center_x - aa_max_d);
	int start_y = (int)(center_y - aa_max_d);
	int end_x = (int)(center_x + aa_max_d);
	int end_y = (int)(center_y + aa_max_d);

	if(start_x < 0) start_x=0;
	if(start_y < 0) start_y=0;
	if(start_x >= dst->width) start_x = dst->width-1;
	if(start_y >= dst->height) start_y = dst->height-1;
	if(end_x < 0) end_x=0;
	if(end_y < 0) end_y=0;
	if(end_x >= dst->width) end_x = dst->width-1;
	if(end_y >= dst->height) end_y = dst->height-1;
	
	int x,y;
	uint8_t pix;
	uint8_t* dst_pix;
	for(y=start_y; y<=end_y; y++){
		dst_pix = GetPixelAddress(dst, start_x, y);
		for(x=start_x; x<=end_x; x++){		
			double frr = (double)(x-center_x+0.5)*(x-center_x+0.5)+(y-center_y+0.5)*(y-center_y+0.5);
			if(frr < aa_min_dd){
				pix = setnum*inv_alpha;
			}
			else if(frr >= aa_max_dd){
				pix = 0;
			}
			else{
				double fr = sqrt(frr);
				double tmp = 1.0 - (fr - aa_min_d)/(aa_max_d - aa_min_d);
				pix = (tmp*tmp) * (3.0-2.0*tmp) * setnum*inv_alpha;
			}

			if(pix > (*dst_pix)){
				(*dst_pix) = pix;
			}
			dst_pix++;
		}
	}
}

void FillCircleMaskAA3(
	IplImage* dst,
	double center_x,
	double center_y,
	double r,
	double aa_d,
	uint8_t setnum,
	uint8_t alpha)
{
	assert(dst);
	assert(r > 0);
	assert(0.0 < aa_d);

	double aa_min_d = r-aa_d;
	double aa_max_d = r+aa_d;

	double inv_alpha = alpha / 255.0;

	int start_x = (int)(center_x - aa_max_d);
	int start_y = (int)(center_y - aa_max_d);
	int end_x = (int)(center_x + aa_max_d);
	int end_y = (int)(center_y + aa_max_d);

	assert(start_x >= 0);
	assert(start_y >= 0);
	assert(end_x < dst->width);
	assert(end_y < dst->height);

	//浮動小数に定数を掛けて整数として計算する
	int FIXMUL = 32;
	int HF_FIXMUL = (int)FIXMUL/2.0;
	int ir = (int)(r * FIXMUL);
	int icenter_x = (int)(center_x * FIXMUL);
	int icenter_y = (int)(center_y * FIXMUL);

	
	int x,y;
	for(y=start_y; y<=end_y; y++){
		for(x=start_x; x<=end_x; x++){
			//int fx = x * FIXMUL - icenter_x;
			//int fy = y * FIXMUL - icenter_y;

			//bool inside0 = false; //左上
			//bool inside1 = false; //右上
			//bool inside2 = false; //左下
			//bool inside3 = false; //右下

			//if(!is_small){
			//	if((fx*fx + fy*fy) <= ir*ir) inside0 = true;
			//	if(((fx + FIXMUL)*(fx + FIXMUL) + fy*fy) <= ir*ir) inside1 = true;
			//	if((fx*fx + (fy + FIXMUL)*(fy + FIXMUL)) <= ir*ir) inside2 = true;
			//	if(((fx + FIXMUL)*(fx + FIXMUL) + (fy + FIXMUL)*(fy + FIXMUL)) <= ir*ir) inside3 = true;
			//}
			//else{
			//	is_small = true;
			//}

			//bool outside = !inside0 && !inside1 && !inside2 && !inside3;
			//if(outside){
			//	continue;
			//}

			//bool notaa = inside0 && inside1 && inside2 && inside3;
			//if(notaa){
			//	SetMaskDataPos(dst, x, y, setnum*inv_alpha);
			//}
			//else{
				uint8_t pix;
				double fr = sqrt((double)(x-center_x+0.5)*(x-center_x+0.5)+(y-center_y+0.5)*(y-center_y+0.5));
				if(fr < aa_min_d){
					pix = setnum*inv_alpha;
				}
				else if(fr >= aa_max_d){
					pix = 0;
				}
				else{
					double tmp = 1.0 - (fr - aa_min_d)/(aa_max_d - aa_min_d);
					pix = tmp * setnum*inv_alpha;
				}
				SetMaskDataPos(dst, x, y, pix);
			//}
		}
	}
}

void AddFillCircleMaskAA3(
	IplImage* dst,
	double center_x,
	double center_y,
	double r,
	double aa_d,
	uint8_t setnum,
	uint8_t alpha)
{
	assert(dst);
	assert(r > 0);
	assert(0.0 < aa_d);

	double aa_min_d = r-aa_d;
	double aa_max_d = r+aa_d;

	double inv_alpha = alpha / 255.0;

	int start_x = (int)(center_x - aa_max_d);
	int start_y = (int)(center_y - aa_max_d);
	int end_x = (int)(center_x + aa_max_d);
	int end_y = (int)(center_y + aa_max_d);

	if(start_x < 0) start_x=0;
	if(start_y < 0) start_y=0;
	if(end_x >= dst->width) end_x = dst->width-1;
	if(end_y >= dst->height) end_y = dst->height-1;

	//浮動小数に定数を掛けて整数として計算する
	int FIXMUL = 32;
	int HF_FIXMUL = (int)FIXMUL/2.0;
	int ir = (int)(r * FIXMUL);
	int icenter_x = (int)(center_x * FIXMUL);
	int icenter_y = (int)(center_y * FIXMUL);

	
	int x,y;
	for(y=start_y; y<=end_y; y++){
		for(x=start_x; x<=end_x; x++){
			//int fx = x * FIXMUL - icenter_x;
			//int fy = y * FIXMUL - icenter_y;

			//bool inside0 = false; //左上
			//bool inside1 = false; //右上
			//bool inside2 = false; //左下
			//bool inside3 = false; //右下

			//if(!is_small){
			//	if((fx*fx + fy*fy) <= ir*ir) inside0 = true;
			//	if(((fx + FIXMUL)*(fx + FIXMUL) + fy*fy) <= ir*ir) inside1 = true;
			//	if((fx*fx + (fy + FIXMUL)*(fy + FIXMUL)) <= ir*ir) inside2 = true;
			//	if(((fx + FIXMUL)*(fx + FIXMUL) + (fy + FIXMUL)*(fy + FIXMUL)) <= ir*ir) inside3 = true;
			//}
			//else{
			//	is_small = true;
			//}

			//bool outside = !inside0 && !inside1 && !inside2 && !inside3;
			//if(outside){
			//	continue;
			//}

			//bool notaa = inside0 && inside1 && inside2 && inside3;
			//if(notaa){
			//	SetMaskDataPos(dst, x, y, setnum*inv_alpha);
			//}
			//else{
				uint8_t pix;
				double fr = sqrt((double)(x-center_x+0.5)*(x-center_x+0.5)+(y-center_y+0.5)*(y-center_y+0.5));
				if(fr < aa_min_d){
					pix = setnum*inv_alpha;
				}
				else if(fr >= aa_max_d){
					pix = 0;
				}
				else{
					double tmp = 1.0 - (fr - aa_min_d)/(aa_max_d - aa_min_d);
					pix = tmp * setnum*inv_alpha;
				}

				if(pix > GetMaskDataPos(dst, x, y)){
					SetMaskDataPos(dst, x, y, pix);
				}
			//}
		}
	}
}

void SoftCircleMask(
	IplImage* dst,
	double center_x,
	double center_y,
	double r,
	uint8_t setnum,
	uint8_t alpha)
{
	assert(dst);
	assert(r > 0);

	double inv_alpha = alpha / 255.0;

	int start_x = (int)(center_x - r);
	int start_y = (int)(center_y - r);
	int end_x = (int)(center_x + r + 0.5);
	int end_y = (int)(center_y + r + 0.5);

	assert(start_x >= 0);
	assert(start_y >= 0);
	assert(end_x < dst->width);
	assert(end_y < dst->height);

	int x,y;
	uint8_t pix;
	for(y=start_y; y<=end_y; y++){
		for(x=start_x; x<=end_x; x++){
			double fr = sqrt((double)(x-center_x+0.5)*(x-center_x+0.5)+(y-center_y+0.5)*(y-center_y+0.5));

			if(fr < r){
				fr = 1.0 - fr/r;
				pix = (fr*fr) * (3.0-2.0*fr) * setnum*inv_alpha;
			}
			else{
				pix = 0;
			}

			SetMaskDataPos(dst, x, y, pix);
		}
	}
}

void AddSoftCircleMask(
	IplImage* dst,
	double center_x,
	double center_y,
	double r,
	uint8_t setnum,
	uint8_t alpha)
{
	assert(dst);
	assert(r > 0);

	double inv_alpha = alpha / 255.0;

	int start_x = (int)(center_x - r);
	int start_y = (int)(center_y - r);
	int end_x = (int)(center_x + r + 0.5);
	int end_y = (int)(center_y + r + 0.5);

	if(start_x < 0) start_x=0;
	if(start_y < 0) start_y=0;
	if(start_x >= dst->width) start_x = dst->width-1;
	if(start_y >= dst->height) start_y = dst->height-1;
	if(end_x < 0) end_x=0;
	if(end_y < 0) end_y=0;
	if(end_x >= dst->width) end_x = dst->width-1;
	if(end_y >= dst->height) end_y = dst->height-1;

	double rr = r*r;

	int x,y;
	uint8_t pix;
	uint8_t* dst_pix;
	for(y=start_y; y<=end_y; y++){
		dst_pix = GetPixelAddress(dst, start_x, y);
		for(x=start_x; x<=end_x; x++){
			double frr = (double)(x-center_x+0.5)*(x-center_x+0.5)+(y-center_y+0.5)*(y-center_y+0.5);

			if(frr < rr){
				double fr = sqrt(frr);
				fr = 1.0 - fr/r;
				pix = (fr*fr) * (3.0-2.0*fr) * setnum*inv_alpha;
	
				if(pix > (*dst_pix)){
					(*dst_pix) = pix;
				}
			}

			dst_pix++;
		}
	}
}

void AddSoftCircleMaskf(
	IplImage* dst,
	float center_x,
	float center_y,
	float r,
	uint8_t setnum,
	uint8_t alpha)
{
	assert(dst);
	assert(r > 0);

	float inv_alpha = alpha / 255.0;

	int start_x = (int)(center_x - r);
	int start_y = (int)(center_y - r);
	int end_x = (int)(center_x + r + 0.5);
	int end_y = (int)(center_y + r + 0.5);

	if(start_x < 0) start_x=0;
	if(start_y < 0) start_y=0;
	if(start_x >= dst->width) start_x = dst->width-1;
	if(start_y >= dst->height) start_y = dst->height-1;
	if(end_x < 0) end_x=0;
	if(end_y < 0) end_y=0;
	if(end_x >= dst->width) end_x = dst->width-1;
	if(end_y >= dst->height) end_y = dst->height-1;

	float rr = r*r;

	int x,y;
	uint8_t pix;
	uint8_t* dst_pix;
	for(y=start_y; y<=end_y; y++){
		dst_pix = GetPixelAddress(dst, start_x, y);
		for(x=start_x; x<=end_x; x++){
			float frr = (float)(x-center_x+0.5)*(x-center_x+0.5)+(y-center_y+0.5)*(y-center_y+0.5);

			if(frr < rr){
				float fr = sqrt(frr);
				fr = 1.0 - fr/r;
				pix = (fr*fr) * (3.0-2.0*fr) * setnum*inv_alpha;
	
				if(pix > (*dst_pix)){
					(*dst_pix) = pix;
				}
			}

			dst_pix++;
		}
	}
}

////////////////////////////////////
/*!
	市松模様を書き込む
	@param[in,out] img 書き込み先 3or4 ch
	@param[in] color1 色1
	@parma[in] color2 色2
	@parma[in] block_size ブロックサイズ
*/
void CheckerImage(IplImage* img, const UCvPixel* color1, const UCvPixel* color2, int block_size)
{
	CheckerImageRect(img, color1, color2, block_size, NULL);
}

/////////////////////////////////////
/*!
	市松模様を書き込む
	@param[in,out] img 書き込み先 3or4 ch
	@param[in] color1 色1 rgb
	@param[in] color2 色2 rgb
	@param[in] block_size 模様のブロックサイズ 2^(block_size)の大きさになる
	@param[in] lprc 書き込み先範囲
*/
void CheckerImageRect(IplImage* img, const UCvPixel* color1, const UCvPixel* color2, int block_size, LPRECT lprc)
{
	assert( img->nChannels == 3 || img->nChannels == 4 );

	RECT rc;
	if(lprc){
		rc.left  = (lprc->left < lprc->right) ? lprc->left : lprc->right;
		rc.right = (lprc->left < lprc->right) ? lprc->right : lprc->left;
		rc.top   = (lprc->top < lprc->bottom) ? lprc->top : lprc->bottom;
		rc.bottom = (lprc->top < lprc->bottom) ? lprc->bottom : lprc->top;
	}
	else{
		rc.left = 0;
		rc.right = img->width;
		rc.top = 0;
		rc.bottom = img->height;
	}

	if (rc.right - rc.left == 0) return;
	if (rc.bottom - rc.top == 0) return;

	if(img->nChannels == 4){
		int m, n;
		int x, y;
		UCvPixel *pix;
		UCvPixel *line = GetPixelAddress32(img, rc.left, rc.top);
		for(y=rc.top; y<rc.bottom; y++){
			n = y >> block_size;
			pix = line;
			for(x=rc.left; x<rc.right; x++){
				m = x >> block_size;

				if((n&0x1) == (m&0x1)){
					pix->value = color1->value;
				}
				else{
					pix->value = color2->value;
				}
				pix++;
			}
			line = GetNextLineAddress32(img, line);
		}
	}
	else if(img->nChannels == 3){
		int m, n;
		int x, y;
		for(y=rc.top; y<rc.bottom; y++){
			n = y >> block_size;
			for(x=rc.left; x<rc.right; x++){
				m = x >> block_size;

				if((n&0x1) == (m&0x1)){
					SetPixelToBGR(img, x, y, color1);
				}
				else{
					SetPixelToBGR(img, x, y, color2);
				}
			}
		}
	}
}

void CheckerImageRect2(IplImage* img, const UCvPixel* color1, const UCvPixel* color2, int block_size, LPRECT lprc)
{
	assert( img->nChannels == 3 || img->nChannels == 4 );
	assert( block_size > 1 );

	RECT rc;
	if(lprc){
		rc.left  = (lprc->left < lprc->right) ? lprc->left : lprc->right;
		rc.right = (lprc->left < lprc->right) ? lprc->right : lprc->left;
		rc.top   = (lprc->top < lprc->bottom) ? lprc->top : lprc->bottom;
		rc.bottom = (lprc->top < lprc->bottom) ? lprc->bottom : lprc->top;
	}
	else{
		rc.left = 0;
		rc.right = img->width;
		rc.top = 0;
		rc.bottom = img->height;
	}

	if (rc.right - rc.left == 0) return;
	if (rc.bottom - rc.top == 0) return;

	if(img->nChannels == 4){
		int m, n;
		int x, y;
		UCvPixel *pix;
		UCvPixel *line = GetPixelAddress32(img, rc.left, rc.top);
		for(y = rc.top; y < rc.bottom; y++){
			n = y >> block_size;
			pix = line;

#ifdef _UOCV_USE_SSE_
			x = rc.left;
			for (; !IS_16BYTE_ALIGNMENT((int)pix); x++) {
				m = x >> block_size;
				if ((n&0x1) == (m&0x1)) {
					pix->value = color1->value;
				}
				else {
					pix->value = color2->value;
				}
				pix++;
			}

			//sse proc loop
			int last_loop = (rc.right - x) >> 2;
			for (int i = 0; i < last_loop; i++) {
				__m128i xm_pix;
				m = x >> block_size;
				if ((n & 0x1) == (m & 0x1)) {
					xm_pix = _mm_set_epi32(color1->value, color1->value, color1->value, color1->value);
				}
				else {
					xm_pix = _mm_set_epi32(color2->value, color2->value, color2->value, color2->value);
				}
				_mm_stream_si128((__m128i*)pix, xm_pix);
				pix += 4;
				x += 4;
			}

			for (; x < rc.right; x++) {
				m = x >> block_size;
				if ((n&0x1) == (m&0x1)) {
					pix->value = color1->value;
				}
				else {
					pix->value = color2->value;
				}
				pix++;
			}
#else
			for(x = rc.left; x < rc.right; x++){
				m = x >> block_size;
				if ((n&0x1) == (m&0x1)) {
					pix->value = color1->value;
				}
				else {
					pix->value = color2->value;
				}
				pix++;
			}
#endif

			line = GetNextLineAddress32(img, line);
		}
	}
	else if (img->nChannels == 3) {
		int m, n;
		int x, y;
		for (y = rc.top; y < rc.bottom; y++){
			n = y >> block_size;
			for(x = rc.left; x < rc.right; x++){
				m = x >> block_size;

				if((n&0x1) == (m&0x1)){
					SetPixelToBGR(img, x, y, color1);
				}
				else{
					SetPixelToBGR(img, x, y, color2);
				}
			}
		}
	}
}

/////////////////////////////////////
/*!
	市松模様を書き込む
	@param[in,out] img 書き込み先 3or4 ch
	@param[in] color1 色1 rgb
	@param[in] color2 色2 rgb
	@param[in] block_size 模様のブロックサイズ
	@param[in] lprc 書き込み先範囲
*/
void CheckerImageExtRect(IplImageExt* img, const UCvPixel* color1, const UCvPixel* color2, int block_size, LPRECT lprc)
{
	assert( img->GetImgNChannel() == 3 || img->GetImgNChannel() == 4 );

	RECT rc;
	if(lprc){
		rc.left  = (lprc->left < lprc->right) ? lprc->left : lprc->right;
		rc.right = (lprc->left < lprc->right) ? lprc->right : lprc->left;
		rc.top   = (lprc->top < lprc->bottom) ? lprc->top : lprc->bottom;
		rc.bottom = (lprc->top < lprc->bottom) ? lprc->bottom : lprc->top;
	}
	else{
		rc.left = 0;
		CvSize size = img->GetSize();
		rc.right = size.width-1;
		rc.top = 0;
		rc.bottom = size.height-1;
	}

	int m, n;
	int x, y;
	for(y=rc.top; y<=rc.bottom; y++){
		n = y/block_size;
		for(x=rc.left; x<=rc.right; x++){
			m = x/block_size;

			if((n%2) == (m%2)){
				if(img->GetImgNChannel() == 3){
					img->SetPixel(x, y, color1);
				}
				else if(img->GetImgNChannel() == 4){
					img->SetPixel(x, y, color1);
				}
			}
			else{
				if(img->GetImgNChannel() == 3){
					img->SetPixel(x, y, color2);
				}
				else if(img->GetImgNChannel() == 4){
					img->SetPixel(x, y, color2);
				}
			}
		}
	}
}


/////////////////////////////////////
/*!
	画像に太さline_weight、色colorの枠を書き込む
	@param[in,out] img 書き込み先3or4 ch
	@param[in] line_weight 枠の太さ
	@param[in] color 枠の色
*/
void AddRectToImage(IplImage* img, int line_weight, const UCvPixel* color)
{
	assert( img->nChannels == 3 || img->nChannels == 4 );

	int i;
	int x, y;
	if (img->nChannels == 3) {
		for(i=1; i<=line_weight; i++){
			//横方向
			for(x=0; x < img->width; x++){
				SetPixelToBGR(img, x, i-1, color);
				SetPixelToBGR(img, x, img->height - i, color);
			}

			//縦方向
			for(y=0; y<img->height; y++){
				SetPixelToBGR(img, i-1, y, color);
				SetPixelToBGR(img, img->width - i, y, color);
			}
		}
	} else if (img->nChannels == 4) {
		for(i=1; i<=line_weight; i++){
			//横方向
			for(x=0; x < img->width; x++){
				SetPixelToBGRA(img, x, i-1, color);
				SetPixelToBGRA(img, x, img->height - i, color);
			}

			//縦方向
			for(y=0; y<img->height; y++){
				SetPixelToBGRA(img, i-1, y, color);
				SetPixelToBGRA(img, img->width - i, y, color);
			}
		}
	}
}