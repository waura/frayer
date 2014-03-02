#ifndef _IPLIMAGEEXT_H_
#define _IPLIMAGEEXT_H_

#include "LibDllHeader.h"
#include "UtilOpenCVBlt.h"
#include "UtilOpenCV.h"
#include "IplExtOperate.h"

#include <vector>

/*
#define IPLEXT_BLOCK_SIZE 64
#define MUL_BLOCK_SIZE(x) ((x) << 6)
#define DIV_BLOCK_SIZE(x) ((x) >> 6)
#define MOD_BLOCK_SIZE(x) (0x3f & (x))
*/

#define IPLEXT_BLOCK_SIZE 128
#define MUL_BLOCK_SIZE(x) ((x) << 7)
#define DIV_BLOCK_SIZE(x) ((x) >> 7)
#define MOD_BLOCK_SIZE(x) (0x7f & (x))

typedef std::vector<IplImage*> IplImage_Vec;

enum IPLEXT_EXTEND_CODE {
	TO_TOP = 0x00ff,
	TO_BOTTOM = 0x000f,
	TO_LEFT = 0xff00,
	TO_RIGHT = 0x0f00,
};

enum IPLEXT_RASTER_CODE {
	COPY=0,
	ALPHA_BLEND,
	NOT_COPY, //反転させてコピー
	ADD,
	MUL,
	COPY_BY_BINARY_MASK,
};


//ブロック分割を行いメモリ消費を減少させる。
//実際に生成される画像サイズはブロックサイズの倍数の高さ、幅になる。
//矩形でピクセル位置を制御して任意のサイズの画像に見せる。
class _EXPORTCPP IplImageExt
{
public:
	IplImageExt();
	~IplImageExt();

	//////////////////////
	/*!
		initialize
	*/
	void Init(int width, int height, int depth, int nChannel);
	void InitToEqualSize(const IplImageExt* src, int depth, int nChannel);

	//////////////////////
	/*!
		extention image
		@param[in] cx X軸方向拡張幅
		@param[in] cy Y軸方向拡張幅
		@param[in] extend_code 拡張方向フラグ
	*/
	void ExtendImage(int cx, int cy, unsigned int extend_code);

	//////////////////////
	/*!
		全ブロックのメモリ開放
	*/
	void ClearAllBlock();

	/////////////////////
	/*!
		範囲を指定してブロックのメモリ開放
		@param[in] rect
	*/
	void ClearBlock(const LPRECT rect);

	/////////////////////
	/*!
		座標(x, y)をふくむブロックを作成する
		@param[in] x
		@param[in] y
	*/
	void CreateBlockFromPos(int x, int y);

	//////////////////////
	/*!
		return image size
	*/
	inline CvSize GetSize() const{
		assert( isInit );

		CvSize size;
		size.width = m_EffectiveRect.right - m_EffectiveRect.left;
		size.height = m_EffectiveRect.bottom - m_EffectiveRect.top;
		return size;
	}

	//////////////////////
	/*!
		return num of blocks
	*/
	inline CvSize GetBlockNumSize() const{
		assert( isInit );

		CvSize size;
		size.width = m_block_x;
		size.height = m_block_y;
		return size;
	}

	///////////////////////
	/*!
		画像の1チャンネルの深度を返す
	*/
	inline int GetImgDepth() const{
		return m_ImgDepth;
	}

	///////////////////////
	/*!
		画像のチャンネル数を返す
	*/
	inline int GetImgNChannel() const{
		return m_ImgNChannel;
	}

	//////////////////////
	/*!
		最大有効範囲を返す。
	*/
	void GetMaxRangeRect(LPRECT lprc) const;

	bool IplImageExt::IsFillZeroMask() const;
	void GetMaskRect(LPRECT lprc) const;

	///////////////////////
	/*!
	*/
	inline int GetNextBlockPosX(int x) const {
		int n = MOD_BLOCK_SIZE(m_EffectiveRect.left + x);
		return x + IPLEXT_BLOCK_SIZE - n-1;
	}
	inline int GetNextBlockPosY(int y) const {
		int n = MOD_BLOCK_SIZE(m_EffectiveRect.top + y);
		return y + IPLEXT_BLOCK_SIZE - n-1;
	}

	/////////////////////////////////
	/*!
		位置(x,y)に要素をセットする。
	*/
	void SetPixel(int x, int y, const UCvPixel* src);
	inline void SetPixelAlphaBlend(int x, int y, const UCvPixel* src);
	inline void SetAlpha(int x, int y, int8_t a);

	inline void SetMaskData(int x, int y, int8_t data);
	inline void SetData(int x, int y, int depth, int8_t data);
	
	/////////////////////////////////
	/*!
		位置(x,y)から要素を取り出す。
		位置(x,y)にブロックが存在指定いればtrueをそうでなければfalseを返す。
	*/
	bool GetPixel(int x, int y, UCvPixel* dst) const;
	inline bool GetAlpha(int x, int y, uint8_t *a) const;

	inline bool GetMaskData(int x, int y, uint8_t *data) const;
	inline bool GetData(int x, int y, int depth, uint8_t *data) const;

	inline uint8_t* GetPixelAddr(int x, int y) const {
		assert( isInit );

		IplImage *image = GetBlockImgFromPos(x, y);
		if(image){
			return GetPixelAddress(
				image,
				MOD_BLOCK_SIZE(m_EffectiveRect.left + x),
				MOD_BLOCK_SIZE(m_EffectiveRect.top + y));
		}
		return NULL;
	}
	inline UCvPixel32* GetPixelAddr32(int x, int y) const {
		assert( isInit );

		IplImage *image = GetBlockImgFromPos(x, y);
		if(image){
			return GetPixelAddress32(
				image,
				MOD_BLOCK_SIZE(m_EffectiveRect.left + x),
				MOD_BLOCK_SIZE(m_EffectiveRect.top + y));
		}
		return NULL;
	}

	////////////////////////////////////////////
	/*!
		範囲を指定してをimageにコピーして渡す。
		@param[out] dst 出力先
		@param[in] dst_startx
		@param[in] dst_starty
		@param[in] width
		@param[in] height
		@param[in] src_startx
		@param[in] src_starty
		@param[in] mask
	*/
	void GetRangeIplImage(IplImage* dst, int dst_startX, int dst_startY,
		int width, int height, int src_startX, int src_startY,
		const IplImage* mask = NULL) const;

