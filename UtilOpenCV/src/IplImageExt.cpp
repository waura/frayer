#include "stdafx.h"

#include "IplImageExt.h"

IplImageExt::IplImageExt()
{
	isInit = false;
	isLockImgBlock = false;
}

IplImageExt::~IplImageExt()
{
	if(isInit){
		//ブロックを消去
		ClearAllBlock();
	}
}

//////////////////////
/*!
	initialize
*/
void IplImageExt::Init(int width, int height, int depth, int nChannel)
{
	if(isInit == true){
		ClearAllBlock();
		m_block_Img_Vec.clear();
	}
	isInit = true;

	m_EffectiveRect.top = m_EffectiveRect.left = 0;
	m_EffectiveRect.right = width;
	m_EffectiveRect.bottom = height;
	m_ImgDepth = depth;
	m_ImgNChannel = nChannel;

	//calc block num
	m_block_x = DIV_BLOCK_SIZE(width - 1) + 1;
	m_block_y = DIV_BLOCK_SIZE(height - 1) + 1;

	//create block vector
	int i;
	int block_num = m_block_x*m_block_y; 
	m_block_Img_Vec.reserve( block_num );
	for(i=0; i<block_num; i++){
		m_block_Img_Vec.push_back(NULL);
	}
}

void IplImageExt::InitToEqualSize(const IplImageExt* src, int depth, int nChannel)
{
	if(isInit == true){
		ClearAllBlock();
		m_block_Img_Vec.clear();
	}
	isInit = true;

	//copy src data
	m_EffectiveRect = src->m_EffectiveRect;
	m_ImgDepth = depth;
	m_ImgNChannel = nChannel;
	
	m_block_x = src->m_block_x;
	m_block_y = src->m_block_y;

	//create block vector
	int i;
	int block_num = m_block_x*m_block_y; 
	m_block_Img_Vec.reserve( block_num );
	for(i=0; i<block_num; i++){
		m_block_Img_Vec.push_back(NULL);
	}
}

//////////////////////
/*!
	画像の拡張
	@param[in] cx X軸方向拡張幅
	@param[in] cy Y軸方向拡張幅
	@param[in] extend_code 拡張方向フラグ
*/
void IplImageExt::ExtendImage(int cx, int cy, unsigned int extend_code)
{
	assert( isInit );

	if(extend_code == 0) return;
	//ロック
	isLockImgBlock = true;

	int i,j;

	//X軸方向
	if(cx > 0){
		if((extend_code & IPLEXT_EXTEND_CODE::TO_LEFT) == IPLEXT_EXTEND_CODE::TO_LEFT){
			int extend_width = cx - m_EffectiveRect.left;
			int extend_block_num = DIV_BLOCK_SIZE(extend_width) + 1;
			if(extend_block_num > 0){
				for(i=0; i<m_block_y; i++){
					for(j=0; j<extend_block_num; j++){
						IplImage_Vec::iterator itr = m_block_Img_Vec.begin();
						itr += i*(m_block_x + extend_block_num);
						m_block_Img_Vec.insert(itr, nullptr);
					}
				}
				m_block_x += extend_block_num;
			}
			int new_left = MUL_BLOCK_SIZE(extend_block_num) + m_EffectiveRect.left - cx;
			m_EffectiveRect.right = new_left + m_EffectiveRect.right - m_EffectiveRect.left + cx;
			m_EffectiveRect.left = new_left;

		}
		else if((extend_code & IPLEXT_EXTEND_CODE::TO_RIGHT) == IPLEXT_EXTEND_CODE::TO_RIGHT){
			int extend_width = cx - (MUL_BLOCK_SIZE(m_block_x) - m_EffectiveRect.right);
			int extend_block_num = DIV_BLOCK_SIZE(extend_width) + 1;
			if(extend_block_num > 0){
				for(i=0; i<m_block_y; i++){
					for(j=0; j<extend_block_num; j++){
						IplImage_Vec::iterator itr = m_block_Img_Vec.begin();
						itr += i*(m_block_x + extend_block_num) + m_block_x;
						m_block_Img_Vec.insert(itr, nullptr);
					}
				}
				m_block_x += extend_block_num;
			}
			m_EffectiveRect.right += cx;
		}
	}

	//Y軸方向
	if(cy > 0){
		if((extend_code & IPLEXT_EXTEND_CODE::TO_TOP) == IPLEXT_EXTEND_CODE::TO_TOP){
			int extend_height = cy - m_EffectiveRect.top;
			int extend_block_num = DIV_BLOCK_SIZE(extend_height) + 1;
			if(extend_block_num > 0){
				for(i=0; i<extend_block_num; i++){
					for(j=0; j<m_block_x; j++){
						IplImage_Vec::iterator itr = m_block_Img_Vec.begin();
						m_block_Img_Vec.insert(itr, nullptr);
					}
				}
				m_block_y += extend_block_num;
			}
			int new_top = MUL_BLOCK_SIZE(extend_block_num) + m_EffectiveRect.top - cy;
			m_EffectiveRect.bottom = new_top + m_EffectiveRect.bottom - m_EffectiveRect.top + cy;
			m_EffectiveRect.top = new_top;
		}
		else if((extend_code & IPLEXT_EXTEND_CODE::TO_BOTTOM) == IPLEXT_EXTEND_CODE::TO_BOTTOM){
			int extend_height = cy - (MUL_BLOCK_SIZE(m_block_y) - m_EffectiveRect.bottom);
			int extend_block_num = DIV_BLOCK_SIZE(extend_height) + 1;
			if(extend_block_num > 0){
				for(i=0; i<extend_block_num; i++){
					for(j=0; j<m_block_x; j++){
						m_block_Img_Vec.push_back(nullptr);
					}
				}
				m_block_y += extend_block_num;
			}
			m_EffectiveRect.bottom += cy;
		}
	}

	//ロック解除
	isLockImgBlock = false;
}

//////////////////////
/*!
	全ブロックのメモリ開放
*/
void IplImageExt::ClearAllBlock()
{
	assert( isInit );

	IplImage_Vec::iterator itr = m_block_Img_Vec.begin();
	for(; itr != m_block_Img_Vec.end(); itr++){
		if(*itr){
			cvReleaseImage(&(*itr));
			(*itr) = NULL;
		}
	}

	isInit = false;
}

/////////////////////
/*!
	範囲を指定してブロックのメモリ開放
*/
void IplImageExt::ClearBlock(const LPRECT rect)
{
	int start_block_x = DIV_BLOCK_SIZE(rect->left);
	int start_block_y = DIV_BLOCK_SIZE(rect->top);
	int end_block_x = DIV_BLOCK_SIZE(rect->right);
	int end_block_y = DIV_BLOCK_SIZE(rect->bottom);

	for(int block_y=start_block_y; block_y <= end_block_y; block_y++){
		for(int block_x=start_block_x; block_x <= end_block_x; block_x++){
			IplImage* block = GetBlock(block_x, block_y);
			if(block){
				cvReleaseImage(&block);
				SetBlock(block_x, block_y, NULL);
			}
		}
	}
}

/////////////////////
/*!
	座標(x, y)をふくむブロックを作成する
	@param[in] x
	@param[in] y
*/
void IplImageExt::CreateBlockFromPos(int x, int y)
{
#ifdef _DEBUG
	IplImage *img = GetBlockImgFromPos(x, y);
	assert(img == NULL);
#endif //_DEBUG

	//
	CreateBlock(
		DIV_BLOCK_SIZE(m_EffectiveRect.left + x),
		DIV_BLOCK_SIZE(m_EffectiveRect.top + y));
}

//////////////////////
/*!
	最大有効範囲を返す。
*/
void IplImageExt::GetMaxRangeRect(LPRECT lprc) const
{
	lprc->top = 0;
	lprc->left = 0;
	lprc->bottom = m_block_x-1;
	lprc->right = m_block_y-1;

	IplImage* right_block;
	IplImage* bottom_block;

	int bx,by;

	//上から下へ走査
	for(by=0; by<m_block_y; by++){
		for(bx=0; bx<m_block_x; bx++){
			IplImage* img = GetBlock(bx, by);	
			if(img){
				lprc->top = by;
				goto downtoup;
			}
		}
	}
downtoup:
	//下から上へ走査
	for(by=m_block_y-1; by>=0; by--){
		for(bx=0; bx<m_block_x; bx++){
			bottom_block = GetBlock(bx, by);
			if(bottom_block){
				lprc->bottom = by;
				goto lefttoright;
			}
		}
	}
lefttoright:
	//左から右へ
	for(bx=0; bx<m_block_x; bx++){
		for(by=0; by<m_block_y; by++){
			IplImage* img = GetBlock(bx, by);
			if(img){
				lprc->left = bx;
				goto righttoleft;
			}
		}
	}
righttoleft:
	//右から左へ
	for(bx=m_block_x-1; bx>=0; bx--){
		for(by=0; by<m_block_y; by++){
			right_block = GetBlock(bx, by);
			if(right_block){
				lprc->right = bx;
				goto end;
			}
		}
	}
end:

	lprc->top = MUL_BLOCK_SIZE(lprc->top);
	lprc->left = MUL_BLOCK_SIZE(lprc->left);
	lprc->bottom = MUL_BLOCK_SIZE(lprc->bottom+1);
	lprc->right = MUL_BLOCK_SIZE(lprc->right+1);

	lprc->top -= m_EffectiveRect.top;
	lprc->bottom -= m_EffectiveRect.top;
	lprc->left -= m_EffectiveRect.left;
	lprc->right -= m_EffectiveRect.left;
}

bool IplImageExt::IsFillZeroMask() const
{
	int block_x;
	int block_y;
	IplImage* block;
	for(block_y=0; block_y < m_block_y; block_y++){
		for(block_x=0; block_x < m_block_x; block_x++){
			block = GetBlock( block_x, block_y );
			if(block){
				if( !isFillZeroMask( block ) )
					return false;
			}
		}
	}

	return true;
}

