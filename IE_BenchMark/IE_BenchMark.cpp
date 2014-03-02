#include <iostream>
#include "IETimer.h"
#include "ImgEdit.h"
#include "libImgEdit.h"
#include "IEToolMG.h"

#define IS_16BYTE_ALIGNMENT(x) (!((int)(x) & (0xf)))
#define DIFF_16BYTE_ALIGNMENT(x) (((~(int)x) + 1) & 0xf)

//int main(void)
//{
//	ImgEdit imgEdit;
//	imgEdit.Init();
//
//	imgEdit.End();
//}

//void SolveSpline3D(int n, CvPoint2D64f* src_points, CvMat* dst)
//{
//	int i,j;
//	double* h = (double*) malloc(sizeof(double)*(n-1));
//	CvMat* A = cvCreateMat(n+1, n+1, CV_32F);
//	CvMat* b = cvCreateMat(n+1, 1, CV_32F);
//
//	for(i=0; i<n; i++){
//		h[i] = src_points[i+1].x - src_points[i].x;
//	}
//
//	//set A param
//	cvmSet(A, 0, 0, 1.0);
//	for(j=1; j<n+1; j++){
//		cvmSet(A, 0, j, 0.0);
//	}
//	for(i=1; i<n; i++){
//		for(j=0; j<i-1; j++){
//			cvmSet(A, i, j, 0.0);
//		}
//
//		cvmSet(A, i, i-1, h[i-1]);
//		cvmSet(A, i, i, 2*(h[i-1] + h[i]));
//		cvmSet(A, i, i+1, h[i]);
//
//		for(j=i+2; j<n+1; j++){
//			cvmSet(A, i, j, 0.0);
//		}
//	}
//	for(j=0; j<n; j++){
//		cvmSet(A, n, j, 0.0);
//	}
//	cvmSet(A, n, n, 1.0);
//
//	//set b param
//	cvmSet(b, 0, 0, 0.0);
//	for(i=1; i<n; i++){
//		cvmSet(b, i, 0, (3.0*(src_points[i+1].y - src_points[i].y)/h[i]) - (3.0*(src_points[i].y - src_points[i-1].y)/h[i-1]));
//	}
//	cvmSet(b, n, 0, 0.0);
//
//	cvSolve(A, b, dst, CV_LU);
//
//	cvReleaseMat(&A);
//	cvReleaseMat(&b);
//}
//
//float FuncSpline3D(int n, float x, const CvPoint2D64f* src_points, const CvMat* c_mat)
//{
//	assert(x >= src_points[0].x);
//	assert(x <= src_points[n].x );
//
//	int i;
//	for(i=0; i<n; i++){
//		if((src_points[i].x <= x) && (x <= src_points[i+1].x)){
//			break;
//		}
//	}
//	assert(i < n);
//
//	float h_i = src_points[i+1].x - src_points[i].x; 
//	float c = cvmGet(c_mat, i, 0);
//	float c2 = cvmGet(c_mat, i+1, 0);
//	float b = (src_points[i+1].y - src_points[i].y)/h_i - h_i*(2.0*c + c2)/3.0;
//	float d = (c2 - c)/(3.0*h_i);
//
//	float h = x - src_points[i].x;
//	return (src_points[i].y + b*h + c *h*h + d*h*h*h);
//}
//
//int main(void)
//{
//	int i,j;
//	int n=13;
//
//	CvPoint2D64f* points = (CvPoint2D64f*) malloc(sizeof(CvPoint2D64f)*(n+1));
//
//	points[0].x = 9; points[0].y = 130;
//	points[1].x = 13; points[1].y = 150;
//	points[2].x = 19; points[2].y = 185;
//	points[3].x = 21; points[3].y = 210;
//	points[4].x = 26; points[4].y = 260;
//	points[5].x = 30; points[5].y = 270;
//	points[6].x = 39; points[6].y = 240;
//	points[7].x = 44; points[7].y = 215;
//	points[8].x = 47; points[8].y = 205;
//	points[9].x = 50; points[9].y = 210;
//	points[10].x = 60; points[10].y = 225;
//	points[11].x = 70; points[11].y = 230;
//	points[12].x = 80; points[12].y = 225;
//	points[13].x = 92; points[13].y = 195;
//
//	IplImage* image = cvCreateImage(cvSize(100, 200), IPL_DEPTH_8U, 3);
//
//	CvMat* c_mat = cvCreateMat(n+1, 1, CV_32F);
//	SolveSpline3D(n, points, c_mat);
//
//	for(i=0; i<n-1; i++){
//		double c = cvmGet(c_mat, i, 0);
//		double c2 = cvmGet(c_mat, i+1, 0);
//		
//		double b = (points[i+1].y - points[i].y)/(points[i+1].x - points[i].x) - (points[i+1].x - points[i].x)*(2.0*c + c2)/3.0;
//		double d = (c2 - c)/(3.0*(points[i+1].x - points[i].x));
//
//		printf("b = %f c = %f d=%f \n", b, c, d);
//	}
//
//	cvSet(image, cvScalar(255, 255, 255));
//	int x;
//	for(x=10; x<=90; x++){
//		float y = FuncSpline3D(n, x, points, c_mat);
//		SetPixelToBGR(image, x, y, 0, 0, 0);
//	}
//
//	cvSaveImage("test.bmp", image);
//
//	cvReleaseMat(&c_mat);
//	cvReleaseImage(&image);
//	free(points);
//}

//int main(void)
//{
//	int x,y;
//
//	int alpha = 150;
//	int r,g,b;
//	int r1, g1, b1;
//	int r2, g2, b2;
//	int ra = 255 - alpha;
//	UCvPixel *buf1;
//	UCvPixel *buf2;
//	UCvPixel *buf3;
//
//	IplImage* test1 = cvLoadImage("test1.bmp");
//	IplImage* test2 = cvLoadImage("test2.bmp");
//	IplImage* b_ch = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);
//	IplImage* g_ch = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);
//	IplImage* r_ch = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);
//	IplImage* a_ch = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);
//	IplImage* img1 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 4);
//	IplImage* img2 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 4);
//	IplImage* img3 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 4);
//	IplImage* img4 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 4);
//	IplImage* img5 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 4);
//
//	cvSet(a_ch, cvScalar(0));
//
//	cvSplit(test1, b_ch, g_ch, r_ch, NULL);
//	cvMerge(b_ch, g_ch, r_ch, a_ch, img1);
//	cvSplit(test2, b_ch, g_ch, r_ch, NULL);
//	cvMerge(b_ch, g_ch, r_ch, a_ch, img2);
//
//	IETimer timer;
//	for(y=0; y<img1->height; y++){
//		for(x=0; x<img1->width; x++){
//			buf1 = GetPixelAddress32(img1, x, y);
//			buf2 = GetPixelAddress32(img2, x, y);
//			buf3 = GetPixelAddress32(img3, x, y);
//			buf3->b = (buf1->b*ra + buf2->b * alpha) >> 8;
//			buf3->g = (buf1->g*ra + buf2->g * alpha) >> 8;
//			buf3->r = (buf1->r*ra + buf2->r * alpha) >> 8;
//		}
//	}
//	std::cout << timer.elapsed() << " msec" << std::endl;
//	cvSaveImage("test3.bmp", img3);
//
//	timer.restart();
//	for(y=0; y<img1->height; y++){
//		for(x=0; x<img1->width; x++){
//			buf1 = GetPixelAddress32(img1, x, y);
//			buf2 = GetPixelAddress32(img2, x, y);
//			buf3 = GetPixelAddress32(img4, x, y);
//			__asm{
//				pslld		xmm6, 32;			// xmm6 [ 0, 0, 0, 0]
//				mov			eax, buf1;			//
//				movdqu		xmm0, [eax];		// xmm0 [ *, *, *, a1r1g1b1]
//				punpcklbw	xmm0, xmm6;			// xmm0 [ *, *, a1r1, g1b1]
//				mov			eax, buf2;
//				movdqu		xmm1, [eax];		// xmm1 [ *, *, *, a2r2g2b2]
//				punpcklbw	xmm1, xmm6;			// xmm1 [ *, *, a2r2, g2b2]
//				mov			eax, alpha;
//				movd		xmm3, eax;			// xmm3 [ *,  *,  *, *a2]
//				pshuflw		xmm3, xmm3, 0x00;	// xmm3 [ *,  *, a2a2, a2a2]
//				mov			eax, ra;
//				movd		xmm4, eax;			// xmm4 [ *, *,   *, *0xff]
//				pshuflw		xmm4, xmm4, 0x00;	// xmm4 [ *, *, 0xff0xff, 0xff0xff]
//				pmullw		xmm0, xmm4;			// xmm0 = data1 * a1
//				pmullw		xmm3, xmm1;			// xmm3 = data2 * a2
//				paddw		xmm3, xmm0;			// xmm3 = data1 * a1 + data2 * a2
//				psrlw		xmm3, 8;			// xmm3 = (data1 * a1 + data2 * a2) >> 8
//				packuswb	xmm3, xmm6;			// xmm3 [ *, *, *, abgr,]
//				mov			eax, buf3;
//				movd		[eax], xmm3;		//
//			}
//		}
//	}					
//	std::cout << timer.elapsed() << " msec" << std::endl;
//	cvSaveImage("test4.bmp", img4);
//
//	timer.restart();
//	for(y=0; y<img1->height; y++){
//		for(x=0; x<img1->width; x+=2){
//			buf1 = GetPixelAddress32(img1, x, y);
//			buf2 = GetPixelAddress32(img2, x, y);
//			buf3 = GetPixelAddress32(img5, x, y);
//			__asm{
//				pslld		xmm6, 32;			// xmm6 [ 0, 0, 0, 0]
//				mov			eax, buf1;			//
//				movdqu		xmm0, [eax];		// xmm0 [ *, *, *, a11r11g11b11]
//				punpcklbw	xmm0, xmm6;			// xmm0 [ *, *, a11r11, g11b11]
//				mov			eax, buf2;
//				movdqu		xmm1, [eax];		// xmm1 [ *, *, *, a21r21g21b21]
//				punpcklbw	xmm1, xmm6;			// xmm1 [ a22r22, g22b22, a21r21, g21b21]
//				mov			eax, alpha;
//				movd		xmm3, eax;			// xmm3 [ *,  *,  *, *a2]
//				pshuflw		xmm3, xmm3, 0x00;	// xmm3 [ *,  *, a2a2, a2a2]
//				pshufd		xmm3, xmm3, 0x00;	// xmm3 [ a2a2, a2a2, a2a2, a2a2]
//				mov			eax, ra;
//				movd		xmm4, eax;			// xmm4 [ *, *,   *, *0xff]
//				pshuflw		xmm4, xmm4, 0x00;	// xmm4 [ *, *, 0xff0xff, 0xff0xff]
//				pshufd		xmm4, xmm4, 0x00;	// xmm4 [ 0xff0xff, 0xff0xff, 0xff0xff, 0xff0xff]
//				pmullw		xmm0, xmm4;			// xmm0 = data1 * a1
//				pmullw		xmm3, xmm1;			// xmm3 = data2 * a2
//				paddw		xmm3, xmm0;			// xmm3 = data1 * a1 + data2 * a2
//				psrlw		xmm3, 8;			// xmm3 = (data1 * a1 + data2 * a2) >> 8
//				packuswb	xmm3, xmm6;			// xmm3 [ *, *, *, abgr,]
//				mov			eax, buf3;
//				movlpd		[eax], xmm3;		//
//			}
//		}
//	}					
//	std::cout << timer.elapsed() << " msec" << std::endl;
//	cvSaveImage("test5.bmp", img5);
//
//	cvReleaseImage(&b_ch);
//	cvReleaseImage(&g_ch);
//	cvReleaseImage(&r_ch);
//	cvReleaseImage(&a_ch);
//	cvReleaseImage(&img1);
//	cvReleaseImage(&img2);
//	cvReleaseImage(&img3);
//	cvReleaseImage(&img4);
//	cvReleaseImage(&img5);
//}

