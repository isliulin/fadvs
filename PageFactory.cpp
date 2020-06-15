// PageFactory.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>
#include "TSCtrlSys.h"
#include "PageFactory.h"


// CPageFactory dialog

IMPLEMENT_DYNAMIC(CPageFactory, CPropertyPage)

CPageFactory::CPageFactory()
	: CPropertyPage(CPageFactory::IDD)
{

}

CPageFactory::~CPageFactory()
{
}

void CPageFactory::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_COMBO_FACTORY_PARAM_MOTOR,m_cbMotor);
	DDX_Control(pDX,IDC_COMBO_FACTORY_PARAM_VEL_TYPE,m_cbVelType);
	DDX_Control(pDX, IDC_COMBO_LOG, m_cbIsLog);
}


BEGIN_MESSAGE_MAP(CPageFactory, CPropertyPage)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_FACTORY_PARAM_MOTOR, &CPageFactory::OnCbnSelchangeComboFactoryParamMotor)
	ON_CBN_SELCHANGE(IDC_COMBO_FACTORY_PARAM_VEL_TYPE, &CPageFactory::OnCbnSelchangeComboFactoryParamVelType)
	ON_EN_CHANGE(IDC_EDIT_FACTORY_PARAM_MAX_POS, &CPageFactory::OnEnChangeEditFactoryParamMaxPos)
	ON_EN_CHANGE(IDC_EDIT_FACTORY_PARAM_MIN_POS, &CPageFactory::OnEnChangeEditFactoryParamMinPos)
	ON_EN_CHANGE(IDC_EDIT_FACTORY_PARAM_MAX_VEL, &CPageFactory::OnEnChangeEditFactoryParamMaxVel)
	ON_EN_CHANGE(IDC_EDIT_FACTORY_PARAM_SCALE, &CPageFactory::OnEnChangeEditFactoryParamScale)
	ON_EN_CHANGE(IDC_EDIT_FACTORY_PARAM_MAX_HOME_VEL, &CPageFactory::OnEnChangeEditFactoryParamMaxHomeVel)
	ON_EN_CHANGE(IDC_EDIT_FACTORY_PARAM_MIN_HOME_VEL, &CPageFactory::OnEnChangeEditFactoryParamMinHomeVel)
	ON_EN_CHANGE(IDC_EDIT_FACTORY_PARAM_HOME_ACC, &CPageFactory::OnEnChangeEditFactoryParamHomeAcc)
	ON_EN_CHANGE(IDC_EDIT_FACTORY_PARAM_VEL, &CPageFactory::OnEnChangeEditFactoryParamVel)
	ON_EN_CHANGE(IDC_EDIT_FACTORY_PARAM_ACC, &CPageFactory::OnEnChangeEditFactoryParamAcc)
	ON_EN_CHANGE(IDC_EDIT_FACTORY_PARAM_JERK, &CPageFactory::OnEnChangeEditFactoryParamJerk)
	ON_EN_CHANGE(IDC_EDIT_FACTORY_PARAM_DEC, &CPageFactory::OnEnChangeEditFactoryParamDec)
	ON_EN_CHANGE(IDC_EDIT_FACTORY_PARAM_STARTVEL, &CPageFactory::OnEnChangeEditFactoryParamStartvel)
END_MESSAGE_MAP()


// CPageFactory message handlers

