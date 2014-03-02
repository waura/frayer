#ifndef _ILAYERSYNTHESIZER_H_
#define _ILAYERSYNTHESIZER_H_

#include "LibIEDllHeader.h"

#include <emmintrin.h>

#define MOD_4(x) ((x) & 0xf);

class _EXPORTCPP ILayerSynthesizer
{
public:
	ILayerSynthesizer();
	virtual ~ILayerSynthesizer();
};

class NormalCopy
{
public:
	NormalCopy(){};
	~NormalCopy(){};

	inline void operator()(UCvPixel* dst, const UCvPixel* src) const
	{
		dst->value = src->value;
	}

	inline void operator()(UCvPixel* pdst, __m128i xmdst) const
	{
		_mm_store_si128((__m128i*)pdst, xmdst);
	}
};

class AlphaLockCopy
{
public:
	AlphaLockCopy(){};
	~AlphaLockCopy(){};

	inline void operator()(UCvPixel* dst, const UCvPixel* src) const
	{
		dst->b = src->b;
		dst->g = src->g;
		dst->r = src->r;
	}

	inline void operator()(UCvPixel* pdst, __m128i xmdst) const
	{
		__m128i xmzero = _mm_setzero_si128();
		__m128i xm_lo16 = _mm_unpacklo_epi8(xmdst, xmzero);
		xm_lo16 = _mm_insert_epi16(xm_lo16, (pdst + 0)->a, 3);
		xm_lo16 = _mm_insert_epi16(xm_lo16, (pdst + 1)->a, 3);
		__m128i xm_hi16 = _mm_unpackhi_epi8(xmdst, xmzero);
		xm_hi16 = _mm_insert_epi16(xm_hi16, (pdst + 2)->a, 3);
		xm_hi16 = _mm_insert_epi16(xm_hi16, (pdst + 3)->a, 3);
		xmdst = _mm_packus_epi16(xm_lo16, xm_hi16);
		_mm_store_si128((__m128i*)pdst, xmdst);
	}
};

