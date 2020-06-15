// DlgPosInfo.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgPosInfo.h"
#include "MainFrm.h"
#include <math.h>
#include "DlgMaterialCode.h"

#define DefSysPassWordA  "FAD9100" 
// CDlgPosInfo dialog
// 报警信息
CString RunEorrorT[]=
{
	"伺服驱动器报警",
	"图像检测错误!",
	"【运行时错误】#1 V9关阀错误!",//2
	"【运行时错误】#2 V9关阀错误!",
	"【运行时错误】#3 V9关阀错误!",
	"【运行时错误】#4 V9关阀错误!",
	"【运行时错误】#1 V9开阀错误!",
	"【运行时错误】#2 V9开阀错误!",
	"【运行时错误】#3 V9开阀错误!",
	"【运行时错误】#4 V9开阀错误!",
	"【运行时错误】#1 V9回零失败!",
	"【运行时错误】#2 V9回零失败!",
	"【运行时错误】#3 V9回零失败!",
	"【运行时错误】#4 V9回零失败!",
	"【运行时错误】料道无料错误！",
	"【运行时错误】缓冲区运动失败！",
	"【运行时错误】Z轴点胶位下于Z轴提升高度！",//16
	"【运行时错误】图像查找失败！",
	"【运行时错误】支架放反",
	"【运行时错误】上料超时",
	"【运行时错误】自动调整胶量失败！",
	"【运行时错误】测高超限！",
	"【运行时错误】缺颗检测失败！",//22
	"【运行时错误】支架倾斜检测失败！",
	"【运行时错误】称重调整胶量失败！",
	"【运行时错误】轨道上料失败！",
	"【运行时错误】正限位触发!",
	"【运行时错误】负限位触发!",
	"【运行时错误】软件使用期已过!",
	"【运行时错误】阀体复位失败！",
	"【运行时错误】坐标轴参数设置失败！",
	"【运行时错误】运动停止失败！",
	"【运行时错误】急停按下！",
	"【运行时错误】设备正进行其他操作！",//33
	"【运行时错误】门禁开关打开",
	"【运行时错误】旋转气缸打开或关闭失败！",
	"【运行时错误】轨迹计算错误",
	"【运行时错误】不确定错误",
	"【运行时错误】预约时间到",//38
      "温度未到",//39
	  "密封圈寿命到",//40
     "阀体清洗时间到",//41
      "A阀测高失败",//42
	  "A阀高度异常",//43
	  "B阀测高失败",//44
	  "B阀高度异常",//45
	  "C阀测高失败",//46
	  "C阀高度异常",//47
	  "缺颗粒相机连接失败",//48
	  "请进行手动排胶",//49
};

IMPLEMENT_DYNAMIC(CDlgPosInfo, CDialog)

CDlgPosInfo::CDlgPosInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPosInfo::IDD, pParent)
{
	m_brush.CreateSolidBrush(RGB(255,251,240));
	TimeStarC=false;
	f=NULL;
	PlcError=NULL;
	ErrorCount=0;
	ErrorVal=0;
	lRegisterStatus=0;
	dRemainSpan=0.0;
    dValveRemainSpan=0.0;
	lTimeInStop = 0;
	PlcEorrTag=false;
	PlcRTag=false;
	bRecordError = false;
	bLogGlueEnable=true;
	bStopPush=true;
}

CDlgPosInfo::~CDlgPosInfo()
{
	delete f;
	if (PlcError!=NULL)
	{
		delete[] PlcError;
		PlcError = NULL;
	}
	m_brush.DeleteObject();
	m_uSocket.Close();
}

void CDlgPosInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgPosInfo, CDialog)
	ON_WM_TIMER()
	ON_MESSAGE(WM_MSG_OUTPUT_EVENT, &CDlgPosInfo::OnMessageOutputEvent)
	ON_MESSAGE(WM_MSG_INPUT_EVENT, &CDlgPosInfo::OnMessageInputEvent)
	ON_MESSAGE(WM_MSG_POS_EVENT, &CDlgPosInfo::OnMessagePosEvent)
    ON_MESSAGE(WM_MSG_GLUE_EVENT,&CDlgPosInfo::OnMessageGlueEvent)

	ON_WM_CTLCOLOR()
	ON_WM_CONTEXTMENU()
	ON_BN_CLICKED(IDC_BTN_INFO_CLEAR_ERROR, &CDlgPosInfo::OnBnClickedBtnInfoClearError)
	ON_BN_CLICKED(IDC_BTN_INFO_RESET, &CDlgPosInfo::OnBnClickedBtnInfoReset)
	ON_BN_CLICKED(IDC_BTN_INFO_CLEAR_CAPACITY, &CDlgPosInfo::OnBnClickedBtnInfoClearCapacity)
	ON_BN_CLICKED(IDC_BTN_INFO_DEBUG, &CDlgPosInfo::OnBnClickedBtnInfoDebug)
	ON_BN_CLICKED(IDC_BTN_INFO_MOVETO_CLEAN_POS, &CDlgPosInfo::OnBnClickedBtnInfoMovetoCleanPos)
	ON_BN_CLICKED(IDC_BTN_INFO_MOVETO_DISCHARGE_POS, &CDlgPosInfo::OnBnClickedBtnInfoMovetoDischargePos)
	ON_BN_CLICKED(IDC_BTN_INFO_CLEAR_SLOT, &CDlgPosInfo::OnBnClickedBtnInfoClearSlot)
	ON_BN_CLICKED(IDC_BTN_INFO_CLEAR_DETECT, &CDlgPosInfo::OnBnClickedBtnInfoClearDetect)
	ON_BN_CLICKED(IDC_BTN_MANUAL_LOAD, &CDlgPosInfo::OnBnClickedBtnManualLoad)
	ON_BN_CLICKED(IDC_BTN_MANUAL_UNLOAD, &CDlgPosInfo::OnBnClickedBtnManualUnload)
	ON_BN_CLICKED(IDC_BTN_FIRST_IN_POS, &CDlgPosInfo::OnBnClickedBtnFirstInPos)
	ON_BN_CLICKED(IDC_BTN_FIRST_OUT_POS, &CDlgPosInfo::OnBnClickedBtnFirstOutPos)
	ON_EN_CHANGE(IDC_EDIT_GLUE_SETTING, &CDlgPosInfo::OnEnChangeEditGlueSetting)
	ON_BN_CLICKED(IDC_CHECK_INFO_ENABLE_ARRANGE, &CDlgPosInfo::OnBnClickedCheckInfoEnableArrange)
	ON_BN_CLICKED(IDC_BTN_INFO_ARRANGE_TIME, &CDlgPosInfo::OnBnClickedBtnInfoArrangeTime)
	ON_BN_CLICKED(IDC_BTN_VALVE_UPDATE, &CDlgPosInfo::OnBnClickedBtnValveUpdate)
	ON_BN_CLICKED(IDC_BUTTON_PRODUCTIVITYR, &CDlgPosInfo::OnBnClickedButtonProductivityr)
	ON_BN_CLICKED(IDC_INFO_UDP_SENDMSG, &CDlgPosInfo::OnBnClickedInfoUdpSendmsg)
END_MESSAGE_MAP()

