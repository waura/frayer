#ifndef _START_EVENT_CAPTURE_H_
#define _START_EVNET_CAPTURE_H_

#include <libImgEdit.h>

class StartEventCapture : public IIECommand
{
public:
	StartEventCapture();
	~StartEventCapture();

	//
	BOOL Run(ImgEdit* pEdit, void* pvoid);
};

#endif //_START_EVENT_CAPTURE_H_