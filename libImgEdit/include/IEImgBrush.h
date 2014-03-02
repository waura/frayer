#ifndef _IEIMGBRUSH_H_
#define _IEIMGBRUSH_H_

#include "IEBrush.h"

//
//
class _EXPORTCPP IEImgBrush :
	public IEBrush
{
public:
	IEImgBrush(){
		m_brush_type_id = IE_IMG_BRUSH;
		m_brushOrgImg = NULL;
		m_brushImg = NULL;
	}
	~IEImgBrush();

	////////////////////////////////////
	/*!
		XML形式でブラシグループのデータを書き込み
	*/
	void WriteBrushToXml(FILE* fp, int indt);

	/////////////////////////////
	/*!
		元ブラシデータを返す
		@return ブラシデータ
	*/
	inline const IplImage* GetOrgBrushData() const{
		return m_brushOrgImg;
	}

	/////////////////////////////
	/*!
		元ブラシデータを返す
		@return ブラシデータ
	*/
	inline const IplImage* GetBrushData() const{
		return m_brushImg;
	}

	////////////////////////////
	/*!
		ブラシの中心座標を返す。
	*/
	inline void GetBrushCenter(LPPOINT lpt){
		RECT brushRect;
		ucvGetMaskRect(m_brushImg, &brushRect);
		int brush_width = brushRect.right - brushRect.left;
		int brush_height = brushRect.bottom - brushRect.top;

		lpt->x = brush_width/2 + brushRect.left;
		lpt->y = brush_height/2 + brushRect.top;
	}

	////////////////////////////
	/*!
		ブラシのサイズを返す
		@return ブラシのサイズ
	*/
	inline CvSize GetBrushSize() const{
		return cvGetSize(m_brushImg);
	}

	//////////////////////////////
	/*!
		ブラシの深度を返す。
		@return ブラシ深度
	*/
	inline int GetBrushDepth() const{
		return m_brushOrgImg->depth;
	}

	//////////////////////////////
	/*!
		画像を元にブラシを作成する。
		@param[in] path 画像のパス
	*/
	bool LoadBrushImg(const char* path);

	/////////////////////////////
	/*!
		1chデータを元にブラシを作成する
		@param[in] buffer 元になるデータ
		@param[in] width ブラシ幅
		@param[in] height ブラシ高さ
		@param[in] depth データ深度
	*/
	void CreateBrush(const uchar* buffer, int width, int height, int depth);

	/////////////////////////////
	/*!
		ブラシデータをdstに転送する
		@param[in,out] dst 1ch image
		@param[in] center_lpd center input data
		@param[out] blted_rc
		@return
	*/
	void BltBrushData(IplImage* dst, const LPIE_INPUT_DATA center_lpd, LPRECT blted_rc);

	/////////////////////////////
	/*!
		ブラシデータをdstに転送する
		@param[in] copyOp
		@param[in,out] dst image, 4ch or 1ch
		@param[in] center_lpd center input data
		@param[out] blted_rc
		@return
	*/
	template <class _CBltOp>
	void BltBrushData(
		const _CBltOp& copyOp,
		IplImageExt* dst,
		const LPIE_INPUT_DATA center_lpd,
		LPRECT blted_rc);

	template <class _CBltOp>
	void BltBrushData(
		const _CBltOp& copyOp,
		IplImage* dst,
		const LPIE_INPUT_DATA center_lpd,
		LPRECT blted_rc);

	/////////////////////////////
	/*!
		サイズ(width, height)のサンプル画像を生成して返す。
		@return 作成したIplImageを返す
	*/
	IplImage* CreateBrushSampleImg(int width, int height);

	/////////////////////////////
	/*!
		ブラシプレビューをdstに描画する
		@param[in] dst
		@return
	*/
	void DrawBrushPreview(IplImage* dst);

	//////////////////////////////
	/*!
		ブラシの有効範囲半径をセットする。
	*/
	void SetBrushRadius(int r);

private:
	std::string m_imgPath;

	IplImage* m_brushOrgImg;	//ブラシ元データ
	IplImage* m_brushImg;		//ブラシサイズ変更後データ
};

_EXPORTCPP IEImgBrush_Ptr CreateIEImgBrush();

template <class _CCopyOp>
class ImgBrushEdit1chOp {
public:
	ImgBrushEdit1chOp(const _CCopyOp& copyOp)
	{
		m_copyOp = copyOp;
	}
	~ImgBrushEdit1chOp(){};

