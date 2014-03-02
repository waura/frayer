#include <cxxtest/TestSuite.h>

#include "libImgEdit.h"

class LayerSynthesizerTestSuite : public CxxTest::TestSuite
{
public:
	LayerSynthesizerTestSuite()
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

	~LayerSynthesizerTestSuite()
	{
		m_ImgEdit.End();
	}

	void test_BlendSynth_position()
	{
		UCvPixel fillColor = ucvPixel(123, 145, 167, 189);
		{//don't use mask
			int param[3][8] = {
				//{ddif_x, ddif_y, sdif_x, sdif_y, fill_start_x, fill_start_y, fill_width, fill_height}
				{100, 300, 0, 0, 200, 100, 300, 400}, //dst image move
				{0, 0, 100, 300, 300, 400, 300, 400}, //over and under image move
				{200, 400, 100, 300, 300, 400, 100, 200}, //dst, over and under image move
			};
			int i;
			for (i = 0; i < 3; i++) {
				CvSize img_size = cvSize(1000, 1000);
				IplImageExt dst_img, over_img, under_img;
				dst_img.Init(img_size.width, img_size.height, IPL_DEPTH_8U, 4);
				over_img.Init(img_size.width, img_size.height, IPL_DEPTH_8U, 4);
				under_img.Init(img_size.width, img_size.height, IPL_DEPTH_8U, 4);

				int fill_start_x = param[i][4];
				int fill_start_y = param[i][5];
				int fill_width = param[i][6];
				int fill_height = param[i][7];
				over_img.RectFillColor(
					fill_start_x,
					fill_start_y,
					fill_width,
					fill_height,
					&fillColor);

				int ddif_x = param[i][0];
				int ddif_y = param[i][1];
				int sdif_x = param[i][2];
				int sdif_y = param[i][3];
				LayerSynthesizerMG::BlendSynth(
					IE_LAYER_SYNTH_FOURCC('n','o','r','m'),
					1.0,
					&dst_img,
					ddif_x, ddif_y,
					img_size.width, img_size.height,
					&under_img,
					sdif_x, sdif_y,
					&over_img,
					sdif_x, sdif_y);

				{//check pixel
					UCvPixel dst_pix;
					dst_img.GetPixel(
						fill_start_x + (ddif_x - sdif_x) - 1,
						fill_start_y + (ddif_y - sdif_y),
						&dst_pix);
					TS_ASSERT(dst_pix.value != fillColor.value);
					dst_img.GetPixel(
						fill_start_x + (ddif_x - sdif_x),
						fill_start_y + (ddif_y - sdif_y) - 1,
						&dst_pix);
					TS_ASSERT(dst_pix.value != fillColor.value);
					dst_img.GetPixel(
						fill_start_x + (ddif_x - sdif_x) + fill_width,
						fill_start_y + (ddif_y - sdif_y) + fill_height - 1,
						&dst_pix);
					TS_ASSERT(dst_pix.value != fillColor.value);
					dst_img.GetPixel(
						fill_start_x + (ddif_x - sdif_x) + fill_width - 1,
						fill_start_y + (ddif_y - sdif_y) + fill_height,
						&dst_pix);
					TS_ASSERT(dst_pix.value != fillColor.value);

					IplImage* blended_img = cvCreateImage(
						cvSize(fill_width, fill_height),
						IPL_DEPTH_8U,
						4);
					dst_img.GetRangeIplImage(
						blended_img,
						0,
						0,
						blended_img->width,
						blended_img->height,
						fill_start_x + (ddif_x - sdif_x),
						fill_start_y + (ddif_y - sdif_y));

					TS_ASSERT(isFillColor(blended_img, &fillColor));

					cvReleaseImage(&blended_img);
				}
			}
		}

		{//use mask IplImage
			CvSize img_size = cvSize(1000, 1000);
			IplImageExt dst_img, over_img, under_img;
			IplImage* mask_img;
			dst_img.Init(img_size.width, img_size.height, IPL_DEPTH_8U, 4);
			over_img.Init(img_size.width, img_size.height, IPL_DEPTH_8U, 4);
			under_img.Init(img_size.width, img_size.height, IPL_DEPTH_8U, 4);
			mask_img = cvCreateImage(img_size, IPL_DEPTH_8U, 1);

			FillMask(mask_img, 300, 400, 100, 200, 255);
			over_img.RectFillColor(0, 0, img_size.width, img_size.height, &fillColor);

			int dif_x = 100;
			int dif_y = 300;
			LayerSynthesizerMG::BlendSynth(
				IE_LAYER_SYNTH_FOURCC('n','o','r','m'),
				1.0,
				&dst_img,
				dif_x, dif_y,
				img_size.width, img_size.height,
				&under_img,
				0, 0,
				&over_img,
				0, 0,
				mask_img,
				0, 0);

			{//check pixel 1
				IplImage* blended_img = cvCreateImage(
					cvSize(img_size.width - dif_x, img_size.height - dif_y),
					IPL_DEPTH_8U,
					4);
				dst_img.GetRangeIplImage(
					blended_img,
					0,
					0,
					blended_img->width,
					blended_img->height,
					dif_x,
					dif_y);

				TS_ASSERT(isFillColor(blended_img, &fillColor) == false);

				cvReleaseImage(&blended_img);
			}
			{//check pixel 2
				IplImage* blended_img = cvCreateImage(
					cvSize(100, 200),
					IPL_DEPTH_8U,
					4);
				IplImage* correct_img = cvCreateImage(
					cvSize(100, 200),
					IPL_DEPTH_8U,
					4);
				dst_img.GetRangeIplImage(
					blended_img,
					0,
					0,
					blended_img->width,
					blended_img->height,
					dif_x + 300,
					dif_y + 400);
				cvSet(correct_img, cvScalar(fillColor.b, fillColor.g, fillColor.r, fillColor.a));

				double diff = GetMaxAbsDiff(blended_img, correct_img);
				TS_ASSERT(diff <= 1.0);

				cvReleaseImage(&blended_img);
			}
		}

		{//use mask IplImageExt
			CvSize img_size = cvSize(1000, 1000);
			IplImageExt dst_img, over_img, under_img, mask_img;
			dst_img.Init(img_size.width, img_size.height, IPL_DEPTH_8U, 4);
			over_img.Init(img_size.width, img_size.height, IPL_DEPTH_8U, 4);
			under_img.Init(img_size.width, img_size.height, IPL_DEPTH_8U, 4);
			mask_img.Init(img_size.width, img_size.height, IPL_DEPTH_8U, 1);

			mask_img.RectFillMask(300, 400, 100, 200, 255);
			over_img.RectFillColor(0, 0, img_size.width, img_size.height, &fillColor);

			int dif_x = 100;
			int dif_y = 300;
			LayerSynthesizerMG::BlendSynth(
				IE_LAYER_SYNTH_FOURCC('n','o','r','m'),
				1.0,
				&dst_img,
				dif_x, dif_y,
				img_size.width, img_size.height,
				&under_img,
				0, 0,
				&over_img,
				0, 0,
				&mask_img,
				0, 0);

			{//check pixel 1
				IplImage* blended_img = cvCreateImage(
					cvSize(img_size.width - dif_x, img_size.height - dif_y),
					IPL_DEPTH_8U,
					4);
				dst_img.GetRangeIplImage(
					blended_img,
					0,
					0,
					blended_img->width,
					blended_img->height,
					dif_x,
					dif_y);

				TS_ASSERT(isFillColor(blended_img, &fillColor) == false);

				cvReleaseImage(&blended_img);
			}
			{//check pixel 2
				IplImage* blended_img = cvCreateImage(
					cvSize(100, 200),
					IPL_DEPTH_8U,
					4);
				IplImage* correct_img = cvCreateImage(
					cvSize(100, 200),
					IPL_DEPTH_8U,
					4);
				dst_img.GetRangeIplImage(
					blended_img,
					0,
					0,
					blended_img->width,
					blended_img->height,
					dif_x + 300,
					dif_y + 400);
				cvSet(correct_img, cvScalar(fillColor.b, fillColor.g, fillColor.r, fillColor.a));

				double diff = GetMaxAbsDiff(blended_img, correct_img);
				TS_ASSERT(diff <= 1.0);

				cvReleaseImage(&blended_img);
			}
		}
	}