void IplImageExt::GetMaskRect(LPRECT lprc) const
{
	//判定のための初期値
	GetMaxRangeRect(lprc);

	uint8_t data;
	int width = m_EffectiveRect.right - m_EffectiveRect.left;
	int height = m_EffectiveRect.bottom - m_EffectiveRect.top;

	//上から下へ走査
	for(int y=0; y<height; y++){
		for(int x=0; x<width; x++){
			GetMaskData(x, y, &data);
			if(data != 0){
				lprc->top = y;
				goto uptodown;
			}
		}
	}
uptodown:
	//下から上へ走査
	for(int y=height-1; y>=0; y--){
		for(int x=0; x<width; x++){
			GetMaskData(x, y, &data);
			if(data != 0){
				lprc->bottom = y+1;
				goto lefttoright;
			}
		}
	}
	lprc->bottom = 0;
lefttoright:
	//左から右へ走査
	for(int x=0; x<width; x++){
		for(int y=0; y<height; y++){
			GetMaskData(x, y, &data);
			if(data != 0){
				lprc->left = x;
				goto righttoleft;
			}
		}
	}
righttoleft:
	//右から左へ走査
	for(int x=width-1; x>=0; x--){
		for(int y=0; y<height; y++){
			GetMaskData(x, y, &data);
			if(data != 0){
				lprc->right = x+1;
				return;
			}
		}
	}
	lprc->right = 0;
}

/////////////////////////////////
/*!
	位置(x,y)に要素をセットする。
*/
void IplImageExt::SetPixel(
	int x, int y,
	const UCvPixel* src)
{
	assert( isInit );
	assert( m_ImgNChannel >= 3 );

#ifdef _DEBUG
	int width =  m_EffectiveRect.right - m_EffectiveRect.left;
	int height = m_EffectiveRect.bottom - m_EffectiveRect.top;
#endif //_DEBUG
	assert((0 <= x && x < width) && (0 <= y && y< height));

	IplImage *img = GetBlockImgFromPos(x, y);
	if(img == NULL){//ブロックなし		
		//
		CreateBlock(
			DIV_BLOCK_SIZE(m_EffectiveRect.left + x),
			DIV_BLOCK_SIZE(m_EffectiveRect.top + y));
		img = GetBlockImgFromPos(x, y);
	}

	if(img){//ブロックがあれば書き込み
		if(m_ImgNChannel == 4){
			SetPixelToBGRA(
				img,
				MOD_BLOCK_SIZE(m_EffectiveRect.left + x),
				MOD_BLOCK_SIZE(m_EffectiveRect.top + y),
				src);
		}
		else if(m_ImgNChannel == 3){
			SetPixelToBGR(
				img,
				MOD_BLOCK_SIZE(m_EffectiveRect.left + x),
				MOD_BLOCK_SIZE(m_EffectiveRect.top + y),
				src);
		}
		else{
			assert(0);
		}
	}
	else{
		assert(0);
	}
}

inline void IplImageExt::SetPixelAlphaBlend(
	int x, int y,
	const UCvPixel* src)
{
	assert( isInit );
	assert( m_ImgNChannel == 4 );

#ifdef _DEBUG
	int width =  m_EffectiveRect.right - m_EffectiveRect.left;
	int height = m_EffectiveRect.bottom - m_EffectiveRect.top;
#endif //_DEBUG
	assert((0 <= x && x < width) && (0 <= y && y< height));

	IplImage *img = GetBlockImgFromPos(x, y);
	if(img == NULL){//ブロックなし		
		//
		CreateBlock(
			DIV_BLOCK_SIZE(m_EffectiveRect.left + x),
			DIV_BLOCK_SIZE(m_EffectiveRect.top + y));
		img = GetBlockImgFromPos(x, y);
	}

	if(img){//ブロックがあれば書き込み
		UCvPixel tmp;
		GetPixelFromBGRA(
			img,
			MOD_BLOCK_SIZE(m_EffectiveRect.left + x),
			MOD_BLOCK_SIZE(m_EffectiveRect.top + y),
			&tmp);

		AlphaBlendPixel(&tmp, src, 255, &tmp);

		SetPixelToBGRA(
			img,
			MOD_BLOCK_SIZE(m_EffectiveRect.left + x),
			MOD_BLOCK_SIZE(m_EffectiveRect.top + y),
			&tmp);
	}
}

inline void IplImageExt::SetAlpha(int x, int y, int8_t a)
{	
	assert( isInit );
	assert( m_ImgNChannel == 4 );

#ifdef _DEBUG
	int width =  m_EffectiveRect.right - m_EffectiveRect.left;
	int height = m_EffectiveRect.bottom - m_EffectiveRect.top;
#endif //_DEBUG

	assert((0 <= x && x < width) && (0 <= y && y< height));
 
	IplImage *img = GetBlockImgFromPos(x, y);
	if(img == NULL){
		//ブロック作成、書き込み
		CreateBlock(
			DIV_BLOCK_SIZE(m_EffectiveRect.left + x),
			DIV_BLOCK_SIZE(m_EffectiveRect.top + y));
		img = GetBlockImgFromPos(x, y);
	}

	if(img){//ブロックがあれば書き込み
		SetAlphaToBGRA(
			img,
			MOD_BLOCK_SIZE(m_EffectiveRect.left + x),
			MOD_BLOCK_SIZE(m_EffectiveRect.top + y),
			a);
	}
	else{
		assert(0);
	}
}

inline void IplImageExt::SetMaskData(int x, int y, int8_t data)
{
	assert( isInit );
	assert( m_ImgNChannel == 1 );

#ifdef _DEBUG
	int width =  m_EffectiveRect.right - m_EffectiveRect.left;
	int height = m_EffectiveRect.bottom - m_EffectiveRect.top;
#endif //_DEBUG

	assert((0 <= x && x < width) && (0 <= y && y< height));

	IplImage *img = GetBlockImgFromPos(x, y);
	if(img == NULL){
		//ブロック作成、書き込み
		CreateBlock(
			DIV_BLOCK_SIZE(m_EffectiveRect.left + x),
			DIV_BLOCK_SIZE(m_EffectiveRect.top + y));
		img = GetBlockImgFromPos(x, y);
	}

	if(img){//ブロックがあれば書き込み
		SetMaskDataPos(
			img,
			MOD_BLOCK_SIZE(m_EffectiveRect.left + x),
			MOD_BLOCK_SIZE(m_EffectiveRect.top + y),
			data);
	}
	else{
		assert(0);
	}
}

inline void IplImageExt::SetData(int x, int y, int depth, int8_t data)
{
	assert( isInit );

#ifdef _DEBUG
	int width =  m_EffectiveRect.right - m_EffectiveRect.left;
	int height = m_EffectiveRect.bottom - m_EffectiveRect.top;
#endif //_DEBUG

	assert((0 <= x && x < width) && (0 <= y && y< height));

	IplImage *img = GetBlockImgFromPos(x, y);
	if(img == NULL){
		//ブロック作成、書き込み
		CreateBlock(
			DIV_BLOCK_SIZE(m_EffectiveRect.left + x),
			DIV_BLOCK_SIZE(m_EffectiveRect.top + y));
		img = GetBlockImgFromPos(x, y);
	}

	if(img){//ブロックがあれば書き込み
		SetDataPos(
			img,
			MOD_BLOCK_SIZE(m_EffectiveRect.left + x),
			MOD_BLOCK_SIZE(m_EffectiveRect.top + y),
			depth,
			data);
	}
	else{
		assert(0);
	}
}

/////////////////////////////////
/*!
	位置(x,y)から要素を取り出す。
	位置(x,y)にブロックが存在指定いればtrueをそうでなければfalseを返す。
*/
bool IplImageExt::GetPixel(int x, int y, UCvPixel* dst) const
{
	assert( isInit );
	assert( m_ImgNChannel >= 3 );

#ifdef _DEBUG
	int width =  m_EffectiveRect.right - m_EffectiveRect.left;
	int height = m_EffectiveRect.bottom - m_EffectiveRect.top;
#endif //_DEBUG

	assert((0 <= x && x < width) && (0 <= y && y< height));

	IplImage *image = GetBlockImgFromPos(x, y);
	if(image){
		if(m_ImgNChannel == 4){
			GetPixelFromBGRA(
				image,
				MOD_BLOCK_SIZE(m_EffectiveRect.left + x),
				MOD_BLOCK_SIZE(m_EffectiveRect.top + y),
				dst);
		}
		else if(m_ImgNChannel == 3){
			GetPixelFromBGR(
				image,
				MOD_BLOCK_SIZE(m_EffectiveRect.left + x),
				MOD_BLOCK_SIZE(m_EffectiveRect.top + y),
				dst); 
		}
		else{
			assert( 0 );
		}
		return true;
	}
	dst->value = 0;
	return false;
}

inline bool IplImageExt::GetAlpha(int x, int y, uint8_t *a) const 
{
	assert( isInit );
	assert( m_ImgNChannel == 4 );

#ifdef _DEBUG
	int width =  m_EffectiveRect.right - m_EffectiveRect.left;
	int height = m_EffectiveRect.bottom - m_EffectiveRect.top;
#endif //_DEBUG
	assert((0 <= x && x < width) && (0 <= y && y< height));

	IplImage *image = GetBlockImgFromPos(x, y);
	if(image){
		GetAlphaFromBGRA(
			image,
			MOD_BLOCK_SIZE(m_EffectiveRect.left + x),
			MOD_BLOCK_SIZE(m_EffectiveRect.top + y),
			a);
		return true;
	}
	*a = 0;
	return false;
}

