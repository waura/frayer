#include "IEIOpsd.h"

#include "RLE.h"

static void write_4byte(FILE* fp, uint32_t val)
{
	fwrite(&val, 1, 4, fp);
}

static void write_4byte_BE(FILE* fp, uint32_t val)
{
	uint32_t be_val = CONVERT_UINT32_BE(val);
	fwrite(&be_val, 1, 4, fp);
}

static void write_2byte_BE(FILE* fp, uint16_t val)
{
	uint16_t be_val = CONVERT_UINT16_BE(val);
	fwrite(&be_val, 1, 2, fp);
}

static void write_1byte_BE(FILE* fp, uint8_t val)
{
	fwrite(&val, 1, 1, fp);
}

static void write_string(FILE* fp, const char* val)
{
	int len = strlen(val);
	write_1byte_BE(fp, len);

	if(len == 0) return;

	fwrite(val, len, 1, fp);
}

static bool write_psd_header(FILE* fp, const ImgFile_Ptr pFile)
{
	fwrite("8BPS", 1, 4, fp);
	write_2byte_BE(fp, 1); //version
	write_2byte_BE(fp, 0); //reserved
	write_2byte_BE(fp, 0); //reserved
	write_2byte_BE(fp, 0); //reserved
	write_2byte_BE(fp, 3); //channels

	CvSize img_size = pFile->GetImgSize();
	write_4byte_BE(fp, img_size.height); //row
	write_4byte_BE(fp, img_size.width);  //col
	write_2byte_BE(fp, 8); //depth
	write_2byte_BE(fp, PSD_COLOR_MODE_RGB);

	return true;
}

static bool write_psd_color_mode_data(FILE* fp, const ImgFile_Ptr pFile)
{
	write_4byte_BE(fp, 0);
	return true;
}

static bool write_psd_image_resource(FILE* fp, const ImgFile_Ptr pFile)
{
	int i;
	uint32_t res_pos;
	uint32_t eof_pos;
	uint32_t name_pos;
		
	res_pos = ftell(fp);
	write_4byte_BE(fp, 0); //image resource length

	////write cahnnel name
	//fwrite("8BIM", 1, 4, fp); //sig
	//write_2byte_BE(fp, 0x03EE);
	//write_2byte_BE(fp, 0); //id name length
	//name_pos = ftell(fp);
	//write_4byte_BE(fp, 0); //0x03EE resource sizse
	//write_string(fp, "Transparency"); //channel name
	//write_string(fp, "");

	//eof_pos = ftell(fp);
	//fseek(fp, name_pos, SEEK_SET);
	//write_4byte_BE(fp, eof_pos - name_pos - sizeof(uint32_t)); //0x03EE resource size
	//fseek(fp, eof_pos, SEEK_SET);

	//write resolution dasta
	//uint32_t xres_fix = 4718592;
	//uint32_t yres_fix = 4718592; 
	//fwrite("8BIM", 1, 4, fp); //sig
	//write_2byte_BE(fp, 0x03ED); //id
	//write_2byte_BE(fp, 0); //id name length
	//write_4byte_BE(fp, 0); //resource size
	//write_4byte_BE(fp, xres_fix); //hRes
	//write_2byte_BE(fp, 1); //hRes unit
	//write_2byte_BE(fp, 1); //width unit
	//write_4byte_BE(fp, yres_fix); //vRes
	//write_2byte_BE(fp, 1); //vRes unit
	//write_2byte_BE(fp, 1); //height unit

	////write active layer number
	//fwrite("8BIM", 1, 4, fp); //sig
	//write_2byte_BE(fp, 0x0400); //id
	//write_2byte_BE(fp, 0); //id name size
	//write_4byte_BE(fp, sizeof(uint16_t)); //resource size

	//write_2byte_BE(fp, 0);

	//eof_pos = ftell(fp);
	//fseek(fp, res_pos, SEEK_SET);
	//write_4byte_BE(fp, eof_pos - res_pos - sizeof(uint32_t));
	//fseek(fp, eof_pos, SEEK_SET);

	return true;
}

