#ifndef _IPLEXTOPERATE_H_
#define _IPLEXTOPERATE_H_

#include "UtilOpenCVType.h"
#include "UtilOpenCVColor.h"
#include "UtilOpenCVPixel.h"

////////////////////////////////////////////////////////////
// Mask Operator
////////////////////////////////////////////////////////////
class NormMaskOp {
public:
	inline bool operator()(IplImage* mask) const {
		return (mask == NULL);
	}
};

class NotMaskOp {
public:
	inline bool operator()(IplImage* mask) const {
		return (mask != NULL);
	}
};

class AllMaskOp {
public:
	inline bool operator()(IplImage* mask) const {
		return false;
	}
};

/////////////////////////////////////////////////////////////
// RectFillMask Operator
/////////////////////////////////////////////////////////////
class RectFillColorOp_NoMask {
public:
	inline void SetFillColor(const UCvPixel* pix){
		m_fill_val.value = pix->value;
	}

	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask == NULL);
		assert(src == NULL);

		UCvPixel32* pdst = (UCvPixel32*) dst;

		int i;
		for(i=0; i<width; i++){
			pdst->value = m_fill_val.value;
			pdst++;
		}
	}
private:
	UCvPixel32 m_fill_val;
};

class RectFillColorOp_Mask4ch {
public:
	inline void SetFillColor(const UCvPixel* pix){
		m_fill_val.value = pix->value;
	}

	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask);
		assert(src == NULL);

		UCvPixel32* pdst = (UCvPixel32*) dst;
		UCvPixel32* pmask = (UCvPixel32*) mask;

		int i;
		for (i=0; i<width; i++) {
			if (pmask->a == 255) {
				pdst->value = m_fill_val.value;
			} else if (pmask->a != 0) {
				pdst->value = m_fill_val.value;
				pdst->a = (pdst->a * pmask->a) >> 8;
			}

			pdst++;
			pmask++;
		}
	}
private:
	UCvPixel32 m_fill_val;
};

class RectFillColorOp_Mask1ch {
public:
	inline void SetFillColor(const UCvPixel* pix){
		m_fill_val.value = pix->value;
	}

	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask);
		assert(src == NULL);

		UCvPixel32* pdst = (UCvPixel32*) dst;
		uint8_t* pmask = (uint8_t*) mask;

		int i;
		for(i=0; i<width; i++){
			if ((*pmask) == 255) {
				pdst->value = m_fill_val.value;
			} else if((*pmask) != 0) {
				pdst->value = m_fill_val.value;
				pdst->a = (pdst->a * (*pmask)) >> 8;
			}

			pdst++;
			pmask++;
		}
	}
private:
	UCvPixel32 m_fill_val;
};

/////////////////////////////////////////////////////////////
// RectFillMask Operator
/////////////////////////////////////////////////////////////
class RectFillMaskOp_NoMask {
public:
	inline void SetFillMaskVal(uint8_t val){
		m_fill_val = val;
	}

	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask == NULL);
		assert(src == NULL);

		uint8_t* pdst = (uint8_t*) dst;

		int i;
		for(i=0; i<width; i++){
			(*pdst) = m_fill_val;
			pdst++;
		}
	}
private:
	uint8_t m_fill_val;
};

class RectFillMaskOp_Mask4ch {
public:
	inline void SetFillMaskVal(uint8_t val){
		m_fill_val = val;
	}

	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask);
		assert(src == NULL);

		uint8_t* pdst = (uint8_t*) dst;
		UCvPixel* pmask = (UCvPixel*) mask;

		int i;
		for(i=0; i<width; i++){
			if(pmask->a != 0)
				(*pdst) = m_fill_val;
			pdst++;
			pmask++;
		}
	}
private:
	uint8_t m_fill_val;
};

class RectFillMaskOp_Mask1ch {
public:
	inline void SetFillMaskVal(uint8_t val){
		m_fill_val = val;
	}

