#include "stdafx.h"

#include "UtilOpenCVTransform.h"
#include "UtilOpenCVPixel.h"
#include "IplImageExt.h"

#include <float.h>

_EXPORT bool resolve2Equ(double a, double b, double c, double *x1, double* x2)
{
	double dd;

	if(a==0.0){
		if(c==0.0) return false;
		*x1 = -b/c;
		*x2 = -b/c;
		return true;
	}
	dd = b*b - 4.0*a*c;
	*x1 = 0.0;
	*x2 = 0.0;
	if(dd < 0.0){
		return false;
	}
	*x1 = (-b + sqrt(dd)) / (2.0*a);
	*x2 = (-b - sqrt(dd)) / (2.0*a);
	return true;
}

//比率を求める ax, cx, bx, dx, dy, ay, cy, by
_EXPORT bool resolveEqu(double ax, double ay, double bx, double by, double cx, double cy, double dx,
			   double dy, double *s1, double* t1, double* s2, double* t2)
{
	double p1, p2, p3;
	double x1, x2;
	double tm;

	p1 = (double)(ax*by - ay*bx);
	p2 = (double)(dx*ay - bx*cy + cx*by - ax*dy);
	p3 = (double)(dx*cy - cx*dy);
	if(p1 == 0.0){
		*t1 = (p2 != 0.0) ? -p3/p2 : 0.0;
		*s1 = (((ax*(*s1))+cx) != 0.0) ? (dx - (bx*(*s1)))/((ax*(*s1)) + cx) : 0.0;
		//*s1 = dx/cx;
		//*t1 = (cx*dy - dx*cy) / (cx*by + dx*ay);
		*t2 = -1000;
		*s2 = -1000;
		return true;
	}
	if(!resolve2Equ(p1, p2, p3, t1, t2)){
		*s1 = -1000;
		*s2 = -1000;
		return true;
	}
	tm = (ax * (*t1) + cx);
	if(tm)
		*s1 = (dx - bx * (*t1)) / tm;
	else
		*s1 = -1000.0;

	tm = (ax * (*t2) + cx);
	if(tm)
		*s2 = (dx - bx * (*t2)) / tm;
	else
		*s2 = -1000.0;

	return true;
}

