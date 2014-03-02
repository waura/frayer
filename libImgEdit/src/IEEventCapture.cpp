#include "stdafx.h"

#include "IEEventCapture.h"
#include "IEFileIOCommon.h"

#include "ImgEdit.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


IEEventCapture::IEEventCapture()
{
	m_capture_fp = NULL;
#ifdef _WIN32
	::InitializeCriticalSection(&m_cs_capture_fp);
#endif
}

IEEventCapture::~IEEventCapture()
{
	if (IsCapturing()) {
		EndCapture();
	}
}

void IEEventCapture::StartCapture(const char *capture_file_path)
{
	OutputError::PushLog(LOG_LEVEL::_INFO, "Start Event Capture");

	if (m_capture_fp) {
		EndCapture();
	}

	if ((m_capture_fp = fopen(capture_file_path, "wb")) == NULL) {
		OutputError::PushLog(
			LOG_LEVEL::_ERROR,
			"on StartCapture, can't open ",
			capture_file_path);
		return;
	}

	LockCaptureFile();

	size_t nfwrite = fwrite("FYED", 1, 4, m_capture_fp);
	if (nfwrite < 4) {
		OutputError::PushLog(LOG_LEVEL::_ERROR, "start capture error: fwrite");
		UnlockCaptureFile();
		return;
	}
	Write2ByteLE(m_capture_fp, 1); //version

	UnlockCaptureFile();

	m_timer.restart();
}

void IEEventCapture::EndCapture()
{
	OutputError::PushLog(LOG_LEVEL::_INFO, "End Event Capture");

	if (IsCapturing()) {
		LockCaptureFile();

		fclose(m_capture_fp);
		m_capture_fp = NULL;

		UnlockCaptureFile();
	}
}

void IEEventCapture::PlayCapture(const char *capture_file_path, ImgEdit *pImgEdit)
{
	FILE* fp;
	if ((fp = fopen(capture_file_path, "rb")) == NULL) {
		OutputError::PushLog(
			LOG_LEVEL::_ERROR,
			"on PlayCapture, can't open ",
			capture_file_path);
	}

	long start_file_pos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	long end_file_pos = ftell(fp);
	long file_size = end_file_pos - start_file_pos;
	fseek(fp, 0, SEEK_SET);

	char signature[4];
	if (fread(signature, 4, 1, fp) < 1) {
		OutputError::PushLog(LOG_LEVEL::_ERROR, "play capture error: fread");
		return;
	}
	if (memcmp(signature, "FYED", 4) != 0) {
		OutputError::PushLog(LOG_LEVEL::_ERROR, "play capture error: format invalid");
		return;
	}

	uint16_t version;
	uint16_t sub_version;
	version = Read2ByteLE(fp);
	if (version != 1) {
		OutputError::PushLog(LOG_LEVEL::_ERROR, "play capture error: don't suppor version");
		return;
	}

	uint16_t event_id;
	double diff_time;
	uint32_t data_size;
	while (ftell(fp) != end_file_pos) {
		ReadEventID(fp, &event_id, &diff_time);

		data_size = Read4ByteLE(fp);
		int sdata_pos = ftell(fp);

		switch (event_id) {
			case IE_EVENT_ID::MOUSE_LDOWN:
				{
					UINT nFlags = Read4ByteLE(fp);
					IE_INPUT_DATA input_data;
					ReadBuffer(fp, &input_data, sizeof(IE_INPUT_DATA));
					pImgEdit->OnMouseLButtonDown(nFlags, &input_data);
				}
				break;
			case IE_EVENT_ID::MOUSE_LDRAG:
				{
					UINT nFlags = Read4ByteLE(fp);
					IE_INPUT_DATA input_data;
					ReadBuffer(fp, &input_data, sizeof(IE_INPUT_DATA));
					pImgEdit->OnMouseMove(nFlags, &input_data);
				}
				break;
			case IE_EVENT_ID::MOUSE_LUP:
				{
					UINT nFlags = Read4ByteLE(fp);
					IE_INPUT_DATA input_data;
					ReadBuffer(fp, &input_data, sizeof(IE_INPUT_DATA));
					pImgEdit->OnMouseLButtonUp(nFlags, &input_data);
				}
				break;
			case IE_EVENT_ID::MOUSE_RDOWN:
				{
					UINT nFlags = Read4ByteLE(fp);
					POINT pt;
					ReadBuffer(fp, &pt, sizeof(POINT));
					pImgEdit->OnMouseRButtonDown(nFlags, &pt);
				}
				break;
			case IE_EVENT_ID::MOUSE_RUP:
				{
					UINT nFlags = Read4ByteLE(fp);
					POINT pt;
					ReadBuffer(fp, &pt, sizeof(POINT));
					pImgEdit->OnMouseRButtonUp(nFlags, &pt);
				}
				break;
			case IE_EVENT_ID::KEY_DOWN:
				{
					UINT nChar = Read4ByteLE(fp);
					UINT optChar = Read4ByteLE(fp);
					pImgEdit->OnKeyboardDown(nChar, optChar);
				}
				break;
			case IE_EVENT_ID::KEY_UP:
				{
					UINT nChar = Read4ByteLE(fp);
					UINT optChar = Read4ByteLE(fp);
					pImgEdit->OnKeyboardUp(nChar, optChar);
				}
				break;
			default:
				OutputError::PushLog(LOG_LEVEL::_WARN, "on PlayCapture unkown event id");
				fseek(fp, data_size, SEEK_CUR);
				break;
		}

		int edata_pos = ftell(fp);
		assert((edata_pos - sdata_pos) == data_size);
	}

	fclose(fp);
}

