#include "stdafx.h"

#include "RLE.h"
#include "Base64.h"
#include "IEImgBrush.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

_EXPORTCPP IEImgBrush_Ptr CreateIEImgBrush()
{
	return IEImgBrush_Ptr(new IEImgBrush());
}

IEImgBrush::~IEImgBrush()
{
	if(m_brushOrgImg){
		cvReleaseImage(&m_brushOrgImg);
	}
	if(m_brushImg){
		cvReleaseImage(&m_brushImg);
	}
}

////////////////////////////////////
/*!
	XML形式でブラシグループのデータを書き込み
*/
void IEImgBrush::WriteBrushToXml(FILE* fp, int indt)
{
	int i;
	for(i=0; i<indt; i++) fprintf(fp, "  ");
	fprintf(fp, "<brush type=\"img\" name=\"%s\">\n", GetName()); 

	//
	IEBrush::WriteBrushToXml(fp, indt);

	if (m_brushOrgImg) {
		for(i=0; i<(indt+1); i++) fprintf(fp, "  ");
		fprintf(fp, "<brush_data width=\"%d\" height=\"%d\" depth=\"%d\">\n",
			m_brushOrgImg->width, m_brushOrgImg->height, m_brushOrgImg->depth);
		{
			uint32_t rle_brush_data_size = 0;
			std::shared_ptr<int8_t> rle_brush_data = std::shared_ptr<int8_t>(new int8_t[2 * m_brushOrgImg->imageSize]);
			encode_rle(
				(const int8_t*)m_brushOrgImg->imageData,
				m_brushOrgImg->imageSize,
				rle_brush_data.get(),
				&rle_brush_data_size);

			size_t dst_len = BASE64_ENCODE_SIZE(rle_brush_data_size);
			std::shared_ptr<char> base64_data = std::shared_ptr<char>(new char[dst_len]);
			encode_base64(
				(const char*)rle_brush_data.get(),
				rle_brush_data_size,
				base64_data.get());
			fprintf(fp, "%s\n", base64_data.get());
		}
		for(i=0; i<(indt+1); i++) fprintf(fp, "  ");
		fprintf(fp, "</brush_data>\n");
	}

	for(i=0; i<indt; i++) fprintf(fp, "  ");
	fprintf(fp, "</brush>\n"); 
}

