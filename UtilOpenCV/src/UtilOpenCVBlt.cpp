#include "stdafx.h"
#include "UtilOpenCVBlt.h"


void sub_ssememcpy(void* _Dst, const void* _Src, size_t size)
{
	assert(IS_16BYTE_ALIGNMENT(_Dst));
	assert(IS_16BYTE_ALIGNMENT(_Src));

	float* dst = (float*)_Dst;
	float* src = (float*)_Src;
	int loop_num = size >> 6;
	for (int i = 0; i < loop_num; i++) {
		//load 64byte data
		__m128 xmm0 = _mm_load_ps(src + 0);
		__m128 xmm1 = _mm_load_ps(src + 4);
		__m128 xmm2 = _mm_load_ps(src + 8);
		__m128 xmm3 = _mm_load_ps(src + 12);
		//store 64byte data
		//_mm_store_ps(dst + 0, xmm0);
		//_mm_store_ps(dst + 4, xmm1);
		//_mm_store_ps(dst + 8, xmm2);
		//_mm_store_ps(dst + 12, xmm3);
		_mm_stream_si128((__m128i*)(dst + 0), _mm_castps_si128(xmm0));
		_mm_stream_si128((__m128i*)(dst + 4), _mm_castps_si128(xmm1));
		_mm_stream_si128((__m128i*)(dst + 8), _mm_castps_si128(xmm2));
		_mm_stream_si128((__m128i*)(dst + 12), _mm_castps_si128(xmm3));

		dst += 16;
		src += 16;
	}

	memcpy(dst, src, size & 0x3F);
}

template <int _SHIFT>
void dif_ssememcpy(void* _Dst, const void* _Src, size_t size)
{
	assert(IS_16BYTE_ALIGNMENT(_Src));

	float* dst = (float*)_Dst;
	float* src = (float*)_Src;
	__m128 xmm0, xmm1, xmm2, xmm3, xmm4;
	int loop_num = size >> 6;

	xmm0 = _mm_load_ps(src + 0);
	_mm_storeu_ps(dst + 0, xmm0);
	dst = (float*)((int)dst + _SHIFT);
	__m128i xmm0i = _mm_srli_si128(_mm_castps_si128(xmm0), _SHIFT); //xmm0 >> _SHIFT
	for (int i = 0; i < loop_num; i++) {
		xmm1 = _mm_load_ps(src + 4);
		xmm3 = _mm_load_ps(src + 8);
		xmm2 = xmm1;
		xmm4 = xmm3;
		__m128i xmm1i = _mm_slli_si128(_mm_castps_si128(xmm1), 16 - _SHIFT); //xmm1 << (16 - _SHIFT)
		__m128i xmm2i = _mm_srli_si128(_mm_castps_si128(xmm2), _SHIFT); //xmm2 >> _SHIFT
		__m128i xmm3i = _mm_slli_si128(_mm_castps_si128(xmm3), 16 - _SHIFT); //xmm3 << (16 - _SHIFT)
		__m128i xmm4i = _mm_srli_si128(_mm_castps_si128(xmm4), _SHIFT); //xmm4 >> _SHIFT
		xmm1i = _mm_or_si128(xmm1i, xmm0i);
		xmm3i = _mm_or_si128(xmm3i, xmm2i);
		_mm_store_ps(dst + 0, _mm_castsi128_ps(xmm1i));
		_mm_store_ps(dst + 4, _mm_castsi128_ps(xmm3i));

		xmm1 = _mm_load_ps(src + 12);
		xmm3 = _mm_load_ps(src + 16);
		xmm2 = xmm1;
		xmm0 = xmm3;
		xmm1i = _mm_slli_si128(_mm_castps_si128(xmm1), 16 - _SHIFT); //xmm1 << (16 - _SHIFT)
		xmm2i = _mm_srli_si128(_mm_castps_si128(xmm2), _SHIFT); //xmm2 >> _SHIFT
		xmm3i = _mm_slli_si128(_mm_castps_si128(xmm3), 16 - _SHIFT); //xmm3 << (16 - _SHIFT)
		xmm0i = _mm_srli_si128(_mm_castps_si128(xmm0), _SHIFT); //xmm0 >> _SHIFT
		xmm1i = _mm_or_si128(xmm1i, xmm4i);
		xmm3i = _mm_or_si128(xmm3i, xmm2i);
		_mm_store_ps(dst + 8, _mm_castsi128_ps(xmm1i));
		_mm_store_ps(dst + 12, _mm_castsi128_ps(xmm3i));

		dst += 16;
		src += 16;
	}

	memcpy((void*)((int)dst - _SHIFT), src, size & 0x3F);
}

void ssememcpy(void* _Dst, const void* _Src, size_t size)
{
	if (IS_16BYTE_ALIGNMENT(_Dst) && IS_16BYTE_ALIGNMENT(_Src)) {
		sub_ssememcpy(_Dst, _Src, size);
		return;
	}

	int dif_d = ((~(int)_Dst) + 1) & 0xf;
	int dif_s = ((~(int)_Src) + 1) & 0xf;
	if (dif_d == dif_s) {
		memcpy(_Dst, _Src, dif_d);
		sub_ssememcpy((char*)_Dst + dif_d, (char*)_Src + dif_d, size - dif_d);
	} else {
		memcpy(_Dst, _Src, dif_s);

		char* dst = (char*)_Dst + dif_s;
		char* src = (char*)_Src + dif_s;
		int shift = ((~(int)dst) + 1) & 0xf;
		switch (shift) {
		case 1:
			dif_ssememcpy<1>(dst, src, size - dif_s);
			break;
		case 2:
			dif_ssememcpy<2>(dst, src, size - dif_s);
			break;
		case 3:
			dif_ssememcpy<3>(dst, src, size - dif_s);
			break;
		case 4:
			dif_ssememcpy<4>(dst, src, size - dif_s);
			break;
		case 5:
			dif_ssememcpy<5>(dst, src, size - dif_s);
			break;
		case 6:
			dif_ssememcpy<6>(dst, src, size - dif_s);
			break;
		case 7:
			dif_ssememcpy<7>(dst, src, size - dif_s);
			break;
		case 8:
			dif_ssememcpy<8>(dst, src, size - dif_s);
			break;
		case 9:
			dif_ssememcpy<9>(dst, src, size - dif_s);
			break;
		case 10:
			dif_ssememcpy<10>(dst, src, size - dif_s);
			break;
		case 11:
			dif_ssememcpy<11>(dst, src, size - dif_s);
			break;
		case 12:
			dif_ssememcpy<12>(dst, src, size - dif_s);
			break;
		case 13:
			dif_ssememcpy<13>(dst, src, size - dif_s);
			break;
		case 14:
			dif_ssememcpy<14>(dst, src, size - dif_s);
			break;
		case 15:
			dif_ssememcpy<15>(dst, src, size - dif_s);
			break;
		default:
			assert(0);
			break;
		}
	}
}