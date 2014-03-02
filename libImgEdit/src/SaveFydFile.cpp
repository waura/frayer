#include "stdafx.h"

#include "IEIOfyd.h"
#include "libImgEdit.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


static void write_4byte(FILE* fp, uint32_t val)
{
	fwrite(&val, 1, 4, fp);
}

static void write_2byte(FILE* fp, uint16_t val)
{
	fwrite(&val, 1, 2, fp);
}

static void write_1byte(FILE* fp, uint8_t val)
{
	fwrite(&val, 1, 1, fp);
}

static void write_string(FILE* fp, const char* val)
{
	int len = strlen(val);
	write_1byte(fp, len);

	if(len == 0) return;

	fwrite(val, len, 1, fp);
}

static bool write_fyd_header(FILE* fp, const ImgFile_Ptr pFile)
{
	fwrite("FFRY", 1, 4, fp);
	write_2byte(fp, 1); //version
	write_2byte(fp, 0); //sub version

	CvSize img_size = pFile->GetImgSize();
	write_2byte(fp, 4);
	write_4byte(fp, img_size.height);
	write_4byte(fp, img_size.width);
	write_2byte(fp, 8);
	write_2byte(fp, 0);

	//write reserved
	write_4byte(fp, 0);
	write_4byte(fp, 0);

	return true;
}

static bool write_fyd_layer_header(FILE* fp, FydLayerData* pLayerData)
{
	int i;
	write_4byte(fp, pLayerData->rect.top);
	write_4byte(fp, pLayerData->rect.left);
	write_4byte(fp, pLayerData->rect.bottom);
	write_4byte(fp, pLayerData->rect.right);
	write_2byte(fp, 4); //channel num

	//alpha
	write_2byte(fp, FYD_CHANNEL_ALPHA);
	//red
	write_2byte(fp, FYD_CHANNEL_RED);
	//green
	write_2byte(fp, FYD_CHANNEL_GREEN);
	//blue
	write_2byte(fp, FYD_CHANNEL_BLUE);

	//
	write_4byte(fp, pLayerData->blend_fourcc);

	write_1byte(fp, pLayerData->opacity); //opacity
	write_1byte(fp, 0); //clipping
	write_1byte(fp, pLayerData->flags); //flags

	//write layer name
	write_string( fp, pLayerData->name );

	return true;
}

static bool write_channel_pixels(
	FILE* fp,
	IplImage* channel,
	int c_width,
	int c_height,
	int c_depth)
{
	int i,j;
	write_2byte(fp, FYD_CM_RLE); //compression

	int bytes_per_pixel = c_depth / 8;
	int bytes_per_row = c_width * bytes_per_pixel;
	int total_bytes = bytes_per_row * c_height;

	uint32_t* rle_pack_len = (uint32_t*) malloc(c_height* sizeof(uint32_t));
	IplImage* dst = cvCreateImage(cvSize(2 * c_width, c_height), IPL_DEPTH_8U, 1); //RLE buffer size maybe larger than original source buffer

	for(i=0; i<c_height; i++){
		uint8_t* src = GetPixelAddress(channel, 0, i);
		uint8_t* dst_line = GetPixelAddress(dst, 0, i);
		encode_rle(
			(int8_t*)src,
			bytes_per_row,
			(int8_t*)dst_line,
			&(rle_pack_len[i]));

		////test RLE
		//{
		//	uint8_t* decode_dst = (uint8_t*) malloc(bytes_per_row* sizeof(uint8_t));
		//	decode_rle(dst_line, rle_pack_len[i], bytes_per_row, decode_dst);
		//	for (j = 0; j < bytes_per_row; j++) {
		//		if (src[j] != decode_dst[j]) {
		//			printf("error RLE encode\n");
		//		}
		//	}
		//	free(decode_dst);
		//}
	}

	uint32_t* rle_len = rle_pack_len;
	for(i=0; i<c_height; i++){
		write_2byte(fp, (*rle_len));
		rle_len++;
	}
	uint32_t channel_data_length = 2 + c_height*sizeof(uint16_t);

	rle_len = rle_pack_len;
	for(i=0; i<c_height; i++){
		uint8_t* write_line = GetPixelAddress(dst, 0, i);
		channel_data_length += rle_pack_len[i];
		for(j=0; j<(*rle_len); j++){
			write_1byte(fp, (*write_line));
			write_line++;
		}
		rle_len++;
	}
	free(rle_pack_len);
	cvReleaseImage(&dst);

	return true;
}

