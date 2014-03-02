#ifndef _E_VTRFILTER_H_
#define _E_VTRFILTER_H_

#include <libImgEdit.h>


class E_VTRFilter : public IEffect {
public:
	E_VTRFilter();
	virtual ~E_VTRFilter();

	virtual bool Init(ImgFile_Ptr pFile);
	virtual void End(ImgFile_Ptr pFile);
	virtual void Edit(ImgFile_Ptr pFile);

	virtual void SetEditDialog(IEffectEditDialog *pEditDialog);

private:
	int m_l_wid;
	int m_l_det;
	int m_cst;
	int m_gst;

	IplImage* m_editImg;
	IplImage* m_editedImg;
	IplImage* m_alphaCh;

	IplImage* m_mask;
	ImgLayer_Ptr m_pEditLayer;
	EditNode* m_pEditNode;
	EditLayerHandle* m_pEditLayerHandle;

	IEffectEditDialog* m_pEditDialog;
};


#endif //_E_VTRFILTER_H_