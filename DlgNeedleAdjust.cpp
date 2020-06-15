// DlgNeedleAdjust.cpp : implementation file
//

#include "stdafx.h"
#include "DlgNeedleAdjust.h"
#include "TSCtrlSys.h"
#include "Function.h"

// CDlgNeedleAdjust dialog

IMPLEMENT_DYNAMIC(CDlgNeedleAdjust, CDialog)

CDlgNeedleAdjust::CDlgNeedleAdjust(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNeedleAdjust::IDD, pParent)
{
     nStep=0;
}

CDlgNeedleAdjust::~CDlgNeedleAdjust()
{
}

void CDlgNeedleAdjust::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgNeedleAdjust, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgNeedleAdjust::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_NEEDLEONE, &CDlgNeedleAdjust::OnBnClickedBtnNeedleone)
	ON_BN_CLICKED(IDC_BTN_NEEDLETWO, &CDlgNeedleAdjust::OnBnClickedBtnNeedletwo)
	ON_BN_CLICKED(IDC_BTN_NEEDLETHREE, &CDlgNeedleAdjust::OnBnClickedBtnNeedlethree)
	ON_BN_CLICKED(IDC_BTN_START, &CDlgNeedleAdjust::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDOVER, &CDlgNeedleAdjust::OnBnClickedOver)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDCANCEL, &CDlgNeedleAdjust::OnBnClickedCancel)
	ON_MESSAGE(WM_MSG_ADJUSTFINISH_CAM, &CDlgNeedleAdjust::OnMessageCamAdjust)
	ON_MESSAGE(WM_MSG_ADJUSTFINISH_NEEDLEONE, &CDlgNeedleAdjust::OnMessageNeedleOneAdjust)
	ON_MESSAGE(WM_MSG_ADJUSTFINISH_NEEDLETWO, &CDlgNeedleAdjust::OnMessageNeedleTwoAdjust)
	ON_MESSAGE(WM_MSG_ADJUSTFINISH_NEEDLETHREE, &CDlgNeedleAdjust::OnMessageNeedleThreeAdjust)
END_MESSAGE_MAP()


// CDlgNeedleAdjust message handlers
//相机调整完成
LRESULT CDlgNeedleAdjust::OnMessageCamAdjust(WPARAM wparam, LPARAM lparam)
{
	//Sleep(1000);          //20180907:3000->1000
	int nResult=int(lparam);
	if (1==nResult) //OK
	{
       OnBnClickedBtnNeedleone();
	}
	else//NG
	{
		g_pFrm->m_CmdRun.m_bAutoNeedleErr=true;
		EnableButton(2);

	}
	return 0;
}
//头1调整完成
LRESULT CDlgNeedleAdjust::OnMessageNeedleOneAdjust(WPARAM wparam, LPARAM lparam)
{
	//Sleep(1000);          //20180907:3000->1000
	int nResult=int(lparam);
	if (1==nResult) //ok
	{
		OnBnClickedBtnNeedletwo();
	}
	else//ng
	{
		g_pFrm->m_CmdRun.m_bAutoNeedleErr=true;
		EnableButton(3);
	}
	return 0;
}
//头2调整完成
LRESULT CDlgNeedleAdjust::OnMessageNeedleTwoAdjust(WPARAM wparam, LPARAM lparam)
{
	//Sleep(1000);         //20180907:3000->1000
	int nResult=int(lparam);
	if (1==nResult) //ok
	{
		OnBnClickedBtnNeedlethree();
	}
	else//ng
	{
		g_pFrm->m_CmdRun.m_bAutoNeedleErr=true;
		EnableButton(4);
	}
	return 0;
}
//头3调整完成
LRESULT CDlgNeedleAdjust::OnMessageNeedleThreeAdjust(WPARAM wparam, LPARAM lparam)
{
	//Sleep(1000);        //20180907:3000->1000
	int nResult=int(lparam);
	if (1==nResult) //ok
	{
		OnBnClickedOver();			
	}
	else//ng
	{
		g_pFrm->m_CmdRun.m_bAutoNeedleErr=true;
		EnableButton(5);
	}
	return 0;
}

