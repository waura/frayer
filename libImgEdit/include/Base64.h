#ifndef _BASE64_H_
#define _BASE64_H_

#include <libImgEdit.h>

inline size_t BASE64_DECODE_SIZE(size_t len) {
	return ((len/4.0)*3.0 + 4);
}

inline size_t BASE64_ENCODE_SIZE(size_t len) {
	return ((len/3.0)*4.0 + 8);
}

_EXPORT bool decode_base64(const char* src, char* dst, size_t* dst_len);
_EXPORT void encode_base64(const char* src, size_t src_len, char* dst);

#endif //_BASE64_H_