	void SetParam(
		double center_x,
		double center_y,
		double rad,
		double in_d,
		double out_d,
		uint8_t brush_alpha,
		IplImage* texture)
	{
		m_center_x = center_x;
		m_center_y = center_y;
		m_rad = rad;
		m_in_d = in_d;
		m_out_d = out_d;
		m_brush_alpha = brush_alpha;
		m_texture = texture;


		if (m_texture) {
			assert(m_texture->width == m_texture->height);
			assert((m_texture->width == 256) || (m_texture->width == 512) || (m_texture->width == 1024));

			if (m_texture->width == 256) {
				m_texture_shift_count = 8;
				m_texture_mod_bit = 0xff;
			}
			else if (m_texture->width == 512) {
				m_texture_shift_count = 9;
				m_texture_mod_bit = 0x1ff;
			}
			else if (m_texture->width == 1024) {
				m_texture_shift_count = 10;
				m_texture_mod_bit = 0x3ff;
			}
		}
	}

	inline void operator()(void* dst, int posX, int posY, int edit_width) const
	{
	}

private:

	_CCopyOp m_copyOp;
	double m_center_x;
	double m_center_y;
	double m_rad;
	double m_in_d;
	double m_out_d;
	uint8_t m_brush_alpha;
	IplImage* m_texture;
	int m_texture_shift_count;
	int m_texture_mod_bit;
};

template <class _CCopyOp>
class ImgBrushEdit4chOp {
public:
	ImgBrushEdit4chOp(const _CCopyOp& copyOp)
	{
		m_copyOp = copyOp;
	}
	~ImgBrushEdit4chOp(){};

	void SetParam(
		double center_x,
		double center_y,
		double rad,
		double in_d,
		double out_d,
		uint8_t brush_alpha,
		IplImage* texture)
	{
		m_center_x = center_x;
		m_center_y = center_y;
		m_rad = rad;
		m_in_d = in_d;
		m_out_d = out_d;
		m_brush_alpha = brush_alpha;
		m_texture = texture;


		if (m_texture) {
			assert(m_texture->width == m_texture->height);
			assert((m_texture->width == 256) || (m_texture->width == 512) || (m_texture->width == 1024));

			if (m_texture->width == 256) {
				m_texture_shift_count = 8;
				m_texture_mod_bit = 0xff;
				}
			else if (m_texture->width == 512) {
				m_texture_shift_count = 9;
				m_texture_mod_bit = 0x1ff;
			}
			else if (m_texture->width == 1024) {
				m_texture_shift_count = 10;
				m_texture_mod_bit = 0x3ff;
			}
		}
	}

	inline void operator()(void* dst, int posX, int posY, int edit_width) const
	{
	}

private:

	_CCopyOp m_copyOp;
	double m_center_x;
	double m_center_y;
	double m_rad;
	double m_in_d;
	double m_out_d;
	uint8_t m_brush_alpha;
	IplImage* m_texture;
	int m_texture_shift_count;
	int m_texture_mod_bit;
};

template <class _CCopyOp>
void IEImgBrush::BltBrushData(
	const _CCopyOp& copyOp,
	IplImageExt* dst,
	const LPIE_INPUT_DATA center_lpd,
	LPRECT blted_rc)
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
	double pow_brush_dist = (brush_ctrl.center_x - m_prev_draw_pt.x) * (brush_ctrl.center_x - m_prev_draw_pt.x) +
		(brush_ctrl.center_y - m_prev_draw_pt.y) * (brush_ctrl.center_y - m_prev_draw_pt.y);
	if (brush_ctrl.interval * brush_ctrl.interval > pow_brush_dist) {
		blted_rc->top = blted_rc->bottom = 0;
		blted_rc->left = blted_rc->right = 0;
		return;
	}
	m_prev_draw_pt.x = brush_ctrl.center_x;
	m_prev_draw_pt.y = brush_ctrl.center_y;

	//
	double out_aa = 1.5;
	double in_aa = brush_ctrl.rad * (1.0 - m_hardness) + 1.5;
	if (brush_ctrl.rad < in_aa)
		in_aa = brush_ctrl.rad;
	
	blted_rc->top = brush_ctrl.center_y - brush_ctrl.rad - 2.0;
	blted_rc->left = brush_ctrl.center_x - brush_ctrl.rad - 2.0;
	blted_rc->bottom = brush_ctrl.center_y + brush_ctrl.rad + 2.0;
	blted_rc->right = brush_ctrl.center_x + brush_ctrl.rad + 2.0;

	//clipping
	CvSize img_size = dst->GetSize();
	RECT img_rect;
	img_rect.left = 0;
	img_rect.top = 0;
	img_rect.right = img_size.width;
	img_rect.bottom = img_size.height; 
	AndRect(blted_rc, &img_rect, blted_rc);

	//
	uint8_t brush_alpha = brush_ctrl.alpha * 255;

	if (dst->GetImgNChannel() == 4) {
		ImgBrushEdit4chOp<_CCopyOp> editOp(copyOp);
		editOp.SetParam(
			brush_ctrl.center_x,
			brush_ctrl.center_y,
			brush_ctrl.rad,
			in_aa,
			out_aa,
			brush_alpha,
			m_texture);
		dst->OperateEdit(
			editOp,
			blted_rc->left,
			blted_rc->top,
			blted_rc->right - blted_rc->left,
			blted_rc->bottom - blted_rc->top);
	} else if (dst->GetImgNChannel() == 1) {
		ImgBrushEdit1chOp<_CCopyOp> editOp(copyOp);
		editOp.SetParam(
			brush_ctrl.center_x,
			brush_ctrl.center_y,
			brush_ctrl.rad,
			in_aa,
			out_aa,
			brush_alpha,
			m_texture);
		dst->OperateEdit(
			editOp,
			blted_rc->left,
			blted_rc->top,
			blted_rc->right - blted_rc->left,
			blted_rc->bottom - blted_rc->top);
	} else {
		assert(0);
	}
}

