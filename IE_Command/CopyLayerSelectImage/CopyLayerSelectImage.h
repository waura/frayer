#ifndef _COPYLAYERSELECTIMAGE_H_
#define _COPYLAYERSELECTIMAGE_H_

#include <libImgEdit.h>


class CopyLayerSelectImage : public IIECommand {
public:
	CopyLayerSelectImage();
	~CopyLayerSelectImage(){};

	//
	BOOL Run(ImgEdit* pEdit, void* pvoid);
};

#endif //_COPYLAYERSELECTIMAGE_H_