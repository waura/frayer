#include "IEIOpsd.h"

#include "RLE.h"


static uint32_t read_4byte(FILE* fp)
{
	if(fp == NULL) return 0;
	uint32_t val; 
	fread(&val, sizeof(uint32_t), 1, fp);
	return val;
}

static uint32_t read_4byte_BE(FILE* fp)
{
	if(fp == NULL) return 0;
	uint32_t val;
	fread(&val, sizeof(uint32_t), 1, fp);
	return CONVERT_UINT32_BE(val);	
}
static uint16_t read_2byte_BE(FILE* fp)
{
	if(fp == NULL) return 0;
	uint16_t val;
	fread(&val, sizeof(uint16_t), 1, fp);
	return CONVERT_UINT16_BE(val);	
}
static uint8_t read_1byte_BE(FILE* fp)
{
	if(fp == NULL) return 0;
	return fgetc(fp);	
}

static void read_string(FILE* fp, char* dst)
{
	int len = read_1byte_BE(fp);

	if(len == 0) return;

	fread(dst, len, 1, fp);
}

static bool read_psd_header(
	FILE* fp,
	ImgFile_Ptr pFile,
	PsdHeader* pHeader)
{
	char signature[4];
	int16_t version;
	char reserved[6];

	if(!fread(signature, 4, 1, fp)) return false;
	if(memcmp(signature, "8BPS", 4) != 0) return false;

	version = read_2byte_BE(fp);
	if(version != 1) return false;
	if(!fread(reserved, 6, 1, fp)) return false;

	pHeader->channels = read_2byte_BE(fp);
	pHeader->row      = read_4byte_BE(fp);
	pHeader->col      = read_4byte_BE(fp);
	pHeader->depth    = read_2byte_BE(fp);
	pHeader->mode     = read_2byte_BE(fp);
	if(pHeader->depth != 8) return false;
	if(pHeader->mode != PSD_COLOR_MODE_RGB) return false;

	pFile->Init( cvSize(pHeader->col, pHeader->row) );

	return true;
}

static bool read_psd_color_mode_data(
	FILE* fp,
	ImgFile_Ptr pFile)
{
	uint32_t color_mode_data = read_4byte_BE(fp);
	return true;
}

static bool read_psd_image_resource(
	FILE* fp,
	ImgFile_Ptr pFile)
{
	uint32_t block_length = read_4byte_BE(fp);
	uint32_t block_start = ftell(fp);
	uint32_t block_end = block_start + block_length;

	//char sig[4];
	//uint32_t id;
	//uint32_t name_len;
	//uint32_t res_size;
	////if(!fread(sig, 4, 1, fp)) return false;
	////if(memcmp(sig, "8BIM", 4) != 0) return false;

	////id = read_2byte_BE(fp);
	////name_len = read_2byte_BE(fp);
	////res_size = read_4byte_BE(fp);

	////char buf[256];
	////read_string(fp, buf);
	////read_string(fp, buf);

	//if(!fread(sig, 4, 1, fp)) return false;
	//if(memcmp(sig, "8BIM", 4) != 0) return false;

	//id = read_2byte_BE(fp);
	//name_len = read_2byte_BE(fp);
	//res_size = read_4byte_BE(fp);
	//uint32_t xres_fix = read_4byte_BE(fp);
	//uint32_t psd_unit = read_2byte_BE(fp);
	//psd_unit = read_2byte_BE(fp);
	//uint32_t yres_fix = read_4byte_BE(fp);
	//psd_unit = read_2byte_BE(fp);
	//psd_unit = read_2byte_BE(fp);

	//if(!fread(sig, 4, 1, fp)) return false;
	//if(memcmp(sig, "8BIM", 4) != 0) return false;

	//id = read_2byte_BE(fp);
	//name_len = read_2byte_BE(fp);
	//res_size = read_4byte_BE(fp);
	//uint32_t layer_sizse = read_2byte_BE(fp);

	//uint32_t now = ftell(fp);
	//if(now != block_end){
		if(fseek(fp, block_end, SEEK_SET)){
			return false;
		}
	//}
	return true;
}

static bool read_layer_resource_luni(FILE* fp)
{
	//read utf-8 name
	uint32_t len = read_4byte_BE(fp);
	if(len == 0){
		if(fseek(fp, 4-1, SEEK_CUR)){
			return false;
		}
		return true;
	}

	int i;
	wchar_t ws;
	for(i=0; i<len; i++){
		ws = read_2byte_BE(fp);
	}
	
	uint32_t padded_len = len + 1;
	while(padded_len % 4 != 0){
		if(fseek(fp, 1, SEEK_CUR) < 0){
			return false;
		}
		padded_len++;
	}

	return true;
}

