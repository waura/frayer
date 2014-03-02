#include <cxxtest/TestSuite.h>

#include "IplExtOperate.h"
#include "IplImageExt.h"

class IplImageExtTestSuite : public CxxTest::TestSuite
{
public:
	IplImageExtTestSuite()
	{
	}

	~IplImageExtTestSuite()
	{
	}

	void test_Init()
	{
		IplImageExt img1;
		img1.Init(300, 400, IPL_DEPTH_8U, 4);
		CvSize size1 = img1.GetSize();
		TS_ASSERT_EQUALS(size1.width, 300);
		TS_ASSERT_EQUALS(size1.height, 400);
		TS_ASSERT_EQUALS(img1.GetImgDepth(), IPL_DEPTH_8U);
		TS_ASSERT_EQUALS(img1.GetImgNChannel(), 4);

		IplImageExt img2;
		img2.InitToEqualSize(&img1, IPL_DEPTH_8U, 1);
		CvSize size2 = img2.GetSize();
		TS_ASSERT_EQUALS(size2.width, 300);
		TS_ASSERT_EQUALS(size2.height, 400);
		TS_ASSERT_EQUALS(img2.GetImgDepth(), IPL_DEPTH_8U);
		TS_ASSERT_EQUALS(img2.GetImgNChannel(), 1);
	}

	void test_ExtendImage()
	{
		IplImageExt img;
		img.Init(300, 400, IPL_DEPTH_8U, 4);
		img.RectFillColor(0, 0, 300, 400, &ucvPixel(120, 130, 140, 150));

		img.ExtendImage(100, 200, TO_LEFT | TO_TOP);

		//check image size
		CvSize size = img.GetSize();
		TS_ASSERT_EQUALS(size.width, 300 + 100);
		TS_ASSERT_EQUALS(size.height, 400 + 200);

		{//check old image pixel
			IplImage* old_img = cvCreateImage(cvSize(300, 400), IPL_DEPTH_8U, 4);
			img.GetRangeIplImage(old_img, 0, 0, 300, 400, 100, 200);
			TS_ASSERT(isFillColor(old_img, &ucvPixel(120,130,140,150)));
			cvReleaseImage(&old_img);
		}

		{//check extend image pixel
			IplImage* extend_img = cvCreateImage(cvSize(100, 200), IPL_DEPTH_8U, 4);
			img.GetRangeIplImage(extend_img, 0, 0, 100, 200, 0, 0);
			TS_ASSERT(isFillColor(extend_img, &ucvPixel(0,0,0,0)));
			cvReleaseImage(&extend_img);
		}
	}

	void test_IsFillZeroMask()
	{
		IplImageExt img;
		img.Init(300, 300, IPL_DEPTH_8U, 1);
		TS_ASSERT(img.IsFillZeroMask() == true);
	}

	void test_RectFillColor()
	{
		IplImageExt img;
		img.Init(300, 400, IPL_DEPTH_8U, 4);

		UCvPixel pix1;
		pix1.r = 100;
		pix1.g = 120;
		pix1.b = 140;
		pix1.a = 255;
		img.RectFillColor(100, 100, 100, 100, &pix1);

		UCvPixel pix2;
		img.GetPixel(100, 100, &pix2);
		TS_ASSERT(pix1.value == pix2.value);
		img.GetPixel(199, 100, &pix2);
		TS_ASSERT(pix1.value == pix2.value);
		img.GetPixel(200, 100, &pix2);
		TS_ASSERT(pix1.value != pix2.value);
		img.GetPixel(199, 199, &pix2);
		TS_ASSERT(pix1.value == pix2.value);
		img.GetPixel(200, 200, &pix2);
		TS_ASSERT(pix1.value != pix2.value);
		img.GetPixel(100, 199, &pix2);
		TS_ASSERT(pix1.value == pix2.value);
		img.GetPixel(100, 200, &pix2);
		TS_ASSERT(pix1.value != pix2.value);

		img.GetPixel(99, 100, &pix2);
		TS_ASSERT(pix1.value != pix2.value);
		img.GetPixel(201, 100, &pix2);
		TS_ASSERT(pix1.value != pix2.value);
		img.GetPixel(100, 201, &pix2);
		TS_ASSERT(pix1.value != pix2.value);
		img.GetPixel(201, 200, &pix2);
		TS_ASSERT(pix1.value != pix2.value);

		IplImage* img2 = cvCreateImage(cvSize(100, 100), IPL_DEPTH_8U, 4);
		img.GetRangeIplImage(img2, 0, 0, 100, 100, 100, 100);
		TS_ASSERT_EQUALS(isFillColor(img2, &pix1), true);
	}

