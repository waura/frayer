#ifndef _IEEMBRUSH_H_
#define _IEEMBRUSH_H_

#include "IEBrush.h"


_EXPORTCPP IEEMBrush_Ptr CreateIEEMBrush();

class _EXPORTCPP IEEMBrush :
	public IEBrush
{
public:
	IEEMBrush(){
		m_brush_type_id = IE_EM_BRUSH;
		m_hardness = 1.0;
	}
	~IEEMBrush(){};

	////////////////////////////////////
	/*!
		XML形式でブラシグループのデータを書き込み
	*/
	void WriteBrushToXml(FILE* fp, int indt);

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

	////////////////////////////////
	/*!
		ブラシの硬さを返す。
	*/
	inline int GetHardness() const{
		return m_hardness*100;
	}

	////////////////////////////////
	/*!
		ブラシの硬さをセットする。
	*/
	void SetHardness(int percent);

private:
	double m_hardness;			//ブラシ硬さ(0.0~1.0)
};

static inline uint8_t CircleBrushPixel(
	int posX,
	int posY,
	double center_x,
	double center_y,
	double aa_min_d,
	double aa_max_d,
	uint8_t brush_alpha)
{
	uint8_t pix;
	double difx = posX - center_x + 0.5;
	double dify = posY - center_y + 0.5;
	double frr = difx * difx + dify * dify;
	if (frr < (aa_min_d * aa_min_d)) {
		pix = brush_alpha;
	}
	else if (frr >= (aa_max_d * aa_max_d)) {
		pix = 0;
	}
	else {
		double fr = sqrtf(frr);
		double tmp = 1.0 - (fr - aa_min_d)/(aa_max_d - aa_min_d);
		pix = (tmp*tmp) * (3.0-2.0*tmp) * brush_alpha;
	}
	return pix;
}

template <class _CCopyOp>
class CircleBrushEdit1chOp {
public:
	CircleBrushEdit1chOp(const _CCopyOp& copyOp)
	{
		m_copyOp = copyOp;
	}
	~CircleBrushEdit1chOp(){};

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
		uint8_t* pdst = (uint8_t*) dst;
		LineBltBrush(pdst, posX, posY, edit_width);
	}

private:

	inline void LineBltBrush(uint8_t* pdst, int posX, int posY, int edit_width) const
	{
		assert(pdst);
		assert(m_rad > 0);
		assert(0.0 <= m_in_d);
		assert(0.0 <= m_out_d);

		double aa_min_d = m_rad - m_in_d;
		double aa_max_d = m_rad + m_out_d;
		//double aa_min_dd = aa_min_d * aa_min_d;
		//double aa_max_dd = aa_max_d * aa_max_d;

		if (m_texture == NULL) {
			int x;
			uint8_t pix;
			for (x = 0; x < edit_width; x++) {
				pix = CircleBrushPixel(
					posX + x,
					posY,
					m_center_x,
					m_center_y,
					aa_min_d,
					aa_max_d,
					m_brush_alpha);
				m_copyOp(pdst, pix);
				pdst++;
			}
		}
		else {
			int i, x;
			uint8_t pix;
			uint8_t mask;
			for (x = 0; x < edit_width; x++) {
				pix = CircleBrushPixel(
					posX + x,
					posY,
					m_center_x,
					m_center_y,
					aa_min_d,
					aa_max_d,
					m_brush_alpha);
				mask = GetMaskDataPos(
					 m_texture,
					(m_texture_mod_bit & (posX + x)),
					(m_texture_mod_bit & posY));
				pix = ((pix * mask) >> 8) + 1;
				m_copyOp(pdst, pix);
				pdst++;
			}
		}
	}

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
class CircleBrushEdit4chOp {
public:
	CircleBrushEdit4chOp(const _CCopyOp& copyOp)
	{
		m_copyOp = copyOp;
	}
	~CircleBrushEdit4chOp(){};

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
		UCvPixel* pdst = (UCvPixel*) dst;
//#ifdef IE_USE_WIN32_SSE
//		int dif = DIFF_16BYTE_ALIGNMENT(pdst);
//		assert((dif & 0x3) == 0);
//
//		int dif_npix = dif >> 2;
//		LineBltBrush(pdst, posX, posY, dif_npix);
//		LineBltBrush_SSE(
//			pdst + dif_npix,
//			posX + dif_npix,
//			posY,
//			edit_width - dif_npix);
//#else
		LineBltBrush(pdst, posX, posY, edit_width);
//#endif //IE_USE_WIN32_SSE
	}

