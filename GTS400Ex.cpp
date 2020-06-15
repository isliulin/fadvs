/*//////////////////////////////////////////////////////////////////////////////////
   GTS运动库 BY JSL 2012-7-2 VR1.0
1，打开缓冲区前用CheckListRun()建立坐标系和前瞻
2，进入缓冲区后不能用立即指令如MOVE 和MOVEXY指令要用ListMoveXYZ指令
3,指令压前瞻结束后用AllListRun()压入缓冲区
/////////////////////////////////////////////////////////////////////////////////////////////*/


#include "StdAfx.h"
#include "GTS400Ex.h"
#include "TimeUtil.h"
#include "TSCtrlSys.h"
#include <math.h>

#pragma comment(lib, "gts.lib")
#pragma comment(lib,"ExtMdl.lib")//IO扩展
#pragma  warning (disable:4244)
CCriticalSection gt_cs;
GTS400Ex *g_pGTS = NULL;
BOOL gt_bClose = FALSE;
CWinThread *gt_pThread=NULL;

GTS400Ex::GTS400Ex(void)
{
	m_bInitOK = FALSE;
	m_pListBox =NULL;
	m_pSysParam = NULL;
	for(int i=0;i<CARD_NUM;i++)
	{
		m_outputSts[i] = 0xFFFF;
		m_inputSts[i] = 0xFFFF;
	}

	m_bExit = FALSE;
	m_bStopEvent = FALSE;
	m_dCurVelEditXY=50;
	m_dCurVelEditZ = 50;
	m_hWnd = NULL;

	//m_tLookData = new TCrdData[20];
	ctrlMode=TYPE_TRAP;//立即指令
}

GTS400Ex::~GTS400Ex(void)
{
	FreeGE();
}

BOOL GTS400Ex::IsInitOK()
{
	return m_bInitOK;
}

void gLock()
{
	gt_cs.Lock();
}

void gUnLock()
{
	gt_cs.Unlock();
}

UINT gCheckInThread(LPVOID param)
{
	GTS400Ex *p = (GTS400Ex*)param;
	while(TRUE != gt_bClose)
	{
		Sleep(10);
		p->CheckGTStatus();
	}
	gt_pThread = NULL;
	return 0;
}

void GTS400Ex::WaitStop()
{	
	if(!m_bInitOK)
	{
		return;
	}
	do 
	{
		if(m_bExit)
		{
			break;
		}
		Sleep(5);
	} while(IsMove());
}

void GTS400Ex::WaitStop(int iAxisNo)
{	
	if(!m_bInitOK)
	{
		return;
	}
	do
	{
		if(m_bExit)
		{
			break;
		}
		Sleep(5);
	}while(IsMove(iAxisNo));
}
void GTS400Ex::WaitValvesStop()
{	
	if(!m_bInitOK)
	{
		return;
	}
	do 
	{
		if(m_bExit)
		{
			return;
		}
		if(!m_bInitOK)
		{
			return;
		}
		long axis_status[3] = {0};
		gLock();
		//for(int i=5;i<=8;i++)
		//{
		//	GT_GetSts(i,&axis_status[i-5],1);
		//}
		GT_GetSts(0,6,&axis_status[0],3);
		gUnLock();
		//if(!(axis_status[0]&(1<<10) || axis_status[1]&(1<<10) || axis_status[2]&(1<<10))|| axis_status[3]&(1<<10))
		if(((axis_status[0]&(1<<10))!=(1<<10))&&((axis_status[1]&(1<<10))!=(1<<10))&&((axis_status[2]&(1<<10))!=(1<<10)))
		{
			return;
		}
		Sleep(5);

	} while(1);

}
void GTS400Ex::SetMessageWnd(HWND hWnd)
{
	m_hWnd = hWnd;
}

void GTS400Ex::AddMsg(CString strText)
{
	if(m_pListBox != NULL)
	{
		CString strTemp;
		strTemp.Format("%04d: ", g_pGTS->m_pListBox->GetCount());
		g_pGTS->m_pListBox->AddString(strTemp + strText);
		g_pGTS->m_pListBox->SetCurSel(g_pGTS->m_pListBox->GetCount()-1);
	}
}

//****************************************数据转换******************************************************//
void GTS400Ex::Trans2Pulse(double *pX, double *pY, double *pZ, double *pU, double *pVel, double *pAcc)
{
	if(pX != NULL) *pX /= m_pSysParam->tAxis[0].scale;
	if(pY != NULL) *pY /= m_pSysParam->tAxis[1].scale;
	if(pZ != NULL) *pZ /= m_pSysParam->tAxis[2].scale;
	if(pU != NULL) *pU /= m_pSysParam->tAxis[3].scale;
	if(pVel != NULL) *pVel /= (m_pSysParam->tAxis[0].scale*1000);
	if(pAcc != NULL) *pAcc /= (m_pSysParam->tAxis[0].scale*1000*1000);//20190920
}

void GTS400Ex::Trans2Pulse(int iAxisNo,double *pX, double *pVel, double *pAcc)
{
	if(pX != NULL) *pX /= m_pSysParam->tAxis[iAxisNo-1].scale;
	if(pVel != NULL) *pVel /= (m_pSysParam->tAxis[iAxisNo-1].scale*1000);
	if(pAcc != NULL) *pAcc /= (m_pSysParam->tAxis[iAxisNo-1].scale*1000*1000);//20190920
}

void GTS400Ex::Trans2MM(double *pX, double *pY, double *pZ, double *pU, double *pVel, double *pAcc)
{
	if(pX != NULL) *pX *= m_pSysParam->tAxis[0].scale;
	if(pY != NULL) *pY *= m_pSysParam->tAxis[1].scale;
	if(pZ != NULL) *pZ *= m_pSysParam->tAxis[2].scale;
	if(pU != NULL) *pU *= m_pSysParam->tAxis[3].scale;
	if(pVel != NULL) *pVel *= (m_pSysParam->tAxis[0].scale*1000);
	if(pAcc != NULL) *pAcc *= (m_pSysParam->tAxis[0].scale*1000*1000);//20190920
}

void GTS400Ex::Trans2MM(int iAxisNo,double *pX,  double *pVel,double *pAcc)
{
	if(pX != NULL) *pX *= m_pSysParam->tAxis[iAxisNo-1].scale;
	if(pVel != NULL) *pVel *= (m_pSysParam->tAxis[iAxisNo-1].scale*1000);
	if(pAcc != NULL) *pAcc *= (m_pSysParam->tAxis[iAxisNo-1].scale*1000*1000); //20190920
}

//***************************************数据修正*******************************************************//
void GTS400Ex::CheckPos(double *pX, double *pY, double *pZ, double *pU)
{
	if(pX != NULL)
	{
		if(*pX < m_pSysParam->tAxis[0].minpos) *pX = m_pSysParam->tAxis[0].minpos;
		if(*pX > m_pSysParam->tAxis[0].maxpos) *pX = m_pSysParam->tAxis[0].maxpos;
	}
	if(pY != NULL)
	{
		if(*pY < m_pSysParam->tAxis[1].minpos) *pY = m_pSysParam->tAxis[1].minpos;
		if(*pY > m_pSysParam->tAxis[1].maxpos) *pY = m_pSysParam->tAxis[1].maxpos;
	}
	if(pZ != NULL)
	{
		if(*pZ < m_pSysParam->tAxis[2].minpos) *pZ = m_pSysParam->tAxis[2].minpos;
		if(*pZ > m_pSysParam->tAxis[2].maxpos) *pZ = m_pSysParam->tAxis[2].maxpos;
	}
	if(pU != NULL)
	{
		if(*pU < m_pSysParam->tAxis[3].minpos) *pU = m_pSysParam->tAxis[3].minpos;
		if(*pU > m_pSysParam->tAxis[3].maxpos) *pU = m_pSysParam->tAxis[3].maxpos;
	}
}

void GTS400Ex::CheckPos(int iAxisNo,double *pX)
{
	if(pX != NULL)
	{
		if(*pX < m_pSysParam->tAxis[iAxisNo-1].minpos) *pX = m_pSysParam->tAxis[iAxisNo-1].minpos;
		if(*pX > m_pSysParam->tAxis[iAxisNo-1].maxpos) *pX = m_pSysParam->tAxis[iAxisNo-1].maxpos;
	}
}
//===================Begin======================
//初始化
//===================End =======================
BOOL GTS400Ex::InitGE(CListBox *pListBox, tgSysParam *pSysParam, CLogFile *pLog)
{	
	//注意 ; 要建立坐标系!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	m_pListBox = pListBox;
	m_pSysParam = pSysParam;
	m_pLog = pLog;

	g_pGTS = this;
	CString strTemp;
	AddMsg("开始初始化.....");

	if(m_bInitOK)
	{
		return TRUE;
	}
	gLock();
	for(int i=0;i<CARD_NUM;i++)
	{
		if(GT_Open(i))
		{
			gUnLock();
			CString strText;
			strText.Format("运动卡%d初始化失败",i);
			AddMsg(strText);
			m_pLog->log(strText);
			gt_pThread=AfxBeginThread(/*(AFX_THREADPROC)*/gCheckInThread, this);
			m_bInitOK = FALSE;
			return FALSE;
		}
	}
	////if (GT_OpenExtMdl("gts.dll")) 
	////{
	////	strTemp.Format(_T("GT_OpenExtMdl error,error code"));
	////	m_pLog->log(strTemp);
	////	return FALSE;
	////}
	//////加载配置文件，配置IO扩展模块
	////if (GT_LoadExtConfig("ExtModuleAD.cfg"))//AD模块
	////{
	////	strTemp.Format(_T("GT_LoadExtMdlConfig error,error code"));
	////	m_pLog->log(strTemp);
	////	m_bInitOK = FALSE;
	////	return FALSE;
	////}
	gUnLock();
	gLock();
	for(int n=0;n<CARD_NUM;n++)
	{
		rtn = GT_Reset(n);
		Sleep(100);
		rtn = GT_LmtSns(n,0xFFFF);//1:low voltage trigger //every axis has 2bits to set +/-limits;
		int i=0;
		int nAxisNo = 8;
		for(i=1;i<=nAxisNo;i++)
		{
			if((n==0&&i<6)||(n==1&&i<5))
			{
				rtn = GT_AlarmOn(n,i);
			}
			else
			{
				rtn = GT_AlarmOff(n,i);//only 3 valves disable the alarm info;
			}
			rtn = GT_StepDir(n,i); //out+dir 
			Sleep(30);
			rtn = GT_AxisOn(n,i);  //servon on
			//if ((1==n)&&((1==i)||(3==i)))  //X方向两调整马达无需 SOV-ON（LG Only）; 
			//{
				//rtn = GT_AxisOff(n,i);    

			//}
			
			rtn = GT_LmtsOn(n,i,MC_LIMIT_POSITIVE);//set litmit+ active
			rtn = GT_LmtsOn(n,i,MC_LIMIT_NEGATIVE);//set litmit- active
			rtn = GT_ClrSts(n,i,1); //update the setting upon;
			Sleep(30);
		}
		if (1==n)  //2号卡取反 
		{
			unsigned short sValue=0;
			sValue|=1<<0;
			sValue|=1<<1; //X1调整
			//sValue|=1<<2;
			//sValue|=1<<3;
			sValue|=1<<4;//X2调整
			sValue|=1<<5;
			sValue|=1<<2; //2016/10/11 axis B  //low voltage trigger
			sValue|=1<<3; //2016/10/11 axis B  //low voltage trigger
			sValue|=1<<6; //2016/10/11 axis D  //low voltage trigger
			sValue|=1<<7; //2016/10/11 axis D  //low voltage trigger
			short ret=GT_LmtSns(n,sValue);
		}
	}
	SetStopDec(K_AXIS_X,10000.0,15000.0);
	SetStopDec(K_AXIS_Y,10000.0,15000.0);
	SetStopDec(K_AXIS_ZA,10000.0,15000.0);
	SetStopDec(K_AXIS_ZB,10000.0,15000.0);
	SetStopDec(K_AXIS_ZC,10000.0,15000.0);
	gUnLock();

	////===============TEST=======================
	////GT_LoadConfig("test.cfg");
	////===================TEST End =======================
	gt_pThread=AfxBeginThread(gCheckInThread, this);
	m_bInitOK = TRUE;
	m_bExit = FALSE;
	strTemp.Format("运动卡初始化正常.");
	AddMsg(strTemp);
	m_pLog->log(strTemp);
	return TRUE;
}