//for 8bit 4pixel alphablend
inline __m128i AlphaBlend_SSE_epi8(
	__m128i xmover,
	__m128i xmunder)
{
	__m128i __xmzero = _mm_setzero_si128();
	__m128i __xmff16 = _mm_set1_epi16(0xff);

	__m128i __xmo16, __xmu16;
	__m128i __xmlo, __xmhi;
	__m128i __xmover_alpha, __xmover_ralpha, __xmunder_alpha, __xmunder_ralpha, __xmalpha;
	__m128 __xmf_lo, __xmf_hi, __xmf_alpha_lo, __xmf_alpha_hi;

	/*load lo double ward */
	__xmo16 = _mm_unpacklo_epi8(xmover, __xmzero);
	__xmu16 = _mm_unpacklo_epi8(xmunder, __xmzero);

	//
	int over_a1 = _mm_extract_epi16(__xmo16, 3);
	int over_a2 = _mm_extract_epi16(__xmo16, 7);
	if (over_a1 == 255 && over_a2 == 255) {
		__xmlo = __xmo16;
	}
	else if (over_a1 == 0 && over_a2 == 0) {
		__xmlo = __xmu16;
	}
	else {
		int under_a1 = _mm_extract_epi16(__xmu16, 3);
		int under_a2 = _mm_extract_epi16(__xmu16, 7);
		if (under_a1 == 0 && under_a2 == 0) {
			__xmlo = __xmo16;
		} else if (under_a1 == 255 && under_a2 == 255) {
			__xmover_alpha = _mm_shufflelo_epi16(__xmo16, _MM_SHUFFLE(3,3,3,3));
			__xmover_alpha = _mm_shufflehi_epi16(__xmover_alpha, _MM_SHUFFLE(3,3,3,3));
			__xmover_ralpha = _mm_sub_epi16(__xmff16, __xmover_alpha);

			__xmu16 = _mm_mullo_epi16(__xmu16, __xmover_ralpha); /* under_pix * over_ralpha */
			__xmo16 = _mm_mullo_epi16(__xmo16, __xmover_alpha); /* over_pix * over_alpha */
			__xmo16 = _mm_add_epi16(__xmu16, __xmo16); /* (under_pix * over_ralpha + over_pix * over_alpha) */
			__xmlo = _mm_srli_epi16(__xmo16, 8); /* (under_pix * over_ralpha + over_pix * over_alpha) >> 8 */
			__xmlo = _mm_insert_epi16(__xmlo, 255, 3); /* write alpha */
			__xmlo = _mm_insert_epi16(__xmlo, 255, 7); /* write alpha */
		} else {
			__xmover_alpha = _mm_shufflelo_epi16(__xmo16, _MM_SHUFFLE(3,3,3,3));
			__xmover_alpha = _mm_shufflehi_epi16(__xmover_alpha, _MM_SHUFFLE(3,3,3,3));
			__xmover_ralpha = _mm_sub_epi16(__xmff16, __xmover_alpha);
			__xmunder_alpha = _mm_shufflelo_epi16(__xmu16, _MM_SHUFFLE(3,3,3,3));
			__xmunder_alpha = _mm_shufflehi_epi16(__xmunder_alpha, _MM_SHUFFLE(3,3,3,3));
			__xmunder_ralpha = _mm_sub_epi16(__xmff16, __xmunder_alpha);
			__xmalpha = _mm_sub_epi16(__xmff16, _mm_srli_epi16( _mm_mullo_epi16(__xmover_ralpha, __xmunder_ralpha), 8)); /* 255 - ((over_ralpha * under_ralpha) >> 8) */

			__xmu16 = _mm_mullo_epi16( _mm_srli_epi16( _mm_mullo_epi16(__xmunder_alpha, __xmover_ralpha), 8), __xmu16); /* under_pix * ((under_alpha * over_ralpha)>>8) */
			__xmo16 = _mm_mullo_epi16(__xmo16, __xmover_alpha); /* over_pix * over_alpha */
			__xmo16 = _mm_add_epi16(__xmu16, __xmo16); /* over_pix * over_alpha + (under_pix * under_alpha * over_ralpha)>>8 */

			/* calc pixel 1 */
			__xmf_lo = _mm_cvtepi32_ps( _mm_unpacklo_epi16(__xmo16, __xmzero) );
			__xmf_alpha_lo = _mm_cvtepi32_ps( _mm_unpacklo_epi16(__xmalpha, __xmzero) );
			__xmf_lo = _mm_mul_ps(__xmf_lo, _mm_rcp_ps(__xmf_alpha_lo));

			/* calc pixel 2 */
			__xmf_hi = _mm_cvtepi32_ps( _mm_unpackhi_epi16(__xmo16, __xmzero) );
			__xmf_alpha_hi = _mm_cvtepi32_ps( _mm_unpackhi_epi16(__xmalpha, __xmzero) );
			__xmf_hi = _mm_mul_ps(__xmf_hi, _mm_rcp_ps(__xmf_alpha_hi));

			/* pack to 16bit data */
			__xmlo = _mm_packs_epi32(_mm_cvtps_epi32(__xmf_lo), _mm_cvtps_epi32(__xmf_hi));
			int a1 = _mm_extract_epi16(__xmalpha, 3);
			__xmlo = _mm_insert_epi16(__xmlo, a1, 3); /* write alpha */
			int a2 = _mm_extract_epi16(__xmalpha, 7);
			__xmlo = _mm_insert_epi16(__xmlo, a2, 7); /* write alpha */
		}
	}
				
	/* load hi double ward */
	__xmo16 = _mm_unpackhi_epi8(xmover, __xmzero);
	__xmu16 = _mm_unpackhi_epi8(xmunder, __xmzero);

	//
	over_a1 = _mm_extract_epi16(__xmo16, 3);
	over_a2 = _mm_extract_epi16(__xmo16, 7);
	if (over_a1 == 255 && over_a2 == 255) {
		__xmhi = __xmo16;
	}
	else if (over_a1 == 0 && over_a2 == 0) {
		__xmhi = __xmu16;
	}
	else {
		int under_a1 = _mm_extract_epi16(__xmu16, 3);
		int under_a2 = _mm_extract_epi16(__xmu16, 7);
		if (under_a1 == 0 && under_a2 == 0) {
			__xmhi = __xmo16;
		} else if (under_a1 == 255 && under_a2 == 255) {
			__xmover_alpha = _mm_shufflelo_epi16(__xmo16, _MM_SHUFFLE(3,3,3,3));
			__xmover_alpha = _mm_shufflehi_epi16(__xmover_alpha, _MM_SHUFFLE(3,3,3,3));
			__xmover_ralpha = _mm_sub_epi16(__xmff16, __xmover_alpha);

			__xmu16 = _mm_mullo_epi16(__xmu16, __xmover_ralpha); /* under_pix * over_ralpha */
			__xmo16 = _mm_mullo_epi16(__xmo16, __xmover_alpha); /* over_pix * over_alpha */
			__xmo16 = _mm_add_epi16(__xmu16, __xmo16); /* (under_pix * over_ralpha + over_pix * over_alpha) */
			__xmhi = _mm_srli_epi16(__xmo16, 8); /* (under_pix * over_ralpha + over_pix * over_alpha) >> 8 */
			__xmhi = _mm_insert_epi16(__xmhi, 255, 3); /* write alpha */
			__xmhi = _mm_insert_epi16(__xmhi, 255, 7); /* write alpha */
		} else {
			__xmover_alpha = _mm_shufflelo_epi16(__xmo16, _MM_SHUFFLE(3,3,3,3));
			__xmover_alpha = _mm_shufflehi_epi16(__xmover_alpha, _MM_SHUFFLE(3,3,3,3));
			__xmover_ralpha = _mm_sub_epi16(__xmff16, __xmover_alpha);
			__xmunder_alpha = _mm_shufflelo_epi16(__xmu16, _MM_SHUFFLE(3,3,3,3));
			__xmunder_alpha = _mm_shufflehi_epi16(__xmunder_alpha, _MM_SHUFFLE(3,3,3,3));
			__xmunder_ralpha = _mm_sub_epi16(__xmff16, __xmunder_alpha);
			__xmalpha = _mm_sub_epi16(__xmff16, _mm_srli_epi16( _mm_mullo_epi16(__xmover_ralpha, __xmunder_ralpha), 8)); /* 255 - ((over_ralpha * under_ralpha) >> 8) */

			__xmu16 = _mm_mullo_epi16( _mm_srli_epi16( _mm_mullo_epi16(__xmunder_alpha, __xmover_ralpha), 8), __xmu16); /* under_pix * ((under_alpha * over_ralpha)>>8) */
			__xmo16 = _mm_mullo_epi16(__xmo16, __xmover_alpha); /* over_pix * over_alpha */
			__xmo16 = _mm_add_epi16(__xmu16, __xmo16); /* over_pix * over_alpha + (under_pix * ((under_alpha * over_ralpha)>>8) */

			/* calc pixel1 */
			__xmf_lo = _mm_cvtepi32_ps( _mm_unpacklo_epi16(__xmo16, __xmzero) );
			__xmf_alpha_lo = _mm_cvtepi32_ps( _mm_unpacklo_epi16(__xmalpha, __xmzero) );
			__xmf_lo = _mm_mul_ps(__xmf_lo, _mm_rcp_ps(__xmf_alpha_lo));

			/* calc pixel 2 */
			__xmf_hi = _mm_cvtepi32_ps( _mm_unpackhi_epi16(__xmo16, __xmzero) );
			__xmf_alpha_hi = _mm_cvtepi32_ps( _mm_unpackhi_epi16(__xmalpha, __xmzero) );
			__xmf_hi = _mm_mul_ps(__xmf_hi, _mm_rcp_ps(__xmf_alpha_hi));

			/* pack to 16bit data */
			__xmhi = _mm_packs_epi32(_mm_cvtps_epi32(__xmf_lo), _mm_cvtps_epi32(__xmf_hi));
			int a3 = _mm_extract_epi16(__xmalpha, 3);
			__xmhi = _mm_insert_epi16(__xmhi, a3, 3); /* write alpha */
			int a4 = _mm_extract_epi16(__xmalpha, 7);
			__xmhi = _mm_insert_epi16(__xmhi, a4, 7); /* write alpha */
		}
	}
	return  _mm_packus_epi16(__xmlo, __xmhi);
}

