#ifndef _IMGFILEHANDLE_H_
#define _IMGFILEHANDLE_H_

#include "LibIEDllHeader.h"

#include <string.h>

class ImgLayer;
class ImgLayerGroup;
class EditNode;

enum{
	MAX_IMG_FILE_HANDLE_NAME = 256,
};

class _EXPORTCPP ImgFileHandle
{
public:
	ImgFileHandle(){
		isValid = true;
	}
	virtual ~ImgFileHandle(){};

	virtual bool Init(ImgFile_weakPtr wpFile){
		return true;
	}

	virtual bool Do(ImgFile_weakPtr wpFile)=0;
	virtual void Redo(ImgFile_weakPtr wpFile)=0;
	virtual void Undo(ImgFile_weakPtr wpFile)=0;

	bool isValid;
	char handle_name[MAX_IMG_FILE_HANDLE_NAME];
};

#endif //_IMGFILEHANDLE_H_