	/////////////////////////////////////////////
	/*!
		ブロック位置(block_x, block_y)にimgをコピーする。
	*/
	void CopyBlock(IplImage *img, int block_x, int block_y); 

	/////////////////////////////////////////////
	/*!
		imageに自身をコピーする
		@param[out] image
	*/
	void Copy(IplImageExt* image) const;

	//////////////////////////////////////////////
	/*!
		矩形範囲を指定色で塗りつぶす
		@param[in] posX 開始位置
		@param[in] posY 開始位置
		@param[in] width 塗りつぶす幅
		@param[in] height 塗りつぶす高さ
		@param[in] src 塗りつぶす色
	*/
	void RectFillColor(int posX, int posY, int width, int height,
		const UCvPixel* src, const IplImageExt* mask_ext,
		int mask_startX, int mask_startY);
	void RectFillColor(int posX, int posY, int width, int height,
		const UCvPixel* src, const IplImage* mask = NULL,
		int mask_startX = 0, int mask_startY = 0);
	void RectFillAlpha(int posX, int posY, int width, int height,
		uint8_t a, const IplImageExt* mask_ext = NULL);
	void RectFillMask(int posX, int posY, int width, int height,
		uint8_t mask, const IplImageExt* mask_ext = NULL,
		int mask_startX = 0, int mask_startY = 0);
	void RectFillZero(int posX, int posY, int width, int height);
	void FillMask(uint8_t mask);

	//////////////////////////////////////////////
	/*!
		IplImageExt型のimageを自身に貼り付ける。
		@param[in] posX
		@param[in] posY
		@param[in] width
		@param[in] height
		@param[in] image 重ねる画像
		@param[in] startX 重ねる画像の貼り付け開始X座標
		@param[in] startY 重ねる画像の貼り付け開始Y座標
		@param[in] raster_code 重ね方の指定
	*/
	void ImgBlt(int posX, int posY, int width, int height,
		const IplImageExt *image, int startX, int startY,
		IPLEXT_RASTER_CODE raster_code = IPLEXT_RASTER_CODE::COPY,
		const IplImageExt* mask_ext = NULL, int mask_startX = 0, int mask_startY = 0);

	void ImgBlt(int posX, int posY, int width, int height,
		const IplImageExt *image, int startX, int startY,
		IPLEXT_RASTER_CODE raster_code = IPLEXT_RASTER_CODE::COPY,
		const IplImage* mask = NULL, int mask_startX = 0, int mask_startY = 0);

	void ImgBlt(int posX, int posY, int width, int height,
		const IplImage *img, int startX, int startY, 
		IPLEXT_RASTER_CODE raster_code = IPLEXT_RASTER_CODE::COPY,
		const IplImage* mask=NULL, int mask_startX = 0, int mask_startY = 0);

	/////////////////////////////////////////////////////
	/*!
		under_imgとover_imgをアルファブレンドして自身に貼り付ける。
	*/
	void AlphaBlend(int posX, int posY, int width, int height,
		const IplImageExt *under_img, int ui_startX, int ui_startY,
		const IplImageExt *over_img, int oi_startX, int oi_startY,
		const IplImageExt *mask_ext);

	void AlphaBlend(int posX, int posY, int width, int height,
		const IplImage *under_img, int ui_startX, int ui_startY,
		const IplImage *over_img, int oi_startX, int oi_startY,
		const IplImage *mask = NULL, int mi_startX=0, int mi_startY=0);

	//////////////////////////////////////////////////////
	template<class _CBlendOp, class _CMaskBlockOp>
	inline void OperateBlend(
		const _CBlendOp& blendOp,
		const _CMaskBlockOp& maskBlockOp,
		int posX, int posY, int width, int height,
		const IplImageExt *under_img, int ui_startX, int ui_startY,
		const IplImageExt *over_img, int oi_startX, int oi_startY,
		const IplImageExt *mask_img, int mi_startX, int mi_startY);

	template<class _CBlendOp>
	inline void OperateBlend(
		const _CBlendOp& blendOp,
		int posX, int posY, int width, int height,
		const IplImageExt *under_img, int ui_startX, int ui_startY,
		const IplImageExt *over_img, int oi_startX, int oi_startY,
		const IplImage *mask_img = NULL, int mi_startX = 0, int mi_startY = 0);

	template<class _CBlendOp>
	inline void OperateBlend(
		const _CBlendOp& blendOp,
		int poX, int posY, int width, int height,
		const IplImage *under_img, int ui_startX, int ui_startY,
		const IplImage *over_img, int oi_startX, int oi_startY,
		const IplImage *mask = NULL, int mi_startX = 0, int mi_startY = 0);

	///////////////////////////////////////////////////////
	template<class _CBltOp, class _CMaskBlockOp>
	inline void OperateBlt(
		const _CBltOp& bltOp,
		const _CMaskBlockOp& maskBlockOp,
		int posX, int posY, int width, int height,
		const IplImageExt *src, int src_startX, int src_startY,
		const IplImageExt *mask, int mask_startX, int mask_startY);

	template<class _CBltOp>
	inline void OperateBlt(
		const _CBltOp& bltOp,
		int posX, int posY, int width, int height,
		const IplImageExt *src, int src_startX, int src_startY,
		const IplImage *mask = NULL, int mask_startX = 0, int mask_startY = 0);

	template<class _CBltOp>
	inline void OperateBlt(
		const _CBltOp& bltOp,
		int posX, int posY, int width, int height,
		const IplImage *src, int startX, int startY,
		const IplImage *mask = NULL, int mask_startX = 0, int mask_startY = 0);

	//////////////////////////////////////////////////////
	template<class _CEditOp>
	inline void OperateEdit(
		const _CEditOp& editOp,
		int posX, int posY, int width, int height);

	//////////////////////////////////////////////////////
	template<class _CBltOp, class _CMaskBlockOp>
	static inline void OperateBlt(
		const _CBltOp& bltOp,
		const _CMaskBlockOp& maskBlockOp,
		IplImage* dst,
		int posX, int posY, int widht, int height,
		const IplImageExt* src, int startX, int startY,
		const IplImageExt* mask = NULL);

	template<class _CBltOp>
	static inline void OperateBlt(
		const _CBltOp& bltOp,
		IplImage* dst,
		int posX, int posY, int width, int height,
		const IplImage* src, int startX, int startY,
		const IplImage* mask = NULL);

	template<class _CEditOp>
	static inline void OperateEdit(
		const _CEditOp& editOp,
		IplImage* dst,
		int posX, int posY, int width, int height);