static bool write_layer_resource_luni(FILE* fp, const char* val)
{
	uint32_t str_len = strlen(val);
	wchar_t* pwsz = (wchar_t*) malloc(sizeof(wchar_t)*str_len+1);
	uint32_t length = ::MultiByteToWideChar(CP_ACP, 0, val, str_len, pwsz, sizeof(wchar_t)*str_len+1);

	uint32_t block_size;
	if(length % 2)
		block_size = length + 1;
	else
		block_size = length;

	block_size = (block_size*2) + 4;

	fwrite("8BIMluni", 8, 1, fp);
	write_4byte_BE(fp, block_size);
	write_4byte_BE(fp, length);

	int i;
	for(i=0; i<length; i++)
		write_2byte_BE(fp, pwsz[i]);

	if(length % 2)
		write_2byte_BE(fp, 0);

	free(pwsz);

	return true;
}

static bool write_layer(FILE* fp, PsdLayerData* pLayerData)
{
	int i;
	write_4byte_BE(fp, pLayerData->rect.top);
	write_4byte_BE(fp, pLayerData->rect.left);
	write_4byte_BE(fp, pLayerData->rect.bottom);
	write_4byte_BE(fp, pLayerData->rect.right);
	if(pLayerData->is_mask){
		write_2byte_BE(fp, 5); //channel_num + mask
		pLayerData->psd_channels = (PsdChannel*) malloc(sizeof(PsdChannel)*5);
	}
	else{
		write_2byte_BE(fp, 4); //channel_num
		pLayerData->psd_channels = (PsdChannel*) malloc(sizeof(PsdChannel)*4);
	}

	int l_width = pLayerData->rect.right - pLayerData->rect.left;
	int l_height = pLayerData->rect.bottom - pLayerData->rect.top;
	uint32_t channel_data_length = sizeof(uint16_t) + l_width*l_height;

	//alpha
	write_2byte_BE(fp, PSD_CHANNEL_ALPHA);
	pLayerData->psd_channels[0].data_length_pos = ftell(fp);
	write_4byte_BE(fp, channel_data_length);

	//red
	write_2byte_BE(fp, PSD_CHANNEL_RED);
	pLayerData->psd_channels[1].data_length_pos = ftell(fp);
	write_4byte_BE(fp, channel_data_length);

	//green
	write_2byte_BE(fp, PSD_CHANNEL_GREEN);
	pLayerData->psd_channels[2].data_length_pos = ftell(fp);
	write_4byte_BE(fp, channel_data_length);

	//blue
	write_2byte_BE(fp, PSD_CHANNEL_BLUE);
	pLayerData->psd_channels[3].data_length_pos = ftell(fp);
	write_4byte_BE(fp, channel_data_length);

	if(pLayerData->is_mask){
		//mask
		write_2byte_BE(fp, PSD_CHANNEL_MASK);
		pLayerData->psd_channels[4].data_length_pos = ftell(fp);
		write_4byte_BE(fp, channel_data_length);
	}

	fwrite("8BIM", 4, 1, fp);

	write_4byte(fp, pLayerData->blend_fourcc);

	write_1byte_BE(fp, pLayerData->opacity); //opacity
	write_1byte_BE(fp, 0); //clipping
	write_1byte_BE(fp, pLayerData->flags); //flags
	write_1byte_BE(fp, 0); //filler

	uint32_t extra_length_data_pos = ftell(fp);
	write_4byte_BE(fp, 0); //extra_len

	if(pLayerData->is_mask){
		write_4byte_BE(fp, 20); //mask_block_length

		RECT mask_rc;
		pLayerData->mask_data->GetLayerRect(&mask_rc);
		write_4byte_BE(fp, mask_rc.top);
		write_4byte_BE(fp, mask_rc.left);
		write_4byte_BE(fp, mask_rc.bottom);
		write_4byte_BE(fp, mask_rc.right);
		write_1byte_BE(fp, 0);
		write_1byte_BE(fp, 0);
		write_2byte_BE(fp, 0);
	}
	else{
		write_4byte_BE(fp, 0); //mask_block_length
	}

	write_4byte_BE(fp, 0); //layer blending ranges block_length

	//write layer_name
	uint32_t layer_name_length = strlen(pLayerData->name);
	char* layer_name = pLayerData->name;
	if(layer_name_length != 0){
		uint32_t length = (layer_name_length > 255) ? 255 : layer_name_length;

		write_1byte_BE(fp, length);
		fwrite(layer_name, 1, length, fp);

		length++;
		if((length % 4) != 0){
			for(i=0; i< (4-(length % 4)); i++){
				write_1byte_BE(fp, 0);
			}
		}
	}
	else{
		write_1byte_BE(fp, 0);
	}

	////write utf layer resource block
	write_layer_resource_luni(fp, layer_name);

	uint32_t end_pos = ftell(fp);

	//write extra data size
	fseek(fp, extra_length_data_pos, SEEK_SET);
	write_4byte_BE(fp, end_pos - extra_length_data_pos - sizeof(uint32_t));

	fseek(fp, end_pos, SEEK_SET);

	return true;
}

