#include "stdafx.h"
#include "RLE.h"
#include "OutputError.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


_EXPORT uint32_t decode_rle(const int8_t* src, uint32_t packed_len, uint32_t unpacked_len, int8_t* dst)
{
	int n;
	char dat;
	uint32_t decode_len = unpacked_len;
	int32_t unpack_left = unpacked_len;
	int32_t pack_left = packed_len;

	while ((unpack_left > 0) && (pack_left > 0)) {
		n = (*src++);
		pack_left--;

		if (n == 128)
			continue;
		else if (n > 128)
			n -= 256;

		if (n < 0) {
			n = 1 - n;
			if (!pack_left) {
				//error;
				OutputError::PushLog(LOG_LEVEL::_ERROR, "Input buffer exhausted in replicate\n");
				break;
			}
			if (!unpack_left) {
				OutputError::PushLog(LOG_LEVEL::_ERROR, "run over unpack size");
				break;
			}

			dat = (*src++);
			pack_left--;
			for (; n > 0; n--) {
				if (!unpack_left) {
					OutputError::PushLog(LOG_LEVEL::_ERROR, "run over unpack size, ", n); 
					break;
				}
				(*dst++) = dat;
				unpack_left--;
			}
		}
		else {
			n++;
			for (; n > 0; n--) {
				if (!pack_left) {
					::OutputDebugString("Input buffer exhausted in copy\n");
					break;
				}
				if (!unpack_left) {
					::OutputDebugString("Input buffer exhausted in copy\n");
					break;
				}
				(*dst++) = (*src++);
				unpack_left--;
				pack_left--;
			}
		}
	}

	if (unpack_left > 0) {
		for (n = 0; n < pack_left; n++) {
			(*dst++) = 0;
		}
	}

	if (unpack_left) {
		char log[256];
		wsprintf(log, "Packbits decode - unpack left %d\n", unpack_left);
		::OutputDebugString(log);
		decode_len -= unpack_left;
	}
	if (pack_left) {
		char log[256];
		wsprintf(log, "Packbits decode - pack left %d\n", pack_left);
		::OutputDebugString(log);
		decode_len = pack_left;
	}
	return decode_len;
}

_EXPORT void encode_rle(const int8_t* src, uint32_t unpacked_len, int8_t *dst, uint32_t* packed_len)
{
	int i,j;
	uint32_t length = 0;
	uint32_t unpack_left = unpacked_len;
	while (unpack_left > 0) {
		i=0;
		while ((i < 128) &&
			(unpack_left - i > 0) &&
			(src[0] == src[i]))
			i++;

		if (i > 1) {
			(*dst++) = -(i - 1);
			(*dst++) = *src;

			src += i;
			unpack_left -= i;
			length += 2;
		}
		else {
			i=0;
			while ((i < 128) &&
				(unpack_left - (i+1) >0) &&
				(src[i] != src[i+1] || unpack_left - (i+2) <= 0 || src[i] != src[i+2]))
				i++;

			if (unpack_left == 1) {
				i = 1;
			}
			if (i > 0) {
				(*dst++) = i-1;
				for (j = 0; j < i; j++) {
					(*dst++) = (*src++);
				}
				unpack_left -= i;
				length += (i + 1);
			}
		}
	}
	*packed_len = length;
}