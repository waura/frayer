#ifndef _PLAY_EVENT_CAPTURE_H_
#define _PLAY_EVNET_CAPTURE_H_

#include <libImgEdit.h>

class PlayEventCapture : public IIECommand
{
public:
	PlayEventCapture();
	~PlayEventCapture();

	//
	BOOL Run(ImgEdit* pEdit, void* pvoid);
};

#endif //_Play_EVENT_CAPTURE_H_