	void test_BlendSynth_forcc()
	{
		IplImageExt dst_img, over_img, under_img, correct_img;
		if (!LoadIplImageExt("test_data\\layer_synth\\over.png", &over_img)) {
			fprintf(stderr, "can't load test_data\\layer_synth\\over.png\n");
			return;
		}
		if (!LoadIplImageExt("test_data\\layer_synth\\under.png", &under_img)) {
			fprintf(stderr, "can't load test_data\\layer_synth\\under.png\n");
			return;
		}

		CvSize over_size = over_img.GetSize();
		CvSize under_size = under_img.GetSize();
		if ((over_size.width != under_size.width) || (over_size.height != under_size.height)) {
			fprintf(stderr, "invalid size image in test_BlendSynth_forcc()\n");
			return;
		}
		CvSize img_size = over_size;
		dst_img.Init(
			img_size.width, img_size.height,
			over_img.GetImgDepth(),
			over_img.GetImgNChannel());

		//normal
		{
			LayerSynthesizerMG::BlendSynth(
				IE_LAYER_SYNTH_FOURCC('n','o','r','m'),
				1.0,
				&dst_img,
				0, 0,
				img_size.width, img_size.height,
				&under_img,
				0, 0,
				&over_img,
				0, 0);
			TS_ASSERT_EQUALS(isEqualIplImageExt(&dst_img, &over_img), true);
		}

		//mul
		{
			if (!LoadIplImageExt("test_data\\layer_synth\\mul.png", &correct_img)) {
				fprintf(stderr,"cant't load test_data\\layer_synth\\mul.png\n"); 
				return;
			}
			LayerSynthesizerMG::BlendSynth(
				IE_LAYER_SYNTH_FOURCC('m','u','l',' '),
				1.0,
				&dst_img,
				0, 0,
				img_size.width, img_size.height,
				&under_img,
				0, 0,
				&over_img,
				0, 0);
			//SaveIplImageExt("test_data\\layer_synth\\mul_dst.bmp", &dst_img);
			double diff = GetMaxAbsDiffExt(&dst_img, &correct_img);
			TS_ASSERT(diff < 4.0);
		}

		//burn
		{
			if (!LoadIplImageExt("test_data\\layer_synth\\burn.png", &correct_img)) {
				fprintf(stderr,"cant't load test_data\\layer_synth\\burn.png\n"); 
				return;
			}
			LayerSynthesizerMG::BlendSynth(
				IE_LAYER_SYNTH_FOURCC('b','u','r','n'),
				1.0,
				&dst_img,
				0, 0,
				img_size.width, img_size.height,
				&under_img,
				0, 0,
				&over_img,
				0, 0);
			//SaveIplImageExt("test_data\\layer_synth\\burn_dst.png", &dst_img);
			double diff = GetMaxAbsDiffExt(&dst_img, &correct_img);
			TS_ASSERT(diff < 1.0);
		}

		//darken
		{
			if (!LoadIplImageExt("test_data\\layer_synth\\darken.png", &correct_img)) {
				fprintf(stderr,"cant't load test_data\\layer_synth\\darken.png\n"); 
				return;
			}
			LayerSynthesizerMG::BlendSynth(
				IE_LAYER_SYNTH_FOURCC('d','a','r','k'),
				1.0,
				&dst_img,
				0, 0,
				img_size.width, img_size.height,
				&under_img,
				0, 0,
				&over_img,
				0, 0);
			//SaveIplImageExt("test_data\\layer_synth\\darken_dst.png", &dst_img);
			double diff = GetMaxAbsDiffExt(&dst_img, &correct_img);
			TS_ASSERT(diff < 1.0);
		}

		//diff
		{
			if (!LoadIplImageExt("test_data\\layer_synth\\diff.png", &correct_img)) {
				fprintf(stderr,"cant't load test_data\\layer_synth\\diff.png\n"); 
				return;
			}
			LayerSynthesizerMG::BlendSynth(
				IE_LAYER_SYNTH_FOURCC('d','i','f','f'),
				1.0,
				&dst_img,
				0, 0,
				img_size.width, img_size.height,
				&under_img,
				0, 0,
				&over_img,
				0, 0);
			//SaveIplImageExt("test_data\\layer_synth\\diff_dst.png", &dst_img);
			double diff = GetMaxAbsDiffExt(&dst_img, &correct_img);
			TS_ASSERT(diff < 1.0);
		}

		//dodge
		{
			if (!LoadIplImageExt("test_data\\layer_synth\\dodg.png", &correct_img)) {
				fprintf(stderr,"cant't load test_data\\layer_synth\\dodg.png\n"); 
				return;
			}
			LayerSynthesizerMG::BlendSynth(
				IE_LAYER_SYNTH_FOURCC('d','o','d','g'),
				1.0,
				&dst_img,
				0, 0,
				img_size.width, img_size.height,
				&under_img,
				0, 0,
				&over_img,
				0, 0);
			//SaveIplImageExt("test_data\\layer_synth\\dodg_dst.png", &dst_img);
			double diff = GetMaxAbsDiffExt(&dst_img, &correct_img);
			TS_ASSERT(diff < 1.0);
		}

		//hardlight
		{
			if (!LoadIplImageExt("test_data\\layer_synth\\hardlight.png", &correct_img)) {
				fprintf(stderr,"cant't load test_data\\layer_synth\\hardlight.png\n"); 
				return;
			}
			LayerSynthesizerMG::BlendSynth(
				IE_LAYER_SYNTH_FOURCC('h','L','i','t'),
				1.0,
				&dst_img,
				0, 0,
				img_size.width, img_size.height,
				&under_img,
				0, 0,
				&over_img,
				0, 0);
			//SaveIplImageExt("test_data\\layer_synth\\hardlight_dst.png", &dst_img);
			double diff = GetMaxAbsDiffExt(&dst_img, &correct_img);
			TS_ASSERT(diff < 1.0);
		}

		//lighten
		{
			if (!LoadIplImageExt("test_data\\layer_synth\\lighten.png", &correct_img)) {
				fprintf(stderr,"cant't load test_data\\layer_synth\\lighten.png\n"); 
				return;
			}
			LayerSynthesizerMG::BlendSynth(
				IE_LAYER_SYNTH_FOURCC('l','i','t','e'),
				1.0,
				&dst_img,
				0, 0,
				img_size.width, img_size.height,
				&under_img,
				0, 0,
				&over_img,
				0, 0);
			//SaveIplImageExt("test_data\\layer_synth\\ighten_dst.png", &dst_img);
			double diff = GetMaxAbsDiffExt(&dst_img, &correct_img);
			TS_ASSERT(diff < 1.0);
		}

		//overlay
		{
			if (!LoadIplImageExt("test_data\\layer_synth\\overlay.png", &correct_img)) {
				fprintf(stderr,"cant't load test_data\\layer_synth\\overlay.png\n"); 
				return;
			}
			LayerSynthesizerMG::BlendSynth(
				IE_LAYER_SYNTH_FOURCC('o','v','e','r'),
				1.0,
				&dst_img,
				0, 0,
				img_size.width, img_size.height,
				&under_img,
				0, 0,
				&over_img,
				0, 0);
			SaveIplImageExt("test_data\\layer_synth\\overlay_dst.png", &dst_img);
			double diff = GetMaxAbsDiffExt(&dst_img, &correct_img);
			TS_ASSERT(diff < 1.0);
		}

		//screen
		{
			if (!LoadIplImageExt("test_data\\layer_synth\\screen.png", &correct_img)) {
				fprintf(stderr,"cant't load test_data\\layer_synth\\screen.png\n"); 
				return;
			}
			LayerSynthesizerMG::BlendSynth(
				IE_LAYER_SYNTH_FOURCC('s','c','r','n'),
				1.0,
				&dst_img,
				0, 0,
				img_size.width, img_size.height,
				&under_img,
				0, 0,
				&over_img,
				0, 0);
			//SaveIplImageExt("test_data\\layer_synth\\screen_dst.png", &dst_img);
			double diff = GetMaxAbsDiffExt(&dst_img, &correct_img);
			TS_ASSERT(diff < 4.0);
		}

		//softlight
		{
			if (!LoadIplImageExt("test_data\\layer_synth\\softlight.png", &correct_img)) {
				fprintf(stderr,"cant't load test_data\\layer_synth\\softlight.png\n"); 
				return;
			}
			LayerSynthesizerMG::BlendSynth(
				IE_LAYER_SYNTH_FOURCC('s','L','i','t'),
				1.0,
				&dst_img,
				0, 0,
				img_size.width, img_size.height,
				&under_img,
				0, 0,
				&over_img,
				0, 0);
			//SaveIplImageExt("test_data\\layer_synth\\softlight_dst.png", &dst_img);
			double diff = GetMaxAbsDiffExt(&dst_img, &correct_img);
			TS_ASSERT(diff < 1.0);
		}
	}