	void test_OperateBlt_IplImageExt_noSrc()
	{
		class BltOp1 {
		public:
			inline void operator()(void* dst, const void* src, const void* mask, int width) const
			{
				assert(dst);
				assert(mask);
				assert(src == NULL);

				uint8_t* pdst = (uint8_t*) dst;
				UCvPixel* pmask = (UCvPixel*) mask;
	
				int i;
				for(i=0; i<width; i++){
					if(pmask->a != 0)
						(*pdst) = pmask->a;
					pdst++;
					pmask++;
				}
			}
		};

		int blt_startX = 200;
		int blt_startY = 300;
		int blt_width = 300;
		int blt_height = 400;

		IplImageExt img;
		img.Init(1000, 1000, IPL_DEPTH_8U, 1);

		IplImageExt mask;
		mask.Init(1000, 1000, IPL_DEPTH_8U, 4);
		mask.RectFillColor(
			blt_startX,
			blt_startY,
			blt_width,
			blt_height,
			&ucvPixel(123,124,125,111));
		
		BltOp1 bltOp1;
		NormMaskOp normMaskOp;
		img.OperateBlt<BltOp1, NormMaskOp>(
			bltOp1,
			normMaskOp,
			blt_startX,
			blt_startY,
			blt_width,
			blt_height,
			NULL,
			0,
			0,
			&mask,
			blt_startX,
			blt_startY);

		SaveIplImageExt("img.bmp", &img);
		SaveIplImageExt("mask.bmp", &mask);

		{//check pixel
			uint8_t correct_pix = 111;
			uint8_t pix;

			img.GetMaskData(blt_startX, blt_startY, &pix);
			TS_ASSERT(pix == correct_pix);
			img.GetMaskData(blt_startX + blt_width - 1, blt_startY, &pix);
			TS_ASSERT(pix == correct_pix);
			img.GetMaskData(blt_startX, blt_startY + blt_height - 1, &pix);
			TS_ASSERT(pix == correct_pix);
			img.GetMaskData(blt_startX + blt_width - 1, blt_startY + blt_height - 1, &pix);
			TS_ASSERT(pix == correct_pix);
			img.GetMaskData(blt_startX - 1, blt_startY, &pix);
			TS_ASSERT(pix != correct_pix);
			img.GetMaskData(blt_startX, blt_startY - 1, &pix);
			TS_ASSERT(pix != correct_pix);
			img.GetMaskData(blt_startX + blt_width, blt_startY + blt_height - 1, &pix);
			TS_ASSERT(pix != correct_pix);
			img.GetMaskData(blt_startX + blt_width - 1, blt_startY + blt_height, &pix);
			TS_ASSERT(pix != correct_pix);

			IplImage* img2 = cvCreateImage(cvSize(blt_width, blt_height), IPL_DEPTH_8U, 1);
			img.GetRangeIplImage(
				img2,
				0,
				0,
				img2->width,
				img2->height,
				blt_startX,
				blt_startY);
			TS_ASSERT_EQUALS(isFillMask(img2, correct_pix), true);
		}
	}

