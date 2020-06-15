// DlgHeightCalib.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgHeightCalib.h"


// CDlgHeightCalib dialog

IMPLEMENT_DYNAMIC(CDlgHeightCalib, CDialog)

CDlgHeightCalib::CDlgHeightCalib(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgHeightCalib::IDD, pParent)
{
	dContactValue = 0.0;
	dHeightValue = 0.0;
}

CDlgHeightCalib::~CDlgHeightCalib()
{
}

void CDlgHeightCalib::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgHeightCalib, CDialog)
	ON_WM_TIMER()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_HEIGHT_CALIB_SPEED, &CDlgHeightCalib::OnNMCustomdrawSliderHeightCalibSpeed)
	ON_BN_CLICKED(IDC_BTN_HEIGHT_CALIB_NEXT, &CDlgHeightCalib::OnBnClickedBtnHeightCalibNext)
	ON_BN_CLICKED(IDOK, &CDlgHeightCalib::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgHeightCalib::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgHeightCalib message handlers

BOOL CDlgHeightCalib::OnInitDialog()
{
	CDialog::OnInitDialog();
	g_pFrm->bIsHeightCalibVisible = true;
	for(int i=0;i<6;i++)
	{
		m_cbMotion[i].m_iIndex = i;
		m_cbMotion[i].SubclassDlgItem(IDC_BTN_HEIGHT_CALIB_X_NEG+i,this);
		m_cbMotion[i].SetBitmapEx(IDB_BITMAP_MOVE);
		m_cbMotion[i].m_pMv = &theApp.m_Mv400;
	}
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_HEIGHT_CALIB_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_HEIGHT_CALIB_SPEED))->SetPos(1);

	CString str;
	str.Format("移动点胶头2到接触位");
	SetDlgItemText(IDC_EDIT_HEIGHT_CALIB_PROMPT,str);
	GetDlgItem(IDOK)->EnableWindow(FALSE);

	if(theApp.m_tSysParam.sensorType==ContactSensorHiPrecision)
	{
		str.Format("%0.2f",g_pFrm->m_CmdRun.ProductParam.ContactStandardValue);
		SetDlgItemText(IDC_EDIT_HEIGHT_CALIB_STANDARD_VALUE,str);
	}
	else if(theApp.m_tSysParam.sensorType==ContactSensorClose)
	{
		m_hBitmap = ::LoadBitmap(::AfxGetResourceHandle(),MAKEINTRESOURCE(IDB_BITMAP_SENSOR_OFF));
		((CButton *)GetDlgItem(IDC_BTN_HEIGHT_CALIB_SWITCH))->SetBitmap(m_hBitmap);
	}
	else if(theApp.m_tSysParam.sensorType==ContactSensorOpen)
	{
		m_hBitmap = ::LoadBitmap(::AfxGetResourceHandle(),MAKEINTRESOURCE(IDB_BITMAP_SENSOR_ON));
		((CButton *)GetDlgItem(IDC_BTN_HEIGHT_CALIB_SWITCH))->SetBitmap(m_hBitmap);
	}

	str.Format("%0.2f",g_pFrm->m_CmdRun.ProductParam.HeightStandardValue);
	SetDlgItemText(IDC_EDIT_HEIGHT_CALIB_HIGHT_STANDARD_VALUE,str);
	MoveToContactSensorPos();

	SetTimer(0,500,NULL);
	return TRUE;
}

void CDlgHeightCalib::OnNMCustomdrawSliderHeightCalibSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_HEIGHT_CALIB_SPEED))->GetPos();
	CColorButton::m_iSpeedNo = n;
	CString strTemp;
	strTemp.Format("X:%.4f,Y:%.4f,Z:%.4f", theApp.m_tSysParam.tAxis[0].vel[0] * n/2000.0,
		theApp.m_tSysParam.tAxis[1].vel[0] * n/2000.0,theApp.m_tSysParam.tAxis[2].vel[0] * n/2000.0);
	SetDlgItemText(IDC_EDIT_HEIGHT_CALIB_SPEED,strTemp);
	*pResult = 0;
}

void CDlgHeightCalib::OnBnClickedBtnHeightCalibNext()
{
	// TODO: Add your control notification handler code here
	CString str;
	if(theApp.m_tSysParam.sensorType == ContactSensorHiPrecision)
	{
		g_pFrm->m_CmdRun.ReadContactSensorValue(&dContactValue);
	}
	m_tgContactPos.x = theApp.m_Mv400.GetPos(K_AXIS_X);
	m_tgContactPos.y = theApp.m_Mv400.GetPos(K_AXIS_Y);
	m_tgContactPos.za = theApp.m_Mv400.GetPos(K_AXIS_ZA);
	Sleep(100);
	MoveToHeightSensorPos();
	str.Format("移动高度传感器到接触位");
	SetDlgItemText(IDC_EDIT_HEIGHT_CALIB_PROMPT,str);
	GetDlgItem(IDC_BTN_HEIGHT_CALIB_NEXT)->EnableWindow(FALSE);
	GetDlgItem(IDOK)->EnableWindow(TRUE);
}

