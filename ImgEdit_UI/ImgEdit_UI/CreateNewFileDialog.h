#pragma once

#include "../resource1.h"
#include <libImgEdit.h>

#include <vector>

typedef struct _FILE_SIZE_TEMPLATE {
	double width;
	double height;
	int unit_id;
	int resolution;
} FILE_SIZE_TEMPLATE;

typedef std::vector<FILE_SIZE_TEMPLATE> FILE_SIZE_TEMPLATE_Vec;

// CCreateNewFileDialog ダイアログ

class CCreateNewFileDialog : public CDialog
{
	DECLARE_DYNAMIC(CCreateNewFileDialog)

public:
	CCreateNewFileDialog(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CCreateNewFileDialog();

	void SetImgEdit(ImgEdit* pImgEdit) {
		m_pImgEdit = pImgEdit;
	}

	ImgFile_Ptr GetCreateImgFile() const {
		return m_pCreateFile;
	}

// ダイアログ データ
	enum { IDD = IDD_CREATE_NEW_FILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	BOOL OnInitDialog();
	void OnOK();
	void OnCancel();

	BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

private:

	void CreateNewTemplateSizeFile();
	void SetFileSizeTemplate(int index);
	int GetImgPixelWidth();
	void SetImgPixelWidth(int pixel);
	int GetImgPixelHeight();
	void SetImgPixelHeight(int pixel);

	int m_file_width;
	int m_file_height;
	int m_size_unit;
	int m_select_gb_color;

	ImgEdit* m_pImgEdit;
	ImgFile_Ptr m_pCreateFile;

	FILE_SIZE_TEMPLATE_Vec m_fileSizeTemplate_Vec;
public:
	afx_msg void OnBnClickedHorizontalRadio();
	afx_msg void OnBnClickedVerticalRadio();
};