void CDlgNeedleAdjust::OnBnClickedOk()
{
	m_DlgNeedleThree.bAdjust=false;
	theApp.ProductParam(FALSE);
	OnOK();
}

BOOL CDlgNeedleAdjust::OnInitDialog()
{
	CDialog::OnInitDialog();	
	GetDlgItem(IDC_PIC)->GetWindowRect(&rect);//screen coordinate
	ScreenToClient(&rect);//client coordinate
	m_DlgCamera.Create(IDD_DLG_CAMERA_ADJUST,this);
	m_DlgCamera.MoveWindow(rect);
	m_DlgCamera.ShowWindow(SW_SHOW);

	m_DlgNeedleOne.Create(IDD_DLG_NEEDLEONE_ADJUST,this);
	m_DlgNeedleOne.MoveWindow(rect);
	m_DlgNeedleOne.ShowWindow(SW_HIDE);

	m_DlgNeedleTwo.Create(IDD_DLG_NEEDLETWO_ADJUST,this);
	m_DlgNeedleTwo.MoveWindow(rect);
	m_DlgNeedleTwo.ShowWindow(SW_HIDE);

	m_DlgNeedleThree.Create(IDD_DLG_NEEDLETHREE_ADJUST,this);
	m_DlgNeedleThree.MoveWindow(rect);
	m_DlgNeedleThree.ShowWindow(SW_HIDE);

    ::RegisterHotKey(this->GetSafeHwnd(),2004,MOD_ALT,VK_F4);//注册ALT+F4

	if(!g_pFrm->m_CmdRun.SetAxisProfile(LOW_VEL))
	{
		AfxMessageBox("设置电机参数错误！");
	}
	//光源控制：
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outNeedleLight,FALSE);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,TRUE);
	//SetTimer(1,500,NULL);
	//按钮状态
	EnableButton(2);
	EnableWindow(2);
	return TRUE; 
}
//使能按钮状态:按钮显示+标识显示
void CDlgNeedleAdjust::EnableButton(int nIndex)
{
	switch(nIndex)
	{
	case 1:
		GetDlgItem(IDC_BTN_START)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTN_NEEDLEONE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_NEEDLETWO)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_NEEDLETHREE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDOVER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_STEPONE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_STEPTWO)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_STATIC_STEPTHREE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_STEPFOUR)->ShowWindow(SW_HIDE);
		break;
	case 2:
		GetDlgItem(IDC_BTN_START)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_NEEDLEONE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTN_NEEDLETWO)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_NEEDLETHREE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDOVER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_STEPONE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_STEPTWO)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_STEPTHREE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_STEPFOUR)->ShowWindow(SW_HIDE);
		break;
	case 3:
		GetDlgItem(IDC_BTN_START)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_NEEDLEONE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_NEEDLETWO)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTN_NEEDLETHREE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDOVER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_STEPONE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_STEPTWO)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_STEPTHREE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_STEPFOUR)->ShowWindow(SW_HIDE);
		break;
	case 4:
		GetDlgItem(IDC_BTN_START)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_NEEDLEONE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_NEEDLETWO)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_NEEDLETHREE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDOVER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_STEPONE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_STEPTWO)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_STEPTHREE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_STEPFOUR)->ShowWindow(SW_SHOW);
		break;
	case 5:
		GetDlgItem(IDC_BTN_START)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_NEEDLEONE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_NEEDLETWO)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_NEEDLETHREE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDOVER)->ShowWindow(SW_SHOW);
		break;
	default:break;
	}
}
//切换窗口显示
void CDlgNeedleAdjust::EnableWindow(int nIndex)
{
	switch(nIndex)
	{
	case 1:
		m_DlgCamera.ShowWindow(SW_SHOW);
		m_DlgNeedleOne.ShowWindow(SW_HIDE);
		m_DlgNeedleTwo.ShowWindow(SW_HIDE);
		m_DlgNeedleThree.ShowWindow(SW_HIDE);
		m_DlgCamera.UpdateWindow();
		//m_DlgNeedleOne.UpdateWindow();
		//m_DlgNeedleTwo.UpdateWindow();
		//m_DlgNeedleThree.UpdateWindow();
		break;
	case 2:
		m_DlgCamera.ShowWindow(SW_HIDE);
		m_DlgNeedleOne.ShowWindow(SW_SHOW);
		m_DlgNeedleTwo.ShowWindow(SW_HIDE);
		m_DlgNeedleThree.ShowWindow(SW_HIDE);
		//m_DlgCamera.UpdateWindow();
		m_DlgNeedleOne.UpdateWindow();
		//m_DlgNeedleTwo.UpdateWindow();
		//m_DlgNeedleThree.UpdateWindow();
		break;
	case 3:
		m_DlgCamera.ShowWindow(SW_HIDE);
		m_DlgNeedleOne.ShowWindow(SW_HIDE);
		m_DlgNeedleTwo.ShowWindow(SW_SHOW);
		m_DlgNeedleThree.ShowWindow(SW_HIDE);
		//m_DlgCamera.UpdateWindow();
		//m_DlgNeedleOne.UpdateWindow();
		m_DlgNeedleTwo.UpdateWindow();
		//m_DlgNeedleThree.UpdateWindow();
		break;
	case 4:
		m_DlgCamera.ShowWindow(SW_HIDE);
		m_DlgNeedleOne.ShowWindow(SW_HIDE);
		m_DlgNeedleTwo.ShowWindow(SW_HIDE);
		m_DlgNeedleThree.ShowWindow(SW_SHOW);
		//m_DlgCamera.UpdateWindow();
		//m_DlgNeedleOne.UpdateWindow();
		//m_DlgNeedleTwo.UpdateWindow();
		m_DlgNeedleThree.UpdateWindow();
		break;
	default:break;
	}
}
void CDlgNeedleAdjust::OnBnClickedBtnNeedleone()
{
	nStep=2;
	g_pFrm->m_CmdRun.m_bAutoNeedleErr=false;	
	GetDlgItem(IDC_BTN_NEEDLEONE)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_STEPTWO)->EnableWindow(FALSE); //隐藏标识
	EnableWindow(2);//2018-09-13
    Sleep(2000);//waiting axis stop......	
	//m_DlgCamera.bAdjust=false;
	/*g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].x=theApp.m_Mv400.GetPos(K_AXIS_X);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].y=theApp.m_Mv400.GetPos(K_AXIS_Y);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].za=theApp.m_Mv400.GetPos(K_AXIS_ZA);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].zb=theApp.m_Mv400.GetPos(K_AXIS_ZB);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].zc=theApp.m_Mv400.GetPos(K_AXIS_ZC);
	theApp.ProductParam(FALSE);*/
    g_pFrm->m_CmdRun.m_bNeedleAutoAdjust=true;
	if (!g_pFrm->m_CmdRun.MoveToZSafety())  //Enter into Mid-Speed secretly
	{
		CString str;
		g_pFrm->m_CmdRun.m_bNeedleAutoAdjust=false;
		str.Format("移动1号针头到基准位时：Z到安全位失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return;
	}

	//Go to the lable position of needle one:relative to the under camera(DH)
	if (!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].x,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].y,true))
	{
		CString str;
		m_DlgNeedleOne.bAdjust=false;
		str.Format("移动1号针头到基准位时：XY定位失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.m_bNeedleAutoAdjust=false;
		AfxMessageBox("1号针头XY运动到基准位时失败！");
		return;
	}

	//Enter into Low-Speed mode
	if(!g_pFrm->m_CmdRun.SetAxisProfile(LOW_VEL)) 
	{
		CString str;
		str.Format("自动调整1号针头时XYZ切换到低速失败！");
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		AfxMessageBox("设置电机参数错误！");
	}

	//Go to the lable position of needle one:relative to the under camera(DH)
	if (!g_pFrm->m_CmdRun.SynchronizeMoveZ(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].za,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].zb
		,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].zc,false,true))
	{
		CString str;
		m_DlgNeedleOne.bAdjust=false;
		str.Format("1号针头到基准位时：Z定位失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.m_bNeedleAutoAdjust=false;
		AfxMessageBox("1号针头到基准位时Z定位失败！");
		return;
	}
	else
	{
		CString str;
		str.Format("1号针头开始对准基准相机!");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);

		//Light Control：
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outNeedleLight,TRUE);
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,FALSE);
		//Load Model Parameter;
		milApp.SetModelWindow(theApp.m_tSysParam.BmpNeedleLearnWin);
		milApp.SetSearchWindow(theApp.m_tSysParam.BmpNeedleROI);
		CString strFile=g_pDoc->GetModeFile(11);
		milApp.RestoreModel(strFile.GetBuffer(strFile.GetLength()));
		m_DlgNeedleOne.bAdjust=true; 

	}
	//界面切换显示
	//EnableWindow(2); //2018-09-13 移位置顶。
	//开始自动调整
	m_DlgNeedleOne.bAdjust=true;
	m_DlgNeedleOne.EnableButton(false);//禁用手动按钮
	m_DlgNeedleOne.AutoNeedleAdjust();//自动校正
	m_DlgNeedleOne.EnableButton(true);//启用手动按钮
	//EnableBotton(true);
}

