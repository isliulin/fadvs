// DlgConfigure.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgConfigure.h"


// CDlgConfigure dialog

IMPLEMENT_DYNAMIC(CDlgConfigure, CDialog)

CDlgConfigure::CDlgConfigure(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgConfigure::IDD, pParent)
{

}

CDlgConfigure::~CDlgConfigure()
{

}

void CDlgConfigure::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_COMBO_CONFIGURE_HEIGHT_SENSOR,m_cbHeightSel);
	DDX_Control(pDX,IDC_COMBO_CONFIGURE_VALVE_NUM,m_cbValveNum);
	DDX_Control(pDX,IDC_COMBO_CONFIGURE_PLC_TYPE,m_cbPlcSel);
	DDX_Control(pDX,IDC_COMBO_CONFIGURE_VALVE_TYPE,m_cbValveSel);
	DDX_Control(pDX,IDC_COMBO_CONFIGURE_HEIGHT_SENSOR_TYPE,m_cbHeightSensor);
	DDX_Control(pDX,IDC_COMBO_CONFIGURE_DETECT_CAMERA_TYPE,m_cbDetectCamera);
	DDX_Control(pDX, IDC_SPIN_MARKCAM, m_spinMarkCam);
	DDX_Control(pDX, IDC_SPIN_NEEDLECAM, m_spinNeedleCam);
}


BEGIN_MESSAGE_MAP(CDlgConfigure, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_CONFIGURE_HEIGHT_SENSOR, &CDlgConfigure::OnCbnSelchangeComboConfigureHeightSensor)
	ON_BN_CLICKED(IDOK, &CDlgConfigure::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_CONFIGURE_VALVE_NUM, &CDlgConfigure::OnCbnSelchangeComboConfigureValveNum)
	ON_CBN_SELCHANGE(IDC_COMBO_CONFIGURE_PLC_TYPE, &CDlgConfigure::OnCbnSelchangeComboConfigurePlcType)
	ON_CBN_SELCHANGE(IDC_COMBO_CONFIGURE_VALVE_TYPE, &CDlgConfigure::OnCbnSelchangeComboConfigureValveType)
	ON_CBN_SELCHANGE(IDC_COMBO_CONFIGURE_HEIGHT_SENSOR_TYPE, &CDlgConfigure::OnCbnSelchangeComboConfigureHeightSensorType)
	ON_CBN_SELCHANGE(IDC_COMBO_CONFIGURE_DETECT_CAMERA_TYPE, &CDlgConfigure::OnCbnSelchangeComboConfigureDetectCameraType)
END_MESSAGE_MAP()


// CDlgConfigure message handlers

BOOL CDlgConfigure::OnInitDialog()
{
	CDialog::OnInitDialog();
	*m_bSaveConfigure = false;
	m_cbHeightSel.AddString("常闭传感器");
	m_cbHeightSel.AddString("常开传感器");
	m_cbHeightSel.AddString("高精度传感器");
	m_cbHeightSel.SetCurSel((int)theApp.m_tSysParam.sensorType);
	m_nHeightIndex = m_cbHeightSel.GetCurSel();

	m_cbValveNum.AddString("0阀");
	m_cbValveNum.AddString("1阀");
	m_cbValveNum.AddString("2阀");
	m_cbValveNum.AddString("3阀");
	m_cbValveNum.SetCurSel((int)theApp.m_tSysParam.valveNum);
	m_nValveNum = m_cbValveNum.GetCurSel();

	m_cbPlcSel.AddString("Keyence");
	m_cbPlcSel.AddString("Panasonic");
	m_cbPlcSel.SetCurSel((int)theApp.m_tSysParam.PlcType);
	m_nPlcSel = m_cbPlcSel.GetCurSel();

	m_cbValveSel.AddString("M9");
	m_cbValveSel.AddString("L9");
	m_cbValveSel.AddString("H9");
	m_cbValveSel.SetCurSel((int)theApp.m_tSysParam.ValveType);
	m_nValveSel = m_cbValveSel.GetCurSel();

	m_cbHeightSensor.AddString("MLD高度传感器");
	m_cbHeightSensor.AddString("松下高度传感器");
	m_cbHeightSensor.AddString("Keyence高度传感器");
	m_cbHeightSensor.SetCurSel((int)theApp.m_tSysParam.heightSensorType);
	m_nHeightSensor = m_cbHeightSensor.GetCurSel();

	//m_cbDetectCamera.AddString("不使用");
	m_cbDetectCamera.AddString("灰点相机");
	m_cbDetectCamera.AddString("大华相机");
	//m_cbDetectCamera.AddString("Im相机");
	m_cbDetectCamera.SetCurSel((int)theApp.m_tSysParam.detectCameraType);
	m_nDetectCamera = m_cbDetectCamera.GetCurSel();
	CString str;
	str.Format("%0.3f",theApp.m_tSysParam.EmbolisD);
    SetDlgItemText(IDC_EDIT_VALVE_DIAMETER,str);
    
	CEdit *pmark=(CEdit *)GetDlgItem(IDC_EDIT_MARKCAMNO);
	m_spinMarkCam.SetBuddy(pmark);
	m_spinMarkCam.SetRange(0,1);
	m_spinMarkCam.SetBase(10);
	m_spinMarkCam.SetPos(theApp.m_tSysParam.nCamMarkNo);

	CEdit *pneedle=(CEdit *)GetDlgItem(IDC_EDIT_NEEDLECAMNO);
	m_spinNeedleCam.SetBuddy(pneedle);
	m_spinNeedleCam.SetRange(0,1);
	m_spinNeedleCam.SetBase(10);
	m_spinNeedleCam.SetPos(theApp.m_tSysParam.nCamNeedleNo);	
	return TRUE;
}