	void test_BltSynth_forcc()
	{
		IplImageExt over_img, under_img, correct_img;
		if (!LoadIplImageExt("test_data\\layer_synth\\over.png", &over_img)) {
			fprintf(stderr, "can't load test_data\\layer_synth\\over.png\n");
			return;
		}
		if (!LoadIplImageExt("test_data\\layer_synth\\under.png", &under_img)) {
			fprintf(stderr, "can't load test_data\\layer_synth\\under.png\n");
			return;
		}

		CvSize over_size = over_img.GetSize();
		CvSize under_size = under_img.GetSize();
		if ((over_size.width != under_size.width) || (over_size.height != under_size.height)) {
			fprintf(stderr, "invalid size image in test_BlendSynth_forcc()\n");
			return;
		}
		CvSize img_size = over_size;

		//normal
		{
			if (!LoadIplImageExt("test_data\\layer_synth\\under.png", &under_img)) {
				fprintf(stderr, "can't load test_data\\layer_synth\\under.png\n");
				return;
			}
			LayerSynthesizerMG::BltSynth(
				IE_LAYER_SYNTH_FOURCC('n','o','r','m'),
				1.0,
				&under_img,
				0, 0,
				img_size.width, img_size.height,
				&over_img,
				0, 0);
			TS_ASSERT_EQUALS(isEqualIplImageExt(&under_img, &over_img), true);
		}

		//mul
		{
			if (!LoadIplImageExt("test_data\\layer_synth\\under.png", &under_img)) {
				fprintf(stderr, "can't load test_data\\layer_synth\\under.png\n");
				return;
			}
			if (!LoadIplImageExt("test_data\\layer_synth\\mul.png", &correct_img)) {
				fprintf(stderr,"cant't load test_data\\layer_synth\\mul.png\n"); 
				return;
			}
			LayerSynthesizerMG::BltSynth(
				IE_LAYER_SYNTH_FOURCC('m','u','l',' '),
				1.0,
				&under_img,
				0, 0,
				img_size.width, img_size.height,
				&over_img,
				0, 0);
			//SaveIplImageExt("test_data\\layer_synth\\mul_dst.bmp", &dst_img);
			double diff = GetMaxAbsDiffExt(&under_img, &correct_img);
			TS_ASSERT(diff < 4.0);
		}

		//burn
		{
			if (!LoadIplImageExt("test_data\\layer_synth\\under.png", &under_img)) {
				fprintf(stderr, "can't load test_data\\layer_synth\\under.png\n");
				return;
			}
			if (!LoadIplImageExt("test_data\\layer_synth\\burn.png", &correct_img)) {
				fprintf(stderr,"cant't load test_data\\layer_synth\\burn.png\n"); 
				return;
			}
			LayerSynthesizerMG::BltSynth(
				IE_LAYER_SYNTH_FOURCC('b','u','r','n'),
				1.0,
				&under_img,
				0, 0,
				img_size.width, img_size.height,
				&over_img,
				0, 0);
			//SaveIplImageExt("test_data\\layer_synth\\burn_dst.png", &dst_img);
			double diff = GetMaxAbsDiffExt(&under_img, &correct_img);
			TS_ASSERT(diff < 1.0);
		}

		//darken
		{
			if (!LoadIplImageExt("test_data\\layer_synth\\under.png", &under_img)) {
				fprintf(stderr, "can't load test_data\\layer_synth\\under.png\n");
				return;
			}
			if (!LoadIplImageExt("test_data\\layer_synth\\darken.png", &correct_img)) {
				fprintf(stderr,"cant't load test_data\\layer_synth\\darken.png\n"); 
				return;
			}
			LayerSynthesizerMG::BltSynth(
				IE_LAYER_SYNTH_FOURCC('d','a','r','k'),
				1.0,
				&under_img,
				0, 0,
				img_size.width, img_size.height,
				&over_img,
				0, 0);
			//SaveIplImageExt("test_data\\layer_synth\\darken_dst.png", &dst_img);
			double diff = GetMaxAbsDiffExt(&under_img, &correct_img);
			TS_ASSERT(diff < 1.0);
		}

		//diff
		{
			if (!LoadIplImageExt("test_data\\layer_synth\\under.png", &under_img)) {
				fprintf(stderr, "can't load test_data\\layer_synth\\under.png\n");
				return;
			}
			if (!LoadIplImageExt("test_data\\layer_synth\\diff.png", &correct_img)) {
				fprintf(stderr,"cant't load test_data\\layer_synth\\diff.png\n"); 
				return;
			}
			LayerSynthesizerMG::BltSynth(
				IE_LAYER_SYNTH_FOURCC('d','i','f','f'),
				1.0,
				&under_img,
				0, 0,
				img_size.width, img_size.height,
				&over_img,
				0, 0);
			//SaveIplImageExt("test_data\\layer_synth\\diff_dst.png", &dst_img);
			double diff = GetMaxAbsDiffExt(&under_img, &correct_img);
			TS_ASSERT(diff < 1.0);
		}

		//dodge
		{
			if (!LoadIplImageExt("test_data\\layer_synth\\under.png", &under_img)) {
				fprintf(stderr, "can't load test_data\\layer_synth\\under.png\n");
				return;
			}
			if (!LoadIplImageExt("test_data\\layer_synth\\dodg.png", &correct_img)) {
				fprintf(stderr,"cant't load test_data\\layer_synth\\dodg.png\n"); 
				return;
			}
			LayerSynthesizerMG::BltSynth(
				IE_LAYER_SYNTH_FOURCC('d','o','d','g'),
				1.0,
				&under_img,
				0, 0,
				img_size.width, img_size.height,
				&over_img,
				0, 0);
			//SaveIplImageExt("test_data\\layer_synth\\dodg_dst.png", &dst_img);
			double diff = GetMaxAbsDiffExt(&under_img, &correct_img);
			TS_ASSERT(diff < 1.0);
		}

		//hardlight
		{
			if (!LoadIplImageExt("test_data\\layer_synth\\under.png", &under_img)) {
				fprintf(stderr, "can't load test_data\\layer_synth\\under.png\n");
				return;
			}
			if (!LoadIplImageExt("test_data\\layer_synth\\hardlight.png", &correct_img)) {
				fprintf(stderr,"cant't load test_data\\layer_synth\\hardlight.png\n"); 
				return;
			}
			LayerSynthesizerMG::BltSynth(
				IE_LAYER_SYNTH_FOURCC('h','L','i','t'),
				1.0,
				&under_img,
				0, 0,
				img_size.width, img_size.height,
				&over_img,
				0, 0);
			//SaveIplImageExt("test_data\\layer_synth\\hardlight_dst.png", &dst_img);
			double diff = GetMaxAbsDiffExt(&under_img, &correct_img);
			TS_ASSERT(diff < 1.0);
		}

		//lighten
		{
			if (!LoadIplImageExt("test_data\\layer_synth\\under.png", &under_img)) {
				fprintf(stderr, "can't load test_data\\layer_synth\\under.png\n");
				return;
			}
			if (!LoadIplImageExt("test_data\\layer_synth\\lighten.png", &correct_img)) {
				fprintf(stderr,"cant't load test_data\\layer_synth\\lighten.png\n"); 
				return;
			}
			LayerSynthesizerMG::BltSynth(
				IE_LAYER_SYNTH_FOURCC('l','i','t','e'),
				1.0,
				&under_img,
				0, 0,
				img_size.width, img_size.height,
				&over_img,
				0, 0);
			//SaveIplImageExt("test_data\\layer_synth\\ighten_dst.png", &dst_img);
			double diff = GetMaxAbsDiffExt(&under_img, &correct_img);
			TS_ASSERT(diff < 1.0);
		}

		//overlay
		{
			if (!LoadIplImageExt("test_data\\layer_synth\\under.png", &under_img)) {
				fprintf(stderr, "can't load test_data\\layer_synth\\under.png\n");
				return;
			}
			if (!LoadIplImageExt("test_data\\layer_synth\\overlay.png", &correct_img)) {
				fprintf(stderr,"cant't load test_data\\layer_synth\\overlay.png\n"); 
				return;
			}
			LayerSynthesizerMG::BltSynth(
				IE_LAYER_SYNTH_FOURCC('o','v','e','r'),
				1.0,
				&under_img,
				0, 0,
				img_size.width, img_size.height,
				&over_img,
				0, 0);
			//SaveIplImageExt("test_data\\layer_synth\\overlay_dst.png", &dst_img);
			double diff = GetMaxAbsDiffExt(&under_img, &correct_img);
			TS_ASSERT(diff < 1.0);
		}

		//screen
		{
			if (!LoadIplImageExt("test_data\\layer_synth\\under.png", &under_img)) {
				fprintf(stderr, "can't load test_data\\layer_synth\\under.png\n");
				return;
			}
			if (!LoadIplImageExt("test_data\\layer_synth\\screen.png", &correct_img)) {
				fprintf(stderr,"cant't load test_data\\layer_synth\\screen.png\n"); 
				return;
			}
			LayerSynthesizerMG::BltSynth(
				IE_LAYER_SYNTH_FOURCC('s','c','r','n'),
				1.0,
				&under_img,
				0, 0,
				img_size.width, img_size.height,
				&over_img,
				0, 0);
			//SaveIplImageExt("test_data\\layer_synth\\screen_dst.png", &dst_img);
			double diff = GetMaxAbsDiffExt(&under_img, &correct_img);
			TS_ASSERT(diff < 4.0);
		}

		//softlight
		{
			if (!LoadIplImageExt("test_data\\layer_synth\\under.png", &under_img)) {
				fprintf(stderr, "can't load test_data\\layer_synth\\under.png\n");
				return;
			}
			if (!LoadIplImageExt("test_data\\layer_synth\\softlight.png", &correct_img)) {
				fprintf(stderr,"cant't load test_data\\layer_synth\\softlight.png\n"); 
				return;
			}
			LayerSynthesizerMG::BltSynth(
				IE_LAYER_SYNTH_FOURCC('s','L','i','t'),
				1.0,
				&under_img,
				0, 0,
				img_size.width, img_size.height,
				&over_img,
				0, 0);
			//SaveIplImageExt("test_data\\layer_synth\\softlight_dst.png", &dst_img);
			double diff = GetMaxAbsDiffExt(&under_img, &correct_img);
			TS_ASSERT(diff < 1.0);
		}
	}

