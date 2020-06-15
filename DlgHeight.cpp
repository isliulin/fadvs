// DlgHeight.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgHeight.h"

// CDlgHeight dialog
IMPLEMENT_DYNAMIC(CDlgHeight, CDialog)

CDlgHeight::CDlgHeight(CWnd* pParent /*=NULL*/)
: CDialog(CDlgHeight::IDD, pParent)
{

}

CDlgHeight::~CDlgHeight()
{
}

void CDlgHeight::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgHeight, CDialog)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_HEIGHT_SPEED, &CDlgHeight::OnNMCustomdrawSliderHeightSpeed)
	ON_BN_CLICKED(IDC_BTN_HEIGHT_SAVE_POS, &CDlgHeight::OnBnClickedBtnHeightSavePos)
	ON_BN_CLICKED(IDC_BTN_HEIGHT_ZSAFE_POS, &CDlgHeight::OnBnClickedBtnHeightZsafePos)
	ON_BN_CLICKED(IDOK, &CDlgHeight::OnBnClickedOk)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_HEIGHT_MEASURE_TWO, &CDlgHeight::OnBnClickedBtnHeightMeasureTwo)
	ON_BN_CLICKED(IDC_BTN_HEIGHT_MEASURE_ONE, &CDlgHeight::OnBnClickedBtnHeightMeasureOne)
	ON_BN_CLICKED(IDC_BTN_HEIGHT_MEASURE_THREE, &CDlgHeight::OnBnClickedBtnHeightMeasureThree)
//	ON_BN_CLICKED(IDC_BTN_HEIGHT_CALIBRATION, &CDlgHeight::OnBnClickedBtnHeightCalibration)
	ON_BN_CLICKED(IDC_BTN_HEIGHT_CAMERA_POS, &CDlgHeight::OnBnClickedBtnHeightCameraPos)
	ON_BN_CLICKED(IDC_BTN_HEIGHT_POS, &CDlgHeight::OnBnClickedBtnHeightPos)
	ON_BN_CLICKED(IDCANCEL, &CDlgHeight::OnBnClickedCancel)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_HEIGHT_ADJUST_SPEED, &CDlgHeight::OnNMCustomdrawSliderHeightAdjustSpeed)
	ON_BN_CLICKED(IDC_BTN_HEIGHT_LASERSAVE, &CDlgHeight::OnBnClickedBtnHeightLasersave)
	ON_BN_CLICKED(IDC_BTN_LASERPOS_MODIFY, &CDlgHeight::OnBnClickedBtnLaserposModify)
	ON_BN_CLICKED(IDC_BTN_LASER_LABLE, &CDlgHeight::OnBnClickedBtnLaserLable)
	
END_MESSAGE_MAP()


// CDlgHeight message handlers

//手动操作时速度调节
void CDlgHeight::OnNMCustomdrawSliderHeightSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_HEIGHT_SPEED))->GetPos();
	CColorButton::m_iSpeedNo = n;
	CString strTemp;
	strTemp.Format("X:%.4f,Y:%.4f,ZA:%.4f,ZB:%.4f,ZC:%.4f", theApp.m_tSysParam.tAxis[0].vel[0] * n/2000.0,
		theApp.m_tSysParam.tAxis[1].vel[0] * n/2000.0,theApp.m_tSysParam.tAxis[2].vel[0] * n/2000.0,
		theApp.m_tSysParam.tAxis[3].vel[0] * n/2000.0,theApp.m_tSysParam.tAxis[4].vel[0] * n/2000.0);
	SetDlgItemText(IDC_EDIT_HEIGHT_SPEED, strTemp);
	*pResult = 0;
}
//记录阀体A位于接触感应器上方时XYZa,Zb,Zc坐标数据 
void CDlgHeight::OnBnClickedBtnHeightSavePos()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能保存当前位置！");
		return;
	}
	if(AfxMessageBox("确定保存当前位置?",MB_YESNO)!=IDYES)
	{
		return;
	}
	for(int i=0;i<5;i++)  
	{
		g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[i]=theApp.m_Mv400.GetPos(i+1);
	}
	CTimeUtil::Delay(100);
	if(theApp.m_tSysParam.sensorType==ContactSensorHiPrecision)//current:open or close mode;
	{
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
		}
		else
		{
			AfxMessageBox("接触开关读取失败！");
		}
		CTimeUtil::Delay(100);
	}
	g_pFrm->m_CmdRun.MoveToZSafety();
	theApp.ProductParam(FALSE);
	theApp.SysParam(FALSE);
	UpdateUI();
}