static bool write_fyd_layer_pixels(FILE* fp, const FydLayerData* pLayerData)
{
	if(pLayerData->flags & 0x10){//is group layer
		write_2byte(fp, FYD_CM_NONE); //compression
		write_2byte(fp, FYD_CM_NONE); //compression
		write_2byte(fp, FYD_CM_NONE); //compression
		write_2byte(fp, FYD_CM_NONE); //compression
		return true;
	}

	int i;
	int c_width = pLayerData->rect.right - pLayerData->rect.left;
	int c_height = pLayerData->rect.bottom - pLayerData->rect.top;

	IplImage* channels[4];
	for(i=0; i<4; i++){
		channels[i] = cvCreateImage(cvSize(c_width, c_height), IPL_DEPTH_8U, 1);
	}

	pLayerData->pLayer->Split(
		channels[3],
		channels[2],
		channels[1],
		channels[0]);

	//write (a) (r) (g) (b)
	for(i=0; i<4; i++){
		if(!write_channel_pixels(
			fp,
			channels[i],
			c_width,
			c_height,
			8))
		{
			goto return_false;
		}
	}

	cvReleaseImage( &channels[0] );
	cvReleaseImage( &channels[1] );
	cvReleaseImage( &channels[2] );
	cvReleaseImage( &channels[3] );
	return true;

return_false:
	cvReleaseImage( &channels[0] );
	cvReleaseImage( &channels[1] );
	cvReleaseImage( &channels[2] );
	cvReleaseImage( &channels[3] );
	return false;
}

static void count_layer(
	ImgLayerGroup_Ptr pLGroup,
	FydLayerData_List& fyd_layer_data_list)
{
	FydLayerData* fyd_layer_data = NULL;
	IImgLayer_Ptr pLayer = pLGroup->GetBottomChildLayer();
	while(pLayer != NULL){
		if(pLayer->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER){
			//close group layer fyd layer data
			fyd_layer_data = (FydLayerData*) malloc(sizeof(FydLayerData));
			memset(fyd_layer_data, 0, sizeof(FydLayerData));

			fyd_layer_data->blend_fourcc = pLayer->GetLayerSynthFourCC();
			fyd_layer_data->flags |= 0x8;
			if(!pLayer->IsVisible())
				fyd_layer_data->flags  |= 0x2;
			fyd_layer_data->flags |= 0x10;

			int name_len = strlen("</Layer group>") + 1;
			strcpy(fyd_layer_data->name, "</Layer group>");
			fyd_layer_data_list.push_back( fyd_layer_data );

			count_layer(std::dynamic_pointer_cast<ImgLayerGroup>(pLayer), fyd_layer_data_list);

			//group layer fyd layer data
			fyd_layer_data = (FydLayerData*) malloc(sizeof(FydLayerData));
			memset(fyd_layer_data, 0, sizeof(FydLayerData));

			fyd_layer_data->opacity = pLayer->GetOpacity();
			fyd_layer_data->blend_fourcc = pLayer->GetLayerSynthFourCC();
			fyd_layer_data->flags |= 0x8;
			if(!pLayer->IsVisible())
				fyd_layer_data->flags |= 0x2;
			fyd_layer_data->flags |= 0x10;
			fyd_layer_data->pLayer = NULL;

			name_len = pLayer->GetNameSize() + 1;
			pLayer->GetName( fyd_layer_data->name );
			fyd_layer_data_list.push_back( fyd_layer_data );
		}
		else{
			ImgLayer_Ptr layer = std::dynamic_pointer_cast<ImgLayer>(pLayer);

			//normal layer fyd layer data
			fyd_layer_data = (FydLayerData*) malloc(sizeof(FydLayerData));
			memset(fyd_layer_data, 0, sizeof(FydLayerData));

			pLayer->GetLayerRect( &(fyd_layer_data->rect) );
			fyd_layer_data->opacity = pLayer->GetOpacity();
			fyd_layer_data->blend_fourcc = pLayer->GetLayerSynthFourCC();
			fyd_layer_data->flags |= 0x8;
			if( !pLayer->IsVisible() )
				fyd_layer_data->flags |= 0x2;
			fyd_layer_data->pLayer = layer;

			int name_len = pLayer->GetNameSize() + 1;
			pLayer->GetName( fyd_layer_data->name );
			fyd_layer_data_list.push_back( fyd_layer_data );
		}

		pLayer = pLayer->GetOverLayer().lock();
	}
}

