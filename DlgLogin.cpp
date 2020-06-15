// DlgLogin.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgLogin.h"


// CDlgLogin dialog

IMPLEMENT_DYNAMIC(CDlgLogin, CDialog)

CDlgLogin::CDlgLogin(CWnd* pParent /*=NULL*/)
: CDialog(CDlgLogin::IDD, pParent)
{

}

CDlgLogin::~CDlgLogin()
{
}

void CDlgLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgLogin, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_PWD, &CDlgLogin::OnBnClickedButtonPwd)
	ON_BN_CLICKED(IDOK, &CDlgLogin::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_USERTYPE, &CDlgLogin::OnCbnSelchangeUsertype)
	ON_BN_CLICKED(IDCANCEL, &CDlgLogin::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_USER_LOGOFF, &CDlgLogin::OnBnClickedUserLogoff)
END_MESSAGE_MAP()


// CDlgLogin message handlers

void CDlgLogin::OnBnClickedButtonPwd()
{
	CString str1, str2,str3;
	GetDlgItemText(IDC_ACCESS_EDIT1, str1);
	GetDlgItemText(IDC_ACCESS_EDIT2, str2);
	str3.Format("%s",theApp.m_SysUser.m_CurUD.Password);
	char *cNewCh = (LPSTR)(LPCSTR)str2;

	if(str2.GetLength() < 4)
	{
		AfxMessageBox("失败:新密码小于4位!");
	}
	else if(0==str3.Compare(str1))
	{
		memcpy(theApp.m_SysUser.m_CurUD.Password,cNewCh,sizeof(char)*20);
		if(theApp.m_SysUser.ChangeUser(theApp.m_SysUser.m_CurUD))
		{
			memcpy(m_LoginData.Password,cNewCh,sizeof(char)*20);
			theApp.m_SysUser.SaveUserData();
			AfxMessageBox("密码修改成功!");
		}
		else
		{
			AfxMessageBox("密码修改失败!");
		}
	}
	else
	{
		AfxMessageBox("密码输入错误!");
	}
	OnOK();
}

void CDlgLogin::OnBnClickedOk()
{
	CString str1, str2;
	GetDlgItemText(IDC_ACCESS_EDIT1, str1);
	CComboBox *pUserBox = (CComboBox*)GetDlgItem(IDC_USERTYPE);
	if(pUserBox->GetCurSel() == 0)
	{
		OnBnClickedUserLogoff();
		return;
	}
	char user[20] = "default";
	m_LoginData.level = pUserBox->GetCurSel();
	memcpy(m_LoginData.UserName,user,sizeof(char)*20);
	char *pCh = (LPSTR)(LPCSTR)str1;
	memset(m_LoginData.Password,0,sizeof(char)*20);
	memcpy(m_LoginData.Password,pCh,sizeof(char)*str1.GetLength());
	int val = theApp.m_SysUser.FindUser(m_LoginData);
	if(val >= 0)
	{
		theApp.m_SysUser.m_CurUD = m_LoginData;
		theApp.m_SysUser.m_CurUD.uID = val;
		if(theApp.m_SysUser.m_CurUD.level > GM_ADMIN)
		{
			//g_pFrm->m_wndToolBar.LoadToolBar(IDR_MAINFRAME_256);//.ReplaceButton(ID_LOGON,CMFCToolBarMenuButton (ID_LOGON,menuHistory,GetCmdMgr()->GetCmdImage(ID_LOGON),str));
			g_pFrm->m_wndToolBar.RestoreOriginalstate();
		}
		g_pFrm->m_bAdmin = TRUE;
	}
	else
		AfxMessageBox("登录失败！");
	g_pFrm->Invalidate();
	OnOK();
}

void CDlgLogin::OnCbnSelchangeUsertype()
{
	CComboBox *pUserBox = (CComboBox*)GetDlgItem(IDC_USERTYPE);
	/*if(theApp.m_SysUser.m_CurUD.uID == pUserBox->GetCurSel() &&
	theApp.m_SysUser.m_CurUD.level == pUserBox->GetCurSel()+1)*/
	if(theApp.m_SysUser.m_CurUD.uID == (pUserBox->GetCurSel()) &&
		theApp.m_SysUser.m_CurUD.level == (pUserBox->GetCurSel())/*&&(theApp.m_SysUser.m_CurUD.level>GM_GUEST)*/)
	{
		GetDlgItem(IDC_ACCESS_EDIT2)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_PWD)->EnableWindow(TRUE);
	}
	/*else
	{
		GetDlgItem(IDC_ACCESS_EDIT2)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_PWD)->EnableWindow(FALSE);
	}*/
	GetDlgItem(IDC_ACCESS_EDIT1)->EnableWindow(TRUE);
	if(GM_GUEST == pUserBox->GetCurSel())
	{
		GetDlgItem(IDC_ACCESS_EDIT1)->EnableWindow(FALSE);
		GetDlgItem(IDC_ACCESS_EDIT2)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_PWD)->EnableWindow(FALSE);
	}
}

BOOL CDlgLogin::OnInitDialog()
{
	CDialog::OnInitDialog();
	CComboBox *pUserBox = (CComboBox*)GetDlgItem(IDC_USERTYPE);
	pUserBox->InsertString(0,"设备商");
	pUserBox->InsertString(0,"管理员");
	pUserBox->InsertString(0,"操作员");
	pUserBox->InsertString(0,"Guest");
	pUserBox->SetCurSel(theApp.m_SysUser.m_CurUD.uID);
	if(theApp.m_SysUser.m_CurUD.level <GM_PERSONNEL)
	{
		GetDlgItem(IDC_ACCESS_EDIT1)->EnableWindow(FALSE);
		GetDlgItem(IDC_ACCESS_EDIT2)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_PWD)->EnableWindow(FALSE);
	}
	else
	{
		if(theApp.m_SysUser.m_CurUD.uID == (pUserBox->GetCurSel()) &&
			theApp.m_SysUser.m_CurUD.level == (pUserBox->GetCurSel()))
		{
			GetDlgItem(IDC_ACCESS_EDIT2)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_PWD)->EnableWindow(TRUE);
		}
	}
	return TRUE;
}

void CDlgLogin::OnBnClickedCancel()
{
	OnCancel();
}

void CDlgLogin::OnBnClickedUserLogoff()
{
	theApp.m_SysUser.m_CurUD.level = GM_GUEST;
	theApp.m_SysUser.m_CurUD.uID = 0;
	g_pFrm->m_bAdmin = FALSE;
	OnCancel();
}