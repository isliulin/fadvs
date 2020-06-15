// DlgURLPrompt.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgURLPrompt.h"


// CDlgURLPrompt dialog

IMPLEMENT_DYNAMIC(CDlgURLPrompt, CDialog)

CDlgURLPrompt::CDlgURLPrompt(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgURLPrompt::IDD, pParent)
{

}

CDlgURLPrompt::~CDlgURLPrompt()
{
}

void CDlgURLPrompt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgURLPrompt, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgURLPrompt::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgURLPrompt::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgURLPrompt message handlers

void CDlgURLPrompt::OnBnClickedOk()
{
	CString str;
	GetDlgItemText(IDC_EDIT_URL,str);
	strcpy(g_pFrm->m_CmdRun.ProductParam.ServerURL,str);
	theApp.ProductParam(FALSE);
	OnOK();
}

void CDlgURLPrompt::OnBnClickedCancel()
{
	OnCancel();
}
