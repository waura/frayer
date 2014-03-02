#ifndef _RLE_H_
#define _RLE_H_

#include <libImgEdit.h>

_EXPORT uint32_t decode_rle(const int8_t* src, uint32_t packed_len, uint32_t unpacked_len, int8_t* dst);
_EXPORT void encode_rle(const int8_t* src, uint32_t unpacked_len, int8_t* dst, uint32_t* packed_len); 

#endif //_RLE_H_