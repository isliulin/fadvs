#pragma once


// CDlgRunInfo dialog

class CDlgRunInfo : public CDialog
{
	DECLARE_DYNAMIC(CDlgRunInfo)

public:
	CDlgRunInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgRunInfo();

// Dialog Data
	enum { IDD = IDD_DLG_RUNINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	bool LoadData();
};
