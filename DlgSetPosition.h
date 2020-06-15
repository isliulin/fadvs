#pragma once


// CDlgSetPosition dialog

class CDlgSetPosition : public CDialog
{
	DECLARE_DYNAMIC(CDlgSetPosition)

public:
	CDlgSetPosition(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSetPosition();

// Dialog Data
	enum { IDD = IDD_DLG_SET_POSITION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_nIndex;
	int m_nCount;
	tgPos m_tgTestPos[10];
	CColorButton m_colorButton[10]; //0702 À©Õ¹°´Å¥6->10
	virtual BOOL OnInitDialog();
	void UpdatePos();
	afx_msg void OnNMCustomdrawSliderSetposSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	void SavePos();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnSetposNext();
};
