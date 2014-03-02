#ifndef _UTILOPENCVIO_H_
#define _UTILOPENCVIO_H_

#include "LibDllHeader.h"

class IplImageExt;

_EXPORT void SaveIplImageExt(const char* path, const IplImageExt* image);

_EXPORT bool LoadIplImageExt(const char* path, IplImageExt* image);

#endif //_UTILOPENCVIO_H_