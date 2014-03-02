#pragma once

class CHoverBitmapButton :
	public CBitmapButton
{
public:
	CHoverBitmapButton() : m_isHover(false), m_isToggle(false) {}
	~CHoverBitmapButton(){};

	BOOL LoadBitmaps(
		UINT nIDBitmapResource,
		UINT nIDBitmapResourceHov,
		UINT nIDBitmapResourceSel=0,
		UINT nIDBitmapResourceFocus=0,
		UINT nIDBitmapResourceDisabled=0);

	void DrawItem(LPDRAWITEMSTRUCT lpDis);

	void SetToggle(bool bl){
		m_isToggle = bl;
		this->Invalidate(FALSE);
	}
	bool GetToggle() const {
		return m_isToggle;
	}

protected:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	LRESULT OnMouseHover(WPARAM wp, LPARAM lp);
	LRESULT OnMouseLeave(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP();

private:
	bool m_isHover;
	bool m_isToggle;
	CBitmap m_bitmapHov;
};