void CDlgHeight::OnBnClickedBtnHeightZsafePos()
{
	// TODO: Add your control notification handler code here
	if(theApp.m_Mv400.IsInitOK())
	{
		if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_STOP)
		{
			g_pFrm->m_CmdRun.MoveToZSafety();
		}
	}
}

void CDlgHeight::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_HEIGHT_X_POS,str);
	g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[0] = atof(str);
	GetDlgItemText(IDC_EDIT_HEIGHT_Y_POS,str);
	g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[1] = atof(str);
	GetDlgItemText(IDC_EDIT_HEIGHT_ZA_POS,str);
	g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2] = atof(str);
	GetDlgItemText(IDC_EDIT_HEIGHT_ZB_POS,str);
	g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[3] = atof(str);
	GetDlgItemText(IDC_EDIT_HEIGHT_ZC_POS,str);
	g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[4] = atof(str);
	GetDlgItemText(IDC_EDIT_HEIGHT_VEL,str);
	g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel = atof(str);
	GetDlgItemText(IDC_EDIT_HEIGHT_ACC,str);
	g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc = atof(str);
	//ModifyParam();
	theApp.ProductParam(FALSE);
	Close();
	OnOK();
}

BOOL CDlgHeight::OnInitDialog()
{
	g_pFrm->bIsHeightDlgVisible = true;
	m_bMotion = true;
	for(int i=0;i<10;i++)
	{
		m_colorButton[i].m_iIndex = i;
		m_colorButton[i].SubclassDlgItem(IDC_BTN_HEIGHT_X_NEG+i,this);
		m_colorButton[i].SetBitmapEx(IDB_BITMAP_MOVE);
		m_colorButton[i].m_pMv = &theApp.m_Mv400;
	}
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_HEIGHT_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_HEIGHT_SPEED))->SetPos(1);
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_HEIGHT_SPEED))->GetPos();
	CColorButton::m_iSpeedNo = n;

	//测高步骤提示框
	GetDlgItem(IDC_EDIT_HEIGHT_PROMPT)->ShowWindow(SW_HIDE);
	//保存激光测高基准数据框
	GetDlgItem(IDC_BTN_HEIGHT_LASERSAVE)->ShowWindow(SW_HIDE);	
	UpdateUI();
	SetTimer(0,200,NULL);
	//AdjustNeedleGap();
	//取消间距调整，自动回到原点位置；
	if(theApp.m_Mv400.IsInitOK())
	{
		MOTION_STATUS status;
		double dPosA,dPosC,dPosB,dPosD;
		dPosB=g_pFrm->m_CmdRun.ProductParam.offset2y;
		dPosD=g_pFrm->m_CmdRun.ProductParam.offset3y;
		dPosA=g_pFrm->m_CmdRun.ProductParam.offset2x;
		dPosC=g_pFrm->m_CmdRun.ProductParam.offset3x;

		if(!g_pFrm->m_CmdRun.MoveToZSafety())
		{
			AfxMessageBox("Z轴回安全位失败！");
			return TRUE;
		}

		//motor move to the lable postion,keep the needle gap of lable;
		g_pFrm->m_CmdRun.m_pMv->Move(K_AXIS_A,dPosA,true,true);
		g_pFrm->m_CmdRun.m_pMv->Move(K_AXIS_C,dPosC,true,true);
		g_pFrm->m_CmdRun.m_pMv->Move(K_AXIS_B,dPosB,true,true);
		g_pFrm->m_CmdRun.m_pMv->Move(K_AXIS_D,dPosD,true,true);
	}
	//修改激光测高位数据
	GetDlgItem(IDC_BTN_LASERPOS_MODIFY)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	return TRUE;
}

