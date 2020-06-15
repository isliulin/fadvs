// DlgCameraLaser.cpp : implementation file
//

#include "stdafx.h"
#include "DlgCameraLaser.h"
#include "TSCtrlSys.h"
#include  "resource.h"


// CDlgCameraLaser dialog

IMPLEMENT_DYNAMIC(CDlgCameraLaser, CDialog)

CDlgCameraLaser::CDlgCameraLaser(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCameraLaser::IDD, pParent)
{

}

CDlgCameraLaser::~CDlgCameraLaser()
{
}

void CDlgCameraLaser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC_CONTATOR, m_picture);
}


BEGIN_MESSAGE_MAP(CDlgCameraLaser, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgCameraLaser::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgCameraLaser::OnBnClickedCancel)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_NEEDLE_SPEED, &CDlgCameraLaser::OnNMCustomdrawSliderNeedleSpeed)
END_MESSAGE_MAP()


// CDlgCameraLaser message handlers

BOOL CDlgCameraLaser::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString str;
	for(int i=0;i<6;i++)
	{
		m_Button[i].m_iIndex = i;
		m_Button[i].SubclassDlgItem(IDC_BTN_X_NEG+i,this);
		//m_adjustButton[i].SetBitmapEx(IDB_BITMAP_ADJUST);
	}
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->SetPos(1);
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->GetPos();
	CJogButton::m_dJogSpdRatio=n;

	m_picture.m_modtype = MOD_RECTANGLE;
	m_picture.m_iImgWidth = milApp.m_lBufSizeX;
	m_picture.m_iImgHeight = milApp.m_lBufSizeY;
	m_picture.m_pImgBuffer = new BYTE[m_picture.m_iImgWidth*m_picture.m_iImgHeight*3];
	m_picture.m_mod_rectangle.dx = theApp.m_tSysParam.BmpMarkRefCam.TopLeft().x + theApp.m_tSysParam.BmpMarkRefCam.Width()/2;
	m_picture.m_mod_rectangle.dy = theApp.m_tSysParam.BmpMarkRefCam.TopLeft().y + theApp.m_tSysParam.BmpMarkRefCam.Height()/2;
	m_picture.m_mod_rectangle.width = theApp.m_tSysParam.BmpMarkRefCam.Width();
	m_picture.m_mod_rectangle.height = theApp.m_tSysParam.BmpMarkRefCam.Height();
	m_picture.m_mod_scale.length = 20;
	m_picture.m_mod_scale.width = 10;
	m_picture.m_mod_scale.height = 10;
	m_picture.RoiRect =  theApp.m_tSysParam.BmpMarkROI;
	m_picture.SetDisplayMarkImage(true);
	m_picture.ImgInit(milApp.m_lBufSizeX,milApp.m_lBufSizeY,24);
	
	//2016/10/17 机械臂将相机移动到上次接触传感器上方；
	if(theApp.m_Mv400.IsInitOK())
	{
		if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_STOP)
		{
			if(!g_pFrm->m_CmdRun.MoveToZSafety())
			{
				return false;
			}
			if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.CameraOnContactor[0],g_pFrm->m_CmdRun.ProductParam.CameraOnContactor[1],true))
			{
				return false;
			}
		}
	}
    theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,true);
	str.Format("请移动XYZ将相机与接触传感器对准！");
	SetDlgItemText(IDC_EDIT_INFO,str);
	return TRUE;  
}

void CDlgCameraLaser::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能保存当前位置！");
		return;
	}
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,false);
	g_pFrm->m_CmdRun.ProductParam.CameraOnContactor[0]=theApp.m_Mv400.GetPos(K_AXIS_X);
	g_pFrm->m_CmdRun.ProductParam.CameraOnContactor[1]=theApp.m_Mv400.GetPos(K_AXIS_Y);
	theApp.ProductParam(FALSE);
    g_pFrm->m_CmdRun.MoveToZSafety();
	OnOK();
}

void CDlgCameraLaser::OnBnClickedCancel()
{
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,false);
    g_pFrm->m_CmdRun.MoveToZSafety();
	OnCancel();
}

void CDlgCameraLaser::OnNMCustomdrawSliderNeedleSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->GetPos();
	CJogButton::m_dJogSpdRatio=n;
	*pResult = 0;
}
