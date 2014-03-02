#include "stdafx.h"

#include "IEBrushIO.h"
#include "OutputError.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

bool IEBrushIO::m_isReading = false;
IEBrush_Vec IEBrushIO::m_ieBrush_Vec;

////////////////////////
/*!
	IEBrushを独自形式で保存
	@param[in] pBrush 保存するブラシ
	@param[in] path 保存先のパス
*/
void IEBrushIO::SaveIEBrush(const IEImgBrush_Ptr pBrush, const char* path)
{
	FILE *fp;
	errno_t err;

	if((err = fopen_s(&fp, path, "wb+")) != 0){
	//if(!(fp = fopen(path, "wb"))){
		char str[256];
		int last_err = GetLastError();
		sprintf(str, "ブラシファイルの保存に失敗しました。 %s err = %d lasterror = %d", path, err, last_err);
		OutputError::Alert(str);
		return;
	}

	//ブラシファイルのバージョン出力
	//1.0
	Write2ByteLE(fp, 1);
	Write2ByteLE(fp, 0);

	const IplImage* dat = pBrush->GetOrgBrushData();
	//ブラシ幅出力
	Write4ByteLE(fp, dat->width);
	//ブラシ高さ出力
	Write4ByteLE(fp, dat->height);
	//ブラシのデータ深度出力
	int depth = pBrush->GetBrushDepth();
	Write2ByteLE(fp, depth);

	//ブラシデータ出力
	int brush_size = dat->width * (depth >> 3) * dat->height;
	uchar* buffer = (uchar*) malloc (brush_size);
	
	int x,y;
	for(y=0; y<dat->height; y++){
		for(x=0; x<dat->width; x++){
			uint8_t d = GetMaskDataPos(dat, x, y);
			buffer[x + y*dat->width] = (uchar)d;
		}
	}
	fwrite(buffer, brush_size, 1, fp); 
	free(buffer);
	fclose(fp);
}

////////////////////////
/*!
	独自形式ブラシファイルを読み込み
	@param[in] path 読込先のパス
	@return 読み込み、生成したブラシオブジェクトへのパス
*/
IEBrush_Ptr IEBrushIO::LoadIEBrush(const char* path)
{
	IEImgBrush_Ptr pBrush = CreateIEImgBrush();
	FILE *fp;
	errno_t err;
	if((err = fopen_s(&fp, path, "rb")) != 0){
		char str[256];
		int last_err = GetLastError();
		sprintf(str, "ブラシファイルの読み込みに失敗しました。 %s err = %d lasterror = %d", path, err, last_err);
		OutputError::Alert(str);
		return NULL;
	}

	//ブラシファイルのバージョン読み込み
	short ver = Read2ByteLE(fp);
	short subver = Read2ByteLE(fp);

	if(ver != 1 || subver != 0){
		OutputError::Alert("error");
		fclose(fp);
		return NULL;
	}

	//ブラシ幅読み込み
	long width = Read4ByteLE(fp);
	//ブラシ高さ読み込み
	long height = Read4ByteLE(fp);
	//ブラシのデータ深度読み込み
	short depth = Read2ByteLE(fp);

	//ブラシデータ読み込み
	int size = width * (depth >> 3) * height;
	uchar* buffer =(uchar*)malloc(size);
	fread(buffer, size, 1, fp);

	pBrush->CreateBrush(buffer, width, height, depth);
	
	free(buffer);
	fclose(fp);

	return pBrush;
}

