// DlgValveHeat.cpp : implementation file
#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgValveHeat.h"


// CDlgValveHeat dialog

IMPLEMENT_DYNAMIC(CDlgValveHeat, CDialog)

CDlgValveHeat::CDlgValveHeat(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgValveHeat::IDD, pParent)
{

}

CDlgValveHeat::~CDlgValveHeat()
{
}

void CDlgValveHeat::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgValveHeat, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_APPLY, &CDlgValveHeat::OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_BTN_HEATA, &CDlgValveHeat::OnBnClickedBtnHeata)
	ON_BN_CLICKED(IDC_BTN_HEATB, &CDlgValveHeat::OnBnClickedBtnHeatb)
	ON_BN_CLICKED(IDC_BTN_HEATC, &CDlgValveHeat::OnBnClickedBtnHeatc)
	ON_BN_CLICKED(IDC_BTN_ADJUSTA, &CDlgValveHeat::OnBnClickedBtnAdjusta)
	ON_BN_CLICKED(IDC_BTN_ADJUSTB, &CDlgValveHeat::OnBnClickedBtnAdjustb)
	ON_BN_CLICKED(IDC_BTN_ADJUSTC, &CDlgValveHeat::OnBnClickedBtnAdjustc)
END_MESSAGE_MAP()


bool CDlgValveHeat::UpdateUI()
{
	bool bReadData = false;
	long lReadData = 0;
	double dReadData = 0.0;
	CString str;

	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1340");
		if(!bReadData)
		{
			return false;
		}
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_SETA,str);

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1342");
		if(!bReadData)
		{
			return false;
		}
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_SETB,str);

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1344");
		if(!bReadData)
		{
			return false;
		}
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_SETC,str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		lReadData = theApp.m_Serial.ReadPlcData("1340");
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_SETA,str);

		lReadData = theApp.m_Serial.ReadPlcData("1342");
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_SETB,str);

		lReadData = theApp.m_Serial.ReadPlcData("1344");
		str.Format("%d",int(lReadData*MM_FACTOR));
		SetDlgItemText(IDC_EDIT_SETC,str);
	}
	return true;
}
bool CDlgValveHeat::UpdateHeatState()
{
	bool bReadData = false;
	long lReadData = 0;
	double dReadData = 0.0;
	CString str;
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"MR","1610");
		if(!bReadData)
		{
			return false;
		}
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_HEATA))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_HEATA))->SetCheck(0);
		}

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"MR","1611");
		if(!bReadData)
		{
			return false;
		}
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_HEATB))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_HEATB))->SetCheck(0);
		}

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"MR","1612");
		if(!bReadData)
		{
			return false;
		}
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_HEATC))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_HEATC))->SetCheck(0);
		}


		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"MR","1613");
		if(!bReadData)
		{
			return false;
		}
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_ADJUSTA))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_ADJUSTA))->SetCheck(0);
		}

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"MR","1614");
		if(!bReadData)
		{
			return false;
		}
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_ADJUSTB))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_ADJUSTB))->SetCheck(0);
		}

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"MR","1615");
		if(!bReadData)
		{
			return false;
		}
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_ADJUSTC))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_ADJUSTC))->SetCheck(0);
		}

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1540");
		if(!bReadData)
		{
			return false;
		}
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_REALTA,str);

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1542");
		if(!bReadData)
		{
			return false;
		}
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_REALTB,str);

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1544");
		if(!bReadData)
		{
			return false;
		}
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_REALTC,str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		lReadData = theApp.m_Serial.ReadPlcRData("1610");
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_HEATA))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_HEATA))->SetCheck(0);
		}

		lReadData = theApp.m_Serial.ReadPlcData("1611");
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_HEATB))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_HEATB))->SetCheck(0);
		}

		lReadData = theApp.m_Serial.ReadPlcData("1612");
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_HEATC))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_HEATC))->SetCheck(0);
		}

		lReadData = theApp.m_Serial.ReadPlcData("1613");
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_ADJUSTA))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_ADJUSTA))->SetCheck(0);
		}

		lReadData = theApp.m_Serial.ReadPlcData("1614");
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_ADJUSTB))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_ADJUSTB))->SetCheck(0);
		}

		lReadData = theApp.m_Serial.ReadPlcData("1615");
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_ADJUSTC))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_ADJUSTC))->SetCheck(0);
		}

		lReadData = theApp.m_Serial.ReadPlcData("1540");
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_REALTA,str);

		lReadData = theApp.m_Serial.ReadPlcData("1542");
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_REALTB,str);

		lReadData = theApp.m_Serial.ReadPlcData("1544");
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_REALTB,str);
	}
	return true;
}
void CDlgValveHeat::OnTimer(UINT_PTR nIDEvent)
{
    UpdateHeatState();
	CDialog::OnTimer(nIDEvent);
}