static bool read_layer_resource_unknown(FILE* fp)
{
	return true;
}

static bool read_layer_resource(FILE* fp)
{
	long posBefore = ftell(fp);
	char sig[4];
	char tag[4];
	if(!fread(sig, 4, 1, fp)){
		fseek(fp, posBefore, SEEK_SET);
		return false;
	}
	if(memcmp(sig, "8BIM", 4) != 0){
		fseek(fp, posBefore, SEEK_SET);
		return false;
	}
	if(!fread(tag, 4, 1, fp)){
		fseek(fp, posBefore, SEEK_SET);
		return false;
	}
	uint32_t data_len = read_4byte_BE(fp);
	long data_start = ftell(fp);

	if(memcmp(tag, "luni", 4) == 0){
		if(!read_layer_resource_luni(fp)){
			fseek(fp, posBefore, SEEK_SET);
			return false;
		}
	}
	else{
		if(!read_layer_resource_unknown(fp)){
			fseek(fp, posBefore, SEEK_SET);
			return false;
		}
	}

	if(data_len % 2 == 0){
		if(fseek(fp, data_start + data_len, SEEK_SET)){
			return false;
		}
	}
	else{
		if(fseek(fp, data_start + data_len + 1, SEEK_SET)){
			return false;
		}
	}

	return true;
}

static bool read_layer(
	FILE* fp,
	PsdLayerData* pLayerData)
{
	int j;
	pLayerData->rect.top    = read_4byte_BE(fp);
	pLayerData->rect.left   = read_4byte_BE(fp);
	pLayerData->rect.bottom = read_4byte_BE(fp);
	pLayerData->rect.right  = read_4byte_BE(fp);
	pLayerData->channel_num = read_2byte_BE(fp);
	pLayerData->psd_channels = (PsdChannel*) malloc(sizeof(PsdChannel) * pLayerData->channel_num);

	for(j=0; j<pLayerData->channel_num; j++){
		pLayerData->psd_channels[j].id = read_2byte_BE(fp);
		pLayerData->psd_channels[j].data_length = read_4byte_BE(fp);
	}

	char mode_key[4];
	if(!fread(mode_key, 4, 1, fp)){
		return false;
	}
	if(memcmp(mode_key, "8BIM", 4) != 0){
		return false;
	}

	pLayerData->blend_fourcc = read_4byte(fp);
	pLayerData->opacity = read_1byte_BE(fp);
	uint8_t clipping    = read_1byte_BE(fp);
	pLayerData->flags   = read_1byte_BE(fp);
	uint8_t filler      = read_1byte_BE(fp);
	uint32_t extra_len  = read_4byte_BE(fp);

	bool trans_prot = (pLayerData->flags & 0x1) ? true : false;
	pLayerData->is_visible = (pLayerData->flags & 0x2) ? false : true;
	if(extra_len > 0){
		uint32_t layer_block_end = ftell(fp) + extra_len;

		//read mask data
		uint32_t mask_block_length = read_4byte_BE(fp);
		switch(mask_block_length){
			case 0:
				pLayerData->is_mask = false;
				break;
			case 20:
				{
					uint32_t top			= read_4byte_BE(fp);
					uint32_t left			= read_4byte_BE(fp);
					uint32_t bottom		= read_4byte_BE(fp);
					uint32_t right		= read_4byte_BE(fp);
					uint8_t default_color = read_1byte_BE(fp);
					uint8_t flags			= read_1byte_BE(fp);
					uint16_t padding		= read_2byte_BE(fp);
					pLayerData->is_mask = true;
				}
				break;
			case 36:
				{
					uint32_t top			= read_4byte_BE(fp);
					uint32_t left			= read_4byte_BE(fp);
					uint32_t bottom		= read_4byte_BE(fp);
					uint32_t right		= read_4byte_BE(fp);
					uint8_t default_color = read_1byte_BE(fp);
					uint8_t flags			= read_1byte_BE(fp);
					uint8_t flags2		= read_1byte_BE(fp);
					uint8_t mask_bg		= read_1byte_BE(fp);
					uint32_t other_top	= read_4byte_BE(fp);
					uint32_t other_left	= read_4byte_BE(fp);
					uint32_t other_bottom	= read_4byte_BE(fp);
					uint32_t other_right	= read_4byte_BE(fp);
					pLayerData->is_mask = true;
				}
				break;
			default:
				//error
				return false;
				break;
		}

		//layer blending ranges
		uint32_t lb_block_length = read_4byte_BE(fp);
		if(lb_block_length > 0){
			if(fseek(fp, lb_block_length, SEEK_CUR)){
				return false;
			}
		}

		//read name
		uint32_t name_read_length;
		uint8_t name_length = read_1byte_BE(fp);
		if(name_length == 0){
			pLayerData->name = NULL;
			if(fseek(fp, 4-1, SEEK_CUR)){
				return false;
			}
			name_read_length = 4;
		}
		else{
			pLayerData->name = (char*) malloc(name_length+1);
			if(!fread((pLayerData->name), name_length, 1, fp)){
				return false;
			}
			int padded_len = name_length + 1;
			if(fseek(fp, 4-(padded_len%4), SEEK_CUR) < 0){
			}
			pLayerData->name[name_length] = '\0';
			name_read_length = name_length + 4 - (padded_len%4);
		}

		//set layer type
		if(pLayerData->flags & 0x10){
			if(strcmp(pLayerData->name, "</Layer group>") == 0){
				pLayerData->layer_type = PSD_LAYER_TYPE::PSD_CLOSE_GROUP_LAYER;
			}
			else{
				pLayerData->layer_type = PSD_LAYER_TYPE::PSD_GROUP_LAYER;
			}
		}
		else{
			pLayerData->layer_type = PSD_LAYER_TYPE::PSD_NORMAL_LAYER;
		}

		//adjastment layer info 
		//sometimes there's a 2-byte padding here, but it's not 4-aligned
		char sig[4];
		long posBefore = ftell(fp);
		if(!fread(sig, 4, 1, fp)){
			return false;
		}
		if(memcmp(sig, "8BIM", 4) != 0){
			if(fseek(fp, posBefore + 2, SEEK_SET)){
				return false;
			}
			if(!fread(sig, 4, 1, fp)){
				return false;
			}
		}

		if(memcmp(sig, "8BIM", 4) != 0){
			if(fseek(fp, posBefore, SEEK_SET)){
				return false;
			}
		}
		else{
			if(fseek(fp, -4, SEEK_CUR)){
				return false;
			}

			//read all layer resource
			while(read_layer_resource(fp));
		}

		long now = ftell(fp);
		if(layer_block_end != now){
			if(fseek(fp, layer_block_end, SEEK_SET)){
				return false;
			}
		}
	}

	return true;
}

