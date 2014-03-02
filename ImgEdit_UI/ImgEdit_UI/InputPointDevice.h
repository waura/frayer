#pragma once


#ifndef WIN32
	#define WIN32
#endif

#include <wintab.h>
#define PACKETDATA	(PK_CURSOR | PK_X | PK_Y | PK_Z | PK_BUTTONS | PK_NORMAL_PRESSURE | PK_ORIENTATION)
#define PACKETMODE	PK_BUTTONS
#include <pktdef.h>

#include <random>

////
//#pragma pack (push,1)
//struct TCustomPacket
//{
//	HCTX pkContext;
//	UINT pkStatus;
//	LONG pkTime;
//	WTPKT pkChanged;
//	UINT pkSerialNumber;
//	UINT pkCursor;
//	DWORD pkButtons;
//	DWORD pkX;
//	DWORD pkY;
//	DWORD pkZ;
//	UINT pkNormalPressure;
//	UINT pkTangentPressure;
//	tagORIENTATION pkOrientation;
//};
//#pragma pack (pop)


enum IE_INPUT_EVENT{
	IE_INPUT_DOWN,
	IE_INPUT_MOVE,
	IE_INPUT_UP,
};

typedef struct _IE_INPUT_DATA_EVENT
{
	IE_INPUT_DATA data;
	IE_INPUT_EVENT ievent;

} IE_INPUT_DATA_EVENT;

typedef std::list<IE_INPUT_DATA_EVENT> IE_INPUT_DATA_EVENT_LIST;


//
typedef BOOL (API *WTClose_)( HCTX Ctx );
typedef UINT (API *WTInfo_)( UINT wCategory, UINT nIndex, LPVOID lpOutput );
typedef HCTX (API *WTOpen_)( HWND Wnd, tagLOGCONTEXTA* lpLogCtx, BOOL fEnable );
typedef BOOL (API *WTPacket_)( HCTX Ctx, UINT wSerial, LPVOID lpPkt );
typedef int  (API *WTPacketsGet_)( HCTX Ctx, int cMaxPkts, LPVOID lpPkts );
typedef BOOL (API *WTQueueSizeSet_)(HCTX Ctx, int size);
typedef BOOL (API *WTOverlap_)(HCTX Ctx, BOOL bl);


class InputPointDevice
{
public:
	InputPointDevice();
	~InputPointDevice();

	BOOL Initialize(HWND hWnd, ImgEdit* pImgEdit);

	VOID OnWtPacket(WPARAM wp, LPARAM lp);
	VOID OnWtProximity(WPARAM wp, LPARAM lp);
	VOID OnRButtonDown(UINT nFlags, CPoint point);
	VOID OnRButtonUp(UINT nFlags, CPoint point);
	VOID OnLButtonDown(UINT nFlags, CPoint point);
	VOID OnLButtonUp(UINT nFlags, CPoint point);
	VOID OnMouseMove(UINT nFlags, CPoint point);

	//IE_INPUT_DATA_EVENT_LIST m_InputDataEvent_List;
private:
	//VOID PollingPoint();


	HWND m_hWnd;
	ImgEdit* m_pImgEdit;

	HMODULE m_dll;
	WTClose_ m_wtClose;
	WTInfo_ m_wtInfo;
	WTOpen_ m_wtOpen;
	WTPacket_ m_wtPacket;
	WTPacketsGet_ m_wtPacketsGet;
	//WTQueueSizeSet_ m_wtQueueSizeSet;
	//WTOverlap_ m_wtOverlap;

	HCTX m_handle;
	tagLOGCONTEXTA m_logcontext;
	//PACKET* m_packetBuf;
	int m_nPacketBufSize;

	bool m_isLoadedTablet;
	bool m_isLDrag;
	POINT m_origin;
	POINT m_tabletSize;
	int m_maxPressure; //ç≈ëÂïMà≥íl
	int m_maxAzimuth;
	int m_maxAltitude;
	bool m_isInside; //îFéØîÕàÕì‡Ç©ÅH
	double m_tab_xscale;
	double m_tab_yscale;

	std::mt19937 m_random_engine;
};