void GTS400Ex::FreeGE()
{
	while(gt_pThread != NULL)
	{
		Sleep(5);
	}
	Sleep(200);
	if(m_bInitOK)
	{
		m_bInitOK = FALSE;
		GT_CloseExtMdl();
		CloseGE();
		gLock();
		for(int n=0;n<CARD_NUM;n++)
		{
			GT_Close(n);
		}
		gUnLock();
	}


	TRACE(_T("g_pThread Exit begin \n"));
	if(gt_pThread != NULL)
	{
		switch( WaitForSingleObject(gt_pThread->m_hThread, 500) )
		{
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			TerminateThread(gt_pThread->m_hThread, 0);
			break;
		}

		gt_pThread = NULL;
	}
	TRACE(_T("g_pThread Exit End \n"));

}

void GTS400Ex::CloseGE() //OK
{
	m_bExit = TRUE;
	if(!m_bInitOK) return;
	StopMove();
	SetOutput(K_ALL_PORTS, FALSE);
	for(int n=0;n<CARD_NUM;n++)
	{
		if(n==0)
		{
			for(int i=1;i<6;i++)
			{
				gLock();
				GT_AxisOff(n,i);
				gUnLock();
				Sleep(30);
			}
		}
		else
		{
			for(int i=1;i<=4;i++)
			{
				gLock();
				GT_AxisOff(n,i);
				gUnLock();
				Sleep(30);
			}
		}
	}
}
//停止运动
void GTS400Ex::StopMove()
{
	AddMsg("停止运行");
	if(!m_bInitOK) return;
	gLock();
	for(int n=0;n<CARD_NUM;n++)
	{
		GT_Stop(n,511,511);
	}
	//GT_Stop(511,511);
	gUnLock();
}
//x-y-z1-z2-z3三轴是否在运动
BOOL GTS400Ex::IsMove()
{
	if(!m_bInitOK) return FALSE;
	
	long axis_status[5] = {0};
	gLock();
	for(int i=1;i<6;i++)
	{
		GT_GetSts(0,i,&axis_status[i-1]);
	}
	gUnLock();
	return(axis_status[0]&(1<<10) || axis_status[1]&(1<<10) || axis_status[2]&(1<<10) || axis_status[3]&(1<<10) || axis_status[4]&(1<<10));
}
//ABCD 是否在运动
BOOL GTS400Ex::IsMoveAdjustMotor()
{
	if(!m_bInitOK)
	{
		return FALSE;
	}

	long axis_status[4] = {0};
	gLock();
	for(int i=1;i<5;i++)  //1203 zwg 4->5
	{
		GT_GetSts(1,i,&axis_status[i-1]);
	}
	gUnLock();
	return(axis_status[0]&(1<<10) || axis_status[1]&(1<<10) || axis_status[2]&(1<<10) || axis_status[3]&(1<<10));
}

BOOL GTS400Ex::IsMoveList()
{
	if(!m_bInitOK) return FALSE;
	CString str;
	short runstatus;
	long crd_status;
	gLock();
	rtn = GT_CrdStatus(0,1,&runstatus,&crd_status);
	gUnLock();
	if(rtn!=0)
	{
		str.Format("GT_CrdStatus=%d",rtn);
		AddMsg(str);
	}
	if(1 == runstatus)
	{
		return TRUE;
	}
	for(int j=0;j<8;j++)
	{
		gLock();
		rtn = GT_GetSts(0,j+1,&crd_status);
		gUnLock();
		if(rtn!=0)
		{
			str.Format("GT_CrdStatus=%d",rtn);
			AddMsg(str);
		}
		if((crd_status&0x400)==0x400)
		{
			return TRUE;
		}
	}
	return FALSE;
}


//*******************************************封装**************************************************//
//bit 10 =1 IsMoving
BOOL GTS400Ex::IsMove(int iAxisNo)
{
	if(!m_bInitOK) return FALSE;
	long axis_status=0;
	int nTempCard = (iAxisNo-1)/8;
	int nTempAxis = (iAxisNo-1)%8;
	gLock();
	GT_GetSts(nTempCard,nTempAxis+1,&axis_status);
	gUnLock();
	return(axis_status&(1<<10));
}
//*Pvalue   unit:pulse
double GTS400Ex::GetPos(int iAxisNo) //OK
{
	if(!m_bInitOK) 
	{
		if(m_pSysParam != NULL)
			return (m_pSysParam->tAxis[iAxisNo-1].maxpos *rand()/RAND_MAX + m_pSysParam->tAxis[iAxisNo-1].minpos);
		return -1;
	}

	double crd_prf_pos; 
	int nTempCard = (iAxisNo-1)/8;
	int nTempAxis = (iAxisNo-1)%8;
	gLock();
	GT_GetPrfPos(nTempCard,nTempAxis+1,&crd_prf_pos);
	gUnLock();
	return (crd_prf_pos*m_pSysParam->tAxis[iAxisNo-1].scale);
}

//*Pvalue   unit:pulse
double GTS400Ex::GetEncPos(int iAxisNo)
{
	int nTempCard = (iAxisNo-1)/8;
	int nTempAxis = (iAxisNo-1)%8;
	double dEncPos = 0;

	short ret = GT_GetEncPos(nTempCard,nTempAxis+1,&dEncPos);
	if(ret)
	{
		return 0;
	}
	return dEncPos*m_pSysParam->tAxis[iAxisNo-1].scale;
}

//*pValue   unit:pulse/ms 
double GTS400Ex::GetVel(int iAxisNo)
{
	if(!m_bInitOK) 
	{	
		return -1;
	}

	double crd_prf_vel;
	int nTempCard = (iAxisNo-1)/8;
	int nTempAxis = (iAxisNo-1)%8;
	gLock();
	GT_GetPrfVel(nTempCard,nTempAxis+1,&crd_prf_vel);
	gUnLock();
	return crd_prf_vel*m_pSysParam->tAxis[iAxisNo-1].scale;
}

//DO:set the target value to some port
void GTS400Ex::SetOutput(int bitNO, BOOL bOn, BOOL bUpdate)
{
	if(K_NONE_PORTS == bitNO)return;

	if(bUpdate)
	{
		CString strTemp;
		strTemp.Format("IO/ PortNo.%d --> %d",bitNO, bOn);
		AddMsg(strTemp);
	}
	if(!m_bInitOK) return;

	if (bitNO == K_ALL_PORTS)
	{
		for(int n=0;n<CARD_NUM;n++)
		{
			if(bOn)
			{
				m_outputSts[n] = 0x0000;
			}
			else
			{
				m_outputSts[n] = 0xFFFF;
			}
			gLock();
			GT_SetDo(n,MC_GPO,m_outputSts[n]);
			gUnLock();
		}
		//gLock();
		//GT_SetExtIoValue(0,m_outputSts[0]);
		//gUnLock();
	}
	else
	{	
		int nTempCard = bitNO/16;
		int nTempNO = bitNO%16;
		if(nTempCard>CARD_NUM)
		{
			gLock();
			GT_SetExtIoBit(0,nTempNO,!bOn);
			gUnLock();
		}
		else
		{
			gLock();
			GT_SetDoBit(nTempCard,MC_GPO,nTempNO+1,!bOn);
			gUnLock();
		}
	}
}

