#ifndef _TONE_CURVE_H_
#define _TONE_CURVE_H_

#include <libImgEdit.h>


typedef struct _TONE_CURVE_DATA {
	CvMat* BGRA_LUT;
	RECT mask_rect;
	EditNode* pEditNode;
	EditLayerHandle* pEditLayerHandle;
}TONE_CURVE_DATA, *LPTONE_CURVE_DATA;

class ToneCurve : public IIECommand {
public:
	ToneCurve();
	~ToneCurve(){};

	bool Init(ImgEdit* pEdit);
	void End(ImgEdit* pEdit);

	//
	BOOL Run(ImgEdit* pEdit, void* pvoid);

	inline IplImage* GetGraphImg(){
		return m_graph_img;
	}

	/*!
		(x,y)付近にあるsrc_pointのインデックスを返す
		src_pointが見つからなければ -1 を返す。
		@param[in] x
		@param[in] y
	*/
	int GetSrcPoint(int x, int y);

	/*!
		点(x, y)をm_src_pointsに追加する。
		@return 追加したsrc_pointのインデックスを返す
	*/
	int AddPoint(int x, int y);

	/*!
		point_indexの点を(x,y)に変更する。
		@return point_indexの点の変更後のインデックス
	*/
	int EditPoint(int point_index, int x, int y);

	void RebuildGraph();

	int FuncToneCurve(int n, int x, const CvPoint2D64f* src_points, const CvMat* c_mat);

	void UpdateImage();

	inline void ChangeEditChannel(int index){
		m_edit_channel_index = index;
	}

private:
	//
	HANDLE m_hUpdateThread;
	TONE_CURVE_DATA m_TCData;

	IplImage* m_graph_img;

	int m_edit_channel_index;
	int m_num_of_src_points;
	CvPoint2D64f* m_src_points;
	CvMat* m_BGRA_LUT;

	RECT m_mask_rect;
	EditNode* m_pEditNode;
	EditLayerHandle* m_pEditLayerHandle;

	ImgFile_Ptr m_pEditFile;
};


#endif