#ifndef _E_SATURATION_VALUE_H_
#define _E_SATURATION_VALUE_H_

#include <libImgEdit.h>


class E_Saturation_Value : public IEffect {
public:
	E_Saturation_Value();
	virtual ~E_Saturation_Value();

	virtual bool Init(ImgFile_Ptr pFile);
	virtual void End(ImgFile_Ptr pFile);
	virtual void Edit(ImgFile_Ptr pFile);

	virtual void SetEditDialog(IEffectEditDialog *editWnd);

private:
	int s_;
	int v_;

	IplImage* m_editImage;
	IplImage* m_editedImage;
	IplImage* m_hsvImage;
	IplImage* m_hueImage;
	IplImage* m_saturationImage;
	IplImage* m_valueImage;
	IplImage* m_addData;

	IplImage* m_mask;
	ImgLayer_Ptr m_pEditLayer;
	EditNode* m_pEditNode;
	EditLayerHandle* m_pEditLayerHandle;

	IEffectEditDialog* m_pEditDialog;
};

#endif