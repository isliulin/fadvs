#pragma once


// CXPButton
class CXPButton : public CButton
{
	DECLARE_DYNAMIC(CXPButton)
public:
	CXPButton(void);
	virtual ~CXPButton(void);

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void SetFont(CFont* pFont, BOOL bRedraw = TRUE);
	virtual void SetFont(LOGFONT *pLogFont, BOOL bRedraw = TRUE);
	virtual void SetFont(int nHeight, LPCTSTR fontName, BOOL bRedraw = TRUE);
	CFont *GetFont()	{ return m_Font;}

	void Set_TextColor(COLORREF cr)
	{
		m_TextColor = cr;
		if(GetSafeHwnd())
		{
			Invalidate();
		}
	}
	// 设置选中时的字体颜色
	void Set_TextColorSel(COLORREF cr)
	{
		m_TextColorSel = cr;
		if(GetSafeHwnd())
		{
			Invalidate();
		}
	}
	COLORREF Get_TextColor()	{ return m_TextColor;}

	// 设置未选中时的背景色
	void Set_BackColor(COLORREF cr);
	// 设置选中时的背景色
	void Set_BackColorSel(COLORREF cr);
	COLORREF Get_BackColor()	{ return m_BackColor;}

	void SetSelected(BOOL bSel);
	void SetBtnStyle(BOOL bStyle);
	BOOL GetCheck();
	void SetCheck(BOOL bCheck, BOOL bRepaint=TRUE);
protected:
	// 按钮外边框
	CPen m_BoundryPen;
	// 保持按下状态
	BOOL m_BtnStyle;
	// 鼠标位于按钮之上时该值为true,反之为false
	BOOL m_bOver;
	// 鼠标按下没有释放时该值为true
	BOOL m_bTracking;
	// 按钮被选中
	BOOL m_bSelected;
	BOOL m_bCheck;

	COLORREF m_DisBackColor;
	COLORREF m_BackColorSel;
	COLORREF m_BackColor;
	COLORREF m_TextColor;
	COLORREF m_TextColorSel;
	CBrush m_BackBrush;
	CBrush m_BackBrushSel;
	CFont *m_Font;

	virtual void PreSubclassWindow();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam,LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClicked();
	afx_msg LRESULT OnSetCheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetCheck(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};