private:

	inline void LineBltBrush(UCvPixel* pdst, int posX, int posY, int edit_width) const
	{
		assert(pdst);
		assert(m_rad > 0);
		assert(0.0 <= m_in_d);
		assert(0.0 <= m_out_d);

		double aa_min_d = m_rad - m_in_d;
		double aa_max_d = m_rad + m_out_d;
		//double aa_min_dd = aa_min_d * aa_min_d;
		//double aa_max_dd = aa_max_d * aa_max_d;

		if (m_texture == NULL) {
			int x;
			uint8_t pix;
			for (x = 0; x < edit_width; x++) {
				pix = CircleBrushPixel(
					posX + x,
					posY,
					m_center_x,
					m_center_y,
					aa_min_d,
					aa_max_d,
					m_brush_alpha);
				m_copyOp(pdst, pix);
				pdst++;
			}
		}
		else {
			int i, x;
			uint8_t pix;
			uint8_t mask;
			for (x = 0; x < edit_width; x++) {
				pix = CircleBrushPixel(
					posX + x,
					posY,
					m_center_x,
					m_center_y,
					aa_min_d,
					aa_max_d,
					m_brush_alpha);
				mask = GetMaskDataPos(
					 m_texture,
					(m_texture_mod_bit & (posX + x)),
					(m_texture_mod_bit & posY));
				pix = ((pix * mask) >> 8) + 1;
				m_copyOp(pdst, pix);
				pdst++;
			}
		}
	}

	inline void LineBltBrush_SSE(UCvPixel* pdst, int posX, int posY, int edit_width) const
	{
		assert(pdst);
		assert(IS_16BYTE_ALIGNMENT(pdst));
		assert(m_rad > 0);
		assert(0.0 <= m_in_d);
		assert(0.0 <= m_out_d);

		double aa_min_d = m_rad - m_in_d;
		double aa_max_d = m_rad + m_out_d;
		double aa_min_dd = aa_min_d * aa_min_d;
		double aa_max_dd = aa_max_d * aa_max_d;

		int nloop = edit_width >> 3; //width / 8
		int i,x;
		uint8_t pix;
		__m128i mmpix16;

		for (i = 0; i < nloop; i++) {
			{
				pix = CircleBrushPixel(
					posX + (i << 3) + 0,
					posY,
					m_center_x,
					m_center_y,
					aa_min_d,
					aa_max_d,
					m_brush_alpha);
				_mm_insert_epi16(mmpix16, pix, 0);
			}
			{
				pix = CircleBrushPixel(
					posX + (i << 3) + 1,
					posY,
					m_center_x,
					m_center_y,
					aa_min_d,
					aa_max_d,
					m_brush_alpha);
				_mm_insert_epi16(mmpix16, pix, 1);
			}
			{
				pix = CircleBrushPixel(
					posX + (i << 3) + 2,
					posY,
					m_center_x,
					m_center_y,
					aa_min_d,
					aa_max_d,
					m_brush_alpha);
				_mm_insert_epi16(mmpix16, pix, 2);
			}
			{
				pix = CircleBrushPixel(
					posX + (i << 3) + 3,
					posY,
					m_center_x,
					m_center_y,
					aa_min_d,
					aa_max_d,
					m_brush_alpha);
				_mm_insert_epi16(mmpix16, pix, 3);
			}
			{
				pix = CircleBrushPixel(
					posX + (i << 3) + 4,
					posY,
					m_center_x,
					m_center_y,
					aa_min_d,
					aa_max_d,
					m_brush_alpha);
				_mm_insert_epi16(mmpix16, pix, 4);
			}
			{
				pix = CircleBrushPixel(
					posX + (i << 3) + 5,
					posY,
					m_center_x,
					m_center_y,
					aa_min_d,
					aa_max_d,
					m_brush_alpha);
				_mm_insert_epi16(mmpix16, pix, 5);
			}
			{
				pix = CircleBrushPixel(
					posX + (i << 3) + 6,
					posY,
					m_center_x,
					m_center_y,
					aa_min_d,
					aa_max_d,
					m_brush_alpha);
				_mm_insert_epi16(mmpix16, pix, 6);
			}
			{
				pix = CircleBrushPixel(
					posX + (i << 3) + 7,
					posY,
					m_center_x,
					m_center_y,
					aa_min_d,
					aa_max_d,
					m_brush_alpha);
				_mm_insert_epi16(mmpix16, pix, 7);
			}

			m_copyOp(pdst, pix);
			pdst += 8;
		}

		int bn = edit_width & 0x7; //width % 8
		LineBltBrush(pdst, posX + edit_width - bn, posY, bn);
	}

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
void IEEMBrush::BltBrushData(
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
		CircleBrushEdit4chOp<_CCopyOp> editOp(copyOp);
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
		CircleBrushEdit1chOp<_CCopyOp> editOp(copyOp);
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
void IEEMBrush::BltBrushData(
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
		CircleBrushEdit4chOp<_CCopyOp> editOp(copyOp);
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
		CircleBrushEdit1chOp<_CCopyOp> editOp(copyOp);
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

	//AddEMBrush(
	//	dst,
	//	center_lpd->x,
	//	center_lpd->y,
	//	ret_brush_rad,
	//	in_aa,
	//	out_aa,
	//	255,
	//	brush_alpha);
}


#endif //_IEEMBRUSH_H_