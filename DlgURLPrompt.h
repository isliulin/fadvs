#pragma once


// CDlgURLPrompt dialog

class CDlgURLPrompt : public CDialog
{
	DECLARE_DYNAMIC(CDlgURLPrompt)

public:
	CDlgURLPrompt(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgURLPrompt();

// Dialog Data
	enum { IDD = IDD_DLG_URLPROMPT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