	//////////////////////////////////////////////////////
	/*!
		1つづつのチャンネルに分割する
	*/
	void Split(IplImageExt* dst0=NULL, IplImageExt* dst1=NULL, IplImageExt* dst2=NULL, IplImageExt* dst3=NULL) const;
	void Split(int posX, int posY, int width, int height,
		IplImageExt* dst0=NULL, IplImageExt* dst1=NULL, IplImageExt* dst2=NULL, IplImageExt* dst3=NULL) const;
	void Split(int posX, int posY, int width, int height,
		IplImage* dst0=NULL, IplImage* dst1=NULL, IplImage* dst2=NULL, IplImage* dst3=NULL) const;

	////////////////////////////////////////////////////////
	/*!
		チャンネルを合成する。
	*/
	void Merge(IplImageExt* src0=NULL, IplImageExt* src1=NULL, IplImageExt* src2=NULL, IplImageExt* src3=NULL);
	void Merge(int posX, int posY, int width, int height,
		IplImageExt* src0=NULL, IplImageExt* src1=NULL, IplImageExt* src2=NULL, IplImageExt* src3=NULL);
	void Merge(int posX, int posY, int width, int height,
		const IplImage* src0=NULL, const IplImage* src1=NULL, const IplImage* const src2=NULL, const IplImage* src3=NULL);

	////////////////////////////////////////////////////////
	/*!
		ルックアップテーブルを用いて画像を変換する
		@param[in] src 入力画像
		@param[in] lut 用いるルックアップテーブル
	*/
	void LUT(IplImageExt* src, const CvArr* lut);
	void LUT(int posX, int posY, int width, int height,
		const IplImageExt* src, int startX, int startY,
		const CvArr* lut);
	//Preserve Luminosity LUT
	void PL_LUT(int posX, int posY, int width, int height,
		const IplImageExt* src, int startX, int startY,
		const CvArr* lut);

	void CalcHist(CvHistogram* hist, int accumulate=0);

private:

	///////////////////////
	/*!
	*/
	void ImgBltCopy(int posX, int posY, int width, int height,
		const IplImageExt* image, int startX, int startY,
		const IplImageExt* mask_ext, int mask_startX, int mask_startY);

	void ImgBltNotCopy(int posX, int posY, int width, int height,
		const IplImageExt* image, int startX, int startY,
		const IplImageExt* mask_ext, int mask_startX, int mask_startY);

	void ImgBltAlphaBlend(int posX, int posY, int width, int height,
		const IplImageExt* image, int startX, int startY,
		const IplImageExt* mask_ext, int mask_startX, int mask_startY);

	void ImgBltCopyByBinaryMask(int posX, int posY, int width, int height,
		const IplImageExt* image, int startX, int startY,
		const IplImageExt* mask_ext, int mask_startX, int mask_startY);

	//
	void ImgBltCopy(int posX, int posY, int width, int height,
		const IplImageExt* img, int startX, int startY,
		const IplImage* mask, int mask_startX, int mask_startY);

	void ImgBltNotCopy(int posX, int posY, int width, int heigth,
		const IplImageExt* img, int startX, int startY,
		const IplImage* mask, int mask_startX, int mask_startY);

	void ImgBltAlphaBlend(int posX, int posY, int width, int height,
		const IplImageExt* img, int startX, int startY,
		const IplImage* mask, int mask_startX, int mask_startY);

	void ImgBltCopyByBinaryMask(int posX, int posY, int width, int height,
		const IplImageExt* img, int startX, int startY,
		const IplImage* mask, int mask_startX, int mask_startY);

	//
	void ImgBltCopy(int posX, int posY, int width, int height,
		const IplImage* img, int startX, int startY,
		const IplImage* mask, int mask_startX, int mask_startY);

	void ImgBltNotCopy(int posX, int posY, int width, int heigth,
		const IplImage* img, int startX, int startY,
		const IplImage* mask, int mask_startX, int mask_startY);

	void ImgBltAlphaBlend(int posX, int posY, int width, int height,
		const IplImage* img, int startX, int startY,
		const IplImage* mask, int mask_startX, int mask_startY);

	void ImgBltCopyByBinaryMask(int posX, int posY, int width, int height,
		const IplImage* img, int startX, int startY,
		const IplImage* mask, int mask_startX, int mask_startY);

	///////////////////////
	/*!
		位置(x,y)のブロックへのポインタを取り出す
		@param[in] x 画像X座標
		@param[in] y 画像Y座標
		@return ブロック画像
	*/
	inline IplImage* GetBlockImgFromPos(int x, int y) const;

	///////////////////////
	/*!
		block_x列 block_y行目のブロックを取り出す。
		block_x(0~ block_y(0~
		@param[in] block_x 列
		@param[in] block_y 行
		@return ブロック画像
	*/
	inline IplImage* GetBlock(int block_x, int block_y) const;

	/////////////////////////
	/*!
		block_x列 block_y行目のブロックをセットする
		block_x(0~ block_y(0~
		@param[in] block_x 列
		@param[in] block_y 行
		@param[in,out] block セットするブロック
		@return ブロック画像
	*/
	inline void SetBlock(int block_x, int block_y, IplImage* block);

	///////////////////////////////////////
	/*!
		ブロック位置(block_x, block_y)のブロックを作成する。
	*/
	inline void CreateBlock(int block_x, int block_y);


	int m_block_x; //横方向個数
	int m_block_y; //縦方向個数

	RECT m_EffectiveRect; //有効範囲
	int m_ImgDepth;
	int m_ImgNChannel;

	IplImage_Vec m_block_Img_Vec;

	bool isInit;
	bool isLockImgBlock;
};


