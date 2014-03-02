#include "stdafx.h"

#include "IEIOfyd.h"
#include "libImgEdit.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


static uint32_t read_4byte(FILE* fp)
{
	if(fp == NULL) return 0;
	uint32_t val; 
	fread(&val, sizeof(uint32_t), 1, fp);
	return val;
}

static uint16_t read_2byte(FILE* fp)
{
	if(fp == NULL) return 0;
	uint16_t val;
	fread(&val, sizeof(uint16_t), 1, fp);
	return val;
}

static uint8_t read_1byte(FILE* fp)
{
	if(fp == NULL) return 0;
	return fgetc(fp);
}

static void read_string(FILE* fp, char* dst)
{
	int len = read_1byte(fp);

	if(len == 0) return;

	fread(dst, len, 1, fp);
}

static bool read_fyd_header(FILE* fp, ImgFile_Ptr pFile)
{
	char signature[4];
	uint16_t version;
	uint16_t subversion;

	if(!fread(signature, 4, 1, fp)) return false;
	if(memcmp(signature, "FFRY", 4) != 0) return false;

	version = read_2byte(fp);
	subversion = read_2byte(fp);

	if(version != 1) return false;
	if(subversion != 0) return false;

	uint16_t channels = read_2byte(fp);
	uint16_t row = read_4byte(fp);
	uint16_t col = read_4byte(fp);
	uint16_t depth = read_2byte(fp);
	uint16_t mode = read_2byte(fp);

	//read reserved
	read_4byte(fp);
	read_4byte(fp);

	pFile->Init( cvSize(col, row) );
	return true;
}

