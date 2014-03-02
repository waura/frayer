#ifndef _LEVEL_CORRECTION_H_
#define _LEVEL_CORRECTION_H_

#include <libImgEdit.h>

typedef struct _LEVEL_CORR_DATA {
	double gamma;
	int min_level;
	int max_level;
	int min_out_val;
	int max_out_val;
	int edit_channel_index;
	CvMat* BGRA_LUT;
	RECT mask_rect;
	EditNode* pEditNode;
	EditLayerHandle* pEditLayerHandle;
}LEVEL_CORR_DATA, *LPLEVEL_CORR_DATA;

class LevelCorrection : public IIECommand {
public:
	LevelCorrection();
	~LevelCorrection(){};

	bool Init(ImgEdit* pEdit);
	void End(ImgEdit* pEdit);

	//
	BOOL Run(ImgEdit* pEdit, void* pvoid);

	inline IplImage* GetHistogramImg(){
		assert((0 <= m_edit_channel_index) && (m_edit_channel_index < 3));
		return m_histogram_img[ m_edit_channel_index ];
	}

	inline IplImage* GetLevelSldImg(){
		return m_level_sld_img;
	}

	inline IplImage* GetOutputSldImg(){
		return m_output_sld_img;
	}

	void UpdateImage();

	inline void ChangeEditChannel(int index){
		m_edit_channel_index = index;
	}

	void MLButtonDown(HWND hWnd, int x, int y);
	void MMove(HWND hWnd, int x, int y);
	void MLButtonUp(HWND hWnd, int x, int y);

	void RebuildLevelDlg();
	void RebuildOutputDlg();

private:
	//
	HANDLE m_hUpdateThread;
	LEVEL_CORR_DATA m_LCorrData;

	IplImage* m_histogram_img[3];
	IplImage* m_level_sld_img;
	IplImage* m_output_sld_img;

	bool m_is_gamma_dlg;
	bool m_is_max_level_dlg;
	bool m_is_min_level_dlg;
	bool m_is_max_out_dlg;
	bool m_is_min_out_dlg;

	double m_gamma; //0.0 ~ 1.0
	int m_max_level;
	int m_min_level;
	int m_max_out_val;
	int m_min_out_val;

	int m_edit_channel_index;
	CvMat* m_BGRA_LUT;

	RECT m_mask_rect;
	EditNode* m_pEditNode;
	EditLayerHandle* m_pEditLayerHandle;

	ImgFile_Ptr m_pEditFile;
};

#endif //_LEVEL_CORRECTION_H_