// CDlgPosInfo message handlers
// 初始化
BOOL CDlgPosInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_Static.SubclassDlgItem(IDC_STATIC_INFO_STS, this);
	theApp.m_Mv400.SetMessageWnd(m_hWnd);
	SetTimer(0, 1000, NULL);
	SetTimer(1, 100, NULL);
 
	f = new CFont; 
	f->CreateFont(20, 0, 0, 0, FW_BOLD, TRUE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("宋体")); 
	GetDlgItem(IDC_STATIC_INFO_ERROR)->SetFont(f);
    ReadEorrTxt();
	m_uSocket.SetParent(this);
	m_uSocket.Create(9000, SOCK_DGRAM);
	m_uSocket.Bind(9000);
	//m_Rest.SubclassDlgItem(IDC_BTN_INFO_CLEAR_ERROR,this);
	//// TODO: 在此添加额外的初始化代码
	//m_Rest.LoadBitmaps(IDB_BITMAP2_UP,IDB_BITMAP1_DOWN);
	//m_Rest.SizeToContent();
	//m_Home.SubclassDlgItem(IDC_BTN_INFO_RESET,this);
	//m_Home.LoadBitmaps(IDB_BITMAPH_UP,IDB_BITMAPH_DOWN);
	//m_Home.SizeToContent();

	//数字转化成字符串
	//sprintf(g_pFrm->m_CmdRun.m_Glue_msg.Qrev_CodeID, "%lf", g_pFrm->m_CmdRun.ProductParam.Save_rev_CodeID);
	return TRUE;
}
// 定时器
void CDlgPosInfo::OnTimer(UINT id)
{
	static bool Strbool=false;
	if(g_pFrm==NULL) return;
	CString str;
    CTimeSpan timeSpan;
	double timeSpanSec;
	switch (id)
	{
	case 0:
		{
			if(0==g_pFrm->m_CmdRun.ProductParam.bMesOffline)
			{
				CanEnterStop();//20180713
			}
			DisplayRemainSpan();
			DisplayValveRemainSpan();
			InitManager();  //定时恢复最低权限；
			if (g_pFrm->m_CmdRun.ProductParam.ArrangeEnable&&(dRemainSpan<0.001)&&(K_RUN_STS_STOP==g_pFrm->m_CmdRun.m_tStatus)&&(g_pFrm->m_CmdRun.ProductParam.ArrangeSpan>0))
			{
				g_pFrm->m_CmdRun.m_bFirstDisp=false;
				g_pFrm->m_CmdRun.m_bTestRunStop=true;
				g_pFrm->m_CmdRun.RunErrorID=38;   //Can assure alarm always;
				//g_pFrm->m_CmdRun.m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE); //forbid the PLC generate 16# alarm!
				//avoid write log always;
				if (bLogGlueEnable)
				{
					CString str;
					str.Format("因报警38【胶水预约时间到】产生，机械臂准备好信号被清除！");
					g_pFrm->m_CmdRun.PutLogIntoList(str);
					bLogGlueEnable=false;
				}

			}
			if ((dValveRemainSpan<0.001)&&(K_RUN_STS_STOP==g_pFrm->m_CmdRun.m_tStatus)&&(g_pFrm->m_CmdRun.ProductParam.ValveMonitorSpan>0))
			{
				g_pFrm->m_CmdRun.RunErrorID=41;   //Can assure alarm always;
			}

			if ((g_pFrm->m_CmdRun.m_tStatus ==K_RUN_STS_PAUSE)&&!PlcEorrTag)
			{
				ErrorVal=30;
				SetDlgItemText(IDC_STATIC_INFO_ERROR,"系统暂停中!");
			}
			if ((g_pFrm->m_CmdRun.m_tStatus ==K_RUN_STS_STOP)&&!PlcEorrTag&&(g_pFrm->m_CmdRun.RunErrorID ==-1))
			{
				ErrorVal=0;
				SetDlgItemText(IDC_STATIC_INFO_ERROR,"系统运行正常!");
			}
			if ((g_pFrm->m_CmdRun.m_tStatus ==K_RUN_STS_RUN)&&!PlcEorrTag)
			{
				ErrorVal=0;
				SetDlgItemText(IDC_STATIC_INFO_ERROR,"系统运行中!");
			}
			if((g_pFrm->m_CmdRun.RunErrorID >=0)&&!PlcEorrTag)
			{
				ErrorVal=20;
				if(g_pFrm->m_CmdRun.RunErrorID>=0&&g_pFrm->m_CmdRun.RunErrorID<=49)
				{
					SetDlgItemText(IDC_STATIC_INFO_ERROR,RunEorrorT[g_pFrm->m_CmdRun.RunErrorID]);
					if(!bRecordError)
					{
						g_pFrm->m_CmdRun.PutLogIntoList(RunEorrorT[g_pFrm->m_CmdRun.RunErrorID]);
						bRecordError = true;
					}
				}
			}
			if(PlcEorrTag)
			{
				ErrorVal=20;
				if(ErrorPlcData>0&&ErrorPlcData<ErrorCount)
				{	
					//if((ErrorPlcData<27)&&(g_pFrm->m_CmdRun.m_tStatus!=K_RUN_STS_STOP)) //27---33 塌陷报警
					//{
					//}
					SetDlgItemText(IDC_STATIC_INFO_ERROR,PlcError[ErrorPlcData]);
					if(!bRecordError)
					{
						g_pFrm->m_CmdRun.PutLogIntoList(PlcError[ErrorPlcData]);
						bRecordError = true;
					}						
				}
				else
				{
					str = "PLC 检测到其他系统异常（无具体文本的报错）!";
					SetDlgItemText(IDC_STATIC_INFO_ERROR,str);
					if(!bRecordError)
					{
						g_pFrm->m_CmdRun.PutLogIntoList(str);
						bRecordError = true;
					}
				}

			}
			if(g_pFrm->m_CmdRun.m_tStatus!=K_RUN_STS_RUN)
			{
				if(false)//if (/*1 == g_pFrm->m_CmdRun.ProductParam.bCleanRemind*/)	
				{
					static bool bAlarm=true;
					timeSpan = CTime::GetCurrentTime() - g_pFrm->m_CmdRun.m_TimeCleanRemindSt;
					timeSpanSec = timeSpan.GetTotalSeconds();
					double timeCleanRemindSec = g_pFrm->m_CmdRun.ProductParam.CleanRemindTimeHour * 3600;
					if ((timeCleanRemindSec > 0) && ((timeSpanSec+1800)> timeCleanRemindSec)&&bAlarm)
					{
						bAlarm=false;
						g_pFrm->m_CmdRun.m_bIsSupend=true;
						if (IDOK == AfxMessageBox(_T("阀体清洗预警！")))
						{
							g_pFrm->m_CmdRun.m_bIsSupend=false;
						}

					}
					if ((timeCleanRemindSec > 0) && (timeSpanSec> timeCleanRemindSec))
					{
						g_pFrm->m_CmdRun.m_bIsSupend=true;
						if (IDOK == AfxMessageBox(_T("请清洗阀体！")))
						{
							bAlarm=true;
							g_pFrm->m_CmdRun.m_bIsSupend=false;
							g_pFrm->m_CmdRun.m_TimeCleanRemindSt = CTime::GetCurrentTime();
							CString str;
							str = g_pFrm->m_CmdRun.m_TimeCleanRemindSt.Format("%Y-%m-%d %H:%M:%S");
							strcpy(theApp.m_tSysParam.szLastCleanTime, str); 
							theApp.SysParam(FALSE);
						}

					}
				}
			}
			Strbool=!Strbool;
			if (!Strbool)
			{
				SetDlgItemText(IDC_STATIC_INFO_ERROR,"");
			}
			g_pFrm->m_CmdRun.SetLamp();
			DisplayWorkStat();
			DisplayGlueVol();

			if(theApp.m_bInitOk)
			{
				// 检测注册信息
				if (CheckRegisterCode(MainRegMsg))
				{
					if(g_pFrm->m_CmdRun.IsExitRun())
					{
						g_pFrm->m_CmdRun.MoveToZSafety();
						g_pFrm->m_CmdRun.RunStop();
						g_pFrm->m_CmdRun.Alarm(28);
						//TimePrompt.Enabled = false;
						//TimePrompt.Stop();

					}

				}
			}
			SetAuthority();//过期禁用
		}
		break;
	//case 1:
	//	if(g_pFrm->m_CmdRun.m_Glue_msg.rev_Succeed == true)
	//	{
	//		//自动填充胶量
	//		//AutoSetGlue();
	//	}
		break;
	}	
	CDialog::OnTimer(id);
}
BOOL CDlgPosInfo::CheckRegisterCode(CRegisterCode &tempRegMsg)
{
	//theApp.m_EncryptOp.InitRegeditInfo(tempRegMsg);
	tempRegMsg = theApp.m_EncryptOp.IsInstalled();
	COleDateTime CurrentTime = COleDateTime::GetCurrentTime();
	CString strInfo = "";

	if (!tempRegMsg.m_bRegisterFlag)
	{
		COleDateTimeSpan Spantime;

		COleDateTime lastTime = tempRegMsg.m_dtLastTime;
		COleDateTime limitTime = tempRegMsg.m_dtLimitTime;

		if (CurrentTime < lastTime)//往前改时间
		{
			if (!theApp.m_EncryptOp.m_bLockSystem)
			{
				tempRegMsg.m_dtLockTime = lastTime;
				theApp.m_EncryptOp.m_bLockSystem = true;
			}

			strInfo.Format("请修正时间到%d年%d月%d日-%d时%d分%d秒之后", lastTime.GetYear(), lastTime.GetMonth(), lastTime.GetDay(), lastTime.GetHour(), lastTime.GetMinute(), lastTime.GetSecond());

			lRegisterStatus = 2;
			g_pFrm->m_CmdRun.m_pMv->SetOutput(theApp.m_tSysParam.outRed,TRUE);
			SetDlgItemText(IDC_STATIC_INFO_REGISTER,"时间已被修改，请勿操作");
			g_pFrm->m_CmdRun.m_pMv->SetOutput(theApp.m_tSysParam.outRed,FALSE);

			tempRegMsg.m_dtLastTime = lastTime;
			theApp.m_EncryptOp.SetAllOFRegisterCode(tempRegMsg);
			g_pFrm->m_CmdRun.m_pLog->log(strInfo);
			g_pFrm->m_CmdRun.AddMsg(strInfo);
			//Alarm(49);
			//AfxMessageBox(strInfo);
		}
		else if(CurrentTime > limitTime)//正常到期
		{
			if (!theApp.m_EncryptOp.m_bLockSystem)
			{
				tempRegMsg.m_dtLockTime = lastTime;
				theApp.m_EncryptOp.m_bLockSystem = true;
			}
			lRegisterStatus = 2;
			Spantime = CurrentTime - limitTime;
			//strInfo.Format("软件过期%s！",Spantime.Format("%D天%H时%M分%S秒"));
			strInfo.Format("软件已过期!请注册");
			SetDlgItemText(IDC_STATIC_INFO_REGISTER,strInfo);
			//tempRegMsg.m_dtLastTime = COleDateTime::GetCurrentTime();
			//theApp.m_EncryptOp.SetAllOFRegisterCode(tempRegMsg);
		}
		else
		{
			if (theApp.m_EncryptOp.m_bLockSystem)
			{
				theApp.m_EncryptOp.m_bLockSystem = false;
			}

			Spantime = limitTime - CurrentTime;
			lRegisterStatus = 1;
			if (Spantime.GetDays() < 2)
			{
				lRegisterStatus = 2;

			}
			strInfo.Format("余%s到期！",Spantime.Format("%D天%H时%M分%S秒"));
			SetDlgItemText(IDC_STATIC_INFO_REGISTER,strInfo);

			tempRegMsg.m_dtLastTime = CurrentTime;
			theApp.m_EncryptOp.SetAllOFRegisterCode(tempRegMsg);
		}
	}
	else
	{
		if (theApp.m_EncryptOp.m_bLockSystem)
		{
			theApp.m_EncryptOp.m_bLockSystem = false;
		}
		lRegisterStatus = 0;
		strInfo.Format("软件已注册！");
		SetDlgItemText(IDC_STATIC_INFO_REGISTER, strInfo);
	}

	return theApp.m_EncryptOp.m_bLockSystem;
}

