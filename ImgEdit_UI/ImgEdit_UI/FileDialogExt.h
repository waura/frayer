#pragma once

class CFileDialogExt : public CFileDialog
{
public:
	CFileDialogExt(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL,
		DWORD dwSize = 0)
		: CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName
	    , dwFlags, lpszFilter, pParentWnd, dwSize)
	{
		isSaveFile = (bOpenFileDialog==FALSE);

		//to use keyboard input this dialog
		CImgEdit_UIApp* pApp = (CImgEdit_UIApp*)AfxGetApp();
		pApp->UnhookKeyBoard();
	}
	virtual ~CFileDialogExt()
	{
		//reset hook
		CImgEdit_UIApp* pApp = (CImgEdit_UIApp*)AfxGetApp();
		pApp->HookKeyboard();
	}

	virtual void OnInitDone(){
		OnTypeChange();
		CFileDialog::OnInitDone();
	}

	void OnTypeChange(){
		//フィルタから拡張子を取得して、デフォルトとして登録
		LPCSTR ptr = m_ofn.lpstrFilter;

		int nTarget = ((m_ofn.nFilterIndex-1)*2) + 1;
		while(nTarget--){
		    while(*(ptr++) != '\0');
		}

		int p;
		m_select_filter_s = ptr;
		p = m_select_filter_s.find_first_of(';');
		if(p == std::string::npos){
			m_select_filter_s = m_select_filter_s.substr(2, m_select_filter_s.size()-2);
		}
		else{
			m_select_filter_s = m_select_filter_s.substr(2, p-2);
		}

		CFileDialog::OnTypeChange();
	}

	BOOL OnFileNameOK(){
		if(isSaveFile){
			char drive[_MAX_DRIVE];
			char dir[_MAX_DIR];
			char fname[_MAX_FNAME];
			char ext[_MAX_EXT];

			_splitpath(m_ofn.lpstrFile, drive, dir, fname, ext);
			_makepath(m_ofn.lpstrFile, drive, dir, fname, m_select_filter_s.c_str());
		}

		return 0;
	}

private:
	bool isSaveFile;
	std::string m_select_filter_s;
};