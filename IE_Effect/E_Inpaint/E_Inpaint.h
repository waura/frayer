#ifndef _E_INPAINT_H_
#define _E_INPAINT_H_

#include <libImgEdit.h>


//
class E_Inpaint : public IEffect {
public:
	E_Inpaint();
	virtual ~E_Inpaint();

	virtual bool Init(ImgFile_Ptr pFile);
	virtual void End(ImgFile_Ptr pFile);
	virtual void Edit(ImgFile_Ptr pFile);

	virtual void SetEditDialog(IEffectEditDialog *pEditDialog);

private:
	int m_inpaintRadius;
	IplImage* m_editImg;
	IplImage* m_editedImg;
	IplImage* m_editedBGRAImg;

	IplImage* m_mask;
	ImgLayer_Ptr m_pEditLayer;
	EditNode* m_pEditNode;
	EditLayerHandle* m_pEditLayerHandle;

	IEffectEditDialog* m_pEditDialog;
};

#endif