	/*!
		@param[out] dst destination image, 1ch
		@param[in] src unused
		@param[in] mask mask image, 1ch
		@param[in] width write width
	*/
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask);
		assert(src == NULL);

		uint8_t* pdst = (uint8_t*) dst;
		uint8_t* pmask = (uint8_t*) mask;

		int i;
		for(i=0; i<width; i++){
			if((*pmask) != 0)
				(*pdst) = m_fill_val;
			pdst++;
			pmask++;
		}
	}
private:
	uint8_t m_fill_val;
};

/////////////////////////////////////////////////////////////
// RectFillZero Operator
/////////////////////////////////////////////////////////////
class RectFillZero4chOp {
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask == NULL);
		assert(src == NULL);

		memset(dst, 0, sizeof(UCvPixel32)*width);
	}
};

class RectFillZero1chOp {
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask == NULL);
		assert(src == NULL);

		memset(dst, 0, sizeof(uint8_t)*width);
	}
};

//////////////////////////////////////////////////////
// BltCopy 4ch Operator
//////////////////////////////////////////////////////
class BltCopy4chOp_NoMask {
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask == NULL);

		if(src){
			UCvPixel32* pdst = (UCvPixel32*) dst;
			UCvPixel32* psrc = (UCvPixel32*) src;

			memcpy(pdst, psrc, sizeof(UCvPixel32)*width);
		}
	}
};

class BltCopy4chOp_Mask4ch {
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask);

		if(src){
			UCvPixel32* pdst = (UCvPixel32*) dst;
			UCvPixel32* psrc = (UCvPixel32*) src;
			UCvPixel32* pmask = (UCvPixel32*) mask;

			int i;
			for(i=0; i<width; i++){
				if(pmask->a == 255){
					pdst->value = psrc->value;
				}
				else if(pmask->a != 0){
					AlphaBlendPixel(pdst, psrc, pmask->a, pdst);
				}

				pdst++;
				psrc++;
				pmask++;
			}
		}
	}
};

class BltCopy4chOp_Mask1ch {
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask);

		if(src){
			UCvPixel32* pdst = (UCvPixel32*) dst;
			UCvPixel32* psrc = (UCvPixel32*) src;
			uint8_t* pmask = (uint8_t*) mask;

			int i;
			for(i=0; i<width; i++){
				if((*pmask) == 255){
					pdst->value = psrc->value;
				}
				else if((*pmask) != 0){
					AlphaBlendPixel(pdst, psrc, (*pmask), pdst);
				}

				pdst++;
				psrc++;
				pmask++;
			}
		}
	}
};

//////////////////////////////////////////////////////
// BltCopy 1ch Operator
//////////////////////////////////////////////////////
class BltCopy1chOp_NoMask {
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask == NULL);

		uint8_t* pdst = (uint8_t*) dst;
		uint8_t* psrc = (uint8_t*) src;

		if(psrc == NULL){
			memset(pdst, 0, sizeof(uint8_t)*width);
			return;
		}

		memcpy(pdst, psrc, sizeof(uint8_t)*width);
	}
};

class BltCopy1chOp_Mask4ch {
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask);

		uint8_t* pdst = (uint8_t*) dst;
		uint8_t* psrc = (uint8_t*) src;
		UCvPixel32* pmask = (UCvPixel32*) mask;

		if(psrc == NULL){
			memset(pdst, 0, sizeof(uint8_t)*width);
			return;
		}

		int i;
		for(i=0; i<width; i++){
			if(pmask->a == 255){
				(*pdst) = (*psrc);
			}
			else if(pmask->a != 0){
				(*pdst) = ((*psrc) * pmask->a) >> 8;
			}

			pdst++;
			psrc++;
			pmask++;
		}
	}
};

