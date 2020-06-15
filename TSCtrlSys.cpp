
// TSCtrlSys.cpp : 定义应用程序的类行为。
#include "stdafx.h"
#include "afxwinappex.h"
#include "TSCtrlSys.h"
#include "MainFrm.h"

#include "TSCtrlSysDoc.h"
#include "TSCtrlSysView.h"
#include "ImgStatic.h"
#include "MilApp.h"
#include "vn_define.h"
#include <math.h>
#include "DlgURLPrompt.h"

#define  PI 3.1415926
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTSCtrlSysApp
BEGIN_MESSAGE_MAP(CTSCtrlSysApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CTSCtrlSysApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()

// CTSCtrlSysApp 构造
CTSCtrlSysApp::CTSCtrlSysApp()
{
	m_bHiColorIcons = TRUE;
	dGlueNewest = 0.5;
    bMachineEnable = false;
	m_bInitOk = false;
}

// 唯一的一个 CTSCtrlSysApp 对象
CTSCtrlSysApp theApp;
CMainFrame *g_pFrm = NULL;
CTSCtrlSysDoc *g_pDoc = NULL;
CTSCtrlSysView *g_pView = NULL;
//CImgStatic *g_imagStatic=NULL;//定义显示图像的指针BY LI
CMilApp milApp;
CRITICAL_SECTION  g_cs; 

// CTSCtrlSysApp 析构
CTSCtrlSysApp::~CTSCtrlSysApp()
{
	DelRegTree(HKEY_CURRENT_USER,_T("SOFTWARE\\KSOFT"));
}

BOOL CTSCtrlSysApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	if(!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("KSOFT"));
	LoadStdProfileSettings(4);// 加载标准 INI 文件选项(包括 MRU)
	InitContextMenuManager();
	InitShellManager();
	InitKeyboardManager();
	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);
	SysParam(TRUE);
	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		//NULL,
		RUNTIME_CLASS(CTSCtrlSysDoc),
		RUNTIME_CLASS(CMainFrame),          //主SDI框架窗口
		RUNTIME_CLASS(CTSCtrlSysView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// 启用“DDE 执行”
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	//SysParam(TRUE);
	// 分析标准外壳命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if(!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 SDI 应用程序中，这应在 ProcessShellCommand 之后发生
	// 启用拖/放
	//m_pMainWnd->DragAcceptFiles();
	//////////////////////////////////////////////////////////////////////////
	CRect rect;
	g_pView->GetClientRect(&rect);
	g_pView->SendMessage(WM_SIZE, SIZE_MAXIMIZED, MAKELPARAM(rect.Width(), rect.Height()));

	m_EncryptOp.Init();
	//SysParam(TRUE);
	BmpParam(TRUE);//图像参数
	ProductParam(TRUE);//产品参数
	V9Param(TRUE);//V9参数
	//开启日志监控线程
	g_pFrm->m_CmdRun.RunThreadLog();
	////////////////////////////////获取服务器配置//////////////////////////
	ServerInfoGet();
	////////////////////////////////////////////////////////////////////////
	CString strTemp;
	strTemp = m_Mv400.InitGE(&g_pFrm->m_wndOutput.m_wndOutputBuild, &m_tSysParam, g_pFrm->m_pLog) ? "运行卡OK" : "运行卡NG";	
	g_pFrm->m_CmdRun.m_bHoming = true;
	theApp.m_tSysParam.ThreeAxHomeSu=false;
	g_pFrm->m_CmdRun.m_tStatus = K_RUN_STS_NONE;
	g_pFrm->m_CmdRun.m_LastStatus = K_RUN_STS_NONE; //20180713
	if(m_Mv400.IsInitOK())
	{
		//theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outDispenseFinish,true);
		//Sleep(200);
		//theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outDispenseFinish,false);//清料 PLC
		if(!theApp.m_Mv400.Home(50,5,1000))
		{
			AfxMessageBox("回原点失败请重新回零!");					
		}
		else
		{
			m_Mv400.WaitStop();
			theApp.m_tSysParam.ThreeAxHomeSu=true;
			g_pFrm->m_CmdRun.m_tStatus = K_RUN_STS_STOP;
			g_pFrm->m_CmdRun.m_LastStatus = K_RUN_STS_STOP;
		}
		theApp.m_tSysParam.V9HomeSu=false;
		theApp.m_tSysParam.AdjustMotorHomeSu=false;//1201
		g_pFrm->m_CmdRun.m_bHoming = false;
		//theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outPlCRSafe,true);
		/////////
		if(!g_pFrm->m_CmdRun.AdjustMotorHome())
		{
			AfxMessageBox("调整电机复位失败！");
		}
		else
		{
			theApp.m_tSysParam.AdjustMotorHomeSu=true;//1201
			if(AfxMessageBox("调整电机移动到记录位置！",MB_YESNO)==IDYES)
			{
				if(!g_pFrm->m_CmdRun.AdjustNeedle())//1027 修改
				{
					AfxMessageBox("调整电机移动到记录位置失败！");
				}
			}
		}

		int HomeResult;
		CString str;
		HomeResult=theApp.m_V9Control.V9C_Home(K_ALL_AXIS);
		str.Format("V9C_Home:%d",HomeResult);
		g_pFrm->m_CmdRun.AddMsg(str);
		if(HomeResult<0)
		{
			str.Format("第%d个控制器回零失败",-HomeResult);
			g_pFrm->m_CmdRun.Alarm(-HomeResult+9);
			AfxMessageBox(str);	
		}
		else if (HomeResult>0)
		{
			str.Format("第%d个控制器关阀失败",HomeResult);
			g_pFrm->m_CmdRun.Alarm(HomeResult+1);
			AfxMessageBox(str);	
		}
		else if (HomeResult==0)
		{
			theApp.m_tSysParam.V9HomeSu=true;
		}
		if (theApp.m_tSysParam.ThreeAxHomeSu&&theApp.m_tSysParam.V9HomeSu)
		{
			g_pFrm->m_CmdRun.m_tStatus = K_RUN_STS_STOP;
			g_pFrm->m_CmdRun.m_LastStatus = K_RUN_STS_STOP;
			g_pFrm->m_CmdRun.RestDisP();
		}
		if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
		{
			AfxMessageBox("设置电机参数错误！");
		}
		if(!theApp.m_V9Control.V9C_SetProfile())
		{
			AfxMessageBox("设置阀体参数错误！");
		}
	}
	//////////////////////////////////////////////////////////////////////////
	for (int i=0;i<3;i++)//COM1 COM2 COM3本地用，COM4 MES插件用
	{
		char parity = 'N';
		int nBaud = theApp.m_tSysParam.nBaud[i];
		int nBytes = theApp.m_tSysParam.nBytes[i];
		if(1==theApp.m_tSysParam.nParity[i]) //基校验
		{
			parity = 'O';
		}
		else if(2==theApp.m_tSysParam.nParity[i])//偶校验
		{
			parity = 'E';
		}
		else//无校验
		{
			parity = 'N';
		}

		if(0 == m_Serial.m_Port[i].m_iComState)
		{
			if(m_Serial.m_Port[i].InitPort(m_pMainWnd,i+1,nBaud,parity,
				nBytes,1,EV_RXCHAR,4096))
			{
				m_Serial.m_Port[i].m_iComState = 1;
				strTemp.Format("串口:COM%d初始化成功!",i+1);
				g_pFrm->m_CmdRun.AddMsg(strTemp);
				m_Serial.m_Port[i].m_chnum=0;
				memset(m_Serial.m_Port[i].m_chbuf,0,1024);
				if(m_Serial.m_Port[i].StartMonitoring())
				{
					m_Serial.m_Port[i].m_iComState = 2;
				}
			}
			else
			{
				m_Serial.m_Port[i].m_iComState = -1;
				strTemp.Format("串口:COM%d初始化失败!",i+1);
				g_pFrm->m_CmdRun.AddMsg(strTemp);
			}
		}
	}

	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		if(!theApp.m_Serial.ConnectPlc(strTemp))
		{
			AfxMessageBox("PLC连接失败！");
		}
		else
		{
			if (g_pFrm->m_CmdRun.ProductParam.HeatWhenStart>0)//启用开机加热
			{
				Sleep(1000);
				HeatSystemOn();
			}
		}
	}

	((CMainFrame*)m_pMainWnd)->SetWindowTextEx();
	m_bComOpen = FALSE;
	if(theApp.m_tSysParam.V9HomeSu&&theApp.m_tSysParam.ThreeAxHomeSu)
	{
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outReady,TRUE);
	}
	else
	{
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outReady,FALSE);
		strTemp.Format("ThreeAxHomeSu:%d,V9HomeSu:%d",theApp.m_tSysParam.ThreeAxHomeSu,theApp.m_tSysParam.V9HomeSu);
		g_pFrm->m_CmdRun.AddMsg(strTemp);
	}
	g_pView->m_ImgStatic.ImgSetParam(theApp.m_tSysParam.CameraExposureTime,theApp.m_tSysParam.CameraGain);//初始化曝光 增益
	g_pFrm->m_CmdRun.InitPadDetect();
	milApp.SetMatchMode(g_pFrm->m_CmdRun.ProductParam.matchMode);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outDetectLight,FALSE);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outNeedleLight,FALSE);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,TRUE);
	theApp.m_Mv400.SetOutput(17,TRUE);//开机即开启，NO CONTROL 
	//theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outDayLight,FALSE);
	g_pFrm->m_wndPosInfo.UpdateArrangeTime();//更新胶水预约相关设置；
	m_bInitOk = true;
	return TRUE;
}
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CTSCtrlSysApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CTSCtrlSysApp 自定义加载/保存方法
void CTSCtrlSysApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CTSCtrlSysApp::LoadCustomState()
{
}

void CTSCtrlSysApp::SaveCustomState()
{
}