//sseを使ったalphablend 
//int main(void)
//{
//	int x,y;
//
//	int alpha = 150; //アルファ値150でアルファブレンドを行う
//	int r,g,b;
//	int r1, g1, b1;
//	int r2, g2, b2;
//	int ra = 255 - alpha;
//	UCvPixel *buf1;
//	UCvPixel *buf2;
//	UCvPixel *buf3;
//
//	IplImage* test1 = cvLoadImage("test1.bmp");
//	IplImage* test2 = cvLoadImage("test2.bmp");
//	IplImage* b_ch = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);
//	IplImage* g_ch = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);
//	IplImage* r_ch = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);
//	IplImage* a_ch = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);
//	IplImage* img1 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 4);
//	IplImage* img2 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 4);
//	IplImage* img3 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 4);
//	IplImage* img4 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 4);
//	IplImage* img5 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 4);
//	IplImage* img6 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 4);
//
//	cvSet(a_ch, cvScalar(255));
//
//	cvSplit(test1, b_ch, g_ch, r_ch, NULL);
//	cvMerge(b_ch, g_ch, r_ch, a_ch, img1);
//	cvSplit(test2, b_ch, g_ch, r_ch, NULL);
//	cvMerge(b_ch, g_ch, r_ch, a_ch, img2);
//
//	//非SSE
//	IETimer timer;
//	for(y=0; y<img1->height; y++){
//		for(x=0; x<img1->width; x++){
//			buf1 = GetPixelAddress32(img1, x, y);
//			buf2 = GetPixelAddress32(img2, x, y);
//			buf3 = GetPixelAddress32(img3, x, y);
//			buf3->b = (buf1->b*ra + buf2->b * alpha) >> 8;
//			buf3->g = (buf1->g*ra + buf2->g * alpha) >> 8;
//			buf3->r = (buf1->r*ra + buf2->r * alpha) >> 8;
//		}
//	}
//	std::cout << timer.elapsed() << " msec" << std::endl;
//	cvSaveImage("test3.bmp", img3);
//
//	//SSE: _mm_loadu_ps + _mm_storeu_ps
//	timer.restart();
//	for(y=0; y<img1->height; y++){
//		for(x=0; x<img1->width; x+=4){
//			buf1 = GetPixelAddress32(img1, x, y);
//			buf2 = GetPixelAddress32(img2, x, y);
//			buf3 = GetPixelAddress32(img4, x, y);
//
//			__m128i under = _mm_castps_si128( _mm_loadu_ps((float*)buf1) );
//			__m128i over = _mm_castps_si128( _mm_loadu_ps((float*)buf2) );
//			__m128i xmalpha = _mm_set1_epi16(alpha);
//			__m128i xmra = _mm_set1_epi16(255-alpha);
//			__m128i xmzero = _mm_setzero_si128();
//
//			//calc lo quad word
//			__m128i xmo = _mm_unpacklo_epi8(over, xmzero);
//			__m128i xmu = _mm_unpacklo_epi8(under, xmzero);
//			xmo = _mm_mullo_epi16(xmo, xmalpha); //(over*(255-alpha)
//			xmu = _mm_mullo_epi16(xmu, xmra); //under * alpha
//			xmo = _mm_add_epi16(xmo, xmu); //(over*(255 - alpha) + under*alpha)
//			__m128i xmlo = _mm_srli_epi16(xmo, 8); //(over*(255 - alpha) + under*alpha) >> 8
//
//			//calc hi quad word
//			xmo = _mm_unpackhi_epi8(over, xmzero);
//			xmu = _mm_unpackhi_epi8(under, xmzero);
//			xmo = _mm_mullo_epi16(xmo, xmalpha); //(over*(255-alpha)
//			xmu = _mm_mullo_epi16(xmu, xmra); //under * alpha
//			xmo = _mm_add_epi16(xmo, xmu); //(over*(255 - alpha) + under*alpha)
//			__m128i xmhi = _mm_srli_epi16(xmo, 8); //(over*(255 - alpha) + under*alpha) >> 8
//
//			//store to memory
//			_mm_storeu_ps((float*)buf3, 
//				_mm_castsi128_ps( _mm_packus_epi16(xmlo, xmhi) ));
//		}
//	}			
//	std::cout << timer.elapsed() << " msec" << std::endl;
//	cvSaveImage("test4.bmp", img4);
//
//	//SSE: _mm_load_ps + _mm_store_ps
//	timer.restart();
//	for(y=0; y<img1->height; y++){
//		for(x=0; x<img1->width; x+=4){
//			buf1 = GetPixelAddress32(img1, x, y);
//			buf2 = GetPixelAddress32(img2, x, y);
//			buf3 = GetPixelAddress32(img5, x, y);
//
//			__m128i under = _mm_castps_si128( _mm_load_ps((float*)buf1) );
//			__m128i over = _mm_castps_si128( _mm_load_ps((float*)buf2) );
//			__m128i xmalpha = _mm_set1_epi16(alpha);
//			__m128i xmra = _mm_set1_epi16(255-alpha);
//			__m128i xmzero = _mm_setzero_si128();
//
//			//calc lo quad word
//			__m128i xmo = _mm_unpacklo_epi8(over, xmzero);
//			__m128i xmu = _mm_unpacklo_epi8(under, xmzero);
//			xmo = _mm_mullo_epi16(xmo, xmalpha); //(over*(255-alpha)
//			xmu = _mm_mullo_epi16(xmu, xmra); //under * alpha
//			xmo = _mm_add_epi16(xmo, xmu); //(over*(255 - alpha) + under*alpha)
//			__m128i xmlo = _mm_srli_epi16(xmo, 8); //(over*(255 - alpha) + under*alpha) >> 8
//
//			//calc hi quad word
//			xmo = _mm_unpackhi_epi8(over, xmzero);
//			xmu = _mm_unpackhi_epi8(under, xmzero);
//			xmo = _mm_mullo_epi16(xmo, xmalpha); //(over*(255-alpha)
//			xmu = _mm_mullo_epi16(xmu, xmra); //under * alpha
//			xmo = _mm_add_epi16(xmo, xmu); //(over*(255 - alpha) + under*alpha)
//			__m128i xmhi = _mm_srli_epi16(xmo, 8); //(over*(255 - alpha) + under*alpha) >> 8
//
//			//store to memory
//			_mm_store_ps((float*)buf3, 
//				_mm_castsi128_ps( _mm_packus_epi16(xmlo, xmhi) ));
//		}
//	}					
//	std::cout << timer.elapsed() << " msec" << std::endl;
//	cvSaveImage("test5.bmp", img5);
//
//	//SSE: _mm_load_ps + _mm_stream_si128
//	timer.restart();
//	for(y=0; y<img1->height; y++){
//		for(x=0; x<img1->width; x+=4){
//			buf1 = GetPixelAddress32(img1, x, y);
//			buf2 = GetPixelAddress32(img2, x, y);
//			buf3 = GetPixelAddress32(img6, x, y);
//
//			__m128i under = _mm_castps_si128( _mm_load_ps((float*)buf1) );
//			__m128i over = _mm_castps_si128( _mm_load_ps((float*)buf2) );
//			__m128i xmalpha = _mm_set1_epi16(alpha);
//			__m128i xmra = _mm_set1_epi16(255-alpha);
//			__m128i xmzero = _mm_setzero_si128();
//
//			//calc lo quad word
//			__m128i xmo = _mm_unpacklo_epi8(over, xmzero);
//			__m128i xmu = _mm_unpacklo_epi8(under, xmzero);
//			xmo = _mm_mullo_epi16(xmo, xmalpha); //(over*(255-alpha)
//			xmu = _mm_mullo_epi16(xmu, xmra); //under * alpha
//			xmo = _mm_add_epi16(xmo, xmu); //(over*(255 - alpha) + under*alpha)
//			__m128i xmlo = _mm_srli_epi16(xmo, 8); //(over*(255 - alpha) + under*alpha) >> 8
//
//			//calc hi quad word
//			xmo = _mm_unpackhi_epi8(over, xmzero);
//			xmu = _mm_unpackhi_epi8(under, xmzero);
//			xmo = _mm_mullo_epi16(xmo, xmalpha); //(over*(255-alpha)
//			xmu = _mm_mullo_epi16(xmu, xmra); //under * alpha
//			xmo = _mm_add_epi16(xmo, xmu); //(over*(255 - alpha) + under*alpha)
//			__m128i xmhi = _mm_srli_epi16(xmo, 8); //(over*(255 - alpha) + under*alpha) >> 8
//
//			//store to memory
//			_mm_stream_si128((__m128i*)buf3, _mm_packus_epi16(xmlo, xmhi));
//		}
//	}					
//	std::cout << timer.elapsed() << " msec" << std::endl;
//	cvSaveImage("test6.bmp", img6);
//
//
//	cvReleaseImage(&b_ch);
//	cvReleaseImage(&g_ch);
//	cvReleaseImage(&r_ch);
//	cvReleaseImage(&a_ch);
//	cvReleaseImage(&img1);
//	cvReleaseImage(&img2);
//	cvReleaseImage(&img3);
//	cvReleaseImage(&img4);
//	cvReleaseImage(&img5);
//	cvReleaseImage(&img6);
//}


//int main(void)
//{
//	IplImage* dst = cvCreateImage(cvSize(1000, 1000), IPL_DEPTH_8U, 4);
//	IplImageExt src;
//	LoadIplImageExt("test.bmp", &src);
//	CvMat* mat = cvCreateMat(2, 3, CV_32F);
//	cvSet(mat, cvScalar(0));
//
//	//回転
//	CvPoint2D64f center;
//	center.x = dst->width / 2.0;
//	center.y = dst->height / 2.0;
//	cv2DRotationMatrix(center, 60, 1.0, mat);
//
//	//拡大縮小、
//	cvmSet(mat, 0, 0, (50/100.0)*cvmGet(mat, 0, 0));
//	cvmSet(mat, 0, 1, (50/100.0)*cvmGet(mat, 0, 1));
//	cvmSet(mat, 0, 2, (50/100.0)*cvmGet(mat, 0, 2));
//	cvmSet(mat, 1, 0, (50/100.0)*cvmGet(mat, 1, 0));
//	cvmSet(mat, 1, 1, (50/100.0)*cvmGet(mat, 1, 1));
//	cvmSet(mat, 1, 2, (50/100.0)*cvmGet(mat, 1, 2));
//
//	//平行移動
//	cvmSet(	mat, 0, 2,
//		300 + cvmGet(mat, 0, 2));
//	cvmSet(mat, 1, 2,
//		300 + cvmGet(mat, 1, 2));
//
//	UCvPixel color = ucvPixel(100, 100, 100, 255);
//	WarpAffine_NN(dst, &src, mat, &color);
//
//	cvSaveImage("dst.bmp", dst);
//	cvReleaseImage(&dst);
//}

//int main(void)
//{
//	CvSize size = cvSize(200,180);
//	IplImage* dst_1c = cvCreateImage(size, IPL_DEPTH_8U, 1);
//	IplImage* fdst_1c = cvCreateImage(size, IPL_DEPTH_8U, 1);
//	IplImage* dst_3c = cvCreateImage(size, IPL_DEPTH_8U, 3);
//
//	cvSet(dst_1c, cvScalar(0));
//
//	double center_x = 0.5;
//	double center_y = 15.5;
//	for(int i=0; i<10; i++){
//		double r = i+1;
//		center_x += 2.0*r + 5.0;
//		FillCircleMaskAA(dst_1c, center_x, center_y, r, 5, 255, 255);
//	}
//
//	center_x = 0.5;
//	center_y = 40.5;
//	for(int i=0; i<10; i++){
//		double r = i+1;
//		center_x += 2.0*r + 5.0;
//		FillCircleMaskAA2(dst_1c, center_x, center_y, r, 1.0, 255, 255);
//	}
//
//	center_x = 0.5;
//	center_y = 65.5;
//	for(int i=0; i<10; i++){
//		double r = i+1;
//		center_x += 2.0*r + 5.0;
//		FillCircleMaskAA2(dst_1c, center_x, center_y, r, 1.5, 255, 255);
//	}
//
//	center_x = 0.5;
//	center_y = 90.5;
//	for(int i=0; i<10; i++){
//		double r = i+1;
//		center_x += 2.0*r + 5.0;
//		FillCircleMaskAA2(dst_1c, center_x, center_y, r, 2.0, 255, 255);
//	}
//
//	center_x = 0.5;
//	center_y = 115.5;
//	for(int i=0; i<10; i++){
//		double r = i+1;
//		center_x += 2.0*r + 5.0;
//		FillCircleMaskAA3(dst_1c, center_x, center_y, r, 1.0, 255, 255);
//	}
//
//	center_x = 0.5;
//	center_y = 140.5;
//	for(int i=0; i<10; i++){
//		double r = i+1;
//		center_x += 2.0*r + 5.0;
//		FillCircleMaskAA3(dst_1c, center_x, center_y, r, 1.5, 255, 255);
//	}
//
//	center_x = 0.5;
//	center_y = 165.5;
//	for(int i=0; i<10; i++){
//		double r = i+1;
//		center_x += 2.0*r + 5.0;
//		FillCircleMaskAA3(dst_1c, center_x, center_y, r, 2.0, 255, 255);
//	}
//
//	cvNot(dst_1c, fdst_1c);
//	cvCvtColor(fdst_1c, dst_3c, CV_GRAY2BGR);
//	cvSaveImage("aa_test.bmp", dst_3c);
//
//	cvReleaseImage(&dst_1c);
//	cvReleaseImage(&fdst_1c);
//	cvReleaseImage(&dst_3c);
//}

//int main(void)
//{
//	CvSize size = cvSize(200,350);
//	IplImage* dst_1c = cvCreateImage(size, IPL_DEPTH_8U, 1);
//	IplImage* fdst_1c = cvCreateImage(size, IPL_DEPTH_8U, 1);
//	IplImage* dst_3c = cvCreateImage(size, IPL_DEPTH_8U, 3);
//
//	cvSet(dst_1c, cvScalar(0));
//
//	double max_r = 2.5;
//	double center_x = 0.5;
//	double center_y = 35.5;
//
//	IETimer timer1;
//	for(int i=0; i<180; i++){
//		double rad = (i/90.0) * 3.14159265;
//		double y = 10.0*sin(rad);
//		double r = fabs(sin(rad/2.0)*max_r+0.001);
//		AddFillCircleMaskAA(dst_1c, i+5, center_y+y, r, 5, 255, 255);
//	}
//	std::cout << timer1.elapsed() << "msec" << std::endl;
//
//	max_r = 60.5;
//	center_x = 0.5;
//	center_y = 85.5;
//
//	timer1.restart();
//	for(int i=0; i<180; i++){
//		double rad = (i/90.0) * 3.14159265;
//		double y = 10.0*sin(rad);
//		double r = fabs(sin(rad/2.0)*max_r+0.001);
//		AddSoftCircleMask(dst_1c, i+5, center_y+y, r, 255, 255);
//	}
//	std::cout << timer1.elapsed() << "msec" << std::endl;
//
//	max_r = 60.5;
//	center_x = 0.5;
//	center_y = 135.5;
//
//	timer1.restart();
//	for(int i=0; i<180; i++){
//		float rad = (i/90.0) * 3.14159265;
//		float y = 10.0*sin(rad);
//		float r = fabs(sin(rad/2.0)*max_r+0.001);
//		AddSoftCircleMaskf(dst_1c, i+5, center_y+y, r, 255, 255);
//	}
//	std::cout << timer1.elapsed() << "msec" << std::endl;
//
//	max_r = 2.5;
//	center_x = 0.5;
//	center_y = 185.5;
//	for(int i=0; i<180; i++){
//		double rad = (i/90.0) * 3.14159265;
//		double y = 10.0*sin(rad);
//		double r = fabs(sin(rad/2.0)*max_r+0.001);
//		AddFillCircleMaskAA2(dst_1c, i+5, center_y+y, r, 2.0, 255, 255);
//	}
//
//	center_x = 0.5;
//	center_y = 235.5;
//	for(int i=0; i<180; i++){
//		double rad = (i/90.0) * 3.14159265;
//		double y = 10.0*sin(rad);
//		double r = fabs(sin(rad/2.0)*max_r+0.001);
//		AddFillCircleMaskAA3(dst_1c, i+5, center_y+y, r, 1.0, 255, 255);
//	}
//
//	center_x = 0.5;
//	center_y = 280.5;
//	for(int i=0; i<180; i++){
//		double rad = (i/90.0) * 3.14159265;
//		double y = 10.0*sin(rad);
//		double r = fabs(sin(rad/2.0)*max_r+0.001);
//		AddFillCircleMaskAA3(dst_1c, i+5, center_y+y, r, 1.5, 255, 255);
//	}
//
//	center_x = 0.5;
//	center_y = 330.5;
//	for(int i=0; i<180; i++){
//		double rad = (i/90.0) * 3.14159265;
//		double y = 10.0*sin(rad);
//		double r = fabs(sin(rad/2.0)*max_r+0.001);
//		AddFillCircleMaskAA3(dst_1c, i+5, center_y+y, r, 2.0, 255, 255);
//	}
//
//	cvNot(dst_1c, fdst_1c);
//	cvCvtColor(fdst_1c, dst_3c, CV_GRAY2BGR);
//	cvSaveImage("brush_test.bmp", dst_3c);
//
//	cvReleaseImage(&dst_1c);
//	cvReleaseImage(&fdst_1c);
//	cvReleaseImage(&dst_3c);
//}


