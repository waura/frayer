#include "stdafx.h"

#include "MaskSynthesizerMG.h"
#include "OrMaskSynthesizer.h"
#include "AddMaskSynthesizer.h"
#include "SubMaskSynthesizer.h"
#include "CopyMaskSynthesizer.h"
#include "OutputError.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


std::string MaskSynthesizerMG::m_MSynthDir;
std::string MaskSynthesizerMG::m_MSynthDat;

MSYNTH_DATA_VEC MaskSynthesizerMG::m_MSynthVec;

static OrMaskSynthesizer ormSynth;

void MaskSynthesizerMG::SetMSynthDir(const char* path)
{
	m_MSynthDir = path;
}

void MaskSynthesizerMG::SetMSynthDat(const char* path)
{
	m_MSynthDat = path;
}

void MaskSynthesizerMG::Init()
{
	MSYNTH_DATA *ord = new MSYNTH_DATA;
	ord->name = "Or";
	ord->hLibrary = NULL;
	ord->mSynth = &ormSynth;
	m_MSynthVec.push_back(ord);
}

void MaskSynthesizerMG::End()
{
	MSYNTH_DATA_VEC::iterator itr = m_MSynthVec.begin();
	for(; itr != m_MSynthVec.end(); itr++){
		delete (*itr);
	}
	m_MSynthVec.clear();
}

void MaskSynthesizerMG::SetDrawModeComboBox(HWND hCombo)
{
	MSYNTH_DATA_VEC::iterator itr = m_MSynthVec.begin();
	for(; itr != m_MSynthVec.end(); itr++){
		//add combobox item
		::SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)(*itr)->name.c_str());
	}
}

const IMaskSynthesizer* MaskSynthesizerMG::GetMaskSynth(int index)
{
	if(0 <= index && index < m_MSynthVec.size()){
		return m_MSynthVec[index]->mSynth;
	}

	return NULL;
}

const IMaskSynthesizer* MaskSynthesizerMG::GetDefaultMaskSynth()
{
	return (IMaskSynthesizer*) &ormSynth;
}

void MaskSynthesizerMG::BlendSynth(
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
		case IE_MASK_SYNTH_FOURCC('a','d','d',' '):
			{
				LineMaskBlender<AddMaskSynthesizer> lineOp;
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
		case IE_MASK_SYNTH_FOURCC('s','u','b',' '):
			{
				LineMaskBlender<SubMaskSynthesizer> lineOp;
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
		case IE_MASK_SYNTH_FOURCC('c','o','p','y'):
			{
				LineMaskBlender<CopyMaskSynthesizer> lineOp;
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
					"MaskSynthesizerMG::BlendSynth() unknown layer synth fourcc");
			}
			break;
	}
}