int IEBrushIO::StartReadAbrBrush(const char *path)
{
	assert(m_isReading == false);
	m_isReading = true;

	FILE *abr;
	AbrInfo abr_hdr;
	int i;

	//if(!(abr = fopen(path, "rb"))){
	errno_t err;
	if((err = fopen_s(&abr, path, "rb")) != 0){
		char str[256];
		int last_err = GetLastError();
		sprintf(str, "ブラシファイルを開けませんでした %s err = %d last_err = %d", path, err, last_err);
		OutputError::Alert(str);
		return 0;
	}

	if(!ReadAbrInfo(abr, &abr_hdr)){
		OutputError::Alert("解析不能なABRファイルです");
		fclose(abr);
		return 0;
	}

	if(!isSupportedAbrHdr(&abr_hdr)){
		char str[256];
		sprintf(str, "読み込み不能なABRファイルフォーマットのバージョンです %d.%d",
			abr_hdr.version, abr_hdr.subversion);
		OutputError::Alert(str);
		fclose(abr);
		return 0;
	}

	if(abr_hdr.count == 0){
		OutputError::Alert("ブラシのサンプルが見つかりません");
		fclose(abr);
		return 0;
	}

	m_isReading = true;

	for(i = 0; i<abr_hdr.count; i++){
		IEImgBrush_Ptr pBrush = CreateIEImgBrush();
		if(AbrBrushLoad(abr, &abr_hdr, path, pBrush, i)){
			m_ieBrush_Vec.push_back(pBrush);
		}
		//else{
		//	ReleaseIEBrush((IEBrush_Ptr*)(&pBrush));
		//}
	}

	fclose(abr);
	return m_ieBrush_Vec.size();
}

//////////////////////////
/*!
	IEBrushIOが保持しているブラシを返す。
	@return ブラシオブジェクトのポインタ
*/
IEBrush_Ptr IEBrushIO::GetBrush()
{
	assert(m_ieBrush_Vec.size() > 0);

	IEBrush_Ptr pBrush = m_ieBrush_Vec[ m_ieBrush_Vec.size() - 1 ];
	m_ieBrush_Vec.pop_back();

	return pBrush;
}

//////////////////////////
/*!
	読み込み処理の終了。
*/
void IEBrushIO::EndReadAbrBrush()
{
	assert(m_isReading == true);
	m_isReading = false;

	if(!m_ieBrush_Vec.empty()){ //まだブラシを保持しているならここで開放する。
		//IEBrush_Vec::iterator itr = m_ieBrush_Vec.begin();
		//for(; itr != m_ieBrush_Vec.end(); itr++){
		//	IEBrush_Ptr pBrush = (*itr);
		//	ReleaseIEBrush(&pBrush);
		//}
		m_ieBrush_Vec.clear();
	}
}

bool IEBrushIO::ReadAbrInfo(FILE *abr, AbrInfo *abr_hdr)
{
	abr_hdr->version = Read2ByteBE(abr);
	abr_hdr->subversion = 0;
	abr_hdr->count = 0;

	switch(abr_hdr->version){
		case 1:
		case 2:
			abr_hdr->count = Read2ByteBE(abr);
			break;
		case 6:
			abr_hdr->subversion = Read2ByteBE(abr);
			abr_hdr->count = FindSampleCountV6(abr, abr_hdr);
			break;
		default:
			return false;
			break;
	}

	return true;
}

int IEBrushIO::FindSampleCountV6(FILE *abr, AbrInfo *abr_hdr)
{
	int origin;
	int sample_section_size;
	int sample_section_end;
	int samples = 0;
	int data_start;

	int brush_size;
	int brush_end;

	origin = ftell(abr);

	if(!ReachAbr8BIMSection(abr, "samp")){
		fseek(abr, origin, SEEK_SET);
		return 0;
	}

	sample_section_size = Read4ByteBE(abr);
	sample_section_end = sample_section_size + ftell(abr);

	data_start = ftell(abr);

	while(ftell(abr) < sample_section_end){
		brush_size = Read4ByteBE(abr);
		brush_end = brush_size;
		//4の倍数に調整
		while (brush_end % 4 != 0) brush_end++;

		fseek(abr, brush_end, SEEK_CUR);
		samples++;
	}

	fseek(abr, data_start, SEEK_SET);

	return samples;
}