void IEEventCapture::PlayCaptureAsync(const char *capture_file_path, ImgEdit *pImgEdit)
{
	assert(0);
}

void IEEventCapture::OnIdle()
{
}

void IEEventCapture::OnMouseLButtonDown(UINT nFlags, LPIE_INPUT_DATA lpd)
{
	if (IsCapturing()) {
		LockCaptureFile();

		WriteEventID(m_capture_fp, IE_EVENT_ID::MOUSE_LDOWN, m_timer.elapsed());
		m_timer.restart();

		long write_size_pos = ftell(m_capture_fp);
		Write4ByteLE(m_capture_fp, 0); //dummy

		long spos = ftell(m_capture_fp);
		Write4ByteLE(m_capture_fp, nFlags);
		WriteBuffer(m_capture_fp, lpd, sizeof(IE_INPUT_DATA));
		long epos = ftell(m_capture_fp);

		fseek(m_capture_fp, write_size_pos, SEEK_SET);
		Write4ByteLE(m_capture_fp, epos - spos);
		fseek(m_capture_fp, epos, SEEK_SET);

		assert((epos - spos) == (sizeof(UINT) + sizeof(IE_INPUT_DATA)));

		UnlockCaptureFile();
	}
}

void IEEventCapture::OnMouseLDrag(UINT nFlags, LPIE_INPUT_DATA lpd)
{
	if (IsCapturing()) {
		LockCaptureFile();

		WriteEventID(m_capture_fp, IE_EVENT_ID::MOUSE_LDRAG, m_timer.elapsed());
		m_timer.restart();

		long write_size_pos = ftell(m_capture_fp);
		Write4ByteLE(m_capture_fp, 0); //dummy

		long spos = ftell(m_capture_fp);
		Write4ByteLE(m_capture_fp, nFlags);
		WriteBuffer(m_capture_fp, lpd, sizeof(IE_INPUT_DATA));
		long epos = ftell(m_capture_fp);

		fseek(m_capture_fp, write_size_pos, SEEK_SET);
		Write4ByteLE(m_capture_fp, epos - spos);
		fseek(m_capture_fp, epos, SEEK_SET);

		assert((epos - spos) == (sizeof(UINT) + sizeof(IE_INPUT_DATA)));

		UnlockCaptureFile();
	}
}

void IEEventCapture::OnMouseLButtonUp(UINT nFlags, LPIE_INPUT_DATA lpd)
{
	if (IsCapturing()) {
		LockCaptureFile();

		WriteEventID(m_capture_fp, IE_EVENT_ID::MOUSE_LUP, m_timer.elapsed());
		m_timer.restart();

		long write_size_pos = ftell(m_capture_fp);
		Write4ByteLE(m_capture_fp, 0); //dummy

		long spos = ftell(m_capture_fp);
		Write4ByteLE(m_capture_fp, nFlags);
		WriteBuffer(m_capture_fp, lpd, sizeof(IE_INPUT_DATA));
		long epos = ftell(m_capture_fp);

		fseek(m_capture_fp, write_size_pos, SEEK_SET);
		Write4ByteLE(m_capture_fp, epos - spos);
		fseek(m_capture_fp, epos, SEEK_SET);

		assert((epos - spos) == (sizeof(UINT) + sizeof(IE_INPUT_DATA)));

		UnlockCaptureFile();
	}
}