// 输入信号响应消息
LRESULT CDlgPosInfo::OnMessageInputEvent(WPARAM wparam, LPARAM lparam)
{	
	int inNo=int(wparam);
	CString str;
	if(inNo == theApp.m_tSysParam.inEmergecy)
	{
		if(lparam == 1 || lparam == 2) 
		{
			if(lparam ==2)
			{
				theApp.m_Mv400.m_bExit=true;//在上下料线程做急停标志
				////theApp.m_Mv400.SetOutput(K_ALL_PORTS, FALSE);
				g_pFrm->m_CmdRun.Alarm(32);
				g_pFrm->m_CmdRun.Emergency();
				ErrorVal=20;
				SetDlgItemText(IDC_STATIC_INFO_ERROR,"系统急停中!");
				PlcRTag=false;
				g_aLarm = TRUE;
				theApp.m_tSysParam.ThreeAxHomeSu=false;
				theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outReady,FALSE);
				str.Format("急停信号触发！");
				if(g_pFrm!=NULL)
				{
					g_pFrm->m_CmdRun.PutLogIntoList(str);
				}
			}
			else
			{
				g_aLarm = TRUE;
			} 
		}
		else if(lparam ==-2) 
		{
			if(g_aLarm)
			{            
				if(theApp.m_tSysParam.ThreeAxHomeSu)
				{
					g_pFrm->m_CmdRun.m_tStatus = K_RUN_STS_STOP;
				}
				else
				{
					g_pFrm->m_CmdRun.m_tStatus = K_RUN_STS_NONE;
				}
				ErrorVal=0;
				SetDlgItemText(IDC_STATIC_INFO_ERROR,"系统运行正常!");
				g_aLarm = FALSE;
				theApp.m_Mv400.m_bExit=false;
			}
		}
	}
	else if(inNo == theApp.m_tSysParam.inStop)
	{
		if(lparam == -2)
		{
			str.Format("inStop = false,m_tStatus = %d",int(g_pFrm->m_CmdRun.m_tStatus));
			g_pFrm->m_CmdRun.AddMsg(str);
			g_pFrm->m_CmdRun.ManalRun = true;
            bStopPush=true;

			if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_PAUSE)
			{
				g_pFrm->m_CmdRun.m_tStatus = K_RUN_STS_STOP;
			}   //avoid white screen ,as timeout .
			//g_pView->m_ImgStatic.ThreadDetectCameraModeSwitch();
		}
	}
	else if(inNo == theApp.m_tSysParam.inTransitionDetect)
	{
		if(lparam == -2)
		{
			if(!g_pFrm->m_CmdRun.ManalRun)    
			{
				str.Format("IO扫描线程：发出待料区缺颗粒检测请求。");
				if(NULL!=g_pFrm)
				{
					g_pFrm->m_CmdRun.PutLogIntoList(str);
				}		
				//Sleep(100);
				g_pFrm->m_CmdRun.RunThreadDetect();
			}
		}
	}
	else if(inNo == theApp.m_tSysParam.inReset)
	{
		if(lparam == -2&&(!g_aLarm))
		{
			str.Format("复位信号触发！");
			if(g_pFrm!=NULL)
			{
				g_pFrm->m_CmdRun.PutLogIntoList(str);
			}
			if (g_pFrm->m_CmdRun.m_tStatus!= K_RUN_STS_EMERGENCY)
			{
				OnBnClickedBtnInfoClearError();
				if((g_pFrm->m_CmdRun.RunErrorID>=42)&&(theApp.m_SysUser.m_CurUD.level<GM_ROOT))
				{
					return 0;
				}
				if((g_pFrm->m_CmdRun.RunErrorID==21)&&(theApp.m_SysUser.m_CurUD.level<GM_ROOT))
				{
					return 0;
				}
				if((ErrorPlcData>=27)&&(ErrorPlcData<=33)&&(theApp.m_SysUser.m_CurUD.level<GM_ROOT))//防塌线权限解除
				{
					return 0;
				}
				if(g_pFrm->m_CmdRun.ManalRun)
				{
					g_pFrm->m_CmdRun.RunThreadHome();
				}
			}
		}
	}
	else if(inNo == theApp.m_tSysParam.inPauseOrRun)//响应门禁信号
	{
		if(g_pFrm->m_CmdRun.ProductParam.DoorSwitch>0) 
		{
			if(lparam == 2&&(!g_aLarm))
			{   //24v->0v :门禁信号解除
				str.Format("inPauseOrRun = Pause,m_tStatus = %d",int(g_pFrm->m_CmdRun.m_tStatus));
				g_pFrm->m_CmdRun.AddMsg(str);
				str.Format("【门禁】暂停信号触发！触发前运行状态：%d",int(g_pFrm->m_CmdRun.m_tStatus));
				if(g_pFrm!=NULL)
				{
					g_pFrm->m_CmdRun.PutLogIntoList(str);
				}
				if (g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
				{
					g_pFrm->m_CmdRun.Pause();
					str.Format("【门禁】触发，进入暂停状态!");
					g_pFrm->m_CmdRun.PutLogIntoList(str);
				}
			}
			else if(lparam ==-2)//解除暂停&&无急停信号->运行
			{   //0v->24v ；门禁信号触发
				str.Format("inPauseOrRun = Run,m_tStatus = %d",int(g_pFrm->m_CmdRun.m_tStatus));
				g_pFrm->m_CmdRun.AddMsg(str);
				str.Format("【门禁】暂停解除信号解除！脱离前运行状态：%d",int(g_pFrm->m_CmdRun.m_tStatus));
				if(g_pFrm!=NULL)
				{
					g_pFrm->m_CmdRun.PutLogIntoList(str);
				}
				if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_PAUSE)
				{
					g_pFrm->m_CmdRun.m_tStatus = K_RUN_STS_RUN;
					str.Format("成功脱离【门禁】暂停状态,返回运行状态！");
					g_pFrm->m_CmdRun.PutLogIntoList(str);
				}
			}

		}
	}
	/*else if(inNo == theApp.m_tSysParam.inPauseOrRun)
	{
	if(lparam == -2)
	{
	str.Format("inPauseOrRun = false,m_tStatus = %d",int(g_pFrm->m_CmdRun.m_tStatus));
	g_pFrm->m_CmdRun.AddMsg(str);
	}
	else if(lparam == 2)
	{
	str.Format("inPauseOrRun = true,m_tStatus = %d",int(g_pFrm->m_CmdRun.m_tStatus));
	g_pFrm->m_CmdRun.AddMsg(str);
	}
	}*/
	else if(inNo == theApp.m_tSysParam.inPlcRun)
	{
		if(lparam == -2)
		{
			str.Format("inPlcRun = true,m_tStatus = %d",int(g_pFrm->m_CmdRun.m_tStatus));
			g_pFrm->m_CmdRun.AddMsg(str);
			g_pFrm->m_CmdRun.ManalRun = false;
		}
		else if(lparam == 2)
		{
			str.Format("inPlcRun = false,m_tStatus = %d",int(g_pFrm->m_CmdRun.m_tStatus));
			g_pFrm->m_CmdRun.AddMsg(str);
			//g_pFrm->m_CmdRun.ManalRun = true;
		}
	}
	else if(inNo == theApp.m_tSysParam.inPlcPause)
	{
		if(lparam == -2)
		{
			str.Format("inPlcPause = Pause,m_tStatus = %d",int(g_pFrm->m_CmdRun.m_tStatus));
			g_pFrm->m_CmdRun.AddMsg(str);
			str.Format("暂停信号触发！");
			if(g_pFrm!=NULL)
			{
				g_pFrm->m_CmdRun.PutLogIntoList(str);
			}
			if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
			{
				g_pFrm->m_CmdRun.Pause();
			}
		}
		else if(lparam == 2&&(!g_aLarm))
		{
			str.Format("inPlcPause = Run,m_tStatus = %d",int(g_pFrm->m_CmdRun.m_tStatus));
			g_pFrm->m_CmdRun.AddMsg(str);                        //此种状态无法由暂停进入运行...
			if((ErrorPlcData>=27)&&(ErrorPlcData<=33)&&(theApp.m_SysUser.m_CurUD.level<GM_ROOT))//防塌线权限解除
			{
				return 0;
			}
			if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_PAUSE)
			{
				g_pFrm->m_CmdRun.m_tStatus = K_RUN_STS_RUN;
			}
		}
	}
	else if(inNo == theApp.m_tSysParam.inPlcReady)
	{
		if(lparam == -2&&(!g_aLarm))
		{
			CString str;
			str.Format("m_tStatus = %d,AxHomeSu = %d",int(g_pFrm->m_CmdRun.m_tStatus),int(theApp.m_tSysParam.ThreeAxHomeSu));
			g_pFrm->m_CmdRun.AddMsg(str);
			str.Format("PLCReady触发！");
			if(g_pFrm!=NULL)
			{
				g_pFrm->m_CmdRun.PutLogIntoList(str);
			}
			if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_STOP&&theApp.m_tSysParam.ThreeAxHomeSu&&(g_pFrm->m_CmdRun.RunErrorID==-1))
			{
				if (g_pFrm->m_CmdRun.ProductParam.TempJudgeBeforeStart&&(!TemperatureCheck()))
				{
					g_pFrm->m_CmdRun.AlarmOnly(39);
					return 0;
				}
				/////////////////////////////胶量对比：机台使能/////////////////////////////
			/*	if ((theApp.m_SysUser.m_CurUD.level==GM_GUEST)&&(0==g_pFrm->m_CmdRun.ProductParam.bMesOffline))
				{
					theApp.m_Serial.SendGlueToMES(g_pFrm->m_CmdRun.V9Parm[0].DispVolume);
					Sleep(1000);
					if ((!theApp.bMachineEnable))
					{
						str.Format("MES禁止开机...");
						g_pFrm->m_CmdRun.AddMsg(str);
						return 0;
					}
				}*/
				if((0==g_pFrm->m_CmdRun.ProductParam.bMesOffline) && bStopPush)
				{
					bStopPush=false;
					g_pFrm->m_CmdRun.ProductParam.UDPSendMsgFlag = true;
					g_pFrm->m_CmdRun.bMesDataUpdate=false;   ////数据更新状态
					g_pFrm->m_CmdRun.bMaterialCodeNew=true;
					if(!UDPSendMsg())
					{
						AfxMessageBox("MES数据发送异常，请重新发送或检查网络!");
					}	
					/* CDlgMaterialCode dlg;
					if(IDOK==dlg.DoModal())
					{
					str.Format("流程单号已经发送到MES服务器...");
					g_pFrm->m_CmdRun.AddMsg(str);
					g_pFrm->m_CmdRun.PutLogIntoList(str);
					}*/
				}
				///////////////////////////////////////////////////////////////////////////
				if (((g_pFrm->m_CmdRun.ProductParam.DoorSwitch>0)&&(theApp.m_Mv400.GetInput(theApp.m_tSysParam.inPauseOrRun)))||(g_pFrm->m_CmdRun.ProductParam.DoorSwitch==0))					
				{
					g_pFrm->m_CmdRun.RunThreadPlay();
				}
			}

		}
		else if(lparam == 2)
		{
			str.Format("PlcReady清除！");
			g_pFrm->m_CmdRun.AddMsg(str);
			if(g_pFrm!=NULL)
			{
				g_pFrm->m_CmdRun.PutLogIntoList(str);
			}
			//g_pFrm->m_CmdRun.Stop();
			g_pFrm->m_CmdRun.RunStop();
		}
	}
	else if((inNo == theApp.m_tSysParam.inPlcAlarm))
	{
		if(((lparam == -2 )||(lparam == 0 ))&&(!PlcEorrTag))
		{
			CString strError;
			g_pFrm->m_CmdRun.AddMsg("PLC故障信号");
			strError.Format("PLC故障信号！");
			g_pFrm->m_CmdRun.PutLogIntoList(strError);

			PlcEorrTag=true;
			g_pFrm->m_CmdRun.m_bPlcRunError = true;
			Sleep(100);
			if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
			{
				theApp.m_Serial.ReadPlcData(&ErrorPlcData,"DM","1250");
				str.Format("PLC故障代码：PLC ErrorID = %d",ErrorPlcData);
				g_pFrm->m_CmdRun.PutLogIntoList(str);
				g_pFrm->m_CmdRun.AddMsg(str);
			}
			else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
			{
				theApp.m_Serial.ReadPlcData("1250");
			}

			if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
			{
				g_pFrm->m_CmdRun.Pause();
			}
		}
	}
	else if(inNo == theApp.m_tSysParam.inRailRun)
	{
		if((lparam == -2||lparam == 0)&&!g_pFrm->bIsCtrlDlgVisible)
		{
			g_pFrm->m_CmdRun.RailMotor(TRUE);
		}
		else if((lparam == 1||lparam == 2)&&!g_pFrm->bIsCtrlDlgVisible)
		{
			g_pFrm->m_CmdRun.RailMotor(FALSE);
		}
	}
	//else if(inNo == theApp.m_tSysParam.inDoorSwitch)
	//{
	//	if(lparam == 1|| lparam == 2)
	//	{
	//		g_pFrm->m_CmdRun.m_bDoorOpen = true;
	//		if(lparam == 2)
	//		{
	//			theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outDayLight,TRUE);
	//			//if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN||g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_PAUSE)
	//			//{
	//			//	g_pFrm->m_CmdRun.Stop();
	//			//}
	//		}
	//	}
	//	else if(lparam == -2|| lparam == 0)
	//	{
	//		g_pFrm->m_CmdRun.m_bDoorOpen = false;
	//		if(lparam == -2)
	//		{
	//			theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outDayLight,FALSE);
	//		}
	//	}
	//}
	return 0;
}

