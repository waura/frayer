#ifndef _IMGSELECTRELEASE_H_
#define _IMGSELECTRELEASE_H_

#include <libImgEdit.h>


class ImgSelectRelease : public IIECommand {
public:
	ImgSelectRelease();
	~ImgSelectRelease(){};

	//
	BOOL Run(ImgEdit* pEdit, void* pvoid);
};

#endif