//for 16bit 2pixel alphablend
inline __m128i AlphaBlend_SSE_epi16(
	__m128i xmover16,
	__m128i xmunder16)
{
	__m128i __xmzero = _mm_setzero_si128();
	__m128i __xmff16 = _mm_set1_epi16(0xff);

	__m128i xmdst16;
	__m128i __xmover_alpha, __xmover_ralpha, __xmunder_alpha, __xmunder_ralpha, __xmalpha;
	__m128 __xmf_lo, __xmf_hi, __xmf_alpha_lo, __xmf_alpha_hi;

	//
	int over_a1 = _mm_extract_epi16(xmover16, 3);
	int over_a2 = _mm_extract_epi16(xmover16, 7);
	if (over_a1 == 255 && over_a2 == 255) {
		xmdst16 = xmover16;
	} 
	else if (over_a1 == 0 && over_a2 == 0) {
		xmdst16 = xmunder16;
	}
	else {
		int under_a1 = _mm_extract_epi16(xmunder16, 3);
		int under_a2 = _mm_extract_epi16(xmunder16, 7);
		if (under_a1 == 0 && under_a2 == 0) {
			xmdst16 = xmover16;
		} else if (under_a1 == 255 && under_a2 == 255) {
			__xmover_alpha = _mm_shufflelo_epi16(xmover16, _MM_SHUFFLE(3,3,3,3));
			__xmover_alpha = _mm_shufflehi_epi16(__xmover_alpha, _MM_SHUFFLE(3,3,3,3));
			__xmover_ralpha = _mm_sub_epi16(__xmff16, __xmover_alpha);

			xmunder16 = _mm_mullo_epi16(xmunder16, __xmover_ralpha); /* under_pix * over_ralpha */
			xmover16 = _mm_mullo_epi16(xmover16, __xmover_alpha); /* over_pix * over_alpha */
			xmover16 = _mm_add_epi16(xmunder16, xmover16); /* (under_pix * over_ralpha + over_pix * over_alpha) */
			xmdst16 = _mm_srli_epi16(xmover16, 8); /* (under_pix * over_ralpha + over_pix * over_alpha) >> 8 */
			xmdst16 = _mm_insert_epi16(xmdst16, 255, 3); /* write alpha */
			xmdst16 = _mm_insert_epi16(xmdst16, 255, 7); /* write alpha */
		} else {
			__xmover_alpha = _mm_shufflelo_epi16(xmover16, _MM_SHUFFLE(3,3,3,3));
			__xmover_alpha = _mm_shufflehi_epi16(__xmover_alpha, _MM_SHUFFLE(3,3,3,3));
			__xmover_ralpha = _mm_sub_epi16(__xmff16, __xmover_alpha);
			__xmunder_alpha = _mm_shufflelo_epi16(xmunder16, _MM_SHUFFLE(3,3,3,3));
			__xmunder_alpha = _mm_shufflehi_epi16(__xmunder_alpha, _MM_SHUFFLE(3,3,3,3));
			__xmunder_ralpha = _mm_sub_epi16(__xmff16, __xmunder_alpha);
			__xmalpha = _mm_sub_epi16(__xmff16, _mm_srli_epi16( _mm_mullo_epi16(__xmover_ralpha, __xmunder_ralpha), 8)); /* 255 - ((over_ralpha * under_ralpha) >> 8) */

			xmunder16 = _mm_mullo_epi16( _mm_srli_epi16( _mm_mullo_epi16(__xmunder_alpha, __xmover_ralpha), 8), xmunder16); /* under_pix * ((under_alpha * over_ralpha)>>8) */
			xmover16 = _mm_mullo_epi16(xmover16, __xmover_alpha); /* over_pix * over_alpha */
			xmover16 = _mm_add_epi16(xmunder16, xmover16); /* over_pix * over_alpha + (under_pix * under_alpha * over_ralpha)>>8 */

			/* calc pixel 1 */
			__xmf_lo = _mm_cvtepi32_ps( _mm_unpacklo_epi16(xmover16, __xmzero) );
			__xmf_alpha_lo = _mm_cvtepi32_ps( _mm_unpacklo_epi16(__xmalpha, __xmzero) );
			__xmf_lo = _mm_mul_ps(__xmf_lo, _mm_rcp_ps(__xmf_alpha_lo));

			/* calc pixel 2 */
			__xmf_hi = _mm_cvtepi32_ps( _mm_unpackhi_epi16(xmover16, __xmzero) );
			__xmf_alpha_hi = _mm_cvtepi32_ps( _mm_unpackhi_epi16(__xmalpha, __xmzero) );
			__xmf_hi = _mm_mul_ps(__xmf_hi, _mm_rcp_ps(__xmf_alpha_hi));

			/* pack to 16bit data */
			xmdst16 = _mm_packs_epi32(_mm_cvtps_epi32(__xmf_lo), _mm_cvtps_epi32(__xmf_hi));
			int a1 = _mm_extract_epi16(__xmalpha, 3);
			xmdst16 = _mm_insert_epi16(xmdst16, a1, 3); /* write alpha */
			int a2 = _mm_extract_epi16(__xmalpha, 7);
			xmdst16 = _mm_insert_epi16(xmdst16, a2, 7); /* write alpha */
		}
	}
	return xmdst16;
}

template<class _CSynthOp, class _CCopyOp>
class LineLayerSynthesizer
{
public:
	LineLayerSynthesizer(){};
	~LineLayerSynthesizer(){};

	inline void SetOpacity(double op){
		m_opacity = op;
	}

