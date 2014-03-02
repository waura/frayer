// IEIOpng.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "IEIOpng.h"
#include "resource.h"

#include <png.h>

#ifdef _MANAGED
#pragma managed(push, off)
#endif

HMODULE g_hModule;
bool g_isInterlaced;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	g_hModule = hModule;
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

IEIO_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::IMG_FILE_IO;
}

IEIO_EXPORT void IEIO_GetFilter(char filter[], size_t buf_size)
{
	strcpy_s(filter, buf_size, "Portable Network Graphics(*.png)|*.png;*.PNG");
}

IEIO_EXPORT bool IEIO_LoadImgFile(const char *filePath, ImgFile_Ptr pFile)
{
	if(filePath == NULL) return false;

	//pngファイルからデータ読み込み
	IplImage *imgBGRA = LoadPng(filePath);
	if(imgBGRA){

		pFile->Init(cvSize(imgBGRA->width, imgBGRA->height));
	
		//add new layer
		AddNewLayerHandle* handle = (AddNewLayerHandle*)pFile->CreateImgFileHandle(IFH_ADD_NEW_LAYER);
		handle->Do( pFile );
		ImgLayer_Ptr new_layer = handle->GetNewLayer().lock();
		pFile->ReleaseImgFileHandle(handle);
		handle = NULL;

		pFile->SetSelectLayer( new_layer );

		//
		new_layer->ImgBltToLayer(
			0, 0,
			imgBGRA->width, imgBGRA->height,
			imgBGRA,
			0, 0);

		//
		LPUPDATE_DATA pData = new_layer->CreateUpdateData();
		pData->isAll = true;
		new_layer->PushUpdateData( pData );

		//
		NullHandle* null_handle = (NullHandle*)pFile->CreateImgFileHandle(IFH_NULL);
		strcpy_s(null_handle->handle_name, MAX_IMG_FILE_HANDLE_NAME, "pngファイル読み込み");
		pFile->DoImgFileHandle( null_handle );

		//
		cvReleaseImage(&imgBGRA);
		return true;
	}

	return false;
}

IEIO_EXPORT bool IEIO_SaveAsImgFile(const char *filePath, ImgFile_Ptr pFile)
{
	//ファイルオブジェクトから出力画像を得る。
	CvSize size = pFile->GetImgSize();
	IplImage* img = cvCreateImage(size, IPL_DEPTH_8U, 4);
	pFile->CopyFileImage(img);

	bool isInterlaced = false;
	::DialogBox((HINSTANCE)g_hModule, MAKEINTRESOURCE(IDD_SAVE_PNG), NULL, (DLGPROC)SavePngDlgProc);

	//ファイルへ出力
	return SavePng(filePath, img, g_isInterlaced);
}

IplImage* LoadPng(const char *filePath)
{
	FILE* pFile = fopen(filePath, "rb");
	if(!pFile){
		return NULL;
	}

	const size_t numSizeSignature = 8;
	const int numBytesSignature = sizeof(BYTE) * numSizeSignature;
	BYTE signature[numBytesSignature];

	size_t numSizeReadSignature = fread(signature, sizeof(BYTE),
										numSizeSignature, pFile);

	if(numSizeReadSignature != numSizeSignature){
		fclose( pFile );
		return NULL;
	}

	if(!png_check_sig( signature, numBytesSignature )){
		fclose( pFile );
		return NULL;
	}

	png_struct* pPngStruct = png_create_read_struct( PNG_LIBPNG_VER_STRING,
													NULL, NULL, NULL );
	if(!pPngStruct){
		fclose( pFile );
		return NULL;
	}

	png_info* pPngInfo = png_create_info_struct( pPngStruct );
	if(!pPngInfo){
		png_destroy_read_struct( &pPngStruct, NULL, NULL );
		fclose( pFile );
		return NULL;
	}

	png_byte** ppRowTable = 0;

	if(setjmp(pPngStruct->jmpbuf)){
		if(ppRowTable){
			delete[] ppRowTable;
			ppRowTable = NULL;
		}

		png_destroy_read_struct( &pPngStruct, &pPngInfo, NULL);
		fclose( pFile );
		return NULL;
	}

	png_init_io( pPngStruct, pFile );
	png_set_sig_bytes( pPngStruct, numBytesSignature );
	png_read_info( pPngStruct, pPngInfo );

	png_uint_32 width = 0;
	png_uint_32 height = 0;
	int bit_depth = 0;
	int color_type = 0;
	png_get_IHDR( pPngStruct, pPngInfo, &width, &height, &bit_depth, &color_type,
				NULL, NULL, NULL);

	if(color_type == PNG_COLOR_TYPE_PALETTE ||
		(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) ||
		png_get_valid( pPngStruct, pPngInfo, PNG_INFO_tRNS )){
			png_set_expand( pPngStruct );
	}

	if(bit_depth == 16){
		png_set_strip_16( pPngStruct );
	}

	if(color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA){
			png_set_gray_to_rgb( pPngStruct );
	}

	png_set_bgr( pPngStruct );

	if(color_type != PNG_COLOR_TYPE_GRAY_ALPHA &&
		color_type != PNG_COLOR_TYPE_RGB_ALPHA){
			png_set_filler( pPngStruct, 255, PNG_FILLER_AFTER );
	}

	const double gammaLUT = 1.0f;
	const double gammaCRT = 2.2f;
	double display_exponent = gammaLUT * gammaCRT;
	double gammaFile = 0.0f;
	if(png_get_gAMA( pPngStruct, pPngInfo, &gammaFile)){
		png_set_gamma( pPngStruct, display_exponent, gammaFile );
	}

	png_read_update_info( pPngStruct, pPngInfo );

	IplImage* iplImg = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 4);
	ppRowTable = new png_byte*[height];
	for(UINT y=0; y < height; y++){
		ppRowTable[y] = reinterpret_cast<BYTE*>(&(iplImg->imageData[y * iplImg->widthStep]));
	}

	png_read_image( pPngStruct, ppRowTable );
	png_read_end( pPngStruct, NULL );

	if(ppRowTable){
		delete[] ppRowTable;
		ppRowTable = NULL;
	}

	png_destroy_read_struct( &pPngStruct, &pPngInfo, NULL );
	fclose( pFile );

	return iplImg;
}