// 输出信号响应消息
LRESULT CDlgPosInfo::OnMessageOutputEvent(WPARAM wparam, LPARAM lparam)
{
	return 0;
}

// X轴、Y轴、Z轴、U轴位置状态响应函数
LRESULT CDlgPosInfo::OnMessagePosEvent(WPARAM wparam, LPARAM lparam)
{ 
	double *p = (double *)wparam;
	CString strTemp;
	for(int i=0; i<5; i++)
	{
		strTemp.Format("%.3f", p[i]);
		SetDlgItemText(IDC_EDIT_INFO_CURRENT_POS_X+i, strTemp);
		if (p[1]<100&&!PlcRTag&&g_pFrm->m_CmdRun.m_tStatus!= K_RUN_STS_NONE)
		{
			//theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outPlCRSafe,true);
			PlcRTag=true;  
		}
		if (p[1]>=100&&PlcRTag)
		{
			//theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outPlCRSafe,false);
			PlcRTag=false;  
		}
	}

	m_Static.Invalidate();
	long *pt=(long *)lparam;

	for (int i=0;i<5;i++)
	{
		if(g_pFrm->m_CmdRun.m_bHoming)
		{
			break;
		}
		if ((pt[i]&0x02)==0x02)//报警位触发
		{
			//wsg
			//////////////////////////////////////////////////////////////////////////
			//if(g_pFrm->bIsCtrlDlgVisible)
			//{
			//	g_pFrm->m_dlgCtrl.OnDlgExit();
			//}
			//if(g_pFrm->IsV9SetDlgVisible)
			//{
			//	g_pFrm->m_V9Param.OnV9SetExit();
			//}
			//////////////////////////////////////////////////////////////////////////
			theApp.m_tSysParam.ThreeAxHomeSu = false;
			g_pFrm->m_CmdRun.Alarm(0);
			theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outReady,FALSE);
		}
		else if((pt[i]&0x20)==0x20)
		{
			//if(g_pFrm->bIsCtrlDlgVisible)
			//{
			//	g_pFrm->m_dlgCtrl.OnDlgExit();
			//}
			//if(g_pFrm->IsV9SetDlgVisible)
			//{
			//	g_pFrm->m_V9Param.OnV9SetExit();
			//}
			//////////////////////////////////////////////////////////////////////////
			theApp.m_tSysParam.ThreeAxHomeSu = false;
			g_pFrm->m_CmdRun.Alarm(26);
			theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outReady,FALSE);
		}
		else if((pt[i]&0x40)==0x40)
		{
			//if(g_pFrm->bIsCtrlDlgVisible)
			//{
			//	g_pFrm->m_dlgCtrl.OnDlgExit();
			//}
			//if(g_pFrm->IsV9SetDlgVisible)
			//{
			//	g_pFrm->m_V9Param.OnV9SetExit();
			//}
			//////////////////////////////////////////////////////////////////////////
			theApp.m_tSysParam.ThreeAxHomeSu = false;
			g_pFrm->m_CmdRun.Alarm(27);
			theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outReady,FALSE);
		}
	}
	for(int i=0;i<3;i++)
	{
		strTemp.Format("%.3f",theApp.m_Mv400.GetPos(6+i));
		SetDlgItemText(IDC_EDIT_INFO_CURRENT_POS_FA+i, strTemp);
	}
	return 0;
}

LRESULT CDlgPosInfo::OnMessageGlueEvent(WPARAM wparam,LPARAM lparam)
{
	CString str;
	if(g_pFrm->m_CmdRun.ProductParam.UDPOffline)
	{
		return 0;
	}
	if(1==lparam)
	{
		//GetDlgItemText(IDC_EDIT_INFO_BARCODE,g_pFrm->m_CmdRun.ProductParam.UDPSendMsg);
		//g_pFrm->m_CmdRun.ProductParam.UDPSendMsgFlag = true;
		CString Sever_IP;
		int iLen,iSend;
		Sever_IP=g_pFrm->m_CmdRun.ProductParam.ServerIP;
		CString Date;
		//Date.Format("%s%d%s%s","ZJMO^",g_pFrm->m_CmdRun.ProductParam.FCompanyCode,"^",g_pFrm->m_CmdRun.ProductParam.UDPSendMsg);
		Date.Format("%d%s%s",g_pFrm->m_CmdRun.ProductParam.FCompanyCode,"+",g_pFrm->m_CmdRun.ProductParam.UDPSendMsg);
		iLen = Date.GetLength();
		char * sendData = (char*)Date.GetBuffer();
		iSend = m_uSocket.SendTo(LPCTSTR(Date),iLen, g_pFrm->m_CmdRun.ProductParam.ServerPort,Sever_IP);		

		if(SOCKET_ERROR==iSend)
		{
			MessageBox(_T("发送消息失败"));
			return 0;
		}
		else
		{
			CTime time;
			time = CTime::GetCurrentTime();
			CString m_strData;
			m_strData.Format(_T("%.2d:%.2d:%.2d"),time.GetHour(), time.GetMinute(), time.GetSecond());
			m_strData=m_strData+_T("【】【】自动运行--本地发送消息：   ")+Date;
			g_pFrm->m_CmdRun.AddMsg(m_strData);
			g_pFrm->m_CmdRun.PutLogIntoList(m_strData);
		}	
		//阀体相关数据更新
		//str.Format("开始刷新胶量...");
		//g_pFrm->m_CmdRun.AddMsg(str);
		//g_pFrm->m_pLog->log(str);
		//AutoSetGlue();
	}
	return 0;
}

