#ifndef _E_GLOW_H_
#define _E_GLOW_H_

#include "IEffect.h"
#include "ImgFileHandleMG.h"


class E_Glow : public IEffect {
public:
	E_Glow();
	virtual ~E_Glow();

	virtual bool Init(ImgFile_Ptr pFile);
	virtual void End(ImgFile_Ptr pFile);
	virtual void Edit(ImgFile_Ptr pFile);

	virtual void SetEditDialog(IEffectEditDialog *pEditDialog);

private:
	int m_bs;
};


#endif //_E_GLOW_H_