
#pragma once

#include "InputPointDevice.h"

//#define WM_USER_PUSH_IE_INPUT_DATA (WM_USER+1)
#define IE_MULTI_THREAD_DRAW

enum IE_UI_VIEW_RESOURCE_ID {
	IE_BRUSH_CORSOR=0x01,
};

//描画スレッドに渡すデータ
typedef struct _DRAW_DATA{
	CView* draw_view;
	HDC hBackDC;
	HBITMAP hBackBmp;
	ImgFile_Ptr imgFile;
	RECT draw_disp_rect;
	RECT draw_img_rect;
	HANDLE pThread;
#ifdef IE_MULTI_THREAD_DRAW
	int* num_of_running_thread;
#endif //IE_MULTI_THREAD_DRAW
}DRAW_DATA, *LPDRAW_DATA;


class CImgEdit_UIView :
	public CView,
	public ImgLayerEventListener,
	public ImgFileEventListener
{
protected: // シリアル化からのみ作成します。
	CImgEdit_UIView();
	DECLARE_DYNCREATE(CImgEdit_UIView)

// 属性
public:
	CImgEdit_UIDoc* GetDocument() const;

// 操作
public:

// イベント
public:
	void OnImgLayerPushUpdateData(const LPUPDATE_DATA data);
	void OnImgFilePushUpdateData(const LPUPDATE_DATA data);
	void OnChangeViewPos(const CvPoint2D64f *pos);
	void OnChangeViewSize(int size);
	void OnChangeViewRot(int rot);

	virtual void OnInitialUpdate();
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 実装
public:
	virtual ~CImgEdit_UIView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	afx_msg int  OnCreate(LPCREATESTRUCT lpcs);
	afx_msg void OnDestroy();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnActivateView(BOOL bActivate, CView* pActivateView, CView *pDeactiveView);
	afx_msg int  OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg LRESULT OnWtPacket(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWtProximity(WPARAM wParam, LPARAM lParam);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnAppMouseWheel(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	void DoMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	void ResourceLock(unsigned int id){
		while(IsResourceLock( id ));
		m_LockResourceID |= id;
	}
	void ResourceUnlock(unsigned int id){
		m_LockResourceID &= (~id);
	}
	bool IsResourceLock(unsigned int id) const {
		return ((m_LockResourceID & id) == id);
	}

	////////////////////////
	/*!
		ブラシカーソルを作成する
	*/
	void CreateBrushCursor(IEBrush_Ptr pBrush, CPoint point);

	////////////////////////
	/*!
		ブラシカーソルに変更する
	*/
	bool SetBrushCorsor();

	void SetNormalCorsor();

	void AdjastScrollSizes();
	void AdjastScrollPos();

	int m_numOfProcessors;
	int m_numOfRunningThread;
	bool m_isDrag;
	InputPointDevice m_ptDevice;
	DRAW_DATA* m_DrawDatas;	//描画データ
	HANDLE m_hDrawThread;	//描画スレッドハンドル

	HCURSOR m_hBrushCursor;

	bool m_isRegistEventListener;
	unsigned int m_LockResourceID;
};

#ifndef _DEBUG  // ImgEdit_UIView.cpp のデバッグ バージョン
inline CImgEdit_UIDoc* CImgEdit_UIView::GetDocument() const
   { return reinterpret_cast<CImgEdit_UIDoc*>(m_pDocument); }
#endif