void CDlgPosInfo::OnBnClickedBtnInfoReset()
{
	CString str;
	if (g_aLarm)
	{
		AfxMessageBox(_T("急停状态不能回原点!"));//wsg20130304
		return;
	}
	if (!g_pFrm->m_CmdRun.ManalRun)
	{
		AfxMessageBox(_T("自动模式不能手动回原点!"));//wsg20130304
		return;
	}
	//if(g_pFrm->bIsAlignDlgVisible||g_pFrm->bIsCalibVisible||g_pFrm->bIsCustomVisible||
	//	g_pFrm->bIsDebugHeightVisible||g_pFrm->bIsDebugNeedleVisible||g_pFrm->bIsRCDDlgVisible)
	//{
	//	AfxMessageBox("请关闭对话框，在启动复位！");
	//	return;
	//}
	if(g_pFrm->bAutoDlgVisible)
	{
		::SendMessage(g_pFrm->m_dlgAutoAlign.m_hWnd,WM_CLOSE,0,0);
		while(g_pFrm->bAutoDlgVisible)
		{
			CTimeUtil::Delay(10);
		}
	}
	if(g_pFrm->bImagePosVisible)
	{
		::SendMessage(g_pFrm->m_dlgImagePos.m_hWnd,WM_CLOSE,0,0);
		while(g_pFrm->bImagePosVisible)
		{
			CTimeUtil::Delay(10);
		}
	}
	if(g_pFrm->bIsAlignDlgVisible)
	{
		::SendMessage(g_pFrm->m_dlgAlignNeedle.m_hWnd,WM_CLOSE,0,0);
		while(g_pFrm->bIsAlignDlgVisible)
		{
			CTimeUtil::Delay(10);
		}
	}
	if(g_pFrm->bIsHeightCalibVisible)
	{
		::SendMessage(g_pFrm->m_dlgHeightCalib.m_hWnd,WM_CLOSE,0,0);
		while(g_pFrm->bIsHeightCalibVisible)
		{
			CTimeUtil::Delay(10);
		}
	}
	if(g_pFrm->bIsCustomVisible)
	{
		::SendMessage(g_pFrm->m_dlgCustom.m_hWnd,WM_CLOSE,0,0);
		while(g_pFrm->bIsCustomVisible)
		{
			CTimeUtil::Delay(10);
		}
	}
	if(g_pFrm->bIsDebugHeightVisible)
	{
		::SendMessage(g_pFrm->m_dlgDebugHeight.m_hWnd,WM_CLOSE,0,0);
		while(g_pFrm->bIsDebugHeightVisible)
		{
			CTimeUtil::Delay(10);
		}
	}
	if(g_pFrm->bIsDebugNeedleVisible)
	{
		::SendMessage(g_pFrm->m_dlgDebugNeedle.m_hWnd,WM_CLOSE,0,0);
		while(g_pFrm->bIsDebugNeedleVisible)
		{
			CTimeUtil::Delay(10);
		}
	}
	if(g_pFrm->bIsRCDDlgVisible)
	{
		::SendMessage(g_pFrm->m_dlgRCD.m_hWnd,WM_CLOSE,0,0);
		while(g_pFrm->bIsRCDDlgVisible)
		{
			CTimeUtil::Delay(10);
		}
	}
	if(g_pFrm->bIsCtrlDlgVisible)
	{
		//g_pFrm->m_dlgCtrl.OnDlgExit();
		::SendMessage(g_pFrm->m_dlgCtrl.m_hWnd,WM_CLOSE,0,0);
		while(g_pFrm->bIsCtrlDlgVisible)
		{
			CTimeUtil::Delay(10);
		}
	}
	if(g_pFrm->IsV9SetDlgVisible)
	{
		//g_pFrm->m_V9Param.OnV9SetExit();
		::SendMessage(g_pFrm->m_V9Param.m_hWnd,WM_CLOSE,0,0);
		while(g_pFrm->IsV9SetDlgVisible)
		{
			CTimeUtil::Delay(10);
		}
	}
	if(g_pFrm->bIsNeedleDlgVisible)
	{
		//g_pFrm->m_dlgNeedle.OnExitDlg();
		::SendMessage(g_pFrm->m_dlgNeedle.m_hWnd,WM_CLOSE,0,0);
		while(g_pFrm->bIsNeedleDlgVisible)
		{
			CTimeUtil::Delay(10);
		}
	}
	if(g_pFrm->bIsHeightDlgVisible)
	{
		//g_pFrm->m_dlgHeight.OnExitDlg();
		::SendMessage(g_pFrm->m_dlgHeight.m_hWnd,WM_CLOSE,0,0);
		while(g_pFrm->bIsHeightDlgVisible)
		{
			CTimeUtil::Delay(10);
		}
	}
	if(g_pFrm->m_CmdRun.m_bCleanNeedle||g_pFrm->m_CmdRun.m_bDischarge||g_pFrm->m_CmdRun.m_bTestDispense||
		g_pFrm->m_CmdRun.m_bSearchMark||g_pFrm->m_CmdRun.m_bSlotDetect||g_pFrm->m_CmdRun.m_bHoming||g_pFrm->m_CmdRun.m_bMeasureHeight||g_pFrm->m_CmdRun.m_bNeedleCalibrate||g_pFrm->m_CmdRun.m_bNeedleAutoAdjust)
	{
		CString strError;
		strError.Format("未知错误！");
		if(g_pFrm->m_CmdRun.m_bCleanNeedle)
		{
			strError.Format("清洗针头时，不能回原点！");
		}
		else if(g_pFrm->m_CmdRun.m_bDischarge)
		{
			strError.Format("排胶时，不能回原点！");
		}
		else if(g_pFrm->m_CmdRun.m_bTestDispense)
		{
			strError.Format("试胶时，不能回原点！");
		}
		else if(g_pFrm->m_CmdRun.m_bSearchMark)
		{
			strError.Format("查找Mark点时，不能回原点！");
		}
		else if(g_pFrm->m_CmdRun.m_bSlotDetect)
		{
			strError.Format("正反检测时，不能回原点！");
		}
		else if(g_pFrm->m_CmdRun.m_bHoming)
		{
			strError.Format("复位时，不能回原点！");
		}
		else if(g_pFrm->m_CmdRun.m_bMeasureHeight)
		{
			strError.Format("测高时，不能回原点！");
		}
		else if (g_pFrm->m_CmdRun.m_bNeedleCalibrate)
		{
			strError.Format("针头标定时，不能回原点！");
		}
		else if (g_pFrm->m_CmdRun.m_bNeedleAutoAdjust)
		{
			strError.Format("自动对针时，不能回原点！");
		}
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		return;
	}
	static bool b = false;
	if(b) return;	
	b = true;

	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN || g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_PAUSE||(g_pFrm->m_CmdRun.bIsThreadRunning))
	{
		AfxMessageBox("运行或暂停状态不能回原点!");
		b = false;
		return;
	}

	g_pFrm->m_CmdRun.m_bHoming = true;
	if (g_pFrm->m_CmdRun.RunErrorID==0)//伺服报警
	{
		for (unsigned short i=1;i<4;i++)
		{
			theApp.m_Mv400.AxisClrsts(i); 
		}
		Sleep(500);
		for(int i=1;i<4;i++)
		{
			long lAxisSts = 0;
			theApp.m_Mv400.GetAxisSts(i,&lAxisSts);
			if((lAxisSts&0x02)==0x02)
			{
				AfxMessageBox("伺服报警未清除，请断电重启后再试!");
				g_pFrm->m_CmdRun.m_bHoming = false;
				b = false;
				return;
			}
		}
		Sleep(500);
	}
	g_pFrm->m_CmdRun.RunErrorID=-1;
	g_pFrm->m_CmdRun.m_tStatus = K_RUN_STS_NONE;  //disable Toolbar
	g_pFrm->m_CmdRun.m_LastStatus = K_RUN_STS_NONE; //20180713
	((CMainFrame*)AfxGetMainWnd())->m_wndToolBar.EnableWindow(FALSE);
	//g_pFrm->m_CmdRun.m_tStatus = K_RUN_STS_NONE;
	//theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outDispenseFinish,TRUE);
	//Sleep(200);
	//theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outDispenseFinish,FALSE);//清料 PLC
	if(!theApp.m_Mv400.Home(50,5,1000))
	{
		AfxMessageBox("回原点失败请重新回零!");
		b = false;
		theApp.m_tSysParam.ThreeAxHomeSu=false;
		g_pFrm->m_CmdRun.m_bHoming = false;
		return;
	}
	if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
	{
		g_pFrm->m_CmdRun.Alarm(30);
		AfxMessageBox("设置电机参数错误！");
		b = false;
		theApp.m_tSysParam.ThreeAxHomeSu=false;
		g_pFrm->m_CmdRun.m_bHoming = false;
		return;
	}
	g_pFrm->m_CmdRun.m_bHoming = false;

	if (PlcEorrTag)
	{
		PlcEorrTag=false;
		bRecordError = false;
		g_pFrm->m_CmdRun.m_bPlcRunError = false;
	}

	//theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outPlCRSafe,true);
	//enable toolbar
	((CMainFrame*)AfxGetMainWnd())->m_wndToolBar.EnableWindow(TRUE);
	g_pFrm->m_CmdRun.m_tStatus = K_RUN_STS_STOP;
	g_pFrm->m_CmdRun.m_LastStatus = K_RUN_STS_STOP;//20180713
	theApp.m_tSysParam.ThreeAxHomeSu=true;
	g_pFrm->m_CmdRun.RunErrorID=-1;
	PlcRTag=false;
	ErrorVal=0;
	ErrorPlcData=0;
	bRecordError = false;
	b=false;
	b = false;
	if(theApp.m_tSysParam.V9HomeSu&&theApp.m_tSysParam.ThreeAxHomeSu)
	{
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outReady,TRUE);
	}
	else
	{
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outReady,FALSE);
		str.Format("ThreeAxHomeSu:%d,V9HomeSu:%d",theApp.m_tSysParam.ThreeAxHomeSu,
			theApp.m_tSysParam.V9HomeSu);
		g_pFrm->m_CmdRun.AddMsg(str);
	}
}

HBRUSH CDlgPosInfo::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)

{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if(nCtlColor==CTLCOLOR_DLG)
	{
		return (HBRUSH) m_brush.GetSafeHandle();
	}

	if(nCtlColor==CTLCOLOR_STATIC)
	{
		pDC->SetTextColor(RGB(0,0,0));
		pDC->SetBkMode(TRANSPARENT);//设置背景透明
		hbr = (HBRUSH)m_brush;
	}
      
	for (int i=0;i<14;i++)
	{
		if (pWnd->GetDlgCtrlID()==IDC_EDIT_INFO_CURRENT_POS_X+i)
		{	
			pDC->SetTextColor(RGB(255, 0, 0));
		}
	}
	if (pWnd->GetDlgCtrlID()==IDC_STATIC_INFO_ERROR)
	{
		if (ErrorVal==0)
		{
			pDC->SetTextColor(RGB(0,255,0));
		}
		else if (ErrorVal==30)
		{
			pDC->SetTextColor(RGB(255,255,0));
		}
		else if ((ErrorVal!=0)&&(ErrorVal!=30))
		{
			pDC->SetTextColor(RGB(255,0,0));
		}
		else if (PlcEorrTag)
		{
			pDC->SetTextColor(RGB(255,0,0));
		}
	}
	if(pWnd->GetDlgCtrlID()==IDC_STATIC_INFO_REGISTER)
	{
		if(lRegisterStatus==0)
		{
			pDC->SetTextColor(RGB(0,255,0));
		}
		else if(lRegisterStatus==1)
		{
			pDC->SetTextColor(RGB(0,0,255));
		}
		else if(lRegisterStatus==2)
		{
			pDC->SetTextColor(RGB(255,0,0));
		}
		else
		{
			pDC->SetTextColor(RGB(255,255,0));
		}
	}
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}
// 显示生产统计
void CDlgPosInfo::DisplayWorkStat(void)
{
	CString str;
	UpdateTimes();
	SetDlgItemText(IDC_EDIT_INFO_TOTAL_TIME,g_pFrm->m_CmdRun.StautDisp.W_TotalTime.Format("%H : %M : %S"));
	SetDlgItemText(IDC_EDIT_INFO_STANDBY_TIME,g_pFrm->m_CmdRun.StautDisp.W_StandbyTime.Format("%H : %M : %S"));
	SetDlgItemText(IDC_EDIT_INFO_RUN_TIME,g_pFrm->m_CmdRun.StautDisp.W_RunTime.Format("%H : %M : %S"));
	//str.Format("%ld",g_pFrm->m_CmdRun.StautDisp.W_Count);
	if(theApp.m_tSysParam.StatCapacity>2000000)
	{
		theApp.m_tSysParam.StatCapacity = theApp.m_tSysParam.StatCapacity - 2000000;
	}
	
	str.Format("%ld",theApp.m_tSysParam.StatCapacity);
	SetDlgItemText(IDC_EDIT_INFO_PRODUCT_COMPLETED,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.StautDisp.W_SingeTimer);
	SetDlgItemText(IDC_EDIT_INFO_ST,str);
	str.Format("%0.1f",g_pFrm->m_CmdRun.StautDisp.W_UPH);
	SetDlgItemText(IDC_EDIT_INFO_UPH,str);

	if ((K_RUN_STS_STOP==g_pFrm->m_CmdRun.m_tStatus)&&(g_pFrm->m_CmdRun.ProductParam.lGasketRingLife>0))
	{
		if (theApp.m_tSysParam.StatCapacity>(g_pFrm->m_CmdRun.ProductParam.lGasketRingLife*1000))
		{
			g_pFrm->m_CmdRun.RunErrorID=40;   //Can assure alarm always;
			if (bLogGlueEnable)
			{
				CString str;
				str.Format("密封圈寿命到，机械臂准备好信号被清除！");
				g_pFrm->m_CmdRun.PutLogIntoList(str);
				bLogGlueEnable=false;
			}

		}		
	}	   
}
// 读取错误的文本
bool CDlgPosInfo::ReadEorrTxt(void)
{
	CString  filename;
	CString  errorTempID,SatauTempID,ReswrveTempID;
	filename=CFunction::GetDirectory()+"system\\Plctext.ini";
	ErrorCount=::GetPrivateProfileInt("ErrorInfo","ErrorCount",NULL,filename);

	if (ErrorCount>0)
	{
		PlcError =new CString[ErrorCount];
		for (int i=0;i<ErrorCount;i++)
		{
			errorTempID.Format("ErrorText%d",i);
			::GetPrivateProfileString("ErrorText",errorTempID,NULL,PlcError[i].GetBuffer(128),128,filename);
			PlcError[i].ReleaseBuffer();
		}
	}
	else
	{
		ErrorCount=0;
		return false;
	}
	return true;
}

