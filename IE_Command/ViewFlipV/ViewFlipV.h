#ifndef _VIEW_FLIP_UD_H_
#define _VIEW_FLIP_UD_H_

#include <libImgEdit.h>


class ViewFlipV : public IIECommand {
public:
	ViewFlipV();
	~ViewFlipV(){};

	//
	BOOL Run(ImgEdit* pEdit, void* pvoid);
};


#endif