#pragma once


// CDlgPrompt dialog

class CDlgPrompt : public CDialog
{
	DECLARE_DYNAMIC(CDlgPrompt)

public:
	CDlgPrompt(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPrompt();

// Dialog Data
	enum { IDD = IDD_DLG_PROMPT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strPrompt;
	void SetShowText(CString str)	{ m_strPrompt = str;}
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
