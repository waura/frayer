#include <cxxtest/TestSuite.h>

#include "libImgEdit.h"
#include "RLE.h"

class IEIOfydTestSuite : public CxxTest::TestSuite
{
public:
	IEIOfydTestSuite()
	{
		//初期カレントディレクトリを取得し、ImgEditに渡す
		char cdPath[MAX_PATH];
		if(0 == ::GetCurrentDirectory(MAX_PATH, cdPath)){
			//error
			TS_ASSERT(true);
		}
		m_ImgEdit.SetStartDirectory(cdPath);

		//
		char exePath[MAX_PATH];
		wsprintf(exePath, "%s/Test_libImgEdit.exe", cdPath);
		m_ImgEdit.Init(".\\config.ini", exePath, ".\\log");
	}

	~IEIOfydTestSuite()
	{
		//
		m_ImgEdit.End();
	}

	void test_LoadImgFile_fyd()
	{
		ImgFile_Ptr f = m_ImgEdit.CreateNewFile(1000, 1000, &ucvPixel(0,0,0,0));

		EditLayerHandle* handle1 = (EditLayerHandle*)f->CreateImgFileHandle(IFH_EDIT_LAYER);
		f->DoImgFileHandle(handle1);
		EditNode* pNode = handle1->CreateEditNode();

		int x,y;
		for (y = 0; y < 1000; y++) {
			for (x = 0; x < 1000; x++) {
				pNode->edit_img.SetPixel(x, y, &ucvPixel(100, 120, 50, 255.0*((double)y/1000.0)));
			}
		}

		IplImageExt copy_img;
		pNode->edit_img.Copy(&copy_img);
		handle1->Update(&(pNode->node_rect));
		handle1->EndEdit();

		ImgLayer_Ptr pLayer = std::dynamic_pointer_cast<ImgLayer>(f->GetLayerAtIndex(0));
		double diff = GetMaxAbsDiffExt(pLayer->GetLayerImage(), &copy_img);
		TS_ASSERT(diff <= 1.0);

		//
		MoveLayerHandle* handle = (MoveLayerHandle*)f->CreateImgFileHandle(IFH_MOVE_LAYER);
		f->DoImgFileHandle(handle);
		handle->SetMoveXY(100, 100);
		handle->Update();

		//
		RECT updated_rc;
		f->LockUpdateData();
		f->Update(NULL, &updated_rc);
		f->ClearUpdateData();
		f->UnlockUpdateData();

		//copy new image to compare
		pLayer = std::dynamic_pointer_cast<ImgLayer>(f->GetLayerAtIndex(0));
		pLayer->GetLayerImage()->Copy(&copy_img);

		m_ImgEdit.SaveAsImgFile("test_data\\fyd\\tmp.fyd", f);
		ImgFile_Ptr load_f = m_ImgEdit.LoadImgFile("test_data\\fyd\\tmp.tyd");

		pLayer = std::dynamic_pointer_cast<ImgLayer>(f->GetLayerAtIndex(0));
		TS_ASSERT(isEqualIplImageExt(pLayer->GetLayerImage(), &copy_img));
	}

	void test_RLE()
	{
		int width = 256;
		int height = 256;
		IplImage* src = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
		int x,y;
		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				double r = sqrt((double)x*x + y*y);
				double max_r = sqrt((double)width * width + height * height);
				SetMaskDataPos(src, x, y, 255.0 * (r / max_r));
			}
		}

		uint32_t* rle_pack_len = (uint32_t*) malloc(height * sizeof(uint32_t));
		IplImage* encode_img = cvCreateImage(cvSize(2 * width, height), IPL_DEPTH_8U, 1); //RLE buffer size maybe larger than original source buffer

		int i,j;
		for (i = 0; i < height; i++) {
			uint8_t* src_line = GetPixelAddress(src, 0, i);
			uint8_t* encode_line = GetPixelAddress(encode_img, 0, i);
			encode_rle(
				(int8_t*)src_line,
				width,
				(int8_t*)encode_line,
				&(rle_pack_len[i]));

			{//
				uint8_t* decode_dst = (uint8_t*) malloc(width* sizeof(uint8_t));
				decode_rle((int8_t*)encode_line, rle_pack_len[i], width, (int8_t*)decode_dst);
				for (j = 0; j < width; j++) {
					TS_ASSERT(src_line[j] == decode_dst[j]);
				}
				free(decode_dst);
			}
		}

		IplImage* decode_img = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
		for (i = 0; i < height; i++) {
			uint8_t* src_line = GetPixelAddress(encode_img, 0, i);				
			uint8_t* decode_line = GetPixelAddress(decode_img, 0, i);
			decode_rle((int8_t*)src_line, rle_pack_len[i], width, (int8_t*)decode_line);

			//test RLE
			{
				uint32_t encode_len;
				uint8_t* encode_dst = (uint8_t*) malloc(rle_pack_len[i]);
				encode_rle((int8_t*)decode_line, width, (int8_t*)encode_dst, &encode_len);

				TS_ASSERT(encode_len == rle_pack_len[i]);

				for (j = 0; j < encode_len; j++) {
					TS_ASSERT(src_line[j] == encode_dst[j]);
				}
				free(encode_dst);
			}
		}

		TS_ASSERT(isEqualIplImage(src, decode_img));

		cvReleaseImage(&src);
		cvReleaseImage(&encode_img);
		cvReleaseImage(&decode_img);
	}

private:
	ImgEdit m_ImgEdit;
};