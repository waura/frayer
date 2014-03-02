#include "StdAfx.h"
#include "InputPointDevice.h"

#define _PI_ 3.14159265358979323846

#include<cstdint>
#include <functional>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

InputPointDevice::InputPointDevice()
{
	m_hWnd = NULL;
	m_dll = NULL;
	//m_packetBuf = 0;
	m_nPacketBufSize = 64;

	m_maxAzimuth = 0;
	m_maxAltitude = 0;

	m_isLDrag = false;
	m_isInside = false;
	m_isLoadedTablet = false;

	//init random engine
	std::random_device rnd;
    std::vector< std::uint_least32_t> v(10) ;
	std::generate(v.begin(), v.end(), std::ref(rnd));
	m_random_engine.seed(std::seed_seq(v.begin(), v.end()));
}

InputPointDevice::~InputPointDevice()
{
	if(m_dll){
		FreeLibrary(m_dll);
		m_dll = NULL;
	}
	//if(m_packetBuf){
	//	delete[] m_packetBuf;
	//	m_packetBuf = NULL;
	//}
}

BOOL InputPointDevice::Initialize(HWND hWnd, ImgEdit* pImgEdit)
{
	m_hWnd = hWnd;
	m_pImgEdit = pImgEdit;

	m_dll = LoadLibrary("wintab32.dll");
	if (m_dll == NULL) {
		OutputError::PushLog(LOG_LEVEL::_WARN, "cannot load wintab32.dll");
		m_isLoadedTablet = false;
		return FALSE;
	}
	m_isLoadedTablet = true;

	m_wtClose = (WTClose_)GetProcAddress(m_dll, "WTClose");
	m_wtInfo  = (WTInfo_)GetProcAddress(m_dll, "WTInfoA");
	m_wtOpen  = (WTOpen_)GetProcAddress(m_dll, "WTOpenA");
	m_wtPacket= (WTPacket_)GetProcAddress(m_dll, "WTPacket");
	m_wtPacketsGet = (WTPacketsGet_)GetProcAddress(m_dll, "WTPacketsGet");
	//m_wtQueueSizeSet = (WTQueueSizeSet_)GetProcAddress(m_dll, "WTQueueSizeSet");
	//m_wtOverlap = (WTOverlap_)GetProcAddress(m_dll, "WTOverlap");

	UINT res = m_wtInfo(0,0, NULL);
	if (res == 0) {
		OutputError::PushLog(LOG_LEVEL::_WARN, "faild WTInfoA");
		::FreeLibrary(m_dll);
		m_dll = NULL;
		return FALSE;
	}

	//最大筆圧の取得
	AXIS pressureInfo;
	res = m_wtInfo(WTI_DEVICES, DVC_NPRESSURE, &pressureInfo);
	if (res == 0) {
		OutputError::PushLog(LOG_LEVEL::_WARN, "faild WTInfo WTI_DEVICES DVC_NPRESSURE");
		::FreeLibrary(m_dll);
		m_dll = NULL;
		return FALSE;
	}
	m_maxPressure = pressureInfo.axMax;

	char strVal[256];
	wsprintf(strVal, "%d", m_maxPressure);
	OutputError::PushLog(LOG_LEVEL::_INFO, "wintab max pressure ", strVal);

	//
	AXIS orientationInfo[3];
	res = m_wtInfo(WTI_DEVICES, DVC_ORIENTATION, &orientationInfo);
	if (res == 0) {
		OutputError::PushLog(LOG_LEVEL::_WARN, "faild WTInfo WTI_DEVICES DVC_ORIENTATION");
	} else {
		m_maxAzimuth = orientationInfo[0].axMax;
		m_maxAltitude = orientationInfo[1].axMax;
	}

	wsprintf(strVal, "%d", m_maxAzimuth);
	OutputError::PushLog(LOG_LEVEL::_INFO, "wintab max azimuth ", strVal);
	wsprintf(strVal, "%d", m_maxAltitude);
	OutputError::PushLog(LOG_LEVEL::_INFO, "wintab max altitude ", strVal);

	//デフォルト設定を取得
	res = m_wtInfo(WTI_DEFSYSCTX, 0, &m_logcontext);
	if (res == 0) {
		OutputError::PushLog(LOG_LEVEL::_WARN, "faild WTInfo WTI_DEFSYSCTX");
		::FreeLibrary(m_dll);
		m_dll = NULL;
		return FALSE;
	}

	//設定値を修正
	wsprintf(m_logcontext.lcName, "ImgEdit %x", AfxGetApp()->m_hInstance);
	m_logcontext.lcPktData = PACKETDATA;
	m_logcontext.lcPktMode = PACKETMODE;
	m_logcontext.lcMoveMask = PACKETDATA;
	m_logcontext.lcBtnUpMask = m_logcontext.lcBtnDnMask;
	m_logcontext.lcOptions = m_logcontext.lcOptions | (CXO_MESSAGES | CXO_SYSTEM | CXO_MARGIN);

	m_logcontext.lcOutOrgX = 0;
	m_logcontext.lcOutExtX = m_logcontext.lcInExtX;
	m_logcontext.lcOutOrgY = 0;
	m_logcontext.lcOutExtY = m_logcontext.lcInExtY;

	int sx = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int sy = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
	m_tab_xscale = (double)sx / m_logcontext.lcInExtX;
	m_tab_yscale = (double)sy / m_logcontext.lcInExtY;

	//
	m_handle = m_wtOpen( hWnd, &m_logcontext, TRUE );
	if(m_handle == NULL){
		OutputError::PushLog(LOG_LEVEL::_WARN, "faild WTOpenA");
		::FreeLibrary(m_dll);
		m_dll = NULL;
		return FALSE;
	}

	//set buffer size
	//m_wtQueueSizeSet(m_handle, m_nPacketBufSize);
	//m_packetBuf = new PACKET[m_nPacketBufSize];
	return TRUE;
}