BOOL CPageFactory::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	m_cbMotor.AddString(_T("X轴"));
	m_cbMotor.AddString(_T("Y轴"));
	m_cbMotor.AddString(_T("ZA轴"));
	m_cbMotor.AddString(_T("ZB轴"));
	m_cbMotor.AddString(_T("ZC轴"));
	m_cbMotor.AddString(_T("U轴"));
	m_cbMotor.AddString(_T("V轴"));
	m_cbMotor.AddString(_T("W轴"));
	m_cbMotor.AddString(_T("A轴"));
	m_cbMotor.AddString(_T("B轴"));
	m_cbMotor.AddString(_T("C轴"));
	m_cbMotor.AddString(_T("D轴"));
	m_cbMotor.SetCurSel(0);
	m_nMotor = m_cbMotor.GetCurSel();
	m_cbVelType.AddString(_T("低速"));
	m_cbVelType.AddString(_T("中速"));
	m_cbVelType.AddString(_T("高速"));
	m_cbVelType.AddString(_T("提升速度"));
	m_cbVelType.SetCurSel(0);
	m_nVelType = m_cbVelType.GetCurSel();
	m_cbIsLog.AddString(_T("不写Log"));
	m_cbIsLog.AddString(_T("写Log"));
	m_cbIsLog.SetCurSel(0);
	UpdateUI();
	InitParam();
	SetTimer(0,100,NULL);
	return TRUE;
}
void CPageFactory::OnCbnSelchangeComboFactoryParamMotor()
{
	// TODO: Add your control notification handler code here
	m_nMotor = m_cbMotor.GetCurSel();
	m_cbVelType.SetCurSel(0);
	m_nVelType = m_cbVelType.GetCurSel();
	UpdateUI();
}

BOOL CPageFactory::OnApply()
{
	if(AfxMessageBox("重要：设置此参数将改变设备参数，需要重启，是否继续?",MB_YESNO) == IDYES)
	{
		SaveParam();
		InitParam();
	}
	else
	{
	}
	return TRUE;
}
void CPageFactory::OnCbnSelchangeComboFactoryParamVelType()
{
	// TODO: Add your control notification handler code here
	m_nVelType = m_cbVelType.GetCurSel();
	UpdateUI();
}

void CPageFactory::UpdateUI()
{
	CString strTemp;
	strTemp.Format("%.3f",theApp.m_tSysParam.tAxis[m_nMotor].maxpos);
	SetDlgItemText(IDC_EDIT_FACTORY_PARAM_MAX_POS,strTemp);
	strTemp.Format("%.3f",theApp.m_tSysParam.tAxis[m_nMotor].minpos);
	SetDlgItemText(IDC_EDIT_FACTORY_PARAM_MIN_POS,strTemp);
	strTemp.Format("%.6f",theApp.m_tSysParam.tAxis[m_nMotor].scale);
	SetDlgItemText(IDC_EDIT_FACTORY_PARAM_SCALE,strTemp);
	strTemp.Format("%.3f",theApp.m_tSysParam.tAxis[m_nMotor].maxVel);
	SetDlgItemText(IDC_EDIT_FACTORY_PARAM_MAX_VEL,strTemp);
	strTemp.Format("%.3f",theApp.m_tSysParam.tAxis[m_nMotor].highHomeVel);
	SetDlgItemText(IDC_EDIT_FACTORY_PARAM_MAX_HOME_VEL,strTemp);
	strTemp.Format("%.3f",theApp.m_tSysParam.tAxis[m_nMotor].lowHomeVel);
	SetDlgItemText(IDC_EDIT_FACTORY_PARAM_MIN_HOME_VEL,strTemp);
	strTemp.Format("%.3f",theApp.m_tSysParam.tAxis[m_nMotor].homeAcc);
	SetDlgItemText(IDC_EDIT_FACTORY_PARAM_HOME_ACC,strTemp);
	strTemp.Format("%.3f",theApp.m_tSysParam.tAxis[m_nMotor].vel[m_nVelType]);
	SetDlgItemText(IDC_EDIT_FACTORY_PARAM_VEL,strTemp);
	strTemp.Format("%.3f",theApp.m_tSysParam.tAxis[m_nMotor].acc[m_nVelType]);
	SetDlgItemText(IDC_EDIT_FACTORY_PARAM_ACC,strTemp);
	strTemp.Format("%0.3f",theApp.m_tSysParam.tAxis[m_nMotor].jerk[m_nVelType]);
	SetDlgItemText(IDC_EDIT_FACTORY_PARAM_JERK,strTemp);
	strTemp.Format("%0.3f",theApp.m_tSysParam.tAxis[m_nMotor].dec[m_nVelType]);
	SetDlgItemText(IDC_EDIT_FACTORY_PARAM_DEC,strTemp);
	strTemp.Format("%0.3f",theApp.m_tSysParam.tAxis[m_nMotor].velStart[m_nVelType]);
	SetDlgItemText(IDC_EDIT_FACTORY_PARAM_STARTVEL,strTemp);
	m_cbIsLog.SetCurSel(theApp.m_tSysParam.iIsLog);
	strTemp.Format("%0.3f",theApp.m_tSysParam.MotorAInitPos);
	SetDlgItemText(IDC_EDIT_FACTORY_PARAM_MOTORA,strTemp);
	strTemp.Format("%0.3f",theApp.m_tSysParam.MotorCInitPos);
	SetDlgItemText(IDC_EDIT_FACTORY_PARAM_MOTORC,strTemp);
	strTemp.Format("%d",theApp.m_tSysParam.iTimeLogOut);
	SetDlgItemText(IDC_EDIT_FACTORY_PARAM_LOGOUT,strTemp);
}

