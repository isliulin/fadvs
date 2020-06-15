// DlgIOCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgIOCtrl.h"


// CDlgIOCtrl dialog

IMPLEMENT_DYNAMIC(CDlgIOCtrl, CDialog)

CDlgIOCtrl::CDlgIOCtrl(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgIOCtrl::IDD, pParent)
{
}

CDlgIOCtrl::~CDlgIOCtrl()
{
}

void CDlgIOCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgIOCtrl, CDialog)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_COMMAND_RANGE(IDC_CHECK_OUTPUT0,IDC_CHECK_OUTPUT31, &CDlgIOCtrl::OnBnClickedOutput)

END_MESSAGE_MAP()
// 初始化
BOOL CDlgIOCtrl::OnInitDialog()
{
	// TODO:  Add extra initialization here
	CDialog::OnInitDialog();
	this->SetTimer(0,200,NULL);
	return TRUE;  
}
// 定时器
void CDlgIOCtrl::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	for(int i = 0; i < 16; i++)
	{	
		((CButton*)GetDlgItem(IDC_CHECK_INPUT0 + i))->SetCheck(theApp.m_Mv400.GetInput(i));
	}
	CDialog::OnTimer(nIDEvent);
}
// 关闭
void CDlgIOCtrl::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	KillTimer(0);
	//FX_Close();
	CDialog::OnClose();
}
// 测试输出信号
void CDlgIOCtrl::OnBnClickedOutput(UINT nID)
{
	// TODO: Add your control notification handler code here
	UINT nBitNum = nID - IDC_CHECK_OUTPUT0;
	BOOL bSts = ((CButton*)GetDlgItem(nID))->GetCheck();
	theApp.m_Mv400.SetOutput(nBitNum,bSts);
}
// 向PLC发送输出信号，测试料架、气缸等
//void CDlgIOCtrl::OnBnClickedFroceM(UINT nID)
//{
//	// TODO: Add your control notification handler code here
//	CString str;
//	UINT nBitNum = nID - IDC_PLCFM0+1000;
//	str.Format("%d",nBitNum);
//	theApp.m_Serial.WritePlcROut(str,true);
//	Sleep(200);
//	theApp.m_Serial.WritePlcROut(str,false);
//}