//////////////////////////
/*!
	8BIMタグ名がnameと等しいかどうか
*/
bool IEBrushIO::ReachAbr8BIMSection(FILE *abr, const char *name)
{
	char tag[4];
	char tagname[5];
	long section_size;
	int r;

	/* find 8BIMname section */
	while(!feof(abr)) {
		r = fread(&tag, 1, 4, abr);
		if (r != 4) {
			OutputError::PushLog( LOG_LEVEL::_ERROR, "Cannot read 8BIM tag");
			return false;
		}

		if (strncmp(tag, "8BIM", 4)) {
			OutputError::PushLog( LOG_LEVEL::_ERROR, "Start tag not 8BIM\n");
			return false;
		}

		r = fread(&tagname, 1, 4, abr);
		if (r != 4) {
			OutputError::PushLog( LOG_LEVEL::_ERROR, "Cannot read 8BIM tag name");
			return false;
		}

		tagname[4] = '\0';

		if (!strncmp(tagname, name, 4))
			return true;

		section_size = Read4ByteBE(abr);
		r = fseek(abr, section_size, SEEK_CUR);
		if(r == -1)
			return false;
	}
	return false;
}

/////////////////////////
/*!
	ブラシのバージョンが読み込み可能なものか？
*/
bool IEBrushIO::isSupportedAbrHdr(const AbrInfo *abr_hdr)
{
	switch(abr_hdr->version){
		case 1:
		case 2:
			return true;
			break;
		case 6:
			if(abr_hdr->subversion == 1 || abr_hdr->subversion == 2)
				return true;
			break;
	}

	return false;
}

/////////////////////////
/*!
	ブラシの読み込み
*/
bool IEBrushIO::AbrBrushLoad(FILE *abr, const AbrInfo *abr_hdr, const char *filename, IEBrush_Ptr pBrush, int id)
{
	switch(abr_hdr->version){
		case 1:
		case 2:
			return AbrBrushLoad_v12(abr, abr_hdr, filename, pBrush, id);
		case 6:
			return AbrBrushLoad_v6(abr, abr_hdr, filename, pBrush, id);
	}
	return false;
}

//////////////////////////
/*!
	ver1 ver2 のブラシの読み込み
*/
bool IEBrushIO::AbrBrushLoad_v12(FILE *abr, const AbrInfo *abr_hdr, const char *filename, IEBrush_Ptr pBrush, int id)
{
	bool bl = true;

	int brush_type;
	int brush_size;
	int next_brush;

	int top,left,bottom,right;
	int depth;
	char compression;
	char *name = NULL;

	int width, height;
	int size;

	brush_type = Read2ByteBE(abr);
	brush_size = Read4ByteBE(abr);
	next_brush = ftell(abr) + brush_size;

	switch(brush_type){
		case 1:/* computed brush はサポートしない */
			fseek(abr, next_brush, SEEK_CUR);
			bl = false;
			break;
		case 2: /*sampled brush*/
			fseek(abr, 6, SEEK_CUR);

			if(abr_hdr->version == 2)
				name = abr_read_ucs2_text(abr);
			if(name == NULL)
				name = abr_v1_brush_name(filename, id);

			/* discard 1 byte for antialiasing and 4 x short for short bounds */
			fseek(abr, 9, SEEK_CUR);

			top = Read4ByteBE(abr);
			left = Read4ByteBE(abr);
			bottom = Read4ByteBE(abr);
			right = Read4ByteBE(abr);
			depth = Read2ByteBE(abr);
			compression = Read1ByteBE(abr);

			width = right - left;
			height = bottom - top;
			size = width * (depth >> 3) * height;

			if(height > 16384){
				OutputError::PushLog( LOG_LEVEL::_WARN, "wide brushes not supported");
				fseek(abr, next_brush, SEEK_SET);
				bl = false;
				break;
			}

			if(!compression){
				AbrBrushNormalLoad(abr, width, height, depth, pBrush);
			}else{
				AbrBrushDecodeLoad(abr, width, height, depth, pBrush);
			}

			if(name){
				free(name);
			}
			break;
		default:
			OutputError::PushLog( LOG_LEVEL::_WARN, "unknown brush type, skipping");
			fseek(abr, next_brush, SEEK_SET);
			bl = false;
			break;
	}

	return bl;
}