	/*
		dst 4ch BGRA
		under 4ch BGRA
		over 4ch BGRA
		mask 4ch BGRA
	*/
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		if(src == NULL)
			return;

		UCvPixel* pdst = (UCvPixel*) dst;
		const UCvPixel* psrc = (const UCvPixel*) src;
		const UCvPixel* pmask = (const UCvPixel*) mask;

#ifdef IE_USE_WIN32_SSE
		if (IS_16BYTE_ALIGNMENT(dst) && IS_16BYTE_ALIGNMENT(src)) {
			SubLineSynth_SSE(pdst, psrc, pmask, width);
			return;
		}

		int dif_d = DIFF_16BYTE_ALIGNMENT(dst);
		int dif_s = DIFF_16BYTE_ALIGNMENT(src);
		assert((dif_d & 0x3) == 0); //1pixel、4byteなので4の倍数になるはず
		assert((dif_s & 0x3) == 0); //

		if (dif_d == dif_s) {
			int dif_npix = dif_s >> 2; //アライメントがあっていないピクセル数を得る
			LineSynth(pdst, psrc, pmask, dif_npix);
			SubLineSynth_SSE(
				pdst + dif_npix,
				psrc + dif_npix,
				(pmask == NULL) ? NULL : (pmask + dif_npix),
				width - dif_npix);
		} else {
			//書き込み先のアライメントを合わせる。
			int dif_npix = dif_d >> 2;
			LineSynth(pdst, psrc, pmask, dif_npix);
			//
			SubLineSynth_SSE_src_unaligned(
				pdst + dif_npix,
				psrc + dif_npix,
				(pmask == NULL) ? NULL : pmask + dif_npix,
				width - dif_npix);
		}
#else
		LineSynth(pdst, psrc, pmask, width);
#endif //IE_USE_WIN32_SSE
	}