class BltCopy1chOp_Mask1ch {
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask);

		uint8_t* pdst = (uint8_t*) dst;
		uint8_t* psrc = (uint8_t*) src;
		uint8_t* pmask = (uint8_t*) mask;

		if(psrc == NULL){
			memset(pdst, 0, sizeof(uint8_t)*width);
			return;
		}

		int i;
		for(i=0; i<width; i++){
			if((*pmask) == 255){
				(*pdst) = (*psrc);
			}
			else if((*pmask) != 0){
				(*pdst) = ((*psrc) * (*pmask)) >> 8;
			}

			pdst++;
			psrc++;
			pmask++;
		}
	}
};

//////////////////////////////////////////////////////
// BltNotCopy 4ch Operator
//////////////////////////////////////////////////////
class BltNotCopy4chOp_NoMask {
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask == NULL);

		UCvPixel32* pdst = (UCvPixel32*) dst;
		UCvPixel32* psrc = (UCvPixel32*) src;

		if(psrc == NULL){
			memset(pdst, 0, sizeof(UCvPixel32)*width);
			return;
		}

		int i;
		for(i=0; i<width; i++){
			pdst->value = ~psrc->value;
		}
	}
};

class BltNotCopy4chOp_Mask4ch {
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask);

		UCvPixel32* pdst = (UCvPixel32*) dst;
		UCvPixel32* psrc = (UCvPixel32*) src;
		UCvPixel32* pmask = (UCvPixel32*) mask;

		if(psrc == NULL){
			memset(pdst, 0, sizeof(UCvPixel32)*width);
			return;
		}

		int i;
		UCvPixel tmp;
		for(i=0; i<width; i++){
			if(pmask->a == 255){
				pdst->value = ~psrc->value;
			}
			else if(pmask->a != 0){
				tmp.value = ~psrc->value;
				AlphaBlendPixel(pdst, &tmp, pmask->a, pdst);
			}

			pdst++;
			psrc++;
			pmask++;
		}
	}
};

class BltNotCopy4chOp_Mask1ch {
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask);

		UCvPixel32* pdst = (UCvPixel32*) dst;
		UCvPixel32* psrc = (UCvPixel32*) src;
		uint8_t* pmask = (uint8_t*) mask;

		if(psrc == NULL){
			memset(pdst, 0, sizeof(UCvPixel32)*width);
			return;
		}

		int i;
		UCvPixel tmp;
		for(i=0; i<width; i++){
			if((*pmask) == 255){
				pdst->value = ~psrc->value;
			}
			else if((*pmask) != 0){
				tmp.value = ~psrc->value;
				AlphaBlendPixel(pdst, &tmp, (*pmask), pdst);
			}

			pdst++;
			psrc++;
			pmask++;
		}
	}
};

//////////////////////////////////////////////////////
// BltNotCopy 1ch Operator
//////////////////////////////////////////////////////
class BltNotCopy1chOp_NoMask {
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(src);
		assert(mask == NULL);

		uint8_t* pdst = (uint8_t*) dst;
		uint8_t* psrc = (uint8_t*) src;

		int i;
		for(i=0; i<width; i++){
			(*pdst) = ~(*psrc);
		}
	}
};

class BltNotCopy1chOp_Mask4ch {
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(src);
		assert(mask);

		uint8_t* pdst = (uint8_t*) dst;
		uint8_t* psrc = (uint8_t*) src;
		UCvPixel32* pmask = (UCvPixel32*) mask;

		int i;
		for(i=0; i<width; i++){
			if(pmask->a == 255){
				(*pdst) = ~(*psrc);
			}
			else if(pmask->a != 0){
				(*pdst) = ((~(*psrc)) * pmask->a) >> 8;
			}

			pdst++;
			psrc++;
			pmask++;
		}
	}
};

class BltNotCopy1chOp_Mask1ch {
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(src);
		assert(mask);

		uint8_t* pdst = (uint8_t*) dst;
		uint8_t* psrc = (uint8_t*) src;
		uint8_t* pmask = (uint8_t*) mask;