void CDlgNeedleAdjust::OnBnClickedBtnNeedletwo()
{
	nStep=3;
	g_pFrm->m_CmdRun.m_bAutoNeedleErr=false;
	GetDlgItem(IDC_BTN_NEEDLETWO)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_STEPTHREE)->EnableWindow(FALSE); //隐藏标识
    EnableWindow(3);//2018-09-13
	double dXOffset,dXNewPos;
	double dYOffset,dYNewPos; 
	CString str;
	dXOffset=dYOffset=0;
	m_DlgNeedleOne.bAdjust=false;
    Sleep(2000);//waiting axis stop......
	dXNewPos=theApp.m_Mv400.GetPos(K_AXIS_X);
	dXOffset=dXNewPos-g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].x;
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].x=dXNewPos;
	g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[0]+=dXOffset; //自动修正更新X偏移值     //点位1
	theApp.m_tSysParam.CleanPosition[0]+=dXOffset;                                         //点位2
	for(int i=0;i<g_pFrm->m_CmdRun.ProductParam.TestDotNumber;i++)                         //点位3
	{
		g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].x+=dXOffset;
	}
	theApp.m_tSysParam.DischargeGluePostion[0]+=dXOffset;                                  //点位4

	dYNewPos=theApp.m_Mv400.GetPos(K_AXIS_Y);
	dYOffset=dYNewPos-g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].y;
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].y=dYNewPos;
	g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[1]+=dYOffset; //自动修正更新Y偏移值     //点位1
	theApp.m_tSysParam.CleanPosition[1]+=dYOffset;                                         //点位2
	for(int i=0;i<g_pFrm->m_CmdRun.ProductParam.TestDotNumber;i++)                         //点位3
	{
		g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].y+=dYOffset;
	}
	theApp.m_tSysParam.DischargeGluePostion[1]+=dYOffset;                                  //点位4
    ////**************************************************************************//
	str.Format("自动对针1后自动修正坐标值:X:%0.3f;Y:%0.3f",dXOffset,dYOffset);
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);
   ////**************************************************************************//
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].za=theApp.m_Mv400.GetPos(K_AXIS_ZA);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].zb=theApp.m_Mv400.GetPos(K_AXIS_ZB);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].zc=theApp.m_Mv400.GetPos(K_AXIS_ZC);
	theApp.ProductParam(FALSE);
    theApp.SysParam(FALSE);

	//Enter into the Mid-Speed secretly;
	if (!g_pFrm->m_CmdRun.MoveToZSafety())  
	{
		CString str;
		str.Format("移动2号针头到基准位时：Z到安全位失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.m_bNeedleAutoAdjust=false;
		return;
	}

	if (!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].x,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].y,true))
	{
		CString str;
		m_DlgNeedleTwo.bAdjust=false;
		str.Format("移动2号针头到基准位时：XY定位失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.m_bNeedleAutoAdjust=false;
		AfxMessageBox("2号针头XY运动到基准位时失败！");
		return;
	}

    //tune slightly A,B motor of Needle Two to the historical lable position 
	theApp.m_Mv400.Move(K_AXIS_A,g_pFrm->m_CmdRun.ProductParam.offset2x,true,true);
	theApp.m_Mv400.Move(K_AXIS_B,g_pFrm->m_CmdRun.ProductParam.offset2y,true,true);

	//Switch to Low-Speed mode
	if(!g_pFrm->m_CmdRun.SetAxisProfile(LOW_VEL)) 
	{
		AfxMessageBox("设置电机参数错误！");
	}

	if (!g_pFrm->m_CmdRun.SynchronizeMoveZ(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].za,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].zb,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].zc,false,true))
	{
		CString str;
		m_DlgNeedleTwo.bAdjust=false;
		str.Format("移动2号针头到基准位时：Z定位失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);	
		g_pFrm->m_CmdRun.m_bNeedleAutoAdjust=false;
		AfxMessageBox("2号针头Z运动到基准位时失败！");
		return;
	}
	else
	{
		CString str;
		str.Format("2号针头开始执行对准基准相机!");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		milApp.SetModelWindow(theApp.m_tSysParam.BmpNeedleLearnWin);
		milApp.SetSearchWindow(theApp.m_tSysParam.BmpNeedleROI);
		CString strFile=g_pDoc->GetModeFile(11);
		milApp.RestoreModel(strFile.GetBuffer(strFile.GetLength()));
		m_DlgNeedleTwo.bAdjust=true;  
	}
	
	//EnableWindow(3);//2018-09-13移至顶部
	m_DlgNeedleTwo.bAdjust=true;
	m_DlgNeedleTwo.EnableButton(false);//Forbid the manual operation
	m_DlgNeedleTwo.AutoNeedleAdjust(); //Implement the auto adjust
	m_DlgNeedleTwo.EnableButton(true); //Permit the manual operation
}

