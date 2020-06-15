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
			SetDlgItemText(IDC_EDIT_REGISTER_CODE_TYPE, "���ð汾");			
			SetDlgItemText(IDC_EDIT_REGISTER_CODE, Str);//ע�����
			SetDlgItemText(IDC_EDIT_VALIDITY_PERIOD, m_OutStr);//��Ч�ڿ�
			break;
		case 1:			
			SetDlgItemText(IDC_EDIT_REGISTER_CODE_TYPE, "��ʽ�汾");
			SetDlgItemText(IDC_EDIT_REGISTER_CODE,  "��ע����ʽ�汾");//ע�����
			SetDlgItemText(IDC_EDIT_VALIDITY_PERIOD, "������Ч");//��Ч�ڿ�
			break;
		}
	}
	else
	{

		if (!theApp.m_EncryptOp.m_bLockSystem)
		{
			theApp.m_EncryptOp.m_bLockSystem = true;
		}

		SetDlgItemText(IDC_EDIT_REMAINING_TIME, "��������ù�������");
	}
	
	SetTimer(0, 1000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

//�ַ���ת��д
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
	CString strDeviceCode;//������
	CRegisterCode RegMsg;
	RegMsg = theApp.m_EncryptOp.IsInstalled();
	GetDlgItem(IDC_EDIT_MACHINE_CODE)->GetWindowText(strDeviceCode);
	
	if (RegMsg.m_bRegisterFlag)
	{
		// ��ע����ʽ�汾
		AfxMessageBox("�����ע����ʽ��");
		OnOK();
		return;
	}
	else
	{
		// δע��
		GetDlgItem(IDC_EDIT_REGISTER_CODE)->GetWindowText(strCode);
		StrToUpper(strCode);//ע����
		if (strCode.GetLength() != 16)
		{
			// ע����λ������
			MessageBox(_T("ע����λ������ȷ"));
			OnCancel();
			return;
		}
		//����ע����Ϣ
		int rtn = theApp.m_EncryptOp.CalcPassWord(strDeviceCode, strCode, RegMsg);

		if (rtn != 0)
		{
			switch(rtn)
			{
				case -1:
					m_OutStr.Format("%s", "ע��������ǰʱ��С���ϴι���ʱ��");
					break;
				case 1:
					m_OutStr.Format("%s", "ע�����������ע����");
					break;
				case 2:
					m_OutStr.Format("%s", "ע��ʧ�ܣ�����ʱ��С�ڵ�ǰʱ��");
					break;
				case 3:
					m_OutStr.Format("%s", "ע��ʧ��, ��ǰʱ�����ϴγ���ر�ʱ��ǰ");
					break;
				default:
					m_OutStr.Format("%s", "ע��ʱ�����쳣");
					break;
			}
			//ע��ʧ�ܡ�������ע��...
			MessageBox(m_OutStr);
			//OnCancel();
			return;

		}
		
		RegMsg.m_sDeviceCode = strDeviceCode;
		RegMsg.m_sRegisterCode = strCode;
		//ע��
		theApp.m_EncryptOp.RegistInfo(RegMsg);
		strcpy(theApp.m_tSysParam.RegCode, strCode); 
		theApp.SysParam(FALSE);
		//RegMsg = theApp.m_EncryptOp.IsInstalled();
		AfxMessageBox("ע��ɹ�����ӭʹ��");
	}
	OnOK();
}
void CUserRegister::OnBnClickedCancel()
{
	OnCancel();
}

void CUserRegister::OnTimer(UINT_PTR nIDEvent)
{
	// ��ȡ��ǰע������Ϣ
	CRegisterCode RegMsg;
	CString m_OutStr;
	RegMsg = theApp.m_EncryptOp.IsInstalled();       
	COleDateTimeSpan span = RegMsg.m_dtLimitTime - RegMsg.m_dtLastTime;
	if (span.m_span >= 0)
	{

		if (RegMsg.m_bRegisterFlag)
		{
			SetDlgItemText(IDC_EDIT_REGISTER_CODE, "�����ע����ʽ�汾");
		}
		else
		{
			if (theApp.m_EncryptOp.m_bLockSystem)
			{
				SetDlgItemText(IDC_EDIT_REMAINING_TIME, "��������ù�������");
			}
			else
			{
				COleDateTimeSpan timeSpan = RegMsg.m_dtLimitTime - RegMsg.m_dtLastTime;
				m_OutStr.Format("%d��%dСʱ%d��%d��", timeSpan.GetDays(), timeSpan.GetHours(), timeSpan.GetMinutes(), timeSpan.GetSeconds());
				SetDlgItemText(IDC_EDIT_REMAINING_TIME, m_OutStr);
				//m_OutStr.Format("%d", RegMsg.m_LimitDays + 1);
				//SetDlgItemText(IDC_EDIT_VALIDITY_PERIOD, m_OutStr);//��Ч�ڿ�
				
			}
		}
	}
	else
	{

		if (!theApp.m_EncryptOp.m_bLockSystem)
		{
			theApp.m_EncryptOp.m_bLockSystem = true;
		}

		SetDlgItemText(IDC_EDIT_REMAINING_TIME, "��������ù�������");
		//RegMsg.m_dtLastTime = COleDateTime::GetCurrentTime();
		//TGlobal.encrypt.SetAllOFRegisterCode(RegMsg);
	}

	CDialog::OnTimer(nIDEvent);
}