		int i;
		for(i=0; i<width; i++){
			if((*pmask) == 255){
				(*pdst) = ~(*psrc);
			}
			else if((*pmask) != 0){
				(*pdst) = ((~(*psrc)) * (*pmask)) >> 8;
			}

			pdst++;
			psrc++;
			pmask++;
		}
	}
};

//////////////////////////////////////////////////////
// BltAlphaBlend Operator
//////////////////////////////////////////////////////
class BltAlphaBlendOp_NoMask {
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		if(src == NULL)
			return;

		UCvPixel* pdst = (UCvPixel*) dst;
		UCvPixel* psrc = (UCvPixel*) src;

		int i;
		for(i=0; i<width; i++){
			if((psrc->a == 255) || (pdst->a == 0)){
				pdst->value = psrc->value;
			}
			else if(psrc->a == 0){
			}
			else if(pdst->a == 255){
				uint8_t ra = ~psrc->a;
				pdst->b = (psrc->b*psrc->a + pdst->b*ra)>>8;
				pdst->g = (psrc->g*psrc->a + pdst->g*ra)>>8;
				pdst->r = (psrc->r*psrc->a + pdst->r*ra)>>8;
			}
			else{
				uint8_t ra = ~psrc->a;
				uint8_t alpha = 255 - (ra*(255 - pdst->a)>>8);
				double inv_alpha = 1.0/alpha;
				pdst->b = (psrc->b*psrc->a + ((pdst->b*pdst->a*ra)>>8))*inv_alpha;
				pdst->g = (psrc->g*psrc->a + ((pdst->g*pdst->a*ra)>>8))*inv_alpha;
				pdst->r = (psrc->r*psrc->a + ((pdst->r*pdst->a*ra)>>8))*inv_alpha;
				pdst->a = alpha;
			}
			pdst++;
			psrc++;
		}
	}
};

class BltAlphaBlendOp_Mask4ch {
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(mask == NULL);

		if(src == NULL)
			return;

		UCvPixel* pdst = (UCvPixel*) dst;
		UCvPixel* psrc = (UCvPixel*) src;
		UCvPixel* pmask = (UCvPixel*) mask;

		int i;
		for(i=0; i<width; i++){
			uint8_t over_alpha = (psrc->a * pmask->a) >> 8;
			if((over_alpha == 255) || (pdst->a == 0)){
				pdst->value = psrc->value;
			}
			else if(over_alpha == 0){
			}
			else if(pdst->a == 255){
				uint8_t ra = ~over_alpha;
				pdst->b = (psrc->b*over_alpha + pdst->b*ra)>>8;
				pdst->g = (psrc->g*over_alpha + pdst->g*ra)>>8;
				pdst->r = (psrc->r*over_alpha + pdst->r*ra)>>8;
			}
			else{
				uint8_t ra = ~over_alpha;
				uint8_t alpha = 255 - (ra*(255 - pdst->a)>>8);
				double inv_alpha = 1.0/alpha;
				pdst->b = (psrc->b*psrc->a + ((pdst->b*pdst->a*ra)>>8))*inv_alpha;
				pdst->g = (psrc->g*psrc->a + ((pdst->g*pdst->a*ra)>>8))*inv_alpha;
				pdst->r = (psrc->r*psrc->a + ((pdst->r*pdst->a*ra)>>8))*inv_alpha;
				pdst->a = alpha;
			}
			pdst++;
			psrc++;
		}
	}
};

class BltAlphaBlendOp_Mask1ch {
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		if(src == NULL)
			return;

		UCvPixel* pdst = (UCvPixel*) dst;
		UCvPixel* psrc = (UCvPixel*) src;
		uint8_t* pmask = (uint8_t*) mask;

