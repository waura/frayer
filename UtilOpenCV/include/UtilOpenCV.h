#ifndef _UTILOPENC_H_
#define _UTILOPENC_H_

#include "LibDllHeader.h"
#include "OpenCV2VC.h"
#include "IplImageExt.h"
#include "UtilOpenCVMat.h"
#include "UtilOpenCVFigure.h"
#include "UtilOpenCVFilter.h"
#include "UtilOpenCVLine.h"
#include "UtilOpenCVTransform.h"
#include "UtilOpenCVIO.h"
#include "UtilOpenCVInterpolate.h"
#include "UtilOpenCVEffect.h"
#include "UtilOpenCVColor.h"


///////////////////////////////////////
/*!
	img1とimg2を比較して同じピクセルデータならtrue、そうでなければfalseを返す
	@param[in] img1 比較する画像1
	@param[in] img2 比較する画像2
	@return img1とimg2を比較して同じピクセルデータならtrue、そうでなければfalseを返す
*/
_EXPORT bool isEqualIplImage(const IplImage* img1, const IplImage* img2);

///////////////////////////////////////
/*!
	img1とimg2を比較して同じピクセルデータならtrue、そうでなければfalseを返す
	@param[in] img1 比較する画像1
	@param[in] img2 比較する画像2
	@return img1とimg2を比較して同じピクセルデータならtrue、そうでなければfalseを返す
*/
_EXPORT bool isEqualIplImageExt(const IplImageExt* img1, const IplImageExt* img2);

///////////////////////////////////////
/*!
	マスクがすべて０で満たされていたらtrueを返す
	@param[in] mask 調べるマスク
	@return マスクの要素の値がすべて０ならtrue そうでなければfalseを返す。
*/
_EXPORT bool isFillZeroMask(const IplImage* mask);

////////////////////////////////////////
/*!
	マスクがすべて０で満たされていなかったらtrueを返す
	@param[in] mask 調べるマスク
	@reutn マスクの要素に０でない値があったらtrue そうでなければfalseを返す。
*/
_EXPORT bool isNotFillZeroMask(const IplImage* mask);

////////////////////////////////////////
/*!
	imgの画素が全てpixと等しいならtrueを返す、そうでなければfalseを返す。
	@param[in] img 調べる画像
	@param[in] pix 調べる値
	@reutn
*/
_EXPORT bool isFillMask(const IplImage* img, uint8_t pix);

////////////////////////////////////////
/*!
	imgの画素が全てpixと等しいならtrueを返す、そうでなければfalseを返す。
	@param[in] img 調べる画像
	@param[in] pix 調べる値
	@reutn
*/
_EXPORT bool isFillColor(const IplImage* img, const UCvPixel* pix);

////////////////////////////////////////
/*!
	0でない値で囲まれている部分をsetnumで埋める
	@param[in,out] mask 調べるマスク
	@param[in] setnum 埋める値
*/
_EXPORT void FillCloseArea(IplImage* mask, uint8_t setnum);

//////////////////////////////////////////
/*!
*/
_EXPORT double GetMaxAbsDiff(const IplImage* img1, const IplImage* img2);
_EXPORT double GetMaxAbsDiffExt(const IplImageExt* img1, const IplImageExt* img2);

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
				   const IplImage* src, int startX, int startY);

///////////////////////////////////////////////
/*!
	maskの範囲lprcをdst_maskにコピーする
	@param[in] mask マスク
	@param[in] lprc 範囲
	@param[out] dst_mask 出力結果
*/
_EXPORT void CutMask(const IplImage* mask, const LPRECT lprc, IplImage* dst_mask);


//////////////////////////////////
/*!
	マスクの可視部分の範囲を返す
	@param[in] mask マスク
	@param[out] lprc 範囲
*/
_EXPORT void ucvGetMaskRect(const IplImage* mask, LPRECT lprc);

///////////////////////////////////////////
/*!
	(x,y)がrcの中に入っているかどうか
	境界を含む
	@param[in] x
	@param[in] y
	@param[in] rc
*/
_EXPORT inline bool isInRect(int x, int y, const LPRECT rc)
{
	if((rc->left <= x && x < rc->right) &&
		(rc->top <= y && y < rc->bottom)){
			return true;
	}
	return false;
}

///////////////////////////////////////////
/*!
	rc1がrc2の中に入っているかどうか
	境界を含む
	@param[in] rc1
	@param[in] rc2
*/
_EXPORT inline bool isRectInRect(const LPRECT rc1, const LPRECT rc2)
{
	if(isInRect(rc1->top, rc1->left, rc2) &&
		isInRect(rc1->top, rc1->right, rc2) &&
		isInRect(rc1->bottom, rc1->right, rc2) &&
		isInRect(rc1->bottom, rc1->left, rc2)){
			return true;
	}
	return false;
}

////////////////////////////////////////
/*!
	範囲の論理和
	src1とsrc2を含む最小の範囲destを返す
	@param[in] src1
	@param[in] src2
	@param[in] dest
*/
_EXPORT inline void OrRect(const LPRECT src1, const LPRECT src2, LPRECT dest)
{
	dest->top = (src1->top < src2->top) ? src1->top : src2->top;
	dest->left = (src1->left < src2->left) ? src1->left : src2->left;
	
	dest->bottom = (src1->bottom > src2->bottom) ? src1->bottom : src2->bottom;
	dest->right = (src1->right > src2->right) ? src1->right : src2->right;
}

////////////////////////////////////////
/*!
	範囲の論理積
	src1とsrc2の重なっている部分の最大の範囲destを返す
	重なりが無ければ範囲を(0,0)(0,0)にして返す。
	@param[in] src1
	@param[in] src2
	@param[in] dest
*/
_EXPORT inline void AndRect(const LPRECT src1, const LPRECT src2, LPRECT dest)
{
	dest->top = (src1->top > src2->top) ? src1->top : src2->top;
	dest->left = (src1->left > src2->left) ? src1->left : src2->left;
	
	dest->bottom = (src1->bottom < src2->bottom) ? src1->bottom : src2->bottom;
	dest->right = (src1->right < src2->right) ? src1->right : src2->right;

	if((dest->top > dest->bottom) || (dest->left > dest->right)){
		dest->top = dest->bottom = dest->left = dest->right = 0;
	}
}

////////////////////////////////////////
/*!
	範囲の引き算
	dest = src1 - src2
	@param[in] src1
	@param[in] src2
	@param[in] dest
*/
_EXPORT inline void SubRect(const LPRECT src1, const LPRECT src2, LPRECT dest)
{
	if(isRectInRect(src1, src2)){
		dest->top = dest->top = 0;
		dest->left = dest->right = 0;
		return;
	}

	(*dest) = (*src1);
	if(isInRect(src1->left, src1->top, src2) && isInRect(src1->right, src1->top, src2)){
		dest->top = src2->bottom;
	}
	if(isInRect(src1->right, src1->top, src2) && isInRect(src1->right, src1->bottom, src2)){
		dest->right = src2->left;
	}
	if(isInRect(src1->right, src1->bottom, src2) && isInRect(src1->left, src1->bottom, src2)){
		dest->bottom = src2->top;
	}
	if(isInRect(src1->left, src1->bottom, src2) && isInRect(src1->left, src1->top, src2)){
		dest->left = src2->right;
	}
}

/////////////////////////////////////////////
_EXPORT void ucvCvtColor(const IplImage* src, IplImage* dst, int code);

#endif // _UTILOPENCV_H_