//int main(void)
//{
//	CvSize size = cvSize(1000,250);
//	IplImage* dst_1c = cvCreateImage(size, IPL_DEPTH_8U, 1);
//	IplImage* fdst_1c = cvCreateImage(size, IPL_DEPTH_8U, 1);
//	IplImage* dst_3c = cvCreateImage(size, IPL_DEPTH_8U, 3);
//
//	cvSet(dst_1c, cvScalar(0));
//	double center_x;
//	double center_y;
//
//	IETimer timer1;
//	for(int k=0; k<10; k++){
//		for(int j=0; j<100; j++){
//			center_x = 0.5;
//			center_y = 70.5;
//			for(int i=0; i<10; i++){
//				double r = 62;
//				center_x += 2.5*r + 5.0;
//				FillCircleMaskAA2(dst_1c, center_x, center_y, 62, 0.5, 255, 255);
//			}
//		}
//	}
//	std::cout << timer1.elapsed() << "msec" << std::endl;
//
//	IETimer timer2;
//	for(int k=0; k<10; k++){
//		for(int j=0; j<100; j++){
//			center_x = 190.5;
//			center_y = 80.5;
//			for(int i=0; i<10; i++){
//				double r = 62;
//				center_x += 2.5*r + 5.0;
//				FillCircleMaskAA2e(dst_1c, center_x, center_y, 62, 0.5, 255, 255);
//			}
//		}
//	}
//	std::cout << timer2.elapsed() << "msec" << std::endl;
//
//	cvNot(dst_1c, fdst_1c);
//	cvCvtColor(fdst_1c, dst_3c, CV_GRAY2BGR);
//	cvSaveImage("aa_test.bmp", dst_3c);
//
//	cvReleaseImage(&dst_1c);
//	cvReleaseImage(&fdst_1c);
//	cvReleaseImage(&dst_3c);
//}


//int main(void)
//{
//	int x,y;
//
//	int alpha = 150;
//	int r,g,b;
//	int r1, g1, b1;
//	int r2, g2, b2;
//	int a = 255 - alpha;
//	unsigned char *buf1;
//	unsigned char *buf2;
//	unsigned char *buf3;
//
//	IplImage* img1 = cvLoadImage("test1.bmp");
//	IplImage* img2 = cvLoadImage("test2.bmp");
//	IplImage* img3 = cvCreateImage(cvSize(img1->width, img1->height), IPL_DEPTH_8U, 3);
//	IplImage* img4 = cvCreateImage(cvSize(img1->width, img1->height), IPL_DEPTH_8U, 3);
//
//
//	UCvPixel pix1;
//	UCvPixel pix2;
//	IETimer timer1;
//	for(y=0; y<img1->height; y++){
//		for(x=0; x<img1->width; x++){
//			GetPixelFromBGR(img1, x, y, &pix1);
//			GetPixelFromBGR(img2, x, y, &pix2);
//			pix1.b = (pix1.b*a + pix2.b * alpha) >> 8;
//			pix1.g = (pix1.g*a + pix2.g * alpha) >> 8;
//			pix1.r = (pix1.r*a + pix2.r * alpha) >> 8;
//			SetPixelToBGR(img3, x, y, &pix1);
//		}
//	}
//	std::cout << timer1.elapsed() << " msec" << std::endl;
//	cvSaveImage("test3.bmp", img3);
//
//	IETimer timer2;
//	for(y=0; y<img1->height; y++){
//		for(x=0; x<img1->width; x++){
//			GetPixelFromBGR(img1, x, y, &pix1);
//			GetPixelFromBGR(img2, x, y, &pix2);
//			pix1.b = pix1.b + (((pix2.b - pix1.b) * alpha) >> 8);
//			pix1.g = pix1.g + (((pix2.g - pix1.g) * alpha) >> 8);
//			pix1.r = pix1.r + (((pix2.r - pix1.r) * alpha) >> 8);
//			SetPixelToBGR(img4, x, y, &pix1);
//		}
//	}					
//	std::cout << timer2.elapsed() << " msec" << std::endl;
//	cvSaveImage("test4.bmp", img4);
//
//	cvReleaseImage(&img1);
//	cvReleaseImage(&img2);
//	cvReleaseImage(&img3);
//	cvReleaseImage(&img4);
//}

//#include <stdio.h>
//
//int main(void)
//{
//    printf("char  : %d\n", sizeof(char));
//    printf("short : %d\n", sizeof(short));
//    printf("int   : %d\n", sizeof(int));
//    printf("long  : %d\n", sizeof(long));
//    printf("float : %d\n", sizeof(float));
//    printf("double: %d\n", sizeof(double));
//    printf("long long  : %d\n", sizeof(long long));
//    printf("long double: %d\n", sizeof(long double));
//    return 0;
//}


/////////////////////////////////////////////
// test gaussian filter
/////////////////////////////////////////////
//int main(void)
//{
//	IplImage* tmp = cvLoadImage("test.bmp");
//	IplImage* src = cvCreateImage(cvGetSize(tmp), IPL_DEPTH_8U, 4);
//	IplImage* dst1 = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 4);
//	IplImage* dst2 = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 4);
//	ucvCvtColor(tmp, src, CV_BGR2BGRA);
//
//	int i=0;
//	IETimer timer1;
//	cvSmooth(src, dst1, CV_GAUSSIAN, 7, 7, 0, 0);
//	std::cout << timer1.elapsed() << " msec" << std::endl;
//	cvSaveImage("cv_gaussian.bmp", dst1);
//
//	IETimer timer2;
//	ucvGaussianFilter(src, 0, 0, src->width, src->height, dst2, 0, 0); 
//	std::cout << timer2.elapsed() << " msec" << std::endl;
//	cvSaveImage("ucv_gaussian.bmp", dst2);
//
//	cvReleaseImage(&tmp);
//	cvReleaseImage(&src);
//	cvReleaseImage(&dst1);
//	cvReleaseImage(&dst2);
//}

////おせぇぇ
//unsigned int fast_sqrtf(unsigned int f)
//{
//	unsigned int s = f, t;
//	if(f == 0) return 0;
//	do{
//		t = s;
//		s = (t + f/ t) >> 1;
//	}while(s < t);
//	return t;
//}
//
//int main(void)
//{
//	int i;
//	double sum1=0.0;
//	float sum2=0.0;
//	double sum3=0.0;
//
//	IETimer timer1;
//	for(i=0; i<10000000; i++){
//		sum1 += sqrt((double)i);
//	}
//	printf("sum = %f time1 = %f \n", sum1, timer1.elapsed());
//
//	IETimer timer2;
//	for(i=0; i<10000000; i++){
//		sum2 += sqrtf((float)i);
//	}
//	printf("sum = %f time2 = %f \n", sum2, timer2.elapsed());
//
//	IETimer timer3;
//	for(i=0; i<10000000; i++){
//		sum3 += ((fast_sqrtf(i * 24)) >> 24);
//	}
//	printf("sum = %f time3 = %f \n", sum3, timer3.elapsed());
//
//	return 0;
//}


////ssememcpy test
//int main(int argc, char** argv)
//{
//	const int BUF_SIZE = 128000000;
//	const int WRITE_SIZE = 40000;
//	char* src = new char[BUF_SIZE];
//	char* dst1 = new char[BUF_SIZE];
//	char* dst2 = new char[BUF_SIZE];
//
//	printf("src = %p\n", src);
//	printf("dst1 = %p\n", dst1);
//	printf("dst2 = %p\n", dst2);
//
//	char* idx = src;
//	srand((unsigned)time(NULL));
//	for (int i = 0; i < BUF_SIZE; i++) {
//		(*idx++) = rand() & 0xff;
//	}
//
//	{
//		IETimer timer;
//		for (int writed = 0; writed < BUF_SIZE; writed += WRITE_SIZE) {
//			memcpy(dst1 + writed, src + writed, WRITE_SIZE);
//		}
//		if (memcmp(dst1, src, BUF_SIZE) != 0) {
//			printf("memcpy error\n");
//		}
//		std::cout << "memcpy" << timer.elapsed() << " msec" << std::endl;
//
//		timer.restart();
//		for (int writed = 0; writed < BUF_SIZE; writed += WRITE_SIZE) {
//			ssememcpy(dst2 + writed, src + writed, WRITE_SIZE);
//		}
//		if (memcmp(dst2, src, BUF_SIZE) != 0) {
//			printf("ssememcpy error\n");
//		}
//		std::cout << "ssememcpy" << timer.elapsed() << " msec" << std::endl;
//	}
//
//	{
//		dst1 += 1;
//		src += 1;
//		IETimer timer;
//		for (int writed = 0; writed < BUF_SIZE - WRITE_SIZE; writed += WRITE_SIZE) {
//			memcpy(dst1 + writed, src + writed, WRITE_SIZE);
//		}
//		if (memcmp(dst1, src, BUF_SIZE - WRITE_SIZE) != 0) {
//			printf("memcpy error\n");
//		}
//		std::cout << "memcpy" << timer.elapsed() << " msec" << std::endl;
//		dst1 -= 1;
//		src -= 1;
//
//		dst2 += 1;
//		src += 1;
//		timer.restart();
//		for (int writed = 0; writed < BUF_SIZE - WRITE_SIZE; writed += WRITE_SIZE) {
//			ssememcpy(dst2 + writed, src + writed, WRITE_SIZE);
//		}
//		if (memcmp(dst2, src, BUF_SIZE - WRITE_SIZE) != 0) {
//			printf("ssememcpy error\n");
//		}
//		std::cout << "ssememcpy" << timer.elapsed() << " msec" << std::endl;
//		dst2 -= 1;
//		src -= 1;
//	}
//
//	{
//		dst1 += 1;
//		IETimer timer;
//		for (int writed = 0; writed < BUF_SIZE - WRITE_SIZE; writed += WRITE_SIZE) {
//			memcpy(dst1 + writed, src + writed, WRITE_SIZE);
//		}
//		if (memcmp(dst1, src, BUF_SIZE - WRITE_SIZE) != 0) {
//			printf("memcpy error\n");
//		}
//		std::cout << "memcpy" << timer.elapsed() << " msec" << std::endl;
//		dst1 -= 1;
//
//		dst2 += 1;
//		timer.restart();
//		for (int writed = 0; writed < BUF_SIZE - WRITE_SIZE; writed += WRITE_SIZE) {
//			ssememcpy(dst2 + writed, src + writed, WRITE_SIZE);
//		}
//		if (memcmp(dst2, src, BUF_SIZE - WRITE_SIZE) != 0) {
//			printf("ssememcpy error\n");
//		}
//		std::cout << "ssememcpy" << timer.elapsed() << " msec" << std::endl;
//		dst2 -= 1;
//	}
//
//	delete dst2;
//	delete dst1;
//	delete src;
//	return 0;
//}

#include <UtilOpenCV.h>

////test CheckerImageRect
//int main(void)
//{
//	IplImage* img1 = cvCreateImage(cvSize(5700, 3600), IPL_DEPTH_8U, 4);
//	IplImage* img2 = cvCreateImage(cvSize(5700, 3600), IPL_DEPTH_8U, 4);
//
//	RECT rc;
//	rc.left = 0;
//	rc.top = 0;
//	rc.right = 5700;
//	rc.bottom = 3600;
//
//	UCvPixel color1;
//	UCvPixel color2;
//	color1.value = 0xffffffff;
//	color2.value = 0xff646464;
//
//	IETimer timer;
//	CheckerImageRect(img1, &color1, &color2, 5, &rc);
//	std::cout << timer.elapsed() << " msec" << std::endl;
//
//	timer.restart();
//	CheckerImageRect2(img2, &color1, &color2, 5, &rc);
//	std::cout << timer.elapsed() << " msec" << std::endl;
//
//	if (!isEqualImage(img1, img2)) {
//		std::cout << "error!" << std::endl;
//
//		cvSaveImage("test1.bmp", img1);
//		cvSaveImage("test2.bmp", img2);
//	}
//
//	cvReleaseImage(&img1);
//	cvReleaseImage(&img2);
//}