inline bool IplImageExt::GetMaskData(int x, int y, uint8_t *data) const
{
	assert( isInit );
	assert( m_ImgNChannel == 1 || m_ImgNChannel == 4 );

	if(m_ImgNChannel == 4){
		return this->GetAlpha(x, y, data);
	}
	else if(m_ImgNChannel == 1){
		int width = m_EffectiveRect.right - m_EffectiveRect.left;
		int height = m_EffectiveRect.bottom - m_EffectiveRect.top;

		assert((0 <= x && x < width) && (0 <= y && y< height));

		IplImage *image = GetBlockImgFromPos(x, y);
		if(image){
			*data = GetMaskDataPos(
				image,
				MOD_BLOCK_SIZE(m_EffectiveRect.left + x),
				MOD_BLOCK_SIZE(m_EffectiveRect.top + y));
			return true;
		}
	}
	*data = 0;
	return false;
}

inline bool IplImageExt::GetData(int x, int y, int depth, uint8_t* data) const
{
	assert( isInit );
	assert( depth < m_ImgNChannel );

#ifdef _DEBUG
	int width =  m_EffectiveRect.right - m_EffectiveRect.left;
	int height = m_EffectiveRect.bottom - m_EffectiveRect.top;
#endif //_DEBUG

	assert((0 <= x && x < width) && (0 <= y && y< height));

	IplImage *image = GetBlockImgFromPos(x, y);
	if(image){
		*data = GetDataPos(
			image,
			MOD_BLOCK_SIZE(m_EffectiveRect.left + x),
			MOD_BLOCK_SIZE(m_EffectiveRect.top + y),
			depth);
		return true;
	}

	*data = 0;
	return false;
}

////////////////////////////////////////////
/*!
	範囲を指定してをimageにコピーして渡す。
*/
void IplImageExt::GetRangeIplImage(
	IplImage* dst,
	int dst_startX,
	int dst_startY,
	int width,
	int height,
	int src_startX,
	int src_startY,
	const IplImage* mask) const
{
	int src_width = m_EffectiveRect.right - m_EffectiveRect.left;
	int src_height = m_EffectiveRect.bottom - m_EffectiveRect.top;

	//clipping
	if(dst_startX < 0){
		width += dst_startX;
		src_startX -= dst_startX;
		dst_startX = 0;
	}
	if(dst_startY < 0){
		height += dst_startY;
		src_startY -= dst_startY;
		dst_startY = 0;
	}
	if(src_startX < 0){
		width += src_startX;
		dst_startX -= src_startX;
		src_startX = 0;
	}
	if(src_startY < 0){
		height += src_startY;
		dst_startY -= src_startY;
		src_startY = 0;
	}

	if((dst_startX + width) > dst->width) width = dst->width - dst_startX;
	if((dst_startY + height) > dst->height) height = dst->height - dst_startY;
	if((src_startX + width) > src_width) width = src_width - src_startX;
	if((src_startY + height) > src_height) height = src_height - src_startY;
 
	int write_offset_y = MOD_BLOCK_SIZE(src_startY + this->m_EffectiveRect.top);
	int write_offset_x = MOD_BLOCK_SIZE(src_startX + this->m_EffectiveRect.left);
	int start_block_y = DIV_BLOCK_SIZE(src_startY + this->m_EffectiveRect.top);
	int start_block_x = DIV_BLOCK_SIZE(src_startX + this->m_EffectiveRect.left);
	int end_block_y = DIV_BLOCK_SIZE(src_startY + this->m_EffectiveRect.top + height - 1);
	int end_block_x = DIV_BLOCK_SIZE(src_startX + this->m_EffectiveRect.left + width - 1);

	int write_to_x;
	int write_to_y;
	int start_write_x;
	int start_write_y;
	int write_width;
	int write_height;
	IplImage* block_img;
	IplImage* mask_block_img;

	for(int block_y = start_block_y; block_y <= end_block_y; block_y++){
		if(block_y == start_block_y){
			write_to_y = dst_startY;
			start_write_y = write_offset_y;
			write_height= IPLEXT_BLOCK_SIZE - write_offset_y;
			if(write_height > height)
				write_height = height;
		}
		else if(block_y == end_block_y){
			write_to_y = dst_startY - write_offset_y + MUL_BLOCK_SIZE(end_block_y - start_block_y);
			start_write_y = 0;
			write_height = height + write_offset_y - MUL_BLOCK_SIZE(end_block_y - start_block_y);
		}
		else{
			write_to_y = dst_startY - write_offset_y + MUL_BLOCK_SIZE(block_y - start_block_y);
			start_write_y = 0;
			write_height = IPLEXT_BLOCK_SIZE;
		}

		for(int block_x= start_block_x; block_x <= end_block_x; block_x++){
			if(block_x == start_block_x){
				write_to_x = dst_startX;
				start_write_x = write_offset_x;
				write_width = IPLEXT_BLOCK_SIZE - write_offset_x;
				if(write_width > width)
					write_width = width;
			}
			else if(block_x == end_block_x){
				write_to_x = dst_startX - write_offset_x + MUL_BLOCK_SIZE(end_block_x - start_block_x);
				start_write_x = 0;
				write_width = width + write_offset_x - MUL_BLOCK_SIZE(end_block_x - start_block_x);
			}
			else{
				write_to_x = dst_startX - write_offset_x + MUL_BLOCK_SIZE(block_x - start_block_x);
				start_write_x = 0;
				write_width = IPLEXT_BLOCK_SIZE;
			}

			//write block
			block_img = this->GetBlock(block_x, block_y);
			if(block_img){
				BltCopy(
					dst,
					write_to_x,
					write_to_y,
					write_width,
					write_height,
					block_img,
					start_write_x,
					start_write_y,
					mask,
					write_to_x,
					write_to_y);
			}
			else{
				FillZero(
					dst,
					write_to_x,
					write_to_y,
					write_width,
					write_height);
			}
		}
	}
}

/////////////////////////////////////////////
/*!
	ブロック位置(block_x, block_y)にimgをコピーする。
*/
void IplImageExt::CopyBlock(IplImage *img, int block_x, int block_y)
{
	IplImage *blk = GetBlock(block_x, block_y);		
	if(blk == NULL){//まだブロックが作成されていないならブロック作成
		CreateBlock(block_x, block_y);
		blk = GetBlock(block_x, block_y);
	}

	if(img){
		cvCopy(img, blk);
	}
}

/////////////////////////////////////////////
/*!
	imageに自身をコピーする
	@param[out] image
*/
void IplImageExt::Copy(IplImageExt *image) const
{
	if(image->isInit == true){
		image->ClearAllBlock();
		image->m_block_Img_Vec.clear();
	}

	image->isInit = true;
	image->m_EffectiveRect = this->m_EffectiveRect;
	image->m_ImgDepth = this->m_ImgDepth;
	image->m_ImgNChannel = this->m_ImgNChannel;

	image->m_block_x = this->m_block_x;
	image->m_block_y = this->m_block_y;

	int i;
	int block_num = m_block_x*m_block_y;
	for(i=0; i<block_num; i++){
		image->m_block_Img_Vec.push_back(NULL);
	}
	
	int bx, by;
	for(by=0; by < m_block_y; by++){
		for(bx=0; bx < m_block_x; bx++){
			IplImage *img = GetBlock(bx, by);
			image->CopyBlock(img, bx, by);
		}
	}
}

//////////////////////////////////////////////
/*!
	矩形範囲を指定色で塗りつぶす
	@param[in] posX 開始位置
	@param[in] posY 開始位置
	@param[in] width 塗りつぶす幅
	@param[in] height 塗りつぶす高さ
	@param[in] r
	@param[in] g
	@param[in] b
	@param[in] a
*/
void IplImageExt::RectFillColor(
	int posX,
	int posY,
	int width,
	int height,
	const UCvPixel* src,
	const IplImageExt* mask_ext,
	int mask_startX,
	int mask_startY)
{
	assert(m_ImgNChannel == 4);

	NormMaskOp normMaskOp;
	if(mask_ext == NULL){
		RectFillColorOp_NoMask rectFillOp;
		rectFillOp.SetFillColor( src );
		this->OperateBlt<RectFillColorOp_NoMask>(
			rectFillOp,
			normMaskOp,
			posX, posY,
			width, height,
			NULL,
			posX, posY,
			mask_ext,
			mask_startX,
			mask_startY);
	}
	else if(mask_ext->GetImgNChannel() == 1){
		RectFillColorOp_Mask1ch rectFillOp;
		rectFillOp.SetFillColor( src );
		this->OperateBlt<RectFillColorOp_Mask1ch>(
			rectFillOp,
			normMaskOp,
			posX, posY,
			width, height,
			NULL,
			posX, posY,
			mask_ext,
			mask_startX,
			mask_startY);
	}
	else if(mask_ext->GetImgNChannel() == 4){
		RectFillColorOp_Mask4ch rectFillOp;
		rectFillOp.SetFillColor( src );
		this->OperateBlt<RectFillColorOp_Mask4ch>(
			rectFillOp,
			normMaskOp,
			posX, posY,
			width, height,
			NULL,
			posX, posY,
			mask_ext,
			mask_startX,
			mask_startY);
	}
	else{
		assert(0);
	}
}

