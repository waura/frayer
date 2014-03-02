#ifndef _UNDO_H_
#define _UNDO_H_

#include <libImgEdit.h>

class Undo : public IIECommand {
public:
	Undo();
	~Undo(){};

	//
	BOOL Run(ImgEdit* pEdit, void* pvoid);
};


#endif //_UNDO_H_