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

//�ֶ�����ʱ�ٶȵ���
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
//��¼����Aλ�ڽӴ���Ӧ���Ϸ�ʱXYZa,Zb,Zc�������� 
void CDlgHeight::OnBnClickedBtnHeightSavePos()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("�˶�״̬���ܱ��浱ǰλ�ã�");
		return;
	}
	if(AfxMessageBox("ȷ�����浱ǰλ��?",MB_YESNO)!=IDYES)
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
			AfxMessageBox("�Ӵ����ض�ȡʧ�ܣ�");
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

	//��߲�����ʾ��
	GetDlgItem(IDC_EDIT_HEIGHT_PROMPT)->ShowWindow(SW_HIDE);
	//���漤���߻�׼���ݿ�
	GetDlgItem(IDC_BTN_HEIGHT_LASERSAVE)->ShowWindow(SW_HIDE);	
	UpdateUI();
	SetTimer(0,200,NULL);
	//AdjustNeedleGap();
	//ȡ�����������Զ��ص�ԭ��λ�ã�
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
			AfxMessageBox("Z��ذ�ȫλʧ�ܣ�");
			return TRUE;
		}

		//motor move to the lable postion,keep the needle gap of lable;
		g_pFrm->m_CmdRun.m_pMv->Move(K_AXIS_A,dPosA,true,true);
		g_pFrm->m_CmdRun.m_pMv->Move(K_AXIS_C,dPosC,true,true);
		g_pFrm->m_CmdRun.m_pMv->Move(K_AXIS_B,dPosB,true,true);
		g_pFrm->m_CmdRun.m_pMv->Move(K_AXIS_D,dPosD,true,true);
	}
	//�޸ļ�����λ����
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
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.NeedleGap); //��ͷ��� ��Ʒ�����������
	SetDlgItemText(IDC_EDIT_HEIGHT_PITCH,str);

	//�����߸�Ӧ��λ�ڽӴ��������Ϸ�ʱX,Y,ZA���ꣻ
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
	g_pFrm->m_CmdRun.ReadHeightSensorValue(&dReadValue);//��ȡ������ֵ
	str.Format("%0.3f",dReadValue);
	SetDlgItemText(IDC_EDIT_HEIGHT_HEIGHT_CURRENT_VALUE,str);
	//open or close mode bitmap fresh;
	if(theApp.m_tSysParam.sensorType==ContactSensorHiPrecision)//�Ӵ�����������1
	{
		g_pFrm->m_CmdRun.ReadContactSensorValue(&dReadValue);
		str.Format("%0.2f",dReadValue);
		SetDlgItemText(IDC_EDIT_HEIGHT_CURRENT_VALUE,str);
	}
	else //�Ӵ�����������2
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

//C����� 
void CDlgHeight::OnBnClickedBtnHeightMeasureThree()
{
	if(theApp.m_Mv400.IsInitOK())
	{
		if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_STOP)
		{
			//g_pFrm->m_CmdRun.MoveToZSafety();
			//if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[0]-g_pFrm->m_CmdRun.ProductParam.NeedleGap,g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[1],TRUE))
			//{
			//	AfxMessageBox("X���Y�����˶�ʧ�ܣ�");
			//	return;
			//}
			//if(!g_pFrm->m_CmdRun.SynchronizeMoveZ((g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]-10)<70?(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]-10):70,true))
			//{
			//	AfxMessageBox("Z���˶�ʧ�ܣ�");
			//	return;
			//}
			//theApp.m_Mv400.Move(K_AXIS_ZA,g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2],g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel,g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc,TRUE);
			//if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
			//{
			//	AfxMessageBox("�趨�ٶ�ʧ�ܣ�");
			//	return;
			//}
			BOOL bMeasureSucceed = g_pFrm->m_CmdRun.CheckHeightEx(K_AXIS_ZC);
			UpdateUI();
			if(bMeasureSucceed)
			{
				AfxMessageBox("��߳ɹ���");
			}
			else
			{
				AfxMessageBox("���ʧ�ܣ�");
			}
		}
	}
}

//B�����
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
					AfxMessageBox("X���Y�����˶�ʧ�ܣ�");
					return;
				}
				if(!g_pFrm->m_CmdRun.SynchronizeMoveZ((g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]-10)<70?(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]-10):70,true))
				{
					AfxMessageBox("Z���˶�ʧ�ܣ�");
					return;
				}
				//theApp.m_Mv400.Move(K_AXIS_Z,g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2],g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel,g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc,TRUE);
				if(!g_pFrm->m_CmdRun.SynchronizeMoveZ(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2],g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel,g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc,true))
				{
					AfxMessageBox("Z���˶�ʧ�ܣ�");
					return;
				}
				if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
				{
					AfxMessageBox("�趨�ٶ�ʧ�ܣ�");
					return;
				}
			}
			else
			{
				BOOL bMeasureSucceed = g_pFrm->m_CmdRun.CheckHeightEx(K_AXIS_ZB);
				UpdateUI();
				if(bMeasureSucceed)
				{
					AfxMessageBox("��߳ɹ���");
				}
				else
				{
					AfxMessageBox("���ʧ�ܣ�");
				}
			}
		}
	}
}