void IplImageExt::RectFillColor(
	int posX,
	int posY,
	int width,
	int height,
	const UCvPixel* src,
	const IplImage* mask,
	int mask_startX,
	int mask_startY)
{
	assert(m_ImgNChannel == 4);

	if(mask == NULL){
		RectFillColorOp_NoMask rectFillOp;
		rectFillOp.SetFillColor( src );
		this->OperateBlt<RectFillColorOp_NoMask>(
			rectFillOp,
			posX, posY,
			width, height,
			(IplImageExt*)NULL,
			posX, posY,
			mask,
			mask_startX,
			mask_startY);
	}
	else if(mask->nChannels == 1){
		RectFillColorOp_Mask1ch rectFillOp;
		rectFillOp.SetFillColor( src );
		this->OperateBlt<RectFillColorOp_Mask1ch>(
			rectFillOp,
			posX, posY,
			width, height,
			(IplImageExt*)NULL,
			posX, posY,
			mask,
			mask_startX,
			mask_startY);
	}
	else if(mask->nChannels == 4){
		RectFillColorOp_Mask4ch rectFillOp;
		rectFillOp.SetFillColor( src );
		this->OperateBlt<RectFillColorOp_Mask4ch>(
			rectFillOp,
			posX, posY,
			width, height,
			(IplImageExt*)NULL,
			posX, posY,
			mask,
			mask_startX,
			mask_startY);
	}
	else{
		assert(0);
	}
}

void IplImageExt::RectFillAlpha(
	int posX,
	int posY,
	int width,
	int height,
	uint8_t a,
	const IplImageExt* mask_ext)
{
	assert(m_ImgNChannel == 4);

	uint8_t mask_data=255;
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			if(mask_ext){
				mask_ext->GetMaskData(x, y, &mask_data);
			}

			if(mask_data != 0){
				this->SetAlpha(posX + x, posY + y, a);
			}
		}
	}
}

void IplImageExt::RectFillMask(
	int posX,
	int posY,
	int width,
	int height,
	uint8_t mask,
	const IplImageExt* mask_ext,
	int mask_startX,
	int mask_startY)
{
	NormMaskOp normMaskOp;
	if(mask_ext == NULL){
		RectFillMaskOp_NoMask rectFillMaskOp;
		rectFillMaskOp.SetFillMaskVal( mask );
		this->OperateBlt<RectFillMaskOp_NoMask>(
			rectFillMaskOp,
			normMaskOp,
			posX, posY,
			width, height,
			NULL,
			posX, posY,
			mask_ext,
			mask_startX,
			mask_startY);
	}
	else if(mask_ext->GetImgNChannel() == 1){
		RectFillMaskOp_Mask1ch rectFillMaskOp;
		rectFillMaskOp.SetFillMaskVal( mask );
		this->OperateBlt<RectFillMaskOp_Mask1ch>(
			rectFillMaskOp,
			normMaskOp,
			posX, posY,
			width, height,
			NULL,
			posX, posY,
			mask_ext,
			mask_startX,
			mask_startY);
	}
	else if(mask_ext->GetImgNChannel() == 4){
		RectFillMaskOp_Mask4ch rectFillMaskOp;
		rectFillMaskOp.SetFillMaskVal( mask );
		this->OperateBlt<RectFillMaskOp_Mask4ch>(
			rectFillMaskOp,
			normMaskOp,
			posX, posY,
			width, height,
			NULL,
			posX, posY,
			mask_ext,
			mask_startX,
			mask_startY);
	}
	else{
		assert(0);
	}
}

void IplImageExt::RectFillZero(
	int posX,
	int posY,
	int width,
	int height)
{
	//
	if(this->GetImgNChannel() == 4){
		RectFillZero4chOp rectFillOp;
		this->OperateBlt<RectFillZero4chOp>(
			rectFillOp,
			posX, posY,
			width, height,
			(IplImage*)NULL,
			posX, posY);
	}
	else if(this->GetImgNChannel() == 1){
		RectFillZero1chOp rectFillOp;
		this->OperateBlt<RectFillZero1chOp>(
			rectFillOp,
			posX, posY,
			width, height,
			(IplImage*)NULL,
			posX, posY);
	}
	else{
		assert(0);
	}
}

void IplImageExt::FillMask(uint8_t mask)
{
	int block_x;
	int block_y;
	IplImage* block;
	for(block_y=0; block_y < m_block_y; block_y++){
		for(block_x=0; block_x < m_block_x; block_x++){
			block = GetBlock( block_x, block_y );
			if((mask != 0) && (block == NULL)){
				CreateBlock( block_x, block_y );
				block = GetBlock( block_x, block_y );
			}

			if(block){
				cvSet(block, cvScalar(mask));
			}
		}
	}
}

//////////////////////////////////////////////
/*!
	IplImageExt型のimageを自分に貼り付ける。
	@param[in] posX
	@param[in] posY
	@param[in] width
	@param[in] height
	@param[in] image 重ねる画像
	@param[in] startX 重ねる画像の貼り付け開始X座標
	@param[in] startY 重ねる画像の貼り付け開始Y座標
*/
void IplImageExt::ImgBlt(
	int posX,
	int posY,
	int width,
	int height,
	const IplImageExt *image,
	int startX,
	int startY,
	IPLEXT_RASTER_CODE raster_code,
	const IplImageExt* mask_ext,
	int mask_startX,
	int mask_startY)
{
	switch(raster_code){
		case IPLEXT_RASTER_CODE::COPY:
			ImgBltCopy(
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask_ext,
				mask_startX,
				mask_startY);
			break;
		case IPLEXT_RASTER_CODE::NOT_COPY:
			ImgBltNotCopy(
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask_ext,
				mask_startX,
				mask_startY);
			break;
		case IPLEXT_RASTER_CODE::ALPHA_BLEND:
			ImgBltAlphaBlend(
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask_ext,
				mask_startX,
				mask_startY);
			break;
		case IPLEXT_RASTER_CODE::COPY_BY_BINARY_MASK:
			ImgBltCopyByBinaryMask(
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask_ext,
				mask_startX,
				mask_startY);
			break;
		default:
			assert(0);
			break;
	}
}

void IplImageExt::ImgBlt(
	int posX,
	int posY,
	int width,
	int height,
	const IplImageExt *image,
	int startX,
	int startY,
	IPLEXT_RASTER_CODE raster_code,
	const IplImage* mask,
	int mask_startX,
	int mask_startY)
{
	switch(raster_code){
		case IPLEXT_RASTER_CODE::COPY:
			ImgBltCopy(
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX,
				mask_startY);
			break;
		case IPLEXT_RASTER_CODE::NOT_COPY:
			ImgBltNotCopy(
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX,
				mask_startY);
			break;
		case IPLEXT_RASTER_CODE::ALPHA_BLEND:
			ImgBltAlphaBlend(
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX,
				mask_startY);
			break;
		case IPLEXT_RASTER_CODE::COPY_BY_BINARY_MASK:
			ImgBltCopyByBinaryMask(
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX,
				mask_startY);
			break;
		default:
			assert(0);
			break;
	}
}

void IplImageExt::ImgBlt(
	int posX,
	int posY,
	int width,
	int height,
	const IplImage *img,
	int startX,
	int startY,
	IPLEXT_RASTER_CODE raster_code,
	const IplImage* mask,
	int mask_startX,
	int mask_startY)
{
	int dst_width = m_EffectiveRect.right - m_EffectiveRect.left;
	int dst_height = m_EffectiveRect.bottom - m_EffectiveRect.top;

	//clipping
	if(posX < 0){
		width += posX;
		startX -= posX;
		posX = 0;
	}
	if(posY < 0){
		height += posY;
		startY -= posY;
		posY = 0;
	}
	if(startX < 0){
		width += startX;
		posX -= startX;
		startX = 0;
	}
	if(startY < 0){
		height += startY;
		posY -= startY;
		startY = 0;
	}
	if((posX + width) > dst_width) width = dst_width - posX;
	if((posY + height) > dst_height) height = dst_height - posY;
	if((startX + width) > img->width) width = img->width - startX;
	if((startY + height) > img->height) height = img->height - startY;

	//
	switch(raster_code){
		case IPLEXT_RASTER_CODE::COPY:
			ImgBltCopy(
				posX, posY, width, height,
				img, startX, startY,
				mask, mask_startX, mask_startY);
			break;
		case IPLEXT_RASTER_CODE::ALPHA_BLEND:
			ImgBltAlphaBlend(
				posX, posY, width, height,
				img, startX, startY,
				mask, mask_startX, mask_startY);
			break;
		case IPLEXT_RASTER_CODE::NOT_COPY:
			ImgBltNotCopy(
				posX, posY, width, height,
				img, startX, startY,
				mask, mask_startX, mask_startY);
			break;
		case IPLEXT_RASTER_CODE::COPY_BY_BINARY_MASK:
			ImgBltCopyByBinaryMask(
				posX, posY, width, height,
				img, startX, startY,
				mask, mask_startX, mask_startY);
			break;
		default:
			assert(0);
			break;
	}
}

