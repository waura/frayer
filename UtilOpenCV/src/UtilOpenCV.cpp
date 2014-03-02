#include "stdafx.h"

#include "UtilOpenCV.h"

static void FillMask(IplImage *image, uint8_t setnum, int x, int y)
{
	if(( 0 <= x && x <= image->width -1 )&&
		(0 <= y && y <= image->height -1 ) )
	{

		uint8_t cl = GetMaskDataPos(image, x, y);
		if(cl == 255){
			::MessageBox(NULL, "", "", MB_OK);
		}
		SetMaskDataPos(image, setnum, x, y);

		//上
		if(GetMaskDataPos(image, x, y-1) == cl){
			FillMask(image, setnum, x, y-1);
		}

		//右
		else if(GetMaskDataPos(image, x+1, y) == cl){
			FillMask(image, setnum, x + 1, y);
		}

		//下
		else if(GetMaskDataPos(image, x, y-1) == cl){
			FillMask(image, setnum, x, y-1);
		}

		//左
		else if(GetMaskDataPos(image, x-1, y) == cl){
			FillMask(image, setnum,  x-1, y);
		}
	}
}

_EXPORT bool isEqualIplImage(const IplImage* img1, const IplImage* img2)
{
	if (img1->height != img2->height)
		return false;
	if (img1->width != img2->width)
		return false;
	if (img1->nChannels != img2->nChannels)
		return false;
	if (img1->imageSize != img2->imageSize)
		return false;
	return (memcmp(img1->imageData, img2->imageData, img1->imageSize) == 0);
}

_EXPORT bool isEqualIplImageExt(const IplImageExt* img1, const IplImageExt* img2)
{
	CvSize size1 = img1->GetSize();
	CvSize size2 = img2->GetSize();

	if (size1.height != size2.height)
		return false;
	if (size1.width != size2.width)
		return false;
	if (img1->GetImgNChannel() != img2->GetImgNChannel())
		return false;

	IplImage* _img1 = cvCreateImage(size1, img1->GetImgDepth(), img1->GetImgNChannel());
	IplImage* _img2 = cvCreateImage(size2, img2->GetImgDepth(), img2->GetImgNChannel());

	img1->GetRangeIplImage(_img1, 0, 0, size1.width, size1.height, 0, 0);
	img2->GetRangeIplImage(_img2, 0, 0, size2.width, size2.height, 0, 0);

	bool ret = isEqualIplImage(_img1, _img2);

	cvReleaseImage(&_img1);
	cvReleaseImage(&_img2);

	return ret;
}

_EXPORT bool isFillZeroMask(const IplImage *mask)
{
	uint8_t* pmask;
	for(int y=0; y<mask->height; y++){
		pmask = GetPixelAddress(mask, 0, y);
		for(int x=0; x<mask->width; x++){
			if((*pmask) != 0){
				return false;
			}
			pmask++;
		}
	}

	return true;
}

_EXPORT bool isNotFillZeroMask(const IplImage *mask)
{
	uint8_t* pmask;
	for (int y=0; y<mask->height; y++) {
		pmask = GetPixelAddress(mask, 0, y);
		for (int x=0; x<mask->width; x++) {
			if ((*pmask) != 0) {
				return true;
			}
		}
	}
	return false;
}

_EXPORT bool isFillMask(const IplImage* img, uint8_t pix)
{
	assert(img->nChannels == 1);

	int x,y;
	uint8_t* img_pix;
	for (y = 0; y < img->height; y++){
		img_pix = GetPixelAddress(img, 0, y);
		for (x = 0; x < img->width; x++){
			if ((*img_pix) != pix) {
				return false;
			}
			img_pix++;
		}
	}
	return true;
}

_EXPORT bool isFillColor(const IplImage* img, const UCvPixel* pix)
{
	assert(img->nChannels == 4);

	int x,y;
	UCvPixel* img_pix;
	for (y = 0; y < img->height; y++){
		img_pix = GetPixelAddress32(img, 0, y);
		for (x = 0; x < img->width; x++){
			if (img_pix->value != pix->value) {
				return false;
			}
			img_pix++;
		}
	}
	return true;
}

_EXPORT void FillCloseArea(IplImage *mask, uint8_t setnum)
{
	CvLine2D64f_Vec line_Vec;

	for (int y=0; y<mask->height; y++) {
		bool isWrite = false;
		bool isData = false;
		int count = 0;
		bool isBeforData = false;
		CvLine2D64f line;
		for (int x=0; x<mask->width; x++) {

			if (0 != GetMaskDataPos(mask, x, y)) {
				isData = true;
			}
			else {
				isData = false;
			}

			if (isData == true && isBeforData == false) {
				count++;
				if (count == 1) {
					line.start.x = x;
					line.start.y = y;
				}
				else if (count == 2) {
					line.end.x = x;
					line.end.y = y;
					line_Vec.push_back(line);
					count = 0;
				}
			}


			isBeforData = isData;
		}
	}

	CvLine2D64f_Vec::iterator itr = line_Vec.begin();
	for(; itr != line_Vec.end(); itr++){
		LineMask(mask, setnum, &((*itr).start), &((*itr).end));
	}
}

