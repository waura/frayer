#ifndef _FREEDEFORMATION_H_
#define _FREEDEFORMATION_H_

#include <libImgEdit.h>


typedef struct _POINT_INDEX {
	int row;
	int col;
} POINT_INDEX;

typedef struct _DEFORM_DATA {
	ImgFile_Ptr pEditFile;
	IplImage* src_img;
	IplImage* src_mask;
	EditLayerHandle* pCutLayerHandle;
	EditMaskHandle* pCutMaskHandle;
	EditNode* pEditNode;
	EditLayerHandle* pEditLayerHandle;
	EditNode* pEditMaskNode;
	EditMaskHandle* pEditMaskHandle;
	CvPoint2D64f cpoints[4]; //コーナーの点
	RECT to_rect;
	RECT pre_to_rect;
} DEFORM_DATA, *LPDEFORM_DATA;

class FreeDeformation : public IIECommand
{
public:
	FreeDeformation();
	~FreeDeformation();

	//
	IE_INPUT_DATA_FLAG GetInputDataFlag(){ return IE_INPUT_DATA_FLAG::IMAGE_POS; }

	//
	HBITMAP GetButtonImg(){ return NULL; };

	//初期化処理
	bool Init(ImgEdit* pEdit);
	//終了処理
	void End(ImgEdit* pEdit);

	BOOL Run(ImgEdit* pEdit, void* pvoid);
	void OnSelect();
	void OnDraw(HDC hdc);
	
	void OnMouseLButtonDown(UINT nFlags, const LPIE_INPUT_DATA lpd);
	void OnMouseLDrag(UINT nFlags, const LPIE_INPUT_DATA lpd);
	void OnMouseLButtonUp(UINT nFlags, const LPIE_INPUT_DATA lpd);

	void OnKeyboardDown(UINT nChar, UINT optChar);

private:

	void FinishCommand();

	//変形処理
	void Deformation();

	void DrawPointRect(HDC hdc, const CvPoint2D64f* pcvPoint);
	bool isOnPointRect(const CvPoint2D64f* pcvPoint, const CvPoint2D64f* pMouse);

	void NormalDrag(const LPIE_INPUT_DATA lpd);
	void ControlDrag(const LPIE_INPUT_DATA lpd);
	void RotationDrag(const LPIE_INPUT_DATA lpd);

	bool m_isFinish; //コマンド終了判定
	
	HPEN m_hPen;
	
	//
	HANDLE m_hDeformThread;
	DEFORM_DATA m_DeformData;

	bool m_isClickOnInsideArea;
	bool m_isClickOnExteriorArea;

	UINT m_DragFlags;
	POINT_INDEX m_SelectPointIndex;
	POINT m_before_mouse_pt;
	POINT m_start_drag_mouse_pt;

	CvPoint2D64f m_DragPoint[3][3];

	IplImage* m_srcImg;
	IplImage* m_srcMask;

	EditNode* m_pEditNode;
	EditLayerHandle* m_pEditLayerHandle;

	EditNode* m_pEditMaskNode;
	EditMaskHandle* m_pEditMaskHandle;

	EditLayerHandle* m_pCutLayerHandle;
	EditMaskHandle* m_pCutMaskHandle;

	StackHandle* m_pStackHandle;

	ImgLayer_Ptr m_pEditLayer;
	ImgFile_Ptr m_pEditFile;
};

#endif //_FREEDEFORMATION_H_