//#define _PRINT_TIME
VOID InputPointDevice::OnWtPacket(WPARAM wp, LPARAM lp)
{
	if(m_isLoadedTablet == false) return;

	BOOL res;
	PACKET packet;
	res = m_wtPacket((HCTX)lp, (UINT)wp, &packet);
	if(res){
		IE_INPUT_DATA_EVENT input_ev;
		
		//calc screen to client distance
		POINT pt;
		pt.x = 0; pt.y=0;
		::ScreenToClient(m_hWnd, &pt);
		
		//タブレットの情報取得
		int tabx = packet.pkX;
		int taby = (m_logcontext.lcOutExtY + m_logcontext.lcOutOrgY) - packet.pkY;
		
		//rescaling
		input_ev.data.x = tabx*m_tab_xscale + pt.x;
		input_ev.data.y = taby*m_tab_yscale + pt.y;
		input_ev.data.p = (double)packet.pkNormalPressure / m_maxPressure;
		if(input_ev.data.p > 1.0) return;

		if (m_maxAzimuth == 0) {
			input_ev.data.azimuth_rad = 0.0;
		} else {
			input_ev.data.azimuth_rad = 2.0 * _PI_ * ((double)packet.pkOrientation.orAzimuth / m_maxAzimuth);
		}
		if (m_maxAltitude == 0) {
			input_ev.data.altitude_rad = _PI_ / 2.0; //pi/2
		} else {
			input_ev.data.altitude_rad = (_PI_ / 2.0) * ((double)packet.pkOrientation.orAzimuth/ m_maxAltitude);
		}

		std::uniform_real_distribution<double> dist(0.0, 1.0);
		input_ev.data.random = dist(m_random_engine);

		////debug print
		//char str[256] = {0};
		//sprintf(str, "x=%f y=%f z=%d pres=%f inside = %d m_isLDrag = %d \n",
		//	input_ev.data.x, input_ev.data.y, packet.pkZ, input_ev.data.p, m_isInside, m_isLDrag);
		//::OutputDebugString(str);
		//sprintf(str, "orAzimuth=%d, orAltitude=%d, orTwist=%d \n",
		//	packet.pkOrientation.orAzimuth, packet.pkOrientation.orAltitude, packet.pkOrientation.orTwist);
		//::OutputDebugString(str);

#ifdef _PRINT_TIME
	static char str_blt_lap[256];
	QPCTimer timer;
#endif
		if(input_ev.data.p >= 0.03){
			RECT client_rc;
			::GetClientRect(m_hWnd, &client_rc);
			if(!isInRect(input_ev.data.x, input_ev.data.y, &client_rc)) return;

			if(m_isLDrag){
				m_pImgEdit->OnMouseMove(IE_MK_LBUTTON, &(input_ev.data));
				//input_ev.ievent = IE_INPUT_MOVE;
				//m_InputDataEvent_List.push_back( input_ev );
			}
			else{
				m_pImgEdit->OnMouseLButtonDown(IE_MK_LBUTTON, &(input_ev.data));
				m_isLDrag = true;
				//input_ev.ievent = IE_INPUT_DOWN;
				//m_InputDataEvent_List.push_back( input_ev );
			}
		}
		else{
			if(m_isLDrag){
				m_pImgEdit->OnMouseLButtonUp(IE_MK_LBUTTON, &(input_ev.data));
				m_isLDrag = false;
				//input_ev.ievent = IE_INPUT_UP;
				//m_InputDataEvent_List.push_back( input_ev );
			}
		}

#ifdef _PRINT_TIME
		sprintf(str_blt_lap, "tablet event %f msec\n", timer.elapsed());
		::OutputDebugString(str_blt_lap);
#endif
	}

	m_wtPacketsGet( m_handle, 1, NULL);
}

