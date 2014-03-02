#ifndef _CUTLAYERSELECTIMAGE_H_
#define _CUTLAYERSELECTIMAGE_H_

#include <libImgEdit.h>


class CutLayerSelectImage : public IIECommand {
public:
	CutLayerSelectImage();
	~CutLayerSelectImage(){};

	//
	BOOL Run(ImgEdit* pEdit, void* pvoid);
};


#endif