		int i;
		for(i=0; i<width; i++){
			uint8_t over_alpha = (psrc->a * (*pmask)) >> 8;
			if((over_alpha == 255) || (pdst->a == 0)){
				pdst->value = psrc->value;
			}
			else if(over_alpha == 0){
			}
			else if(pdst->a == 255){
				uint8_t ra = ~over_alpha;
				pdst->b = (psrc->b*over_alpha + pdst->b*ra)>>8;
				pdst->g = (psrc->g*over_alpha + pdst->g*ra)>>8;
				pdst->r = (psrc->r*over_alpha + pdst->r*ra)>>8;
			}
			else{
				uint8_t ra = ~over_alpha;
				uint8_t alpha = 255 - (ra*(255 - pdst->a)>>8);
				double inv_alpha = 1.0/alpha;
				pdst->b = (psrc->b*psrc->a + ((pdst->b*pdst->a*ra)>>8))*inv_alpha;
				pdst->g = (psrc->g*psrc->a + ((pdst->g*pdst->a*ra)>>8))*inv_alpha;
				pdst->r = (psrc->r*psrc->a + ((pdst->r*pdst->a*ra)>>8))*inv_alpha;
				pdst->a = alpha;
			}
			pdst++;
			psrc++;
		}
	}
};

//////////////////////////////////////////////////////
// BltCopy By Binary 4ch Operator
//////////////////////////////////////////////////////
class BltCopyByBinaryMask4chOp_NoMask {
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask == NULL);

		UCvPixel32* pdst = (UCvPixel32*) dst;
		UCvPixel32* psrc = (UCvPixel32*) src;

		if(psrc == NULL){
			memset(pdst, 0, sizeof(UCvPixel32)*width);
			return;
		}

		memcpy(pdst, psrc, sizeof(UCvPixel32)*width);
	}
};

class BltCopyByBinaryMask4chOp_Mask4ch {
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask);

		UCvPixel32* pdst = (UCvPixel32*) dst;
		UCvPixel32* psrc = (UCvPixel32*) src;
		UCvPixel32* pmask = (UCvPixel32*) mask;

		if(psrc == NULL){
			memset(pdst, 0, sizeof(UCvPixel32)*width);
			return;
		}

		int i;
		for(i=0; i<width; i++){
			if(pmask->a != 0){
				pdst->value = psrc->value;
			}

			pdst++;
			psrc++;
			pmask++;
		}
	}
};

class BltCopyByBinaryMask4chOp_Mask1ch {
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask);

		UCvPixel32* pdst = (UCvPixel32*) dst;
		UCvPixel32* psrc = (UCvPixel32*) src;
		uint8_t* pmask = (uint8_t*) mask;

		if(psrc == NULL){
			memset(pdst, 0, sizeof(UCvPixel32)*width);
			return;
		}

		int i;
		for(i=0; i<width; i++){
			if((*pmask) != 0){
				pdst->value = psrc->value;
			}

			pdst++;
			psrc++;
			pmask++;
		}
	}
};

//////////////////////////////////////////////////////
// BltCopy By Binary 1ch Operator
//////////////////////////////////////////////////////
class BltCopyByBinaryMask1chOp_NoMask {
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask == NULL);

		uint8_t* pdst = (uint8_t*) dst;
		uint8_t* psrc = (uint8_t*) src;

		if(psrc == NULL){
			memset(pdst, 0, sizeof(uint8_t)*width);
			return;
		}

		memcpy(pdst, psrc, sizeof(uint8_t)*width);
	}
};

class BltCopyByBinaryMask1chOp_Mask4ch {
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask);

		uint8_t* pdst = (uint8_t*) dst;
		uint8_t* psrc = (uint8_t*) src;
		UCvPixel32* pmask = (UCvPixel32*) mask;

		if(psrc == NULL){
			memset(pdst, 0, sizeof(uint8_t)*width);
			return;
		}

		int i;
		for(i=0; i<width; i++){
			if(pmask->a != 0){
				(*pdst) = (*psrc);
			}

			pdst++;
			psrc++;
			pmask++;
		}
	}
};