static bool read_channel_pixels(
	FILE* fp,
	IplImage* channel,
	int c_width,
	int c_height,
	int c_depth,
	bool isMerged)
{
	int i,j;
	uint16_t compression = read_2byte_BE(fp);

	int bytes_per_pixel = c_depth / 8;
	int bytes_per_row = c_width * bytes_per_pixel;
	int total_bytes = bytes_per_row * c_height;

	switch(compression){
		case PSD_CM_NONE:
			for(j=0; j<c_height; j++){
				uint8_t* dst = GetPixelAddress(channel, 0, j);
				for(i=0; i<c_width; i++){
					(*dst) = read_1byte_BE(fp);
					dst++;
				}
			}
			break;
		case PSD_CM_RLE:
			{
				uint16_t* rle_pack_len = (uint16_t*) malloc(c_height * 2);
				for(i=0; i<c_height; i++){
					rle_pack_len[i] = read_2byte_BE(fp);
				}

				for(i=0; i<c_height; i++){
					uint8_t* src = (uint8_t*) malloc(rle_pack_len[i]);

					if(!fread(src, rle_pack_len[i], 1, fp)){
						return false;
					}

					uint8_t* dst = GetPixelAddress(channel, 0, i);
					decode_rle((int8_t*)src, rle_pack_len[i], bytes_per_row, (int8_t*)dst);

					free(src);
				}
			}
			break;
		default:
			return false;
			break;
	}

	return true;
}

