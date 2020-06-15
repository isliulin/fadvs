#pragma once
#include "Resource.h"
#include "EncryptOp.h"

// CUserRegister dialog

class CUserRegister : public CDialog
{
	DECLARE_DYNAMIC(CUserRegister)

public:
	CUserRegister(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUserRegister();
	CString  strDeviceCode;//ЛњЦїТы

	
	// Dialog Data
	enum { IDD = IDD_DLG_USER_REGISTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void StrToUpper(CString &CStr);
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