void CDlgHeight::UpdateUI()
{
	CString str;
	//load contact sensor position of valve A;
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[0]);
	SetDlgItemText(IDC_EDIT_HEIGHT_X_POS,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[1]);
	SetDlgItemText(IDC_EDIT_HEIGHT_Y_POS,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]);
	SetDlgItemText(IDC_EDIT_HEIGHT_ZA_POS,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[3]);
	SetDlgItemText(IDC_EDIT_HEIGHT_ZB_POS,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[4]);
	SetDlgItemText(IDC_EDIT_HEIGHT_ZC_POS,str);
	
	//the measure action move parameter;
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel);
	SetDlgItemText(IDC_EDIT_HEIGHT_VEL,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc);
	SetDlgItemText(IDC_EDIT_HEIGHT_ACC,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.NeedleGap); //针头间距 产品参数界面决定
	SetDlgItemText(IDC_EDIT_HEIGHT_PITCH,str);

	//激光测高感应器位于接触传感器上方时X,Y,ZA坐标；
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.HeightSensorPos[0]);
	SetDlgItemText(IDC_EDIT_HEIGHT_CAMERA_X_POS,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.HeightSensorPos[1]);
	SetDlgItemText(IDC_EDIT_HEIGHT_CAMERA_Y_POS,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.HeightSensorPos[2]);
	SetDlgItemText(IDC_EDIT_HEIGHT_CAMERA_ZA_POS,str);


	if(theApp.m_tSysParam.sensorType==ContactSensorHiPrecision)
	{
		str.Format("%0.2f",g_pFrm->m_CmdRun.ProductParam.ContactStandardValue);
		SetDlgItemText(IDC_EDIT_HEIGHT_STANDARD_VALUE,str);
	}
	str.Format("%0.2f",g_pFrm->m_CmdRun.ProductParam.HeightStandardValue);
	SetDlgItemText(IDC_EDIT_HEIGHT_HEIGHT_STANDARD_VALUE,str);
}

void CDlgHeight::OnTimer(UINT_PTR nIDEvent)
{
	CString str;
	double dReadValue = 0.0;
	g_pFrm->m_CmdRun.ReadHeightSensorValue(&dReadValue);//读取激光数值
	str.Format("%0.3f",dReadValue);
	SetDlgItemText(IDC_EDIT_HEIGHT_HEIGHT_CURRENT_VALUE,str);
	//open or close mode bitmap fresh;
	if(theApp.m_tSysParam.sensorType==ContactSensorHiPrecision)//接触传感器类型1
	{
		g_pFrm->m_CmdRun.ReadContactSensorValue(&dReadValue);
		str.Format("%0.2f",dReadValue);
		SetDlgItemText(IDC_EDIT_HEIGHT_CURRENT_VALUE,str);
	}
	else //接触传感器类型2
	{
		if(!theApp.m_Mv400.GetInput(theApp.m_tSysParam.inHeight))
		{
			m_bMotion = true;
			DeleteObject(m_hBitmap);
			m_hBitmap = ::LoadBitmap(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP_SENSOR_OFF));
			if(theApp.m_tSysParam.sensorType==ContactSensorClose)
			{
				GetDlgItem(IDC_BTN_HEIGHT_ZA_POS)->EnableWindow(TRUE);
				GetDlgItem(IDC_BTN_HEIGHT_ZB_POS)->EnableWindow(TRUE);
				GetDlgItem(IDC_BTN_HEIGHT_ZC_POS)->EnableWindow(TRUE);
			}
			else if(theApp.m_tSysParam.sensorType==ContactSensorOpen)
			{
				if(theApp.m_Mv400.IsMove(K_AXIS_ZA))
				{
					theApp.m_Mv400.Stop(K_AXIS_ZA);
				}
				else if(theApp.m_Mv400.IsMove(K_AXIS_ZB))
				{
					theApp.m_Mv400.Stop(K_AXIS_ZB);
				}
				else if(theApp.m_Mv400.IsMove(K_AXIS_ZC))
				{
					theApp.m_Mv400.Stop(K_AXIS_ZC);
				}
				GetDlgItem(IDC_BTN_HEIGHT_ZA_POS)->EnableWindow(FALSE);
				GetDlgItem(IDC_BTN_HEIGHT_ZB_POS)->EnableWindow(FALSE);
				GetDlgItem(IDC_BTN_HEIGHT_ZC_POS)->EnableWindow(FALSE);//19-10-22
			}
		}
		else
		{
			//if(m_bMotion)
			//{
			//	theApp.m_Mv400.Stop(K_AXIS_Z);
			//}
			//m_bMotion = false;
			DeleteObject(m_hBitmap);
			m_hBitmap = ::LoadBitmap(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP_SENSOR_ON));
			if(theApp.m_tSysParam.sensorType==ContactSensorClose)
			{
				if(theApp.m_Mv400.IsMove(K_AXIS_ZA))
				{
					theApp.m_Mv400.Stop(K_AXIS_ZA);
				}
				else if(theApp.m_Mv400.IsMove(K_AXIS_ZB))
				{
					theApp.m_Mv400.Stop(K_AXIS_ZB);
				}
				else if(theApp.m_Mv400.IsMove(K_AXIS_ZC))
				{
					theApp.m_Mv400.Stop(K_AXIS_ZC);
				}
				//GetDlgItem(IDC_BTN_HEIGHT_ZA_POS)->EnableWindow(FALSE);
				//GetDlgItem(IDC_BTN_HEIGHT_ZB_POS)->EnableWindow(FALSE);
				//GetDlgItem(IDC_BTN_HEIGHT_ZC_POS)->EnableWindow(FALSE);//19-10-22
			}
			else if(theApp.m_tSysParam.sensorType==ContactSensorOpen)
			{
				GetDlgItem(IDC_BTN_HEIGHT_ZA_POS)->EnableWindow(TRUE);
				GetDlgItem(IDC_BTN_HEIGHT_ZB_POS)->EnableWindow(TRUE);
				GetDlgItem(IDC_BTN_HEIGHT_ZC_POS)->EnableWindow(TRUE);
			}
		}
		((CButton*)GetDlgItem(IDC_BTN_HEIGHT_SWITCH))->SetBitmap(m_hBitmap);//Load the different bitmap;
	}
	RefreshPosition();
	CDialog::OnTimer(nIDEvent);
}