template<class _CBlendOp, class _CMaskBlockOp>
void IplImageExt::OperateBlend(
	const _CBlendOp& blendOp,
	const _CMaskBlockOp& maskBlockOp,
	int posX,
	int posY,
	int width,
	int height,
	const IplImageExt *under_img,
	int ui_startX,
	int ui_startY,
	const IplImageExt *over_img,
	int oi_startX,
	int oi_startY,
	const IplImageExt *mask_img,
	int mi_startX,
	int mi_startY)
{
	if (mask_img == NULL) {
		this->OperateBlend(
			blendOp,
			posX,
			posY,
			width,
			height,
			under_img,
			ui_startX,
			ui_startY,
			over_img,
			oi_startX,
			oi_startY,
			NULL, 0, 0);
		return;
	}

	int mask_width = mask_img->m_EffectiveRect.right - mask_img->m_EffectiveRect.left;
	int mask_height = mask_img->m_EffectiveRect.bottom - mask_img->m_EffectiveRect.top;

	//clipping mask
	if (mi_startX < 0) {
		width += mi_startX;
		posX -= mi_startX;
		oi_startX -= mi_startX;
		ui_startX -= mi_startX;
		mi_startX = 0;
	}
	if (mi_startY < 0) {
		height += mi_startY;
		posY -= mi_startY;
		oi_startY -= mi_startY;
		ui_startY -= mi_startY;
		mi_startY = 0;
	}
	if ((mi_startX + width)  > mask_width) {
		width  = mask_width  - mi_startX;
	}
	if ((mi_startY + height) > mask_height) {
		height = mask_height - mi_startY;
	}

	//
	int mask_offset_y = MOD_BLOCK_SIZE(mi_startY + mask_img->m_EffectiveRect.top);
	int mask_offset_x = MOD_BLOCK_SIZE(mi_startX + mask_img->m_EffectiveRect.left);
	int mask_start_block_y = DIV_BLOCK_SIZE(mi_startY + mask_img->m_EffectiveRect.top);
	int mask_start_block_x = DIV_BLOCK_SIZE(mi_startX + mask_img->m_EffectiveRect.left);

	int block_y_num = DIV_BLOCK_SIZE(mask_offset_y + height - 1) + 1;
	int block_x_num = DIV_BLOCK_SIZE(mask_offset_x + width - 1) + 1;

	int block_x;
	int block_y;
	int write_to_x;
	int write_to_y;
	int start_mask_x;
	int start_mask_y;
	int write_width;
	int write_height;
	int writed_width;
	int writed_height;
	IplImage* mask_block_img;

	writed_height = 0;
	for (block_y = 0; block_y < block_y_num; block_y++) {
		if (block_y == 0) {
			start_mask_y = mask_offset_y;
			write_height= IPLEXT_BLOCK_SIZE - mask_offset_y;
			if (write_height > height) {
				write_height = height;
			}
		} else if (block_y == block_y_num-1) {
			start_mask_y = 0;
			write_height = height + mask_offset_y - MUL_BLOCK_SIZE(block_y_num-1);
		} else {
			start_mask_y = 0;
			write_height = IPLEXT_BLOCK_SIZE;
		}

		writed_width = 0;
		for (block_x= 0; block_x < block_x_num; block_x++) {
			if (block_x == 0) {
				start_mask_x = mask_offset_x;
				write_width = IPLEXT_BLOCK_SIZE - mask_offset_x;
				if (write_width > width) {
					write_width = width;
				}
			} else if (block_x == block_x_num-1) {
				start_mask_x = 0;
				write_width = width + mask_offset_x - MUL_BLOCK_SIZE(block_x_num-1);
			} else {
				start_mask_x = 0;
				write_width = IPLEXT_BLOCK_SIZE;
			}

			mask_block_img = NULL;
			if (mask_img) {
				mask_block_img = mask_img->GetBlock(
					mask_start_block_x + block_x,
					mask_start_block_y + block_y);
				if(maskBlockOp( mask_block_img )){
					writed_width += write_width;
					continue;
				}
			}

			OperateBlend(
				blendOp,
				posX + writed_width,
				posY + writed_height,
				write_width,
				write_height,
				under_img,
				ui_startX + writed_width,
				ui_startY + writed_height,
				over_img,
				oi_startX + writed_width,
				oi_startY + writed_height,
				mask_block_img,
				start_mask_x,
				start_mask_y);

			writed_width += write_width;
		}

		writed_height += write_height;
	}
}