template <class _CCopyOp>
void IEImgBrush::BltBrushData(
	const _CCopyOp& copyOp,
	IplImage* dst,
	const LPIE_INPUT_DATA center_lpd,
	LPRECT blted_rc)
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
	double pow_brush_dist = (brush_ctrl.center_x - m_prev_draw_pt.x) * (brush_ctrl.center_x - m_prev_draw_pt.x) +
		(brush_ctrl.center_y - m_prev_draw_pt.y) * (brush_ctrl.center_y - m_prev_draw_pt.y);
	if (brush_ctrl.interval * brush_ctrl.interval > pow_brush_dist) {
		blted_rc->top = blted_rc->bottom = 0;
		blted_rc->left = blted_rc->right = 0;
		return;
	}
	m_prev_draw_pt.x = brush_ctrl.center_x;
	m_prev_draw_pt.y = brush_ctrl.center_y;

	//
	double out_aa = 1.5;
	double in_aa = brush_ctrl.rad * (1.0 - m_hardness) + 1.5;
	if (brush_ctrl.rad < in_aa)
		in_aa = brush_ctrl.rad;
	
	blted_rc->top = brush_ctrl.center_y - brush_ctrl.rad - 2.0;
	blted_rc->left = brush_ctrl.center_x - brush_ctrl.rad - 2.0;
	blted_rc->bottom = brush_ctrl.center_y + brush_ctrl.rad + 2.0;
	blted_rc->right = brush_ctrl.center_x + brush_ctrl.rad + 2.0;

	//clipping
	RECT img_rect;
	img_rect.left = 0;
	img_rect.top = 0;
	img_rect.right = dst->width;
	img_rect.bottom = dst->height; 
	AndRect(blted_rc, &img_rect, blted_rc);

	//
	uint8_t brush_alpha = brush_ctrl.alpha * 255;

	if (dst->nChannels == 4) {
		ImgBrushEdit4chOp<_CCopyOp> editOp(copyOp);
		editOp.SetParam(
			brush_ctrl.center_x,
			brush_ctrl.center_y,
			brush_ctrl.rad,
			in_aa,
			out_aa,
			brush_alpha,
			m_texture);
		IplImageExt::OperateEdit(
			editOp,
			dst,
			blted_rc->left,
			blted_rc->top,
			blted_rc->right - blted_rc->left,
			blted_rc->bottom - blted_rc->top);
	} else if (dst->nChannels == 1) {
		ImgBrushEdit1chOp<_CCopyOp> editOp(copyOp);
		editOp.SetParam(
			brush_ctrl.center_x,
			brush_ctrl.center_y,
			brush_ctrl.rad,
			in_aa,
			out_aa,
			brush_alpha,
			m_texture);
		IplImageExt::OperateEdit(
			editOp,
			dst,
			blted_rc->left,
			blted_rc->top,
			blted_rc->right - blted_rc->left,
			blted_rc->bottom - blted_rc->top);
	} else {
		assert(0);
	}
}


#endif //_IEIMGBRUSH_H_