void IplImageExt::AlphaBlend(
	int posX, int posY, int width, int height,
	const IplImageExt *under_img, int ui_startX, int ui_startY,
	const IplImageExt *over_img, int oi_startX, int oi_startY,
	const IplImageExt* mask_ext)
{
	assert( m_ImgNChannel == 4 );
	assert( under_img->GetImgNChannel() == 4 );
	assert( over_img->GetImgNChannel() == 4);

	CvSize block_num_size = under_img->GetBlockNumSize();

	//ブロックがきっちり重なっていること
	assert( MOD_BLOCK_SIZE(ui_startY) == MOD_BLOCK_SIZE(oi_startY) );
	assert( MOD_BLOCK_SIZE(ui_startX) == MOD_BLOCK_SIZE(oi_startX) );


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
	IplImage* under_block_img;
	IplImage* over_block_img;
	IplImage* mask_block_img;
	for(block_y = 0; block_y < block_y_num; block_y++){
		if(block_y == 0){
			write_to_y = posY;
			start_write_y = write_offset_y;
			write_height= IPLEXT_BLOCK_SIZE - write_offset_y;
			if(write_height > height)
				write_height = height;
		}
		else if(block_y == block_y_num-1){
			write_to_y = posY - write_offset_y + MUL_BLOCK_SIZE(block_y_num-1);
			start_write_y = 0;
			write_height = height + write_offset_y - MUL_BLOCK_SIZE(block_y_num-1);
		}
		else{
			write_to_y = posY - write_offset_y + MUL_BLOCK_SIZE(block_y);
			start_write_y = 0;
			write_height = IPLEXT_BLOCK_SIZE;
		}

		for(block_x= 0; block_x < block_x_num; block_x++){
			if(block_x == 0){
				write_to_x = posX;
				start_write_x = write_offset_x;
				write_width = IPLEXT_BLOCK_SIZE - write_offset_x;
				if(write_width > width)
					write_width = width;
			}
			else if(block_x == block_x_num-1){
				write_to_x = posX - write_offset_x + MUL_BLOCK_SIZE(block_x_num-1);
				start_write_x = 0;
				write_width = width + write_offset_x - MUL_BLOCK_SIZE(block_x_num-1);
			}
			else{
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

			mask_block_img = NULL;
			if(mask_ext){
				mask_block_img = mask_ext->GetBlock(
					over_start_block_x + block_x,
					over_start_block_y + block_y);
				if(!mask_block_img) continue;
			}

			if(under_block_img && over_block_img){
				this->AlphaBlend(
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
					mask_block_img,
					start_write_x,
					start_write_y);
			}
			else if(under_block_img == NULL){
				this->ImgBlt(
					write_to_x,
					write_to_y,
					write_width,
					write_height,
					over_block_img,
					start_write_x,
					start_write_y,
					IPLEXT_RASTER_CODE::COPY,
					mask_block_img,
					start_write_x,
					start_write_y);
			}
			else if(over_block_img == NULL){
				this->ImgBlt(
					write_to_x,
					write_to_y,
					write_width,
					write_height,
					under_block_img,
					start_write_x,
					start_write_y,
					IPLEXT_RASTER_CODE::COPY,
					NULL);
			}
		}
	}
}

void IplImageExt::AlphaBlend(
	int posX, int posY, int width, int height,
	const IplImage *under_img, int ui_startX, int ui_startY,
	const IplImage *over_img, int oi_startX, int oi_startY,
	const IplImage* mask_img, int mi_startX, int mi_startY)
{
	assert( this->m_ImgNChannel == 4 );
	assert( under_img->nChannels == 4 );
	assert( over_img->nChannels == 4);

	int i;
	int x,y;
	int next_x;
	int blend_width;
	UCvPixel* pdst;
	UCvPixel* punder;
	UCvPixel* pover;

	if(mask_img == NULL){
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){
				next_x = this->GetNextBlockPosX(posX + x);
				blend_width = next_x - (posX + x) + 1;
				
				punder = GetPixelAddress32(under_img, ui_startX + x, ui_startY + y);
				pover = GetPixelAddress32(over_img, oi_startX + x, oi_startY + y);
				pdst = this->GetPixelAddr32(posX + x, posY + y);
				if(pdst == NULL){
					CreateBlockFromPos(posX + x, posY + y);
					pdst = this->GetPixelAddr32(posX + x, posY + y);
					assert(pdst);
				}

				for(i=0; i<blend_width; i++){
					AlphaBlendPixel(punder, pover, 255, pdst);
					pdst++;
					punder++;
					pover++;
				}
				x += blend_width-1;
			}
		}
	}
	else if(mask_img->nChannels == 4){
		UCvPixel* mask;
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){
				next_x = this->GetNextBlockPosX(posX + x);
				blend_width = next_x - (posX + x) + 1;
				
				punder = GetPixelAddress32(under_img, ui_startX + x, ui_startY + y);
				pover = GetPixelAddress32(over_img, oi_startX + x, oi_startY + y);
				mask  = GetPixelAddress32(mask_img, mi_startX, mi_startY + y);
				pdst = this->GetPixelAddr32(posX + x, posY + y);
				if(pdst == NULL){
					CreateBlockFromPos(posX + x, posY + y);
					pdst = this->GetPixelAddr32(posX + x, posY + y);
					assert(pdst);
				}

				for(i=0; i<blend_width; i++){
					if(mask->a == 0){
						pdst++;
						punder++;
						pover++;
						mask++;
						continue;
					}

					AlphaBlendPixel(punder, pover, mask->a, pdst);
					pdst++;
					punder++;
					pover++;
					mask++;
				}
				x += blend_width-1;
			}
		}
	}
	else if(mask_img->nChannels == 1){
		uint8_t* mask;

		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){
				next_x = this->GetNextBlockPosX(posX + x);
				blend_width = next_x - (posX + x) + 1;
				
				punder = GetPixelAddress32(under_img, ui_startX + x, ui_startY + y);
				pover = GetPixelAddress32(over_img, oi_startX + x, oi_startY + y);
				mask  = GetPixelAddress(mask_img, mi_startX, mi_startY + y);
				pdst = this->GetPixelAddr32(posX + x, posY + y);
				if(pdst == NULL){
					CreateBlockFromPos(posX + x, posY + y);
					pdst = this->GetPixelAddr32(posX + x, posY + y);
					assert(pdst);
				}

				for(i=0; i<blend_width; i++){
					if((*mask) == 0){
						pdst++;
						punder++;
						pover++;
						mask++;
						continue;
					}

					AlphaBlendPixel(punder, pover, (*mask), pdst);
					pdst++;
					punder++;
					pover++;
					mask++;
				}
				x += blend_width-1;
			}
		}
	}
	else{
		assert(0);
	}
}

//////////////////////////////////////////////////////
/*!
	1つづつのチャンネルに分割する
	@param[in] dst0
	@param[in] dst1
	@param[in] dst2a
	@param[in] dst3
*/
void IplImageExt::Split(
	IplImageExt* dst0,
	IplImageExt* dst1,
	IplImageExt* dst2,
	IplImageExt* dst3) const
{
	int i,j,k;
	IplImageExt* dstn[4] = {dst0, dst1, dst2, dst3};
	for(i=0; i<4; i++){
		if(dstn[i]){
			if(dstn[i]->isInit == false){ 
				dstn[i]->Init(
					m_EffectiveRect.right - m_EffectiveRect.left,
					m_EffectiveRect.bottom - m_EffectiveRect.top,
					m_ImgDepth,
					1);
			}
		}
	}

	IplImage* dst_block[4];
	for(i=0; i<m_block_y; i++){
		for(j=0; j<m_block_x; j++){
			IplImage* block = this->GetBlock(j, i);
			if(block){
				for(k=0; k<4; k++){
					if(dstn[k]){
						dstn[k]->CreateBlock(j, i);
						dst_block[k] = dstn[k]->GetBlock(j,i);
					}
					else{
						dst_block[k] = NULL;
					}
				}
				cvSplit(block, dst_block[0], dst_block[1], dst_block[2], dst_block[3]);
			}
		}
	}
}

void IplImageExt::Split(
	int posX,
	int posY,
	int width,
	int height,
	IplImageExt* dst0,
	IplImageExt* dst1,
	IplImageExt* dst2,
	IplImageExt* dst3) const
{
	int i;
	int x,y;
	IplImageExt* dstn[4] = {dst0, dst1, dst2, dst3};
	//for(i=0; i<4; i++){
	//	if(dstn[i]){
	//		if(dstn[i]->isInit == false){ 
	//			dstn[i]->Init(
	//				width,
	//				height,
	//				m_ImgDepth,
	//				1);
	//		}
	//	}
	//}

	uint8_t data;
	for(y=0; y<height; y++){
		for(x=0; x<width; x++){
			for(i=0; i<4; i++){
				if(dstn[i]){
					this->GetData(posX+x, posY+y, i, &data);
					dstn[i]->SetMaskData(x, y, data);
				}
			}
		}
	}
}

void IplImageExt::Split(
	int posX,
	int posY,
	int width,
	int height,
	IplImage* dst0,
	IplImage* dst1,
	IplImage* dst2,
	IplImage* dst3) const
{
	assert(GetImgNChannel() == 4);

	int x,y;
	int next_x;
	int blend_width;
	UCvPixel* psrc;
	uint8_t* pdst0 = NULL;
	uint8_t* pdst1 = NULL;
	uint8_t* pdst2 = NULL;
	uint8_t* pdst3 = NULL;

	for(y = 0; y < height; y++){
		for(x = 0; x < width; x++){
			next_x = this->GetNextBlockPosX(posX + x);
			blend_width = next_x - (posX + x) + 1;
			if((x + blend_width) > width)
				blend_width = width - x;
		
			if(dst0){
				pdst0 = GetPixelAddress(dst0, x, y);
			}
			if(dst1){
				pdst1 = GetPixelAddress(dst1, x, y);
			}
			if(dst2){
				pdst2 = GetPixelAddress(dst2, x, y);
			}
			if(dst3){
				pdst3 = GetPixelAddress(dst3, x, y);
			}

			psrc = this->GetPixelAddr32(posX + x, posY + y);

			//split
			if(psrc){
				int i;
				for(i=0; i<blend_width; i++){
					if(pdst0){
						(*pdst0) = psrc->b;
						pdst0++;
					}
					if(pdst1){
						(*pdst1) = psrc->g;
						pdst1++;
					}
					if(pdst2){
						(*pdst2) = psrc->r;
						pdst2++;
					}
					if(pdst3){
						(*pdst3) = psrc->a;
						pdst3++;
					}
					psrc++;
				}
			}
			else{
				if(pdst0)
					memset(pdst0, 0, sizeof(uint8_t)*blend_width);
				if(pdst1)
					memset(pdst1, 0, sizeof(uint8_t)*blend_width);
				if(pdst2)
					memset(pdst2, 0, sizeof(uint8_t)*blend_width);
				if(pdst3)
					memset(pdst3, 0, sizeof(uint8_t)*blend_width);
			}

			x += blend_width-1;
		}
	}
}