// 清除报警信号
void CDlgPosInfo::OnBnClickedBtnInfoClearError()
{
	CString str;
	if(g_aLarm )
	{
		return;
	}
	if((g_pFrm->m_CmdRun.RunErrorID>=42)&&(theApp.m_SysUser.m_CurUD.level<GM_ROOT))
	{
		return;
	}
	if((g_pFrm->m_CmdRun.RunErrorID==21)&&(theApp.m_SysUser.m_CurUD.level<GM_ROOT))
	{
		return;
	}
	if((ErrorPlcData>=27)&&(ErrorPlcData<=33)&&(theApp.m_SysUser.m_CurUD.level<GM_ROOT))//防塌线权限解除
	{
		return;
	}
	theApp.m_Mv400.m_bExit=false;
	static bool b = false;
	if(b)return;
	b=true;
	if (PlcEorrTag)
	{
		//theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outPlcRest, true);
		//Sleep(200);
		//theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outPlcRest, false);
		Sleep(1000);
		PlcEorrTag=false;
		bRecordError = false;
		g_pFrm->m_CmdRun.m_bPlcRunError = false;
	}
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outRed, false);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outGreen ,true);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outYellow, false);
	if (g_pFrm->m_CmdRun.RunErrorID==0||g_pFrm->m_CmdRun.RunErrorID==26||g_pFrm->m_CmdRun.RunErrorID==27)//伺服报警、正负限位触发
	{
		if (!theApp.m_tSysParam.ThreeAxHomeSu)
		{
			OnBnClickedBtnInfoReset();
		}
		g_pFrm->m_CmdRun.V9RestOK=false;
		theApp.m_tSysParam.V9HomeSu=false;
	}
	g_pFrm->m_CmdRun.RunErrorID=-1;
	PlcRTag=false;
	ErrorVal=0;
	ErrorPlcData=0;
	bRecordError = false;
	b=false;
	if(theApp.m_tSysParam.V9HomeSu&&theApp.m_tSysParam.ThreeAxHomeSu)
	{
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outReady,TRUE);
	}
	else
	{
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outReady,FALSE);
		str.Format("ThreeAxHomeSu:%d,V9HomeSu:%d",theApp.m_tSysParam.ThreeAxHomeSu,theApp.m_tSysParam.V9HomeSu);
		g_pFrm->m_CmdRun.AddMsg(str);
	}
}

void CDlgPosInfo::OnContextMenu(CWnd* pWnd, CPoint point)
{
	//CDialog::OnContextMenu(pWnd, point);
}

void CDlgPosInfo::OnBnClickedBtnInfoClearCapacity()
{
	// TODO: Add your control notification handler code here
	//g_pFrm->m_CmdRun.StautDisp.W_Count = 0;
	theApp.m_tSysParam.StatCapacity = 0;
}

void CDlgPosInfo::UpdateTimes()
{
	CTimeSpan timeSpan;
	g_pFrm->m_CmdRun.m_EndT = CTime::GetCurrentTime();
	timeSpan = g_pFrm->m_CmdRun.m_EndT - g_pFrm->m_CmdRun.m_StarT;
	g_pFrm->m_CmdRun.m_StarT = g_pFrm->m_CmdRun.m_EndT;
	g_pFrm->m_CmdRun.StautDisp.W_TotalTime += timeSpan;
	if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_RUN)
	{
		g_pFrm->m_CmdRun.StautDisp.W_RunTime += timeSpan;
	}
	g_pFrm->m_CmdRun.StautDisp.W_StandbyTime = g_pFrm->m_CmdRun.StautDisp.W_TotalTime - g_pFrm->m_CmdRun.StautDisp.W_RunTime;
}

void CDlgPosInfo::OnBnClickedBtnInfoDebug()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_RUN)
	{
		AfxMessageBox("运行状态不能进行生产调试！");
		return;
	}
	if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_PAUSE)
	{
		AfxMessageBox("暂停状态不能进行生产调试！");
	}

	if(AfxMessageBox("确定进行位置标定？",MB_YESNO)==IDYES)
	{
		g_pFrm->m_dlgLabel.DoModal();
	}

	//if(AfxMessageBox("确定进行针头测高？",MB_YESNO)==IDYES)
	//{
	//	g_pFrm->m_dlgDebugHeight.DoModal();
	//}
	if(AfxMessageBox("确定进行针头测高？",MB_YESNO)==IDYES)
	{
		if(!g_pFrm->m_CmdRun.CheckHeightEx(K_AXIS_ZA))
		{
			AfxMessageBox("A阀测高失败！");
			return;
		}
		if(!g_pFrm->m_CmdRun.CheckHeightEx(K_AXIS_ZB))
		{
			AfxMessageBox("B阀测高失败！");
			return;
		}
		if(!g_pFrm->m_CmdRun.CheckHeightEx(K_AXIS_ZC))
		{
			AfxMessageBox("C阀测高失败！");
			return;
		}
	}

	if(AfxMessageBox("确定进行排胶清洗？",MB_YESNO)==IDYES)
	{
		if(!g_pFrm->m_CmdRun.DischargePosition(true))
		{
			AfxMessageBox("排胶清洗失败！");
			return;
		}
	}
	if(g_pFrm->m_CmdRun.ProductParam.dispSequence == CLEAN_LATER_TEST)
	{
		if(AfxMessageBox("确定进行清洗针头？",MB_YESNO)==IDYES)
		{
			if(!g_pFrm->m_CmdRun.CleanNeedle(true))
			{
				AfxMessageBox("清洗针头失败！");
				return;
			}
		}
		if(AfxMessageBox("确定进行首点排胶？",MB_YESNO)==IDYES)
		{
			if(!g_pFrm->m_CmdRun.TestDispense())
			{
				AfxMessageBox("首点排胶失败！");
				return;
			}
		}
	}
	else
	{
		if(AfxMessageBox("确定进行首点排胶？",MB_YESNO)==IDYES)
		{
			if(!g_pFrm->m_CmdRun.TestDispense())
			{
				AfxMessageBox("首点排胶失败！");
				return;
			}
		}
		if(AfxMessageBox("确定进行清洗针头？",MB_YESNO)==IDYES)
		{
			if(!g_pFrm->m_CmdRun.CleanNeedle(true))
			{
				AfxMessageBox("清洗针头失败！");
				return;
			}
		}
	}
}

void CDlgPosInfo::OnBnClickedBtnInfoMovetoCleanPos()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能移动到清洗位!");
		return;
	}
	//if (!g_pFrm->m_CmdRun.IsRotatingLightClose())
	//{
	//	if (IDOK != AfxMessageBox("旋转气缸未关闭到位，\n按 确定 忽略提示继续运动到目标位置", MB_OKCANCEL))
	//	{
	//		return;
	//	}
	//}
	if (g_pFrm->m_CmdRun.ManalRun)
	{
		g_pFrm->m_CmdRun.CleanNeedle(false);
	}
}

void CDlgPosInfo::OnBnClickedBtnInfoMovetoDischargePos()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能移动到排胶位!");
		return;
	}
	//if (!g_pFrm->m_CmdRun.IsRotatingLightClose())
	//{
	//	if (IDOK!=AfxMessageBox("旋转气缸未关闭到位，\n按 确定 忽略提示继续运动到目标位置", MB_OKCANCEL))
	//	{
	//		return;
	//	}
	//}
	if (g_pFrm->m_CmdRun.ManalRun)
	{
		g_pFrm->m_CmdRun.DischargePosition(false);
	}
}

void CDlgPosInfo::OnBnClickedBtnInfoClearSlot()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ClearPadDetect();
	g_pFrm->m_CmdRun.ProductParam.tgTrackInfo.TrackReset();
	g_pFrm->m_CmdRun.m_bStepOne=true;//综合模式画线步骤...
}

void CDlgPosInfo::DisplayGlueVol()
{
	if(g_pFrm!=NULL)
	{
		CString str;
		str.Format("%0.3f",g_pFrm->m_CmdRun.V9Parm[0].DispVolume);
		SetDlgItemText(IDC_EDIT_INFO_GLUE_A,str);
		str.Format("%0.3f",g_pFrm->m_CmdRun.V9Parm[1].DispVolume);
		SetDlgItemText(IDC_EDIT_INFO_GLUE_B,str);
		str.Format("%0.3f",g_pFrm->m_CmdRun.V9Parm[2].DispVolume);
		SetDlgItemText(IDC_EDIT_INFO_GLUE_C,str);
	}
}
void CDlgPosInfo::OnBnClickedBtnInfoClearDetect()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ClearPadDetect();
}

void CDlgPosInfo::SetAuthority()
{
	//GetDlgItem(IDC_BTN_INFO_MOVETO_CLEAN_POS)->EnableWindow(!g_pFrm->m_CmdRun.m_bLockScreen);
	//GetDlgItem(IDC_BTN_INFO_MOVETO_DISCHARGE_POS)->EnableWindow(!g_pFrm->m_CmdRun.m_bLockScreen);
	//GetDlgItem(IDC_BTN_INFO_DEBUG)->EnableWindow(!g_pFrm->m_CmdRun.m_bLockScreen);
	//GetDlgItem(IDC_BTN_INFO_CLEAR_ERROR)->EnableWindow(!g_pFrm->m_CmdRun.m_bLockScreen);
	//GetDlgItem(IDC_BTN_INFO_RESET)->EnableWindow(!g_pFrm->m_CmdRun.m_bLockScreen);
    GetDlgItem(IDC_BTN_INFO_CLEAR_CAPACITY)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
    GetDlgItem(IDC_BTN_VALVE_UPDATE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	//GetDlgItem(IDC_BUTTON_PRODUCTIVITYR)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
}
void CDlgPosInfo::OnBnClickedBtnManualLoad()
{
	// TODO: Add your control notification handler code here
	CString str;
	str.Format("1308");
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcData(1,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcROut(str,true);
	}
	Sleep(100);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcData(0,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcROut(str,false);
	}
}

void CDlgPosInfo::OnBnClickedBtnManualUnload()
{
	// TODO: Add your control notification handler code here
	CString str;
	str.Format("1309");
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcData(1,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcROut(str,true);
	}
	Sleep(100);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcData(0,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcROut(str,false);
	}
}

void CDlgPosInfo::OnBnClickedBtnFirstInPos()
{
	// TODO: Add your control notification handler code here
	CString str;
	int nBitNum = 1205;
	str.Format("%d",nBitNum);
	if(g_pFrm!=NULL)
	{
		g_pFrm->m_CmdRun.AddMsg(str);
	}
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcData(1,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcROut(str,true);
	}
	Sleep(200);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcData(0,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcROut(str,false);
	}
}

void CDlgPosInfo::OnBnClickedBtnFirstOutPos()
{
	// TODO: Add your control notification handler code here
	CString str;
	int nBitNum = 1213;
	str.Format("%d",nBitNum);
	if(g_pFrm!=NULL)
	{
		g_pFrm->m_CmdRun.AddMsg(str);
	}
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcData(1,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcROut(str,true);
	}
	Sleep(200);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcData(0,"R",str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		theApp.m_Serial.WritePlcROut(str,false);
	}
}

void CDlgPosInfo::OnEnChangeEditGlueSetting()
{
	CString str;
	GetDlgItemText(IDC_EDIT_GLUE_SETTING,str);
	g_pFrm->m_CmdRun.ProductParam.ArrangeSpan=atof(str);
	theApp.ProductParam(FALSE);
	
}

void CDlgPosInfo::OnBnClickedCheckInfoEnableArrange()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.ArrangeEnable=((CButton*)GetDlgItem(IDC_CHECK_INFO_ENABLE_ARRANGE))->GetCheck();
	theApp.ProductParam(FALSE);
}