void CDlgHeight::Close()
{
	if(theApp.m_Mv400.IsInitOK())
	{
		if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_STOP)
		{
			g_pFrm->m_CmdRun.MoveToZSafety();
		}
	}
	KillTimer(0);
	DeleteObject(m_hBitmap);
	g_pFrm->bIsHeightDlgVisible = false;
}

//C阀测高 
void CDlgHeight::OnBnClickedBtnHeightMeasureThree()
{
	if(theApp.m_Mv400.IsInitOK())
	{
		if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_STOP)
		{
			//g_pFrm->m_CmdRun.MoveToZSafety();
			//if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[0]-g_pFrm->m_CmdRun.ProductParam.NeedleGap,g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[1],TRUE))
			//{
			//	AfxMessageBox("X轴和Y轴电机运动失败！");
			//	return;
			//}
			//if(!g_pFrm->m_CmdRun.SynchronizeMoveZ((g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]-10)<70?(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]-10):70,true))
			//{
			//	AfxMessageBox("Z轴运动失败！");
			//	return;
			//}
			//theApp.m_Mv400.Move(K_AXIS_ZA,g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2],g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel,g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc,TRUE);
			//if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
			//{
			//	AfxMessageBox("设定速度失败！");
			//	return;
			//}
			BOOL bMeasureSucceed = g_pFrm->m_CmdRun.CheckHeightEx(K_AXIS_ZC);
			UpdateUI();
			if(bMeasureSucceed)
			{
				AfxMessageBox("测高成功！");
			}
			else
			{
				AfxMessageBox("测高失败！");
			}
		}
	}
}

//B阀测高
void CDlgHeight::OnBnClickedBtnHeightMeasureTwo()
{
	if(theApp.m_Mv400.IsInitOK())
	{
		if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_STOP)
		{
			if(theApp.m_tSysParam.sensorType==ContactSensorHiPrecision)
			{
				g_pFrm->m_CmdRun.MoveToZSafety();
				if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[0],g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[1],TRUE))
				{
					AfxMessageBox("X轴和Y轴电机运动失败！");
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
			else
			{
				BOOL bMeasureSucceed = g_pFrm->m_CmdRun.CheckHeightEx(K_AXIS_ZB);
				UpdateUI();
				if(bMeasureSucceed)
				{
					AfxMessageBox("测高成功！");
				}
				else
				{
					AfxMessageBox("测高失败！");
				}
			}
		}
	}
}

