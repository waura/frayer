#ifndef _IEEVENTCAPTURE_H_
#define _IEEVENTCAPTURE_H_

#include "LibIEDllHeader.h"
#include "IETimer.h"
#include "InputData.h"

class ImgEdit;

enum IE_EVENT_ID {
	MOUSE_LDOWN = 0,
	MOUSE_LDRAG,
	MOUSE_LUP,
	MOUSE_RDOWN,
	MOUSE_RUP,
	KEY_DOWN,
	KEY_UP,
};

class _EXPORTCPP IEEventCapture
{
public:
	IEEventCapture();
	~IEEventCapture();

	void StartCapture(const char* capture_file_path);
	void EndCapture();

	void PlayCapture(const char* capture_file_path, ImgEdit* pImgEdit);
	void PlayCaptureAsync(const char* capture_file_path, ImgEdit* pImgEdit);

	bool IsCapturing()
	{
		return (m_capture_fp != NULL);
	}

	void OnIdle();

	void OnMouseLButtonDown(UINT nFlags, LPIE_INPUT_DATA lpd);
	void OnMouseLDrag(UINT nFlags, LPIE_INPUT_DATA lpd);
	void OnMouseLButtonUp(UINT nFlags, LPIE_INPUT_DATA lpd);

	void OnMouseRButtonDown(UINT nFlags, LPPOINT lps);
	void OnMouseRButtonUp(UINT nFlags, LPPOINT lps);

	void OnKeyboardDown(UINT nChar, UINT optChar);
	void OnKeyboardUp(UINT nChar, UINT optChar);

	void OnRunCommand(int cm_id, void* pvoid);

	inline void LockCaptureFile(){
#ifdef _WIN32
		::EnterCriticalSection(&m_cs_capture_fp);
#endif
	}

	inline void UnlockCaptureFile(){
#ifdef _WIN32
		::LeaveCriticalSection(&m_cs_capture_fp);
#endif
	}

private:

	void WriteEventID(FILE* fp, uint16_t id, double diff_time);
	void ReadEventID(FILE* fp, uint16_t* id, double* diff_time);

	FILE* m_capture_fp;
	IETimer m_timer;

#ifdef _WIN32
	CRITICAL_SECTION m_cs_capture_fp;
#endif //_WIN32
};

#endif //_IEEVENTCAPTURE_H_