void CPageFactory::SaveParam()
{
	CString str;
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_MAX_POS,str);
	theApp.m_tSysParam.tAxis[m_nMotor].maxpos = atof(str);
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_MIN_POS,str);
	theApp.m_tSysParam.tAxis[m_nMotor].minpos = atof(str);
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_SCALE,str);
	theApp.m_tSysParam.tAxis[m_nMotor].scale = atof(str);
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_MAX_VEL,str);
	theApp.m_tSysParam.tAxis[m_nMotor].maxVel = atof(str);
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_MAX_HOME_VEL,str);
	theApp.m_tSysParam.tAxis[m_nMotor].highHomeVel = atof(str);
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_MIN_HOME_VEL,str);
	theApp.m_tSysParam.tAxis[m_nMotor].lowHomeVel = atof(str);
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_HOME_ACC,str);
	theApp.m_tSysParam.tAxis[m_nMotor].homeAcc = atof(str);
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_VEL,str);
	theApp.m_tSysParam.tAxis[m_nMotor].vel[m_nVelType] = atof(str);
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_ACC,str);
	theApp.m_tSysParam.tAxis[m_nMotor].acc[m_nVelType] = atof(str);
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_JERK,str);
	theApp.m_tSysParam.tAxis[m_nMotor].jerk[m_nVelType] = atof(str);
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_DEC,str);
	theApp.m_tSysParam.tAxis[m_nMotor].dec[m_nVelType] = atof(str);
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_STARTVEL,str);
	theApp.m_tSysParam.tAxis[m_nMotor].velStart[m_nVelType] = atof(str);

	theApp.m_tSysParam.iIsLog=m_cbIsLog.GetCurSel();
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_MOTORA,str);
	theApp.m_tSysParam.MotorAInitPos=atof(str);
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_MOTORC,str);
    theApp.m_tSysParam.MotorCInitPos=atof(str);
	if (theApp.m_tSysParam.MotorCInitPos<=theApp.m_tSysParam.MotorAInitPos)
	{
        theApp.m_tSysParam.MotorCInitPos=theApp.m_tSysParam.MotorAInitPos+1;
	}
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_LOGOUT,str);
	theApp.m_tSysParam.iTimeLogOut=atoi(str);
	theApp.SysParam(FALSE);
}

void CPageFactory::OnEnChangeEditFactoryParamMaxPos()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_MAX_POS,str);
	if(fabs(atof(str)-theApp.m_tSysParam.tAxis[m_nMotor].maxpos)<0.001)
	{
		m_bMaxPosModify = false;
	}
	else
	{
		m_bMaxPosModify = true;
	}
}

void CPageFactory::OnEnChangeEditFactoryParamMinPos()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_MIN_POS,str);
	if(fabs(atof(str)-theApp.m_tSysParam.tAxis[m_nMotor].minpos)<0.001)
	{
		m_bMinPosModify = false;
	}
	else
	{
		m_bMinPosModify = true;
	}
}

void CPageFactory::OnEnChangeEditFactoryParamMaxVel()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_MAX_VEL,str);
	if(fabs(atof(str)-theApp.m_tSysParam.tAxis[m_nMotor].maxVel)<0.001)
	{
		m_bMaxVelModify = false;
	}
	else
	{
		m_bMaxVelModify = true;
	}
}