private:
	inline void LineSynth(
		UCvPixel* pdst,
		const UCvPixel* psrc,
		const UCvPixel* pmask,
		int width) const
	{
		UCvPixel dst_pix;
		UCvPixel over_pix;
		UCvPixel under_pix;

		_CSynthOp SynthOp;
		_CCopyOp CopyOp;

		int i;
		for (i=0; i<width; i++) {
			under_pix.value = pdst->value;
			over_pix.value = psrc->value;

			if (pmask) {
				over_pix.a = (over_pix.a * pmask->a) >> 8;
			}
			over_pix.a = over_pix.a * m_opacity;

			SynthOp(over_pix, under_pix);

			//alpha blend
			if ((over_pix.a == 255) || (under_pix.a == 0)) {
				dst_pix.value = over_pix.value;
			} else if (over_pix.a == 0) {
				dst_pix.value = under_pix.value;
			} else if (under_pix.a == 255) {
				uint8_t ra = ~over_pix.a;
				dst_pix.b = (over_pix.b*over_pix.a + under_pix.b*ra)>>8;
				dst_pix.g = (over_pix.g*over_pix.a + under_pix.g*ra)>>8;
				dst_pix.r = (over_pix.r*over_pix.a + under_pix.r*ra)>>8;
				dst_pix.a = 255;
			} else {
				uint8_t ra = ~over_pix.a;
				uint8_t alpha = 255 - (ra*(255 - under_pix.a)>>8);
				double inv_alpha = 1.0/alpha;
				dst_pix.b = (over_pix.b*over_pix.a + ((under_pix.b*under_pix.a*ra)>>8))*inv_alpha;
				dst_pix.g = (over_pix.g*over_pix.a + ((under_pix.g*under_pix.a*ra)>>8))*inv_alpha;
				dst_pix.r = (over_pix.r*over_pix.a + ((under_pix.r*under_pix.a*ra)>>8))*inv_alpha;
				dst_pix.a = alpha;
			}

			CopyOp(pdst, &dst_pix);

			pdst++;
			psrc++;

			if (pmask) {
				pmask++;
			}
		}
	}

	inline void SubLineSynth_SSE(
		UCvPixel* pdst,
		const UCvPixel* psrc,
		const UCvPixel* pmask,
		int width) const
	{
		assert (IS_16BYTE_ALIGNMENT(pdst) == IS_16BYTE_ALIGNMENT(psrc));

		_CSynthOp SynthOp;
		_CCopyOp CopyOp;

		int nloop = width >> 2; //width / 4

		__m128i xmmask = _mm_setzero_si128();
		__m128i xmzero = _mm_setzero_si128();

		int x;
		for(x = 0; x < nloop; x++){
			__m128i xmunder = _mm_load_si128((const __m128i*)pdst);
			__m128i xmover = _mm_load_si128((const __m128i*)psrc);

			if (pmask) {
				xmmask = _mm_loadu_si128((const __m128i*)pmask);
			}

			//lo double word pixel
			__m128i xmo_lo16 = _mm_unpacklo_epi8(xmover, xmzero);
			__m128i xmu_lo16 = _mm_unpacklo_epi8(xmunder, xmzero);

			if (pmask) {
				//over_pix.a = ((over_pix.a * pmask->a) >> 8) * m_opacity;;
				__m128i xmmask16 = _mm_unpacklo_epi8(xmmask, xmzero);
				int oa1 = _mm_extract_epi16(xmo_lo16, 3);
				int ma1 = _mm_extract_epi16(xmmask16, 3);
				xmo_lo16 = _mm_insert_epi16(xmo_lo16, ((oa1 * ma1) >> 8) * m_opacity, 3);
				int oa2 = _mm_extract_epi16(xmo_lo16, 7);
				int ma2 = _mm_extract_epi16(xmmask16, 7);
				xmo_lo16 = _mm_insert_epi16(xmo_lo16, ((oa2 * ma2) >> 8) * m_opacity, 7);
			} else {
				//over_pix.a = over_pix.a * m_opacity;
				int oa1 = _mm_extract_epi16(xmo_lo16, 3);
				xmo_lo16 = _mm_insert_epi16(xmo_lo16, oa1 * m_opacity, 3);
				int oa2 = _mm_extract_epi16(xmo_lo16, 7);
				xmo_lo16 = _mm_insert_epi16(xmo_lo16, oa2 * m_opacity, 7);
			}
			xmo_lo16 = SynthOp(xmo_lo16, xmu_lo16);
			xmo_lo16 = AlphaBlend_SSE_epi16(xmo_lo16, xmu_lo16);

			//hi double word pixel
			__m128i xmo_hi16 = _mm_unpackhi_epi8(xmover, xmzero);
			__m128i xmu_hi16 = _mm_unpackhi_epi8(xmunder, xmzero);

			if (pmask) {
				//over_pix.a = ((over_pix.a * pmask->a) >> 8) * m_opacity;;
				__m128i xmmask16 = _mm_unpacklo_epi8(xmmask, xmzero);
				int oa3 = _mm_extract_epi16(xmo_hi16, 3);
				int ma3 = _mm_extract_epi16(xmmask16, 3);
				xmo_hi16 = _mm_insert_epi16(xmo_hi16, ((oa3 * ma3) >> 8) * m_opacity, 3);
				int oa4 = _mm_extract_epi16(xmo_hi16, 7);
				int ma4 = _mm_extract_epi16(xmmask16, 7);
				xmo_hi16 = _mm_insert_epi16(xmo_hi16, ((oa4 * ma4) >> 8) * m_opacity, 7);
			} else {
				//over_pix.a = over_pix.a * m_opacity;
				int oa3 = _mm_extract_epi16(xmo_hi16, 3);
				xmo_hi16 = _mm_insert_epi16(xmo_hi16, oa3 * m_opacity, 3);
				int oa4 = _mm_extract_epi16(xmo_hi16, 7);
				xmo_hi16 = _mm_insert_epi16(xmo_hi16, oa4 * m_opacity, 7);
			}
			xmo_hi16 = SynthOp(xmo_hi16, xmu_hi16);
			xmo_hi16 = AlphaBlend_SSE_epi16(xmo_hi16, xmu_hi16);

			__m128i xmdst = _mm_packus_epi16(xmo_lo16, xmo_hi16);

			CopyOp(pdst, xmdst);
			/*_mm_store_si128((__m128i*)pdst, xmdst);*/

			pdst += 4;
			psrc += 4;

			if (pmask) {
				pmask += 4;
			}
		}

		int bn = width & 0x3; //width % 4
		LineSynth(pdst, psrc, pmask, bn);

		return;
	}

	inline void SubLineSynth_SSE_src_unaligned(
		UCvPixel* pdst,
		const UCvPixel* psrc,
		const UCvPixel* pmask,
		int width) const
	{
		assert (IS_16BYTE_ALIGNMENT(pdst));

		_CSynthOp SynthOp;
		_CCopyOp CopyOp;

		int nloop = width >> 2; //width / 4

		__m128i xmmask = _mm_setzero_si128();
		__m128i xmzero = _mm_setzero_si128();

		int x;
		for(x = 0; x < nloop; x++){
			__m128i xmunder = _mm_load_si128((const __m128i*)(pdst));
			__m128i xmover = _mm_loadu_si128((const __m128i*)(psrc));

			if (pmask) {
				xmmask = _mm_loadu_si128((const __m128i*)pmask);
			}

			//lo double word pixel
			__m128i xmo_lo16 = _mm_unpacklo_epi8(xmover, xmzero);
			__m128i xmu_lo16 = _mm_unpacklo_epi8(xmunder, xmzero);

			if (pmask) {
				//over_pix.a = ((over_pix.a * pmask->a) >> 8) * m_opacity;;
				__m128i xmmask16 = _mm_unpacklo_epi8(xmmask, xmzero);
				int oa1 = _mm_extract_epi16(xmo_lo16, 3);
				int ma1 = _mm_extract_epi16(xmmask16, 3);
				xmo_lo16 = _mm_insert_epi16(xmo_lo16, ((oa1 * ma1) >> 8) * m_opacity, 3);
				int oa2 = _mm_extract_epi16(xmo_lo16, 7);
				int ma2 = _mm_extract_epi16(xmmask16, 7);
				xmo_lo16 = _mm_insert_epi16(xmo_lo16, ((oa2 * ma2) >> 8) * m_opacity, 7);
			} else {
				//over_pix.a = over_pix.a * m_opacity;
				int oa1 = _mm_extract_epi16(xmo_lo16, 3);
				xmo_lo16 = _mm_insert_epi16(xmo_lo16, oa1 * m_opacity, 3);
				int oa2 = _mm_extract_epi16(xmo_lo16, 7);
				xmo_lo16 = _mm_insert_epi16(xmo_lo16, oa2 * m_opacity, 7);
			}
			xmo_lo16 = SynthOp(xmo_lo16, xmu_lo16);
			xmo_lo16 = AlphaBlend_SSE_epi16(xmo_lo16, xmu_lo16);

			//hi double word pixel
			__m128i xmo_hi16 = _mm_unpackhi_epi8(xmover, xmzero);
			__m128i xmu_hi16 = _mm_unpackhi_epi8(xmunder, xmzero);

			if (pmask) {
				//over_pix.a = ((over_pix.a * pmask->a) >> 8) * m_opacity;;
				__m128i xmmask16 = _mm_unpacklo_epi8(xmmask, xmzero);
				int oa3 = _mm_extract_epi16(xmo_hi16, 3);
				int ma3 = _mm_extract_epi16(xmmask16, 3);
				xmo_hi16 = _mm_insert_epi16(xmo_hi16, ((oa3 * ma3) >> 8) * m_opacity, 3);
				int oa4 = _mm_extract_epi16(xmo_hi16, 7);
				int ma4 = _mm_extract_epi16(xmmask16, 7);
				xmo_hi16 = _mm_insert_epi16(xmo_hi16, ((oa4 * ma4) >> 8) * m_opacity, 7);
			} else {
				//over_pix.a = over_pix.a * m_opacity;
				int oa3 = _mm_extract_epi16(xmo_hi16, 3);
				xmo_hi16 = _mm_insert_epi16(xmo_hi16, oa3 * m_opacity, 3);
				int oa4 = _mm_extract_epi16(xmo_hi16, 7);
				xmo_hi16 = _mm_insert_epi16(xmo_hi16, oa4 * m_opacity, 7);
			}
			xmo_hi16 = SynthOp(xmo_hi16, xmu_hi16);
			xmo_hi16 = AlphaBlend_SSE_epi16(xmo_hi16, xmu_hi16);

			__m128i xmdst = _mm_packus_epi16(xmo_lo16, xmo_hi16);

			CopyOp(pdst, xmdst);
			/*_mm_store_si128((__m128i*)pdst, xmdst);*/

			pdst += 4;
			psrc += 4;

			if (pmask) {
				pmask += 4;
			}
		}

		int bn = width & 0x3; //width % 4
		LineSynth(pdst, psrc, pmask, bn);

		return;
	}