//////////////////////////////
/*!
	画像を元にブラシを作成する。
	@param[in] path 画像のパス
*/
bool IEImgBrush::LoadBrushImg(const char* path)
{
	m_imgPath = path;

	//画像を読み込み
	IplImage* img = cvLoadImage(path, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
	if (img == NULL) {//読み込み失敗
		TCHAR log[512];
		wsprintf(log, "cant find brush image %s", path);
		OutputError::PushLog(LOG_LEVEL::_WARN, log);
		return false;
	}

	IplImage* img8;
	//8bit符号なしデータでなければ8bit符号なしデータに変換
	if (img->depth != IPL_DEPTH_8U) {
		img8 = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
		cvConvertScaleAbs( img, img8, 1, 0);
		cvReleaseImage( &img );
	}
	else {
		img8 = img;
	}

	m_brushOrgImg = cvCreateImage(cvGetSize(img8), IPL_DEPTH_8U, 1);
	m_brushImg = cvCreateImage(cvGetSize(img8), IPL_DEPTH_8U, 1);

	cvCvtColor(img8, m_brushOrgImg, CV_BGR2GRAY);
	cvNot(m_brushOrgImg, m_brushOrgImg);
	cvCopy(m_brushOrgImg, m_brushImg);

	//ブラシ直径の計算
	int dim = (img8->width > img8->height) ? img8->width : img8->height;
	m_orgRadius = dim/2;
	IEBrush::SetRadius( m_orgRadius );

	return true;
}

/////////////////////////////
/*!
	1chデータを元にブラシを作成する
	@param[in] buffer 元になるデータ
	@param[in] width ブラシ幅
	@param[in] height ブラシ高さ
	@param[in] depth データ深度
*/
void IEImgBrush::CreateBrush(const uchar *buffer, int width, int height, int depth)
{
	if(m_brushOrgImg) return;

	CvSize s;
	s.width = width;
	s.height= height;
	m_brushOrgImg = cvCreateImage(s, IPL_DEPTH_8U, 1);
	m_brushImg = cvCreateImage(s, IPL_DEPTH_8U, 1);

	int x,y;
	for(y=0; y<height; y++){
		for(x=0; x<width; x++){
			SetMaskDataPos(m_brushOrgImg, x, y, buffer[x + y*width]);
			SetMaskDataPos(m_brushImg, x, y, buffer[x + y*width]);
		}
	}
	//memcpy(m_brushOrgImg->imageData, buffer, height*width);
	//memcpy(m_brushImg->imageData, buffer, height*width);

	//ブラシ直径の計算
	RECT brushRect;
	ucvGetMaskRect(m_brushImg, &brushRect);
	int brush_width = brushRect.right - brushRect.left;
	int brush_height = brushRect.bottom - brushRect.top;
	int dim = (brush_width > brush_height) ? brush_width : brush_height;
	m_orgRadius = dim/2;
	IEBrush::SetRadius( m_orgRadius );
}

/////////////////////////////
/*!
	ブラシデータを返す
	@return ブラシデータ
*/
void IEImgBrush::BltBrushData(IplImage* dst, const LPIE_INPUT_DATA center_lpd, LPRECT blted_rc)
{
	BRUSH_CTRL brush_ctrl;
	GetBrushCtrl(&brush_ctrl, center_lpd);

	// check brush radius
	if (brush_ctrl.rad <= 0.0) {
		blted_rc->top = blted_rc->bottom = 0;
		blted_rc->left = blted_rc->right = 0;
		return;
	}

	// check brush interval
	double pow_brush_dist = (center_lpd->x - m_prev_draw_pt.x) * (center_lpd->x - m_prev_draw_pt.x) +
		(center_lpd->y - m_prev_draw_pt.y) * (center_lpd->y - m_prev_draw_pt.y);
	if (brush_ctrl.interval * brush_ctrl.interval > pow_brush_dist) {
		blted_rc->top = blted_rc->bottom = 0;
		blted_rc->left = blted_rc->right = 0;
		return;
	}
	m_prev_draw_pt.x = center_lpd->x;
	m_prev_draw_pt.y = center_lpd->y;


	//make brush
	double mag = (double)brush_ctrl.rad / m_orgRadius;
	CvSize orgSize = cvGetSize(m_brushOrgImg);
	CvSize size;
	size.width = orgSize.width * mag;
	size.height = orgSize.height * mag;

	size.width = (size.width < 1) ? 1 : size.width;
	size.height = (size.height < 1) ? 1 : size.height;

	IplImage* brush_data = cvCreateImage(size, IPL_DEPTH_8U, 1);
	cvResize(m_brushOrgImg, brush_data, CV_INTER_LINEAR);


	blted_rc->left = (int)brush_ctrl.center_x - (brush_data->width/2);
	blted_rc->top = (int)brush_ctrl.center_y - (brush_data->height/2);
	blted_rc->right = blted_rc->left + brush_data->width;
	blted_rc->bottom = blted_rc->top + brush_data->height; 

	if(blted_rc->left < 0) blted_rc->left = 0;
	if(blted_rc->top < 0) blted_rc->top = 0;
	if(blted_rc->right < 0) blted_rc->right = 0;
	if(blted_rc->bottom < 0) blted_rc->bottom = 0;
	if(blted_rc->left > dst->width) blted_rc->left = dst->width;
	if(blted_rc->top > dst->height) blted_rc->top = dst->height;
	if(blted_rc->right > dst->width) blted_rc->right = dst->width;
	if(blted_rc->bottom > dst->height) blted_rc->bottom = dst->height;

	//blt
	int x,y;
	int8_t bval;
	for(y=blted_rc->top; y<blted_rc->bottom; y++){
		for(x=blted_rc->left; x<blted_rc->right; x++){
			bval = GetMaskDataPos(brush_data, x - blted_rc->left, y - blted_rc->top);
			if(bval > GetMaskDataPos(dst, x, y)){
				SetMaskDataPos(dst, x, y, bval);
			}
		}
	}
	
	cvReleaseImage(&brush_data);
}

/////////////////////////////
/*!
	サイズ(width, height)のサンプル画像を生成して返す。
	@return 作成したIplImageを返す
*/
IplImage* IEImgBrush::CreateBrushSampleImg(int width, int height)
{

	if((m_brushOrgImg->width < width) && (m_brushOrgImg->height < height)){
		CvSize size;
		size.width = width; size.height = height;
		IplImage* sampleImg_resize_1ch = cvCreateImage(size, IPL_DEPTH_8U, 1);
		IplImage* sampleImg_resize = cvCreateImage(size, IPL_DEPTH_8U, 3);

		cvSet(sampleImg_resize_1ch, cvScalar(0));
		BltMaskToMask(
			sampleImg_resize_1ch,
			(width - m_brushOrgImg->width)/2,
			(height - m_brushOrgImg->height)/2,
			m_brushOrgImg->width,
			m_brushOrgImg->height,
			m_brushOrgImg,
			0,
			0);

		cvNot(sampleImg_resize_1ch, sampleImg_resize_1ch);
		cvCvtColor(sampleImg_resize_1ch, sampleImg_resize, CV_GRAY2BGR);

		cvReleaseImage(&sampleImg_resize_1ch);

		return sampleImg_resize;
	}
	else{
		CvSize size;
		size.width = width; size.height = height;
		IplImage* sampleImg = cvCreateImage(cvGetSize(m_brushOrgImg), IPL_DEPTH_8U, 3);
		IplImage* sampleImg_resize = cvCreateImage(size, IPL_DEPTH_8U, 3);

		cvCvtColor(m_brushOrgImg, sampleImg, CV_GRAY2BGR);
		cvResize(sampleImg, sampleImg_resize, CV_INTER_LINEAR);
		cvNot(sampleImg_resize, sampleImg_resize);

		cvReleaseImage(&sampleImg);

		return sampleImg_resize;
	}
}

/////////////////////////////
/*!
	ブラシプレビューをdstに描画する
	@param[in] dst
	@return
*/
void IEImgBrush::DrawBrushPreview(IplImage* dst)
{
	int i;
	int start_x = 5;
	int end_x = dst->width-5;
	double center_y = dst->height/2.0;

	double r = m_radius;
	if(r >= (center_y/2.0 - 1.0))
		r = center_y/2.0 - 1.0;

	IplImage* tmp = cvCreateImage(cvSize(dst->width,dst->height), IPL_DEPTH_8U, 1);
	cvSet(tmp, cvScalar(0));

	RECT blted_rc;
	for(i=start_x; i<end_x; i++){
		double rad =  (double)i/(end_x-start_x)* 3.14159265*2.0;
		double y = 10.0*sin(rad);
		double p = sin(rad/2.0);
		

		double ret_brush_rad = p * r;
		double min_ret_brush_rad = m_min_radius_per * r;
		//apply min brush radius
		if( ret_brush_rad < min_ret_brush_rad )
			ret_brush_rad = min_ret_brush_rad;

		uint8_t brush_alpha = p * 255;
		uint8_t min_brush_alpha = m_min_alpha_per * 255;
		//apply min brush alpha
		if( brush_alpha < min_brush_alpha )
			brush_alpha = min_brush_alpha;

		if(ret_brush_rad <= 0.0)
			continue;

		//make brush
		double mag = (double)ret_brush_rad / m_orgRadius;
		CvSize orgSize = cvGetSize(m_brushOrgImg);
		CvSize size;
		size.width = orgSize.width * mag;
		size.height = orgSize.height * mag;

		size.width = (size.width < 1) ? 1 : size.width;
		size.height = (size.height < 1) ? 1 : size.height;

		IplImage* brush_data = cvCreateImage(size, IPL_DEPTH_8U, 1);
		cvResize(m_brushOrgImg, brush_data, CV_INTER_LINEAR);

		blted_rc.left = i - (brush_data->width/2);
		blted_rc.top = center_y + y - (brush_data->height/2);
		blted_rc.right = blted_rc.left + brush_data->width;
		blted_rc.bottom = blted_rc.top + brush_data->height; 

		//blt
		int8_t bval;
		for(int y=blted_rc.top; y<blted_rc.bottom; y++){
			for(int x=blted_rc.left; x<blted_rc.right; x++){
				bval = GetMaskDataPos(brush_data, x - blted_rc.left, y - blted_rc.top);
				if(bval > GetMaskDataPos(tmp, x, y)){
					SetMaskDataPos(tmp, x, y, bval);
				}
			}
		}
		
		cvReleaseImage(&brush_data);
	}

	cvNot(tmp, tmp);
	cvCvtColor(tmp, dst, CV_GRAY2BGR);

	cvReleaseImage(&tmp);
}

//////////////////////////////
/*!
	ブラシの有効範囲半径をセットする。
*/
void IEImgBrush::SetBrushRadius(int r)
{
	if(m_brushImg){
		cvReleaseImage(&m_brushImg);
	}

	double mag = (double)m_radius / m_orgRadius;
	CvSize orgSize = cvGetSize(m_brushOrgImg);
	CvSize size;
	size.width = orgSize.width * mag;
	size.height = orgSize.height * mag;

	size.width = (size.width < 1) ? 1 : size.width;
	size.height = (size.height < 1) ? 1 : size.height;

	m_brushImg = cvCreateImage(size, IPL_DEPTH_8U, 1);
	cvResize(m_brushOrgImg, m_brushImg, CV_INTER_LINEAR);

	IEBrush::SetRadius( r );
}