/////////////////////////////////////
/*!
	比率による自由変形
	@param[out] dst 出力画像
	@param[in] src 入力画像
	@param[in] d_pts 出力先変形点(左上の点から時計回りの順に4点を指定する)
*/
_EXPORT bool FreeDeformationByRatio(IplImage* dst, const IplImage* src, const CvPoint2D64f* d_pts)
{
	assert(dst->nChannels == src->nChannels);

	if (dst->height <= 0) return true;
	if (dst->width <= 0) return true;

	int i;
	int x,y;
	int dst_w=0;
	int dst_h=0;
	UCvPixel pixel;
	int rx1, ry1, rx2, ry2;
	double Ax, Bx, Cx, Dx;
	double Ay, By, Cy, Dy;
	double ax,ay,bx,by,cx,cy,dx,dy;

	int min_x = d_pts[0].x;
	int min_y = d_pts[0].y;
	int max_x = d_pts[0].x;
	int max_y = d_pts[0].y;
	for(i=1; i<4; i++){
		if(min_x > d_pts[i].x){
			min_x = d_pts[i].x;
		}
		if(min_y > d_pts[i].y){
			min_y = d_pts[i].y;
		}
		if(max_x < d_pts[i].x){
			max_x = d_pts[i].x;
		}
		if(max_y < d_pts[i].y){
			max_y = d_pts[i].y;
		}
	}
	dst_w = max_x - min_x;
	dst_h = max_y - min_y;

	rx1=0; ry1=0;
	rx2=src->width-1; ry2=src->height-1;

	Ax = (double)d_pts[0].x + 1.0;
	Ay = (double)d_pts[0].y + 1.0;

	Bx = (double)d_pts[1].x - 1.0;
	By = (double)d_pts[1].y - 1.0;

	Cx = (double)d_pts[2].x - 1.0;
	Cy = (double)d_pts[2].y + 1.0;

	Dx = (double)d_pts[3].x + 1.0;
	Dy = (double)d_pts[3].y - 1.0;

	ax = (double)(Cx - Dx - Bx + Ax);
	ay = (double)(Cy - Dy - By + Ay);
	bx = (double)(Dx - Ax);
	by = (double)(Dy - Ay);
	cx = (double)(Bx - Ax);
	cy = (double)(By - Ay);

	if(dst->nChannels == 4){
		UCvPixel32*  dst_line = GetPixelAddress32(dst, 0, 0);
		for(y=0; y<dst_h; y++){
			UCvPixel32* dst_pix = dst_line;
			for(x=0; x<dst_w; x++){
				double ss1, tt1, ss2, tt2;
				double ttx, tty;

				dx = (double)(x - Ax);
				dy = (double)(y - Ay);
				//比率を求める
				if(!resolveEqu(ax, ay, bx, by, cx, cy, dx, dy, &ss1, &tt1, &ss2, &tt2)){
					dst_pix++;
					return false;
				}
				//比率から変形元の座標を求める
				//この際 0<=s<=1 0<=t<=1 となる解を用いる
				if(ss1 >= 0.0 && tt1 >= 0.0 && ss1 <= 1.0 && tt1 <= 1.0){
					ttx = ss1 * (rx2 - rx1) + rx1;
					tty = tt1 * (ry2 - ry1) + ry1;
				}
				else if (ss2 >= 0.0 && tt2 >= 0.0 && ss2 <= 1.0 && tt2 <= 1.0){
					ttx = ss2 * (rx2 - rx1) + rx1;
					tty = tt2 * (ry2 - ry1) + ry1;
				}
				else{
					//範囲内に解が無ければ
					(*dst_pix++).value = 0;
					continue;
				}

				GetPixelFromBGRA(src, ttx + 0.5, tty + 0.5, &pixel);
				(*dst_pix++).value = pixel.value;
			}
			dst_line = GetNextLineAddress32(dst, dst_line);
		}
	}
	else if(dst->nChannels == 3){
		assert(0);
	}
	else if(dst->nChannels == 1){
		uint8_t*  dst_line = GetPixelAddress(dst, 0, 0);
		for(y=0; y<dst_h; y++){
			uint8_t* dst_pix = dst_line;
			for(x=0; x<dst_w; x++){
				double ss1, tt1, ss2, tt2;
				double ttx, tty;

				dx = (double)(x - Ax);
				dy = (double)(y - Ay);
				//比率を求める
				if(!resolveEqu(ax, ay, bx, by, cx, cy, dx, dy, &ss1, &tt1, &ss2, &tt2)){
					dst_pix++;
					return false;
				}
				//比率から変形元の座標を求める
				//この際 0<=s<=1 0<=t<=1 となる解を用いる
				if(ss1 >= 0.0 && tt1 >= 0.0 && ss1 <= 1.0 && tt1 <= 1.0){
					ttx = ss1 * (rx2 - rx1) + rx1;
					tty = tt1 * (ry2 - ry1) + ry1;
				}
				else if (ss2 >= 0.0 && tt2 >= 0.0 && ss2 <= 1.0 && tt2 <= 1.0){
					ttx = ss2 * (rx2 - rx1) + rx1;
					tty = tt2 * (ry2 - ry1) + ry1;
				}
				else{
					//範囲内に解が無ければ
					(*dst_pix++) = 0;
					continue;
				}

				(*dst_pix++) = GetMaskDataPos(src, ttx + 0.5, tty + 0.5);
			}
			dst_line = GetNextLineAddress(dst, dst_line);
		}
	}

	return true;
}

/////////////////////////////////////
/*!
	アフィン変換
	@param[out] dst 出力先画像
	@param[in] src 入力画像
	@param[in] matrix 変換行列
	@param[in] fillval 
*/
_EXPORT void WarpAffine(
	IplImage* dst,
	const IplImageExt* src,
	const CvMat* matrix, 
	int flag,
	const UCvPixel* fillval)
{
	WarpAffine_NN(dst, src, matrix, fillval);
}

