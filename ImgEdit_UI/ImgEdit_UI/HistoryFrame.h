
#pragma once

//オーナードローをオーバーライドするクラス
class CListCtrlOD : public CListCtrl 
{
public:

	void SetNowHandleItem(int iItem){
		m_nowHandleItem = iItem;
		this->SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED);
		this->EnsureVisible(iItem, TRUE);
	}

	int GetNowHandleItem(){
		return m_nowHandleItem;
	}

protected:
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

private:
	int m_nowHandleItem;
};

//
class CHistoryFrame :
	public CWnd,
	public ImgEditEventListener,
	public ImgFileEventListener
{
public:
	CHistoryFrame();
	~CHistoryFrame();

	void OnChangeActiveFile(ImgFile_Ptr pNewFile, ImgFile_Ptr pOldFile);

	void OnAddUndoImgFileHandle(const ImgFileHandle* pHandle);
	void OnRemoveOldUndoImgFileHandle(int num);
	void OnUndoImgFileHandle(const ImgFileHandle* pHandle);
	void OnRedoImgFileHandle(const ImgFileHandle* pHandle);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnListChange(NMHDR* pNotifyStruct, LRESULT* result);
	
	DECLARE_MESSAGE_MAP();

private:

	void ResetList();

	ImgFile_Ptr m_pEditFile;
	CListCtrlOD m_handleListCtrl;
};