template<class _CBlendOp>
void IplImageExt::OperateBlend(
	const _CBlendOp& blendOp,
	int posX,
	int posY,
	int width,
	int height,
	const IplImageExt *under_img,
	int ui_startX,
	int ui_startY,
	const IplImageExt *over_img,
	int oi_startX,
	int oi_startY,
	const IplImage *mask,
	int mi_startX,
	int mi_startY)
{
	//under_imgとover_imgのブロックがきっちり重なっていること
	assert( MOD_BLOCK_SIZE(ui_startY) == MOD_BLOCK_SIZE(oi_startY) );
	assert( MOD_BLOCK_SIZE(ui_startX) == MOD_BLOCK_SIZE(oi_startX) );

	int dst_width = m_EffectiveRect.right - m_EffectiveRect.left;
	int dst_height = m_EffectiveRect.bottom - m_EffectiveRect.top;
	int under_width = under_img->m_EffectiveRect.right - under_img->m_EffectiveRect.left;
	int under_height = under_img->m_EffectiveRect.bottom - under_img->m_EffectiveRect.top;
	int over_width = over_img->m_EffectiveRect.right - over_img->m_EffectiveRect.left;
	int over_height = over_img->m_EffectiveRect.bottom - over_img->m_EffectiveRect.top;

	//clipping
	if (posX < 0) {
		width += posX;
		ui_startX -= posX;
		oi_startX -= posX;
		posX = 0;
	}
	if (posY < 0) {
		height += posY;
		ui_startY -= posY;
		oi_startY -= posY;
		posY = 0;
	}
	if (ui_startX < 0) {
		width += ui_startX;
		posX -= ui_startX;
		oi_startX -= ui_startX;
		ui_startX = 0;
	}
	if (ui_startY < 0) {
		height += ui_startY;
		posY -= ui_startY;
		oi_startY -= ui_startY;
		ui_startY = 0;
	}
	if (oi_startX < 0) {
		width += oi_startX;
		posX -= oi_startX;
		ui_startX -= oi_startX;
		oi_startX = 0;
	}
	if (oi_startY < 0) {
		height += oi_startY;
		posY -= oi_startY;
		ui_startY -= oi_startY;
		oi_startY = 0;
	}

	if ((posX + width)  > dst_width) {
		width  = dst_width  - posX;
	}
	if ((posY + height) > dst_height) {
		height = dst_height - posY;
	}
	if ((ui_startX + width)  > under_width) {
		width  = under_width  - ui_startX;
	}
	if ((ui_startY + height) > under_height) {
		height = under_height - ui_startY;
	}
	if ((oi_startX + width)  > over_width) {
		width  = over_width  - oi_startX;
	}
	if ((oi_startY + height) > over_height) {
		height = over_height - oi_startY;
	}

	int dst_start_block_y = DIV_BLOCK_SIZE(posY + this->m_EffectiveRect.top);
	int dst_start_block_x = DIV_BLOCK_SIZE(posX + this->m_EffectiveRect.left);

	int write_offset_y = MOD_BLOCK_SIZE(oi_startY + over_img->m_EffectiveRect.top);
	int write_offset_x = MOD_BLOCK_SIZE(oi_startX + over_img->m_EffectiveRect.left);
	int under_start_block_y = DIV_BLOCK_SIZE(ui_startY + under_img->m_EffectiveRect.top);
	int under_start_block_x = DIV_BLOCK_SIZE(ui_startX + under_img->m_EffectiveRect.left);
	int over_start_block_y = DIV_BLOCK_SIZE(oi_startY + over_img->m_EffectiveRect.top);
	int over_start_block_x = DIV_BLOCK_SIZE(oi_startX + over_img->m_EffectiveRect.left);

	int block_y_num = DIV_BLOCK_SIZE(write_offset_y + height - 1) + 1;
	int block_x_num = DIV_BLOCK_SIZE(write_offset_x + width - 1) + 1;

	int block_x;
	int block_y;
	int write_to_x;
	int write_to_y;
	int start_write_x;
	int start_write_y;
	int write_width;
	int write_height;
	int writed_width;
	int writed_height;
	IplImage* under_block_img;
	IplImage* over_block_img;

	writed_height = 0;
	for (block_y = 0; block_y < block_y_num; block_y++) {
		if (block_y == 0) {
			write_to_y = posY;
			start_write_y = write_offset_y;
			write_height= IPLEXT_BLOCK_SIZE - write_offset_y;
			if(write_height > height)
				write_height = height;
		} else if (block_y == block_y_num-1) {
			write_to_y = posY - write_offset_y + MUL_BLOCK_SIZE(block_y_num-1);
			start_write_y = 0;
			write_height = height + write_offset_y - MUL_BLOCK_SIZE(block_y_num-1);
		} else {
			write_to_y = posY - write_offset_y + MUL_BLOCK_SIZE(block_y);
			start_write_y = 0;
			write_height = IPLEXT_BLOCK_SIZE;
		}

		writed_width = 0;
		for (block_x= 0; block_x < block_x_num; block_x++) {
			if (block_x == 0) {
				write_to_x = posX;
				start_write_x = write_offset_x;
				write_width = IPLEXT_BLOCK_SIZE - write_offset_x;
				if (write_width > width)
					write_width = width;
			} else if (block_x == block_x_num-1) {
				write_to_x = posX - write_offset_x + MUL_BLOCK_SIZE(block_x_num-1);
				start_write_x = 0;
				write_width = width + write_offset_x - MUL_BLOCK_SIZE(block_x_num-1);
			} else {
				write_to_x = posX - write_offset_x + MUL_BLOCK_SIZE(block_x);
				start_write_x = 0;
				write_width = IPLEXT_BLOCK_SIZE;
			}

			under_block_img = under_img->GetBlock(
				under_start_block_x + block_x,
				under_start_block_y + block_y);
			over_block_img = over_img->GetBlock(
				over_start_block_x + block_x,
				over_start_block_y + block_y);

			if ((under_block_img != NULL) || (over_block_img != NULL) ) {
				OperateBlend(
					blendOp,
					write_to_x,
					write_to_y,
					write_width,
					write_height,
					under_block_img,
					start_write_x,
					start_write_y,
					over_block_img,
					start_write_x,
					start_write_y,
					mask,
					mi_startX + writed_width,
					mi_startY + writed_height);
			}

			writed_width += write_width;
		}

		writed_height += write_height;
	}
}

template<class _CBlendOp>
void IplImageExt::OperateBlend(
	const _CBlendOp& blendOp,
	int posX,
	int posY,
	int width,
	int height,
	const IplImage *under_img,
	int ui_startX,
	int ui_startY,
	const IplImage *over_img,
	int oi_startX,
	int oi_startY,
	const IplImage *mask,
	int mi_startX,
	int mi_startY)
{
	int i;
	int x,y;
	int next_x;
	int next_y;
	int blend_width;
	int blend_height;

	IplImage* dst;
	UCvPixel* pdst;
	UCvPixel* punder = NULL;
	UCvPixel* pover = NULL;
	uint8_t* pmask = NULL;

	for (y = 0; y < height; y++) {
		next_y = this->GetNextBlockPosY(posY + y);
		blend_height = next_y - (posY + y) + 1;
		if ((y + blend_height) > height) {
			blend_height = height - y;
		}

		for (x = 0; x < width; x++) {
			next_x = this->GetNextBlockPosX(posX + x);
			blend_width = next_x - (posX + x) + 1;
			if ((x + blend_width) > width){
				blend_width = width - x;
			}
		
			if (under_img) {
				punder = GetPixelAddress32(under_img, ui_startX + x, ui_startY + y);
			}
			if (over_img) {
				pover = GetPixelAddress32(over_img, oi_startX + x, oi_startY + y);
			}
			if (mask) {
				pmask = GetPixelAddress(mask, mi_startX + x, mi_startY + y);
			}
			dst = GetBlockImgFromPos(posX + x, posY + y);
			if (dst == NULL) {
				CreateBlockFromPos(posX + x, posY + y);
				dst = GetBlockImgFromPos(posX + x, posY + y);
				assert(dst);
			}
			pdst = GetPixelAddress32(
				dst,
				MOD_BLOCK_SIZE(m_EffectiveRect.left + posX + x),
				MOD_BLOCK_SIZE(m_EffectiveRect.top + posY + y));

			for (i = 0; i < blend_height; i++) {
	
				blendOp(pdst, punder, pover, pmask, blend_width);

				if (under_img) {
					punder = GetNextLineAddress32(under_img, punder);
				}
				if (over_img) {
					pover = GetNextLineAddress32(over_img, pover);
				}
				if (mask) {
					pmask = GetNextLineAddress(mask, pmask);
				}
				pdst = GetNextLineAddress32(dst, pdst);
			}
			x += blend_width-1;
		}
		y += blend_height-1;
	}
}

