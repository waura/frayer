#ifndef _E_EDGESOBEL_H_
#define _E_EDGESOBEL_H_

#include <libImgEdit.h>


//Sobelオペレータを使用して、エッジを抽出する。
class E_EdgeSobel : public IEffect{
public:
	E_EdgeSobel();
	virtual ~E_EdgeSobel();

	virtual bool Init(ImgFile_Ptr pFile);
	virtual void End(ImgFile_Ptr pFile);
	virtual void Edit(ImgFile_Ptr pFile);
	virtual void SetEditDialog(IEffectEditDialog *pEditDialog);

private:

	IplImage* m_editImage;
	IplImage* m_grayImage;
	IplImage* m_sobelXImage16;
	IplImage* m_sobelYImage16;
	IplImage* m_sobelImage8;
	IplImage* m_editedImage;
	IplImage* m_alphaCh;

	IplImage* m_mask;
	ImgLayer_Ptr m_pEditLayer;
	EditNode* m_pEditNode;
	EditLayerHandle* m_pEditLayerHandle;
};

#endif