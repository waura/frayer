#ifndef _IMGSELECTALL_H_
#define _IMGSELECTALL_H_

#include <libImgEdit.h>


class ImgSelectAll : public IIECommand {
public:
	ImgSelectAll();
	~ImgSelectAll(){};

	//
	BOOL Run(ImgEdit* pEdit, void* pvoid);
};

#endif //_IMGSELECTALL_H_