void CDlgConfigure::OnCbnSelchangeComboConfigureHeightSensor()
{
	// TODO: Add your control notification handler code here
	m_nHeightIndex = m_cbHeightSel.GetCurSel();
}

void CDlgConfigure::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	bool bModify = false;
	if(m_nHeightIndex!=(int)theApp.m_tSysParam.sensorType)
	{
		bModify = true;
	}
	if(m_nValveNum!=(int)theApp.m_tSysParam.valveNum)
	{
		bModify = true;
	}
	if(m_nPlcSel!=(int)theApp.m_tSysParam.PlcType)
	{
		bModify = true;
	}
	if(m_nValveSel!=(int)theApp.m_tSysParam.ValveType)
	{
		bModify = true;
	}
	if(m_nHeightSensor!=(int)theApp.m_tSysParam.heightSensorType)
	{
		bModify = true;
	}
	if(m_nDetectCamera!=(int)theApp.m_tSysParam.detectCameraType)
	{
		bModify = true;
	}
	CString str;
	GetDlgItemText(IDC_EDIT_VALVE_DIAMETER,str);
	double dTemp=atof(str);
	if (dTemp!=theApp.m_tSysParam.EmbolisD)
	{
		bModify=true;
	}
	int nTemp1,nTemp2;
	GetDlgItemText(IDC_EDIT_MARKCAMNO,str);
	nTemp1=atoi(str);
	if(nTemp1!=theApp.m_tSysParam.nCamMarkNo)
	{
		bModify=true;
	}
	GetDlgItemText(IDC_EDIT_NEEDLECAMNO,str);
	nTemp2=atoi(str);
	if(nTemp2!=theApp.m_tSysParam.nCamNeedleNo)
	{
		bModify=true;
	}

	if(bModify)
	{
		if(AfxMessageBox("确认设定配置参数，退出程序，重新登录？",MB_YESNO)==IDYES)
		{
			theApp.m_tSysParam.sensorType = (ContactSensorType)m_nHeightIndex;
			theApp.m_tSysParam.valveNum = (ValveNum)m_nValveNum;
			theApp.m_tSysParam.PlcType = (PLC_TYPE)m_nPlcSel;
			theApp.m_tSysParam.ValveType = (VALVE_TYPE)m_nValveSel;
			theApp.m_tSysParam.heightSensorType = (HeightSensorType)m_nHeightSensor;
			theApp.m_tSysParam.detectCameraType = (DetectCameraType)m_nDetectCamera;
			theApp.m_tSysParam.EmbolisD=dTemp;
			theApp.m_tSysParam.nCamMarkNo=nTemp1;
			theApp.m_tSysParam.nCamNeedleNo=nTemp2;
			*m_bSaveConfigure = true;
			theApp.SysParam(FALSE);
			theApp.SysParam(TRUE);//recompute dispscale;//20180502 add;
		}
	}
	OnOK();
}

void CDlgConfigure::OnCbnSelchangeComboConfigureValveNum()
{
	// TODO: Add your control notification handler code here
	m_nValveNum = m_cbValveNum.GetCurSel();
}

void CDlgConfigure::OnCbnSelchangeComboConfigurePlcType()
{
	// TODO: Add your control notification handler code here
	m_nPlcSel = m_cbPlcSel.GetCurSel();
}

void CDlgConfigure::OnCbnSelchangeComboConfigureValveType()
{
	// TODO: Add your control notification handler code here
	m_nValveSel = m_cbValveSel.GetCurSel();
}

void CDlgConfigure::OnCbnSelchangeComboConfigureHeightSensorType()
{
	// TODO: Add your control notification handler code here
	m_nHeightSensor = m_cbHeightSensor.GetCurSel();
}

void CDlgConfigure::OnCbnSelchangeComboConfigureDetectCameraType()
{
	// TODO: Add your control notification handler code here
	m_nDetectCamera = m_cbDetectCamera.GetCurSel();
}

