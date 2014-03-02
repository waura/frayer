#ifndef _UNDO_H_
#define _UNDO_H_

#include <libImgEdit.h>

class Redo : public IIECommand
{
public:
	Redo();
	~Redo(){};

	//
	BOOL Run(ImgEdit* pEdit, void* pvoid);
};

#endif