#pragma once


// CMyButton

class CMyButton : public CButton
{
	DECLARE_DYNAMIC(CMyButton)

public:
	BOOL m_bCheck;
	COLORREF m_crTextColor,m_crDownColor,m_crUpColor;
	CMyButton();
	virtual ~CMyButton();
	void SetDownColor(COLORREF color);
	void SetUpColor(COLORREF color);
	BOOL Attach(const UINT nID, CWnd* pParent);
	//afx_msg void OnClicked();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	DECLARE_MESSAGE_MAP()
};


