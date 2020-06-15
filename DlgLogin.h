#include "UserManage.h"
#pragma once


// CDlgLogin dialog

class CDlgLogin : public CDialog
{
	DECLARE_DYNAMIC(CDlgLogin)

public:
	CDlgLogin(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLogin();

	// Dialog Data
	enum { IDD = IDD_DLG_LOGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonPwd();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeUsertype();
	virtual BOOL OnInitDialog();
public:
	USERDATA m_LoginData;
	BOOL		Lonin();
	BOOL		Logoff();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedUserLogoff();
};