BOOL CDlgValveHeat::OnInitDialog()
{
	CDialog::OnInitDialog();
	if(theApp.m_Serial.m_Port[0].m_iComState>0)
	{
		if(!UpdateUI())
		{
			AfxMessageBox("PLC数据读取失败！");
		}
	}
	else
	{
		AfxMessageBox("PLC通讯失败！");
	}
	SetTimer(0,500,NULL);
	return TRUE;  
}

void CDlgValveHeat::OnBnClickedBtnApply()
{
	BOOL bRet = 0;
	long lWriteData = 0;
	double dWriteData = 0.0;
	int nWriteData = 0;
	CString str;

	GetDlgItemText(IDC_EDIT_SETA,str);
	dWriteData = atof(str);
	lWriteData = long(dWriteData/MM_FACTOR);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePlcData(lWriteData,"DM","1340");
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePLCData("1340",lWriteData);
	}
	if(!bRet)
	{
		AfxMessageBox("保存失败！");
		return;
	}
	Sleep(100);

	GetDlgItemText(IDC_EDIT_SETB,str);
	dWriteData = atof(str);
	lWriteData = long(dWriteData/MM_FACTOR);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePlcData(lWriteData,"DM","1342");
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePLCData("1342",lWriteData);
	}
	if(!bRet)
	{
		AfxMessageBox("保存失败！");
		return;
	}
	Sleep(100);

	GetDlgItemText(IDC_EDIT_SETC,str);
	dWriteData = atof(str);
	lWriteData = long(dWriteData/MM_FACTOR);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePlcData(lWriteData,"DM","1344");
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePLCData("1344",lWriteData);
	}
	if(!bRet)
	{
		AfxMessageBox("保存失败！");
		return;
	}
	Sleep(100);
	AfxMessageBox("保存成功！");
}

void CDlgValveHeat::OnBnClickedBtnHeata()
{
	CString str;
	str.Format("1610");
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcData(1,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcROut(str,true);
	}
	Sleep(200);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcData(0,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcROut(str,false);
	}
}

void CDlgValveHeat::OnBnClickedBtnHeatb()
{
	CString str;
	str.Format("1611");
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcData(1,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcROut(str,true);
	}
	Sleep(200);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcData(0,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcROut(str,false);
	}
}

void CDlgValveHeat::OnBnClickedBtnHeatc()
{
	CString str;
	str.Format("1612");
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcData(1,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcROut(str,true);
	}
	Sleep(200);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcData(0,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcROut(str,false);
	}
}

void CDlgValveHeat::OnBnClickedBtnAdjusta()
{
	CString str;
	str.Format("1613");
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcData(1,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcROut(str,true);
	}
	Sleep(200);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcData(0,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcROut(str,false);
	}
}

void CDlgValveHeat::OnBnClickedBtnAdjustb()
{
	CString str;
	str.Format("1614");
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcData(1,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcROut(str,true);
	}
	Sleep(200);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcData(0,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcROut(str,false);
	}
}

void CDlgValveHeat::OnBnClickedBtnAdjustc()
{
	CString str;
	str.Format("1615");
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcData(1,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcROut(str,true);
	}
	Sleep(200);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcData(0,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcROut(str,false);
	}
}