class BltCopyByBinaryMask1chOp_Mask1ch {
public:
	/*!
		@param[out] dst destination image, 1ch
		@param[in] src source image, 1ch
		@param[in] mask image, 1ch
		@param[in] width write width	
	*/
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask);

		uint8_t* pdst = (uint8_t*) dst;
		uint8_t* psrc = (uint8_t*) src;
		uint8_t* pmask = (uint8_t*) mask;

		if(psrc == NULL){
			memset(pdst, 0, sizeof(uint8_t)*width);
			return;
		}

		int i;
		for(i=0; i<width; i++){
			if((*pmask) != 0){
				(*pdst) = (*psrc);
			}

			pdst++;
			psrc++;
			pmask++;
		}
	}
};

//////////////////////////////////////////////////////
// LUT 4ch Operator
//////////////////////////////////////////////////////
class LUT4chOp_NoMask {
public:
	inline void SetLUTTable(const CvArr* lut_table){
		cv::Mat lutm = cv::cvarrToMat( lut_table );
		const uint8_t* _lut = (const uint8_t*)lutm.data;

		int i,j;
		for(j=0; j<4; j++){
			for(i=0; i<256; i++){
				m_lut_table[j][i] = _lut[i*4 + j];
			}
		}
	}

	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask == NULL);

		uint8_t* pdst = (uint8_t*) dst;
		uint8_t* psrc = (uint8_t*) src;

		int i,j;
		if(psrc == NULL){
			for(i=0; i<width; i++){
				for(j=0; j<4; j++){
					const uint8_t* lut = m_lut_table[j];
					(*pdst) = lut[0];
					pdst++;
				}
			}
			return;
		}

		for(i=0; i<width; i++){
			for(j=0; j<4; j++){
				const uint8_t* lut = m_lut_table[j];
				(*pdst) = lut[(*psrc)];
				pdst++;
				psrc++;
			}
		}
	}

	uint8_t m_lut_table [4][256];
};

//////////////////////////////////////////////////////
// Preserve Luminosity LUT 4ch Operator
//////////////////////////////////////////////////////
class PL_LUT4chOp_NoMask {
public:
	inline void SetLUTTable(const CvArr* lut_table){
		cv::Mat lutm = cv::cvarrToMat( lut_table );
		const uint8_t* _lut = (const uint8_t*)lutm.data;

		int i,j;
		for(j=0; j<4; j++){
			for(i=0; i<256; i++){
				m_lut_table[j][i] = _lut[i*4 + j];
			}
		}
	}

	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst);
		assert(mask == NULL);

		UCvPixel32* pdst = (UCvPixel32*) dst;
		uint8_t* psrc = (uint8_t*) src;

		UCvPixel32 color;

		int i,j;
		if(psrc == NULL){
			for(i=0; i<width; i++){
				color.b = m_lut_table[0][0];
				color.g = m_lut_table[1][0];
				color.r = m_lut_table[2][0];
				color.a = m_lut_table[3][0];

				pdst->value = color.value;
			}
			return;
		}

		int pl;
		int h, l, s;
		//double pv;
		//double h, s, v;
		for(i=0; i<width; i++){
			//pv = ucvRGB2V( *((UCvPixel32*)psrc) );
			pl = ucvRGB2L( *((UCvPixel32*)psrc) );

			color.b = m_lut_table[0][(*psrc++)];
			color.g = m_lut_table[1][(*psrc++)];
			color.r = m_lut_table[2][(*psrc++)];

			ucvRGB2HLS(color, &h, &l, &s);
			color = ucvHLS2RGB(h, pl, s);

			color.a = m_lut_table[3][(*psrc++)];
			//ucvRGB2HSV(color, &h, &s, &v);
			//color = ucvHSV2RGB(h, s, pv);

			pdst->value = color.value;
			pdst++;
		}
	}

	uint8_t m_lut_table [4][256];
};
#endif //_IPLEXTOPERATE_H_