bool SavePng(const char *filePath, IplImage *img, bool isInterlaced)
{
	if(!filePath){
		return false;
	}

	FILE* pFile = fopen(filePath, "wb");
	if(!pFile){
		return false;
	}

	png_struct* pPngStruct = png_create_write_struct( PNG_LIBPNG_VER_STRING,
													NULL, NULL, NULL);

	if(!pPngStruct){
		fclose(pFile);
		return false;
	}

	png_info* pPngInfo = png_create_info_struct(pPngStruct);
	if(!pPngInfo){
		png_destroy_write_struct(&pPngStruct, NULL);
		fclose(pFile);
		return false;
	}

	png_byte** ppRowTable = 0;
	if(setjmp( pPngStruct->jmpbuf )){
		if(ppRowTable){
			delete[] ppRowTable;
			ppRowTable = NULL;
		}
		png_destroy_write_struct( &pPngStruct, &pPngInfo );
		fclose(pFile);
		return false;
	}

	png_init_io(pPngStruct, pFile);
	png_set_compression_level(pPngStruct, Z_BEST_COMPRESSION);

	png_set_IHDR( pPngStruct, pPngInfo,
		img->width, img->height,
		8, PNG_COLOR_TYPE_RGB_ALPHA,
		isInterlaced ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	double CRT_exponent = 2.2;
	double LUT_exponent = 1.0;
	double default_display_exponent = LUT_exponent * CRT_exponent;
	double default_gamma = 1.0 / default_display_exponent;
	png_set_gAMA( pPngStruct, pPngInfo, default_gamma );

	png_write_info( pPngStruct, pPngInfo );

	png_set_bgr( pPngStruct );

	if(isInterlaced){
		ppRowTable = new png_byte*[img->height];
		for(UINT y = 0; y < img->height; y++){
			ppRowTable[y] = const_cast<png_byte*>(reinterpret_cast<const png_byte*>(&(img->imageData[y * img->widthStep])));
		}
		png_write_image(pPngStruct, ppRowTable);
	}
	else{
		for(UINT y=0; y < img->height; y++){
			png_write_row(pPngStruct,
				const_cast<png_byte*>(reinterpret_cast<const png_byte*>(&(img->imageData[y * img->widthStep]))));
		}
	}
	png_write_end(pPngStruct, NULL);

	if(ppRowTable){
		delete[] ppRowTable;
		ppRowTable = NULL;
	}
	png_destroy_write_struct(&pPngStruct, &pPngInfo);
	fclose(pFile);

	return true;
}

BOOL CALLBACK SavePngDlgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uiMsg) {
	case WM_INITDIALOG:
		SendMessage(GetDlgItem(hWnd, IDC_RADIO2), BM_SETCHECK, (WPARAM)1, 0L); 
		return TRUE;

	case WM_COMMAND:
		if(LOWORD(wParam) == IDOK) {
			//OKボタンが押されたときの処理
			if(IsDlgButtonChecked(hWnd, IDC_RADIO1) == BST_CHECKED){
				g_isInterlaced = true;
			}
			else if (IsDlgButtonChecked(hWnd, IDC_RADIO2) == BST_CHECKED){
				g_isInterlaced = false;
			}
			EndDialog(hWnd, IDOK);
			return TRUE;
		}
		break;

	default:
		break;
	}
	return FALSE;
}