////////////////////////////
/*!
	ver6　のブラシの読み込み
*/
bool IEBrushIO::AbrBrushLoad_v6(FILE *abr, const AbrInfo *abr_hdr, const char *filename, IEBrush_Ptr pBrush, int id)
{
	bool bl = true;

	int brush_size;
	int brush_end;
	int complement_to_4;
	int next_brush;

	int top,left,bottom,right;
	short depth;
	char compression;
	char *name = NULL;

	int width, height;

	brush_size = Read4ByteBE(abr);
	brush_end = brush_size;
	//4の倍数に調整
	while(brush_end % 4 != 0) brush_end++;
	complement_to_4 = brush_end - brush_size;
	next_brush = ftell(abr) + brush_end;

	if(abr_hdr->subversion == 1)
		//discard short coordinates and unknown short
		fseek(abr, 47, SEEK_CUR);
	else
		//discard unknown bytes
		fseek(abr, 301, SEEK_CUR);

	top    = Read4ByteBE(abr);
	left   = Read4ByteBE(abr);
	bottom = Read4ByteBE(abr);
	right  = Read4ByteBE(abr);
	depth  = Read2ByteBE(abr);
	compression = Read1ByteBE(abr);

	width = right - left;
	height = bottom - top;

	name = abr_v1_brush_name(filename, id);

	if(!compression){
		AbrBrushNormalLoad(abr, width, height, depth, pBrush);
	}else{
		AbrBrushDecodeLoad(abr, width, height, depth, pBrush);
	}

	if(name){
		free(name);
	}

	fseek(abr, next_brush, SEEK_SET);

	return bl;
}

void IEBrushIO::AbrBrushNormalLoad(FILE *abr, int width, int height, short depth, IEBrush_Ptr pBrush)
{
	int size = width * (depth >> 3) * height;
	uchar* buffer =(uchar*)malloc(size);
	fread(buffer, size, 1, abr);

	pBrush->CreateBrush(buffer, width, height, depth);

	free(buffer);
}

void IEBrushIO::AbrBrushDecodeLoad(FILE *abr, int width, int height, short depth, IEBrush_Ptr pBrush)
{
	int size = width * (depth >> 3) * height;
	unsigned char* buffer = (unsigned char*)malloc(size);
	
	int n;
	char ch;
	int i, j, c;
	short *cscanline_len;
	char *data = (char*)buffer;
	int count=0;
	int cs_len=0;


	/* read compressed size foreach scanline */
	cscanline_len = (short*)malloc(sizeof(short) * height);
	for (i = 0; i < height; i++){
		cscanline_len[i] = Read2ByteBE(abr);
		cs_len += cscanline_len[i];
	}

	/* unpack each scanline data */
	for (i = 0; i < height; i++) {
		for (j = 0; j < cscanline_len[i];) {
			
			if(count > width*height){
				::MessageBox(NULL, "err", "", MB_OK);
			}

			n = Read1ByteBE(abr);
			j++;
			if (n >= 128)     /* force sign */
				n -= 256;

			if (n < 0) {      /* copy the following char -n + 1 times */
				if (n == -128)  /* it's a nop */
					continue;

				n = -n + 1;
				ch = Read1ByteBE(abr);
				j++;
				for (c = 0; c < n; c++, data++){
					 *data = ch;
					 count++;
				}
			}
			else {          /* read the following n + 1 chars (no compr) */
				for (c = 0; c < n + 1; c++, j++, data++){
					ch = Read1ByteBE(abr);
					*data = ch;
					count++;
				}
			}
		}
	}

	free(cscanline_len);

	pBrush->CreateBrush(buffer, width, height, depth);

	free(buffer);
}

char* IEBrushIO::abr_v1_brush_name(const char* filename, int id)
{
	return NULL;
}

char* IEBrushIO::abr_read_ucs2_text(FILE *abr)
{
	return NULL;
}