static bool write_channel_pixels(
	FILE* fp,
	IplImage* channel,
	int c_width,
	int c_height,
	int c_depth,
	const PsdChannel* psd_channel)
{
	int i,j;
	write_2byte_BE(fp, PSD_CM_RLE); //compression

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
	}

	uint32_t* rle_len = rle_pack_len;
	for(i=0; i<c_height; i++){
		write_2byte_BE(fp, (*rle_len));
		rle_len++;
	}
	uint32_t channel_data_length=2 + c_height*sizeof(uint16_t);

	rle_len = rle_pack_len;
	for(i=0; i<c_height; i++){
		uint8_t* write_line = GetPixelAddress(dst, 0, i);
		channel_data_length += rle_pack_len[i];
		for(j=0; j<(*rle_len); j++){
			write_1byte_BE(fp, (*write_line));
			write_line++;
		}
		rle_len++;
	}
	free(rle_pack_len);
	cvReleaseImage(&dst);

	//write channel data length
	if(psd_channel){
		fseek(fp, psd_channel->data_length_pos, SEEK_SET);
		write_4byte_BE(fp, channel_data_length);
		fseek(fp, 0, SEEK_END);
	}

	return true;
}

static bool write_layer_pixels(FILE* fp, const PsdLayerData* pLayerData)
{
	if(pLayerData->flags & 0x10){//is group layer
		write_2byte_BE(fp, PSD_CM_NONE); //compression
		write_2byte_BE(fp, PSD_CM_NONE); //compression
		write_2byte_BE(fp, PSD_CM_NONE); //compression
		write_2byte_BE(fp, PSD_CM_NONE); //compression
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
			8,
			&(pLayerData->psd_channels[i])))
		{
			goto return_false;
		}
	}

	if(pLayerData->is_mask){
		//write layer mask
		int x,y;
		int8_t mask_val;
		UCvPixel pixel;
		const IplImageExt* layer_image = pLayerData->mask_data->GetLayerImage();
		for(y=0; y<c_height; y++){
			for(x=0; x<c_width; x++){
				layer_image->GetPixel(x, y, &pixel);
				mask_val = min(pixel.b, min(pixel.g, pixel.r));
				SetMaskDataPos(channels[0], x, y, mask_val);
			}
		}

		if(!write_channel_pixels(
			fp,
			channels[0],
			c_width,
			c_height,
			8,
			&(pLayerData->psd_channels[4])))
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

static void count_layer_and_mask(
	ImgLayerGroup_Ptr pLGroup,
	PsdLayerData_List& psd_layer_data_list)
{
	PsdLayerData* psd_layer_data = NULL;
	IImgLayer_Ptr pLayer = pLGroup->GetBottomChildLayer();
	while(pLayer != NULL){
		if(pLayer->GetLayerType() == IE_LAYER_TYPE::GROUP_LAYER){
			//close group layer psd layer data
			psd_layer_data = (PsdLayerData*) malloc(sizeof(PsdLayerData));
			memset(psd_layer_data, 0, sizeof(PsdLayerData));

			psd_layer_data->blend_fourcc = pLayer->GetLayerSynthFourCC();
			psd_layer_data->flags |= 0x8;
			if(!pLayer->IsVisible())
				psd_layer_data->flags |= 0x2;
			psd_layer_data->flags |= 0x10;
			
			int name_len = strlen("</Layer group>") + 1;
			psd_layer_data->name = (char*) malloc(sizeof(char)*name_len);
			strcpy(psd_layer_data->name, "</Layer group>");
			psd_layer_data_list.push_back( psd_layer_data );

			count_layer_and_mask(std::dynamic_pointer_cast<ImgLayerGroup>(pLayer), psd_layer_data_list);

			//group layer psd layer data
			psd_layer_data = (PsdLayerData*) malloc(sizeof(PsdLayerData));
			memset(psd_layer_data, 0, sizeof(PsdLayerData));

			psd_layer_data->opacity = pLayer->GetOpacity();
			psd_layer_data->blend_fourcc = pLayer->GetLayerSynthFourCC();
			psd_layer_data->flags |= 0x8;
			if(!pLayer->IsVisible())
				psd_layer_data->flags |= 0x2;
			psd_layer_data->flags |= 0x10;
			psd_layer_data->pLayer = NULL;

			name_len = pLayer->GetNameSize() + 1;
			psd_layer_data->name = (char*) malloc(sizeof(char)*name_len);
			pLayer->GetName(psd_layer_data->name);
			psd_layer_data_list.push_back( psd_layer_data );
		}
		else{
			ImgLayer_Ptr layer = std::dynamic_pointer_cast<ImgLayer>(pLayer);
			if( layer->isAlphaMask() ){
				if( psd_layer_data ){
					psd_layer_data->is_mask = true;
					psd_layer_data->mask_data = layer;
				}
			}
			else{
				//normal layer psd layer data
				psd_layer_data = (PsdLayerData*) malloc(sizeof(PsdLayerData));
				memset(psd_layer_data, 0, sizeof(PsdLayerData));

				pLayer->GetLayerRect( &(psd_layer_data->rect) );
				psd_layer_data->opacity = pLayer->GetOpacity();
				psd_layer_data->is_mask = false;
				psd_layer_data->blend_fourcc = pLayer->GetLayerSynthFourCC();
				psd_layer_data->mask_data = NULL;
				psd_layer_data->flags |= 0x8;
				if( !pLayer->IsVisible() )
					psd_layer_data->flags |= 0x2;
				psd_layer_data->pLayer = layer;

				int name_len = pLayer->GetNameSize() + 1;
				psd_layer_data->name = (char*) malloc(sizeof(char)*name_len);
				pLayer->GetName(psd_layer_data->name);
				psd_layer_data_list.push_back( psd_layer_data );
			}
		}

		pLayer = pLayer->GetOverLayer().lock();
	}
}

static bool write_psd_layer_and_mask(FILE* fp, const ImgFile_Ptr pFile)
{
	int i;
	uint32_t block_length_data_pos = ftell(fp);
	write_4byte_BE(fp, 0); //layer_mask block_length
	uint32_t nSize_data_pos = ftell(fp);
	write_4byte_BE(fp, 0); //layers info section length

	uint16_t layer_cnt=0;
	ImgLayerGroup_Ptr pRootLayerGroup = pFile->GetRootLayerGroup();

	PsdLayerData_List psd_layer_data_list;
	count_layer_and_mask(pRootLayerGroup, psd_layer_data_list);
	write_2byte_BE(fp, psd_layer_data_list.size()); //num of layer

	//
	PsdLayerData_List::iterator itr = psd_layer_data_list.begin();
	for(; itr != psd_layer_data_list.end(); itr++){
		if(!write_layer(fp, (*itr))){
			return false;
		}
	}

	//write layer pixels
	itr = psd_layer_data_list.begin();
	for(; itr != psd_layer_data_list.end(); itr++){
		if(!write_layer_pixels(fp, (*itr))){
			return false;
		}
	}

	free_psd_layer_data_list(psd_layer_data_list);
	psd_layer_data_list.clear();

	uint32_t end_pos = ftell(fp);
	//write actual size of layer info section
	fseek(fp, nSize_data_pos, SEEK_SET);
	write_4byte_BE(fp, end_pos - nSize_data_pos - sizeof(uint32_t));

	//write actual size of layer and mask info section
	fseek(fp, block_length_data_pos, SEEK_SET);
	write_4byte_BE(fp, end_pos - block_length_data_pos - sizeof(uint32_t));

	fseek(fp, end_pos, SEEK_SET);
	return true;
}

static void
write_merge_image_channel_pixel(
	FILE   *fp,
	IplImage* pixel_data,
	int depth,
	int32_t  length_table_pos)
{
	CvSize size;
	size.width = pixel_data->width;
	size.height = pixel_data->height;
	int32_t height = size.height;
	int32_t width = size.width;
	int32_t bytes = depth / 8;
	int x, y;

	int bytes_per_row = width * bytes;

	int32_t len;
	
	uint32_t* rle_pack_len = (uint32_t*) malloc(height * sizeof(uint32_t));
	uint8_t* dst = (uint8_t*) malloc(bytes_per_row * sizeof(uint8_t*));
	uint8_t* src = (uint8_t*) malloc(bytes_per_row * sizeof(uint8_t));
	for(y=0; y<height; y++){
		uint8_t* pixs = GetPixelAddress(pixel_data, 0, y);
		uint8_t* line = src;
		for(x=0; x<width; x++){
			(*line) = (*pixs);
			line++;
			pixs++;
		}

		encode_rle((int8_t*)src, bytes_per_row, (int8_t*)dst, &(rle_pack_len[y]));

		//write complessed pixel
		for(x=0; x<rle_pack_len[y]; x++){
			write_1byte_BE(fp, dst[x]);
		}
	}
	free(src);
	free(dst);
		
	//write complessed pixel length table
	fseek (fp, length_table_pos, SEEK_SET);
	for (y = 0; y < height; y++){
		write_2byte_BE(fp, rle_pack_len[y]); //RLE length	
	}
	
	free(rle_pack_len);

	fseek (fp, 0, SEEK_END);
}

bool write_merge_image(FILE* fp, const ImgFile_Ptr pFile)
{
	int i,j;
	long offset;
	write_2byte_BE(fp, 1); //RLE compression

	int rle_length_pos[4];
	CvSize img_size = pFile->GetImgSize();
	for(i=0; i<3; i++){
		rle_length_pos[i] = ftell(fp);
		for(j=0; j<img_size.height; j++){
			write_2byte_BE(fp, 0); //padding header
		}
	}

	IplImage* channels[4];
	channels[0] = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
	channels[1] = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
	channels[2] = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
	channels[3] = cvCreateImage(img_size, IPL_DEPTH_8U, 1);
	
	const IplImage* merge_image = pFile->GetDrawImg();

	cvSplit(
		merge_image,
		channels[2], //b
		channels[1], //g
		channels[0], //r
		channels[3]); //a

	write_merge_image_channel_pixel(fp, channels[0], 8, rle_length_pos[0]);
	write_merge_image_channel_pixel(fp, channels[1], 8, rle_length_pos[1]);
	write_merge_image_channel_pixel(fp, channels[2], 8, rle_length_pos[2]);
	//write_merge_image_channel_pixel(fp, channels[3], 8, rle_length_pos[3]);

	return true;
}

IEIO_EXPORT bool IEIO_SaveAsImgFile(const char* filePath, ImgFile_Ptr pFile)
{
	FILE* fp;
	if((fp = fopen(filePath, "wb")) == NULL){
		return false;
	}

	if(!write_psd_header(fp, pFile)){
		fclose(fp);
		return false;
	}

	if(!write_psd_color_mode_data(fp, pFile)){
		fclose(fp);
		return false;
	}

	if(!write_psd_image_resource(fp, pFile)){
		fclose(fp);
		return false;
	}

	if(!write_psd_layer_and_mask(fp, pFile)){
		fclose(fp);
		return false;
	}

	if(!write_merge_image(fp, pFile)){
		fclose(fp);
		return false;
	}

	fclose(fp);
	return true;
}