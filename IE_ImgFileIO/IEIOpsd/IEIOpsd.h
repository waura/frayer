#ifndef _IEIOPSD_H_
#define _IEIOPSD_H_

#include <libImgEdit.h>

#define PSD_CM_NONE 0
#define PSD_CM_RLE 1
#define PSD_CM_ZIP_NO_PREDICTION 2
#define PSD_CM_ZIP_PREDICTION 3
#define PSD_CM_JPEG 4

#define PSD_COLOR_MODE_RGB 3

#define PSD_CHANNEL_MASK  (-2)
#define PSD_CHANNEL_ALPHA (-1)
#define PSD_CHANNEL_RED   0
#define PSD_CHANNEL_GREEN 1
#define PSD_CHANNEL_BLUE  2

typedef struct _PsdHeader{
	uint16_t channels;
	uint32_t row;
	uint32_t col;
	uint16_t depth;
	uint16_t mode;
}PsdHeader;


typedef struct _PsdChannel{
	uint16_t id;
	uint32_t data_length;
	uint32_t data_length_pos;
}PsdChannel;

enum PSD_LAYER_TYPE{
	PSD_NORMAL_LAYER,
	PSD_GROUP_LAYER,
	PSD_CLOSE_GROUP_LAYER,
};

typedef struct _PsdLayerData{
	RECT rect;
	uint32_t channel_num;
	PsdChannel* psd_channels;
	uint8_t flags;
	uint16_t depth;
	uint8_t opacity;
	bool is_mask;
	bool is_visible;
	const_ImgLayer_Ptr mask_data;
	uint32_t blend_fourcc;
	char* name;
	uint32_t layer_type;
	ImgLayer_Ptr pLayer; //to use when save to psd
}PsdLayerData;

typedef std::list<PsdLayerData*> PsdLayerData_List;

void free_psd_layer_data(int nLayer, PsdLayerData* pLayerData);
void free_psd_layer_data_list(PsdLayerData_List& psd_layer_data_list);

IEIO_EXPORT IEMODULE_ID GetIEModule_ID();
IEIO_EXPORT void IEIO_GetFilter(char filter[], size_t buf_size);
IEIO_EXPORT bool IEIO_LoadImgFile(const char *filePath, ImgFile_Ptr pFile);
IEIO_EXPORT bool IEIO_SaveAsImgFile(const char *filePath, ImgFile_Ptr pFile);


#endif //_IEIOPSD_H_