//A阀测高
void CDlgHeight::OnBnClickedBtnHeightMeasureOne()
{
	if(theApp.m_Mv400.IsInitOK())
	{
		if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_STOP)
		{
			//g_pFrm->m_CmdRun.MoveToZSafety();
			//if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[0]+g_pFrm->m_CmdRun.ProductParam.NeedleGap,g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[1],TRUE))
			//{
			//	AfxMessageBox("X轴和Y轴电机运动失败！");
			//	return;
			//}
			//if(!g_pFrm->m_CmdRun.SynchronizeMoveZ((g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]-10)<70?(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]-10):70,true))
			//{
			//	AfxMessageBox("Z轴运动失败！");
			//	return;
			//}
			////theApp.m_Mv400.Move(K_AXIS_Z,g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2],g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel,g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc,TRUE);
			//if(!g_pFrm->m_CmdRun.SynchronizeMoveZ(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2],g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel,g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc,true))
			//{
			//	AfxMessageBox("Z轴运动失败！");
			//	return;
			//}
			//if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
			//{
			//	AfxMessageBox("设定速度失败！");
			//	return;
			//}
			BOOL bMeasureSucceed = g_pFrm->m_CmdRun.CheckHeightEx(K_AXIS_ZA);
			UpdateUI();
			if(bMeasureSucceed)
			{
				AfxMessageBox("测高成功！");
			}
			else
			{
				AfxMessageBox("测高失败！");
			}
		}
	}
}

//void CDlgHeight::OnBnClickedBtnHeightCalibration()
//{
//	// TODO: Add your control notification handler code here
//	//if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
//	//{
//	//	AfxMessageBox("运行状态不能校正针头");
//	//	return;
//	//}
//	//if(g_pFrm->m_dlgHeightCalib.DoModal()==IDOK)
//	//{
//	//	theApp.ProductParam(FALSE);
//	//	UpdateUI();
//	//}
//}


//激光测高位高度读取
//Move to laser test position;
//激光头到接触感应器上方设定位置
void CDlgHeight::OnBnClickedBtnHeightCameraPos()
{
	// TODO: Add your control notification handler code here
	KillTimer(0);
	if(theApp.m_Mv400.IsInitOK())
	{
		if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_STOP)
		{
			if(!g_pFrm->m_CmdRun.MoveToZSafety())
			{
				return;
			}
			if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].x,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].y,true))
			{
				return;
			}
			if (!g_pFrm->m_CmdRun.SynchronizeMoveZ(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].za,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].zb,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].zc,false,true))
			{
				return;
			}
			GetDlgItem(IDC_EDIT_HEIGHT_PROMPT)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BTN_HEIGHT_LASERSAVE)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_EDIT_HEIGHT_PROMPT)->SetWindowText("请将ZA缓慢下移，直到针头离接触传感器端面3mm以内!");
			//if(!g_pFrm->m_CmdRun.MoveZ(K_AXIS_ZA,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].za,true))
			//{
			//	return;
			//}
		}
	}

	//读取激光感应器数据5次后更新到界面；
	CString str;
	double dReadValue = 0.0;
	for (int i=0; i<5; i++)
	{
		g_pFrm->m_CmdRun.ReadHeightSensorValue(&dReadValue);
		str.Format("%0.3f",dReadValue);
		SetDlgItemText(IDC_EDIT_HEIGHT_HEIGHT_CURRENT_VALUE,str);
		CFunction::DoEvents();
	}

	SetTimer(0, 200, NULL);
}

void CDlgHeight::ModifyParam()
{
}

