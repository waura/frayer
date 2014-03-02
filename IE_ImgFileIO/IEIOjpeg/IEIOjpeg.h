#ifndef _IEIOJPEG_H_
#define _IEIOJPEG_H_

#include <libImgEdit.h>

IEIO_EXPORT IEMODULE_ID GetIEModule_ID();
IEIO_EXPORT void IEIO_GetFilter(char filter[], size_t buf_size);
IEIO_EXPORT bool IEIO_LoadImgFile(const char *filePath, ImgFile_Ptr pFile);
IEIO_EXPORT bool IEIO_SaveAsImgFile(const char *filePath, ImgFile_Ptr pFile);

IplImage* LoadJpeg(const char *filePath);
bool SaveJpeg(const char *filePath, IplImage* img, int quality);
BOOL CALLBACK SaveJpegDlgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

#endif //_IEIOJPEG_H_