////sseを使った4ch同士のalphablend 
//int main(void)
//{
//	int x,y;
//
//	int r,g,b;
//	int r1, g1, b1;
//	int r2, g2, b2;
//
//	UCvPixel *over_pix;
//	UCvPixel *under_pix;
//	UCvPixel *dst_pix;
//	UCvPixel *over_line;
//	UCvPixel *under_line;
//	UCvPixel *dst_line;
//
//	IplImage* test1 = cvLoadImage("test1.bmp");
//	IplImage* test2 = cvLoadImage("test2.bmp");
//	IplImage* b_ch = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);
//	IplImage* g_ch = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);
//	IplImage* r_ch = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);
//	IplImage* a_ch1 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);
//	IplImage* a_ch2 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);
//	IplImage* img1 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 4);
//	IplImage* img2 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 4);
//	IplImage* img3 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 4);
//	IplImage* img4 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 4);
//	IplImage* img5 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 4);
//	IplImage* img6 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 4);
//
//	//
//	//cvSet(a_ch1, cvScalar(255));
//	//cvSet(a_ch2, cvScalar(255));
//	
//	//
//	uint8_t* line1 = GetPixelAddress(a_ch1, 0, 0);
//	for (y = 0; y < a_ch1->height; y++) {
//		uint8_t* pix = line1;
//		for (x = 0; x < a_ch1->width; x++) {
//			(*pix++) = ((double)x / a_ch1->width) * 255;
//		}
//		line1 = GetNextLineAddress(a_ch1, line1);
//	}
//
//	//
//	uint8_t* line2 = GetPixelAddress(a_ch2, 0, 0);
//	for (y = 0; y < a_ch2->height; y++) {
//		uint8_t* pix = line2;
//		for (x = 0; x < a_ch2->width; x++) {
//			(*pix++) = ((double)y / a_ch2->height) * 255;
//		}
//		line2 = GetNextLineAddress(a_ch2, line2);
//	}
//
//	cvSplit(test1, b_ch, g_ch, r_ch, NULL);
//	cvMerge(b_ch, g_ch, r_ch, a_ch1, img1);
//	cvSplit(test2, b_ch, g_ch, r_ch, NULL);
//	cvMerge(b_ch, g_ch, r_ch, a_ch2, img2);
//
//	//非SSE
//	IETimer timer;
//	over_line = GetPixelAddress32(img1, 0, 0);
//	under_line = GetPixelAddress32(img2, 0, 0);
//	dst_line = GetPixelAddress32(img3, 0, 0);
//	for(y=0; y<img1->height; y++){
//		over_pix = over_line;
//		under_pix = under_line;
//		dst_pix = dst_line;
//		for(x=0; x<img1->width; x++){
//
//			//alpha blend
//			if ((over_pix->a == 255) || (under_pix->a == 0)) {
//				dst_pix->value = over_pix->value;
//			} else if (over_pix->a == 0) {
//				dst_pix->value = under_pix->value;
//			} else if (under_pix->a == 255) {
//				uint8_t ra = ~over_pix->a;
//				dst_pix->b = (over_pix->b*over_pix->a + under_pix->b*ra)>>8;
//				dst_pix->g = (over_pix->g*over_pix->a + under_pix->g*ra)>>8;
//				dst_pix->r = (over_pix->r*over_pix->a + under_pix->r*ra)>>8;
//				dst_pix->a = 255;
//			} else {
//				uint8_t ra = ~over_pix->a;
//				uint8_t alpha = 255 - (ra*(255 - under_pix->a)>>8);
//				double inv_alpha = 1.0/alpha;
//				dst_pix->b = (over_pix->b*over_pix->a + ((under_pix->b*under_pix->a*ra)>>8))*inv_alpha;
//				dst_pix->g = (over_pix->g*over_pix->a + ((under_pix->g*under_pix->a*ra)>>8))*inv_alpha;
//				dst_pix->r = (over_pix->r*over_pix->a + ((under_pix->r*under_pix->a*ra)>>8))*inv_alpha;
//				dst_pix->a = alpha;
//			}
//
//			over_pix++;
//			under_pix++;
//			dst_pix++;
//		}
//		over_line = GetNextLineAddress32(img1, over_line);
//		under_line = GetNextLineAddress32(img2, under_line);
//		dst_line = GetNextLineAddress32(img3, dst_line);
//	}
//	std::cout << timer.elapsed() << " msec" << std::endl;
//	cvSaveImage("test3.bmp", img3);
//
//	//SSE: _mm_loadu_ps + _mm_storeu_ps
//	timer.restart();
//	{
//		over_line = GetPixelAddress32(img1, 0, 0);
//		under_line = GetPixelAddress32(img2, 0, 0);
//		dst_line = GetPixelAddress32(img4, 0, 0);
//		__m128i xmzero = _mm_setzero_si128();
//		__m128i xmff16 = _mm_set1_epi16(0xff);
//		for(y=0; y<img1->height; y++){
//			over_pix = over_line;
//			under_pix = under_line;
//			dst_pix = dst_line;
//			for(x=0; x<img1->width; x+=4){
//
//				__m128i under = _mm_castps_si128( _mm_loadu_ps((float*)under_pix) );
//				__m128i over = _mm_castps_si128( _mm_loadu_ps((float*)over_pix) );
//
//				__m128i xmo16, xmu16;
//				__m128i xmlo, xmhi;
//				__m128i xmover_alpha, xmover_ralpha, xmunder_alpha, xmunder_ralpha, xmalpha;
//				__m128 xmf_lo, xmf_hi, xmf_alpha_lo, xmf_alpha_hi;
//
//				//load lo double ward
//				xmo16 = _mm_unpacklo_epi8(over, xmzero);
//				xmu16 = _mm_unpacklo_epi8(under, xmzero);
//
//				int over_a1 = _mm_extract_epi16(xmo16, 3);
//				int over_a2 = _mm_extract_epi16(xmo16, 7);
//				int under_a1 = _mm_extract_epi16(xmu16, 3);
//				int under_a2 = _mm_extract_epi16(xmu16, 7);
//				if (((over_a1 == 255 && over_a2 == 255)) || (under_a1 == 0 && under_a2 == 0)) {
//					xmlo = xmo16;
//				} else if (under_a1 == 0 && under_a2 == 0) {
//					xmlo = xmu16;
//				} else if (under_a1 == 255 && under_a2 == 255) {
//					xmover_alpha = _mm_shufflelo_epi16(xmo16, _MM_SHUFFLE(3,3,3,3));
//					xmover_alpha = _mm_shufflehi_epi16(xmover_alpha, _MM_SHUFFLE(3,3,3,3));
//					xmover_ralpha = _mm_sub_epi16(xmff16, xmover_alpha);
//
//					xmu16 = _mm_mullo_epi16(xmu16, xmover_ralpha); // under_pix * over_ralpha
//					xmo16 = _mm_mullo_epi16(xmo16, xmover_alpha); // over_pix * over_alpha
//					xmo16 = _mm_add_epi16(xmu16, xmo16); // (under_pix * over_ralpha + over_pix * over_alpha)
//					xmlo = _mm_srli_epi16(xmo16, 8); // (under_pix * over_ralpha + over_pix * over_alpha) >> 8
//					xmlo = _mm_insert_epi16(xmlo, 255, 3); //write alpha
//					xmlo = _mm_insert_epi16(xmlo, 255, 7); //write alpha
//				} else {
//					//
//					xmover_alpha = _mm_shufflelo_epi16(xmo16, _MM_SHUFFLE(3,3,3,3));
//					xmover_alpha = _mm_shufflehi_epi16(xmover_alpha, _MM_SHUFFLE(3,3,3,3));
//					xmover_ralpha = _mm_sub_epi16(xmff16, xmover_alpha);
//					xmunder_alpha = _mm_shufflelo_epi16(xmu16, _MM_SHUFFLE(3,3,3,3));
//					xmunder_alpha = _mm_shufflehi_epi16(xmunder_alpha, _MM_SHUFFLE(3,3,3,3));
//					xmunder_ralpha = _mm_sub_epi16(xmff16, xmunder_alpha);
//					xmalpha = _mm_sub_epi16(xmff16, _mm_srli_epi16( _mm_mullo_epi16(xmover_ralpha, xmunder_ralpha), 8)); //255 - ((over_ralpha * under_ralpha) >> 8)
//
//					xmu16 = _mm_mullo_epi16( _mm_srli_epi16( _mm_mullo_epi16(xmunder_alpha, xmover_ralpha), 8), xmu16); //under_pix * ((under_alpha * over_ralpha)>>8)
//					xmo16 = _mm_mullo_epi16(xmo16, xmover_alpha); // over_pix * over_alpha
//					xmo16 = _mm_add_epi16(xmu16, xmo16); // over_pix * over_alpha + (under_pix * under_alpha * over_ralpha)>>8
//
//					//calc pixel 1
//					xmf_lo = _mm_cvtepi32_ps( _mm_unpacklo_epi16(xmo16, xmzero) );
//					xmf_alpha_lo = _mm_cvtepi32_ps( _mm_unpacklo_epi16(xmalpha, xmzero) );
//					xmf_lo = _mm_mul_ps(xmf_lo, _mm_rcp_ps(xmf_alpha_lo));
//
//					//calc pixel 2
//					xmf_hi = _mm_cvtepi32_ps( _mm_unpackhi_epi16(xmo16, xmzero) );
//					xmf_alpha_hi = _mm_cvtepi32_ps( _mm_unpackhi_epi16(xmalpha, xmzero) );
//					xmf_hi = _mm_mul_ps(xmf_hi, _mm_rcp_ps(xmf_alpha_hi));
//
//					//pack to 16bit data
//					xmlo = _mm_packs_epi32(_mm_cvtps_epi32(xmf_lo), _mm_cvtps_epi32(xmf_hi));
//					int a1 = _mm_extract_epi16(xmalpha, 3);
//					xmlo = _mm_insert_epi16(xmlo, a1, 3); //write alpha
//					int a2 = _mm_extract_epi16(xmalpha, 7);
//					xmlo = _mm_insert_epi16(xmlo, a2, 7); //write alpha
//				}
//
//				//load hi double ward
//				xmo16 = _mm_unpackhi_epi8(over, xmzero);
//				xmu16 = _mm_unpackhi_epi8(under, xmzero);
//
//				over_a1 = _mm_extract_epi16(xmo16, 3);
//				over_a2 = _mm_extract_epi16(xmo16, 7);
//				under_a1 = _mm_extract_epi16(xmu16, 3);
//				under_a2 = _mm_extract_epi16(xmu16, 7);
//				if (((over_a1 == 255 && over_a2 == 255)) || (under_a1 == 0 && under_a2 == 0)) {
//					xmhi = xmo16;
//				} else if (under_a1 == 0 && under_a2 == 0) {
//					xmhi = xmu16;
//				} else if (under_a1 == 255 && under_a2 == 255) {
//					xmover_alpha = _mm_shufflelo_epi16(xmo16, _MM_SHUFFLE(3,3,3,3));
//					xmover_alpha = _mm_shufflehi_epi16(xmover_alpha, _MM_SHUFFLE(3,3,3,3));
//					xmover_ralpha = _mm_sub_epi16(xmff16, xmover_alpha);
//
//					xmu16 = _mm_mullo_epi16(xmu16, xmover_ralpha); // under_pix * over_ralpha
//					xmo16 = _mm_mullo_epi16(xmo16, xmover_alpha); // over_pix * over_alpha
//					xmo16 = _mm_add_epi16(xmu16, xmo16); // (under_pix * over_ralpha + over_pix * over_alpha)
//					xmhi = _mm_srli_epi16(xmo16, 8); // (under_pix * over_ralpha + over_pix * over_alpha) >> 8
//					xmhi = _mm_insert_epi16(xmhi, 255, 3); //write alpha
//					xmhi = _mm_insert_epi16(xmhi, 255, 7); //write alpha
//				} else {
//					//
//					xmover_alpha = _mm_shufflelo_epi16(xmo16, _MM_SHUFFLE(3,3,3,3));
//					xmover_alpha = _mm_shufflehi_epi16(xmover_alpha, _MM_SHUFFLE(3,3,3,3));
//					xmover_ralpha = _mm_sub_epi16(xmff16, xmover_alpha);
//					xmunder_alpha = _mm_shufflelo_epi16(xmu16, _MM_SHUFFLE(3,3,3,3));
//					xmunder_alpha = _mm_shufflehi_epi16(xmunder_alpha, _MM_SHUFFLE(3,3,3,3));
//					xmunder_ralpha = _mm_sub_epi16(xmff16, xmunder_alpha);
//					xmalpha = _mm_sub_epi16(xmff16, _mm_srli_epi16( _mm_mullo_epi16(xmover_ralpha, xmunder_ralpha), 8)); //255 - ((over_ralpha * under_ralpha) >> 8)
//
//					xmu16 = _mm_mullo_epi16( _mm_srli_epi16( _mm_mullo_epi16(xmunder_alpha, xmover_ralpha), 8), xmu16); //under_pix * ((under_alpha * over_ralpha)>>8)
//					xmo16 = _mm_mullo_epi16(xmo16, xmover_alpha); // over_pix * over_alpha
//					xmo16 = _mm_add_epi16(xmu16, xmo16); // over_pix * over_alpha + (under_pix * ((under_alpha * over_ralpha)>>8)
//
//					//calc pixel1
//					xmf_lo = _mm_cvtepi32_ps( _mm_unpacklo_epi16(xmo16, xmzero) );
//					xmf_alpha_lo = _mm_cvtepi32_ps( _mm_unpacklo_epi16(xmalpha, xmzero) );
//					xmf_lo = _mm_mul_ps(xmf_lo, _mm_rcp_ps(xmf_alpha_lo));
//
//					//calc pixel 2
//					xmf_hi = _mm_cvtepi32_ps( _mm_unpackhi_epi16(xmo16, xmzero) );
//					xmf_alpha_hi = _mm_cvtepi32_ps( _mm_unpackhi_epi16(xmalpha, xmzero) );
//					xmf_hi = _mm_mul_ps(xmf_hi, _mm_rcp_ps(xmf_alpha_hi));
//
//					//pack to 16bit data
//					xmhi = _mm_packs_epi32(_mm_cvtps_epi32(xmf_lo), _mm_cvtps_epi32(xmf_hi));
//					int a3 = _mm_extract_epi16(xmalpha, 3);
//					xmhi = _mm_insert_epi16(xmhi, a3, 3); //write alpha
//					int a4 = _mm_extract_epi16(xmalpha, 7);
//					xmhi = _mm_insert_epi16(xmhi, a4, 7); //write alpha
//				}
//
//				//store to memory
//				_mm_storeu_ps((float*)dst_pix,
//					_mm_castsi128_ps( _mm_packus_epi16(xmlo, xmhi)) );
//
//				//
//				over_pix += 4;
//				under_pix += 4;
//				dst_pix += 4;
//			}
//			over_line = GetNextLineAddress32(img1, over_line);
//			under_line = GetNextLineAddress32(img2, under_line);
//			dst_line = GetNextLineAddress32(img4, dst_line);
//		}
//	}
//	std::cout << timer.elapsed() << " msec" << std::endl;
//	cvSaveImage("test4.bmp", img4);
//
//	//SSE: _mm_load_ps + _mm_store_ps			
//	timer.restart();
//	{
//		over_line = GetPixelAddress32(img1, 0, 0);
//		under_line = GetPixelAddress32(img2, 0, 0);
//		dst_line = GetPixelAddress32(img5, 0, 0);
//		__m128i xmzero = _mm_setzero_si128();
//		__m128i xmff16 = _mm_set1_epi16(0xff);
//		for(y=0; y<img1->height; y++){
//			over_pix = over_line;
//			under_pix = under_line;
//			dst_pix = dst_line;
//			for(x=0; x<img1->width; x+=4){
//
//				__m128i under = _mm_castps_si128( _mm_load_ps((float*)under_pix) );
//				__m128i over = _mm_castps_si128( _mm_load_ps((float*)over_pix) );
//
//				__m128i xmo16, xmu16;
//				__m128i xmlo, xmhi;
//				__m128i xmover_alpha, xmover_ralpha, xmunder_alpha, xmunder_ralpha, xmalpha;
//				__m128 xmf_lo, xmf_hi, xmf_alpha_lo, xmf_alpha_hi;
//
//				//load lo double ward
//				xmo16 = _mm_unpacklo_epi8(over, xmzero);
//				xmu16 = _mm_unpacklo_epi8(under, xmzero);
//
//				int over_a1 = _mm_extract_epi16(xmo16, 3);
//				int over_a2 = _mm_extract_epi16(xmo16, 7);
//				int under_a1 = _mm_extract_epi16(xmu16, 3);
//				int under_a2 = _mm_extract_epi16(xmu16, 7);
//				if (((over_a1 == 255 && over_a2 == 255)) || (under_a1 == 0 && under_a2 == 0)) {
//					xmlo = xmo16;
//				} else if (under_a1 == 0 && under_a2 == 0) {
//					xmlo = xmu16;
//				} else if (under_a1 == 255 && under_a2 == 255) {
//					xmover_alpha = _mm_shufflelo_epi16(xmo16, _MM_SHUFFLE(3,3,3,3));
//					xmover_alpha = _mm_shufflehi_epi16(xmover_alpha, _MM_SHUFFLE(3,3,3,3));
//					xmover_ralpha = _mm_sub_epi16(xmff16, xmover_alpha);
//
//					xmu16 = _mm_mullo_epi16(xmu16, xmover_ralpha); // under_pix * over_ralpha
//					xmo16 = _mm_mullo_epi16(xmo16, xmover_alpha); // over_pix * over_alpha
//					xmo16 = _mm_add_epi16(xmu16, xmo16); // (under_pix * over_ralpha + over_pix * over_alpha)
//					xmlo = _mm_srli_epi16(xmo16, 8); // (under_pix * over_ralpha + over_pix * over_alpha) >> 8
//					xmlo = _mm_insert_epi16(xmlo, 255, 3); //write alpha
//					xmlo = _mm_insert_epi16(xmlo, 255, 7); //write alpha
//				} else {
//					//
//					xmover_alpha = _mm_shufflelo_epi16(xmo16, _MM_SHUFFLE(3,3,3,3));
//					xmover_alpha = _mm_shufflehi_epi16(xmover_alpha, _MM_SHUFFLE(3,3,3,3));
//					xmover_ralpha = _mm_sub_epi16(xmff16, xmover_alpha);
//					xmunder_alpha = _mm_shufflelo_epi16(xmu16, _MM_SHUFFLE(3,3,3,3));
//					xmunder_alpha = _mm_shufflehi_epi16(xmunder_alpha, _MM_SHUFFLE(3,3,3,3));
//					xmunder_ralpha = _mm_sub_epi16(xmff16, xmunder_alpha);
//					xmalpha = _mm_sub_epi16(xmff16, _mm_srli_epi16( _mm_mullo_epi16(xmover_ralpha, xmunder_ralpha), 8)); //255 - ((over_ralpha * under_ralpha) >> 8)
//
//					xmu16 = _mm_mullo_epi16( _mm_srli_epi16( _mm_mullo_epi16(xmunder_alpha, xmover_ralpha), 8), xmu16); //under_pix * ((under_alpha * over_ralpha)>>8)
//					xmo16 = _mm_mullo_epi16(xmo16, xmover_alpha); // over_pix * over_alpha
//					xmo16 = _mm_add_epi16(xmu16, xmo16); // over_pix * over_alpha + (under_pix * under_alpha * over_ralpha)>>8
//
//					//calc pixel 1
//					xmf_lo = _mm_cvtepi32_ps( _mm_unpacklo_epi16(xmo16, xmzero) );
//					xmf_alpha_lo = _mm_cvtepi32_ps( _mm_unpacklo_epi16(xmalpha, xmzero) );
//					xmf_lo = _mm_mul_ps(xmf_lo, _mm_rcp_ps(xmf_alpha_lo));
//
//					//calc pixel 2
//					xmf_hi = _mm_cvtepi32_ps( _mm_unpackhi_epi16(xmo16, xmzero) );
//					xmf_alpha_hi = _mm_cvtepi32_ps( _mm_unpackhi_epi16(xmalpha, xmzero) );
//					xmf_hi = _mm_mul_ps(xmf_hi, _mm_rcp_ps(xmf_alpha_hi));
//
//					//pack to 16bit data
//					xmlo = _mm_packs_epi32(_mm_cvtps_epi32(xmf_lo), _mm_cvtps_epi32(xmf_hi));
//					int a1 = _mm_extract_epi16(xmalpha, 3);
//					xmlo = _mm_insert_epi16(xmlo, a1, 3); //write alpha
//					int a2 = _mm_extract_epi16(xmalpha, 7);
//					xmlo = _mm_insert_epi16(xmlo, a2, 7); //write alpha
//				}
//
//				//load hi double ward
//				xmo16 = _mm_unpackhi_epi8(over, xmzero);
//				xmu16 = _mm_unpackhi_epi8(under, xmzero);
//
//				over_a1 = _mm_extract_epi16(xmo16, 3);
//				over_a2 = _mm_extract_epi16(xmo16, 7);
//				under_a1 = _mm_extract_epi16(xmu16, 3);
//				under_a2 = _mm_extract_epi16(xmu16, 7);
//				if (((over_a1 == 255 && over_a2 == 255)) || (under_a1 == 0 && under_a2 == 0)) {
//					xmhi = xmo16;
//				} else if (under_a1 == 0 && under_a2 == 0) {
//					xmhi = xmu16;
//				} else if (under_a1 == 255 && under_a2 == 255) {
//					xmover_alpha = _mm_shufflelo_epi16(xmo16, _MM_SHUFFLE(3,3,3,3));
//					xmover_alpha = _mm_shufflehi_epi16(xmover_alpha, _MM_SHUFFLE(3,3,3,3));
//					xmover_ralpha = _mm_sub_epi16(xmff16, xmover_alpha);
//
//					xmu16 = _mm_mullo_epi16(xmu16, xmover_ralpha); // under_pix * over_ralpha
//					xmo16 = _mm_mullo_epi16(xmo16, xmover_alpha); // over_pix * over_alpha
//					xmo16 = _mm_add_epi16(xmu16, xmo16); // (under_pix * over_ralpha + over_pix * over_alpha)
//					xmhi = _mm_srli_epi16(xmo16, 8); // (under_pix * over_ralpha + over_pix * over_alpha) >> 8
//					xmhi = _mm_insert_epi16(xmhi, 255, 3); //write alpha
//					xmhi = _mm_insert_epi16(xmhi, 255, 7); //write alpha
//				} else {
//					//
//					xmover_alpha = _mm_shufflelo_epi16(xmo16, _MM_SHUFFLE(3,3,3,3));
//					xmover_alpha = _mm_shufflehi_epi16(xmover_alpha, _MM_SHUFFLE(3,3,3,3));
//					xmover_ralpha = _mm_sub_epi16(xmff16, xmover_alpha);
//					xmunder_alpha = _mm_shufflelo_epi16(xmu16, _MM_SHUFFLE(3,3,3,3));
//					xmunder_alpha = _mm_shufflehi_epi16(xmunder_alpha, _MM_SHUFFLE(3,3,3,3));
//					xmunder_ralpha = _mm_sub_epi16(xmff16, xmunder_alpha);
//					xmalpha = _mm_sub_epi16(xmff16, _mm_srli_epi16( _mm_mullo_epi16(xmover_ralpha, xmunder_ralpha), 8)); //255 - ((over_ralpha * under_ralpha) >> 8)
//
//					xmu16 = _mm_mullo_epi16( _mm_srli_epi16( _mm_mullo_epi16(xmunder_alpha, xmover_ralpha), 8), xmu16); //under_pix * ((under_alpha * over_ralpha)>>8)
//					xmo16 = _mm_mullo_epi16(xmo16, xmover_alpha); // over_pix * over_alpha
//					xmo16 = _mm_add_epi16(xmu16, xmo16); // over_pix * over_alpha + (under_pix * ((under_alpha * over_ralpha)>>8)
//
//					//calc pixel1
//					xmf_lo = _mm_cvtepi32_ps( _mm_unpacklo_epi16(xmo16, xmzero) );
//					xmf_alpha_lo = _mm_cvtepi32_ps( _mm_unpacklo_epi16(xmalpha, xmzero) );
//					xmf_lo = _mm_mul_ps(xmf_lo, _mm_rcp_ps(xmf_alpha_lo));
//
//					//calc pixel 2
//					xmf_hi = _mm_cvtepi32_ps( _mm_unpackhi_epi16(xmo16, xmzero) );
//					xmf_alpha_hi = _mm_cvtepi32_ps( _mm_unpackhi_epi16(xmalpha, xmzero) );
//					xmf_hi = _mm_mul_ps(xmf_hi, _mm_rcp_ps(xmf_alpha_hi));
//
//					//pack to 16bit data
//					xmhi = _mm_packs_epi32(_mm_cvtps_epi32(xmf_lo), _mm_cvtps_epi32(xmf_hi));
//					int a3 = _mm_extract_epi16(xmalpha, 3);
//					xmhi = _mm_insert_epi16(xmhi, a3, 3); //write alpha
//					int a4 = _mm_extract_epi16(xmalpha, 7);
//					xmhi = _mm_insert_epi16(xmhi, a4, 7); //write alpha
//				}
//
//				//store to memory
//				_mm_store_ps((float*)dst_pix,
//					_mm_castsi128_ps( _mm_packus_epi16(xmlo, xmhi)) );
//
//				//
//				over_pix += 4;
//				under_pix += 4;
//				dst_pix += 4;
//			}
//			over_line = GetNextLineAddress32(img1, over_line);
//			under_line = GetNextLineAddress32(img2, under_line);
//			dst_line = GetNextLineAddress32(img5	, dst_line);
//		}
//	}
//	std::cout << timer.elapsed() << " msec" << std::endl;
//	cvSaveImage("test5.bmp", img5);
//
//	//SSE: _mm_load_ps + _mm_stream_si128
//	timer.restart();
//	{
//		over_line = GetPixelAddress32(img1, 0, 0);
//		under_line = GetPixelAddress32(img2, 0, 0);
//		dst_line = GetPixelAddress32(img6, 0, 0);
//		__m128i xmzero = _mm_setzero_si128();
//		__m128i xmff16 = _mm_set1_epi16(0xff);
//		for(y=0; y<img1->height; y++){
//			over_pix = over_line;
//			under_pix = under_line;
//			dst_pix = dst_line;
//			for(x=0; x<img1->width; x+=4){
//
//				__m128i under = _mm_castps_si128( _mm_load_ps((float*)under_pix) );
//				__m128i over = _mm_castps_si128( _mm_load_ps((float*)over_pix) );
//
//				__m128i xmo16, xmu16;
//				__m128i xmlo, xmhi;
//				__m128i xmover_alpha, xmover_ralpha, xmunder_alpha, xmunder_ralpha, xmalpha;
//				__m128 xmf_lo, xmf_hi, xmf_alpha_lo, xmf_alpha_hi;
//
//				//load lo double ward
//				xmo16 = _mm_unpacklo_epi8(over, xmzero);
//				xmu16 = _mm_unpacklo_epi8(under, xmzero);
//
//				int over_a1 = _mm_extract_epi16(xmo16, 3);
//				int over_a2 = _mm_extract_epi16(xmo16, 7);
//				int under_a1 = _mm_extract_epi16(xmu16, 3);
//				int under_a2 = _mm_extract_epi16(xmu16, 7);
//				if (((over_a1 == 255 && over_a2 == 255)) || (under_a1 == 0 && under_a2 == 0)) {
//					xmlo = xmo16;
//				} else if (under_a1 == 0 && under_a2 == 0) {
//					xmlo = xmu16;
//				} else if (under_a1 == 255 && under_a2 == 255) {
//					xmover_alpha = _mm_shufflelo_epi16(xmo16, _MM_SHUFFLE(3,3,3,3));
//					xmover_alpha = _mm_shufflehi_epi16(xmover_alpha, _MM_SHUFFLE(3,3,3,3));
//					xmover_ralpha = _mm_sub_epi16(xmff16, xmover_alpha);
//
//					xmu16 = _mm_mullo_epi16(xmu16, xmover_ralpha); // under_pix * over_ralpha
//					xmo16 = _mm_mullo_epi16(xmo16, xmover_alpha); // over_pix * over_alpha
//					xmo16 = _mm_add_epi16(xmu16, xmo16); // (under_pix * over_ralpha + over_pix * over_alpha)
//					xmlo = _mm_srli_epi16(xmo16, 8); // (under_pix * over_ralpha + over_pix * over_alpha) >> 8
//					xmlo = _mm_insert_epi16(xmlo, 255, 3); //write alpha
//					xmlo = _mm_insert_epi16(xmlo, 255, 7); //write alpha
//				} else {
//					//
//					xmover_alpha = _mm_shufflelo_epi16(xmo16, _MM_SHUFFLE(3,3,3,3));
//					xmover_alpha = _mm_shufflehi_epi16(xmover_alpha, _MM_SHUFFLE(3,3,3,3));
//					xmover_ralpha = _mm_sub_epi16(xmff16, xmover_alpha);
//					xmunder_alpha = _mm_shufflelo_epi16(xmu16, _MM_SHUFFLE(3,3,3,3));
//					xmunder_alpha = _mm_shufflehi_epi16(xmunder_alpha, _MM_SHUFFLE(3,3,3,3));
//					xmunder_ralpha = _mm_sub_epi16(xmff16, xmunder_alpha);
//					xmalpha = _mm_sub_epi16(xmff16, _mm_srli_epi16( _mm_mullo_epi16(xmover_ralpha, xmunder_ralpha), 8)); //255 - ((over_ralpha * under_ralpha) >> 8)
//
//					xmu16 = _mm_mullo_epi16( _mm_srli_epi16( _mm_mullo_epi16(xmunder_alpha, xmover_ralpha), 8), xmu16); //under_pix * ((under_alpha * over_ralpha)>>8)
//					xmo16 = _mm_mullo_epi16(xmo16, xmover_alpha); // over_pix * over_alpha
//					xmo16 = _mm_add_epi16(xmu16, xmo16); // over_pix * over_alpha + (under_pix * under_alpha * over_ralpha)>>8
//
//					//calc pixel 1
//					xmf_lo = _mm_cvtepi32_ps( _mm_unpacklo_epi16(xmo16, xmzero) );
//					xmf_alpha_lo = _mm_cvtepi32_ps( _mm_unpacklo_epi16(xmalpha, xmzero) );
//					xmf_lo = _mm_mul_ps(xmf_lo, _mm_rcp_ps(xmf_alpha_lo));
//
//					//calc pixel 2
//					xmf_hi = _mm_cvtepi32_ps( _mm_unpackhi_epi16(xmo16, xmzero) );
//					xmf_alpha_hi = _mm_cvtepi32_ps( _mm_unpackhi_epi16(xmalpha, xmzero) );
//					xmf_hi = _mm_mul_ps(xmf_hi, _mm_rcp_ps(xmf_alpha_hi));
//
//					//pack to 16bit data
//					xmlo = _mm_packs_epi32(_mm_cvtps_epi32(xmf_lo), _mm_cvtps_epi32(xmf_hi));
//
//					//write alpha
//					int a1 = _mm_extract_epi16(xmalpha, 3);
//					xmlo = _mm_insert_epi16(xmlo, a1, 3); //write alpha
//					int a2 = _mm_extract_epi16(xmalpha, 7);
//					xmlo = _mm_insert_epi16(xmlo, a2, 7); //write alpha
//				}
//
//				//load hi double ward
//				xmo16 = _mm_unpackhi_epi8(over, xmzero);
//				xmu16 = _mm_unpackhi_epi8(under, xmzero);
//
//				over_a1 = _mm_extract_epi16(xmo16, 3);
//				over_a2 = _mm_extract_epi16(xmo16, 7);
//				under_a1 = _mm_extract_epi16(xmu16, 3);
//				under_a2 = _mm_extract_epi16(xmu16, 7);
//				if (((over_a1 == 255 && over_a2 == 255)) || (under_a1 == 0 && under_a2 == 0)) {
//					xmhi = xmo16;
//				} else if (under_a1 == 0 && under_a2 == 0) {
//					xmhi = xmu16;
//				} else if (under_a1 == 255 && under_a2 == 255) {
//					xmover_alpha = _mm_shufflelo_epi16(xmo16, _MM_SHUFFLE(3,3,3,3));
//					xmover_alpha = _mm_shufflehi_epi16(xmover_alpha, _MM_SHUFFLE(3,3,3,3));
//					xmover_ralpha = _mm_sub_epi16(xmff16, xmover_alpha);
//
//					xmu16 = _mm_mullo_epi16(xmu16, xmover_ralpha); // under_pix * over_ralpha
//					xmo16 = _mm_mullo_epi16(xmo16, xmover_alpha); // over_pix * over_alpha
//					xmo16 = _mm_add_epi16(xmu16, xmo16); // (under_pix * over_ralpha + over_pix * over_alpha)
//					xmhi = _mm_srli_epi16(xmo16, 8); // (under_pix * over_ralpha + over_pix * over_alpha) >> 8
//					xmhi = _mm_insert_epi16(xmhi, 255, 3); //write alpha
//					xmhi = _mm_insert_epi16(xmhi, 255, 7); //write alpha
//				} else {
//					//
//					xmover_alpha = _mm_shufflelo_epi16(xmo16, _MM_SHUFFLE(3,3,3,3));
//					xmover_alpha = _mm_shufflehi_epi16(xmover_alpha, _MM_SHUFFLE(3,3,3,3));
//					xmover_ralpha = _mm_sub_epi16(xmff16, xmover_alpha);
//					xmunder_alpha = _mm_shufflelo_epi16(xmu16, _MM_SHUFFLE(3,3,3,3));
//					xmunder_alpha = _mm_shufflehi_epi16(xmunder_alpha, _MM_SHUFFLE(3,3,3,3));
//					xmunder_ralpha = _mm_sub_epi16(xmff16, xmunder_alpha);
//					xmalpha = _mm_sub_epi16(xmff16, _mm_srli_epi16( _mm_mullo_epi16(xmover_ralpha, xmunder_ralpha), 8)); //255 - ((over_ralpha * under_ralpha) >> 8)
//
//					xmu16 = _mm_mullo_epi16( _mm_srli_epi16( _mm_mullo_epi16(xmunder_alpha, xmover_ralpha), 8), xmu16); //under_pix * ((under_alpha * over_ralpha)>>8)
//					xmo16 = _mm_mullo_epi16(xmo16, xmover_alpha); // over_pix * over_alpha
//					xmo16 = _mm_add_epi16(xmu16, xmo16); // over_pix * over_alpha + (under_pix * ((under_alpha * over_ralpha)>>8)
//
//					//calc pixel1
//					xmf_lo = _mm_cvtepi32_ps( _mm_unpacklo_epi16(xmo16, xmzero) );
//					xmf_alpha_lo = _mm_cvtepi32_ps( _mm_unpacklo_epi16(xmalpha, xmzero) );
//					xmf_lo = _mm_mul_ps(xmf_lo, _mm_rcp_ps(xmf_alpha_lo));
//
//					//calc pixel 2
//					xmf_hi = _mm_cvtepi32_ps( _mm_unpackhi_epi16(xmo16, xmzero) );
//					xmf_alpha_hi = _mm_cvtepi32_ps( _mm_unpackhi_epi16(xmalpha, xmzero) );
//					xmf_hi = _mm_mul_ps(xmf_hi, _mm_rcp_ps(xmf_alpha_hi));
//
//					//pack to 16bit data
//					xmhi = _mm_packs_epi32(_mm_cvtps_epi32(xmf_lo), _mm_cvtps_epi32(xmf_hi));
//					int a3 = _mm_extract_epi16(xmalpha, 3);
//					xmhi = _mm_insert_epi16(xmhi, a3, 3); //write alpha
//					int a4 = _mm_extract_epi16(xmalpha, 7);
//					xmhi = _mm_insert_epi16(xmhi, a4, 7); //write alpha
//				}
//
//				//store to memory
//				_mm_stream_si128((__m128i*)dst_pix, _mm_packus_epi16(xmlo, xmhi));
//
//				//
//				over_pix += 4;
//				under_pix += 4;
//				dst_pix += 4;
//			}
//			over_line = GetNextLineAddress32(img1, over_line);
//			under_line = GetNextLineAddress32(img2, under_line);
//			dst_line = GetNextLineAddress32(img6, dst_line);
//		}
//	}
//	std::cout << timer.elapsed() << " msec" << std::endl;
//	cvSaveImage("test6.bmp", img6);
//
//
//	cvReleaseImage(&b_ch);
//	cvReleaseImage(&g_ch);
//	cvReleaseImage(&r_ch);
//	cvReleaseImage(&a_ch1);
//	cvReleaseImage(&a_ch2);
//	cvReleaseImage(&img1);
//	cvReleaseImage(&img2);
//	cvReleaseImage(&img3);
//	cvReleaseImage(&img4);
//	cvReleaseImage(&img5);
//	cvReleaseImage(&img6);
//}

