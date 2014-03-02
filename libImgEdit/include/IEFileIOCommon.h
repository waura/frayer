#ifndef _IEFILEIOCOMMON_H_
#define _IEFILEIOCOMMON_H_

#include "LibIEDllHeader.h"

#include <ios>

//リトルエンディアンからビックエンディアンへ変換
//ビックエンディアンからリトルエンディアンへ変換
#define CONVERT_UINT16_BE(x) ((uint16_t)(\
	(((uint16_t)(x) & (uint16_t) 0x00ffU) << 8) | \
	(((uint16_t)(x) & (uint16_t) 0xff00U) >> 8)))
#define CONVERT_UINT32_BE(x) ((uint32_t)(\
	(((uint32_t)(x) & (uint32_t) 0x000000ffU) << 24) | \
	(((uint32_t)(x) & (uint32_t) 0x0000ff00U) <<  8) | \
	(((uint32_t)(x) & (uint32_t) 0x00ff0000U) >>  8) | \
	(((uint32_t)(x) & (uint32_t) 0xff000000U) >> 24)))

#define CONVERT_INT16_BE(x) ((int16_t)(CONVERT_UINT16BE(x)))
#define CONVERT_INT32_BE(x) ((int32_t)(CONVERT_UINT32BE(x)))


inline void ReadBuffer(FILE* fp, void* dst, size_t size) throw(std::ios_base::failure)
{
	assert(fp);
	assert(feof(fp) == 0);
	assert(ferror(fp) == 0);

	if (fread(dst, size, 1, fp) < 1) {
		throw std::ios_base::failure("fread");
	}
}

inline void WriteBuffer(FILE* fp, void* src, size_t size) throw(std::ios_base::failure)
{
	assert(fp);
	assert(feof(fp) == 0);
	assert(ferror(fp) == 0);

	if (fwrite(src, size, 1, fp) < 1) {
		throw std::ios_base::failure("fwrite");
	}
}

inline uint32_t Read4ByteBE(FILE* fp)
{
	uint32_t val;
	ReadBuffer(fp, &val, sizeof(uint32_t));
	return CONVERT_UINT32_BE(val);	
}

inline uint16_t Read2ByteBE(FILE* fp)
{
	uint16_t val;
	ReadBuffer(fp, &val, sizeof(uint16_t));
	return CONVERT_UINT16_BE(val);	
}

inline uint8_t Read1ByteBE(FILE* fp)
{
	uint8_t val;
	ReadBuffer(fp, &val, sizeof(uint8_t));
	return val;
}

inline void Write4ByteLE(FILE* fp, uint32_t val)
{
	WriteBuffer(fp, &val, sizeof(uint32_t));
}

inline void Write2ByteLE(FILE* fp, uint16_t val)
{
	WriteBuffer(fp, &val, sizeof(uint16_t));
}

inline void Write1ByteLE(FILE* fp, uint8_t val)
{
	WriteBuffer(fp, &val, sizeof(uint8_t));
}

inline void WriteDoubleLE(FILE* fp, double val)
{
	assert(fp);
	WriteBuffer(fp, &val, sizeof(double));
}

inline uint32_t Read4ByteLE(FILE* fp)
{
	uint32_t val;
	ReadBuffer(fp, &val, sizeof(uint32_t));
	return val;
}

inline uint16_t Read2ByteLE(FILE* fp)
{
	uint16_t val;
	ReadBuffer(fp, &val, sizeof(uint16_t));
	return val;
}

inline uint8_t Read1ByteLE(FILE* fp)
{
	uint8_t val;
	ReadBuffer(fp, &val, sizeof(uint8_t));
	return val;
}

inline double ReadDoubleLE(FILE* fp)
{
	assert(fp);
	double val;
	ReadBuffer(fp, &val, sizeof(double));
	return val;
}

#endif //_IEFILEIOCOMMON_H_