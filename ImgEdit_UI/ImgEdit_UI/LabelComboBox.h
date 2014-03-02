#pragma once 

class CLabelComboBox :
	public CWnd
{
public:
	CLabelComboBox(){};
	~CLabelComboBox(){};

	BOOL CreateLabelComboBox(int label_width, int label_height, int combo_width, int combo_height,
		const RECT& rc, CWnd* parentWnd, int id);

	VOID SetSize(int label_width, int label_height, int combo_width, int combo_height);
	VOID SetLabel(LPCSTR lpszString);
	VOID SetChangeVal(int* change_val){
		m_change_val = change_val;
	}

	VOID SetCurSel(int nIndex){
		m_Combo.SetCurSel(nIndex);
	}

	HWND GetComboBoxHandle(){
		return m_Combo.GetSafeHwnd();
	}

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSelChangeCombo();
	DECLARE_MESSAGE_MAP();

private:

	int m_label_width;
	int m_label_height;
	int m_combo_width;
	int m_combo_height;

	int* m_change_val;

	CFont m_Font;
	CStatic m_Label;
	CComboBox m_Combo;
};