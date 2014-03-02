#ifndef _VIEW_FLIP_LR_H_
#define _VIEW_FLIP_LR_H_

#include <libImgEdit.h>


class ViewFlipH : public IIECommand {
public:
	ViewFlipH();
	~ViewFlipH(){};

	//
	BOOL Run(ImgEdit* pEdit, void* pvoid);
};


#endif