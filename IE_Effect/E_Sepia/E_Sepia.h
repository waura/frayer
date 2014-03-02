#ifndef _IE_SEPIA_H_
#define _IE_SEPIA_H_

#include <libImgEdit.h>


//画像をセピア調にする。
class E_Sepia : public IEffect{
public:
	E_Sepia();
	virtual ~E_Sepia();

	virtual bool Init(ImgFile_Ptr pFile);
	virtual void End(ImgFile_Ptr pFile);
	virtual void Edit(ImgFile_Ptr pFile);

	virtual void SetEditDialog(IEffectEditDialog *editWnd);

private:
	int h_;
	int s_;

	IplImage *m_editImage;
	IplImage *m_editedImage;
	IplImage *m_hsvImage;
	IplImage *m_mergeImage;
	IplImage *m_hueImage;
	IplImage *m_saturationImage;
	IplImage *m_valueImage;
	IplImage *m_alphaCh;

	IplImage* m_mask;
	ImgLayer_Ptr m_pEditLayer;
	EditNode* m_pEditNode;
	EditLayerHandle* m_pEditLayerHandle;

	IEffectEditDialog* m_pEditDialog;
};

#endif