	void test_BlendSynth_alphablend()
	{
		{//
			IplImageExt dst_img, under_img, over_img, correct_img;
			ImgFile_Ptr f1A = m_ImgEdit.LoadImgFile("test_data\\alphablend\\alphablend_test1A.fyd");
			if (f1A == NULL) {
				fprintf(stderr,"cant't load test_data\\alphablend\\alphablend_test1A.fyd\n"); 
				return;
			}
			ImgFile_Ptr f1B = m_ImgEdit.LoadImgFile("test_data\\alphablend\\alphablend_test1B.fyd");
			if (f1B == NULL) {
				fprintf(stderr,"cant't load test_data\\alphablend\\alphablend_test1B.fyd\n"); 
				return;
			}

			ImgLayer_Ptr l1A_1 = std::dynamic_pointer_cast<ImgLayer>(f1A->GetLayerAtIndex(1));
			ImgLayer_Ptr l1A_0 = std::dynamic_pointer_cast<ImgLayer>(f1A->GetLayerAtIndex(0));
			l1A_1->GetLayerImage()->Copy(&over_img);
			l1A_0->GetLayerImage()->Copy(&under_img);

			ImgLayer_Ptr l1B_0 = std::dynamic_pointer_cast<ImgLayer>(f1B->GetLayerAtIndex(0));
			l1B_0->GetLayerImage()->Copy(&correct_img);

			CvSize img_size = over_img.GetSize();
			dst_img.Init(img_size.width, img_size.height, IPL_DEPTH_8U, 4);

			LayerSynthesizerMG::BlendSynth(
				IE_LAYER_SYNTH_FOURCC('n','o','r','m'),
				1.0,
				&dst_img,
				0, 0,
				img_size.width, img_size.height,
				&under_img,
				0, 0,
				&over_img,
				0, 0);

			//SaveIplImageExt("test_data\\alphablend\\test1_dst_img.bmp", &dst_img);
			//SaveIplImageExt("test_data\\alphablend\\test1_over_img.bmp", &over_img);
			//SaveIplImageExt("test_data\\alphablend\\test1_under_img.bmp", &under_img);
			//SaveIplImageExt("test_data\\alphablend\\test1_correct_img.bmp", &correct_img);

			double diff = GetMaxAbsDiffExt(&dst_img, &correct_img);
			TS_ASSERT(diff < 4.0);
		}

		{//
			IplImageExt dst_img, under_img, over_img, correct_img;
			ImgFile_Ptr f2A = m_ImgEdit.LoadImgFile("test_data\\alphablend\\alphablend_test2A.fyd");
			if (f2A == NULL) {
				fprintf(stderr,"cant't load test_data\\alphablend\\alphablend_test2A.fyd\n"); 
				return;
			}
			ImgFile_Ptr f2B = m_ImgEdit.LoadImgFile("test_data\\alphablend\\alphablend_test2B.fyd");
			if (f2B == NULL) {
				fprintf(stderr,"cant't load test_data\\alphablend\\alphablend_test2B.fyd\n"); 
				return;
			}

			ImgLayer_Ptr l2A_1 = std::dynamic_pointer_cast<ImgLayer>(f2A->GetLayerAtIndex(1));
			ImgLayer_Ptr l2A_0 = std::dynamic_pointer_cast<ImgLayer>(f2A->GetLayerAtIndex(0));
			l2A_1->GetLayerImage()->Copy(&over_img);
			l2A_0->GetLayerImage()->Copy(&under_img);

			ImgLayer_Ptr l2B_0 = std::dynamic_pointer_cast<ImgLayer>(f2B->GetLayerAtIndex(0));
			l2B_0->GetLayerImage()->Copy(&correct_img);

			CvSize img_size = over_img.GetSize();
			dst_img.Init(img_size.width, img_size.height, IPL_DEPTH_8U, 4);

			LayerSynthesizerMG::BlendSynth(
				IE_LAYER_SYNTH_FOURCC('n','o','r','m'),
				1.0,
				&dst_img,
				0, 0,
				img_size.width, img_size.height,
				&under_img,
				0, 0,
				&over_img,
				0, 0);

			//SaveIplImageExt("test_data\\alphablend\\test2_dst_img.bmp", &dst_img);
			//SaveIplImageExt("test_data\\alphablend\\test2_over_img.bmp", &over_img);
			//SaveIplImageExt("test_data\\alphablend\\test2_under_img.bmp", &under_img);
			//SaveIplImageExt("test_data\\alphablend\\test2_correct_img.bmp", &correct_img);
			double diff = GetMaxAbsDiffExt(&dst_img, &correct_img);
			TS_ASSERT(diff < 4.0);
		}
	}