	void test_OperateBlt_IplImageExt_noSrc_After_ExtendImage()
	{
		class BltOp1 {
		public:
			inline void operator()(void* dst, const void* src, const void* mask, int width) const
			{
				assert(dst);
				assert(mask);
				assert(src == NULL);

				uint8_t* pdst = (uint8_t*) dst;
				UCvPixel* pmask = (UCvPixel*) mask;
	
				int i;
				for(i=0; i<width; i++){
					if(pmask->a != 0)
						(*pdst) = pmask->a;
					pdst++;
					pmask++;
				}
			}
		};

		int blt_startX = 200;
		int blt_startY = 300;
		int blt_width = 300;
		int blt_height = 400;

		IplImageExt img;
		img.Init(1000, 1000, IPL_DEPTH_8U, 1);
		img.ExtendImage(100, 200, TO_TOP | TO_LEFT);

		IplImageExt mask;
		mask.Init(1100, 1200, IPL_DEPTH_8U, 4);
		mask.RectFillColor(
			blt_startX,
			blt_startY,
			blt_width,
			blt_height,
			&ucvPixel(123,124,125,111));
		
		BltOp1 bltOp1;
		NormMaskOp normMaskOp;
		img.OperateBlt<BltOp1, NormMaskOp>(
			bltOp1,
			normMaskOp,
			blt_startX,
			blt_startY,
			blt_width,
			blt_height,
			NULL,
			0,
			0,
			&mask,
			blt_startX,
			blt_startY);

		{//check pixel
			uint8_t correct_pix = 111;
			uint8_t pix;

			img.GetMaskData(blt_startX, blt_startY, &pix);
			TS_ASSERT(pix == correct_pix);
			img.GetMaskData(blt_startX + blt_width - 1, blt_startY, &pix);
			TS_ASSERT(pix == correct_pix);
			img.GetMaskData(blt_startX, blt_startY + blt_height - 1, &pix);
			TS_ASSERT(pix == correct_pix);
			img.GetMaskData(blt_startX + blt_width - 1, blt_startY + blt_height - 1, &pix);
			TS_ASSERT(pix == correct_pix);
			img.GetMaskData(blt_startX - 1, blt_startY, &pix);
			TS_ASSERT(pix != correct_pix);
			img.GetMaskData(blt_startX, blt_startY -1, &pix);
			TS_ASSERT(pix != correct_pix);
			img.GetMaskData(blt_startX + blt_width, blt_startY + blt_height - 1, &pix);
			TS_ASSERT(pix != correct_pix);
			img.GetMaskData(blt_startX + blt_width - 1, blt_startY + blt_height, &pix);
			TS_ASSERT(pix != correct_pix);

			IplImage* img2 = cvCreateImage(cvSize(blt_width, blt_height), IPL_DEPTH_8U, 1);
			img.GetRangeIplImage(
				img2,
				0,
				0,
				img2->width,
				img2->height,
				blt_startX,
				blt_startY);
			TS_ASSERT_EQUALS(isFillMask(img2, correct_pix), true);
		}
	}

	void test_OperateEdit_IplImageExt()
	{
		class EditOp1 {
		public:
			void operator()(void* dst, int posX, int posY, int width) const
			{
				UCvPixel* pdst = (UCvPixel*) dst;
				int x;
				for (x = 0; x < width; x++) {
					UCvPixel pix = ucvPixel(120, 130, 140, 150);
					pdst->value = pix.value;
					pdst++;
				}
			}
		};

		IplImageExt img;
		img.Init(1000, 1000, IPL_DEPTH_8U, 4);

		EditOp1 editOp1;
		img.OperateEdit(
			editOp1,
			200,
			300,
			400,
			500);

		UCvPixel pix1 = ucvPixel(120, 130, 140, 150);
		UCvPixel pix2;

		img.GetPixel(200, 300, &pix2);
		TS_ASSERT(pix1.value == pix2.value);
		img.GetPixel(599, 300, &pix2);
		TS_ASSERT(pix1.value == pix2.value);
		img.GetPixel(200, 799, &pix2);
		TS_ASSERT(pix1.value == pix2.value);
		img.GetPixel(599, 799, &pix2);
		TS_ASSERT(pix1.value == pix2.value);
		img.GetPixel(199, 300, &pix2);
		TS_ASSERT(pix1.value != pix2.value);
		img.GetPixel(599, 801, &pix2);
		TS_ASSERT(pix1.value != pix2.value);
		img.GetPixel(200, 800, &pix2);
		TS_ASSERT(pix1.value != pix2.value);
		img.GetPixel(599, 800, &pix2);
		TS_ASSERT(pix1.value != pix2.value);

		IplImage* img2 = cvCreateImage(cvSize(400, 500), IPL_DEPTH_8U, 4);
		img.GetRangeIplImage(img2, 0, 0, 400, 500, 200, 300);
		TS_ASSERT_EQUALS(isFillColor(img2, &pix1), true);

		class EditOp2 {
		public:
			void operator()(void* dst, int posX, int posY, int width) const
			{
				UCvPixel* pdst = (UCvPixel*) dst;
				int x;
				for (x = 0; x < width; x++) {
					UCvPixel pix = ucvPixel(posX + x, posY, 140, 150);
					pdst->value = pix.value;
					pdst++;
				}
			}
		};

		EditOp2 editOp2;
		img.OperateEdit(
			editOp2,
			0,
			0,
			256,
			256);

		int x,y;
		for (y = 0; y < 256; y++) {
			for (x = 0; x < 256; x++) {
				pix1 = ucvPixel(x, y, 140, 150);
				img.GetPixel(x, y, &pix2);

				TS_ASSERT(pix1.value == pix2.value);
			}
		}
	}

