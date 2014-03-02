#ifndef _IEIOPNG_H_
#define _IEIOPNG_H_

#include <libImgEdit.h>

IEIO_EXPORT IEMODULE_ID GetIEModule_ID();
IEIO_EXPORT void IEIO_GetFilter(char filter[], size_t buf_size);
IEIO_EXPORT bool IEIO_LoadImgFile(const char *filePath, ImgFile_Ptr pFile);
IEIO_EXPORT bool IEIO_SaveAsImgFile(const char *filePath, ImgFile_Ptr pFile);

IplImage* LoadPng(const char *filePath);
bool SavePng(const char *filePath, IplImage* img, bool isInterlaced);
BOOL CALLBACK SavePngDlgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

#endif //_IEIOPNG_H_