static bool read_mask_pixels(
	FILE* fp,
	PsdLayerData* pLayerData,
	ImgFile_Ptr pFile,
	ImgLayerGroup_Ptr pLayerGroup)
{
	int i,j;
	AddNewLayerHandle* handle = (AddNewLayerHandle*)pFile->CreateImgFileHandle(IFH_ADD_NEW_LAYER );
	handle->SetNewLayerRect(&(pLayerData->rect));
	handle->Do( pFile );
	ImgLayer_Ptr new_layer = handle->GetNewLayer().lock();
	pFile->ReleaseImgFileHandle(handle);
	handle = NULL;

	//set blend forurCC
	new_layer->SetLayerSynthFourCC( LayerSynthesizerMG::GetAlphaMaskLayerSynthFourCC() );

	pFile->SetSelectLayer( new_layer );

	//set name
	char mask_name[MAX_IMG_LAYER_NAME + 32];
	sprintf(mask_name, "%s マスク", pLayerData->name);
	new_layer->SetName( mask_name );

	int c_width  = pLayerData->rect.right - pLayerData->rect.left;
	int c_height = pLayerData->rect.bottom - pLayerData->rect.top; 

	IplImage* channel;
	IplImage* alpha;
	channel = cvCreateImage(cvSize(c_width, c_height), IPL_DEPTH_8U, 1);
	alpha = cvCreateImage(cvSize(c_width, c_height), IPL_DEPTH_8U, 1);

	if(!read_channel_pixels(fp, channel, c_width, c_height, pLayerData->depth, false)){
		return false;
	}

	//set alpha channel
	cvSet(alpha, cvScalar(255));

	new_layer->Merge(
		pLayerData->rect.left,
		pLayerData->rect.top,
		pLayerData->rect.right - pLayerData->rect.left,
		pLayerData->rect.bottom - pLayerData->rect.top,
		channel,
		channel,
		channel,
		alpha);

	LPUPDATE_DATA pData = new_layer->CreateUpdateData();
	pData->isAll = true;
	new_layer->PushUpdateData( pData );

	return true;
}

static bool read_layer_pixels(
	FILE* fp,
	PsdLayerData* pLayerData,
	ImgFile_Ptr pFile,
	ImgLayerGroup_Ptr pLayerGroup)
{
	int i,j;

	//create new layer	
	AddNewLayerHandle* handle = (AddNewLayerHandle*)pFile->CreateImgFileHandle(IFH_ADD_NEW_LAYER );
	handle->SetNewLayerRect(&(pLayerData->rect));
	handle->Do( pFile );
	ImgLayer_Ptr new_layer = handle->GetNewLayer().lock();
	pFile->ReleaseImgFileHandle(handle);
	handle = NULL;

	//set layer synthesizer
	new_layer->SetLayerSynthFourCC( pLayerData->blend_fourcc );

	//set layer opacity
	new_layer->SetOpacity( pLayerData->opacity );

	pFile->SetSelectLayer( new_layer );

	//
	new_layer->SetName( pLayerData->name );
	new_layer->ExtendLayer(); //不足分があれば拡張

	IplImage* channels[4];
	CvSize size = cvSize(
		pLayerData->rect.right - pLayerData->rect.left,
		pLayerData->rect.bottom - pLayerData->rect.top);
	for(i=0; i<4; i++){
		channels[i] = cvCreateImage(size, IPL_DEPTH_8U, 1);
	}
	
	int c_width  = pLayerData->rect.right - pLayerData->rect.left;
	int c_height = pLayerData->rect.bottom - pLayerData->rect.top; 
	for(i=0; i<pLayerData->channel_num; i++){
		int cn=0;
		int16_t id = pLayerData->psd_channels[i].id;

		if(id == PSD_CHANNEL_MASK){
			read_mask_pixels(fp, pLayerData, pFile, pLayerGroup);
			pLayerData->is_mask = false;
			break;
		}

		switch(id){
			case PSD_CHANNEL_RED:
				cn = 2;
				break;
			case PSD_CHANNEL_GREEN:
				cn = 1;
				break;
			case PSD_CHANNEL_BLUE:
				cn = 0;
				break;
			case PSD_CHANNEL_ALPHA:
				cn = 3;
				break;
		}

		if(!read_channel_pixels(
			fp,
			channels[cn],
			c_width,
			c_height,
			pLayerData->depth,
			false))
		{
			return false;
		}
	}
	if(pLayerData->channel_num == 3){
		//set alpha channel
		cvSet(channels[3], cvScalar(255));
	}

	new_layer->Merge(
		pLayerData->rect.left,
		pLayerData->rect.top,
		pLayerData->rect.right - pLayerData->rect.left,
		pLayerData->rect.bottom - pLayerData->rect.top,
		channels[0],
		channels[1],
		channels[2],
		channels[3]);

	LPUPDATE_DATA pData = new_layer->CreateUpdateData();
	pData->isAll = true;
	new_layer->PushUpdateData( pData );

	cvReleaseImage( &channels[0] );
	cvReleaseImage( &channels[1] );
	cvReleaseImage( &channels[2] );
	cvReleaseImage( &channels[3] );

	return true;
}