_EXPORT double GetMaxAbsDiff(const IplImage* img1, const IplImage* img2)
{
	assert(img1->width == img2->width);
	assert(img1->height == img2->height);
	assert(img1->nChannels == img2->nChannels);

	if (img1->nChannels == 4) {
		double max_abs_diff = 0.0;
		int x,y;
		UCvPixel* pix1;
		UCvPixel* pix2;
		UCvPixel* line1;
		UCvPixel* line2;
		line1 = GetPixelAddress32(img1, 0, 0);
		line2 = GetPixelAddress32(img2, 0, 0);
		for (y = 0; y < img1->height; y++) {
			pix1 = line1;
			pix2 = line2;
			for (x = 0; x < img1->width; x++) {
				int dif_b = pix1->b - pix2->b;
				int dif_g = pix1->g - pix2->g;
				int dif_r = pix1->r - pix2->r;
				int dif_a = pix1->a - pix2->a;

				dif_b *= ((double)max(pix1->a, pix2->a)/255.0);
				dif_g *= ((double)max(pix1->a, pix2->a)/255.0);
				dif_r *= ((double)max(pix1->a, pix2->a)/255.0);

				double abs_diff = sqrt((double)(dif_b*dif_b + dif_g*dif_g + dif_r*dif_r + dif_a*dif_a));
				if (abs_diff > max_abs_diff) {
					max_abs_diff = abs_diff;
				}

				pix1++;
				pix2++;
			}
			line1 = GetNextLineAddress32(img1, line1);
			line2 = GetNextLineAddress32(img2, line2);
		}
		return max_abs_diff;
	}
	else if (img1->nChannels == 3) {
		double max_abs_diff = 0.0;
		int x,y;
		UCvBGR* pix1;
		UCvBGR* pix2;
		UCvBGR* line1;
		UCvBGR* line2;
		line1 = GetPixelAddress24(img1, 0, 0);
		line2 = GetPixelAddress24(img2, 0, 0);
		for (y = 0; y < img1->height; y++) {
			pix1 = line1;
			pix2 = line2;
			for (x = 0; x < img1->width; x++) {
				int dif_b = pix1->b - pix2->b;
				int dif_g = pix1->g - pix2->g;
				int dif_r = pix1->r - pix2->r;

				double abs_diff = sqrt((double)(dif_b*dif_b + dif_g*dif_g + dif_r*dif_r));
				if (abs_diff > max_abs_diff) {
					max_abs_diff = abs_diff;
				}

				pix1++;
				pix2++;
			}
			line1 = GetNextLineAddress24(img1, line1);
			line2 = GetNextLineAddress24(img2, line2);
		}
		return max_abs_diff;
	}
	else if (img1->nChannels == 1) {
		double max_abs_diff = 0.0;
		int x,y;
		uint8_t* pix1;
		uint8_t* pix2;
		uint8_t* line1;
		uint8_t* line2;
		line1 = GetPixelAddress(img1, 0, 0);
		line2 = GetPixelAddress(img2, 0, 0);
		for (y = 0; y < img1->height; y++) {
			pix1 = line1;
			pix2 = line2;
			for (x = 0; x < img1->width; x++) {
				int dif = (*pix1) - (*pix2);

				double abs_diff = sqrt((double)dif*dif);
				if (abs_diff > max_abs_diff) {
					max_abs_diff = abs_diff;
				}

				pix1++;
				pix2++;
			}
			line1 = GetNextLineAddress(img1, line1);
			line2 = GetNextLineAddress(img2, line2);
		}
		return max_abs_diff;
	}
	else {
		assert(0);
	}
}

_EXPORT double GetMaxAbsDiffExt(const IplImageExt* img1, const IplImageExt* img2)
{
	CvSize size1 = img1->GetSize();
	CvSize size2 = img2->GetSize();
	IplImage* _img1 = cvCreateImage(size1, img1->GetImgDepth(), img1->GetImgNChannel());
	IplImage* _img2 = cvCreateImage(size2, img2->GetImgDepth(), img2->GetImgNChannel());

	img1->GetRangeIplImage(_img1, 0, 0, size1.width, size1.height, 0, 0);
	img2->GetRangeIplImage(_img2, 0, 0, size2.width, size2.height, 0, 0);

	double ret = GetMaxAbsDiff(_img1, _img2);

	cvReleaseImage(&_img1);
	cvReleaseImage(&_img2);
	return ret;
}

//////////////////////////////////////////
/*!
	srcをdstに転送
	@param[out] dst 出力先
	@param[in] posX dst転送開始位置
	@param[in] posY dst転送開始位置
	@param[in] width 転送する画像幅
	@param[in] height 転送画像高さ
	@param[in] src 転送元画像
	@param[in] startX 転送元画像開始位置
	@param[in] startY 転送元画像開始位置
*/
_EXPORT void BltMaskToMask(IplImage* dst, int posX, int posY, int width, int height, 
				   const IplImage* src, int startX, int startY)
{
	int x,y;
	for(y=0; y<height; y++){
		for(x=0; x<width; x++){
			uint8_t data = GetMaskDataPos(src, x + startX, y + startY);
			SetMaskDataPos(dst, posX + x, posY + y, data);
		}
	}
}

