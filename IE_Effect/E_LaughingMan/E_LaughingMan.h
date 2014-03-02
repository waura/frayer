#ifndef _E_LAUGH_MAN_H_
#define _E_LAUGH_MAN_H_

#include <libImgEdit.h>


class E_LaughingMan : public IEffect {
public:
	E_LaughingMan();
	virtual ~E_LaughingMan();

	virtual bool Init(ImgFile_Ptr pFile);
	virtual void End(ImgFile_Ptr pFile);
	virtual void Edit(ImgFile_Ptr pFile);

	virtual void SetEditDialog(IEffectEditDialog *pEditDialog);

private:
	CvSeq* m_faces;
	CvMemStorage* m_storage;
	CvHaarClassifierCascade* m_cascade;

	IplImage* laughman;

	ImgLayer_Ptr m_pEditLayer;
	EditNode* m_pEditNode;
	EditLayerHandle* m_pEditLayerHandle;
};

#endif