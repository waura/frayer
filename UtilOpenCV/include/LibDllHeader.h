#ifndef _UTILOPENCV_LIBDLLHEADER_H_
#define _UTILOPENCV_LIBDLLHEADER_H_

#include <windows.h>
#include <commctrl.h>

#ifdef _COMPILE_UOCV_DLL
	#ifdef __cplusplus
	#define _EXPORT extern "C" __declspec (dllexport)
	#else
	#define _EXPORT __declspec (dllexport)
	#endif

	#define _EXPORTCPP __declspec(dllexport)
#else
	#ifdef __cplusplus
	#define _EXPORT extern "C" __declspec (dllimport)
	#else
	#define _EXPORT __declspec (dllimport)
	#endif

	#define _EXPORTCPP __declspec(dllimport)
#endif

#ifdef _COMPILE_UOCV_LIB
	#ifdef __cplusplus
	#define _EXPORT 
	#else
	#define _EXPORT 
	#endif

	#define _EXPORTCPP 
#endif

#define _USE_WIN32_SSE

#endif //_UTILOPENCV_LIBDLLHEADER_H_