//#define ALPHABLEND_SSE(dst, under, over)										\
//{																				\
//	__m128i __xmzero = _mm_setzero_si128();										\
//	__m128i __xmff16 = _mm_set1_epi16(0xff);									\
//																				\
//	__m128i __xmo16, __xmu16;													\
//	__m128i __xmlo, __xmhi;														\
//	__m128i __xmover_alpha, __xmover_ralpha, __xmunder_alpha, __xmunder_ralpha, __xmalpha;\
//	__m128 __xmf_lo, __xmf_hi, __xmf_alpha_lo, __xmf_alpha_hi;					\
//																				\
//	/*load lo double ward */													\
//	__xmo16 = _mm_unpacklo_epi8(over, __xmzero);								\
//	__xmu16 = _mm_unpacklo_epi8(under, __xmzero);								\
//																				\
//	int over_a1 = _mm_extract_epi16(__xmo16, 3);								\
//	int over_a2 = _mm_extract_epi16(__xmo16, 7);								\
//	if ((over_a1 == 255 && over_a2 == 255)) {									\
//		__xmlo = __xmo16;														\
//	} else {																	\
//		int under_a1 = _mm_extract_epi16(__xmu16, 3);							\
//		int under_a2 = _mm_extract_epi16(__xmu16, 7);							\
//		if (under_a1 == 0 && under_a2 == 0) {									\
//			__xmlo = __xmo16;													\
//		} else if (under_a1 == 0 && under_a2 == 0) {							\
//			__xmlo = __xmu16;													\
//		} else if (under_a1 == 255 && under_a2 == 255) {						\
//			__xmover_alpha = _mm_shufflelo_epi16(__xmo16, _MM_SHUFFLE(3,3,3,3));\
//			__xmover_alpha = _mm_shufflehi_epi16(__xmover_alpha, _MM_SHUFFLE(3,3,3,3));\
//			__xmover_ralpha = _mm_sub_epi16(__xmff16, __xmover_alpha);			\
//																				\
//			__xmu16 = _mm_mullo_epi16(__xmu16, __xmover_ralpha); /* under_pix * over_ralpha */\
//			__xmo16 = _mm_mullo_epi16(__xmo16, __xmover_alpha); /* over_pix * over_alpha */\
//			__xmo16 = _mm_add_epi16(__xmu16, __xmo16); /* (under_pix * over_ralpha + over_pix * over_alpha) */\
//			__xmlo = _mm_srli_epi16(__xmo16, 8); /* (under_pix * over_ralpha + over_pix * over_alpha) >> 8 */\
//			__xmlo = _mm_insert_epi16(__xmlo, 255, 3); /* write alpha */		\
//			__xmlo = _mm_insert_epi16(__xmlo, 255, 7); /* write alpha */		\
//		} else {																\
//			__xmover_alpha = _mm_shufflelo_epi16(__xmo16, _MM_SHUFFLE(3,3,3,3));\
//			__xmover_alpha = _mm_shufflehi_epi16(__xmover_alpha, _MM_SHUFFLE(3,3,3,3));\
//			__xmover_ralpha = _mm_sub_epi16(__xmff16, __xmover_alpha);			\
//			__xmunder_alpha = _mm_shufflelo_epi16(__xmu16, _MM_SHUFFLE(3,3,3,3));\
//			__xmunder_alpha = _mm_shufflehi_epi16(__xmunder_alpha, _MM_SHUFFLE(3,3,3,3));\
//			__xmunder_ralpha = _mm_sub_epi16(__xmff16, __xmunder_alpha);		\
//			__xmalpha = _mm_sub_epi16(__xmff16, _mm_srli_epi16( _mm_mullo_epi16(__xmover_ralpha, __xmunder_ralpha), 8)); /* 255 - ((over_ralpha * under_ralpha) >> 8) */\
//																				\
//			__xmu16 = _mm_mullo_epi16( _mm_srli_epi16( _mm_mullo_epi16(__xmunder_alpha, __xmover_ralpha), 8), __xmu16); /* under_pix * ((under_alpha * over_ralpha)>>8) */\
//			__xmo16 = _mm_mullo_epi16(__xmo16, __xmover_alpha); /* over_pix * over_alpha */\
//			__xmo16 = _mm_add_epi16(__xmu16, __xmo16); /* over_pix * over_alpha + (under_pix * under_alpha * over_ralpha)>>8 */\
//																				\
//			/* calc pixel 1 */													\
//			__xmf_lo = _mm_cvtepi32_ps( _mm_unpacklo_epi16(__xmo16, __xmzero) );\
//			__xmf_alpha_lo = _mm_cvtepi32_ps( _mm_unpacklo_epi16(__xmalpha, __xmzero) );\
//			__xmf_lo = _mm_mul_ps(__xmf_lo, _mm_rcp_ps(__xmf_alpha_lo));		\
//																				\
//			/* calc pixel 2 */													\
//			__xmf_hi = _mm_cvtepi32_ps( _mm_unpackhi_epi16(__xmo16, __xmzero) );\
//			__xmf_alpha_hi = _mm_cvtepi32_ps( _mm_unpackhi_epi16(__xmalpha, __xmzero) );\
//			__xmf_hi = _mm_mul_ps(__xmf_hi, _mm_rcp_ps(__xmf_alpha_hi));		\
//																				\
//			/* pack to 16bit data */											\
//			__xmlo = _mm_packs_epi32(_mm_cvtps_epi32(__xmf_lo), _mm_cvtps_epi32(__xmf_hi));\
//			int a1 = _mm_extract_epi16(__xmalpha, 3);							\
//			__xmlo = _mm_insert_epi16(__xmlo, a1, 3); /* write alpha */			\
//			int a2 = _mm_extract_epi16(__xmalpha, 7);							\
//			__xmlo = _mm_insert_epi16(__xmlo, a2, 7); /* write alpha */			\
//		}																		\
//	}																			\
//																				\
//	/* load hi double ward */													\
//	__xmo16 = _mm_unpackhi_epi8(over, __xmzero);								\
//	__xmu16 = _mm_unpackhi_epi8(under, __xmzero);								\
//																				\
//	over_a1 = _mm_extract_epi16(__xmo16, 3);									\
//	over_a2 = _mm_extract_epi16(__xmo16, 7);									\
//	if ((over_a1 == 255 && over_a2 == 255)) {									\
//		__xmhi = __xmo16;														\
//	} else {																	\
//		int under_a1 = _mm_extract_epi16(__xmu16, 3);							\
//		int under_a2 = _mm_extract_epi16(__xmu16, 7);							\
//		if (under_a1 == 0 && under_a2 == 0) {									\
//			__xmhi = __xmo16;													\
//		} else if (under_a1 == 0 && under_a2 == 0) {							\
//			__xmhi = __xmu16;													\
//		} else if (under_a1 == 255 && under_a2 == 255) {						\
//			__xmover_alpha = _mm_shufflelo_epi16(__xmo16, _MM_SHUFFLE(3,3,3,3));\
//			__xmover_alpha = _mm_shufflehi_epi16(__xmover_alpha, _MM_SHUFFLE(3,3,3,3));\
//			__xmover_ralpha = _mm_sub_epi16(__xmff16, __xmover_alpha);			\
//																				\
//			__xmu16 = _mm_mullo_epi16(__xmu16, __xmover_ralpha); /* under_pix * over_ralpha */\
//			__xmo16 = _mm_mullo_epi16(__xmo16, __xmover_alpha); /* over_pix * over_alpha */\
//			__xmo16 = _mm_add_epi16(__xmu16, __xmo16); /* (under_pix * over_ralpha + over_pix * over_alpha) */\
//			__xmhi = _mm_srli_epi16(__xmo16, 8); /* (under_pix * over_ralpha + over_pix * over_alpha) >> 8 */\
//			__xmhi = _mm_insert_epi16(__xmhi, 255, 3); /* write alpha */		\
//			__xmhi = _mm_insert_epi16(__xmhi, 255, 7); /* write alpha */		\
//		} else {																\
//			__xmover_alpha = _mm_shufflelo_epi16(__xmo16, _MM_SHUFFLE(3,3,3,3));\
//			__xmover_alpha = _mm_shufflehi_epi16(__xmover_alpha, _MM_SHUFFLE(3,3,3,3));\
//			__xmover_ralpha = _mm_sub_epi16(__xmff16, __xmover_alpha);			\
//			__xmunder_alpha = _mm_shufflelo_epi16(__xmu16, _MM_SHUFFLE(3,3,3,3));\
//			__xmunder_alpha = _mm_shufflehi_epi16(__xmunder_alpha, _MM_SHUFFLE(3,3,3,3));\
//			__xmunder_ralpha = _mm_sub_epi16(__xmff16, __xmunder_alpha);		\
//			__xmalpha = _mm_sub_epi16(__xmff16, _mm_srli_epi16( _mm_mullo_epi16(__xmover_ralpha, __xmunder_ralpha), 8)); /* 255 - ((over_ralpha * under_ralpha) >> 8) */\
//																				\
//			__xmu16 = _mm_mullo_epi16( _mm_srli_epi16( _mm_mullo_epi16(__xmunder_alpha, __xmover_ralpha), 8), __xmu16); /* under_pix * ((under_alpha * over_ralpha)>>8) */\
//			__xmo16 = _mm_mullo_epi16(__xmo16, __xmover_alpha); /* over_pix * over_alpha */\
//			__xmo16 = _mm_add_epi16(__xmu16, __xmo16); /* over_pix * over_alpha + (under_pix * ((under_alpha * over_ralpha)>>8) */\
//																				\
//			/* calc pixel1 */													\
//			__xmf_lo = _mm_cvtepi32_ps( _mm_unpacklo_epi16(__xmo16, __xmzero) );\
//			__xmf_alpha_lo = _mm_cvtepi32_ps( _mm_unpacklo_epi16(__xmalpha, __xmzero) );\
//			__xmf_lo = _mm_mul_ps(__xmf_lo, _mm_rcp_ps(__xmf_alpha_lo));		\
//																				\
//			/* calc pixel 2 */													\
//			__xmf_hi = _mm_cvtepi32_ps( _mm_unpackhi_epi16(__xmo16, __xmzero) );\
//			__xmf_alpha_hi = _mm_cvtepi32_ps( _mm_unpackhi_epi16(__xmalpha, __xmzero) );\
//			__xmf_hi = _mm_mul_ps(__xmf_hi, _mm_rcp_ps(__xmf_alpha_hi));		\
//																				\
//			/* pack to 16bit data */											\
//			__xmhi = _mm_packs_epi32(_mm_cvtps_epi32(__xmf_lo), _mm_cvtps_epi32(__xmf_hi));\
//			int a3 = _mm_extract_epi16(__xmalpha, 3);							\
//			__xmhi = _mm_insert_epi16(__xmhi, a3, 3); /* write alpha */			\
//			int a4 = _mm_extract_epi16(__xmalpha, 7);							\
//			__xmhi = _mm_insert_epi16(__xmhi, a4, 7); /* write alpha */			\
//		}																		\
//	}																			\
//	dst =  _mm_packus_epi16(__xmlo, __xmhi);									\
//}
//
//inline void BltLineAlphaBlend(
//	UCvPixel* dst,
//	const UCvPixel* under,
//	const UCvPixel* over,
//	int blt_width)
//{
//	int x;
//	for(x = 0; x < blt_width; x++){
//		//alpha blend
//		if ((over->a == 255) || (under->a == 0)) {
//			dst->value = over->value;
//		} else if (over->a == 0) {
//			dst->value = under->value;
//		} else if (under->a == 255) {
//			uint8_t ra = ~over->a;
//			dst->b = (over->b*over->a + under->b*ra)>>8;
//			dst->g = (over->g*over->a + under->g*ra)>>8;
//			dst->r = (over->r*over->a + under->r*ra)>>8;
//			dst->a = 255;
//		} else {
//			uint8_t ra = ~over->a;
//			uint8_t alpha = 255 - (ra*(255 - under->a)>>8);
//			double inv_alpha = 1.0/alpha;
//			dst->b = (over->b*over->a + ((under->b*under->a*ra)>>8))*inv_alpha;
//			dst->g = (over->g*over->a + ((under->g*under->a*ra)>>8))*inv_alpha;
//			dst->r = (over->r*over->a + ((under->r*under->a*ra)>>8))*inv_alpha;
//			dst->a = alpha;
//		}
//
//		over++;
//		under++;
//		dst++;
//	}
//}
//
//inline void BltLineAlphaBlend_SSE_unaligned(
//	UCvPixel* dst,
//	const UCvPixel* under,
//	const UCvPixel* over,
//	int blt_width)
//{
//	int nloop = blt_width >> 2; //blt_width / 4
//
//	int x;
//	for(x = 0; x < nloop; x++){
//		__m128i xmunder = _mm_loadu_si128((const __m128i*)under);
//		__m128i xmover = _mm_loadu_si128((const __m128i*)over);
//
//		__m128i xmdst;
//		ALPHABLEND_SSE(xmdst, xmunder, xmover);
//		_mm_storeu_si128((__m128i*)dst, xmdst);
//
//		over += 4;
//		under += 4;
//		dst += 4;
//	}
//
//	int bn = blt_width & 0x3; //blt_width % 4
//	BltLineAlphaBlend(
//		dst,
//		under,
//		over,
//		bn);
//}
//
//inline void SubBltLineAlphaBlend_SSE(
//	UCvPixel* dst,
//	const UCvPixel* under,
//	const UCvPixel* over,
//	int blt_width)
//{
//	assert (DIFF_16BYTE_ALIGNMENT(under) == DIFF_16BYTE_ALIGNMENT(over));
//	assert (IS_16BYTE_ALIGNMENT(dst) == IS_16BYTE_ALIGNMENT(under));
//
//	int nloop = blt_width >> 2; //blt_width / 4
//
//	int x;
//	for(x = 0; x < nloop; x++){
//		__m128i xmunder = _mm_load_si128((const __m128i*)under);
//		__m128i xmover = _mm_load_si128((const __m128i*)over);
//
//		__m128i xmdst;
//		ALPHABLEND_SSE(xmdst, xmunder, xmover);
//		_mm_store_si128((__m128i*)dst, xmdst);
//
//		over += 4;
//		under += 4;
//		dst += 4;
//	}
//
//	int bn = blt_width & 0x3; //blt_width % 4
//	BltLineAlphaBlend(
//		dst,
//		under,
//		over,
//		bn);
//	return;
//}
//
//template <int _SHIFT>
//void ShiftSubBltLineAlphaBlend_SSE(
//	UCvPixel* dst,
//	const UCvPixel* under,
//	const UCvPixel* over,
//	int blt_width)
//{
//	UCvPixel* _dst = dst;
//	const UCvPixel* _over = over;
//	assert(DIFF_16BYTE_ALIGNMENT(under) == DIFF_16BYTE_ALIGNMENT(over));
//	assert(IS_16BYTE_ALIGNMENT(under) && IS_16BYTE_ALIGNMENT(over));
//
//	int nshift_pix = _SHIFT >> 2;
//	int nloop = (blt_width >> 2) - 1; //(blt_width / 4) - 1
//
//	__m128i xmunder, xmunder0, xmunder1, xmover, xmover0, xmover1;
//	xmunder0 = _mm_load_si128((const __m128i*)under);
//	xmover0 = _mm_load_si128((const __m128i*)over);
//
//	//_SHIFT byte分処理
//	BltLineAlphaBlend(dst, under, over, nshift_pix);
//	dst = dst + nshift_pix;
//	assert(IS_16BYTE_ALIGNMENT(dst));
//
//	xmunder0 = _mm_srli_si128(xmunder0, _SHIFT); //xmunder0 >> _SHIFT
//	xmover0 = _mm_srli_si128(xmover0, _SHIFT); //xmover0 >> _SHIFT
//
//	int x;
//	for(x = 0; x < nloop; x++){
//		xmunder = _mm_load_si128((const __m128i*)(under + 4));
//		xmover = _mm_load_si128((const __m128i*)(over + 4));
//		xmunder1 = _mm_slli_si128(xmunder, 16 - _SHIFT); //xmunder << (16 - _SHIFT)
//		xmover1 = _mm_slli_si128(xmover, 16 - _SHIFT); //xmmover << (16 - _SHIFT)
//		xmunder1 = _mm_or_si128(xmunder1, xmunder0);
//		xmover1 = _mm_or_si128(xmover1, xmover0);
//		xmunder0 = _mm_srli_si128(xmunder, _SHIFT); //xmunder >> _SHIFT
//		xmover0 = _mm_srli_si128(xmover, _SHIFT); //xmover >> _SHIFT
//
//		__m128i xmdst;
//		ALPHABLEND_SSE(xmdst, xmunder1, xmover1);
//		_mm_store_si128((__m128i*)dst, xmdst);
//
//		over += 4;
//		under += 4;
//		dst += 4;
//	}
//
//	int bn = blt_width - (nloop << 2) - nshift_pix;
//	BltLineAlphaBlend(
//		dst,
//		under + nshift_pix,
//		over + nshift_pix,
//		bn);
//
//	return;
//}
//
//inline void BltLineAlphaBlend_SSE(
//	UCvPixel* dst,
//	const UCvPixel* under,
//	const UCvPixel* over,
//	int blt_width)
//{
//	assert (DIFF_16BYTE_ALIGNMENT(under) == DIFF_16BYTE_ALIGNMENT(over));
//
//	if (IS_16BYTE_ALIGNMENT(dst) && IS_16BYTE_ALIGNMENT(under)) {
//		SubBltLineAlphaBlend_SSE(dst, under, over, blt_width);
//		return;
//	}
//
//	int dif_d = DIFF_16BYTE_ALIGNMENT(dst);
//	int dif_s = DIFF_16BYTE_ALIGNMENT(under);
//	assert((dif_d & 0x3) == 0); //1pixel、4byteなので4の倍数になるはず
//	assert((dif_s & 0x3) == 0); //
//
//	if (dif_d == dif_s) {
//		int dif_npix = dif_s >> 2; //アライメントがあっていないピクセル数を得る
//		BltLineAlphaBlend(
//			dst,
//			under,
//			over,
//			dif_npix);
//		SubBltLineAlphaBlend_SSE(
//			dst + dif_npix,
//			under + dif_npix,
//			over + dif_npix,
//			blt_width - dif_npix);
//	} else {
//		//読み込み元のアライメントを合わせる。
//		int dif_npix = dif_s >> 2;
//		BltLineAlphaBlend(
//			dst,
//			under,
//			over,
//			dif_npix);
//		//
//		dst += dif_npix;
//		under += dif_npix;
//		over += dif_npix;
//		int nshift_byte = DIFF_16BYTE_ALIGNMENT(dst);
//		assert((nshift_byte & 0x3) == 0);
//		switch (nshift_byte) {
//		case 4: //1pixelずれ
//			ShiftSubBltLineAlphaBlend_SSE<4>(dst, under, over, blt_width);
//			break;
//		case 8: //2pixelずれ
//			ShiftSubBltLineAlphaBlend_SSE<8>(dst, under, over, blt_width);
//			break;
//		case 12: //3pixelずれ
//			ShiftSubBltLineAlphaBlend_SSE<12>(dst, under, over, blt_width);
//			break;
//		default:
//			assert(0);
//		}
//	}
//}
//
//inline void SubBltLineAlphaBlend_SSE_nocache(
//	UCvPixel* dst,
//	const UCvPixel* under,
//	const UCvPixel* over,
//	int blt_width)
//{
//	assert (DIFF_16BYTE_ALIGNMENT(under) == DIFF_16BYTE_ALIGNMENT(over));
//	assert (IS_16BYTE_ALIGNMENT(dst) == IS_16BYTE_ALIGNMENT(under));
//
//	int nloop = blt_width >> 2; //blt_width / 4
//
//	int x;
//	for(x = 0; x < nloop; x++){
//		__m128i xmunder = _mm_load_si128((const __m128i*)under);
//		__m128i xmover = _mm_load_si128((const __m128i*)over);
//
//		__m128i xmdst;
//		ALPHABLEND_SSE(xmdst, xmunder, xmover);
//		_mm_stream_si128((__m128i*)dst, xmdst);
//
//		over += 4;
//		under += 4;
//		dst += 4;
//	}
//
//	int bn = blt_width & 0x3; //blt_width % 4
//	BltLineAlphaBlend(
//		dst,
//		under,
//		over,
//		bn);
//	return;
//}
//
//template <int _SHIFT>
//void ShiftSubBltLineAlphaBlend_SSE_nocache(
//	UCvPixel* dst,
//	const UCvPixel* under,
//	const UCvPixel* over,
//	int blt_width)
//{
//	assert(DIFF_16BYTE_ALIGNMENT(under) == DIFF_16BYTE_ALIGNMENT(over));
//	assert(IS_16BYTE_ALIGNMENT(under) && IS_16BYTE_ALIGNMENT(over));
//
//	int nshift_pix = _SHIFT >> 2;
//	int nloop = (blt_width >> 2) - 1; //(blt_width / 4) - 1
//
//	__m128i xmunder, xmunder0, xmunder1, xmover, xmover0, xmover1;
//	xmunder0 = _mm_load_si128((const __m128i*)under);
//	xmover0 = _mm_load_si128((const __m128i*)over);
//
//	//_SHIFT byte分処理
//	BltLineAlphaBlend(dst, under, over, nshift_pix);
//	dst = dst + nshift_pix;
//	assert(IS_16BYTE_ALIGNMENT(dst));
//
//	xmunder0 = _mm_srli_si128(xmunder0, _SHIFT); //xmunder0 >> _SHIFT
//	xmover0 = _mm_srli_si128(xmover0, _SHIFT); //xmover0 >> _SHIFT
//
//	int x;
//	for(x = 0; x < nloop; x++){
//		xmunder = _mm_load_si128((const __m128i*)(under + 4));
//		xmover = _mm_load_si128((const __m128i*)(over + 4));
//		xmunder1 = _mm_slli_si128(xmunder, 16 - _SHIFT); //xmunder << (16 - _SHIFT)
//		xmover1 = _mm_slli_si128(xmover, 16 - _SHIFT); //xmmover << (16 - _SHIFT)
//		xmunder1 = _mm_or_si128(xmunder1, xmunder0);
//		xmover1 = _mm_or_si128(xmover1, xmover0);
//		xmunder0 = _mm_srli_si128(xmunder, _SHIFT); //xmunder >> _SHIFT
//		xmover0 = _mm_srli_si128(xmover, _SHIFT); //xmover >> _SHIFT
//
//		__m128i xmdst;
//		ALPHABLEND_SSE(xmdst, xmunder1, xmover1);
//		_mm_store_si128((__m128i*)dst, xmdst);
//
//		over += 4;
//		under += 4;
//		dst += 4;
//	}
//
//	int bn = blt_width - (nloop << 2) - nshift_pix;
//	BltLineAlphaBlend(
//		dst,
//		under + nshift_pix,
//		over + nshift_pix,
//		bn);
//	return;
//}
//
//inline void BltLineAlphaBlend_SSE_nocache(
//	UCvPixel* dst,
//	const UCvPixel* under,
//	const UCvPixel* over,
//	int blt_width)
//{
//	assert (DIFF_16BYTE_ALIGNMENT(under) == DIFF_16BYTE_ALIGNMENT(over));
//
//	if (IS_16BYTE_ALIGNMENT(dst) && IS_16BYTE_ALIGNMENT(under)) {
//		SubBltLineAlphaBlend_SSE(dst, under, over, blt_width);
//		return;
//	}
//
//	int dif_d = DIFF_16BYTE_ALIGNMENT(dst);
//	int dif_s = DIFF_16BYTE_ALIGNMENT(under);
//	assert((dif_d & 0x3) == 0); //1pixel、4byteなので4の倍数になるはず
//	assert((dif_s & 0x3) == 0); //
//
//	if (dif_d == dif_s) {
//		int dif_npix = dif_s >> 2; //アライメントがあっていないピクセル数を得る
//		BltLineAlphaBlend(
//			dst,
//			under,
//			over,
//			dif_npix);
//		SubBltLineAlphaBlend_SSE_nocache(
//			dst + dif_npix,
//			under + dif_npix,
//			over + dif_npix,
//			blt_width - dif_npix);
//	} else {
//		//読み込み元のアライメントを合わせる。
//		int dif_npix = dif_s >> 2;
//		BltLineAlphaBlend(
//			dst,
//			under,
//			over,
//			dif_npix);
//		//
//		dst += dif_npix;
//		under += dif_npix;
//		over += dif_npix;
//		int nshift_byte = DIFF_16BYTE_ALIGNMENT(dst);
//		assert((nshift_byte & 0x3) == 0);
//		switch (nshift_byte) {
//		case 4: //1pixelずれ
//			ShiftSubBltLineAlphaBlend_SSE_nocache<4>(dst, under, over, blt_width);
//			break;
//		case 8: //2pixelずれ
//			ShiftSubBltLineAlphaBlend_SSE_nocache<8>(dst, under, over, blt_width);
//			break;
//		case 12: //3pixelずれ
//			ShiftSubBltLineAlphaBlend_SSE_nocache<12>(dst, under, over, blt_width);
//			break;
//		default:
//			assert(0);
//		}
//	}
//}
//
//void BltAlphaBlend1(
//	IplImage* dst_img,
//	int dst_startX,
//	int dst_startY,
//	int blt_width,
//	int blt_height,
//	const IplImage* under_img,
//	int under_startX,
//	int under_startY,
//	const IplImage* over_img,
//	int over_startX,
//	int over_startY)
//{
//	int y;
//	UCvPixel *over_line = GetPixelAddress32(over_img, over_startX, over_startY);
//	UCvPixel *under_line = GetPixelAddress32(under_img, under_startX, under_startY);
//	UCvPixel *dst_line = GetPixelAddress32(dst_img, dst_startX, dst_startY);
//	for(y = 0; y < blt_height; y++){
//
//		BltLineAlphaBlend(dst_line, under_line, over_line, blt_width);
//
//		over_line = GetNextLineAddress32(over_img, over_line);
//		under_line = GetNextLineAddress32(under_img, under_line);
//		dst_line = GetNextLineAddress32(dst_img, dst_line);
//	}
//}
//
//void BltAlphaBlend2(
//	IplImage* dst_img,
//	int dst_startX,
//	int dst_startY,
//	int blt_width,
//	int blt_height,
//	const IplImage* under_img,
//	int under_startX,
//	int under_startY,
//	const IplImage* over_img,
//	int over_startX,
//	int over_startY)
//{
//	int y;
//	UCvPixel *over_line = GetPixelAddress32(over_img, over_startX, over_startY);
//	UCvPixel *under_line = GetPixelAddress32(under_img, under_startX, under_startY);
//	UCvPixel *dst_line = GetPixelAddress32(dst_img, dst_startX, dst_startY);
//	for(y = 0; y < blt_height; y++){
//
//		BltLineAlphaBlend_SSE_unaligned(
//			dst_line,
//			under_line,
//			over_line,
//			blt_width);
//
//
//		over_line = GetNextLineAddress32(over_img, over_line);
//		under_line = GetNextLineAddress32(under_img, under_line);
//		dst_line = GetNextLineAddress32(dst_img, dst_line);
//	}
//}
//
//void BltAlphaBlend3(
//	IplImage* dst_img,
//	int dst_startX,
//	int dst_startY,
//	int blt_width,
//	int blt_height,
//	const IplImage* under_img,
//	int under_startX,
//	int under_startY,
//	const IplImage* over_img,
//	int over_startX,
//	int over_startY)
//{
//	int y;
//	UCvPixel *over_line = GetPixelAddress32(over_img, over_startX, over_startY);
//	UCvPixel *under_line = GetPixelAddress32(under_img, under_startX, under_startY);
//	UCvPixel *dst_line = GetPixelAddress32(dst_img, dst_startX, dst_startY);
//	for(y = 0; y < blt_height; y++){
//
//		BltLineAlphaBlend_SSE(
//			dst_line,
//			under_line,
//			over_line,
//			blt_width);
//
//
//		over_line = GetNextLineAddress32(over_img, over_line);
//		under_line = GetNextLineAddress32(under_img, under_line);
//		dst_line = GetNextLineAddress32(dst_img, dst_line);
//	}
//}
//
//void BltAlphaBlend4(
//	IplImage* dst_img,
//	int dst_startX,
//	int dst_startY,
//	int blt_width,
//	int blt_height,
//	const IplImage* under_img,
//	int under_startX,
//	int under_startY,
//	const IplImage* over_img,
//	int over_startX,
//	int over_startY)
//{
//	int y;
//	UCvPixel *over_line = GetPixelAddress32(over_img, over_startX, over_startY);
//	UCvPixel *under_line = GetPixelAddress32(under_img, under_startX, under_startY);
//	UCvPixel *dst_line = GetPixelAddress32(dst_img, dst_startX, dst_startY);
//	for(y = 0; y < blt_height; y++){
//
//		BltLineAlphaBlend_SSE_nocache(
//			dst_line,
//			under_line,
//			over_line,
//			blt_width);
//
//
//		over_line = GetNextLineAddress32(over_img, over_line);
//		under_line = GetNextLineAddress32(under_img, under_line);
//		dst_line = GetNextLineAddress32(dst_img, dst_line);
//	}
//}
//
////sseを使った4ch同士のalphablend 
//int main(void)
//{
//	int dst_dif = 2;
//	int src_dif = 2;
//
//	int x,y;
//
//	int r,g,b;
//	int r1, g1, b1;
//	int r2, g2, b2;
//
//	UCvPixel *over_pix;
//	UCvPixel *under_pix;
//	UCvPixel *dst_pix;
//	UCvPixel *over_line;
//	UCvPixel *under_line;
//	UCvPixel *dst_line;
//
//	IplImage* test1 = cvLoadImage("test1.bmp");
//	IplImage* test2 = cvLoadImage("test2.bmp");
//	IplImage* b_ch = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);
//	IplImage* g_ch = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);
//	IplImage* r_ch = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);
//	IplImage* a_ch1 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);
//	IplImage* a_ch2 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 1);
//	IplImage* img1 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 4);
//	IplImage* img2 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 4);
//	IplImage* img3 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 4);
//	IplImage* img4 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 4);
//	IplImage* img5 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 4);
//	IplImage* img6 = cvCreateImage(cvGetSize(test1), IPL_DEPTH_8U, 4);
//
//	//
//	//cvSet(a_ch1, cvScalar(255));
//	//cvSet(a_ch2, cvScalar(255));
//	
//	//
//	uint8_t* line1 = GetPixelAddress(a_ch1, 0, 0);
//	for (y = 0; y < a_ch1->height; y++) {
//		uint8_t* pix = line1;
//		for (x = 0; x < a_ch1->width; x++) {
//			(*pix++) = ((double)x / a_ch1->width) * 255;
//		}
//		line1 = GetNextLineAddress(a_ch1, line1);
//	}
//
//	//
//	uint8_t* line2 = GetPixelAddress(a_ch2, 0, 0);
//	for (y = 0; y < a_ch2->height; y++) {
//		uint8_t* pix = line2;
//		for (x = 0; x < a_ch2->width; x++) {
//			(*pix++) = ((double)y / a_ch2->height) * 255;
//		}
//		line2 = GetNextLineAddress(a_ch2, line2);
//	}
//
//	cvSplit(test1, b_ch, g_ch, r_ch, NULL);
//	cvMerge(b_ch, g_ch, r_ch, a_ch1, img1);
//	cvSplit(test2, b_ch, g_ch, r_ch, NULL);
//	cvMerge(b_ch, g_ch, r_ch, a_ch2, img2);
//
//	int write_width = img1->width - max(dst_dif, src_dif);
//
//	//非SSE
//	IETimer timer;
//	BltAlphaBlend1(
//		img3,
//		dst_dif, 0,
//		write_width, img3->height,
//		img2,
//		src_dif, 0,
//		img1,
//		src_dif, 0);
//	std::cout << timer.elapsed() << " msec" << std::endl;
//	cvSaveImage("test3.bmp", img3);
//
//	//SSE: _mm_loadu_ps + _mm_storeu_ps
//	timer.restart();
//	BltAlphaBlend2(
//		img4,
//		dst_dif, 0,
//		write_width, img4->height,
//		img2,
//		src_dif, 0,
//		img1,
//		src_dif, 0);
//	std::cout << timer.elapsed() << " msec" << std::endl;
//	cvSaveImage("test4.bmp", img4);
//
//	//SSE: _mm_load_ps + _mm_store_ps			
//	timer.restart();
//	BltAlphaBlend3(
//		img5,
//		dst_dif, 0,
//		write_width, img5->height,
//		img2,
//		src_dif, 0,
//		img1,
//		src_dif, 0);
//	std::cout << timer.elapsed() << " msec" << std::endl;
//	cvSaveImage("test5.bmp", img5);
//
//	//SSE: _mm_load_ps + _mm_stream_si128
//	timer.restart();
//	BltAlphaBlend4(
//		img6,
//		dst_dif, 0,
//		write_width, img6->height,
//		img2,
//		src_dif, 0,
//		img1,
//		src_dif, 0);
//	std::cout << timer.elapsed() << " msec" << std::endl;
//	cvSaveImage("test6.bmp", img6);
//
//
//	cvReleaseImage(&b_ch);
//	cvReleaseImage(&g_ch);
//	cvReleaseImage(&r_ch);
//	cvReleaseImage(&a_ch1);
//	cvReleaseImage(&a_ch2);
//	cvReleaseImage(&img1);
//	cvReleaseImage(&img2);
//	cvReleaseImage(&img3);
//	cvReleaseImage(&img4);
//	cvReleaseImage(&img5);
//	cvReleaseImage(&img6);
//}