VOID InputPointDevice::OnWtProximity(WPARAM wp, LPARAM lp)
{
	if(m_isLoadedTablet == false) return;

	//認識範囲内か?
	if((lp & 0xFFFF) != 0) m_isInside = true;
	else m_isInside = false;

	m_wtPacketsGet( m_handle, 1, NULL);
}

VOID InputPointDevice::OnRButtonDown(UINT nFlags, CPoint point)
{
	POINT pt;
	pt.x = point.x;
	pt.y = point.y;
	g_ImgEdit.OnMouseRButtonDown(nFlags, &pt);
}

VOID InputPointDevice::OnRButtonUp(UINT nFlags, CPoint point)
{
	POINT pt;
	pt.x = point.x;
	pt.y = point.y;
	g_ImgEdit.OnMouseRButtonUp(nFlags, &pt);
}

VOID InputPointDevice::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(!m_isInside){
		::SetCapture(m_hWnd);

		IE_INPUT_DATA_EVENT input_ev;
		input_ev.data.x = point.x;
		input_ev.data.y = point.y;
		input_ev.data.p = 1.0;
		input_ev.data.azimuth_rad = 0.0;
		input_ev.data.altitude_rad = _PI_ / 2.0; //pi/2

		std::uniform_real_distribution<double> dist(0.0, 1.0);
		input_ev.data.random = dist(m_random_engine);
		
		g_ImgEdit.OnMouseLButtonDown(nFlags, &(input_ev.data));
		//input_ev.ievent = IE_INPUT_DOWN;
		//m_InputDataEvent_List.push_back( input_ev );

		m_isLDrag = true;
	}
}

VOID InputPointDevice::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(!m_isInside){
		::ReleaseCapture();

		IE_INPUT_DATA_EVENT input_ev;
		input_ev.data.x = point.x;
		input_ev.data.y = point.y;
		input_ev.data.p = 1.0;
		input_ev.data.azimuth_rad = 0.0;
		input_ev.data.altitude_rad = _PI_ / 2.0; //pi/2

		std::uniform_real_distribution<double> dist(0.0, 1.0);
		input_ev.data.random = dist(m_random_engine);

		g_ImgEdit.OnMouseLButtonUp(nFlags, &(input_ev.data));
		//input_ev.ievent = IE_INPUT_UP;
		//m_InputDataEvent_List.push_back( input_ev );

		m_isLDrag = false;
	}
}

VOID InputPointDevice::OnMouseMove(UINT nFlags, CPoint point)
{
	//PollingPoint();

	if(!m_isInside){
		IE_INPUT_DATA_EVENT input_ev;
		input_ev.data.x = point.x;
		input_ev.data.y = point.y;
		input_ev.data.p = 1.0;
		input_ev.data.azimuth_rad = 0.0;
		input_ev.data.altitude_rad = _PI_ / 2.0;

		std::uniform_real_distribution<double> dist(0.0, 1.0);
		input_ev.data.random = dist(m_random_engine);

		g_ImgEdit.OnMouseMove(nFlags, &(input_ev.data));
		//input_ev.ievent = IE_INPUT_MOVE;
		//m_InputDataEvent_List.push_back( input_ev );
	}
}

//VOID InputPointDevice::PollingPoint()
//{
//	if(m_isLoadedTablet == false) return;
//
//	m_wtOverlap(m_handle, TRUE);
//
//	int nPackets;
//	IE_INPUT_DATA data;
//
//	//calc screen to client distance
//	POINT pt;
//	pt.x = 0; pt.y=0;
//	::ScreenToClient(m_hWnd, &pt);
//
//	if(nPackets = m_wtPacketsGet(m_handle, m_nPacketBufSize, m_packetBuf)){
//		for(int i=0; i<nPackets; i++){
//			int tabx = m_packetBuf[i].pkX;
//			int taby = (m_logcontext.lcOutExtY + m_logcontext.lcOutOrgY) - m_packetBuf[i].pkY;
//			
//			//rescaling
//			data.x = tabx*m_tab_xscale + pt.x;
//			data.y = taby*m_tab_yscale + pt.y;
//			data.p = (double)m_packetBuf[i].pkNormalPressure / m_maxPressure;
//
//			if(data.p > 1.0) continue;
//
//			//debug print
//			char str[256];
//			sprintf(str, "x=%f y=%f pres=%f inside = %d m_isLDrag = %d \n", data.x, data.y, data.p, m_isInside, m_isLDrag);
//			::OutputDebugString(str);
//
//			if(data.p >= 0.001){
//				if(m_isLDrag){
//					m_pImgEdit->OnMouseMove(IE_MK_LBUTTON, &data);
//				}
//				else{
//					m_pImgEdit->OnMouseLButtonDown(IE_MK_LBUTTON, &data);
//					m_isLDrag = true;
//				}
//			}
//			else{
//				if(m_isLDrag){
//					m_pImgEdit->OnMouseLButtonUp(IE_MK_LBUTTON, &data);
//					m_isLDrag = false;
//				}
//			}
//		}
//	}
//}