// 读取/保存系统参数
void CTSCtrlSysApp::SysParam(BOOL bRead)
{
	CString strFile = CFunction::GetDirectory() + "system\\sysparam.dat";
	CString strTemp, strKey;
	int i=0;

	for(i=0;i<4;i++)
	{
		strTemp.Format("Port%d",i);
		CFunction::HandleInt(bRead, strTemp, "BAUD", m_tSysParam.nBaud[i], strFile);
		CFunction::HandleInt(bRead, strTemp, "BYTES", m_tSysParam.nBytes[i], strFile);
		CFunction::HandleInt(bRead, strTemp, "PARITY", m_tSysParam.nParity[i], strFile);
	}
	CFunction::HandleString(bRead, "APPLICATION", "NAME", m_tSysParam.szApp, strFile);
	for(i=0; i<12; i++)
	{
		strKey.Format("SCALE%d",i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].scale, strFile);

		strKey.Format("MAXPOS%d",i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].maxpos, strFile);

		strKey.Format("MINPOS%d", i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].minpos, strFile);

		strKey.Format("MAXVEL%d",i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].maxVel, strFile);

		strKey.Format("LOWHOMEVEL%d", i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].lowHomeVel, strFile);

		strKey.Format("HIGHHOMEVEL%d", i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].highHomeVel, strFile);

		strKey.Format("HOMEACC%d", i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].homeAcc, strFile);

		strKey.Format("LOWACC%d", i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].acc[0], strFile);

		strKey.Format("LOWVEL%d",i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].vel[0], strFile);

		strKey.Format("LOWJERK%d", i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].jerk[0], strFile);

		strKey.Format("LOWDEC%d",i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].dec[0], strFile);

		strKey.Format("LOWSTARTVEL%d",i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].velStart[0], strFile);

		strKey.Format("MEDIUMACC%d", i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].acc[1], strFile);

		strKey.Format("MEDIUMVEL%d",i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].vel[1], strFile);

		strKey.Format("MEDIUMJERK%d", i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].jerk[1], strFile);

		strKey.Format("MEDIUMDEC%d",i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].dec[1], strFile);

		strKey.Format("MEDIUMSTARTVEL%d",i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].velStart[1], strFile);

		strKey.Format("HIGHACC%d", i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].acc[2], strFile);

		strKey.Format("HIGHVEL%d",i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].vel[2], strFile);

		strKey.Format("HIGHJERK%d", i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].jerk[2], strFile);

		strKey.Format("HIGHDEC%d",i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].dec[2], strFile);

		strKey.Format("HIGHSTARTVEL%d",i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].velStart[2], strFile);

		strKey.Format("LIFTACC%d", i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].acc[3], strFile);

		strKey.Format("LIFTVEL%d",i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].vel[3], strFile);

		strKey.Format("LIFTJERK%d", i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].jerk[3], strFile);

		strKey.Format("LIFTDEC%d",i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].dec[3], strFile);

		strKey.Format("LIFTSTARTVEL%d",i);
		CFunction::HandleDouble(bRead, "SYSTEM", strKey, m_tSysParam.tAxis[i].velStart[3], strFile);
	}
	CFunction::HandleString(bRead, "SYSTEM", "OUTPORT",m_tSysParam.szOutputPortDisp, strFile);
	CFunction::HandleString(bRead, "SYSTEM", "INPORT",m_tSysParam.szInputPortButton, strFile);
	CFunction::HandleInt(bRead, "SYSTEM", "X_DIR",m_tSysParam.iXDir, strFile);
	CFunction::HandleInt(bRead, "SYSTEM", "Y_DIR",m_tSysParam.iYDir, strFile);
	CFunction::HandleInt(bRead, "SYSTEM", "Z_DIR",m_tSysParam.iZDir, strFile);
	CFunction::HandleInt(bRead, "CONFIGURE", "ContactSensorType",(INT &)m_tSysParam.sensorType, strFile);
	CFunction::HandleInt(bRead, "CONFIGURE", "ValveNum",(INT &)m_tSysParam.valveNum, strFile);
	CFunction::HandleInt(bRead, "CONFIGURE", "PlcType",(INT &)m_tSysParam.PlcType, strFile);
	CFunction::HandleInt(bRead, "CONFIGURE", "ValveType",(INT &)m_tSysParam.ValveType, strFile);
	CFunction::HandleInt(bRead, "CONFIGURE", "HeightSensorType",(INT &)m_tSysParam.heightSensorType, strFile);
	CFunction::HandleInt(bRead, "CONFIGURE", "DetectCameraType",(INT &)m_tSysParam.detectCameraType, strFile);

	for(i=0; i<3; i++)
	{
		strKey.Format("EditSpeedXY%d", i);
		CFunction::HandleDouble(bRead, "SPEED", strKey, m_tSysParam.dVelEditXY[i], strFile);
		strKey.Format("EditSpeedZ%d", i);
		CFunction::HandleDouble(bRead, "SPEED", strKey, m_tSysParam.dVelEditZ[i], strFile);
	}

	for(i=0; i<5; i++)
	{
		strKey.Format("VALUE%d",i);
		CFunction::HandleDouble(bRead, "LOOK", strKey, m_tSysParam.dLook[i], strFile);
	}

	CFunction::HandleInt(bRead, "LOOK", "NUM", m_tSysParam.iLookNum, strFile);
	m_tSysParam.GetPortNo();
	
	if(bRead)
	{
		CString strProject('\0',256);
		::GetPrivateProfileString("Project", "Project Name", "", (LPSTR)(LPCSTR)strProject, 256, strFile);
		if(strProject == _T("project331"))
		{
			g_currentProject = Project_331;
		}
		else
		{
			g_currentProject = Project_COMMON;
		}
	}
	for(i=0; i<5; i++)
	{
		strKey.Format("VALUE%d",i);
		CFunction::HandleDouble(bRead, "CleanPosition", strKey, m_tSysParam.CleanPosition[i], strFile);
		CFunction::HandleDouble(bRead, "TestDotPostion", strKey, m_tSysParam.TestDotPostion[i], strFile);
		CFunction::HandleDouble(bRead, "DefoamingPostion", strKey, m_tSysParam.DischargeGluePostion[i], strFile);
	}
	CFunction::HandleInt(bRead, "NeedleTime", "VALUE", m_tSysParam.NeedleTime, strFile);
	CFunction::HandleDouble(bRead, "PixelScale", "ScaleX", m_tSysParam.dPixelScaleX, strFile);
	CFunction::HandleDouble(bRead, "PixelScale", "ScaleY", m_tSysParam.dPixelScaleY, strFile);

	///////////////////////////////////V9参数////////////////////////////////////////////////////////////////
	CFunction::HandleDouble(bRead, "控制器参数", "阀栓直径", m_tSysParam.EmbolisD, strFile);
	CFunction::HandleDouble(bRead, "控制器参数", "最大行程", m_tSysParam.DisEndDistance, strFile);
	CFunction::HandleDouble(bRead, "控制器参数", "空闲行程", m_tSysParam.DispFreeDistance, strFile);
	CFunction::HandleDouble(bRead, "控制器参数", "最大速度", m_tSysParam.DispMaxSpeed,strFile);
	CFunction::HandleDouble(bRead, "控制器参数", "最大加速度", m_tSysParam.DispMaxAcc, strFile);
	CFunction::HandleDouble(bRead,"伺服电机参数","最大速度",m_tSysParam.MotorMaxSpeed,strFile);
	CFunction::HandleDouble(bRead,"伺服电机参数","最大加速度",m_tSysParam.MotorMaxAcc,strFile);
	CFunction::HandleLong(bRead, "STAT", "Capacity", theApp.m_tSysParam.StatCapacity, strFile);

	if (bRead)
	{
		m_tSysParam.Dispratios=PI*m_tSysParam.EmbolisD*m_tSysParam.EmbolisD/(2*2);//1mm行程对应的多少UL胶量
	}
	CFunction::HandleInt(bRead,"Substrate","Type",m_tSysParam.nSubstrateType,strFile);
	CFunction::HandleDouble(bRead, "CEMERADATA", "FOCUSZPOS",m_tSysParam.dFocusZPos, strFile);
	CFunction::HandleInt(bRead,"LOGFILE","WRITELOG",m_tSysParam.iIsLog,strFile);
	CFunction::HandleInt(bRead,"TimeSet","LOGOUT",m_tSysParam.iTimeLogOut,strFile);//自动注销时间。
	CFunction::HandleDouble(bRead, "INITPOS", "MOTORA",m_tSysParam.MotorAInitPos, strFile);
	CFunction::HandleDouble(bRead, "INITPOS", "MOTORC",m_tSysParam.MotorCInitPos, strFile);
	CFunction::HandleInt(bRead,"CAMNO","MARK",m_tSysParam.nCamMarkNo,strFile);
	CFunction::HandleInt(bRead,"CAMNO","NEEDLE",m_tSysParam.nCamNeedleNo,strFile);
	CFunction::HandleString(bRead, "REMIND", "LAST CLEAN",m_tSysParam.szLastCleanTime, strFile);
	CFunction::HandleString(bRead, "REG", "CODE",m_tSysParam.RegCode, strFile);
}
void CTSCtrlSysApp::ProductParam(BOOL bRead)
{
	CString strFile = CFunction::GetDirectory() + "system\\ProductParam.dat";
	CString strKey;
	int tempVal=0;

	CFunction::HandleInt(bRead, "DISPENSINGPATH", "FirstDispRow", g_pFrm->m_CmdRun.ProductParam.FirstDispRow, strFile);
	CFunction::HandleInt(bRead, "DISPENSINGPATH", "FirstDispColumn", g_pFrm->m_CmdRun.ProductParam.FirstDispColumn, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGPATH", "FirstDispRowDistance", g_pFrm->m_CmdRun.ProductParam.FirstDispRowD, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGPATH", "FirstDispColumnDistance", g_pFrm->m_CmdRun.ProductParam.FirstDispColumnD, strFile);
	CFunction::HandleInt(bRead, "DISPENSINGPATH", "SecondDispRow", g_pFrm->m_CmdRun.ProductParam.SecondDispRow, strFile);
	CFunction::HandleInt(bRead, "DISPENSINGPATH", "SecondDispColumn", g_pFrm->m_CmdRun.ProductParam.SecondDispColumn, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGPATH", "SecondDispRowDistance", g_pFrm->m_CmdRun.ProductParam.SecondDispRowD, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGPATH", "SecondDispColumnDistance", g_pFrm->m_CmdRun.ProductParam.SecondDispColumnD, strFile);
	CFunction::HandleInt(bRead, "DISPENSINGPATH", "ThirdDispRow", g_pFrm->m_CmdRun.ProductParam.ThirdDispRow, strFile);
	CFunction::HandleInt(bRead, "DISPENSINGPATH", "ThirdDispColumn", g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGPATH", "ThirdDispRowDistance", g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGPATH", "ThirdDispColumnDistance", g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD, strFile);
	CFunction::HandleInt(bRead, "DISPENSINGPATH", "IrregularRowNumber", g_pFrm->m_CmdRun.ProductParam.IrregularRow, strFile);
	CFunction::HandleInt(bRead, "DISPENSINGPATH", "IrregularColumnNumber", g_pFrm->m_CmdRun.ProductParam.IrregularColumn, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGPATH", "IrregularSlotLength", g_pFrm->m_CmdRun.ProductParam.IrregularSlotLength, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGPATH", "IrregularSlotWidth", g_pFrm->m_CmdRun.ProductParam.IrregularSlotWidth, strFile);
	CFunction::HandleInt(bRead, "DISPENSINGPATH", "MultiDispRow", g_pFrm->m_CmdRun.ProductParam.MultiDispRow, strFile);
	CFunction::HandleInt(bRead, "DISPENSINGPATH", "MultiDispColumn", g_pFrm->m_CmdRun.ProductParam.MultiDispColumn, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGPATH", "MultiDispRowDistance", g_pFrm->m_CmdRun.ProductParam.MultiDispRowD, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGPATH", "MultiDispColumnDistance", g_pFrm->m_CmdRun.ProductParam.MultiDispColumnD, strFile);

	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "DisPreTime", g_pFrm->m_CmdRun.ProductParam.dPreTime, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "DisPostTime", g_pFrm->m_CmdRun.ProductParam.dPostDisTime, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "DisVelXY", g_pFrm->m_CmdRun.ProductParam.dDispVelXY, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "DisAccXY", g_pFrm->m_CmdRun.ProductParam.dDispAccXY, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "ZUpHeight", g_pFrm->m_CmdRun.ProductParam.dZupHigh, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "DisPausePos", g_pFrm->m_CmdRun.ProductParam.dPauseHeight, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "DisPauseTime", g_pFrm->m_CmdRun.ProductParam.dPauseTime, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "DisFirstDelay", g_pFrm->m_CmdRun.ProductParam.dFirstDispDelay, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "FreeVel", g_pFrm->m_CmdRun.ProductParam.FreeVal, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "FreeAcc", g_pFrm->m_CmdRun.ProductParam.FreeAcc, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "CircleVel", g_pFrm->m_CmdRun.ProductParam.CircleVel, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "CircleAcc", g_pFrm->m_CmdRun.ProductParam.CircleAcc, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "LineVel", g_pFrm->m_CmdRun.ProductParam.LineVel, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "LineAcc", g_pFrm->m_CmdRun.ProductParam.LineAcc, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "LineLength", g_pFrm->m_CmdRun.ProductParam.LineLength, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "RectWidth", g_pFrm->m_CmdRun.ProductParam.RectWidth, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "RectLength", g_pFrm->m_CmdRun.ProductParam.RectLength, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "DispStartPosX", g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[0], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "DispStartPosY", g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[1], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "DispStartPosZA", g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[2], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "DispStartPosZB", g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[3], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "DispStartPosZC", g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[4], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "DispRadius", g_pFrm->m_CmdRun.ProductParam.dDispRadius, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "DispDistance", g_pFrm->m_CmdRun.ProductParam.dDistance, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "GrabImageDelay", g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "CompAngle", g_pFrm->m_CmdRun.ProductParam.dCompAngle, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "CleanDelay", g_pFrm->m_CmdRun.ProductParam.dCleanDelay, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "DisVelZ", g_pFrm->m_CmdRun.ProductParam.dDispVelZ, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "DisAccZ", g_pFrm->m_CmdRun.ProductParam.dDispAccZ, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "TestDelay", g_pFrm->m_CmdRun.ProductParam.dTestDotDelay, strFile);
	CFunction::HandleInt(bRead, "DISPENSINGTECH", "TestDotNumber", g_pFrm->m_CmdRun.ProductParam.TestDotNumber, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "OffsetX", g_pFrm->m_CmdRun.ProductParam.DispOffsetX, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "OffsetY", g_pFrm->m_CmdRun.ProductParam.DispOffsetY, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "MaxOffsetAngle", g_pFrm->m_CmdRun.ProductParam.MaxOffsetAngle, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "MaxOffsetX", g_pFrm->m_CmdRun.ProductParam.MaxOffsetX, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "MaxOffsetY", g_pFrm->m_CmdRun.ProductParam.MaxOffsetY, strFile);
	CFunction::HandleInt(bRead, "DISPENSINGTECH", "NLackPadCmpValue", g_pFrm->m_CmdRun.ProductParam.nLackPadCmpValue, strFile);
	

	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "NeedleFirstPosX", g_pFrm->m_CmdRun.ProductParam.NeedlePos[0], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "NeedleFirstPosY", g_pFrm->m_CmdRun.ProductParam.NeedlePos[1], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "NeedleFirstPosZA", g_pFrm->m_CmdRun.ProductParam.NeedlePos[2], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "NeedleFirstPosZB", g_pFrm->m_CmdRun.ProductParam.NeedlePos[3], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "NeedleFirstPosZC", g_pFrm->m_CmdRun.ProductParam.NeedlePos[4], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "NeedleVel", g_pFrm->m_CmdRun.ProductParam.NeedleVel, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "NeedleAcc", g_pFrm->m_CmdRun.ProductParam.NeedleAcc, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "NeedlePitch", g_pFrm->m_CmdRun.ProductParam.NeedleGap, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "CleanPitch", g_pFrm->m_CmdRun.ProductParam.CleanGap, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "NeedleAngleOne", g_pFrm->m_CmdRun.ProductParam.AdjustNeedleAngle[0], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "NeedleAngleTwo", g_pFrm->m_CmdRun.ProductParam.AdjustNeedleAngle[1], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "HeightSensorPosX", g_pFrm->m_CmdRun.ProductParam.HeightSensorPos[0], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "HeightSensorPosY", g_pFrm->m_CmdRun.ProductParam.HeightSensorPos[1], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "HeightSensorPosZ", g_pFrm->m_CmdRun.ProductParam.HeightSensorPos[2], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "ContactStandardBug", g_pFrm->m_CmdRun.ProductParam.HeightStandardValue, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "ContactSensorPosX", g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[0], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "ContactSensorPosY", g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[1], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "ContactSensorPosZA", g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "ContactSensorPosZB", g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[3], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "ContactSensorPosZC", g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[4], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "ContactMeasureVel", g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "ContactMeasureAcc", g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "HeightGlueVol", g_pFrm->m_CmdRun.ProductParam.HeightGlueVol, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "ContactStandard", g_pFrm->m_CmdRun.ProductParam.ContactStandardValue, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "ImageDispensePosX", g_pFrm->m_CmdRun.ProductParam.ImageDispensePosition[0], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "ImageDispensePosY", g_pFrm->m_CmdRun.ProductParam.ImageDispensePosition[1], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "ImageDispensePosZA", g_pFrm->m_CmdRun.ProductParam.ImageDispensePosition[2], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "HeightDispensePosX", g_pFrm->m_CmdRun.ProductParam.HeightDispensePosition[0], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "HeightDispensePosY", g_pFrm->m_CmdRun.ProductParam.HeightDispensePosition[1], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "HeightDispensePosZA", g_pFrm->m_CmdRun.ProductParam.HeightDispensePosition[2], strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "SlotDetectPosX", g_pFrm->m_CmdRun.ProductParam.tSlotDetectPos.x, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "SlotDetectPosY", g_pFrm->m_CmdRun.ProductParam.tSlotDetectPos.y, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "SlotDetectPosZA", g_pFrm->m_CmdRun.ProductParam.tSlotDetectPos.za, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "PadDetectPosX", g_pFrm->m_CmdRun.ProductParam.tPadDetectPos.x, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "PadDetectPosY", g_pFrm->m_CmdRun.ProductParam.tPadDetectPos.y, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "PadDetectPosZA", g_pFrm->m_CmdRun.ProductParam.tPadDetectPos.za, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "CustomLabelPosX", g_pFrm->m_CmdRun.ProductParam.tgCustomLabelPos.x, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "CustomLabelPosY", g_pFrm->m_CmdRun.ProductParam.tgCustomLabelPos.y, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "CustomLabelPosZA", g_pFrm->m_CmdRun.ProductParam.tgCustomLabelPos.za, strFile);
	CFunction::HandleInt(bRead, "DISPENSINGTECH", "HeatOne", g_pFrm->m_CmdRun.ProductParam.HeatOne, strFile);
	CFunction::HandleInt(bRead, "DISPENSINGTECH", "HeatTwo", g_pFrm->m_CmdRun.ProductParam.HeatTwo, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "ScanDelay", g_pFrm->m_CmdRun.ProductParam.ScanDelay, strFile);
	CFunction::HandleInt(bRead, "DISPENSINGTECH", "ImageNumber", g_pFrm->m_CmdRun.ProductParam.nImageNum, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "MeasureHeightDelay", g_pFrm->m_CmdRun.ProductParam.dHeightDelay, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "MeasureHeightError", g_pFrm->m_CmdRun.ProductParam.dHeightError, strFile);
	CFunction::HandleInt(bRead, "DISPENSINGTECH", "MeasureHeightNumber", g_pFrm->m_CmdRun.ProductParam.HeightNumber, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "DispenseHeight", g_pFrm->m_CmdRun.ProductParam.dDispenseHeight, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "TempCheckError", g_pFrm->m_CmdRun.ProductParam.dTempError, strFile);
	CFunction::HandleDouble(bRead, "DISPENSINGTECH", "HeatSetting", g_pFrm->m_CmdRun.ProductParam.dHeatSetting, strFile);

	for(int i=0;i<3;i++)
	{
		strKey.Format("ImagePosition%d",i);
		CFunction::HandleDouble(bRead, strKey, "PosX", g_pFrm->m_CmdRun.ProductParam.tgImagePos[i].x, strFile);
		CFunction::HandleDouble(bRead, strKey, "PosY", g_pFrm->m_CmdRun.ProductParam.tgImagePos[i].y, strFile);
		CFunction::HandleDouble(bRead, strKey, "PosZA", g_pFrm->m_CmdRun.ProductParam.tgImagePos[i].za, strFile);//0630;Z + Cemera;
	}

	for(int i=0;i<20;i++)
	{
		strKey.Format("IrregularRowDis%d",i);
		CFunction::HandleDouble(bRead, "DISPENSINGPATH", strKey, g_pFrm->m_CmdRun.ProductParam.IrregularRowD[i], strFile);
	}

	for(int i=0;i<50;i++)
	{
		strKey.Format("IrregularColumnDis%d",i);
		CFunction::HandleDouble(bRead, "DISPENSINGPATH", strKey, g_pFrm->m_CmdRun.ProductParam.IrregularColumnD[i], strKey);
	}

	for(int i=0;i<3;i++)
	{
		switch(i)
		{
		case 0:
			strKey = "SelectValveA";
			break;
		case 1:
			strKey = "SelectValveB";
			break;
		case 2:
			strKey = "SelectValveC";
			break;
		default:
			break;
		}
		if(bRead)
		{
			CFunction::HandleInt(bRead, "VALVE", strKey, tempVal, strFile);
			if(tempVal==1)
			{
				g_pFrm->m_CmdRun.ProductParam.FSelectVal[i] = true;
			}
			else
			{
				g_pFrm->m_CmdRun.ProductParam.FSelectVal[i] = false;
			}
		}
		else
		{
			if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
			{
				tempVal = 1;
			}
			else
			{
				tempVal = 0;
			}
			CFunction::HandleInt(bRead, "VALVE", strKey, tempVal, strFile);
		}
	}
	CFunction::HandleInt(bRead, "VALVE", "DisValveCount", g_pFrm->m_CmdRun.ProductParam.DispFCount, strFile);

	CFunction::HandleInt(bRead, "FUCTION", "DisMode", g_pFrm->m_CmdRun.ProductParam.DispMode, strFile);
	CFunction::HandleInt(bRead,"FUNCTION","MESOFFLINE",g_pFrm->m_CmdRun.ProductParam.bMesOffline,strFile);
	CFunction::HandleInt(bRead, "FUCTION", "CleanNeedle", g_pFrm->m_CmdRun.ProductParam.CleanNd, strFile);
	CFunction::HandleInt(bRead, "FUCTION", "VisionMode", (INT &)g_pFrm->m_CmdRun.ProductParam.visionMode, strFile);
	CFunction::HandleInt(bRead, "FUCTION", "FillMode", (INT &)g_pFrm->m_CmdRun.ProductParam.fillMode, strFile);
	CFunction::HandleInt(bRead, "FUCTION", "InspectMode", (INT &)g_pFrm->m_CmdRun.ProductParam.inspectMode, strFile);
	CFunction::HandleInt(bRead, "FUCTION", "Orientation", (INT &)g_pFrm->m_CmdRun.ProductParam.dispOrientation, strFile);
	CFunction::HandleInt(bRead, "FUCTION", "FirstTest", g_pFrm->m_CmdRun.ProductParam.FirstTest, strFile);
	CFunction::HandleInt(bRead, "FUCTION", "SlotDetect", g_pFrm->m_CmdRun.ProductParam.SlotDetect, strFile);
	CFunction::HandleInt(bRead, "FUCTION", "FullTest", g_pFrm->m_CmdRun.ProductParam.FullTestDot, strFile);
	CFunction::HandleInt(bRead, "FUCTION", "IrregularMatrix", g_pFrm->m_CmdRun.ProductParam.IrregularMatrix, strFile);
	CFunction::HandleInt(bRead, "FUCTION", "ValveSelect", (INT &)g_pFrm->m_CmdRun.ProductParam.valveSelect, strFile);
	CFunction::HandleInt(bRead, "FUCTION", "FullClean", g_pFrm->m_CmdRun.ProductParam.FullClean, strFile);
	CFunction::HandleInt(bRead, "FUCTION", "DoorSwitch", g_pFrm->m_CmdRun.ProductParam.DoorSwitch, strFile);
	CFunction::HandleInt(bRead, "FUCTION", "DispFinishFill", g_pFrm->m_CmdRun.ProductParam.DispFinishFill, strFile);
	CFunction::HandleInt(bRead, "FUCTION", "CleanDischarge", g_pFrm->m_CmdRun.ProductParam.CleanDischarge, strFile);
	CFunction::HandleInt(bRead, "FUCTION", "PauseCleanTest", g_pFrm->m_CmdRun.ProductParam.PauseCleanTest, strFile);
	CFunction::HandleInt(bRead, "FUCTION", "MeasureHeightEnable", g_pFrm->m_CmdRun.ProductParam.MeasureHeightEnable, strFile);
	CFunction::HandleInt(bRead, "FUCTION", "Sequence", (INT &)g_pFrm->m_CmdRun.ProductParam.dispSequence, strFile);
	CFunction::HandleInt(bRead, "FUCTION", "MatchMode", (INT &)g_pFrm->m_CmdRun.ProductParam.matchMode, strFile);
	CFunction::HandleInt(bRead, "FUCTION", "AutoAdjust", g_pFrm->m_CmdRun.ProductParam.AutoAdjust, strFile);
    CFunction::HandleInt(bRead, "FUCTION", "TempJudgeBeforeStart", g_pFrm->m_CmdRun.ProductParam.TempJudgeBeforeStart, strFile);
	CFunction::HandleInt(bRead,"FUNCTION","HeatWhenStart",g_pFrm->m_CmdRun.ProductParam.HeatWhenStart,strFile);
	CFunction::HandleInt(bRead, "Path", "ListElementNum", g_pFrm->m_CmdRun.ProductParam.ListElementNum, strFile);
	if(bRead)
	{
		g_pFrm->m_rawList.RemoveAll();
	}
	for(int i=0;i<g_pFrm->m_CmdRun.ProductParam.ListElementNum;i++)
	{
		tgPos tgTempPos;
		if(bRead)
		{
			strKey.Format("Point%d_x",i);
			CFunction::HandleDouble(bRead, "Path", strKey,tgTempPos.x,strFile);
			strKey.Format("Point%d_y",i);
			CFunction::HandleDouble(bRead, "Path", strKey,tgTempPos.y,strFile);
			strKey.Format("Point%d_za",i);
			CFunction::HandleDouble(bRead, "Path", strKey,tgTempPos.za,strFile);
			strKey.Format("Point%d_zb",i);
			CFunction::HandleDouble(bRead, "Path", strKey,tgTempPos.zb,strFile);
			strKey.Format("Point%d_zc",i);
			CFunction::HandleDouble(bRead, "Path", strKey,tgTempPos.zb,strFile);
			g_pFrm->m_rawList.AddTail(tgTempPos);
		}
		else
		{
			tgTempPos = g_pFrm->m_rawList.GetAt(g_pFrm->m_rawList.FindIndex(i));
			strKey.Format("Point%d_x",i);
			CFunction::HandleDouble(bRead, "Path", strKey,tgTempPos.x,strFile);
			strKey.Format("Point%d_y",i);
			CFunction::HandleDouble(bRead, "Path", strKey,tgTempPos.y,strFile);
			strKey.Format("Point%d_za",i);
			CFunction::HandleDouble(bRead, "Path", strKey,tgTempPos.za,strFile);
			strKey.Format("Point%d_zb",i);
			CFunction::HandleDouble(bRead, "Path", strKey,tgTempPos.zb,strFile);
			strKey.Format("Point%d_zc",i);
			CFunction::HandleDouble(bRead, "Path", strKey,tgTempPos.zc,strFile);
		}
	}
	for(int i=0;i<10;i++)
	{
		strKey.Format("Point%d_x",i);
		CFunction::HandleDouble(bRead, "TestDispPos", strKey,g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].x,strFile);
		strKey.Format("Point%d_y",i);
		CFunction::HandleDouble(bRead, "TestDispPos", strKey,g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].y,strFile);
		strKey.Format("Point%d_za",i);
		CFunction::HandleDouble(bRead, "TestDispPos", strKey,g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].za,strFile);
		strKey.Format("Point%d_zb",i);
		CFunction::HandleDouble(bRead, "TestDispPos", strKey,g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].zb,strFile);
		strKey.Format("Point%d_zc",i);
		CFunction::HandleDouble(bRead, "TestDispPos", strKey,g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].zc,strFile);

		strKey.Format("Point%d_x",i);
		CFunction::HandleDouble(bRead, "MeasureHeightPos", strKey,g_pFrm->m_CmdRun.ProductParam.tgMeasureHeightPos[i].x,strFile);
		strKey.Format("Point%d_y",i);
		CFunction::HandleDouble(bRead, "MeasureHeightPos", strKey,g_pFrm->m_CmdRun.ProductParam.tgMeasureHeightPos[i].y,strFile);
		strKey.Format("Point%d_za",i);
		CFunction::HandleDouble(bRead, "MeasureHeightPos", strKey,g_pFrm->m_CmdRun.ProductParam.tgMeasureHeightPos[i].za,strFile);//0630 :调整激光工作距离；
		
		strKey.Format("Point%d",i);
		CFunction::HandleDouble(bRead,"LaserBaseData",strKey,g_pFrm->m_CmdRun.ProductParam.dLaserDataBase[i],strFile);
	}

	for(int i=0;i<6;i++)
	{
		strKey.Format("Point%d_x",i);
		CFunction::HandleDouble(bRead, "LABELPOS", strKey,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[i].x,strFile);
		strKey.Format("Point%d_y",i);
		CFunction::HandleDouble(bRead, "LABELPOS", strKey,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[i].y,strFile);
		strKey.Format("Point%d_za",i);
		CFunction::HandleDouble(bRead, "LABELPOS", strKey,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[i].za,strFile);
		strKey.Format("Point%d_zb",i);
		CFunction::HandleDouble(bRead, "LABELPOS", strKey,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[i].zb,strFile);
		strKey.Format("Point%d_zc",i);
		CFunction::HandleDouble(bRead, "LABELPOS", strKey,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[i].zc,strFile);
	}

	//标定补偿：
	CFunction::HandleDouble(bRead, "LABLEOFFSET", "2X", g_pFrm->m_CmdRun.ProductParam.offset2x, strFile);
	CFunction::HandleDouble(bRead, "LABLEOFFSET", "2Y", g_pFrm->m_CmdRun.ProductParam.offset2y, strFile);
	CFunction::HandleDouble(bRead, "LABLEOFFSET", "3X", g_pFrm->m_CmdRun.ProductParam.offset3x, strFile);
	CFunction::HandleDouble(bRead, "LABLEOFFSET", "3Y", g_pFrm->m_CmdRun.ProductParam.offset3y, strFile);
    
	CFunction::HandleDouble(bRead,"COMPOSEDATA","X2",g_pFrm->m_CmdRun.ProductParam.dComposeSecondX,strFile);
	CFunction::HandleDouble(bRead,"COMPOSEDATA","X3",g_pFrm->m_CmdRun.ProductParam.dComposedThirdX,strFile);
	CFunction::HandleDouble(bRead,"COMPOSEDATA","Y2",g_pFrm->m_CmdRun.ProductParam.dComposedSecondY,strFile);
	CFunction::HandleDouble(bRead,"COMPOSEDATA","Y3",g_pFrm->m_CmdRun.ProductParam.dComposedThirdY,strFile);

	CFunction::HandleDouble(bRead, "AUTONEEDLESTEP", "PitchX", g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.dPitchX, strFile);
	CFunction::HandleDouble(bRead, "AUTONEEDLESTEP", "PitchY", g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.dPitchY, strFile);
	CFunction::HandleInt(bRead, "AUTONEEDLESTEP", "CountX", g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCountX, strFile);
	CFunction::HandleInt(bRead, "AUTONEEDLESTEP", "CountY", g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCountY, strFile);
	CFunction::HandleInt(bRead, "AUTONEEDLESTEP", "CurrentX", g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentX, strFile);
	CFunction::HandleInt(bRead, "AUTONEEDLESTEP", "CurrentY", g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentY, strFile);

	CFunction::HandleDouble(bRead, "AUTONEEDLEPRARAM", "GlueVol", g_pFrm->m_CmdRun.ProductParam.dAutoNeedleGlueVol, strFile);
	CFunction::HandleDouble(bRead, "AUTONEEDLEPRARAM", "ErrorX", g_pFrm->m_CmdRun.ProductParam.dAutoNeedleErrorX, strFile);
	CFunction::HandleDouble(bRead, "AUTONEEDLEPRARAM", "ErrorY", g_pFrm->m_CmdRun.ProductParam.dAutoNeedleErrorY, strFile);
	CFunction::HandleInt(bRead, "AUTONEEDLEPRARAM", "AdjustCount", g_pFrm->m_CmdRun.ProductParam.nAutoAdjustCount, strFile);
	CFunction::HandleInt(bRead, "AUTONEEDLEPARAM", "SearchMode", (INT &)g_pFrm->m_CmdRun.ProductParam.searchMode, strFile);
	CFunction::HandleInt(bRead, "AUTONEEDLEPRARAM", "Threshold", g_pFrm->m_CmdRun.ProductParam.nBlobThreshold, strFile);
	CFunction::HandleInt(bRead, "AUTONEEDLEPRARAM", "AreaMin", g_pFrm->m_CmdRun.ProductParam.nBlobAreaMin, strFile);
	CFunction::HandleInt(bRead, "AUTONEEDLEPRARAM", "AreaMax", g_pFrm->m_CmdRun.ProductParam.nBlobAreaMax, strFile);
	CFunction::HandleInt(bRead, "AUTONEEDLEPRARAM", "LengthMin", g_pFrm->m_CmdRun.ProductParam.nBlobLengthMin, strFile);
	CFunction::HandleInt(bRead, "AUTONEEDLEPRARAM", "LengthMax", g_pFrm->m_CmdRun.ProductParam.nBlobLengthMax, strFile);
	CFunction::HandleInt(bRead, "AUTONEEDLEPRARAM", "HeightMin", g_pFrm->m_CmdRun.ProductParam.nBlobHeightMin, strFile);
	CFunction::HandleInt(bRead, "AUTONEEDLEPRARAM", "HeightMax", g_pFrm->m_CmdRun.ProductParam.nBlobHeightMax, strFile);

	CFunction::HandleDouble(bRead,"LASERHEIGHTZa","LASERZA",g_pFrm->m_CmdRun.ProductParam.dNeedleLaserHeight,strFile);
	
	CFunction::HandleDouble(bRead,"ADJUSTMOTORPOS","MOTORA",g_pFrm->m_CmdRun.ProductParam.dAdjustMotorInRunA,strFile);
	CFunction::HandleDouble(bRead,"ADJUSTMOTORPOS","MOTORB",g_pFrm->m_CmdRun.ProductParam.dAdjustMotorInRunB,strFile);
	CFunction::HandleDouble(bRead,"ADJUSTMOTORPOS","MOTORC",g_pFrm->m_CmdRun.ProductParam.dAdjustMotorInRunC,strFile);
	CFunction::HandleDouble(bRead,"ADJUSTMOTORPOS","MOTORD",g_pFrm->m_CmdRun.ProductParam.dAdjustMotorInRunD,strFile);	

	CFunction::HandleDouble(bRead,"CAMERAONCONTACTOR","XPOSITION",g_pFrm->m_CmdRun.ProductParam.CameraOnContactor[0],strFile);
	CFunction::HandleDouble(bRead,"CAMERAONCONTACTOR","YPOSITION",g_pFrm->m_CmdRun.ProductParam.CameraOnContactor[1],strFile);

	CFunction::HandleDouble(bRead,"OFFSET","ZDISPOFFSETALL",g_pFrm->m_CmdRun.ProductParam.dZDispOffsetAll,strFile);	

	CFunction::HandleDouble(bRead, "ARRANGE", "ArrangeSpan", g_pFrm->m_CmdRun.ProductParam.ArrangeSpan, strFile);
	CFunction::HandleInt(bRead, "ARRANGE", "ArrangeEnable", g_pFrm->m_CmdRun.ProductParam.ArrangeEnable, strFile);
	CFunction::HandleLong(bRead, "ARRANGE", "ArrangTime", g_pFrm->m_CmdRun.ProductParam.ArrangeTime, strFile);
	CFunction::HandleDouble(bRead, "ValveMonitor", "MonitorSpan", g_pFrm->m_CmdRun.ProductParam.ValveMonitorSpan, strFile);
	CFunction::HandleInt(bRead, "ValveMonitor", "MonitorEnable", g_pFrm->m_CmdRun.ProductParam.ValveMonitorEnable, strFile);
	CFunction::HandleLong(bRead, "ValveMonitor", "MonitorTime", g_pFrm->m_CmdRun.ProductParam.ValveMonitorTime, strFile);
	CFunction::HandleLong(bRead, "GASKET", "GASKETLIFE", g_pFrm->m_CmdRun.ProductParam.lGasketRingLife, strFile);
	////
	CFunction::HandleInt(bRead, "REMIND", "remindClean", g_pFrm->m_CmdRun.ProductParam.bCleanRemind, strFile); 
	CFunction::HandleDouble(bRead, "REMIND", "remindCleanTime", g_pFrm->m_CmdRun.ProductParam.CleanRemindTimeHour, strFile);
    CFunction::HandleDouble(bRead, "DISPENSINGTECH", "HeightCompMAX", g_pFrm->m_CmdRun.ProductParam.dHeightCompMax, strFile);
	////接触测高参数
	CFunction::HandleInt(bRead, "DISPENSINGTECH", "NeedleHeightCheck",g_pFrm->m_CmdRun.ProductParam.NeedleHeightCheck,strFile);
	CFunction::HandleLong(bRead, "DISPENSINGTECH", "ContactFreq",g_pFrm->m_CmdRun.ProductParam.ContactFrequency,strFile);
	CFunction::HandleDouble(bRead,"DISPENSINGTECH", "ContactErrPermit",g_pFrm->m_CmdRun.ProductParam.ContactErrPermit,strFile);

	//UDP相关参数
	CFunction::HandleString(bRead, "UDPPARAM", "MACHINECODE",g_pFrm->m_CmdRun.ProductParam.MachineID,strFile);
	CFunction::HandleString(bRead, "UDPPARAM", "ServerURL",g_pFrm->m_CmdRun.ProductParam.ServerURL,strFile);
	CFunction::HandleInt(bRead, "UDPPARAM", "FCompanyCode",g_pFrm->m_CmdRun.ProductParam.FCompanyCode,strFile);
	CFunction::HandleInt(bRead, "UDPPARAM", "ServerPort",g_pFrm->m_CmdRun.ProductParam.ServerPort,strFile);
	CFunction::HandleString(bRead, "UDPPARAM", "UDPaddress",g_pFrm->m_CmdRun.ProductParam.ServerIP,strFile);

	CFunction::HandleDouble(bRead,"DISPENSINGTECH", "MESMIN",g_pFrm->m_CmdRun.ProductParam.MesDataMin,strFile);
	CFunction::HandleDouble(bRead,"DISPENSINGTECH", "MESMAX",g_pFrm->m_CmdRun.ProductParam.MesDataMax,strFile);
	CFunction::HandleString(bRead, "GLUEID", "GLUEID", g_pFrm->m_CmdRun.ProductParam.Save_rev_CodeID, strFile);

	CFunction::HandleDouble(bRead, "Temperature", "PreTemp", g_pFrm->m_CmdRun.ProductParam.dPreHeatTemp, strFile);
	CFunction::HandleDouble(bRead, "Temperature", "DispTemp", g_pFrm->m_CmdRun.ProductParam.dDispHeatTemp, strFile);
	CFunction::HandleDouble(bRead, "Temperature", "RealPreTemp", g_pFrm->m_CmdRun.ProductParam.dRealPreHeatTemp, strFile);
	CFunction::HandleDouble(bRead, "Temperature", "RealDispTemp", g_pFrm->m_CmdRun.ProductParam.dRealDispHeatTemp, strFile);

	CFunction::HandleInt(bRead, "Mark", "NOCOMPENSATE",g_pFrm->m_CmdRun.ProductParam.nCheckIsCompensate,strFile);
}
void CTSCtrlSysApp::V9Param(BOOL bRead)
{
	//
	CString strFile = CFunction::GetDirectory() + "system\\V9Param.dat";
	CString strTemp, strKey;
	for(int i=0;i<3;i++)
	{
		switch(i)
		{
		case 0:
			strTemp = "ValveA";
			break;
		case 1:
			strTemp = "ValveB";
			break;
		case 2:
			strTemp = "ValveC";
			break;
		}
		CFunction::HandleDouble(bRead, strTemp, "DispVol",g_pFrm->m_CmdRun.V9Parm[i].DispVolume, strFile);
		CFunction::HandleDouble(bRead, strTemp, "DispVel", g_pFrm->m_CmdRun.V9Parm[i].dispspeed, strFile);
		CFunction::HandleLong(bRead, strTemp, "DispCount",g_pFrm->m_CmdRun.V9Parm[i].DispCount, strFile);
		CFunction::HandleDouble(bRead, strTemp, "FillVel", g_pFrm->m_CmdRun.V9Parm[i].FullSpeed, strFile);
		CFunction::HandleDouble(bRead, strTemp, "CleanVel",g_pFrm->m_CmdRun.V9Parm[i].CleanSpeed, strFile);
		CFunction::HandleLong(bRead, strTemp, "CleanCount", g_pFrm->m_CmdRun.V9Parm[i].CleanCount, strFile);
		CFunction::HandleDouble(bRead, strTemp, "DispDistance", g_pFrm->m_CmdRun.V9Parm[i].DispDistance, strFile);
		CFunction::HandleDouble(bRead, strTemp, "ResetDistance", g_pFrm->m_CmdRun.V9Parm[i].RestDistance, strFile);
		CFunction::HandleDouble(bRead, strTemp, "BackVolumn", g_pFrm->m_CmdRun.V9Parm[i].BackVolume, strFile);
		CFunction::HandleDouble(bRead, strTemp, "BackVel", g_pFrm->m_CmdRun.V9Parm[i].BackSpeed, strFile);
		CFunction::HandleDouble(bRead, strTemp, "ForwardVol", g_pFrm->m_CmdRun.V9Parm[i].ForwardVol, strFile);
		CFunction::HandleDouble(bRead, strTemp, "FirstCompVol", g_pFrm->m_CmdRun.V9Parm[i].FirstCompVol, strFile);
		CFunction::HandleDouble(bRead, strTemp, "StartVel", g_pFrm->m_CmdRun.V9Parm[i].StartVel, strFile);
		CFunction::HandleDouble(bRead, strTemp, "TestVol", g_pFrm->m_CmdRun.V9Parm[i].TestVol, strFile);
		CFunction::HandleLong(bRead, strTemp, "DispNum", g_pFrm->m_CmdRun.V9Parm[i].DispNum, strFile);
		CFunction::HandleDouble(bRead,strTemp,"AllFullHeight",g_pFrm->m_CmdRun.V9Parm[i].dHeightFullALL,strFile);
		CFunction::HandleDouble(bRead, strTemp, "CompVolumn",g_pFrm->m_CmdRun.V9Parm[i].CompVolume, strFile);
	}
}

