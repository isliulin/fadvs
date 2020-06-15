// DlgPrompt.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgPrompt.h"


// CDlgPrompt dialog

IMPLEMENT_DYNAMIC(CDlgPrompt, CDialog)

CDlgPrompt::CDlgPrompt(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPrompt::IDD, pParent)
{
}

CDlgPrompt::~CDlgPrompt()
{
}

void CDlgPrompt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgPrompt, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgPrompt::OnBnClickedOk)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

// CDlgPrompt message handlers

BOOL CDlgPrompt::OnInitDialog()
{
	CDialog::OnInitDialog();
	if(m_strPrompt!="")
	{
		SetDlgItemText(IDC_EDIT_PROMPT_INFO,m_strPrompt);
	}
	return TRUE;
}
void CDlgPrompt::OnBnClickedOk()
{
	
	OnOK();
}

HBRUSH CDlgPrompt::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if(pWnd->GetDlgCtrlID()==IDC_EDIT_PROMPT_INFO)
	{
		pDC->SetTextColor(RGB(255,0,0));
	}
	return hbr;
}