void CDlgNeedleAdjust::OnBnClickedBtnNeedlethree()
{
	nStep=4;
	g_pFrm->m_CmdRun.m_bAutoNeedleErr=false;
	GetDlgItem(IDC_BTN_NEEDLETHREE)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_STEPFOUR)->EnableWindow(FALSE); //隐藏标识
	m_DlgNeedleTwo.bAdjust=false;
	EnableWindow(4);//2018-09-13
	Sleep(2000);//waiting axis stop......
	//Update and record the lable position of needle two;
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].x=theApp.m_Mv400.GetPos(K_AXIS_X);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].y=theApp.m_Mv400.GetPos(K_AXIS_Y);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].za=theApp.m_Mv400.GetPos(K_AXIS_ZA);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].zb=theApp.m_Mv400.GetPos(K_AXIS_ZB);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].zc=theApp.m_Mv400.GetPos(K_AXIS_ZC);
	g_pFrm->m_CmdRun.ProductParam.offset2x=theApp.m_Mv400.GetPos(K_AXIS_A);
	g_pFrm->m_CmdRun.ProductParam.offset2y=theApp.m_Mv400.GetPos(K_AXIS_B);
	theApp.ProductParam(FALSE);

    //Enter into Mid-Speed secretly
	if (!g_pFrm->m_CmdRun.MoveToZSafety())  
	{
		CString str;
		str.Format("准备针头3调整时Z到安全位失败 !");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return;
	}

	if (!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2].x,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].y,true))
	{
		CString str;
		m_DlgNeedleThree.bAdjust=false;
		str.Format("针头3调整时XY定位失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		AfxMessageBox("针头3调整时XY定位失败！");
		return;
	}
 
	//tune slightly the C,D motor of needle three to historical position
	theApp.m_Mv400.Move(K_AXIS_C,g_pFrm->m_CmdRun.ProductParam.offset3x,true,true);
	theApp.m_Mv400.Move(K_AXIS_D,g_pFrm->m_CmdRun.ProductParam.offset3y,true,true);

	//Switch to low mode
	if(!g_pFrm->m_CmdRun.SetAxisProfile(LOW_VEL)) 
	{
		AfxMessageBox("设置电机参数错误！");
	}
	if (!g_pFrm->m_CmdRun.SynchronizeMoveZ(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2].za,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2].zb,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2].zc,false,true))
	{
		CString str;
		m_DlgNeedleThree.bAdjust=false;
		str.Format("针头3调整时Z定位失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);		
		AfxMessageBox("针头3调整时Z定位失败！");
		return;
	}
	else
	{
		CString str;
		str.Format("针头3开始自动调整!");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		milApp.SetModelWindow(theApp.m_tSysParam.BmpNeedleLearnWin);
		milApp.SetSearchWindow(theApp.m_tSysParam.BmpNeedleROI);
		CString strFile=g_pDoc->GetModeFile(11);
		milApp.RestoreModel(strFile.GetBuffer(strFile.GetLength()));
		m_DlgNeedleThree.bAdjust=true;  
	}

	//EnableWindow(4); //2018-09-13 移至顶部 
	m_DlgNeedleThree.bAdjust=true;
	m_DlgNeedleThree.EnableButton(false);//forbid the manual operation before action
	m_DlgNeedleThree.AutoNeedleAdjust(); //implement the auto adjust 
	m_DlgNeedleThree.EnableButton(true); //permit the munual operation after action
	
}
//开始按钮
void CDlgNeedleAdjust::OnBnClickedBtnStart()
{
	nStep=1;
    g_pFrm->m_CmdRun.m_bNeedleAutoAdjust=true; //20180830

	if (!g_pFrm->m_CmdRun.MoveToZSafety()) //Enter into Mid-Speed mode secretly
	{
		CString str;
		str.Format("上下相机标定前Z到安全位失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.m_bNeedleAutoAdjust=false; //20180830
		return ;
	}

	if (!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].x,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].y,true))
	{
		CString str;
		m_DlgCamera.bAdjust=false;
		str.Format("上下相机标定时XY到纠偏位失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		AfxMessageBox("上下相机标定时XY到纠偏位失败！");
		g_pFrm->m_CmdRun.m_bNeedleAutoAdjust=false; //20180830
		return ;
	}

	if(!g_pFrm->m_CmdRun.SetAxisProfile(LOW_VEL)) //switch to Low-Speed mode
	{
		AfxMessageBox("设置电机参数错误！");
	}

	if (!g_pFrm->m_CmdRun.SynchronizeMoveZ(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].za,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].zb,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].zc,false,true))
	{
		CString str;
		m_DlgCamera.bAdjust=false;
		str.Format("上下相机标定时Z到纠偏位失败!");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.m_bNeedleAutoAdjust=false; //20180830
		AfxMessageBox("上下相机标定时Z到纠偏位失败");
		return ;
	}
	else
	{
		GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);  //隐藏按钮
		GetDlgItem(IDC_STATIC_STEPONE)->EnableWindow(FALSE); //隐藏标识
		CString str;
		str.Format("开始准备上下相机调整！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
        //加载下方圆孔模板数据
		milApp.SetModelWindow(theApp.m_tSysParam.BmpMarkRefCam);
		milApp.SetSearchWindow(theApp.m_tSysParam.BmpMarkRefCamROI);
		CString strFile=g_pDoc->GetModeFile(10);
		milApp.RestoreModel(strFile.GetBuffer(strFile.GetLength()));
		CFunction::DelayEx(1);
		m_DlgCamera.bAdjust=true;
        m_DlgCamera.AutoCamAdjust();//
	}
}


