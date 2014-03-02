#ifndef _E_BINARY_H_
#define _E_BINARY_H_

#include <libImgEdit.h>


//âÊëúÇ2ílâªÇ∑ÇÈÅB
class E_Binary_Threshold : public IEffect{
public:
	E_Binary_Threshold();
	virtual ~E_Binary_Threshold();

	virtual bool Init(ImgFile_Ptr pFile);
	virtual void End(ImgFile_Ptr pFile);
	virtual void Edit(ImgFile_Ptr pFile);

	virtual void SetEditDialog(IEffectEditDialog *pEditDialog);

private:
	int m_threshold;
	IplImage* m_editImg;
	IplImage* m_grayImg;
	IplImage* m_binaryImg;
	IplImage* m_editedImg;
	IplImage* m_alphaCh;

	IplImage* m_mask;
	ImgLayer_Ptr m_pEditLayer;
	EditNode* m_pEditNode;
	EditLayerHandle* m_pEditLayerHandle;

	IEffectEditDialog* m_pEditDialog;
};

#endif