void CDlgPosInfo::OnBnClickedBtnInfoArrangeTime()
{
	//TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.ArrangeTime=CTimeUtil::GetCurTime();
	theApp.ProductParam(FALSE);
	bLogGlueEnable=true;
}

//初始化界面显示//胶水监控
void CDlgPosInfo::UpdateArrangeTime()
{
	CString str;
	str.Format("%.3f",g_pFrm->m_CmdRun.ProductParam.ArrangeSpan);
	SetDlgItemText(IDC_EDIT_GLUE_SETTING,str);
	if(g_pFrm->m_CmdRun.ProductParam.ArrangeEnable==0||g_pFrm->m_CmdRun.ProductParam.ArrangeEnable==1)
	{
		((CButton*)GetDlgItem(IDC_CHECK_INFO_ENABLE_ARRANGE))->SetCheck(g_pFrm->m_CmdRun.ProductParam.ArrangeEnable);
	}
}
void CDlgPosInfo::DisplayRemainSpan()
{
	CString str;
	long lCurrentTime=CTimeUtil::GetCurTime();
	dRemainSpan=0.0;//初始化剩余时间值；
	if ((lCurrentTime-g_pFrm->m_CmdRun.ProductParam.ArrangeTime)>0&&((lCurrentTime-g_pFrm->m_CmdRun.ProductParam.ArrangeTime)<g_pFrm->m_CmdRun.ProductParam.ArrangeSpan*60*1000))
	{
		dRemainSpan=g_pFrm->m_CmdRun.ProductParam.ArrangeSpan-(double)(lCurrentTime-g_pFrm->m_CmdRun.ProductParam.ArrangeTime)/60000;

	}
	str.Format("%.3f",dRemainSpan);
	SetDlgItemText(IDC_EDIT_GLUE_REAL,str);
}
bool CDlgPosInfo::TemperatureCheck()
{
	long lReadData;
	bool bReadData;
	double dReadData;
	double dErrorValue;
	bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1242");
	if(!bReadData)
	{
		return false;
	}
	dReadData = lReadData*MM_FACTOR;
	///////////////////////////////////////////////////////////////////
	dErrorValue=g_pFrm->m_CmdRun.ProductParam.dHeatSetting-dReadData;
	if (fabs(dErrorValue)<g_pFrm->m_CmdRun.ProductParam.dTempError)
	{
		return true;
	}
	else
	{
		return false;
	}
}
//定时复位权限
void CDlgPosInfo::InitManager()
{
	if (theApp.m_SysUser.m_CurUD.level==GM_GUEST)
	{
		return;
	} 

	if (!bStartTime)
	{
		StartTime = GetTickCount();
		bStartTime=true;
	}

	DWORD EndTime = GetTickCount();
	if ((EndTime-StartTime)/1000>(theApp.m_tSysParam.iTimeLogOut*60))//设定时间自动注销
	{
		theApp.m_SysUser.m_CurUD.level = GM_GUEST;
		theApp.m_SysUser.m_CurUD.uID = 0;
		g_pFrm->m_bAdmin = FALSE; 
		bStartTime=false; 
	}

}
void CDlgPosInfo::OnBnClickedBtnValveUpdate()
{
	g_pFrm->m_CmdRun.ProductParam.ValveMonitorTime=CTimeUtil::GetCurTime();
	theApp.ProductParam(FALSE);	
}
void CDlgPosInfo::DisplayValveRemainSpan()
{
	CString str;
	long lCurrentTime=CTimeUtil::GetCurTime();
	dValveRemainSpan=0.0;
	if ((lCurrentTime-g_pFrm->m_CmdRun.ProductParam.ValveMonitorTime)>0&&((lCurrentTime-g_pFrm->m_CmdRun.ProductParam.ValveMonitorTime)<g_pFrm->m_CmdRun.ProductParam.ValveMonitorSpan*60*60*1000))
	{
		dValveRemainSpan=g_pFrm->m_CmdRun.ProductParam.ValveMonitorSpan*60-(double)(lCurrentTime-g_pFrm->m_CmdRun.ProductParam.ValveMonitorTime)/60000.0f;
	}
	str.Format("余%.3f分",dValveRemainSpan);
	SetDlgItemText(IDC_EDIT_VALVE_REMAIN,str);

}