void CTSCtrlSysApp::BmpParam(BOOL bRead)
{
	CString strFile = CFunction::GetDirectory() + "system\\BmpParam.dat";
	CString strKey;

	CFunction::HandleDouble(bRead, "BmpCenter", "StartCenterX", theApp.m_tSysParam.BmpCenterX[0], strFile);
	CFunction::HandleDouble(bRead, "BmpCenter", "StartCenterY", theApp.m_tSysParam.BmpCenterY[0], strFile);
	CFunction::HandleDouble(bRead, "BmpCenter", "EndCenterX", theApp.m_tSysParam.BmpCenterX[1], strFile);
	CFunction::HandleDouble(bRead, "BmpCenter", "EndCenterY", theApp.m_tSysParam.BmpCenterY[1], strFile);

	CFunction::HandleLong(bRead, "BmpROI", "StartROILeft", theApp.m_tSysParam.BmpROI[0].left, strFile);
	CFunction::HandleLong(bRead, "BmpROI", "StartROIRight", theApp.m_tSysParam.BmpROI[0].right, strFile);
	CFunction::HandleLong(bRead, "BmpROI", "StartROITop", theApp.m_tSysParam.BmpROI[0].top, strFile);
	CFunction::HandleLong(bRead, "BmpROI", "StartROIBottom", theApp.m_tSysParam.BmpROI[0].bottom, strFile);
	CFunction::HandleLong(bRead, "BmpROI", "EndROILeft", theApp.m_tSysParam.BmpROI[1].left, strFile);
	CFunction::HandleLong(bRead, "BmpROI", "EndROIRight", theApp.m_tSysParam.BmpROI[1].right, strFile);
	CFunction::HandleLong(bRead, "BmpROI", "EndROITop", theApp.m_tSysParam.BmpROI[1].top, strFile);
	CFunction::HandleLong(bRead, "BmpROI", "EndROIBottom", theApp.m_tSysParam.BmpROI[1].bottom, strFile);

	CFunction::HandleInt(bRead, "BlobParam", "StandardLen", theApp.m_tSysParam.StandardLen, strFile);
	CFunction::HandleDouble(bRead, "BlobParam", "BmpScale", theApp.m_tSysParam.BmpScale, strFile);
	CFunction::HandleDouble(bRead, "BlobParam", "BmpWidthMin", theApp.m_tSysParam.BmpWidthMin, strFile);
	CFunction::HandleDouble(bRead, "BlobParam", "BmpWidthMax", theApp.m_tSysParam.BmpWidthMax, strFile);
	CFunction::HandleDouble(bRead, "BlobParam", "BmpHeightMin", theApp.m_tSysParam.BmpHeighMin, strFile);
	CFunction::HandleDouble(bRead, "BlobParam", "BmpHeightMax", theApp.m_tSysParam.BmpHeighMax, strFile);
	CFunction::HandleDouble(bRead, "BlobParam", "BmpAreaMin", theApp.m_tSysParam.BmpAreaMin, strFile);
	CFunction::HandleDouble(bRead, "BlobParam", "BmpAreaMax", theApp.m_tSysParam.BmpAreaMax, strFile);

	//CFunction::HandleDouble(bRead, "MarkParam", "BmpMarkScale", theApp.m_tSysParam.BmpMarkScale, strFile);
	CFunction::HandleDouble(bRead, "MarkParam", "BmpMarkAngle", theApp.m_tSysParam.BmpMarkAngle, strFile);
	CFunction::HandleDouble(bRead, "MarkParam", "BmpMarkAcceptance", theApp.m_tSysParam.BmpMarkAcceptance, strFile);

	//NEEDLE 
	CFunction::HandleDouble(bRead,"NeedleMarkParam","NeedleMarkAngle",theApp.m_tSysParam.BmpNeedleMarkAngle,strFile);
	CFunction::HandleDouble(bRead,"NeedleMarkParam","NeedleMarkAcceptance",theApp.m_tSysParam.BmpNeedleMarkAcceptance,strFile);

	CFunction::HandleLong(bRead, "BmpMarkROI", "ROILeft", theApp.m_tSysParam.BmpMarkROI.left, strFile);
	CFunction::HandleLong(bRead, "BmpMarkROI", "ROIRight", theApp.m_tSysParam.BmpMarkROI.right, strFile);
	CFunction::HandleLong(bRead, "BmpMarkROI", "ROITop", theApp.m_tSysParam.BmpMarkROI.top, strFile);
	CFunction::HandleLong(bRead, "BmpMarkROI", "ROIBottom", theApp.m_tSysParam.BmpMarkROI.bottom, strFile);

	CFunction::HandleLong(bRead,"BmpMarkRefCam","ROILeft",theApp.m_tSysParam.BmpMarkRefCamROI.left,strFile);
	CFunction::HandleLong(bRead,"BmpMarkRefCam","ROIRight",theApp.m_tSysParam.BmpMarkRefCamROI.right,strFile);
	CFunction::HandleLong(bRead,"BmpMarkRefCam","ROITop",theApp.m_tSysParam.BmpMarkRefCamROI.top,strFile);
	CFunction::HandleLong(bRead,"BmpMarkRefCam","ROIBottom",theApp.m_tSysParam.BmpMarkRefCamROI.bottom,strFile);

	CFunction::HandleLong(bRead,"BmpNeedleModelWin","WinLeft",theApp.m_tSysParam.BmpNeedleLearnWin.left,strFile);
	CFunction::HandleLong(bRead,"BmpNeedleModelWin","WinRight",theApp.m_tSysParam.BmpNeedleLearnWin.right,strFile);
	CFunction::HandleLong(bRead,"BmpNeedleModelWin","WinTop",theApp.m_tSysParam.BmpNeedleLearnWin.top,strFile);
	CFunction::HandleLong(bRead,"BmpNeedleModelWin","WinBottom",theApp.m_tSysParam.BmpNeedleLearnWin.bottom,strFile);

	CFunction::HandleLong(bRead,"BmpNeedleROI","ROILeft",theApp.m_tSysParam.BmpNeedleROI.left,strFile);
	CFunction::HandleLong(bRead,"BmpNeedleROI","ROIRight",theApp.m_tSysParam.BmpNeedleROI.right,strFile);
	CFunction::HandleLong(bRead,"BmpNeedleROI","WinTop",theApp.m_tSysParam.BmpNeedleROI.top,strFile);
	CFunction::HandleLong(bRead,"BmpNeedleROI","ROIBottom",theApp.m_tSysParam.BmpNeedleROI.bottom,strFile);



	CFunction::HandleLong(bRead, "BmpModelWinOne", "WinLeft", theApp.m_tSysParam.BmpMarkLearnWin[0].left, strFile);
	CFunction::HandleLong(bRead, "BmpModelWinOne", "WinRight", theApp.m_tSysParam.BmpMarkLearnWin[0].right, strFile);
	CFunction::HandleLong(bRead, "BmpModelWinOne", "WinTop", theApp.m_tSysParam.BmpMarkLearnWin[0].top, strFile);
	CFunction::HandleLong(bRead, "BmpModelWinOne", "WinBottom", theApp.m_tSysParam.BmpMarkLearnWin[0].bottom, strFile);

	CFunction::HandleLong(bRead, "BmpModelWinTwo", "WinLeft", theApp.m_tSysParam.BmpMarkLearnWin[1].left, strFile);
	CFunction::HandleLong(bRead, "BmpModelWinTwo", "WinRight", theApp.m_tSysParam.BmpMarkLearnWin[1].right, strFile);
	CFunction::HandleLong(bRead, "BmpModelWinTwo", "WinTop", theApp.m_tSysParam.BmpMarkLearnWin[1].top, strFile);
	CFunction::HandleLong(bRead, "BmpModelWinTwo", "WinBottom", theApp.m_tSysParam.BmpMarkLearnWin[1].bottom, strFile);

	CFunction::HandleLong(bRead,"BmpModelWinRef","WinLeft",theApp.m_tSysParam.BmpMarkRefCam.left,strFile);
	CFunction::HandleLong(bRead,"BmpModelWinRef", "WinRight",theApp.m_tSysParam.BmpMarkRefCam.right,strFile);
	CFunction::HandleLong(bRead,"BmpModelWinRef","WinTop",theApp.m_tSysParam.BmpMarkRefCam.top,strFile);
	CFunction::HandleLong(bRead,"BmpModelWinRef","WinBottom",theApp.m_tSysParam.BmpMarkRefCam.bottom,strFile);

	CFunction::HandleDouble(bRead, "BmpMarkCalibration", "CalibrationA", theApp.m_tSysParam.BmpMarkCalibration.dCalibrationA, strFile);
	CFunction::HandleDouble(bRead, "BmpMarkCalibration", "CalibrationB", theApp.m_tSysParam.BmpMarkCalibration.dCalibrationB, strFile);
	CFunction::HandleDouble(bRead, "BmpMarkCalibration", "CalibrationC", theApp.m_tSysParam.BmpMarkCalibration.dCalibrationC, strFile);
	CFunction::HandleDouble(bRead, "BmpMarkCalibration", "CalibrationD", theApp.m_tSysParam.BmpMarkCalibration.dCalibrationD, strFile);

	CFunction::HandleDouble(bRead,"BmpMarkRefCalibration","CalibrationA",theApp.m_tSysParam.BmpMarkRefCamCalibration.dCalibrationA,strFile);
	CFunction::HandleDouble(bRead,"BmpMarkRefCalibration","CalibrationB",theApp.m_tSysParam.BmpMarkRefCamCalibration.dCalibrationB,strFile);

	CFunction::HandleDouble(bRead, "BmpSlotParam", "Angle", theApp.m_tSysParam.BmpSlotAngle, strFile);
	CFunction::HandleDouble(bRead, "BmpSlotParam", "Acceptance", theApp.m_tSysParam.BmpSlotAcceptance, strFile);

	CFunction::HandleLong(bRead, "BmpSlotParam", "ROILeft", theApp.m_tSysParam.BmpSlotROI.left, strFile);
	CFunction::HandleLong(bRead, "BmpSlotParam", "ROIRight", theApp.m_tSysParam.BmpSlotROI.right, strFile);
	CFunction::HandleLong(bRead, "BmpSlotParam", "ROITop", theApp.m_tSysParam.BmpSlotROI.top, strFile);
	CFunction::HandleLong(bRead, "BmpSlotParam", "ROIBottom", theApp.m_tSysParam.BmpSlotROI.bottom, strFile);

	CFunction::HandleLong(bRead, "BmpSlotParam", "WinLeft", theApp.m_tSysParam.BmpSlotLearnWin.left, strFile);
	CFunction::HandleLong(bRead, "BmpSlotParam", "WinRight", theApp.m_tSysParam.BmpSlotLearnWin.right, strFile);
	CFunction::HandleLong(bRead, "BmpSlotParam", "WinTop", theApp.m_tSysParam.BmpSlotLearnWin.top, strFile);
	CFunction::HandleLong(bRead, "BmpSlotParam", "WinBottom", theApp.m_tSysParam.BmpSlotLearnWin.bottom, strFile);

	CFunction::HandleDouble(bRead, "BmpSlotParam", "CalibrationA", theApp.m_tSysParam.BmpSlotCalibration.dCalibrationA, strFile);
	CFunction::HandleDouble(bRead, "BmpSlotParam", "CalibrationB", theApp.m_tSysParam.BmpSlotCalibration.dCalibrationB, strFile);
	CFunction::HandleDouble(bRead, "BmpSlotParam", "CalibrationC", theApp.m_tSysParam.BmpSlotCalibration.dCalibrationC, strFile);
	CFunction::HandleDouble(bRead, "BmpSlotParam", "CalibrationD", theApp.m_tSysParam.BmpSlotCalibration.dCalibrationD, strFile);

	//search angle and acceptant
	for (int i=0;i<2;i++)
	{
		strKey.Format("Angle%d",i);
		CFunction::HandleDouble(bRead, "PadDetectParam", strKey, theApp.m_tSysParam.BmpPadDetectAngle[i], strFile);
		strKey.Format("Acceptance%d",i);
		CFunction::HandleDouble(bRead, "PadDetectParam",strKey, theApp.m_tSysParam.BmpPadDetectAcceptance[i], strFile);
	}

	for (int i=0;i<2;i++)
	{
		strKey.Format("PadDetectROI%d",i);
		CFunction::HandleLong(bRead, strKey, "ROILeft", theApp.m_tSysParam.BmpPadDetectROI[i].left, strFile);
		CFunction::HandleLong(bRead, strKey, "ROIRight", theApp.m_tSysParam.BmpPadDetectROI[i].right, strFile);
		CFunction::HandleLong(bRead, strKey, "ROITop", theApp.m_tSysParam.BmpPadDetectROI[i].top, strFile);
		CFunction::HandleLong(bRead, strKey, "ROIBottom", theApp.m_tSysParam.BmpPadDetectROI[i].bottom, strFile);
		
	}
	for (int i=0;i<5;i++)
	{
		strKey.Format("PadDetectModelWin%d",i);
		CFunction::HandleLong(bRead, strKey, "ModelWinLeft", theApp.m_tSysParam.BmpPadDetectLearnWin[i].left, strFile);
		CFunction::HandleLong(bRead, strKey, "ModelWinRight", theApp.m_tSysParam.BmpPadDetectLearnWin[i].right, strFile);
		CFunction::HandleLong(bRead, strKey, "ModelWinTop", theApp.m_tSysParam.BmpPadDetectLearnWin[i].top, strFile);
		CFunction::HandleLong(bRead, strKey, "ModelWinBottom", theApp.m_tSysParam.BmpPadDetectLearnWin[i].bottom, strFile);

	}

	CFunction::HandleDouble(bRead, "PadDetectCenter", "CenterX", theApp.m_tSysParam.BmpPadDetectCenterX, strFile);
	CFunction::HandleDouble(bRead, "PadDetectCenter", "CenterY", theApp.m_tSysParam.BmpPadDetectCenterY, strFile);
	//two padmark can use the same datas of this kind;
	CFunction::HandleDouble(bRead, "PadDetectCalibration", "CalibrationA", theApp.m_tSysParam.BmpPadDetectCalibration.dCalibrationA, strFile);
	CFunction::HandleDouble(bRead, "PadDetectCalibration", "CalibrationB", theApp.m_tSysParam.BmpPadDetectCalibration.dCalibrationB, strFile);
	CFunction::HandleDouble(bRead, "PadDetectCalibration", "CalibrationC", theApp.m_tSysParam.BmpPadDetectCalibration.dCalibrationC, strFile);
	CFunction::HandleDouble(bRead, "PadDetectCalibration", "CalibrationD", theApp.m_tSysParam.BmpPadDetectCalibration.dCalibrationD, strFile);

	CFunction::HandleDouble(bRead,"NeedleRefCalibration","CalibrationA",theApp.m_tSysParam.BmpNeedleCalibration.dCalibrationA,strFile);
	CFunction::HandleDouble(bRead,"NeedleRefCalibration","CalibrationB",theApp.m_tSysParam.BmpNeedleCalibration.dCalibrationB,strFile);

	CFunction::HandleDouble(bRead,"StepNeedleRefCalibration","CalibrationA",theApp.m_tSysParam.BmpStepNeedleCalibration.dCalibrationA,strFile); //步进标定
	CFunction::HandleDouble(bRead,"StepNeedleRefCalibration","CalibrationB",theApp.m_tSysParam.BmpStepNeedleCalibration.dCalibrationB,strFile);

	CFunction::HandleDouble(bRead, "PatternParam", "BmpPatternScale", theApp.m_tSysParam.BmpPatternScale, strFile);
	CFunction::HandleDouble(bRead, "PatternParam", "BmpPatternAngle", theApp.m_tSysParam.BmpPatternAngle, strFile);
	CFunction::HandleDouble(bRead, "PatternParam", "BmpPatternAcceptance", theApp.m_tSysParam.BmpPatternAcceptance, strFile);

	CFunction::HandleLong(bRead, "BmpPatternROI", "ROILeft", theApp.m_tSysParam.BmpPatternROI.left, strFile);
	CFunction::HandleLong(bRead, "BmpPatternROI", "ROIRight", theApp.m_tSysParam.BmpPatternROI.right, strFile);
	CFunction::HandleLong(bRead, "BmpPatternROI", "ROITop", theApp.m_tSysParam.BmpPatternROI.top, strFile);
	CFunction::HandleLong(bRead, "BmpPatternROI", "ROIBottom", theApp.m_tSysParam.BmpPatternROI.bottom, strFile);

	CFunction::HandleLong(bRead, "BmpPatternModelWin", "WinLeft", theApp.m_tSysParam.BmpPatternLearnWin.left, strFile);
	CFunction::HandleLong(bRead, "BmpPatternModelWin", "WinRight", theApp.m_tSysParam.BmpPatternLearnWin.right, strFile);
	CFunction::HandleLong(bRead, "BmpPatternModelWin", "WinTop", theApp.m_tSysParam.BmpPatternLearnWin.top, strFile);
	CFunction::HandleLong(bRead, "BmpPatternModelWin", "WinBottom", theApp.m_tSysParam.BmpPatternLearnWin.bottom, strFile);

	CFunction::HandleLong(bRead, "NeedleCameraParam", "ExposureTime", theApp.m_tSysParam.NeedleCameraExposure, strFile);
	CFunction::HandleLong(bRead, "NeedleCameraParam", "Gain", theApp.m_tSysParam.NeedleCameraGain, strFile);
	CFunction::HandleLong(bRead, "CameraParam", "ExposureTime", theApp.m_tSysParam.CameraExposureTime, strFile);
	CFunction::HandleLong(bRead, "CameraParam", "Gain", theApp.m_tSysParam.CameraGain, strFile);
	CFunction::HandleLong(bRead, "DetectCameraParam", "ExposureTime", theApp.m_tSysParam.CameraDetectExposureTime, strFile);
	CFunction::HandleLong(bRead, "DetectCameraParam", "Gain", theApp.m_tSysParam.CameraDetectGain, strFile);

	CFunction::HandleDouble(bRead, "AutoNeedleParam", "Angle", theApp.m_tSysParam.dAutoNeedleAngle, strFile);
	CFunction::HandleDouble(bRead, "AutoNeedleParam", "Acceptance", theApp.m_tSysParam.dAutoNeedleAcceptance, strFile);

	CFunction::HandleLong(bRead, "AutoNeedleROI", "ROILeft", theApp.m_tSysParam.AutoNeedleROI.left, strFile);
	CFunction::HandleLong(bRead, "AutoNeedleROI", "ROIRight", theApp.m_tSysParam.AutoNeedleROI.right, strFile);
	CFunction::HandleLong(bRead, "AutoNeedleROI", "ROITop", theApp.m_tSysParam.AutoNeedleROI.top, strFile);
	CFunction::HandleLong(bRead, "AutoNeedleROI", "ROIBottom", theApp.m_tSysParam.AutoNeedleROI.bottom, strFile);

	CFunction::HandleLong(bRead, "AutoNeedleModelWin", "WinLeft", theApp.m_tSysParam.AutoNeedleModelWin.left, strFile);
	CFunction::HandleLong(bRead, "AutoNeedleModelWin", "WinRight", theApp.m_tSysParam.AutoNeedleModelWin.right, strFile);
	CFunction::HandleLong(bRead, "AutoNeedleModelWin", "WinTop", theApp.m_tSysParam.AutoNeedleModelWin.top, strFile);
	CFunction::HandleLong(bRead, "AutoNeedleModelWin", "WinBottom", theApp.m_tSysParam.AutoNeedleModelWin.bottom, strFile);

	CFunction::HandleDouble(bRead, "AutoNeedleCalibration", "CalibrationA", theApp.m_tSysParam.AutoNeedleCalib.dCalibrationA, strFile);
	CFunction::HandleDouble(bRead, "AutoNeedleCalibration", "CalibrationB", theApp.m_tSysParam.AutoNeedleCalib.dCalibrationB, strFile);
	CFunction::HandleDouble(bRead, "AutoNeedleCalibration", "CalibrationC", theApp.m_tSysParam.AutoNeedleCalib.dCalibrationC, strFile);
	CFunction::HandleDouble(bRead, "AutoNeedleCalibration", "CalibrationD", theApp.m_tSysParam.AutoNeedleCalib.dCalibrationD, strFile);

	CFunction::HandleDouble(bRead,"FirstPixsel","PixselX",theApp.m_tSysParam.BmpPadDetectFirstPixselX,strFile);
	CFunction::HandleDouble(bRead,"FirstPixsel","PixselY",theApp.m_tSysParam.BmpPadDetectFirstPixselY,strFile);
}
// 退出应用程序
int CTSCtrlSysApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class
	SysParam(FALSE);// 保存参数
	m_EncryptOp.DeMD5();
	//ProductParam(FALSE);
	//V9Param(FALSE);
	return CWinAppEx::ExitInstance();
}
// 初始化
BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
}
// 读取参数
void CTSCtrlSysApp::LoadParam()
{
	ProductParam(TRUE);//产品参数
	V9Param(TRUE);//V9参数
	SysParam(TRUE);
	g_pFrm->SetWindowTextEx();
	g_pFrm->m_CmdRun.ProductParam.tgTrackInfo.TrackReset();
}
//开启加热系统
void CTSCtrlSysApp::HeatSystemOn()
{
	PlcPreHeatOn();
	PlcHeatOn();
}
void CTSCtrlSysApp::PlcPreHeatOn()
{
	CString str;
	//20180519 modify by zwg
	bool bReadData = false;
	long lReadData = 0;
	bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"R","1410");
	if(!bReadData)
	{
		return;       //failed to read
	}
	if(lReadData==1)
	{
        return;     //already heat
	}
	//******************************************************************//
	str.Format("1310");
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		m_Serial.WritePlcData(1,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		m_Serial.WritePlcROut(str,true);
	}
	Sleep(200);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		m_Serial.WritePlcData(0,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		m_Serial.WritePlcROut(str,false);
	}
	str.Format("待料区加热开启...");
	g_pFrm->m_CmdRun.AddMsg(str);
}
void CTSCtrlSysApp::PlcHeatOn()
{
	CString str;
	//20180519 modify by zwg
	bool bReadData = false;
	long lReadData = 0;
	bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"R","1411");
	if(!bReadData)
	{
		return;       //failed to read
	}
	if(lReadData==1)
	{
		return;     //already heat
	}
	//******************************************************************//
	str.Format("1311");
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		m_Serial.WritePlcData(1,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		m_Serial.WritePlcROut(str,true);
	}
	Sleep(200);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		m_Serial.WritePlcData(0,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		m_Serial.WritePlcROut(str,false);
	}
	str.Format("点胶区加热开启...");
	g_pFrm->m_CmdRun.AddMsg(str);
}

bool CTSCtrlSysApp::ServerInfoGet()
{
	CString str;
	CString ServerIP;
	CString strURL=g_pFrm->m_CmdRun.ProductParam.ServerURL;
	int ServerPort,FCompanyCode;

	if(strURL.IsEmpty())
	{
		str.Format("【】软件开启时检测到URL链接为空...");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		CDlgURLPrompt dlg;
		dlg.DoModal();
	}
	int ret =g_pFrm->m_CmdRun.ReadURLText(g_pFrm->m_CmdRun.ProductParam.ServerURL,ServerIP,ServerPort,FCompanyCode);
	if(ret == -1)
	{
		str.Format("【】软件开启时--获取IP、端口和公司代码失败！");
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		return false;
	}
	strcpy(g_pFrm->m_CmdRun.ProductParam.ServerIP,ServerIP);
	g_pFrm->m_CmdRun.ProductParam.ServerPort=ServerPort;
	g_pFrm->m_CmdRun.ProductParam.FCompanyCode=FCompanyCode;

	str.Format("软件开启时--获取IP、端口和公司代码成功！");
	g_pFrm->m_CmdRun.AddMsg(str);
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	return true;
}