void IplImageExt::Merge(
	IplImageExt* src0,
	IplImageExt* src1,
	IplImageExt* src2,
	IplImageExt* src3)
{
	int i,j,k;
	IplImageExt* srcn[4] = {src0, src1, src2, src3};
	IplImage* src_block[4];

	for(i=0; i<m_block_y; i++){
		for(j=0; j<m_block_x; j++){
			for(k=0; k<4; k++){
				if(srcn[k]){
					src_block[k] = srcn[k]->GetBlock(j,i);
				}
				else{
					src_block[k] = NULL;
				}
			}

			if( src_block[0] == NULL &&
				src_block[1] == NULL &&
				src_block[2] == NULL &&
				src_block[3] == NULL )
				continue;

			IplImage* block = this->GetBlock(j, i);
			if(block == NULL){
				this->CreateBlock(j, i);
				block = this->GetBlock(j, i);
			}

			cvMerge(src_block[0], src_block[1], src_block[2], src_block[3], block);
		}
	}
}

void IplImageExt::Merge(
	int posX,
	int posY,
	int width,
	int height,
	IplImageExt* src0,
	IplImageExt* src1,
	IplImageExt* src2,
	IplImageExt* src3)
{
	IplImageExt* srcn[4] = {src0, src1, src2, src3};

	int x,y,i;
	uint8_t data;
	for(y=0; y<height; y++){
		for(x=0; x<width; x++){
			for(i=0; i<4; i++){
				if(srcn[i]){
					srcn[i]->GetMaskData(x, y, &data);
					this->SetData(posX+x, posY+y, i, data);
				}
			}
		}
	}
}

void IplImageExt::Merge(
	int posX,
	int posY,
	int width,
	int height,
	const IplImage *src0,
	const IplImage *src1,
	const IplImage *src2,
	const IplImage *src3)
{
	assert(GetImgNChannel() == 4);

	int x,y;
	int next_x;
	int blend_width;
	UCvPixel* pdst;
	uint8_t* psrc0 = NULL;
	uint8_t* psrc1 = NULL;
	uint8_t* psrc2 = NULL;
	uint8_t* psrc3 = NULL;

	static UCvPixel buf_line[IPLEXT_BLOCK_SIZE];

	for(y = 0; y < height; y++){
		for(x = 0; x < width; x++){
			next_x = this->GetNextBlockPosX(posX + x);
			blend_width = next_x - (posX + x) + 1;
			if((x + blend_width) > width)
				blend_width = width - x;
		
			if(src0){
				psrc0 = GetPixelAddress(src0, x, y);
			}
			if(src1){
				psrc1 = GetPixelAddress(src1, x, y);
			}
			if(src2){
				psrc2 = GetPixelAddress(src2, x, y);
			}
			if(src3){
				psrc3 = GetPixelAddress(src3, x, y);
			}

			//merge
			bool isVisible=false;
			{
				pdst = &(buf_line[0]);
				int i;
				for(i=0; i<blend_width; i++){
					if(psrc0){
						pdst->b = (*psrc0);
						psrc0++;
					}
					if(psrc1){
						pdst->g = (*psrc1);
						psrc1++;
					}
					if(psrc2){
						pdst->r = (*psrc2);
						psrc2++;
					}
					if(psrc3){
						pdst->a = (*psrc3);
						if(pdst->a != 0)
							isVisible = true;
						psrc3++;
					}
					pdst++;
				}
			}

			//blt
			if(isVisible){
				pdst = this->GetPixelAddr32(posX + x, posY + y);
				if(pdst == NULL){
					CreateBlockFromPos(posX + x, posY + y);
					pdst = this->GetPixelAddr32(posX + x, posY + y);
					assert(pdst);
				}
				memcpy(pdst, &(buf_line[0]), blend_width * sizeof(UCvPixel32));
			}

			x += blend_width-1;
		}
	}
}

void IplImageExt::LUT(IplImageExt* src, const CvArr* lut)
{
	assert(src->m_block_x == m_block_x);
	assert(src->m_block_y == m_block_y);

	int i,j;
	for(i=0; i<m_block_y; i++){
		for(j=0; j<m_block_x; j++){
			IplImage* src_block = src->GetBlock(j, i);
			IplImage* block = this->GetBlock(j, i);
			if(src_block == NULL){
				src->CreateBlock(j, i);
				src_block = src->GetBlock(j, i);
			}
			if(block == NULL){
				this->CreateBlock(j, i);
				block = this->GetBlock(j, i);
			}

			cvLUT(src_block, block, lut);
		}
	}
}

void IplImageExt::LUT(
	int posX,
	int posY,
	int width,
	int height,
	const IplImageExt* src,
	int startX,
	int startY,
	const CvArr* lut)
{
	assert(this->GetImgNChannel() == 4);
	assert(src->GetImgNChannel() == 4);

	NormMaskOp normMaskOp;
	LUT4chOp_NoMask lutOp;
	lutOp.SetLUTTable( lut );
	this->OperateBlt<LUT4chOp_NoMask>(
		lutOp,
		posX, posY,
		width, height,
		src,
		startX, startY);
}

void IplImageExt::PL_LUT(
	int posX,
	int posY,
	int width,
	int height,
	const IplImageExt* src,
	int startX,
	int startY,
	const CvArr* lut)
{
	assert(this->GetImgNChannel() == 4);
	assert(src->GetImgNChannel() == 4);

	NormMaskOp normMaskOp;
	PL_LUT4chOp_NoMask lutOp;
	lutOp.SetLUTTable( lut );
	this->OperateBlt<PL_LUT4chOp_NoMask>(
		lutOp,
		posX, posY,
		width, height,
		src,
		startX, startY);
}

void IplImageExt::ImgBltCopy(
	int posX,
	int posY,
	int width,
	int height,
	const IplImageExt *image,
	int startX,
	int startY,
	const IplImageExt* mask_ext,
	int mask_startX,
	int mask_startY)
{
	assert(this->GetImgNChannel() == image->GetImgNChannel());

	NormMaskOp normMaskOp;
	if(image->GetImgNChannel() == 4){
		if(mask_ext == NULL){
			BltCopy4chOp_NoMask bltCopyOp;
			this->OperateBlt<BltCopy4chOp_NoMask, NormMaskOp>(
				bltCopyOp,
				normMaskOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask_ext,
				mask_startX,
				mask_startY);
		}
		else if(mask_ext->GetImgNChannel() == 1){
			BltCopy4chOp_Mask1ch bltCopyOp;
			this->OperateBlt<BltCopy4chOp_Mask1ch, NormMaskOp>(
				bltCopyOp,
				normMaskOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask_ext,
				mask_startX,
				mask_startY);
		}
		else if(mask_ext->GetImgNChannel() == 4){
			BltCopy4chOp_Mask4ch bltCopyOp;
			this->OperateBlt<BltCopy4chOp_Mask4ch>(
				bltCopyOp,
				normMaskOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask_ext,
				mask_startX,
				mask_startY);
		}
		else{
			assert(0);
		}
	}
	else if(image->GetImgNChannel() == 1){
		if(mask_ext == NULL){
			BltCopy1chOp_NoMask bltCopyOp;
			this->OperateBlt<BltCopy1chOp_NoMask, NormMaskOp>(
				bltCopyOp,
				normMaskOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask_ext,
				mask_startX,
				mask_startY);
		}
		else if(mask_ext->GetImgNChannel() == 1){
			BltCopy1chOp_Mask1ch bltCopyOp;
			this->OperateBlt<BltCopy1chOp_Mask1ch, NormMaskOp>(
				bltCopyOp,
				normMaskOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask_ext,
				mask_startX,
				mask_startY);
		}
		else if(mask_ext->GetImgNChannel() == 4){
			BltCopy1chOp_Mask4ch bltCopyOp;
			this->OperateBlt<BltCopy1chOp_Mask4ch>(
				bltCopyOp,
				normMaskOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask_ext,
				mask_startX,
				mask_startY);
		}
		else{
			assert(0);
		}
	}
	else{
		assert(0);
	}
}

void IplImageExt::ImgBltNotCopy(
	int posX,
	int posY,
	int width,
	int height,
	const IplImageExt *image,
	int startX,
	int startY,
	const IplImageExt* mask_ext,
	int mask_startX,
	int mask_startY)
{
	assert(this->GetImgNChannel() == image->GetImgNChannel());

	NormMaskOp normMaskOp;
	if(image->GetImgNChannel() == 4){
		if(mask_ext == NULL){
			BltNotCopy4chOp_NoMask bltNotCopyOp;
			this->OperateBlt<BltNotCopy4chOp_NoMask, NormMaskOp>(
				bltNotCopyOp,
				normMaskOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask_ext,
				mask_startX,
				mask_startY);
		}
		else if(mask_ext->GetImgNChannel() == 1){
			BltNotCopy4chOp_Mask1ch bltNotCopyOp;
			this->OperateBlt<BltNotCopy4chOp_Mask1ch, NormMaskOp>(
				bltNotCopyOp,
				normMaskOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask_ext,
				mask_startX,
				mask_startY);
		}
		else if(mask_ext->GetImgNChannel() == 4){
			BltNotCopy4chOp_Mask4ch bltNotCopyOp;
			this->OperateBlt<BltNotCopy4chOp_Mask4ch>(
				bltNotCopyOp,
				normMaskOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask_ext,
				mask_startX,
				mask_startY);
		}
		else{
			assert(0);
		}
	}
	else if(image->GetImgNChannel() == 1){
		if(mask_ext == NULL){
			BltNotCopy1chOp_NoMask bltNotCopyOp;
			this->OperateBlt<BltNotCopy1chOp_NoMask, NormMaskOp>(
				bltNotCopyOp,
				normMaskOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask_ext,
				mask_startX,
				mask_startY);
		}
		else if(mask_ext->GetImgNChannel() == 1){
			BltNotCopy1chOp_Mask1ch bltNotCopyOp;
			this->OperateBlt<BltNotCopy1chOp_Mask1ch, NormMaskOp>(
				bltNotCopyOp,
				normMaskOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask_ext,
				mask_startX,
				mask_startY);
		}
		else if(mask_ext->GetImgNChannel() == 4){
			BltNotCopy1chOp_Mask4ch bltNotCopyOp;
			this->OperateBlt<BltNotCopy1chOp_Mask4ch>(
				bltNotCopyOp,
				normMaskOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask_ext,
				mask_startX,
				mask_startY);
		}
		else{
			assert(0);
		}
	}
	else{
		assert(0);
	}
}

