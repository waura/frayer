#ifndef _LAYERSYNTHESIZERMG_H_
#define _LAYERSYNTHESIZERMG_H_

#include <vector>

#include "ILayerSynthesizer.h"
#include "NormalLayerSynthesizer.h"
#include "AlphaMaskLayerSynthesizer.h"
#include "MultLayerSynthesizer.h"
#include "BurnLayerSynthesizer.h"
#include "DarkenLayerSynthesizer.h"
#include "DifferenceLayerSynthesizer.h"
#include "DodgeLayerSynthesizer.h"
#include "HardlightLayerSynthesizer.h"
#include "LightenLayerSynthesizer.h"
#include "OverlayLayerSynthesizer.h"
#include "ScreenLayerSynthesizer.h"
#include "SoftlightLayerSynthesizer.h"
#include "SubALayerSynthesizer.h"
#include "OutputError.h"

#define IE_LAYER_SYNTH_FOURCC(ch0, ch1, ch2, ch3)					\
                ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |	\
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))

//typedef ILayerSynthesizer* (*CreateIELayerSynthesizer)();
//typedef void (*ReleaseIELayerSynthesizer)(ILayerSynthesizer *synth);

typedef struct _LSYNTH_DATA{
	uint32_t fourcc;
	std::string disp_name;
} LSYNTH_DATA;

typedef std::vector<LSYNTH_DATA*> LSYNTH_DATA_VEC;

class _EXPORTCPP LayerSynthesizerMG {
public:

	static void Init();
	static void End();

	/////////////////////////////////
	/*!
	*/
	static void SetDrawModeComboBox(HWND hCombo);

	static void GetLayerSynthDisplayName(uint32_t fourcc, char* dst);
	static uint32_t GetLayerSynthFourCC(int index);
	static int GetLayerSynthIndex(uint32_t fourcc);

	static inline uint32_t GetDefaultLayerSynthFourCC() {
		return IE_LAYER_SYNTH_FOURCC('n','o','r','m');
	}

	static inline uint32_t GetAlphaMaskLayerSynthFourCC() {
		return IE_LAYER_SYNTH_FOURCC('a','m','s','k');
	}

	static void BlendSynth(uint32_t fourcc, double opacity,
		IplImageExt* dst, int posX, int posY, int width, int height,
		const IplImageExt* under_img, int ui_startX, int ui_startY,
		const IplImageExt* over_img, int oi_startX, int oi_startY,
		const IplImageExt* mask_ext, int mi_startX, int mi_startY)
	{
		BlendSynth<NormalCopy>(
			fourcc, opacity,
			dst, posX, posY, width, height,
			under_img, ui_startX, ui_startY,
			over_img, oi_startX, oi_startY,
			mask_ext, mi_startX, mi_startY);
	}

	static void BlendSynth(uint32_t fourcc, double opacity,
		IplImageExt* dst, int posX, int posY, int width, int height,
		const IplImageExt* under_img, int ui_startX, int ui_startY,
		const IplImageExt* over_img, int oi_startX, int oi_startY,
		const IplImage* mask = NULL, int mi_startX = 0, int mi_startY = 0)
	{
		BlendSynth<NormalCopy>(
			fourcc, opacity,
			dst, posX, posY, width, height,
			under_img, ui_startX, ui_startY,
			over_img, oi_startX, oi_startY,
			mask, mi_startX, mi_startY);
	}

	static void BltSynth(uint32_t fourcc, double opacity,
		IplImage* dst, int d_x, int d_y, int width, int height,
		const IplImageExt* src, int s_x, int s_y)
	{
		BltSynth<NormalCopy>(
			fourcc, opacity,
			dst, d_x, d_y, width, height,
			src, s_x, s_y);
	}

	static void BltSynth(uint32_t fourcc, double opacity,
		IplImageExt* dst, int d_x, int d_y, int width, int height,
		const IplImageExt* src, int s_x, int s_y,
		const IplImageExt* mask = NULL, int m_x = 0, int m_y = 0)
	{
		BltSynth<NormalCopy>(
			fourcc, opacity,
			dst, d_x, d_y, width, height,
			src, s_x, s_y,
			mask, m_x, m_y);
	}

	template<class _CCopyOp>
	static inline void BlendSynth(uint32_t fourcc, double opacity,
		IplImageExt* dst, int posX, int posY, int width, int height,
		const IplImageExt* under_img, int ui_startX, int ui_startY,
		const IplImageExt* over_img, int oi_startX, int oi_startY,
		const IplImageExt* mask_ext, int mi_startX, int mi_startY);

