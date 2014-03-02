#ifndef _NULL_HANDLE_H_
#define _NULL_HANDLE_H_

#include "ImgFileHandle.h"

class _EXPORTCPP NullHandle : public ImgFileHandle
{
public:
	NullHandle(){
	}
	~NullHandle(){
	}

	bool Do(ImgFile_weakPtr wpFile){
		return true;
	}
	void Redo(ImgFile_weakPtr wpFile){
	}
	void Undo(ImgFile_weakPtr wpFile){
	}
};

#endif //_NULL_HANDLE_H_