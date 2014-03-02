#include <cxxtest/TestSuite.h>

#include "UtilOpenCV.h"

class UtilOpenCVTestSuite : public CxxTest::TestSuite
{
public:
	UtilOpenCVTestSuite()
	{
	}

	~UtilOpenCVTestSuite()
	{
	}

	void test_isEqualIplImage()
	{
		{//1ch
			IplImage* img1 = cvCreateImage(cvSize(100, 100), IPL_DEPTH_8U, 1);
			IplImage* img2 = cvCreateImage(cvSize(100, 100), IPL_DEPTH_8U, 1);

			cvSet(img1, cvScalar(100));
			cvSet(img2, cvScalar(100));
			TS_ASSERT(isEqualIplImage(img1, img2) == true);

			cvSet(img2, cvScalar(10));
			TS_ASSERT(isEqualIplImage(img1, img2) == false);

			cvReleaseImage(&img1);
			cvReleaseImage(&img2);
		}
		{//4ch
			IplImage* img1 = cvCreateImage(cvSize(100, 100), IPL_DEPTH_8U, 4);
			IplImage* img2 = cvCreateImage(cvSize(100, 100), IPL_DEPTH_8U, 4);

			cvSet(img1, cvScalar(100, 120, 130, 255));
			cvSet(img2, cvScalar(100, 120, 130, 255));
			TS_ASSERT(isEqualIplImage(img1, img2) == true);

			cvSet(img2, cvScalar(100, 120, 130, 254));
			TS_ASSERT(isEqualIplImage(img1, img2) == false);

			cvReleaseImage(&img1);
			cvReleaseImage(&img2);
		}

	}

	void test_isFillZeroMask()
	{
		IplImage* img = cvCreateImage(cvSize(100, 100), IPL_DEPTH_8U, 1);

		cvSet(img, cvScalar(0));
		TS_ASSERT(isFillZeroMask(img) == true);

		cvSet(img, cvScalar(111));
		TS_ASSERT(isFillZeroMask(img) == false);

		cvReleaseImage(&img);
	}

	void test_isNotFillZeroMask()
	{
		IplImage* img = cvCreateImage(cvSize(100, 100), IPL_DEPTH_8U, 1);

		cvSet(img, cvScalar(0));
		TS_ASSERT(isNotFillZeroMask(img) == false);

		cvSet(img, cvScalar(111));
		TS_ASSERT(isNotFillZeroMask(img) == true);

		cvReleaseImage(&img);
	}

	void test_isFillColor()
	{
		IplImage* img = cvCreateImage(cvSize(100, 100), IPL_DEPTH_8U, 4);

		UCvPixel pix;
		pix.b = 100;
		pix.g = 120;
		pix.r = 130;
		pix.a = 255;

		cvSet(img, cvScalar(100, 120, 130, 255));
		TS_ASSERT(isFillColor(img, &pix) == true);

		cvSet(img, cvScalar(101, 120, 130, 255));
		TS_ASSERT(isFillColor(img, &pix) == false);
	}

	void test_isInRect()
	{
		RECT rc;
		rc.left = 100;
		rc.top = 200;
		rc.right = 300;
		rc.bottom = 450;

		TS_ASSERT(isInRect(100, 200, &rc) == true);
		TS_ASSERT(isInRect(100, 450, &rc) == false);
		TS_ASSERT(isInRect(300, 450, &rc) == false);
		TS_ASSERT(isInRect(300, 200, &rc) == false);

		TS_ASSERT(isInRect(200, 300, &rc) == true);
		TS_ASSERT(isInRect(0, 300, &rc) == false);
		TS_ASSERT(isInRect(200, -300, &rc) == false);
	}
};