///////////////////////////////////////////////
/*!
	maskの範囲lprcをdst_maskにコピーする
	@param[in] mask マスク
	@param[in] lprc 範囲
	@param[out] dst_mask 出力結果
*/
_EXPORT void CutMask(const IplImage *mask, const LPRECT lprc, IplImage *dst_mask)
{
	int x,y;
	for(y=lprc->top; y<= lprc->bottom; y++){
		for(x=lprc->left; x<= lprc->right; x++){
			int data = GetMaskDataPos(mask, x, y);
			SetMaskDataPos(dst_mask, x - lprc->left, y - lprc->top, data);
		}
	}
}

//////////////////////////////////
/*!
	マスクの可視部分の範囲を返す
	@param[in] mask マスク
	@param[out] lprc 範囲
*/
_EXPORT void ucvGetMaskRect(const IplImage *mask, LPRECT lprc)
{
	if(!mask) return;

	//判定のための初期値
	lprc->left = 0;
	lprc->top = 0;
	lprc->bottom = mask->height;
	lprc->right = mask->width;
	
	uint8_t* pmask;

	//上から下へ走査
	for(int y=0; y<mask->height; y++){
		pmask = GetPixelAddress(mask, 0, y);
		for(int x=0; x<mask->width; x++){
			if((*pmask) != 0){
				lprc->top = y;
				goto uptodown;
			}
			pmask++;
		}
	}
uptodown:
	//下から上へ走査
	for(int y=mask->height-1; y>=0; y--){
		pmask = GetPixelAddress(mask, 0, y);
		for(int x=0; x<mask->width; x++){
			if((*pmask) != 0){
				lprc->bottom = y+1;
				goto lefttoright;
			}
			pmask++;
		}
	}
	lprc->bottom = 0;
lefttoright:
	//左から右へ走査
	for(int x=0; x<mask->width; x++){
		for(int y=0; y<mask->height; y++){
			int data = GetMaskDataPos(mask, x, y);
			if(data != 0){
				lprc->left = x;
				goto righttoleft;
			}
		}
	}
righttoleft:
	//右から左へ走査
	for(int x=mask->width-1; x>=0; x--){
		for(int y=0; y<mask->height; y++){
			int data = GetMaskDataPos(mask, x, y);
			if(data != 0){
				lprc->right = x+1;
				return;
			}
		}
	}
	lprc->right = 0;
}

_EXPORT void ucvCvtColor(const IplImage* src, IplImage* dst, int code)
{
	if(code == CV_GRAY2BGRA){
		assert(src->nChannels == 1);
		assert(dst->nChannels == 4);
		assert(dst->depth == IPL_DEPTH_8U);
		assert(src->depth == IPL_DEPTH_8U);
		assert(src->width == dst->width);
		assert(src->height == dst->height);

		int x,y;
		uint8_t* psrc;
		UCvPixel32* pdst;
		for(y=0; y<src->height; y++){
			psrc = GetPixelAddress(src, 0, y);
			pdst = GetPixelAddress32(dst, 0, y);
			for(x=0; x<src->width; x++){
				pdst->b = (*psrc);
				pdst->g = (*psrc);
				pdst->r = (*psrc);
				pdst->a = 255;

				pdst++;
				psrc++;
			}
		}
	}
	else if(code == CV_BGR2BGRA){
		assert(src->nChannels == 3);
		assert(dst->nChannels == 4);
		assert(dst->depth == IPL_DEPTH_8U);
		assert(src->depth == IPL_DEPTH_8U);
		assert(src->width == dst->width);
		assert(src->height == dst->height);

		int x,y;
		UCvBGR24* psrc;
		UCvPixel32* pdst;
		for(y=0; y<src->height; y++){
			psrc = GetBGRAddress(src, 0, y);
			pdst = GetPixelAddress32(dst, 0, y);
			for(x=0; x<src->width; x++){
				pdst->b = psrc->b;
				pdst->g = psrc->g;
				pdst->r = psrc->r;
				pdst->a = 255;

				pdst++;
				psrc++;
			}
		}
	}
	else if(code == CV_BGRA2BGR){
		assert(src->nChannels == 4);
		assert(dst->nChannels == 3);
		assert(dst->depth == IPL_DEPTH_8U);
		assert(src->depth == IPL_DEPTH_8U);
		assert(src->width == dst->width);
		assert(src->height == dst->height);

		int x,y;
		UCvPixel32* psrc;
		UCvBGR24* pdst;
		for(y=0; y<src->height; y++){
			psrc = GetPixelAddress32(src, 0, y);
			pdst = GetBGRAddress(dst, 0, y);
			for(x=0; x<src->width; x++){
				pdst->b = psrc->b;
				pdst->g = psrc->g;
				pdst->r = psrc->r;

				pdst++;
				psrc++;
			}
		}
	}
}