#ifndef _E_COLORREVERSE_H_
#define _E_COLORREVERSE_H_

#include <libImgEdit.h>


//êFÇÃîΩì]
class E_ColorReverse : public IEffect {
public:
	E_ColorReverse();
	virtual ~E_ColorReverse();

	virtual bool Init(ImgFile_Ptr pFile);
	virtual void End(ImgFile_Ptr pFile);
	virtual void Edit(ImgFile_Ptr pFile);

	virtual void SetEditDialog(IEffectEditDialog *pEditDialog);

private:

	ImgLayer_Ptr m_pEditLayer;
	EditNode* m_pEditNode;
	EditLayerHandle* m_pEditLayerHandle;

	IplImage* m_mask;

	IEffectEditDialog* m_pEditDialog;
};


class ColorReverseBltOp
{
public:
	inline void operator()(void* dst, const void* src, const void* mask, int width) const
	{
		assert(dst != NULL);
		assert(mask == NULL);

		UCvPixel* pdst = (UCvPixel*) dst;
		UCvPixel* psrc = (UCvPixel*) src;

		int i;
		if(src == NULL){
			memset(dst, 0, sizeof(UCvPixel) * width);
			return;
		}

		for(i=0; i<width; i++){
			//
			pdst->b = ~psrc->b;
			pdst->g = ~psrc->g;
			pdst->r = ~psrc->r;
			pdst->a = psrc->a;

			pdst++;
			psrc++;
		}
	}
};

#endif