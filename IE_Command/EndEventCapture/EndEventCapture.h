#ifndef _END_EVENT_CAPTURE_H_
#define _END_EVNET_CAPTURE_H_

#include <libImgEdit.h>

class EndEventCapture : public IIECommand
{
public:
	EndEventCapture();
	~EndEventCapture();

	//
	BOOL Run(ImgEdit* pEdit, void* pvoid);
};

#endif //_END_EVENT_CAPTURE_H_