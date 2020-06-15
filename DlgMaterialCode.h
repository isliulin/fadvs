#pragma once
#include "resource.h"

// CDlgMaterialCode dialog

class CDlgMaterialCode : public CDialog
{
	DECLARE_DYNAMIC(CDlgMaterialCode)

public:
	CDlgMaterialCode(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMaterialCode();

// Dialog Data
	enum { IDD = IDD_DLG_MATERIALCODE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnSend();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