#include <fstream>
#include <iomanip>

#include "LayerSynthesizer_BenchMark.h"
//
bool FileUpdate_BenchMark(std::ofstream& ofs, ImgEdit* pImgEdit)
{
	char load_file[] = "test_data\\folder_test.fyd";

	double load_file_time = 0.0;
	double update_all_time = 0.0;
	double update_mask_time = 0.0;
	double update_layer_time = 0.0;
	double update_draw_image_time = 0.0;
	double update_mask_image_time = 0.0;
	double update_display_image_time = 0.0;
	for (int i = 0; i < 10; i++) {
		IETimer timer1;
		ImgFile_Ptr pFile = pImgEdit->LoadImgFile(load_file);
		if (pFile == NULL) {
			std::cerr << "can't load " << load_file << std::endl;
			exit(1);
		}
		load_file_time += timer1.elapsed();

		LPUPDATE_DATA pdata = pFile->CreateUpdateData();
		pdata->isAll = true;
		pFile->PushUpdateData(pdata);

		RECT updated_rc;
		timer1.restart();
		pFile->LockUpdateData();
		if (pFile->Update(NULL, &updated_rc)) {
		} else {
			std::cerr << "didn't update file" << std::endl;
		}
		pFile->ClearUpdateData();
		pFile->UnlockUpdateData();

		update_all_time += timer1.elapsed();
		update_mask_time += pFile->m_UpdateMaskTime;
		update_layer_time += pFile->m_UpdateLayerTime;
		update_draw_image_time += pFile->m_UpdateDrawImageTime;
		update_mask_image_time += pFile->m_UpdateMaskImageTime;
		update_display_image_time += pFile->m_UpdateDisplayImageTime;

		pImgEdit->DeleteImgFile(pFile);
	}

	ofs << "all file update time = " << update_all_time / 10.0 << "msec" << std::endl;
	ofs << "Update Mask Time = " << update_mask_time / 10.0 << " msec" << std::endl;
	ofs << "Update Layer Time = " << update_layer_time / 10.0 << " msec" << std::endl;
	ofs << "Update Draw Image Time = " << update_draw_image_time / 10.0 << " msec" << std::endl;
	ofs << "Update Mask Image Time = " << update_mask_image_time / 10.0 << " msec" << std::endl;
	ofs << "Update Display Time = " << update_display_image_time / 10.0 << " msec" << std::endl;

	pImgEdit->End();
	return true;
}

