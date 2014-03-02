#ifndef _E_MOTIONBLUR_H_
#define _E_MOTIONBLUR_H_

#include <libImgEdit.h>


class E_MotionBlur : public IEffect {
public:
	E_MotionBlur();
	virtual ~E_MotionBlur();

	virtual bool Init(ImgFile_Ptr pFile);
	virtual void End(ImgFile_Ptr pFile);
	virtual void Edit(ImgFile_Ptr pFile);

	virtual void SetEditDialog(IEffectEditDialog *pEditDialog);

private:
	double m_blur_vec_theta;
	int m_rot;
	int m_size;

	IplImage* m_editImage;
	IplImage* m_editedImage;

	IplImage* m_mask;
	ImgLayer_Ptr m_pEditLayer;
	EditNode* m_pEditNode;
	EditLayerHandle* m_pEditLayerHandle;

	IEffectEditDialog* m_pEditDialog;
};


#endif