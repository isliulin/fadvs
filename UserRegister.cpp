// UserRegister.cpp : implementation file
//

#include "stdafx.h"
#include "UserRegister.h"
#include "TSCtrlSys.h"

// CUserRegister dialog

IMPLEMENT_DYNAMIC(CUserRegister, CDialog)

CUserRegister::CUserRegister(CWnd* pParent /*=NULL*/)
: CDialog(CUserRegister::IDD, pParent)
{

}

CUserRegister::~CUserRegister()
{

}

void CUserRegister::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CUserRegister, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CUserRegister::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CUserRegister::OnBnClickedCancel)
END_MESSAGE_MAP()

BOOL CUserRegister::OnInitDialog()
{ 
	CDialog::OnInitDialog();

	CString Str = theApp.m_EncryptOp.GetDriverCode();
	if(!Str.IsEmpty())
	{
		SetDlgItemText(IDC_EDIT_MACHINE_CODE,Str);
	}
	CRegisterCode RegMsg;
	CString m_OutStr;
	RegMsg = theApp.m_EncryptOp.IsInstalled();
	RegMsg.m_sDeviceCode = Str;
	if (!RegMsg.m_sRegisterCode.CompareNoCase("Init"))
	{
		m_OutStr.Format("%d", RegMsg.m_LimitDays);
		Str = "********";
	}
	else
	{
		m_OutStr.Format("%d", RegMsg.m_LimitDays + 1);
		Str = "******" + RegMsg.m_sRegisterCode.Mid(14, 2);
	}
	
	if (RegMsg.m_nCurIdAllDays >= 0)
	{
		switch ((int)(RegMsg.m_bRegisterFlag))
		{
		case 0:
			SetDlgItemText(IDC_EDIT_REGISTER_CODE_TYPE, "试用版本");			
			SetDlgItemText(IDC_EDIT_REGISTER_CODE, Str);//注册码框
			SetDlgItemText(IDC_EDIT_VALIDITY_PERIOD, m_OutStr);//有效期框
			break;
		case 1:			
			SetDlgItemText(IDC_EDIT_REGISTER_CODE_TYPE, "正式版本");
			SetDlgItemText(IDC_EDIT_REGISTER_CODE,  "已注册正式版本");//注册码框
			SetDlgItemText(IDC_EDIT_VALIDITY_PERIOD, "持续有效");//有效期框
			break;
		}
	}
	else
	{

		if (!theApp.m_EncryptOp.m_bLockSystem)
		{
			theApp.m_EncryptOp.m_bLockSystem = true;
		}

		SetDlgItemText(IDC_EDIT_REMAINING_TIME, "软件已试用过期锁定");
	}
	
	SetTimer(0, 1000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

//字符串转大写
void CUserRegister::StrToUpper(CString &CStr)
{
	 CString tmp = "", tmp1 = "", ret = "";
	 for (int i = 0; i < CStr.GetLength(); i++)
	 {
		 tmp = CStr.Mid(i, 1);
		 if (tmp == " ")
		 {
			 tmp = "";
		 }
		 else if (tmp == "-")
		 {
			 tmp = "";
		 }
		 tmp1 += tmp;

	 }
	 CStr = tmp1.MakeUpper();
}

void CUserRegister::OnBnClickedOk()
{
	CString m_OutStr;
	CString strCode;
	CString strDeviceCode;//机器码
	CRegisterCode RegMsg;
	RegMsg = theApp.m_EncryptOp.IsInstalled();
	GetDlgItem(IDC_EDIT_MACHINE_CODE)->GetWindowText(strDeviceCode);
	
	if (RegMsg.m_bRegisterFlag)
	{
		// 已注册正式版本
		AfxMessageBox("软件已注册正式版");
		OnOK();
		return;
	}
	else
	{
		// 未注册
		GetDlgItem(IDC_EDIT_REGISTER_CODE)->GetWindowText(strCode);
		StrToUpper(strCode);//注册码
		if (strCode.GetLength() != 16)
		{
			// 注册码位数不对
			MessageBox(_T("注册码位数不正确"));
			OnCancel();
			return;
		}
		//计算注册信息
		int rtn = theApp.m_EncryptOp.CalcPassWord(strDeviceCode, strCode, RegMsg);

		if (rtn != 0)
		{
			switch(rtn)
			{
				case -1:
					m_OutStr.Format("%s", "注册码错误或当前时间小于上次过期时间");
					break;
				case 1:
					m_OutStr.Format("%s", "注册码错误，请检查注册码");
					break;
				case 2:
					m_OutStr.Format("%s", "注册失败，期限时间小于当前时间");
					break;
				case 3:
					m_OutStr.Format("%s", "注册失败, 当前时间在上次程序关闭时间前");
					break;
				default:
					m_OutStr.Format("%s", "注册时出现异常");
					break;
			}
			//注册失败。请重新注册...
			MessageBox(m_OutStr);
			//OnCancel();
			return;

		}
		
		RegMsg.m_sDeviceCode = strDeviceCode;
		RegMsg.m_sRegisterCode = strCode;
		//注册
		theApp.m_EncryptOp.RegistInfo(RegMsg);
		strcpy(theApp.m_tSysParam.RegCode, strCode); 
		theApp.SysParam(FALSE);
		//RegMsg = theApp.m_EncryptOp.IsInstalled();
		AfxMessageBox("注册成功，欢迎使用");
	}
	OnOK();
}
void CUserRegister::OnBnClickedCancel()
{
	OnCancel();
}

void CUserRegister::OnTimer(UINT_PTR nIDEvent)
{
	// 获取当前注册码信息
	CRegisterCode RegMsg;
	CString m_OutStr;
	RegMsg = theApp.m_EncryptOp.IsInstalled();       
	COleDateTimeSpan span = RegMsg.m_dtLimitTime - RegMsg.m_dtLastTime;
	if (span.m_span >= 0)
	{

		if (RegMsg.m_bRegisterFlag)
		{
			SetDlgItemText(IDC_EDIT_REGISTER_CODE, "软件已注册正式版本");
		}
		else
		{
			if (theApp.m_EncryptOp.m_bLockSystem)
			{
				SetDlgItemText(IDC_EDIT_REMAINING_TIME, "软件已试用过期锁定");
			}
			else
			{
				COleDateTimeSpan timeSpan = RegMsg.m_dtLimitTime - RegMsg.m_dtLastTime;
				m_OutStr.Format("%d天%d小时%d分%d秒", timeSpan.GetDays(), timeSpan.GetHours(), timeSpan.GetMinutes(), timeSpan.GetSeconds());
				SetDlgItemText(IDC_EDIT_REMAINING_TIME, m_OutStr);
				//m_OutStr.Format("%d", RegMsg.m_LimitDays + 1);
				//SetDlgItemText(IDC_EDIT_VALIDITY_PERIOD, m_OutStr);//有效期框
				
			}
		}
	}
	else
	{

		if (!theApp.m_EncryptOp.m_bLockSystem)
		{
			theApp.m_EncryptOp.m_bLockSystem = true;
		}

		SetDlgItemText(IDC_EDIT_REMAINING_TIME, "软件已试用过期锁定");
		//RegMsg.m_dtLastTime = COleDateTime::GetCurrentTime();
		//TGlobal.encrypt.SetAllOFRegisterCode(RegMsg);
	}

	CDialog::OnTimer(nIDEvent);
}