protected:
	double m_opacity;
};

template<class _CSynthOp, class _CCopyOp>
class LineLayerBlender
{
public:
	LineLayerBlender(){};
	~LineLayerBlender(){};

	inline void SetOpacity(double op){
		m_opacity = op;
	}

	/*
		dst 4ch BGRA
		under 4ch BGRA
		over 4ch BGRA
		mask 1ch
	*/
	inline void operator()(
		void* dst,
		const void* under,
		const void* over,
		const void* mask,
		int width) const
	{
		if (over == NULL)
			return;
		assert((under == NULL) || (DIFF_16BYTE_ALIGNMENT(under) == DIFF_16BYTE_ALIGNMENT(over)));

		UCvPixel* pdst = (UCvPixel*)dst;
		const UCvPixel* punder = (const UCvPixel*)under;
		const UCvPixel* pover = (const UCvPixel*)over;
		const uint8_t* pmask = (const uint8_t*)mask;

#ifdef IE_USE_WIN32_SSE
		if (IS_16BYTE_ALIGNMENT(pdst) && IS_16BYTE_ALIGNMENT(pover)) {
			SubLineBlend_SSE(pdst, punder, pover, pmask, width);
			return;
		}

		int dif_d = DIFF_16BYTE_ALIGNMENT(pdst);
		int dif_s = DIFF_16BYTE_ALIGNMENT(pover);
		assert((dif_d & 0x3) == 0); //1pixel、4byteなので4の倍数になるはず
		assert((dif_s & 0x3) == 0); //

		if (dif_d == dif_s) {
			int dif_npix = dif_s >> 2; //アライメントがあっていないピクセル数を得る
			LineBlend(pdst, punder, pover, pmask, dif_npix);
			SubLineBlend_SSE(
				pdst + dif_npix,
				punder + dif_npix,
				pover + dif_npix,
				(pmask == NULL) ? NULL : (pmask + dif_npix),
				width - dif_npix);
		} else {
			//書き込み先のアライメントを合わせる。
			int dif_npix = dif_d >> 2;
			LineBlend(pdst, punder, pover, pmask, dif_npix);
			//
			SubLineBlend_SSE_src_unaligned(
				pdst + dif_npix,
				punder + dif_npix,
				pover + dif_npix,
				(pmask == NULL) ? NULL : pmask + dif_npix,
				width - dif_npix);
		}
#else
		LineBlend(pdst, punder, pover, pmask, width);
#endif //IE_USE_WIN32_SSE
	}

private:
	inline void LineBlend(
		UCvPixel* pdst,
		const UCvPixel* punder,
		const UCvPixel* pover,
		const uint8_t* pmask,
		int width) const
	{
		if (pover == NULL)
			return;

		UCvPixel dst_pix;
		UCvPixel over_pix;
		UCvPixel under_pix;

		_CSynthOp SynthOp;
		_CCopyOp CopyOp;

		int i;
		for (i=0; i<width; i++) {
			if (punder == NULL) {
				under_pix.value = 0;
			} else {
				under_pix.value = punder->value;
			}
			over_pix.value = pover->value;

			if (pmask) {
				over_pix.a = (over_pix.a * (*pmask)) >> 8;
			}
			over_pix.a = over_pix.a * m_opacity;

			SynthOp(over_pix, under_pix);

			//alpha blend over pixel with under pixel
			if ((over_pix.a == 255) || (under_pix.a == 0)) {
				dst_pix.value = over_pix.value;
			} else if (over_pix.a == 0) {
				dst_pix.value = under_pix.value;
			} else if (under_pix.a == 255) {
				uint8_t ra = ~over_pix.a;
				dst_pix.b = (over_pix.b*over_pix.a + under_pix.b*ra)>>8;
				dst_pix.g = (over_pix.g*over_pix.a + under_pix.g*ra)>>8;
				dst_pix.r = (over_pix.r*over_pix.a + under_pix.r*ra)>>8;
				dst_pix.a = 255;
			} else {
				uint8_t ra = ~over_pix.a;
				uint8_t alpha = 255 - (ra*(255 - under_pix.a)>>8);
				double inv_alpha = 1.0/alpha;
				dst_pix.b = (over_pix.b*over_pix.a + ((under_pix.b*under_pix.a*ra)>>8))*inv_alpha;
				dst_pix.g = (over_pix.g*over_pix.a + ((under_pix.g*under_pix.a*ra)>>8))*inv_alpha;
				dst_pix.r = (over_pix.r*over_pix.a + ((under_pix.r*under_pix.a*ra)>>8))*inv_alpha;
				dst_pix.a = alpha;
			}

			//alpha blend dst_pix into dst image
			over_pix.value = dst_pix.value;
			under_pix.value = pdst->value;

			if ((over_pix.a == 255) || (under_pix.a == 0)) {
				dst_pix.value = over_pix.value;
			} else if (over_pix.a == 0) {
				dst_pix.value = under_pix.value;
			} else if (under_pix.a == 255) {
				uint8_t ra = ~over_pix.a;
				dst_pix.b = (over_pix.b*over_pix.a + under_pix.b*ra)>>8;
				dst_pix.g = (over_pix.g*over_pix.a + under_pix.g*ra)>>8;
				dst_pix.r = (over_pix.r*over_pix.a + under_pix.r*ra)>>8;
				dst_pix.a = 255;
			} else {
				uint8_t ra = ~over_pix.a;
				uint8_t alpha = 255 - (ra*(255 - under_pix.a)>>8);
				double inv_alpha = 1.0/alpha;
				dst_pix.b = (over_pix.b*over_pix.a + ((under_pix.b*under_pix.a*ra)>>8))*inv_alpha;
				dst_pix.g = (over_pix.g*over_pix.a + ((under_pix.g*under_pix.a*ra)>>8))*inv_alpha;
				dst_pix.r = (over_pix.r*over_pix.a + ((under_pix.r*under_pix.a*ra)>>8))*inv_alpha;
				dst_pix.a = alpha;
			}

			//
			CopyOp(pdst, &dst_pix);

			pdst++;
			punder++;
			pover++;
			if (pmask) {
				pmask++;
			}
		}
	}

