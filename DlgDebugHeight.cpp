// DlgDebugHeight.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgDebugHeight.h"


// CDlgDebugHeight dialog

IMPLEMENT_DYNAMIC(CDlgDebugHeight, CDialog)

CDlgDebugHeight::CDlgDebugHeight(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDebugHeight::IDD, pParent)
{

}

CDlgDebugHeight::~CDlgDebugHeight()
{
}

void CDlgDebugHeight::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgDebugHeight, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_DEBUG_HEIGHT_NEXT, &CDlgDebugHeight::OnBnClickedBtnDebugHeightNext)
	ON_BN_CLICKED(IDOK, &CDlgDebugHeight::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgDebugHeight::OnBnClickedCancel)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_DEBUG_HEIGHT_SPEED, &CDlgDebugHeight::OnNMCustomdrawSliderDebugHeightSpeed)
END_MESSAGE_MAP()


// CDlgDebugHeight message handlers
BOOL CDlgDebugHeight::OnInitDialog()
{
	g_pFrm->bIsDebugHeightVisible = true;
	for(int i=0;i<6;i++)
	{
		m_colorButton[i].m_iIndex = i;
		m_colorButton[i].SubclassDlgItem(IDC_BTN_DEBUG_HEIGHT_X_NEG+i,this);
		m_colorButton[i].SetBitmapEx(IDB_BITMAP_MOVE);
		m_colorButton[i].m_pMv = &theApp.m_Mv400;
	}
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_DEBUG_HEIGHT_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_DEBUG_HEIGHT_SPEED))->SetPos(1);
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_DEBUG_HEIGHT_SPEED))->GetPos();
	CColorButton::m_iSpeedNo = n;
	nStep = 0;
	if(theApp.m_tSysParam.sensorType==ContactSensorHiPrecision)
	{
		CString str;
		str.Format("%0.2f",g_pFrm->m_CmdRun.ProductParam.ContactStandardValue);
		SetDlgItemText(IDC_EDIT_DEBUG_HEIGHT_STANDARD_VALUE,str);
	}
	SetTimer(0,100,NULL);
	MoveToContactSensorPos(1);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	return TRUE;
}

void CDlgDebugHeight::OnBnClickedBtnDebugHeightNext()
{
	// TODO: Add your control notification handler code here
	if(nStep==0)
	{
		SavePos();
		Sleep(100);
		if(theApp.m_tSysParam.sensorType==ContactSensorHiPrecision)
		{
			CString str;
			double dHeight = 0.0;
			char szHeight[5];
			szHeight[0] = 0x4d;
			szHeight[1] = 0x30;
			szHeight[2] = 0x0d;
			szHeight[3] = 0x0a;
			szHeight[4] = '\0';
			dHeight = theApp.m_Serial.ReadData(szHeight,1,KEYENCE_IL);
			if(dHeight>0)
			{
				g_pFrm->m_CmdRun.ProductParam.ContactStandardValue = dHeight;
				str.Format("%0.2f",dHeight);
				SetDlgItemText(IDC_EDIT_DEBUG_HEIGHT_STANDARD_VALUE,str);
			}
		}
		MoveToContactSensorPos(0);
		nStep++;
	}
	else if(nStep==1)
	{
		MoveToContactSensorPos(2);
		nStep++;
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_DEBUG_HEIGHT_NEXT)->EnableWindow(FALSE);
	}
}

void CDlgDebugHeight::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	Exit();
	OnOK();
}

void CDlgDebugHeight::MoveToContactSensorPos(int nIndex)
{
	if(theApp.m_Mv400.IsInitOK())
	{
		if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_STOP)
		{
			if(!g_pFrm->m_CmdRun.MoveToZSafety())
			{
				return;
			}
			Sleep(100);
			if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[0]-(nIndex-1)*g_pFrm->m_CmdRun.ProductParam.NeedleGap,
				g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[1],true))
			{
				AfxMessageBox("X轴或Y轴运动失败！");
				return;
			}
			if(!g_pFrm->m_CmdRun.SynchronizeMoveZ((g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]-10)<70?(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]-10):70,true))
			{
				AfxMessageBox("Z轴运动失败！");
				return;
			}
			//theApp.m_Mv400.Move(K_AXIS_Z,g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2],g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel,g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc,TRUE);
			if(!g_pFrm->m_CmdRun.SynchronizeMoveZ(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2],g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel,g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc,true))
			{
				AfxMessageBox("Z轴运动失败！");
				return;
			}
			if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
			{
				AfxMessageBox("设定速度失败！");
				return;
			}
		}
	}
}

