#ifndef _CHANGE_CANVAS_SIZE_H_
#define _CHANGE_CANVAS_SIZE_H_

#include <libImgEdit.h>

class ChangeCanvasSize : public IIECommand {
public:
	ChangeCanvasSize();
	~ChangeCanvasSize(){
	}

	//
	BOOL Run(ImgEdit* pEdit, void* pvoid);
};

#endif //_CHANGE_CANVAS_SIZE_H_