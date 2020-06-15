#pragma once


// CV9Param dialog

class CV9Param : public CDialog
{
	DECLARE_DYNAMIC(CV9Param)

public:
	CV9Param(CWnd* pParent = NULL);   // standard constructor
	virtual ~CV9Param();

// Dialog Data
	enum { IDD = IDD_DlG_V9SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void OnV9SetExit();
	afx_msg void OnBnClickedFroceM(UINT nID);

	afx_msg void OnBnClickedResetAll();
	afx_msg void OnBnClickedHomeAll();
	afx_msg void OnBnClickedDispensingAll();
	afx_msg void OnBnClickedClearAll();
	afx_msg void OnBnClickedUniformParam();
	void UpeditData(void);
	virtual BOOL OnInitDialog();
	bool SaveAllParam(int ParamID);
	afx_msg void OnBnClickedSaveAll();
	afx_msg void OnEnChangeEdit1(UINT nID);
	afx_msg void OnEnFirstCompensation(UINT nID);

	double get_Disppost[3];
	double get_DispRestD[3];
	long  get_dispcount[3];

	afx_msg void OnBnClickedPress1(UINT nID);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClose();

	void ButtonEnable(bool bEnable);
	afx_msg void OnBnClickedBtnBlockclean();
	afx_msg void OnBnClickedBtnUnionSaveAll();
};