void IEEventCapture::OnMouseRButtonDown(UINT nFlags, LPPOINT lps)
{
	if (IsCapturing()) {
		LockCaptureFile();

		WriteEventID(m_capture_fp, IE_EVENT_ID::MOUSE_RDOWN, m_timer.elapsed());
		m_timer.restart();

		long write_size_pos = ftell(m_capture_fp);
		Write4ByteLE(m_capture_fp, 0); //dummy

		long spos = ftell(m_capture_fp);
		Write4ByteLE(m_capture_fp, nFlags);
		WriteBuffer(m_capture_fp, lps, sizeof(POINT));
		long epos = ftell(m_capture_fp);

		fseek(m_capture_fp, write_size_pos, SEEK_SET);
		Write4ByteLE(m_capture_fp, epos - spos);
		fseek(m_capture_fp, epos, SEEK_SET);

		assert((epos - spos) == (sizeof(UINT) + sizeof(POINT)));

		UnlockCaptureFile();
	}
}

void IEEventCapture::OnMouseRButtonUp(UINT nFlags, LPPOINT lps)
{
	if (IsCapturing()) {
		LockCaptureFile();

		WriteEventID(m_capture_fp, IE_EVENT_ID::MOUSE_RUP, m_timer.elapsed());
		m_timer.restart();

		long write_size_pos = ftell(m_capture_fp);
		Write4ByteLE(m_capture_fp, 0); //dummy

		long spos = ftell(m_capture_fp);
		Write4ByteLE(m_capture_fp, nFlags);
		WriteBuffer(m_capture_fp, lps, sizeof(POINT));
		long epos = ftell(m_capture_fp);

		fseek(m_capture_fp, write_size_pos, SEEK_SET);
		Write4ByteLE(m_capture_fp, epos - spos);
		fseek(m_capture_fp, epos, SEEK_SET);

		assert((epos - spos) == (sizeof(UINT) + sizeof(POINT)));

		UnlockCaptureFile();
	}
}

void IEEventCapture::OnKeyboardDown(UINT nChar, UINT optChar)
{
	if (IsCapturing()) {
		LockCaptureFile();

		WriteEventID(m_capture_fp, IE_EVENT_ID::KEY_DOWN, m_timer.elapsed());
		m_timer.restart();

		long write_size_pos = ftell(m_capture_fp);
		Write4ByteLE(m_capture_fp, 0); //dummy

		long spos = ftell(m_capture_fp);
		Write4ByteLE(m_capture_fp, nChar);
		Write4ByteLE(m_capture_fp, optChar);
		long epos = ftell(m_capture_fp);

		fseek(m_capture_fp, write_size_pos, SEEK_SET);
		Write4ByteLE(m_capture_fp, epos - spos);
		fseek(m_capture_fp, epos, SEEK_SET);

		assert((epos - spos) == (sizeof(UINT) + sizeof(UINT)));

		UnlockCaptureFile();
	}
}

void IEEventCapture::OnKeyboardUp(UINT nChar, UINT optChar)
{
	if (IsCapturing()) {
		LockCaptureFile();

		WriteEventID(m_capture_fp, IE_EVENT_ID::KEY_UP, m_timer.elapsed());
		m_timer.restart();

		long write_size_pos = ftell(m_capture_fp);
		Write4ByteLE(m_capture_fp, 0); //dummy

		long spos = ftell(m_capture_fp);
		Write4ByteLE(m_capture_fp, nChar);
		Write4ByteLE(m_capture_fp, optChar);
		long epos = ftell(m_capture_fp);

		fseek(m_capture_fp, write_size_pos, SEEK_SET);
		Write4ByteLE(m_capture_fp, epos - spos);
		fseek(m_capture_fp, epos, SEEK_SET);

		assert((epos - spos) == (sizeof(UINT) + sizeof(UINT)));

		UnlockCaptureFile();
	}
}

void IEEventCapture::OnRunCommand(int cm_id, void *pvoid)
{
}

void IEEventCapture::WriteEventID(FILE* fp, uint16_t id, double diff_time)
{
	char log[256];
	sprintf(log, "event id = %d, diff_time = %f\n", id, diff_time);
	OutputError::PushLog(LOG_LEVEL::_INFO, log);

	fwrite("evid", 1, 4, fp);
	Write2ByteLE(fp, id);
	WriteDoubleLE(fp, diff_time);
}

void IEEventCapture::ReadEventID(FILE *fp, uint16_t *id, double *diff_time)
{
	char sig[4];
	fread(sig, 1, 4, fp);
	if (memcmp(sig, "evid", 4) != 0) {
		assert(0);
	}

	(*id) = Read2ByteLE(fp);
	(*diff_time) = ReadDoubleLE(fp);

	char log[256];
	sprintf(log, "event id = %d, diff_time = %f\n", (*id), (*diff_time));
	OutputError::PushLog(LOG_LEVEL::_INFO, log);
}