static bool read_fyd_layer_header(FILE* fp, FydLayerData* pLayerData)
{
	int i;
	pLayerData->rect.top    = read_4byte(fp);
	pLayerData->rect.left   = read_4byte(fp);
	pLayerData->rect.bottom = read_4byte(fp);
	pLayerData->rect.right  = read_4byte(fp);
	pLayerData->channel_num = read_2byte(fp);
	pLayerData->fyd_channels = (FydChannel*) malloc(sizeof(FydChannel) * pLayerData->channel_num);

	for(i=0; i<pLayerData->channel_num; i++){
		pLayerData->fyd_channels[i].id = read_2byte(fp);
	}

	pLayerData->blend_fourcc = read_4byte(fp);
	pLayerData->opacity = read_1byte(fp);
	read_1byte(fp); //clipping
	pLayerData->flags = read_1byte(fp);

	read_string(fp, pLayerData->name);

	//set layer type
	if(pLayerData->flags & 0x10){
		if(strcmp(pLayerData->name, "</Layer group>") == 0){
			pLayerData->layer_type = FYD_LAYER_TYPE::FYD_CLOSE_GROUP_LAYER;
		}
		else{
			pLayerData->layer_type = FYD_LAYER_TYPE::FYD_GROUP_LAYER;
		}
	}
	else{
		pLayerData->layer_type = FYD_LAYER_TYPE::FYD_NORMAL_LAYER;
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
	uint16_t compression = read_2byte(fp);

	int bytes_per_pixel = c_depth / 8;
	int bytes_per_row = c_width * bytes_per_pixel;
	int total_bytes = bytes_per_row * c_height;

	switch(compression){
		case FYD_CM_NONE:
			for(j=0; j<c_height; j++){
				uint8_t* dst = GetPixelAddress(channel, 0, j);
				for(i=0; i<c_width; i++){
					(*dst) = read_1byte(fp);
					dst++;
				}
			}
			break;
		case FYD_CM_RLE:
			{
				uint16_t* rle_pack_len = (uint16_t*) malloc(c_height * 2);
				for(i=0; i<c_height; i++){
					rle_pack_len[i] = read_2byte(fp);
				}
				for(i=0; i<c_height; i++){
					uint8_t* src = (uint8_t*) malloc(rle_pack_len[i]);
				
					if(!fread(src, rle_pack_len[i], 1, fp)){
						return false;
					}

					uint8_t* dst = GetPixelAddress(channel, 0, i);
					decode_rle((int8_t*)src, rle_pack_len[i], bytes_per_row, (int8_t*)dst);

					////test RLE
					//{
					//	uint16_t encode_len;
					//	uint8_t* encode_dst = (uint8_t*) malloc(rle_pack_len[i]);
					//	encode_rle(dst, bytes_per_row, encode_dst, &encode_len);
					//	if (encode_len != rle_pack_len[i]) {
					//		printf("error RLE decode\n");
					//	}
					//	for (j = 0; j < encode_len; j++) {
					//		if (src[j] != encode_dst[j]) {
					//			printf("error RLE decode\n");
					//		}
					//	}
					//	free(encode_dst);
					//}

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

static bool read_fyd_layer_pixels(
	FILE* fp,
	FydLayerData* pLayerData,
	ImgFile_Ptr pFile,
	ImgLayerGroup_Ptr pLayerGroup)
{
	int i,j;
	
	//create new layer
	AddNewLayerHandle* handle = (AddNewLayerHandle*)pFile->CreateImgFileHandle(IFH_ADD_NEW_LAYER);
	handle->SetNewLayerRect(&(pLayerData->rect));
	handle->Do( pFile );
	ImgLayer_Ptr new_layer = handle->GetNewLayer().lock();
	pFile->ReleaseImgFileHandle(handle);
	handle = NULL;

	new_layer->SetName( pLayerData->name );
	new_layer->SetLayerSynthFourCC( pLayerData->blend_fourcc );
	//set layer opacity
	new_layer->SetOpacity( pLayerData->opacity );

	pFile->SetSelectLayer( new_layer );

	//
	new_layer->ExtendLayer(); //不足分があれば拡張

	IplImage* channels[4];
	int c_width  = pLayerData->rect.right - pLayerData->rect.left;
	int c_height = pLayerData->rect.bottom - pLayerData->rect.top; 
	CvSize size = cvSize(c_width, c_height);
	for(i=0; i<4; i++){
		channels[i] = cvCreateImage(size, IPL_DEPTH_8U, 1);
	}

	for(i=0; i<pLayerData->channel_num; i++){
		int cn=0;
		int16_t id = pLayerData->fyd_channels[i].id;
		switch(id){
			case FYD_CHANNEL_RED:
				cn = 2;
				break;
			case FYD_CHANNEL_GREEN:
				cn = 1;
				break;
			case FYD_CHANNEL_BLUE:
				cn = 0;
				break;
			case FYD_CHANNEL_ALPHA:
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

	//
	LPUPDATE_DATA pData = new_layer->CreateUpdateData();
	pData->isAll = true;
	new_layer->PushUpdateData( pData );

	cvReleaseImage( &channels[0] );
	cvReleaseImage( &channels[1] );
	cvReleaseImage( &channels[2] );
	cvReleaseImage( &channels[3] );

	return true;
}

static bool read_fyd_layer_group(
	FILE* fp,
	int layer_num,
	FydLayerData* fyd_layers,
	int* layer_index,
	ImgFile_Ptr pFile,
	ImgLayerGroup_Ptr pLayerGroup)
{
	if((*layer_index) >= layer_num)
		return true;

	for(; (*layer_index)<layer_num; (*layer_index)++){

		switch(fyd_layers[ (*layer_index) ].layer_type){
			case FYD_LAYER_TYPE::FYD_NORMAL_LAYER:
				if(!read_fyd_layer_pixels(fp, &fyd_layers[ (*layer_index) ], pFile, pLayerGroup)){
					return false;
				}
				break;
			case FYD_LAYER_TYPE::FYD_GROUP_LAYER:
				{
					int j;
					uint16_t compression;
					for(j=0; j<fyd_layers[ (*layer_index) ].channel_num; j++){
						compression = read_2byte(fp);
					}
					//
					pLayerGroup->SetName( fyd_layers[ (*layer_index) ].name );
				}
				return true; //finish read layer
			case FYD_LAYER_TYPE::FYD_CLOSE_GROUP_LAYER:
				{
					int j;
					uint16_t compression;
					for(j=0; j<fyd_layers[ (*layer_index) ].channel_num; j++){
						compression = read_2byte(fp);
					}

					(*layer_index)++;
					//create new layer group
					AddNewLayerGroupHandle* handle = (AddNewLayerGroupHandle*)pFile->CreateImgFileHandle(IFH_ADD_NEW_LAYER_GROUP);
					handle->Do( pFile );
					read_fyd_layer_group(
						fp,
						layer_num,
						fyd_layers,
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

static bool read_fyd_layer_block(FILE* fp, ImgFile_Ptr pFile)
{
	uint32_t block_length = read_4byte(fp);
	if(block_length > 0){
		uint32_t block_start = ftell(fp);
		uint32_t block_end = block_start + block_length;

		uint16_t layer_num = read_2byte(fp);

		if(layer_num > 0){
			int i;
			//read layer header
			FydLayerData* fyd_layers = (FydLayerData*) malloc(sizeof(FydLayerData)*layer_num);
			memset(fyd_layers, 0, sizeof(FydLayerData)*layer_num);
			for(i=0; i<layer_num; i++){
				fyd_layers[i].depth = 8;
				if(!read_fyd_layer_header(fp, &fyd_layers[i])){
					return false;
				}
			}

			//read layer pixels
			ImgLayerGroup_Ptr pRootLayerGroup = pFile->GetRootLayerGroup();
			int start_index = 0;
			read_fyd_layer_group(fp, layer_num, fyd_layers, &start_index, pFile, pRootLayerGroup);

			//free fyd_layers
			for(i=0; i<layer_num; i++){
				if(fyd_layers[i].fyd_channels){
					free(fyd_layers[i].fyd_channels);
				}
			}
			free(fyd_layers);
		}

		uint32_t now = ftell(fp);
		if(now != block_end){
			if(fseek(fp, block_end, SEEK_SET)){
				return false;
			}
		}
	}

	return true;
}

_EXPORT bool IEIOFyd_LoadImgFile(const char* filePath, ImgFile_Ptr pFile)
{
	if(filePath == NULL) return NULL;

	FILE* fp;
	errno_t err;
	if(!(fp = fopen(filePath, "rb"))){
		char str[256];
		int last_err = GetLastError();
		sprintf(str, "FYDファイルの読み込みに失敗しました。 %s err = %d lasterror = %d", filePath, err, last_err);
		OutputError::Alert(str);
		return false;
	}

	//read fyd header
	if(!read_fyd_header(fp, pFile)){
		return false;
	}
	//
	if(!read_fyd_layer_block(fp, pFile)){
		return false;
	}

	fclose(fp);

	//
	NullHandle* null_handle = (NullHandle*)pFile->CreateImgFileHandle(IFH_NULL);
	strcpy_s(null_handle->handle_name, MAX_IMG_FILE_HANDLE_NAME, "FYDファイル読み込み");
	pFile->DoImgFileHandle( null_handle );

	return true;
}