template<class _CBltOp, class _CMaskBlockOp>
void IplImageExt::OperateBlt(
	const _CBltOp& bltOp,
	const _CMaskBlockOp& maskBlockOp,
	int posX, int posY,
	int width, int height,
	const IplImageExt *src,
	int src_startX, int src_startY,
	const IplImageExt *mask,
	int mask_startX, int mask_startY)
{
	if (mask == NULL) {
		this->OperateBlt(
			bltOp,
			posX,
			posY,
			width,
			height,
			src,
			src_startX,
			src_startY,
			NULL, 0, 0);
		return;
	}

	int mask_width = mask->m_EffectiveRect.right - mask->m_EffectiveRect.left;
	int mask_height = mask->m_EffectiveRect.bottom - mask->m_EffectiveRect.top;

	//clipping mask
	if (mask_startX < 0) {
		width += mask_startX;
		posX -= mask_startX;
		src_startX -= mask_startX;
		mask_startX = 0;
	}
	if (mask_startY < 0) {
		height += mask_startY;
		posY -= mask_startY;
		src_startY -= mask_startY;
		mask_startY = 0;
	}
	if ((mask_startX + width)  > mask_width) {
		width  = mask_width  - mask_startX;
	}
	if ((mask_startY + height) > mask_height) {
		height = mask_height - mask_startY;
	}

	//
	int mask_offset_y = MOD_BLOCK_SIZE(mask_startY + mask->m_EffectiveRect.top);
	int mask_offset_x = MOD_BLOCK_SIZE(mask_startX + mask->m_EffectiveRect.left);
	int mask_start_block_y = DIV_BLOCK_SIZE(mask_startY + mask->m_EffectiveRect.top);
	int mask_start_block_x = DIV_BLOCK_SIZE(mask_startX + mask->m_EffectiveRect.left);

	int block_y_num = DIV_BLOCK_SIZE(mask_offset_y + height - 1) + 1;
	int block_x_num = DIV_BLOCK_SIZE(mask_offset_x + width - 1) + 1;


	int block_x;
	int block_y;
	int write_to_x;
	int write_to_y;
	int start_mask_x;
	int start_mask_y;
	int write_width;
	int write_height;
	int writed_width;
	int writed_height;
	IplImage* mask_block_img;

	writed_height = 0;
	for (block_y = 0; block_y < block_y_num; block_y++) {
		if (block_y == 0) {
			start_mask_y = mask_offset_y;
			write_height= IPLEXT_BLOCK_SIZE - mask_offset_y;
			if (write_height > height)
				write_height = height;
		} else if(block_y == block_y_num-1) {
			start_mask_y = 0;
			write_height = height + mask_offset_y - MUL_BLOCK_SIZE(block_y_num-1);
		} else {
			start_mask_y = 0;
			write_height = IPLEXT_BLOCK_SIZE;
		}

		writed_width = 0;
		for (block_x = 0; block_x < block_x_num; block_x++) {
			if (block_x == 0) {
				start_mask_x = mask_offset_x;
				write_width = IPLEXT_BLOCK_SIZE - mask_offset_x;
				if(write_width > width)
					write_width = width;
			} else if (block_x == block_x_num-1) {
				start_mask_x = 0;
				write_width = width + mask_offset_x - MUL_BLOCK_SIZE(block_x_num-1);
			} else {
				start_mask_x = 0;
				write_width = IPLEXT_BLOCK_SIZE;
			}

			//get mask block
			mask_block_img = NULL;
			if (mask) {
				mask_block_img = mask->GetBlock(
					mask_start_block_x + block_x,
					mask_start_block_y + block_y);
				if(maskBlockOp( mask_block_img )) {
					writed_width += write_width;
					continue;
				}
			}

			this->OperateBlt(
				bltOp,
				posX + writed_width,
				posY + writed_height,
				write_width,
				write_height,
				src,
				src_startX + writed_width,
				src_startY + writed_height,
				mask_block_img,
				start_mask_x,
				start_mask_y);

			writed_width += write_width;
		}

		writed_height += write_height;
	}
}

template<class _CBltOp>
void IplImageExt::OperateBlt(
	const _CBltOp& bltOp,
	int posX, int posY,
	int width, int height,
	const IplImageExt *src,
	int src_startX, int src_startY,
	const IplImage *mask,
	int mask_startX, int mask_startY)
{
	int dst_width = m_EffectiveRect.right - m_EffectiveRect.left;
	int dst_height = m_EffectiveRect.bottom - m_EffectiveRect.top;
	int src_width = 0;
	int src_height = 0;

	if (src) {
		src_width = src->m_EffectiveRect.right - src->m_EffectiveRect.left;
		src_height = src->m_EffectiveRect.bottom - src->m_EffectiveRect.top;
	}

	//clipping
	if (posX < 0) {
		width += posX;
		src_startX -= posX;
		posX = 0;
	}
	if (posY < 0) {
		height += posY;
		src_startY -= posY;
		posY = 0;
	}
	if (src_startX < 0) {
		width += src_startX;
		posX -= src_startX;
		src_startX = 0;
	}
	if (src_startY < 0) {
		height += src_startY;
		posY -= src_startY;
		src_startY = 0;
	}

	if ((posX + width) > dst_width) {
		width = dst_width - posX;
	}
	if ((posY + height) > dst_height) {
		height = dst_height - posY;
	}
	if (src) {
		if ((src_startX + width) > src_width) {
			width = src_width - src_startX;
		}
		if ((src_startY + height) > src_height) {
			height = src_height - src_startY;
		}
	}
 
	int write_offset_y = 0;
	int write_offset_x = 0;
	int src_start_block_y = 0;
	int src_start_block_x = 0;
	if (src) {
		write_offset_y = MOD_BLOCK_SIZE(src_startY + src->m_EffectiveRect.top);
		write_offset_x = MOD_BLOCK_SIZE(src_startX + src->m_EffectiveRect.left);
		src_start_block_y = DIV_BLOCK_SIZE(src_startY + src->m_EffectiveRect.top);
		src_start_block_x = DIV_BLOCK_SIZE(src_startX + src->m_EffectiveRect.left);
	}

	int block_y_num = DIV_BLOCK_SIZE(write_offset_y + height - 1) + 1;
	int block_x_num = DIV_BLOCK_SIZE(write_offset_x + width - 1) + 1;


	int block_x;
	int block_y;
	int write_to_x;
	int write_to_y;
	int start_write_x;
	int start_write_y;
	int write_width;
	int write_height;
	int writed_width;
	int writed_height;
	IplImage* block_img;
	IplImage* mask_block_img;

	writed_height = 0;
	for (block_y = 0; block_y < block_y_num; block_y++) {
		if (block_y == 0) {
			write_to_y = posY;
			start_write_y = write_offset_y;
			write_height= IPLEXT_BLOCK_SIZE - write_offset_y;
			if (write_height > height)
				write_height = height;
		} else if(block_y == block_y_num-1) {
			write_to_y = posY - write_offset_y + MUL_BLOCK_SIZE(block_y_num-1);
			start_write_y = 0;
			write_height = height + write_offset_y - MUL_BLOCK_SIZE(block_y_num-1);
		} else {
			write_to_y = posY - write_offset_y + MUL_BLOCK_SIZE(block_y);
			start_write_y = 0;
			write_height = IPLEXT_BLOCK_SIZE;
		}

		writed_width = 0;
		for (block_x = 0; block_x < block_x_num; block_x++) {
			if (block_x == 0) {
				write_to_x = posX;
				start_write_x = write_offset_x;
				write_width = IPLEXT_BLOCK_SIZE - write_offset_x;
				if (write_width > width)
					write_width = width;
			} else if (block_x == block_x_num-1) {
				write_to_x = posX - write_offset_x + MUL_BLOCK_SIZE(block_x_num-1);
				start_write_x = 0;
				write_width = width + write_offset_x - MUL_BLOCK_SIZE(block_x_num-1);
			} else {
				write_to_x = posX - write_offset_x + MUL_BLOCK_SIZE(block_x);
				start_write_x = 0;
				write_width = IPLEXT_BLOCK_SIZE;
			}

			//write block
			block_img = NULL;
			if (src) {
				block_img = src->GetBlock(
					src_start_block_x + block_x,
					src_start_block_y + block_y);
			}

			this->OperateBlt(
				bltOp,
				write_to_x,
				write_to_y,
				write_width,
				write_height,
				block_img,
				start_write_x,
				start_write_y,
				mask,
				mask_startX + writed_width,
				mask_startY + writed_height);

			writed_width += write_width;
		}

		writed_height += write_height;
	}
}

