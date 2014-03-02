#ifndef _CHANGE_RESOLUTION_H_
#define _CHANGE_RESOLUTION_H_

#include <libImgEdit.h>

class ChangeResolution : public IIECommand {
public:
	ChangeResolution();
	~ChangeResolution(){
	}

	//
	BOOL Run(ImgEdit* pEdit, void* pvoid);
};

#endif //_CHANGE_RESOLUTION_H_