	void test_OperateEdit_IplImage()
	{
		class EditOp1 {
		public:
			void operator()(void* dst, int posX, int posY, int width) const
			{
				UCvPixel* pdst = (UCvPixel*) dst;
				int x;
				for (x = 0; x < width; x++) {
					UCvPixel pix = ucvPixel(120, 130, 140, 150);
					pdst->value = pix.value;
					pdst++;
				}
			}
		};

		IplImage* img = cvCreateImage(cvSize(1000, 1000), IPL_DEPTH_8U, 4);

		EditOp1 editOp1;
		IplImageExt::OperateEdit(
			editOp1,
			img,
			200,
			300,
			400,
			500);

		UCvPixel pix1 = ucvPixel(120, 130, 140, 150);
		UCvPixel pix2;

		GetPixelFromBGRA(img, 200, 300, &pix2);
		TS_ASSERT(pix1.value == pix2.value);
		GetPixelFromBGRA(img, 599, 300, &pix2);
		TS_ASSERT(pix1.value == pix2.value);
		GetPixelFromBGRA(img, 200, 799, &pix2);
		TS_ASSERT(pix1.value == pix2.value);
		GetPixelFromBGRA(img, 599, 799, &pix2);
		TS_ASSERT(pix1.value == pix2.value);
		GetPixelFromBGRA(img, 199, 300, &pix2);
		TS_ASSERT(pix1.value != pix2.value);
		GetPixelFromBGRA(img, 599, 801, &pix2);
		TS_ASSERT(pix1.value != pix2.value);
		GetPixelFromBGRA(img, 200, 800, &pix2);
		TS_ASSERT(pix1.value != pix2.value);
		GetPixelFromBGRA(img, 599, 800, &pix2);
		TS_ASSERT(pix1.value != pix2.value);

		//IplImage* img2 = cvCreateImage(cvSize(400, 500), IPL_DEPTH_8U, 4);
		//img.GetRangeIplImage(img2, 0, 0, 400, 500, 200, 300);
		//TS_ASSERT_EQUALS(isFillColor(img2, &pix1), true);

		class EditOp2 {
		public:
			void operator()(void* dst, int posX, int posY, int width) const
			{
				UCvPixel* pdst = (UCvPixel*) dst;
				int x;
				for (x = 0; x < width; x++) {
					UCvPixel pix = ucvPixel(posX + x, posY, 140, 150);
					pdst->value = pix.value;
					pdst++;
				}
			}
		};

		EditOp2 editOp2;
		IplImageExt::OperateEdit(
			editOp2,
			img,
			0,
			0,
			256,
			256);

		int x,y;
		for (y = 0; y < 256; y++) {
			for (x = 0; x < 256; x++) {
				pix1 = ucvPixel(x, y, 140, 150);
				GetPixelFromBGRA(img, x, y, &pix2);

				TS_ASSERT(pix1.value == pix2.value);
			}
		}
	}

};