template<class _CBltOp>
void IplImageExt::OperateBlt(
	const _CBltOp& bltOp,
	int posX,
	int posY,
	int width,
	int height,
	const IplImage *src,
	int startX,
	int startY,
	const IplImage *mask,
	int mask_startX,
	int mask_startY)
{
	//clipping
	if (posX < 0) {
		width += posX;
		startX -= posX;
		mask_startX -= posX;
		posX = 0;
	}
	if (posY < 0) {
		height += posY;
		startY -= posY;
		mask_startY -= posY;
		posY = 0;
	}
	if (startX < 0) {
		width += startX;
		posX -= startX;
		startX = 0;
	}
	if (startY < 0) {
		width += startY;
		posY -= startX;
		startY = 0;
	}
	if (mask_startX < 0) {
		width += mask_startX;
		posX -= mask_startX;
		startX -= mask_startX;
		mask_startX = 0;
	}
	if (mask_startY < 0) {
		height += mask_startY;
		posY -= mask_startY;
		startY -= mask_startY;
		mask_startY = 0;
	}

	int dst_width = m_EffectiveRect.right - m_EffectiveRect.left;
	int dst_height = m_EffectiveRect.bottom - m_EffectiveRect.top;
	if ((posX + width) > dst_width) {
		width = dst_width - posX;
	}
	if ((posY + height) > dst_height) {
		height = dst_height - posY;
	}
	if (src) {
		if ((startX + width) > src->width) {
			width = src->width - startX;
		}
		if ((startY + height) > src->height) {
			height = src->height - startY;
		}
	}
	if (mask) {
		if ((mask_startX + width) > mask->width) {
			width = mask->width - mask_startX;
		}
		if ((mask_startY + height) > mask->height) {
			height = mask->height - mask_startY;
		}
	}

	//blt image
	int i;
	int x,y;
	int next_x;
	int next_y;
	int blend_width;
	int blend_height;

	IplImage* dst;
	UCvPixel* pdst;
	UCvPixel* psrc = NULL;
	uint8_t* pmask = NULL;

	for (y = 0; y < height; y++) {
		next_y = this->GetNextBlockPosY(posY + y);
		blend_height = next_y - (posY + y) + 1;
		if ((y + blend_height) > height)
			blend_height = height - y;

		for (x = 0; x < width; x++) {
			next_x = this->GetNextBlockPosX(posX + x);
			blend_width = next_x - (posX + x) + 1;
			if ((x + blend_width) > width)
				blend_width = width - x;

			if (src) {
				psrc = GetPixelAddress32(src, startX + x, startY + y);
			}
			if (mask) {
				pmask = GetPixelAddress(mask, mask_startX + x, mask_startY + y);
			}
			dst = GetBlockImgFromPos(posX + x, posY + y);
			if (dst == NULL) {
				CreateBlockFromPos(posX + x, posY + y);
				dst = GetBlockImgFromPos(posX + x, posY + y);
				assert(dst);
			}
			pdst = GetPixelAddress32(
				dst,
				MOD_BLOCK_SIZE(m_EffectiveRect.left + posX + x),
				MOD_BLOCK_SIZE(m_EffectiveRect.top + posY + y));
			
			for (i = 0; i < blend_height; i++) {

				bltOp(pdst, psrc, pmask, blend_width);

				if (src) {
					psrc = GetNextLineAddress32(src, psrc);
				}
				if (mask) {
					pmask = GetNextLineAddress(mask, pmask);
				}
				pdst = GetNextLineAddress32(dst, pdst);
			}
			x += blend_width-1;
		}

		y += blend_height-1;
	}
}

