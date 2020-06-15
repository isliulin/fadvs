#pragma once


// CDlgCustom dialog

class CDlgCustom : public CDialog
{
	DECLARE_DYNAMIC(CDlgCustom)

public:
	CDlgCustom(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCustom();

	// Dialog Data
	enum { IDD = IDD_DLG_CUSTOM };
	CListCtrl m_wndList;
	CColorButton m_colorButton[6];
	int m_nSpeed;
	int m_nSelIndex;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void UpateCmdListBox(int nIndex);
	void OnClose();
	afx_msg void OnNMCustomdrawSliderCustomSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnCustomInsert();
	afx_msg void OnBnClickedBtnCustomModify();
	afx_msg void OnBnClickedBtnCustomDelet();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnCustomMove();
	afx_msg void OnBnClickedBtnCustomAutoAlign();
	afx_msg void OnBnClickedBtnCustomSave();
	afx_msg void OnNMClickListPath(NMHDR *pNMHDR, LRESULT *pResult);
	void UpdateUI();
	afx_msg void OnBnClickedBtnCustomLabelSave();
};