void IplImageExt::ImgBltAlphaBlend(
	int posX,
	int posY,
	int width,
	int height,
	const IplImageExt* image,
	int startX,
	int startY,
	const IplImageExt* mask_ext,
	int mask_startX,
	int mask_startY)
{
	assert(this->m_ImgNChannel == 4);
	assert(image->GetImgNChannel() == 4);

	NormMaskOp normMaskOp;
	if(mask_ext == NULL){
		BltAlphaBlendOp_NoMask bltOp;
		this->OperateBlt<BltAlphaBlendOp_NoMask, NormMaskOp>(
			bltOp,
			normMaskOp,
			posX, posY,
			width, height,
			image,
			startX, startY,
			mask_ext,
			mask_startX,
			mask_startY);
	}
	else if(mask_ext->GetImgNChannel() == 1){
		BltAlphaBlendOp_Mask1ch bltOp;
		this->OperateBlt<BltAlphaBlendOp_Mask1ch, NormMaskOp>(
			bltOp,
			normMaskOp,
			posX, posY,
			width, height,
			image,
			startX, startY,
			mask_ext,
			mask_startX,
			mask_startY);
	}
	else if(mask_ext->GetImgNChannel() == 4){
		BltAlphaBlendOp_Mask4ch bltOp;
		this->OperateBlt<BltAlphaBlendOp_Mask4ch, NormMaskOp>(
			bltOp,
			normMaskOp,
			posX, posY,
			width, height,
			image,
			startX, startY,
			mask_ext,
			mask_startX,
			mask_startY);
	}
	else{
		assert(0);
	}
}

void IplImageExt::ImgBltCopyByBinaryMask(
	int posX,
	int posY,
	int width,
	int height,
	const IplImageExt *image,
	int startX,
	int startY,
	const IplImageExt* mask_ext,
	int mask_startX,
	int mask_startY)
{
	assert(this->GetImgNChannel() == image->GetImgNChannel());

	NormMaskOp normMaskOp;
	if(image->GetImgNChannel() == 4){
		if(mask_ext == NULL){
			BltCopyByBinaryMask4chOp_NoMask bltCopyOp;
			this->OperateBlt<BltCopyByBinaryMask4chOp_NoMask, NormMaskOp>(
				bltCopyOp,
				normMaskOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask_ext,
				mask_startX,
				mask_startY);
		}
		else if(mask_ext->GetImgNChannel() == 1){
			BltCopyByBinaryMask4chOp_Mask1ch bltCopyOp;
			this->OperateBlt<BltCopyByBinaryMask4chOp_Mask1ch, NormMaskOp>(
				bltCopyOp,
				normMaskOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask_ext,
				mask_startX,
				mask_startY);
		}
		else if(mask_ext->GetImgNChannel() == 4){
			BltCopyByBinaryMask4chOp_Mask4ch bltCopyOp;
			this->OperateBlt<BltCopyByBinaryMask4chOp_Mask4ch>(
				bltCopyOp,
				normMaskOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask_ext,
				mask_startX,
				mask_startY);
		}
		else{
			assert(0);
		}
	}
	else if(image->GetImgNChannel() == 1){
		if(mask_ext == NULL){
			BltCopyByBinaryMask1chOp_NoMask bltCopyOp;
			this->OperateBlt<BltCopyByBinaryMask1chOp_NoMask, NormMaskOp>(
				bltCopyOp,
				normMaskOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask_ext,
				mask_startX,
				mask_startY);
		}
		else if(mask_ext->GetImgNChannel() == 1){
			BltCopyByBinaryMask1chOp_Mask1ch bltCopyOp;
			this->OperateBlt<BltCopyByBinaryMask1chOp_Mask1ch, NormMaskOp>(
				bltCopyOp,
				normMaskOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask_ext,
				mask_startX,
				mask_startY);
		}
		else if(mask_ext->GetImgNChannel() == 4){
			BltCopyByBinaryMask1chOp_Mask4ch bltCopyOp;
			this->OperateBlt<BltCopyByBinaryMask1chOp_Mask4ch>(
				bltCopyOp,
				normMaskOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask_ext,
				mask_startX,
				mask_startY);
		}
		else{
			assert(0);
		}
	}
	else{
		assert(0);
	}
}

