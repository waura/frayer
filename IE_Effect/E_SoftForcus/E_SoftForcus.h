#ifndef _E_GAUSSIANBLUR_H_
#define _E_GAUSSIANBLUR_H_

#include <libImgEdit.h>


class E_SoftForcus : public IEffect {
public:
	E_SoftForcus();
	virtual ~E_SoftForcus();

	virtual bool Init(ImgFile_Ptr pFile);
	virtual void End(ImgFile_Ptr pFile);
	virtual void Edit(ImgFile_Ptr pFile);

	virtual void SetEditDialog(IEffectEditDialog *pEditDialog);

private:
	int m_ef;
	int m_rate;

	IplImage* m_editImg;
	IplImage* m_editedImg;
	IplImage* m_alphaCh;

	IplImage* m_mask;
	ImgLayer_Ptr m_pEditLayer;
	EditNode* m_pEditNode;
	EditLayerHandle* m_pEditLayerHandle;

	IEffectEditDialog* m_pEditDialog;
};


#endif //_E_GAUSSIANBLUR