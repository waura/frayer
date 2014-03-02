// IEIOjpeg.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "IEIOjpeg.h"

#define XMD_H

#include <stdio.h>
#include <setjmp.h>
extern "C" {
#include "jpeglib.h"
}
#include "resource.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif

HMODULE g_hModule;
int g_quality=100;

struct JpegErrorManageStruct {
	jpeg_error_mgr m_filedPublic;
	jmp_buf m_bufferSetJmp;
};

void exitByLongJump(j_common_ptr pInfo)
{
	JpegErrorManageStruct* pErrorManager = reinterpret_cast<JpegErrorManageStruct*>(pInfo->err);
	longjmp( pErrorManager->m_bufferSetJmp, 1);
}

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
	strcpy_s(filter, buf_size, "JPEG(*.jpg;*.jpeg)|*.jpg;*.JPG;*.jpeg;*.JPEG");
}

IEIO_EXPORT bool IEIO_LoadImgFile(const char *filePath, ImgFile_Ptr pFile)
{
	////jpgファイルからデータ読み込み
	if(filePath == NULL){
		return false;
	}

	FILE* fp = fopen(filePath, "rb");
	if(!fp){
		return false;
	}

	jpeg_decompress_struct jpegStruct;
	jpeg_create_decompress(&jpegStruct);
	JpegErrorManageStruct structErrorManagement;
	jpegStruct.err = jpeg_std_error( &structErrorManagement.m_filedPublic );
	structErrorManagement.m_filedPublic.error_exit = exitByLongJump;
	if(setjmp(structErrorManagement.m_bufferSetJmp)){
		jpeg_destroy_decompress( &jpegStruct );
		fclose( fp );
		return false;
	}

	jpeg_stdio_src( &jpegStruct, fp );
	jpeg_read_header( &jpegStruct, TRUE );
	jpeg_start_decompress( &jpegStruct );
	int row_stride = jpegStruct.output_width * jpegStruct.output_components;

	JSAMPARRAY ppBuffer = (*jpegStruct.mem->alloc_sarray)
							(reinterpret_cast<j_common_ptr>(&jpegStruct),
							JPOOL_IMAGE, row_stride, 1);

	//init img file
	pFile->Init(cvSize(jpegStruct.output_width, jpegStruct.output_height));

	AddNewLayerHandle* handle = (AddNewLayerHandle*)pFile->CreateImgFileHandle( IFH_ADD_NEW_LAYER );
	handle->Do( pFile );
	ImgLayer_Ptr new_layer = handle->GetNewLayer().lock();
	pFile->ReleaseImgFileHandle(handle);
	handle = NULL;

	pFile->SetSelectLayer( new_layer );

	//
	UINT y = 0;
	while(jpegStruct.output_scanline < jpegStruct.output_height){
		jpeg_read_scanlines( &jpegStruct, ppBuffer, 1);

		JSAMPROW pBuf = *ppBuffer;
		UCvPixel pixel;
		for(UINT x = 0; x < jpegStruct.output_width; x++){
			if(jpegStruct.out_color_components == 3){
				pixel.r = *pBuf++;
				pixel.g = *pBuf++;
				pixel.b = *pBuf++;
			}
			else{
				pixel.r = pixel.g = pixel.b = *pBuf++;
			}
			pixel.a = 255;
			new_layer->SetPixel(x, y, &pixel);
		}
		y++;
	}

	jpeg_finish_decompress( &jpegStruct );
	jpeg_destroy_decompress( &jpegStruct );
	fclose( fp );

	//
	LPUPDATE_DATA pData = new_layer->CreateUpdateData();
	pData->isAll = true;
	new_layer->PushUpdateData( pData );

	//
	NullHandle* null_handle = (NullHandle*)pFile->CreateImgFileHandle(IFH_NULL);
	strcpy_s(null_handle->handle_name, MAX_IMG_FILE_HANDLE_NAME, "jpegファイル読み込み");
	pFile->DoImgFileHandle( null_handle );

	return true;
}

IEIO_EXPORT bool IEIO_SaveAsImgFile(const char *filePath, ImgFile_Ptr pFile)
{
	//ファイルオブジェクトから出力画像を得る。
	CvSize size = pFile->GetImgSize();
	IplImage* img = cvCreateImage(size, IPL_DEPTH_8U, 4);
	pFile->CopyFileImage(img);

	bool isInterlaced = false;
	::DialogBox((HINSTANCE)g_hModule, MAKEINTRESOURCE(IDD_SAVE_JPEG), NULL, (DLGPROC)SaveJpegDlgProc);

	//ファイルへ出力
	return SaveJpeg(filePath, img, g_quality);
}

bool SaveJpeg(const char *filePath, IplImage *img, int quality)
{
	if(filePath == NULL || img == NULL){
		return false;
	}

	FILE* pFile = fopen(filePath, "wb");
	if(!pFile){
		return false;
	}

	jpeg_compress_struct jpegStruct;
	jpeg_create_compress( &jpegStruct );
	
	JSAMPROW pRow = 0;

	JpegErrorManageStruct structErrorManagement;
	jpegStruct.err = jpeg_std_error( &structErrorManagement.m_filedPublic );
	structErrorManagement.m_filedPublic.error_exit = exitByLongJump;
	if(setjmp(structErrorManagement.m_bufferSetJmp)){
		if(pRow){
			delete[] pRow;
			pRow = NULL;
		}
		jpeg_destroy_compress(&jpegStruct);
		fclose(pFile);

		return false;
	}

	jpeg_stdio_dest(&jpegStruct, pFile);

	jpegStruct.image_width = img->width;
	jpegStruct.image_height = img->height;
	jpegStruct.input_components = 3;
	jpegStruct.in_color_space = JCS_RGB;

	jpeg_set_defaults(&jpegStruct);
	jpeg_set_quality(&jpegStruct, quality, true);

	jpeg_start_compress( &jpegStruct, true );

	UINT y = 0;
	UCvPixel pixel;
	pRow = new JSAMPLE[img->width * 3];
	while(jpegStruct.next_scanline < jpegStruct.image_height){
		JSAMPROW pBuf = pRow;
		for(UINT x = 0; x < img->width; x++){
			int8_t r,g,b;
			GetPixelFromBGRA(img, x, y, &pixel);
			*pBuf++ = pixel.r;
			*pBuf++ = pixel.g;
			*pBuf++ = pixel.b;
		}
		jpeg_write_scanlines(&jpegStruct, &pRow, 1);
		y++;
	}

	jpeg_finish_compress( &jpegStruct );

	if(pRow){
		delete[] pRow;
		pRow = NULL;
	}

	jpeg_destroy_compress( &jpegStruct );
	fclose(pFile);

	return true;
}

BOOL CALLBACK SaveJpegDlgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uiMsg) {
	case WM_INITDIALOG:
		::SendDlgItemMessage(hWnd, IDC_SLIDER1, TBM_SETRANGE, TRUE, MAKELPARAM(0, 100));
		::SendDlgItemMessage(hWnd, IDC_SLIDER1, TBM_SETPOS, TRUE, g_quality);
		return TRUE;

	case WM_COMMAND:
		if(LOWORD(wParam) == IDOK) {
			//OKボタンが押されたときの処理
			g_quality = ::SendDlgItemMessage(hWnd, IDC_SLIDER1, TBM_GETPOS, 0, 0);
			EndDialog(hWnd, IDOK);
			return TRUE;
		}
		break;

	default:
		break;
	}
	return FALSE;
}