void CPageFactory::OnEnChangeEditFactoryParamScale()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_SCALE,str);
	if(fabs(atof(str)-theApp.m_tSysParam.tAxis[m_nMotor].scale)<0.0000001)
	{
		m_bScaleModify = false;
	}
	else
	{
		m_bScaleModify = true;
	}
}

void CPageFactory::OnEnChangeEditFactoryParamMaxHomeVel()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_MAX_HOME_VEL,str);
	if(fabs(atof(str)-theApp.m_tSysParam.tAxis[m_nMotor].highHomeVel)<0.001)
	{
		m_bMaxHomeVelModify = false;
	}
	else
	{
		m_bMaxHomeVelModify = true;
	}
}

void CPageFactory::OnEnChangeEditFactoryParamMinHomeVel()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_MIN_HOME_VEL,str);
	if(fabs(atof(str)-theApp.m_tSysParam.tAxis[m_nMotor].lowHomeVel)<0.001)
	{
		m_bMinHomeVelModify = false;
	}
	else
	{
		m_bMinHomeVelModify = true;
	}
}

void CPageFactory::OnEnChangeEditFactoryParamHomeAcc()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_HOME_ACC,str);
	if(fabs(atof(str)-theApp.m_tSysParam.tAxis[m_nMotor].homeAcc)<0.001)
	{
		m_bHomeAccModify = false;
	}
	else
	{
		m_bHomeAccModify = true;
	}
}

void CPageFactory::OnEnChangeEditFactoryParamVel()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_VEL,str);
	if(fabs(atof(str)-theApp.m_tSysParam.tAxis[m_nMotor].vel[m_nVelType])<0.001)
	{
		m_bVelModify = false;
	}
	else
	{
		m_bVelModify = true;
	}
}

void CPageFactory::OnEnChangeEditFactoryParamAcc()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_ACC,str);
	if(fabs(atof(str)-theApp.m_tSysParam.tAxis[m_nMotor].acc[m_nVelType])<0.001)
	{
		m_bAccModify = false;
	}
	else
	{
		m_bAccModify = true;
	}
}

void CPageFactory::OnEnChangeEditFactoryParamJerk()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_JERK,str);
	if(fabs(atof(str)-theApp.m_tSysParam.tAxis[m_nMotor].jerk[m_nVelType])<0.001)
	{
		m_bJerkModify = false;
	}
	else
	{
		m_bJerkModify = true;
	}
}

void CPageFactory::OnTimer(UINT_PTR nIDEvent)
{
	if(m_bMaxPosModify||m_bMinPosModify||m_bMaxVelModify||m_bScaleModify||m_bMaxHomeVelModify||m_bMinHomeVelModify||
		m_bHomeAccModify||m_bVelModify||m_bAccModify||m_bJerkModify||m_bDecModify||m_bStartVelModify)
	{
		SetModified(TRUE);
	}
	else
	{
		SetModified(FALSE);
	}
}

void CPageFactory::InitParam()
{
	m_bMaxPosModify = false;
	m_bMinPosModify = false;
	m_bMaxVelModify = false;
	m_bScaleModify = false;
	m_bMaxHomeVelModify = false;
	m_bMinHomeVelModify = false;
	m_bHomeAccModify = false;
	m_bVelModify = false;
	m_bAccModify = false;
	m_bJerkModify = false;
	m_bDecModify = false;
	m_bStartVelModify = false;
}

void CPageFactory::OnEnChangeEditFactoryParamDec()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_DEC,str);
	if(fabs(atof(str)-theApp.m_tSysParam.tAxis[m_nMotor].dec[m_nVelType])<0.001)
	{
		m_bDecModify = false;
	}
	else
	{
		m_bDecModify = true;
	}
}

void CPageFactory::OnEnChangeEditFactoryParamStartvel()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_FACTORY_PARAM_STARTVEL,str);
	if(fabs(atof(str)-theApp.m_tSysParam.tAxis[m_nMotor].velStart[m_nVelType])<0.001)
	{
		m_bStartVelModify = false;
	}
	else
	{
		m_bStartVelModify = true;
	}
}

