#ifndef _E_BILATERALFILTER_H_
#define _E_BILATERALFILTER_H_

#include <libImgEdit.h>


//ÉGÉbÉWï€éùïΩääâª
class E_BilateralFilter : public IEffect {
public:
	E_BilateralFilter();
	virtual ~E_BilateralFilter();
	
	virtual bool Init(ImgFile_Ptr pFile);
	virtual void End(ImgFile_Ptr pFile);
	virtual void Edit(ImgFile_Ptr pFile);

	virtual void SetEditDialog(IEffectEditDialog *pEditDialog);

private:
	int m_v_sig;
	int m_dis_sig;

	IplImage* m_mask;
	IplImage* m_editImg;
	IplImage* m_editedImg;
	IplImage* m_editedBGRAImg;
	ImgLayer_Ptr m_pEditLayer;
	EditNode* m_pEditNode;
	EditLayerHandle* m_pEditLayerHandle;

	IEffectEditDialog* m_pEditDialog;
};


#endif