	inline void SubLineBlend_SSE(
		UCvPixel* pdst,
		const UCvPixel* punder,
		const UCvPixel* pover,
		const uint8_t* pmask,
		int width) const
	{
		assert ((punder == NULL) || IS_16BYTE_ALIGNMENT(punder));
		assert (IS_16BYTE_ALIGNMENT(pdst) && IS_16BYTE_ALIGNMENT(pover));

		_CSynthOp SynthOp;
		_CCopyOp CopyOp;

		int nloop = width >> 2; //width / 4

		__m128i xmunder = _mm_setzero_si128();
		__m128i xmzero = _mm_setzero_si128();

		int x;
		for(x = 0; x < nloop; x++){
			__m128i xmdst = _mm_load_si128((const __m128i*)pdst);
			__m128i xmover = _mm_load_si128((const __m128i*)pover);
			if (punder) {
				xmunder = _mm_load_si128((const __m128i*)punder);
			}

			//__m128i xmd_lo16, xmd_hi16;

			//lo double word pixel
			__m128i xmo_lo16 = _mm_unpacklo_epi8(xmover, xmzero);
			__m128i xmu_lo16 = _mm_unpacklo_epi8(xmunder, xmzero);
			__m128i xmd_lo16 = _mm_unpacklo_epi8(xmdst, xmzero);

			if (pmask) {
				//over_pix.a = ((over_pix.a * pmask->a) >> 8) * m_opacity;;
				int oa1 = _mm_extract_epi16(xmo_lo16, 3);
				int ma1 = (*pmask++);
				xmo_lo16 = _mm_insert_epi16(xmo_lo16, ((oa1 * ma1) >> 8) * m_opacity, 3);
				int oa2 = _mm_extract_epi16(xmo_lo16, 7);
				int ma2 = (*pmask++);
				xmo_lo16 = _mm_insert_epi16(xmo_lo16, ((oa2 * ma2) >> 8) * m_opacity, 7);
			} else {
				//over_pix.a = over_pix.a * m_opacity;
				int oa1 = _mm_extract_epi16(xmo_lo16, 3);
				xmo_lo16 = _mm_insert_epi16(xmo_lo16, oa1 * m_opacity, 3);
				int oa2 = _mm_extract_epi16(xmo_lo16, 7);
				xmo_lo16 = _mm_insert_epi16(xmo_lo16, oa2 * m_opacity, 7);
			}
			xmo_lo16 = SynthOp(xmo_lo16, xmu_lo16);
			xmo_lo16 = AlphaBlend_SSE_epi16(xmo_lo16, xmu_lo16);
			xmd_lo16 = AlphaBlend_SSE_epi16(xmo_lo16, xmd_lo16);

			//hi double word pixel
			__m128i xmo_hi16 = _mm_unpackhi_epi8(xmover, xmzero);
			__m128i xmu_hi16 = _mm_unpackhi_epi8(xmunder, xmzero);
			__m128i xmd_hi16 = _mm_unpackhi_epi8(xmdst, xmzero);

			if (pmask) {
				//over_pix.a = ((over_pix.a * pmask->a) >> 8) * m_opacity;;
				int oa3 = _mm_extract_epi16(xmo_hi16, 3);
				int ma3 = (*pmask++);
				xmo_hi16 = _mm_insert_epi16(xmo_hi16, ((oa3 * ma3) >> 8) * m_opacity, 3);
				int oa4 = _mm_extract_epi16(xmo_hi16, 7);
				int ma4 = (*pmask++);
				xmo_hi16 = _mm_insert_epi16(xmo_hi16, ((oa4 * ma4) >> 8) * m_opacity, 7);
			} else {
				//over_pix.a = over_pix.a * m_opacity;
				int oa3 = _mm_extract_epi16(xmo_hi16, 3);
				xmo_hi16 = _mm_insert_epi16(xmo_hi16, oa3 * m_opacity, 3);
				int oa4 = _mm_extract_epi16(xmo_hi16, 7);
				xmo_hi16 = _mm_insert_epi16(xmo_hi16, oa4 * m_opacity, 7);
			}
			xmo_hi16 = SynthOp(xmo_hi16, xmu_hi16);
			xmo_hi16 = AlphaBlend_SSE_epi16(xmo_hi16, xmu_hi16);
			xmd_hi16 = AlphaBlend_SSE_epi16(xmo_hi16, xmd_hi16);

			__m128i xmblend = _mm_packus_epi16(xmd_lo16, xmd_hi16);

			CopyOp(pdst, xmblend);
			/*_mm_store_si128((__m128i*)pdst, xmdst);*/

			pdst += 4;
			pover += 4;
			if (punder) {
				punder += 4;
			}
		}

		int bn = width & 0x3; //width % 4
		LineBlend(pdst, punder, pover, pmask, bn);

		return;
	}

