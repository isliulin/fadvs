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
	// ����ѡ��ʱ��������ɫ
	void Set_TextColorSel(COLORREF cr)
	{
		m_TextColorSel = cr;
		if(GetSafeHwnd())
		{
			Invalidate();
		}
	}
	COLORREF Get_TextColor()	{ return m_TextColor;}

	// ����δѡ��ʱ�ı���ɫ
	void Set_BackColor(COLORREF cr);
	// ����ѡ��ʱ�ı���ɫ
	void Set_BackColorSel(COLORREF cr);
	COLORREF Get_BackColor()	{ return m_BackColor;}

	void SetSelected(BOOL bSel);
	void SetBtnStyle(BOOL bStyle);
	BOOL GetCheck();
	void SetCheck(BOOL bCheck, BOOL bRepaint=TRUE);
protected:
	// ��ť��߿�
	CPen m_BoundryPen;
	// ���ְ���״̬
	BOOL m_BtnStyle;
	// ���λ�ڰ�ť֮��ʱ��ֵΪtrue,��֮Ϊfalse
	BOOL m_bOver;
	// ��갴��û���ͷ�ʱ��ֵΪtrue
	BOOL m_bTracking;
	// ��ť��ѡ��
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