///////////////////////
/*!
*/
void IplImageExt::ImgBltCopy(
	int posX,
	int posY,
	int width,
	int height,
	const IplImageExt* image,
	int startX,
	int startY,
	const IplImage* mask,
	int mask_startX,
	int mask_startY)
{
	assert(this->GetImgNChannel() == image->GetImgNChannel());

	if(image->GetImgNChannel() == 4){
		if(mask == NULL){
			BltCopy4chOp_NoMask bltCopyOp;
			this->OperateBlt<BltCopy4chOp_NoMask>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 1){
			BltCopy4chOp_Mask1ch bltCopyOp;
			this->OperateBlt<BltCopy4chOp_Mask1ch>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 4){
			BltCopy4chOp_Mask4ch bltCopyOp;
			this->OperateBlt<BltCopy4chOp_Mask4ch>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else{
			assert(0);
		}
	}
	else if(image->GetImgNChannel() == 1){
		if(mask == NULL){
			BltCopy1chOp_NoMask bltCopyOp;
			this->OperateBlt<BltCopy1chOp_NoMask>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 1){
			BltCopy1chOp_Mask1ch bltCopyOp;
			this->OperateBlt<BltCopy1chOp_Mask1ch>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 4){
			BltCopy1chOp_Mask4ch bltCopyOp;
			this->OperateBlt<BltCopy1chOp_Mask4ch>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else{
			assert(0);
		}
	}
	else{
		assert(0);
	}
}

///////////////////////
/*!
*/
void IplImageExt::ImgBltNotCopy(
	int posX,
	int posY,
	int width,
	int height,
	const IplImageExt* image,
	int startX,
	int startY,
	const IplImage* mask,
	int mask_startX,
	int mask_startY)
{
	assert(this->GetImgNChannel() == image->GetImgNChannel());

	if(image->GetImgNChannel() == 4){
		if(mask == NULL){
			BltNotCopy4chOp_NoMask bltNotCopyOp;
			this->OperateBlt<BltNotCopy4chOp_NoMask>(
				bltNotCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 1){
			BltNotCopy4chOp_Mask1ch bltNotCopyOp;
			this->OperateBlt<BltNotCopy4chOp_Mask1ch>(
				bltNotCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 4){
			BltNotCopy4chOp_Mask4ch bltNotCopyOp;
			this->OperateBlt<BltNotCopy4chOp_Mask4ch>(
				bltNotCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else{
			assert(0);
		}
	}
	else if(image->GetImgNChannel() == 1){
		if(mask == NULL){
			BltNotCopy1chOp_NoMask bltNotCopyOp;
			this->OperateBlt<BltNotCopy1chOp_NoMask>(
				bltNotCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 1){
			BltNotCopy1chOp_Mask1ch bltNotCopyOp;
			this->OperateBlt<BltNotCopy1chOp_Mask1ch>(
				bltNotCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 4){
			BltNotCopy1chOp_Mask4ch bltNotCopyOp;
			this->OperateBlt<BltNotCopy1chOp_Mask4ch>(
				bltNotCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else{
			assert(0);
		}
	}
	else{
		assert(0);
	}
}

///////////////////////
/*!
*/
void IplImageExt::ImgBltAlphaBlend(
	int posX,
	int posY,
	int width,
	int height,
	const IplImageExt* image,
	int startX,
	int startY,
	const IplImage* mask,
	int mask_startX,
	int mask_startY)
{
	assert(this->m_ImgNChannel == 4);
	assert(image->GetImgNChannel() == 4);

	if(mask == NULL){
		BltAlphaBlendOp_NoMask bltOp;
		this->OperateBlt<BltAlphaBlendOp_NoMask>(
			bltOp,
			posX, posY,
			width, height,
			image,
			startX, startY,
			mask,
			mask_startX, mask_startY);
	}
	else if(mask->nChannels == 1){
		BltAlphaBlendOp_Mask1ch bltOp;
		this->OperateBlt<BltAlphaBlendOp_Mask1ch>(
			bltOp,
			posX, posY,
			width, height,
			image,
			startX, startY,
			mask,
			mask_startX, mask_startY);
	}
	else if(mask->nChannels == 4){
		BltAlphaBlendOp_Mask4ch bltOp;
		this->OperateBlt<BltAlphaBlendOp_Mask4ch>(
			bltOp,
			posX, posY,
			width, height,
			image,
			startX, startY,
			mask,
			mask_startX, mask_startY);
	}
	else{
		assert(0);
	}
}

///////////////////////
/*!
*/
void IplImageExt::ImgBltCopyByBinaryMask(
	int posX,
	int posY,
	int width,
	int height,
	const IplImageExt* image,
	int startX,
	int startY,
	const IplImage* mask,
	int mask_startX,
	int mask_startY)
{
	assert(this->GetImgNChannel() == image->GetImgNChannel());

	if(image->GetImgNChannel() == 4){
		if(mask == NULL){
			BltCopyByBinaryMask4chOp_NoMask bltCopyOp;
			this->OperateBlt<BltCopyByBinaryMask4chOp_NoMask>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 1){
			BltCopyByBinaryMask4chOp_Mask1ch bltCopyOp;
			this->OperateBlt<BltCopyByBinaryMask4chOp_Mask1ch>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 4){
			BltCopyByBinaryMask4chOp_Mask4ch bltCopyOp;
			this->OperateBlt<BltCopyByBinaryMask4chOp_Mask4ch>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else{
			assert(0);
		}
	}
	else if(image->GetImgNChannel() == 1){
		if(mask == NULL){
			BltCopyByBinaryMask1chOp_NoMask bltCopyOp;
			this->OperateBlt<BltCopyByBinaryMask1chOp_NoMask>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 1){
			BltCopyByBinaryMask1chOp_Mask1ch bltCopyOp;
			this->OperateBlt<BltCopyByBinaryMask1chOp_Mask1ch>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 4){
			BltCopyByBinaryMask1chOp_Mask4ch bltCopyOp;
			this->OperateBlt<BltCopyByBinaryMask1chOp_Mask4ch>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else{
			assert(0);
		}
	}
	else{
		assert(0);
	}
}

///////////////////////
/*!
*/
void IplImageExt::ImgBltCopy(
	int posX,
	int posY,
	int width,
	int height,
	const IplImage* image,
	int startX,
	int startY,
	const IplImage* mask,
	int mask_startX,
	int mask_startY)
{
	assert(this->GetImgNChannel() == image->nChannels);

	if(image->nChannels == 4){
		if(mask == NULL){
			BltCopy4chOp_NoMask bltCopyOp;
			this->OperateBlt<BltCopy4chOp_NoMask>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 1){
			BltCopy4chOp_Mask1ch bltCopyOp;
			this->OperateBlt<BltCopy4chOp_Mask1ch>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 4){
			BltCopy4chOp_Mask4ch bltCopyOp;
			this->OperateBlt<BltCopy4chOp_Mask4ch>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else{
			assert(0);
		}
	}
	else if(image->nChannels == 1){
		if(mask == NULL){
			BltCopy1chOp_NoMask bltCopyOp;
			this->OperateBlt<BltCopy1chOp_NoMask>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 1){
			BltCopy1chOp_Mask1ch bltCopyOp;
			this->OperateBlt<BltCopy1chOp_Mask1ch>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 4){
			BltCopy1chOp_Mask4ch bltCopyOp;
			this->OperateBlt<BltCopy1chOp_Mask4ch>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else{
			assert(0);
		}
	}
	else{
		assert(0);
	}
}

///////////////////////
/*!
*/
void IplImageExt::ImgBltNotCopy(
	int posX,
	int posY,
	int width,
	int height,
	const IplImage* image,
	int startX,
	int startY,
	const IplImage* mask,
	int mask_startX,
	int mask_startY)
{
	assert(this->GetImgNChannel() == image->nChannels);

	if(image->nChannels == 4){
		if(mask == NULL){
			BltNotCopy4chOp_NoMask bltNotCopyOp;
			this->OperateBlt<BltNotCopy4chOp_NoMask>(
				bltNotCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 1){
			BltNotCopy4chOp_Mask1ch bltNotCopyOp;
			this->OperateBlt<BltNotCopy4chOp_Mask1ch>(
				bltNotCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 4){
			BltNotCopy4chOp_Mask4ch bltNotCopyOp;
			this->OperateBlt<BltNotCopy4chOp_Mask4ch>(
				bltNotCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else{
			assert(0);
		}
	}
	else if(image->nChannels == 1){
		if(mask == NULL){
			BltNotCopy1chOp_NoMask bltNotCopyOp;
			this->OperateBlt<BltNotCopy1chOp_NoMask>(
				bltNotCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 1){
			BltNotCopy1chOp_Mask1ch bltNotCopyOp;
			this->OperateBlt<BltNotCopy1chOp_Mask1ch>(
				bltNotCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 4){
			BltNotCopy1chOp_Mask4ch bltNotCopyOp;
			this->OperateBlt<BltNotCopy1chOp_Mask4ch>(
				bltNotCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else{
			assert(0);
		}
	}
	else{
		assert(0);
	}
}

///////////////////////
/*!
*/
void IplImageExt::ImgBltAlphaBlend(
	int posX,
	int posY,
	int width,
	int height,
	const IplImage* image,
	int startX,
	int startY,
	const IplImage* mask,
	int mask_startX,
	int mask_startY)
{
	assert(this->m_ImgNChannel == 4);
	assert(image->nChannels == 4);

	if(mask == NULL){
		BltAlphaBlendOp_NoMask bltOp;
		this->OperateBlt<BltAlphaBlendOp_NoMask>(
			bltOp,
			posX, posY,
			width, height,
			image,
			startX, startY,
			mask,
			mask_startX, mask_startY);
	}
	else if(mask->nChannels == 1){
		BltAlphaBlendOp_Mask1ch bltOp;
		this->OperateBlt<BltAlphaBlendOp_Mask1ch>(
			bltOp,
			posX, posY,
			width, height,
			image,
			startX, startY,
			mask,
			mask_startX, mask_startY);
	}
	else if(mask->nChannels == 4){
		BltAlphaBlendOp_Mask4ch bltOp;
		this->OperateBlt<BltAlphaBlendOp_Mask4ch>(
			bltOp,
			posX, posY,
			width, height,
			image,
			startX, startY,
			mask,
			mask_startX, mask_startY);
	}
	else{
		assert(0);
	}
}

///////////////////////
/*!
*/
void IplImageExt::ImgBltCopyByBinaryMask(
	int posX,
	int posY,
	int width,
	int height,
	const IplImage* image,
	int startX,
	int startY,
	const IplImage* mask,
	int mask_startX,
	int mask_startY)
{
	assert(this->GetImgNChannel() == image->nChannels);

	if(image->nChannels == 4){
		if(mask == NULL){
			BltCopyByBinaryMask4chOp_NoMask bltCopyOp;
			this->OperateBlt<BltCopyByBinaryMask4chOp_NoMask>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 1){
			BltCopyByBinaryMask4chOp_Mask1ch bltCopyOp;
			this->OperateBlt<BltCopyByBinaryMask4chOp_Mask1ch>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 4){
			BltCopyByBinaryMask4chOp_Mask4ch bltCopyOp;
			this->OperateBlt<BltCopyByBinaryMask4chOp_Mask4ch>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else{
			assert(0);
		}
	}
	else if(image->nChannels == 1){
		if(mask == NULL){
			BltCopyByBinaryMask1chOp_NoMask bltCopyOp;
			this->OperateBlt<BltCopyByBinaryMask1chOp_NoMask>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 1){
			BltCopyByBinaryMask1chOp_Mask1ch bltCopyOp;
			this->OperateBlt<BltCopyByBinaryMask1chOp_Mask1ch>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else if(mask->nChannels == 4){
			BltCopyByBinaryMask1chOp_Mask4ch bltCopyOp;
			this->OperateBlt<BltCopyByBinaryMask1chOp_Mask4ch>(
				bltCopyOp,
				posX, posY,
				width, height,
				image,
				startX, startY,
				mask,
				mask_startX, mask_startY);
		}
		else{
			assert(0);
		}
	}
	else{
		assert(0);
	}
}

///////////////////////
/*!
	位置(x,y)のブロックへのポインタを取り出す
	@param[in] x 画像X座標
	@param[in] y 画像Y座標
	@return ブロック画像
*/
inline IplImage* IplImageExt::GetBlockImgFromPos(int x, int y) const
{
	assert(isInit);

	return GetBlock(
		DIV_BLOCK_SIZE(x + m_EffectiveRect.left),
		DIV_BLOCK_SIZE(y + m_EffectiveRect.top));
}

///////////////////////
/*!
	block_x列 block_y行目のブロックを取り出す。
	block_x(0~ block_y(0~
	@param[in] block_x 列
	@param[in] block_y 行
	@return ブロック画像
*/
inline IplImage* IplImageExt::GetBlock(int block_x, int block_y) const
{
	assert(isInit);

	while(isLockImgBlock);

	assert((0 <= block_x && block_x <= m_block_x-1) &&
		( 0 <= block_y && block_y <= m_block_y-1));
	return m_block_Img_Vec[block_y*m_block_x + block_x];
}

/////////////////////////
/*!
	block_x列 block_y行目のブロックをセットする
	block_x(0~ block_y(0~
	@param[in] block_x 列
	@param[in] block_y 行
	@param[in,out] block セットするブロック画像
*/
inline void IplImageExt::SetBlock(int block_x, int block_y, IplImage* block)
{
	assert(isInit);
	assert( 0 <= block_x && block_x <= m_block_x-1);
	assert( 0 <= block_y && block_y <= m_block_y-1);

	while(isLockImgBlock);

	m_block_Img_Vec[block_y*m_block_x + block_x] = block;
}

///////////////////////////////////////
/*!
	ブロック位置(block_x, block_y)のブロックを作成する。
*/
inline void IplImageExt::CreateBlock(int block_x, int block_y)
{
	assert(isInit);
	while(isLockImgBlock);

	CvSize size;
	size.height = size.width = IPLEXT_BLOCK_SIZE;

	IplImage *blk = cvCreateImage(size, m_ImgDepth, m_ImgNChannel);
	assert(blk);

	//zero clear
	cvSet(blk, cvScalar(0));

	m_block_Img_Vec[block_y*m_block_x + block_x] = blk;
}