bool LayerEdit_BenchMark(std::ofstream& ofs, ImgEdit* pImgEdit)
{
	UCvPixel bg_pix;
	bg_pix.b = bg_pix.g = bg_pix.r = bg_pix.a = 0;
	ImgFile_Ptr pFile = pImgEdit->CreateNewFile(10000, 10000, &bg_pix);
	if (pFile == NULL) {
		std::cerr << "cant create new file" << std::endl;
		exit(1);
	}

	IImgLayer_Ptr _pLayer = pFile->GetLayerAtIndex(0);
	ImgLayer_Ptr pLayer = std::dynamic_pointer_cast<ImgLayer>(_pLayer);

	RECT edit_rc;
	double time = 0.0;
	for (int i = 0; i < 100; i++) {
		edit_rc.left = 50 * i;
		edit_rc.top = 50 * i;
		edit_rc.right = edit_rc.left + 2000;
		edit_rc.bottom = edit_rc.top + 2000;

		//add edit node
		UCvPixel color;
		color.value = 123456;
		EditNode* pNode = pLayer->CreateEditNode(&edit_rc);
		pNode->edit_img.RectFillColor(
			edit_rc.left,
			edit_rc.top,
			edit_rc.right - edit_rc.left,
			edit_rc.bottom - edit_rc.top,
			&color);
		pNode->edit_img.RectFillAlpha(
			edit_rc.left,
			edit_rc.top,
			edit_rc.right - edit_rc.left,
			edit_rc.bottom - edit_rc.top,
			100);

		pLayer->PushAddEditNodeQueue(pNode);

		//update
		LPUPDATE_DATA pdata = pLayer->CreateUpdateData();
		pdata->isAll = true;
		pLayer->PushUpdateData(pdata);

		RECT updated_rc;
		IETimer timer;
		pLayer->LockUpdateData();
		if (!pLayer->Update(NULL, &updated_rc)) {
			std::cerr << "didn't update layer" << std::endl;
		}
		pLayer->ClearUpdateData();
		pLayer->UnlockUpdateData();
		time += timer.elapsed();

		//
		pLayer->DestroyEditNode(pNode);
	}

	pImgEdit->DeleteImgFile(pFile);

	ofs << "Edit Layer Time = " << time << " msec" << std::endl;
	return true;
}

