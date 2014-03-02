#ifndef _COLOR_BALANCE_H_
#define _COLOR_BALANCE_H_

#include <libImgEdit.h>

typedef enum
{
	IE_SHADOWS=0,
	IE_MIDTONES=1,
	IE_HIGHLIGHTS=2,
} IETransferMode;

typedef struct _COLOR_BALANCE_DATA {
	bool is_preserve_luminosity;
	int r_edit_vals[3];
	int g_edit_vals[3];
	int b_edit_vals[3];
	double* r_transfer[3];
	double* g_transfer[3];
	double* b_transfer[3];
	CvMat* BGRA_LUT;
	RECT mask_rect;
	EditNode* pEditNode;
	EditLayerHandle* pEditLayerHandle;
}COLOR_BALANCE_DATA, *LPCOLOR_BALANCE_DATA;

class ColorBalance : public IIECommand {
public:
	ColorBalance();
	~ColorBalance(){};

	bool Init(ImgEdit* pEdit);
	void End(ImgEdit* pEdit);

	//
	BOOL Run(ImgEdit* pEdit, void* pvoid);

	void UpdateImage();

	bool m_is_preserve_luminosity;
	int m_r_edit_vals[3];
	int m_g_edit_vals[3];
	int m_b_edit_vals[3];

private:
	//
	HANDLE m_hUpdateThread;
	COLOR_BALANCE_DATA m_CBData;

	double m_shadows_add[256];
	double m_shadows_sub[256];
	double m_midtones_add[256];
	double m_midtones_sub[256];
	double m_highlights_add[256];
	double m_highlights_sub[256];

	CvMat* m_BGRA_LUT;

	RECT m_mask_rect;
	EditNode* m_pEditNode;
	EditLayerHandle* m_pEditLayerHandle;

	ImgFile_Ptr m_pEditFile;
};

#endif //_COLOR_BALANCE_H_