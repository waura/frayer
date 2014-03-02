#pragma once

#include "ImgEdit.h"


class CImgEdit_UIDoc : public CDocument
{
protected: // シリアル化からのみ作成します。
	CImgEdit_UIDoc();
	DECLARE_DYNCREATE(CImgEdit_UIDoc)

public:
	virtual ~CImgEdit_UIDoc();

	// オーバーライド
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	virtual BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace = 1);
	virtual void Serialize(CArchive& ar);
	virtual HMENU GetDefaultMenu();

	static void SetHInstance(HINSTANCE hInst);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual void SetEmptyPath();
	virtual ImgFile_Ptr GetImgFile() const;

protected:
	DECLARE_MESSAGE_MAP()

private:
	void InitIEMenu();

	HMENU m_hIEMenu;
	HMENU m_hMenuFile;
	HMENU m_hMenuEdit;
	HMENU m_hMenuImage;
	HMENU m_hMenuDisplay;
	HMENU m_hMenuImgSelect;
	HMENU m_hMenuTool;
	HMENU m_hMenuHelp;
	HMENU m_hMenuWindow;

	ImgFile_Ptr m_pImgFile;

	static HINSTANCE m_hInst;
};