static bool read_psd_layer_group(
	FILE* fp,
	int layer_num,
	PsdLayerData* psd_layers,
	int* layer_index,
	ImgFile_Ptr pFile,
	ImgLayerGroup_Ptr pLayerGroup)
{
	if((*layer_index) >= layer_num)
		return true;

	for(; (*layer_index)<layer_num; (*layer_index)++){

		switch(psd_layers[ (*layer_index) ].layer_type){
			case PSD_LAYER_TYPE::PSD_NORMAL_LAYER:
				if(!read_layer_pixels(fp, &psd_layers[ (*layer_index) ], pFile, pLayerGroup)){
					return false;
				}

				if(psd_layers[ (*layer_index) ].is_mask){
					if(!read_mask_pixels(fp, &psd_layers[ (*layer_index) ], pFile, pLayerGroup)){
						return false;
					}
				}
				break;
			case PSD_LAYER_TYPE::PSD_GROUP_LAYER:
				{
					int j;
					uint16_t compression;
					for(j=0; j<psd_layers[ (*layer_index) ].channel_num; j++){
						psd_layers[ (*layer_index) ].psd_channels[j].data_length;
						compression = read_2byte_BE(fp);
					}
					//
					pLayerGroup->SetName( psd_layers[ (*layer_index) ].name );
				}
				return true; //finish read layer 
			case PSD_LAYER_TYPE::PSD_CLOSE_GROUP_LAYER:
				{
					int j;
					uint16_t compression;
					for(j=0; j<psd_layers[ (*layer_index) ].channel_num; j++){
						psd_layers[ (*layer_index) ].psd_channels[j].data_length;
						compression = read_2byte_BE(fp);
					}

					(*layer_index)++;
					//create new layer group
					AddNewLayerGroupHandle* handle = (AddNewLayerGroupHandle*)pFile->CreateImgFileHandle(IFH_ADD_NEW_LAYER_GROUP );
					handle->Do( pFile );
					read_psd_layer_group(
						fp,
						layer_num,
						psd_layers,
						layer_index,
						pFile,
						handle->GetNewLayeyGroup().lock());

					pFile->SetSelectLayer( pLayerGroup );
					pFile->ReleaseImgFileHandle(handle);
					handle = NULL;
				}
				break;
		}
	}

	return true;
}

static bool read_psd_layer_and_mask(
	FILE* fp,
	ImgFile_Ptr pFile,
	const PsdHeader* pHeader)
{
	uint32_t block_length = read_4byte_BE(fp);
	if(block_length > 0){
		uint32_t block_start = ftell(fp);
		uint32_t block_end = block_start + block_length;

		read_4byte_BE(fp);
		int16_t layer_num = read_2byte_BE(fp);

		if(layer_num <0){
			layer_num = -layer_num;
		}

		if(layer_num > 0){
			int i;
			//read layer data
			PsdLayerData* psd_layers = (PsdLayerData*) malloc(sizeof(PsdLayerData)*layer_num);
			memset(psd_layers, 0, sizeof(PsdLayerData)*layer_num);
			for(i=0; i<layer_num; i++){
				psd_layers[i].depth = pHeader->depth;
				if(!read_layer(fp, &psd_layers[i])){
					return false;
				}
			}

			//read layer pixel data
			ImgLayerGroup_Ptr pRootLayerGroup = pFile->GetRootLayerGroup();
			int start_index = 0;
			read_psd_layer_group(fp, layer_num, psd_layers, &start_index, pFile, pRootLayerGroup);

			free_psd_layer_data(layer_num, psd_layers);
		}

		long now = ftell(fp);
		if(now != block_end){
			if(fseek(fp, block_end, SEEK_SET)){
				return false;
			}
		}
	
	}

	return true;
}

