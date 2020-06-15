// DlgRunInfo.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgRunInfo.h"


// CDlgRunInfo dialog

IMPLEMENT_DYNAMIC(CDlgRunInfo, CDialog)

CDlgRunInfo::CDlgRunInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRunInfo::IDD, pParent)
{

}

CDlgRunInfo::~CDlgRunInfo()
{
}

void CDlgRunInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRunInfo, CDialog)
END_MESSAGE_MAP()


// CDlgRunInfo message handlers

BOOL CDlgRunInfo::OnInitDialog()
{
	CDialog::OnInitDialog();
    LoadData();
	return TRUE;  
}
bool CDlgRunInfo::LoadData()
{
	bool bReadData = false;
	long lReadData = 0;
	double dReadData = 0.0;
	CString str;
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1240");
		if(!bReadData)
		{
			return false;
		}
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_TEMPONE,str);

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1242");
		if(!bReadData)
		{
			return false;
		}
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_TEMPTWO,str);

	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		lReadData = theApp.m_Serial.ReadPlcData("1240");
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_TEMPONE,str);

		lReadData = theApp.m_Serial.ReadPlcData("1242");
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_TEMPTWO,str);
	}
	return true;
}