//DI:get the value of some port
BOOL GTS400Ex::GetInput(int bitNO)
{
	if(!m_bInitOK) return 0;
	long inputStatus;
	short rtn;
	unsigned short inputStatus2;

	int nTempCard = bitNO/16;
	int nTempNO = bitNO%16;
	if(nTempCard>CARD_NUM)
	{
		gLock();
		rtn = GT_GetExtIoValue(0,&inputStatus2);
		gUnLock();
		if((inputStatus2 & (1<<((int)nTempNO-(nTempCard-1)*16)))!=0)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		gLock();
		inputStatus = 0;
		GT_GetDi(nTempCard,MC_GPI,&inputStatus);
		gUnLock();
		if((inputStatus & (1<<(int)nTempNO))!=0)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
}

//get the status of DO
BOOL GTS400Ex::GetOutput(int bitNO)
{
	if(!m_bInitOK) return 0;
	long outputStatus;

	int nTempCard = bitNO/16;
	int nTempNO = bitNO%16;

	gLock();
	outputStatus = 0;
	GT_GetDo(nTempCard,MC_GPO,&outputStatus);
	gUnLock();
	if((outputStatus & (1<<(int)nTempNO))!=0)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void GTS400Ex::Move(int iAxisNO, double dPos, double dVel,double dAcc, BOOL bWaitStop, BOOL bCheckPos)//立即模式
{
	//if(fabs(dPos -GetPos(iAxisNO)) < 0.001 || dVel<0.01 || dAcc<0.01)return;
	if(m_bExit)
	{
		return;
	}
	CString strTemp;
	TTrapPrm pPrm;
	strTemp.Format("Move(%d, %.3f, v_%.3f, acc_%.3f, W%d", iAxisNO, dPos, dVel, dAcc, bWaitStop);
	AddMsg(strTemp);
	if(!m_bInitOK) return;
	double prf_pos;
	//if (ctrlMode!=TYPE_TRAP)
	//{
	//	SetAllAxisTrap();
	//}
	if(iAxisNO==K_AXIS_X||iAxisNO==K_AXIS_Y||iAxisNO==K_AXIS_ZA||iAxisNO==K_AXIS_ZB||iAxisNO==K_AXIS_ZC)
	{
		SetAllAxisTrap();
		ctrlMode=TYPE_TRAP;
	}
	switch(iAxisNO)
	{
	case K_AXIS_X:
	case K_AXIS_Y:
	case K_AXIS_ZA:
	case K_AXIS_ZB:
	case K_AXIS_ZC:
	case K_AXIS_A:
	case K_AXIS_B:
	case K_AXIS_C:
	case K_AXIS_D:
		if(bCheckPos)
		{
			CheckPos(iAxisNO,&dPos);
		}
		Trans2Pulse(iAxisNO,&dPos,&dVel,&dAcc);
		strTemp.Format("MoveToPluse(%d, %.3f, v_%.3f, acc_%.3f, W%d", iAxisNO, dPos, dVel, dAcc, bWaitStop);
		AddMsg(strTemp);
		break;
	default:
		return;
	}
	int nTempCard = (iAxisNO-1)/8;
	int nTempAxis = (iAxisNO-1)%8;
	pPrm.acc = dAcc;
	pPrm.dec = dAcc;
	pPrm.smoothTime = 0;
	pPrm.velStart = 0;
	gLock();
	GT_GetPrfPos(nTempCard,nTempAxis+1,&prf_pos);
	prf_pos = dPos;
	GT_ClrSts(nTempCard,nTempAxis+1);

	GT_PrfTrap(nTempCard,nTempAxis+1);
	GT_SetTrapPrm(nTempCard,nTempAxis+1,&pPrm);
	GT_SetPos(nTempCard,nTempAxis+1,prf_pos);
	GT_SetVel(nTempCard,nTempAxis+1,dVel);
	GT_Update(nTempCard,1<<nTempAxis);
	gUnLock();
	if(bWaitStop)
	{
		WaitStop(iAxisNO);
	}
}
void GTS400Ex::MoveDot(int iAxisNO, double dPos, double dVel,double dAcc, BOOL bWaitStop, BOOL bCheckPos)//其它四轴为立即模式的运动
{
	//if(fabs(dPos -GetPos(iAxisNO)) < 0.001 || dVel<0.01 || dAcc<0.01)return;
	if(m_bExit)
	{
		return;
	}
	CString strTemp;
	TTrapPrm pPrm;
	//strTemp.Format("Movedot(%d, %.3f, v_%.3f, acc_%.3f, W%d", iAxisNO, dPos, dVel, dAcc, bWaitStop);
	//AddMsg(strTemp);
	if(!m_bInitOK || IsMove(iAxisNO)) return;
	double prf_pos;
	if(bCheckPos)CheckPos(iAxisNO,&dPos);
	Trans2Pulse(iAxisNO,&dPos, &dVel, &dAcc);
  
	int nTempCard = (iAxisNO-1)/8;
	int nTempAxis = (iAxisNO-1)%8;
	if(nTempCard==0&&nTempAxis<2)
	{
		ctrlMode=TYPE_TRAP;
	}
	pPrm.acc = dAcc;
	pPrm.dec = dAcc;
	pPrm.smoothTime = 0;
	pPrm.velStart = 0;
	gLock();
	GT_GetPrfPos(nTempCard,nTempAxis+1,&prf_pos);
	prf_pos = dPos;
	GT_ClrSts(nTempCard,nTempAxis+1);
	GT_PrfTrap(nTempCard,nTempAxis+1);
	GT_SetTrapPrm(nTempCard,nTempAxis+1,&pPrm);
	GT_SetPos(nTempCard,nTempAxis+1,prf_pos);
	GT_SetVel(nTempCard,nTempAxis+1,dVel);
	GT_Update(nTempCard,1<<nTempAxis);
	gUnLock();
	if(bWaitStop)
	{
		WaitStop(iAxisNO);
	}
}

void GTS400Ex::MoveXY(double dX, double dY, double dVel, double dAcc, BOOL bWaitStop, BOOL bCheckPos)//立即模式的插补
{
	if( (fabs(dX -GetPos(K_AXIS_X)) < 0.001 && fabs(dY -GetPos(K_AXIS_Y)) < 0.001)|| dVel<0.01 || dAcc<0.01)	return;
	if(IsMove(K_AXIS_X)||IsMove(K_AXIS_Y)) return;
	if(m_bExit)return;
	CString strTemp;
	
	if(!m_bInitOK) return;	
	TCrdPrm crdPrm;
	double prf_pos[4];
	memset(&crdPrm,0,sizeof(crdPrm));

	if(bCheckPos)CheckPos(&dX, &dY, NULL, NULL);//实际值计算
	Trans2Pulse(&dX, &dY, NULL, NULL, &dVel, &dAcc);//脉冲值计算

	strTemp.Format("MoveXY(x_%.3f, y_%.3f, v_%.3f, acc_%.3f, W%d", dX, dY, dVel, dAcc, bWaitStop);
	AddMsg(strTemp);
	prf_pos[K_AXIS_X-1] = dX;
	prf_pos[K_AXIS_Y-1] = dY;

	SetCrdPrm(FALSE,1,2);
	gLock();
	GT_LnXYG0(0,1,prf_pos[0],prf_pos[1],dVel,dAcc,0);
	GT_CrdStart(0,1,0);
	gUnLock();
	if(bWaitStop)
	{
		//WaitStop();
		WaitStop(K_AXIS_X);
		WaitStop(K_AXIS_Y);
	}
}

void GTS400Ex::MoveBtn(int iAxisNo, int iMvSpeed/*0,1,2*/, int iDir/*-1, +1*/)
{
	if( (iAxisNo == K_AXIS_X)||(iAxisNo == K_AXIS_Y)||(iAxisNo == K_AXIS_A)||(iAxisNo == K_AXIS_B)||(iAxisNo == K_AXIS_C)||(iAxisNo == K_AXIS_D))
	{
		Move(iAxisNo,iDir<0 ? m_pSysParam->tAxis[iAxisNo-1].minpos : m_pSysParam->tAxis[iAxisNo-1].maxpos,
			m_pSysParam->tAxis[iAxisNo-1].vel[0]*iMvSpeed/2000.0, m_pSysParam->tAxis[iAxisNo-1].acc[0]*iMvSpeed/2000.0, FALSE);
	}

	else if(iAxisNo == K_AXIS_ZA||iAxisNo == K_AXIS_ZB||iAxisNo == K_AXIS_ZC)
	{
		if (iDir < 0)
		{
			Move(iAxisNo,m_pSysParam->tAxis[iAxisNo-1].minpos, m_pSysParam->tAxis[iAxisNo-1].vel[0]*iMvSpeed/2000.0, 
				m_pSysParam->tAxis[iAxisNo-1].acc[0]*iMvSpeed/2000.0, FALSE);
		}
		else
		{
			Move(iAxisNo, m_pSysParam->tAxis[iAxisNo-1].maxpos, m_pSysParam->tAxis[iAxisNo-1].vel[0]*iMvSpeed/2000.0, 
				m_pSysParam->tAxis[iAxisNo-1].acc[0]*iMvSpeed/2000.0, FALSE);
		}
	}
}
//单轴复位
BOOL GTS400Ex::HomeAxis(unsigned short axis, double pos, double offset, double homeOffset, double high_Vel, double low_Vel, double dAcc)
{
	if(!m_bInitOK)
	{
		return FALSE;
	}
	CString str;
	long sts;
	int nTempCard = (axis-1)/8;
	int nTempAxis = (axis-1)%8;
	gLock();
	GT_ClrSts(nTempCard,nTempAxis+1);
	GT_ZeroPos(nTempCard,nTempAxis+1);
	gUnLock();
	MoveDot(axis,pos,high_Vel,dAcc,TRUE,FALSE);
	gLock();
	GT_GetSts(nTempCard,nTempAxis+1,&sts);
	gUnLock();
	if(!(sts&(1<<6)))
	{
		return FALSE;
	}

	gLock();
	GT_ClrSts(nTempCard,nTempAxis+1);
	GT_ZeroPos(nTempCard,nTempAxis+1);
	gUnLock();

	MoveDot(axis,offset,high_Vel,dAcc,TRUE,FALSE);
	str.Format("Home:%0.3f",GetPos(axis));
	AddMsg(str);
	MoveDot(axis,pos,low_Vel,dAcc,TRUE,FALSE);

	gLock();
	GT_GetSts(nTempCard,nTempAxis+1,&sts);
	gUnLock();
	if(!(sts&(1<<6)))
	{
		return FALSE;
	}
	gLock();
	GT_ClrSts(nTempCard,nTempAxis+1);
	GT_ZeroPos(nTempCard,nTempAxis+1);
	gUnLock();
	MoveDot(axis,homeOffset,high_Vel,dAcc,TRUE,FALSE);
	Sleep(200);
	gLock();
	GT_ClrSts(nTempCard,nTempAxis+1);
	GT_ZeroPos(nTempCard,nTempAxis+1);
	gUnLock();
	return TRUE;
}
BOOL GTS400Ex::HomeServoAixs(unsigned short axis, double pos, double offset, double vel_high, double vel_low, double dAcc)
{
	if(!m_bInitOK)
	{
		return FALSE;
	}
	CString str;
	TTrapPrm trapPrm;
	short rtn,capture;
	long status,capturePos;
	double prfPos,encPos,axisPrfPos,axisEncPos;
	int nTempCard = (axis-1)/8;
	int nTempAxis = (axis-1)%8;
	Trans2Pulse(axis,&pos, &vel_high, &dAcc);
	Trans2Pulse(axis,&offset, &vel_low,NULL);
	// 启动Home捕获
	rtn = GT_SetCaptureMode(nTempCard,nTempAxis+1,CAPTURE_HOME);
	if(0!=rtn)
	{
		str.Format("GT_SetCaptureMode()=%d",rtn);
		AddMsg(str);
		m_pLog->log(str);
		return FALSE;
	}
	// 切换到点位运动模式
	rtn = GT_PrfTrap(nTempCard,nTempAxis+1);
	if(0!=rtn)
	{
		str.Format("GT_PrfTrap()=%d",rtn);
		AddMsg(str);
		m_pLog->log(str);
		return FALSE;
	}
	// 读取点位模式运动参数
	rtn = GT_GetTrapPrm(nTempCard,nTempAxis+1,&trapPrm);
	if(0!=rtn)
	{
		str.Format("GT_GetTrapPrm()=%d",rtn);
		AddMsg(str);
		m_pLog->log(str);
		return FALSE;
	}
	trapPrm.acc = dAcc;
	trapPrm.dec = dAcc;
	// 设置点位模式运动参数
	rtn = GT_SetTrapPrm(nTempCard,nTempAxis+1,&trapPrm);
	if(0!=rtn)
	{
		str.Format("GT_SetTrapPrm()=%d",rtn);
		AddMsg(str);
		m_pLog->log(str);
		return FALSE;
	}
	// 设置点位模式目标速度，即回原点速度
	rtn = GT_SetVel(nTempCard,nTempAxis+1,vel_low);
	if(0!=rtn)
	{
		str.Format("GT_SetVel()=%d",rtn);
		AddMsg(str);
		m_pLog->log(str);
		return FALSE;
	}
	// 设置点位模式目标位置，即原点搜索距离
	rtn = GT_SetPos(nTempCard,nTempAxis+1,pos);
	if(0!=rtn)
	{
		str.Format("GT_SetPos()=%d",rtn);
		AddMsg(str);
		m_pLog->log(str);
		return FALSE;
	}
	// 启动运动
	rtn = GT_Update(nTempCard,1<<nTempAxis);
	if(0!=rtn)
	{
		str.Format("GT_Update()=%d",rtn);
		AddMsg(str);
		m_pLog->log(str);
		return FALSE;
	}

	do
	{
		// 读取轴状态
		rtn = GT_GetSts(nTempCard,nTempAxis+1,&status);
		// 读取捕获状态
		rtn = GT_GetCaptureStatus(nTempCard,nTempAxis+1,&capture,&capturePos);
		// 读取规划位置
		rtn = GT_GetPrfPos(nTempCard,nTempAxis+1,&prfPos);
		// 读取编码器位置
		rtn = GT_GetEncPos(nTempCard,nTempAxis+1,&encPos);
		str.Format("capture = %d,prfPos = %.3f,encPos = %.3f",capture,prfPos,encPos);
		AddMsg(str);
		if(0==(status&0x400))
		{
			return FALSE;
		}
	}while(0==capture);
	str.Format("capture pos = %.3f",capturePos);
	AddMsg(str);
	// 运动到“捕获位置+偏移量”
	rtn = GT_SetPos(nTempCard,nTempAxis+1,capturePos + offset);
	// 在运动状态下更新目标位置
	rtn = GT_Update(nTempCard,1<<nTempAxis);
	do 
	{
		// 读取轴状态
		rtn = GT_GetSts(nTempCard,nTempAxis+1,&status);
		// 读取规划位置
		rtn = GT_GetPrfPos(nTempCard,nTempAxis+1,&prfPos);
		// 读取编码器位置
		rtn = GT_GetEncPos(nTempCard,nTempAxis+1,&encPos);
		str.Format("status = 0x%,prfPos = %.3f,encPos = %.3f",status,prfPos,encPos);
		AddMsg(str);
	} while (status&0x400);
	// 检查是否到达“Home捕获位置+偏移量”
	if(prfPos!=pos+offset)
	{
		return FALSE;
	}
	// 位置清零
	rtn = GT_ZeroPos(nTempCard,nTempAxis+1);
	// 读取规划位置
	rtn = GT_GetPrfPos(nTempCard,nTempAxis+1,&prfPos);
	// 读取编码器位置
	rtn = GT_GetEncPos(nTempCard,nTempAxis+1,&encPos);
	// 读取axis规划位置
	rtn = GT_GetAxisPrfPos(nTempCard,nTempAxis+1,&axisPrfPos);
	// 读取axis编码器位置
	rtn = GT_GetAxisEncPos(nTempCard,nTempAxis+1,&axisEncPos);
	str.Format("prfPos = %.3f,encPos = %.3f,axisPrfPos = %.3f,axisEncPos = %.3f",prfPos,encPos,axisPrfPos,axisEncPos);
	AddMsg(str);
	return TRUE;
}

bool GTS400Ex::HomeXY()//负限位回零方式//修正FAD9200回零方法先Y后X
{
	long sts;

	gLock();
	GT_ClrSts(0,K_AXIS_Y);
	GT_ZeroPos(0,K_AXIS_Y);
	gUnLock();

	Delay(0.1);

	Move(K_AXIS_Y, -100000,50, 1000, TRUE, FALSE);
	//MoveXY(-1000,-1000,50, 2000, TRUE, FALSE);
	gLock();
	GT_GetSts(0,K_AXIS_Y, &sts);
	gUnLock();
	if(!(sts & (1<<6)))
	{
		Move(K_AXIS_Y, -100000, 50, 1000, TRUE, FALSE);
		gLock();
		GT_GetSts(0,K_AXIS_Y, &sts);
		gUnLock();
		if(!(sts & (1<<6)))
		{
			return false;
		}
	}
	gLock();
	GT_ClrSts(0,K_AXIS_Y);
	GT_ZeroPos(0,K_AXIS_Y);
	gUnLock();
	Move(K_AXIS_Y,5,50,1000,TRUE,FALSE);
	Move(K_AXIS_Y,-1000,10,1000,TRUE,FALSE);
	gLock();
	GT_GetSts(0,K_AXIS_Y,&sts);
	gUnLock();
	if(!(sts & (1<<6)))
	{
		return false;
	}
	
	gLock();
	GT_ClrSts(0,K_AXIS_X);
	gUnLock();
	Move(K_AXIS_X, -10000,50, 1000, TRUE, FALSE);
	sts = 0;
	gLock();
	GT_ZeroPos(0,K_AXIS_X);
	GT_GetSts(0,K_AXIS_X, &sts);
	gUnLock();
	if(!(sts & (1<<6)))
	{
		Move(K_AXIS_X, -100000,50, 1000, TRUE, FALSE);
		gLock();
		GT_GetSts(0,K_AXIS_X, &sts);
		gUnLock();
		if(!(sts & (1<<6)))
		{
			return false;
		}

	}
	gLock();
	GT_ClrSts(0,K_AXIS_X);
	GT_ZeroPos(0,K_AXIS_X);
	gUnLock();
	Move(K_AXIS_X,5,50,1000,TRUE,FALSE);
	Move(K_AXIS_X,-1000,10,1000,TRUE,FALSE);
	gLock();
	GT_GetSts(0,K_AXIS_X,&sts);
	gUnLock();
	if(!(sts & (1<<6)))
	{
		return false;
	}

	gLock();
	GT_ZeroPos(0,K_AXIS_X);
	GT_ZeroPos(0,K_AXIS_Y);
	gUnLock();
	MoveXY(5,5, 50, 2000, TRUE, FALSE);
	Sleep(200);
	gLock();
	GT_ClrSts(0,K_AXIS_X);
	GT_ClrSts(0,K_AXIS_Y);
	GT_ZeroPos(0,K_AXIS_X);
	GT_ZeroPos(0,K_AXIS_Y);
	gUnLock();
	return true;
}

bool GTS400Ex::AllListRun()
{
	//强制所有数据从前瞻进入运动缓冲区
	short run0;
	long segment,space;
	short rtn = GT_CrdData(0,1,NULL,0);
	//if(rtn)
	//{
	//	return false;
	//}
	//如果运动缓冲区无运动,重新启动
	rtn = GT_CrdStatus(0,1,&run0,&segment,0);
	//if(rtn)
	//{
	//	return false;
	//}
	if(0==run0)
	{
		rtn = GT_CrdSpace(0,1,&space,0);
		if(space<CRD_FIFO_MAX)
		{	
			rtn = GT_CrdStart(0,1,0);
			//if(rtn)
			//{
			//	return false;
			//}
		}
	}
	//等待停止,并监控运动和暂停
	WaitStop();
	//if(!WaitStop(1.0))
	//{
	//	return false;
	//}
	return true;
}

BOOL GTS400Ex::Home(double dHighVel, double dLowVel,double dAcc)
{
	if(!m_bInitOK)
	{
		return FALSE;
	}
	if(m_bExit)
	{
		return FALSE;
	}
	if(!HomeAxis(K_AXIS_ZA, -500, 5, 1, dHighVel, dLowVel,dAcc))
	{
		return FALSE;
	}
	if(!HomeAxis(K_AXIS_ZB, -500, 5, 1, dHighVel, dLowVel,dAcc))
	{
		return FALSE;
	}
	if(!HomeAxis(K_AXIS_ZC, -500, 5, 1, dHighVel, dLowVel,dAcc))
	{
		return FALSE;
	}
	if(m_bExit)
	{
		return FALSE;
	}
	if (!HomeXY())
	{
		return FALSE;
	}
	if(m_bExit)
	{
		return FALSE;
	}

	// 2016.03.18
	////一定要执行如下4个轴的操作，缓冲区命令运行才能正确运行，否则无法启动缓冲区命令。
	//MoveXY(5,5,dHighVel,dAcc,FALSE);
	//if(m_bExit)
	//{
	//	return FALSE;
	//}
	//Move(K_AXIS_Z,5,dHighVel,dAcc,FALSE);
	//if(m_bExit)
	//{
	//	return FALSE;
	//}
	//WaitStop();
	return TRUE;
}

bool GTS400Ex::CheckBufSpace()
{
	long lBufSpace = 0;
	do	//缓冲区满了，等待
	{
		if(m_bStopEvent) return false;
		gLock();
		GT_CrdSpace(0,1,&lBufSpace);
		gUnLock();
		Delay(0.02);
	}while(1 > lBufSpace);
	return true;
}

void GTS400Ex::ListAddArc(double dEx, double dEy,double r, short iDirection, double dVel, double dAcc)//画圆弧
{
	CString strTemp;
	strTemp.Format("ListAddArcP(x_%5.3f, y_%5.3f, r_%5.3f, dir_%d)\n", dEx, dEy, r, iDirection);
	AddMsg(strTemp);

	if(!m_bInitOK) return;	

	if (ctrlMode!=TYPE_INT_NOLOOK)
	{
		SetCrdPrm(false,1,2);
		ctrlMode=TYPE_INT_NOLOOK;

	}
	Trans2Pulse(&dEx, &dEy, NULL, NULL, &dVel, &dAcc);

	r /= m_pSysParam->tAxis[0].scale;
	//CheckBufSpace();
	gLock();
	GT_ArcXYR(0,1,dEx,dEy,r,iDirection,dVel,dAcc);
	gUnLock();

}

void GTS400Ex::ListAddArc(double dEx,double dEy,double dCx, double dCy, short Dir,double dVel,double dAcc)//画圆
{
	CString strTemp;
	strTemp.Format("ListAddArcP(x_%5.3f, y_%5.3f, dx_%5.3f,dy_%5.3f, dir_%d)\n", dEx, dEy, dCx,dCy, Dir);

	AddMsg(strTemp);
	if(!m_bInitOK) return;

	if (ctrlMode!=TYPE_INT_NOLOOK)
	{
		SetCrdPrm(false,1,2);
		ctrlMode=TYPE_INT_NOLOOK;

	}
	//CheckBufSpace();
	//double prf_pos[4];

	Trans2Pulse(&dEx, &dEy, NULL, NULL, &dVel, &dAcc);

	//prf_pos[K_AXIS_X-1] = dEx;
	//prf_pos[K_AXIS_Y-1] = dEy;
	Trans2Pulse(&dCx, &dCy, NULL, NULL, NULL, NULL);
	CheckBufSpace();
	gLock();
	GT_ArcXYC(0,1,/*prf_pos[0]*/dEx,/*prf_pos[1]*/dEy,dCx,dCy,Dir,dVel,dAcc);
	gUnLock();
}

//void GTS400Ex::ListAddLine(double x, double y, double z,double dVel, double dAcc)
//{
//	CString strTemp;
//	strTemp.Format("ListAddLine(%.3f,%.3f,%.3f)", x,y,z);
//	AddMsg(strTemp);
//
//	if(!m_bInitOK) return;
//	if (ctrlMode!=TYPE_INT_NOLOOK)
//	{
//		SetCrdPrm(false,3);
//		ctrlMode=TYPE_INT_NOLOOK;
//
//	}
//	double prf_pos[4];
//	CheckPos(&x, &y, &z, NULL);
//	Trans2Pulse(&x, &y, &z, NULL, &dVel, &dAcc);
//
//	prf_pos[K_AXIS_X-1] = x;
//	prf_pos[K_AXIS_Y-1] = y;
//	prf_pos[K_AXIS_Z-1] = z;
//
//	short rtn;
//	m_bAddListCmd = TRUE;
//	//CheckBufSpace();
//	gLock();
//	rtn = GT_LnXYZ(0,1,prf_pos[0],prf_pos[1],prf_pos[2],dVel,dAcc,0);
//	gUnLock();
//}

//执行插补运动
void GTS400Ex::ListAddLine(double x, double y, double dVel, double dAcc)
{
	CString strTemp;
	strTemp.Format("ListAddLine(%.3f,%.3f)", x,y);
	AddMsg(strTemp);

	if(!m_bInitOK) return;
	if (ctrlMode!=TYPE_INT_NOLOOK)
	{
		SetCrdPrm(false,1,2);
		ctrlMode=TYPE_INT_NOLOOK;

	}
	double prf_pos[4];
	CheckPos(&x, &y, NULL, NULL);
	Trans2Pulse(&x, &y, NULL, NULL, &dVel, &dAcc);

	prf_pos[K_AXIS_X-1] = x;
	prf_pos[K_AXIS_Y-1] = y;

	short rtn;
	//CheckBufSpace();
	gLock();
	rtn = GT_LnXY(0,1,prf_pos[0],prf_pos[1],dVel,dAcc,0);
	gUnLock();
}

//启用跟随运动：跟随轴不能已经在坐标系中，同时不能运动中
//跟随位置：相对值
//默认是与CARD0 ,CRD1建立跟随关系
void GTS400Ex::AddBufGear(int gearAxisNo,double dPos)
{
	CString strTemp;
	if(!m_bInitOK)
	{
		return;
	}
	Trans2Pulse(gearAxisNo,&dPos,NULL,NULL);
	short rtn;
	gLock();
	rtn = GT_BufGear(0,1,gearAxisNo,dPos,0);
	gUnLock();
}


//void GTS400Ex::ListMoveXYZ(double dX, double dY, double dZ,double dVel, double dAcc, BOOL bCheckPos)//前瞻指令
//{
//	CString strTemp;
//	strTemp.Format("ListMoveXYZ(x_%.3f, y_%.3f,z_%.3f, dVel_%.3f,dAcc_%.3f)\n", dX, dY, dZ,dVel,dAcc);
//	AddMsg(strTemp);
//	if(!m_bInitOK) return;	
//	if (ctrlMode!=TYPE_INT_NOLOOK)
//	{
//		SetCrdPrm(false,3);
//		ctrlMode=TYPE_INT_NOLOOK;
//
//	}
//   //CheckBufSpace();
//	double prf_pos[4];
//
//	if(bCheckPos)CheckPos(&dX, &dY, &dZ, NULL);
//	Trans2Pulse(&dX, &dY, &dZ, NULL, &dVel, &dAcc);
//
//	prf_pos[K_AXIS_X-1] = dX;
//	prf_pos[K_AXIS_Y-1] = dY;
//	prf_pos[K_AXIS_Z-1] = dZ;
//	gLock();
//	GT_LnXYZG0(0,1,prf_pos[0],prf_pos[1],prf_pos[2],dVel,dAcc,0);
//	gUnLock();
//}

void GTS400Ex::ListDelay(double dTime)//S
{
	if(dTime<0.001)return;
	CString strTemp;
	strTemp.Format("ListDelay(%.3f S)", dTime);
	AddMsg(strTemp);
	if(fabs(dTime)<0.001)return;
	if(!m_bInitOK)	return;

	CheckBufSpace();

	if(!m_bStopEvent)
	{
		gLock();
		GT_BufDelay(0,1,dTime*1000);
		gUnLock();
	}
}

//check card info and send message to wnd
void GTS400Ex::CheckGTStatus()
{
	//static unsigned short outStatus= 0xFFFF;
	//static unsigned short inOldStatus = 0xFFFF, outOldStatus=0xFFFF;
	//unsigned short inTemp=0xFFFF, outTemp=0xFFFF;
	//unsigned short iTemp=0x0;
	static unsigned short outStatus[CARD_NUM];
	static unsigned short inOldStatus[CARD_NUM], outOldStatus[CARD_NUM];
	static unsigned short outStatus0 = 0xFFFF,outStatus1 = 0xFFFF;
	static unsigned short inOldStatus0=0xFFFF,inOldStatus1=0xFFFF;
	static unsigned short outOldStatus0=0xFFFF,outOldStatus1=0xFFFF;
	unsigned short inTemp[CARD_NUM], outTemp[CARD_NUM];
	unsigned short iTemp = 0x0;
	for(int i = 0;i < CARD_NUM; i++)
	{
		inTemp[i] = outTemp[i] = 0xFFFF;
		inOldStatus[i] = outOldStatus[i] = 0xFFFF;
	}
	int sts = 0;
	double dAxisPos[16];
	double dEncPos[16];
	long axissts[16];
	long InputSts = 0;
	CString str;
	//
	if(m_bInitOK)
	{
		//for(int i=0;i<CARD_NUM;i++)
		//{
		//	long InputSts = 0;
		//	gLock();
		//	GT_GetDi(i,MC_GPI,&InputSts);
		//	gUnLock();
		//	m_inputSts[i] = (unsigned short)InputSts;
		//}
		//gLock();
		//GT_GetDi(MC_GPI,&InputSts);
		//gUnLock();
		//m_inputSts = (unsigned short)InputSts;
	}

	for(int n=0;n<CARD_NUM;n++)
	{
		gLock();
		GT_GetDi(n,MC_GPI,&InputSts);
		gUnLock();
		m_inputSts[n] = (unsigned short)InputSts;

		if(n==0)
		{
			inTemp[n] = inOldStatus0 ^ m_inputSts[n];
			inOldStatus0 = m_inputSts[n];
		}
		else
		{
			inTemp[n] = inOldStatus1 ^ m_inputSts[n];
			inOldStatus1 = m_inputSts[n];
		}
		for(int i=0;i<16;i++)
		{
			iTemp = 1<<i;
			if(m_hWnd==NULL)
			{
				break;
			}
			if(m_inputSts[n] & iTemp)//检测当前位是否为1？
			{
				sts = 1;
			}
			else
			{
				sts = 0;
			}
			if(inTemp[n] & iTemp)
			{
				if(m_inputSts[n] & iTemp)
				{
					sts = 2;
				}
				else
				{
					sts = -2;
				}
			}
			::SendMessage(m_hWnd,WM_MSG_INPUT_EVENT,n*16+i,sts);
		}
	}

	for(int i=0; i<5; i++)
	{
		if(!m_bInitOK)
			dAxisPos[i] = double(rand())/RAND_MAX*400;
		else
		{
			dAxisPos[i] = GetPos(i+1);
			dEncPos[i] = GetEncPos(i+1);
		}

		if(m_bInitOK)
		{
			gLock();
			GT_GetSts(0,i+1, &axissts[i]);
			gUnLock();
		}
	}
	//str.Format("X:%.3f,Y:%.3f,ZA:%.3f,ZB:%.3f,ZC:%.3f",dEncPos[0],dEncPos[1],dEncPos[2],dEncPos[3],dEncPos[4]);
	//AddMsg(str);
	::SendMessage(m_hWnd, WM_MSG_POS_EVENT, (WPARAM)dAxisPos, (LPARAM)axissts);
}

void GTS400Ex::TransPosToGTEx(double &x, double &y)
{
	if(x < theApp.m_tSysParam.tAxis[0].minpos) x = theApp.m_tSysParam.tAxis[0].minpos;
	if(x > theApp.m_tSysParam.tAxis[0].maxpos) x = theApp.m_tSysParam.tAxis[0].maxpos;
	x /= theApp.m_tSysParam.tAxis[0].scale;

	if(y < theApp.m_tSysParam.tAxis[1].minpos) y = theApp.m_tSysParam.tAxis[1].minpos;
	if(y > theApp.m_tSysParam.tAxis[1].maxpos) y = theApp.m_tSysParam.tAxis[1].maxpos;
	y /= theApp.m_tSysParam.tAxis[1].scale;
}

void GTS400Ex::TransPosToGTEx(double &x, double &y, double &z)
{
	TransPosToGTEx(x, y);	
	if(z < theApp.m_tSysParam.tAxis[2].minpos) z = theApp.m_tSysParam.tAxis[2].minpos;
	if(z > theApp.m_tSysParam.tAxis[2].maxpos) z = theApp.m_tSysParam.tAxis[2].maxpos;
	z /= theApp.m_tSysParam.tAxis[2].scale;
}

void GTS400Ex::TransPosToGTEx(double &x, double &y, double &z, double &u)
{
	TransPosToGTEx(x, y, z);	
	u /= theApp.m_tSysParam.tAxis[3].scale;
}

void GTS400Ex::Delay(double dTime)
{
	DWORD time = (DWORD)(dTime*1000);
	DWORD t=::GetTickCount();
	while(::GetTickCount() -t < time)
	{
		DoEvents();
	}
}
void GTS400Ex::DoEvents()
{
	MSG msg;

	// Process existing messages in the application's message queue.
	// When the queue is empty, do clean up and return.
	while (::PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
	{
		if (!AfxGetThread()->PumpMessage())
			return;
		break;
	}
}
//建立坐标系
short GTS400Ex::SetCrdPrm(BOOL lookAhead,short crd,short dimension,double T,double accMax)
{
			//定义坐标系
	short rtn;
	//检查是否处于插补模式	
	if(ctrlMode < TYPE_INT_NOLOOK)
	{	//printf("dimension:%d\n",dimension);
		memset(&crdPrm,0x00,sizeof(crdPrm));
		crdPrm.dimension = dimension;
		crdPrm.synVelMax = 5000;			//最大合成速度
		//		crdPrm.synVelMax = m_MaxVelR;			//最大合成速度,R轴		
		crdPrm.synAccMax = 1000;			//最大加速度
		crdPrm.evenTime = 0;					//平滑时间
		for(short i=0;i<dimension;i++)
		{
			if(crd == 1)
			{
				crdPrm.profile[i] = i+1; //坐标系映射的轴
				crdPrm.originPos[i] = 0; //坐标系各轴的原点位置
			}
			else if(crd == 2)
			{
				crdPrm.profile[i+2] = i+1;
				crdPrm.originPos[i+2] = 0;
			}
		}
		crdPrm.setOriginFlag = 1;//指定坐标系原点的规划位置
		
		//XYZ插补模式
		rtn = GT_SetCrdPrm(0,crd, &crdPrm); 
		
		if(crd == 1)
		{
			rtn = GT_ClrSts(0,1,2); //清除驱动器相关标志
		}
		else if(crd == 2)
		{
			rtn = GT_ClrSts(0,3,3); 
		}
		rtn = GT_CrdClear(0,crd,0);
	
		rtn = GT_CrdClear(0,1,1);
		

	}

	if(! lookAhead)
	{
		//fifo1
		//rtn = GT_InitLookAhead(1,0,T,accMax,0,NULL);
	
		rtn = GT_CrdClear(0,crd,0);//清除FIFO0中数据：对应轴1
		rtn = GT_CrdClear(0,crd,1);//清除FIFO1中数据：对应轴2
		if(crd == 1)
		{
			rtn = GT_ClrSts(0,1,2); 
		}
		else if(crd == 2)
		{
			rtn = GT_ClrSts(0,3,3); 
		}
		ctrlMode = TYPE_INT_NOLOOK;
	}
	else
	{	
		//修改前瞻拐弯时间参数
		rtn = GT_InitLookAhead(0,1,0,T,accMax,0,NULL);
		rtn = GT_CrdClear(0,1,0);
		rtn = GT_CrdClear(0,1,1);
		rtn = GT_ClrSts(0,1,3);
		ctrlMode = TYPE_INT_LOOKAHEAD;
	}
	return 0;
}
//设置各轴为点位模式 ,并设置启动速度/加速度/急停加速度
short GTS400Ex::SetAllAxisTrap()
{
    TTrapPrm trap;
	short rtn;
	//for(int n=0;n<CARD_NUM;n++)
	//{
		for(int j=0;j<AXIS_NUM;j++)
		{

			trap.acc = 0.5;
			trap.dec = 0.5;	
			trap.velStart = 5;
			rtn = GT_PrfTrap(0,j+1);
			if(rtn)
				return rtn;


			//设置轴的常规加速度和急停加速度

			rtn = GT_SetTrapPrm(0,j+1,&trap);
			if(rtn)
				return rtn;
		}
	//}
	ctrlMode = TYPE_TRAP;
	AddMsg("TYPE_TRAP");
	return 0;
}
void GTS400Ex::CheckListRun(void)
{
   	CString strTemp;

	AddMsg(strTemp);
	if(!m_bInitOK) return;

	gLock();

	if (ctrlMode!=TYPE_INT_NOLOOK)
	{
		SetCrdPrm(false,1,2);
		ctrlMode=TYPE_INT_NOLOOK;
		AddMsg("TYPE_INT_NOLOOK");
	}


  
	 short run0,rtn;
	 long segment,space;

	while(1)
	{	
		//确认运动缓冲区启动
		rtn = GT_CrdStatus(0,1,&run0,&segment,0);
		if(0==run0)
		{
			rtn = GT_CrdSpace(0,1,&space,0);
			if((space+2)<CRD_FIFO_MAX)
			//{	//要求运动缓冲区中,必须有2条指令才能在此处启动
				//printf("space:%ld\n",space);
				rtn = GT_CrdStart(0,1,0);
			//}
		}
		//确认控制卡运动缓冲区数据量
		rtn = GT_CrdSpace(0,1,&space,0);
		//运动缓冲区至少保证5个数据空间
		strTemp.Format("Space:%d",(int)space);
		AddMsg(strTemp);
		if(space>5)
		{
			break;
		}
	}
	gUnLock();
	AddMsg("CheckListRun");
}
BOOL GTS400Ex::WarnCheckPos(double *pX, double *pY, double *pZ, double *pU)
{
	if(pX != NULL)
	{
		if(*pX < m_pSysParam->tAxis[0].minpos) return FALSE;
		if(*pX > m_pSysParam->tAxis[0].maxpos) return FALSE;
	}
	if(pY != NULL)
	{
		if(*pY < m_pSysParam->tAxis[1].minpos) return FALSE;
		if(*pY > m_pSysParam->tAxis[1].maxpos) return FALSE;
	}
	if(pZ != NULL)
	{
		if(*pZ < m_pSysParam->tAxis[2].minpos) return FALSE;
		if(*pZ > m_pSysParam->tAxis[2].maxpos) return FALSE;
	}
	if(pU != NULL)
	{
		if(*pU < m_pSysParam->tAxis[3].minpos) return FALSE;
		if(*pU > m_pSysParam->tAxis[3].maxpos) return FALSE;
	}
	return TRUE;
}

BOOL GTS400Ex::WarnCheckPos(double *pX, double *pY, double *pZa, double *pZb, double *pZc)
{
	if(pX != NULL)
	{
		if(*pX < m_pSysParam->tAxis[0].minpos)
		{
			return FALSE;
		}
		if(*pX > m_pSysParam->tAxis[0].maxpos)
		{
			return FALSE;
		}
	}
	if(pY != NULL)
	{
		if(*pY < m_pSysParam->tAxis[1].minpos)
		{
			return FALSE;
		}
		if(*pY > m_pSysParam->tAxis[1].maxpos)
		{
			return FALSE;
		}
	}
	if(pZa != NULL)
	{
		if(*pZa < m_pSysParam->tAxis[2].minpos)
		{
			return FALSE;
		}
		if(*pZa > m_pSysParam->tAxis[2].maxpos)
		{
			return FALSE;
		}
	}
	if(pZb != NULL)
	{
		if(*pZb < m_pSysParam->tAxis[3].minpos)
		{
			return FALSE;
		}
		if(*pZb > m_pSysParam->tAxis[3].maxpos)
		{
			return FALSE;
		}
	}
	if(pZc != NULL)
	{
		if(*pZc < m_pSysParam->tAxis[4].minpos)
		{
			return FALSE;
		}
		if(*pZc > m_pSysParam->tAxis[4].maxpos)
		{
			return FALSE;
		}
	}
	return TRUE;
}

double GTS400Ex::GetAdVolVal(int chanalID)
{
	double VolVal = 0.0;
	GT_GetExtAdVoltage(0,chanalID-1,&VolVal);
	return VolVal;
}
int GTS400Ex::GetAdValue(int chanalID)
{
	short value = 0;
	//short rtn = GT_GetDac(nCardNum,nChannelID,&value);
	GT_GetExtAdValue(0,chanalID-1,&value);
	//GT_GetExtAdVoltage(0,chanalID,&VolVal);
	return (int)value;
}

void GTS400Ex::AxisClrsts(unsigned short axis)
{
	int nTempCard = (axis-1)/8;
	int nTempAxis = (axis-1)%8;
	gLock();
	GT_ClrSts(nTempCard,nTempAxis+1);
	gUnLock();
	Sleep(30);
	gLock();
	GT_AxisOn(nTempCard,nTempAxis+1);
	gUnLock();
}

MOTION_STATUS  GTS400Ex::GetAxisSts(int iAxisNo,long *pSts)
{
	if(!IsInitOK())
	{
		return MOT_STATUS_COMM_ERROR;
	}
	if(!pSts)
	{
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	CString str;
	int nTempCard = (iAxisNo-1)/8;
	int nTempAxis = (iAxisNo-1)%8;
	short rtn = GT_GetSts(nTempCard,nTempAxis+1,pSts);
	if(0!=rtn)
	{
		str.Format("GetAxisSts()=%d",rtn);
		AddMsg(str);
		m_pLog->log(str);
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	return MOT_STATUS_OK;
}

void GTS400Ex::Stop(int iAxisNo)
{
	if(!m_bInitOK)
	{
		return;
	}
	int nTempCard = (iAxisNo-1)/8;
	int nTempAxis = (iAxisNo-1)%8;
	gLock();
	GT_Stop(nTempCard,1<<nTempAxis,1<<nTempAxis);
	gUnLock();
}
bool GTS400Ex::ListWaitStop()
{
	bool bMoveStop = false;
	LONGLONG llDueTime = CTimeUtil::GetDueTime(30000);
	while(!CTimeUtil::IsTimeout(llDueTime))
	{
		if(!IsMoveList())
		{
			bMoveStop = true;
			break;
		}
		Sleep(5);
	}
	return bMoveStop;
}
void GTS400Ex::Stop()
{
	AddMsg("Stop\n");
	if(!m_bInitOK)
	{
		return;
	}
	gLock();
	for(int i=0;i<CARD_NUM;i++)
	{
		GT_Stop(i,511,0);
	}
	gUnLock();
}
void GTS400Ex::EStop()
{
	AddMsg("EStop\n");
	if(!m_bInitOK)
	{
		return;
	}
	gLock();
	for(int i=0;i<CARD_NUM;i++)
	{
		GT_Stop(i,511,511);
	}
	gUnLock();
}
bool GTS400Ex::ContinuousMove(int iAxisNo,double dVel,double dAcc,bool bPositiveDir)
{
	int nTempCard = (iAxisNo-1)/8;
	int nTempAxis = (iAxisNo-1)%8;
	short ret = GT_PrfJog(nTempCard,nTempAxis+1);
	if(ret)
	{
		return false;
	}
	TJogPrm jog;
	Trans2Pulse(iAxisNo,NULL,&dVel,&dAcc);
	jog.acc = dAcc;
	jog.dec = dAcc;
	jog.smooth = 0.6;
	ret = GT_SetJogPrm(nTempCard,nTempAxis+1,&jog);
	if(ret)
	{
		return false;
	}
	if(bPositiveDir)
	{
		ret = GT_SetVel(nTempCard,nTempAxis+1,dVel);
	}
	else
	{
		ret = GT_SetVel(nTempCard,nTempAxis+1,-dVel);
	}
	if(ret)
	{
		return false;
	}
	ret = GT_Update(nTempCard,1<<nTempAxis);
	if(ret)
	{
		return false;
	}
	return true;
}

void GTS400Ex::Trans2Pulse(int nAxisNo,double *pos,double *vel,double *acc,double *dec,double *jerk)
{
	if(pos != NULL)
	{
		*pos /= m_pSysParam->tAxis[nAxisNo-1].scale;
	}
	if(vel != NULL)
	{
		*vel /= (m_pSysParam->tAxis[nAxisNo-1].scale*1000);
	}
	if(acc != NULL) 
	{
		*acc /= (m_pSysParam->tAxis[nAxisNo-1].scale*1000*1000);//20190920
	}
	if(dec != NULL)
	{
		*dec /= (m_pSysParam->tAxis[nAxisNo-1].scale*1000*1000);//20190920
	}
	if(jerk != NULL)
	{
		*jerk /= (m_pSysParam->tAxis[nAxisNo-1].scale*1000*1000*1000);//20190920
	}
}

MOTION_STATUS GTS400Ex::SetProfile(int nAxisNo,double dVel,double dAcc)
{
	if(!IsInitOK())
	{
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	CString str;
	long lProfileType;
	rtn = 0;
	int nTempCard = (nAxisNo-1)/8;
	int nTempAxis = (nAxisNo-1)%8;
	Trans2Pulse(nAxisNo,NULL,&dVel,&dAcc);
	TTrapPrm tProfile;
	rtn = GT_ClrSts(nTempCard,nTempAxis+1);
	if(rtn!=0)
	{
		str.Format("SetProfile:GT_ClrSts()=%d",rtn);
		AddMsg(str);
		m_pLog->log(str);
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	rtn = GT_GetPrfMode(nTempCard,nTempAxis+1,&lProfileType);
	if(rtn!=0)
	{
		str.Format("SetProfile:GT_GetPrfMode(%d)=%d",nAxisNo,rtn);
		AddMsg(str);
		m_pLog->log(str);
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	if(g_GOOGOL_MOTION_MODE_TCURVE!=lProfileType)
	{
		rtn = GT_PrfTrap(nTempCard,nTempAxis+1);
		if(rtn!=0)
		{
			str.Format("SetProfile:PrfTrap(%d)=%d",nAxisNo,rtn);
			AddMsg(str);
			m_pLog->log(str);
			return MOT_STATUS_UNKNOWN_ERROR;
		}
		ctrlMode = TYPE_TRAP;
	}
	rtn = GT_GetTrapPrm(nTempCard,nTempAxis+1,&tProfile);
	if(rtn!=0)
	{
		str.Format("SetPrifile:GetTrapPrm(startvel_%.1f,acc_%.1f,dec_%.1f,smoothtime_%d)",
			tProfile.velStart,tProfile.acc,tProfile.dec,tProfile.smoothTime);
		AddMsg(str);
		m_pLog->log(str);
		str.Format("SetProfile:GT_SetTrapPrm(%d)=%d",nAxisNo,rtn);
		AddMsg(str);
		m_pLog->log(str);
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	tProfile.acc = dAcc;
	rtn = GT_SetTrapPrm(nTempCard,nTempAxis+1,&tProfile);
	if(rtn!=0)
	{
		str.Format("SetPrifile:SetTrapPrm(startvel_%.1f,acc_%.1f,dec_%.1f,smoothtime_%d)",
			tProfile.velStart,tProfile.acc,tProfile.dec,tProfile.smoothTime);
		AddMsg(str);
		m_pLog->log(str);
		str.Format("SetProfile:GT_SetTrapPrm(%d)=%d",nAxisNo,rtn);
		AddMsg(str);
		m_pLog->log(str);
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	rtn = GT_SetVel(nTempCard,nTempAxis+1,dVel);
	if(rtn!=0)
	{
		str.Format("SetProfile:GT_SetVel(%d)=%d",nAxisNo,rtn);
		AddMsg(str);
		m_pLog->log(str);
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	return MOT_STATUS_OK;
}

MOTION_STATUS GTS400Ex::SetProfile(int nAxisNo,double dVel,double dAcc,double dDec,double dJerk,double dStartVel)
{
	if(!IsInitOK())
	{
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	CString str;
	long lProfileType;
	rtn = 0;
	int nTempCard = (nAxisNo-1)/8;
	int nTempAxis = (nAxisNo-1)%8;
	Trans2Pulse(nAxisNo,NULL,&dVel,&dAcc,&dDec,&dJerk);
	Trans2Pulse(nAxisNo,NULL,&dStartVel,NULL,NULL,NULL);
	TTrapPrm tProfile;
	rtn = GT_ClrSts(nTempCard,nTempAxis+1);
	if(rtn!=0)
	{
		str.Format("SetProfile:GT_ClrSts()=%d",rtn);
		AddMsg(str);
		m_pLog->log(str);
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	rtn = GT_GetPrfMode(nTempCard,nTempAxis+1,&lProfileType);
	if(rtn!=0)
	{
		str.Format("SetProfile:GT_GetPrfMode(%d)=%d",nAxisNo,rtn);
		AddMsg(str);
		m_pLog->log(str);
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	if(g_GOOGOL_MOTION_MODE_TCURVE!=lProfileType)
	{
		rtn = GT_PrfTrap(nTempCard,nTempAxis+1);
		if(rtn!=0)
		{
			str.Format("SetProfile:PrfTrap(%d)=%d",nAxisNo,rtn);
			AddMsg(str);
			m_pLog->log(str);
			return MOT_STATUS_UNKNOWN_ERROR;
		}
		ctrlMode = TYPE_TRAP;
	}
	rtn = GT_GetTrapPrm(nTempCard,nTempAxis+1,&tProfile);
	if(rtn!=0)
	{
		str.Format("SetPrifile:GetTrapPrm(startvel_%.1f,acc_%.1f,dec_%.1f,smoothtime_%d)",
			tProfile.velStart,tProfile.acc,tProfile.dec,tProfile.smoothTime);
		AddMsg(str);
		m_pLog->log(str);
		str.Format("SetProfile:GT_SetTrapPrm(%d)=%d",nAxisNo,rtn);
		AddMsg(str);
		m_pLog->log(str);
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	tProfile.acc = dAcc;
	tProfile.dec = dDec;
	tProfile.velStart = dStartVel;
	if(fabs(dJerk)<0.001)
	{
		tProfile.smoothTime = 0;
	}
	else
	{
		tProfile.smoothTime = short(dAcc/dJerk);
		if(tProfile.smoothTime<0)
		{
			tProfile.smoothTime = 0;
		}
		else if(tProfile.smoothTime>50)
		{
			tProfile.smoothTime = 50;
		}
	}
	rtn = GT_SetTrapPrm(nTempCard,nTempAxis+1,&tProfile);
	if(rtn!=0)
	{
		str.Format("SetPrifile:SetTrapPrm(startvel_%.1f,acc_%.1f,dec_%.1f,smoothtime_%d)",
			tProfile.velStart,tProfile.acc,tProfile.dec,tProfile.smoothTime);
		AddMsg(str);
		m_pLog->log(str);
		str.Format("SetProfile:GT_SetTrapPrm(%d)=%d",nAxisNo,rtn);
		AddMsg(str);
		m_pLog->log(str);
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	rtn = GT_SetVel(nTempCard,nTempAxis+1,dVel); //pulse/ms;
	if(rtn!=0)
	{
		str.Format("SetProfile:GT_SetVel(%d)=%d",nAxisNo,rtn);
		AddMsg(str);
		m_pLog->log(str);
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	return MOT_STATUS_OK;
}

MOTION_STATUS GTS400Ex::GetProfile(int nAxisNo,double *pVel,double *pAcc,double *pDec,double *pStartVel,int *pSmoothTime)
{
	if(!IsInitOK())
	{
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	CString str;
	long lProfileType;
	rtn = 0;
	int nTempCard = (nAxisNo-1)/8;
	int nTempAxis = (nAxisNo-1)%8;
	TTrapPrm tProfile;
	rtn = GT_ClrSts(nTempCard,nTempAxis+1);
	if(rtn!=0)
	{
		str.Format("SetProfile:GT_ClrSts()=%d",rtn);
		AddMsg(str);
		m_pLog->log(str);
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	rtn = GT_GetPrfMode(nTempCard,nTempAxis+1,&lProfileType);
	if(rtn!=0)
	{
		str.Format("SetProfile:GT_GetPrfMode(%d)=%d",nAxisNo,rtn);
		AddMsg(str);
		m_pLog->log(str);
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	if(g_GOOGOL_MOTION_MODE_TCURVE!=lProfileType)
	{
		rtn = GT_PrfTrap(nTempCard,nTempAxis+1);
		if(rtn!=0)
		{
			str.Format("SetProfile:PrfTrap(%d)=%d",nAxisNo,rtn);
			AddMsg(str);
			m_pLog->log(str);
			return MOT_STATUS_UNKNOWN_ERROR;
		}
		ctrlMode = TYPE_TRAP;
	}
	rtn = GT_GetVel(nTempCard,nTempAxis+1,pVel);
	if(rtn!=0)
	{
		str.Format("SetPrifile:GetVel(axis_%d,vel_%.1f)",nAxisNo,*pVel);
		AddMsg(str);
		m_pLog->log(str);
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	rtn = GT_GetTrapPrm(nTempCard,nTempAxis+1,&tProfile);
	if(rtn!=0)
	{
		str.Format("SetPrifile:GetTrapPrm(startvel_%.1f,acc_%.1f,dec_%.1f,smoothtime_%d)",
			tProfile.velStart,tProfile.acc,tProfile.dec,tProfile.smoothTime);
		AddMsg(str);
		m_pLog->log(str);
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	*pAcc = tProfile.acc;
	*pDec = tProfile.dec;
	*pStartVel = tProfile.velStart;
	*pSmoothTime = tProfile.smoothTime;
	Trans2MM(nAxisNo,NULL,pVel,pAcc);
	Trans2MM(nAxisNo,NULL,pStartVel,pDec);
	return MOT_STATUS_OK;
}

MOTION_STATUS GTS400Ex::Move(int nAxisNo,double dPos,bool bWaitStop,bool bCheckPos)
{
	if(m_bExit)
	{
		return MOT_STATUS_MOTION_FAILURE;
	}
	if(!IsInitOK())
	{
		return MOT_STATUS_COMM_ERROR;
	}
	if(bCheckPos)
	{
		CheckPos(nAxisNo,&dPos);
	}
	Trans2Pulse(nAxisNo,&dPos,NULL,NULL);

	long lPrfileType;
	CString strTemp;
	MOTION_STATUS status = MOT_STATUS_OK;
	int nTempCard = (nAxisNo-1)/8;
	int nTempAxis = (nAxisNo-1)%8;
	rtn = GT_GetPrfMode(nTempCard,(short)(nTempAxis+1),&lPrfileType);
	if(0 != rtn)
	{
		strTemp.Format("Move:GT_GetPrfMode(%d)=%d",nAxisNo,rtn);
		AddMsg(strTemp);
		m_pLog->log(strTemp);
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	if(g_GOOGOL_MOTION_MODE_TCURVE != lPrfileType)
	{
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	rtn = GT_SetPos(nTempCard,nTempAxis+1,long(dPos));
	if(0 != rtn)
	{
		strTemp.Format("Move:GT_SetPos(%d)=%d",nAxisNo,rtn);
		AddMsg(strTemp);
		m_pLog->log(strTemp);
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	rtn = GT_Update(nTempCard,1<<nTempAxis);
	if(0 != rtn)
	{
		strTemp.Format("Move:GT_Update(%d)=%d",nAxisNo,rtn);
		AddMsg(strTemp);
		m_pLog->log(strTemp);
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	if(bWaitStop)
	{
		status = WaitStop(nAxisNo,60*1000);
		if(MOT_STATUS_OK != status)
		{
			return status;
		}
	}
	return MOT_STATUS_OK;
}

MOTION_STATUS GTS400Ex::WaitStop(int nAxisNo,unsigned long lTimeout)
{
	bool bIsCompleted = false;
	MOTION_STATUS status = WaitMotionComplete(nAxisNo,&bIsCompleted,lTimeout);
	if(MOT_STATUS_OK!=status)
	{
		return status;
	}
	if(!bIsCompleted)
	{
		return MOT_STATUS_TIMEOUT;
	}
	return MOT_STATUS_OK;
}

MOTION_STATUS GTS400Ex::WaitMotionComplete(int nAxisNo,bool *pCompleted,unsigned long lTimeout)
{
	if(!IsInitOK())
	{
		return MOT_STATUS_COMM_ERROR;
	}
	MOTION_STATUS status;
	LONGLONG llDueTime = CTimeUtil::GetDueTime(lTimeout);
	status = IsMoveCompleted(nAxisNo,pCompleted);
	while(MOT_STATUS_OK == status && !(*pCompleted) && !CTimeUtil::IsTimeout(llDueTime))
	{
		//Sleep(0);
		status = IsMoveCompleted(nAxisNo,pCompleted);
	}
	if(!(*pCompleted) && MOT_STATUS_OK == status)
	{
		return MOT_STATUS_TIMEOUT;
	}
	return status;
}

//return two values :func and parameters
MOTION_STATUS GTS400Ex::IsMoveCompleted(int nAxisNo,bool *pCompleted)
{
	if(!IsInitOK())
	{
		return MOT_STATUS_COMM_ERROR;
	}
	if(!pCompleted)
	{
		return MOT_STATUS_UNKNOWN_ERROR;
	}
	MOTION_STATUS status;
	long lStatus;
	*pCompleted = false;
	status = GetAxisSts(nAxisNo,&lStatus);
	if(MOT_STATUS_OK != status)
	{
		return status;
	}
	if(!(lStatus & g_GOOGOL_STATUS_PROFILE_RUNNING))
	{
		*pCompleted = true;
	}
	if((lStatus & g_GOOGOL_STATUS_DRIVE_ALARM)|| IsDriverAlarm(nAxisNo))
	{
		return MOT_STATUS_MOTION_FAILURE;
	}
	return MOT_STATUS_OK;
}

//all alarm read once
bool GTS400Ex::IsDriverAlarm(int nAxisNo)
{
	if(!IsInitOK())
	{
		return false;
	}

	CString str;
	long lPortValue;
	int nTempCard = (nAxisNo-1)/8;
	int nTempAxis = (nAxisNo-1)%8;
	short rtn = GT_GetDi(nTempCard,MC_ALARM,&lPortValue);
	if(0 != rtn)
	{
		str.Format("IsDriverAlarm:GT_GetDi(%d)=%d",nAxisNo,rtn);
		AddMsg(str);
		m_pLog->log(str);
		return false;
	}

	lPortValue = lPortValue & (1 << nTempAxis);
	return (0 != lPortValue);
}

//bit6=1  limit-  trig
bool GTS400Ex::IsNegativeLimitOn(int nAxisNo)
{
	if(!IsInitOK())
	{
		return false;
	}

	CString str;
	MOTION_STATUS status;
	long lStatus;
	status = GetAxisSts(nAxisNo,&lStatus);
	if(MOT_STATUS_OK != status)
	{
		str.Format("IsNegativeLimitOn:GetAxisSts(%d)=%d",nAxisNo,status);
		AddMsg(str);
		m_pLog->log(str);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		return false;
	}
	return (0 != (lStatus & g_GOOGOL_STATUS_NEGATIVE_LIMIT));
}

//bit5=1  limit+  trig
bool GTS400Ex::IsPositiveLimitOn(int nAxisNo)
{
	if(!IsInitOK())
	{
		return false;
	}

	CString str;
	MOTION_STATUS status;
	long lStatus;
	status = GetAxisSts(nAxisNo,&lStatus);
	if(MOT_STATUS_OK != status)
	{
		str.Format("IsPositiveLimitOn:GetAxisSts(%d)=%d",nAxisNo,status);
		AddMsg(str);
		return false;
	}         //when the status trigger;the bit value is 1;
	return (0 != (lStatus & g_GOOGOL_STATUS_POSITIVE_LIMIT));
}

bool GTS400Ex::SetStopDec(int nAxisNo,double decSmoothStop,double decAbrupStop)
{
	double decSmooth = decSmoothStop;
	double decAbrup = decAbrupStop;
	Trans2Pulse(nAxisNo,NULL,NULL,&decSmooth,&decAbrup,NULL);
	int nTempCard = (nAxisNo-1)/8;
	int nTempAxis = (nAxisNo-1)%8;
	GT_SetStopDec((short)nTempCard,(short)(nTempAxis+1),decSmooth,decAbrup);
	return true;
}