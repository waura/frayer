#include "stdafx.h"
#include "Base64.h"
#include "OutputError.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

static int FindIndexInB64(char c)
{
	int index = 0;
	while (base64_table[index] != '\0' && base64_table[index] != c)
		index++;

	if(base64_table[index] == '\0')
		return -1;

	return index;
}

_EXPORT bool decode_base64(const char* src, char* dst, size_t* dst_len)
{
	int i, j, l,iWriteCount;
	char c;
	char buf[4];

	int len = strlen(src);
	for (i = 0, j = 0; i < len; i += 4) {
		iWriteCount = 3;
		for (l = 0; l < 4 && i+l<len; l++) {
			c = src[i+l];
			if (c == base64_table[64])	//padding
				iWriteCount--;
			else {
				buf[l] = FindIndexInB64(c);
				if (buf[l] == -1)
					return false;
			}
		}
		dst[j] = ((buf[0] << 2) & 0xfc) | ((buf[1] >> 4) & 0x03);
		if (iWriteCount >= 2)
			dst[j+1] = ((buf[1] << 4) & 0xf0) | ((buf[2] >> 2) & 0x0f);
		if (iWriteCount == 3)		
			dst[j+2] = ((buf[2] << 6) & 0xc0) | (buf[3] & 0x3f);
		j+=iWriteCount;
	}
	(*dst_len) = j;
	return true;
}

_EXPORT void encode_base64(const char* src, size_t src_len, char* dst)
{
	int i,j;
	int nloop = src_len - (src_len % 3);
	for (i = 0,j = 0; i < nloop; i+=3) {
		dst[j+0] = base64_table[(src[i] & 0xfc) >> 2];
		dst[j+1] = base64_table[((src[i] &0x03) << 4) | ((src[i+1] & 0xf0) >> 4)];
		dst[j+2] = base64_table[((src[i+1] & 0x0f) <<2 ) | ((src[i+2] & 0xc0) >> 6)];
		dst[j+3] = base64_table[(src[i+2] & 0x3f)];
		j += 4;
	}

	switch (src_len % 3) {
	case 2:
		dst[j+0] = base64_table[(src[i] & 0xfc) >> 2];
		dst[j+1] = base64_table[((src[i] &0x03) << 4) | ((src[i+1] & 0xf0) >> 4)];
		dst[j+2] = base64_table[((src[i+1] & 0x0f) <<2 )];
		dst[j+3] = base64_table[64]; //padding
		dst[j+4] = '\0';
		break;
	case 1:
		dst[j+0] = base64_table[(src[i] & 0xfc) >> 2];
		dst[j+1] = base64_table[((src[i] &0x03) << 4)];
		dst[j+2] = base64_table[64]; //padding
		dst[j+3] = base64_table[64]; //padding
		dst[j+4] = '\0';
		break;
	}
	dst[j+4] = '\0';
}