static bool read_psd_merged_image(
	FILE* fp,
	ImgFile_Ptr pFile,
	PsdHeader* header)
{
	int i,j,k;

	AddNewLayerHandle* handle = (AddNewLayerHandle*)pFile->CreateImgFileHandle(IFH_ADD_NEW_LAYER );
	handle->Do( pFile );
	ImgLayer_Ptr new_layer = handle->GetNewLayer().lock();
	pFile->ReleaseImgFileHandle(handle);
	handle = NULL;

	pFile->SetSelectLayer( new_layer );

	IplImage* channels[4];
	for(i=0; i<4; i++){
		channels[i] = cvCreateImage(cvSize(header->col, header->row), IPL_DEPTH_8U, 1);
	}

	int bytes_per_pixel = header->depth / 8;
	int bytes_per_row = header->col * bytes_per_pixel;
	int total_bytes = bytes_per_row * header->row;

	int extra_channels;

	switch(header->mode){
		case PSD_COLOR_MODE_RGB:
			extra_channels = header->channels - 3;
	}

	uint16_t** rle_pack_len = (uint16_t**) malloc(sizeof(uint16_t*) * header->channels);
	memset(rle_pack_len, 0, sizeof(uint16_t*) * header->channels);

	uint16_t compression = read_2byte_BE(fp);
	switch(compression){
		case PSD_CM_NONE:
			{
				for(i=0; i<header->channels; i++){
					uint8_t* dst = GetPixelAddress(channels[i], 0, 0);
					if(!fread(dst, total_bytes, 1, fp)){
						return false;
					}
				}
			}
			break;
		case PSD_CM_RLE:
			{
				for(i=0; i<header->channels; i++){
					rle_pack_len[i] = (uint16_t*) malloc(sizeof(uint16_t) * header->row);
					for(j=0; j<header->row; j++){
						rle_pack_len[i][j] = read_2byte_BE(fp);
					}
				}

				for(i=0; i<header->channels; i++){
					for(j=0; j<header->row; j++){
						uint8_t* src = (uint8_t*) malloc(rle_pack_len[i][j]);
						uint8_t* dst = GetPixelAddress(channels[i], 0, j);

						if(!fread(src, rle_pack_len[i][j], 1, fp)){
							return false;
						}

						decode_rle((int8_t*)src, rle_pack_len[i][j], bytes_per_row, (int8_t*)dst);

						free(src);
					}
				}
			}
			break;
	}

	if(rle_pack_len){
		for(i=0; i<header->channels; i++){
			if(rle_pack_len[i]){
				free(rle_pack_len[i]);
			}
		}
		free(rle_pack_len);
	}

	if(header->channels == 3){
		//set alpha channel
		cvSet(channels[3], cvScalar(255));
	}

	new_layer->Merge(
		0, 0,
		header->col,
		header->row,
		channels[2],
		channels[1],
		channels[0],
		channels[3]);

	LPUPDATE_DATA pData = new_layer->CreateUpdateData();
	pData->isAll = true;
	new_layer->PushUpdateData( pData );

	//
	cvReleaseImage( &channels[0] );
	cvReleaseImage( &channels[1] );
	cvReleaseImage( &channels[2] );
	cvReleaseImage( &channels[3] );

	uint32_t now = ftell(fp);
	fseek(fp, 0, SEEK_END);
	uint32_t end = ftell(fp);

	return true;
}



IEIO_EXPORT bool IEIO_LoadImgFile(
	const char* filePath,
	ImgFile_Ptr pFile)
{
	if(filePath == NULL) return NULL;

	FILE* fp;
	errno_t err;
	//if(err = fopen_s(&fp, filePath, "rb") != 0){
	if(!(fp = fopen(filePath, "rb"))){
		char str[256];
		int last_err = GetLastError();
		sprintf(str, "PSDファイルの読み込みに失敗しました。 %s err = %d lasterror = %d", filePath, last_err);
		OutputError::Alert(str);
		return false;
	}

	//read psd header
	PsdHeader header;
	if(!read_psd_header(fp, pFile, &header)){
		fclose(fp);
		return false;
	}
	//
	if(!read_psd_color_mode_data(fp, pFile)){
		fclose(fp);
		return false;
	}
	//
	if(!read_psd_image_resource(fp, pFile)){
		fclose(fp);
		return false;
	}
	//
	if(!read_psd_layer_and_mask(fp, pFile, &header)){
		fclose(fp);
		return false;
	}
	//
	if(pFile->GetLayerSize() == 0){
		if(!read_psd_merged_image(fp, pFile, &header)){
			fclose(fp);
			return false;
		}
	}

	fclose(fp);

	//
	NullHandle* null_handle = (NullHandle*) pFile->CreateImgFileHandle(IFH_NULL);
	strcpy_s(null_handle->handle_name, MAX_IMG_FILE_HANDLE_NAME, "PSDファイル読み込み");
	pFile->DoImgFileHandle( null_handle );

	return true;
}