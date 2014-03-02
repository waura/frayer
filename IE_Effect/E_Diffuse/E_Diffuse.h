#ifndef _E_DIFFUSE_H_
#define _E_DIFFUSE_H_

#include <libImgEdit.h>


class E_Diffuse : public IEffect {
public:
	E_Diffuse();
	virtual ~E_Diffuse();

	virtual bool Init(ImgFile_Ptr pFile);
	virtual void End(ImgFile_Ptr pFile);
	virtual void Edit(ImgFile_Ptr pFile);

	virtual void SetEditDialog(IEffectEditDialog *pEditDialog);

private:
	int rnoise(int a, int b);

	int m_seed;
	int m_range;

	IplImage* m_editImg;
	IplImage* m_editedImg;
	IplImage* m_alphaCh;

	IplImage* m_mask;
	ImgLayer_Ptr m_pEditLayer;
	EditNode* m_pEditNode;
	EditLayerHandle* m_pEditLayerHandle;

	IEffectEditDialog* m_pEditDialog;
};


#endif //_E_DIFFUSE_H_