	inline void SubLineBlend_SSE_src_unaligned(
		UCvPixel* pdst,
		const UCvPixel* punder,
		const UCvPixel* pover,
		const uint8_t* pmask,
		int width) const
	{
		assert (IS_16BYTE_ALIGNMENT(pdst));
		assert ((punder == NULL) || (DIFF_16BYTE_ALIGNMENT(punder) == DIFF_16BYTE_ALIGNMENT(pover)));

		_CSynthOp SynthOp;
		_CCopyOp CopyOp;

		int nloop = width >> 2; //width / 4

		__m128i xmunder = _mm_setzero_si128();
		__m128i xmzero = _mm_setzero_si128();

		int x;
		for(x = 0; x < nloop; x++){
			__m128i xmdst = _mm_load_si128((const __m128i*)pdst);
			__m128i xmover = _mm_loadu_si128((const __m128i*)pover);
			if (punder) {
				xmunder = _mm_loadu_si128((const __m128i*)punder);
			}

			//__m128i xmd_lo16, xmd_hi16;

			//lo double word pixel
			__m128i xmo_lo16 = _mm_unpacklo_epi8(xmover, xmzero);
			__m128i xmu_lo16 = _mm_unpacklo_epi8(xmunder, xmzero);
			__m128i xmd_lo16 = _mm_unpacklo_epi8(xmdst, xmzero);

			if (pmask) {
				//over_pix.a = ((over_pix.a * pmask->a) >> 8) * m_opacity;;
				int oa1 = _mm_extract_epi16(xmo_lo16, 3);
				int ma1 = (*pmask++);
				xmo_lo16 = _mm_insert_epi16(xmo_lo16, ((oa1 * ma1) >> 8) * m_opacity, 3);
				int oa2 = _mm_extract_epi16(xmo_lo16, 7);
				int ma2 = (*pmask++);
				xmo_lo16 = _mm_insert_epi16(xmo_lo16, ((oa2 * ma2) >> 8) * m_opacity, 7);
			} else {
				//over_pix.a = over_pix.a * m_opacity;
				int oa1 = _mm_extract_epi16(xmo_lo16, 3);
				xmo_lo16 = _mm_insert_epi16(xmo_lo16, oa1 * m_opacity, 3);
				int oa2 = _mm_extract_epi16(xmo_lo16, 7);
				xmo_lo16 = _mm_insert_epi16(xmo_lo16, oa2 * m_opacity, 7);
			}
			xmo_lo16 = SynthOp(xmo_lo16, xmu_lo16);
			xmo_lo16 = AlphaBlend_SSE_epi16(xmo_lo16, xmu_lo16);
			xmd_lo16 = AlphaBlend_SSE_epi16(xmo_lo16, xmd_lo16);

			//hi double word pixel
			__m128i xmo_hi16 = _mm_unpackhi_epi8(xmover, xmzero);
			__m128i xmu_hi16 = _mm_unpackhi_epi8(xmunder, xmzero);
			__m128i xmd_hi16 = _mm_unpackhi_epi8(xmdst, xmzero);

			if (pmask) {
				//over_pix.a = ((over_pix.a * pmask->a) >> 8) * m_opacity;;
				int oa3 = _mm_extract_epi16(xmo_hi16, 3);
				int ma3 = (*pmask++);
				xmo_hi16 = _mm_insert_epi16(xmo_hi16, ((oa3 * ma3) >> 8) * m_opacity, 3);
				int oa4 = _mm_extract_epi16(xmo_hi16, 7);
				int ma4 = (*pmask++);
				xmo_hi16 = _mm_insert_epi16(xmo_hi16, ((oa4 * ma4) >> 8) * m_opacity, 7);
			} else {
				//over_pix.a = over_pix.a * m_opacity;
				int oa3 = _mm_extract_epi16(xmo_hi16, 3);
				xmo_hi16 = _mm_insert_epi16(xmo_hi16, oa3 * m_opacity, 3);
				int oa4 = _mm_extract_epi16(xmo_hi16, 7);
				xmo_hi16 = _mm_insert_epi16(xmo_hi16, oa4 * m_opacity, 7);
			}
			xmo_hi16 = SynthOp(xmo_hi16, xmu_hi16);
			xmo_hi16 = AlphaBlend_SSE_epi16(xmo_hi16, xmu_hi16);
			xmd_hi16 = AlphaBlend_SSE_epi16(xmo_hi16, xmd_hi16);

			__m128i xmblend = _mm_packus_epi16(xmd_lo16, xmd_hi16);

			CopyOp(pdst, xmblend);
			/*_mm_store_si128((__m128i*)pdst, xmdst);*/

			pdst += 4;
			pover += 4;
			if (punder) {
				punder += 4;
			}
		}

		int bn = width & 0x3; //width % 4
		LineBlend(pdst, punder, pover, pmask, bn);

		return;
	}

protected:
	double m_opacity;
};

/*!
	this functer don't use alpha blend
*/
template<class _CSynthOp, class _CCopyOp>
class SPLineLayerBlender
{
public:
	SPLineLayerBlender(){};
	~SPLineLayerBlender(){};

	inline void operator()(void* dst, const void* under, const void* over, const void* mask, int width) const
	{
		if(over == NULL)
			return;

		UCvPixel* pdst = (UCvPixel*) dst;
		UCvPixel* punder = (UCvPixel*) under;
		UCvPixel* pover = (UCvPixel*) over;
		//UCvPixel* pmask = (UCvPixel*) mask;

		UCvPixel over_pix;
		UCvPixel under_pix;

		_CSynthOp SynthOp;
		_CCopyOp CopyOp;

		int i;
		for(i=0; i<width; i++){
			under_pix.value = punder->value;
			over_pix.value = pover->value;
			over_pix.a = over_pix.a * m_opacity;

			SynthOp(over_pix, under_pix);

			//pdst->value = over_pix.value;
			CopyOp(pdst, &over_pix);

			pdst++;
			punder++;
			pover++;
		}

	}

	inline void SetOpacity(double op){
		m_opacity = op;
	}

protected:
	double m_opacity;
};



#endif