bool NormalBrushEdit_BenchMark(std::ofstream& ofs, ImgEdit* pImgEdit)
{
	UCvPixel bg_pix;
	bg_pix.b = bg_pix.g = bg_pix.r = bg_pix.a = 0;
	ImgFile_Ptr pFile = pImgEdit->CreateNewFile(10000, 10000, &bg_pix);
	if (pFile == NULL) {
		std::cerr << "cant create new file" << std::endl;
		exit(1);
	}

	IIETool* pTool = pImgEdit->GetIEToolFromName("NormalBrushDraw");
	pImgEdit->SetSelectTool(pTool);

	pImgEdit->SetSelectBrushGroup(0);
	pImgEdit->SetSelectBrush(0);

	IEEMBrush_Ptr pBrush = CreateIEEMBrush();
	pBrush->SetHardness(50);
	pBrush->SetRadius(100);
	pImgEdit->AddBrush(pBrush);
	pImgEdit->SetSelectBrush(pBrush);

	RECT edit_rc;
	double time = 0.0;
	for (int i = 0; i < 3; i++) {
		IE_INPUT_DATA input_data;
		input_data.p = 1.0;

		//line1
		IETimer timer;
		{
			input_data.x = 200;
			input_data.y = 200;
			pImgEdit->OnMouseLButtonDown(0, &input_data);

			input_data.x = 9800;
			input_data.y = 200;
			pImgEdit->OnMouseLButtonUp(0, &input_data);
		}
		time += timer.elapsed();

		//line2
		timer.restart();
		{
			input_data.x = 9800;
			input_data.y = 200;
			pImgEdit->OnMouseLButtonDown(0, &input_data);

			input_data.x = 200;
			input_data.y = 9800;
			pImgEdit->OnMouseLButtonUp(0, &input_data);
		}
		time += timer.elapsed();

		//line3
		timer.restart();
		{
			input_data.x = 200;
			input_data.y = 9800;
			pImgEdit->OnMouseLButtonDown(0, &input_data);

			input_data.x = 9800;
			input_data.y = 9800;
			pImgEdit->OnMouseLButtonUp(0, &input_data);
		}
		time += timer.elapsed();

		//line4
		timer.restart();
		{
			input_data.x = 9800;
			input_data.y = 9800;
			pImgEdit->OnMouseLButtonDown(0, &input_data);

			input_data.x = 200;
			input_data.y = 200;
			pImgEdit->OnMouseLButtonUp(0, &input_data);
		}
		time += timer.elapsed();
	}

	pImgEdit->DeleteBrush(pBrush);
	pImgEdit->DeleteImgFile(pFile);

	ofs << "Normal Brush Edit Time = " << (time / 3.0) << "msec" << std::endl;
	return true;
}

int main()
{
	std::ofstream ofs("output.txt");


	ImgEdit imgEdit;
	{//init ImgEdit
		//初期カレントディレクトリを取得し、ImgEditに渡す
		char cdPath[MAX_PATH];
		if(0 == ::GetCurrentDirectory(MAX_PATH, cdPath)){
			//error
			std::cerr << "faild GetCurrentDirectory" << std::endl;
			return 1;
		}
		imgEdit.SetStartDirectory(cdPath);

		//
		char exePath[MAX_PATH];
		wsprintf(exePath, "%s/IE_BenchMark.exe", cdPath);
		if (!imgEdit.Init(".\\config.ini", exePath, ".\\log")) {
			std::cerr << "faild Init ImgEdt" << std::endl;
			return 1;
		}
	}

	ofs << "----- LayerSynthesizer -----" << std::endl;
	LayerSynthesizer_BenchMark(ofs);
	ofs << std::endl;

	ofs << "----- LayerEdit -----" << std::endl;
	LayerEdit_BenchMark(ofs, &imgEdit);
	ofs << std::endl;

	ofs << "----- FileUpdate -----" << std::endl;
	FileUpdate_BenchMark(ofs, &imgEdit);
	ofs << std::endl;

	ofs << "----- NormalBrushEdit -----" << std::endl;
	NormalBrushEdit_BenchMark(ofs, &imgEdit);
	ofs << std::endl;

	return 0;
}