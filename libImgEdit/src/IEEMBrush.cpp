#include "stdafx.h"

#include "IEEMBrush.h"

#include <random>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

_EXPORTCPP IEEMBrush_Ptr CreateIEEMBrush()
{
	return IEEMBrush_Ptr(new IEEMBrush());
}

////////////////////////////////////
/*!
	XML形式でブラシグループのデータを書き込み
*/
void IEEMBrush::WriteBrushToXml(FILE* fp, int indt)
{
	int i;
	for(i=0; i<indt; i++) fprintf(fp, "  ");
	fprintf(fp, "<brush type=\"em\" name=\"%s\" radius=\"%d\">\n", GetName(), GetOrgRadius()); 

	IEBrush::WriteBrushToXml(fp, indt);

	for(i=0; i<(indt+1); i++) fprintf(fp, "  ");
	fprintf(fp, "<hardness val=\"%d\"/>\n", GetHardness());

	for(i=0; i<indt; i++) fprintf(fp, "  ");
	fprintf(fp, "</brush>\n"); 
}

/////////////////////////////
/*!
	1chデータを元にブラシを作成する
	@param[in] buffer 元になるデータ
	@param[in] width ブラシ幅
	@param[in] height ブラシ高さ
	@param[in] depth データ深度
*/
void IEEMBrush::CreateBrush(const uchar* buffer, int width, int height, int depth)
{
	m_radius = m_orgRadius = width/2.0-1.0;
}

class PreviewCopyOp
{
public:
	inline void operator()(void* dst, uint8_t pix) const
	{
		uint8_t* pdst = (uint8_t*) dst;

		if (pix == 0) return;

		if ((*pdst) == 0) {
			(*pdst) = pix;
		} else {
			uint8_t ra = ~pix;
			uint8_t alpha = 255 - (ra*(255 - (*pdst))>>8);
			(*pdst) = alpha;
		}
		//if ((*pdst) < pix) {
		//	(*pdst) = pix;
		//}
	}
};

/////////////////////////////
/*!
	サイズ(width, height)のサンプル画像を生成して返す。
	@return 作成したIplImageを返す
*/
IplImage* IEEMBrush::CreateBrushSampleImg(int width, int height)
{
	//create smaple input data
	IE_INPUT_DATA input_data;
	input_data.x = width / 2.0;
	input_data.y = height / 2.0;
	input_data.p = 1.0;
	input_data.azimuth_rad = 1.0;
	input_data.altitude_rad = 1.0;
	input_data.random = 1.0;

	if(((m_orgRadius-1.0) < (width/2)) && ((m_orgRadius-1.0) < (height/2))){
		IplImage* sampleImg_1ch = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
		IplImage* sampleImg = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);

		cvSet(sampleImg_1ch, cvScalar(0));

		ClearPrevDrawPt();

		RECT blted_rc;
		PreviewCopyOp copyOp;
		BltBrushData(
				copyOp,
				sampleImg_1ch,
				&input_data,
				&blted_rc);


		cvNot(sampleImg_1ch, sampleImg_1ch);
		cvCvtColor(sampleImg_1ch, sampleImg, CV_GRAY2BGR);

		cvReleaseImage(&sampleImg_1ch);
		return sampleImg;
	}
	else{
		CvSize size;
		size.width = width; size.height = height;
		IplImage* sampleImg_1ch = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
		IplImage* sampleImg = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);

		int rad = (width < height) ? (width/2.0-1) : (height/2.0-1);
		cvSet(sampleImg_1ch, cvScalar(0));

		//change brush radius to draw brush detail
		int old_radius = m_radius;
		m_radius = rad;
		{
			ClearPrevDrawPt();

			RECT blted_rc;
			PreviewCopyOp copyOp;
			BltBrushData(
					copyOp,
					sampleImg_1ch,
					&input_data,
					&blted_rc);
		}
		m_radius = old_radius;

		cvNot(sampleImg_1ch, sampleImg_1ch);
		cvCvtColor(sampleImg_1ch, sampleImg, CV_GRAY2BGR);

		cvReleaseImage(&sampleImg_1ch);
		return sampleImg;
	}
}

/////////////////////////////
/*!
	ブラシプレビューをdstに描画する
	@param[in] dst
	@return
*/
void IEEMBrush::DrawBrushPreview(IplImage* dst)
{
	assert(dst->nChannels == 3);

	int i;
	int start_x = 10;
	int end_x = dst->width-10;
	double center_y = dst->height/2.0;

	IplImage* tmp = cvCreateImage(cvSize(dst->width,dst->height), IPL_DEPTH_8U, 1);
	cvSet(tmp, cvScalar(0));

	double r = m_radius;
	if(r >= (center_y/2.0 - 1.0))
		r = center_y/2.0 - 1.0;

	//change brush radius to draw brush detail
	int old_radius = m_radius;
	m_radius = r;
	{
		std::mt19937 random_engine;
		std::uniform_real_distribution<double> dist(0.0, 1.0);

		RECT rc;
		IE_INPUT_DATA input;
		for(i=start_x; i<end_x; i++){
			double rad = (double)i/(end_x-start_x) * 2.0 * CV_PI;
			input.x = i;
			input.y = center_y + 10.0*sin(rad);
			input.p = sin(rad/2.0);
			input.azimuth_rad = rad;
			input.altitude_rad = rad - CV_PI;
			input.random = dist(random_engine);
		
			PreviewCopyOp copyOp;
			BltBrushData(
				copyOp,
				tmp,
				&input,
				&rc);
		}
	}
	m_radius = old_radius;

	cvNot(tmp, tmp);
	cvCvtColor(tmp, dst, CV_GRAY2BGR);

	cvReleaseImage(&tmp);
}

void IEEMBrush::SetHardness(int percent)
{
	if(m_hardness == (percent/100.0)) return;

	m_hardness = percent/100.0;

	//call event listener
	IEBrushEventListener_List::iterator itr = m_IEBrushEventListener_List.begin();
	for(; itr != m_IEBrushEventListener_List.end(); itr++){
		if((*itr)->IsLockBrushEvent() == false && (*itr)->IsCalledIEBrush() == false){
			//無限にイベント呼び出しが続かないようにフラグを立てる
			(*itr)->SetCalledIEBrush(true);
			//
			(*itr)->OnChangeBrushShape();
		}

		(*itr)->SetCalledIEBrush(false);
	}
}
