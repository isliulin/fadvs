#pragma once

#define  MM_P 0.1
#define  MM_FACTOR 0.01
// CDlgIOCtrl dialog

class CDlgIOCtrl : public CDialog
{
	DECLARE_DYNAMIC(CDlgIOCtrl)

public:
	CDlgIOCtrl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgIOCtrl();

// Dialog Data
	enum { IDD = IDD_DlG_IO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOutput(UINT nID);
	afx_msg void OnClose();
	//afx_msg void OnBnClickedFroceM(UINT nID);
};
