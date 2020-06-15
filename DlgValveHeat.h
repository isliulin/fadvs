#pragma once


// CDlgValveHeat dialog

class CDlgValveHeat : public CDialog
{
	DECLARE_DYNAMIC(CDlgValveHeat)

public:
	CDlgValveHeat(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgValveHeat();

// Dialog Data
	enum { IDD = IDD_DLG_VALVEHEAT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	bool UpdateUI();
	bool UpdateHeatState();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedBtnHeata();
	afx_msg void OnBnClickedBtnHeatb();
	afx_msg void OnBnClickedBtnHeatc();
	afx_msg void OnBnClickedBtnAdjusta();
	afx_msg void OnBnClickedBtnAdjustb();
	afx_msg void OnBnClickedBtnAdjustc();
};
