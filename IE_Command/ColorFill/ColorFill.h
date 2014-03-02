#ifndef _COLOR_FILL_H_
#define _COLOR_FILL_H_

#include <libImgEdit.h>


class ColorFill : public IIECommand {
public:
	ColorFill();
	~ColorFill(){
	}

	//
	BOOL Run(ImgEdit* pEdit, void* pvoid);

private:
	EditLayerHandle* m_editHandle;
};


#endif