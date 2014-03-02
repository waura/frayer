#ifndef _E_EDGELAPLACIAN_H_
#define _E_EDGELAPLACIAN_H_

#include <libImgEdit.h>


//Laplacianオペレータを使用して、エッジを抽出する
class E_EdgeLaplacian : public IEffect{
public:
	E_EdgeLaplacian();
	virtual ~E_EdgeLaplacian();

	virtual bool Init(ImgFile_Ptr pFile);
	virtual void End(ImgFile_Ptr pFile);
	virtual void Edit(ImgFile_Ptr pFile);

	virtual void SetEditDialog(IEffectEditDialog *pEditDialog);

private:

	IplImage* m_editImage;
	IplImage* m_grayImage;
	IplImage* m_laplaceImage16;
	IplImage* m_laplaceImage8;
	IplImage* m_editedImage;
	IplImage* m_alphaCh;

	IplImage* m_mask;
	ImgLayer_Ptr m_pEditLayer;
	EditNode* m_pEditNode;
	EditLayerHandle* m_pEditLayerHandle;
};

#endif