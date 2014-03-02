#pragma once
#include "libImgEdit.h"

#include <iostream>
#include <fstream>
#include <iomanip>

double MeasureBlendSynth(
	uint32_t fourcc,
	IplImageExt* dst_img,
	IplImageExt* under_img,
	IplImageExt* over_img)
{
	CvSize img_size = dst_img->GetSize();
	double time = 0.0;
	for (int i = 0; i < 3; i++) {
		IETimer timer;
		LayerSynthesizerMG::BlendSynth(
			fourcc,
			1.0,
			dst_img,
			0, 0,
			img_size.width, img_size.height,
			under_img,
			0, 0,
			over_img,
			0, 0);

		LayerSynthesizerMG::BlendSynth(
			fourcc,
			1.0,
			dst_img,
			2, 0,
			img_size.width - 2, img_size.height,
			under_img,
			2, 0,
			over_img,
			2, 0);

		LayerSynthesizerMG::BlendSynth(
			fourcc,
			1.0,
			dst_img,
			1, 0,
			img_size.width - 2, img_size.height,
			under_img,
			2, 0,
			over_img,
			2, 0);
		time += timer.elapsed();
	}
	return (time / 3.0);
}

bool LayerSynthesizer_BenchMark(std::ofstream& ofs)
{
	int x, y;
	IplImage* test1 = cvLoadImage("test_data\\test1.bmp");
	IplImage* test2 = cvLoadImage("test_data\\test2.bmp");
	if ((test1->width != test2->width) || (test1->height != test2->height)) {
		std::cerr << "invalid img size in LayerSynthesizer_BenchMark" << std::endl;
		return false;
	}
	ofs << "use " << test1->width << "x" << test1->height << "image" << std::endl;
	IplImage* a_ch1 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);
	IplImage* a_ch2 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);
	IplImage* a_ch3 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);
	IplImage* b_ch = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);
	IplImage* g_ch = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);
	IplImage* r_ch = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);

	IplImageExt dst_img, over_img, under_img;
	dst_img.Init(test1->width, test1->height, IPL_DEPTH_8U, 4);
	under_img.Init(test2->width, test2->height, IPL_DEPTH_8U, 4);
	over_img.Init(test1->width, test1->height, IPL_DEPTH_8U, 4);

	//
	uint8_t* line1 = GetPixelAddress(a_ch1, 0, 0);
	for (y = 0; y < a_ch1->height; y++) {
		uint8_t* pix = line1;
		for (x = 0; x < a_ch1->width; x++) {
			(*pix++) = ((double)x / a_ch1->width) * 255;
		}
		line1 = GetNextLineAddress(a_ch1, line1);
	}

	//
	uint8_t* line2 = GetPixelAddress(a_ch2, 0, 0);
	for (y = 0; y < a_ch2->height; y++) {
		uint8_t* pix = line2;
		for (x = 0; x < a_ch2->width; x++) {
			(*pix++) = ((double)y / a_ch2->height) * 255;
		}
		line2 = GetNextLineAddress(a_ch2, line2);
	}

	typedef struct _TestFourcc {
		uint32_t fourcc;
		char name[10];
	} TestForcc;

	TestForcc testFourcc[] = {
		{IE_LAYER_SYNTH_FOURCC('n','o','r','m'), "norm"},
		{IE_LAYER_SYNTH_FOURCC('m','u','l',' '), "mul "},
		{IE_LAYER_SYNTH_FOURCC('b','u','r','n'), "burn"},
		{IE_LAYER_SYNTH_FOURCC('d','a','r','k'), "dark"},
		{IE_LAYER_SYNTH_FOURCC('d','i','f','f'), "diff"},
		{IE_LAYER_SYNTH_FOURCC('d','o','d','g'), "dodg"},
		{IE_LAYER_SYNTH_FOURCC('h','L','i','t'), "hLit"},
		{IE_LAYER_SYNTH_FOURCC('l','i','t','e'), "lite"},
		{IE_LAYER_SYNTH_FOURCC('o','v','e','r'), "over"},
		{IE_LAYER_SYNTH_FOURCC('s','c','r','n'), "scrn"},
		{IE_LAYER_SYNTH_FOURCC('s','L','i','t'), "sLit"},
	};

	int nloop = sizeof(testFourcc) / sizeof(TestForcc);
	for (int i = 0; i < nloop; i++) {
		double time = 0.0;
		{
			cvSplit(test1, b_ch, g_ch, r_ch, NULL);
			over_img.Merge(
				0, 0,
				test1->width, test1->height,
				b_ch, g_ch, r_ch, a_ch1);

			cvSplit(test2, b_ch, g_ch, r_ch, NULL);
			under_img.Merge(
				0, 0,
				test2->width, test2->height,
				b_ch, g_ch, r_ch, a_ch2);

			time += MeasureBlendSynth(
				testFourcc[i].fourcc,
				&dst_img,
				&over_img,
				&under_img);
		}

		{
			cvSplit(test1, b_ch, g_ch, r_ch, NULL);
			over_img.Merge(
				0, 0,
				test1->width, test1->height,
				b_ch, g_ch, r_ch, a_ch1);

			cvSplit(test2, b_ch, g_ch, r_ch, NULL);
			under_img.Merge(
				0, 0,
				test2->width, test2->height,
				b_ch, g_ch, r_ch, a_ch3);

			time += MeasureBlendSynth(
				testFourcc[i].fourcc,
				&dst_img,
				&over_img,
				&under_img);
		}

		{
			cvSplit(test1, b_ch, g_ch, r_ch, NULL);
			over_img.Merge(
				0, 0,
				test1->width, test1->height,
				b_ch, g_ch, r_ch, a_ch3);

			cvSplit(test2, b_ch, g_ch, r_ch, NULL);
			under_img.Merge(
				0, 0,
				test2->width, test2->height,
				b_ch, g_ch, r_ch, a_ch3);

			time += MeasureBlendSynth(
				testFourcc[i].fourcc,
				&dst_img,
				&over_img,
				&under_img);
		}

		ofs << "BlendSynth " << testFourcc[i].name << " = " << time << " msec" << std::endl;
	}

	cvReleaseImage(&b_ch);
	cvReleaseImage(&g_ch);
	cvReleaseImage(&r_ch);
	cvReleaseImage(&a_ch1);
	cvReleaseImage(&a_ch2);
	cvReleaseImage(&a_ch3);
	cvReleaseImage(&test1);
	cvReleaseImage(&test2);

	return true;
}