//A�����
void CDlgHeight::OnBnClickedBtnHeightMeasureOne()
{
	if(theApp.m_Mv400.IsInitOK())
	{
		if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_STOP)
		{
			//g_pFrm->m_CmdRun.MoveToZSafety();
			//if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[0]+g_pFrm->m_CmdRun.ProductParam.NeedleGap,g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[1],TRUE))
			//{
			//	AfxMessageBox("X���Y�����˶�ʧ�ܣ�");
			//	return;
			//}
			//if(!g_pFrm->m_CmdRun.SynchronizeMoveZ((g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]-10)<70?(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]-10):70,true))
			//{
			//	AfxMessageBox("Z���˶�ʧ�ܣ�");
			//	return;
			//}
			////theApp.m_Mv400.Move(K_AXIS_Z,g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2],g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel,g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc,TRUE);
			//if(!g_pFrm->m_CmdRun.SynchronizeMoveZ(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2],g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel,g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc,true))
			//{
			//	AfxMessageBox("Z���˶�ʧ�ܣ�");
			//	return;
			//}
			//if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
			//{
			//	AfxMessageBox("�趨�ٶ�ʧ�ܣ�");
			//	return;
			//}
			BOOL bMeasureSucceed = g_pFrm->m_CmdRun.CheckHeightEx(K_AXIS_ZA);
			UpdateUI();
			if(bMeasureSucceed)
			{
				AfxMessageBox("��߳ɹ���");
			}
			else
			{
				AfxMessageBox("���ʧ�ܣ�");
			}
		}
	}
}

//void CDlgHeight::OnBnClickedBtnHeightCalibration()
//{
//	// TODO: Add your control notification handler code here
//	//if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
//	//{
//	//	AfxMessageBox("����״̬����У����ͷ");
//	//	return;
//	//}
//	//if(g_pFrm->m_dlgHeightCalib.DoModal()==IDOK)
//	//{
//	//	theApp.ProductParam(FALSE);
//	//	UpdateUI();
//	//}
//}


//������λ�߶ȶ�ȡ
//Move to laser test position;
//����ͷ���Ӵ���Ӧ���Ϸ��趨λ��
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
			GetDlgItem(IDC_EDIT_HEIGHT_PROMPT)->SetWindowText("�뽫ZA�������ƣ�ֱ����ͷ��Ӵ�����������3mm����!");
			//if(!g_pFrm->m_CmdRun.MoveZ(K_AXIS_ZA,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].za,true))
			//{
			//	return;
			//}
		}
	}

	//��ȡ�����Ӧ������5�κ���µ����棻
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

//ˢ��9�����ʵʱλ��
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
//A������¼�Ĳ��λ�ã�
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
			if(AfxMessageBox("Z���ƶ����Ӵ�λ��",MB_YESNO)==IDYES)
			{
				//theApp.m_Mv400.Move(K_AXIS_Z,g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2],
				//	g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel,g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc,TRUE);
				if(!g_pFrm->m_CmdRun.SynchronizeMoveZ(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2],g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel,g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc,true))
				{
					AfxMessageBox("Z���˶�ʧ�ܣ�");
					return;
				}
			}
			if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
			{
				AfxMessageBox("�趨�ٶ�ʧ�ܣ�");
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
				AfxMessageBox("��������ƶ�ʧ�ܣ�");
		}
		else
		{
			AfxMessageBox("Z����ȫλʧ�ܣ���ͷ������������ֹ��");

		}
	}
}
//���漤��������� �Լ���ʱZa�����ꣻ
void CDlgHeight::OnBnClickedBtnHeightLasersave()
{
	// TODO: Add your control notification handler code here
	if(AfxMessageBox("ȷ�ϱ��浱ǰ����λ�ã�",MB_YESNO)==IDYES)
	{
		double dHeightValue;
		//Save the position of read data;
		g_pFrm->m_CmdRun.ProductParam.dNeedleLaserHeight=theApp.m_Mv400.GetPos(K_AXIS_ZA);
		if(g_pFrm->m_CmdRun.ReadHeightSensorValue(&dHeightValue))  //��ȡ���⴫�����ڽӴ��������Ϸ�ʱ��ֵ��
		{
			g_pFrm->m_CmdRun.ProductParam.HeightStandardValue = dHeightValue;

		}
		theApp.ProductParam(FALSE);
		UpdateUI();//if standvalue update;this also;
	}

}

//��ͷ��С�Լ�Ť����ͬ��΢���������λ������
void CDlgHeight::OnBnClickedBtnLaserposModify()
{
	// TODO: Add your control notification handler code here
	if(AfxMessageBox("ȷ�ϱ��浱ǰ������λ�ã�",MB_YESNO)==IDYES)
	{
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].x=theApp.m_Mv400.GetPos(K_AXIS_X);
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].y=theApp.m_Mv400.GetPos(K_AXIS_Y);
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].za=theApp.m_Mv400.GetPos(K_AXIS_ZA);
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].zb=theApp.m_Mv400.GetPos(K_AXIS_ZB);
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].zc=theApp.m_Mv400.GetPos(K_AXIS_ZC);
		theApp.ProductParam(FALSE);
	}
}

//�궨 ���//�Ӵ�������
void CDlgHeight::OnBnClickedBtnLaserLable()
{
	CDlgCameraLaser dlg;
	dlg.DoModal();
}