//刷新9个轴的实时位置
void CDlgHeight::RefreshPosition()
{
	CString str;

	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_X));
	SetDlgItemText(IDC_EDIT_HEIGHT_CURRENT_X,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_Y));
	SetDlgItemText(IDC_EDIT_HEIGHT_CURRENT_Y,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_ZA));
	SetDlgItemText(IDC_EDIT_HEIGHT_CURRENT_ZA,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_ZB));
	SetDlgItemText(IDC_EDIT_HEIGHT_CURRENT_ZB,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_ZC));
	SetDlgItemText(IDC_EDIT_HEIGHT_CURRENT_ZC,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_A));
	SetDlgItemText(IDC_EDIT_HEIGHT_CURRENT_A,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_B));
	SetDlgItemText(IDC_EDIT_HEIGHT_CURRENT_B,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_C));
	SetDlgItemText(IDC_EDIT_HEIGHT_CURRENT_C,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_D));
	SetDlgItemText(IDC_EDIT_HEIGHT_CURRENT_D,str);
}
//A阀到记录的测高位置；
void CDlgHeight::OnBnClickedBtnHeightPos()
{
	// TODO: Add your control notification handler code here
	if(theApp.m_Mv400.IsInitOK())
	{
		if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_STOP)
		{
			if(!g_pFrm->m_CmdRun.MoveToZSafety())
			{
				return;
			}
			if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[0],
				g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[1],true))
			{
				return;
			}
			if(AfxMessageBox("Z轴移动到接触位！",MB_YESNO)==IDYES)
			{
				//theApp.m_Mv400.Move(K_AXIS_Z,g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2],
				//	g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel,g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc,TRUE);
				if(!g_pFrm->m_CmdRun.SynchronizeMoveZ(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2],g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel,g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc,true))
				{
					AfxMessageBox("Z轴运动失败！");
					return;
				}
			}
			if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
			{
				AfxMessageBox("设定速度失败！");
				return;
			}
		}
	}
}

void CDlgHeight::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	Close();
	OnCancel();
}


void CDlgHeight::OnNMCustomdrawSliderHeightAdjustSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_HEIGHT_ADJUST_SPEED))->GetPos();
	CJogButton::m_dJogSpdRatio = n;
	CString strTemp;
	strTemp.Format("A:%.4f,B:%.4f,C:%.4f,D:%.4f", theApp.m_tSysParam.tAxis[K_AXIS_A-1].vel[0]*n/2000.0,theApp.m_tSysParam.tAxis[K_AXIS_B-1].vel[0] * n/2000.0,
		theApp.m_tSysParam.tAxis[K_AXIS_C-1].vel[0] * n/2000.0,theApp.m_tSysParam.tAxis[K_AXIS_D-1].vel[0] * n/2000.0);
	SetDlgItemText(IDC_EDIT_HEIGHT_ADJUST_SPEED, strTemp);
	*pResult = 0;
}

void CDlgHeight::AdjustNeedleGap()
{
	if(theApp.m_Mv400.IsInitOK())
	{
		if(g_pFrm->m_CmdRun.MoveToZSafety())
		{
			if(!g_pFrm->m_CmdRun.AdjustNeedle())
				AfxMessageBox("调整电机移动失败！");
		}
		else
		{
			AfxMessageBox("Z到安全位失败，针头间距调整启动终止！");

		}
	}
}
//保存激光测量数据 以及此时Za的坐标；
void CDlgHeight::OnBnClickedBtnHeightLasersave()
{
	// TODO: Add your control notification handler code here
	if(AfxMessageBox("确认保存当前激光位置？",MB_YESNO)==IDYES)
	{
		double dHeightValue;
		//Save the position of read data;
		g_pFrm->m_CmdRun.ProductParam.dNeedleLaserHeight=theApp.m_Mv400.GetPos(K_AXIS_ZA);
		if(g_pFrm->m_CmdRun.ReadHeightSensorValue(&dHeightValue))  //读取激光传感器在接触传感器上方时数值；
		{
			g_pFrm->m_CmdRun.ProductParam.HeightStandardValue = dHeightValue;

		}
		theApp.ProductParam(FALSE);
		UpdateUI();//if standvalue update;this also;
	}

}

//针头大小以及扭曲不同，微调激光测量位置数据
void CDlgHeight::OnBnClickedBtnLaserposModify()
{
	// TODO: Add your control notification handler code here
	if(AfxMessageBox("确认保存当前激光测高位置？",MB_YESNO)==IDYES)
	{
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].x=theApp.m_Mv400.GetPos(K_AXIS_X);
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].y=theApp.m_Mv400.GetPos(K_AXIS_Y);
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].za=theApp.m_Mv400.GetPos(K_AXIS_ZA);
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].zb=theApp.m_Mv400.GetPos(K_AXIS_ZB);
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].zc=theApp.m_Mv400.GetPos(K_AXIS_ZC);
		theApp.ProductParam(FALSE);
	}
}

//标定 相机//接触传感器
void CDlgHeight::OnBnClickedBtnLaserLable()
{
	CDlgCameraLaser dlg;
	dlg.DoModal();
}

