#ifndef _IEIOFYD_H_
#define _IEIOFYD_H_

#include "LibIEDllHeader.h"

#include "RLE.h"
#include "ImgLayer.h"
#include "ImgFile.h"
#include "IEFileIOCommon.h"

#define FYD_CM_NONE 0 //pixeld data isn't compressed
#define FYD_CM_RLE 1  //pixeld data is compressed with RLE 

#define FYD_CHANNEL_ALPHA (-1)
#define FYD_CHANNEL_RED   0
#define FYD_CHANNEL_GREEN 1
#define FYD_CHANNEL_BLUE  2

typedef struct _FydChannel{
	uint16_t id;
} FydChannel;

enum FYD_LAYER_TYPE{
	FYD_NORMAL_LAYER,
	FYD_GROUP_LAYER,
	FYD_CLOSE_GROUP_LAYER,
};

typedef struct _FydLayerData{
	RECT rect;
	uint32_t channel_num;
	FydChannel* fyd_channels;
	uint8_t flags;
	uint16_t depth;
	uint8_t opacity;
	uint32_t blend_fourcc;
	char name[MAX_IMG_LAYER_NAME];
	uint32_t layer_type;
	ImgLayer_Ptr pLayer; //to use when save to psd
}FydLayerData;

typedef std::list<FydLayerData*> FydLayerData_List;

_EXPORT bool IEIOFyd_LoadImgFile(const char *filePath, ImgFile_Ptr pFile);
_EXPORT bool IEIOFyd_SaveAsImgFile(const char *filePath, const ImgFile_Ptr pFile);

#endif //_IEIOFYD_H_