void CDlgNeedleAdjust::OnBnClickedOver()
{
	g_pFrm->m_CmdRun.m_bAutoNeedleErr=false;
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outNeedleLight,FALSE);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,TRUE);
    Sleep(2000);//waiting axis stop......
	m_DlgNeedleThree.bAdjust=false;
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2].x=theApp.m_Mv400.GetPos(K_AXIS_X);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2].y=theApp.m_Mv400.GetPos(K_AXIS_Y);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2].za=theApp.m_Mv400.GetPos(K_AXIS_ZA);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2].zb=theApp.m_Mv400.GetPos(K_AXIS_ZB);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2].zc=theApp.m_Mv400.GetPos(K_AXIS_ZC);
	g_pFrm->m_CmdRun.ProductParam.offset2x=theApp.m_Mv400.GetPos(K_AXIS_A);
	g_pFrm->m_CmdRun.ProductParam.offset2y=theApp.m_Mv400.GetPos(K_AXIS_B);
	g_pFrm->m_CmdRun.ProductParam.offset3x=theApp.m_Mv400.GetPos(K_AXIS_C);
	g_pFrm->m_CmdRun.ProductParam.offset3y=theApp.m_Mv400.GetPos(K_AXIS_D);
	theApp.ProductParam(FALSE);
	if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
	{
		AfxMessageBox("设置电机参数错误！");
	}
	if (!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		CString str;
		str.Format("针头3调整完毕，Z回安全位失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
	}
	g_pFrm->m_CmdRun.m_bNeedleAutoAdjust=false;
	OnOK();
}