template<class _CEditOp>
void IplImageExt::OperateEdit(
	const _CEditOp& editOp,
	int posX,
	int posY,
	int width,
	int height)
{
	int dst_width = m_EffectiveRect.right - m_EffectiveRect.left;
	int dst_height = m_EffectiveRect.bottom - m_EffectiveRect.top;

	//clipping
	if (posX < 0) {
		width += posX;
		posX = 0;
	}
	if (posY < 0) {
		height += posY;
		posY = 0;
	}
	if ((posX + width) > dst_width) {
		width = dst_width - posX;
	}
	if ((posY + height) > dst_height) {
		height = dst_height - posY;
	}

	//
	int i;
	int x,y;
	int next_x;
	int next_y;
	int edit_width;
	int edit_height;

	IplImage* dst;
	UCvPixel* pdst;

	for (y = 0; y < height; y++) {
		next_y = this->GetNextBlockPosY(posY + y);
		edit_height = next_y - (posY + y) + 1;
		if ((y + edit_height) > height) {
			edit_height = height - y;
		}

		for (x = 0; x < width; x++) {
			next_x = this->GetNextBlockPosX(posX + x);
			edit_width = next_x - (posX + x) + 1;
			if ((x + edit_width) > width){
				edit_width = width - x;
			}
		
			dst = GetBlockImgFromPos(posX + x, posY + y);
			if (dst == NULL) {
				CreateBlockFromPos(posX + x, posY + y);
				dst = GetBlockImgFromPos(posX + x, posY + y);
				assert(dst);
			}
			pdst = GetPixelAddress32(
				dst,
				MOD_BLOCK_SIZE(m_EffectiveRect.left + posX + x),
				MOD_BLOCK_SIZE(m_EffectiveRect.top + posY + y));

			for (i = 0; i < edit_height; i++) {
				editOp(pdst, posX + x, posY + y + i, edit_width);
				pdst = GetNextLineAddress32(dst, pdst);
			}
			x += edit_width-1;
		}
		y += edit_height-1;
	}
}

template<class _CBltOp, class _CMaskBlockOp>
void IplImageExt::OperateBlt(
	const _CBltOp& bltOp,
	const _CMaskBlockOp& maskBlockOp,
	IplImage* dst,
	int posX,
	int posY,
	int width,
	int height,
	const IplImageExt* src,
	int startX,
	int startY,
	const IplImageExt* mask_ext)
{
	CvSize src_size;
	if (src) {
		src_size = src->GetSize();
	} else {
		src_size.width = 0; src_size.height = 0;
	}

	if ((startX + width) > src_size.width) {
		width = src_size.width - startX;
	}
	if ((startY + height) > src_size.height) {
		height = src_size.height - startY;
	}
	if ((posX + width) > dst->width) {
		width = dst->width - posX;
	}
	if ((posY + height) > dst->height) {
		height = dst->height - posY;
	}

	int write_offset_y = MOD_BLOCK_SIZE(startY + src->m_EffectiveRect.top);
	int write_offset_x = MOD_BLOCK_SIZE(startX + src->m_EffectiveRect.left);
	int start_block_y = DIV_BLOCK_SIZE(startY + src->m_EffectiveRect.top);
	int start_block_x = DIV_BLOCK_SIZE(startX + src->m_EffectiveRect.left);
	int end_block_y = DIV_BLOCK_SIZE(startY + src->m_EffectiveRect.top + height - 1);
	int end_block_x = DIV_BLOCK_SIZE(startX + src->m_EffectiveRect.left + width - 1);

	int block_x;
	int block_y;
	int write_to_x;
	int write_to_y;
	int start_write_x;
	int start_write_y;
	int write_width;
	int write_height;
	IplImage* block_img;
	IplImage* mask_block_img;

	for (block_y = start_block_y; block_y <= end_block_y; block_y++) {
		if (block_y == start_block_y) {
			write_to_y = posY;
			start_write_y = write_offset_y;
			write_height= IPLEXT_BLOCK_SIZE - write_offset_y;
			if (write_height > height)
				write_height = height;
		} else if (block_y == end_block_y) {
			write_to_y = posY - write_offset_y + MUL_BLOCK_SIZE(end_block_y - start_block_y);
			start_write_y = 0;
			write_height = height + write_offset_y - MUL_BLOCK_SIZE(end_block_y - start_block_y);
		} else {
			write_to_y = posY - write_offset_y + MUL_BLOCK_SIZE(block_y - start_block_y);
			start_write_y = 0;
			write_height = IPLEXT_BLOCK_SIZE;
		}

		for (block_x= start_block_x; block_x <= end_block_x; block_x++) {
			if (block_x == start_block_x) {
				write_to_x = posX;
				start_write_x = write_offset_x;
				write_width = IPLEXT_BLOCK_SIZE - write_offset_x;
				if (write_width > width)
					write_width = width;
			} else if (block_x == end_block_x) {
				write_to_x = posX - write_offset_x + MUL_BLOCK_SIZE(end_block_x - start_block_x);
				start_write_x = 0;
				write_width = width + write_offset_x - MUL_BLOCK_SIZE(end_block_x - start_block_x);
			} else {
				write_to_x = posX - write_offset_x + MUL_BLOCK_SIZE(block_x - start_block_x);
				start_write_x = 0;
				write_width = IPLEXT_BLOCK_SIZE;
			}

			//get mask block
			mask_block_img = NULL;
			if (mask_ext) {
				mask_block_img = mask_ext->GetBlock(block_x, block_y);
				if (maskBlockOp( mask_block_img ))
					continue;
			}

			//write block
			block_img = NULL;
			if (src) {
				block_img = src->GetBlock(block_x, block_y);
			}

			//
			OperateBlt(
				bltOp,
				dst,
				write_to_x,
				write_to_y,
				write_width,
				write_height,
				block_img,
				start_write_x,
				start_write_y,
				mask_block_img);
		}
	}
}

template<class _CBltOp>
void IplImageExt::OperateBlt(
	const _CBltOp& bltOp,
	IplImage* dst,
	int posX,
	int posY,
	int width,
	int height,
	const IplImage* src,
	int startX,
	int startY,
	const IplImage* mask)
{
	int y;
	UCvPixel* psrc = NULL;
	uint8_t* pmask = NULL;
	UCvPixel* pdst = GetPixelAddress32(dst, posX, posY);

	if (src) {
		psrc = GetPixelAddress32(src, startX, startY);
	}
	if (mask) {
		pmask = GetPixelAddress(mask, startX, startY);
	}

	for (y = 0; y < height; y++) {

		bltOp(pdst, psrc, pmask, width);	

		if (src) {
			psrc = GetNextLineAddress32(src, psrc);
		}
		if (mask) {
			pmask = GetNextLineAddress(mask, pmask);
		}
		pdst = GetNextLineAddress32(dst, pdst);
	}
}

template<class _CEditOp>
void IplImageExt::OperateEdit(
	const _CEditOp& editOp,
	IplImage* dst,
	int posX, int posY, int width, int height)
{
	int y;
	UCvPixel* pdst = GetPixelAddress32(dst, posX, posY);
	for (y = 0; y < height; y++) {
		editOp(pdst, posX, posY + y, width);
		pdst = GetNextLineAddress32(dst, pdst);
	}

}

#endif //_IPLIMAGEEXT_H_