_EXPORT void WarpAffine_NN(
	IplImage* dst,
	const IplImageExt* src,
	const CvMat* matrix,
	const UCvPixel* fillval)
{
	int x,y;
	int src_x,src_y;
	UCvPixel pixel;
	CvSize src_size = src->GetSize();

	CvMat* InvCtr = cvCreateMat(2, 3, CV_32F);
	CvMat* p = cvCreateMat(3, 1, CV_32F);
	CvMat* xp = cvCreateMat(2, 1, CV_32F);


	InvertAffineMat(matrix, InvCtr);

	for(y=0; y<dst->height; y++){
		for(x=0; x<dst->width; x++){
			cvmSet(p, 0, 0, (double)x);
			cvmSet(p, 1, 0, (double)y);
			cvmSet(p, 2, 0, 1.0);
			cvMatMul(InvCtr, p, xp);

			src_x = cvmGet(xp, 0, 0);
			src_y = cvmGet(xp, 1, 0);

			if((0 <= src_x && src_x < src_size.width) &&
				(0 <= src_y && src_y < src_size.height)){
					src->GetPixel(src_x, src_y, &pixel);
					SetPixelToBGRA(dst, x, y, &pixel);
			}
			else{
				SetPixelToBGRA(dst, x, y, 
					fillval);
			}
		}
	}

	cvReleaseMat(&InvCtr);
	cvReleaseMat(&p);
	cvReleaseMat(&xp);
}

//_EXPORT void WarpAffine_Bilinear(
//	IplImage* dst,
//	const IplImageExt* src,
//	const CvMat* matrix,
//	CvScalar fillval)
//{
//	int x,y;
//	double src_x,src_y;
//	for(y=0; y<dst->height; y++){
//		for(x=0; x<dst->width; x++){
//		src_x;
//		src_y;
//
//　　　　// 求める座標から小数点を切り捨てた座標②
//
//　　　　int isrcX = static_cast<int>(dsrcX);
//　　　　int isrcY = static_cast<int>(dsrcY);
//
//　　　　// ①と②の距離
//
//　　　　double dx = dsrcX - isrcX;
//　　　　double dy = dsrcY - isrcY;
//
//　　　　// 元画像の座標
//　　　　int srcX = static_cast<int>(dsrcX + 0.5);
//　　　　int srcY = static_cast<int>(dsrcY + 0.5);
//
//　　　　if((0 <= isrcX) && (isrcX < nSrcWidth) && (0 <= isrcY) && (isrcY < nSrcHeight))
//　　　　{
//
//　　　　　　// 直近4座標の色データを取得する
//　　　　　　RGBTRIPLE rtLB, rtRB, rtLT, rtRT;
//　　　　　　memcpy(&rtLB, &m_pSrcData[isrcX*3 + isrcY*srcLength],3);
//　　　　　　memcpy(&rtRB, &m_pSrcData[(isrcX+1)*3 + isrcY*srcLength],3);
//　　　　　　memcpy(&rtLT, &m_pSrcData[isrcX*3 + (isrcY+1)*srcLength],3);
//　　　　　　memcpy(&rtRT, &m_pSrcData[(isrcX+1)*3 + (isrcY+1)*srcLength],3);
//
//
//　　　　　　pDstData[dstX * 3 + dstY * dstLength] =
//　　　　　　　　(BYTE)((1-dy) * ((1-dx) * rtLB.rgbtBlue + dx * rtRB.rgbtBlue) +
//
//　　　　　　　　　　　　　　　　　　　dy * ((1-dx) * rtLT.rgbtBlue + dx * rtRT.rgbtBlue) + 0.5);
//　　　　　　pDstData[dstX * 3 + dstY * dstLength + 1] =
//　　　　　　　　(BYTE)((1-dy) * ((1-dx) * rtLB.rgbtGreen + dx * rtRB.rgbtGreen) +
//
//　　　　　　　　　　　　　　　　　　　dy * ((1-dx) * rtLT.rgbtGreen + dx * rtRT.rgbtGreen) + 0.5);
//　　　　　　pDstData[dstX * 3 + dstY * dstLength + 2] =
//　　　　　　　　(BYTE)((1-dy) * ((1-dx) * rtLB.rgbtRed + dx * rtRB.rgbtRed) +
//
//　　　　　　　　　　　　　　　　　　　dy * ((1-dx) * rtLT.rgbtRed + dx * rtRT.rgbtRed) + 0.5);
//　　　　}
//　　}
//}
//}