	void test_BltSynth_alphablend()
	{
		{//
			IplImageExt under_img, over_img, correct_img;
			ImgFile_Ptr f1A = m_ImgEdit.LoadImgFile("test_data\\alphablend\\alphablend_test1A.fyd");
			if (f1A == NULL) {
				fprintf(stderr,"cant't load test_data\\alphablend\\alphablend_test1A.fyd\n"); 
				return;
			}
			ImgFile_Ptr f1B = m_ImgEdit.LoadImgFile("test_data\\alphablend\\alphablend_test1B.fyd");
			if (f1B == NULL) {
				fprintf(stderr,"cant't load test_data\\alphablend\\alphablend_test1B.fyd\n"); 
				return;
			}

			ImgLayer_Ptr l1A_1 = std::dynamic_pointer_cast<ImgLayer>(f1A->GetLayerAtIndex(1));
			ImgLayer_Ptr l1A_0 = std::dynamic_pointer_cast<ImgLayer>(f1A->GetLayerAtIndex(0));
			l1A_1->GetLayerImage()->Copy(&over_img);
			l1A_0->GetLayerImage()->Copy(&under_img);

			ImgLayer_Ptr l1B_0 = std::dynamic_pointer_cast<ImgLayer>(f1B->GetLayerAtIndex(0));
			l1B_0->GetLayerImage()->Copy(&correct_img);

			CvSize img_size = over_img.GetSize();

			LayerSynthesizerMG::BltSynth(
				IE_LAYER_SYNTH_FOURCC('n','o','r','m'),
				1.0,
				&under_img,
				0, 0,
				img_size.width, img_size.height,
				&over_img,
				0, 0);

			//SaveIplImageExt("test_data\\alphablend\\test1_dst_img.bmp", &dst_img);
			//SaveIplImageExt("test_data\\alphablend\\test1_over_img.bmp", &over_img);
			//SaveIplImageExt("test_data\\alphablend\\test1_under_img.bmp", &under_img);
			//SaveIplImageExt("test_data\\alphablend\\test1_correct_img.bmp", &correct_img);

			double diff = GetMaxAbsDiffExt(&under_img, &correct_img);
			TS_ASSERT(diff < 4.0);
		}

		{//
			IplImageExt under_img, over_img, correct_img;
			ImgFile_Ptr f2A = m_ImgEdit.LoadImgFile("test_data\\alphablend\\alphablend_test2A.fyd");
			if (f2A == NULL) {
				fprintf(stderr,"cant't load test_data\\alphablend\\alphablend_test2A.fyd\n"); 
				return;
			}
			ImgFile_Ptr f2B = m_ImgEdit.LoadImgFile("test_data\\alphablend\\alphablend_test2B.fyd");
			if (f2B == NULL) {
				fprintf(stderr,"cant't load test_data\\alphablend\\alphablend_test2B.fyd\n"); 
				return;
			}

			ImgLayer_Ptr l2A_1 = std::dynamic_pointer_cast<ImgLayer>(f2A->GetLayerAtIndex(1));
			ImgLayer_Ptr l2A_0 = std::dynamic_pointer_cast<ImgLayer>(f2A->GetLayerAtIndex(0));
			l2A_1->GetLayerImage()->Copy(&over_img);
			l2A_0->GetLayerImage()->Copy(&under_img);

			ImgLayer_Ptr l2B_0 = std::dynamic_pointer_cast<ImgLayer>(f2B->GetLayerAtIndex(0));
			l2B_0->GetLayerImage()->Copy(&correct_img);

			CvSize img_size = over_img.GetSize();

			LayerSynthesizerMG::BltSynth(
				IE_LAYER_SYNTH_FOURCC('n','o','r','m'),
				1.0,
				&under_img,
				0, 0,
				img_size.width, img_size.height,
				&over_img,
				0, 0);

			//SaveIplImageExt("test_data\\alphablend\\test2_dst_img.bmp", &dst_img);
			//SaveIplImageExt("test_data\\alphablend\\test2_over_img.bmp", &over_img);
			//SaveIplImageExt("test_data\\alphablend\\test2_under_img.bmp", &under_img);
			//SaveIplImageExt("test_data\\alphablend\\test2_correct_img.bmp", &correct_img);
			double diff = GetMaxAbsDiffExt(&under_img, &correct_img);
			TS_ASSERT(diff < 4.0);
		}
	}

private:
	ImgEdit m_ImgEdit;
};