BOOL CDlgPosInfo::PreTranslateMessage(MSG* pMsg)
{
	//if((WM_KEYDOWN==pMsg->message)&&(13==pMsg->wParam))
	//{
	//	//if (GetDlgItem(IDC_EDIT_INFO_BARCODE)==GetFocus())//仅仅处理此编辑框
	//	if (GetFocus()->GetDlgCtrlID()==IDC_EDIT_INFO_BARCODE)
	//	{
	//		CString str;
	//		CString substr[10];
	//		//S1:此状态数据无效
	//		if (K_RUN_STS_STOP!=g_pFrm->m_CmdRun.m_tStatus)
	//		{
	//			g_pFrm->m_CmdRun.AddMsg("非停止状态条码数据不生效！");
	//			return true;
	//		}

	//		GetDlgItemText(IDC_EDIT_INFO_BARCODE,str);
	//		/*if (NULL!=g_pFrm)
	//		{
	//		g_pFrm->SetWindowText(str);
	//		}*/
	//		//S2:数据预处理////////
	//		str = str.Trim(_T(" "));
	//		int count=0;
	//		int index = str.Find(_T(","));
	//		while (index != -1 && count<BARCODE_MAX_FIELD_NUM)  //最长9段字符
	//		{
	//			CFunction::DoEvents();
	//			substr[count++] = str.Left(index);
	//			str=str.Right(str.GetLength()-index-1);
	//			index = str.Find(_T(","));
	//		}
	//		substr[count++]=str;
	//		//compute/process data 
	//		//S1：字符长度有效性判断
	//		if (BARCODE_FIELD_NUM!=count)
	//		{
	//			g_pFrm->m_CmdRun.AddMsg("条码字符段数异常！");
	//			return true;
	//		}			
	//		//S3:*************************************************************************数据1处理
	//		//double dGlueVolumnNew=atof(substr[0]);
	//		double dGlueVolumnNew[3];
	//		for (int i=0;i<3;i++)
	//		{
	//			dGlueVolumnNew[i]=atof(substr[i]);
	//		}

	//		long lDispCountOld;
	//		double dFirstCompvel; 
	//		double dCompVel;
	//		long lDispNum;
	//		for (int i=0;i<3;i++)
	//		{
	//			lDispCountOld = g_pFrm->m_CmdRun.V9Parm[i].DispCount;	//单程次数
	//			dFirstCompvel = g_pFrm->m_CmdRun.V9Parm[i].FirstCompVol;	// 第一点胶位补偿胶量
	//			lDispNum = g_pFrm->m_CmdRun.V9Parm[i].DispNum;			// 点胶次数
	//			g_pFrm->m_CmdRun.V9Parm[i].DispVolume = dGlueVolumnNew[i];
	//			dCompVel = g_pFrm->m_CmdRun.V9Parm[i].CompVolume;			//补偿胶量
	//			g_pFrm->m_CmdRun.V9Parm[i].RestDistance = g_pFrm->m_CmdRun.CapRestDistance(dGlueVolumnNew[i],dCompVel,20000,&g_pFrm->m_CmdRun.V9Parm[i].DispCount,&g_pFrm->m_CmdRun.V9Parm[i].DispDistance,dFirstCompvel);
	//			/*	if (lDispNum>g_pFrm->m_CmdRun.V9Parm[i].DispCount)
	//			{
	//			g_pFrm->m_CmdRun.V9Parm[i].DispNum=g_pFrm->m_CmdRun.V9Parm[i].DispCount;
	//			}*/
	//			if (g_pFrm->m_CmdRun.V9Parm[i].DispCount>10)
	//			{
	//				g_pFrm->m_CmdRun.V9Parm[i].DispNum=g_pFrm->m_CmdRun.V9Parm[i].DispCount-10;
	//			}
	//			else
	//			{
	//				g_pFrm->m_CmdRun.V9Parm[i].DispNum=g_pFrm->m_CmdRun.V9Parm[i].DispCount;
	//			}
	//		}
	//		theApp.V9Param(false);
	//		g_pFrm->m_CmdRun.V9RestOK=false;    //数据修改,毕阀复位
	//		//*****************************************************************************数据1处理
	//		Sleep(1000);
	//		str.Format("");
	//		SetDlgItemText(IDC_EDIT_INFO_BARCODE,str);	
	//		return true;
	//	}
	//	return true;
	//}

	if(WM_KEYDOWN==pMsg->message)
	{
		//防止点击编辑框后按下回车键和退出键出现退出对话框界面
		if (VK_RETURN==pMsg->wParam||VK_ESCAPE==pMsg->wParam)
		{
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgPosInfo::CanEnterStop()
{
	if (theApp.m_SysUser.m_CurUD.level!=GM_GUEST)
		return;
	if ((g_pFrm->m_CmdRun.m_LastStatus!=g_pFrm->m_CmdRun.m_tStatus)&&(K_RUN_STS_STOP==g_pFrm->m_CmdRun.m_tStatus))
	{
		lTimeInStop = CTimeUtil::GetCurTime();
	}
	if (g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_RUN||K_RUN_STS_PAUSE==g_pFrm->m_CmdRun.m_tStatus)
	{
		lTimeInStop = CTimeUtil::GetCurTime();
	}
	if (g_pFrm->m_CmdRun.m_tStatus!=g_pFrm->m_CmdRun.m_LastStatus)
	{
		g_pFrm->m_CmdRun.m_LastStatus=g_pFrm->m_CmdRun.m_tStatus;
	}
	if(((CTimeUtil::GetCurTime()-lTimeInStop)/1000)>300) //5 min
	{
		theApp.m_Serial.SendHaltToMES();
		lTimeInStop = CTimeUtil::GetCurTime();
		CString str;
		str.Format("--->ToMes__CMD:STOP...");
		g_pFrm->m_CmdRun.AddMsg(str);
		//g_pFrm->m_pLog->log(str);
	}
}
void CDlgPosInfo::OnBnClickedButtonProductivityr()
{
	theApp.m_tSysParam.StatCapacity = 0;
}

void CDlgPosInfo::OnBnClickedInfoUdpSendmsg()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItemText(IDC_EDIT_INFO_BARCODE,g_pFrm->m_CmdRun.ProductParam.UDPSendMsg);
	//g_pFrm->m_CmdRun.ProductParam.UDPSendMsgFlag = true;
    UDPSendMsg();
}
bool CDlgPosInfo::OnReceive()
{
	char	*pBuf = new char[1025];
	CString	strRecvd;//接收的数据信息字符串
	//BYTE	*pBuf = new BYTE[1025];
	int		iBufSize = 1024;
	int		iRecvd;
	int		iLen;
	// 接收信息，并返回接收到的字节数
	iRecvd = m_uSocket.Receive(pBuf, iBufSize);
	//iRecvd = m_uSocket.ReceiveFrom(pBuf,iBufSize,hostaddr_tmp,&iLen);
	// 判断是否有错误发生
	if(iRecvd == SOCKET_ERROR)
	{
		delete pBuf;
		return false;
	}
	else
	{
		// 利用NULL来截去接收缓存区中多余的字符
		pBuf[iRecvd] = NULL;
		// 将接收到的有用的信息赋值给CString变量
		strRecvd = pBuf;
		CString rev_GlueA;
		CString rev_GlueB;
		CString rev_GlueC;
		CString rev_Status;
		CString T_rev_Status;

		rev_GlueA = g_pFrm->m_CmdRun.ReadProdutMag(_T("GlueA"),strRecvd);
		g_pFrm->m_CmdRun.m_Glue_msg.rev_MESGlue[0] = atof(rev_GlueA);
		rev_GlueB = g_pFrm->m_CmdRun.ReadProdutMag(_T("GlueB"),strRecvd);
		g_pFrm->m_CmdRun.m_Glue_msg.rev_MESGlue[1] = atof(rev_GlueB);
		rev_GlueC = g_pFrm->m_CmdRun.ReadProdutMag(_T("GlueC"),strRecvd);
		g_pFrm->m_CmdRun.m_Glue_msg.rev_MESGlue[2] = atof(rev_GlueC);
		//strRecvd = "M0:1100060943;Status:3;";
		g_pFrm->m_CmdRun.m_Glue_msg.rev_CodeID = g_pFrm->m_CmdRun.ReadProdutMag(_T("MO"),strRecvd);
		if (g_pFrm->m_CmdRun.ProductParam.Save_rev_CodeID != g_pFrm->m_CmdRun.m_Glue_msg.rev_CodeID)//胶杯号已经不同了，提醒排胶           
		{
			CString str;
			str.Format("获取胶杯号：%s，存储的胶杯号：%s", g_pFrm->m_CmdRun.m_Glue_msg.rev_CodeID, g_pFrm->m_CmdRun.ProductParam.Save_rev_CodeID);
			//if (g_pFrm->m_CmdRun.ProductParam.Save_rev_CodeID.CompareNoCase(g_pFrm->m_CmdRun.m_Glue_msg.rev_CodeID))//胶杯号已经不同了，提醒排胶//不同返回非0值
			
			g_pFrm->m_CmdRun.AddMsg(str);
			g_pFrm->m_CmdRun.ProductParam.Save_rev_CodeID.Format("%s", g_pFrm->m_CmdRun.m_Glue_msg.rev_CodeID);
			
			str.Format("获取胶杯号：%s，存储的胶杯号：%s", g_pFrm->m_CmdRun.m_Glue_msg.rev_CodeID, g_pFrm->m_CmdRun.ProductParam.Save_rev_CodeID);
			g_pFrm->m_CmdRun.AddMsg(str);
			theApp.ProductParam(FALSE);
			rev_Status = g_pFrm->m_CmdRun.ReadProdutMag(_T("Status"),strRecvd);
			// 1:多排胶;2:连续加胶;3:清洗腔体;4:新粉验证;5:正常排胶
			if(rev_Status.Find(_T("1"))!=-1||rev_Status.Find(_T("3"))!=-1||rev_Status.Find(_T("4"))!=-1||rev_Status.Find(_T("5")) != -1)
			{
				g_pFrm->m_CmdRun.m_Glue_msg.rev_bIsDischarge = true;
			}
			else
			{
				g_pFrm->m_CmdRun.m_Glue_msg.rev_bIsDischarge = false;
			}
		}

		strRecvd = _T("-->服务器消息：  ") + strRecvd;
		iLen = strRecvd.GetLength();
		g_pFrm->m_CmdRun.AddMsg(strRecvd);	
		g_pFrm->m_CmdRun.PutLogIntoList(strRecvd);

		if(0 == g_pFrm->m_CmdRun.ProductParam.bMesOffline)
		{
			g_pFrm->m_CmdRun.m_Glue_msg.rev_MESGlue[0]*=1000;
			g_pFrm->m_CmdRun.m_Glue_msg.rev_MESGlue[1]*=1000;
			g_pFrm->m_CmdRun.m_Glue_msg.rev_MESGlue[2]*=1000;
			AutoSetGlue();
			g_pFrm->m_CmdRun.bMesDataUpdate=true;
		}			

	}
	delete pBuf;
	return true;
}
void CDlgPosInfo::AutoSetGlue(void)
{	
	CString str;
	CString substr[10];
	double dGlueVolumnNew[3];
	for(int i=0;i<3;i++)
	{
		dGlueVolumnNew[i]=g_pFrm->m_CmdRun.m_Glue_msg.rev_MESGlue[i];
	}

	long lDispCountOld;
	double dFirstCompvel; 
	double dCompVel;
	long lDispNum;
	for (int i=0;i<3;i++)
	{
		lDispCountOld = g_pFrm->m_CmdRun.V9Parm[i].DispCount;	//单程次数
		dFirstCompvel = g_pFrm->m_CmdRun.V9Parm[i].FirstCompVol;	// 第一点胶位补偿胶量
		lDispNum = g_pFrm->m_CmdRun.V9Parm[i].DispNum;			// 点胶次数
		g_pFrm->m_CmdRun.V9Parm[i].DispVolume = dGlueVolumnNew[i];
		dCompVel = g_pFrm->m_CmdRun.V9Parm[i].CompVolume;			//补偿胶量
		g_pFrm->m_CmdRun.V9Parm[i].RestDistance = g_pFrm->m_CmdRun.CapRestDistance(dGlueVolumnNew[i],dCompVel,20000,&g_pFrm->m_CmdRun.V9Parm[i].DispCount,&g_pFrm->m_CmdRun.V9Parm[i].DispDistance,dFirstCompvel);
		
		if (g_pFrm->m_CmdRun.V9Parm[i].DispCount>10)
		{
			g_pFrm->m_CmdRun.V9Parm[i].DispNum=g_pFrm->m_CmdRun.V9Parm[i].DispCount-10;
		}
		else
		{
			g_pFrm->m_CmdRun.V9Parm[i].DispNum=g_pFrm->m_CmdRun.V9Parm[i].DispCount;
		}
	}
	theApp.V9Param(false);
	g_pFrm->m_CmdRun.V9RestOK=false;    //数据修改,毕阀复位

}

bool CDlgPosInfo::UDPSendMsg()
{
	CString Sever_IP;
	Sever_IP=g_pFrm->m_CmdRun.ProductParam.ServerIP;
	CString Date;
	int iLen,iSend;
	Date.Format("%d%s%s",g_pFrm->m_CmdRun.ProductParam.FCompanyCode,"+ZJMO+",g_pFrm->m_CmdRun.ProductParam.MachineID);
	//Date.Format("%d%s%s",g_pFrm->m_CmdRun.ProductParam.FCompanyCode,"+",g_pFrm->m_CmdRun.ProductParam.UDPSendMsg);
	//Date.Format("%d",g_pFrm->m_CmdRun.ProductParam.FCompanyCode);
	iLen = Date.GetLength();
	char * sendData = (char*)Date.GetBuffer();
	iSend = m_uSocket.SendTo(LPCTSTR(Date),iLen, g_pFrm->m_CmdRun.ProductParam.ServerPort, Sever_IP);		
	if(SOCKET_ERROR==iSend)
	{
		CString strInfo;
		strInfo.Format("UDP发送消息失败:Error ={%d}",m_uSocket.GetLastError());
		MessageBox(strInfo);
		return false;
	}
	else
	{
		CTime time;
		time = CTime::GetCurrentTime();
		CString m_strData;
		m_strData.Format(_T("%.4d|%.2d:%.2d:%.2d"),time.GetYear(),time.GetHour(), time.GetMinute(), time.GetSecond());
		m_strData=m_strData+_T("本地发送消息:  ")+Date;
		g_pFrm->m_CmdRun.AddMsg(m_strData);
	}
	return true;
}

bool CDlgPosInfo::ReadTemperature()
{
	bool bReadData = false;
	long lReadData = 0;
	double dReadData = 0.0;
	CString str;

	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"R","1410");
		if(!bReadData)
		{
			return false;
		}
		if(lReadData==1)
		{
			bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1240");
			if(!bReadData)
			{
				return false;
			}
			dReadData = lReadData*MM_FACTOR;
		}
		g_pFrm->m_CmdRun.ProductParam.dRealPreHeatTemp = dReadData;


		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"R","1411");
		if(!bReadData)
		{
			return false;
		}
		if(lReadData==1)
		{
			bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1242");
			if(!bReadData)
			{
				return false;
			}
			dReadData = lReadData*MM_FACTOR;
		}		
		g_pFrm->m_CmdRun.ProductParam.dRealDispHeatTemp = dReadData;
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		lReadData = theApp.m_Serial.ReadPlcRData("1410");
		if(lReadData==1)
		{
			lReadData = theApp.m_Serial.ReadPlcData("1240");
			dReadData = lReadData*MM_FACTOR;
		}
		g_pFrm->m_CmdRun.ProductParam.dRealPreHeatTemp = dReadData;

		lReadData = theApp.m_Serial.ReadPlcData("1411");
		if(lReadData==1)
		{
			lReadData = theApp.m_Serial.ReadPlcData("1242");
			dReadData = lReadData*MM_FACTOR;
		}
		g_pFrm->m_CmdRun.ProductParam.dRealDispHeatTemp = dReadData;
	}

	return true;

}
