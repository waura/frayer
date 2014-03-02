#ifndef _LIBIMGEDIT_LIBIEDLLHEADER_H_
#define _LIBIMGEDIT_LIBIEDLLHEADER_H_

#include <windows.h>
#include <commctrl.h>

#include <UtilOpenCV.h>


#define IE_USE_WIN32_SSE

#ifdef IE_USE_WIN32_SSE
	#ifndef IS_16BYTE_ALIGNMENT
		#define IS_16BYTE_ALIGNMENT(x) (!((int)(x) & (0xf)))
	#endif

	#ifndef DIFF_16BYTE_ALIGNMENT
		#define DIFF_16BYTE_ALIGNMENT(x) (((~(int)x) + 1) & 0xf)
	#endif
#endif //IE_USE_WIN32_SSE

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


#ifdef _COMPILE_LIB_IMGEDIT_DLL
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


enum{
	MAX_IE_MODULE_NAME				= 256,
	MAX_IE_MODULE_DISPLAY_NAME		= 256,
	MAX_IE_TOOL_GROUP_NAME			= 256,
	MAX_IELAYER_SYNTH_DISPLAY_NAME	= 256,
	MAX_IMG_LAYER_NAME				= 256,
	MAX_IMG_MASK_NAME				= 256,
	MAX_IE_BRUSH_NAME				= 256,
	MAX_IE_BRUSH_GROUP_NAME			= 256,
	MAX_IE_PALETTE_NAME				= 256,
	MAX_IE_PALETTE_NODE_NAME		= 256,
	MAX_IE_PARAM_REGISTER_NAME		= 256,
};

class IEBrush;
class IEEMBrush;
class IEImgBrush;
class IEBrushGroup;
class IEPalette;
class IImgLayer;
class ImgLayer;
class ImgLayerGroup;
class ImgMask;
class ImgFile;

class ImgFileHandle;

class IEBrushEventListener;
class IEBrushGroupEventListener;
class IEBrushMGEventListener;
class IEPaletteEventListener;
class IEPaletteMGEventListener;
class IEToolMGEventListener;
class ImgLayerEventListener;
class ImgFileEventListener;
class ImgEditEventListener;

typedef std::shared_ptr<IEBrush> IEBrush_Ptr;
typedef std::shared_ptr<IEEMBrush> IEEMBrush_Ptr;
typedef std::shared_ptr<IEImgBrush> IEImgBrush_Ptr;
typedef std::shared_ptr<IEBrushGroup> IEBrushGroup_Ptr;

typedef std::shared_ptr<IEPalette> IEPalette_Ptr;

typedef std::shared_ptr<IImgLayer> IImgLayer_Ptr;
typedef std::weak_ptr<IImgLayer> IImgLayer_weakPtr;
typedef std::shared_ptr<ImgLayer> ImgLayer_Ptr;
typedef std::shared_ptr<const ImgLayer> const_ImgLayer_Ptr;
typedef std::weak_ptr<ImgLayer> ImgLayer_weakPtr;
typedef std::weak_ptr<const ImgLayer> const_ImgLayer_weakPtr;
typedef std::shared_ptr<ImgLayerGroup> ImgLayerGroup_Ptr;
typedef std::weak_ptr<ImgLayerGroup> ImgLayerGroup_weakPtr;
typedef std::shared_ptr<ImgMask> ImgMask_Ptr;
typedef std::weak_ptr<ImgMask> ImgMask_weakPtr;
typedef std::shared_ptr<ImgFile> ImgFile_Ptr;
typedef std::weak_ptr<ImgFile> ImgFile_weakPtr;

typedef std::shared_ptr<ImgFileHandle> ImgFileHandle_Ptr;


#endif //_LIBIMGEDIT_LIBIEDLLHEADER_H_