	template<class _CCopyOp>
	static void BlendSynth(uint32_t fourcc, double opacity,
		IplImageExt* dst, int posX, int posY, int width, int height,
		const IplImageExt* under_img, int ui_startX, int ui_startY,
		const IplImageExt* over_img, int oi_startX, int oi_startY,
		const IplImage* mask = NULL, int mi_startX = 0, int mi_startY = 0);

	template<class _CCopyOp>
	static void BltSynth(uint32_t fourcc, double opacity,
		IplImage* dst, int d_x, int d_y, int width, int height,
		const IplImageExt* src, int s_x, int s_y);

	template<class _CCopyOp>
	static void BltSynth(uint32_t fourcc, double opacity,
		IplImageExt* dst, int d_x, int d_y, int width, int height,
		const IplImageExt* src, int s_x, int s_y,
		const IplImageExt* mask = NULL, int m_x = 0, int m_y = 0);
private:

	static LSYNTH_DATA_VEC m_LSynthVec;
};


template<class _CCopyOp>
void LayerSynthesizerMG::BlendSynth(
	uint32_t fourcc,
	double opacity,
	IplImageExt* dst,
	int posX,
	int posY,
	int width,
	int height,
	const IplImageExt *under_img,
	int ui_startX,
	int ui_startY,
	const IplImageExt *over_img,
	int oi_startX,
	int oi_startY,
	const IplImageExt *mask_ext,
	int mi_startX,
	int mi_startY)
{
	NormMaskOp normMaskOp;

	switch(fourcc){
		case IE_LAYER_SYNTH_FOURCC('n','o','r','m'):
			{
				LineLayerBlender<NormalLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					normMaskOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask_ext,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('a','m','s','k'):
			{
				LineLayerBlender<AlphaMaskLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					normMaskOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask_ext,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('m','u','l',' '):
			{
				LineLayerBlender<MultLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					normMaskOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask_ext,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('b','u','r','n'):
			{
				LineLayerBlender<BurnLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					normMaskOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask_ext,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('d','a','r','k'):
			{
				LineLayerBlender<DarkenLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					normMaskOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask_ext,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('d','i','f','f'):
			{
				LineLayerBlender<DifferenceLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					normMaskOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask_ext,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('d','o','d','g'):
			{
				LineLayerBlender<DodgeLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					normMaskOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask_ext,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('h','L','i','t'):
			{
				LineLayerBlender<HardlightLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					normMaskOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask_ext,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('l','i','t','e'):
			{
				LineLayerBlender<LightenLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					normMaskOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask_ext,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('o','v','e','r'):
			{
				LineLayerBlender<OverlayLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					normMaskOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask_ext,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('s','c','r','n'):
			{
				LineLayerBlender<ScreenLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					normMaskOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask_ext,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('s','L','i','t'):
			{
				LineLayerBlender<SoftlightLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					normMaskOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask_ext,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('s','u','b','A'):
			{
				SPLineLayerBlender<SubALayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					normMaskOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask_ext,
					mi_startX, mi_startY);
			}
			break;
		default:
			{
				OutputError::PushLog(LOG_LEVEL::_ERROR,
					"LayerSynthesizerMG::BlendSynth() unknown layer synth fourcc");
			}
			break;
	}
}

template<class _CCopyOp>
void LayerSynthesizerMG::BlendSynth(
	uint32_t fourcc,
	double opacity,
	IplImageExt* dst,
	int posX,
	int posY,
	int width,
	int height,
	const IplImageExt *under_img,
	int ui_startX,
	int ui_startY,
	const IplImageExt *over_img,
	int oi_startX,
	int oi_startY,
	const IplImage *mask,
	int mi_startX,
	int mi_startY)
{
	switch(fourcc){
		case IE_LAYER_SYNTH_FOURCC('n','o','r','m'):
			{
				LineLayerBlender<NormalLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('a','m','s','k'):
			{
				LineLayerBlender<AlphaMaskLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('m','u','l',' '):
			{
				LineLayerBlender<MultLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('b','u','r','n'):
			{
				LineLayerBlender<BurnLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('d','a','r','k'):
			{
				LineLayerBlender<DarkenLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('d','i','f','f'):
			{
				LineLayerBlender<DifferenceLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('d','o','d','g'):
			{
				LineLayerBlender<DodgeLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('h','L','i','t'):
			{
				LineLayerBlender<HardlightLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('l','i','t','e'):
			{
				LineLayerBlender<LightenLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('o','v','e','r'):
			{
				LineLayerBlender<OverlayLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('s','c','r','n'):
			{
				LineLayerBlender<ScreenLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('s','L','i','t'):
			{
				LineLayerBlender<SoftlightLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask,
					mi_startX, mi_startY);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('s','u','b','A'):
			{
				SPLineLayerBlender<SubALayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlend(
					lineOp,
					posX, posY,
					width, height,
					under_img,
					ui_startX, ui_startY,
					over_img,
					oi_startX, oi_startY,
					mask,
					mi_startX, mi_startY);
			}
			break;
		default:
			{
				OutputError::PushLog(LOG_LEVEL::_ERROR,
					"LayerSynthesizerMG::BlendSynth() unknown layer synth fourcc");
			}
			break;
	}
}

template<class _CCopyOp>
void LayerSynthesizerMG::BltSynth(
	uint32_t fourcc,
	double opacity,
	IplImage *dst,
	int d_x,
	int d_y,
	int width,
	int height,
	const IplImageExt *src,
	int s_x,
	int s_y)
{
	NormMaskOp normMaskOp;

	switch(fourcc){
		case IE_LAYER_SYNTH_FOURCC('n','o','r','m'):
			{
				LineLayerSynthesizer<NormalLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity( opacity );
				IplImageExt::OperateBlt(
					lineOp,
					normMaskOp,
					dst, d_x, d_y, width, height,
					src, s_x, s_y);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('a','m','s','k'):
			{
				LineLayerSynthesizer<AlphaMaskLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity( opacity );
				IplImageExt::OperateBlt(
					lineOp,
					normMaskOp,
					dst, d_x, d_y, width, height,
					src, s_x, s_y);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('m','u','l',' '):
			{
				LineLayerSynthesizer<MultLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity( opacity );
				IplImageExt::OperateBlt(
					lineOp,
					normMaskOp,
					dst, d_x, d_y, width, height,
					src, s_x, s_y);			}
			break;
		case IE_LAYER_SYNTH_FOURCC('b','u','r','n'):
			{
				LineLayerSynthesizer<BurnLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity( opacity );
				IplImageExt::OperateBlt(
					lineOp,
					normMaskOp,
					dst, d_x, d_y, width, height,
					src, s_x, s_y);			}
			break;
		case IE_LAYER_SYNTH_FOURCC('d','a','r','k'):
			{
				LineLayerSynthesizer<DarkenLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity( opacity );
				IplImageExt::OperateBlt(
					lineOp,
					normMaskOp,
					dst, d_x, d_y, width, height,
					src, s_x, s_y);			}
			break;
		case IE_LAYER_SYNTH_FOURCC('d','i','f','f'):
			{
				LineLayerSynthesizer<DifferenceLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity( opacity );
				IplImageExt::OperateBlt(
					lineOp,
					normMaskOp,
					dst, d_x, d_y, width, height,
					src, s_x, s_y);			}
			break;
		case IE_LAYER_SYNTH_FOURCC('d','o','d','g'):
			{
				LineLayerSynthesizer<DodgeLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity( opacity );
				IplImageExt::OperateBlt(
					lineOp,
					normMaskOp,
					dst, d_x, d_y, width, height,
					src, s_x, s_y);			}
			break;
		case IE_LAYER_SYNTH_FOURCC('h','L','i','t'):
			{
				LineLayerSynthesizer<HardlightLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity( opacity );
				IplImageExt::OperateBlt(
					lineOp,
					normMaskOp,
					dst, d_x, d_y, width, height,
					src, s_x, s_y);			}
			break;
		case IE_LAYER_SYNTH_FOURCC('l','i','t','e'):
			{
				LineLayerSynthesizer<LightenLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity( opacity );
				IplImageExt::OperateBlt(
					lineOp,
					normMaskOp,
					dst, d_x, d_y, width, height,
					src, s_x, s_y);			}
			break;
		case IE_LAYER_SYNTH_FOURCC('o','v','e','r'):
			{
				LineLayerSynthesizer<OverlayLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity( opacity );
				IplImageExt::OperateBlt(
					lineOp,
					normMaskOp,
					dst, d_x, d_y, width, height,
					src, s_x, s_y);			}
			break;
		case IE_LAYER_SYNTH_FOURCC('s','c','r','n'):
			{
				LineLayerSynthesizer<ScreenLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity( opacity );
				IplImageExt::OperateBlt(
					lineOp,
					normMaskOp,
					dst, d_x, d_y, width, height,
					src, s_x, s_y);			}
			break;
		case IE_LAYER_SYNTH_FOURCC('s','L','i','t'):
			{
				LineLayerSynthesizer<SoftlightLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity( opacity );
				IplImageExt::OperateBlt(
					lineOp,
					normMaskOp,
					dst, d_x, d_y, width, height,
					src, s_x, s_y);			}
			break;
		default:
			{
				OutputError::PushLog(LOG_LEVEL::_ERROR,
					"LayerSynthesizerMG::BltSynth() unknown layer synth fourcc");
			}
			break;
	}
}

template<class _CCopyOp>
void LayerSynthesizerMG::BltSynth(
	uint32_t fourcc,
	double opacity,
	IplImageExt *dst,
	int d_x,
	int d_y,
	int width,
	int height,
	const IplImageExt *src,
	int s_x,
	int s_y,
	const IplImageExt* mask,
	int m_x,
	int m_y)
{
	NormMaskOp normMaskOp;

	switch(fourcc){
		case IE_LAYER_SYNTH_FOURCC('n','o','r','m'):
			{
				LineLayerSynthesizer<NormalLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlt(
					lineOp,
					normMaskOp,
					d_x, d_y, width, height,
					src, s_x, s_y,
					mask, m_x, m_y);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('a','m','s','k'):
			{
				LineLayerSynthesizer<AlphaMaskLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlt(
					lineOp,
					normMaskOp,
					d_x, d_y, width, height,
					src, s_x, s_y,
					mask, m_x, m_y);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('m','u','l',' '):
			{
				LineLayerSynthesizer<MultLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlt(
					lineOp,
					normMaskOp,
					d_x, d_y, width, height,
					src, s_x, s_y,
					mask, m_x, m_y);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('b','u','r','n'):
			{
				LineLayerSynthesizer<BurnLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlt(
					lineOp,
					normMaskOp,
					d_x, d_y, width, height,
					src, s_x, s_y,
					mask, m_x, m_y);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('d','a','r','k'):
			{
				LineLayerSynthesizer<DarkenLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlt(
					lineOp,
					normMaskOp,
					d_x, d_y, width, height,
					src, s_x, s_y,
					mask, m_x, m_y);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('d','i','f','f'):
			{
				LineLayerSynthesizer<DifferenceLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlt(
					lineOp,
					normMaskOp,
					d_x, d_y, width, height,
					src, s_x, s_y,
					mask, m_x, m_y);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('d','o','d','g'):
			{
				LineLayerSynthesizer<DodgeLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlt(
					lineOp,
					normMaskOp,
					d_x, d_y, width, height,
					src, s_x, s_y,
					mask, m_x, m_y);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('h','L','i','t'):
			{
				LineLayerSynthesizer<HardlightLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlt(
					lineOp,
					normMaskOp,
					d_x, d_y, width, height,
					src, s_x, s_y,
					mask, m_x, m_y);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('l','i','t','e'):
			{
				LineLayerSynthesizer<LightenLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlt(
					lineOp,
					normMaskOp,
					d_x, d_y, width, height,
					src, s_x, s_y,
					mask, m_x, m_y);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('o','v','e','r'):
			{
				LineLayerSynthesizer<OverlayLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlt(
					lineOp,
					normMaskOp,
					d_x, d_y, width, height,
					src, s_x, s_y,
					mask, m_x, m_y);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('s','c','r','n'):
			{
				LineLayerSynthesizer<ScreenLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlt(
					lineOp,
					normMaskOp,
					d_x, d_y, width, height,
					src, s_x, s_y,
					mask, m_x, m_y);
			}
			break;
		case IE_LAYER_SYNTH_FOURCC('s','L','i','t'):
			{
				LineLayerSynthesizer<SoftlightLayerSynthesizer, _CCopyOp> lineOp;
				lineOp.SetOpacity(opacity);
				dst->OperateBlt(
					lineOp,
					normMaskOp,
					d_x, d_y, width, height,
					src, s_x, s_y,
					mask, m_x, m_y);
			}
			break;
		default:
			{
				OutputError::PushLog(LOG_LEVEL::_ERROR, 
					"LayerSynthesizerMG::BltSynth() unknown layer synth fourcc");
			}
			break;
	}
}

#endif