void CDlgNeedleAdjust::OnTimer(UINT_PTR nIDEvent)
{
	static bool bdraw=false;
	if (bdraw)
		return;
	bdraw=true;
	switch(nStep)
	{
	case 1:
		m_DlgCamera.Invalidate();
		m_DlgCamera.UpdateWindow();
		break;
	case 2:
		m_DlgNeedleOne.Invalidate();
		m_DlgNeedleOne.UpdateWindow();
		break;
	case 3:
		m_DlgNeedleTwo.Invalidate();
		m_DlgNeedleTwo.UpdateWindow();
		break;
	case 4:
		m_DlgNeedleThree.Invalidate();
		m_DlgNeedleThree.UpdateWindow();
		break;
	default:
		break;
	}
	this->Invalidate();
	this->UpdateWindow();
	bdraw=false;
	CDialog::OnTimer(nIDEvent);
}

void CDlgNeedleAdjust::ButtonClever()
{
	if (m_DlgCamera.bOver)
	{
		GetDlgItem(IDC_BTN_START)->EnableWindow(false);
	}
	if (m_DlgNeedleOne.bOver)
	{
		GetDlgItem(IDC_BTN_NEEDLEONE)->EnableWindow(false);
	}
	if (m_DlgNeedleTwo.bOver)
	{
		GetDlgItem(IDC_BTN_NEEDLETWO)->EnableWindow(false);
	}
	if (m_DlgNeedleThree.bOver)
	{
		GetDlgItem(IDC_BTN_NEEDLETHREE)->EnableWindow(false);
	}
}

void CDlgNeedleAdjust::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
}

void CDlgNeedleAdjust::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
	{
		AfxMessageBox("设置电机参数错误！");
	}
	g_pFrm->m_CmdRun.MoveToZSafety();
	OnCancel();
}

BOOL CDlgNeedleAdjust::PreTranslateMessage(MSG* pMsg)
{
	if(WM_SYSKEYDOWN==pMsg->message&&VK_F4==pMsg->wParam)   //ALT+F4
		return true;

	if(WM_KEYDOWN==pMsg->message&&VK_ESCAPE==pMsg->wParam) //ESC
		return true;
	return CDialog::PreTranslateMessage(pMsg);
}
