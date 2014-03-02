#ifndef _MASKSYNTHESIZERMG_H_
#define _MASKSYNTHESIZERMG_H_

#include "IMaskSynthesizer.h"
#include <vector>

#define DEFAULT_MASK_SYNTH_SELECT_INDEX 0

#define IE_MASK_SYNTH_FOURCC(ch0, ch1, ch2, ch3)					\
                ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |	\
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))

typedef IMaskSynthesizer* (*createIEMaskSynthesizer)();
typedef void (*ReleaseIEMaskSynthesizer)(IMaskSynthesizer* synth);

typedef struct _MSYNTH_DATA{
	std::string name;
	IMaskSynthesizer* mSynth;
	HANDLE hLibrary;
} MSYNTH_DATA;

typedef std::vector<MSYNTH_DATA*> MSYNTH_DATA_VEC;

class _EXPORTCPP MaskSynthesizerMG {
public:
	static void SetMSynthDir(const char* path);
	static void SetMSynthDat(const char* path);

	static void Init();
	static void End();

	static void SetDrawModeComboBox(HWND hCombo);
	static const IMaskSynthesizer* GetMaskSynth(int index);
	static const IMaskSynthesizer* GetDefaultMaskSynth();

	static inline uint32_t GetDefaultMaskSynthFourCC(){
		return IE_MASK_SYNTH_FOURCC('a', 'd', 'd', ' ');
	}

	static void BlendSynth(uint32_t fourcc, double opacity,
		IplImageExt* dst, int posX, int posY, int width, int height,
		const IplImageExt* under_img, int ui_startX, int ui_startY,
		const IplImageExt* over_img, int oi_startX, int oi_startY,
		const IplImageExt* mask_ext = NULL, int mi_startX = 0, int mi_startY = 0);

private:

	static std::string m_MSynthDir;
	static std::string m_MSynthDat;
	static MSYNTH_DATA_VEC m_MSynthVec;
};

#endif //_MASKSYNTHESIZERMG_H_