void CDlgDebugHeight::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	Exit();
	OnCancel();
}

void CDlgDebugHeight::OnNMCustomdrawSliderDebugHeightSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_DEBUG_HEIGHT_SPEED))->GetPos();
	CColorButton::m_iSpeedNo = n;
	CString strTemp;
	strTemp.Format("X:%.4f,Y:%.4f,Z:%.4f", theApp.m_tSysParam.tAxis[0].vel[0] * n/2000.0,
		theApp.m_tSysParam.tAxis[1].vel[0] * n/2000.0,theApp.m_tSysParam.tAxis[2].vel[0] * n/2000.0);
	SetDlgItemText(IDC_EDIT_DEBUG_HEIGHT_SPEED, strTemp);
}

void CDlgDebugHeight::SavePos()
{
	g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[0] = theApp.m_Mv400.GetPos(K_AXIS_X);
	g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[1] = theApp.m_Mv400.GetPos(K_AXIS_Y);
	g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2] = theApp.m_Mv400.GetPos(K_AXIS_ZA);
	theApp.ProductParam(FALSE);
}

void CDlgDebugHeight::OnTimer(UINT_PTR nIDEvent)
{
	if(theApp.m_tSysParam.sensorType==ContactSensorHiPrecision)
	{
		CString str;
		double dHeight = 0.0;
		char szHeight[5];
		szHeight[0] = 0x4d;
		szHeight[1] = 0x30;
		szHeight[2] = 0x0d;
		szHeight[3] = 0x0a;
		szHeight[4] = '\0';
		dHeight = theApp.m_Serial.ReadData(szHeight,1,KEYENCE_IL);
		str.Format("%0.2f",dHeight);
		SetDlgItemText(IDC_EDIT_DEBUG_HEIGHT_CURRENT_VALUE,str);
	}
	else
	{
		if(!theApp.m_Mv400.GetInput(theApp.m_tSysParam.inHeight))
		{
			DeleteObject(m_hBitmap);
			m_hBitmap = ::LoadBitmap(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP_SENSOR_OFF));
			if(theApp.m_tSysParam.sensorType==ContactSensorClose)
			{
				GetDlgItem(IDC_BTN_DEBUG_HEIGHT_Z_POS)->EnableWindow(TRUE);
			}
			else if(theApp.m_tSysParam.sensorType==ContactSensorOpen)
			{
				theApp.m_Mv400.StopMove();
				GetDlgItem(IDC_BTN_DEBUG_HEIGHT_Z_POS)->EnableWindow(FALSE);
			}
		}
		else
		{
			DeleteObject(m_hBitmap);
			m_hBitmap = ::LoadBitmap(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP_SENSOR_ON));
			if(theApp.m_tSysParam.sensorType==ContactSensorClose)
			{
				theApp.m_Mv400.StopMove();
				GetDlgItem(IDC_BTN_DEBUG_HEIGHT_Z_POS)->EnableWindow(FALSE);
			}
			else if(theApp.m_tSysParam.sensorType==ContactSensorOpen)
			{
				GetDlgItem(IDC_BTN_DEBUG_HEIGHT_Z_POS)->EnableWindow(TRUE);
			}
		}
		((CButton*)GetDlgItem(IDC_BTN_DEBUG_HEIGHT_SWITCH))->SetBitmap(m_hBitmap);
	}
	CDialog::OnTimer(nIDEvent);
}

void CDlgDebugHeight::Exit()
{
	g_pFrm->m_CmdRun.MoveToZSafety();
	KillTimer(0);
	g_pFrm->bIsDebugHeightVisible = false;
}