void CDlgHeightCalib::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ReadHeightSensorValue(&dHeightValue);
	m_tgHeightPos.x = theApp.m_Mv400.GetPos(K_AXIS_X);
	m_tgHeightPos.y = theApp.m_Mv400.GetPos(K_AXIS_Y);
	m_tgHeightPos.za = theApp.m_Mv400.GetPos(K_AXIS_ZA);
	SaveData();
	g_pFrm->m_CmdRun.MoveToZSafety();
	g_pFrm->bIsHeightCalibVisible = false;
	OnOK();
}

void CDlgHeightCalib::UpdateContactSensorValue()
{
	if(theApp.m_tSysParam.sensorType == ContactSensorHiPrecision)
	{
		CString str;
		double dHeight = 0.0;
		g_pFrm->m_CmdRun.ReadContactSensorValue(&dHeight);
		str.Format("%0.2f",dHeight);
		SetDlgItemText(IDC_EDIT_HEIGHT_CALIB_CURRENT_VALUE,str);
	}
	else if(theApp.m_tSysParam.sensorType == ContactSensorClose||theApp.m_tSysParam.sensorType == ContactSensorOpen)
	{
		DeleteObject(m_hBitmap);
		if(!theApp.m_Mv400.GetInput(theApp.m_tSysParam.inHeight))
		{
			m_hBitmap = ::LoadBitmap(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP_SENSOR_OFF));
		}
		else
		{
			m_hBitmap = ::LoadBitmap(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP_SENSOR_ON));
		}
		((CButton*)GetDlgItem(IDC_BTN_HEIGHT_CALIB_SWITCH))->SetBitmap(m_hBitmap);
	}
}

void CDlgHeightCalib::UpdateHeightSensorValue()
{
	CString str;
	double dReadValue = 0;
	g_pFrm->m_CmdRun.ReadHeightSensorValue(&dReadValue);
	str.Format("%0.2f",dReadValue);
	SetDlgItemText(IDC_EDIT_HEIGHT_CALIB_HIGHT_CURRENT_VALUE,str);
}

void CDlgHeightCalib::OnTimer(UINT_PTR nIDEvent)
{
	UpdateHeightSensorValue();
	UpdateContactSensorValue();
	CDialog::OnTimer(nIDEvent);
}

void CDlgHeightCalib::MoveToContactSensorPos()
{
	if(theApp.m_Mv400.IsInitOK())
	{
		if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_STOP)
		{
			if(!g_pFrm->m_CmdRun.MoveToZSafety())
			{
				return;
			}
			if(AfxMessageBox("确定移动针头到接触传感器位置？",MB_YESNO)!=IDYES)
			{
				return;
			}
			if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[0],g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[1],true))
			{
				AfxMessageBox("X轴或Y轴运动失败！");
				return;
			}
			if(!g_pFrm->m_CmdRun.SynchronizeMoveZ((g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]-10)<40?(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]-10):40,true))
			{
				AfxMessageBox("Z轴运动失败！");
				return;
			}
			theApp.m_Mv400.Move(K_AXIS_ZA,g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2],
				g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel,g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc,TRUE);
			if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
			{
				AfxMessageBox("设定速度失败！");
				return;
			}
		}
	}
}

void CDlgHeightCalib::MoveToHeightSensorPos()
{
	if(theApp.m_Mv400.IsInitOK())
	{
		if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_STOP)
		{
			if(!g_pFrm->m_CmdRun.MoveToZSafety())
			{
				return;
			}
			if(AfxMessageBox("确定移动高度传感器到接触位？",MB_YESNO)!=IDYES)
			{
				return;
			}
			if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.HeightSensorPos[0],g_pFrm->m_CmdRun.ProductParam.HeightSensorPos[1],true))
			{
				return;
			}
			if(!g_pFrm->m_CmdRun.SynchronizeMoveZ(g_pFrm->m_CmdRun.ProductParam.HeightSensorPos[2],true))
			{
				return;
			}
		}
	}
}

void CDlgHeightCalib::SaveData()
{
	g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[0] = m_tgContactPos.x;
	g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[1] = m_tgContactPos.y;
	g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2] = m_tgContactPos.za;

	g_pFrm->m_CmdRun.ProductParam.HeightSensorPos[0] = m_tgHeightPos.x;
	g_pFrm->m_CmdRun.ProductParam.HeightSensorPos[1] = m_tgHeightPos.y;
	g_pFrm->m_CmdRun.ProductParam.HeightSensorPos[2] = m_tgHeightPos.za;

	theApp.ProductParam(FALSE);
}
void CDlgHeightCalib::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code her
	g_pFrm->m_CmdRun.MoveToZSafety();
	g_pFrm->bIsHeightCalibVisible = false;
	OnCancel();
}