static bool write_fyd_layer_block(FILE* fp, const ImgFile_Ptr pFile)
{
	int i;
	uint32_t block_length_data_pos = ftell(fp);
	write_4byte(fp, 0); //layer block length

	ImgLayerGroup_Ptr pRootLayerGroup = pFile->GetRootLayerGroup();

	FydLayerData_List fyd_layer_data_list;
	count_layer(pRootLayerGroup, fyd_layer_data_list);
	write_2byte(fp, fyd_layer_data_list.size()); //num of layer

	//write layer header
	FydLayerData_List::iterator itr = fyd_layer_data_list.begin();
	for(; itr != fyd_layer_data_list.end(); itr++){
		if(!write_fyd_layer_header(fp, (*itr))){
			return false;
		}
	}
	
	//write layer pixels
	itr = fyd_layer_data_list.begin();
	for(; itr != fyd_layer_data_list.end(); itr++){
		if(!write_fyd_layer_pixels(fp, (*itr))){
			return false;
		}
	}

	//free fyd_layer_data_list
	itr = fyd_layer_data_list.begin();
	for(; itr != fyd_layer_data_list.end(); itr++){
		if(*itr){
			delete (*itr);
		}
	}
	fyd_layer_data_list.clear();

	//write block length
	uint32_t end_pos = ftell(fp);
	fseek(fp, block_length_data_pos, SEEK_SET);
	write_4byte(fp, end_pos - block_length_data_pos -sizeof(uint32_t));

	fseek(fp, end_pos, SEEK_SET);
	return true;
}

static bool write_fyd_merge_image(FILE* fp, const ImgFile_Ptr pFile)
{
	int i;

	CvSize img_size = pFile->GetImgSize();

	IplImage* channels[4];
	channels[0] = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
	channels[1] = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
	channels[2] = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
	channels[3] = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
	
	const IplImage* merge_image = pFile->GetDrawImg();
	cvSplit(merge_image, channels[0], channels[1], channels[2], channels[3]);

	//merge_image->Split(
	//	&(channels[2]), //b
	//	&(channels[1]), //g
	//	&(channels[0]), //r
	//	&(channels[3])); //a
	
	//write (a) (r) (g) (b)
	for(i=0; i<4; i++){
		if(!write_channel_pixels(
			fp,
			channels[i],
			img_size.width,
			img_size.height,
			8))
		{
			return false;
		}
		cvReleaseImage(&(channels[i]));
	}

	return true;
}

_EXPORT bool IEIOFyd_SaveAsImgFile(const char* filePath, const ImgFile_Ptr pFile)
{
	FILE* fp;
	if((fp = fopen(filePath, "wb")) == NULL){
		return false;
	}

	if(!write_fyd_header(fp, pFile)){
		return false;
	}
	if(!write_fyd_layer_block(fp, pFile)){
		return false;
	}
	if(!write_fyd_merge_image(fp, pFile)){
		return false;
	}
	fclose(fp);
	return true;
}