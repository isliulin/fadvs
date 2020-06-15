#include "StdAfx.h"
#include "CmdRun.h"
#include "MainFrm.h"
#include "TSCtrlSys.h"
#include "DlgManualAlign.h"
#include <math.h>
#include <Windows.h>
#include <MMSystem.h>

#include <afxinet.h>
#include <wininet.h>

#define KGB_PI_HALF	1.5707963
#define KGB_PI		3.1415926
#define KGB_2PI		6.2831852
#define EPS	        0.000001
#pragma   warning(disable:4244)
#pragma comment(lib, "winmm.lib")

CWinThread *g_pRunThread = NULL;
CWinThread *g_pHomeThread = NULL;
CWinThread *g_pTestRunThread = NULL;
CWinThread *g_pImageRunThread = NULL;
CWinThread *g_pValveResetThread = NULL;
CWinThread *g_pPadDetectThread = NULL;
CWinThread *g_pCleanValveThread=NULL; 
CWinThread *g_pLogRunData = NULL;


CCriticalSection g_csPadDetect; 
CCriticalSection g_csLog;
//Line 14550        ��ֹй©//2017-11-18

UINT RunThread(LPVOID param)
{
	CCmdRun *p = (CCmdRun *)param;
	p->AddMsg("��ʼ����");
	timeBeginPeriod(1);
	g_pFrm->m_CmdRun.bIsThreadRunning=true;//2016-03-11
	try
	{
		p->Run();
	}
	catch(...)
	{
		CString str;
		str.Format("�㽺ģ���׳��쳣...");
		if (NULL!=g_pFrm)
		{
			g_pFrm->m_CmdRun.AddMsg(str);
			g_pFrm->m_CmdRun.PutLogIntoList(str);
		}

	}	
	g_pFrm->m_CmdRun.bIsThreadRunning=false;
	timeEndPeriod(1);
	p->AddMsg("��������");
	return 0;
}

UINT TestRunThread(LPVOID param)
{
	CCmdRun *p = (CCmdRun *)param;
	p->m_bTestRunStop = false;
	while(!p->m_bTestRunStop)
	{
		p->TestRun();
	}
	return 0;
}

UINT CCmdRun::ThreadHome(LPVOID lparam)
{
	CCmdRun *p = (CCmdRun *)lparam;
	p->Home();
	return 0;
}

UINT CCmdRun::ThreadTestImage(LPVOID lparam)
{
	CCmdRun *p = (CCmdRun *)lparam;
	p->TestImage();
	return 0;
}

UINT CCmdRun::ThreadValveReset(LPVOID lparam)
{
	CCmdRun *p = (CCmdRun *)lparam;
	p->ValveReset();
	return 0;
}

UINT CCmdRun::ThreadDetect(LPVOID lparam)
{
	CCmdRun *p = (CCmdRun *)lparam;
	p->PadDetect();
	return 0;
}


UINT CCmdRun::ThreadClean(LPVOID lparam)
{
	CCmdRun *p=(CCmdRun *)lparam;
	g_pFrm->m_CmdRun.m_bIsCleanValve=true;
	p->DischargePositionEX();
	p->CleanValves();
	g_pFrm->m_CmdRun.m_bIsCleanValve=false;
	return 0;

}
UINT CCmdRun::ThreadLog(LPVOID lparam)
{
	CString str;
	POSITION pos;
	CCmdRun*p =(CCmdRun*)lparam;
	bool bFlag;
	while(true)
	{
		Sleep(5);
		bFlag=false;
		g_csLog.Lock();
		if(NULL!=p->m_LogInfo)
		{
			if((p->m_LogInfo->GetCount()>=1)&&(!p->m_LogInfo->IsEmpty()))
			{
				str=p->m_LogInfo->GetHead();
				p->m_LogInfo->RemoveHead();
				bFlag=true;
			}
		}
		g_csLog.Unlock();
		if(bFlag)
		{
			p->m_pLog->logAsync(str);
		}
		CFunction::DoEvents();
		if(gt_bClose)
			break;
	}
	return 0;
}
void CCmdRun::PutLogIntoList(CString& str)
{
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	CString logMessage;
	logMessage.Format(_T("%04d/%02d/%02d %02d:%02d:%02d:%03d\t"),SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond,SystemTime.wMilliseconds);
    logMessage.Append(str);
	g_csLog.Lock();
	m_LogInfo->AddTail(logMessage);
	g_csLog.Unlock();
}
CCmdRun::CCmdRun(void)
{
	m_pMv = NULL;
	m_pListBox = NULL;
	m_tStatus = K_RUN_STS_NONE;
	m_LastStatus = K_RUN_STS_NONE;//20180713
	m_dRotateAngle = 0.0;
	bIsThreadRunning=False;
	m_bIsCleanValve=false;//2016-03-14
	m_nCleanValveNum=-3;
	bIsExitClean=true; //2016-03-14

	PDposting=NULL;
	PDpostingBug=NULL;
	m_pPadDetect = NULL;
	m_pTransitionDetect = NULL;
	m_nPadNumber = 0;
	nTempCount=0;
	RunErrorID=-1;
	//StautDisp.W_Count=0;
	StautDisp.W_UPH=0.0;
	StautDisp.W_SingeTimer=0.0;
	m_StarT = m_EndT = CTime::GetCurrentTime();

	DisCompCount=0;
	m_nDetectImageIndex = 0;
	nTempLackStatic=0;
    nFind=0;

	ManalRun=true;
	V9RestOK=false;
	AutoRunCp=false;
	V9DotRest=false;
	m_bV9FirstDisp = false;
	m_bFirstDisp = false;
	m_bIsPause = false;
	m_bTestRunStop = true;
	m_bRepair = false;
	m_bIsSupend = false;
	m_bRefresh = false;
	m_bValveReset = false;
	m_bStartRun = true;
	m_bDispenseSuccess = false;

	m_bCleanNeedle = false;
	m_bTestDispense = false;
	m_bDischarge = false;
	m_bSearchMark = false;
	m_bSlotDetect = false;
	m_bHoming = false;
	m_bMeasureHeight = false;
	m_bPadDetecting=false;
	m_bImageUse = false;
	m_bDoorOpen = false;
    m_bStepOne=true;
	m_bNeedleCalibrate=false;
    m_bNeedleAutoAdjust=false;
    m_bAutoNeedleErr=false;
    bMesDataUpdate =true;   //��ȡMES���ݸ��±����������
    bMaterialCodeNew=false; //�Ƿ�ɨ������̵���

	dTempValue1 = dTempValue2 = 0.0;
	dTempDelay = 0.0;
    padLackTotal = 0;
	pcsDoneFromStart=0;
	m_bRunLock = false;
	for(int i=0;i<3;i++)
	{
		m_bValveSelect[i] = true;
	}
	m_bPlcRunError = false;
	////
	int nYear, nMonth, nDate, nHour, nMin, nSec; 
	CString s2(theApp.m_tSysParam.szLastCleanTime); 
	sscanf(s2, "%d-%d-%d %d:%d:%d", &nYear, &nMonth, &nDate, &nHour, &nMin, &nSec); 
	CTime t2(nYear, nMonth, nDate, nHour, nMin, nSec);
	m_TimeCleanRemindSt = t2;
	m_LogInfo = new CStringList;
	
}

CCmdRun::~CCmdRun(void)
{
	if (PDposting!=NULL)
	{
		delete[] PDposting;
		PDposting = NULL;
	}
	if (PDpostingBug!=NULL)
	{
		delete[] PDpostingBug;
		PDpostingBug = NULL;
	}

	if(m_pTransitionDetect!=NULL)
	{
		delete[] m_pTransitionDetect;
		m_pTransitionDetect = NULL;
	}

	if(m_pPadDetect!=NULL)
	{
		delete[] m_pPadDetect;
		m_pPadDetect = NULL;
	}

	//2016-03-14
	TRACE(_T("g_pCleanValveThread  Exit begin\n"));
	if(g_pCleanValveThread != NULL)
	{
		switch( WaitForSingleObject(g_pCleanValveThread->m_hThread, 500) )
		{
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			TerminateThread(g_pCleanValveThread->m_hThread, 0);
			break;
		}

		g_pRunThread = NULL;
	}
	TRACE(_T("g_pCleanValveThread Exit Ok\n"));//2016-03-14

	TRACE(_T("g_pHomeThread Exit begin\n"));
	if(g_pHomeThread != NULL)
	{
		switch( WaitForSingleObject(g_pHomeThread->m_hThread, 500) )
		{
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			TerminateThread(g_pHomeThread->m_hThread, 0);
			break;
		}

		g_pHomeThread = NULL;
	}
	TRACE(_T("g_pHomeThread Exit Ok\n"));

	TRACE(_T("g_pRunThread Exit begin\n"));
	if(g_pRunThread != NULL)
	{
		switch( WaitForSingleObject(g_pRunThread->m_hThread, 500) )
		{
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			TerminateThread(g_pRunThread->m_hThread, 0);
			break;
		}

		g_pRunThread = NULL;
	}
	TRACE(_T("g_pRunThread Exit Ok\n"));

	TRACE(_T("g_pTestRunThread Exit begin\n"));
	if(g_pTestRunThread != NULL)
	{
		switch( WaitForSingleObject(g_pTestRunThread->m_hThread, 500) )
		{
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			TerminateThread(g_pTestRunThread->m_hThread, 0);
			break;
		}

		g_pTestRunThread = NULL;
	}
	TRACE(_T("g_pTestRunThread Exit Ok\n"));

	TRACE(_T("g_pImageRunThread Exit begin\n"));
	if(g_pImageRunThread != NULL)
	{
		switch( WaitForSingleObject(g_pImageRunThread->m_hThread, 500) )
		{
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			TerminateThread(g_pImageRunThread->m_hThread, 0);
			break;
		}
		g_pImageRunThread = NULL;
	}
	TRACE(_T("g_pImageRunThread Exit Ok\n"));

	TRACE(_T("g_pValveResetThread Exit begin\n"));
	if(g_pValveResetThread != NULL)
	{
		switch( WaitForSingleObject(g_pValveResetThread->m_hThread, 500) )
		{
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			TerminateThread(g_pValveResetThread->m_hThread, 0);
			break;
		}
		g_pValveResetThread = NULL;
	}
	TRACE(_T("g_pValveResetThread Exit Ok\n"));

	TRACE(_T("g_pPadDetectThread Exit begin\n"));
	if(g_pPadDetectThread != NULL)
	{
		switch( WaitForSingleObject(g_pPadDetectThread->m_hThread, 500) )
		{
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			TerminateThread(g_pPadDetectThread->m_hThread, 0);
			break;
		}
		g_pPadDetectThread = NULL;
	}
	TRACE(_T("g_pPadDetectThread Exit Ok\n"));
	TRACE(_T("g_pLogRunData Exit begin\n"));
	if(NULL!=g_pLogRunData)
	{
		switch(WaitForSingleObject(g_pLogRunData->m_hThread,500))
		{
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			TerminateThread(g_pLogRunData->m_hThread,0);
			break;
		}
		g_pLogRunData=NULL;
	}
	TRACE(_T("g_pLogRunData Exit Ok\n"));
	if(m_LogInfo)
	{
		delete m_LogInfo;
		m_LogInfo=NULL;
	}
}

void CCmdRun::Create(GTS400Ex *pMv, CListBox *pListBox, CLogFile *pLog)
{
	m_pMv = pMv;
	m_pListBox = pListBox;
	m_pLog = pLog;
}
// �����Ϣ
void CCmdRun::AddMsg(CString strText)
{
	if(m_pListBox != NULL)
	{
		if(m_pListBox->GetCount()>10000)
		{
			m_pListBox->ResetContent();
		}
		CString strTemp;
		strTemp.Format("%04d: ", m_pListBox->GetCount());
		m_pListBox->AddString(strTemp + strText);
		m_pListBox->SetCurSel(m_pListBox->GetCount()-1);
	}
}
void CCmdRun::RunThreadLog()
{
	g_pLogRunData=AfxBeginThread(ThreadLog, this);
	if(g_pLogRunData==NULL)
	{
		AfxMessageBox("��־�̴߳���ʧ��.");
		return;
	}
	return;
}
// �����߳�
void CCmdRun::RunThreadPlay()
{
	CString strError;
	//��ˢ���̵���
	//�ȴ����ݸ������...
	if((0 == ProductParam.bMesOffline) && bMaterialCodeNew)
	{
		DWORD StartTime,EndTime;
		StartTime = EndTime = GetTickCount();

		CString str;
		while(!bMesDataUpdate)
		{
			str.Format("�ȴ�MESͨѶ���...");
			AddMsg(str);
			PutLogIntoList(str);
			CFunction::DoEvents();
			EndTime= GetTickCount();

			if((EndTime-StartTime)/1000>5)  //5S
			{
				AfxMessageBox("�ȴ�MES���ݸ��³�ʱ");
				bMaterialCodeNew=false;       //��λ����ɨ���¼
				return;
			}
			CTimeUtil::Delay(10);
			CFunction::DoEvents();
		}

		bMaterialCodeNew=false;              //��λ����ɨ���¼
		bMesDataUpdate=false;                //��λ���ݸ���״̬

		str.Format("MESͨѶ��ɣ����ݸ��³ɹ�!");
		AddMsg(str);
		PutLogIntoList(str);

		if(m_Glue_msg.rev_bIsDischarge)
		{
			m_Glue_msg.rev_bIsDischarge = false;
			strError.Format("�Ž����ѱ���");
			PutLogIntoList(strError);
			//Alarm(49);
			AddMsg(strError);
			//AfxMessageBox("�Ž����ѱ��������Ž�������������");

			str.Format("�����Ѹ���, ��ȷ���Ƿ����Ž���");
			if(AfxMessageBox(str,MB_YESNO) != IDYES)
			{
				m_tStatus = K_RUN_STS_STOP;
				return;
			}
		}

		str.Format("�������¡�GlueA:%0.6f,GlueB:%0.6f,GlueC:%0.6f��,ȷ�������㽺��",m_Glue_msg.rev_MESGlue[0],m_Glue_msg.rev_MESGlue[1],m_Glue_msg.rev_MESGlue[2]);
		if(AfxMessageBox(str,MB_YESNO)!=IDYES)
		{
			m_tStatus = K_RUN_STS_STOP;
			return;
		}
	}
	if(!GlueCheck())
	{
       AfxMessageBox("MES���ݳ��ޣ�");
	   return;
	}

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
	if(m_tStatus == K_RUN_STS_RUN||m_bHoming)
	{
		return;
	}
	if(m_tStatus == K_RUN_STS_PAUSE)
	{
		m_tStatus = K_RUN_STS_RUN;
		return;
	}
	if(m_bCleanNeedle||m_bDischarge||m_bTestDispense||m_bSearchMark||m_bSlotDetect||m_bHoming||m_bMeasureHeight||m_bNeedleCalibrate||m_bNeedleAutoAdjust)
	{
		strError.Format("δ֪����");
		if(m_bCleanNeedle)
		{
			strError.Format("��ϴ��ͷʱ���������У�");
		}
		else if(m_bDischarge)
		{
			strError.Format("�Ž�ʱ���������У�");
		}
		else if(m_bTestDispense)
		{
			strError.Format("�Խ�ʱ���������У�");
		}
		else if(m_bSearchMark)
		{
			strError.Format("����Mark��ʱ���������У�");
		}
		else if(m_bSlotDetect)
		{
			strError.Format("�������ʱ���������У�");
		}
		else if(m_bHoming)
		{
			strError.Format("��λʱ���������У�");
		}
		else if(m_bMeasureHeight)
		{
			strError.Format("���ʱ���������У�");
		}
		else if (m_bNeedleCalibrate)
		{
			strError.Format("��ͷ�궨ʱ����������");
		}
		else if (m_bNeedleAutoAdjust)
		{
			strError.Format("�Զ�����ʱ����������");
		}
		PutLogIntoList(strError);
		Alarm(33);
		AddMsg(strError);
		AfxMessageBox("�豸�����������������븴λ������������");
		return;
	}
	//g_pView->m_ImgStatic.Stop();
	g_pFrm->m_wndPosInfo.ReadTemperature();
	theApp.ProductParam(FALSE);
	g_pRunThread=AfxBeginThread(RunThread, this);
	if(g_pRunThread==NULL)
	{
		m_tStatus = K_RUN_STS_STOP;
		AfxMessageBox("�̴߳���ʧ��.");
		return;
	}
	return;
}
// �����߳�
void CCmdRun::RunThreadTest()
{
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
	if(m_tStatus == K_RUN_STS_RUN||m_bHoming)
	{
		return;
	}
	if(m_tStatus == K_RUN_STS_PAUSE)
	{
		m_tStatus = K_RUN_STS_RUN;
		return;
	}
	g_pTestRunThread=AfxBeginThread(TestRunThread,this);
	if(g_pTestRunThread==NULL)
	{
		m_tStatus = K_RUN_STS_STOP;
		AfxMessageBox("�����̴߳���ʧ��.");
		return;
	}
	return;
}
// ͼ�����
void CCmdRun::RunThreadTestImage()
{
	g_pImageRunThread = AfxBeginThread(CCmdRun::ThreadTestImage,this);
	if(g_pImageRunThread==NULL)
	{
		m_tStatus = K_RUN_STS_STOP;
		AfxMessageBox("ͼ���̴߳���ʧ��.");
		return;
	}
}

void CCmdRun::RunThreadValveReset()
{
	g_pValveResetThread = AfxBeginThread(CCmdRun::ThreadValveReset,this);
	if(g_pValveResetThread==NULL)
	{
		m_tStatus = K_RUN_STS_STOP;
		AfxMessageBox("���帴λ�̴߳���ʧ��.");
		return;
	}
}

void CCmdRun::RunThreadDetect()
{
	g_pPadDetectThread = AfxBeginThread(CCmdRun::ThreadDetect,this);
	if(g_pPadDetectThread==NULL)
	{
		m_tStatus = K_RUN_STS_STOP;
		AfxMessageBox("��������̴߳���ʧ�ܣ�");
		return;
	}
}
// ��ͣ
void CCmdRun::Pause()
{
	m_tStatus = K_RUN_STS_PAUSE;
}
// ֹͣ
void CCmdRun::Stop()
{
	if (m_tStatus == K_RUN_STS_NONE)
	{
		return;
	}

	//m_pMv->SetOutput(theApp.m_tSysParam.outDispenseFinish,TRUE);
	m_pMv->Stop();
	if(!m_pMv->ListWaitStop())
	{
		Alarm(31);
		m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
		AfxMessageBox("�˶�ֹͣʧ�ܣ�");
	}

	if(m_tStatus!=K_RUN_STS_EMERGENCY)
	{
		m_tStatus = K_RUN_STS_STOP;//����ֹͣ�źź�һ��Ҫ�˳�����ָ��
	}
	Sleep(100);
	if(!SetAxisProfile(MEDIUM_VEL))
	{
		Alarm(30);
		AfxMessageBox("�����������ٶ�ʧ�ܣ�");
	}
	//m_pMv->SetOutput(theApp.m_tSysParam.outDispenseFinish,FALSE);	
}
// ����ֹͣ
void CCmdRun::RunStop()
{
	m_bFirstDisp = false;
	m_bTestRunStop = true;
	g_pFrm->m_CmdRun.m_tStatus = K_RUN_STS_STOP;
}
// ��ͣ
void CCmdRun::Emergency()
{
	m_tStatus = K_RUN_STS_EMERGENCY;//�㽺��
	m_bFirstDisp = false;
	m_bTestRunStop = true;
	m_pMv->EStop();
	m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
	AddMsg("--->����ֹͣ----");
}
// ָʾ������
void CCmdRun::SetLamp()
{
	if(m_pMv == NULL)
	{
		return;
	}
	m_pMv->SetOutput(theApp.m_tSysParam.outGreen, m_tStatus ==K_RUN_STS_RUN &&!g_pFrm->m_wndPosInfo.PlcEorrTag&&RunErrorID ==-1);
	m_pMv->SetOutput(theApp.m_tSysParam.outYellow, m_tStatus ==K_RUN_STS_STOP&&!g_pFrm->m_wndPosInfo.PlcEorrTag&&RunErrorID ==-1 );
	if(m_pMv->GetOutput(theApp.m_tSysParam.outRed))   //0708 temply block;
	{
		m_pMv->SetOutput(theApp.m_tSysParam.outRed,FALSE);
	}
	else
	{
		//m_pMv->SetOutput(theApp.m_tSysParam.outRed, m_tStatus ==K_RUN_STS_EMERGENCY ||m_bIsSupend||m_bAutoNeedleErr||g_pFrm->m_wndPosInfo.PlcEorrTag||RunErrorID >=0);
		if(m_tStatus==K_RUN_STS_STOP)
		{
			m_pMv->SetOutput(theApp.m_tSysParam.outRed, m_tStatus ==K_RUN_STS_EMERGENCY ||m_bIsSupend||m_bAutoNeedleErr||(g_pFrm->m_wndPosInfo.ErrorPlcData>0&&g_pFrm->m_wndPosInfo.ErrorPlcData<27)||RunErrorID >=0);

		}
		else
		{
			m_pMv->SetOutput(theApp.m_tSysParam.outRed, m_tStatus ==K_RUN_STS_EMERGENCY ||m_bIsSupend||m_bAutoNeedleErr||g_pFrm->m_wndPosInfo.PlcEorrTag||RunErrorID >=0);
		}
	}
}
// �Ƿ��˳�����
BOOL CCmdRun::IsExitRun()
{
	if(m_tStatus == K_RUN_STS_STOP||m_tStatus == K_RUN_STS_EMERGENCY||m_tStatus == K_RUN_STS_ALARM)
	{
		m_bFirstDisp = false;
		m_bTestRunStop = true;
		//Stop();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


//Dot Mode 
bool CCmdRun::RunSingleDot(double x,double y,double za,double zb,double zc,bool RunDot_RestV9,bool bMultiMode)
{
	CString str;
	double dz1,dz2,dz3;
	dz1=dz2=dz3=0;
	m_bDispenseSuccess = false;
	str.Format("[���ģʽ]ָ��λ��:x_%0.3f;y_%0.3f;za_%0.3f;zb_%0.3f;zc_%0.3f...",x,y,za,zb,zc);
	AddMsg(str);
	PutLogIntoList(str);
	if(m_tStatus == K_RUN_STS_PAUSE)
	{
		if(!WaitPasueEnd())
		{
			return false;
		}
	}
	if(IsRunStop())
	{
		return false;
	}
	if(bMultiMode)
	{
		bool m_bStartSubRun;
		m_bStartSubRun=true;  //��ѭ���¿�ʼ
		double dOffsetX = 0.0;
		double dOffsetY = 0.0;
		for(int i=ProductParam.tgTrackInfoMulti.m_nMultiRow;i<ProductParam.MultiDispRow;i++)
		{
			if(!(i%2))
			{
				if(!m_bStartSubRun)
				{
					ProductParam.tgTrackInfoMulti.m_nMultiColumn=0;
				}
				for(int j=ProductParam.tgTrackInfoMulti.m_nMultiColumn;j<ProductParam.MultiDispColumn;j++)
				{
					if(!m_bStartSubRun)
					{
						m_bStartSubRun=false;
					 }
					dOffsetX = j*ProductParam.MultiDispColumnD;
					dOffsetY = i*ProductParam.MultiDispRowD;
					if(m_bRepair||m_bFirstDisp)
					{
						if(!SynchronizeMoveXY(x+dOffsetX,y+dOffsetY,true))
						{
							ProductParam.tgTrackInfoMulti.m_nMultiRow=i;  //���䵱ǰ�������
							ProductParam.tgTrackInfoMulti.m_nMultiColumn=j; //���䵱ǰ�������
							return false;
						}
						if(IsExitRun())
						{
							ProductParam.tgTrackInfoMulti.m_nMultiRow=i;  //���䵱ǰ�������
							ProductParam.tgTrackInfoMulti.m_nMultiColumn=j; //���䵱ǰ�������
							MoveToZSafety();
							return false;
						}
						if(!SynchronizeMoveZ(za-ProductParam.dZupHigh*0.5,zb-ProductParam.dZupHigh*0.5,zc-ProductParam.dZupHigh*0.5,true,true))
						{
							ProductParam.tgTrackInfoMulti.m_nMultiRow=i;  //���䵱ǰ�������
							ProductParam.tgTrackInfoMulti.m_nMultiColumn=j; //���䵱ǰ�������
							return false;
						}
					}
					else
					{
						if(!SynchronizeMoveXY(x+dOffsetX,y+dOffsetY,false))
						{
							ProductParam.tgTrackInfoMulti.m_nMultiRow=i;  //���䵱ǰ�������
							ProductParam.tgTrackInfoMulti.m_nMultiColumn=j; //���䵱ǰ�������
							return false;
						}
						if(IsExitRun())
						{
							ProductParam.tgTrackInfoMulti.m_nMultiRow=i;  //���䵱ǰ�������
							ProductParam.tgTrackInfoMulti.m_nMultiColumn=j; //���䵱ǰ�������
							MoveToZSafety();
							return false;
						}
						if(!SynchronizeMoveZ(za-ProductParam.dZupHigh*0.5,zb-ProductParam.dZupHigh*0.5,zc-ProductParam.dZupHigh*0.5,true,true))
						{
							ProductParam.tgTrackInfoMulti.m_nMultiRow=i;  //���䵱ǰ�������
							ProductParam.tgTrackInfoMulti.m_nMultiColumn=j; //���䵱ǰ�������
							return false;
						}
						if(!WaitStopXY(10000))
						{
							ProductParam.tgTrackInfoMulti.m_nMultiRow=i;  //���䵱ǰ�������
							ProductParam.tgTrackInfoMulti.m_nMultiColumn=j; //���䵱ǰ�������
							return false;
						}
					}
					if(!SynchronizeMoveZ(za,zb,zc,true,true))
					{
						ProductParam.tgTrackInfoMulti.m_nMultiRow=i;  //���䵱ǰ�������
						ProductParam.tgTrackInfoMulti.m_nMultiColumn=j; //���䵱ǰ�������
						return false;
					}
					if(m_bFirstDisp)
					{
						CFunction::DelayEx(ProductParam.dFirstDispDelay);
						m_bFirstDisp = false;
					}
					CFunction::DelayEx(ProductParam.dPreTime);
					////////////////////////////
					dz1=m_pMv->GetPos(K_AXIS_ZA);
					dz2=m_pMv->GetPos(K_AXIS_ZB);
                    dz3=m_pMv->GetPos(K_AXIS_ZC);
					str.Format("���ģʽ|z���滮λ�ã�Z1:%0.3f;Z2��%0.3f;Z3��%0.3f.",dz1,dz2,dz3);
					PutLogIntoList(str);
					dz1=m_pMv->GetEncPos(K_AXIS_ZA);
					dz2=m_pMv->GetEncPos(K_AXIS_ZB);
					dz3=m_pMv->GetEncPos(K_AXIS_ZC);
					str.Format("���ģʽ|z���ʵ��λ�ã�Z1:%0.3f;Z2��%0.3f;Z3��%0.3f.",dz1,dz2,dz3);
					PutLogIntoList(str);
					///////////////////////////////////////////////////////////////////////////////////

					if(!ProductParam.DispMode)//����ģʽ
					{						
						if(theApp.m_V9Control.V9C_Dot(K_SELECT_AXIS,false))
						{
							V9DotRest = TRUE;
						}
					}
					if ((ProductParam.dPostDisTime<=0.005)&&!ProductParam.DispMode)
					{   
						ProductParam.dPostDisTime=0.005;
					}
					if(!CTimeUtil::Delay(long(ProductParam.dPostDisTime*1000)))
					{
						str.Format("Delay(ProductParam.dPostDisTime) = false");
						PutLogIntoList(str);
					}
					if(!RunZUp(za,zb,zc))
					{
						ProductParam.tgTrackInfoMulti.m_nMultiRow=i;  //���䵱ǰ�������
						ProductParam.tgTrackInfoMulti.m_nMultiColumn=j+1; //���䵱ǰ�������
						return false;
					}

					//�����߶ȴ�������ͣ�ź�//2016-03-11
					if(m_tStatus == K_RUN_STS_PAUSE)
					{
						if(!WaitPasueEnd())
						{
							ProductParam.tgTrackInfoMulti.m_nMultiRow=i; //���䵱ǰ�������
							ProductParam.tgTrackInfoMulti.m_nMultiColumn=j+1; //�����´��������
							return false;
						}
					}
					if(IsRunStop())
					{
						ProductParam.tgTrackInfoMulti.m_nMultiRow=i; //���䵱ǰ�������
						ProductParam.tgTrackInfoMulti.m_nMultiColumn=j+1; //�����´��������
						return false;
					}

					if (V9DotRest&&RunDot_RestV9)
					{
						if(!RunValveOperation())
						{
							ProductParam.tgTrackInfoMulti.m_nMultiRow=i; //���䵱ǰ�������
							ProductParam.tgTrackInfoMulti.m_nMultiColumn=j+1; //�����´��������
							return false;
						}
					}
				}
			}
			else
			{
				if(!m_bStartSubRun)
				{
					ProductParam.tgTrackInfoMulti.m_nMultiColumn=ProductParam.MultiDispColumn-1;
				}
				for(int j=ProductParam.tgTrackInfoMulti.m_nMultiColumn;j>=0;j--)
				{
					if(m_bStartSubRun)
					{
						m_bStartSubRun=false;
					}
					dOffsetX = j*ProductParam.MultiDispColumnD;
					dOffsetY = i*ProductParam.MultiDispRowD;
					if(m_bRepair||m_bFirstDisp)
					{
						if(!SynchronizeMoveXY(x+dOffsetX,y+dOffsetY,true))
						{
							ProductParam.tgTrackInfoMulti.m_nMultiRow=i;   //���䵱ǰ�������
							ProductParam.tgTrackInfoMulti.m_nMultiColumn=j;  //���䵱ǰ�������
							return false;
						}
						if(IsExitRun())
						{
							ProductParam.tgTrackInfoMulti.m_nMultiRow=i;   //���䵱ǰ�������
							ProductParam.tgTrackInfoMulti.m_nMultiColumn=j;  //���䵱ǰ�������
							MoveToZSafety();
							return false;
						}
						if(!SynchronizeMoveZ(za-ProductParam.dZupHigh*0.5,zb-ProductParam.dZupHigh*0.5,zc-ProductParam.dZupHigh*0.5,true,true))
						{
							ProductParam.tgTrackInfoMulti.m_nMultiRow=i;   //���䵱ǰ�������
							ProductParam.tgTrackInfoMulti.m_nMultiColumn=j;  //���䵱ǰ�������
							return false;
						}
					}
					else
					{
						if(!SynchronizeMoveXY(x+dOffsetX,y+dOffsetY,false))
						{
							ProductParam.tgTrackInfoMulti.m_nMultiRow=i;   //���䵱ǰ�������
							ProductParam.tgTrackInfoMulti.m_nMultiColumn=j;  //���䵱ǰ�������
							return false;
						}
						if(IsExitRun())
						{
							ProductParam.tgTrackInfoMulti.m_nMultiRow=i;   //���䵱ǰ�������
							ProductParam.tgTrackInfoMulti.m_nMultiColumn=j;  //���䵱ǰ�������
							MoveToZSafety();
							return false;
						}
						if(!SynchronizeMoveZ(za-ProductParam.dZupHigh*0.5,zb-ProductParam.dZupHigh*0.5,zc-ProductParam.dZupHigh*0.5,true,true))
						{
							ProductParam.tgTrackInfoMulti.m_nMultiRow=i;   //���䵱ǰ�������
							ProductParam.tgTrackInfoMulti.m_nMultiColumn=j;  //���䵱ǰ�������
							return false;
						}
						if(!WaitStopXY(10000))
						{
							ProductParam.tgTrackInfoMulti.m_nMultiRow=i;   //���䵱ǰ�������
							ProductParam.tgTrackInfoMulti.m_nMultiColumn=j;  //���䵱ǰ�������
							return false;
						}
					}
					if(!SynchronizeMoveZ(za,zb,zc,true,true))
					{
						ProductParam.tgTrackInfoMulti.m_nMultiRow=i;   //���䵱ǰ�������
						ProductParam.tgTrackInfoMulti.m_nMultiColumn=j;  //���䵱ǰ�������
						return false;
					}
					if(m_bFirstDisp)
					{
						CFunction::DelayEx(ProductParam.dFirstDispDelay);
						m_bFirstDisp = false;
					}
					CFunction::DelayEx(ProductParam.dPreTime);
					dz1=m_pMv->GetPos(K_AXIS_ZA);
                    dz2=m_pMv->GetPos(K_AXIS_ZB);
					dz3=m_pMv->GetPos(K_AXIS_ZC);
					str.Format("���ģʽ|z���滮λ�ã�Z1:%0.3f;Z2��%0.3f;Z3��%0.3f.",dz1,dz2,dz3);
					PutLogIntoList(str);
					dz1=m_pMv->GetEncPos(K_AXIS_ZA);
					dz2=m_pMv->GetEncPos(K_AXIS_ZB);
					dz3=m_pMv->GetEncPos(K_AXIS_ZC);
                    str.Format("���ģʽ|z���ʵ��λ�ã�Z1:%0.3f;Z2��%0.3f;Z3��%0.3f.",dz1,dz2,dz3);
                    PutLogIntoList(str);

					if(!ProductParam.DispMode)//����ģʽ
					{						
						if(theApp.m_V9Control.V9C_Dot(K_SELECT_AXIS,false))
						{
							V9DotRest = TRUE;
						}
					}
					if ((ProductParam.dPostDisTime<=0.005)&&!ProductParam.DispMode)
					{   
						ProductParam.dPostDisTime=0.005;
					}
					if(!CTimeUtil::Delay(long(ProductParam.dPostDisTime*1000)))
					{
						str.Format("Delay(ProductParam.dPostDisTime) = false");
						PutLogIntoList(str);
					}
					if(!RunZUp(za,zb,zc))
					{
						ProductParam.tgTrackInfoMulti.m_nMultiRow=i; //���䵱ǰ�������
						ProductParam.tgTrackInfoMulti.m_nMultiColumn=j-1;  //�����´��������
						return false;
					}

					//�����߶ȴ�������ͣ�ź�//2016-03-11
					if(m_tStatus == K_RUN_STS_PAUSE)
					{
						if(!WaitPasueEnd())
						{
							ProductParam.tgTrackInfoMulti.m_nMultiRow=i; //���䵱ǰ�������
							ProductParam.tgTrackInfoMulti.m_nMultiColumn=j-1;  //�����´��������
							return false;
						}
					}
					if(IsRunStop())
					{
						ProductParam.tgTrackInfoMulti.m_nMultiRow=i; //���䵱ǰ�������
						ProductParam.tgTrackInfoMulti.m_nMultiColumn=j-1;  //�����´��������
						return false;
					}

					if(V9DotRest&&RunDot_RestV9)
					{
						if(!RunValveOperation())
						{
							ProductParam.tgTrackInfoMulti.m_nMultiRow=i; //���䵱ǰ�������
							ProductParam.tgTrackInfoMulti.m_nMultiColumn=j-1;  //�����´��������
							return false;
						}
					}
				}
			}
		}
		m_bDispenseSuccess = true;
		ProductParam.tgTrackInfoMulti.TrackReset();
	}
	else
	{
		if(m_bRepair||m_bFirstDisp)
		{
			if(!SynchronizeMoveXY(x,y,true))
			{
				return false;
			}
			if(!SynchronizeMoveZ(za-ProductParam.dZupHigh,zb-ProductParam.dZupHigh,
				zc-ProductParam.dZupHigh,true, true))
			{
				return false;
			}
		}
		else
		{
			if(!SynchronizeMoveXY(x,y,false))
			{
				return false;
			}
			if(!SynchronizeMoveZ(za-ProductParam.dZupHigh,zb-ProductParam.dZupHigh,
				zc-ProductParam.dZupHigh,true,true))
			{
				return false;
			}
			if(!WaitStopXY(10000))
			{
				return false;
			}
		}
		if(!SynchronizeMoveZ(za,zb,zc,true,true))
		{
			return false;
		}
		//if(m_bFirstDisp)
		//{
		//	CFunction::DelayEx(ProductParam.dFirstDispDelay);
		//	m_bFirstDisp = false;
		//}
		if(!CTimeUtil::Delay(long(ProductParam.dPreTime*1000)))
		{
			str.Format("Delay(ProductParam.dPreTime) = false");
			PutLogIntoList(str);
		}
        dz1=m_pMv->GetPos(K_AXIS_ZA);
		dz2=m_pMv->GetPos(K_AXIS_ZB);
		dz3=m_pMv->GetPos(K_AXIS_ZC);
		str.Format("����ģʽ|z���滮λ�ã�Z1:%0.3f;Z2��%0.3f;Z3��%0.3f.",dz1,dz2,dz3);
        PutLogIntoList(str);
		dz1=m_pMv->GetEncPos(K_AXIS_ZA);
		dz2=m_pMv->GetEncPos(K_AXIS_ZB);
		dz3=m_pMv->GetEncPos(K_AXIS_ZC);
        str.Format("����ģʽ|z���ʵ��λ�ã�Z1:%0.3f;Z2��%0.3f;Z3��%0.3f.",dz1,dz2,dz3);
		PutLogIntoList(str);
		//�㽺�߶ȴ�������ͣ�ź�//2016-03-11
		if (m_tStatus == K_RUN_STS_PAUSE)
		{
			if(!KeepStillWhenPause())
			{
				return false;
			}
		}

		if(!ProductParam.DispMode)//����ģʽ
		{						
			if(theApp.m_V9Control.V9C_Dot(K_SELECT_AXIS,false))
			{
				V9DotRest = TRUE;
			}
		}
		m_bDispenseSuccess = true;
		if(m_bFirstDisp)
		{
			if(!CTimeUtil::Delay(long(ProductParam.dFirstDispDelay*1000)))
			{
				str.Format("Delay(ProductParam.dFirstDispDelay) = false");
				PutLogIntoList(str);
			}
			m_bFirstDisp = false;
		}
		if ((ProductParam.dPostDisTime<=0.005)&&!ProductParam.DispMode)
		{   
			ProductParam.dPostDisTime=0.005;
		}
		if(!CTimeUtil::Delay(long(ProductParam.dPostDisTime*1000)))
		{
			str.Format("Delay(ProductParam.dPostDisTime) = false");
			PutLogIntoList(str);
		}

		if(!RunZUp(za,zb,zc))
		{
			return false;
		}

		//�����߶ȴ�������ͣ�ź�//2016-03-11
		if (m_tStatus == K_RUN_STS_PAUSE)
		{
			if(!KeepStillWhenPause())
			{
				return false;
			}
		}
		if(IsRunStop())
		{
			return false;
		}

		if (V9DotRest&&RunDot_RestV9)
		{
			if(!RunValveOperation())
			{
				return false;
			}
		}
	}
	return true;
}

bool CCmdRun::RunSingleDot(double x,double y,double za,double zb,double zc,double dCircleRadius,double dCircleDistance,bool RunDot_RestV9)
{
	CString str;
	double dz1,dz2,dz3;
	dz1=dz2=dz3=0;
	m_bDispenseSuccess = false;
	if (m_tStatus == K_RUN_STS_PAUSE)
	{
		if(!WaitPasueEnd())
		{
			str.Format("����ģʽS1ʧ��...");
			PutLogIntoList(str);
			return false;
		}
	}
	double totalLength = 0;
	double dTemp = dCircleRadius;
	double dRadius = 0.0;
	while(dTemp-dCircleDistance>0.1)
	{
		dRadius = dTemp;
		totalLength += 2*KGB_PI*dTemp*(1+ProductParam.dCompAngle/360);
		dTemp -= dCircleDistance;
	}
	if(!ProductParam.DispMode)
	{
		theApp.m_V9Control.V9_RunCount++;
	}
	if(m_bRepair||m_bFirstDisp)
	{
		if(!SynchronizeMoveXY(x,y,true))
		{
			str.Format("����ģʽS2ʧ��...");
			PutLogIntoList(str);
			return false;
		}
		if(!SynchronizeMoveZ(za-ProductParam.dZupHigh,zb-ProductParam.dZupHigh,
			zc-ProductParam.dZupHigh,true,true))
		{
			str.Format("����ģʽS3ʧ��...");
			PutLogIntoList(str);
			return false;
		}
	}
	else
	{
		if(!SynchronizeMoveXY(x,y,false))
		{
			str.Format("����ģʽS4ʧ��...");
			PutLogIntoList(str);
			return false;
		}
		if(!SynchronizeMoveZ(za-ProductParam.dZupHigh,zb-ProductParam.dZupHigh,
			zc-ProductParam.dZupHigh,true,true))
		{
			str.Format("����ģʽS5ʧ��...");
			PutLogIntoList(str);
			return false;
		}
		if(!WaitStopXY(10000))
		{
			str.Format("����ģʽS6ʧ��...");
			PutLogIntoList(str);
			return false;
		}
	}
	if(!SynchronizeMoveZ(za,zb,zc,true,true))
	{
		str.Format("����ģʽS7ʧ��...");
		PutLogIntoList(str);
		return false;
	}
	dz1=m_pMv->GetPos(K_AXIS_ZA);
	dz2=m_pMv->GetPos(K_AXIS_ZB);
	dz3=m_pMv->GetPos(K_AXIS_ZC);
	str.Format("����ģʽ|z���滮λ�ã�Z1:%0.3f;Z2��%0.3f;Z3��%0.3f.",dz1,dz2,dz3);
	PutLogIntoList(str);
    dz1=m_pMv->GetEncPos(K_AXIS_ZA);
	dz2=m_pMv->GetEncPos(K_AXIS_ZB);
	dz3=m_pMv->GetEncPos(K_AXIS_ZC);
	str.Format("����ģʽ|z���ʵ��λ�ã�Z1:%0.3f;Z2��%0.3f;Z3��%0.3f.",dz1,dz2,dz3);
	PutLogIntoList(str);

	if(m_tStatus == K_RUN_STS_PAUSE)  
	{
		if(!WaitPasueEnd())
		{
			str.Format("����ģʽS8ʧ��...");
			PutLogIntoList(str);
			return false;
		}
	}

	while(dCircleRadius-dCircleDistance>0.1)
	{
		m_pMv->ListAddLine(x-dCircleRadius,y,ProductParam.CircleVel,ProductParam.CircleAcc);
		if(!ProductParam.DispMode)
		{
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if(g_pFrm->m_CmdRun.RunDispFSelect[i])
				{
					if((VALVE_M9==theApp.m_tSysParam.ValveType)||(VALVE_L9==theApp.m_tSysParam.ValveType))
					{
						m_pMv->AddBufGear(K_AXIS_U+i,g_pFrm->m_CmdRun.V9Parm[i].DispDistance*(2*KGB_PI*dCircleRadius)/totalLength);
					}
					else if(VALVE_H9==theApp.m_tSysParam.ValveType)
					{
						m_pMv->AddBufGear(K_AXIS_U+i,-g_pFrm->m_CmdRun.V9Parm[i].DispDistance*(2*KGB_PI*dCircleRadius)/totalLength);
					}
				}
			}
		}
		m_pMv->ListAddArc(x-dCircleRadius,y,dCircleRadius,0,0,ProductParam.CircleVel,ProductParam.CircleAcc);
		if(!ProductParam.DispMode)
		{
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if(g_pFrm->m_CmdRun.RunDispFSelect[i])
				{
					if((VALVE_M9==theApp.m_tSysParam.ValveType)||(VALVE_L9==theApp.m_tSysParam.ValveType))
					{
						m_pMv->AddBufGear(K_AXIS_U+i,g_pFrm->m_CmdRun.V9Parm[i].DispDistance*(2*KGB_PI*dCircleRadius*(ProductParam.dCompAngle/360.0))/totalLength);
					}
					else if(VALVE_H9==theApp.m_tSysParam.ValveType)
					{
						m_pMv->AddBufGear(K_AXIS_U+i,-g_pFrm->m_CmdRun.V9Parm[i].DispDistance*(2*KGB_PI*dCircleRadius*(ProductParam.dCompAngle/360.0))/totalLength);
					}
				}
			}
		}
		m_pMv->ListAddArc(x-dCircleRadius*cos(ProductParam.dCompAngle/180.0*KGB_PI),y+dCircleRadius*sin(ProductParam.dCompAngle/180.0*KGB_PI),ProductParam.dCompAngle<180?dCircleRadius:-dCircleRadius,0,ProductParam.CircleVel,ProductParam.CircleAcc);
		dCircleRadius -= dCircleDistance;
	}
	if(!m_pMv->AllListRun())
	{
		str.Format("����ģʽS9ʧ��...");
		PutLogIntoList(str);
		Alarm(15);
		AddMsg("�˶��������˶�ʧ�ܣ�");
		return false;
	}
	m_bDispenseSuccess = true;
	if ((ProductParam.dPostDisTime<=0.005)&&!ProductParam.DispMode)
	{   
		ProductParam.dPostDisTime=0.005;
	}
	CTimeUtil::Delay(long(ProductParam.dPostDisTime*1000));
	if(!SetAxisProfile(HIGH_VEL))
	{
		str.Format("����ģʽS10ʧ��...");
		PutLogIntoList(str);
		Alarm(30);
		AddMsg("�岹�˶����趨�ٶ�ʧ�ܣ�");
		return false;
	}
	if(!theApp.m_V9Control.V9C_SetProfile())
	{
		str.Format("����ģʽS11ʧ��...");
		PutLogIntoList(str);
		Alarm(30);
		AddMsg("�������ò�������");
		return false;
	}
	
	if(!RunZUp(za,zb,zc))
	{
		str.Format("����ģʽS12ʧ��...");
		PutLogIntoList(str);
		return false;
	}

	if (m_tStatus == K_RUN_STS_PAUSE)  //2016-03-11
	{
		if(!WaitPasueEnd())
		{
			str.Format("����ģʽS13ʧ��...");
			PutLogIntoList(str);
			return false;
		}
	}

	int nCount = V9Parm[0].DispNum;
	for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
	{
		if(nCount>=V9Parm[i].DispNum)
		{
			nCount = V9Parm[i].DispNum;
		}
	}
	if(theApp.m_V9Control.V9_RunCount>=nCount)
	{
		V9DotRest=true;
	}
	if (V9DotRest&&RunDot_RestV9)
	{
		if(!RunValveOperation())
		{
			str.Format("����ģʽS14ʧ��...");
			PutLogIntoList(str);
			return false;
		}
	}
	return true;
}

bool CCmdRun::RunSingleDot(double x,double y,double za,double zb,double zc,double dLength,bool bVertical,int DisBufCout,bool RunDot_RestV9)
{
	CString str;
	double dz1,dz2,dz3;
	dz1=dz2=dz3=0;
	m_bDispenseSuccess = false;
	if (m_tStatus == K_RUN_STS_PAUSE)
	{
		if(!WaitPasueEnd())
		{
			return false;
		}
	}
	if(IsRunStop())
	{
		return false;
	}
	if(bVertical)
	{
		if(m_bRepair||m_bFirstDisp)
		{
			if(!SynchronizeMoveXY(x,y-dLength/2,true))
			{
				return false;
			}
			if(!SynchronizeMoveZ(za-ProductParam.dZupHigh,zb-ProductParam.dZupHigh,
				zc-ProductParam.dZupHigh,true,true))
			{
				return false;
			}
		}
		else
		{
			if(!SynchronizeMoveXY(x,y-dLength/2,false))
			{
				return false;
			}
			if(!SynchronizeMoveZ(za-ProductParam.dZupHigh,zb-ProductParam.dZupHigh,
				zc-ProductParam.dZupHigh,true,true))
			{
				return false;
			}
			if(!WaitStopXY(10000))
			{
				return false;
			}
		}
		if(!SynchronizeMoveZ(za,zb,zc,true,true))
		{
			return false;
		}
	}
	else
	{
		if(m_bRepair||m_bFirstDisp)
		{
			if(!SynchronizeMoveXY(x-dLength/2,y,true))
			{
				return false;
			}
			if(!SynchronizeMoveZ(za-ProductParam.dZupHigh,zb-ProductParam.dZupHigh,
				zc-ProductParam.dZupHigh,true,true))
			{
				return false;
			}
		}
		else
		{
			if(!SynchronizeMoveXY(x-dLength/2,y,false))
			{
				return false;
			}
			if(!SynchronizeMoveZ(za-ProductParam.dZupHigh,zb-ProductParam.dZupHigh,
				zc-ProductParam.dZupHigh,true,true))
			{
				return false;
			}
			if(!WaitStopXY(10000))
			{
				return false;
			}
		}
		if(!SynchronizeMoveZ(za,zb,zc,true,true))
		{
			return false;
		}
	}
	if(m_bFirstDisp)
	{
		CTimeUtil::Delay(long(ProductParam.dFirstDispDelay*1000));
		m_bFirstDisp = false;
	}
	CTimeUtil::Delay(long(ProductParam.dPreTime*1000));

	dz1=m_pMv->GetPos(K_AXIS_ZA);
	dz2=m_pMv->GetPos(K_AXIS_ZB);
	dz3=m_pMv->GetPos(K_AXIS_ZC);
	str.Format("�߶�ģʽ|z���滮λ�ã�Z1:%0.3f;Z2��%0.3f;Z3��%0.3f.",dz1,dz2,dz3);
	PutLogIntoList(str);

	dz1=m_pMv->GetEncPos(K_AXIS_ZA);
	dz2=m_pMv->GetEncPos(K_AXIS_ZB);
	dz3=m_pMv->GetEncPos(K_AXIS_ZC);
	str.Format("�߶�ģʽ|z���ʵ��λ�ã�Z1:%0.3f;Z2��%0.3f;Z3��%0.3f.",dz1,dz2,dz3);
	PutLogIntoList(str);


	if (m_tStatus == K_RUN_STS_PAUSE)  //2016-03-11
	{
		if(!KeepStillWhenPause())
		{
			return false;
		}
	}

	if(!ProductParam.DispMode)
	{
		theApp.m_V9Control.V9C_Dot_Forward(K_SELECT_AXIS);// ��ǰ����������������ģʽ
	}

	m_pMv->SetCrdPrm(FALSE,1,2);//��������ϵ������

	if(!ProductParam.DispMode)
	{
		V9DotRest = theApp.m_V9Control.V9C_Dot_Line(K_SELECT_AXIS,true);//����ģʽ����������
	}
	if(bVertical)
	{
		m_pMv->ListAddLine(x,y+dLength/2,ProductParam.LineVel,ProductParam.LineAcc);
	}
	else
	{
		m_pMv->ListAddLine(x+dLength/2,y,ProductParam.LineVel,ProductParam.LineAcc);
	}
	if(!m_pMv->AllListRun())
	{
		Alarm(15);
		AddMsg("�˶��������˶�ʧ��!");
		return false;
	}
	m_bDispenseSuccess = true;
	if ((ProductParam.dPostDisTime<=0.005)&&!ProductParam.DispMode)
	{   
		ProductParam.dPostDisTime=0.005;
	}
	CTimeUtil::Delay(long(ProductParam.dPostDisTime*1000));
	if(!SetAxisProfile(HIGH_VEL))
	{
		Alarm(30);
		AddMsg("�岹�˶����趨�ٶ�ʧ�ܣ�");
		return false;
	}
	if(!theApp.m_V9Control.V9C_SetProfile())
	{
		Alarm(30);
		AddMsg("�岹�˶����趨�����ٶ�ʧ�ܣ�");
		return false;
	}
	//if(!SynchronizeMoveXY(x,y,true))
	//{
	//	return false;
	//}

	if(!RunZUp(za,zb,zc))
	{
		return false;
	}

	if (m_tStatus == K_RUN_STS_PAUSE)  //2016-03-11
	{
		if(!KeepStillWhenPause())
		{
			return false;
		}
	}

	if(IsRunStop())//2016-03-03 :Del the Not Condition
	{
		return false;
	}

	if (V9DotRest&&RunDot_RestV9)
	{
		if(!RunValveOperation())
		{
			return false;
		}
	}
	return true;
}

//����ģʽ��չ
bool CCmdRun::RunSingleDotComplex(double x,double y,double za,double zb,double zc,double dLength,bool bVertical,int DisBufCout,bool RunDot_RestV9)
{
	CString str;
	double dz1,dz2,dz3;
	dz1=dz2=dz3=0;
	m_bDispenseSuccess = false;
	if (m_tStatus == K_RUN_STS_PAUSE)
	{
		if(!WaitPasueEnd())
		{
			return false;
		}
	}
	if(IsRunStop())
	{
		return false;
	}
	if(bVertical)
	{
		if(m_bRepair||m_bFirstDisp)
		{
			if(!SynchronizeMoveXY(x,y,true))
			{
				return false;
			}
			if(!SynchronizeMoveZ(za-ProductParam.dZupHigh,zb-ProductParam.dZupHigh,
				zc-ProductParam.dZupHigh,true,true))
			{
				return false;
			}
		}
		else
		{
			if(!SynchronizeMoveXY(x,y,false))
			{
				return false;
			}
			if(!SynchronizeMoveZ(za-ProductParam.dZupHigh,zb-ProductParam.dZupHigh,
				zc-ProductParam.dZupHigh,true,true))
			{
				return false;
			}
			if(!WaitStopXY(10000))
			{
				return false;
			}
		}
		if(!SynchronizeMoveZ(za,zb,zc,true,true))
		{
			return false;
		}
	}
	else
	{
		if(m_bRepair||m_bFirstDisp)
		{
			if(!SynchronizeMoveXY(x,y,true))
			{
				return false;
			}
			if(!SynchronizeMoveZ(za-ProductParam.dZupHigh,zb-ProductParam.dZupHigh,
				zc-ProductParam.dZupHigh,true,true))
			{
				return false;
			}
		}
		else
		{
			if(!SynchronizeMoveXY(x,y,false))
			{
				return false;
			}
			if(!SynchronizeMoveZ(za-ProductParam.dZupHigh,zb-ProductParam.dZupHigh,
				zc-ProductParam.dZupHigh,true,true))
			{
				return false;
			}
			if(!WaitStopXY(10000))
			{
				return false;
			}
		}
		if(!SynchronizeMoveZ(za,zb,zc,true,true))
		{
			return false;
		}
	}
	if(m_bFirstDisp)
	{
		CTimeUtil::Delay(long(ProductParam.dFirstDispDelay*1000));
		m_bFirstDisp = false;
	}
	CTimeUtil::Delay(long(ProductParam.dPreTime*1000));

	dz1=m_pMv->GetPos(K_AXIS_ZA);
	dz2=m_pMv->GetPos(K_AXIS_ZB);
	dz3=m_pMv->GetPos(K_AXIS_ZC);
	str.Format("�߶�ģʽ|z���滮λ�ã�Z1:%0.3f;Z2��%0.3f;Z3��%0.3f.",dz1,dz2,dz3);
	PutLogIntoList(str);

	dz1=m_pMv->GetEncPos(K_AXIS_ZA);
	dz2=m_pMv->GetEncPos(K_AXIS_ZB);
	dz3=m_pMv->GetEncPos(K_AXIS_ZC);
	str.Format("�߶�ģʽ|z���ʵ��λ�ã�Z1:%0.3f;Z2��%0.3f;Z3��%0.3f.",dz1,dz2,dz3);
	PutLogIntoList(str);

	if (m_tStatus == K_RUN_STS_PAUSE)  //2016-03-11
	{
		if(!KeepStillWhenPause())
		{
			return false;
		}
	}

	if(!ProductParam.DispMode)
	{
		theApp.m_V9Control.V9C_Dot_Forward(K_SELECT_AXIS);// ��ǰ����������������ģʽ
	}

	m_pMv->SetCrdPrm(FALSE,1,2);//��������ϵ������

	if(!ProductParam.DispMode)
	{
		V9DotRest = theApp.m_V9Control.V9C_Dot_Line(K_SELECT_AXIS,true);//����ģʽ����������
	}
	if(bVertical)
	{
		m_pMv->ListAddLine(x,y+dLength,ProductParam.LineVel,ProductParam.LineAcc);
	}
	else
	{
		m_pMv->ListAddLine(x+dLength,y,ProductParam.LineVel,ProductParam.LineAcc);
	}
	if(!m_pMv->AllListRun())
	{
		Alarm(15);
		AddMsg("�˶��������˶�ʧ��!");
		return false;
	}
	m_bDispenseSuccess = true;
	if ((ProductParam.dPostDisTime<=0.005)&&!ProductParam.DispMode)
	{   
		ProductParam.dPostDisTime=0.005;
	}
	CTimeUtil::Delay(long(ProductParam.dPostDisTime*1000));
	if(!SetAxisProfile(HIGH_VEL))
	{
		Alarm(30);
		AddMsg("�岹�˶����趨�ٶ�ʧ�ܣ�");
		return false;
	}
	if(!theApp.m_V9Control.V9C_SetProfile())
	{
		Alarm(30);
		AddMsg("�岹�˶����趨�����ٶ�ʧ�ܣ�");
		return false;
	}
	//if(!SynchronizeMoveXY(x,y,true))
	//{
	//	return false;
	//}

	if(!RunZUp(za,zb,zc))
	{
		return false;
	}

	if (m_tStatus == K_RUN_STS_PAUSE)  //2016-03-11
	{
		if(!KeepStillWhenPause())
		{
			return false;
		}
	}

	if(IsRunStop())//2016-03-03 :Del the Not Condition
	{
		return false;
	}

	if (V9DotRest&&RunDot_RestV9)
	{
		if(!RunValveOperation())
		{
			return false;
		}
	}
	return true;
}

bool CCmdRun::RunSingleDot(double x,double y,double za,double zb,double zc,double dRectWidth,double dRectLength,int DisBufCout,bool RunDot_RestV9)
{
    CString str;
    double dz1,dz2,dz3;
	dz1=dz2=dz3=0;
	m_bDispenseSuccess = false;
	if (m_tStatus == K_RUN_STS_PAUSE)
	{
		if(!WaitPasueEnd())
		{
			return false;
		}
	}
	if(IsRunStop())
	{
		return false;
	}
	if(m_bRepair||m_bFirstDisp)
	{
		if(!SynchronizeMoveXY(x-dRectWidth*0.5,y-dRectLength*0.5,true))
		{
			return false;
		}
		if(!SynchronizeMoveZ(za-ProductParam.dZupHigh,zb-ProductParam.dZupHigh,
			zc-ProductParam.dZupHigh,true,true))
		{
			return false;
		}
	}
	else
	{
		if(!SynchronizeMoveXY(x-dRectWidth*0.5,y-dRectLength*0.5,false))
		{
			return false;
		}
		if(!SynchronizeMoveZ(za-ProductParam.dZupHigh,zb-ProductParam.dZupHigh,
			zc-ProductParam.dZupHigh,true,true))
		{
			return false;
		}
		if(!WaitStopXY(10000))
		{
			return false;
		}
	}
	if(!SynchronizeMoveZ(za,zb,zc,true,true))
	{
		return false;
	}
	if(m_bFirstDisp)
	{
		CTimeUtil::Delay(long(ProductParam.dFirstDispDelay*1000));
		m_bFirstDisp = false;
	}
	CTimeUtil::Delay(long(ProductParam.dPreTime*1000));

    dz1=m_pMv->GetPos(K_AXIS_ZA);
	dz2=m_pMv->GetPos(K_AXIS_ZB);
	dz3=m_pMv->GetPos(K_AXIS_ZC);
	str.Format("����ģʽ|z���滮λ�ã�Z1:%0.3f;Z2��%0.3f;Z3��%0.3f.",dz1,dz2,dz3);
	PutLogIntoList(str);
    dz1=m_pMv->GetEncPos(K_AXIS_ZA);
	dz2=m_pMv->GetEncPos(K_AXIS_ZB);
	dz3=m_pMv->GetEncPos(K_AXIS_ZC);
	str.Format("����ģʽ|z���ʵ��λ�ã�Z1:%0.3f;Z2��%0.3f;Z3��%0.3f.",dz1,dz2,dz3);
    PutLogIntoList(str);

	if (m_tStatus == K_RUN_STS_PAUSE)  //2016-03-11
	{
		if(!KeepStillWhenPause())
		{
			return false;
		}
	}
	double dTotalLength = 2*(dRectLength+dRectWidth);
	m_pMv->SetCrdPrm(FALSE,1,2);
	if(dRectLength>=dRectWidth)
	{
		if(!ProductParam.DispMode)//����ģʽ
		{
			theApp.m_V9Control.V9_RunCount++;
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if(g_pFrm->m_CmdRun.RunDispFSelect[i])
				{
					m_pMv->AddBufGear(K_AXIS_U+i,g_pFrm->m_CmdRun.V9Parm[i].DispDistance*dRectLength/dTotalLength);
				}
			}
		}
		m_pMv->ListAddLine(x-dRectWidth*0.5,y+dRectLength*0.5,ProductParam.LineVel,ProductParam.LineAcc);
		if(!ProductParam.DispMode)//����ģʽ
		{
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if(g_pFrm->m_CmdRun.RunDispFSelect[i])
				{
					m_pMv->AddBufGear(K_AXIS_U+i,g_pFrm->m_CmdRun.V9Parm[i].DispDistance*dRectWidth/dTotalLength);
				}
			}
		}
		m_pMv->ListAddLine(x+dRectWidth*0.5,y+dRectLength*0.5,ProductParam.LineVel,ProductParam.LineAcc);
		if(!ProductParam.DispMode)//����ģʽ
		{
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if(g_pFrm->m_CmdRun.RunDispFSelect[i])
				{
					m_pMv->AddBufGear(K_AXIS_U+i,g_pFrm->m_CmdRun.V9Parm[i].DispDistance*dRectLength/dTotalLength);
				}
			}
		}
		m_pMv->ListAddLine(x+dRectWidth*0.5,y-dRectLength*0.5,ProductParam.LineVel,ProductParam.LineAcc);
		if(!ProductParam.DispMode)//����ģʽ
		{
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if(g_pFrm->m_CmdRun.RunDispFSelect[i])
				{
					m_pMv->AddBufGear(K_AXIS_U+i,g_pFrm->m_CmdRun.V9Parm[i].DispDistance*dRectWidth/dTotalLength);
				}
			}
		}
		m_pMv->ListAddLine(x-dRectWidth*0.5,y-dRectLength*0.5,ProductParam.LineVel,ProductParam.LineAcc);
	}
	else
	{
		if(!ProductParam.DispMode)//����ģʽ
		{
			theApp.m_V9Control.V9_RunCount++;
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if(g_pFrm->m_CmdRun.RunDispFSelect[i])
				{
					m_pMv->AddBufGear(K_AXIS_U+i,g_pFrm->m_CmdRun.V9Parm[i].DispDistance*dRectWidth/dTotalLength);
				}
			}
		}
		m_pMv->ListAddLine(x+dRectWidth*0.5,y-dRectLength*0.5,ProductParam.LineVel,ProductParam.LineAcc);
		if(!ProductParam.DispMode)//����ģʽ
		{
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if(g_pFrm->m_CmdRun.RunDispFSelect[i])
				{
					m_pMv->AddBufGear(K_AXIS_U+i,g_pFrm->m_CmdRun.V9Parm[i].DispDistance*dRectLength/dTotalLength);
				}
			}
		}
		m_pMv->ListAddLine(x+dRectWidth*0.5,y+dRectLength*0.5,ProductParam.LineVel,ProductParam.LineAcc);
		if(!ProductParam.DispMode)//����ģʽ
		{
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if(g_pFrm->m_CmdRun.RunDispFSelect[i])
				{
					m_pMv->AddBufGear(K_AXIS_U+i,g_pFrm->m_CmdRun.V9Parm[i].DispDistance*dRectWidth/dTotalLength);
				}
			}
		}
		m_pMv->ListAddLine(x-dRectWidth*0.5,y+dRectLength*0.5,ProductParam.LineVel,ProductParam.LineAcc);
		if(!ProductParam.DispMode)//����ģʽ
		{
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if(g_pFrm->m_CmdRun.RunDispFSelect[i])
				{
					m_pMv->AddBufGear(K_AXIS_U+i,g_pFrm->m_CmdRun.V9Parm[i].DispDistance*dRectLength/dTotalLength);
				}
			}
		}
		m_pMv->ListAddLine(x-dRectWidth*0.5,y-dRectLength*0.5,ProductParam.LineVel,ProductParam.LineAcc);
	}
	if(!m_pMv->AllListRun())
	{
		Alarm(15);
		AddMsg("�˶��������˶�ʧ��!");
		return false;
	}
	if ((ProductParam.dPostDisTime<=0.005)&&!ProductParam.DispMode)
	{   
		ProductParam.dPostDisTime=0.005;
	}
	CTimeUtil::Delay(long(ProductParam.dPostDisTime*1000));
	if(!SetAxisProfile(HIGH_VEL))
	{
		Alarm(30);
		AddMsg("�岹�˶����趨�ٶ�ʧ�ܣ�");
		return false;
	}
	if(!theApp.m_V9Control.V9C_SetProfile())
	{
		Alarm(30);
		AddMsg("�������ò�������");
		return false;
	}
	m_bDispenseSuccess = true;
	if(!SynchronizeMoveXY(x,y,true))
	{
		return false;
	}

	if(!RunZUp(za,zb,zc))
	{
		return false;
	}

	if (m_tStatus == K_RUN_STS_PAUSE)  //2016-03-11
	{
		if(!KeepStillWhenPause())
		{
			return false;
		}
	}


	if(!IsRunStop())
	{
		return false;
	}
	
	int nCount = V9Parm[0].DispNum;
	for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
	{
		if(nCount>=V9Parm[i].DispNum)
		{
			nCount = V9Parm[i].DispNum;
		}
	}
	if(theApp.m_V9Control.V9_RunCount>=nCount)
	{
		V9DotRest=true;
	}
	if (V9DotRest&&RunDot_RestV9)
	{
		if(!RunValveOperation())
		{
			return false;
		}
	}
	return true;
}

bool CCmdRun::RunSingleDot(double x,double y,double za,double zb,double zc,CList<tgPos,tgPos&> &list,bool RunDot_RestV9)
{
	CString str;
	double dz1,dz2,dz3;
	dz1=dz2=dz3=0;
	m_bDispenseSuccess = false;
	if(list.GetCount()<2)
	{
		AfxMessageBox("�Զ���·������С��2��.");
		return false;
	}
	if (m_tStatus == K_RUN_STS_PAUSE)
	{
		if(!m_pMv->AllListRun())
		{
			Alarm(15);
			AddMsg("�˶��������˶�ʧ��!");
			return false;
		}
		if(!WaitPasueEnd())
		{
			return false;
		}
	}
	if(IsRunStop())
	{
		if(!m_pMv->AllListRun())
		{
			Alarm(15);
			AddMsg("�˶��������˶�ʧ��!");
			return false;
		}
		return false;
	}
	double dTotalLength = 0;
	tgPos tgCurrentPos,tgNextPos,tgPrePos;
	for(int i=0;i<list.GetCount()-1;i++)
	{
		tgCurrentPos = list.GetAt(list.FindIndex(i));
		tgNextPos = list.GetAt(list.FindIndex(i+1));
		dTotalLength += sqrt((tgCurrentPos.x-tgNextPos.x)*(tgCurrentPos.x-tgNextPos.x)+(tgCurrentPos.y-tgNextPos.y)*(tgCurrentPos.y-tgNextPos.y));
	}

	tgCurrentPos = list.GetAt(list.FindIndex(0));
	if(m_bRepair||m_bFirstDisp)
	{
		if(!SynchronizeMoveXY(tgCurrentPos.x+x,tgCurrentPos.y+y,true))
		{
			return false;
		}
		if(!SynchronizeMoveZ(za-ProductParam.dZupHigh,zb-ProductParam.dZupHigh,
			zc-ProductParam.dZupHigh,true,true))
		{
			return false;
		}
	}
	else
	{
		if(!SynchronizeMoveXY(tgCurrentPos.x+x,tgCurrentPos.y+y,false))
		{
			return false;
		}
		if(!SynchronizeMoveZ(za-ProductParam.dZupHigh,zb-ProductParam.dZupHigh,
			zc-ProductParam.dZupHigh,true,true))
		{
			return false;
		}
		if(!WaitStopXY(10000))
		{
			return false;
		}
	}
	if(!SynchronizeMoveZ(za,zb,zc,true,true))
	{
		return false;
	}
	if(m_bFirstDisp)
	{
		CTimeUtil::Delay(long(ProductParam.dFirstDispDelay*1000));
		m_bFirstDisp = false;
	}
	CTimeUtil::Delay(long(ProductParam.dPreTime*1000));
    dz1=m_pMv->GetPos(K_AXIS_ZA);
	dz2=m_pMv->GetPos(K_AXIS_ZB);
	dz3=m_pMv->GetPos(K_AXIS_ZC);
	str.Format("�Զ���ģʽ|z���滮λ�ã�Z1:%0.3f;Z2��%0.3f;Z3��%0.3f.",dz1,dz2,dz3);
	PutLogIntoList(str);
	dz1=m_pMv->GetEncPos(K_AXIS_ZA);
    dz2=m_pMv->GetEncPos(K_AXIS_ZB);
    dz3=m_pMv->GetEncPos(K_AXIS_ZC);
	str.Format("�Զ���ģʽ|z���ʵ��λ�ã�Z1:%0.3f;Z2��%0.3f;Z3��%0.3f.",dz1,dz2,dz3);
	PutLogIntoList(str);
	if(m_tStatus == K_RUN_STS_PAUSE)  //2016-03-11
	{
		if(!KeepStillWhenPause())
		{
			return false;
		}
	}

	if(!ProductParam.DispMode)
	{
		theApp.m_V9Control.V9_RunCount++;
	}
	m_pMv->SetCrdPrm(FALSE,1,2);
	for(int i=1;i<list.GetCount();i++)
	{
		tgPrePos = tgCurrentPos;
		tgCurrentPos = list.GetAt(list.FindIndex(i));
		double dLength = sqrt((tgCurrentPos.x-tgPrePos.x)*(tgCurrentPos.x-tgPrePos.x)+(tgCurrentPos.y-tgPrePos.y)*(tgCurrentPos.y-tgPrePos.y));
		if(!ProductParam.DispMode)
		{
			for(int j=0;j<(int)theApp.m_tSysParam.valveNum;j++)
			{
				if(g_pFrm->m_CmdRun.RunDispFSelect[j])
				{
					m_pMv->AddBufGear(K_AXIS_U+j,-g_pFrm->m_CmdRun.V9Parm[j].DispDistance*dLength/dTotalLength);
				}
			}
		}
		m_pMv->ListAddLine(tgCurrentPos.x+x,tgCurrentPos.y+y,ProductParam.LineVel,ProductParam.LineAcc);
	}
	if(!m_pMv->AllListRun())
	{
		Alarm(15);
		AddMsg("�˶��������˶�ʧ��!");
		return false;
	}
	m_bDispenseSuccess = true;
	if ((ProductParam.dPostDisTime<=0.005)&&!ProductParam.DispMode)
	{   
		ProductParam.dPostDisTime=0.005;
	}
	CTimeUtil::Delay(long(ProductParam.dPostDisTime*1000));
	if(!SetAxisProfile(HIGH_VEL)||!theApp.m_V9Control.V9C_SetProfile())
	{
		Alarm(30);
		AddMsg("�岹�˶����趨�ٶ�ʧ�ܣ�");
		return false;
	}
	
	if(!RunZUp(za,zb,zc))
	{
		return false;
	}

	if(m_tStatus == K_RUN_STS_PAUSE)  //2016-03-11
	{
		if(!KeepStillWhenPause())
		{
			return false;
		}
	}

	if(IsRunStop())//2016-03-13 Del the Not Condition
	{
		return false;
	}

	int nCount = V9Parm[0].DispNum;
	for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
	{
		if(nCount>=V9Parm[i].DispNum)
		{
			nCount = V9Parm[i].DispNum;
		}
	}
	if(theApp.m_V9Control.V9_RunCount>=nCount)
	{
		V9DotRest=true;
	}
	if (V9DotRest&&RunDot_RestV9)
	{
		if(!RunValveOperation())
		{
			return false;
		}
	}
	return true;
}

bool CCmdRun::RunSingleDot(double x,double y,double za,double zb,double zc,double dCircleRadius,bool RunDot_RestV9)
{
    CString str;
	double dx,dy;
	dx=dy=0;
	double dz1,dz2,dz3;
	dz1=dz2=dz3=0;
	m_bDispenseSuccess = false;
	if(m_tStatus == K_RUN_STS_PAUSE)
	{
		if(!WaitPasueEnd())
		{
			str.Format("��Բ��ʼǰ��ͣ����ʧ��0...");
			PutLogIntoList(str);
			return false;
		}
	}
	double totalLength = 0;
	double dTemp = dCircleRadius;
	double dRadius = 0.0;
	totalLength += 2*KGB_PI*dTemp*(ProductParam.dCompAngle/360);
	if(!ProductParam.DispMode)
	{
		theApp.m_V9Control.V9_RunCount++;
	}
	if(m_bRepair||m_bFirstDisp)
	{
		if(!SynchronizeMoveXY(x,y,true))
		{
			str.Format("��ԲʱXY�����ĵ�ʧ��1...");
			PutLogIntoList(str);
			return false;
		}
		if(!SynchronizeMoveZ(za-ProductParam.dZupHigh,zb-ProductParam.dZupHigh,
			zc-ProductParam.dZupHigh,true,true))
		{
			str.Format("��ԲʱZ�������߶�ʧ��2...");
			PutLogIntoList(str);
			return false;
		}
	}
	else
	{
		if(!SynchronizeMoveXY(x,y,false))
		{
			str.Format("��ԲʱXY�����ĵ�ʧ��3...");
			PutLogIntoList(str);
			return false;
		}
		if(!SynchronizeMoveZ(za-ProductParam.dZupHigh,zb-ProductParam.dZupHigh,
			zc-ProductParam.dZupHigh,true,true))
		{
			str.Format("��ԲʱZ�������߶�ʧ��4...");
			PutLogIntoList(str);
			return false;
		}
		if(!WaitStopXY(10000))
		{
			str.Format("��ԲʱXY�ȴ�ֹͣʧ��5...");
			PutLogIntoList(str);
			return false;
		}
	}
	if(!SynchronizeMoveZ(za,zb,zc,true,true))
	{
		str.Format("��ԲʱZ���㽺λʧ��6...");
		PutLogIntoList(str);
		return false;
	}
	dx=m_pMv->GetEncPos(K_AXIS_X);
	dy=m_pMv->GetEncPos(K_AXIS_Y);
	dz1=m_pMv->GetEncPos(K_AXIS_ZA);
	dz2=m_pMv->GetEncPos(K_AXIS_ZB);
	dz3=m_pMv->GetEncPos(K_AXIS_ZC);
	str.Format("��Բģʽ|���ʵ��λ�ã�X:%0.3f;Y:%0.3f;Z1:%0.3f;Z2��%0.3f;Z3��%0.3f.",dx,dy,dz1,dz2,dz3);
	PutLogIntoList(str);

	if(m_tStatus == K_RUN_STS_PAUSE)  //2016-03-11
	{
		if(!WaitPasueEnd())
		{
			str.Format("��Բʱ��ͣ����ʧ��7...");
			PutLogIntoList(str);
			return false;
		}
	}
	m_pMv->SetCrdPrm(FALSE,1,2);
	m_pMv->ListAddLine(x-dCircleRadius,y,ProductParam.CircleVel,ProductParam.CircleAcc);
	if(!ProductParam.DispMode)
	{
		for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
		{
			if(g_pFrm->m_CmdRun.RunDispFSelect[i])
			{
				if((VALVE_M9==theApp.m_tSysParam.ValveType)||(VALVE_L9==theApp.m_tSysParam.ValveType))
				{
					m_pMv->AddBufGear(K_AXIS_U+i,g_pFrm->m_CmdRun.V9Parm[i].DispDistance*(2*KGB_PI*dCircleRadius*(ProductParam.dCompAngle/360.0))/totalLength);
				}
				else if(VALVE_H9==theApp.m_tSysParam.ValveType)
				{
					m_pMv->AddBufGear(K_AXIS_U+i,-g_pFrm->m_CmdRun.V9Parm[i].DispDistance*(2*KGB_PI*dCircleRadius*(ProductParam.dCompAngle/360.0))/totalLength);
				}
			}
		}
	}
	if(ProductParam.dCompAngle>0)
	{
		//���Ƕȣ�˳ʱ��
		m_pMv->ListAddArc(x-dCircleRadius*cos(ProductParam.dCompAngle/180.0*KGB_PI),y+dCircleRadius*sin(ProductParam.dCompAngle/180.0*KGB_PI),ProductParam.dCompAngle<180?dCircleRadius:-dCircleRadius,0,ProductParam.CircleVel,ProductParam.CircleAcc);
	}
	else
	{  //���Ƕȣ���ʱ��
		m_pMv->ListAddArc(x-dCircleRadius*cos(ProductParam.dCompAngle/180.0*KGB_PI),y+dCircleRadius*sin(ProductParam.dCompAngle/180.0*KGB_PI),(fabs(ProductParam.dCompAngle))<180?dCircleRadius:-dCircleRadius,1,ProductParam.CircleVel,ProductParam.CircleAcc);

	}

	if(!m_pMv->AllListRun())
	{
		Alarm(15);
		AddMsg("�˶��������˶�ʧ�ܣ�");
		str.Format("��Բģʽ�˶�������ʧ��8...");
		PutLogIntoList(str);
		return false;
	}
	if((ProductParam.dPostDisTime<=0.005)&&!ProductParam.DispMode)
	{   
		ProductParam.dPostDisTime=0.005;
	}
	CTimeUtil::Delay(long(ProductParam.dPostDisTime*1000));
	if(!SetAxisProfile(HIGH_VEL))
	{
		Alarm(30);
		AddMsg("�岹�˶����趨�ٶ�ʧ�ܣ�");
		str.Format("��Բ��е���л�����ʧ��9...");
		PutLogIntoList(str);
		return false;
	}
	if(!theApp.m_V9Control.V9C_SetProfile())
	{
		Alarm(30);
		AddMsg("�岹�˶����趨�����ٶ�ʧ�ܣ�");
		str.Format("��Բ��Ϸ����ٶ�����ʧ��10...");
		PutLogIntoList(str);
		return false;
	}
	m_bDispenseSuccess = true;
	
	if(!RunZUp(za,zb,zc))
	{
		str.Format("��Բ���Z����ʧ��11...");
		PutLogIntoList(str);
		return false;
	}

	if (m_tStatus == K_RUN_STS_PAUSE)  //2016-03-11
	{
		if(!WaitPasueEnd())
		{
			str.Format("��Բ�����ͣ����ʧ��12...");
			PutLogIntoList(str);
			return false;
		}
	}
	int nCount = V9Parm[0].DispNum;
	for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
	{
		if(nCount>=V9Parm[i].DispNum)
		{
			nCount = V9Parm[i].DispNum;
		}
	}
	if(theApp.m_V9Control.V9_RunCount>=nCount)
	{
		V9DotRest=true;
	}
	if (V9DotRest&&RunDot_RestV9)
	{
		if(!RunValveOperation())
		{
			str.Format("��Բ������ʧ��13...");
			PutLogIntoList(str);
			return false;
		}
	}
	return true;
}

bool CCmdRun::SingleDot(int nDisNumber)
{
	DWORD W_Startime,W_EndTime;
	bool m_temPRestV9 = true;
	W_Startime = W_EndTime = GetTickCount();

	if (0==g_pFrm->m_CmdRun.ProductParam.DispFCount)  //2016-03-13
	{
		return true;
	}
	if(ProductParam.IrregularMatrix)
	{
		if (DisCompCount+1==(ProductParam.IrregularColumn*ProductParam.IrregularRow))
		{
			m_temPRestV9 = false;
		}
	}
	else
	{
		if(ProductParam.FirstDispColumn>1)
		{
			if (DisCompCount+1==(ProductParam.ThirdDispColumn*ProductParam.ThirdDispRow*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.FirstDispRow*(ProductParam.FirstDispColumn+2)/3))
			{
				m_temPRestV9 = false;
			}
		}
		else if(ProductParam.SecondDispColumn>1)
		{
			if (DisCompCount+1==(ProductParam.ThirdDispColumn*ProductParam.ThirdDispRow*ProductParam.SecondDispRow*(ProductParam.SecondDispColumn+2)/3*ProductParam.FirstDispRow*ProductParam.FirstDispColumn))
			{
				m_temPRestV9 = false;
			}
		}
		else
		{
			if (DisCompCount+1==((ProductParam.ThirdDispColumn+2)/3*ProductParam.ThirdDispRow*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.FirstDispRow*ProductParam.FirstDispColumn))
			{
				m_temPRestV9 = false;
			}
		}
	}
	if(ProductParam.fillMode==CUSTOM_DISP_MODE)
	{
		if(!RunSingleDot(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,PDposting[nDisNumber].Dza,
			PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc,g_pFrm->m_rawList,m_temPRestV9))
		{
			return false;
		}
	}
	else if(ProductParam.fillMode==HELIX_DISP_MODE)
	{
		if(!RunSingleDot(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,PDposting[nDisNumber].Dza,
			PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc,ProductParam.dDispRadius,ProductParam.dDistance,m_temPRestV9))
		{
			return false;
		}
	}
	else if(ProductParam.fillMode==RECTANGLE_DISP_MODE)
	{
		if(!RunSingleDot(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,
			PDposting[nDisNumber].Dzc,ProductParam.RectWidth,ProductParam.RectLength,ProductParam.DispFCount,m_temPRestV9))
		{
		}
	}
	else if(ProductParam.fillMode==LINE_VERTICAL_DISP_MODE)
	{
		if(!RunSingleDot(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,
			PDposting[nDisNumber].Dzc,ProductParam.LineLength,true,ProductParam.DispFCount,m_temPRestV9))
		{
			return false;
		}
	}
	else if(ProductParam.fillMode==LINE_HORIZONTAL_DISP_MODE)
	{
		if(!RunSingleDot(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,
			PDposting[nDisNumber].Dzc,ProductParam.LineLength,false,ProductParam.DispFCount,m_temPRestV9))
		{
			return false;
		}
	}
	else if(ProductParam.fillMode==CIRCLE_DISP_MODE)
	{
		if(!RunSingleDot(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,PDposting[nDisNumber].Dza,
			PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc,ProductParam.dDispRadius,m_temPRestV9))
		{
			return false;
		}
	}
	else if(ProductParam.fillMode == MULTI_DISP_MODE)
	{
		if(!RunSingleDot(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,PDposting[nDisNumber].Dza,
			PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc,m_temPRestV9,true))
		{
			return false;
		}
	}
	else
	{
		if(!RunSingleDot(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,PDposting[nDisNumber].Dza,
			PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc,m_temPRestV9,false))
		{
			return false;
		}
	}
	//ͳ��
	//StautDisp.W_Count+=ProductParam.DispFCount;
	DisCompCount++;
	theApp.m_tSysParam.StatCapacity += ProductParam.DispFCount;
	/*W_EndTime=GetTickCount();
	StautDisp.W_SingeTimer=(W_EndTime-W_Startime)/1000.00;
	if ((StautDisp.W_SingeTimer>0.05))
	{
		StautDisp.W_UPH=3600/StautDisp.W_SingeTimer*ProductParam.DispFCount;
	}*/
	return true;
}

// �����㽺�ۺ�ģʽ
bool CCmdRun::SingleDotComplex(double x,double y,double za,double zb,double zc,bool bVertical,bool bV9Reset)
{
	DWORD W_Startime,W_EndTime;
	W_Startime = W_EndTime = GetTickCount();

	if(bVertical)
	{
		if(!RunSingleDotComplex(x,y,za,zb,zc,ProductParam.LineLength,true,ProductParam.DispFCount,bV9Reset))
		{
			return false;
		}
	}
	else
	{
		if(!RunSingleDotComplex(x,y,za,zb,zc,ProductParam.LineLength,false,ProductParam.DispFCount,bV9Reset))
		{
			return false;
		}
	}
	//ͳ��
	//StautDisp.W_Count+=ProductParam.DispFCount;
	DisCompCount++;
	theApp.m_tSysParam.StatCapacity += ProductParam.DispFCount;
	/*W_EndTime=GetTickCount();
	StautDisp.W_SingeTimer=(W_EndTime-W_Startime)/1000.00;
	if ((StautDisp.W_SingeTimer>0.05))
	{
		StautDisp.W_UPH=3600/StautDisp.W_SingeTimer*ProductParam.DispFCount;
	}*/
	return true;
}

// �����㽺
bool CCmdRun::SingleDot(double x,double y,double za,double zb,double zc,bool bV9Reset /* = true */)
{
	DWORD W_Startime,W_EndTime;
	W_Startime = W_EndTime = GetTickCount();
	if(ProductParam.fillMode==CUSTOM_DISP_MODE)
	{
		if(!RunSingleDot(x,y,za,zb,zc,g_pFrm->m_rawList,bV9Reset))
		{
			return false;
		}
	}
	else if(ProductParam.fillMode==HELIX_DISP_MODE)
	{
		if(!RunSingleDot(x,y,za,zb,zc,ProductParam.dDispRadius,ProductParam.dDistance,bV9Reset))
		{
			return false;
		}
	}
	else if(ProductParam.fillMode==RECTANGLE_DISP_MODE)
	{
		if(!RunSingleDot(x,y,za,zb,zc,ProductParam.RectWidth,ProductParam.RectLength,ProductParam.DispFCount,bV9Reset))
		{
			return false;
		}
	}
	else if(ProductParam.fillMode==LINE_VERTICAL_DISP_MODE)
	{
		if(!RunSingleDot(x,y,za,zb,zc,ProductParam.LineLength,true,ProductParam.DispFCount,bV9Reset))
		{
			return false;
		}
	}
	else if(ProductParam.fillMode==LINE_HORIZONTAL_DISP_MODE)
	{
		if(!RunSingleDot(x,y,za,zb,zc,ProductParam.LineLength,false,ProductParam.DispFCount,bV9Reset))
		{
			return false;
		}
	}
	else if(ProductParam.fillMode==CIRCLE_DISP_MODE)
	{
		if(!RunSingleDot(x,y,za,zb,zc,ProductParam.dDispRadius,bV9Reset))
		{
			return false;
		}
	}
	else if(ProductParam.fillMode==MULTI_DISP_MODE)
	{
		if(!RunSingleDot(x,y,za,zb,zc,bV9Reset,true))
		{
			return false;
		}
	}
	else
	{
		if(!RunSingleDot(x,y,za,zb,zc,bV9Reset,false))
		{
			return false;
		}
	}
	//ͳ��
	//StautDisp.W_Count+=ProductParam.DispFCount;
	DisCompCount++;
	theApp.m_tSysParam.StatCapacity += ProductParam.DispFCount;
	/*W_EndTime=GetTickCount();
	StautDisp.W_SingeTimer=(W_EndTime-W_Startime)/1000.00;
	if ((StautDisp.W_SingeTimer>0.05))
	{
		StautDisp.W_UPH=3600/StautDisp.W_SingeTimer*ProductParam.DispFCount;
	}*/
	return true;
}

// �ȴ���ͣ����
bool CCmdRun::WaitPasueEnd(void)
{
	static bool Run_Pasue=false;
	m_bFirstDisp = true;
	if (Run_Pasue)
	{
		return true;
	}
	Run_Pasue=true;
	AddMsg("�㽺��ͣ�У�");
	if(!MoveToZSafety())
	{
		return false;
	}
	while (m_tStatus == K_RUN_STS_PAUSE)
	{
		CFunction::DoEvents();
		Sleep(50);
		if (IsRunStop())
		{
			Run_Pasue=false;
			return false;
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
	if (!V9RestOK)
	{
		if(!RestDisP())
		{
			m_tStatus = K_RUN_STS_STOP;
			Run_Pasue = false;
			m_bFirstDisp = false;
			return false;
		}
	}
	if(ProductParam.PauseCleanTest)
	{
		if(!CleanAndTest(FALSE))
		{
			return false;
		}
		if(!SetAxisProfile(HIGH_VEL))
		{
			Alarm(30);
			AddMsg("��ϴ���Խ����趨�ٶ�ʧ�ܣ�");
			return false;
		}
	}
	Run_Pasue=false;
	return true;
}

// �㽺�ֽ⶯���н�����ͣ�ź�ʱ����ԭ״
bool CCmdRun::KeepStillWhenPause(void)
{
	static bool Run_Pasue=false;
	//m_bFirstDisp = true; ���ı��׵��־
	if (Run_Pasue)
	{
		return true;
	}
	Run_Pasue=true;
	AddMsg("�㽺�ֽ⶯����ͣ�У�");
	
	//if(!MoveToZSafety()) //Z�ᱣ��ԭ��λ��
	//{
	//	return false;
	//}

	while (m_tStatus == K_RUN_STS_PAUSE)
	{
		CFunction::DoEvents();
		Sleep(50);
		if (IsRunStop())
		{
			MoveToZSafety();//��ͣ�����а���ֹͣ��Z�ذ�ȫλ��
			Run_Pasue=false;
			return false;
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
	//if (!V9RestOK)
	//{
	//	if(!RestDisP())
	//	{
	//		m_tStatus = K_RUN_STS_STOP;
	//		Run_Pasue = false;
	//		m_bFirstDisp = false;
	//		return false;
	//	}
	//}
	//if(ProductParam.PauseCleanTest)
	//{
	//	if(!CleanAndTest(FALSE))
	//	{
	//		return false;
	//	}
	//	if(!SetAxisProfile(HIGH_VEL))
	//	{
	//		Alarm(30);
	//		AddMsg("��ϴ���Խ����趨�ٶ�ʧ�ܣ�");
	//		return false;
	//	}
	//}
	Run_Pasue=false;
	return true;
}

// �����
bool CCmdRun::CleanNeedle(bool Z_ON)
{
	CString strError;
	if(m_bCleanNeedle||m_bDischarge||m_bTestDispense||m_bSearchMark||m_bSlotDetect||m_bHoming||m_bMeasureHeight||m_bNeedleCalibrate||m_bNeedleAutoAdjust)
	{
		strError.Format("δ֪����");
		if(m_bCleanNeedle)
		{
			strError.Format("��ϴ��ͷʱ��������ϴ��ͷ��");
		}
		else if(m_bDischarge)
		{
			strError.Format("�Ž�ʱ��������ϴ��ͷ��");
		}
		else if(m_bTestDispense)
		{
			strError.Format("�Խ�ʱ��������ϴ��ͷ��");
		}
		else if(m_bSearchMark)
		{
			strError.Format("����Mark��ʱ��������ϴ��ͷ��");
		}
		else if(m_bSlotDetect)
		{
			strError.Format("�������ʱ��������ϴ��ͷ��");
		}
		else if(m_bHoming)
		{
			strError.Format("��λʱ��������ϴ��ͷ��");
		}
		else if(m_bMeasureHeight)
		{
			strError.Format("���ʱ��������ϴ��ͷ��");
		}
		else if (m_bNeedleCalibrate)
		{
			strError.Format("��ͷ�궨ʱ��������ϴ��ͷ");
		}
		else if (m_bNeedleAutoAdjust)
		{
			strError.Format("�Զ�����ʱ��������ϴ��ͷ");
		}
		PutLogIntoList(strError);
		return false;
	}
	strError.Format("��ʼ��ϴ��ͷ");
	PutLogIntoList(strError);
	m_bCleanNeedle = true;
	if(!MoveToZSafety())
	{
		m_bCleanNeedle = false;
		strError.Format("��ϴ��ͷʧ��");
		PutLogIntoList(strError);
		return false;
	}
	if(IsExisAlarm())
	{
		m_bCleanNeedle = false;
		strError.Format("��ϴ��ͷʧ��");
		PutLogIntoList(strError);
		return false;
	}
	if(!SynchronizeMoveXY(theApp.m_tSysParam.CleanPosition[0],theApp.m_tSysParam.CleanPosition[1],true))
	{
		m_bCleanNeedle = false;
		strError.Format("��ϴ��ͷʧ��");
		PutLogIntoList(strError);
		return false;
	}
	if(IsExisAlarm())
	{
		m_bCleanNeedle = false;
		strError.Format("��ϴ��ͷʧ��");
		PutLogIntoList(strError);
		return false;
	}
	if(Z_ON)
	{
		if(!AdjustMotorToCleanPos())   
		{
			m_bCleanNeedle = false;
			strError.Format("��ϴ��ͷʧ��");
			PutLogIntoList(strError);
			return false;
		}

		if(!SynchronizeMoveZ(theApp.m_tSysParam.CleanPosition[2],theApp.m_tSysParam.CleanPosition[3],
			theApp.m_tSysParam.CleanPosition[4],false,true))
		{
			m_bCleanNeedle = false;
			strError.Format("��ϴ��ͷʧ��");
			PutLogIntoList(strError);
			return false;
		}
		m_pMv->SetOutput(theApp.m_tSysParam.outNeedleClean,TRUE);
		if(ProductParam.CleanDischarge)
		{
			theApp.m_V9Control.V9C_Test(K_ALL_AXIS);
		}
		CFunction::Delay(ProductParam.dCleanDelay);
		if(!MoveToZSafety())
		{
			m_bCleanNeedle = false;
			strError.Format("��ϴ��ͷʧ��");
			PutLogIntoList(strError);
			return false;
		}
		m_pMv->SetOutput(theApp.m_tSysParam.outNeedleClean,FALSE);
		//if(!AdjustNeedle())
		if(!AdjustNeedle())
		{
			m_bCleanNeedle = false;
			strError.Format("��ϴ��ϵ�����ͷʧ��");
			PutLogIntoList(strError);
			return false;
		}
	}
	m_bCleanNeedle = false;
	strError.Format("��ϴ��ͷ�ɹ�");
	PutLogIntoList(strError);
	return true;
}
// ��ȡͼλ
bool CCmdRun::StartGrabImagePos()
{
	if(!MoveToZSafety())
	{
		return false;
	}
	if(IsExisAlarm())
	{
		return false;
	}
	tgPos tgStartPos = GetFirstCameraPos();
	if(!SynchronizeMoveXY(tgStartPos.x,tgStartPos.y,true))
	{
		return false;
	}
	if(IsExisAlarm())
	{
		return false;
	}
	if(!SynchronizeMoveZ(tgStartPos.za,true))
	{
		return false;
	}
	return true;
}

bool CCmdRun::EndGrabImagePos()
{
	if(!MoveToZSafety())
	{
		return false;
	}
	if(IsExisAlarm())
	{
		return false;
	}
	tgPos tgEndPos = GetLastCameraPos();
	if(!SynchronizeMoveXY(tgEndPos.x,tgEndPos.y,true))
	{
		return false;
	}
	if(IsExisAlarm())
	{
		return false;
	}
	if(!SynchronizeMoveZ(tgEndPos.za,true))
	{
		return false;
	}
	return true;
}

bool CCmdRun::GrabImagePos(int nIndex,bool bIsSafety)
{
	CString strError;
	if(m_bCleanNeedle||m_bDischarge||m_bTestDispense||m_bSearchMark||m_bSlotDetect||m_bHoming||m_bMeasureHeight||m_bNeedleCalibrate||m_bNeedleAutoAdjust)
	{
		strError.Format("δ֪����");
		if(m_bCleanNeedle)
		{
			strError.Format("��ϴ��ͷʱ�����ܵ�ȡͼλ��");
		}
		else if(m_bDischarge)
		{
			strError.Format("�Ž�ʱ�����ܵ�ȡͼλ��");
		}
		else if(m_bTestDispense)
		{
			strError.Format("�Խ�ʱ�����ܵ�ȡͼλ��");
		}
		else if(m_bSearchMark)
		{
			strError.Format("����Mark��ʱ�����ܵ�ȡͼλ��");
		}
		else if(m_bSlotDetect)
		{
			strError.Format("�������ʱ�����ܵ�ȡͼλ��");
		}
		else if(m_bHoming)
		{
			strError.Format("��λʱ�����ܵ�ȡͼλ��");
		}
		else if(m_bMeasureHeight)
		{
			strError.Format("���ʱ�����ܵ�ȡͼλ��");
		}
		else if (m_bNeedleCalibrate)
		{
            strError.Format("��ͷ�궨ʱ�����ܵ�ȡͼλ��");
		}
		else if (m_bNeedleAutoAdjust)
		{
			strError.Format("�Զ�����ʱ�����ܵ�ȡͼλ��");
		}
		PutLogIntoList(strError);
		return false;
	}
	m_bSearchMark = true;
	if(nIndex<0||nIndex>=ProductParam.nImageNum)
	{
		m_bSearchMark = false;
		return false;
	}
	if(!bIsSafety)
	{
		if(!MoveToZSafety())
		{
			m_bSearchMark = false;
			return false;
		}
		if(IsExisAlarm())
		{
			m_bSearchMark = false;
			return false;
		}
	}
	if(!SynchronizeMoveXY(ProductParam.tgImagePos[nIndex].x,ProductParam.tgImagePos[nIndex].y,true))
	{
		m_bSearchMark = false;
		return false;
	}
	if(IsExisAlarm())
	{
		m_bSearchMark = false;
		return false;
	}
	if(!SynchronizeMoveZ(ProductParam.tgImagePos[nIndex].za,true))
	{
		m_bSearchMark = false;
		return false;
	}
	m_bSearchMark = false;
	return true;
}

bool CCmdRun::TestDotPos(bool Z_ON)
{
	if(!MoveToZSafety())
	{
		return false;
	}
	if(IsExisAlarm())
	{
		return false;
	}
	if(!SynchronizeMoveXY(theApp.m_tSysParam.TestDotPostion[0],theApp.m_tSysParam.TestDotPostion[1],true))
	{
		return false;
	}
	if(IsExisAlarm())
	{
		return false;
	}
	if(Z_ON)
	{
		if(!SynchronizeMoveZ(theApp.m_tSysParam.TestDotPostion[2],true))
		{
			return false;
		}
		for(int i=0;i<ProductParam.TestDotNumber;i++)
		{
			theApp.m_V9Control.V9C_Test(K_ALL_AXIS);
			CFunction::Delay(ProductParam.dTestDotDelay);
		}
		if(!MoveToZSafety())
		{
			return false;
		}
	}
	return true;
}

bool CCmdRun::TestDispense(int nIndex,bool bDispense)
{
	if(m_bCleanNeedle||m_bDischarge||m_bTestDispense||m_bSearchMark||m_bSlotDetect||m_bHoming||m_bMeasureHeight||m_bNeedleCalibrate||m_bNeedleAutoAdjust)
	{
		CString strError;
		strError.Format("δ֪����");
		if(m_bCleanNeedle)
		{
			strError.Format("��ϴ��ͷʱ�������Խ���");
		}
		else if(m_bDischarge)
		{
			strError.Format("�Ž�ʱ�������Խ���");
		}
		else if(m_bTestDispense)
		{
			strError.Format("�Խ�ʱ�������Խ���");
		}
		else if(m_bSearchMark)
		{
			strError.Format("����Mark��ʱ�������Խ���");
		}
		else if(m_bSlotDetect)
		{
			strError.Format("�������ʱ�������Խ���");
		}
		else if(m_bHoming)
		{
			strError.Format("��λʱ�������Խ���");
		}
		else if(m_bMeasureHeight)
		{
			strError.Format("���ʱ�������Խ���");
		}
		else if (m_bNeedleCalibrate)
		{
			strError.Format("��ͷ�궨ʱ�������Խ���");
		}
		else if (m_bNeedleAutoAdjust)
		{
			strError.Format("�Զ�����ʱ�������Խ���");
		}
		PutLogIntoList(strError);
		return false;
	}
	m_bTestDispense = true;
	if(!MoveToZSafety())
	{
		m_bTestDispense = false;
		return false;
	}
	if(IsExisAlarm())
	{
		m_bTestDispense = false;
		return false;
	}
	if(!SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[nIndex].x,g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[nIndex].y,true))
	{
		m_bTestDispense = false;
		return false;
	}
	if(IsExisAlarm())
	{
		m_bTestDispense = false;
		return false;
	}
	if(bDispense)
	{
		if(!SynchronizeMoveZ(g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[nIndex].za,g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[nIndex].zb,
			g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[nIndex].zc,false,true))
		{
			m_bTestDispense = false;
			return false;
		}
		CFunction::Delay(ProductParam.dTestDotDelay*0.5);
		theApp.m_V9Control.V9C_Test(K_ALL_AXIS);
		CFunction::Delay(ProductParam.dTestDotDelay*0.5);
		if(!MoveToZSafety())
		{
			m_bTestDispense = false;
			return false;
		}
	}
	else
	{
		if(AfxMessageBox("ȷ���ƶ����Խ�λ��",MB_YESNO)==IDYES)
		{
			if(!SynchronizeMoveZ(g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[nIndex].za,g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[nIndex].zb,
				g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[nIndex].zc,false,true))
			{
				m_bTestDispense = false;
				return false;
			}
		}
	}
	m_bTestDispense = false;
	return true;
}

bool CCmdRun::TestDispense()
{
	CString strError;
	if(m_bCleanNeedle||m_bDischarge||m_bTestDispense||m_bSearchMark||m_bSlotDetect||m_bHoming||m_bMeasureHeight||m_bNeedleCalibrate||m_bNeedleAutoAdjust)
	{
		strError.Format("δ֪����");
		if(m_bCleanNeedle)
		{
			strError.Format("��ϴ��ͷʱ�������Խ���");
		}
		else if(m_bDischarge)
		{
			strError.Format("�Ž�ʱ�������Խ���");
		}
		else if(m_bTestDispense)
		{
			strError.Format("�Խ�ʱ�������Խ���");
		}
		else if(m_bSearchMark)
		{
			strError.Format("����Mark��ʱ�������Խ���");
		}
		else if(m_bSlotDetect)
		{
			strError.Format("�������ʱ�������Խ���");
		}
		else if(m_bHoming)
		{
			strError.Format("��λʱ�������Խ���");
		}
		else if(m_bMeasureHeight)
		{
			strError.Format("���ʱ�������Խ���");
		}
		else if (m_bNeedleCalibrate)
		{
			strError.Format("��ͷ�궨ʱ�������Խ���");
		}
		else if (m_bNeedleAutoAdjust)
		{
			strError.Format("�Զ�����ʱ�������Խ���");
		}
		PutLogIntoList(strError);
		return false;
	}
	strError.Format("��ʼ�Խ�");
	PutLogIntoList(strError);
	m_bTestDispense = true;
	if(ProductParam.TestDotNumber<=0||ProductParam.TestDotNumber>10)
	{
		m_bTestDispense = false;
		strError.Format("�Խ�ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	if(!MoveToZSafety())
	{
		m_bTestDispense = false;
		strError.Format("�Խ�ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	if(IsExisAlarm())
	{
		m_bTestDispense = false;
		strError.Format("�Խ�ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	for(int i=0;i<ProductParam.TestDotNumber;i++)
	{
		if(!SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].x,g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].y,true))
		{
			m_bTestDispense = false;
			strError.Format("�Խ�ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		if(IsExisAlarm())
		{
			m_bTestDispense = false;
			strError.Format("�Խ�ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		//Z��
		if(!SynchronizeMoveZ(g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].za,g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].zb,
			g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].zc,false,true))
		{
			m_bTestDispense = false;
			strError.Format("�Խ�ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		if(IsExisAlarm())
		{
			m_bTestDispense = false;
			strError.Format("�Խ�ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		CFunction::Delay(ProductParam.dTestDotDelay*0.5);
		theApp.m_V9Control.V9C_Test(K_ALL_AXIS);
		CFunction::Delay(ProductParam.dTestDotDelay*0.5);
		/*if(!SynchronizeMoveZ(g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].za-ProductParam.dZupHigh,g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].zb-ProductParam.dZupHigh,
		g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].zb-ProductParam.dZupHigh,false,true))*/
		//Z��
		if(!SynchronizeMoveZ(g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].za-ProductParam.dZupHigh,g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].zb-ProductParam.dZupHigh,
			g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].zb-ProductParam.dZupHigh,false,true))
		{
			m_bTestDispense = false;
			strError.Format("�Խ�ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		if(IsExisAlarm())
		{
			m_bTestDispense = false;
			strError.Format("�Խ�ʧ��");
			PutLogIntoList(strError);
			return false;
		}
	}
	if(!MoveToZSafety())
	{
		m_bTestDispense = false;
		strError.Format("�Խ�ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	if(IsExisAlarm())
	{
		m_bTestDispense = false;
		strError.Format("�Խ�ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	m_bTestDispense = false;
	strError.Format("�Խ��ɹ�");
	PutLogIntoList(strError);
	return true;
}

bool CCmdRun::DischargePosition(bool Z_ON,bool bDefoam)
{
	CString strError;
	if(m_bCleanNeedle||m_bDischarge||m_bTestDispense||m_bSearchMark||m_bSlotDetect||m_bHoming||m_bMeasureHeight||m_bNeedleCalibrate||m_bNeedleAutoAdjust)
	{
		strError.Format("δ֪����");
		if(m_bCleanNeedle)
		{
			strError.Format("��ϴ��ͷʱ�������Ž���");
		}
		else if(m_bDischarge)
		{
			strError.Format("�Ž�ʱ�������Ž���");
		}
		else if(m_bTestDispense)
		{
			strError.Format("�Խ�ʱ�������Ž���");
		}
		else if(m_bSearchMark)
		{
			strError.Format("����Mark��ʱ�������Ž���");
		}
		else if(m_bSlotDetect)
		{
			strError.Format("�������ʱ�������Ž���");
		}
		else if(m_bHoming)
		{
			strError.Format("��λʱ�������Ž���");
		}
		else if(m_bMeasureHeight)
		{
			strError.Format("���ʱ�������Ž���");
		}
		else if (m_bNeedleCalibrate)
		{
			strError.Format("��ͷ�궨ʱ�������Ž���");
		}
		else if (m_bNeedleAutoAdjust)
		{
			strError.Format("�Զ�����ʱ�������Ž���");
		}
		PutLogIntoList(strError);
		return false;
	}
	strError.Format("��ʼ�Ž�");
	PutLogIntoList(strError);
	m_bDischarge = true;
	if(!MoveToZSafety())
	{
		m_bDischarge = false;
		strError.Format("�Ž�ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	if(IsExisAlarm())
	{
		m_bDischarge = false;
		strError.Format("�Ž�ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	if(!SynchronizeMoveXY(theApp.m_tSysParam.DischargeGluePostion[0],theApp.m_tSysParam.DischargeGluePostion[1],true))
	{
		m_bDischarge = false;
		strError.Format("�Ž�ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	if(IsExisAlarm())
	{
		m_bDischarge = false;
		strError.Format("�Ž�ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	if(Z_ON)
	{
		if(bDefoam)
		{
			if(!SynchronizeMoveZ(theApp.m_tSysParam.DischargeGluePostion[2],theApp.m_tSysParam.DischargeGluePostion[3],theApp.m_tSysParam.DischargeGluePostion[4],false,true))
			{
				m_bDischarge = false;
				strError.Format("�Ž�ʧ��");
				PutLogIntoList(strError);
				return false;
			}
			if(VALVE_H9 == theApp.m_tSysParam.ValveType)
			{
				theApp.m_V9Control.V9C_Defoam(K_ALL_AXIS);
				theApp.m_V9Control.V9C_SetProfile();
			}
			else if((VALVE_L9 == theApp.m_tSysParam.ValveType)||(VALVE_M9 == theApp.m_tSysParam.ValveType))
			{
				int nReturnValue = theApp.m_V9Control.V9C_Home(K_ALL_AXIS);
				theApp.m_V9Control.V9C_SetProfile();
				if(nReturnValue<0)
				{
					Alarm(-nReturnValue+9);
				}
				else if(nReturnValue>0)
				{
					Alarm(nReturnValue+9);
				}
				for (int i=0;i<3;i++)
				{
					theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[i],TRUE);
				}
			}
		}
		else
		{
			if(!SynchronizeMoveZ(theApp.m_tSysParam.DischargeGluePostion[2],theApp.m_tSysParam.DischargeGluePostion[3],theApp.m_tSysParam.DischargeGluePostion[4],false,true))
			{
				m_bDischarge = false;
				strError.Format("�Ž�ʧ��");
				PutLogIntoList(strError);
				return false;
			}
			theApp.m_V9Control.V9C_Clearn(K_ALL_AXIS,g_pFrm->m_CmdRun.V9Parm[0].CleanCount,g_pFrm->m_CmdRun.V9Parm[0].CleanSpeed);
			theApp.m_V9Control.V9C_SetProfile();
			if(!MoveToZSafety())
			{
				m_bDischarge = false;
				strError.Format("�Ž�ʧ��");
				PutLogIntoList(strError);
				return false;
			}
		}
	}
	m_bDischarge = false;
	strError.Format("�Ž��ɹ�");
	PutLogIntoList(strError);
	return true;
}

bool CCmdRun::DischargePositionEX()
{
	CString strError;
	if(m_bCleanNeedle||m_bDischarge||m_bTestDispense||m_bSearchMark||m_bSlotDetect||m_bHoming||m_bMeasureHeight||m_bNeedleCalibrate||m_bNeedleAutoAdjust)
	{
		strError.Format("δ֪����");
		if(m_bCleanNeedle)
		{
			strError.Format("��ϴ��ͷʱ�������Ž���");
		}
		else if(m_bDischarge)
		{
			strError.Format("�Ž�ʱ�������Ž���");
		}
		else if(m_bTestDispense)
		{
			strError.Format("�Խ�ʱ�������Ž���");
		}
		else if(m_bSearchMark)
		{
			strError.Format("����Mark��ʱ�������Ž���");
		}
		else if(m_bSlotDetect)
		{
			strError.Format("�������ʱ�������Ž���");
		}
		else if(m_bHoming)
		{
			strError.Format("��λʱ�������Ž���");
		}
		else if(m_bMeasureHeight)
		{
			strError.Format("���ʱ�������Ž���");
		}
		else if (m_bNeedleCalibrate)
		{
			strError.Format("��ͷ�궨ʱ�������Ž���");
		}
		else if (m_bNeedleAutoAdjust)
		{
			strError.Format("�Զ�����ʱ�������Ž���");
		}
		PutLogIntoList(strError);
		return false;
	}
	strError.Format("��ʼ�Ž�");
	PutLogIntoList(strError);
	m_bDischarge = true;
	if(!MoveToZSafety())
	{
		m_bDischarge = false;
		strError.Format("�Ž�ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	if(IsExisAlarm())
	{
		m_bDischarge = false;
		strError.Format("�Ž�ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	if(!SynchronizeMoveXY(theApp.m_tSysParam.DischargeGluePostion[0],theApp.m_tSysParam.DischargeGluePostion[1],true))
	{
		m_bDischarge = false;
		strError.Format("�Ž�ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	if(IsExisAlarm())
	{
		m_bDischarge = false;
		strError.Format("�Ž�ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	if(!SynchronizeMoveZ(theApp.m_tSysParam.DischargeGluePostion[2],theApp.m_tSysParam.DischargeGluePostion[3],theApp.m_tSysParam.DischargeGluePostion[4],false,true))
	{
		m_bDischarge = false;
		strError.Format("�Ž�ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	m_bDischarge = false;
	strError.Format("���Ž�λ�ɹ�");
	PutLogIntoList(strError);
	return true;
}

bool CCmdRun::SlotDetectPosition()
{
	if(!MoveToZSafety())
	{
		return false;
	}
	if(IsExisAlarm())
	{
		return false;
	}
	//m_pMv->MoveXY(tgHeightPos.x, tgHeightPos.y, ProductParam.FreeVal, ProductParam.FreeAcc, true);
	if(!SynchronizeMoveXY(ProductParam.tSlotDetectPos.x,ProductParam.tSlotDetectPos.y,true))
	{
		return false;
	}
	if(IsExisAlarm())
	{
		return false;
	}
	if(!SynchronizeMoveZ(ProductParam.tSlotDetectPos.za,true)) //0630 add Z motion;
	{
		return false;
	}

	return true;
}

bool CCmdRun::MoveToHeightPosition(int nIndex)
{
	CString strError;
	if(m_bCleanNeedle||m_bDischarge||m_bTestDispense||m_bSearchMark||m_bSlotDetect||m_bHoming||m_bMeasureHeight||m_bNeedleCalibrate||m_bNeedleAutoAdjust)
	{
		strError.Format("δ֪����");
		if(m_bCleanNeedle)
		{
			strError.Format("��ϴ��ͷʱ�����ܵ����λ��");
		}
		else if(m_bDischarge)
		{
			strError.Format("�Ž�ʱ�����ܵ����λ��");
		}
		else if(m_bTestDispense)
		{
			strError.Format("�Խ�ʱ�����ܵ����λ��");
		}
		else if(m_bSearchMark)
		{
			strError.Format("����Mark��ʱ�����ܵ����λ��");
		}
		else if(m_bSlotDetect)
		{
			strError.Format("�������ʱ�����ܵ����λ��");
		}
		else if(m_bHoming)
		{
			strError.Format("��λʱ�����ܵ����λ��");
		}
		else if(m_bMeasureHeight)
		{
			strError.Format("���ʱ�����ܵ����λ��");
		}
		else if (m_bNeedleCalibrate)
		{
			strError.Format("��ͷ�궨ʱ�����ܵ����λ��");
		}
		else if (m_bNeedleAutoAdjust)
		{
			strError.Format("�Զ�����ʱ�����ܵ����λ��");
		}
		PutLogIntoList(strError);
		return false;
	}
	m_bMeasureHeight = true;
	if(!MoveToZSafety())
	{
		m_bMeasureHeight = false;
		return false;
	}
	if(IsExisAlarm())
	{
		m_bMeasureHeight = false;
		return false;
	}
	//m_pMv->MoveXY(tgHeightPos.x, tgHeightPos.y, ProductParam.FreeVal, ProductParam.FreeAcc, true);
	if(!SynchronizeMoveXY(ProductParam.tgMeasureHeightPos[nIndex].x,ProductParam.tgMeasureHeightPos[nIndex].y,true))
	{
		m_bMeasureHeight = false;
		return false;
	}
	if(IsExisAlarm())
	{
		m_bMeasureHeight = false;
		return false;
	}
	if(!SynchronizeMoveZ(ProductParam.tgMeasureHeightPos[nIndex].za,true))
	{
		m_bMeasureHeight = false;
		return false;
	}
	m_bMeasureHeight = false;
	return true;
}

tgPos CCmdRun::PosAfterMarkComp(tgPos posIn)
{
	tgPos tgTempPos;
	CString strTemp;
	double dStartPos[2],dEndPos[2];
	double dCenterX,dCenterY;
	double dAlignCenterX,dAlignCenterY;

	dStartPos[0]=ProductParam.DispenseStartPos[0];
	dStartPos[1]=ProductParam.DispenseStartPos[1];
	tgTempPos =GetPadDispensePos(ProductParam.FirstDispRow-1,ProductParam.FirstDispColumn-1,ProductParam.SecondDispRow-1,
		ProductParam.SecondDispColumn-1,ProductParam.ThirdDispRow-1,ProductParam.ThirdDispColumn-1);
	dEndPos[0] = tgTempPos.x;
	dEndPos[1] = tgTempPos.y;

	dCenterX = (dStartPos[0]+dEndPos[0])/2;// X in Theory;
	dCenterY = (dStartPos[1]+dEndPos[1])/2;//Y in Theory;
	dAlignCenterX = dCenterX - m_tOffset.x; // X Actually
	dAlignCenterY = dCenterY - m_tOffset.y;// Y Actual

	if(ProductParam.visionMode==VISION_TWO_MARK)
	{
		double dX = posIn.x - dCenterX;
		double dY = posIn.y - dCenterY;
		Rotation(dX,dY,m_dRotateAngle,dX,dY);
		posIn.x = dAlignCenterX + dX;
		posIn.y = dAlignCenterY + dY;
	}
	else 
	{
		posIn.x -= m_tOffset.x;
		posIn.y -= m_tOffset.y;
	}
	return posIn;
}
// ���
HEIGHT_STATUS CCmdRun::MeasureHeight(double& dOffsetZ)
{
	CString strError;
	if(m_bCleanNeedle||m_bDischarge||m_bTestDispense||m_bSearchMark||m_bSlotDetect||m_bHoming||m_bMeasureHeight||m_bNeedleCalibrate||m_bNeedleAutoAdjust)
	{
		strError.Format("δ֪����");
		if(m_bCleanNeedle)
		{
			strError.Format("��ϴ��ͷʱ�����ܵ����λ��");
		}
		else if(m_bDischarge)
		{
			strError.Format("�Ž�ʱ�����ܵ����λ��");
		}
		else if(m_bTestDispense)
		{
			strError.Format("�Խ�ʱ�����ܵ����λ��");
		}
		else if(m_bSearchMark)
		{
			strError.Format("����Mark��ʱ�����ܵ����λ��");
		}
		else if(m_bSlotDetect)
		{
			strError.Format("�������ʱ�����ܵ����λ��");
		}
		else if(m_bHoming)
		{
			strError.Format("��λʱ�����ܵ����λ��");
		}
		else if(m_bMeasureHeight)
		{
			strError.Format("���ʱ�����ܵ����λ��");
		}
		else if (m_bNeedleCalibrate)
		{
			strError.Format("��ͷ�궨ʱ�����ܵ����λ��");
		}
		else if (m_bNeedleAutoAdjust)
		{
			strError.Format("�Զ�����ʱ�����ܵ����λ��");
		}
		PutLogIntoList(strError);
		return HEIGHT_STATUS_UNKNOWN_ERROR;
	}
	m_bMeasureHeight = true;
	strError.Format("��ʼ���");
	PutLogIntoList(strError);
	CString str;
	CString strText="";
	CString strTest[2];
	tgPos tgMeasurePos;
	double dTestHeight[2];
	double dRealTimeData[10]; //��¼ʵʱ����
	for(int i=0;i<10;i++)
	{
		dRealTimeData[i]=0;
	}

	double dTempHeight = 0.0;
	double dTotalHeight = 0.0;
	double dMax = 0;
	double dMin = 0;
	tgMeasurePos.x = ProductParam.tgMeasureHeightPos[0].x;
	tgMeasurePos.y = ProductParam.tgMeasureHeightPos[0].y;
	tgMeasurePos.za = ProductParam.tgMeasureHeightPos[0].za;
	//tgMeasurePos=PosAfterMarkComp(tgMeasurePos);//20180421 �������λ...
	if(m_tStatus == K_RUN_STS_PAUSE)
	{
		if(!WaitPasueEnd())
		{
			m_bMeasureHeight = false;
			strError.Format("���ʧ��");
			PutLogIntoList(strError);
			return HEIGHT_STATUS_UNKNOWN_ERROR;
		}
	}
	if(IsRunStop())
	{
		m_bMeasureHeight = false;
		strError.Format("���ʧ��");
		PutLogIntoList(strError);
		return HEIGHT_STATUS_UNKNOWN_ERROR;
	}
	if(!SynchronizeMoveXY(tgMeasurePos.x,tgMeasurePos.y,true))
	{
		m_bMeasureHeight = false;
		strError.Format("���ʧ��");
		PutLogIntoList(strError);
		return HEIGHT_STATUS_MOTION_FAILURE;
	}
	if(m_tStatus == K_RUN_STS_PAUSE)
	{
		if(!WaitPasueEnd())
		{
			m_bMeasureHeight = false;
			strError.Format("���ʧ��");
			PutLogIntoList(strError);
			return HEIGHT_STATUS_UNKNOWN_ERROR;
		}
	}
	if(IsRunStop())
	{
		m_bMeasureHeight = false;
		strError.Format("���ʧ��");
		PutLogIntoList(strError);
		return HEIGHT_STATUS_UNKNOWN_ERROR;
	}
	if(!SynchronizeMoveZ(tgMeasurePos.za,true))
	{
		m_bMeasureHeight = false;
		strError.Format("���ʧ��");
		PutLogIntoList(strError);
		return HEIGHT_STATUS_MOTION_FAILURE;
	}
	Sleep(DWORD(ProductParam.dHeightDelay*1000));
	ReadHeightSensorValue(&dTestHeight[0]);
	ReadHeightSensorValue(&dTestHeight[1]);
	ReadHeightSensorValue(&dTempHeight);
	dRealTimeData[0]=dTempHeight;

	dTotalHeight += dTempHeight;
	dMax = dMin = dTempHeight;
	str.Format("Test1:%0.3f",dTestHeight[0]);
	str.Format("Test2:%0.3f",dTestHeight[1]);
	str.Format("Height:%0.3f",dTempHeight);
	strTest[0] += str;
	strTest[1] += str;
	strText += str;
	for(int i=1;i<ProductParam.HeightNumber;i++)
	{
		tgMeasurePos.x = ProductParam.tgMeasureHeightPos[i].x;
		tgMeasurePos.y = ProductParam.tgMeasureHeightPos[i].y;
		tgMeasurePos.za = ProductParam.tgMeasureHeightPos[i].za;
		//tgMeasurePos=PosAfterMarkComp(tgMeasurePos);//20180421 �������λ...
		if(m_tStatus == K_RUN_STS_PAUSE)
		{
			if(!WaitPasueEnd())
			{
				m_bMeasureHeight = false;
				strError.Format("���ʧ��");
				PutLogIntoList(strError);
				return HEIGHT_STATUS_UNKNOWN_ERROR;
			}
		}
		if(IsRunStop())
		{
			m_bMeasureHeight = false;
			strError.Format("���ʧ��");
			PutLogIntoList(strError);
			return HEIGHT_STATUS_UNKNOWN_ERROR;
		}
		if(!SynchronizeMoveXY(tgMeasurePos.x,tgMeasurePos.y,true))
		{
			m_bMeasureHeight = false;
			strError.Format("���ʧ��");
			PutLogIntoList(strError);
			return HEIGHT_STATUS_MOTION_FAILURE;
		}
		if(m_tStatus == K_RUN_STS_PAUSE)
		{
			if(!WaitPasueEnd())
			{
				m_bMeasureHeight = false;
				strError.Format("���ʧ��");
				PutLogIntoList(strError);
				return HEIGHT_STATUS_UNKNOWN_ERROR;
			}
		}
		if(IsRunStop())
		{
			m_bMeasureHeight = false;
			strError.Format("���ʧ��");
			PutLogIntoList(strError);
			return HEIGHT_STATUS_UNKNOWN_ERROR;
		}
		if(!SynchronizeMoveZ(tgMeasurePos.za,true))
		{
			m_bMeasureHeight = false;
			strError.Format("���ʧ��");
			PutLogIntoList(strError);
			return HEIGHT_STATUS_MOTION_FAILURE;
		}
		Sleep(DWORD(ProductParam.dHeightDelay*1000));
		ReadHeightSensorValue(&dTestHeight[0]);
		ReadHeightSensorValue(&dTestHeight[1]);
		ReadHeightSensorValue(&dTempHeight);
		dRealTimeData[i]=dTempHeight;

		dTotalHeight += dTempHeight;
		if(dMax<dTempHeight)
		{
			dMax = dTempHeight;
		}
		else if(dMin>dTempHeight)
		{
			dMin = dTempHeight;
		}
		str.Format(",%0.3f",dTestHeight[0]);
		str.Format(",%0.3f",dTestHeight[1]);
		str.Format(",%0.3f",dTempHeight);
		strTest[0] += str;
		strTest[1] += str;
		strText += str;
	}
	AddMsg(strText);
	PutLogIntoList(strText);
	AddMsg(strTest[0]);
	PutLogIntoList(strTest[0]);
	AddMsg(strTest[1]);
	PutLogIntoList(strTest[1]);
	//����������
	strError.Format("���ϸ߶����ƫ�%0.3f; �趨ֵ��%0.3f��",(dMax-dMin),ProductParam.dHeightError);
	AddMsg(strError);
	PutLogIntoList(strError);

	dOffsetZ = dTotalHeight/ProductParam.HeightNumber;//����ƽ��ֵ��Ϊ����������֮ǰ�趨��Z�߶�...//20180421	
	//strError.Format("�����߼���ZУ��ֵ(even):%0.3f",dOffsetZ);
	//AddMsg(strError);
	PutLogIntoList(strError);

	for(int i=0;i<ProductParam.HeightNumber;i++)
	{
		if(fabs(dRealTimeData[i]-g_pFrm->m_CmdRun.ProductParam.dLaserDataBase[i])>ProductParam.dHeightError)
		{
			m_bMeasureHeight = false;
			strError.Format("��%d���ʧ��:",i);
			PutLogIntoList(strError);
			AddMsg(strError);
			strError.Format("��%d��׼ֵ��%0.3f:",i,g_pFrm->m_CmdRun.ProductParam.dLaserDataBase[i]);
			PutLogIntoList(strError);
			AddMsg(strError);
			strError.Format("��%dʵʱֵ��%0.3f:",i,dRealTimeData[i]);
			PutLogIntoList(strError);
			AddMsg(strError);
			return HEIGHT_STATUS_EXCEED_LIMIT;
		}
	}

	//if(fabs(dMax-dMin)>ProductParam.dHeightError)
	//{
	//	m_bMeasureHeight = false;
	//	strError.Format("���ʧ��");
	//	m_pLog->log(strError);
	//	return HEIGHT_STATUS_EXCEED_LIMIT;
	//}

	//if(fabs(dOffsetZ)>ProductParam.dHeightCompMax)//���ֵ��Ч���ж�
	//{
	//	m_bMeasureHeight = false;
	//	strError.Format("���ʧ��");
	//	m_pLog->log(strError);
	//	return HEIGHT_STATUS_EXCEED_HEIGHTCOMP_LIMIT;
	//}
	m_bMeasureHeight = false;
	strError.Format("��߳ɹ�");
	PutLogIntoList(strError);
	return HEIGHT_STATUS_OK;
}

// �㽺
int  CCmdRun::DisDotCtrl()//�����ʱ���õ�æ�ź�
{
	if (ManalRun&&!AutoRunCp)
	{
		return theApp.m_V9Control.V9C_Dot(K_ALL_AXIS,false);
	}
	else
	{
		return theApp.m_V9Control.V9C_Dot(K_SELECT_AXIS,false);
	}
}

// �㽺λ
bool CCmdRun::DispensingPos(bool Z_OFF)
{
	if(!MoveToZSafety())
	{
		return false;
	}
	if(IsExisAlarm())
	{
		return false;
	}
	if(!SynchronizeMoveXY(ProductParam.DispenseStartPos[0],ProductParam.DispenseStartPos[1],true))
	{
		return false;
	}
	if(IsExisAlarm())
	{
		return false;
	}
	if (Z_OFF)
	{
		return SynchronizeMoveZ(ProductParam.DispenseStartPos[2],ProductParam.DispenseStartPos[3],ProductParam.DispenseStartPos[4],false,true);
	}
	return true;
}
// ����Ƿ��ڵ㽺��һ��λ��
bool CCmdRun::CheckMPdotFirst(void)
{
	if (fabs(PDposting[0].Dx-m_pMv->GetPos(K_AXIS_X))>0.1)
	{
	   return false;
	}
	if (fabs(PDposting[0].Dy-m_pMv->GetPos(K_AXIS_Y))>0.1)
	{
		return false;
	}
	return true;
}
// ��λ����
bool CCmdRun::RestDisP()
{
	static bool b=false;
	if (b)
	{
		return true;
	}
	b=true;
	
	int RestResult;
	CString str;
	int HomeResult;
	if(!theApp.m_tSysParam.V9HomeSu)//����
	{
		HomeResult=theApp.m_V9Control.V9C_Home(K_ALL_AXIS);
		if (HomeResult<0)
		{
			str.Format("��%d������������ʧ��",-HomeResult);
			AddMsg(str);
			Alarm(-HomeResult+9);
			b=false;
			return false;
		}
		else if (HomeResult>0)
		{
			str.Format("��%d���������ط�ʧ��",HomeResult);
			AddMsg(str);
			Alarm(HomeResult+1);
			b=false;
			return false;	
		}
		else if (HomeResult==0)
		{
			theApp.m_tSysParam.V9HomeSu=true;
		}	
	}
	//��λ
	V9RestOK=false;
	m_bValveReset = true;
	RestResult=theApp.m_V9Control.V9C_Rest(K_ALL_AXIS);
	m_bValveReset = false;
	if(RestResult<0)
	{
		str.Format("��%d���������ط�ʧ��",-RestResult);
		AddMsg(str);
		Alarm(-RestResult+1);
		b=false;
		AfxMessageBox(str);
		return false;
	}
	else if (RestResult>0)
	{
		str.Format("��%d������������ʧ��",RestResult);
		AddMsg(str);
		Alarm(RestResult+5);
		b=false;
		AfxMessageBox(str);
		return false;
	}
	if(!theApp.m_V9Control.V9C_SetProfile())
	{
		b = false;
		AfxMessageBox("�������ò�������");
		return false;
	}
	if(theApp.m_tSysParam.V9HomeSu&&theApp.m_tSysParam.ThreeAxHomeSu)
	{
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outReady,TRUE);
	}
	else
	{
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outReady,FALSE);
		str.Format("ThreeAxHomeSu:%d,V9HomeSu:%d",theApp.m_tSysParam.ThreeAxHomeSu,theApp.m_tSysParam.V9HomeSu);
		AddMsg(str);
	}
	V9RestOK=true;
	b=false;
	return true;

}
// ���ظ�λλ�ú͵㽺�����͵㽺�г�:��������ֵ
double  CCmdRun::CapRestDistance(double dispul,long m_dispsetcount, long *discount,double *dispdistance, double dFirstCompVol)
{
	double dReturnValue = -1;
	double m_dispdistance;
	double m_dFirstCompDistance;
	long  m_count = 0;
	if (theApp.m_tSysParam.Dispratios>0)
	{
		m_dispdistance=dispul/theApp.m_tSysParam.Dispratios;//�㽺�г�
		m_dFirstCompDistance = dFirstCompVol/theApp.m_tSysParam.Dispratios;//��һ�㽺�㲹���г�
	}   
	else
	{
		return -1;
	}
	if(fabs(m_dispdistance)<0.0001)
	{
		return -2;
	}
	if((VALVE_M9 == theApp.m_tSysParam.ValveType)||(VALVE_L9 == theApp.m_tSysParam.ValveType))
	{
		m_count=(theApp.m_tSysParam.DisEndDistance-theApp.m_tSysParam.DispFreeDistance-m_dFirstCompDistance-1)/m_dispdistance;
	}
	else if(VALVE_H9 == theApp.m_tSysParam.ValveType)
	{
		m_count=(theApp.m_tSysParam.DisEndDistance-theApp.m_tSysParam.DispFreeDistance-m_dFirstCompDistance)/m_dispdistance;
	}
	if (m_count<=0)
	{
		return -3;
	}
	*dispdistance=m_dispdistance;
	if (m_count<=m_dispsetcount)
	{
		*discount=m_count;
		if((VALVE_M9 == theApp.m_tSysParam.ValveType)||(VALVE_L9 == theApp.m_tSysParam.ValveType))
		{
			dReturnValue = theApp.m_tSysParam.DispFreeDistance+1;
		}
		else if(VALVE_H9 == theApp.m_tSysParam.ValveType)
		{
			dReturnValue = theApp.m_tSysParam.DisEndDistance - theApp.m_tSysParam.DispFreeDistance;
		}
		return dReturnValue;
	}
	*discount=m_dispsetcount;
	if((VALVE_M9 == theApp.m_tSysParam.ValveType)||(VALVE_L9 == theApp.m_tSysParam.ValveType))
	{
		dReturnValue = theApp.m_tSysParam.DisEndDistance-m_dispsetcount*m_dispdistance-m_dFirstCompDistance;
	}
	else if(VALVE_H9 == theApp.m_tSysParam.ValveType)
	{
		dReturnValue = m_dispsetcount*m_dispdistance+m_dFirstCompDistance;
	}
	return dReturnValue;
}
// ���ظ�λλ�ú͵㽺�����͵㽺�г̣�������ֵ 20180503 add by zwg
double  CCmdRun::CapRestDistance(double dispul,double compVol,long m_dispsetcount, long *discount,double *dispdistance, double dFirstCompVol)
{
	double dReturnValue = -1;
	double m_dispdistance;
	double m_dFirstCompDistance;
	long  m_count = 0;
	if (theApp.m_tSysParam.Dispratios>0)
	{
		m_dispdistance = (dispul+compVol)/theApp.m_tSysParam.Dispratios;//�㽺�г�
		m_dFirstCompDistance = dFirstCompVol/theApp.m_tSysParam.Dispratios;//��һ�㽺�㲹���г�
	}   
	else
	{
		return -1;
	}
	if(fabs(m_dispdistance)<0.0001)
	{
		return -2;
	}
	if((VALVE_M9 == theApp.m_tSysParam.ValveType)||(VALVE_L9 == theApp.m_tSysParam.ValveType))
	{
		m_count=(theApp.m_tSysParam.DisEndDistance-theApp.m_tSysParam.DispFreeDistance-m_dFirstCompDistance-1)/m_dispdistance;
	}
	else if(VALVE_H9 == theApp.m_tSysParam.ValveType)
	{
		m_count=(theApp.m_tSysParam.DisEndDistance-theApp.m_tSysParam.DispFreeDistance-m_dFirstCompDistance)/m_dispdistance;
	}
	if (m_count<=0)
	{
		return -3;
	}
	*dispdistance=m_dispdistance;
	if (m_count<=m_dispsetcount)
	{
		*discount=m_count;
		if((VALVE_M9 == theApp.m_tSysParam.ValveType)||(VALVE_L9 == theApp.m_tSysParam.ValveType))
		{
			dReturnValue = theApp.m_tSysParam.DispFreeDistance+1;
		}
		else if(VALVE_H9 == theApp.m_tSysParam.ValveType)
		{
			dReturnValue = theApp.m_tSysParam.DisEndDistance - theApp.m_tSysParam.DispFreeDistance;
		}
		return dReturnValue;
	}
	*discount=m_dispsetcount;
	if((VALVE_M9 == theApp.m_tSysParam.ValveType)||(VALVE_L9 == theApp.m_tSysParam.ValveType))
	{
		dReturnValue = theApp.m_tSysParam.DisEndDistance-m_dispsetcount*m_dispdistance-m_dFirstCompDistance;
	}
	else if(VALVE_H9 == theApp.m_tSysParam.ValveType)
	{
		dReturnValue = m_dispsetcount*m_dispdistance+m_dFirstCompDistance;
	}
	return dReturnValue;
}

// �ֶ���������
void CCmdRun::ZEROClearn(void)
{
	//StautDisp.W_Count=0;
	theApp.m_tSysParam.StatCapacity = 0;
}

// ���и�λ�߳�
void CCmdRun::RunThreadHome()
{
	g_pHomeThread = AfxBeginThread(CCmdRun::ThreadHome,this);
	if(g_pHomeThread==NULL)
	{
		m_tStatus = K_RUN_STS_STOP;
		AfxMessageBox("�����̴߳���ʧ��.");
		return;
	}
}

// �㽺����
void CCmdRun::DispenseOperation()
{
	CString strError;
	
	if(!MoveToZSafety())
	{
		return;
	}

	V9DotRest=false;
	if(m_tStatus == K_RUN_STS_PAUSE)
	{
		if(!WaitPasueEnd())
		{
			return;
		}
	}
	if(IsRunStop())
	{
		return ;
	}
	if(ONE_VALVE == ProductParam.valveSelect)
	{
		if (COMPLEX_DISP_MODE==ProductParam.fillMode)
		{
			if(!SingleComplexDispense())
			{
				if(!IsRunStop()&&!IsRunAlarm()&&!IsRunEmc())
				{
					Alarm(37);
				}
				strError.Format("�����ۺ�ģʽ�㽺����");
				PutLogIntoList(strError);
				return;
			}
		}
		else
		{
			if(!SingleDispense())
			{
				if(!IsRunStop()&&!IsRunAlarm()&&!IsRunEmc())
				{
					Alarm(37);
				}
				strError.Format("�����㽺����");
				PutLogIntoList(strError);
				return;
			}
		}	
	}
	else if(TWO_VALVE == ProductParam.valveSelect)
	{
		if(!SecondDispense())
		{
			if(!IsRunStop()&&!IsRunAlarm()&&!IsRunEmc())
			{
				Alarm(37);
			}
			strError.Format("˫���㽺����");
			PutLogIntoList(strError);
			return;
		}
	}
	else
	{
		if(!ThirdDispense())
		{
			if(!IsRunStop()&&!IsRunAlarm()&&!IsRunEmc())
			{
				Alarm(37);
			}
			strError.Format("�����㽺����");
			PutLogIntoList(strError);
			return;
		}
	}
	ProductParam.tgTrackInfo.TrackReset();
}

//������λ��
void CCmdRun::PosInit()
{
	int nSecondNum = ProductParam.SecondDispRow*ProductParam.SecondDispColumn;
	int nThirdNum = ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
	int temID2=nThirdNum*nSecondNum*ProductParam.FirstDispRow*ProductParam.FirstDispColumn;
	if (temID2<=0)
	{
		return ;
	}
	if (PDpostingBug!=NULL)
	{
		delete[] PDpostingBug;
	}

	PDpostingBug=new tagDPostion[temID2];
	//0//1//2//3//4//5//6//7//
	//8//9//10/11/12//13/14//15//
	///////////////////
	///////////////////
	///////////////////
	///////////////////
	///////////////////
	///////////////////
	///////////////////
	///////////////////
	///////////////////
	///////////////////
	int temID;
	for(int i=0;i<ProductParam.FirstDispRow;i++)
	{
		for(int j=0;j<ProductParam.FirstDispColumn;j++)
		{
			for(int m=0;m<ProductParam.SecondDispRow;m++)
			{
				for(int n=0;n<ProductParam.SecondDispColumn;n++)
				{
					for(int k=0;k<ProductParam.ThirdDispRow;k++)
					{
						for(int l=0;l<ProductParam.ThirdDispColumn;l++)
						{
							temID=l+k*ProductParam.ThirdDispColumn+n*nThirdNum+m*ProductParam.SecondDispColumn*nThirdNum+
								j*nSecondNum*nThirdNum+i*ProductParam.FirstDispColumn*nSecondNum*nThirdNum;
							tgPos tgTempPos = GetPadDispensePos(i,j,m,n,k,l);
							
							PDpostingBug[temID].Dx=tgTempPos.x;
							PDpostingBug[temID].Dy=tgTempPos.y;
							if(!theApp.m_Mv400.WarnCheckPos(&PDpostingBug[temID].Dx,&PDpostingBug[temID].Dy,&ProductParam.DispenseStartPos[2],NULL))
							{
								AfxMessageBox("��ͷ�������ʱλ�ü��㳬��.");
								return ;
							}
						}
					}
				}
			}
		}
	}
	
}
// �㽺λ�ü���
BOOL CCmdRun::ExtandTrack(double offsetX,double offsetY,double offsetZ,double dAngle)
{
	tgPos tgTempPos;
	CString strTemp;
	double dStartPos[5],dEndPos[5];
	double dCenterX,dCenterY;
	double dAlignCenterX,dAlignCenterY;
	if(ProductParam.IrregularMatrix)
	{
		int temID2 = ProductParam.IrregularRow*ProductParam.IrregularColumn;
		if(temID2<=0)
		{
			return false;
		}
		if (PDposting!=NULL)
		{
			LONGLONG llDueTime = CTimeUtil::GetDueTime(5000);
			while(!CTimeUtil::IsTimeout(llDueTime))
			{
				if(!m_bRefresh)
				{
					delete[] PDposting;
					PDposting = NULL;
					break;
				}
			}
			if(PDposting!=NULL)
			{
				//AfxMessageBox("ˢ�½��泬ʱ��");
				return false;
			}
		}
		PDposting=new tagDPostion[temID2];
		int temID = 0;
		if(ONE_VALVE==ProductParam.valveSelect)
		{
			tgTempPos = GetPadDispensePos(ProductParam.IrregularRow-1,ProductParam.IrregularColumn-1);
		}
		else if(TWO_VALVE==ProductParam.valveSelect)
		{
			tgTempPos = GetPadDispensePos(ProductParam.IrregularRow-1,(ProductParam.IrregularColumn-1)/2);
		}
		else
		{
			tgTempPos = GetPadDispensePos(ProductParam.IrregularRow-1,(ProductParam.IrregularColumn-1)/3);
		}
		for(int i=0;i<5;i++)
		{
			dStartPos[i] = ProductParam.DispenseStartPos[i];
			if(i>=2)
			{
				dStartPos[i]=dStartPos[i]- ProductParam.dDispenseHeight;
			}
		}
		dEndPos[0] = tgTempPos.x;
		dEndPos[1] = tgTempPos.y;
		dEndPos[2] = dStartPos[2];
		dEndPos[3] = dStartPos[3];
		dEndPos[4] = dStartPos[4];
		dCenterX = (dStartPos[0]+dEndPos[0])/2;
		dCenterY = (dStartPos[1]+dEndPos[1])/2;
		dAlignCenterX = dCenterX - offsetX;
		dAlignCenterY = dCenterY - offsetY;
		for(int i=0;i<ProductParam.IrregularRow;i++)
		{
			for(int j=0;j<ProductParam.IrregularColumn;j++)
			{
				tgTempPos = GetPadDispensePos(i,j);
				temID=j+i*ProductParam.IrregularColumn;
				PDposting[temID].bIsCompleted = false;
				PDposting[temID].bIsHasPad = true;
				PDposting[temID].Dx=tgTempPos.x;
				PDposting[temID].Dy=tgTempPos.y;
				//if (1 == ProductParam.IfUseLaser)
				//{
				//	PDposting[temID].Dz = g_pFrm->m_CmdRun.ProductParam.tgLaserPos.za - g_pFrm->m_CmdRun.ProductParam.HeightCalibrationPos[2] + 
				//		g_pFrm->m_CmdRun.ProductParam.LaserHeightValue - g_pFrm->m_CmdRun.ProductParam.LaserFeedHeight;
				//}
				//else
				if(ProductParam.MeasureHeightEnable)
				{ 
					double dLaserBaser=ProductParam.HeightStandardValue;
					double dLaserRelative=fabs(dLaserBaser-offsetZ);
					PDposting[temID].Dza=ProductParam.ContactSensorPos[2]+dLaserRelative-0.3;
					PDposting[temID].Dzb=ProductParam.ContactSensorPos[3]+dLaserRelative-0.3;
					PDposting[temID].Dzc=ProductParam.ContactSensorPos[4]+dLaserRelative-0.3;								
				}
				else
				{
					PDposting[temID].Dza=dStartPos[2];
					PDposting[temID].Dzb=dStartPos[3];
					PDposting[temID].Dzc=dStartPos[4];
				}
				if(m_pPadDetect!=NULL)
				{
					if(m_pPadDetect[temID]==false)
					{
						PDposting[temID].bIsHasPad = false;
					}
				}
				if(ONE_VALVE==ProductParam.valveSelect)
				{
					if(ProductParam.visionMode==VISION_TWO_MARK)
					{
						double dX = PDposting[temID].Dx - dCenterX;
						double dY = PDposting[temID].Dy - dCenterY;
						Rotation(dX,dY,-dAngle,dX,dY);
						PDposting[temID].Dx = dAlignCenterX + dX;
						PDposting[temID].Dy = dAlignCenterY + dY;
					}
					else
					{
						PDposting[temID].Dx -= offsetX;
						PDposting[temID].Dy -= offsetY;
						//strTemp.Format("OffsetX = %0.3f,OffsetY = %0.3f,temID = %d",offsetX,offsetY,temID);
						//AddMsg(strTemp);
					}
					PDposting[temID].Dx += ProductParam.DispOffsetX;
					PDposting[temID].Dy += ProductParam.DispOffsetY;
					if(!theApp.m_Mv400.WarnCheckPos(&PDposting[temID].Dx,&PDposting[temID].Dy,&dStartPos[2],NULL))
					{
						AfxMessageBox("��������г̷�Χ�������Ʒ�������趨");
						return false;
					}
				}
				else if(TWO_VALVE==ProductParam.valveSelect)
				{
					if(ProductParam.visionMode==VISION_TWO_MARK)
					{
						if(j<(ProductParam.IrregularColumn+1)/2)
						{
							double dX = PDposting[temID].Dx - dCenterX;
							double dY = PDposting[temID].Dy - dCenterY;
							Rotation(dX,dY,-dAngle,dX,dY);
							PDposting[temID].Dx = dAlignCenterX + dX;
							PDposting[temID].Dy = dAlignCenterY + dY;
						}
					}
					else
					{
						PDposting[temID].Dx -= offsetX;
						PDposting[temID].Dy -= offsetY;
					}
					PDposting[temID].Dx += ProductParam.DispOffsetX;
					PDposting[temID].Dy += ProductParam.DispOffsetY;
					if(j<(ProductParam.IrregularColumn+1)/2)
					{
						if(!theApp.m_Mv400.WarnCheckPos(&PDposting[temID].Dx,&PDposting[temID].Dy,&dStartPos[2],NULL))
						{
							AfxMessageBox("��������г̷�Χ�������Ʒ�������趨");
							return false;
						}
					}
				}
				else
				{
					if(ProductParam.visionMode==VISION_TWO_MARK)
					{
						if(j<(ProductParam.IrregularColumn+2)/3)
						{
							double dX = PDposting[temID].Dx - dCenterX;
							double dY = PDposting[temID].Dy - dCenterY;
							Rotation(dX,dY,-dAngle,dX,dY);
							PDposting[temID].Dx = dAlignCenterX + dX;
							PDposting[temID].Dy = dAlignCenterY + dY;
							//strTemp.Format("OffsetX = %0.3f,OffsetY = %0.3f,temID = %d",dX,dY,temID);
							//AddMsg(strTemp);
						}
					}
					else
					{
						PDposting[temID].Dx -= offsetX;
						PDposting[temID].Dy -= offsetY;
						//strTemp.Format("OffsetX = %0.3f,OffsetY = %0.3f,temID = %d",offsetX,offsetY,temID);
						//AddMsg(strTemp);
					}
					PDposting[temID].Dx += ProductParam.DispOffsetX;
					PDposting[temID].Dy += ProductParam.DispOffsetY;
					if(j<(ProductParam.IrregularColumn+2)/3)
					{
						if(!theApp.m_Mv400.WarnCheckPos(&PDposting[temID].Dx,&PDposting[temID].Dy,&dStartPos[2],NULL))
						{
							AfxMessageBox("��������г̷�Χ�������Ʒ�������趨");
							return false;
						}
					}
				}
			}
		}
	}
	else
	{
		int nThirdNum = ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
		int nSecondNum = ProductParam.SecondDispRow*ProductParam.SecondDispColumn;
		int temID2=ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.FirstDispRow*ProductParam.FirstDispColumn;
		if(temID2<=0)
		{
			return false;
		}
		//strTemp.Format("tempID2:%d",temID2);
		//AddMsg(strTemp);
		//if(PDposting!=NULL)
		//{
		//	for(int i=0;i<temID2;i++)
		//	{
		//		strTemp.Format("PDposting[%d]:%.3f,%.3f",i,PDposting[i].Dx,PDposting[i].Dy);
		//		AddMsg(strTemp);
		//	}
		//}
		if(PDposting!=NULL)
		{
			LONGLONG llDueTime = CTimeUtil::GetDueTime(5000);
			while(!CTimeUtil::IsTimeout(llDueTime))
			{
				if(!m_bRefresh)
				{
					delete[] PDposting;
					PDposting = NULL;
					break;
				}
			}
			if(PDposting!=NULL)
			{
				AfxMessageBox("ˢ�½��泬ʱ��");
				return false;
			}
		}
		PDposting=new tagDPostion[temID2];
		//0//1//2//3//4//5//6//7//
		//8//9//10/11/12//13/14//15//
		///////////////////
		///////////////////
		///////////////////
		///////////////////
		///////////////////
		///////////////////
		///////////////////
		///////////////////
		///////////////////
		///////////////////
		int temID = 0;
		for(int i=0;i<5;i++)
		{
			dStartPos[i] = ProductParam.DispenseStartPos[i];
		}
		tgTempPos =GetPadDispensePos(ProductParam.FirstDispRow-1,ProductParam.FirstDispColumn-1,ProductParam.SecondDispRow-1,
			ProductParam.SecondDispColumn-1,ProductParam.ThirdDispRow-1,ProductParam.ThirdDispColumn-1);

		dEndPos[0] = tgTempPos.x;
		dEndPos[1] = tgTempPos.y;
		dEndPos[2] = dStartPos[2];
		dEndPos[3] = dStartPos[3];
		dEndPos[4] = dStartPos[4];
		//////////////////////////////ʵ������ϵ�µ�ĳһ��׼��/////////////////////////////////////////////
		dStartPos[0]=ProductParam.tgImagePos[0].x+(ProductParam.tgLabelPos[0].x-ProductParam.tgLabelPos[3].x);
		dStartPos[1]=ProductParam.tgImagePos[0].y+(ProductParam.tgLabelPos[0].y-ProductParam.tgLabelPos[3].y);
		dEndPos[0]=ProductParam.tgImagePos[1].x+(ProductParam.tgLabelPos[0].x-ProductParam.tgLabelPos[3].x);
		dEndPos[1]=ProductParam.tgImagePos[1].y+(ProductParam.tgLabelPos[0].y-ProductParam.tgLabelPos[3].y);
		////////////////////////////////////////////////////////////////////////////////////////////////////
		dCenterX = (dStartPos[0]+dEndPos[0])/2;// X in Theory;
		dCenterY = (dStartPos[1]+dEndPos[1])/2;//Y in Theory;
		dAlignCenterX = dCenterX - offsetX; // X Actually
		dAlignCenterY = dCenterY - offsetY; // Y Actual

		for(int i=0;i<ProductParam.FirstDispRow;i++)
		{
			for(int j=0;j<ProductParam.FirstDispColumn;j++)
			{
				for(int m=0;m<ProductParam.SecondDispRow;m++)
				{
					for(int n=0;n<ProductParam.SecondDispColumn;n++)
					{
						for(int k=0;k<ProductParam.ThirdDispRow;k++)
						{
							for(int l=0;l<ProductParam.ThirdDispColumn;l++)
							{
								temID=l+k*ProductParam.ThirdDispColumn+n*nThirdNum+m*ProductParam.SecondDispColumn*nThirdNum+
									j*nSecondNum*nThirdNum+i*ProductParam.FirstDispColumn*nSecondNum*nThirdNum;
								tgTempPos = GetPadDispensePos(i,j,m,n,k,l);
								PDposting[temID].nFirstRow = i;
								PDposting[temID].nFirstColumn = j;
								PDposting[temID].nSecondRow = m;
								PDposting[temID].nSecondColumn = n;
								PDposting[temID].nThirdRow = k;
								PDposting[temID].nThirdColumn = l;
								PDposting[temID].bIsCompleted = false;
								PDposting[temID].bIsHasPad = true;
								PDposting[temID].Dx=tgTempPos.x;
								PDposting[temID].Dy=tgTempPos.y;
								//PDposting[temID].Dz=dStartPos[2]+offsetZ;
								//if (1 == ProductParam.IfUseLaser)
								//{
								//	PDposting[temID].Dz = g_pFrm->m_CmdRun.ProductParam.tgLaserPos.za - g_pFrm->m_CmdRun.ProductParam.HeightCalibrationPos[2] + 
								//		g_pFrm->m_CmdRun.ProductParam.LaserHeightValue - g_pFrm->m_CmdRun.ProductParam.LaserFeedHeight;
								//}
								//else
								/*if(ProductParam.MeasureHeightEnable)
								{ 
									double dLaserBaser=ProductParam.HeightStandardValue;
									double dLaserRelative=fabs(dLaserBaser-offsetZ);
									PDposting[temID].Dza=ProductParam.ContactSensorPos[2]+dLaserRelative-0.3-ProductParam.dDispenseHeight;
									PDposting[temID].Dzb=ProductParam.ContactSensorPos[3]+dLaserRelative-0.3-ProductParam.dDispenseHeight;
									PDposting[temID].Dzc=ProductParam.ContactSensorPos[4]+dLaserRelative-0.3-ProductParam.dDispenseHeight;								
								}
								else*/
								{
									PDposting[temID].Dza=dStartPos[2]/*-ProductParam.dDispenseHeight+ProductParam.dZDispOffsetAll*/;
									PDposting[temID].Dzb=dStartPos[3]/*-ProductParam.dDispenseHeight+ProductParam.dZDispOffsetAll*/;
									PDposting[temID].Dzc=dStartPos[4]/*-ProductParam.dDispenseHeight+ProductParam.dZDispOffsetAll*/;
								}
								//�����Լ��
								if(TRANSITION_INSPECT==ProductParam.inspectMode)
								{
									if(m_pPadDetect!=NULL)
									{
										if(!m_pPadDetect[temID])
										{
											PDposting[temID].bIsHasPad = false;
										}
									}
								}
								//ƫ��������
								if(THREE_VALVE==ProductParam.valveSelect)
								{
									if (ProductParam.FirstDispColumn>1)
									{
										if (j<((ProductParam.FirstDispColumn+2)/3))
										{
											if(ProductParam.visionMode==VISION_TWO_MARK)
											{
												double dX = PDposting[temID].Dx - dCenterX;
												double dY = PDposting[temID].Dy - dCenterY;
												Rotation(dX,dY,dAngle,dX,dY);
												PDposting[temID].Dx = dAlignCenterX + dX;
												PDposting[temID].Dy = dAlignCenterY + dY;
												strTemp.Format("OffsetX = %0.3f,OffsetY = %0.3f,temID = %d",dX,dY,temID);
												AddMsg(strTemp);
											}
											else //A.1MARKʱ����Ƕȣ�B.ͼ�����ʱoffsetX/offsetY/offsetZ��0--->���ô˶��߼���
											{
												PDposting[temID].Dx -= offsetX;
												PDposting[temID].Dy -= offsetY;
												//strTemp.Format("OffsetX = %0.3f,OffsetY = %0.3f,temID = %d",offsetX,offsetY,temID);
												//AddMsg(strTemp);
											}
											PDposting[temID].Dx += ProductParam.DispOffsetX;
											PDposting[temID].Dy += ProductParam.DispOffsetY;
											//���ݹ������ݼ����г̷�Χ���Ӿ�׼��
											if(!theApp.m_Mv400.WarnCheckPos(&PDposting[temID].Dx,&PDposting[temID].Dy,
												&PDposting[temID].Dza,&PDposting[temID].Dzb,&PDposting[temID].Dzc))
											{
												AfxMessageBox("��������г̷�Χ�������Ʒ�������趨");
												return false;
											}
										}
									}
									else if (ProductParam.SecondDispColumn>1)
									{
										if(n<((ProductParam.SecondDispColumn+2)/3))
										{
											if(ProductParam.visionMode==VISION_TWO_MARK)
											{
												double dX = PDposting[temID].Dx - dCenterX;
												double dY = PDposting[temID].Dy - dCenterY;
												Rotation(dX,dY,dAngle,dX,dY);
												PDposting[temID].Dx = dAlignCenterX + dX;
												PDposting[temID].Dy = dAlignCenterY + dY;
												strTemp.Format("OffsetX = %0.3f,OffsetY = %0.3f,temID = %d",dX,dY,temID);
												AddMsg(strTemp);
											}
											else //A.1MARKʱ����Ƕȣ�B.ͼ�����ʱoffsetX/offsetY/offsetZ��0--->���ô˶��߼���
											{
												PDposting[temID].Dx -= offsetX;
												PDposting[temID].Dy -= offsetY;
												//strTemp.Format("OffsetX = %0.3f,OffsetY = %0.3f,temID = %d",offsetX,offsetY,temID);
												//AddMsg(strTemp);
											}
											PDposting[temID].Dx += ProductParam.DispOffsetX;
											PDposting[temID].Dy += ProductParam.DispOffsetY;
											//���ݹ������ݼ����г̷�Χ���Ӿ�׼��
											if(!theApp.m_Mv400.WarnCheckPos(&PDposting[temID].Dx,&PDposting[temID].Dy,
												&PDposting[temID].Dza,&PDposting[temID].Dzb,&PDposting[temID].Dzc))
											{
												AfxMessageBox("��������г̷�Χ�������Ʒ�������趨");
												return false;
											}
										}
									}
									else
									{
										if(l<((ProductParam.ThirdDispColumn+2)/3))
										{
											if(ProductParam.visionMode==VISION_TWO_MARK)
											{
												double dX = PDposting[temID].Dx - dCenterX;
												double dY = PDposting[temID].Dy - dCenterY;
												Rotation(dX,dY,dAngle,dX,dY);
												PDposting[temID].Dx = dAlignCenterX + dX;
												PDposting[temID].Dy = dAlignCenterY + dY;
												strTemp.Format("OffsetX = %0.3f,OffsetY = %0.3f,temID = %d",dX,dY,temID);
												AddMsg(strTemp);
											}
											else //A.1MARKʱ����Ƕȣ�B.ͼ�����ʱoffsetX/offsetY/offsetZ��0--->���ô˶��߼���
											{
												PDposting[temID].Dx -= offsetX;
												PDposting[temID].Dy -= offsetY;
												//strTemp.Format("OffsetX = %0.3f,OffsetY = %0.3f,temID = %d",offsetX,offsetY,temID);
												//AddMsg(strTemp);
											}
											PDposting[temID].Dx += ProductParam.DispOffsetX;
											PDposting[temID].Dy += ProductParam.DispOffsetY;
											//���ݹ������ݼ����г̷�Χ���Ӿ�׼��
											if(!theApp.m_Mv400.WarnCheckPos(&PDposting[temID].Dx,&PDposting[temID].Dy,
												&PDposting[temID].Dza,&PDposting[temID].Dzb,&PDposting[temID].Dzc))
											{
												AfxMessageBox("��������г̷�Χ�������Ʒ�������趨");
												return false;
											}
										}
									}
								}
								else if(TWO_VALVE==ProductParam.valveSelect)
								{
									if (ProductParam.FirstDispColumn>1)
									{
										if(j<((ProductParam.FirstDispColumn+1)/2))
										{
											if(ProductParam.visionMode==VISION_TWO_MARK)
											{
												double dX = PDposting[temID].Dx - dCenterX;
												double dY = PDposting[temID].Dy - dCenterY;
												Rotation(dX,dY,dAngle,dX,dY);
												PDposting[temID].Dx = dAlignCenterX + dX;
												PDposting[temID].Dy = dAlignCenterY + dY;
												strTemp.Format("OffsetX = %0.3f,OffsetY = %0.3f,temID = %d",dX,dY,temID);
												AddMsg(strTemp);
											}
											else //A.1MARKʱ����Ƕȣ�B.ͼ�����ʱoffsetX/offsetY/offsetZ��0--->���ô˶��߼���
											{
												PDposting[temID].Dx -= offsetX;
												PDposting[temID].Dy -= offsetY;
												//strTemp.Format("OffsetX = %0.3f,OffsetY = %0.3f,temID = %d",offsetX,offsetY,temID);
												//AddMsg(strTemp);
											}
											PDposting[temID].Dx += ProductParam.DispOffsetX;
											PDposting[temID].Dy += ProductParam.DispOffsetY;
											//���ݹ������ݼ����г̷�Χ���Ӿ�׼��
											if(!theApp.m_Mv400.WarnCheckPos(&PDposting[temID].Dx,&PDposting[temID].Dy,
												&PDposting[temID].Dza,&PDposting[temID].Dzb,&PDposting[temID].Dzc))
											{
												AfxMessageBox("��������г̷�Χ�������Ʒ�������趨");
												return false;
											}
										}
									}
									else if (ProductParam.SecondDispColumn>1)
									{
										if(n<((ProductParam.SecondDispColumn+1)/2))
										{
											if(ProductParam.visionMode==VISION_TWO_MARK)
											{
												double dX = PDposting[temID].Dx - dCenterX;
												double dY = PDposting[temID].Dy - dCenterY;
												Rotation(dX,dY,dAngle,dX,dY);
												PDposting[temID].Dx = dAlignCenterX + dX;
												PDposting[temID].Dy = dAlignCenterY + dY;
												strTemp.Format("OffsetX = %0.3f,OffsetY = %0.3f,temID = %d",dX,dY,temID);
												AddMsg(strTemp);
											}
											else //A.1MARKʱ����Ƕȣ�B.ͼ�����ʱoffsetX/offsetY/offsetZ��0--->���ô˶��߼���
											{
												PDposting[temID].Dx -= offsetX;
												PDposting[temID].Dy -= offsetY;
												//strTemp.Format("OffsetX = %0.3f,OffsetY = %0.3f,temID = %d",offsetX,offsetY,temID);
												//AddMsg(strTemp);
											}
											PDposting[temID].Dx += ProductParam.DispOffsetX;
											PDposting[temID].Dy += ProductParam.DispOffsetY;
											//���ݹ������ݼ����г̷�Χ���Ӿ�׼��
											if(!theApp.m_Mv400.WarnCheckPos(&PDposting[temID].Dx,&PDposting[temID].Dy,
												&PDposting[temID].Dza,&PDposting[temID].Dzb,&PDposting[temID].Dzc))
											{
												AfxMessageBox("��������г̷�Χ�������Ʒ�������趨");
												return false;
											}
										}
									}
									else
									{
										if(l<((ProductParam.ThirdDispColumn+1)/2))
										{
											if(ProductParam.visionMode==VISION_TWO_MARK)
											{
												double dX = PDposting[temID].Dx - dCenterX;
												double dY = PDposting[temID].Dy - dCenterY;
												Rotation(dX,dY,dAngle,dX,dY);
												PDposting[temID].Dx = dAlignCenterX + dX;
												PDposting[temID].Dy = dAlignCenterY + dY;
												strTemp.Format("OffsetX = %0.3f,OffsetY = %0.3f,temID = %d",dX,dY,temID);
												AddMsg(strTemp);
											}
											else //A.1MARKʱ����Ƕȣ�B.ͼ�����ʱoffsetX/offsetY/offsetZ��0--->���ô˶��߼���
											{
												PDposting[temID].Dx -= offsetX;
												PDposting[temID].Dy -= offsetY;
												//strTemp.Format("OffsetX = %0.3f,OffsetY = %0.3f,temID = %d",offsetX,offsetY,temID);
												//AddMsg(strTemp);
											}
											PDposting[temID].Dx += ProductParam.DispOffsetX;
											PDposting[temID].Dy += ProductParam.DispOffsetY;
											//���ݹ������ݼ����г̷�Χ���Ӿ�׼��
											if(!theApp.m_Mv400.WarnCheckPos(&PDposting[temID].Dx,&PDposting[temID].Dy,
												&PDposting[temID].Dza,&PDposting[temID].Dzb,&PDposting[temID].Dzc))
											{
												AfxMessageBox("��������г̷�Χ�������Ʒ�������趨");
												return false;
											}
										}
									}
								}
								else
								{
									if(ProductParam.visionMode==VISION_TWO_MARK)
									{
										double dX = PDposting[temID].Dx - dCenterX;
										double dY = PDposting[temID].Dy - dCenterY;
										Rotation(dX,dY,dAngle,dX,dY);
										PDposting[temID].Dx = dAlignCenterX + dX;
										PDposting[temID].Dy = dAlignCenterY + dY;
										strTemp.Format("OffsetX = %0.3f,OffsetY = %0.3f,temID = %d",dX,dY,temID);
										AddMsg(strTemp);
									}
									else //A.1MARKʱ����Ƕȣ�B.ͼ�����ʱoffsetX/offsetY/offsetZ��0--->���ô˶��߼���
									{
										PDposting[temID].Dx -= offsetX;
										PDposting[temID].Dy -= offsetY;
										//strTemp.Format("OffsetX = %0.3f,OffsetY = %0.3f,temID = %d",offsetX,offsetY,temID);
										//AddMsg(strTemp);
									}
									PDposting[temID].Dx += ProductParam.DispOffsetX;
									PDposting[temID].Dy += ProductParam.DispOffsetY;
									//���ݹ������ݼ����г̷�Χ���Ӿ�׼��
									if(!theApp.m_Mv400.WarnCheckPos(&PDposting[temID].Dx,&PDposting[temID].Dy,
										&PDposting[temID].Dza,&PDposting[temID].Dzb,&PDposting[temID].Dzc))
									{
										AfxMessageBox("��������г̷�Χ�������Ʒ�������趨");
										return false;
									}
								}						
							}
						}
					}
				}
			}
		}
	}
	if(ProductParam.MeasureHeightEnable)
	{
		strTemp.Format("[[�Զ���߿���]]������ʵʱֵ:%0.3f;�����׼ֵ:%0.3f",offsetZ,ProductParam.HeightStandardValue);
		AddMsg(strTemp);
		PutLogIntoList(strTemp);
		strTemp.Format("��λ�ü�����ɡ��Ӵ����Zֵ��%0.3f;%0.3f;%0.3f...",ProductParam.ContactSensorPos[2],ProductParam.ContactSensorPos[3],ProductParam.ContactSensorPos[4]);
		AddMsg(strTemp);
		PutLogIntoList(strTemp);
		//strTemp.Format("��λ�ü�����ɡ�����������㽺Zֵ��%0.3f;%0.3f;%0.3f...",PDposting[0].Dza,PDposting[0].Dzb,PDposting[0].Dzc);
		//AddMsg(strTemp);
		//m_pLog->log(strTemp);
		strTemp.Format("�㽺�߶ȣ�%0.3f",ProductParam.dDispenseHeight);
		AddMsg(strTemp);
		PutLogIntoList(strTemp);
	}
	return true;
}
// ����
void CCmdRun::Run()
{
	CString str;
	DWORD StartTime,EndTime;
	DWORD W_Startime,W_EndTime;
	W_Startime = W_EndTime = GetTickCount();//UPH ���� 2018-04-16
	DisCompCount=0;
	m_tOffset.x = 0.0;
	m_tOffset.y = 0.0;
	m_tOffset.za = 0.0;
	m_dRotateAngle = 0.0;
	m_bFirstDisp = true;
	m_bStartRun = true;
	if(m_bIsPause)
	{
		m_tStatus = K_RUN_STS_PAUSE;
	}
	else
	{
		m_tStatus = K_RUN_STS_RUN;
	}
	AddMsg("Start CopyPadDtect Result!");
	CopyPadDtect();
	AddMsg("End CopyPadDtect Result!");
	if(g_pFrm->m_wndPosInfo.TimeStarC)//�Զ�����
	{
		g_pFrm->m_wndPosInfo.TimeStarC=false;
		RunErrorID=-1;
	}
	if(m_tStatus == K_RUN_STS_PAUSE)
	{
		if(!WaitPasueEnd())
		{
			return;
		}
	}
	if(IsRunStop())
	{
		return;
	}
	str.Format("��ʼ����");
	PutLogIntoList(str);

    //if((1==ProductParam.NeedleHeightCheck)&&(pcsDoneFromStart%ProductParam.ContactFrequency==0)) //��ͷ�߶�У��
	//{		
	//	if(!NeedleHeightCheck())
	//	{
	//		return;
	//	}
	//}
	if(pcsDoneFromStart>500)
	{
		pcsDoneFromStart=0;
	}

	if(!ProductParam.DispMode)//����ģʽ
	{
		str.Format("V9Reset:%d",(int)V9RestOK);
		AddMsg(str);
		if (!V9RestOK)//
		{
			if(!RestDisP())
			{
				m_tStatus = K_RUN_STS_STOP;
				str.Format("���帴λʧ�ܣ�����ֹͣ");
				PutLogIntoList(str);
				return;
			}
		}
		StartTime = EndTime = GetTickCount();
		while (!V9RestOK)  //���帴λ�Ƿ�ɹ�
		{
			CFunction::DoEvents();
			EndTime= GetTickCount();
			if ((EndTime-StartTime)/1000>8)//4S
			{
				m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
				Alarm(10);
				str.Format("���帴λʧ��!");
				AddMsg(str);
				PutLogIntoList(str);
				return;
			}
			Sleep(350);
		}
		g_pFrm->m_CmdRun.ProductParam.DispFCount=0;

		if(!ManalRun)
		{
			AutoRunCp=true;

			for (int i=0;i<3;i++)
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					g_pFrm->m_CmdRun.RunDispFSelect[i]=true;
					g_pFrm->m_CmdRun.ProductParam.DispFCount++;
				}
				else
				{
					g_pFrm->m_CmdRun.RunDispFSelect[i]=false;
				}
			}
		}
		else
		{
			for (int i=0;i<3;i++)
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					g_pFrm->m_CmdRun.RunDispFSelect[i]=true;
					g_pFrm->m_CmdRun.ProductParam.DispFCount++;
					AutoRunCp=false;
				}
				else
				{
					g_pFrm->m_CmdRun.RunDispFSelect[i]=false;
				}
			}
		}
	}
	LONGLONG llDueDalay = CTimeUtil::GetDueTime(30000);
	while(!CTimeUtil::IsTimeout(llDueDalay))
	{
		if(!m_bValveReset)
		{
			break;
		}
		Sleep(1);
	}
	if(m_bValveReset)
	{
		Alarm(29);
		str.Format("���帴λ��ʱ������ֹͣ��");
		PutLogIntoList(str);
		return;
	}
	if(ProductParam.SlotDetect)
	{
		StartTime = EndTime = GetTickCount();
		if(!SlotDetection())
		{
			m_bIsSupend = true;
			if(AfxMessageBox("��ȷ��֧�������棡�Ƿ������",MB_YESNO)!=IDYES)
			{
				m_bIsSupend = false;
				m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
				Alarm(18);
				str.Format("��Ʒ�ŷ�������ֹͣ");
				PutLogIntoList(str);
				return;
			}
			m_bIsSupend = false;
		}
		EndTime = GetTickCount();
		str.Format("SlotDetection:%ld",EndTime-StartTime);
		AddMsg(str);
	}
	///////////////////////////////////////////////////////////////////////////
	//�ָ�ȱ������⹦�ܣ�2016-05-27       2016-12-05 �����δ˲��ִ���
	//if(ProductParam.tgTrackInfo.IsTrackReset())
	//{
	//	if(ProductParam.inspectMode==SCAN_INSPECT)
	//	{	
	//		StartTime = EndTime = GetTickCount();
	//		if(!ScanInspect())
	//		{
	//			m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
	//			Alarm(22);
	//			str.Format("ɨ����ʧ�ܣ�����ֹͣ");
	//			m_pLog->log(str);
	//			return;
	//		}
	//		EndTime = GetTickCount();
	//		str.Format("ScanInspect:%ld",EndTime-StartTime);
	//		AddMsg(str);
	//	
	//	}
	//	else if(ProductParam.inspectMode==SESSION_INDPECT)
	//	{
	//		nTempLackStatic=0;//ȱ�������֮ǰ����ʼ��ȱ������Ŀ��
	//		MoveToPadDetectPos();  //////////////////////////////////////////////////////////////////////////
	//		//m_pMv->SetOutput(theApp.m_tSysParam.outRotatingLight,TRUE);
	//					
	//		str.Format("ȱ�������ֶμ�⡿��ʼ!");
	//		m_pLog->log(str);
	//		
	//		llDueDalay = CTimeUtil::GetDueTime(1000);
	//		while(!CTimeUtil::IsTimeout(llDueDalay))
	//		{
	//			if(IsRotatingLightOpen())
	//			{
	//				break;
	//			}
	//			Sleep(100);
	//		}

	//		//if(!IsRotatingLightOpen())
	//		//{
	//		//	m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
	//		//	Alarm(35);
	//		//	str.Format("��ת���״�ʧ�ܣ���е��׼�����źű������");
	//		//	m_pLog->log(str);
	//		//	return;
	//		//}
	//		Sleep(500);
	//		StartTime = EndTime = GetTickCount();
	//		if(!SpliceInspect(true))
	//		{
	//			//m_pMv->SetOutput(theApp.m_tSysParam.outRotatingLight,FALSE);
	//			//str.Format("�ֶμ��ִ����ϣ�׼���ر���ת���ף�");
	//			//m_pLog->log(str);

	//			llDueDalay = CTimeUtil::GetDueTime(1000); //5000->1000  2016-05-27
	//			while(!CTimeUtil::IsTimeout(llDueDalay))
	//			{
	//				if(IsRotatingLightClose())
	//				{
	//					break;
	//				}
	//				Sleep(100);
	//			}
	//		/*	if(!IsRotatingLightClose())
	//			{
	//				m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
	//				Alarm(35);
	//				str.Format("��ת���׹ر�ʧ�ܣ���е��׼�����źŶ�ʧ��");
	//				m_pLog->log(str);
	//				return;
	//			 }*/

	//			m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
	//			Alarm(22);
	//			str.Format("�ֶμ��ʧ�ܣ���е��׼�����źű����������ֹͣ��");
	//			AddMsg(str);//2016-03-14
	//			m_pLog->log(str);
	//			return;
	//		}
	//		EndTime = GetTickCount();
	//		str.Format("SpliceInspect:%ld",EndTime-StartTime);
	//		AddMsg(str);

	//		//m_pMv->SetOutput(theApp.m_tSysParam.outRotatingLight,FALSE);
	//		//str.Format("�ֶμ��ɹ�����ʼ�ر���ת���ף�");
	//		//m_pLog->log(str);

	//	/*	llDueDalay = CTimeUtil::GetDueTime(5000);
	//		while(!CTimeUtil::IsTimeout(llDueDalay))
	//		{
	//			if(IsRotatingLightClose())
	//			{
	//				break;
	//			}
	//			Sleep(100);
	//		}*/
	//		/*if(!IsRotatingLightClose())
	//		{
	//			m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
	//			Alarm(35);
	//			str.Format("�ֶμ��ɹ�����ת���׹ر�ʧ�ܣ�");
	//			m_pLog->log(str);
	//			return;
	//		}*/
	//	}
	//}
	////////////////////////////////////////////////////////////////////////
	//AddMsg("Start CopyPadDtect Result!");
	//CopyPadDtect();
	//AddMsg("End CopyPadDtect Result!");

	//��ʼ��ͼ��
	if((ProductParam.visionMode==VISION_TWO_MARK)||(ProductParam.visionMode==VISION_ONE_MARK))
	{
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,TRUE);
		StartTime = EndTime = GetTickCount();
		////////////////////////////////////////////////////////////////////////////////
		bool bFindResult = FindImageOffset(m_tOffset.x,m_tOffset.y,m_dRotateAngle,false);
		/////////////////////////////////////////////////////////////////////////////
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,FALSE);
		if(!bFindResult)
		{
			m_tOffset.x = 0.0;
			m_tOffset.y = 0.0;
			m_dRotateAngle = 0.0;
			AddMsg("û���ҵ�ͼ��");
			m_bIsSupend = true;
			if(AfxMessageBox("ͼ�����ʧ�ܣ��Ƿ������",MB_YESNO)!=IDYES)
			{
				if(AfxMessageBox("������Ƭ֧�ܣ�",MB_YESNO)!=IDYES)
				{
					m_bIsSupend = false;
					m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
					str.Format("ͼ�����ʧ��!");
					PutLogIntoList(str);
					Alarm(17);
					str.Format("ͼ�����ʧ�ܣ�����ֹͣ");
					PutLogIntoList(str);
					return;
				}
				m_bIsSupend = false;
				if(!MoveToZSafety())
				{
					return;
				}
				m_pMv->SetOutput(theApp.m_tSysParam.outDispenseFinish,TRUE);
				Sleep(200);
				m_pMv->SetOutput(theApp.m_tSysParam.outDispenseFinish,FALSE);
				AddMsg("�㽺����,�ȴ��㽺�ź�!");
				AutoRunCp=false;
				m_tStatus = K_RUN_STS_STOP;
				return;
			}
			m_bIsSupend = false;
		}
		EndTime = GetTickCount();
		str.Format("FindMark:time = %ld,angle = %0.3f",EndTime-StartTime,m_dRotateAngle);
		AddMsg(str);
		if(fabs(m_dRotateAngle)>ProductParam.MaxOffsetAngle)
		{
			m_bIsSupend = true;
			if(AfxMessageBox("ƫ�ƽǶȳ�������ĽǶȣ��Ƿ�������У�",MB_YESNO)!=IDYES)
			{
				m_dRotateAngle = 0.0;
				m_bIsSupend = false;
				m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
				str.Format("ͼ�����ʧ��!");
				PutLogIntoList(str);
				Alarm(17);
				str.Format("�Ƕ�ƫ�Ƴ������ƣ�����ֹͣ");
				PutLogIntoList(str);
				return;
			}
			m_bIsSupend = false;
		}
		if(fabs(m_tOffset.x)>ProductParam.MaxOffsetX||fabs(m_tOffset.y)>ProductParam.MaxOffsetY)
		{
			m_bIsSupend = true;
			if(AfxMessageBox("λ��ƫ�Ƴ����趨������ֵ���Ƿ�������У�",MB_YESNO)!=IDYES)
			{
				m_tOffset.x = 0.0;
				m_tOffset.y = 0.0;
				m_bIsSupend = false;
				m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
				str.Format("ͼ�����ʧ��!");
				PutLogIntoList(str);
				Alarm(17);
				str.Format("λ��ƫ�Ƴ������ƣ�����ֹͣ");
				PutLogIntoList(str);
				return;
			}
			m_bIsSupend = false;
		}
	}
	if(ProductParam.MeasureHeightEnable)
	{
		StartTime = EndTime = GetTickCount();
		HEIGHT_STATUS ret = MeasureHeight(m_tOffset.za);
		EndTime = GetTickCount();
		str.Format("MeasureHeight:time = %ld,�߶�ֵ = %0.3f",EndTime-StartTime,m_tOffset.za);
		AddMsg(str);
		if(HEIGHT_STATUS_OK != ret)
		{
			if(HEIGHT_STATUS_EXCEED_LIMIT == ret)
			{
				m_bIsSupend = true;
				if(AfxMessageBox("�߶�ƫ�Ƴ����趨������ֵ���Ƿ�������У�",MB_YESNO)!=IDYES)
				{
					m_tOffset.za = 0.0;
					m_bIsSupend = false;
					m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
					Alarm(21);
					str.Format("�߶�ƫ�Ƴ������ƣ�����ֹͣ");
					PutLogIntoList(str);
					return;
				}
				m_bIsSupend = false;
			}
			else
			{
				m_tOffset.za = 0.0;
				m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
				Alarm(21);
				str.Format("��ߴ�������ֹͣ");
				PutLogIntoList(str);
				return;
			}
		}
	}
	//if(!ProductParam.DispMode)//����ģʽ
	if(true)//2019-04-29
	{
		if(!CleanAndTest(false))
		{
			m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
			str.Format("�׵���ϴ���Խ�ʧ�ܣ�");
			PutLogIntoList(str);
			AfxMessageBox(str);
			return;
		}
	}
	//2016-11-17 ȡ���Զ���������
	//if(ProductParam.AutoAdjust)
	//{
	//	if(!AutoAdjustNeedle())
	//	{
	//		m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
	//		AfxMessageBox("������ͷʧ�ܣ�");
	//		return;
	//	}
	//}
	if (ProductParam.nCheckIsCompensate == 1)
	{
		m_tOffset.x = 0;
		m_tOffset.y = 0;
	}

	if(!ExtandTrack(m_tOffset.x,m_tOffset.y,m_tOffset.za,m_dRotateAngle))
	{
		Alarm(36);
		str.Format("�켣�������");
		PutLogIntoList(str);
		return;
	}

	if (TRANSITION_INSPECT==ProductParam.inspectMode)
	{
		//ȱ������Ŀ�жϣ�
		int nTempLackStatic=0;
		int nCount = g_pFrm->m_CmdRun.ProductParam.FirstDispRow*g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*g_pFrm->m_CmdRun.ProductParam.SecondDispRow*
			g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*g_pFrm->m_CmdRun.ProductParam.ThirdDispRow*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn;
		for (int i=0;i<nCount;i++)
		{
			if (NULL!=PDposting)
			{
				if(!PDposting[i].bIsHasPad)
				{
					nTempLackStatic++;
				}
			}
		}

		if ((nTempLackStatic>=0)&&(nTempLackStatic>g_pFrm->m_CmdRun.ProductParam.nLackPadCmpValue)&&(g_pFrm->m_CmdRun.ProductParam.nLackPadCmpValue>=0))
		{
			str.Format("ȱ������ĿΪ:%d-�����趨ֵ��%d...�Ƿ������",nTempLackStatic,g_pFrm->m_CmdRun.ProductParam.nLackPadCmpValue);//��ʾȱ������Ŀ�ڽ��棻
			AddMsg(str);
			g_pFrm->m_CmdRun.PutLogIntoList(str); //��¼����ȱ������Ŀ��Log��־��

			m_bIsSupend=true;//2016-03-13
			if(AfxMessageBox(str,MB_YESNO)!=IDYES)
			{
				str.Format("ȱ������ⳬ���趨��Ŀ,����ֹͣ");
				AddMsg(str);
				PutLogIntoList(str);
				m_bIsSupend=false;//2016-03-13
				m_tStatus = K_RUN_STS_STOP;
				return;
			}
			m_bIsSupend=false;//2016-03-13
		}
	}

	if(!SetAxisProfile(HIGH_VEL))
	{
		Alarm(30);
		m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
		str.Format("��ʼ�㽺�������������ٶ�ʧ�ܣ�");
		PutLogIntoList(str);
		AfxMessageBox(str);
		return;
	}
	DispenseOperation();
	pcsDoneFromStart++;
	if(!SetAxisProfile(MEDIUM_VEL))
	{
		Alarm(30);
		m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
		str.Format("�����㽺�������������ٶ�ʧ�ܣ���");
		PutLogIntoList(str);
		AfxMessageBox(str);
		return;
	}
	//Z��ذ�ȫλ��
	if(!MoveToZSafety())
	{
		return;
	}
	if(IsRunStop())
	{
		return;
	}
	if(ProductParam.DispFinishFill||V9DotRest)
	{
		//DischargePosition(false);
		RunThreadValveReset();
	}

	if((1==ProductParam.NeedleHeightCheck)&&(pcsDoneFromStart%ProductParam.ContactFrequency==0)) //��ͷ�߶�У��
	{	
		if(!CleanNeedle(true))
		{
			str.Format("��ͷ�߶�У��ǰ��ϴʧ��!");
			PutLogIntoList(str);
			AddMsg(str);
			return;
		}
		if(!NeedleHeightCheck())
		{
			str.Format("��ͷ�߶�У��ʧ�ܣ�");
			PutLogIntoList(str);
			AddMsg(str);
			return;
		}
	}
	m_pMv->SetOutput(theApp.m_tSysParam.outDispenseFinish,TRUE);
	Sleep(200);
	m_pMv->SetOutput(theApp.m_tSysParam.outDispenseFinish,FALSE);
	str.Format("�㽺����,�ȴ��㽺�ź�!");
	AddMsg(str);
	PutLogIntoList(str);
	
	////UPH ���� 2018-04-16
	W_EndTime=GetTickCount();
	double dTotalNum=ProductParam.FirstDispRow*ProductParam.SecondDispRow*ProductParam.ThirdDispRow*ProductParam.FirstDispColumn*ProductParam.SecondDispColumn*ProductParam.ThirdDispColumn;
	StautDisp.W_SingeTimer=(W_EndTime-W_Startime)/1000.00;
	if (dTotalNum>0)
	{
		StautDisp.W_SingeTimer=StautDisp.W_SingeTimer/dTotalNum;
	}
	if ((StautDisp.W_SingeTimer>0.05))
	{
		StautDisp.W_UPH=3600/StautDisp.W_SingeTimer;
	}

	AutoRunCp=false;
	m_tStatus = K_RUN_STS_STOP;
}
void CCmdRun::TestRun()
{
	CString str;
	DWORD StartTime,EndTime;
	DisCompCount=0;
	m_tOffset.x = 0.0;
	m_tOffset.y = 0.0;
	m_tOffset.za = 0.0;
	m_dRotateAngle = 0.0;
	m_bFirstDisp = true;
	m_bStartRun = true;
	if(m_bIsPause)
	{
		m_tStatus = K_RUN_STS_PAUSE;
	}
	else
	{
		m_tStatus = K_RUN_STS_RUN;
	}
	str.Format("���Ե㽺��ʼ����");
	PutLogIntoList(str);
	if(g_pFrm->m_wndPosInfo.TimeStarC)//�Զ�����
	{
		g_pFrm->m_wndPosInfo.TimeStarC=false;
		RunErrorID=-1;
	}
	if (m_tStatus == K_RUN_STS_PAUSE)
	{
		if(!WaitPasueEnd())
		{
			m_bTestRunStop = true;
			return;
		}
	}
	if(IsRunStop())
	{
		m_bTestRunStop = true;
		return;
	}
	if( !ProductParam.DispMode)//����ģʽ
	{
		str.Format("V9Reset:%d",(int)V9RestOK);
		AddMsg(str);
		if (!V9RestOK)//
		{
			if(!RestDisP())
			{
				m_bTestRunStop = true;
				m_tStatus = K_RUN_STS_STOP;
				return;
			}
		}
		StartTime = EndTime = GetTickCount();
		while (!V9RestOK)//���帴λ�Ƿ�ɹ�
		{
			CFunction::DoEvents();
			EndTime= GetTickCount();
			if ((EndTime-StartTime)/1000>8)//4S
			{
				Alarm(10);
				str.Format("���帴λʧ��!");
				AddMsg(str);
				PutLogIntoList(str);
				m_bTestRunStop = true;
				return;
			}
			Sleep(350);
		}
		g_pFrm->m_CmdRun.ProductParam.DispFCount=0;

		if(!ManalRun)
		{
			AutoRunCp=true;

			for (int i=0;i<3;i++)
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					g_pFrm->m_CmdRun.RunDispFSelect[i]=true;
					g_pFrm->m_CmdRun.ProductParam.DispFCount++;
				}
				else
				{
					g_pFrm->m_CmdRun.RunDispFSelect[i]=false;
				}
			}
		}
		else
		{
			for (int i=0;i<3;i++)
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					g_pFrm->m_CmdRun.RunDispFSelect[i]=true;
					g_pFrm->m_CmdRun.ProductParam.DispFCount++;
					AutoRunCp=false;
				}
				else
				{
					g_pFrm->m_CmdRun.RunDispFSelect[i]=false;
				}
			}
		}
	}
	LONGLONG llDueDalay = CTimeUtil::GetDueTime(30000);
	while(!CTimeUtil::IsTimeout(llDueDalay))
	{
		if(!m_bValveReset)
		{
			break;
		}
		Sleep(1);
	}
	if(m_bValveReset)
	{
		Alarm(29);
		AfxMessageBox("���帴λʧ�ܣ�");
		return;
	}
	if(ProductParam.SlotDetect)
	{
		StartTime = EndTime = GetTickCount();
		if(!SlotDetection())
		{
			m_bIsSupend = true;
			if(AfxMessageBox("��ȷ��֧�������棡�Ƿ������",MB_YESNO)!=IDYES)
			{
				m_bTestRunStop = true;
				m_bIsSupend = false;
				Alarm(18);
				return;
			}
			m_bIsSupend = false;
		}
		EndTime = GetTickCount();
		str.Format("SlotDetection:%ld",EndTime-StartTime);
		AddMsg(str);
	}

	if(ProductParam.visionMode==VISION_TWO_MARK)
	{
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,TRUE);
		StartTime = EndTime = GetTickCount();
		//if(!FindTwoMark())
		if(!FindImageOffset(m_tOffset.x,m_tOffset.y,m_dRotateAngle,false))
		{
			m_tOffset.x = 0.0;
			m_tOffset.y = 0.0;
			m_dRotateAngle = 0.0;
			AddMsg("û���ҵ�ͼ��");
			m_bIsSupend = true;
			theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,FALSE);
			if(AfxMessageBox("ͼ�����ʧ�ܣ��Ƿ������",MB_YESNO)!=IDYES)
			{
				m_bIsSupend = false;
				str.Format("ͼ�����ʧ��!");
				PutLogIntoList(str);
				Alarm(17);
				m_bTestRunStop = true;
				return;
			}
			m_bIsSupend = false;
		}
		EndTime = GetTickCount();
		str.Format("FindMark:time = %ld,angle = %0.3f",EndTime-StartTime,m_dRotateAngle);
		AddMsg(str);
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,FALSE);
		if(fabs(m_dRotateAngle)>ProductParam.MaxOffsetAngle)
		{
			m_bIsSupend = true;
			if(AfxMessageBox("ƫ�ƽǶȳ�������ĽǶȣ��Ƿ�������У�",MB_YESNO)!=IDYES)
			{
				m_bIsSupend = false;
				str.Format("ͼ�����ʧ��!");
				PutLogIntoList(str);
				Alarm(17);
				m_bTestRunStop = true;
				return;
			}
			m_bIsSupend = false;
		}
		if(fabs(m_tOffset.x)>2||fabs(m_tOffset.y)>2)
		{
			m_bIsSupend = true;
			if(AfxMessageBox("λ��ƫ�Ƴ���2mm���Ƿ�������У�",MB_YESNO)!=IDYES)
			{
				m_tOffset.x = 0.0;
				m_tOffset.y = 0.0;
				m_dRotateAngle = 0.0;
				m_bIsSupend = false;
				str.Format("λ��ƫ�Ƴ���2mm");
				PutLogIntoList(str);
				Alarm(17);
				return;
			}
			if(AfxMessageBox("�Ƿ����λ��ƫ�ƣ�",MB_YESNO)!=IDYES)
			{
				m_tOffset.x = 0.0;
				m_tOffset.y = 0.0;
				m_dRotateAngle = 0.0;
			}
			m_bIsSupend = false;
		}
	}
	if(ProductParam.MeasureHeightEnable)
	{
		StartTime = EndTime = GetTickCount();
		HEIGHT_STATUS ret = MeasureHeight(m_tOffset.za);
		EndTime = GetTickCount();
		str.Format("MeasureHeight:time = %ld,�߶�ֵ = %0.3f",EndTime-StartTime,m_tOffset.za);
		AddMsg(str);
		if(HEIGHT_STATUS_OK != ret)
		{
			if(HEIGHT_STATUS_EXCEED_LIMIT == ret)
			{
				m_bIsSupend = true;
				if(AfxMessageBox("�߶�ƫ�Ƴ����趨������ֵ���Ƿ�������У�",MB_YESNO)!=IDYES)
				{
					m_tOffset.za = 0.0;
					m_bIsSupend = false;
					m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
					Alarm(21);
					str.Format("�߶�ƫ�Ƴ������ƣ�����ֹͣ");
					PutLogIntoList(str);
					return;
				}
			}
			else
			{
				m_tOffset.za = 0.0;
				m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
				Alarm(21);
				str.Format("��ߴ�������ֹͣ");
				PutLogIntoList(str);
				return;
			}
		}
	}
	if(!ProductParam.DispMode)//����ģʽ
	{
		if(!CleanAndTest(false))
		{
			AfxMessageBox("�׵���ϴ���Խ�ʧ�ܣ�");
			m_bTestRunStop = true;
			return;
		}
	}
	if(!ExtandTrack(m_tOffset.x,m_tOffset.y,m_tOffset.za,m_dRotateAngle))
	{
		Alarm(36);
		str.Format("�켣�������");
		PutLogIntoList(str);
		return;
	}
	if(!MoveToZSafety())
	{
		AfxMessageBox("�ƶ�����ȫλ��ʧ��!");
		return;
	}
	if(!SetAxisProfile(HIGH_VEL))
	{
		AfxMessageBox("�������п�ʼ�������������ٶ�ʧ�ܣ�");
		m_bTestRunStop = true;
		return;
	}
	DispenseOperation();
	if(!SetAxisProfile(MEDIUM_VEL))
	{
		AfxMessageBox("�������н����������������ٶ�ʧ�ܣ�");
		m_bTestRunStop = true;
		return;
	}
	//Z��ذ�ȫλ��
	if(!MoveToZSafety())
	{
		m_bTestRunStop = true;
		return;
	}
	if(IsRunStop())
	{
		m_bTestRunStop = true;
		return;
	}
	if(ProductParam.DispFinishFill||V9DotRest)
	{
		DischargePosition(false);
		RunThreadValveReset();
	}
	str.Format("���Ե㽺����,�ȴ��㽺�ź�!");
	AddMsg(str);
	PutLogIntoList(str);
	AutoRunCp=false;
	m_tStatus = K_RUN_STS_STOP;
}

void CCmdRun::ValveReset()
{
	m_bValveReset = true;
	int nReturnValue = theApp.m_V9Control.V9C_Rest(K_ALL_AXIS);
	m_bValveReset = false;
	if(nReturnValue>0)
	{
		m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
		Alarm(nReturnValue+5);
		AfxMessageBox("���帴λʧ�ܣ�");
		return;
	}
	if(nReturnValue<0)
	{
		m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
		Alarm(-nReturnValue+1);
		AfxMessageBox("���帴λʧ�ܣ�");
		return;
	}
	if(!theApp.m_V9Control.V9C_SetProfile())
	{
		Alarm(30);
		AfxMessageBox("�������ò�������");
		return;
	}
	V9DotRest = FALSE;
}
// ��λ
void CCmdRun::Home()
{
	CString str;
	if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_EMERGENCY)
	{
		AfxMessageBox(_T("��ͣ״̬���ܻ�ԭ��!"));
		return;
	}
	if(!g_pFrm->m_CmdRun.ManalRun)
	{
		AfxMessageBox(_T("�Զ�ģʽ�����ֶ���ԭ��!"));
		return;
	}
	//if(g_pFrm->bIsAlignDlgVisible||g_pFrm->bIsCalibVisible||g_pFrm->bIsCustomVisible||
	//	g_pFrm->bIsDebugHeightVisible||g_pFrm->bIsDebugNeedleVisible||g_pFrm->bIsRCDDlgVisible)
	//{
	//	AfxMessageBox("��رնԻ�����������λ��");
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
	if(m_bCleanNeedle||m_bDischarge||m_bTestDispense||m_bSearchMark||m_bSlotDetect||m_bHoming||m_bMeasureHeight||m_bNeedleCalibrate||m_bNeedleAutoAdjust)
	{
		CString strError;
		strError.Format("δ֪����");
		if(m_bCleanNeedle)
		{
			strError.Format("��ϴ��ͷʱ�����ܻ�ԭ�㣡");
		}
		else if(m_bDischarge)
		{
			strError.Format("�Ž�ʱ�����ܻ�ԭ�㣡");
		}
		else if(m_bTestDispense)
		{
			strError.Format("�Խ�ʱ�����ܻ�ԭ�㣡");
		}
		else if(m_bSearchMark)
		{
			strError.Format("����Mark��ʱ�����ܻ�ԭ�㣡");
		}
		else if(m_bSlotDetect)
		{
			strError.Format("�������ʱ�����ܻ�ԭ�㣡");
		}
		else if(m_bHoming)
		{
			strError.Format("��λʱ�����ܻ�ԭ�㣡");
		}
		else if(m_bMeasureHeight)
		{
			strError.Format("���ʱ�����ܻ�ԭ�㣡");
		}
		else if (m_bNeedleCalibrate)
		{
			strError.Format("��ͷ�궨ʱ�����ܻ�ԭ�㣡");
		}
		else if (m_bNeedleAutoAdjust)
		{
			strError.Format("�Զ�����ʱ�����ܻ�ԭ�㣡");
		}
		PutLogIntoList(strError);
		return;
	}
	static bool b = false;
	if(b) return;
	b = true;//�ڻ����в���Ӧ����

	if((g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN) || (g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_PAUSE)||(g_pFrm->m_CmdRun.bIsThreadRunning))
	{
		AfxMessageBox("���л���ͣ״̬���ܻ�ԭ��!");
		b = false;
		return;
	}

	//if (g_pFrm->m_wndPosInfo.PlcEorrTag)
	//{
	//	m_pMv->SetOutput(theApp.m_tSysParam.outPlcRest, true);
	//	Sleep(200);
	//	m_pMv->SetOutput(theApp.m_tSysParam.outPlcRest, false);
	//	g_pFrm->m_wndPosInfo.PlcEorrTag=false;
	//}
	m_pMv->SetOutput(theApp.m_tSysParam.outRed, false);
	m_pMv->SetOutput(theApp.m_tSysParam.outGreen ,true);
	m_pMv->SetOutput(theApp.m_tSysParam.outYellow, false);

	m_bHoming = true;
	if (g_pFrm->m_CmdRun.RunErrorID==0)//�ŷ�����
	{
		for (int i=1;i<4;i++)
		{
			m_pMv->AxisClrsts(i); 
		}
		Sleep(500);
		for(int i=1;i<4;i++)
		{
			long lAxisSts = 0;
			theApp.m_Mv400.GetAxisSts(i,&lAxisSts);
			if((lAxisSts&0x02)==0x02)
			{
				b = false;
				m_bHoming = false;
				AfxMessageBox("�ŷ�����δ�������ϵ�����������!");
				return;
			}
		}
		Sleep(500);
	}
	g_pFrm->m_CmdRun.RunErrorID=-1;
	g_pFrm->m_CmdRun.m_tStatus = K_RUN_STS_NONE;
	//m_pMv->SetOutput(theApp.m_tSysParam.outDispenseFinish,TRUE);
	//g_pFrm->m_CmdRun.m_tStatus = K_RUN_STS_NONE;
	//Sleep(200);
	//m_pMv->SetOutput(theApp.m_tSysParam.outDispenseFinish,FALSE);//���� PLC

	theApp.m_tSysParam.ThreeAxHomeSu=false;
	if(!(m_pMv->Home(50,5,1000)))
	{
		b = false;
		theApp.m_tSysParam.ThreeAxHomeSu=false;
		m_bHoming = false;
		AfxMessageBox("��ԭ��ʧ�������»���!");
		return;
	}
	if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
	{
		b = false;
		m_bHoming = false;
		AfxMessageBox("����������ó���");
		return;
	}
	m_bHoming = false;

	if (g_pFrm->m_wndPosInfo.PlcEorrTag)
	{
		g_pFrm->m_wndPosInfo.PlcEorrTag=false;
		g_pFrm->m_wndPosInfo.bRecordError = false;
		g_pFrm->m_CmdRun.m_bPlcRunError = false;
	}

	g_pFrm->m_CmdRun.m_tStatus = K_RUN_STS_STOP;
	theApp.m_tSysParam.ThreeAxHomeSu=true;
	g_pFrm->m_CmdRun.RunErrorID=-1;
	g_pFrm->m_wndPosInfo.PlcRTag=false;
	g_pFrm->m_wndPosInfo.ErrorVal=0;
	g_pFrm->m_wndPosInfo.ErrorPlcData=0;
	g_pFrm->m_wndPosInfo.bRecordError = false;
	b = false;
	if(theApp.m_tSysParam.V9HomeSu&&theApp.m_tSysParam.ThreeAxHomeSu)
	{
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outReady,TRUE);
	}
	else
	{
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outReady,FALSE);
		str.Format("ThreeAxHomeSu:%d,V9HomeSu:%d",theApp.m_tSysParam.ThreeAxHomeSu,theApp.m_tSysParam.V9HomeSu);
		AddMsg(str);
	}
}

bool CCmdRun::FindTwoCenter()
{
	CString str;
	double dX=0.0,dY=0.0;
	int nBmpTestCount=0;
	double dBmpPosOff=0.0;
	double dOffsetX1=0.0,dOffsetY1=0.0,dOffsetX2=0.0,dOffsetY2=0.0;
	if(!MoveToZSafety())
	{
		return false;
	}
	if(IsRunStop())
	{
		return false;
	}
	tgPos tgStartPos = GetFirstCameraPos();
	tgPos tgEndPos = GetLastCameraPos();

	if(!SynchronizeMoveXY(tgEndPos.x,tgEndPos.y,true))
	{
		return false;
	}
	if(IsRunStop())
	{
		return false;
	}
	if(!SynchronizeMoveZ(tgEndPos.za,true))
	{
		return false;
	}

	Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
	if(!g_pView->m_ImgStatic.CopyImage(RGB8))
	{
		return false;
	}
	if(IsRunStop())
	{
		return false;
	}
	while(!g_pView->m_ImgStatic.FindBlobCenter(theApp.m_tSysParam.BmpROI[1],dX,dY))
	{
		nBmpTestCount++;
		dBmpPosOff = 3*nBmpTestCount;
		if(!SynchronizeMoveXY(tgEndPos.x-dBmpPosOff,tgEndPos.y,true))
		{
			return false;
		}
		Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
		if(!g_pView->m_ImgStatic.CopyImage(RGB8))
		{
			return false;
		}
		if(nBmpTestCount>3)
		{
			AddMsg("ͼ��λ��2����쳣");
			return false;
		}
		if(IsRunStop())
		{
			return false;
		}
	}
	g_pView->m_ImgStatic.m_dDisplayCenterX[3] = dX;
	g_pView->m_ImgStatic.m_dDisplayCenterY[3] = dY;
	dOffsetX2 = (dX-theApp.m_tSysParam.BmpCenterX[1])*theApp.m_tSysParam.BmpScale-dBmpPosOff;
	dOffsetY2 = (dY-theApp.m_tSysParam.BmpCenterY[1])*theApp.m_tSysParam.BmpScale;
	str.Format("ȡͼ %d�Σ����½�ͼ��ƫ��x=  %0.3f y= %0.3f",nBmpTestCount+1,dOffsetX2,dOffsetY2);
	AddMsg(str);

	nBmpTestCount = 0;
	dBmpPosOff = 0.0;
	if(!MoveToZSafety())
	{
		return false;
	}
	if(IsRunStop())
	{
		return false;
	}
	if(!SynchronizeMoveXY(tgStartPos.x,tgStartPos.y,true))
	{
		return false;
	}
	if(IsRunStop())
	{
		return false;
	}
	if(!SynchronizeMoveZ(tgStartPos.za,true))
	{
		return false;
	}
	if(IsRunStop())
	{
		return false;
	}
	Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
	if(!g_pView->m_ImgStatic.CopyImage(RGB8))
	{
		return false;
	}
	while(!g_pView->m_ImgStatic.FindBlobCenter(theApp.m_tSysParam.BmpROI[0],dX,dY))
	{
		nBmpTestCount++;
		dBmpPosOff = 3*nBmpTestCount;
		if(!SynchronizeMoveXY(tgStartPos.x-dBmpPosOff,tgStartPos.y,true))
		{
			return false;
		}
		Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
		if(!g_pView->m_ImgStatic.CopyImage(RGB8))
		{
			return false;
		}
		if(nBmpTestCount>3)
		{
			AddMsg("ͼ��λ��1����쳣");
			return false;
		}
		if(IsRunStop())
		{
			return false;
		}
	}
	g_pView->m_ImgStatic.m_dDisplayCenterX[2] = dX;
	g_pView->m_ImgStatic.m_dDisplayCenterY[2] = dY;
	dOffsetX1 = (dX-theApp.m_tSysParam.BmpCenterX[0])*theApp.m_tSysParam.BmpScale-dBmpPosOff;
	dOffsetY1 = (dY-theApp.m_tSysParam.BmpCenterY[0])*theApp.m_tSysParam.BmpScale;
	str.Format("ȡͼ %d�Σ����Ͻ�ͼ��ƫ��y=  %0.3f y= %0.3f",nBmpTestCount+1,dOffsetX1,dOffsetY1);
	AddMsg(str);

	m_tOffset.x = (dOffsetX1+dOffsetX2)/2;
	m_tOffset.y = (dOffsetY1+dOffsetY2)/2;
	m_dRotateAngle = CalculateAngle((tgEndPos.x+dOffsetX2)-(tgStartPos.x+dOffsetX1),(tgEndPos.y-dOffsetY2)-(tgStartPos.y-dOffsetY1))-
		CalculateAngle(tgEndPos.x-tgStartPos.x,tgEndPos.y-tgStartPos.y);
	if(abs(m_tOffset.x)>2)
	{
		m_tOffset.x = 0;
	}
	if(abs(m_tOffset.y)>2)
	{
		m_tOffset.y = 0;
	}
	str.Format("ͼ��ƫ��OffsetX=  %0.3f,OffsetY= %0.3f,RotateAngle = %0.3f",m_tOffset.x,m_tOffset.y,m_dRotateAngle);
	AddMsg(str);
	return true;
}

bool CCmdRun::FindTwoMark()
{
	CString strError;
	if(m_bCleanNeedle||m_bDischarge||m_bTestDispense||m_bSearchMark||m_bSlotDetect||m_bHoming||m_bMeasureHeight||m_bNeedleCalibrate||m_bNeedleAutoAdjust)
	{
		strError.Format("δ֪����");
		if(m_bCleanNeedle)
		{
			strError.Format("��ϴ��ͷʱ�����ܲ���Mark�㣡");
		}
		else if(m_bDischarge)
		{
			strError.Format("�Ž�ʱ�����ܲ���Mark��");
		}
		else if(m_bTestDispense)
		{
			strError.Format("�Խ�ʱ�����ܲ���Mark��");
		}
		else if(m_bSearchMark)
		{
			strError.Format("����Mark��ʱ�����ܲ���Mark��");
		}
		else if(m_bSlotDetect)
		{
			strError.Format("�������ʱ�����ܲ���Mark��");
		}
		else if(m_bHoming)
		{
			strError.Format("��λʱ�����ܲ���Mark��");
		}
		else if(m_bMeasureHeight)
		{
			strError.Format("���ʱ�����ܲ���Mark�㣡");
		}
		else if (m_bNeedleCalibrate)
		{
			strError.Format("��ͷ�궨ʱ�����ܲ���Mark�㣡");
		}
		else if (m_bNeedleAutoAdjust)
		{
			strError.Format("�Զ�����ʱ�����ܲ���Mark�㣡");
		}
		PutLogIntoList(strError);
		return false;
	}
	strError.Format("��ʼ����Mark��");
	PutLogIntoList(strError);
	m_bSearchMark = true;
	CString strFile,strText;
	int nIndex;
	double dCenterX[2];
	double dCenterY[2];
	double dAngle[2];
	double dOffsetX1,dOffsetY1,dOffsetX2,dOffsetY2;
	double dBmpOffset = 0.5;
	tgPos tgEndBmpOffset,tgStartBmpOffset;
	bool bMamualAlign = false;
	tgEndBmpOffset.x = 0.0;
	tgEndBmpOffset.y = 0.0;
	tgStartBmpOffset.x = 0.0;
	tgStartBmpOffset.y = 0.0;
	dOffsetX1 = 0.0;
	dOffsetY1 = 0.0;
	dOffsetX2 = 0.0;
	dOffsetY2 = 0.0;
	if(!MoveToZSafety())
	{
		m_bSearchMark = false;
		strError.Format("Mark�����ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	if(IsRunStop())
	{
		m_bSearchMark = false;
		strError.Format("Mark�����ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	tgPos tgStartPos = ProductParam.tgImagePos[0];
	tgPos tgEndPos = ProductParam.tgImagePos[1];

	if(!SynchronizeMoveXY(tgEndPos.x,tgEndPos.y,true))
	{
		m_bSearchMark = false;
		strError.Format("Mark�����ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	if(IsRunStop())
	{
		m_bSearchMark = false;
		strError.Format("Mark�����ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	if(!SynchronizeMoveZ(tgEndPos.za,true))
	{
		m_bSearchMark = false;
		strError.Format("Mark�����ʧ��");
		PutLogIntoList(strError);
		return false;
	}

	Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
	if(!g_pView->m_ImgStatic.CopyImage(RGB8))
	{
		m_bSearchMark = false;
		strError.Format("Mark�����ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	nIndex = 1;
	strFile = g_pDoc->GetModeFile(nIndex);
	//milApp.SetMatchMode(ProductParam.matchMode);
	milApp.RestoreModel(strFile.GetBuffer(strFile.GetLength()));
	milApp.SetPatParam(theApp.m_tSysParam.BmpMarkAcceptance,theApp.m_tSysParam.BmpMarkAngle);
	milApp.SetModelWindow(theApp.m_tSysParam.BmpMarkLearnWin[1]);
	milApp.SetSearchWindow(theApp.m_tSysParam.BmpMarkROI);
	//if(!milApp.FindModel())
	//{
	//	AddMsg("ͼ��λ��2����쳣");
	//	return false;
	//}
	while(!milApp.FindModel(true))
	{
		if(IsRunStop())
		{
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		if(!SynchronizeMoveXY(tgEndPos.x-dBmpOffset,tgEndPos.y,true))
		{
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
		if(!g_pView->m_ImgStatic.CopyImage(RGB8))
		{
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		if(milApp.FindModel(true))
		{
			tgEndBmpOffset.x = -dBmpOffset;
			tgEndBmpOffset.y = 0.0;
			break;
		}
		if(IsRunStop())
		{
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		if(!SynchronizeMoveXY(tgEndPos.x,tgEndPos.y-dBmpOffset,true))
		{
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
		if(!g_pView->m_ImgStatic.CopyImage(RGB8))
		{
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		if(milApp.FindModel(true))
		{
			tgEndBmpOffset.x = 0.0;
			tgEndBmpOffset.y = -dBmpOffset;
			break;
		}
		if(IsRunStop())
		{
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		if(!SynchronizeMoveXY(tgEndPos.x+dBmpOffset,tgEndPos.y,true))
		{
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
		if(!g_pView->m_ImgStatic.CopyImage(RGB8))
		{
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		if(milApp.FindModel(true))
		{
			tgEndBmpOffset.x = dBmpOffset;
			tgEndBmpOffset.y = 0.0;
			break;
		}
		if(IsRunStop())
		{
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		if(!SynchronizeMoveXY(tgEndPos.x+dBmpOffset,tgEndPos.y,true))
		{
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
		if(!g_pView->m_ImgStatic.CopyImage(RGB8))
		{
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		if(milApp.FindModel(true))
		{
			tgEndBmpOffset.x = 0.0;
			tgEndBmpOffset.y = dBmpOffset;
			break;
		}
		AddMsg("ͼ��λ��2����쳣");
		m_bIsSupend = true;
		if(AfxMessageBox("�ֶ�����ͼ��",MB_YESNO)!=IDYES)
		{
			m_bIsSupend = false;
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		else
		{
			CDlgManualAlign manualAlign;
			if(IDOK!=manualAlign.DoModal())
			{
				m_bIsSupend = false;
				m_bSearchMark = false;
				strError.Format("Mark�����ʧ��");
				PutLogIntoList(strError);
				return false;
			}
			else
			{
				dOffsetX2 = tgEndPos.x - m_pMv->GetPos(K_AXIS_X);
				dOffsetY2 = tgEndPos.y - m_pMv->GetPos(K_AXIS_Y);
				tgEndBmpOffset.x = 0;
				tgEndBmpOffset.y = 0;
				bMamualAlign = true;
				m_bIsSupend = false;
				break;
			}
		}
		m_bIsSupend = false;
	}
	if(!bMamualAlign)
	{
		dCenterX[1] = milApp.m_stResult.dResultCenterX[0];
		dCenterY[1] = milApp.m_stResult.dResultCenterY[0];
		dAngle[1] = milApp.m_stResult.dResultAngle[0];
		strText.Format("���Ͻ�ͼ����ҽ��:CenterX=%.3f,CenterY=%.3f,Angle=%.3f,Score=%.3f",milApp.m_stResult.dResultCenterX[0],
			milApp.m_stResult.dResultCenterY[0],milApp.m_stResult.dResultAngle[0],milApp.m_stResult.dResultScore[0]);
		AddMsg(strText);
		theApp.m_tSysParam.BmpMarkCalibration.ConvertPixelToTable(dCenterX[1]-(milApp.m_lBufSizeX-1)/2,
			dCenterY[1]-(milApp.m_lBufSizeY-1)/2,dOffsetX2,dOffsetY2);
		dOffsetX2 -= tgEndBmpOffset.x;
		dOffsetY2 -= tgEndBmpOffset.y;
	}
	strText.Format("���Ͻ�ͼ��ƫ��:OffsetX=%.3f,OffsetY=%.3f",dOffsetX2,dOffsetY2);
	AddMsg(strText);

	bMamualAlign = false;
	//m_pMv->GotoSafeZPos();
	if(IsRunStop())
	{
		m_bSearchMark = false;
		strError.Format("Mark�����ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	if(!SynchronizeMoveXY(tgStartPos.x,tgStartPos.y,true))
	{
		m_bSearchMark = false;
		strError.Format("Mark�����ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	if(IsRunStop())
	{
		m_bSearchMark = false;
		strError.Format("Mark�����ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	if(!SynchronizeMoveZ(tgStartPos.za,true))
	{
		m_bSearchMark = false;
		strError.Format("Mark�����ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
	if(!g_pView->m_ImgStatic.CopyImage(RGB8))
	{
		m_bSearchMark = false;
		strError.Format("Mark�����ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	if(IsRunStop())
	{
		m_bSearchMark = false;
		strError.Format("Mark�����ʧ��");
		PutLogIntoList(strError);
		return false;
	}
	nIndex = 0;
	strFile = g_pDoc->GetModeFile(nIndex);
	//milApp.SetMatchMode(ProductParam.matchMode);
	milApp.RestoreModel(strFile.GetBuffer(strFile.GetLength()));
	milApp.SetPatParam(theApp.m_tSysParam.BmpMarkAcceptance,theApp.m_tSysParam.BmpMarkAngle);
	milApp.SetModelWindow(theApp.m_tSysParam.BmpMarkLearnWin[0]);
	milApp.SetSearchWindow(theApp.m_tSysParam.BmpMarkROI);
	//strFile = g_pDoc->GetModeFile(nIndex);
	//milApp.RestoreModel(strFile.GetBuffer(strFile.GetLength()));
	//milApp.SetPatParam(theApp.m_tSysParam.BmpMarkAcceptance,theApp.m_tSysParam.BmpMarkAngle);
	//milApp.SetModelWindow(theApp.m_tSysParam.BmpMarkLearnWin[nIndex]);
	//milApp.SetSearchWindow(theApp.m_tSysParam.BmpMarkROI[nIndex]);
	//if(!milApp.FindModel())
	//{
	//	AddMsg("ͼ��λ��1����쳣");
	//	return false;
	//}
	while(!milApp.FindModel(true))
	{
		if(IsRunStop())
		{
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		if(!SynchronizeMoveXY(tgStartPos.x-dBmpOffset,tgStartPos.y,true))
		{
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
		if(!g_pView->m_ImgStatic.CopyImage(RGB8))
		{
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		if(milApp.FindModel(true))
		{
			tgStartBmpOffset.x = -dBmpOffset;
			tgStartBmpOffset.y = 0.0;
			break;
		}
		if(IsRunStop())
		{
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		if(!SynchronizeMoveXY(tgStartPos.x,tgStartPos.y-dBmpOffset,true))
		{
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			m_bSearchMark = false;
			return false;
		}
		Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
		if(!g_pView->m_ImgStatic.CopyImage(RGB8))
		{
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		if(milApp.FindModel(true))
		{
			tgStartBmpOffset.x = 0.0;
			tgStartBmpOffset.y = -dBmpOffset;
			break;
		}
		if(IsRunStop())
		{
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		if(!SynchronizeMoveXY(tgStartPos.x+dBmpOffset,tgStartPos.y,true))
		{
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
		if(!g_pView->m_ImgStatic.CopyImage(RGB8))
		{
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		if(milApp.FindModel(true))
		{
			tgStartBmpOffset.x = dBmpOffset;
			tgStartBmpOffset.y = 0.0;
			break;
		}
		if(IsRunStop())
		{
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		if(!SynchronizeMoveXY(tgStartPos.x+dBmpOffset,tgStartPos.y,true))
		{
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
		if(!g_pView->m_ImgStatic.CopyImage(RGB8))
		{
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		if(milApp.FindModel(true))
		{
			tgStartBmpOffset.x = 0.0;
			tgStartBmpOffset.y = dBmpOffset;
			break;
		}
		AddMsg("ͼ��λ��1����쳣");
		m_bIsSupend = true;
		if(AfxMessageBox("�ֶ�����ͼ��",MB_YESNO)!=IDYES)
		{
			m_bIsSupend = false;
			m_bSearchMark = false;
			strError.Format("Mark�����ʧ��");
			PutLogIntoList(strError);
			return false;
		}
		else
		{
			CDlgManualAlign manualAlign;
			if(IDOK!=manualAlign.DoModal())
			{
				m_bIsSupend = false;
				m_bSearchMark = false;
				strError.Format("Mark�����ʧ��");
				PutLogIntoList(strError);
				return false;
			}
			else
			{
				dOffsetX1 = tgStartPos.x - m_pMv->GetPos(K_AXIS_X);
				dOffsetY1 = tgStartPos.y - m_pMv->GetPos(K_AXIS_Y);
				tgStartBmpOffset.x = 0;
				tgStartBmpOffset.y = 0;
				bMamualAlign = true;
				m_bIsSupend = false;
				break;
			}
		}
		m_bIsSupend = false;
	}
	if(!bMamualAlign)
	{
		dCenterX[0] = milApp.m_stResult.dResultCenterX[0];
		dCenterY[0] = milApp.m_stResult.dResultCenterY[0];
		dAngle[0] = milApp.m_stResult.dResultAngle[0];
		strText.Format("���½ǽ�ͼ����ҽ��:CenterX=%.3f,CenterY=%.3f,Angle=%.3f,Score=%.3f",milApp.m_stResult.dResultCenterX[0],
			milApp.m_stResult.dResultCenterY[0],milApp.m_stResult.dResultAngle[0],milApp.m_stResult.dResultScore[0]);
		AddMsg(strText);

		theApp.m_tSysParam.BmpMarkCalibration.ConvertPixelToTable(dCenterX[0]-(milApp.m_lBufSizeX-1)/2,
			dCenterY[0]-(milApp.m_lBufSizeY-1)/2,dOffsetX1,dOffsetY1);
		dOffsetX1 -= tgStartBmpOffset.x;
		dOffsetY1 -= tgStartBmpOffset.y;
	}
	strText.Format("���½�ͼ��ƫ��:OffsetX=%.3f,OffsetY=%.3f",dOffsetX1,dOffsetY1);
	AddMsg(strText);

	m_tOffset.x = (dOffsetX1+dOffsetX2)/2;
	m_tOffset.y = (dOffsetY1+dOffsetY2)/2;
	m_dRotateAngle = CalculateAngle((tgEndPos.x-dOffsetX2)-(tgStartPos.x-dOffsetX1),(tgEndPos.y-dOffsetY2)-(tgStartPos.y-dOffsetY1))-
		CalculateAngle(tgEndPos.x-tgStartPos.x,tgEndPos.y-tgStartPos.y);
	strText.Format("ͼ��ƫ��:OffsetX=%.3f,OffsetY=%.3f,Angle=%.3f",m_tOffset.x,m_tOffset.y,m_dRotateAngle);
	AddMsg(strText);
	m_bSearchMark = false;
	strError.Format("Mark����ҳɹ�");
	PutLogIntoList(strError);
	return true;
}

bool CCmdRun::SlotDetection()
{
	if(m_bCleanNeedle||m_bDischarge||m_bTestDispense||m_bSearchMark||m_bSlotDetect||m_bHoming||m_bMeasureHeight||m_bNeedleCalibrate||m_bNeedleAutoAdjust)
	{
		CString strError;
		strError.Format("δ֪����");
		if(m_bCleanNeedle)
		{
			strError.Format("��ϴ��ͷʱ������������⣡");
		}
		else if(m_bDischarge)
		{
			strError.Format("�Ž�ʱ������������⣡");
		}
		else if(m_bTestDispense)
		{
			strError.Format("�Խ�ʱ������������⣡");
		}
		else if(m_bSearchMark)
		{
			strError.Format("����Mark��ʱ������������⣡");
		}
		else if(m_bSlotDetect)
		{
			strError.Format("�������ʱ������������⣡");
		}
		else if(m_bHoming)
		{
			strError.Format("��λʱ������������⣡");
		}
		else if(m_bMeasureHeight)
		{
			strError.Format("���ʱ������������⣡");
		}
		else if (m_bNeedleCalibrate)
		{
			strError.Format("��ͷ�궨ʱ������������⣡");
		}
		else if (m_bNeedleAutoAdjust)
		{
			strError.Format("�Զ�����ʱ������������⣡");
		}
		PutLogIntoList(strError);
		return false;
	}
	m_bSlotDetect = true;
	tgPos tgDetectPos;
	tgDetectPos.x = ProductParam.tSlotDetectPos.x;
	tgDetectPos.y = ProductParam.tSlotDetectPos.y;
	tgDetectPos.za = ProductParam.tSlotDetectPos.za;
	if(!MoveToZSafety())
	{
		m_bSlotDetect = false;
		return false;
	}
	if(IsRunStop())
	{
		m_bSlotDetect = false;
		return false;
	}
	if(!SynchronizeMoveXY(tgDetectPos.x,tgDetectPos.y,true))
	{
		m_bSlotDetect = false;
		return false;
	}
	if(IsRunStop())
	{
		m_bSlotDetect = false;
		return false;
	}
	//if(!SynchronizeMoveZ(tgDetectPos.za,true))
	//{
	//	m_bSlotDetect = false;
	//	return false;
	//}
	//if(IsRunStop())
	//{
	//	m_bSlotDetect = false;
	//	return false;
	//}
	Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
	if(!g_pView->m_ImgStatic.CopyImage(RGB8))
	{
		m_bSlotDetect = false;
		return false;
	}
	int nIndex = g_pFrm->m_CmdRun.ProductParam.nImageNum;
	CString strFile = g_pDoc->GetModeFile(nIndex);
	//milApp.SetMatchMode(ProductParam.matchMode);
	milApp.RestoreModel(strFile.GetBuffer(strFile.GetLength()));
	milApp.SetPatParam(theApp.m_tSysParam.BmpSlotAcceptance,theApp.m_tSysParam.BmpSlotAngle);
	milApp.SetModelWindow(theApp.m_tSysParam.BmpSlotLearnWin);
	milApp.SetSearchWindow(theApp.m_tSysParam.BmpSlotROI);
	if(!milApp.FindModel(true))
	{
		MoveToZSafety();
		AddMsg("��ȷ��֧���Ƿ�ŷ�");
		m_bSlotDetect = false;
		return false;
	}
	if(!MoveToZSafety())
	{
		m_bSlotDetect = false;
		return false;
	}
	m_bSlotDetect = false;
	return true;
}
void CCmdRun::SelectDispense(bool *bIsDispense)
{
	//int nCountNum = ProductParam.FirstDispRow*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn;
	CString str;
	m_tOffset.x = 0.0;
	m_tOffset.y = 0.0;
	m_tOffset.za = 0.0;
	m_dRotateAngle = 0.0;
	DisCompCount = 0;
	DWORD StartTime,EndTime;
	StartTime = EndTime = GetTickCount();
	m_tStatus = K_RUN_STS_RUN;
	if(ProductParam.visionMode==VISION_TWO_MARK)
	{
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,TRUE);
		StartTime = EndTime = GetTickCount();
		//if(!FindTwoMark())
		if(!FindImageOffset(m_tOffset.x,m_tOffset.y,m_dRotateAngle,false))
		{
			m_tOffset.x = 0.0;
			m_tOffset.y = 0.0;
			m_dRotateAngle = 0.0;
			AddMsg("û���ҵ�ͼ��");
			m_bIsSupend = true;
			theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,FALSE);
			if(AfxMessageBox("ͼ�����ʧ�ܣ��Ƿ������",MB_YESNO)!=IDYES)
			{
				m_bIsSupend = false;
				Alarm(17);
				return;
			}
			m_bIsSupend = false;
		}
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,FALSE);
		EndTime = GetTickCount();
		str.Format("FindMark:%ld",EndTime-StartTime);
		AddMsg(str);
		if(fabs(m_dRotateAngle)>ProductParam.MaxOffsetAngle)
		{
			m_bIsSupend = true;
			if(AfxMessageBox("ƫ�ƽǶȳ�������ĽǶȣ��Ƿ�������У�",MB_YESNO)!=IDYES)
			{
				m_tOffset.x = 0.0;
				m_tOffset.y = 0.0;
				m_dRotateAngle = 0.0;
				m_bIsSupend = false;
				m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
				str.Format("ͼ�����ʧ��!");
				PutLogIntoList(str);
				Alarm(17);
				return;
			}
			m_bIsSupend = false;
		}
		if(fabs(m_tOffset.x)>ProductParam.MaxOffsetX||fabs(m_tOffset.y)>ProductParam.MaxOffsetY)
		{
			m_bIsSupend = true;
			if(AfxMessageBox("λ��ƫ�Ƴ���2mm���Ƿ�������У�",MB_YESNO)!=IDYES)
			{
				m_tOffset.x = 0.0;
				m_tOffset.y = 0.0;
				m_dRotateAngle = 0.0;
				m_bIsSupend = false;
				m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
				str.Format("ͼ�����ʧ��!");
				PutLogIntoList(str);
				Alarm(17);
				return;
			}
			if(AfxMessageBox("�Ƿ����λ��ƫ�ƣ�",MB_YESNO)!=IDYES)
			{
				m_tOffset.x = 0.0;
				m_tOffset.y = 0.0;
				m_dRotateAngle = 0.0;
			}
			m_bIsSupend = false;
		}
	}
	if(ProductParam.MeasureHeightEnable)
	{
		StartTime = EndTime = GetTickCount();
		HEIGHT_STATUS ret = MeasureHeight(m_tOffset.za);
		EndTime = GetTickCount();
		str.Format("MeasureHeight:time = %ld,�߶�ֵ = %0.3f",EndTime-StartTime,m_tOffset.za);
		AddMsg(str);
		if(HEIGHT_STATUS_OK != ret)
		{
			if(HEIGHT_STATUS_EXCEED_LIMIT == ret)
			{
				m_bIsSupend = true;
				if(AfxMessageBox("�߶�ƫ�Ƴ����趨������ֵ���Ƿ�������У�",MB_YESNO)!=IDYES)
				{
					m_tOffset.za = 0.0;
					m_bIsSupend = false;
					m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
					Alarm(21);
					str.Format("�߶�ƫ�Ƴ������ƣ�����ֹͣ");
					PutLogIntoList(str);
					return;
				}
			}
			else
			{
				m_tOffset.za = 0.0;
				m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
				Alarm(21);
				str.Format("��ߴ�������ֹͣ");
				PutLogIntoList(str);
				return;
			}
		}
	}
	if(!CleanAndTest(false))
	{
		AfxMessageBox("�׵���ϴ���Խ�ʧ�ܣ�");
		return;
	}
	if(!ExtandTrack(m_tOffset.x,m_tOffset.y,m_tOffset.za,m_dRotateAngle))
	{
		g_pFrm->m_CmdRun.AddMsg("ExtandTrack Failed!");
		Alarm(36);
		return;
	}
	bool bSelect[3];
	for(int i=0;i<3;i++)
	{
		bSelect[i] = ProductParam.FSelectVal[i];
		RunDispFSelect[i] = false;
	}
	if(!SetAxisProfile(HIGH_VEL))
	{
		return;
	}
	if(!MoveToZSafety())
	{
		return;
	}
	m_bRepair = true;
	g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
	if(ONE_VALVE==ProductParam.valveSelect)
	{
		OneValveRepaire(bIsDispense);
	}
	else if(TWO_VALVE==ProductParam.valveSelect)
	{
		TwoValveRepaire(bIsDispense);
	}
	else
	{
		ThreeValveRepaire(bIsDispense);
	}
	m_bRepair = false;
	for(int i=0;i<3;i++)
	{
		ProductParam.FSelectVal[i] = bSelect[i];
	}
	if(!SetAxisProfile(MEDIUM_VEL))
	{
		Alarm(30);
		m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
		return;
	}
	if(!MoveToZSafety())
	{
		return;
	}
	m_tStatus = K_RUN_STS_STOP;
}

bool CCmdRun::SelectDispense(double x,double y,double za,double zb,double zc)
{
	if(!m_pMv->WarnCheckPos(&x,&y,&za,&zb,&zc))
	{
		//AddMsg("Exceed Motor Limit!");
		AddMsg("�޸�λ�ó������г�!");
		return false;
	}
	if(ProductParam.DispFCount==0)
	{
		return true;
	}
	double r = ProductParam.dDispRadius;
	double distance = ProductParam.dDistance;
	double length = ProductParam.LineLength;
	double dRectLength = ProductParam.RectLength;
	double dRectWidth = ProductParam.RectWidth;
	if(ProductParam.fillMode==HELIX_DISP_MODE)
	{
		if(!RunSingleDot(x,y,za,zb,zc,r,distance,true))
		{
			return false;
		}
	}
	else if(ProductParam.fillMode == LINE_HORIZONTAL_DISP_MODE)
	{
		if(!RunSingleDot(x,y,za,zb,zc,length,false,ProductParam.DispFCount,true))
		{
			return false;
		}
	}
	else if(ProductParam.fillMode == LINE_VERTICAL_DISP_MODE)
	{
		if(!RunSingleDot(x,y,za,zb,zc,length,true,ProductParam.DispFCount,true))
		{
			return false;
		}
	}
	else if(ProductParam.fillMode == RECTANGLE_DISP_MODE)
	{
		if(!RunSingleDot(x,y,za,zb,zc,dRectLength,dRectWidth,true,true))
		{
			return false;
		}
	}
	else if(ProductParam.fillMode == CUSTOM_DISP_MODE)
	{
		if(!RunSingleDot(x,y,za,zb,zc,g_pFrm->m_rawList,true))
		{
			return false;
		}
	}
	else if(ProductParam.fillMode == CIRCLE_DISP_MODE)
	{
		if(!RunSingleDot(x,y,za,zb,zc,r,true))
		{
			return false;
		}
	}
	else if(ProductParam.fillMode == MULTI_DISP_MODE)
	{
		if(!RunSingleDot(x,y,za,zb,zc,true,true))
		{
			return false;
		}
	}
	else
	{
		if(!RunSingleDot(x,y,za,zb,zc,true,false))
		{
			return false;
		}
	}
	DisCompCount++;
	return true;
}

//20180905 �����Ƕȼ���
double CCmdRun::CalculateAngle(double dX,double dY)
{
	//double dAngle = 0.0;
	//double dXNew,dYNew;
	//dXNew=fabs(dX);
	//dYNew=fabs(dY);
	//double dis = sqrt(dX*dX+dY*dY);
	//if(fabs(dis)<0.01)
	//{
	//	dAngle = 0.0;
	//}
	//else
	//{
	//	dAngle = acos((double)dXNew/dis);
	//}
	//if ((dX*dY)<0)
	//{
	//	dAngle=KGB_PI-dAngle;            //�Ƕ���:������
	//}
	//return dAngle/KGB_PI*180;
	double angle;
	angle=0;
	angle=Getagforpoint(dX,dY);
	return angle;
}

int	CCmdRun::Getquforpoint(double dX,double dY)
{
	if(dX > 0 && dY> 0) //1
	{
		return 1;
	}
	else if( abs(dX-0)<EPS && dY>0) //+y
	{
		return 12;
	}
	else if(dX<0 && dY>0) //2
	{
		return 2;
	}
	else if(dX<0&& abs(dY-0)<EPS) //-x
	{
		return 23;
	}
	else if(dX<0&& dY<0) //3
	{
		return 3;
	}
	else if(abs(dX-0)<EPS && dY<0) //-y
	{
		return 34;
	}
	else if(dX>0&& dY<0) //4
	{
		return 4;
	}
	else if(dX>0&& abs(dY-0)<EPS) //+x
	{
		return 41;
	}
	else if(abs(dX-0)<EPS && abs(dY-0)<EPS) //o
	{
		return 0;
	}
	else
	{
		return -1;
	}
}
double CCmdRun::Getagforpoint(double dX,double dY)
{
	double x=abs(dX);
	double y=abs(dY);
	double angle;

	if(Getquforpoint(dX,dY) == 12)
	{
		angle = 90.00;
	}
	else if(Getquforpoint(dX,dY) == 23)
	{
		angle = 180.00;
	} 
	else if(Getquforpoint(dX,dY) == 34)
	{
		angle = 270.00;
	} 
	else if(Getquforpoint(dX,dY) == 41)
	{
		angle = 0.00;
	} 
	else if(Getquforpoint(dX,dY) == 1)
	{
		angle = atan(y/x)/KGB_PI*180;
	} 
	else if(Getquforpoint(dX,dY) == 2)
	{
		angle = atan(x/y)/KGB_PI*180+90;
	} 
	else if(Getquforpoint(dX,dY) == 3)
	{
		angle = atan(y/x)/KGB_PI*180+180;
	} 
	else if(Getquforpoint(dX,dY) == 4)
	{
		angle = atan(x/y)/KGB_PI*180+270;
	} 
	else
	{
		angle = 0;
	}
	return angle;
}
void CCmdRun::Rotation(double vx,double vy,double dAngle,double &dx,double &dy)
{
	double dsin = sin(dAngle/180*KGB_PI);
	double dcos = cos(dAngle/180*KGB_PI);
	dx = vx*dcos - vy*dsin;
	dy = vx*dsin + vy*dcos;
}

bool CCmdRun::MoveZ(int nAxisNo,double z,double vel,double acc,bool bWaitStop)
{
	if(nAxisNo<K_AXIS_ZA||nAxisNo>K_AXIS_ZC)
	{
		return false;
	}
	if(!m_pMv->IsInitOK())
	{
		return false;
	}
	if(IsRunEmc())
	{
		return false;
	}
	if(IsRunAlarm())
	{
		return false;
	}
	CString str;
	//str.Format("za = %0.3f,zb = %0.3f,zc = %0.3f",za,zb,zc);
	//AddMsg(str);
	MOTION_STATUS status;
	status = m_pMv->SetProfile(nAxisNo,vel,acc);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	if(!IsZInPosition(nAxisNo,z))
	{
		status = m_pMv->Move(nAxisNo,z,false,true);
		if(MOT_STATUS_OK!=status)
		{
			return false;
		}
	}
	if(bWaitStop)
	{
		if(!WaitStopZ(nAxisNo,20000))
		{
			return false;
		}
		//if(!IsZCommandAt(z))
		//{
		//	return false;
		//}
		if(!WaitInPlaceZ(nAxisNo,z,1000))
		{
			return false;
		}
	}
	return true;
}

bool CCmdRun::MoveZ(int nAxisNo,double z,bool bWaitStop)
{
	if(nAxisNo<K_AXIS_ZA||nAxisNo>K_AXIS_ZC)
	{
		return false;
	}
	if(!m_pMv->IsInitOK())
	{
		return false;
	}
	if(IsRunEmc())
	{
		return false;
	}
	if(IsRunAlarm())
	{
		return false;
	}
	CString str;
	//str.Format("za = %0.3f,zb = %0.3f,zc = %0.3f",za,zb,zc);
	//AddMsg(str);
	MOTION_STATUS status;
	if(!IsZInPosition(nAxisNo,z))
	{
		status = m_pMv->Move(nAxisNo,z,false,true);
		if(MOT_STATUS_OK!=status)
		{
			return false;
		}
	}
	if(bWaitStop)
	{
		if(!WaitStopZ(nAxisNo,10000))
		{
			return false;
		}
		//if(!IsZCommandAt(z))
		//{
		//	return false;
		//}
		if(!WaitInPlaceZ(nAxisNo,z,1000))
		{
			return false;
		}
	}
	return true;
}

bool CCmdRun::SynchronizeMoveZ(double z,double vel,double acc,bool bWaitStop)
{
	if(!m_pMv->IsInitOK())
	{
		return false;
	}
	if(IsRunEmc())
	{
		return false;
	}
	if(IsRunAlarm())
	{
		return false;
	}
	CString str;
	//str.Format("za = %0.3f,zb = %0.3f,zc = %0.3f",za,zb,zc);
	//AddMsg(str);
	MOTION_STATUS status;
	status = m_pMv->SetProfile(K_AXIS_ZA,vel,acc);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	if(!IsZInPosition(K_AXIS_ZA,z))
	{
		status = m_pMv->Move(K_AXIS_ZA,z,false,true);
		if(MOT_STATUS_OK!=status)
		{
			return false;
		}
	}
	if(bWaitStop)
	{
		if(!WaitStopZ(K_AXIS_ZA,20000))
		{
			return false;
		}
		//if(!IsZCommandAt(z))
		//{
		//	return false;
		//}
		if(!WaitInPlaceZ(K_AXIS_ZA,z,1000))
		{
			return false;
		}
	}
	return true;
}

bool CCmdRun::SynchronizeMoveZ(double z,bool bWaitStop)
{
	if(!m_pMv->IsInitOK())
	{
		return false;
	}
	if(IsRunEmc())
	{
		return false;
	}
	if(IsRunAlarm())
	{
		return false;
	}
	CString str;
	//str.Format("za = %0.3f,zb = %0.3f,zc = %0.3f",za,zb,zc);
	//AddMsg(str);
	MOTION_STATUS status;
	if(!IsZInPosition(K_AXIS_ZA,z))
	{
		status = m_pMv->Move(K_AXIS_ZA,z,false,true);
		if(MOT_STATUS_OK!=status)
		{
			return false;
		}
	}
	if(bWaitStop)
	{
		if(!WaitStopZ(K_AXIS_ZA,10000))
		{
			return false;
		}
		//if(!IsZCommandAt(z))
		//{
		//	return false;
		//}
		if(!WaitInPlaceZ(K_AXIS_ZA,z,1000))
		{
			return false;
		}
	}
	return true;
}

bool CCmdRun::SynchronizeMoveZ(double za,double zb,double zc,double dVel,double dAcc,bool bRunStatus,bool bWaitStop)
{
	if(!m_pMv->IsInitOK())
	{
		return false;
	}
	if(IsRunEmc())
	{
		return false;
	}
	if(IsRunAlarm())
	{
		return false;
	}
	CString str;
	//str.Format("za = %0.3f,zb = %0.3f,zc = %0.3f",za,zb,zc);
	//AddMsg(str);
	MOTION_STATUS status;
	status = m_pMv->SetProfile(K_AXIS_ZA,dVel,dAcc);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	status = m_pMv->SetProfile(K_AXIS_ZB,dVel,dAcc);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	status = m_pMv->SetProfile(K_AXIS_ZC,dVel,dAcc);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}

	if(!bRunStatus)
	{
		if(!IsZInPosition(K_AXIS_ZA,za))
		{
			status = m_pMv->Move(K_AXIS_ZA,za,false,true);
			if(MOT_STATUS_OK!=status)
			{
				return false;
			}
		}

		if(!IsZInPosition(K_AXIS_ZB,zb))
		{
			status = m_pMv->Move(K_AXIS_ZB,zb,false,true);
			if(MOT_STATUS_OK!=status)
			{
				return false;
			}
		}

		if(!IsZInPosition(K_AXIS_ZC,zc))
		{
			status = m_pMv->Move(K_AXIS_ZC,zc,false,true);
			if(MOT_STATUS_OK!=status)
			{
				return false;
			}
		}

		if(bWaitStop)
		{
			if(!WaitStopZ(K_AXIS_ZA,20000))
			{
				return false;
			}
			if(!WaitStopZ(K_AXIS_ZB,20000))
			{
				return false;
			}
			if(!WaitStopZ(K_AXIS_ZC,20000))
			{
				return false;
			}
			if(!WaitInPlaceZ(K_AXIS_ZA,za,1000))
			{
				return false;
			}
			if(!WaitInPlaceZ(K_AXIS_ZB,zb,1000))
			{
				return false;
			}
			if(!WaitInPlaceZ(K_AXIS_ZB,zc,1000))
			{
				return false;
			}
		}
	}
	else
	{
		if(RunDispFSelect[0])
		{
			if(!IsZInPosition(K_AXIS_ZA,za))
			{
				status = m_pMv->Move(K_AXIS_ZA,za,false,true);
				if(MOT_STATUS_OK!=status)
				{
					return false;
				}
			}
		}

		if(RunDispFSelect[1])
		{
			if(!IsZInPosition(K_AXIS_ZB,zb))
			{
				status = m_pMv->Move(K_AXIS_ZB,zb,false,true);
				if(MOT_STATUS_OK!=status)
				{
					return false;
				}
			}
		}

		if(RunDispFSelect[2])
		{
			if(!IsZInPosition(K_AXIS_ZC,zc))
			{
				status = m_pMv->Move(K_AXIS_ZC,zc,false,true);
				if(MOT_STATUS_OK!=status)
				{
					return false;
				}
			}
		}

		if(bWaitStop)
		{
			if(!WaitStopZ(K_AXIS_ZA,20000))
			{
				return false;
			}
			if(!WaitStopZ(K_AXIS_ZB,20000))
			{
				return false;
			}
			if(!WaitStopZ(K_AXIS_ZC,20000))
			{
				return false;
			}
			if(RunDispFSelect[0])
			{
				if(!WaitInPlaceZ(K_AXIS_ZA,za,1000))
				{
					return false;
				}
			}
			if(RunDispFSelect[1])
			{
				if(!WaitInPlaceZ(K_AXIS_ZB,zb,1000))
				{
					return false;
				}
			}
			if(RunDispFSelect[2])
			{
				if(!WaitInPlaceZ(K_AXIS_ZB,zc,1000))
				{
					return false;
				}
			}
		}
	}
	return true;
}
bool CCmdRun::SynchronizeMoveZ(double za,double zb,double zc,bool bRunStatus, bool bWaitStop)
{
	if(!m_pMv->IsInitOK())
	{
		return false;
	}
	if(IsRunEmc())
	{
		return false;
	}
	if(IsRunAlarm())
	{
		return false;
	}
	CString str;
	//str.Format("za = %0.3f,zb = %0.3f,zc = %0.3f",za,zb,zc);
	//AddMsg(str);
	MOTION_STATUS status;

	if(!bRunStatus)
	{
		if(!IsZInPosition(K_AXIS_ZA,za))
		{
			status = m_pMv->Move(K_AXIS_ZA,za,false,true);
			if(MOT_STATUS_OK!=status)
			{
				return false;
			}
		}

		if(!IsZInPosition(K_AXIS_ZB,zb))
		{
			status = m_pMv->Move(K_AXIS_ZB,zb,false,true);
			if(MOT_STATUS_OK!=status)
			{
				return false;
			}
		}

		if(!IsZInPosition(K_AXIS_ZC,zc))
		{
			status = m_pMv->Move(K_AXIS_ZC,zc,false,true);
			if(MOT_STATUS_OK!=status)
			{
				return false;
			}
		}

		if(bWaitStop)
		{
			if(!WaitStopZ(K_AXIS_ZA,20000))
			{
				return false;
			}
			if(!WaitStopZ(K_AXIS_ZB,20000))
			{
				return false;
			}
			if(!WaitStopZ(K_AXIS_ZC,20000))
			{
				return false;
			}
			if(!WaitInPlaceZ(K_AXIS_ZA,za,1000))
			{
				return false;
			}
			if(!WaitInPlaceZ(K_AXIS_ZB,zb,1000))
			{
				return false;
			}
			if(!WaitInPlaceZ(K_AXIS_ZC,zc,1000))
			{
				return false;
			}
		}
	}
	else
	{
		if(RunDispFSelect[0])
		{
			if(!IsZInPosition(K_AXIS_ZA,za))
			{
				status = m_pMv->Move(K_AXIS_ZA,za,false,true);
				if(MOT_STATUS_OK!=status)
				{
					return false;
				}
			}
		}

		if(RunDispFSelect[1])
		{
			if(!IsZInPosition(K_AXIS_ZB,zb))
			{
				status = m_pMv->Move(K_AXIS_ZB,zb,false,true);
				if(MOT_STATUS_OK!=status)
				{
					return false;
				}
			}
		}

		if(RunDispFSelect[2])
		{
			if(!IsZInPosition(K_AXIS_ZC,zc))
			{
				status = m_pMv->Move(K_AXIS_ZC,zc,false,true);
				if(MOT_STATUS_OK!=status)
				{
					return false;
				}
			}
		}

		if(bWaitStop)
		{
			if(!WaitStopZ(K_AXIS_ZA,20000))
			{
				return false;
			}
			if(!WaitStopZ(K_AXIS_ZB,20000))
			{
				return false;
			}
			if(!WaitStopZ(K_AXIS_ZC,20000))
			{
				return false;
			}
			if(RunDispFSelect[0])
			{
				if(!WaitInPlaceZ(K_AXIS_ZA,za,1000))
				{
					return false;
				}
			}
			if(RunDispFSelect[1])
			{
				if(!WaitInPlaceZ(K_AXIS_ZB,zb,1000))
				{
					return false;
				}
			}
			if(RunDispFSelect[2])
			{
				if(!WaitInPlaceZ(K_AXIS_ZC,zc,1000))
				{
					return false;
				}
			}
		}
	}
	return true;
}
bool CCmdRun::WaitStopZ(int nAxisNo, unsigned long lTimeout)
{
	if(!m_pMv->IsInitOK())
	{
		return false;
	}
	if(K_AXIS_ZA!=nAxisNo&&K_AXIS_ZB!=nAxisNo&&K_AXIS_ZC!=nAxisNo)
	{
		return false;
	}
	if(IsRunEmc())
	{
		return false;
	}
	if(IsRunAlarm())
	{
		return false;
	}
	MOTION_STATUS status;
	status = m_pMv->WaitStop(nAxisNo,lTimeout);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	return true;
}

bool CCmdRun::IsZCommandAt(int nAxisNo, double dPosZ)
{
	if(!m_pMv->IsInitOK())
	{
		return false;
	}
	if(K_AXIS_ZA!=nAxisNo&&K_AXIS_ZB!=nAxisNo&&K_AXIS_ZC!=nAxisNo)
	{
		return false;
	}
	return fabs(m_pMv->GetEncPos(nAxisNo)-dPosZ)<0.5;
}

bool CCmdRun::SynchronizeMoveXY(double x,double y,double vel,double acc,bool bWaitStop)
{
	if(!m_pMv->IsInitOK())
	{
		return false;
	}
	if(IsRunEmc())
	{
		return false;
	}
	if(IsRunAlarm())
	{
		return false;
	}
	MOTION_STATUS status;
	status = m_pMv->SetProfile(K_AXIS_X,vel,acc);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	status = m_pMv->SetProfile(K_AXIS_Y,vel,acc);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	if(!IsXInPosition(x))
	{
		status = m_pMv->Move(K_AXIS_X,x,false,true);
		if(MOT_STATUS_OK!=status)
		{
			return false;
		}
	}
	if(!IsYInPosition(y))
	{
		status = m_pMv->Move(K_AXIS_Y,y,false,true);
		if(MOT_STATUS_OK!=status)
		{
			return false;
		}
	}
	if(bWaitStop)
	{
		if(!WaitStopXY(10000))
		{
			return false;
		}
		if((!WaitInPlaceX(x,1000))||(!WaitInPlaceY(y,1000)))
		{
			return false;
		}
	}
	return true;
}

bool CCmdRun::SynchronizeMoveXY(double x,double y,bool bWaitStop)
{
	CString strText;
	if(!m_pMv->IsInitOK())
	{
		return false;
	}
	if(IsRunEmc())
	{
		return false;
	}
	if(IsRunAlarm())
	{
		return false;
	}
	MOTION_STATUS status;
	if(!IsXInPosition(x))
	{
		status = m_pMv->Move(K_AXIS_X,x,false,true);
		if(MOT_STATUS_OK!=status)
		{
			strText.Format("move status = %d",int(status));
			AddMsg(strText);
			return false;
		}
	}
	if(!IsYInPosition(y))
	{
		status = m_pMv->Move(K_AXIS_Y,y,false,true);
		if(MOT_STATUS_OK!=status)
		{
			strText.Format("move status = %d",int(status));
			AddMsg(strText);
			return false;
		}
	}
	if(bWaitStop)
	{
		if(!WaitStopXY(10000))
		{
			AddMsg("WaitStopXY Failed!");
			return false;
		}
		if((!WaitInPlaceX(x,1000))||(!WaitInPlaceY(y,1000)))
		{
			return false;
		}
	}
	return true;
}

bool CCmdRun::WaitStopXY(unsigned long lTimeout)
{
	if(!m_pMv->IsInitOK())
	{
		return false;
	}
	if(IsRunEmc())
	{
		return false;
	}
	if(IsRunAlarm())
	{
		return false;
	}
	MOTION_STATUS status;
	status = m_pMv->WaitStop(K_AXIS_X,lTimeout);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	status = m_pMv->WaitStop(K_AXIS_Y,lTimeout);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	return true;
}

bool CCmdRun::IsXCommandAt(double dPosX)
{
	if(!m_pMv->IsInitOK())
	{
		return false;
	}
	return fabs(m_pMv->GetEncPos(K_AXIS_X)-dPosX)<1; //2017-07-08
}

bool CCmdRun::IsYCommandAt(double dPosY)
{
	if(!m_pMv->IsInitOK())
	{
		return false;
	}
	return fabs(m_pMv->GetEncPos(K_AXIS_Y)-dPosY)<0.5;
}

bool CCmdRun::WaitInPlaceX(double dPosX,long lTimeout)
{
	LONGLONG llDueTime = CTimeUtil::GetDueTime(lTimeout);
	while(!CTimeUtil::IsTimeout(llDueTime))
	{
		if(IsXCommandAt(dPosX))
		{
			break;
		}
		Sleep(1);
	}
	if(!IsXCommandAt(dPosX))
	{
		return false;
	}
	return true;
}

bool CCmdRun::WaitInPlaceY(double dPosY,long lTimeout)
{
	LONGLONG llDueTime = CTimeUtil::GetDueTime(lTimeout);
	while(!CTimeUtil::IsTimeout(llDueTime))
	{
		if(IsYCommandAt(dPosY))
		{
			break;
		}
		Sleep(1);
	}
	if(!IsYCommandAt(dPosY))
	{
		return false;
	}
	return true;
}

bool CCmdRun::WaitInPlaceZ(int nAxisNo, double dPosZ,long lTimeout)
{
	if(K_AXIS_ZA!=nAxisNo&&K_AXIS_ZB!=nAxisNo&&K_AXIS_ZC!=nAxisNo)
	{
		return false;
	}
	LONGLONG llDueTime = CTimeUtil::GetDueTime(lTimeout);
	while(!CTimeUtil::IsTimeout(llDueTime))
	{
		if(IsZCommandAt(nAxisNo,dPosZ))
		{
			break;
		}
		Sleep(1);
	}
	if(!IsZCommandAt(nAxisNo,dPosZ))
	{
		return false;
	}
	return true;
}

bool CCmdRun::IsXInPosition(double dPosX)
{
	if(!m_pMv->IsInitOK())
	{
		return false;
	}
	return fabs(m_pMv->GetEncPos(K_AXIS_X)-dPosX)<0.01;
}

bool CCmdRun::IsYInPosition(double dPosY)
{
	if(!m_pMv->IsInitOK())
	{
		return false;
	}
	return fabs(m_pMv->GetEncPos(K_AXIS_Y)-dPosY)<0.01;
}

bool CCmdRun::IsZInPosition(int nAxisNo, double dPosZ)
{
	if(!m_pMv->IsInitOK())
	{
		return false;
	}
	if(K_AXIS_ZA!=nAxisNo&&K_AXIS_ZB!=nAxisNo&&K_AXIS_ZC!=nAxisNo)
	{
		return false;
	}
	return fabs(m_pMv->GetEncPos(nAxisNo)-dPosZ)<0.01;
}

tgPos CCmdRun::GetPadDispensePos(int nFirstRow,int nFirstColumn,int nSecondRow,int nSecondColumn,int nThirdRow,int nThirdColumn)
{
	tgPos tgDispensePos;
	tgDispensePos.x = ProductParam.DispenseStartPos[0] + nFirstColumn*ProductParam.FirstDispColumnD + nSecondColumn*ProductParam.SecondDispColumnD + nThirdColumn*ProductParam.ThirdDispColumnD;
	tgDispensePos.y = ProductParam.DispenseStartPos[1] + nFirstRow*ProductParam.FirstDispRowD + nSecondRow*ProductParam.SecondDispRowD + nThirdRow*ProductParam.ThirdDispRowD;
	tgDispensePos.za = ProductParam.DispenseStartPos[2];
	tgDispensePos.zb = ProductParam.DispenseStartPos[3];
	tgDispensePos.zc = ProductParam.DispenseStartPos[4];
	return tgDispensePos;
}

tgPos CCmdRun::GetPadCameraPos(int nFirstRow,int nFirstColumn,int nSecondRow,int nSecondColumn,int nThirdRow,int nThirdColumn)
{
	tgPos tgCameraPos;
	tgCameraPos = GetPadDispensePos(nFirstRow,nFirstColumn,nSecondRow,nSecondColumn,nThirdRow,nThirdColumn);
	tgCameraPos.x = tgCameraPos.x + (ProductParam.tgLabelPos[3].x - ProductParam.tgLabelPos[0].x);
	tgCameraPos.y = tgCameraPos.y + (ProductParam.tgLabelPos[3].y - ProductParam.tgLabelPos[0].y);
	tgCameraPos.za = ProductParam.tgImagePos[0].za;
	//tgPos tgCameraPos;
	//tgCameraPos.x = ProductParam.StartImagePos[0] + nFirstColumn*ProductParam.FirstDispColumnD + nSecondColumn*ProductParam.SecondDispColumnD + nThirdColumn*ProductParam.ThirdDispColumnD;
	//tgCameraPos.y = ProductParam.StartImagePos[1] + nFirstRow*ProductParam.FirstDispRowD + nSecondRow*ProductParam.SecondDispRowD + nThirdRow*ProductParam.ThirdDispRowD;
	//tgCameraPos.z = ProductParam.StartImagePos[2];
	return tgCameraPos;
}

tgPos CCmdRun::GetPadDispensePos(int nIrregularRow,int nIrregularColumn)
{
	tgPos tgDispensePos;
	tgDispensePos.x = ProductParam.DispenseStartPos[0];
	tgDispensePos.y = ProductParam.DispenseStartPos[1];
	tgDispensePos.za = ProductParam.DispenseStartPos[2];
	tgDispensePos.zb = ProductParam.DispenseStartPos[3];
	tgDispensePos.zc = ProductParam.DispenseStartPos[4];
	for(int i=0;i<nIrregularRow;i++)
	{
		tgDispensePos.y += ProductParam.IrregularRowD[i];
	}
	for(int i=0;i<nIrregularColumn;i++)
	{
		tgDispensePos.x += ProductParam.IrregularColumnD[i];
	}
	return tgDispensePos;
}

tgPos CCmdRun::GetPadCameraPos(int nIrregularRow,int nIrregularColumn)
{
	//tgPos tgCameraPos;
	//tgCameraPos.x = ProductParam.StartImagePos[0];
	//tgCameraPos.y = ProductParam.StartImagePos[1];
	//tgCameraPos.z = ProductParam.StartImagePos[2];
	//for(int i=0;i<nIrregularRow;i++)
	//{
	//	tgCameraPos.y += ProductParam.IrregularRowD[i];
	//}
	//for(int i=0;i<nIrregularColumn;i++)
	//{
	//	tgCameraPos.x += ProductParam.IrregularColumnD[i];
	//}
	tgPos tgCameraPos;
	tgCameraPos = GetPadDispensePos(nIrregularRow,nIrregularColumn);
	tgCameraPos.x = tgCameraPos.x + (ProductParam.tgLabelPos[3].x - ProductParam.tgLabelPos[0].x);;
	tgCameraPos.y = tgCameraPos.y + (ProductParam.tgLabelPos[3].y - ProductParam.tgLabelPos[0].y);;
	tgCameraPos.za = ProductParam.tgImagePos[0].za;
	//tgCameraPos.zb = ProductParam.tgImagePos[0].zb;
	//tgCameraPos.zc = ProductParam.tgImagePos[0].zc;
	return tgCameraPos;
}

bool CCmdRun::ScanInspect()
{
	CString strFile;
	int nTempCount = 0;
	if(m_pPadDetect==NULL)
	{
		m_pPadDetect = new bool[m_nPadNumber];
	}
	memset(m_pPadDetect,1,m_nPadNumber*sizeof(bool));
	if(!MoveToZSafety())
	{
		return false;
	}
	if(IsRunStop())
	{
		return false;
	}
	tgPos tgScanPos = GetFirstCameraPos();
	if(!SynchronizeMoveXY(tgScanPos.x,tgScanPos.y,true))
	{
		return false;
	}
	if(!SynchronizeMoveZ(tgScanPos.za,true))
	{
		return false;
	}
	strFile = g_pDoc->GetModeFile(0);
	//milApp.SetMatchMode(ProductParam.matchMode);
	milApp.RestoreModel(strFile.GetBuffer(strFile.GetLength()));
	milApp.SetPatParam(theApp.m_tSysParam.BmpMarkAcceptance,theApp.m_tSysParam.BmpMarkAngle);
	milApp.SetModelWindow(theApp.m_tSysParam.BmpMarkLearnWin[0]);
	milApp.SetSearchWindow(theApp.m_tSysParam.BmpMarkROI);
	if(ProductParam.IrregularMatrix)
	{
		for(int i=0;i<ProductParam.IrregularRow;i++)
		{
			if(!(i%2))
			{
				for(int j=0;j<ProductParam.IrregularColumn;j++)
				{
					tgScanPos = GetPadCameraPos(i,j);
					if(fabs(m_pMv->GetPos(K_AXIS_X)-tgScanPos.x)>0.01||fabs(m_pMv->GetPos(K_AXIS_Y)-tgScanPos.y)>0.01)
					{
						if(!SynchronizeMoveXY(tgScanPos.x,tgScanPos.y,true))
						{
							return false;
						}
					}
					if(fabs(m_pMv->GetPos(K_AXIS_ZA)-tgScanPos.za)>0.01)
					{
						if(!SynchronizeMoveZ(tgScanPos.za,true))
						{
							return false;
						}
					}
					Sleep(ProductParam.ScanDelay*1000);
					if(!g_pView->m_ImgStatic.CopyImage(RGB8))
					{
						return false;
					}
					if(!milApp.FindModel(true))
					{
						m_pPadDetect[j+i*ProductParam.IrregularColumn] = false;
					
					}
					else
					{
						m_pPadDetect[j+i*ProductParam.IrregularColumn] = true;
					}
					if(IsRunStop())
					{
						return false;
					}
				}
			}
			else
			{
				for(int j=ProductParam.IrregularColumn-1;j>=0;j--)
				{
					tgScanPos = GetPadCameraPos(i,j);
					if(fabs(m_pMv->GetPos(K_AXIS_X)-tgScanPos.x)>0.01||fabs(m_pMv->GetPos(K_AXIS_Y)-tgScanPos.y)>0.01)
					{
						if(!SynchronizeMoveXY(tgScanPos.x,tgScanPos.y,true))
						{
							return false;
						}
					}
					if(fabs(m_pMv->GetPos(K_AXIS_ZA)-tgScanPos.za)>0.01)
					{
						if(!SynchronizeMoveZ(tgScanPos.za,true))
						{
							return false;
						}
					}
					Sleep(ProductParam.ScanDelay*1000);
					if(!g_pView->m_ImgStatic.CopyImage(RGB8))
					{
						return false;
					}
					if(!milApp.FindModel(true))
					{
						m_pPadDetect[j+i*ProductParam.IrregularColumn] = false;
						
					}
					else
					{
						m_pPadDetect[j+i*ProductParam.IrregularColumn] = true;
					}
					if(IsRunStop())
					{
						return false;
					}
				}
			}
		}
	}
	else
	{
		int secondNum = ProductParam.SecondDispRow*ProductParam.SecondDispColumn;
		int thirdNum = ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
		for(int i=0;i<ProductParam.FirstDispRow;i++)
		{
			for(int m=0;m<ProductParam.SecondDispRow;m++)
			{
				for(int k=0;k<ProductParam.ThirdDispRow;k++)
				{
					if(!((i*ProductParam.ThirdDispRow*ProductParam.SecondDispRow+m*ProductParam.ThirdDispRow+k)%2))
					{
						for(int j=0;j<ProductParam.FirstDispColumn;j++)
						{
							for(int n=0;n<ProductParam.SecondDispColumn;n++)
							{
								for(int l=0;l<ProductParam.ThirdDispColumn;l++)
								{
									nTempCount = l+k*ProductParam.ThirdDispColumn+n*thirdNum+m*ProductParam.SecondDispColumn*thirdNum+
										j*secondNum*thirdNum+i*ProductParam.FirstDispColumn*secondNum*thirdNum;
									tgScanPos = GetPadCameraPos(i,j,m,n,k,l);
									if(fabs(m_pMv->GetPos(K_AXIS_X)-tgScanPos.x)>0.01||fabs(m_pMv->GetPos(K_AXIS_Y)-tgScanPos.y)>0.01)
									{
										if(!SynchronizeMoveXY(tgScanPos.x,tgScanPos.y,true))
										{
											return false;
										}
									}
									if(fabs(m_pMv->GetPos(K_AXIS_ZA)-tgScanPos.za)>0.01)
									{
										if(!SynchronizeMoveZ(tgScanPos.za,true))
										{
											return false;
										}
									}
									Sleep(ProductParam.ScanDelay*1000);
									if(!g_pView->m_ImgStatic.CopyImage(RGB8))
									{
										return false;
									}
									if(!milApp.FindModel(true))
									{
										m_pPadDetect[nTempCount] = false;
									
									}
									else
									{
										m_pPadDetect[nTempCount] = true;
									}
									if(IsRunStop())
									{
										return false;
									}
								}
							}
						}
					}
					else
					{
						for(int j=ProductParam.FirstDispColumn-1;j>=0;j--)
						{
							for(int n=ProductParam.SecondDispColumn-1;n>=0;n--)
							{
								for(int l=ProductParam.ThirdDispColumn-1;l>=0;l--)
								{
									nTempCount = l+k*ProductParam.ThirdDispColumn+n*thirdNum+m*ProductParam.SecondDispColumn*thirdNum+
										j*secondNum*thirdNum+i*ProductParam.FirstDispColumn*secondNum*thirdNum;
									tgScanPos = GetPadCameraPos(i,j,m,n,k,l);
									if(fabs(m_pMv->GetPos(K_AXIS_X)-tgScanPos.x)>0.01||fabs(m_pMv->GetPos(K_AXIS_Y)-tgScanPos.y)>0.01)
									{
										if(!SynchronizeMoveXY(tgScanPos.x,tgScanPos.y,true))
										{
											return false;
										}
									}
									if(fabs(m_pMv->GetPos(K_AXIS_ZA)-tgScanPos.za)>0.01)
									{
										if(!SynchronizeMoveZ(tgScanPos.za,true))
										{
											return false;
										}
									}
									Sleep(ProductParam.ScanDelay*1000);
									if(!g_pView->m_ImgStatic.CopyImage(RGB8))
									{
										return false;
									}
									if(!milApp.FindModel(true))
									{
										m_pPadDetect[nTempCount] = false;
									
									}
									else
									{
										m_pPadDetect[nTempCount] = true;
									}
									if(IsRunStop())
									{
										return false;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return true;
}

void CCmdRun::PadDetect()
{
	static bool bRun=false;
	if(bRun)
	{
		return;
	}
	bRun=true;
	Sleep(2500);

	CString str;
	str.Format("���Զ�ģʽ��׼������ȱ�����������");
	PutLogIntoList(str);

	nTempLackStatic=0;//��ʼ��ȱ����Ŀ
	if(TRANSITION_INSPECT == ProductParam.inspectMode)      
	{
		str.Format("Only One PadDetect...");
		//AddMsg(str);
		PutLogIntoList(str);
		if(!g_pView->m_ImgStatic.m_pDetectCamera->IsConnected())
		{
			g_csPadDetect.Lock();  
			memset(m_pTransitionDetect,1,m_nPadNumber*sizeof(bool)); 
			g_csPadDetect.Unlock(); 
			Alarm(48);
		}
		else
		{
			if(!SpliceInspectUpGrade(false))
			{
				g_csPadDetect.Lock();  
				memset(m_pTransitionDetect,1,m_nPadNumber*sizeof(bool)); 
				g_csPadDetect.Unlock(); 
			}
		}
	}
	m_pMv->SetOutput(theApp.m_tSysParam.outDetectFinish,TRUE);
	CTimeUtil::Delay(500);
	m_pMv->SetOutput(theApp.m_tSysParam.outDetectFinish,FALSE);
	str.Format("���Զ�����->ȱ���������ɡ�");
	AddMsg(str);
	PutLogIntoList(str);
	str.Format("���Զ�����->ȫ��Ƭ�����ɡ�ȱ����Ŀ��%d ��",nTempLackStatic);
	AddMsg(str);
	PutLogIntoList(str);
	//g_csPadDetect.Unlock();
	bRun=false;
}

tgPos CCmdRun::GetFirstCameraPos()
{
	tgPos tgFirstPos;
	if(ProductParam.IrregularMatrix)
	{
		tgFirstPos = GetPadCameraPos(0,0);
	}
	else
	{
		tgFirstPos = GetPadCameraPos(0,0,0,0,0,0);
	}
	return tgFirstPos;
}

tgPos CCmdRun::GetLastCameraPos()
{
	tgPos tgLastPos;
	if(ONE_VALVE==ProductParam.valveSelect)
	{
		if(ProductParam.IrregularMatrix)
		{
			tgLastPos = GetPadCameraPos(ProductParam.IrregularRow-1,ProductParam.IrregularColumn-1);
		}
		else
		{
			tgLastPos = GetPadCameraPos(ProductParam.FirstDispRow-1,ProductParam.FirstDispColumn-1,ProductParam.SecondDispRow-1,ProductParam.SecondDispColumn-1,ProductParam.ThirdDispRow-1,ProductParam.ThirdDispColumn-1);
		}
	}
	else if(TWO_VALVE==ProductParam.valveSelect)
	{
		if(ProductParam.IrregularMatrix)
		{
			tgLastPos = GetPadCameraPos(ProductParam.IrregularRow-1,(ProductParam.IrregularColumn-1)/2);
		}
		else
		{
			if(ProductParam.FirstDispColumn>1)
			{
				tgLastPos = GetPadCameraPos(ProductParam.FirstDispRow-1,(ProductParam.FirstDispColumn-1)/2,ProductParam.SecondDispRow-1,ProductParam.SecondDispColumn-1,ProductParam.ThirdDispRow-1,ProductParam.ThirdDispColumn-1);
			}
			else if(ProductParam.SecondDispColumn>1)
			{
				tgLastPos = GetPadCameraPos(ProductParam.FirstDispRow-1,ProductParam.FirstDispColumn-1,ProductParam.SecondDispRow-1,(ProductParam.SecondDispColumn-1)/2,ProductParam.ThirdDispRow-1,ProductParam.ThirdDispColumn-1);
			}
			else
			{
				tgLastPos = GetPadCameraPos(ProductParam.FirstDispRow-1,ProductParam.FirstDispColumn-1,ProductParam.SecondDispRow-1,ProductParam.SecondDispColumn-1,ProductParam.ThirdDispRow-1,(ProductParam.ThirdDispColumn-1)/2);
			}
		}
	}
	else
	{
		if(ProductParam.IrregularMatrix)
		{
			tgLastPos = GetPadCameraPos(ProductParam.IrregularRow-1,(ProductParam.IrregularColumn-1)/3);
		}
		else
		{
			if(ProductParam.FirstDispColumn>1)
			{
				tgLastPos = GetPadCameraPos(ProductParam.FirstDispRow-1,(ProductParam.FirstDispColumn-1)/3,ProductParam.SecondDispRow-1,ProductParam.SecondDispColumn-1,ProductParam.ThirdDispRow-1,ProductParam.ThirdDispColumn-1);
			}
			else if(ProductParam.SecondDispColumn>1)
			{
				tgLastPos = GetPadCameraPos(ProductParam.FirstDispRow-1,ProductParam.FirstDispColumn-1,ProductParam.SecondDispRow-1,(ProductParam.SecondDispColumn-1)/3,ProductParam.ThirdDispRow-1,ProductParam.ThirdDispColumn-1);
			}
			else
			{
				tgLastPos = GetPadCameraPos(ProductParam.FirstDispRow-1,ProductParam.FirstDispColumn-1,ProductParam.SecondDispRow-1,ProductParam.SecondDispColumn-1,ProductParam.ThirdDispRow-1,(ProductParam.ThirdDispColumn-1)/3);
			}
		}
	}
	return tgLastPos;
}

bool CCmdRun::MoveToCenter()
{
	tgPos tgCurrentPos;
	tgCurrentPos.x = m_pMv->GetPos(K_AXIS_X);
	tgCurrentPos.y = m_pMv->GetPos(K_AXIS_Y);
	if(!g_pView->m_ImgStatic.CopyImage(RGB8))
	{
		return false;
	}
	CString strFile = g_pDoc->GetModeFile(0);
	//milApp.SetMatchMode(ProductParam.matchMode);
	milApp.RestoreModel(strFile.GetBuffer(strFile.GetLength()));
	milApp.SetPatParam(theApp.m_tSysParam.BmpMarkAcceptance,theApp.m_tSysParam.BmpMarkAngle);
	milApp.SetModelWindow(theApp.m_tSysParam.BmpMarkLearnWin[0]);
	milApp.SetSearchWindow(theApp.m_tSysParam.BmpMarkROI);
	if(!milApp.FindModel(true))
	{
		return false;
	}
	double dX,dY;
	theApp.m_tSysParam.BmpMarkCalibration.ConvertPixelToTable(milApp.m_stResult.dResultCenterX[0]-(milApp.m_lBufSizeX-1)/2,
		milApp.m_stResult.dResultCenterY[0]-(milApp.m_lBufSizeY-1)/2,dX,dY);
	tgCurrentPos.x -= dX;
	tgCurrentPos.y -= dY;

	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
	{
		return false;
	}
	return true;
}

BOOL CCmdRun::CheckHeightEx(int nAxisNo)
{
	if(nAxisNo<K_AXIS_ZA||nAxisNo>K_AXIS_ZC)
	{
		return FALSE;
	}
	double dHeight = 0.0;
	long lStatus;
	BOOL bHeightSensor = FALSE;
	double dPosSel=0.0;
	double dNewPos,dPosOffset;
	dPosOffset=0;
	CString str;
	if(theApp.m_tSysParam.sensorType==ContactSensorClose)
	{
		bHeightSensor = TRUE; 
	}
	else if(theApp.m_tSysParam.sensorType==ContactSensorOpen)
	{
		bHeightSensor = FALSE;
	}
	if(!MoveToZSafety())
	{
		return FALSE;
	}
	//:posing choose:
	if (nAxisNo==K_AXIS_ZA)
	{
		dPosSel=ProductParam.ContactSensorPos[0];
	}
	if (nAxisNo==K_AXIS_ZB)
	{
		dPosSel=ProductParam.ContactSensorPos[0]-(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].x-g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].x);
	}
	if (nAxisNo==K_AXIS_ZC)
	{
		dPosSel=ProductParam.ContactSensorPos[0]-(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].x-g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2].x);
	}
	//posing the target position;
	if(!SynchronizeMoveXY(dPosSel,ProductParam.ContactSensorPos[1],ProductParam.FreeVal,ProductParam.FreeAcc,true))
	{
		return FALSE;
	}
	dHeight = (g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[nAxisNo-1]-10)<40?(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[nAxisNo-1]-10):40;
	if(!MoveZ(nAxisNo,dHeight,5,20,false))
	{
		return FALSE;
	}
	theApp.m_Mv400.GetAxisSts(nAxisNo,&lStatus);
	while(lStatus&0x400)
	{
		if(theApp.m_Mv400.GetInput(theApp.m_tSysParam.inHeight)==bHeightSensor)
		{
			theApp.m_Mv400.Stop(nAxisNo);
			break;
		}
		theApp.m_Mv400.GetAxisSts(nAxisNo,&lStatus);
	}
	Sleep(100);
	if(!MoveZ(nAxisNo,theApp.m_tSysParam.tAxis[nAxisNo-1].maxpos,
		g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel,g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc,false))
	{
		return FALSE;
	}
	BOOL bMeasureSucceed = FALSE;
	theApp.m_Mv400.GetAxisSts(nAxisNo,&lStatus);
	while(lStatus&0x400)
	{
		if(theApp.m_Mv400.GetInput(theApp.m_tSysParam.inHeight)==bHeightSensor)
		{
			theApp.m_Mv400.Stop(nAxisNo);
			break;
		}
		theApp.m_Mv400.GetAxisSts(nAxisNo,&lStatus);
	}
	if(theApp.m_Mv400.GetInput(theApp.m_tSysParam.inHeight)==(!bHeightSensor))
	{
		return bMeasureSucceed;
	}
	//��������
	double dPos = theApp.m_Mv400.GetPos(nAxisNo);
	if(!MoveZ(nAxisNo,dPos-1.0,g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel,
		g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc,true))
	{
		return FALSE;
	}
	Sleep(100);
	if(!MoveZ(nAxisNo,theApp.m_tSysParam.tAxis[nAxisNo-1].maxpos,
		g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel/5,g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc/2,false))
	{
		return FALSE;
	}
	theApp.m_Mv400.GetAxisSts(nAxisNo,&lStatus);
	//����ƽ
	while(lStatus&0x400)
	{
		if(theApp.m_Mv400.GetInput(theApp.m_tSysParam.inHeight)==bHeightSensor)
		{
			theApp.m_Mv400.Stop(nAxisNo);
			Sleep(50);
			dNewPos=theApp.m_Mv400.GetPos(nAxisNo);
			dPosOffset=dNewPos-g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[nAxisNo-1];
			g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[nAxisNo-1] = dNewPos;
			bMeasureSucceed = TRUE;
			if (K_AXIS_ZA==nAxisNo)
			{
               str.Format("ZA���β��ֵ��%0.3f...",dNewPos);
			}
			if (K_AXIS_ZB==nAxisNo)
			{
				str.Format("ZB���β��ֵ��%0.3f...",dNewPos);
			}
			if (K_AXIS_ZC==nAxisNo)
			{
				str.Format("ZC���β��ֵ��%0.3f...",dNewPos);
			}
			PutLogIntoList(str);
			break;
		}
		theApp.m_Mv400.GetAxisSts(nAxisNo,&lStatus);
	}
	if(!SetAxisProfile(MEDIUM_VEL))
	{
		return FALSE;
	}
	if(!MoveToZSafety())
	{
		return FALSE;
	}
	////�ֱ��Զ�����Z����
	g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[nAxisNo-1]+= dPosOffset;                //��λ1
	theApp.m_tSysParam.CleanPosition[nAxisNo-1]+=dPosOffset;                               //��λ2
	if (nAxisNo==K_AXIS_ZA)
	{
		for(int i=0;i<g_pFrm->m_CmdRun.ProductParam.TestDotNumber;i++)                     //��λ3
		{
			g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].za+=dPosOffset;
		}
	}
	else if(nAxisNo==K_AXIS_ZB)
	{
		for(int i=0;i<g_pFrm->m_CmdRun.ProductParam.TestDotNumber;i++)                     //��λ3
		{
			g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].zb+=dPosOffset;
		}
	}
	else
	{
		for(int i=0;i<g_pFrm->m_CmdRun.ProductParam.TestDotNumber;i++)                     //��λ3
		{
			g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].zc+=dPosOffset;
		}
	}
	theApp.m_tSysParam.DischargeGluePostion[nAxisNo-1]+=dPosOffset;                        //��λ4
	theApp.ProductParam(FALSE);
	theApp.SysParam(FALSE);
	return bMeasureSucceed;
}

bool CCmdRun::SetAxisProfile(VelType type)
{
	CString str;
	MOTION_STATUS status;
	int nVelType = int(type);
	status = m_pMv->SetProfile(K_AXIS_X,theApp.m_tSysParam.tAxis[K_AXIS_X-1].vel[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_X-1].acc[nVelType],
		theApp.m_tSysParam.tAxis[K_AXIS_X-1].dec[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_X-1].jerk[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_X-1].velStart[nVelType]);
	if(MOT_STATUS_OK!=status)
	{
		str.Format("X�����ò���ʧ�ܣ�");
		AddMsg(str);
		PutLogIntoList(str);
		return false;
	}
	status = m_pMv->SetProfile(K_AXIS_Y,theApp.m_tSysParam.tAxis[K_AXIS_Y-1].vel[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_Y-1].acc[nVelType],
		theApp.m_tSysParam.tAxis[K_AXIS_Y-1].acc[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_Y-1].jerk[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_Y-1].velStart[nVelType]);
	if(MOT_STATUS_OK!=status)
	{
		str.Format("Y�����ò���ʧ�ܣ�");
		AddMsg(str);
		PutLogIntoList(str);
		return false;
	}
	status = m_pMv->SetProfile(K_AXIS_ZA,theApp.m_tSysParam.tAxis[K_AXIS_ZA-1].vel[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_ZA-1].acc[nVelType],
		theApp.m_tSysParam.tAxis[K_AXIS_ZA-1].acc[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_ZA-1].jerk[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_ZA-1].velStart[nVelType]);
	if(MOT_STATUS_OK!=status)
	{
		str.Format("ZA�����ò���ʧ�ܣ�");
		AddMsg(str);
		PutLogIntoList(str);
		return false;
	}
	status = m_pMv->SetProfile(K_AXIS_ZB,theApp.m_tSysParam.tAxis[K_AXIS_ZB-1].vel[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_ZB-1].acc[nVelType],
		theApp.m_tSysParam.tAxis[K_AXIS_ZB-1].acc[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_ZB-1].jerk[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_ZB-1].velStart[nVelType]);
	if(MOT_STATUS_OK!=status)
	{
		str.Format("ZB�����ò���ʧ�ܣ�");
		AddMsg(str);
		PutLogIntoList(str);
		return false;
	}
	status = m_pMv->SetProfile(K_AXIS_ZC,theApp.m_tSysParam.tAxis[K_AXIS_ZC-1].vel[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_ZC-1].acc[nVelType],
		theApp.m_tSysParam.tAxis[K_AXIS_ZC-1].acc[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_ZC-1].jerk[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_ZC-1].velStart[nVelType]);
	if(MOT_STATUS_OK!=status)
	{
		str.Format("ZC�����ò���ʧ�ܣ�");
		AddMsg(str);
		PutLogIntoList(str);
		return false;
	}
	return true;
}

bool CCmdRun::MoveToZSafety()
{
	if( !m_pMv->IsInitOK() ) return false;
	if( IsExisAlarm())	return false;
	if(!SetAxisProfile(MEDIUM_VEL))
	{
		AfxMessageBox("�趨�ٶ�ʧ�ܣ�");
		return false;
	}
	if(!SynchronizeMoveZ(0,0,0,false,true))
	{
		return false;
	}
	return true;
}

void CCmdRun::TestImage()
{
	CString str;
	tgPos tgStartImage,tgEndImage,tgImageCenter,tgAlignCenter,tgCurrentPos,tgOffset;
	double dX = 0.0,dY = 0.0,dAngle = 0.0;
	m_tOffset.x = 0.0;
	m_tOffset.y = 0.0;
	m_tOffset.za = 0.0;
	m_dRotateAngle = 0.0;
	m_tStatus = K_RUN_STS_RUN;
	if(!FindTwoMark())
	{
		m_tOffset.x = 0.0;
		m_tOffset.y = 0.0;
		m_dRotateAngle = 0.0;
		AddMsg("û���ҵ�ͼ��");
		m_bIsSupend = true;
		if(AfxMessageBox("ͼ�����ʧ�ܣ��Ƿ������",MB_YESNO)!=IDYES)
		{
			m_bIsSupend = false;
			return;
		}
		m_bIsSupend = false;
	}
	if(fabs(m_dRotateAngle)>ProductParam.MaxOffsetAngle)
	{
		m_bIsSupend = true;
		if(AfxMessageBox("ƫ�ƽǶȳ�������ĽǶȣ��Ƿ�������У�",MB_YESNO)!=IDYES)
		{
			m_tOffset.x = 0.0;
			m_tOffset.y = 0.0;
			m_dRotateAngle = 0.0;
			m_bIsSupend = false;
			return;
		}
		m_bIsSupend = false;
	}
	if(fabs(m_tOffset.x)>2||fabs(m_tOffset.y)>2)
	{
		m_bIsSupend = true;
		if(AfxMessageBox("λ��ƫ�Ƴ���2mm���Ƿ�������У�",MB_YESNO)!=IDYES)
		{
			m_tOffset.x = 0.0;
			m_tOffset.y = 0.0;
			m_dRotateAngle = 0.0;
			m_bIsSupend = false;
			return;
		}
		if(AfxMessageBox("�Ƿ����λ��ƫ�ƣ�",MB_YESNO)!=IDYES)
		{
			m_tOffset.x = 0.0;
			m_tOffset.y = 0.0;
			m_dRotateAngle = 0.0;
		}
		m_bIsSupend = false;
	}
	Sleep(1000);
	tgOffset.x = m_tOffset.x;
	tgOffset.y = m_tOffset.y;
	dAngle = m_dRotateAngle;
	tgStartImage = GetFirstCameraPos();
	tgEndImage = GetLastCameraPos();
	tgImageCenter.x = (tgStartImage.x+tgEndImage.x)/2;
	tgImageCenter.y = (tgStartImage.y+tgEndImage.y)/2;
	tgImageCenter.za = tgStartImage.za;
	tgAlignCenter.x = tgImageCenter.x - tgOffset.x;
	tgAlignCenter.y = tgImageCenter.y - tgOffset.y;
	tgAlignCenter.za = tgImageCenter.za;

	if(ProductParam.IrregularMatrix)
	{
		if(ProductParam.dispOrientation==COLUMN_DISP)
		{
			for(int i=0;i<(ProductParam.IrregularColumn+2)/3;i++)
			{
				if(!(i%2))
				{
					for(int j=0;j<ProductParam.IrregularRow;j++)
					{
						if(IsRunStop())
						{
							return;
						}
						tgCurrentPos = GetPadCameraPos(j,i);
						dX = tgCurrentPos.x - tgImageCenter.x;
						dY = tgCurrentPos.y - tgImageCenter.y;
						str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
						AddMsg(str);
						Rotation(dX,dY,-dAngle,dX,dY);
						tgCurrentPos.x = tgAlignCenter.x + dX;
						tgCurrentPos.y = tgAlignCenter.y + dY;
						str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
							tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
						AddMsg(str);
						tgCurrentPos.x += ProductParam.DispOffsetX;
						tgCurrentPos.y += ProductParam.DispOffsetY;
						if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
						{
							return;
						}
						//if(!SynchronizeMoveZ(tgCurrentPos.za,true))
						//{
						//	return;
						//}
						Sleep(1000);
					}
				}
				else
				{
					for(int j=ProductParam.IrregularRow-1;j>=0;j--)
					{
						if(IsRunStop())
						{
							return;
						}
						tgCurrentPos = GetPadCameraPos(j,i);
						dX = tgCurrentPos.x - tgImageCenter.x;
						dY = tgCurrentPos.y - tgImageCenter.y;
						str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
						AddMsg(str);
						Rotation(dX,dY,-dAngle,dX,dY);
						tgCurrentPos.x = tgAlignCenter.x + dX;
						tgCurrentPos.y = tgAlignCenter.y + dY;
						str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
							tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
						AddMsg(str);
						tgCurrentPos.x += ProductParam.DispOffsetX;
						tgCurrentPos.y += ProductParam.DispOffsetY;
						if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
						{
							return;
						}
						//if(!SynchronizeMoveZ(tgCurrentPos.za,true))
						//{
						//	return;
						//}
						Sleep(1000);
					}
				}
			}
		}
		else
		{
			for(int i=0;i<ProductParam.IrregularRow;i++)
			{
				if(!(i%2))
				{
					for(int j=0;j<(ProductParam.IrregularColumn+2)/3;j++)
					{
						if(IsRunStop())
						{
							return;
						}
						tgCurrentPos = GetPadCameraPos(i,j);
						dX = tgCurrentPos.x - tgImageCenter.x;
						dY = tgCurrentPos.y - tgImageCenter.y;
						str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
						AddMsg(str);
						Rotation(dX,dY,-dAngle,dX,dY);
						tgCurrentPos.x = tgAlignCenter.x + dX;
						tgCurrentPos.y = tgAlignCenter.y + dY;
						str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
							tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
						AddMsg(str);
						tgCurrentPos.x += ProductParam.DispOffsetX;
						tgCurrentPos.y += ProductParam.DispOffsetY;
						if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
						{
							return;
						}
						//if(!SynchronizeMoveZ(tgCurrentPos.za,true))
						//{
						//	return;
						//}
						Sleep(1000);
					}
				}
				else
				{
					for(int j=(ProductParam.IrregularColumn-1)/3;j>=0;j--)
					{
						if(IsRunStop())
						{
							return;
						}
						tgCurrentPos = GetPadCameraPos(i,j);
						dX = tgCurrentPos.x - tgImageCenter.x;
						dY = tgCurrentPos.y - tgImageCenter.y;
						str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
						AddMsg(str);
						Rotation(dX,dY,-dAngle,dX,dY);
						tgCurrentPos.x = tgAlignCenter.x + dX;
						tgCurrentPos.y = tgAlignCenter.y + dY;
						str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
							tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
						AddMsg(str);
						tgCurrentPos.x += ProductParam.DispOffsetX;
						tgCurrentPos.y += ProductParam.DispOffsetY;
						if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
						{
							return;
						}
						//if(!SynchronizeMoveZ(tgCurrentPos.za,true))
						//{
						//	return;
						//}
						Sleep(1000);
					}
				}
			}
		}
	}
	else
	{
		if(ProductParam.dispOrientation==COLUMN_DISP)
		{
			if(ProductParam.FirstDispColumn>1)
			{
				for(int i=0;i<(ProductParam.FirstDispColumn+2)/3;i++)
				{
					for(int m=0;m<ProductParam.SecondDispColumn;m++)
					{
						for(int k=0;k<ProductParam.ThirdDispColumn;k++)
						{
							if(!((i*ProductParam.SecondDispColumn*ProductParam.ThirdDispColumn+m*ProductParam.ThirdDispColumn+k)%2))
							{
								for(int j=0;j<ProductParam.FirstDispRow;j++)
								{
									for(int n=0;n<ProductParam.SecondDispRow;n++)
									{
										for(int l=0;l<ProductParam.ThirdDispRow;l++)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(j,i,n,m,l,k);
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											//if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											//{
											//	return;
											//}
											Sleep(1000);
										}
									}
								}
							}
							else
							{
								for(int j=ProductParam.FirstDispRow-1;j>=0;j--)
								{
									for(int n=ProductParam.SecondDispRow-1;n>=0;n--)
									{
										for(int l=ProductParam.ThirdDispRow-1;l>=0;l--)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(j,i,n,m,l,k);
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											//if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											//{
											//	return;
											//}
											Sleep(1000);
										}
									}
								}
							}
						}
					}
				}
			}
			else if(ProductParam.SecondDispColumn>1)
			{
				for(int i=0;i<ProductParam.FirstDispColumn;i++)
				{
					for(int m=0;m<(ProductParam.SecondDispColumn+2)/3;m++)
					{
						for(int k=0;k<ProductParam.ThirdDispColumn;k++)
						{
							if(!((i*ProductParam.SecondDispColumn*ProductParam.ThirdDispColumn+m*ProductParam.ThirdDispColumn+k)%2))
							{
								for(int j=0;j<ProductParam.FirstDispRow;j++)
								{
									for(int n=0;n<ProductParam.SecondDispRow;n++)
									{
										for(int l=0;l<ProductParam.ThirdDispRow;l++)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(j,i,n,m,l,k);
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											//if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											//{
											//	return;
											//}
											Sleep(1000);
										}
									}
								}
							}
							else
							{
								for(int j=ProductParam.FirstDispRow-1;j>=0;j--)
								{
									for(int n=ProductParam.SecondDispRow-1;n>=0;n--)
									{
										for(int l=ProductParam.ThirdDispRow-1;l>=0;l--)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(j,i,n,m,l,k);
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											//if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											//{
											//	return;
											//}
											Sleep(1000);
										}
									}
								}
							}
						}
					}
				}
			}
			else
			{
				for(int i=0;i<ProductParam.FirstDispColumn;i++)
				{
					for(int m=0;m<ProductParam.SecondDispColumn;m++)
					{
						for(int k=0;k<(ProductParam.ThirdDispColumn+2)/3;k++)
						{
							if(!((i*ProductParam.SecondDispColumn*ProductParam.ThirdDispColumn+m*ProductParam.ThirdDispColumn+k)%2))
							{
								for(int j=0;j<ProductParam.FirstDispRow;j++)
								{
									for(int n=0;n<ProductParam.SecondDispRow;n++)
									{
										for(int l=0;l<ProductParam.ThirdDispRow;l++)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(j,i,n,m,l,k);
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											//if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											//{
											//	return;
											//}
											Sleep(1000);
										}
									}
								}
							}
							else
							{
								for(int j=ProductParam.FirstDispRow-1;j>=0;j--)
								{
									for(int n=ProductParam.SecondDispRow-1;n>=0;n--)
									{
										for(int l=ProductParam.ThirdDispRow-1;l>=0;l--)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(j,i,n,m,l,k);
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											//if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											//{
											//	return;
											//}
											Sleep(1000);
										}
									}
								}
							}
						}
					}
				}
			}
		}
		else
		{
			if(ProductParam.FirstDispColumn>1)
			{
				for(int i=0;i<ProductParam.FirstDispRow;i++)
				{
					for(int m=0;m<ProductParam.SecondDispRow;m++)
					{
						for(int k=0;k<ProductParam.ThirdDispRow;k++)
						{
							if(!((i*ProductParam.SecondDispRow*ProductParam.ThirdDispRow+m*ProductParam.ThirdDispRow+k)%2))
							{
								for(int j=0;j<(ProductParam.FirstDispColumn+2)/3;j++)
								{
									for(int n=0;n<ProductParam.SecondDispColumn;n++)
									{
										for(int l=0;l<ProductParam.ThirdDispColumn;l++)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(i,j,m,n,k,l);
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											//if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											//{
											//	return;
											//}
											Sleep(1000);
										}
									}
								}
							}
							else
							{
								for(int j=(ProductParam.FirstDispColumn-1)/3;j>=0;j--)
								{
									for(int n=ProductParam.SecondDispColumn-1;n>=0;n--)
									{
										for(int l=ProductParam.ThirdDispColumn-1;l>=0;l--)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(i,j,m,n,k,l);
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											//if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											//{
											//	return;
											//}
											Sleep(1000);
										}
									}
								}
							}
						}
					}
				}
			}
			else if(ProductParam.SecondDispColumn>1)
			{
				for(int i=0;i<ProductParam.FirstDispRow;i++)
				{
					for(int m=0;m<ProductParam.SecondDispRow;m++)
					{
						for(int k=0;k<ProductParam.ThirdDispRow;k++)
						{
							if(!((i*ProductParam.SecondDispRow*ProductParam.ThirdDispRow+m*ProductParam.ThirdDispRow+k)%2))
							{
								for(int j=0;j<ProductParam.FirstDispColumn;j++)
								{
									for(int n=0;n<(ProductParam.SecondDispColumn+2)/3;n++)
									{
										for(int l=0;l<ProductParam.ThirdDispColumn;l++)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(i,j,m,n,k,l);
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											//if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											//{
											//	return;
											//}
											Sleep(1000);
										}
									}
								}
							}
							else
							{
								for(int j=ProductParam.FirstDispColumn-1;j>=0;j--)
								{
									for(int n=(ProductParam.SecondDispColumn-1)/3;n>=0;n--)
									{
										for(int l=ProductParam.ThirdDispColumn-1;l>=0;l--)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(i,j,m,n,k,l);
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											//if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											//{
											//	return;
											//}
											Sleep(1000);
										}
									}
								}
							}
						}
					}
				}
			}
			else
			{
				for(int i=0;i<ProductParam.FirstDispRow;i++)
				{
					for(int m=0;m<ProductParam.SecondDispRow;m++)
					{
						for(int k=0;k<ProductParam.ThirdDispRow;k++)
						{
							if(!((i*ProductParam.SecondDispRow*ProductParam.ThirdDispRow+m*ProductParam.ThirdDispRow+k)%2))
							{
								for(int j=0;j<ProductParam.FirstDispColumn;j++)
								{
									for(int n=0;n<ProductParam.SecondDispColumn;n++)
									{
										for(int l=0;l<(ProductParam.ThirdDispColumn+2)/3;l++)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(i,j,m,n,k,l);
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											//if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											//{
											//	return;
											//}
											Sleep(1000);
										}
									}
								}
							}
							else
							{
								for(int j=ProductParam.FirstDispColumn-1;j>=0;j--)
								{
									for(int n=ProductParam.SecondDispColumn-1;n>=0;n--)
									{
										for(int l=(ProductParam.ThirdDispColumn-1)/3;l>=0;l--)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(i,j,m,n,k,l);
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											//if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											//{
											//	return;
											//}
											Sleep(1000);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	m_tStatus = K_RUN_STS_STOP;
}

bool CCmdRun::RailMotor(bool bRun,bool bPositive)
{
	//CString str;
	//if(bRun)
	//{
	//	if(!m_pMv->IsMove(K_AXIS_O))
	//	{
	//		str.Format("������У�vel=%0.3f,acc=%0.3f,Positive=%d",theApp.m_tSysParam.tAxis[K_AXIS_O-1].vel[2],theApp.m_tSysParam.tAxis[K_AXIS_O-1].acc[2],bPositive);
	//		AddMsg(str);
	//		m_pMv->ContinuousMove(K_AXIS_O,theApp.m_tSysParam.tAxis[K_AXIS_O-1].vel[2],theApp.m_tSysParam.tAxis[K_AXIS_O-1].acc[2],bPositive);
	//	}
	//}
	//else
	//{
	//	if(m_pMv->IsMove(K_AXIS_O))
	//	{
	//		m_pMv->Stop(K_AXIS_O);
	//		AddMsg("�������ֹͣ");
	//	}
	//}
	return TRUE;
}

bool CCmdRun::CleanAndTest(bool bFill)
{
	if(bFill)
	{
		if(ProductParam.dispSequence == CLEAN_LATER_TEST)
		{
			if(ProductParam.FullClean)
			{
				if(!CleanNeedle(true))
				{
					return false;
				}
			}
			if(ProductParam.FullTestDot)
			{
				if(!TestDispense())
				{
					return false;
				}
			}
		}
		else if(ProductParam.dispSequence == TEST_LATER_CLEAN)
		{
			if(ProductParam.FullTestDot)
			{
				if(!TestDispense())
				{
					return false;
				}
			}
			if(ProductParam.FullClean)
			{
				if(!CleanNeedle(true))
				{
					return false;
				}
			}
		}
	}
	else
	{
		if(ProductParam.dispSequence == CLEAN_LATER_TEST)
		{
			if(ProductParam.CleanNd)
			{
				if(!CleanNeedle(true))
				{
					return false;
				}
			}
			if(ProductParam.FirstTest)
			{
				if(!TestDispense())
				{
					return false;
				}
			}
		}
		else if(ProductParam.dispSequence == TEST_LATER_CLEAN)
		{
			if(ProductParam.FirstTest)
			{
				if(!TestDispense())
				{
					return false;
				}
			}
			if(ProductParam.CleanNd)
			{
				if(!CleanNeedle(true))
				{
					return false;
				}
			}
		}
	}
	return true;
}

void CCmdRun::TestImageTwo()
{
	CString str;
	tgPos tgStartImage,tgEndImage,tgImageCenter,tgAlignCenter,tgCurrentPos,tgOffset;
	double dX = 0.0,dY = 0.0,dAngle = 0.0;
	m_tOffset.x = 0.0;
	m_tOffset.y = 0.0;
	m_tOffset.za = 0.0;
	m_dRotateAngle = 0.0;
	m_tStatus = K_RUN_STS_RUN;
	if(!FindImageOffset(m_tOffset.x,m_tOffset.y,m_dRotateAngle,false))
	{
		m_tOffset.x = 0.0;
		m_tOffset.y = 0.0;
		m_dRotateAngle = 0.0;
		AddMsg("û���ҵ�ͼ��");
		m_bIsSupend = true;
		if(AfxMessageBox("ͼ�����ʧ�ܣ��Ƿ������",MB_YESNO)!=IDYES)
		{
			m_bIsSupend = false;
			return;
		}
		m_bIsSupend = false;
	}
	Sleep(1000);
	tgOffset.x = m_tOffset.x;
	tgOffset.y = m_tOffset.y;
	dAngle = m_dRotateAngle;
	tgStartImage = GetFirstCameraPos();
	tgStartImage.za = ProductParam.tgImagePos[0].za;
	tgEndImage = GetLastCameraPos();
	tgEndImage.za = ProductParam.tgImagePos[1].za;
	tgImageCenter.x = (tgStartImage.x+tgEndImage.x)/2;
	tgImageCenter.y = (tgStartImage.y+tgEndImage.y)/2;
	tgImageCenter.za = ProductParam.tgImagePos[0].za;
	tgAlignCenter.x = tgImageCenter.x - tgOffset.x;
	tgAlignCenter.y = tgImageCenter.y - tgOffset.y;
	tgAlignCenter.za = ProductParam.tgImagePos[0].za;

	if(ProductParam.IrregularMatrix)
	{
		if(ProductParam.dispOrientation==COLUMN_DISP)
		{
			for(int i=0;i<(ProductParam.IrregularColumn+2)/3;i++)
			{
				if(!(i%2))
				{
					for(int j=0;j<ProductParam.IrregularRow;j++)
					{
						if(IsRunStop())
						{
							return;
						}
						tgCurrentPos = GetPadCameraPos(j,i);
						tgCurrentPos.x += ProductParam.DispOffsetX;
						tgCurrentPos.y += ProductParam.DispOffsetY;
						dX = tgCurrentPos.x - tgImageCenter.x;
						dY = tgCurrentPos.y - tgImageCenter.y;
						str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
						AddMsg(str);
						Rotation(dX,dY,-dAngle,dX,dY);
						tgCurrentPos.x = tgAlignCenter.x + dX;
						tgCurrentPos.y = tgAlignCenter.y + dY;
						str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
							tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
						AddMsg(str);
						if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
						{
							return;
						}
						if(!SynchronizeMoveZ(tgCurrentPos.za,true))
						{
							return;
						}
						Sleep(1000);
					}
				}
				else
				{
					for(int j=ProductParam.IrregularRow-1;j>=0;j--)
					{
						if(IsRunStop())
						{
							return;
						}
						tgCurrentPos = GetPadCameraPos(j,i);
						tgCurrentPos.x += ProductParam.DispOffsetX;
						tgCurrentPos.y += ProductParam.DispOffsetY;
						dX = tgCurrentPos.x - tgImageCenter.x;
						dY = tgCurrentPos.y - tgImageCenter.y;
						str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
						AddMsg(str);
						Rotation(dX,dY,-dAngle,dX,dY);
						tgCurrentPos.x = tgAlignCenter.x + dX;
						tgCurrentPos.y = tgAlignCenter.y + dY;
						str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
							tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
						AddMsg(str);
						if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
						{
							return;
						}
						if(!SynchronizeMoveZ(tgCurrentPos.za,true))
						{
							return;
						}
						Sleep(1000);
					}
				}
			}
		}
		else
		{
			for(int i=0;i<ProductParam.IrregularRow;i++)
			{
				if(!(i%2))
				{
					for(int j=0;j<(ProductParam.IrregularColumn+2)/3;j++)
					{
						if(IsRunStop())
						{
							return;
						}
						tgCurrentPos = GetPadCameraPos(i,j);
						tgCurrentPos.x += ProductParam.DispOffsetX;
						tgCurrentPos.y += ProductParam.DispOffsetY;
						dX = tgCurrentPos.x - tgImageCenter.x;
						dY = tgCurrentPos.y - tgImageCenter.y;
						str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
						AddMsg(str);
						Rotation(dX,dY,-dAngle,dX,dY);
						tgCurrentPos.x = tgAlignCenter.x + dX;
						tgCurrentPos.y = tgAlignCenter.y + dY;
						str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
							tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
						AddMsg(str);
						if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
						{
							return;
						}
						if(!SynchronizeMoveZ(tgCurrentPos.za,true))
						{
							return;
						}
						Sleep(1000);
					}
				}
				else
				{
					for(int j=(ProductParam.IrregularColumn-1)/3;j>=0;j--)
					{
						if(IsRunStop())
						{
							return;
						}
						tgCurrentPos = GetPadCameraPos(i,j);
						tgCurrentPos.x += ProductParam.DispOffsetX;
						tgCurrentPos.y += ProductParam.DispOffsetY;
						dX = tgCurrentPos.x - tgImageCenter.x;
						dY = tgCurrentPos.y - tgImageCenter.y;
						str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
						AddMsg(str);
						Rotation(dX,dY,-dAngle,dX,dY);
						tgCurrentPos.x = tgAlignCenter.x + dX;
						tgCurrentPos.y = tgAlignCenter.y + dY;
						str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
							tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
						AddMsg(str);
						if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
						{
							return;
						}
						if(!SynchronizeMoveZ(tgCurrentPos.za,true))
						{
							return;
						}
						Sleep(1000);
					}
				}
			}
		}
	}
	else
	{
		if(ProductParam.dispOrientation==COLUMN_DISP)
		{
			if(ProductParam.FirstDispColumn>1)
			{
				for(int i=0;i<(ProductParam.FirstDispColumn+2)/3;i++)
				{
					for(int m=0;m<ProductParam.SecondDispColumn;m++)
					{
						for(int k=0;k<ProductParam.ThirdDispColumn;k++)
						{
							if(!((i*ProductParam.SecondDispColumn*ProductParam.ThirdDispColumn+m*ProductParam.ThirdDispColumn+k)%2))
							{
								for(int j=0;j<ProductParam.FirstDispRow;j++)
								{
									for(int n=0;n<ProductParam.SecondDispRow;n++)
									{
										for(int l=0;l<ProductParam.ThirdDispRow;l++)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(j,i,n,m,l,k);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											{
												return;
											}
											Sleep(1000);
										}
									}
								}
							}
							else
							{
								for(int j=ProductParam.FirstDispRow-1;j>=0;j--)
								{
									for(int n=ProductParam.SecondDispRow-1;n>=0;n--)
									{
										for(int l=ProductParam.ThirdDispRow-1;l>=0;l--)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(j,i,n,m,l,k);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											{
												return;
											}
											Sleep(1000);
										}
									}
								}
							}
						}
					}
				}
			}
			else if(ProductParam.SecondDispColumn>1)
			{
				for(int i=0;i<ProductParam.FirstDispColumn;i++)
				{
					for(int m=0;m<(ProductParam.SecondDispColumn+2)/3;m++)
					{
						for(int k=0;k<ProductParam.ThirdDispColumn;k++)
						{
							if(!((i*ProductParam.SecondDispColumn*ProductParam.ThirdDispColumn+m*ProductParam.ThirdDispColumn+k)%2))
							{
								for(int j=0;j<ProductParam.FirstDispRow;j++)
								{
									for(int n=0;n<ProductParam.SecondDispRow;n++)
									{
										for(int l=0;l<ProductParam.ThirdDispRow;l++)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(j,i,n,m,l,k);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											{
												return;
											}
											Sleep(1000);
										}
									}
								}
							}
							else
							{
								for(int j=ProductParam.FirstDispRow-1;j>=0;j--)
								{
									for(int n=ProductParam.SecondDispRow-1;n>=0;n--)
									{
										for(int l=ProductParam.ThirdDispRow-1;l>=0;l--)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(j,i,n,m,l,k);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											{
												return;
											}
											Sleep(1000);
										}
									}
								}
							}
						}
					}
				}
			}
			else
			{
				for(int i=0;i<ProductParam.FirstDispColumn;i++)
				{
					for(int m=0;m<ProductParam.SecondDispColumn;m++)
					{
						for(int k=0;k<(ProductParam.ThirdDispColumn+2)/3;k++)
						{
							if(!((i*ProductParam.SecondDispColumn*ProductParam.ThirdDispColumn+m*ProductParam.ThirdDispColumn+k)%2))
							{
								for(int j=0;j<ProductParam.FirstDispRow;j++)
								{
									for(int n=0;n<ProductParam.SecondDispRow;n++)
									{
										for(int l=0;l<ProductParam.ThirdDispRow;l++)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(j,i,n,m,l,k);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											{
												return;
											}
											Sleep(1000);
										}
									}
								}
							}
							else
							{
								for(int j=ProductParam.FirstDispRow-1;j>=0;j--)
								{
									for(int n=ProductParam.SecondDispRow-1;n>=0;n--)
									{
										for(int l=ProductParam.ThirdDispRow-1;l>=0;l--)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(j,i,n,m,l,k);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											{
												return;
											}
											Sleep(1000);
										}
									}
								}
							}
						}
					}
				}
			}
		}
		else
		{
			if(ProductParam.FirstDispColumn>1)
			{
				for(int i=0;i<ProductParam.FirstDispRow;i++)
				{
					for(int m=0;m<ProductParam.SecondDispRow;m++)
					{
						for(int k=0;k<ProductParam.ThirdDispRow;k++)
						{
							if(!((i*ProductParam.SecondDispRow*ProductParam.ThirdDispRow+m*ProductParam.ThirdDispRow+k)%2))
							{
								for(int j=0;j<(ProductParam.FirstDispColumn+2)/3;j++)
								{
									for(int n=0;n<ProductParam.SecondDispColumn;n++)
									{
										for(int l=0;l<ProductParam.ThirdDispColumn;l++)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(i,j,m,n,k,l);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											{
												return;
											}
											Sleep(1000);
										}
									}
								}
							}
							else
							{
								for(int j=(ProductParam.FirstDispColumn-1)/3;j>=0;j--)
								{
									for(int n=ProductParam.SecondDispColumn-1;n>=0;n--)
									{
										for(int l=ProductParam.ThirdDispColumn-1;l>=0;l--)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(i,j,m,n,k,l);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											{
												return;
											}
											Sleep(1000);
										}
									}
								}
							}
						}
					}
				}
			}
			else if(ProductParam.SecondDispColumn>1)
			{
				for(int i=0;i<ProductParam.FirstDispRow;i++)
				{
					for(int m=0;m<ProductParam.SecondDispRow;m++)
					{
						for(int k=0;k<ProductParam.ThirdDispRow;k++)
						{
							if(!((i*ProductParam.SecondDispRow*ProductParam.ThirdDispRow+m*ProductParam.ThirdDispRow+k)%2))
							{
								for(int j=0;j<ProductParam.FirstDispColumn;j++)
								{
									for(int n=0;n<(ProductParam.SecondDispColumn+2)/3;n++)
									{
										for(int l=0;l<ProductParam.ThirdDispColumn;l++)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(i,j,m,n,k,l);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											{
												return;
											}
											Sleep(1000);
										}
									}
								}
							}
							else
							{
								for(int j=ProductParam.FirstDispColumn-1;j>=0;j--)
								{
									for(int n=(ProductParam.SecondDispColumn-1)/3;n>=0;n--)
									{
										for(int l=ProductParam.ThirdDispColumn-1;l>=0;l--)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(i,j,m,n,k,l);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											{
												return;
											}
											Sleep(1000);
										}
									}
								}
							}
						}
					}
				}
			}
			else
			{
				for(int i=0;i<ProductParam.FirstDispRow;i++)
				{
					for(int m=0;m<ProductParam.SecondDispRow;m++)
					{
						for(int k=0;k<ProductParam.ThirdDispRow;k++)
						{
							if(!((i*ProductParam.SecondDispRow*ProductParam.ThirdDispRow+m*ProductParam.ThirdDispRow+k)%2))
							{
								for(int j=0;j<ProductParam.FirstDispColumn;j++)
								{
									for(int n=0;n<ProductParam.SecondDispColumn;n++)
									{
										for(int l=0;l<(ProductParam.ThirdDispColumn+2)/3;l++)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(i,j,m,n,k,l);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											{
												return;
											}
											Sleep(1000);
										}
									}
								}
							}
							else
							{
								for(int j=ProductParam.FirstDispColumn-1;j>=0;j--)
								{
									for(int n=ProductParam.SecondDispColumn-1;n>=0;n--)
									{
										for(int l=(ProductParam.ThirdDispColumn-1)/3;l>=0;l--)
										{
											if(IsRunStop())
											{
												return;
											}
											tgCurrentPos = GetPadCameraPos(i,j,m,n,k,l);
											tgCurrentPos.x += ProductParam.DispOffsetX;
											tgCurrentPos.y += ProductParam.DispOffsetY;
											dX = tgCurrentPos.x - tgImageCenter.x;
											dY = tgCurrentPos.y - tgImageCenter.y;
											str.Format("tgCurrentPos(%0.3f,%0.3f),Offset(%0.3f,%0.3f)",tgCurrentPos.x,tgCurrentPos.y,dX,dY);
											AddMsg(str);
											Rotation(dX,dY,-dAngle,dX,dY);
											tgCurrentPos.x = tgAlignCenter.x + dX;
											tgCurrentPos.y = tgAlignCenter.y + dY;
											str.Format("CurrentPos(%0.3f,%0.3f),ImageCenter(%0.3f,%0.3f),tgOffset(%0.3f,%0.3f),Angle = %0.3f,Offset(%0.3f,%0.3f)",
												tgCurrentPos.x,tgCurrentPos.y,tgImageCenter.x,tgImageCenter.y,tgOffset.x,tgOffset.y,dAngle,dX,dY);
											AddMsg(str);
											if(!SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
											{
												return;
											}
											if(!SynchronizeMoveZ(tgCurrentPos.za,true))
											{
												return;
											}
											Sleep(1000);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	m_tStatus = K_RUN_STS_STOP;
}

bool CCmdRun::FindTwoImageOffset(tgPos tgStartPos,tgPos tgEndPos,double &dOffsetX,double &dOffsetY,double &dRotateAngle,bool bTest)
{
	CString strFile,strText;
	double dCenterX[2];
	double dCenterY[2];
	double dAngle[2];
	double dOffsetX1,dOffsetY1,dOffsetX2,dOffsetY2;
	double dBmpOffset = 0.5;
	tgPos tgEndBmpOffset,tgStartBmpOffset;
	bool bMamualAlign = false;
	tgEndBmpOffset.x = 0.0;
	tgEndBmpOffset.y = 0.0;
	tgStartBmpOffset.x = 0.0;
	tgStartBmpOffset.y = 0.0;
	dOffsetX1 = 0.0;
	dOffsetY1 = 0.0;
	dOffsetX2 = 0.0;
	dOffsetY2 = 0.0;
	if(!MoveToZSafety())
	{
		return false;
	}
	if(!bTest)
	{
		if(IsRunStop())
		{
			return false;
		}
	}
	if(!SynchronizeMoveXY(tgEndPos.x,tgEndPos.y,true))
	{
		return false;
	}
	if(!bTest)
	{
		if(IsRunStop())
		{
			return false;
		}
	}
	if(!SynchronizeMoveZ(tgEndPos.za,true))
	{
		return false;
	}
	if(!bTest)
	{
		if(IsRunStop())
		{
			return false;
		}
	}

	CString str;
	Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
	str.Format("start CopyImage");
	PutLogIntoList(str);
	g_pView->m_ImgStatic.CopyImage(RGB8);
	str.Format("start GetModeFile");
	PutLogIntoList(str);
	strFile = g_pDoc->GetModeFile(ProductParam.nImageNum-1);
	//milApp.SetMatchMode(ProductParam.matchMode);
	str.Format("start RestoreModel:&s",strFile);
	PutLogIntoList(str);
	milApp.RestoreModel(strFile.GetBuffer(strFile.GetLength()));
	str.Format("start SetPatParam");
	PutLogIntoList(str);
	milApp.SetPatParam(theApp.m_tSysParam.BmpMarkAcceptance,theApp.m_tSysParam.BmpMarkAngle);
	str.Format("start SetModelWindow");
	PutLogIntoList(str);
	milApp.SetModelWindow(theApp.m_tSysParam.BmpMarkLearnWin[ProductParam.nImageNum-1]);
	str.Format("start SetSearchWindow");
	PutLogIntoList(str);
	milApp.SetSearchWindow(theApp.m_tSysParam.BmpMarkROI);
	str.Format("start FindModel");
	PutLogIntoList(str);
	//if(!milApp.FindModel())
	//{
	//	AddMsg("ͼ��λ��2����쳣");
	//	return false;
	//}
	while(!milApp.FindModel(true))
	{
		if(!bTest)
		{
			if(IsRunStop())
			{
				return false;
			}
		}
		if(!SynchronizeMoveXY(tgEndPos.x-dBmpOffset,tgEndPos.y,true))
		{
			return false;
		}
		if(!bTest)
		{
			if(IsRunStop())
			{
				return false;
			}
		}
		Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
		g_pView->m_ImgStatic.CopyImage(RGB8);
		if(milApp.FindModel(true))
		{
			tgEndBmpOffset.x = -dBmpOffset;
			tgEndBmpOffset.y = 0.0;
			break;
		}
		if(!SynchronizeMoveXY(tgEndPos.x,tgEndPos.y-dBmpOffset,true))
		{
			return false;
		}
		if(!bTest)
		{
			if(IsRunStop())
			{
				return false;
			}
		}
		Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
		g_pView->m_ImgStatic.CopyImage(RGB8);
		if(milApp.FindModel(true))
		{
			tgEndBmpOffset.x = 0.0;
			tgEndBmpOffset.y = -dBmpOffset;
			break;
		}
		if(!SynchronizeMoveXY(tgEndPos.x+dBmpOffset,tgEndPos.y,true))
		{
			return false;
		}
		if(!bTest)
		{
			if(IsRunStop())
			{
				return false;
			}
		}
		Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
		g_pView->m_ImgStatic.CopyImage(RGB8);
		if(milApp.FindModel(true))
		{
			tgEndBmpOffset.x = dBmpOffset;
			tgEndBmpOffset.y = 0.0;
			break;
		}
		if(!SynchronizeMoveXY(tgEndPos.x,tgEndPos.y+dBmpOffset,true))
		{
			return false;
		}
		if(!bTest)
		{
			if(IsRunStop())
			{
				return false;
			}
		}
		Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
		g_pView->m_ImgStatic.CopyImage(RGB8);
		if(milApp.FindModel(true))
		{
			tgEndBmpOffset.x = 0.0;
			tgEndBmpOffset.y = dBmpOffset;
			break;
		}
		AddMsg("ͼ��λ��2����쳣");
		m_bIsSupend = true;
		if(AfxMessageBox("�ֶ�����ͼ��",MB_YESNO)!=IDYES)
		{
			m_bIsSupend = false;
			return false;
		}
		else
		{
			CDlgManualAlign manualAlign;
			if(IDOK!=manualAlign.DoModal())
			{
				m_bIsSupend = false;
				return false;
			}
			else
			{
				dOffsetX2 = tgEndPos.x - m_pMv->GetPos(K_AXIS_X);
				dOffsetY2 = tgEndPos.y - m_pMv->GetPos(K_AXIS_Y);
				tgEndBmpOffset.x = 0;
				tgEndBmpOffset.y = 0;
				bMamualAlign = true;
				m_bIsSupend = false;
				break;
			}
		}
		m_bIsSupend = false;
	}
	if(!bMamualAlign)
	{
		dCenterX[1] = milApp.m_stResult.dResultCenterX[0];
		dCenterY[1] = milApp.m_stResult.dResultCenterY[0];
		dAngle[1] = milApp.m_stResult.dResultAngle[0];
		strText.Format("���Ͻ�ͼ����ҽ��:CenterX=%.3f,CenterY=%.3f,Angle=%.3f,Score=%.3f",milApp.m_stResult.dResultCenterX[0],
			milApp.m_stResult.dResultCenterY[0],milApp.m_stResult.dResultAngle[0],milApp.m_stResult.dResultScore[0]);
		AddMsg(strText);
		theApp.m_tSysParam.BmpMarkCalibration.ConvertPixelToTable(dCenterX[1]-(milApp.m_lBufSizeX-1)/2,
			dCenterY[1]-(milApp.m_lBufSizeY-1)/2,dOffsetX2,dOffsetY2);
		dOffsetX2 -= tgEndBmpOffset.x;
		dOffsetY2 -= tgEndBmpOffset.y;
	}
	strText.Format("���Ͻ�ͼ��ƫ��:OffsetX=%.3f,OffsetY=%.3f",dOffsetX2,dOffsetY2);
	AddMsg(strText);

	bMamualAlign = false;
	if(!bTest)
	{
		if(IsRunStop())
		{
			return false;
		}
	}
	if(!SynchronizeMoveXY(tgStartPos.x,tgStartPos.y,true))
	{
		return false;
	}
	if(!bTest)
	{
		if(IsRunStop())
		{
			return false;
		}
	}
	//m_pMv->Move(K_AXIS_ZA,tgStartPos.z,ProductParam.FreeVal,ProductParam.FreeAcc,TRUE);
	//if(!SynchronizeMoveZ(tgStartPos.za,true))
	//{
	//	return false;
	//}
	strFile = g_pDoc->GetModeFile(0);
	//milApp.SetMatchMode(ProductParam.matchMode);
	milApp.RestoreModel(strFile.GetBuffer(strFile.GetLength()));
	milApp.SetPatParam(theApp.m_tSysParam.BmpMarkAcceptance,theApp.m_tSysParam.BmpMarkAngle);
	milApp.SetModelWindow(theApp.m_tSysParam.BmpMarkLearnWin[0]);
	milApp.SetSearchWindow(theApp.m_tSysParam.BmpMarkROI);
	Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
	g_pView->m_ImgStatic.CopyImage(RGB8);
	if(!bTest)
	{
		if(IsRunStop())
		{
			return false;
		}
	}
	while(!milApp.FindModel(true))
	{
		if(!SynchronizeMoveXY(tgStartPos.x-dBmpOffset,tgStartPos.y,true))
		{
			return false;
		}
		if(!bTest)
		{
			if(IsRunStop())
			{
				return false;
			}
		}
		Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
		g_pView->m_ImgStatic.CopyImage(RGB8);
		if(milApp.FindModel(true))
		{
			tgStartBmpOffset.x = -dBmpOffset;
			tgStartBmpOffset.y = 0.0;
			break;
		}
		if(!SynchronizeMoveXY(tgStartPos.x,tgStartPos.y-dBmpOffset,true))
		{
			return false;
		}
		if(!bTest)
		{
			if(IsRunStop())
			{
				return false;
			}
		}
		Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
		g_pView->m_ImgStatic.CopyImage(RGB8);
		if(milApp.FindModel(true))
		{
			tgStartBmpOffset.x = 0.0;
			tgStartBmpOffset.y = -dBmpOffset;
			break;
		}
		if(!SynchronizeMoveXY(tgStartPos.x+dBmpOffset,tgStartPos.y,true))
		{
			return false;
		}
		if(!bTest)
		{
			if(IsRunStop())
			{
				return false;
			}
		}
		Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
		g_pView->m_ImgStatic.CopyImage(RGB8);
		if(milApp.FindModel(true))
		{
			tgStartBmpOffset.x = dBmpOffset;
			tgStartBmpOffset.y = 0.0;
			break;
		}
		if(!SynchronizeMoveXY(tgStartPos.x,tgStartPos.y+dBmpOffset,true))
		{
			return false;
		}
		if(!bTest)
		{
			if(IsRunStop())
			{
				return false;
			}
		}
		Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
		g_pView->m_ImgStatic.CopyImage(RGB8);
		if(milApp.FindModel(true))
		{
			tgStartBmpOffset.x = 0.0;
			tgStartBmpOffset.y = dBmpOffset;
			break;
		}
		AddMsg("ͼ��λ��1����쳣");
		m_bIsSupend = true;
		if(AfxMessageBox("�ֶ�����ͼ��",MB_YESNO)!=IDYES)
		{
			m_bIsSupend = false;
			return false;
		}
		else
		{
			CDlgManualAlign manualAlign;
			if(IDOK!=manualAlign.DoModal())
			{
				m_bIsSupend = false;
				return false;
			}
			else
			{
				dOffsetX1 = tgStartPos.x - m_pMv->GetPos(K_AXIS_X);
				dOffsetY1 = tgStartPos.y - m_pMv->GetPos(K_AXIS_Y);
				tgStartBmpOffset.x = 0;
				tgStartBmpOffset.y = 0;
				bMamualAlign = true;
				m_bIsSupend = false;
				break;
			}
		}
		m_bIsSupend = false;
	}
	if(!bMamualAlign)
	{
		dCenterX[0] = milApp.m_stResult.dResultCenterX[0];
		dCenterY[0] = milApp.m_stResult.dResultCenterY[0];
		dAngle[0] = milApp.m_stResult.dResultAngle[0];
		strText.Format("���½ǽ�ͼ����ҽ��:CenterX=%.3f,CenterY=%.3f,Angle=%.3f,Score=%.3f",milApp.m_stResult.dResultCenterX[0],
			milApp.m_stResult.dResultCenterY[0],milApp.m_stResult.dResultAngle[0],milApp.m_stResult.dResultScore[0]);
		AddMsg(strText);

		theApp.m_tSysParam.BmpMarkCalibration.ConvertPixelToTable(dCenterX[0]-(milApp.m_lBufSizeX-1)/2,
			dCenterY[0]-(milApp.m_lBufSizeY-1)/2,dOffsetX1,dOffsetY1);
		dOffsetX1 -= tgStartBmpOffset.x;
		dOffsetY1 -= tgStartBmpOffset.y;
	}
	strText.Format("���½�ͼ��ƫ��:OffsetX=%.3f,OffsetY=%.3f",dOffsetX1,dOffsetY1);
	AddMsg(strText);

	dOffsetX = (dOffsetX1+dOffsetX2)/2;
	dOffsetY = (dOffsetY1+dOffsetY2)/2;
	dRotateAngle = CalculateAngle((tgEndPos.x-dOffsetX2)-(tgStartPos.x-dOffsetX1),(tgEndPos.y-dOffsetY2)-(tgStartPos.y-dOffsetY1))-
		CalculateAngle(tgEndPos.x-tgStartPos.x,tgEndPos.y-tgStartPos.y);
	return true;
}

bool CCmdRun::FindOneImageOffset(tgPos tgImagePos,double &dOffsetX,double &dOffsetY,bool bTest)
{
	CString strFile,strText;
	double dCenterX[2];
	double dCenterY[2];
	double dBmpOffset = 0.5;
	tgPos tgEndBmpOffset;
	bool bMamualAlign = false;
	tgEndBmpOffset.x = 0.0;
	tgEndBmpOffset.y = 0.0;
	if(!MoveToZSafety())
	{
		return false;
	}
	if(!bTest)
	{
		if(IsRunStop())
		{
			return false;
		}
	}

	if(!SynchronizeMoveXY(tgImagePos.x,tgImagePos.y,true))
	{
		return false;
	}
	if(!bTest)
	{
		if(IsRunStop())
		{
			return false;
		}
	}
	if(!SynchronizeMoveZ(tgImagePos.za,true))
	{
		return false;
	}
	if(!bTest)
	{
		if(IsRunStop())
		{
			return false;
		}
	}

	Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
	g_pView->m_ImgStatic.CopyImage(RGB8);
	strFile = g_pDoc->GetModeFile(0);
	//milApp.SetMatchMode(ProductParam.matchMode);
	milApp.RestoreModel(strFile.GetBuffer(strFile.GetLength()));
	milApp.SetPatParam(theApp.m_tSysParam.BmpMarkAcceptance,theApp.m_tSysParam.BmpMarkAngle);
	milApp.SetModelWindow(theApp.m_tSysParam.BmpMarkLearnWin[0]);
	milApp.SetSearchWindow(theApp.m_tSysParam.BmpMarkROI);
	//if(!milApp.FindModel())
	//{
	//	AddMsg("ͼ��λ��2����쳣");
	//	return false;
	//}
	while(!milApp.FindModel(true))
	{
		if(!SynchronizeMoveXY(tgImagePos.x-dBmpOffset,tgImagePos.y,true))
		{
			return false;
		}
		if(!bTest)
		{
			if(IsRunStop())
			{
				return false;
			}
		}
		Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
		g_pView->m_ImgStatic.CopyImage(RGB8);
		if(milApp.FindModel(true))
		{
			tgEndBmpOffset.x = -dBmpOffset;
			tgEndBmpOffset.y = 0.0;
			break;
		}
		if(!SynchronizeMoveXY(tgImagePos.x,tgImagePos.y-dBmpOffset,true))
		{
			return false;
		}
		if(!bTest)
		{
			if(IsRunStop())
			{
				return false;
			}
		}
		Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
		g_pView->m_ImgStatic.CopyImage(RGB8);
		if(milApp.FindModel(true))
		{
			tgEndBmpOffset.x = 0.0;
			tgEndBmpOffset.y = -dBmpOffset;
			break;
		}
		if(!SynchronizeMoveXY(tgImagePos.x+dBmpOffset,tgImagePos.y,true))
		{
			return false;
		}
		if(!bTest)
		{
			if(IsRunStop())
			{
				return false;
			}
		}
		Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
		g_pView->m_ImgStatic.CopyImage(RGB8);
		if(milApp.FindModel(true))
		{
			tgEndBmpOffset.x = dBmpOffset;
			tgEndBmpOffset.y = 0.0;
			break;
		}
		if(!SynchronizeMoveXY(tgImagePos.x+dBmpOffset,tgImagePos.y,true))
		{
			return false;
		}
		if(!bTest)
		{
			if(IsRunStop())
			{
				return false;
			}
		}
		Sleep(DWORD(ProductParam.dGrabImageDelay*1000));
		g_pView->m_ImgStatic.CopyImage(RGB8);
		if(milApp.FindModel(true))
		{
			tgEndBmpOffset.x = 0.0;
			tgEndBmpOffset.y = dBmpOffset;
			break;
		}
		AddMsg("ͼ��λ��2����쳣");
		m_bIsSupend = true;
		if(AfxMessageBox("�ֶ�����ͼ��",MB_YESNO)!=IDYES)
		{
			m_bIsSupend = false;
			return false;
		}
		else
		{
			CDlgManualAlign manualAlign;
			if(IDOK!=manualAlign.DoModal())
			{
				m_bIsSupend = false;
				return false;
			}
			else
			{
				dOffsetX = tgImagePos.x - m_pMv->GetPos(K_AXIS_X);
				dOffsetY = tgImagePos.y - m_pMv->GetPos(K_AXIS_Y);
				tgEndBmpOffset.x = 0;
				tgEndBmpOffset.y = 0;
				bMamualAlign = true;
				m_bIsSupend = false;
				break;
			}
		}
		m_bIsSupend = false;
	}
	if(!bMamualAlign)
	{
		dCenterX[1] = milApp.m_stResult.dResultCenterX[0];
		dCenterY[1] = milApp.m_stResult.dResultCenterY[0];
		strText.Format("���Ͻ�ͼ����ҽ��:CenterX=%.3f,CenterY=%.3f,Angle=%.3f,Score=%.3f",milApp.m_stResult.dResultCenterX[0],
			milApp.m_stResult.dResultCenterY[0],milApp.m_stResult.dResultAngle[0],milApp.m_stResult.dResultScore[0]);
		AddMsg(strText);
		theApp.m_tSysParam.BmpMarkCalibration.ConvertPixelToTable(dCenterX[1]-(milApp.m_lBufSizeX-1)/2,
			dCenterY[1]-(milApp.m_lBufSizeY-1)/2,dOffsetX,dOffsetY);
		dOffsetX -= tgEndBmpOffset.x;
		dOffsetY -= tgEndBmpOffset.y;
	}
	strText.Format("���Ͻ�ͼ��ƫ��:OffsetX=%.3f,OffsetY=%.3f",dOffsetX,dOffsetY);
	AddMsg(strText);
	return true;
}

bool CCmdRun::FindImageOffset(double &dOffsetX,double &dOffsetY,double &dAngle,bool bTest)
{
	dOffsetX = 0;
	dOffsetY = 0;
	dAngle = 0;
	CString str;
	double dTempAngle = 0.0;
	tgPos tgTempPos,tgStartPos,tgEndPos;
	if((VISION_TWO_MARK == g_pFrm->m_CmdRun.ProductParam.visionMode)&&(2==g_pFrm->m_CmdRun.ProductParam.nImageNum))
	{
		tgStartPos.x = g_pFrm->m_CmdRun.ProductParam.tgImagePos[0].x;
		tgStartPos.y = g_pFrm->m_CmdRun.ProductParam.tgImagePos[0].y;
		tgStartPos.za = g_pFrm->m_CmdRun.ProductParam.tgImagePos[0].za;
		tgEndPos.x = g_pFrm->m_CmdRun.ProductParam.tgImagePos[1].x;
		tgEndPos.y = g_pFrm->m_CmdRun.ProductParam.tgImagePos[1].y;
		tgEndPos.za = g_pFrm->m_CmdRun.ProductParam.tgImagePos[1].za;
		if(!FindTwoImageOffset(tgStartPos,tgEndPos,dOffsetX,dOffsetY,dAngle,bTest))
		{
			dOffsetX = 0;
			dOffsetY = 0;
			dAngle = 0;
			return false;
		}
	}
	else if((VISION_ONE_MARK == g_pFrm->m_CmdRun.ProductParam.visionMode)&&(1==g_pFrm->m_CmdRun.ProductParam.nImageNum))
	{
		if(!FindOneImageOffset(ProductParam.tgImagePos[0],dOffsetX,dOffsetY,bTest))
		{
			dOffsetX = 0;
			dOffsetY = 0;
			return false;
		}
	}
	return true;
}
bool CCmdRun::SingleComplexDispense()
{
	int nSelValve = 0;
	for(int i=0;i<3;i++)
	{
		if(i==0)
		{
			ProductParam.FSelectVal[i] = true;
			RunDispFSelect[i] = true;
		}
		else
		{
			ProductParam.FSelectVal[i] = false;
			RunDispFSelect[i] = false;
		}
	}
	ProductParam.DispFCount = 1;
	if(IsRunStop())
	{
		return false;
	}

	int secondNum = ProductParam.SecondDispColumn*ProductParam.SecondDispRow;
	int thirdNum = ProductParam.ThirdDispColumn*ProductParam.ThirdDispRow;

	//ִ��������
	if(m_bStepOne)
	{
		if(ProductParam.dispOrientation==COLUMN_DISP)//���ѡ���У����Ȼ����ߡ�
		{
			if(!m_bStartRun)
			{
				ProductParam.tgTrackInfo.m_nFirstCol = 0;
			}
			for(int i=ProductParam.tgTrackInfo.m_nFirstCol;i<ProductParam.FirstDispColumn;i++)
			{
				if(!m_bStartRun)
				{
					ProductParam.tgTrackInfo.m_nSecondCol = 0;
				}
				for(int j=ProductParam.tgTrackInfo.m_nSecondCol;j<ProductParam.SecondDispColumn;j++)
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nThirdCol = 0;
					}
					for(int k=ProductParam.tgTrackInfo.m_nThirdCol;k<ProductParam.ThirdDispColumn;k++)
					{
						/*int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*thirdNum+n*ProductParam.SecondDispColumn*thirdNum+
						i*secondNum*thirdNum+m*ProductParam.FirstDispColumn*secondNum*thirdNum;*/
						//����m=n=l=0�� ��Чģʽ��
						if(m_bStartRun)
						{
							m_bStartRun = false;
						}
						int nDisNumber = k+j*thirdNum+i*secondNum*thirdNum;

						if(!PDposting[nDisNumber].bIsHasPad)
						{
							RunDispFSelect[nSelValve] = false;
						}
						PDposting[nDisNumber].Dx -= ProductParam.NeedleGap*nSelValve;
						if(!m_pMv->WarnCheckPos(&PDposting[nDisNumber].Dx,&PDposting[nDisNumber].Dy,
							&PDposting[nDisNumber].Dza,&PDposting[nDisNumber].Dzb,&PDposting[nDisNumber].Dzc))
						{
							AfxMessageBox("��������г̷�Χ�������Ʒ�������趨");
							return false;
						}
						if(!SingleDotComplex(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
							PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc,true))
						{
							if(m_bDispenseSuccess)
							{
								if(RunDispFSelect[nSelValve])
								{
									PDposting[nDisNumber].bIsCompleted = true;
								}
								//ProductParam.tgTrackInfo.m_nThirdRow = l+1;
							}
							return false;
						}
						if(RunDispFSelect[nSelValve])
						{
							PDposting[nDisNumber].bIsCompleted = true;
						}
						RunDispFSelect[nSelValve] = true;					
						ProductParam.tgTrackInfo.m_nThirdCol = k+1;
					}
					if(m_bStartRun)
					{
						m_bStartRun = false;
					}
					ProductParam.tgTrackInfo.m_nSecondCol = j+1;
				}
				if(m_bStartRun)
				{
					m_bStartRun = false;
				}
				ProductParam.tgTrackInfo.m_nFirstCol = i+1;
			}
		}
		else
		{
			if(!m_bStartRun)
			{
				ProductParam.tgTrackInfo.m_nFirstRow = 0;
			}
			for(int m=ProductParam.tgTrackInfo.m_nFirstRow;m<ProductParam.FirstDispRow;m++)
			{
				if(!m_bStartRun)
				{
					ProductParam.tgTrackInfo.m_nSecondRow = 0;
				}

				for(int n=ProductParam.tgTrackInfo.m_nSecondRow;n<ProductParam.SecondDispRow;n++)
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nThirdRow = 0;
					}
					for(int l=ProductParam.tgTrackInfo.m_nThirdRow;l<ProductParam.ThirdDispRow;l++)
					{
						if(m_bStartRun)
						{
							m_bStartRun = false;
						}
						if(!m_bStartRun)
						{
							ProductParam.tgTrackInfo.m_nFirstCol = 0;
						}
						/*int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*thirdNum+n*ProductParam.SecondDispColumn*thirdNum+
						i*secondNum*thirdNum+m*ProductParam.FirstDispColumn*secondNum*thirdNum;*/
						//ִ��i=j=k=0�ĵ�Чģʽ��
						int nDisNumber = l*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*thirdNum+m*ProductParam.FirstDispColumn*secondNum*thirdNum;
						if(!PDposting[nDisNumber].bIsHasPad)
						{
							RunDispFSelect[nSelValve] = false;
						}
						PDposting[nDisNumber].Dx -= ProductParam.NeedleGap*nSelValve;
						if(!m_pMv->WarnCheckPos(&PDposting[nDisNumber].Dx,&PDposting[nDisNumber].Dy,
							&PDposting[nDisNumber].Dza,&PDposting[nDisNumber].Dzb,&PDposting[nDisNumber].Dzc))
						{
							AfxMessageBox("��������г̷�Χ�������Ʒ�������趨");
							return false;
						}
						if(!SingleDotComplex(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
							PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc,false))
						{
							if(m_bDispenseSuccess)
							{
								if(RunDispFSelect[nSelValve])
								{
									PDposting[nDisNumber].bIsCompleted = true;
								}
								//ProductParam.tgTrackInfo.m_nThirdCol = k+1;
							}
							return false;
						}
						if(RunDispFSelect[nSelValve])
						{
							PDposting[nDisNumber].bIsCompleted = true;
						}
						RunDispFSelect[nSelValve] = true;

						ProductParam.tgTrackInfo.m_nThirdRow = l+1;
					}
					if(m_bStartRun)
					{
						m_bStartRun = false;
					}
					ProductParam.tgTrackInfo.m_nSecondRow = n+1;
				}
				if(m_bStartRun)
				{
					m_bStartRun = false;
				}
				ProductParam.tgTrackInfo.m_nFirstRow = m+1;
			}
		}
	}

	m_bStepOne=false;
	m_bStartRun=true;

	if(!m_bStepOne)  //ִ�к�����
	{
		if(ProductParam.dispOrientation==COLUMN_DISP)//���ѡ���У����Ȼ����ߡ�
		{
			if(!m_bStartRun)
			{
				ProductParam.tgTrackInfo.m_nFirstRow = 0;
			}
			for(int m=ProductParam.tgTrackInfo.m_nFirstRow;m<ProductParam.FirstDispRow;m++)
			{
				if(!m_bStartRun)
				{
					ProductParam.tgTrackInfo.m_nSecondRow = 0;
				}

				for(int n=ProductParam.tgTrackInfo.m_nSecondRow;n<ProductParam.SecondDispRow;n++)
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nThirdRow = 0;
					}
					for(int l=ProductParam.tgTrackInfo.m_nThirdRow;l<ProductParam.ThirdDispRow;l++)
					{
						if(m_bStartRun)
						{
							m_bStartRun = false;
						}
						if(!m_bStartRun)
						{
							ProductParam.tgTrackInfo.m_nFirstCol = 0;
						}
						/*int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*thirdNum+n*ProductParam.SecondDispColumn*thirdNum+
						i*secondNum*thirdNum+m*ProductParam.FirstDispColumn*secondNum*thirdNum;*/
						//ִ��i=j=k=0�ĵ�Чģʽ��
						int nDisNumber = l*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*thirdNum+m*ProductParam.FirstDispColumn*secondNum*thirdNum;
						if(!PDposting[nDisNumber].bIsHasPad)
						{
							RunDispFSelect[nSelValve] = false;
						}
						PDposting[nDisNumber].Dx -= ProductParam.NeedleGap*nSelValve;
						if(!m_pMv->WarnCheckPos(&PDposting[nDisNumber].Dx,&PDposting[nDisNumber].Dy,
							&PDposting[nDisNumber].Dza,&PDposting[nDisNumber].Dzb,&PDposting[nDisNumber].Dzc))
						{
							AfxMessageBox("��������г̷�Χ�������Ʒ�������趨");
							return false;
						}
						if(!SingleDotComplex(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
							PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc,false))
						{
							if(m_bDispenseSuccess)
							{
								if(RunDispFSelect[nSelValve])
								{
									PDposting[nDisNumber].bIsCompleted = true;
								}
								//ProductParam.tgTrackInfo.m_nThirdCol = k+1;
							}
							return false;
						}
						if(RunDispFSelect[nSelValve])
						{
							PDposting[nDisNumber].bIsCompleted = true;
						}
						RunDispFSelect[nSelValve] = true;

						ProductParam.tgTrackInfo.m_nThirdRow = l+1;
					}
					if(m_bStartRun)
					{
						m_bStartRun = false;
					}
					ProductParam.tgTrackInfo.m_nSecondRow = n+1;
				}
				if(m_bStartRun)
				{
					m_bStartRun = false;
				}
				ProductParam.tgTrackInfo.m_nFirstRow = m+1;
			}
		}
		else
		{
			if(!m_bStartRun)
			{
				ProductParam.tgTrackInfo.m_nFirstCol = 0;
			}
			for(int i=ProductParam.tgTrackInfo.m_nFirstCol;i<ProductParam.FirstDispColumn;i++)
			{
				if(!m_bStartRun)
				{
					ProductParam.tgTrackInfo.m_nSecondCol = 0;
				}
				for(int j=ProductParam.tgTrackInfo.m_nSecondCol;j<ProductParam.SecondDispColumn;j++)
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nThirdCol = 0;
					}
					for(int k=ProductParam.tgTrackInfo.m_nThirdCol;k<ProductParam.ThirdDispColumn;k++)
					{
						/*int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*thirdNum+n*ProductParam.SecondDispColumn*thirdNum+
						i*secondNum*thirdNum+m*ProductParam.FirstDispColumn*secondNum*thirdNum;*/
						//����m=n=l=0�� ��Чģʽ��
						if(m_bStartRun)
						{
							m_bStartRun = false;
						}
						int nDisNumber = k+j*thirdNum+i*secondNum*thirdNum;

						if(!PDposting[nDisNumber].bIsHasPad)
						{
							RunDispFSelect[nSelValve] = false;
						}
						PDposting[nDisNumber].Dx -= ProductParam.NeedleGap*nSelValve;
						if(!m_pMv->WarnCheckPos(&PDposting[nDisNumber].Dx,&PDposting[nDisNumber].Dy,
							&PDposting[nDisNumber].Dza,&PDposting[nDisNumber].Dzb,&PDposting[nDisNumber].Dzc))
						{
							AfxMessageBox("��������г̷�Χ�������Ʒ�������趨");
							return false;
						}
						if(!SingleDotComplex(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
							PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc,true))
						{
							if(m_bDispenseSuccess)
							{
								if(RunDispFSelect[nSelValve])
								{
									PDposting[nDisNumber].bIsCompleted = true;
								}
								//ProductParam.tgTrackInfo.m_nThirdRow = l+1;
							}
							return false;
						}
						if(RunDispFSelect[nSelValve])
						{
							PDposting[nDisNumber].bIsCompleted = true;
						}
						RunDispFSelect[nSelValve] = true;					
						ProductParam.tgTrackInfo.m_nThirdCol = k+1;
					}
					if(m_bStartRun)
					{
						m_bStartRun = false;
					}
					ProductParam.tgTrackInfo.m_nSecondCol = j+1;
				}
				if(m_bStartRun)
				{
					m_bStartRun = false;
				}
				ProductParam.tgTrackInfo.m_nFirstCol = i+1;
			}
		}
	}
	m_bStepOne=true;
	return true;
}

bool CCmdRun::SingleDispense()
{
	int nSelValve = 0;
	for(int i=0;i<3;i++)
	{
		if(i==0)
		{
			ProductParam.FSelectVal[i] = true;
			RunDispFSelect[i] = true;
		}
		else
		{
			ProductParam.FSelectVal[i] = false;
			RunDispFSelect[i] = false;
		}
		//if(ProductParam.FSelectVal[i])
		//{
		//	RunDispFSelect[i] = true;
		//	nSelValve = i;
		//	break;
		//}
	}
	ProductParam.DispFCount = 1;
	//SynchronizeMoveXY(PDposting[0].Dx-ProductParam.NeedleGap*nSelValve,PDposting[0].Dy,true);
	//SynchronizeMoveZ(PDposting[0].Dz-ProductParam.dZupHigh,true);
	if(IsRunStop())
	{
		return false;
	}
	if(ProductParam.IrregularMatrix)
	{
		if(ProductParam.dispOrientation==COLUMN_DISP)
		{
			if(!m_bStartRun)
			{
				ProductParam.tgTrackInfo.m_nIrregularCol = 0;
			}
			for(int i=ProductParam.tgTrackInfo.m_nIrregularCol;i<ProductParam.IrregularColumn;i++)
			{
				if(!(i%2))
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nIrregularRow = 0;
					}
					for(int j=ProductParam.tgTrackInfo.m_nIrregularRow;j<ProductParam.IrregularRow;j++)
					{
						int nDisNumber = i+j*ProductParam.IrregularColumn;
						if(!PDposting[nDisNumber].bIsHasPad)
						{
							RunDispFSelect[nSelValve] = false;
						}
						PDposting[nDisNumber].Dx -= ProductParam.NeedleGap*nSelValve;
						if(!m_pMv->WarnCheckPos(&PDposting[nDisNumber].Dx,&PDposting[nDisNumber].Dy,
							&PDposting[nDisNumber].Dza,&PDposting[nDisNumber].Dzb,&PDposting[nDisNumber].Dzc))
						{
							AfxMessageBox("��������г̷�Χ�������Ʒ�������趨");
							return false;
						}
						if(!SingleDot(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
							PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
						{
							if(m_bDispenseSuccess)
							{
								if(RunDispFSelect[nSelValve])
								{
									PDposting[nDisNumber].bIsCompleted = true;
								}
								ProductParam.tgTrackInfo.m_nIrregularRow = j+1;
							}
							return false;
						}
						if(RunDispFSelect[nSelValve])
						{
							PDposting[nDisNumber].bIsCompleted = true;
						}
						RunDispFSelect[nSelValve] = true;
						ProductParam.tgTrackInfo.m_nIrregularRow = j+1;
					}
				}
				else
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nIrregularRow = ProductParam.IrregularRow;
					}
					for(int j=ProductParam.tgTrackInfo.m_nIrregularRow-1;j>=0;j--)
					{
						int nDisNumber = i+j*ProductParam.IrregularColumn;
						if(!PDposting[nDisNumber].bIsHasPad)
						{
							RunDispFSelect[nSelValve] = false;
						}
						PDposting[nDisNumber].Dx -= ProductParam.NeedleGap*nSelValve;
						if(!m_pMv->WarnCheckPos(&PDposting[nDisNumber].Dx,&PDposting[nDisNumber].Dy,
							&PDposting[nDisNumber].Dza,&PDposting[nDisNumber].Dzb,&PDposting[nDisNumber].Dzc))
						{
							AfxMessageBox("��������г̷�Χ�������Ʒ�������趨");
							return false;
						}
						if(!SingleDot(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
							PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
						{
							if(m_bDispenseSuccess)
							{
								if(RunDispFSelect[nSelValve])
								{
									PDposting[nDisNumber].bIsCompleted = true;
								}
								ProductParam.tgTrackInfo.m_nIrregularRow = j;
							}
							return false;
						}
						if(RunDispFSelect[nSelValve])
						{
							PDposting[nDisNumber].bIsCompleted = true;
						}
						RunDispFSelect[nSelValve] = true;
						ProductParam.tgTrackInfo.m_nIrregularRow = j;
					}
				}
				ProductParam.tgTrackInfo.m_nIrregularCol = i+1;
			}
		}
		else
		{
			if(!m_bStartRun)
			{
				ProductParam.tgTrackInfo.m_nIrregularRow = 0;
			}
			for(int i=ProductParam.tgTrackInfo.m_nIrregularRow;i<ProductParam.IrregularRow;i++)
			{
				if(!(i%2))
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nIrregularCol = 0;
					}
					for(int j=ProductParam.tgTrackInfo.m_nIrregularCol;j<ProductParam.IrregularColumn;j++)
					{
						int nDisNumber = j+i*ProductParam.IrregularColumn;
						if(!PDposting[nDisNumber].bIsHasPad)
						{
							RunDispFSelect[nSelValve] = false;
						}
						PDposting[nDisNumber].Dx -= ProductParam.NeedleGap*nSelValve;
						if(!m_pMv->WarnCheckPos(&PDposting[nDisNumber].Dx,&PDposting[nDisNumber].Dy,
							&PDposting[nDisNumber].Dza,&PDposting[nDisNumber].Dzb,&PDposting[nDisNumber].Dzc))
						{
							AfxMessageBox("��������г̷�Χ�������Ʒ�������趨");
							return false;
						}
						if(!SingleDot(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
							PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
						{
							if(m_bDispenseSuccess)
							{
								if(RunDispFSelect[nSelValve])
								{
									PDposting[nDisNumber].bIsCompleted = true;
								}
								ProductParam.tgTrackInfo.m_nIrregularCol = j+1;
							}
							return false;
						}
						if(RunDispFSelect[nSelValve])
						{
							PDposting[nDisNumber].bIsCompleted = true;
						}
						RunDispFSelect[nSelValve] = true;
						ProductParam.tgTrackInfo.m_nIrregularCol = j+1;
					}
				}
				else
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nIrregularCol = ProductParam.IrregularColumn;
					}
					for(int j=ProductParam.tgTrackInfo.m_nIrregularCol-1;j>=0;j--)
					{
						int nDisNumber = j+i*ProductParam.IrregularColumn;
						if(!PDposting[nDisNumber].bIsHasPad)
						{
							RunDispFSelect[nSelValve] = false;
						}
						PDposting[nDisNumber].Dx -= ProductParam.NeedleGap*nSelValve;
						if(!m_pMv->WarnCheckPos(&PDposting[nDisNumber].Dx,&PDposting[nDisNumber].Dy,
							&PDposting[nDisNumber].Dza,&PDposting[nDisNumber].Dzb,&PDposting[nDisNumber].Dzc))
						{
							AfxMessageBox("��������г̷�Χ�������Ʒ�������趨");
							return false;
						}
						if(!SingleDot(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
							PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
						{
							if(m_bDispenseSuccess)
							{
								if(RunDispFSelect[nSelValve])
								{
									PDposting[nDisNumber].bIsCompleted = true;
								}
								ProductParam.tgTrackInfo.m_nIrregularCol = j;
							}
							return false;
						}
						if(RunDispFSelect[nSelValve])
						{
							PDposting[nDisNumber].bIsCompleted = true;
						}
						RunDispFSelect[nSelValve] = true;
						ProductParam.tgTrackInfo.m_nIrregularCol = j;
					}
				}
				ProductParam.tgTrackInfo.m_nIrregularRow = i+1;
			}
		}
	}
	else
	{
		int secondNum = ProductParam.SecondDispColumn*ProductParam.SecondDispRow;
		int thirdNum = ProductParam.ThirdDispColumn*ProductParam.ThirdDispRow;
		if(ProductParam.dispOrientation==COLUMN_DISP)
		{
			if(!m_bStartRun)
			{
				ProductParam.tgTrackInfo.m_nFirstCol = 0;
			}
			for(int i=ProductParam.tgTrackInfo.m_nFirstCol;i<ProductParam.FirstDispColumn;i++)
			{
				if(!m_bStartRun)
				{
					ProductParam.tgTrackInfo.m_nSecondCol = 0;
				}
				for(int j=ProductParam.tgTrackInfo.m_nSecondCol;j<ProductParam.SecondDispColumn;j++)
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nThirdCol = 0;
					}
					for(int k=ProductParam.tgTrackInfo.m_nThirdCol;k<ProductParam.ThirdDispColumn;k++)
					{
						if(!((i*ProductParam.SecondDispColumn*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispColumn+k)%2))
						{
							if(!m_bStartRun)
							{
								ProductParam.tgTrackInfo.m_nFirstRow = 0;
							}
							for(int m=ProductParam.tgTrackInfo.m_nFirstRow;m<ProductParam.FirstDispRow;m++)
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nSecondRow = 0;
								}
								for(int n=ProductParam.tgTrackInfo.m_nSecondRow;n<ProductParam.SecondDispRow;n++)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nThirdRow = 0;
									}
									for(int l=ProductParam.tgTrackInfo.m_nThirdRow;l<ProductParam.ThirdDispRow;l++)
									{
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*thirdNum+n*ProductParam.SecondDispColumn*thirdNum+
											i*secondNum*thirdNum+m*ProductParam.FirstDispColumn*secondNum*thirdNum;
										if(!PDposting[nDisNumber].bIsHasPad)
										{
											RunDispFSelect[nSelValve] = false;
										}
										PDposting[nDisNumber].Dx -= ProductParam.NeedleGap*nSelValve;
										if(!m_pMv->WarnCheckPos(&PDposting[nDisNumber].Dx,&PDposting[nDisNumber].Dy,
											&PDposting[nDisNumber].Dza,&PDposting[nDisNumber].Dzb,&PDposting[nDisNumber].Dzc))
										{
											AfxMessageBox("��������г̷�Χ�������Ʒ�������趨");
											return false;
										}
										if(!SingleDot(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
											PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
										{
											if(m_bDispenseSuccess)
											{
												if(RunDispFSelect[nSelValve])
												{
													PDposting[nDisNumber].bIsCompleted = true;
												}
												ProductParam.tgTrackInfo.m_nThirdRow = l+1;
											}
											return false;
										}
										if(RunDispFSelect[nSelValve])
										{
											PDposting[nDisNumber].bIsCompleted = true;
										}
										RunDispFSelect[nSelValve] = true;
										ProductParam.tgTrackInfo.m_nThirdRow = l+1;
									}
									if(m_bStartRun)
									{
										m_bStartRun = false;
									}
									ProductParam.tgTrackInfo.m_nSecondRow = n+1;
								}
								if(m_bStartRun)
								{
									m_bStartRun = false;
								}
								ProductParam.tgTrackInfo.m_nFirstRow = m+1;
							}
						}
						else
						{
							if(!m_bStartRun)
							{
								ProductParam.tgTrackInfo.m_nFirstRow = ProductParam.FirstDispRow;
							}
							for(int m=ProductParam.tgTrackInfo.m_nFirstRow-1;m>=0;m--)
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nSecondRow = ProductParam.SecondDispRow;
								}
								for(int n=ProductParam.tgTrackInfo.m_nSecondRow-1;n>=0;n--)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nThirdRow = ProductParam.ThirdDispRow;
									}
									for(int l=ProductParam.tgTrackInfo.m_nThirdRow-1;l>=0;l--)
									{
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*thirdNum+n*ProductParam.SecondDispColumn*thirdNum+
											i*secondNum*thirdNum+m*ProductParam.FirstDispColumn*secondNum*thirdNum;
										if(!PDposting[nDisNumber].bIsHasPad)
										{
											RunDispFSelect[nSelValve] = false;
										}
										PDposting[nDisNumber].Dx -= ProductParam.NeedleGap*nSelValve;
										if(!m_pMv->WarnCheckPos(&PDposting[nDisNumber].Dx,&PDposting[nDisNumber].Dy,
											&PDposting[nDisNumber].Dza,&PDposting[nDisNumber].Dzb,&PDposting[nDisNumber].Dzc))
										{
											AfxMessageBox("��������г̷�Χ�������Ʒ�������趨");
											return false;
										}
										if(!SingleDot(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
											PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
										{
											if(m_bDispenseSuccess)
											{
												if(RunDispFSelect[nSelValve])
												{
													PDposting[nDisNumber].bIsCompleted = true;
												}
												ProductParam.tgTrackInfo.m_nThirdRow = l;
											}
											return false;
										}
										if(RunDispFSelect[nSelValve])
										{
											PDposting[nDisNumber].bIsCompleted = true;
										}
										RunDispFSelect[nSelValve] = true;
										ProductParam.tgTrackInfo.m_nThirdRow = l;
									}
									if(m_bStartRun)
									{
										m_bStartRun = false;
									}
									ProductParam.tgTrackInfo.m_nSecondRow = n;
								}
								if(m_bStartRun)
								{
									m_bStartRun = false;
								}
								ProductParam.tgTrackInfo.m_nFirstRow = m;
							}
						}
						ProductParam.tgTrackInfo.m_nThirdCol = k+1;
					}
					ProductParam.tgTrackInfo.m_nSecondCol = j+1;
				}
				ProductParam.tgTrackInfo.m_nFirstCol = i+1;
			}
		}
		else
		{
			if(!m_bStartRun)
			{
				ProductParam.tgTrackInfo.m_nFirstRow = 0;
			}
			for(int m=ProductParam.tgTrackInfo.m_nFirstRow;m<ProductParam.FirstDispRow;m++)
			{
				if(!m_bStartRun)
				{
					ProductParam.tgTrackInfo.m_nSecondRow = 0;
				}
				
				for(int n=ProductParam.tgTrackInfo.m_nSecondRow;n<ProductParam.SecondDispRow;n++)
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nThirdRow = 0;
					}
					for(int l=ProductParam.tgTrackInfo.m_nThirdRow;l<ProductParam.ThirdDispRow;l++)
					{
						if(!((m*ProductParam.SecondDispRow*ProductParam.ThirdDispRow+n*ProductParam.ThirdDispRow+l)%2))
						{
							if(!m_bStartRun)
							{
								ProductParam.tgTrackInfo.m_nFirstCol = 0;
							}
							for(int i=ProductParam.tgTrackInfo.m_nFirstCol;i<ProductParam.FirstDispColumn;i++)
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nSecondCol = 0;
								}
								for(int j=ProductParam.tgTrackInfo.m_nSecondCol;j<ProductParam.SecondDispColumn;j++)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nThirdCol = 0;
									}
									for(int k=ProductParam.tgTrackInfo.m_nThirdCol;k<ProductParam.ThirdDispColumn;k++)
									{
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*thirdNum+n*ProductParam.SecondDispColumn*thirdNum+
											i*secondNum*thirdNum+m*ProductParam.FirstDispColumn*secondNum*thirdNum;
										if(!PDposting[nDisNumber].bIsHasPad)
										{
											RunDispFSelect[nSelValve] = false;
										}
										PDposting[nDisNumber].Dx -= ProductParam.NeedleGap*nSelValve;
										if(!m_pMv->WarnCheckPos(&PDposting[nDisNumber].Dx,&PDposting[nDisNumber].Dy,
											&PDposting[nDisNumber].Dza,&PDposting[nDisNumber].Dzb,&PDposting[nDisNumber].Dzc))
										{
											AfxMessageBox("��������г̷�Χ�������Ʒ�������趨");
											return false;
										}
										if(!SingleDot(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
											PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
										{
											if(m_bDispenseSuccess)
											{
												if(RunDispFSelect[nSelValve])
												{
													PDposting[nDisNumber].bIsCompleted = true;
												}
												ProductParam.tgTrackInfo.m_nThirdCol = k+1;
											}
											return false;
										}
										if(RunDispFSelect[nSelValve])
										{
											PDposting[nDisNumber].bIsCompleted = true;
										}
										RunDispFSelect[nSelValve] = true;
										ProductParam.tgTrackInfo.m_nThirdCol = k+1;
									}
									if(m_bStartRun)
									{
										m_bStartRun = false;
									}
									ProductParam.tgTrackInfo.m_nSecondCol = j+1;
								}
								if(m_bStartRun)
								{
									m_bStartRun = false;
								}
								ProductParam.tgTrackInfo.m_nFirstCol = i+1;
							}
						}
						else
						{
							if(!m_bStartRun)
							{
								ProductParam.tgTrackInfo.m_nFirstCol = ProductParam.FirstDispColumn;
							}
							for(int i=ProductParam.tgTrackInfo.m_nFirstCol-1;i>=0;i--)
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nSecondCol = ProductParam.SecondDispColumn;
								}
								for(int j=ProductParam.tgTrackInfo.m_nSecondCol-1;j>=0;j--)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nThirdCol = ProductParam.ThirdDispColumn;
									}
									for(int k=ProductParam.tgTrackInfo.m_nThirdCol-1;k>=0;k--)
									{
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*thirdNum+n*ProductParam.SecondDispColumn*thirdNum+
											i*secondNum*thirdNum+m*ProductParam.FirstDispColumn*secondNum*thirdNum;
										if(!PDposting[nDisNumber].bIsHasPad)
										{
											RunDispFSelect[nSelValve] = false;
										}
										PDposting[nDisNumber].Dx -= ProductParam.NeedleGap*nSelValve;
										if(!m_pMv->WarnCheckPos(&PDposting[nDisNumber].Dx,&PDposting[nDisNumber].Dy,
											&PDposting[nDisNumber].Dza,&PDposting[nDisNumber].Dzb,&PDposting[nDisNumber].Dzc))
										{
											AfxMessageBox("��������г̷�Χ�������Ʒ�������趨");
											return false;
										}
										if(!SingleDot(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,PDposting[nDisNumber].Dza,
											PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
										{
											if(m_bDispenseSuccess)
											{
												if(RunDispFSelect[nSelValve])
												{
													PDposting[nDisNumber].bIsCompleted = true;
												}
												ProductParam.tgTrackInfo.m_nThirdCol = k;
											}
											return false;
										}
										if(RunDispFSelect[nSelValve])
										{
											PDposting[nDisNumber].bIsCompleted = true;
										}
										RunDispFSelect[nSelValve] = true;
										ProductParam.tgTrackInfo.m_nThirdCol = k;
									}
									if(m_bStartRun)
									{
										m_bStartRun = false;
									}
									ProductParam.tgTrackInfo.m_nSecondCol = j;
								}
								if(m_bStartRun)
								{
									m_bStartRun = false;
								}
								ProductParam.tgTrackInfo.m_nFirstCol = i;
							}
						}
						ProductParam.tgTrackInfo.m_nThirdRow = l+1;
					}
					ProductParam.tgTrackInfo.m_nSecondRow = n+1;
				}
				ProductParam.tgTrackInfo.m_nFirstRow = m+1;
			}
		}
	}
	return true;
}

bool CCmdRun::SecondDispense()
{
	g_pFrm->m_CmdRun.ProductParam.FSelectVal[2] = false;
	RunDispFSelect[2] = false;
	//SynchronizeMoveXY(PDposting[0].Dx,PDposting[0].Dy,true);
	//SynchronizeMoveZ(PDposting[0].Dz-ProductParam.dZupHigh,true);
	if(IsRunStop())
	{
		return false;
	}
	if(ProductParam.IrregularMatrix)
	{
		if(ProductParam.dispOrientation==COLUMN_DISP)
		{
			if(!m_bStartRun)
			{
				ProductParam.tgTrackInfo.m_nIrregularCol = 0;
			}
			for(int i=ProductParam.tgTrackInfo.m_nIrregularCol;i<(ProductParam.IrregularColumn+1)/2;i++)
			{
				if(!(i%2))
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nIrregularRow = 0;
					}
					for(int j=ProductParam.tgTrackInfo.m_nIrregularRow;j<ProductParam.IrregularRow;j++)
					{
						int nDisNumber = i+j*ProductParam.IrregularColumn;
						if((i+(ProductParam.IrregularColumn+1)/2)<ProductParam.IrregularColumn)
						{
							if(!g_pFrm->m_CmdRun.RunDispFSelect[1]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[1])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[1] = 1;
								g_pFrm->m_CmdRun.ProductParam.DispFCount++;
							}
						}
						else
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[1])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[1] = 0;
								g_pFrm->m_CmdRun.ProductParam.DispFCount--;
							}
						}
						for(int r=0;r<2;r++)
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[r])
							{
								if(!PDposting[nDisNumber+(ProductParam.IrregularColumn+1)/2*r].bIsHasPad)
								{
									g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
									g_pFrm->m_CmdRun.ProductParam.DispFCount--;
								}
							}
						}
						if(!SingleDot(nDisNumber))
						{
							if(m_bDispenseSuccess)
							{
								for(int r=0;r<2;r++)
								{
									if(g_pFrm->m_CmdRun.RunDispFSelect[r])
									{
										PDposting[nDisNumber+(ProductParam.IrregularColumn+1)/2*r].bIsCompleted = true;
									}
									ProductParam.tgTrackInfo.m_nIrregularRow = j+1;
								}
							}
							return false;
						}
						g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
						for(int r=0;r<2;r++)
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[r])
							{
								PDposting[nDisNumber+(ProductParam.IrregularColumn+1)/2*r].bIsCompleted = true;
							}
							if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
								g_pFrm->m_CmdRun.ProductParam.DispFCount++;
							}
						}
						ProductParam.tgTrackInfo.m_nIrregularRow = j+1;
					}
				}
				else
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nIrregularRow = ProductParam.IrregularRow;
					}
					for(int j=ProductParam.tgTrackInfo.m_nIrregularRow-1;j>=0;j--)
					{
						int nDisNumber = i+j*ProductParam.IrregularColumn;
						if((i+(ProductParam.IrregularColumn+1)/2)<ProductParam.IrregularColumn)
						{
							if(!g_pFrm->m_CmdRun.RunDispFSelect[1]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[1])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[1] = 1;
								g_pFrm->m_CmdRun.ProductParam.DispFCount++;
							}
						}
						else
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[1])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[1] = 0;
								g_pFrm->m_CmdRun.ProductParam.DispFCount--;
							}
						}
						for(int r=0;r<2;r++)
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[r])
							{
								if(!PDposting[nDisNumber+(ProductParam.IrregularColumn+1)/2*r].bIsHasPad)
								{
									g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
									g_pFrm->m_CmdRun.ProductParam.DispFCount--;
								}
							}
						}
						if(!SingleDot(nDisNumber))
						{
							if(m_bDispenseSuccess)
							{
								for(int r=0;r<2;r++)
								{
									if(g_pFrm->m_CmdRun.RunDispFSelect[r])
									{
										PDposting[nDisNumber+(ProductParam.IrregularColumn+1)/2*r].bIsCompleted = true;
									}
									ProductParam.tgTrackInfo.m_nIrregularRow = j;
								}
							}
							return false;
						}
						g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
						for(int r=0;r<2;r++)
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[r])
							{
								PDposting[nDisNumber+(ProductParam.IrregularColumn+1)/2*r].bIsCompleted = true;
							}
							if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
								g_pFrm->m_CmdRun.ProductParam.DispFCount++;
							}
						}
						ProductParam.tgTrackInfo.m_nIrregularRow = j;
					}
				}
				ProductParam.tgTrackInfo.m_nIrregularCol = i+1;
			}
		}
		else
		{
			for(int j=ProductParam.tgTrackInfo.m_nIrregularRow;j<ProductParam.IrregularRow;j++)
			{
				if(!(j%2))
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nIrregularCol = 0;
					}
					for(int i=ProductParam.tgTrackInfo.m_nIrregularCol;i<(ProductParam.IrregularColumn+1)/2;i++)
					{
						int nDisNumber = i+j*ProductParam.IrregularColumn;
						if((i+(ProductParam.IrregularColumn+1)/2)<ProductParam.IrregularColumn)
						{
							if(!g_pFrm->m_CmdRun.RunDispFSelect[1]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[1])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[1] = 1;
								g_pFrm->m_CmdRun.ProductParam.DispFCount++;
							}
						}
						else
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[1])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[1] = 0;
								g_pFrm->m_CmdRun.ProductParam.DispFCount--;
							}
						}
						for(int r=0;r<2;r++)
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[r])
							{
								if(!PDposting[nDisNumber+(ProductParam.IrregularColumn+1)/2*r].bIsHasPad)
								{
									g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
									g_pFrm->m_CmdRun.ProductParam.DispFCount--;
								}
							}
						}
						if(!SingleDot(nDisNumber))
						{
							if(m_bDispenseSuccess)
							{
								for(int r=0;r<2;r++)
								{
									if(g_pFrm->m_CmdRun.RunDispFSelect[r])
									{
										PDposting[nDisNumber+(ProductParam.IrregularColumn+1)/2*r].bIsCompleted = true;
									}
								}
								ProductParam.tgTrackInfo.m_nIrregularCol = i+1;
							}
							return false;
						}
						g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
						for(int r=0;r<2;r++)
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[r])
							{
								PDposting[nDisNumber+(ProductParam.IrregularColumn+1)/2*r].bIsCompleted = true;
							}
							if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
								g_pFrm->m_CmdRun.ProductParam.DispFCount++;
							}
						}
						ProductParam.tgTrackInfo.m_nIrregularCol = i+1;
					}
				}
				else
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nIrregularCol = ProductParam.IrregularColumn;
					}
					for(int i=ProductParam.tgTrackInfo.m_nIrregularCol-1;i>=0;i--)
					{
						int nDisNumber = i+j*ProductParam.IrregularColumn;
						if((i+(ProductParam.IrregularColumn+1)/2)<ProductParam.IrregularColumn)
						{
							if(!g_pFrm->m_CmdRun.RunDispFSelect[1]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[1])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[1] = 1;
								g_pFrm->m_CmdRun.ProductParam.DispFCount++;
							}
						}
						else
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[1])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[1] = 0;
								g_pFrm->m_CmdRun.ProductParam.DispFCount--;
							}
						}
						for(int r=0;r<2;r++)
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[r])
							{
								if(!PDposting[nDisNumber+(ProductParam.IrregularColumn+1)/2*r].bIsHasPad)
								{
									g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
									g_pFrm->m_CmdRun.ProductParam.DispFCount--;
								}
							}
						}
						if(!SingleDot(nDisNumber))
						{
							if(m_bDispenseSuccess)
							{
								for(int r=0;r<2;r++)
								{
									if(g_pFrm->m_CmdRun.RunDispFSelect[r])
									{
										PDposting[nDisNumber+(ProductParam.IrregularColumn+1)/2*r].bIsCompleted = true;
									}
								}
								ProductParam.tgTrackInfo.m_nIrregularCol = i;
							}
							return false;
						}
						g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
						for(int r=0;r<2;r++)
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[r])
							{
								PDposting[nDisNumber+(ProductParam.IrregularColumn+1)/2*r].bIsCompleted = true;
							}
							if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
								g_pFrm->m_CmdRun.ProductParam.DispFCount++;
							}
						}
						ProductParam.tgTrackInfo.m_nIrregularCol = i;
					}
				}
				ProductParam.tgTrackInfo.m_nIrregularRow = j+1;
			}
		}
	}
	else
	{
		if(ProductParam.dispOrientation==COLUMN_DISP)
		{
			if(ProductParam.FirstDispColumn>1)
			{
				if(!m_bStartRun)
				{
					ProductParam.tgTrackInfo.m_nFirstCol = 0;
				}
				for(int i=ProductParam.tgTrackInfo.m_nFirstCol;i<(ProductParam.FirstDispColumn+1)/2;i++)
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nSecondCol = 0;
					}
					for(int j=ProductParam.tgTrackInfo.m_nSecondCol;j<ProductParam.SecondDispColumn;j++)
					{
						if(!m_bStartRun)
						{
							ProductParam.tgTrackInfo.m_nThirdCol = 0;
						}
						for(int k=ProductParam.tgTrackInfo.m_nThirdCol;k<ProductParam.ThirdDispColumn;k++)
						{
							if(!((i*ProductParam.SecondDispColumn*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispColumn+k)%2))
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstRow = 0;
								}
								for(int m=ProductParam.tgTrackInfo.m_nFirstRow;m<ProductParam.FirstDispRow;m++)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondRow = 0;
									}
									for(int n=ProductParam.tgTrackInfo.m_nSecondRow;n<ProductParam.SecondDispRow;n++)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdRow = 0;
										}
										for(int l=ProductParam.tgTrackInfo.m_nThirdRow;l<ProductParam.ThirdDispRow;l++)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((i+(ProductParam.FirstDispColumn+1)/2)<ProductParam.FirstDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[1]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 1;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 0;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.FirstDispColumn+1)/2*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<2;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.FirstDispColumn+1)/2*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdRow = l+1;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.FirstDispColumn+1)/2*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdRow = l+1;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondRow = n+1;
									}
									ProductParam.tgTrackInfo.m_nFirstRow = m+1;
								}
							}
							else
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstRow = ProductParam.FirstDispRow;
								}
								for(int m=ProductParam.tgTrackInfo.m_nFirstRow-1;m>=0;m--)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondRow = ProductParam.SecondDispRow;
									}
									for(int n=ProductParam.tgTrackInfo.m_nSecondRow-1;n>=0;n--)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdRow = ProductParam.ThirdDispRow;
										}
										for(int l=ProductParam.tgTrackInfo.m_nThirdRow-1;l>=0;l--)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((i+(ProductParam.FirstDispColumn+1)/2)<ProductParam.FirstDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[1]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 1;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 0;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.FirstDispColumn+1)/2*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<2;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.FirstDispColumn+1)/2*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdRow = l;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.FirstDispColumn+1)/2*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdRow = l;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondRow = n;
									}
									ProductParam.tgTrackInfo.m_nFirstRow = m;
								}
							}
							ProductParam.tgTrackInfo.m_nThirdCol = k+1;
						}
						ProductParam.tgTrackInfo.m_nSecondCol = j+1;
					}
					ProductParam.tgTrackInfo.m_nFirstCol = i+1;
				}
			}
			else if(ProductParam.SecondDispColumn>1)
			{
				if(!m_bStartRun)
				{
					ProductParam.tgTrackInfo.m_nThirdCol = 0;
				}
				for(int i=ProductParam.tgTrackInfo.m_nFirstCol;i<ProductParam.FirstDispColumn;i++)
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nSecondCol = 0;
					}
					for(int j=ProductParam.tgTrackInfo.m_nSecondCol;j<(ProductParam.SecondDispColumn+1)/2;j++)
					{
						if(!m_bStartRun)
						{
							ProductParam.tgTrackInfo.m_nThirdCol = 0;
						}
						for(int k=ProductParam.tgTrackInfo.m_nThirdCol;k<ProductParam.ThirdDispColumn;k++)
						{
							if(!((i*(ProductParam.SecondDispColumn+1)/2*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispColumn+k)%2))
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstRow = 0;
								}
								for(int m=ProductParam.tgTrackInfo.m_nFirstRow;m<ProductParam.FirstDispRow;m++)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondRow = 0;
									}
									for(int n=ProductParam.tgTrackInfo.m_nSecondRow;n<ProductParam.SecondDispRow;n++)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdRow = 0;
										}
										for(int l=ProductParam.tgTrackInfo.m_nThirdRow;l<ProductParam.ThirdDispRow;l++)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((j+(ProductParam.SecondDispColumn+1)/2)<ProductParam.SecondDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[1]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 1;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 0;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.SecondDispColumn+1)/2*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<2;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.SecondDispColumn+1)/2*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdRow = l+1;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.SecondDispColumn+1)/2*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdRow = l+1;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondRow = n+1;
									}
									ProductParam.tgTrackInfo.m_nFirstRow = m+1;
								}
							}
							else
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstRow = ProductParam.FirstDispRow;
								}
								for(int m=ProductParam.tgTrackInfo.m_nFirstRow-1;m>=0;m--)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondRow = ProductParam.SecondDispRow;
									}
									for(int n=ProductParam.tgTrackInfo.m_nSecondRow-1;n>=0;n--)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdRow = ProductParam.ThirdDispRow;
										}
										for(int l=ProductParam.tgTrackInfo.m_nThirdRow-1;l>=0;l--)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((j+(ProductParam.SecondDispColumn+1)/2)<ProductParam.SecondDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[1]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 1;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 0;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.SecondDispColumn+1)/2*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<2;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.SecondDispColumn+1)/2*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdRow = l;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.SecondDispColumn+1)/2*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdRow = l;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondRow = n;
									}
									ProductParam.tgTrackInfo.m_nFirstRow = m;
								}
							}
							ProductParam.tgTrackInfo.m_nThirdCol = k+1;
						}
						ProductParam.tgTrackInfo.m_nSecondCol = j+1;
					}
					ProductParam.tgTrackInfo.m_nFirstCol = i+1;
				}
			}
			else
			{
				if(!m_bStartRun)
				{
					ProductParam.tgTrackInfo.m_nFirstCol = 0;
				}
				for(int i=ProductParam.tgTrackInfo.m_nFirstCol;i<ProductParam.FirstDispColumn;i++)
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nSecondCol = 0;
					}
					for(int j=ProductParam.tgTrackInfo.m_nSecondCol;j<ProductParam.SecondDispColumn;j++)
					{
						if(!m_bStartRun)
						{
							ProductParam.tgTrackInfo.m_nThirdCol = 0;
						}
						for(int k=ProductParam.tgTrackInfo.m_nThirdCol;k<(ProductParam.ThirdDispColumn+1)/2;k++)
						{
							if(!((i*ProductParam.SecondDispColumn*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispColumn+k)%2))
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstRow = 0;
								}
								for(int m=ProductParam.tgTrackInfo.m_nFirstRow;m<ProductParam.FirstDispRow;m++)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondRow = 0;
									}
									for(int n=ProductParam.tgTrackInfo.m_nSecondRow;n<ProductParam.SecondDispRow;n++)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdRow = 0;
										}
										for(int l=ProductParam.tgTrackInfo.m_nThirdRow;l<ProductParam.ThirdDispRow;l++)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((k+(ProductParam.ThirdDispColumn+1)/2)<ProductParam.ThirdDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[1]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 1;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 0;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.ThirdDispColumn+1)/2*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<2;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.ThirdDispColumn+1)/2*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdRow = l+1;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.ThirdDispColumn+1)/2*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdRow = l+1;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondRow = n+1;
									}
									ProductParam.tgTrackInfo.m_nFirstRow = m+1;
								}
							}
							else
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstRow = ProductParam.FirstDispRow;
								}
								for(int m=ProductParam.tgTrackInfo.m_nFirstRow-1;m>=0;m--)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondRow = ProductParam.SecondDispRow;
									}
									for(int n=ProductParam.tgTrackInfo.m_nSecondRow-1;n>=0;n--)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdRow = ProductParam.ThirdDispRow;
										}
										for(int l=ProductParam.tgTrackInfo.m_nThirdRow-1;l>=0;l--)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((k+(ProductParam.ThirdDispColumn+1)/2)<ProductParam.ThirdDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[1]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 1;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 0;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.ThirdDispColumn+1)/2*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<2;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.ThirdDispColumn+1)/2*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdRow = l;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.ThirdDispColumn+1)/2*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdRow = l;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondRow = n;
									}
									ProductParam.tgTrackInfo.m_nFirstRow = m;
								}
							}
							ProductParam.tgTrackInfo.m_nThirdCol = k+1;
						}
						ProductParam.tgTrackInfo.m_nSecondCol = j+1;
					}
					ProductParam.tgTrackInfo.m_nFirstCol = i+1;
				}
			}
		}
		else
		{
			if(ProductParam.FirstDispColumn>1)
			{
				if(!m_bStartRun)
				{
					ProductParam.tgTrackInfo.m_nFirstRow = 0;
				}
				for(int m=ProductParam.tgTrackInfo.m_nFirstRow;m<ProductParam.FirstDispRow;m++)
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nSecondRow = 0;
					}
					for(int n=ProductParam.tgTrackInfo.m_nSecondRow;n<ProductParam.SecondDispRow;n++)
					{
						if(!m_bStartRun)
						{
							ProductParam.tgTrackInfo.m_nThirdRow = 0;
						}
						for(int l=ProductParam.tgTrackInfo.m_nThirdRow;l<ProductParam.ThirdDispRow;l++)
						{
							if(!((m*ProductParam.SecondDispRow*ProductParam.ThirdDispRow+n*ProductParam.ThirdDispRow+l)%2))
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstCol = 0;
								}
								for(int i=ProductParam.tgTrackInfo.m_nFirstCol;i<(ProductParam.FirstDispColumn+1)/2;i++)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondCol = 0;
									}
									for(int j=ProductParam.tgTrackInfo.m_nSecondCol;j<ProductParam.SecondDispColumn;j++)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdCol = 0;
										}
										for(int k=ProductParam.tgTrackInfo.m_nThirdCol;k<ProductParam.ThirdDispColumn;k++)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((i+(ProductParam.FirstDispColumn+1)/2)<ProductParam.FirstDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[1]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 1;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 0;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.FirstDispColumn+1)/2*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<2;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.FirstDispColumn+1)/2*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdCol = k+1;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.FirstDispColumn+1)/2*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdCol = k+1;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondCol = j+1;
									}
									ProductParam.tgTrackInfo.m_nFirstCol = i+1;
								}
							}
							else
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstCol = (ProductParam.FirstDispColumn+1)/2;
								}
								for(int i=ProductParam.tgTrackInfo.m_nFirstCol-1;i>=0;i--)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondCol = ProductParam.SecondDispColumn;
									}
									for(int j=ProductParam.tgTrackInfo.m_nSecondCol-1;j>=0;j--)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdCol = ProductParam.ThirdDispColumn;
										}
										for(int k=ProductParam.tgTrackInfo.m_nThirdCol-1;k>=0;k--)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((i+(ProductParam.FirstDispColumn+1)/2)<ProductParam.FirstDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[1]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 1;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 0;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.FirstDispColumn+1)/2*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<2;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.FirstDispColumn+1)/2*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdCol = k;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.FirstDispColumn+1)/2*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdCol = k;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondCol = j;
									}
									ProductParam.tgTrackInfo.m_nFirstCol = i;
								}
							}
							ProductParam.tgTrackInfo.m_nThirdRow = l+1;
						}
						ProductParam.tgTrackInfo.m_nSecondRow = n+1;
					}
					ProductParam.tgTrackInfo.m_nFirstRow = m+1;
				}
			}
			else if(ProductParam.SecondDispColumn>1)
			{
				if(!m_bStartRun)
				{
					ProductParam.tgTrackInfo.m_nFirstRow = 0;
				}
				for(int m=ProductParam.tgTrackInfo.m_nFirstRow;m<ProductParam.FirstDispRow;m++)
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nSecondRow = 0;
					}
					for(int n=ProductParam.tgTrackInfo.m_nSecondRow;n<ProductParam.SecondDispRow;n++)
					{
						if(!m_bStartRun)
						{
							ProductParam.tgTrackInfo.m_nThirdRow = 0;
						}
						for(int l=ProductParam.tgTrackInfo.m_nThirdRow;l<ProductParam.ThirdDispRow;l++)
						{
							if(!((m*ProductParam.SecondDispRow*ProductParam.ThirdDispRow+n*ProductParam.ThirdDispRow+l)%2))
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstCol = 0;
								}
								for(int i=ProductParam.tgTrackInfo.m_nFirstCol;i<ProductParam.FirstDispColumn;i++)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondCol = 0;
									}
									for(int j=ProductParam.tgTrackInfo.m_nSecondCol;j<(ProductParam.SecondDispColumn+1)/2;j++)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdCol = 0;
										}
										for(int k=ProductParam.tgTrackInfo.m_nThirdCol;k<ProductParam.ThirdDispColumn;k++)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((j+(ProductParam.SecondDispColumn+1)/2)<ProductParam.SecondDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[1]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 1;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 0;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.SecondDispColumn+1)/2*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<2;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.SecondDispColumn+1)/2*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdCol = k+1;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.SecondDispColumn+1)/2*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdCol = k+1;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondCol = j+1;
									}
									ProductParam.tgTrackInfo.m_nFirstCol = i+1;
								}
							}
							else
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstCol = ProductParam.FirstDispColumn;
								}
								for(int i=ProductParam.tgTrackInfo.m_nFirstCol-1;i>=0;i--)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondCol = (ProductParam.SecondDispColumn+1)/2;
									}
									for(int j=ProductParam.tgTrackInfo.m_nSecondCol-1;j>=0;j--)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdCol = ProductParam.ThirdDispColumn;
										}
										for(int k=ProductParam.tgTrackInfo.m_nThirdCol-1;k>=0;k--)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((j+(ProductParam.SecondDispColumn+1)/2)<ProductParam.SecondDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[1]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 1;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 0;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.SecondDispColumn+1)/2*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<2;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.SecondDispColumn+1)/2*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdCol = k;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.SecondDispColumn+1)/2*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdCol = k;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondCol = j;
									}
									ProductParam.tgTrackInfo.m_nFirstCol = i;
								}
							}
							ProductParam.tgTrackInfo.m_nThirdRow = l+1;
						}
						ProductParam.tgTrackInfo.m_nSecondRow = n+1;
					}
					ProductParam.tgTrackInfo.m_nFirstRow = m+1;
				}
			}
			else
			{
				if(!m_bStartRun)
				{
					ProductParam.tgTrackInfo.m_nFirstRow = 0;
				}
				for(int m=ProductParam.tgTrackInfo.m_nFirstRow;m<ProductParam.FirstDispRow;m++)
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nSecondRow = 0;
					}
					for(int n=ProductParam.tgTrackInfo.m_nSecondRow;n<ProductParam.SecondDispRow;n++)
					{
						if(!m_bStartRun)
						{
							ProductParam.tgTrackInfo.m_nThirdRow = 0;
						}
						for(int l=ProductParam.tgTrackInfo.m_nThirdRow;l<ProductParam.ThirdDispRow;l++)
						{
							if(!((m*ProductParam.SecondDispRow*ProductParam.ThirdDispRow+n*ProductParam.ThirdDispRow+l)%2))
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstCol = 0;
								}
								for(int i=ProductParam.tgTrackInfo.m_nFirstCol;i<ProductParam.FirstDispColumn;i++)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondCol = 0;
									}
									for(int j=ProductParam.tgTrackInfo.m_nSecondCol;j<ProductParam.SecondDispColumn;j++)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdCol = 0;
										}
										for(int k=ProductParam.tgTrackInfo.m_nThirdCol;k<(ProductParam.ThirdDispColumn+1)/2;k++)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((k+(ProductParam.ThirdDispColumn+1)/2)<ProductParam.ThirdDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[1]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 1;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 0;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.ThirdDispColumn+1)/2*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<2;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.ThirdDispColumn+1)/2*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdCol = k+1;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.ThirdDispColumn+1)/2*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdCol = k+1;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondCol = j+1;
									}
									ProductParam.tgTrackInfo.m_nFirstCol = i+1;
								}
							}
							else
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstCol = ProductParam.FirstDispColumn;
								}
								for(int i=ProductParam.tgTrackInfo.m_nFirstCol-1;i>=0;i--)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondCol = ProductParam.SecondDispColumn;
									}
									for(int j=ProductParam.tgTrackInfo.m_nSecondCol-1;j>=0;j--)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdCol = (ProductParam.ThirdDispColumn+1)/2;
										}
										for(int k=ProductParam.tgTrackInfo.m_nThirdCol-1;k>=0;k--)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((k+(ProductParam.ThirdDispColumn+1)/2)<ProductParam.ThirdDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[1]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 1;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[1])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[1] = 0;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.ThirdDispColumn+1)/2*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<2;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.ThirdDispColumn+1)/2*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdCol = k;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<2;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.ThirdDispColumn+1)/2*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdCol = k;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondCol = j;
									}
									if(m_bStartRun)
									{
										m_bStartRun = false;
									}
									ProductParam.tgTrackInfo.m_nFirstCol = i;
								}
							}
							ProductParam.tgTrackInfo.m_nThirdRow = l+1;
						}
						ProductParam.tgTrackInfo.m_nSecondRow = n+1;
					}
					ProductParam.tgTrackInfo.m_nFirstRow = m+1;
				}
			}
		}
	}
	return true;
}

bool CCmdRun::ThirdDispense()
{
	//if(!SynchronizeMoveXY(PDposting[0].Dx,PDposting[0].Dy,true))
	//{
	//	return false;
	//}
	//if(!SynchronizeMoveZ(PDposting[0].Dz-ProductParam.dZupHigh,true))
	//{
	//	return false;
	//}
	if(IsRunStop())
	{
		return false;
	}
	if(ProductParam.IrregularMatrix)
	{
		if(ProductParam.dispOrientation==COLUMN_DISP)
		{
			if(!m_bStartRun)
			{
				ProductParam.tgTrackInfo.m_nIrregularCol = 0;
			}
			for(int i=ProductParam.tgTrackInfo.m_nIrregularCol;i<(ProductParam.IrregularColumn+2)/3;i++)
			{
				if(!(i%2))
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nIrregularRow = 0;
					}
					for(int j=ProductParam.tgTrackInfo.m_nIrregularRow;j<ProductParam.IrregularRow;j++)
					{
						int nDisNumber = i+j*ProductParam.IrregularColumn;
						if((i+(ProductParam.IrregularColumn+2)/3*2)<ProductParam.IrregularColumn)
						{
							if(!g_pFrm->m_CmdRun.RunDispFSelect[2]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[2])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[2] = 1;
								g_pFrm->m_CmdRun.ProductParam.DispFCount++;
							}
						}
						else
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[2])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[2] = 0;
								g_pFrm->m_CmdRun.ProductParam.DispFCount--;
							}
						}
						for(int r=0;r<3;r++)
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[r])
							{
								if(!PDposting[nDisNumber+(ProductParam.IrregularColumn+2)/3*r].bIsHasPad)
								{
									g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
									g_pFrm->m_CmdRun.ProductParam.DispFCount--;
								}
							}
						}
						if(!SingleDot(nDisNumber))
						{
							if(m_bDispenseSuccess)
							{
								for(int r=0;r<3;r++)
								{
									if(g_pFrm->m_CmdRun.RunDispFSelect[r])
									{
										PDposting[nDisNumber+(ProductParam.IrregularColumn+2)/3*r].bIsCompleted = true;
									}
								}
								ProductParam.tgTrackInfo.m_nIrregularRow = j+1;
							}
							return false;
						}
						g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
						for(int r=0;r<3;r++)
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[r])
							{
								PDposting[nDisNumber+(ProductParam.IrregularColumn+2)/3*r].bIsCompleted = true;
							}
							if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
								g_pFrm->m_CmdRun.ProductParam.DispFCount++;
							}
						}
						ProductParam.tgTrackInfo.m_nIrregularRow = j+1;
					}
				}
				else
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nIrregularRow = ProductParam.IrregularRow;
					}
					for(int j=ProductParam.tgTrackInfo.m_nIrregularRow-1;j>=0;j--)
					{
						int nDisNumber = i+j*ProductParam.IrregularColumn;
						if((i+(ProductParam.IrregularColumn+2)/3*2)<ProductParam.IrregularColumn)
						{
							if(!g_pFrm->m_CmdRun.RunDispFSelect[2]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[2])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[2] = 1;
								g_pFrm->m_CmdRun.ProductParam.DispFCount++;
							}
						}
						else
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[2])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[2] = 0;
								g_pFrm->m_CmdRun.ProductParam.DispFCount--;
							}
						}
						for(int r=0;r<3;r++)
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[r])
							{
								if(!PDposting[nDisNumber+(ProductParam.IrregularColumn+2)/3*r].bIsHasPad)
								{
									g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
									g_pFrm->m_CmdRun.ProductParam.DispFCount--;
								}
							}
						}
						if(!SingleDot(nDisNumber))
						{
							if(m_bDispenseSuccess)
							{
								for(int r=0;r<3;r++)
								{
									if(g_pFrm->m_CmdRun.RunDispFSelect[r])
									{
										PDposting[nDisNumber+(ProductParam.IrregularColumn+2)/3*r].bIsCompleted = true;
									}
								}
								ProductParam.tgTrackInfo.m_nIrregularRow = j;
							}
							return false;
						}
						g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
						for(int r=0;r<3;r++)
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[r])
							{
								PDposting[nDisNumber+(ProductParam.IrregularColumn+2)/3*r].bIsCompleted = true;
							}
							if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
								g_pFrm->m_CmdRun.ProductParam.DispFCount++;
							}
						}
						ProductParam.tgTrackInfo.m_nIrregularRow = j;
					}
				}
				ProductParam.tgTrackInfo.m_nIrregularCol = i+1;
			}
		}
		else
		{
			if(!m_bStartRun)
			{
				ProductParam.tgTrackInfo.m_nIrregularRow = 0;
			}
			for(int j=ProductParam.tgTrackInfo.m_nIrregularRow;j<ProductParam.IrregularRow;j++)
			{
				if(!(j%2))
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nIrregularCol = 0;
					}
					for(int i=ProductParam.tgTrackInfo.m_nIrregularCol;i<(ProductParam.IrregularColumn+2)/3;i++)
					{
						int nDisNumber = i+j*ProductParam.IrregularColumn;
						if((i+(ProductParam.IrregularColumn+2)/3*2)<ProductParam.IrregularColumn)
						{
							if(!g_pFrm->m_CmdRun.RunDispFSelect[2]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[2])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[2] = 1;
								g_pFrm->m_CmdRun.ProductParam.DispFCount++;
							}
						}
						else
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[2])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[2] = 0;
								g_pFrm->m_CmdRun.ProductParam.DispFCount--;
							}
						}
						for(int r=0;r<3;r++)
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[r])
							{
								if(!PDposting[nDisNumber+(ProductParam.IrregularColumn+2)/3*r].bIsHasPad)
								{
									g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
									g_pFrm->m_CmdRun.ProductParam.DispFCount--;
								}
							}
						}
						if(!SingleDot(nDisNumber))
						{
							if(m_bDispenseSuccess)
							{
								for(int r=0;r<3;r++)
								{
									if(g_pFrm->m_CmdRun.RunDispFSelect[r])
									{
										PDposting[nDisNumber+(ProductParam.IrregularColumn+2)/3*r].bIsCompleted = true;
									}
								}
								ProductParam.tgTrackInfo.m_nIrregularCol = i+1;
							}
							return false;
						}
						g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
						for(int r=0;r<3;r++)
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[r])
							{
								PDposting[nDisNumber+(ProductParam.IrregularColumn+2)/3*r].bIsCompleted = true;
							}
							if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
								g_pFrm->m_CmdRun.ProductParam.DispFCount++;
							}
						}
						ProductParam.tgTrackInfo.m_nIrregularCol = i+1;
					}
				}
				else
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nIrregularCol = ProductParam.IrregularColumn;
					}
					for(int i=ProductParam.tgTrackInfo.m_nIrregularCol-1;i>=0;i--)
					{
						int nDisNumber = i+j*ProductParam.IrregularColumn;
						if((i+(ProductParam.IrregularColumn+2)/3*2)<ProductParam.IrregularColumn)
						{
							if(!g_pFrm->m_CmdRun.RunDispFSelect[2]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[2])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[2] = 1;
								g_pFrm->m_CmdRun.ProductParam.DispFCount++;
							}
						}
						else
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[2])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[2] = 0;
								g_pFrm->m_CmdRun.ProductParam.DispFCount--;
							}
						}
						for(int r=0;r<3;r++)
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[r])
							{
								if(!PDposting[nDisNumber+(ProductParam.IrregularColumn+2)/3*r].bIsHasPad)
								{
									g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
									g_pFrm->m_CmdRun.ProductParam.DispFCount--;
								}
							}
						}
						if(!SingleDot(nDisNumber))
						{
							if(m_bDispenseSuccess)
							{
								for(int r=0;r<3;r++)
								{
									if(g_pFrm->m_CmdRun.RunDispFSelect[r])
									{
										PDposting[nDisNumber+(ProductParam.IrregularColumn+2)/3*r].bIsCompleted = true;
									}
								}
								ProductParam.tgTrackInfo.m_nIrregularCol = i;
							}
							return false;
						}
						g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
						for(int r=0;r<3;r++)
						{
							if(g_pFrm->m_CmdRun.RunDispFSelect[r])
							{
								PDposting[nDisNumber+(ProductParam.IrregularColumn+2)/3*r].bIsCompleted = true;
							}
							if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
							{
								g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
								g_pFrm->m_CmdRun.ProductParam.DispFCount++;
							}
						}
						ProductParam.tgTrackInfo.m_nIrregularCol = i;
					}
				}
				ProductParam.tgTrackInfo.m_nIrregularRow = j+1;
			}
		}
	}
	else
	{
		if(ProductParam.dispOrientation==COLUMN_DISP)
		{
			if(ProductParam.FirstDispColumn>1)
			{
				if(!m_bStartRun)
				{
					ProductParam.tgTrackInfo.m_nFirstCol = 0;
				}
				for(int i=ProductParam.tgTrackInfo.m_nFirstCol;i<(ProductParam.FirstDispColumn+2)/3;i++)
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nSecondCol = 0;
					}
					for(int j=ProductParam.tgTrackInfo.m_nSecondCol;j<ProductParam.SecondDispColumn;j++)
					{
						if(!m_bStartRun)
						{
							ProductParam.tgTrackInfo.m_nThirdCol = 0;
						}
						for(int k=ProductParam.tgTrackInfo.m_nThirdCol;k<ProductParam.ThirdDispColumn;k++)
						{
							if(!((i*ProductParam.SecondDispColumn*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispColumn+k)%2))
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstRow = 0;
								}
								for(int m=ProductParam.tgTrackInfo.m_nFirstRow;m<ProductParam.FirstDispRow;m++)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondRow = 0;
									}
									for(int n=ProductParam.tgTrackInfo.m_nSecondRow;n<ProductParam.SecondDispRow;n++)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdRow = 0;
										}
										for(int l=ProductParam.tgTrackInfo.m_nThirdRow;l<ProductParam.ThirdDispRow;l++)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((i+(ProductParam.FirstDispColumn+2)/3*2)<ProductParam.FirstDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[2]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = 1;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = 0;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.FirstDispColumn+2)/3*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<3;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.FirstDispColumn+2)/3*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdRow = l+1;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.FirstDispColumn+2)/3*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdRow = l+1;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondRow = n+1;
									}
									ProductParam.tgTrackInfo.m_nFirstRow = m+1;
								}
							}
							else
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstRow = ProductParam.FirstDispRow;
								}
								for(int m=ProductParam.tgTrackInfo.m_nFirstRow-1;m>=0;m--)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondRow = ProductParam.SecondDispRow;
									}
									for(int n=ProductParam.tgTrackInfo.m_nSecondRow-1;n>=0;n--)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdRow = ProductParam.ThirdDispRow;
										}
										for(int l=ProductParam.tgTrackInfo.m_nThirdRow-1;l>=0;l--)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((i+(ProductParam.FirstDispColumn+2)/3*2)<ProductParam.FirstDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[2]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = 1;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = 0;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.FirstDispColumn+2)/3*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<3;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.FirstDispColumn+2)/3*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdRow = l;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.FirstDispColumn+2)/3*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdRow = l;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondRow = n;
									}
									ProductParam.tgTrackInfo.m_nFirstRow = m;
								}
							}
							ProductParam.tgTrackInfo.m_nThirdCol = k+1;
						}
						ProductParam.tgTrackInfo.m_nSecondCol = j+1;
					}
					ProductParam.tgTrackInfo.m_nFirstCol = i+1;
				}
			}
			else if(ProductParam.SecondDispColumn>1)
			{
				if(!m_bStartRun)
				{
					ProductParam.tgTrackInfo.m_nFirstCol = 0;
				}
				for(int i=ProductParam.tgTrackInfo.m_nFirstCol;i<ProductParam.FirstDispColumn;i++)
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nSecondCol = 0;
					}
					for(int j=ProductParam.tgTrackInfo.m_nSecondCol;j<(ProductParam.SecondDispColumn+2)/3;j++)
					{
						if(!m_bStartRun)
						{
							ProductParam.tgTrackInfo.m_nThirdCol = 0;
						}
						for(int k=ProductParam.tgTrackInfo.m_nThirdCol;k<ProductParam.ThirdDispColumn;k++)
						{
							if(!((i*(ProductParam.SecondDispColumn+2)/3*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispColumn+k)%2))
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstRow = 0;
								}
								for(int m=ProductParam.tgTrackInfo.m_nFirstRow;m<ProductParam.FirstDispRow;m++)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondRow = 0;
									}
									for(int n=ProductParam.tgTrackInfo.m_nSecondRow;n<ProductParam.SecondDispRow;n++)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdRow = 0;
										}
										for(int l=ProductParam.tgTrackInfo.m_nThirdRow;l<ProductParam.ThirdDispRow;l++)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((j+(ProductParam.SecondDispColumn+2)/3*2)<ProductParam.SecondDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[2]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = 1;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = 0;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.SecondDispColumn+2)/3*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<3;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.SecondDispColumn+2)/3*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdRow = l+1;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.SecondDispColumn+2)/3*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdRow = l+1;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondRow = n+1;
									}
									ProductParam.tgTrackInfo.m_nFirstRow = m+1;
								}
							}
							else
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstRow = ProductParam.FirstDispRow;
								}
								for(int m=ProductParam.tgTrackInfo.m_nFirstRow-1;m>=0;m--)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondRow = ProductParam.SecondDispRow;
									}
									for(int n=ProductParam.tgTrackInfo.m_nSecondRow-1;n>=0;n--)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdRow = ProductParam.ThirdDispRow;
										}
										for(int l=ProductParam.tgTrackInfo.m_nThirdRow-1;l>=0;l--)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((j+(ProductParam.SecondDispColumn+2)/3*2)<ProductParam.SecondDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[2]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = 1;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = 0;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.SecondDispColumn+2)/3*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<3;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.SecondDispColumn+2)/3*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdRow = l;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.SecondDispColumn+2)/3*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdRow = l;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondRow = n;
									}
									ProductParam.tgTrackInfo.m_nFirstRow = m;
								}
							}
							ProductParam.tgTrackInfo.m_nThirdCol = k+1;
						}
						ProductParam.tgTrackInfo.m_nSecondCol = j+1;
					}
					ProductParam.tgTrackInfo.m_nFirstCol = i+1;
				}
			}
			else
			{
				if(!m_bStartRun)
				{
					ProductParam.tgTrackInfo.m_nFirstCol = 0;
				}
				for(int i=ProductParam.tgTrackInfo.m_nFirstCol;i<ProductParam.FirstDispColumn;i++)
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nSecondCol = 0;
					}
					for(int j=ProductParam.tgTrackInfo.m_nSecondCol;j<ProductParam.SecondDispColumn;j++)
					{
						if(!m_bStartRun)
						{
							ProductParam.tgTrackInfo.m_nThirdCol = 0;
						}
						for(int k=ProductParam.tgTrackInfo.m_nThirdCol;k<(ProductParam.ThirdDispColumn+2)/3;k++)
						{
							if(!((i*ProductParam.SecondDispColumn*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispColumn+k)%2))
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstRow = 0;
								}
								for(int m=ProductParam.tgTrackInfo.m_nFirstRow;m<ProductParam.FirstDispRow;m++)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondRow = 0;
									}
									for(int n=ProductParam.tgTrackInfo.m_nSecondRow;n<ProductParam.SecondDispRow;n++)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdRow = 0;
										}
										for(int l=ProductParam.tgTrackInfo.m_nThirdRow;l<ProductParam.ThirdDispRow;l++)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((k+(ProductParam.ThirdDispColumn+2)/3*2)<ProductParam.ThirdDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[2]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = false;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.ThirdDispColumn+2)/3*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<3;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.ThirdDispColumn+2)/3*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdRow = l+1;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.ThirdDispColumn+2)/3*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdRow = l+1;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondRow = n+1;
									}
									ProductParam.tgTrackInfo.m_nFirstRow = m+1;
								}
							}
							else
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstRow = ProductParam.FirstDispRow;
								}
								for(int m=ProductParam.tgTrackInfo.m_nFirstRow-1;m>=0;m--)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondRow = ProductParam.SecondDispRow;
									}
									for(int n=ProductParam.tgTrackInfo.m_nSecondRow-1;n>=0;n--)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdRow = ProductParam.ThirdDispRow;
										}
										for(int l=ProductParam.tgTrackInfo.m_nThirdRow-1;l>=0;l--)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((k+(ProductParam.ThirdDispColumn+2)/3*2)<ProductParam.ThirdDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[2]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = 1;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = 0;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.ThirdDispColumn+2)/3*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<3;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.ThirdDispColumn+2)/3*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdRow = l;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.ThirdDispColumn+2)/3*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdRow = l;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondRow = n;
									}
									ProductParam.tgTrackInfo.m_nFirstRow = m;
								}
							}
							ProductParam.tgTrackInfo.m_nThirdCol = k+1;
						}
						ProductParam.tgTrackInfo.m_nSecondCol = j+1;
					}
					ProductParam.tgTrackInfo.m_nFirstCol = i+1;
				}
			}
		}
		else
		{
			if(ProductParam.FirstDispColumn>1)
			{
				if(!m_bStartRun)
				{
					ProductParam.tgTrackInfo.m_nFirstRow = 0;
				}
				for(int m=ProductParam.tgTrackInfo.m_nFirstRow;m<ProductParam.FirstDispRow;m++)
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nSecondRow = 0;
					}
					for(int n=ProductParam.tgTrackInfo.m_nSecondRow;n<ProductParam.SecondDispRow;n++)
					{
						if(!m_bStartRun)
						{
							ProductParam.tgTrackInfo.m_nThirdRow = 0;
						}
						for(int l=ProductParam.tgTrackInfo.m_nThirdRow;l<ProductParam.ThirdDispRow;l++)
						{
							if(!((m*ProductParam.SecondDispRow*ProductParam.ThirdDispRow+n*ProductParam.ThirdDispRow+l)%2))
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstCol = 0;
								}
								for(int i=ProductParam.tgTrackInfo.m_nFirstCol;i<(ProductParam.FirstDispColumn+2)/3;i++)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondCol = 0;
									}
									for(int j=ProductParam.tgTrackInfo.m_nSecondCol;j<ProductParam.SecondDispColumn;j++)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdCol = 0;
										}
										for(int k=ProductParam.tgTrackInfo.m_nThirdCol;k<ProductParam.ThirdDispColumn;k++)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((i+(ProductParam.FirstDispColumn+2)/3*2)<ProductParam.FirstDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[2]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = 1;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = 0;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.FirstDispColumn+2)/3*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<3;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.FirstDispColumn+2)/3*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdCol = k+1;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.FirstDispColumn+2)/3*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdCol = k+1;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondCol = j+1;
									}
									ProductParam.tgTrackInfo.m_nFirstCol = i+1;
								}
							}
							else
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstCol = (ProductParam.FirstDispColumn+2)/3;
								}
								for(int i=ProductParam.tgTrackInfo.m_nFirstCol-1;i>=0;i--)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondCol = ProductParam.SecondDispColumn;
									}
									for(int j=ProductParam.tgTrackInfo.m_nSecondCol-1;j>=0;j--)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdCol = ProductParam.ThirdDispColumn;
										}
										for(int k=ProductParam.tgTrackInfo.m_nThirdCol-1;k>=0;k--)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((i+(ProductParam.FirstDispColumn+2)/3*2)<ProductParam.FirstDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[2]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = 1;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = 0;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.FirstDispColumn+2)/3*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<3;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.FirstDispColumn+2)/3*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdCol = k;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.FirstDispColumn+2)/3*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdCol = k;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondCol = j;
									}
									ProductParam.tgTrackInfo.m_nFirstCol = i;
								}
							}
							ProductParam.tgTrackInfo.m_nThirdRow = l+1;
						}
						ProductParam.tgTrackInfo.m_nSecondRow = n+1;
					}
					ProductParam.tgTrackInfo.m_nFirstRow = m+1;
				}
			}
			else if(ProductParam.SecondDispColumn>1)
			{
				if(!m_bStartRun)
				{
					ProductParam.tgTrackInfo.m_nFirstRow = 0;
				}
				for(int m=ProductParam.tgTrackInfo.m_nFirstRow;m<ProductParam.FirstDispRow;m++)
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nSecondRow = 0;
					}
					for(int n=ProductParam.tgTrackInfo.m_nSecondRow;n<ProductParam.SecondDispRow;n++)
					{
						if(!m_bStartRun)
						{
							ProductParam.tgTrackInfo.m_nThirdRow = 0;
						}
						for(int l=ProductParam.tgTrackInfo.m_nThirdRow;l<ProductParam.ThirdDispRow;l++)
						{
							if(!((m*ProductParam.SecondDispRow*ProductParam.ThirdDispRow+n*ProductParam.ThirdDispRow+l)%2))
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstCol = 0;
								}
								for(int i=ProductParam.tgTrackInfo.m_nFirstCol;i<ProductParam.FirstDispColumn;i++)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondCol = 0;
									}
									for(int j=ProductParam.tgTrackInfo.m_nSecondCol;j<(ProductParam.SecondDispColumn+2)/3;j++)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdCol = 0;
										}
										for(int k=ProductParam.tgTrackInfo.m_nThirdCol;k<ProductParam.ThirdDispColumn;k++)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((j+(ProductParam.SecondDispColumn+2)/3*2)<ProductParam.SecondDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[2]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = 1;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = 0;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.SecondDispColumn+2)/3*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<3;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.SecondDispColumn+2)/3*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdCol = k+1;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.SecondDispColumn+2)/3*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdCol = k+1;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondCol = j+1;
									}
									ProductParam.tgTrackInfo.m_nFirstCol = i+1;
								}
							}
							else
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstCol = ProductParam.FirstDispColumn;
								}
								for(int i=ProductParam.tgTrackInfo.m_nFirstCol-1;i>=0;i--)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondCol = (ProductParam.SecondDispColumn+2)/3;
									}
									for(int j=ProductParam.tgTrackInfo.m_nSecondCol-1;j>=0;j--)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdCol = ProductParam.ThirdDispColumn;
										}
										for(int k=ProductParam.tgTrackInfo.m_nThirdCol-1;k>=0;k--)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((j+(ProductParam.SecondDispColumn+2)/3*2)<ProductParam.SecondDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[2]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = 1;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = 0;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.SecondDispColumn+2)/3*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<3;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.SecondDispColumn+2)/3*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdCol = k;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.SecondDispColumn+2)/3*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdCol = k;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondCol = j;
									}
									ProductParam.tgTrackInfo.m_nFirstCol = i;
								}
							}
							ProductParam.tgTrackInfo.m_nThirdRow = l+1;
						}
						ProductParam.tgTrackInfo.m_nSecondRow = n+1;
					}
					ProductParam.tgTrackInfo.m_nFirstRow = m+1;
				}
			}
			else
			{
				if(!m_bStartRun)
				{
					ProductParam.tgTrackInfo.m_nFirstRow = 0;
				}
				for(int m=ProductParam.tgTrackInfo.m_nFirstRow;m<ProductParam.FirstDispRow;m++)
				{
					if(!m_bStartRun)
					{
						ProductParam.tgTrackInfo.m_nSecondRow = 0;
					}
					for(int n=ProductParam.tgTrackInfo.m_nSecondRow;n<ProductParam.SecondDispRow;n++)
					{
						if(!m_bStartRun)
						{
							ProductParam.tgTrackInfo.m_nThirdRow = 0;
						}
						for(int l=ProductParam.tgTrackInfo.m_nThirdRow;l<ProductParam.ThirdDispRow;l++)
						{
							if(!((m*ProductParam.SecondDispRow*ProductParam.ThirdDispRow+n*ProductParam.ThirdDispRow+l)%2))
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstCol = 0;
								}
								for(int i=ProductParam.tgTrackInfo.m_nFirstCol;i<ProductParam.FirstDispColumn;i++)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondCol = 0;
									}
									for(int j=ProductParam.tgTrackInfo.m_nSecondCol;j<ProductParam.SecondDispColumn;j++)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdCol = 0;
										}
										for(int k=ProductParam.tgTrackInfo.m_nThirdCol;k<(ProductParam.ThirdDispColumn+2)/3;k++)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((k+(ProductParam.ThirdDispColumn+2)/3*2)<ProductParam.ThirdDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[2]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = 1;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = 0;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.ThirdDispColumn+2)/3*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<3;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.ThirdDispColumn+2)/3*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdCol = k+1;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.ThirdDispColumn+2)/3*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdCol = k+1;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondCol = j+1;
									}
									if(m_bStartRun)
									{
										m_bStartRun = false;
									}
									ProductParam.tgTrackInfo.m_nFirstCol = i+1;
								}
							}
							else
							{
								if(!m_bStartRun)
								{
									ProductParam.tgTrackInfo.m_nFirstCol = ProductParam.FirstDispColumn;
								}
								for(int i=ProductParam.tgTrackInfo.m_nFirstCol-1;i>=0;i--)
								{
									if(!m_bStartRun)
									{
										ProductParam.tgTrackInfo.m_nSecondCol = ProductParam.SecondDispColumn;
									}
									for(int j=ProductParam.tgTrackInfo.m_nSecondCol-1;j>=0;j--)
									{
										if(!m_bStartRun)
										{
											ProductParam.tgTrackInfo.m_nThirdCol = (ProductParam.ThirdDispColumn+2)/3;
										}
										for(int k=ProductParam.tgTrackInfo.m_nThirdCol-1;k>=0;k--)
										{
											if(m_bStartRun)
											{
												m_bStartRun = false;
											}
											int nDisNumber = k+l*ProductParam.ThirdDispColumn+j*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+n*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
												i*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
											if((k+(ProductParam.ThirdDispColumn+2)/3*2)<ProductParam.ThirdDispColumn)
											{
												if(!g_pFrm->m_CmdRun.RunDispFSelect[2]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = 1;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											else
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[2])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[2] = 0;
													g_pFrm->m_CmdRun.ProductParam.DispFCount--;
												}
											}
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													if(!PDposting[nDisNumber+(ProductParam.ThirdDispColumn+2)/3*r].bIsHasPad)
													{
														g_pFrm->m_CmdRun.RunDispFSelect[r] = false;
														g_pFrm->m_CmdRun.ProductParam.DispFCount--;
													}
												}
											}
											if(!SingleDot(nDisNumber))
											{
												if(m_bDispenseSuccess)
												{
													for(int r=0;r<3;r++)
													{
														if(g_pFrm->m_CmdRun.RunDispFSelect[r])
														{
															PDposting[nDisNumber+(ProductParam.ThirdDispColumn+2)/3*r].bIsCompleted = true;
														}
													}
													ProductParam.tgTrackInfo.m_nThirdCol = k;
												}
												return false;
											}
											g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
											for(int r=0;r<3;r++)
											{
												if(g_pFrm->m_CmdRun.RunDispFSelect[r])
												{
													PDposting[nDisNumber+(ProductParam.ThirdDispColumn+2)/3*r].bIsCompleted = true;
												}
												if(g_pFrm->m_CmdRun.ProductParam.FSelectVal[r])
												{
													g_pFrm->m_CmdRun.RunDispFSelect[r] = true;
													g_pFrm->m_CmdRun.ProductParam.DispFCount++;
												}
											}
											ProductParam.tgTrackInfo.m_nThirdCol = k;
										}
										if(m_bStartRun)
										{
											m_bStartRun = false;
										}
										ProductParam.tgTrackInfo.m_nSecondCol = j;
									}
									if(m_bStartRun)
									{
										m_bStartRun = false;
									}
									ProductParam.tgTrackInfo.m_nFirstCol = i;
								}
							}
							ProductParam.tgTrackInfo.m_nThirdRow = l+1;
						}
						ProductParam.tgTrackInfo.m_nSecondRow = n+1;
					}
					ProductParam.tgTrackInfo.m_nFirstRow = m+1;
				}
			}
		}
	}
	return true;
}

void CCmdRun::OneValveRepaire(bool *bIsDispense)
{
	CString str;
	int nSelValve = 0;
	for(int i=0;i<3;i++)
	{
		if(i==0)
		{
			ProductParam.FSelectVal[i] = true;
			RunDispFSelect[i] = true;
			nSelValve = i;
		}
		else
		{
			ProductParam.FSelectVal[i] = false;
			RunDispFSelect[i] = false;
		}
	}
	if(IsRunStop())
	{
		return ;
	}
	if(ProductParam.IrregularMatrix)
	{
		if(ProductParam.dispOrientation==COLUMN_DISP)
		{
			for(int j=0;j<ProductParam.IrregularColumn;j++)
			{
				for(int i=0;i<ProductParam.IrregularRow;i++)
				{
					int nDisNumber = j+i*ProductParam.IrregularColumn;
					if(bIsDispense[nDisNumber])
					{
						str.Format("RunDisp:%d",nDisNumber);
						AddMsg(str);
						g_pFrm->m_CmdRun.RunDispFSelect[nSelValve] = true;
						g_pFrm->m_CmdRun.ProductParam.DispFCount=1;
					}
					if(!SelectDispense(PDposting[nDisNumber].Dx-ProductParam.NeedleGap*nSelValve,
						PDposting[nDisNumber].Dy,PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
					{
						m_bRepair = false;
						return;
					}
					g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
					RunDispFSelect[nSelValve] = false;
				}
			}
		}
		else
		{
			for(int i=0;i<ProductParam.IrregularRow;i++)
			{
				for(int j=0;j<ProductParam.IrregularColumn;j++)
				{
					int nDisNumber = j+i*ProductParam.IrregularColumn;
					if(bIsDispense[nDisNumber])
					{
						str.Format("RunDisp:%d",nDisNumber);
						AddMsg(str);
						g_pFrm->m_CmdRun.RunDispFSelect[nSelValve] = true;
						g_pFrm->m_CmdRun.ProductParam.DispFCount=1;
					}
					if(!SelectDispense(PDposting[nDisNumber].Dx-ProductParam.NeedleGap*nSelValve,
						PDposting[nDisNumber].Dy,PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
					{
						m_bRepair = false;
						return;
					}
					g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
					RunDispFSelect[nSelValve] = false;
				}
			}
		}
	}
	else
	{
		if(ProductParam.dispOrientation==COLUMN_DISP)
		{
			for(int j=0;j<ProductParam.FirstDispColumn;j++)
			{
				for(int n=0;n<ProductParam.SecondDispColumn;n++)
				{
					for(int l=0;l<ProductParam.ThirdDispColumn;l++)
					{
						for(int i=0;i<ProductParam.FirstDispRow;i++)
						{
							for(int m=0;m<ProductParam.SecondDispRow;m++)
							{
								for(int k=0;k<ProductParam.ThirdDispRow;k++)
								{
									int nDisNumber = l+k*ProductParam.ThirdDispColumn+n*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
										j*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+i*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
									if(bIsDispense[nDisNumber])
									{
										str.Format("RunDisp:%d",nDisNumber);
										AddMsg(str);
										g_pFrm->m_CmdRun.RunDispFSelect[nSelValve] = true;
										g_pFrm->m_CmdRun.ProductParam.DispFCount = 1;
									}
									if(!SelectDispense(PDposting[nDisNumber].Dx-ProductParam.NeedleGap*nSelValve,
										PDposting[nDisNumber].Dy,PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
									{
										m_bRepair = false;
										return;
									}
									g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
									RunDispFSelect[nSelValve] = false;
								}
							}
						}
					}
				}
			}
		}
		else
		{
			for(int i=0;i<ProductParam.FirstDispRow;i++)
			{
				for(int m=0;m<ProductParam.SecondDispRow;m++)
				{
					for(int k=0;k<ProductParam.ThirdDispRow;k++)
					{
						for(int j=0;j<ProductParam.FirstDispColumn;j++)
						{
							for(int n=0;n<ProductParam.SecondDispColumn;n++)
							{
								for(int l=0;l<ProductParam.ThirdDispColumn;l++)
								{
									int nDisNumber = l+k*ProductParam.ThirdDispColumn+n*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
										j*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+i*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
									if(bIsDispense[nDisNumber])
									{
										str.Format("RunDisp:%d",nDisNumber);
										AddMsg(str);
										g_pFrm->m_CmdRun.RunDispFSelect[nSelValve] = true;
										g_pFrm->m_CmdRun.ProductParam.DispFCount = 1;
									}
									if(!SelectDispense(PDposting[nDisNumber].Dx-ProductParam.NeedleGap*nSelValve,
										PDposting[nDisNumber].Dy,PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
									{
										m_bRepair = false;
										return;
									}
									g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
									RunDispFSelect[nSelValve] = false;
								}
							}
						}
					}
				}
			}
		}
	}
}

void CCmdRun::TwoValveRepaire(bool *bIsDispense)
{
	CString str;
	ProductParam.FSelectVal[2] = false;
	RunDispFSelect[2] = false;
	//if(!SynchronizeMoveXY(PDposting[0].Dx,PDposting[0].Dy,true))
	//{
	//	return;
	//}
	//if(!SynchronizeMoveZ(PDposting[0].Dz-ProductParam.dZupHigh,true))
	//{
	//	return;
	//}
	if(IsRunStop())
	{
		return ;
	}
	if(ProductParam.IrregularMatrix)
	{
		if(ProductParam.dispOrientation==COLUMN_DISP)
		{
			for(int j=0;j<(ProductParam.IrregularColumn+1)/2;j++)
			{
				for(int i=0;i<ProductParam.IrregularRow;i++)
				{
					int nDisNumber = j+i*ProductParam.IrregularColumn;
					if(bIsDispense[nDisNumber])
					{
						str.Format("RunDisp:%d",nDisNumber);
						AddMsg(str);
						g_pFrm->m_CmdRun.RunDispFSelect[0] = true;
						g_pFrm->m_CmdRun.ProductParam.DispFCount++;
					}
					if(j+(ProductParam.IrregularColumn+1)/2<ProductParam.IrregularColumn)
					{
						if(bIsDispense[nDisNumber+(ProductParam.IrregularColumn+1)/2])
						{
							str.Format("RunDisp:%d",nDisNumber+(ProductParam.IrregularColumn+1)/2);
							AddMsg(str);
							g_pFrm->m_CmdRun.RunDispFSelect[1] = true;
							g_pFrm->m_CmdRun.ProductParam.DispFCount++;
						}
					}
					if(!SelectDispense(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
						PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
					{
						m_bRepair = false;
						return;
					}
					g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
					for(int i=0;i<2;i++)
					{
						RunDispFSelect[i] = false;
					}
				}
			}
		}
		else
		{
			for(int i=0;i<ProductParam.IrregularRow;i++)
			{
				for(int j=0;j<(ProductParam.IrregularColumn+1)/2;j++)
				{
					int nDisNumber = j+i*ProductParam.IrregularColumn;
					if(bIsDispense[nDisNumber])
					{
						str.Format("RunDisp:%d",nDisNumber);
						AddMsg(str);
						g_pFrm->m_CmdRun.RunDispFSelect[0] = true;
						g_pFrm->m_CmdRun.ProductParam.DispFCount++;
					}
					if(j+(ProductParam.IrregularColumn+1)/2<ProductParam.IrregularColumn)
					{
						if(bIsDispense[nDisNumber+(ProductParam.IrregularColumn+1)/2])
						{
							str.Format("RunDisp:%d",nDisNumber+(ProductParam.IrregularColumn+1)/2);
							AddMsg(str);
							g_pFrm->m_CmdRun.RunDispFSelect[1] = true;
							g_pFrm->m_CmdRun.ProductParam.DispFCount++;
						}
					}
					if(!SelectDispense(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
						PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
					{
						m_bRepair = false;
						return;
					}
					g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
					for(int i=0;i<2;i++)
					{
						RunDispFSelect[i] = false;
					}
				}
			}
		}
	}
	else
	{
		if(ProductParam.dispOrientation==COLUMN_DISP)
		{
			if(ProductParam.FirstDispColumn>1)
			{
				for(int j=0;j<(ProductParam.FirstDispColumn+1)/2;j++)
				{
					for(int n=0;n<ProductParam.SecondDispColumn;n++)
					{
						for(int l=0;l<ProductParam.ThirdDispColumn;l++)
						{
							for(int i=0;i<ProductParam.FirstDispRow;i++)
							{
								for(int m=0;m<ProductParam.SecondDispRow;m++)
								{
									for(int k=0;k<ProductParam.ThirdDispRow;k++)
									{
										int nDisNumber = l+k*ProductParam.ThirdDispColumn+n*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
											j*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+i*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
										if(bIsDispense[nDisNumber])
										{
											str.Format("RunDisp:%d",nDisNumber);
											AddMsg(str);
											g_pFrm->m_CmdRun.RunDispFSelect[0] = true;
											g_pFrm->m_CmdRun.ProductParam.DispFCount++;
										}
										if(j+(ProductParam.FirstDispColumn+1)/2<ProductParam.FirstDispColumn)
										{
											if(bIsDispense[nDisNumber+(ProductParam.FirstDispColumn+1)/2*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn])
											{
												str.Format("RunDisp:%d",nDisNumber+(ProductParam.FirstDispColumn+1)/2*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn);
												AddMsg(str);
												g_pFrm->m_CmdRun.RunDispFSelect[1] = true;
												g_pFrm->m_CmdRun.ProductParam.DispFCount++;
											}
										}
										if(!SelectDispense(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
											PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
										{
											m_bRepair = false;
											return;
										}
										g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
										for(int i=0;i<2;i++)
										{
											RunDispFSelect[i] = false;
										}
									}
								}
							}
						}
					}
				}
			}
			else if(ProductParam.SecondDispColumn>1)
			{
				for(int j=0;j<ProductParam.FirstDispColumn;j++)
				{
					for(int n=0;n<(ProductParam.SecondDispColumn+1)/2;n++)
					{
						for(int l=0;l<ProductParam.ThirdDispColumn;l++)
						{
							for(int i=0;i<ProductParam.FirstDispRow;i++)
							{
								for(int m=0;m<ProductParam.SecondDispRow;m++)
								{
									for(int k=0;k<ProductParam.ThirdDispRow;k++)
									{
										int nDisNumber = l+k*ProductParam.ThirdDispColumn+n*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
											j*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+i*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
										if(bIsDispense[nDisNumber])
										{
											g_pFrm->m_CmdRun.RunDispFSelect[0] = true;
											g_pFrm->m_CmdRun.ProductParam.DispFCount++;
										}
										if(n+(ProductParam.SecondDispColumn+1)/2<ProductParam.SecondDispColumn)
										{
											if(bIsDispense[nDisNumber+(ProductParam.SecondDispColumn+1)/2*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn])
											{
												g_pFrm->m_CmdRun.RunDispFSelect[1] = true;
												g_pFrm->m_CmdRun.ProductParam.DispFCount++;
											}
										}
										if(!SelectDispense(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
											PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
										{
											m_bRepair = false;
											return;
										}
										g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
										for(int i=0;i<2;i++)
										{
											RunDispFSelect[i] = false;
										}
									}
								}
							}
						}
					}
				}
			}
			else
			{
				for(int j=0;j<ProductParam.FirstDispColumn;j++)
				{
					for(int n=0;n<ProductParam.SecondDispColumn;n++)
					{
						for(int l=0;l<(ProductParam.ThirdDispColumn+1)/2;l++)
						{
							for(int i=0;i<ProductParam.FirstDispRow;i++)
							{
								for(int m=0;m<ProductParam.SecondDispRow;m++)
								{
									for(int k=0;k<ProductParam.ThirdDispRow;k++)
									{
										int nDisNumber = l+k*ProductParam.ThirdDispColumn+n*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
											j*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+i*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
										if(bIsDispense[nDisNumber])
										{
											g_pFrm->m_CmdRun.RunDispFSelect[0] = true;
											g_pFrm->m_CmdRun.ProductParam.DispFCount++;
										}
										if(l+(ProductParam.ThirdDispColumn+1)/2<ProductParam.ThirdDispColumn)
										{
											if(bIsDispense[nDisNumber+(ProductParam.ThirdDispColumn+1)/2])
											{
												g_pFrm->m_CmdRun.RunDispFSelect[1] = true;
												g_pFrm->m_CmdRun.ProductParam.DispFCount++;
											}
										}
										if(!SelectDispense(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
											PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
										{
											m_bRepair = false;
											return;
										}
										g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
										for(int i=0;i<2;i++)
										{
											RunDispFSelect[i] = false;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		else
		{
			if(ProductParam.FirstDispColumn>1)
			{
				for(int i=0;i<ProductParam.FirstDispRow;i++)
				{
					for(int m=0;m<ProductParam.SecondDispRow;m++)
					{
						for(int k=0;k<ProductParam.ThirdDispRow;k++)
						{
							for(int j=0;j<(ProductParam.FirstDispColumn+1)/2;j++)
							{
								for(int n=0;n<ProductParam.SecondDispColumn;n++)
								{
									for(int l=0;l<ProductParam.ThirdDispColumn;l++)
									{
										int nDisNumber = l+k*ProductParam.ThirdDispColumn+n*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
											j*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+i*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
										if(bIsDispense[nDisNumber])
										{
											str.Format("RunDisp:%d",nDisNumber);
											AddMsg(str);
											g_pFrm->m_CmdRun.RunDispFSelect[0] = true;
											g_pFrm->m_CmdRun.ProductParam.DispFCount++;
										}
										if(j+(ProductParam.FirstDispColumn+1)/2<ProductParam.FirstDispColumn)
										{
											if(bIsDispense[nDisNumber+(ProductParam.FirstDispColumn+1)/2*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn])
											{
												str.Format("RunDisp:%d",nDisNumber+(ProductParam.FirstDispColumn+1)/2*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn);
												AddMsg(str);
												g_pFrm->m_CmdRun.RunDispFSelect[1] = true;
												g_pFrm->m_CmdRun.ProductParam.DispFCount++;
											}
										}
										if(!SelectDispense(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
											PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
										{
											m_bRepair = false;
											return;
										}
										g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
										for(int i=0;i<2;i++)
										{
											RunDispFSelect[i] = false;
										}
									}
								}
							}
						}
					}
				}
			}
			else if(ProductParam.SecondDispColumn>1)
			{
				for(int i=0;i<ProductParam.FirstDispRow;i++)
				{
					for(int m=0;m<ProductParam.SecondDispRow;m++)
					{
						for(int k=0;k<ProductParam.ThirdDispRow;k++)
						{
							for(int j=0;j<ProductParam.FirstDispColumn;j++)
							{
								for(int n=0;n<(ProductParam.SecondDispColumn+1)/2;n++)
								{
									for(int l=0;l<ProductParam.ThirdDispColumn;l++)
									{
										int nDisNumber = l+k*ProductParam.ThirdDispColumn+n*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
											j*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+i*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
										if(bIsDispense[nDisNumber])
										{
											g_pFrm->m_CmdRun.RunDispFSelect[0] = true;
											g_pFrm->m_CmdRun.ProductParam.DispFCount++;
										}
										if(n+(ProductParam.SecondDispColumn+1)/2<ProductParam.SecondDispColumn)
										{
											if(bIsDispense[nDisNumber+(ProductParam.SecondDispColumn+1)/2*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn])
											{
												g_pFrm->m_CmdRun.RunDispFSelect[1] = true;
												g_pFrm->m_CmdRun.ProductParam.DispFCount++;
											}
										}
										if(!SelectDispense(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
											PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
										{
											m_bRepair = false;
											return;
										}
										g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
										for(int i=0;i<2;i++)
										{
											RunDispFSelect[i] = false;
										}
									}
								}
							}
						}
					}
				}
			}
			else
			{
				for(int i=0;i<ProductParam.FirstDispRow;i++)
				{
					for(int m=0;m<ProductParam.SecondDispRow;m++)
					{
						for(int k=0;k<ProductParam.ThirdDispRow;k++)
						{
							for(int j=0;j<ProductParam.FirstDispColumn;j++)
							{
								for(int n=0;n<ProductParam.SecondDispColumn;n++)
								{
									for(int l=0;l<(ProductParam.ThirdDispColumn+1)/2;l++)
									{
										int nDisNumber = l+k*ProductParam.ThirdDispColumn+n*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
											j*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+i*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
										if(bIsDispense[nDisNumber])
										{
											g_pFrm->m_CmdRun.RunDispFSelect[0] = true;
											g_pFrm->m_CmdRun.ProductParam.DispFCount++;
										}
										if(l+(ProductParam.ThirdDispColumn+1)/2<ProductParam.ThirdDispColumn)
										{
											if(bIsDispense[nDisNumber+(ProductParam.ThirdDispColumn+1)/2])
											{
												g_pFrm->m_CmdRun.RunDispFSelect[1] = true;
												g_pFrm->m_CmdRun.ProductParam.DispFCount++;
											}
										}
										if(!SelectDispense(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
											PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
										{
											m_bRepair = false;
											return;
										}
										g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
										for(int i=0;i<2;i++)
										{
											RunDispFSelect[i] = false;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void CCmdRun::ThreeValveRepaire(bool *bIsDispense)
{
	CString str;
	//if(!SynchronizeMoveXY(PDposting[0].Dx,PDposting[0].Dy,true))
	//{
	//	return;
	//}
	//if(!SynchronizeMoveZ(PDposting[0].Dz-ProductParam.dZupHigh,true))
	//{
	//	return;
	//}
	if(IsRunStop())
	{
		return ;
	}
	if(ProductParam.IrregularMatrix)
	{
		if(ProductParam.dispOrientation==COLUMN_DISP)
		{
			for(int j=0;j<(ProductParam.IrregularColumn+2)/3;j++)
			{
				for(int i=0;i<ProductParam.IrregularRow;i++)
				{
					int nDisNumber = j+i*ProductParam.IrregularColumn;
					if(bIsDispense[nDisNumber])
					{
						str.Format("RunDisp:%d",nDisNumber);
						AddMsg(str);
						g_pFrm->m_CmdRun.RunDispFSelect[0] = true;
						g_pFrm->m_CmdRun.ProductParam.DispFCount++;
					}
					if(bIsDispense[nDisNumber+(ProductParam.IrregularColumn+2)/3])
					{
						str.Format("RunDisp:%d",nDisNumber+(ProductParam.IrregularColumn+2)/3);
						AddMsg(str);
						g_pFrm->m_CmdRun.RunDispFSelect[1] = true;
						g_pFrm->m_CmdRun.ProductParam.DispFCount++;
					}
					if(j+(ProductParam.IrregularColumn+2)/3*2<ProductParam.IrregularColumn)
					{
						if(bIsDispense[nDisNumber+(ProductParam.IrregularColumn+2)/3*2])
						{
							str.Format("RunDisp:%d",nDisNumber+(ProductParam.IrregularColumn+2)/3*2);
							AddMsg(str);
							g_pFrm->m_CmdRun.RunDispFSelect[2] = true;
							g_pFrm->m_CmdRun.ProductParam.DispFCount++;
						}
					}
					if(!SelectDispense(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
						PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
					{
						m_bRepair = false;
						return;
					}
					g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
					for(int i=0;i<3;i++)
					{
						RunDispFSelect[i] = false;
					}
				}
			}
		}
		else
		{
			for(int i=0;i<ProductParam.IrregularRow;i++)
			{
				for(int j=0;j<(ProductParam.IrregularColumn+2)/3;j++)
				{
					int nDisNumber = j+i*ProductParam.IrregularColumn;
					if(bIsDispense[nDisNumber])
					{
						str.Format("RunDisp:%d",nDisNumber);
						AddMsg(str);
						g_pFrm->m_CmdRun.RunDispFSelect[0] = true;
						g_pFrm->m_CmdRun.ProductParam.DispFCount++;
					}
					if(bIsDispense[nDisNumber+(ProductParam.IrregularColumn+2)/3])
					{
						str.Format("RunDisp:%d",nDisNumber+(ProductParam.IrregularColumn+2)/3);
						AddMsg(str);
						g_pFrm->m_CmdRun.RunDispFSelect[1] = true;
						g_pFrm->m_CmdRun.ProductParam.DispFCount++;
					}
					if(j+(ProductParam.IrregularColumn+2)/3*2<ProductParam.IrregularColumn)
					{
						if(bIsDispense[nDisNumber+(ProductParam.IrregularColumn+2)/3*2])
						{
							str.Format("RunDisp:%d",nDisNumber+(ProductParam.IrregularColumn+2)/3*2);
							AddMsg(str);
							g_pFrm->m_CmdRun.RunDispFSelect[2] = true;
							g_pFrm->m_CmdRun.ProductParam.DispFCount++;
						}
					}
					if(!SelectDispense(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
						PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
					{
						m_bRepair = false;
						return;
					}
					g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
					for(int i=0;i<3;i++)
					{
						RunDispFSelect[i] = false;
					}
				}
			}
		}
	}
	else
	{
		if(ProductParam.dispOrientation==COLUMN_DISP)
		{
			if(ProductParam.FirstDispColumn>1)
			{
				for(int j=0;j<(ProductParam.FirstDispColumn+2)/3;j++)
				{
					for(int n=0;n<ProductParam.SecondDispColumn;n++)
					{
						for(int l=0;l<ProductParam.ThirdDispColumn;l++)
						{
							for(int i=0;i<ProductParam.FirstDispRow;i++)
							{
								for(int m=0;m<ProductParam.SecondDispRow;m++)
								{
									for(int k=0;k<ProductParam.ThirdDispRow;k++)
									{
										int nDisNumber = l+k*ProductParam.ThirdDispColumn+n*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
											j*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+i*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
										if(bIsDispense[nDisNumber])
										{
											str.Format("RunDisp:%d",nDisNumber);
											AddMsg(str);
											g_pFrm->m_CmdRun.RunDispFSelect[0] = true;
											g_pFrm->m_CmdRun.ProductParam.DispFCount++;
										}
										if(bIsDispense[nDisNumber+(ProductParam.FirstDispColumn+2)/3*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn])
										{
											str.Format("RunDisp:%d",nDisNumber+(ProductParam.FirstDispColumn+2)/3*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn);
											AddMsg(str);
											g_pFrm->m_CmdRun.RunDispFSelect[1] = true;
											g_pFrm->m_CmdRun.ProductParam.DispFCount++;
										}
										if(j+(ProductParam.FirstDispColumn+2)/3*2<ProductParam.FirstDispColumn)
										{
											if(bIsDispense[nDisNumber+(ProductParam.FirstDispColumn+2)/3*2*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn])
											{
												str.Format("RunDisp:%d",nDisNumber+(ProductParam.FirstDispColumn+2)/3*2*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn);
												AddMsg(str);
												g_pFrm->m_CmdRun.RunDispFSelect[2] = true;
												g_pFrm->m_CmdRun.ProductParam.DispFCount++;
											}
										}
										if(!SelectDispense(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
											PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
										{
											m_bRepair = false;
											return;
										}
										g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
										for(int i=0;i<3;i++)
										{
											RunDispFSelect[i] = false;
										}
									}
								}
							}
						}
					}
				}
			}
			else if(ProductParam.SecondDispColumn>1)
			{
				for(int j=0;j<ProductParam.FirstDispColumn;j++)
				{
					for(int n=0;n<(ProductParam.SecondDispColumn+2)/3;n++)
					{
						for(int l=0;l<ProductParam.ThirdDispColumn;l++)
						{
							for(int i=0;i<ProductParam.FirstDispRow;i++)
							{
								for(int m=0;m<ProductParam.SecondDispRow;m++)
								{
									for(int k=0;k<ProductParam.ThirdDispRow;k++)
									{
										int nDisNumber = l+k*ProductParam.ThirdDispColumn+n*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
											j*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+i*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
										if(bIsDispense[nDisNumber])
										{
											g_pFrm->m_CmdRun.RunDispFSelect[0] = true;
											g_pFrm->m_CmdRun.ProductParam.DispFCount++;
										}
										if(bIsDispense[nDisNumber+(ProductParam.SecondDispColumn+2)/3*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn])
										{
											g_pFrm->m_CmdRun.RunDispFSelect[1] = true;
											g_pFrm->m_CmdRun.ProductParam.DispFCount++;
										}
										if(n+(ProductParam.SecondDispColumn+2)/3*2<ProductParam.SecondDispColumn)
										{
											if(bIsDispense[nDisNumber+(ProductParam.SecondDispColumn+2)/3*2*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn])
											{
												g_pFrm->m_CmdRun.RunDispFSelect[2] = true;
												g_pFrm->m_CmdRun.ProductParam.DispFCount++;
											}
										}
										if(!SelectDispense(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
											PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
										{
											m_bRepair = false;
											return;
										}
										g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
										for(int i=0;i<3;i++)
										{
											RunDispFSelect[i] = false;
										}
									}
								}
							}
						}
					}
				}
			}
			else
			{
				for(int j=0;j<ProductParam.FirstDispColumn;j++)
				{
					for(int n=0;n<ProductParam.SecondDispColumn;n++)
					{
						for(int l=0;l<(ProductParam.ThirdDispColumn+2)/3;l++)
						{
							for(int i=0;i<ProductParam.FirstDispRow;i++)
							{
								for(int m=0;m<ProductParam.SecondDispRow;m++)
								{
									for(int k=0;k<ProductParam.ThirdDispRow;k++)
									{
										int nDisNumber = l+k*ProductParam.ThirdDispColumn+n*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
											j*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+i*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
										if(bIsDispense[nDisNumber])
										{
											g_pFrm->m_CmdRun.RunDispFSelect[0] = true;
											g_pFrm->m_CmdRun.ProductParam.DispFCount++;
										}
										if(bIsDispense[nDisNumber+(ProductParam.ThirdDispColumn+2)/3])
										{
											g_pFrm->m_CmdRun.RunDispFSelect[1] = true;
											g_pFrm->m_CmdRun.ProductParam.DispFCount++;
										}
										if(l+(ProductParam.ThirdDispColumn+2)/3*2<ProductParam.ThirdDispColumn)
										{
											if(bIsDispense[nDisNumber+(ProductParam.ThirdDispColumn+2)/3*2])
											{
												g_pFrm->m_CmdRun.RunDispFSelect[2] = true;
												g_pFrm->m_CmdRun.ProductParam.DispFCount++;
											}
										}
										if(!SelectDispense(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
											PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
										{
											m_bRepair = false;
											return;
										}
										g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
										for(int i=0;i<3;i++)
										{
											RunDispFSelect[i] = false;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		else
		{
			if(ProductParam.FirstDispColumn>1)
			{
				for(int i=0;i<ProductParam.FirstDispRow;i++)
				{
					for(int m=0;m<ProductParam.SecondDispRow;m++)
					{
						for(int k=0;k<ProductParam.ThirdDispRow;k++)
						{
							for(int j=0;j<(ProductParam.FirstDispColumn+2)/3;j++)
							{
								for(int n=0;n<ProductParam.SecondDispColumn;n++)
								{
									for(int l=0;l<ProductParam.ThirdDispColumn;l++)
									{
										int nDisNumber = l+k*ProductParam.ThirdDispColumn+n*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
											j*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+i*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
										if(bIsDispense[nDisNumber])
										{
											str.Format("RunDisp:%d",nDisNumber);
											AddMsg(str);
											g_pFrm->m_CmdRun.RunDispFSelect[0] = true;
											g_pFrm->m_CmdRun.ProductParam.DispFCount++;
										}
										if(bIsDispense[nDisNumber+(ProductParam.FirstDispColumn+2)/3*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn])
										{
											str.Format("RunDisp:%d",nDisNumber+(ProductParam.FirstDispColumn+2)/3*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn);
											AddMsg(str);
											g_pFrm->m_CmdRun.RunDispFSelect[1] = true;
											g_pFrm->m_CmdRun.ProductParam.DispFCount++;
										}
										if(j+(ProductParam.FirstDispColumn+2)/3*2<ProductParam.FirstDispColumn)
										{
											if(bIsDispense[nDisNumber+(ProductParam.FirstDispColumn+2)/3*2*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn])
											{
												str.Format("RunDisp:%d",nDisNumber+(ProductParam.FirstDispColumn+2)/3*2*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn);
												AddMsg(str);
												g_pFrm->m_CmdRun.RunDispFSelect[2] = true;
												g_pFrm->m_CmdRun.ProductParam.DispFCount++;
											}
										}
										if(!SelectDispense(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
											PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
										{
											m_bRepair = false;
											return;
										}
										g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
										for(int i=0;i<3;i++)
										{
											RunDispFSelect[i] = false;
										}
									}
								}
							}
						}
					}
				}
			}
			else if(ProductParam.SecondDispColumn>1)
			{
				for(int i=0;i<ProductParam.FirstDispRow;i++)
				{
					for(int m=0;m<ProductParam.SecondDispRow;m++)
					{
						for(int k=0;k<ProductParam.ThirdDispRow;k++)
						{
							for(int j=0;j<ProductParam.FirstDispColumn;j++)
							{
								for(int n=0;n<(ProductParam.SecondDispColumn+2)/3;n++)
								{
									for(int l=0;l<ProductParam.ThirdDispColumn;l++)
									{
										int nDisNumber = l+k*ProductParam.ThirdDispColumn+n*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
											j*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+i*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
										if(bIsDispense[nDisNumber])
										{
											g_pFrm->m_CmdRun.RunDispFSelect[0] = true;
											g_pFrm->m_CmdRun.ProductParam.DispFCount++;
										}
										if(bIsDispense[nDisNumber+(ProductParam.SecondDispColumn+2)/3*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn])
										{
											g_pFrm->m_CmdRun.RunDispFSelect[1] = true;
											g_pFrm->m_CmdRun.ProductParam.DispFCount++;
										}
										if(n+(ProductParam.SecondDispColumn+2)/3*2<ProductParam.SecondDispColumn)
										{
											if(bIsDispense[nDisNumber+(ProductParam.SecondDispColumn+2)/3*2*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn])
											{
												g_pFrm->m_CmdRun.RunDispFSelect[2] = true;
												g_pFrm->m_CmdRun.ProductParam.DispFCount++;
											}
										}
										if(!SelectDispense(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
											PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
										{
											m_bRepair = false;
											return;
										}
										g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
										for(int i=0;i<3;i++)
										{
											RunDispFSelect[i] = false;
										}
									}
								}
							}
						}
					}
				}
			}
			else
			{
				for(int i=0;i<ProductParam.FirstDispRow;i++)
				{
					for(int m=0;m<ProductParam.SecondDispRow;m++)
					{
						for(int k=0;k<ProductParam.ThirdDispRow;k++)
						{
							for(int j=0;j<ProductParam.FirstDispColumn;j++)
							{
								for(int n=0;n<ProductParam.SecondDispColumn;n++)
								{
									for(int l=0;l<(ProductParam.ThirdDispColumn+2)/3;l++)
									{
										int nDisNumber = l+k*ProductParam.ThirdDispColumn+n*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
											j*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+i*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
										if(bIsDispense[nDisNumber])
										{
											g_pFrm->m_CmdRun.RunDispFSelect[0] = true;
											g_pFrm->m_CmdRun.ProductParam.DispFCount++;
										}
										if(bIsDispense[nDisNumber+(ProductParam.ThirdDispColumn+2)/3])
										{
											g_pFrm->m_CmdRun.RunDispFSelect[1] = true;
											g_pFrm->m_CmdRun.ProductParam.DispFCount++;
										}
										if(l+(ProductParam.ThirdDispColumn+2)/3*2<ProductParam.ThirdDispColumn)
										{
											if(bIsDispense[nDisNumber+(ProductParam.ThirdDispColumn+2)/3*2])
											{
												g_pFrm->m_CmdRun.RunDispFSelect[2] = true;
												g_pFrm->m_CmdRun.ProductParam.DispFCount++;
											}
										}
										if(!SelectDispense(PDposting[nDisNumber].Dx,PDposting[nDisNumber].Dy,
											PDposting[nDisNumber].Dza,PDposting[nDisNumber].Dzb,PDposting[nDisNumber].Dzc))
										{
											m_bRepair = false;
											return;
										}
										g_pFrm->m_CmdRun.ProductParam.DispFCount = 0;
										for(int i=0;i<3;i++)
										{
											RunDispFSelect[i] = false;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

bool CCmdRun::SpliceInspect(bool bSplice)
{
	CString str;
	tgPos tgStartPixel;
	double dFirstPixelX,dFirstPixelY,dSecondPixelX,dSecondPixelY,dThirdPixelX,dThirdPixelY;
	CRect rect;
	bool *pDetectResult=NULL;
	int nPadNumber;
	int nTotalNumbers;

	DWORD W_Startime,W_EndTime;
	double dTimeSingleStep;
	W_Startime = W_EndTime = GetTickCount();

	str.Format("���Զ�ģʽ����ʼ����ȱ�����������");	
	AddMsg(str);
	PutLogIntoList(str);

	theApp.m_tSysParam.BmpPadDetectCalibration.ConvertMMToPixsel(ProductParam.FirstDispColumnD,ProductParam.FirstDispRowD,dFirstPixelX,dFirstPixelY);
	theApp.m_tSysParam.BmpPadDetectCalibration.ConvertMMToPixsel(ProductParam.SecondDispColumnD,ProductParam.SecondDispRowD,dSecondPixelX,dSecondPixelY);
	theApp.m_tSysParam.BmpPadDetectCalibration.ConvertMMToPixsel(ProductParam.ThirdDispColumnD,ProductParam.ThirdDispRowD,dThirdPixelX,dThirdPixelY);

	m_pMv->SetOutput(theApp.m_tSysParam.outDetectLight,TRUE);
	CTimeUtil::Delay(long(ProductParam.dGrabImageDelay*1000));
	str.Format("start DetectSnap/Copy image to Mil...");
	PutLogIntoList(str);
	////////////////////////////////////////////////////////////
	//2017-07-27:ȡ����֡�ɼ� ��һֱ����ʵʱģʽ��
	/*if(!g_pView->m_ImgStatic.DetectSnap())
	{
	m_pMv->SetOutput(theApp.m_tSysParam.outDetectLight,FALSE); 
	return false;
	}*/

	//m_bPadDetecting=true;
	g_pView->m_ImgStatic.DetectSnap();
	//Sleep(6000);
	W_EndTime=GetTickCount();
	dTimeSingleStep=(W_EndTime-W_Startime);
	W_Startime=W_EndTime=GetTickCount();

	W_Startime=W_EndTime=GetTickCount();
	m_pMv->SetOutput(theApp.m_tSysParam.outDetectLight,FALSE);
	str.Format("end DetectSnap/Copy image to Mil Over...");
	PutLogIntoList(str);
	str.Format("���Զ�ģʽ/ȡ��ɹ���ȱ������֡ȡ���ʱ��%0.3f S...��",dTimeSingleStep);
	//AddMsg(str);
	PutLogIntoList(str);
	//g_pView->m_ImgStatic.DetectContinueGrab();      //2017-11-18  ȡ�������������ʵʱģʽ

	//compute search zone
	//rect.left=theApp.m_tSysParam.BmpPadDetectROI[0].left+(theApp.m_tSysParam.BmpPadDetectROI[0].Width()/2);
	//rect.bottom=theApp.m_tSysParam.BmpPadDetectROI[0].top+(theApp.m_tSysParam.BmpPadDetectROI[0].Height()/2);
	rect.left=theApp.m_tSysParam.BmpPadDetectROI[0].left;
	rect.top=theApp.m_tSysParam.BmpPadDetectROI[0].top;
	rect.right=theApp.m_tSysParam.BmpPadDetectROI[0].right;
	rect.bottom=theApp.m_tSysParam.BmpPadDetectROI[0].bottom;

	CString strFile = g_pDoc->GetPadModelFile(0);
	milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));
	milApp.SetPadSearchParam(theApp.m_tSysParam.BmpPadDetectAcceptance[0],theApp.m_tSysParam.BmpPadDetectAngle[0]);
	milApp.SetPadModelWindow(theApp.m_tSysParam.BmpPadDetectLearnWin[0]);
	milApp.SetPadSearchWindow(rect);
	if(!milApp.FindPadModels())
	{
		CString str;
		str.Format("���Զ����У�ȱ������⡿����padʱʧ��//�˳�...");
		PutLogIntoList(str);
		AddMsg(str);
		return false;
	}
	if(milApp.m_stPadResult.nResultNum<=0)
	{
		CString str;
		str.Format("���Զ����У�ȱ������⡿����padʱƥ����ĿΪ0//�˳�...");
		PutLogIntoList(str);
		AddMsg(str);
		return false;
	}
	else
	{
		W_EndTime=GetTickCount();
		dTimeSingleStep=(W_EndTime-W_Startime)/1000.00;
		W_Startime=W_EndTime=GetTickCount();

		str.Format("Pad match time :%0.3f S",dTimeSingleStep);
		AddMsg(str);
		PutLogIntoList(str);

		str.Format("���Զ�����ȫ��Ƭ��⡿��ʼ�����������...");
		PutLogIntoList(str);
		AddMsg(str);

		nTotalNumbers=milApp.m_stPadResult.nResultNum; 
		tgStartPixel.x=milApp.m_stPadResult.dResultCenterX[0];
		tgStartPixel.y=milApp.m_stPadResult.dResultCenterY[0];
		for (int i=0;i<nTotalNumbers;i++)
		{
			if (milApp.m_stPadResult.dResultCenterX[i]>tgStartPixel.x)
			{
				tgStartPixel.x=milApp.m_stPadResult.dResultCenterX[i];
			}
			if (milApp.m_stPadResult.dResultCenterY[i]<tgStartPixel.y)
			{
				tgStartPixel.y=milApp.m_stPadResult.dResultCenterY[i];
			}
		}

		if ((tgStartPixel.x<rect.left)||
			(tgStartPixel.x>rect.right)||
			(tgStartPixel.y<rect.top)||
			(tgStartPixel.y>rect.bottom)
			)
		{
			CString str;
			str.Format("���Զ����У�ȱ������⡿������λ�ü����쳣//�˳�...");
			g_pFrm->m_CmdRun.PutLogIntoList(str);
			g_pFrm->m_CmdRun.AddMsg(str);
			return false;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	//��ʼ������ռ�
	try
	{
		if(pDetectResult!=NULL)
		{
			delete[] pDetectResult;
			pDetectResult = NULL;
		}
		nPadNumber = g_pFrm->m_CmdRun.ProductParam.FirstDispRow*g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*g_pFrm->m_CmdRun.ProductParam.SecondDispRow*
			g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*g_pFrm->m_CmdRun.ProductParam.ThirdDispRow*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn;
		pDetectResult = new bool[nPadNumber];//���Ϸ����ڴ�
		if (NULL==pDetectResult)
		{
			AfxMessageBox("���Զ����У�ȱ�������ڴ濪��ʧ��...��");
			//g_pView->m_ImgStatic.DetectContinueGrab();      //2017-09-07
			return false; 
		}
	}
	catch(...)
	{
		CString str;
		str.Format("SpliceInspect����pDetectResult�ڴ�����ͷŲ����쳣... ");
		if (NULL!=g_pFrm)
		{
			g_pFrm->m_CmdRun.PutLogIntoList(str);
		}
		AfxMessageBox("SpliceInspect����pDetectResult�ڴ�����쳣");

	}
	for(int i=0;i<nPadNumber;i++)
	{
		if (NULL==pDetectResult)
		{
			str.Format("���Զ����У�ȱ������⣬��ʼ�����ռ仺��ʱʧ�ܡ�");
			AddMsg(str);
			PutLogIntoList(str);
			//g_pView->m_ImgStatic.DetectContinueGrab(); //2017-09-07
			return false;
		}
		pDetectResult[i] = false;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	int nThirdNum = g_pFrm->m_CmdRun.ProductParam.ThirdDispRow*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn;
	int nSecondNum = g_pFrm->m_CmdRun.ProductParam.SecondDispRow*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn;
	int nFindNum=0;
	nTempLackStatic=0;//���ȱʧ��Ŀ
	if(true)
	{              
		//�����׵����������//ÿ�ι̶����׿ŵ���������ֵ//����׿ű����쳣������
		//�˴�Ӧ��֧�ܵ�ƫ�����������׵��ƫ������
		for(int i=0;i<g_pFrm->m_CmdRun.ProductParam.FirstDispRow;i++)
		{
			for(int j=0;j<g_pFrm->m_CmdRun.ProductParam.FirstDispColumn;j++)
			{
				for(int m=0;m<g_pFrm->m_CmdRun.ProductParam.SecondDispRow;m++)
				{
					for(int n=0;n<g_pFrm->m_CmdRun.ProductParam.SecondDispColumn;n++)
					{
						for(int k=0;k<g_pFrm->m_CmdRun.ProductParam.ThirdDispRow;k++)
						{
							for(int l=0;l<g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn;l++)//Right to left;
							{
								double dPixelX,dPixelY;
								int temID = l + k*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn + n*nThirdNum + m*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*nThirdNum +
									j*nSecondNum*nThirdNum + i*g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*nSecondNum*nThirdNum;
								//��ǰ�������׿ŵ���Ծ��룺mm
								double dDistanceX = j*g_pFrm->m_CmdRun.ProductParam.FirstDispColumnD + n*g_pFrm->m_CmdRun.ProductParam.SecondDispColumnD +
									l*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD;
								double dDistanceY = i*g_pFrm->m_CmdRun.ProductParam.FirstDispRowD + m*g_pFrm->m_CmdRun.ProductParam.SecondDispRowD +
									k*g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD;
								//��Ծ��룺mm-->pixsel
								theApp.m_tSysParam.BmpPadDetectCalibration.ConvertMMToPixsel(dDistanceX,dDistanceY,dPixelX,dPixelY);
								dPixelX=fabs(dPixelX);

								long lTempX,lTempY;
								//������������:���ʵ������λ��////////
								lTempX=tgStartPixel.x - dPixelX;//on the left,X decrease;
								lTempY= tgStartPixel.y + dPixelY;//on the bottom:Y increased;
								//ˢ����������
								rect.left =lTempX - (0.65*dThirdPixelX);
								rect.right = lTempX +  (0.65*dThirdPixelX);
								rect.top = lTempY - (0.65*dThirdPixelY);
								rect.bottom = lTempY + (0.65*dThirdPixelY);
								////////////////////////////////////////////////////////////////////////////
								nTotalNumbers=milApp.m_stPadResult.nResultNum;
								for (int k=0;k<nTotalNumbers;k++)
								{
									double PadX=milApp.m_stPadResult.dResultCenterX[k];
									double PadY=milApp.m_stPadResult.dResultCenterY[k];
									if (IsInRect(PadX,PadY,rect))
									{
										pDetectResult[temID] = true;
										nFindNum++;
										break;
									}
									else
									{
										pDetectResult[temID]=false;
									}

								}
								if (!pDetectResult[temID])
								{
									nTempLackStatic++;    //ͳ����Ŀ
								}

							}
						}
					}
				}
			}
		}
	}

	str.Format("��Ƭ����һ�� %d �ţ����ʶ����Ŀ %d ��...",nPadNumber,nFindNum);
	AddMsg(str);
	PutLogIntoList(str);

	W_EndTime=GetTickCount();
	dTimeSingleStep=(W_EndTime-W_Startime)/1000.00;
	W_Startime=W_EndTime=GetTickCount();
	str.Format("��ż��㹲��ʱ��%0.3f",dTimeSingleStep);
	AddMsg(str);
	PutLogIntoList(str);

	g_csPadDetect.Lock();
	if(m_pTransitionDetect==NULL)
	{
		m_pTransitionDetect = new bool[m_nPadNumber];
	}
	if(NULL==m_pTransitionDetect)
	{
		if(pDetectResult!=NULL)         ////////��ֹй©
		{                               ////////��ֹй©
			delete[] pDetectResult;     ////////��ֹй©
			pDetectResult = NULL;       ////////��ֹй©
		}                               ////////��ֹй©       //2017-11-18
		g_csPadDetect.Unlock();
		AfxMessageBox("SpiceInspect() Line 14550 �쳣...");
		return false;
	}
	memcpy(m_pTransitionDetect,pDetectResult,sizeof(bool)*m_nPadNumber);  //write address 
	g_csPadDetect.Unlock();

	W_EndTime=GetTickCount();
	dTimeSingleStep=(W_EndTime-W_Startime)/1000.00;
	str.Format("Pad ������忽����ʱ��%0.3f S...",dTimeSingleStep);
	AddMsg(str);
	PutLogIntoList(str);


	if (NULL!=pDetectResult)
	{
		delete[] pDetectResult;
		pDetectResult = NULL;
	}
	str.Format("���Զ����У�ȱ�������ͼ������ɡ�");
	AddMsg(str);
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	//g_pView->m_ImgStatic.DetectContinueGrab();//2017-09-07
	return true;
}

bool CCmdRun::SpliceInspectEx(bool bSplice)
{
	CString str;
	tgPos tgStartPixel;
	bool *pDetectResult=NULL;
	double dFirstPixelX,dFirstPixelY,dSecondPixelX,dSecondPixelY,dThirdPixelX,dThirdPixelY;
	DWORD W_Startime,W_EndTime;
	double dTimeSingleStep;
	nFind=0; 
	W_Startime = W_EndTime = GetTickCount();

	CRect rect;
	rect.left = theApp.m_tSysParam.BmpPadDetectROI[0].left;
	rect.right = theApp.m_tSysParam.BmpPadDetectROI[0].right;
	rect.top = theApp.m_tSysParam.BmpPadDetectROI[0].top;
	rect.bottom = theApp.m_tSysParam.BmpPadDetectROI[0].bottom;
	str.Format("SpliceInspectEx��ROI��ʼ����ֵ[L:%d;R:%d;T:%d;B:%d.]",rect.left,rect.right,rect.top,rect.bottom);
	AddMsg(str);
	rect.NormalizeRect();
	str.Format("�淶����:SpliceInspectEx��ROI��ʼ����ֵ[L:%d;R:%d;T:%d;B:%d.]",rect.left,rect.right,rect.top,rect.bottom);
	AddMsg(str);

	theApp.m_tSysParam.BmpPadDetectCalibration.ConvertMMToPixsel(g_pFrm->m_CmdRun.ProductParam.FirstDispColumnD,g_pFrm->m_CmdRun.ProductParam.FirstDispRowD,dFirstPixelX,dFirstPixelY);
	theApp.m_tSysParam.BmpPadDetectCalibration.ConvertMMToPixsel(g_pFrm->m_CmdRun.ProductParam.SecondDispColumnD,g_pFrm->m_CmdRun.ProductParam.SecondDispRowD,dSecondPixelX,dSecondPixelY);
	theApp.m_tSysParam.BmpPadDetectCalibration.ConvertMMToPixsel(g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD,g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD,dThirdPixelX,dThirdPixelY);	

	if(pDetectResult!=NULL)
	{
		delete[] pDetectResult;
		pDetectResult = NULL;
	}
	pDetectResult = new bool[m_nPadNumber];
	if (NULL==pDetectResult)
	{
		str.Format("SpliceInspectEx pDetectResult�����ڴ�ʧ��.");
		AddMsg(str);
		PutLogIntoList(str);
		return false;
	}
	memset(pDetectResult,0,m_nPadNumber*sizeof(bool));  //init all missing;

	int nThirdNum = ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
	int nSecondNum = ProductParam.SecondDispRow*ProductParam.SecondDispColumn;
	////////////////////////////////////////////////////////////////////////////////////
	int nStartColumn = 0;
	int nEndColumn = 0;			

	int nIndex = int(dFirstPixelX*ProductParam.FirstDispColumn+theApp.m_tSysParam.BmpPadDetectROI[0].Width())/theApp.m_tSysParam.BmpPadDetectROI[0].Width();
	str.Format("Ready to PadDetect Section>>");
	AddMsg(str);
	PutLogIntoList(str);

	for(int i=0;i<nIndex;i++)
	{
		str.Format("PadDetect Section %d Start...",i);
		AddMsg(str);
		PutLogIntoList(str);

		CRect RectFresh;
		RectFresh.top=rect.top;
		RectFresh.bottom=rect.bottom;
		RectFresh.right=rect.right-i*rect.Width();
		RectFresh.left=rect.left-i*rect.Width();
		//ȡ��
		m_pMv->SetOutput(theApp.m_tSysParam.outDetectLight,TRUE);
		Sleep(ProductParam.dGrabImageDelay*1000); 
		if(!g_pView->m_ImgStatic.DetectSnap())
		{
			str.Format("********�ֶμ��ʱ���ȡ��ʧ�ܣ�********");
			PutLogIntoList(str);
			AddMsg(str);

			g_pView->m_ImgStatic.DetectCamReConnect();
			str.Format("ȱ���������������һ��");
			PutLogIntoList(str);
			AddMsg(str);

			Sleep(2000);
			if(!g_pView->m_ImgStatic.DetectSnap())
			{
				str.Format("�ֶμ��ʱ����ٴ�ȡ��ʧ�ܣ�");
				PutLogIntoList(str);
				AddMsg(str);

				m_pMv->SetOutput(theApp.m_tSysParam.outDetectLight,FALSE);
				if(pDetectResult!=NULL)
				{
					delete[] pDetectResult;
					pDetectResult = NULL;
				}
				return false;
			}
			str.Format("�ֶμ��ʱ���������ȡ��ok��");
			PutLogIntoList(str);
			AddMsg(str);
		}
		m_pMv->SetOutput(theApp.m_tSysParam.outDetectLight,FALSE);
		
		str.Format("��%d�Σ�SpliceInspectEx��ROI��ʼ����ֵ[L:%d;R:%d;T:%d;B:%d.]",i,RectFresh.left,RectFresh.right,RectFresh.top,RectFresh.bottom);
		AddMsg(str);

		RectFresh.NormalizeRect();
		str.Format("�淶����:��%d�Σ�SpliceInspectEx��ROI��ʼ����ֵ[L:%d;R:%d;T:%d;B:%d.]",i,RectFresh.left,RectFresh.right,RectFresh.top,RectFresh.bottom);
		AddMsg(str); 

		AddMsg("Start SplicePadDetection Function");
		if(!SplicePadDetection(pDetectResult/*pPadDetect*/,nStartColumn,nEndColumn,RectFresh))
		{
			if(nEndColumn>=ProductParam.ThirdDispColumn*ProductParam.SecondDispColumn*ProductParam.FirstDispColumn)
			{
				break;
			}
			if(pDetectResult!=NULL)
			{
				delete[] pDetectResult;
				pDetectResult = NULL;
			}
			str.Format("�ֶμ��ʱ��ģ��ʧ�ܣ�");
			AddMsg(str);
			PutLogIntoList(str);
			return false;
		}
		AddMsg("End SplicePadDetection Function");
		nStartColumn = nEndColumn;
	}

	str.Format("PadDetect Section Finish,Ready to Copy...");
	AddMsg(str);
	PutLogIntoList(str);
	///////////////////////////////////////////////////////////////////////////////////////////////////
	if (NULL!=m_pTransitionDetect)
	{
		delete[] m_pTransitionDetect;
		m_pTransitionDetect=NULL;
	}
	if(m_pTransitionDetect==NULL)
	{
		m_pTransitionDetect = new bool[m_nPadNumber];
	}
	if (NULL==m_pTransitionDetect)
	{
		str.Format("SpliceInspectEx m_pTransitionDetect �����ڴ�ʧ��.");
		AddMsg(str);
		PutLogIntoList(str);
		if (NULL!=pDetectResult)
		{
			delete[] pDetectResult;
			pDetectResult=NULL;
		}
		return false;
	}


	str.Format("��������ǰͳ�Ƽ����");
	AddMsg(str);
	PutLogIntoList(str);
	int nPadMissingNum=0;
	for (int i=0;i<m_nPadNumber;i++)
	{
		if (!pDetectResult[i])
		{
			nPadMissingNum++;
		}

	}
	str.Format("��������ǰͳ�ƽ��Ϊ:%d",nPadMissingNum);
	AddMsg(str);
	PutLogIntoList(str);

	AddMsg("Start copy");
	memcpy(m_pTransitionDetect,pDetectResult,sizeof(bool)*m_nPadNumber);
	AddMsg("End copy");

	str.Format("�������ݺ�ͳ�Ƽ����");
	AddMsg(str);
	PutLogIntoList(str);
	nPadMissingNum=0;
	for (int i=0;i<m_nPadNumber;i++)
	{
		if (!m_pTransitionDetect[i])
		{
			nPadMissingNum++;
		}

	}
	nTempLackStatic=nPadMissingNum;
	str.Format("�������ݺ�ͳ�ƽ��Ϊ:%d",nPadMissingNum);
	AddMsg(str);
	PutLogIntoList(str);

	delete[] pDetectResult;
	pDetectResult = NULL;

	W_EndTime=GetTickCount();
	dTimeSingleStep=(W_EndTime-W_Startime)/1000.00;
	str.Format("Pad ����ʱ��%0.3f S...",dTimeSingleStep);
	AddMsg(str);
	PutLogIntoList(str);
	return true;
}

bool CCmdRun::SpliceInspectUpGrade(bool bSplice)
{
	CString str;
	CString strFile;
	double dTimeSingleStep;
	int nFindNum=0;
	tgPos tgStartPixel;
	bool *pDetectResult=NULL;
	double dFirstPixelX,dFirstPixelY,dSecondPixelX,dSecondPixelY,dThirdPixelX,dThirdPixelY;
	DWORD W_Startime,W_EndTime;
	nFind=0; 
	W_Startime = W_EndTime = GetTickCount();

	CRect rect;
	rect.left = theApp.m_tSysParam.BmpPadDetectROI[0].left;
	rect.right = theApp.m_tSysParam.BmpPadDetectROI[0].right;
	rect.top = theApp.m_tSysParam.BmpPadDetectROI[0].top;
	rect.bottom = theApp.m_tSysParam.BmpPadDetectROI[0].bottom;
	str.Format("SpliceInspectEx��ROI��ʼ����ֵ[L:%d;R:%d;T:%d;B:%d.]",rect.left,rect.right,rect.top,rect.bottom);
	//AddMsg(str);
	rect.NormalizeRect();
	str.Format("�淶����:SpliceInspectEx��ROI��ʼ����ֵ[L:%d;R:%d;T:%d;B:%d.]",rect.left,rect.right,rect.top,rect.bottom);
	//AddMsg(str);

	theApp.m_tSysParam.BmpPadDetectCalibration.ConvertMMToPixsel(g_pFrm->m_CmdRun.ProductParam.FirstDispColumnD,g_pFrm->m_CmdRun.ProductParam.FirstDispRowD,dFirstPixelX,dFirstPixelY);
	theApp.m_tSysParam.BmpPadDetectCalibration.ConvertMMToPixsel(g_pFrm->m_CmdRun.ProductParam.SecondDispColumnD,g_pFrm->m_CmdRun.ProductParam.SecondDispRowD,dSecondPixelX,dSecondPixelY);
	theApp.m_tSysParam.BmpPadDetectCalibration.ConvertMMToPixsel(g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD,g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD,dThirdPixelX,dThirdPixelY);	

	if(pDetectResult!=NULL)
	{
		delete[] pDetectResult;
		pDetectResult = NULL;
	}
	pDetectResult = new bool[m_nPadNumber];
	if(NULL==pDetectResult)
	{
		str.Format("SpliceInspectEx pDetectResult�����ڴ�ʧ��.");
		//AddMsg(str);
		PutLogIntoList(str);
		return false;
	}
	memset(pDetectResult,1,m_nPadNumber*sizeof(bool));  //init all missing;

	int nThirdNum = ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
	int nSecondNum = ProductParam.SecondDispRow*ProductParam.SecondDispColumn;
	////////////////////////////////////////////////////////////////////////////////////
	str.Format("Ready to PadDetect Section>>");
	//AddMsg(str);
	PutLogIntoList(str);

	m_pMv->SetOutput(theApp.m_tSysParam.outDetectLight,TRUE);
	Sleep(ProductParam.dGrabImageDelay*1000); 
	if(!g_pView->m_ImgStatic.DetectSnap())
	{
		m_pMv->SetOutput(theApp.m_tSysParam.outDetectLight,false);
		if(pDetectResult!=NULL)
		{
			delete[] pDetectResult;
			pDetectResult = NULL;
		}
		str.Format("********ȱ�������ȡ��ʧ�ܣ�********");
		PutLogIntoList(str);
		//AddMsg(str);
		return false;
	}
	m_pMv->SetOutput(theApp.m_tSysParam.outDetectLight,false);
	//g_pView->m_ImgStatic.DetectContinueGrab();//////
	/////////////////////////////////////////////////////////////////
	rect.left = rect.left + rect.Width()/2;
	rect.right = rect.right;
	rect.top = rect.top;
	rect.bottom = rect.top + rect.Height()/2;

	strFile = g_pDoc->GetPadModelFile(0);
	milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));
	milApp.SetPadSearchParam(theApp.m_tSysParam.BmpPadDetectAcceptance[0],theApp.m_tSysParam.BmpPadDetectAngle[0]);
	milApp.SetPadModelWindow(theApp.m_tSysParam.BmpPadDetectLearnWin[0]);
	milApp.SetPadSearchWindow(rect);

	if(!milApp.FindPadModels())
	{
		if(pDetectResult!=NULL)
		{
			delete[] pDetectResult;
			pDetectResult = NULL;
		}
		str.Format("����ʱRT-1/4�ֲ�����ʧ��");
		//AddMsg(str);
		PutLogIntoList(str);
		return false;
	}

	if(milApp.m_stPadResult.nResultNum==0)
	{
		if(pDetectResult!=NULL)
		{
			delete[] pDetectResult;
			pDetectResult = NULL;
		}
		str.Format("����ʱRT-1/4�ֲ�������ĿΪ0,ʧ��");
		//AddMsg(str);
		PutLogIntoList(str);
		return false;
	}
	else if(milApp.m_stPadResult.nResultNum>0)
	{
		tgStartPixel.x = milApp.m_stPadResult.dResultCenterX[0];
		tgStartPixel.y = milApp.m_stPadResult.dResultCenterY[0];
		for(int i=0;i<milApp.m_stPadResult.nResultNum;i++)
		{
			if(tgStartPixel.x<milApp.m_stPadResult.dResultCenterX[i])
			{
				tgStartPixel.x = milApp.m_stPadResult.dResultCenterX[i];
			}
			if(tgStartPixel.y>milApp.m_stPadResult.dResultCenterY[i])
			{
				tgStartPixel.y = milApp.m_stPadResult.dResultCenterY[i];
			}
		}
		if(tgStartPixel.y>rect.top + dThirdPixelY)
		{
			tgStartPixel.y = rect.top + dThirdPixelY/2;
		}
		if(tgStartPixel.y<rect.top)//20180907
		{
			tgStartPixel.y = rect.top + dThirdPixelY/2;
		}
		if(tgStartPixel.x<rect.right - dThirdPixelX)
		{
			tgStartPixel.x = rect.right - dThirdPixelX/2;
		}
		if(tgStartPixel.x>rect.right) //20180907
		{
			tgStartPixel.x = rect.right - dThirdPixelX/2;
		}
		str.Format("�Զ�����--->>������ҡ������׵��������꣺X:%0.3f,Y:%0.3f",tgStartPixel.x,tgStartPixel.y);
		//AddMsg(str);
		PutLogIntoList(str);

		if(true)
		{              
			//�����׵����������//ÿ�ι̶����׿ŵ���������ֵ//����׿ű����쳣������
			//�˴�Ӧ��֧�ܵ�ƫ�����������׵��ƫ������
			for(int i=0;i<g_pFrm->m_CmdRun.ProductParam.FirstDispRow;i++)
			{
				for(int j=0;j<g_pFrm->m_CmdRun.ProductParam.FirstDispColumn;j++)
				{
					for(int m=0;m<g_pFrm->m_CmdRun.ProductParam.SecondDispRow;m++)
					{
						for(int n=0;n<g_pFrm->m_CmdRun.ProductParam.SecondDispColumn;n++)
						{
							for(int k=0;k<g_pFrm->m_CmdRun.ProductParam.ThirdDispRow;k++)
							{
								for(int l=0;l<g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn;l++)//Right to left;
								{
									Sleep(1);
									double dPixelX,dPixelY;
									int temID = l + k*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn + n*nThirdNum + m*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*nThirdNum +
										j*nSecondNum*nThirdNum + i*g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*nSecondNum*nThirdNum;
									//��ǰ�������׿ŵ���Ծ��룺mm
									double dDistanceX = j*g_pFrm->m_CmdRun.ProductParam.FirstDispColumnD + n*g_pFrm->m_CmdRun.ProductParam.SecondDispColumnD +
										l*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD;
									double dDistanceY = i*g_pFrm->m_CmdRun.ProductParam.FirstDispRowD + m*g_pFrm->m_CmdRun.ProductParam.SecondDispRowD +
										k*g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD;
									//��Ծ��룺mm-->pixsel
									theApp.m_tSysParam.BmpPadDetectCalibration.ConvertMMToPixsel(dDistanceX,dDistanceY,dPixelX,dPixelY);
									dPixelX=fabs(dPixelX);

									long lTempX,lTempY;
									//������������:���ʵ������λ��////////
									lTempX=tgStartPixel.x - dPixelX;//on the left,X decrease;
									lTempY= tgStartPixel.y + dPixelY;//on the bottom:Y increased;
									//ˢ����������
									rect.left =lTempX - (0.5*dThirdPixelX);
									rect.right = lTempX +  (0.5*dThirdPixelX);
									rect.top = lTempY - (0.5*dThirdPixelY);
									rect.bottom = lTempY + (0.5*dThirdPixelY);

									//str.Format("Pad�����ԣ���%d��⿪ʼ",temID);
									//g_pFrm->PutLogIntoList(str);
									if(K_RUN_STS_EMERGENCY==m_tStatus)
									{
										if(pDetectResult!=NULL)
										{
											delete[] pDetectResult;
											pDetectResult = NULL;
										}
										str.Format("�Զ�����->ȱ�������ʱ����ͣ���˳����");
										g_pFrm->m_CmdRun.PutLogIntoList(str);
										//g_pFrm->m_CmdRun.AddMsg(str);
										return false;
									}
									strFile = g_pDoc->GetPadModelFile(0);
									milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));//����ģ��1...
									milApp.SetPadSearchWindow(rect);
									if(!milApp.FindPadModels())
									{
										if(pDetectResult!=NULL)
										{
											delete[] pDetectResult;
											pDetectResult = NULL;
										}
										return false;
									}
									if(milApp.m_stPadResult.nResultNum>0)
									{
										//Draw the first pat result on UI;
										pDetectResult[temID] = true;
										nFindNum++;
									}
									else
									{
										////////////////////////////�ٴζ��Ҽ���/////////////////
										str.Format("[...�в����״μ��ʧ��]");
										g_pFrm->m_CmdRun.PutLogIntoList(str);
										//g_pFrm->m_CmdRun.AddMsg(str);

										/*m_pMv->SetOutput(theApp.m_tSysParam.outDetectLight,TRUE);
										Sleep(ProductParam.dGrabImageDelay*1000); 
										if(!g_pView->m_ImgStatic.DetectSnap())
										{
											m_pMv->SetOutput(theApp.m_tSysParam.outDetectLight,false);
											if(pDetectResult!=NULL)
											{
												delete[] pDetectResult;
												pDetectResult = NULL;
											}
											str.Format("********ȱ�������ȡ��ʧ�ܣ�********");
											m_pLog->log(str);
											AddMsg(str);
											return false;
										}
										m_pMv->SetOutput(theApp.m_tSysParam.outDetectLight,false);*/
										
										str.Format("һ����:%d;һ����:%d;������:%d;������:%d;������%d;������:%d �ٴ�ȡͼ...",i,j,m,n,k,l);
										g_pFrm->m_CmdRun.PutLogIntoList(str);
										//g_pFrm->m_CmdRun.AddMsg(str);

										int recordInum=1;
										for(int i=1;i<5;i++)
										{
											Sleep(1);
											str.Format("���뱸�û���Pad����%d���!",i);
											g_pFrm->m_CmdRun.PutLogIntoList(str);
											//g_pFrm->m_CmdRun.AddMsg(str);
											strFile = g_pDoc->GetPadModelFile(i);
											milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));
											milApp.SetPadSearchWindow(rect);
											if(!milApp.FindPadModels())
											{
												if(pDetectResult!=NULL)
												{
													delete[] pDetectResult;
													pDetectResult = NULL;
												}
												return false;
											}
											if(milApp.m_stPadResult.nResultNum>0)
											{
												str.Format("���뱸�û��Ƶ�%d���ɹ�!",i);
												g_pFrm->m_CmdRun.PutLogIntoList(str);
												//g_pFrm->m_CmdRun.AddMsg(str);
												recordInum=i;
												break;
											}
										}
										if(milApp.m_stPadResult.nResultNum>0)
										{
											pDetectResult[temID] = true;
											nFindNum++;
										}
										else
										{
											str.Format("////****�в�������NG****����////");
											g_pFrm->m_CmdRun.PutLogIntoList(str);
											//g_pFrm->m_CmdRun.AddMsg(str);
											str.Format("һ����:%d;һ����:%d;������:%d;������:%d;������%d;������:%d nResultNum:%d",i,j,m,n,k,l,milApp.m_stPadResult.nResultNum);
											g_pFrm->m_CmdRun.PutLogIntoList(str);
											//g_pFrm->m_CmdRun.AddMsg(str);
											pDetectResult[temID] = false;
											nTempLackStatic++;
											padLackTotal++;
										}
										///////////////////////////////////////////////////////////
										str.Format("Pad���û��Ƽ�����!");
										g_pFrm->m_CmdRun.PutLogIntoList(str);
										//g_pFrm->m_CmdRun.AddMsg(str);

									}
									//str.Format("Pad�����ԣ���%d������!",temID);
									//g_pFrm->PutLogIntoList(str);
								}
							}
						}
					}
				}
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////
	str.Format("��������ǰͳ�Ƽ����");
	//AddMsg(str);
	PutLogIntoList(str);

	str.Format("�Զ�����ʱ����������ǰͳ�ƽ��Ϊ:%d",nFindNum);
	//AddMsg(str);
	PutLogIntoList(str);

	str.Format("�Զ�����ʱ���Կ������ĿǰΪֹ��ȱ������:%d",padLackTotal);
	//AddMsg(str);
	PutLogIntoList(str);
	

	//AddMsg("Start copy");
	g_csPadDetect.Lock();  
	memcpy(m_pTransitionDetect,pDetectResult,sizeof(bool)*m_nPadNumber);
	g_csPadDetect.Unlock();  
	//AddMsg("End copy");

	delete[] pDetectResult;
	pDetectResult = NULL;

	W_EndTime=GetTickCount();
	dTimeSingleStep=(W_EndTime-W_Startime)/1000.00;
	str.Format("�Զ�����ʱ:Pad ����ʱ��%0.3f S...",dTimeSingleStep);
	AddMsg(str);
	PutLogIntoList(str);
	return true;
}

bool CCmdRun::IsInRect(double x,double y,CRect rect)
{
	if ((x>rect.left)&&(x<rect.right)&&(y>rect.top)&&(y<rect.bottom))
	{
		return true;
	}
	return false;
}
bool CCmdRun::SplicePadDetection(bool *pResult,int nStartColumn,int &nEndColumn,CRect RectOriginal)
{
	CString str;
	tgPos tgStartPixel;
	double dFirstPixelX,dFirstPixelY,dSecondPixelX,dSecondPixelY,dThirdPixelX,dThirdPixelY;
	int nColumn = nStartColumn;
	int nFirstRow,nFirstColumn,nSecondRow,nSecondColumn,nThirdRow,nThirdColumn;
	nFirstColumn = nStartColumn/(g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn);
	nSecondColumn = nStartColumn/g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn%g_pFrm->m_CmdRun.ProductParam.SecondDispColumn;
	nThirdColumn = nStartColumn%g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn;
	theApp.m_tSysParam.BmpPadDetectCalibration.ConvertMMToPixsel(ProductParam.FirstDispColumnD,ProductParam.FirstDispRowD,dFirstPixelX,dFirstPixelY);
	theApp.m_tSysParam.BmpPadDetectCalibration.ConvertMMToPixsel(ProductParam.SecondDispColumnD,ProductParam.SecondDispRowD,dSecondPixelX,dSecondPixelY);
	theApp.m_tSysParam.BmpPadDetectCalibration.ConvertMMToPixsel(ProductParam.ThirdDispColumnD,ProductParam.ThirdDispRowD,dThirdPixelX,dThirdPixelY);
	CRect rect;
	rect.left = RectOriginal.left + RectOriginal.Width()/2;
	rect.right = RectOriginal.right;
	rect.top = RectOriginal.top;
	rect.bottom = RectOriginal.top + RectOriginal.Height()/2;
    
	str.Format("������ʱRT-1/4��ʱ����У��:�趨����λ��[L:%d;R:%d;T:%d;B:%d.] //ģ���[L:%d;R:%d;T:%d;B:%d.]//����:%0.3f /�Ƕ�:%0.3f.",rect.left,rect.right,rect.top,rect.bottom,
		theApp.m_tSysParam.BmpPadDetectLearnWin[0].left,theApp.m_tSysParam.BmpPadDetectLearnWin[0].right,theApp.m_tSysParam.BmpPadDetectLearnWin[0].top,theApp.m_tSysParam.BmpPadDetectLearnWin[0].bottom,theApp.m_tSysParam.BmpPadDetectAcceptance[0],theApp.m_tSysParam.BmpPadDetectAngle[0]);
	AddMsg(str);
	rect.NormalizeRect();
	theApp.m_tSysParam.BmpPadDetectLearnWin[0].NormalizeRect();
	str.Format("�淶����:������ʱRT-1/4��ʱ����У��:�趨����λ��[L:%d;R:%d;T:%d;B:%d.] //ģ���[L:%d;R:%d;T:%d;B:%d.]//����:%0.3f /�Ƕ�:%0.3f.",rect.left,rect.right,rect.top,rect.bottom,
		theApp.m_tSysParam.BmpPadDetectLearnWin[0].left,theApp.m_tSysParam.BmpPadDetectLearnWin[0].right,theApp.m_tSysParam.BmpPadDetectLearnWin[0].top,theApp.m_tSysParam.BmpPadDetectLearnWin[0].bottom,theApp.m_tSysParam.BmpPadDetectAcceptance[0],theApp.m_tSysParam.BmpPadDetectAngle[0]);
	AddMsg(str);

	CString strFile = g_pDoc->GetPadModelFile(0);
	milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));
	milApp.SetPadSearchParam(theApp.m_tSysParam.BmpPadDetectAcceptance[0],theApp.m_tSysParam.BmpPadDetectAngle[0]);
	milApp.SetPadModelWindow(theApp.m_tSysParam.BmpPadDetectLearnWin[0]);
	milApp.SetPadSearchWindow(rect);
	
	if(!milApp.FindPadModels())
	{
		str.Format("����ʱRT-1/4�ֲ�����ʧ��");
		AddMsg(str);
		PutLogIntoList(str);
		return false;
	}

	if(milApp.m_stPadResult.nResultNum==0)
	{
		str.Format("����ʱRT-1/4�ֲ�������ĿΪ0,ʧ��");
		AddMsg(str);
		PutLogIntoList(str);
		return false;
	}
	else if(milApp.m_stPadResult.nResultNum>0)
	{
		tgStartPixel.x = milApp.m_stPadResult.dResultCenterX[0];
		tgStartPixel.y = milApp.m_stPadResult.dResultCenterY[0];
		for(int i=0;i<milApp.m_stPadResult.nResultNum;i++)
		{
			if(tgStartPixel.x<milApp.m_stPadResult.dResultCenterX[i])
			{
				tgStartPixel.x = milApp.m_stPadResult.dResultCenterX[i];
			}
			if(tgStartPixel.y>milApp.m_stPadResult.dResultCenterY[i])
			{
				tgStartPixel.y = milApp.m_stPadResult.dResultCenterY[i];
			}
		}
		if(tgStartPixel.y>rect.top + dThirdPixelY)
		{
			tgStartPixel.y = rect.top + dThirdPixelY/2;
		}
		if(tgStartPixel.y<rect.top)//20180907
		{
			tgStartPixel.y = rect.top + dThirdPixelY/2;
		}
		if(tgStartPixel.x<rect.right - dThirdPixelX)
		{
			tgStartPixel.x = rect.right - dThirdPixelX/2;
		}
		if(tgStartPixel.x>rect.right) //20180907
		{
			tgStartPixel.x = rect.right - dThirdPixelX/2;
		}
		///////////////////////////////////////////////////  1.��¼�����׵㣺�Ƿ��׵�y���ߣ��������һ��ֹͣ����?
		str.Format("������ҡ������׵��������꣺X:%0.3f,Y:%0.3f",tgStartPixel.x,tgStartPixel.y);
		AddMsg(str);
		PutLogIntoList(str);
		////////////////////////////////////////////////////
		//rect.bottom = RectOriginal.bottom;
		rect.bottom = RectOriginal.bottom*1.0;              //2.��չ�ײ��������� 2017-11-25 xxxx

		int nThirdNum = g_pFrm->m_CmdRun.ProductParam.ThirdDispRow*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn;
		int nSecondNum = g_pFrm->m_CmdRun.ProductParam.SecondDispRow*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn;
		double dRowDistance = dThirdPixelX;
		int nColumnNum=0;                                  //3.��ؼ�¼ÿ��ÿ��ƥ������
		for(rect.right = long(tgStartPixel.x + dThirdPixelX/2),rect.left = long(tgStartPixel.x - dThirdPixelX/2);
			(rect.left+rect.right)/2>RectOriginal.left;rect.left-=long(dRowDistance),rect.right-=long(dRowDistance))
		{
			nColumnNum++;
			if(rect.left<0)
			{
				rect.left = 0;
			}
			if(rect.right>=IMAGEWIDTH1)
			{
				rect.right = IMAGEWIDTH1-1;
			}
			if(nFirstColumn>=ProductParam.FirstDispColumn||nSecondColumn>=ProductParam.SecondDispColumn||
				nThirdColumn>=ProductParam.ThirdDispColumn)
			{
				break;
			}
			milApp.SetPadSearchWindow(rect);
			str.Format("��%d ������������:%d��:%d��:%d��:%d",nColumnNum,rect.left,rect.right,rect.top,rect.bottom);
			AddMsg(str);
			PutLogIntoList(str);

			if(!milApp.FindPadModels())
			{
				str.Format("���в���ʱ��%d�в���ʧ�ܣ��˳�.",nColumnNum);
				AddMsg(str);
				PutLogIntoList(str);
				return false;
			}
			if(milApp.m_stPadResult.nResultNum==0)
			{
				str.Format("���в���ʱ��%d�в�����ĿΪ0���˳�.",nColumnNum);
				AddMsg(str);
				PutLogIntoList(str);
				return false;
			}
			////////////////////////////////////////////////////3.��¼��ƥ����Ŀ
			str.Format("���в���ƥ����Ŀ---��%d��%d",nColumnNum,milApp.m_stPadResult.nResultNum);
			AddMsg(str);
			PutLogIntoList(str);
			///////////////////////////////////////////////////
			for(int i=0;i<milApp.m_stPadResult.nResultNum;i++)
			{                                                 //5.��¼ÿ����������
				str.Format("���������������д�ӡ��x:%0.3f;y:%0.3f",milApp.m_stPadResult.dResultCenterX[i],milApp.m_stPadResult.dResultCenterY[i]);
                AddMsg(str);
				PutLogIntoList(str);

				int nIndex = int((milApp.m_stPadResult.dResultCenterY[i]-tgStartPixel.y+dThirdPixelY*0.25)/dFirstPixelY);//0.5->0.25
				nFirstRow = nIndex;
				nSecondRow = int((milApp.m_stPadResult.dResultCenterY[i]-tgStartPixel.y-nFirstRow*dFirstPixelY+dThirdPixelY*0.3)/dSecondPixelY);   //5.������ͼ��ƫ�����ݲ��㣺dThirdPixelY/4->0.35xdThirdPixelY
				nThirdRow = int((milApp.m_stPadResult.dResultCenterY[i]-tgStartPixel.y-nFirstRow*dFirstPixelY-nSecondRow*dSecondPixelY+(dThirdPixelY*0.25))/dThirdPixelY);
				str.Format("����ֵ��1��:%d 2��:%d 3��:%d",nFirstRow,nSecondRow,nThirdRow);
				AddMsg(str);
				PutLogIntoList(str);
				
				if(nFirstRow>=g_pFrm->m_CmdRun.ProductParam.FirstDispRow||nSecondRow>=g_pFrm->m_CmdRun.ProductParam.SecondDispRow||
					nThirdRow>=g_pFrm->m_CmdRun.ProductParam.ThirdDispRow)
				{
					continue;
				}
				else
				{
					nIndex = nFirstRow*ProductParam.FirstDispColumn*nSecondNum*nThirdNum+nFirstColumn*nSecondNum*nThirdNum+
						nSecondRow*ProductParam.SecondDispColumn*nThirdNum+nSecondColumn*nThirdNum+nThirdRow*ProductParam.ThirdDispColumn+nThirdColumn;
					pResult[nIndex] = true;
					str.Format("PadReult[%d] = true",nIndex);
					PutLogIntoList(str);
					nFind++;
				}
			}
			/////////////////////////////////////////////////////4.��ӡ�м������
			str.Format("///////////////////////�������ݼ���ֿ�/////////////////////////////");
			AddMsg(str);
			PutLogIntoList(str);

			nThirdColumn++;
			nColumn++;
			dRowDistance = dThirdPixelX;
			if(nThirdColumn>=g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn)
			{
				nThirdColumn = 0;
				nSecondColumn++;
				dRowDistance = dSecondPixelX - (g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn-1)*dThirdPixelX;
				if(nSecondColumn>=g_pFrm->m_CmdRun.ProductParam.SecondDispColumn)
				{
					nSecondColumn = 0;
					nFirstColumn++;
					dRowDistance = dFirstPixelX - (g_pFrm->m_CmdRun.ProductParam.SecondDispColumn-1)*dSecondPixelX;
					if(nFirstColumn>=g_pFrm->m_CmdRun.ProductParam.FirstDispColumn)
					{
						break;
					}
				}
			}
		}
	}
	nEndColumn = nColumn;
	return true;
}

bool CCmdRun::ReadContactSensorValue(double *pValue)
{
	CString str;
	*pValue = 0.0;
	if (ContactSensorHiPrecision == theApp.m_tSysParam.sensorType)
	{
		char szHeight[5];
		szHeight[0] = 0x4d;
		szHeight[1] = 0x30;
		szHeight[2] = 0x0d;
		szHeight[3] = 0x0a;
		szHeight[4] = '\0';
		*pValue = theApp.m_Serial.ReadData(szHeight,1,KEYENCE_IL);
	}
	return true;
}
//Read the laser data of height;
bool CCmdRun::ReadHeightSensorValue(double *pValue)
{
	if(HEIGHT_SENSOR_PANASONIC == theApp.m_tSysParam.heightSensorType)
	{
		CString str;
		char szHeight[11];
		szHeight[0] = 0x25;
		szHeight[1] = 0x30;
		szHeight[2] = 0x31;
		szHeight[3] = 0x23;
		szHeight[4] = 0x52;
		szHeight[5] = 0x4D;
		szHeight[6] = 0x44;
		szHeight[7] = 0x2A;
		szHeight[8] = 0x2A;
		szHeight[9] = 0x0D;
		szHeight[10] = '\0';
		*pValue = theApp.m_Serial.ReadData(szHeight,2,PANASONIC_HL);//��ȡ3�Ŷ˿�:�������Ը�485 ��
	}
	else if(HEIGHT_SENSOR_MLD == theApp.m_tSysParam.heightSensorType)
	{
		CString str;
		char szHeight[9];
		szHeight[0] = 0x02;
		szHeight[1] = 0x4D;
		szHeight[2] = 0x45;
		szHeight[3] = 0x41;
		szHeight[4] = 0x53;
		szHeight[5] = 0x55;
		szHeight[6] = 0x52;
		szHeight[7] = 0x45;
		szHeight[8] = 0x03;
		*pValue = theApp.m_Serial.ReadData(szHeight,2,MLD_HS);
	}
	else if(HEIGHT_SENSOR_KEYENCE == theApp.m_tSysParam.heightSensorType)
	{
		//double dValue = m_pMv->GetAdValue(1);
		*pValue = m_pMv->GetAdVolVal(1)*(-1);
	}
	return true;
}

void CCmdRun::ClearPadDetect()
{
	//g_csPadDetect.Lock();
	if(m_pTransitionDetect!=NULL)
	{
		memset(m_pTransitionDetect,1,m_nPadNumber*sizeof(bool));
	}
	if(m_pPadDetect!=NULL)
	{
		memset(m_pPadDetect,1,m_nPadNumber*sizeof(bool));
	}
	//g_csPadDetect.Unlock();
}

//***********************************************************************
//***m_pTransitionDetect:������������棻m_pPadDetect���ڲ����ת��
void CCmdRun::CopyPadDtect()
{
	g_csPadDetect.Lock();  
	if(ProductParam.inspectMode==NO_INSPECT)
	{
		ClearPadDetect();
	}
	else
	{
		if(m_pTransitionDetect!=NULL&&m_pPadDetect!=NULL)
		{
			memcpy(m_pPadDetect,m_pTransitionDetect,m_nPadNumber*sizeof(bool));
		}
	}
	g_csPadDetect.Unlock();
}

void CCmdRun::InitPadDetect()
{
	try{
		if(ProductParam.IrregularMatrix)
		{
			m_nPadNumber = ProductParam.IrregularRow*ProductParam.IrregularColumn;
		}
		else
		{
			m_nPadNumber = ProductParam.FirstDispRow*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
		}
		g_csPadDetect.Lock();  //2017-11-02
		if(m_pTransitionDetect!=NULL)
		{
			delete[] m_pTransitionDetect;
			m_pTransitionDetect = NULL;
		}
		if(m_pPadDetect!=NULL)
		{
			delete[] m_pPadDetect;
			m_pPadDetect = NULL;
		}
		m_pTransitionDetect = new bool[m_nPadNumber];
		m_pPadDetect = new bool[m_nPadNumber];
		if (NULL==m_pTransitionDetect)
		{
			g_csPadDetect.Unlock();  
			//PutLogIntoList("InitPadDetect()����m_pTransitionDetectʧ��.") ;   //2017-11-02
			AfxMessageBox("InitPadDetect()����m_pTransitionDetectʧ��.");  //2017-11-02
			return;
		}
		if (NULL==m_pPadDetect)
		{
			g_csPadDetect.Unlock();  
			//PutLogIntoList("InitPadDetect()����m_pPadDetectʧ��.");           //2017-11-02
			AfxMessageBox("InitPadDetect()����m_pPadDetectʧ��.");         //2017-11-02
			return;
		}
		memset(m_pTransitionDetect,1,m_nPadNumber*sizeof(bool));//Init status all pos have pads;
		memset(m_pPadDetect,1,m_nPadNumber*sizeof(bool));//Init status all pos have pads;
		g_csPadDetect.Unlock();  //2017-11-02

	}
	catch(...)
	{
		CString str;
		str.Format("InitPadDetect()�׳��쳣...");
		if(NULL!=g_pFrm)
		{
			PutLogIntoList(str);
		}
		AfxMessageBox("InitPadDetect()�׳��쳣...");
	}
}

bool CCmdRun::IsRotatingLightOpen()
{
	bool bReadSuccess = false;
	long lReadData = 0;
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bReadSuccess = theApp.m_Serial.ReadPlcData(&lReadData,"R","202");
		if(!bReadSuccess)
		{
			return false;
		}
		if(lReadData == 0)
		{
			return false;
		}
	}
	return true;
}

bool CCmdRun::IsRotatingLightClose()
{
	bool bReadSuccess = false;
	long lReadData = 0;
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bReadSuccess = theApp.m_Serial.ReadPlcData(&lReadData,"R","203");
		if(!bReadSuccess)
		{
			return false;
		}
		if(lReadData == 0)
		{
			return false;
		}
	}
	return true;
}

bool CCmdRun::MoveToPadDetectPos()
{
	tgPos tgPadDetect = g_pFrm->m_CmdRun.ProductParam.tPadDetectPos;
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgPadDetect.x,tgPadDetect.y,true))
	{
		return false;
	}
	if(!g_pFrm->m_CmdRun.SynchronizeMoveZ(tgPadDetect.za,true))
	{
		return false;
	}
	return true;
}
bool CCmdRun::IsRunAlarm()
{
	return m_tStatus == K_RUN_STS_ALARM;
}
bool CCmdRun::IsRunEmc()
{
	return m_tStatus == K_RUN_STS_EMERGENCY;
}
bool CCmdRun::IsRunStop()
{
	return m_tStatus == K_RUN_STS_STOP;
}
bool CCmdRun::IsExisAlarm()
{
	CString str;
	if(m_tStatus == K_RUN_STS_EMERGENCY)
	{
		str.Format("����ֹͣ���£�");
		AddMsg(str);
		PutLogIntoList(str);
		return true;
	}
	if(RunErrorID >= 0)
	{
		str.Format("�������룺RunErrorID = %d",RunErrorID);
		AddMsg(str);
		PutLogIntoList(str);
		return true;
	}
	return false;
}

void CCmdRun::Alarm(int nID)
{
	m_bFirstDisp = false;
	m_bTestRunStop = true;
	RunErrorID = nID;
	m_tStatus = K_RUN_STS_ALARM;
	m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);
}

void CCmdRun::AlarmOnly(int nID)
{
	m_bFirstDisp = false;
	m_bTestRunStop = true;
	RunErrorID = nID;
	/*m_tStatus = K_RUN_STS_ALARM;*/
	/*m_pMv->SetOutput(theApp.m_tSysParam.outReady,FALSE);*/
}
//����Z��
bool CCmdRun::RunZUp(double za,double zb,double zc)
{
	if(za-ProductParam.dPauseHeight<0||
		zb-ProductParam.dPauseHeight<0||
		zc-ProductParam.dPauseHeight<0)
	{
		Alarm(16);
		AddMsg("��������Z��㽺λС����ͣ�߶�");
		return false;
	}
	if(!SetAxisProfile(LIFT_VEL))
	{
		Alarm(30);
		AddMsg("�����߶ȣ��趨�����ٶ�ʧ�ܣ�");
		return false;
	}
	if(!SynchronizeMoveZ(za-ProductParam.dPauseHeight,zb-ProductParam.dPauseHeight,
		zc-ProductParam.dPauseHeight,true,true))
	{
		return false;
	}
	if(!SetAxisProfile(HIGH_VEL))
	{
		Alarm(30);
		AddMsg("�����߶ȣ��趨�����ٶ�ʧ�ܣ�");
		return false;
	}
	CTimeUtil::Delay(long(ProductParam.dPauseTime*1000));
	if(za-ProductParam.dZupHigh<0||
		zb-ProductParam.dZupHigh<0||
		zc-ProductParam.dZupHigh<0)
	{
		Alarm(16);
		AddMsg("��������Z��㽺λС��Z�������߶�");
		return false;
	}
	if(!SynchronizeMoveZ(za-ProductParam.dZupHigh,zb-ProductParam.dZupHigh,
		zc-ProductParam.dZupHigh,true,true))
	{
		return false;
	}

	return true;
}
bool CCmdRun::RunValveOperation()
{
	m_bValveReset = true;
	int nReturnValue = theApp.m_V9Control.V9C_Rest(K_ALL_AXIS);
	m_bValveReset = false;
	if(nReturnValue>0)
	{
		Alarm(nReturnValue+5);
		AddMsg("���帴λʧ�ܣ�");
		return false;
	}
	if(nReturnValue<0)
	{
		Alarm(-nReturnValue+1);
		AddMsg("���帴λʧ�ܣ�");
		return false;
	}
	if(!theApp.m_V9Control.V9C_SetProfile())
	{
		Alarm(30);
		AddMsg("�������ò�������");
		return false;
	}
	m_bFirstDisp = true;
	V9DotRest = false;
	if(!CleanAndTest(true))
	{
		AddMsg("��ϴ���Խ�ʧ�ܣ�");
		return false;
	}
	if(ProductParam.FullTestDot||ProductParam.FullClean)
	{
		if(!SetAxisProfile(HIGH_VEL))
		{
			Alarm(30);
			AddMsg("��ϴ���Խ����趨�ٶ�ʧ�ܣ�");
			return false;
		}
	}
	return true;
}
void CCmdRun::CleanValves()
{
	//while(true)
	for(int i = 0; i< g_pFrm->m_CmdRun.V9Parm[0].CleanCount; i++)
	{
		bool bTemp=g_pFrm->m_CmdRun.bIsExitClean;
		if (!bTemp)
		{
			int nTemp=g_pFrm->m_CmdRun.m_nCleanValveNum;
			double dTmpCleanVel=g_pFrm->m_CmdRun.V9Parm[0].CleanSpeed;
			theApp.m_V9Control.V9C_Clearn(nTemp,1,dTmpCleanVel);
			
			CFunction::DoEvents();
		}
		else
		{
			break;
		}
		CFunction::DoEvents();	
	}
}
void CCmdRun::CleanThreadPlay()
{
	if (NULL!=g_pCleanValveThread)
	{
		TerminateThread(g_pCleanValveThread->m_hThread, 0);
	}
	CFunction::DoEvents();
	g_pCleanValveThread=AfxBeginThread(ThreadClean,this);
	if(NULL==g_pCleanValveThread)
	{
		AfxMessageBox("��ϴ�̴߳���ʧ��.");
	}

}

void CCmdRun::CleanSuspend()
{
	if (NULL!=g_pCleanValveThread)
	{
		TerminateThread(g_pCleanValveThread->m_hThread, 0);
	}
}

bool CCmdRun::AdjustMotorHome()
{
	if(!m_pMv->IsInitOK())
	{
		return false;
	}
	BOOL bReturn = FALSE;

	bReturn = m_pMv->HomeAxis(K_AXIS_B,-100,1,0.2,theApp.m_tSysParam.tAxis[K_AXIS_B-1].highHomeVel,
		theApp.m_tSysParam.tAxis[K_AXIS_B-1].lowHomeVel,theApp.m_tSysParam.tAxis[K_AXIS_B-1].homeAcc);
	if(FALSE == bReturn)
	{
		AfxMessageBox("��ͷ����B�ᣬ��λʧ�ܣ�");
		return false;
	}

	bReturn = m_pMv->HomeAxis(K_AXIS_D,-100,1,0.2,theApp.m_tSysParam.tAxis[K_AXIS_D-1].highHomeVel,
		theApp.m_tSysParam.tAxis[K_AXIS_D-1].lowHomeVel,theApp.m_tSysParam.tAxis[K_AXIS_D-1].homeAcc);
	if(FALSE == bReturn)
	{
		AfxMessageBox("��ͷ����D�ᣬ��λʧ�ܣ�");
		return false;
	}

	bReturn = m_pMv->HomeAxis(K_AXIS_A,-100,1,0.2,theApp.m_tSysParam.tAxis[K_AXIS_A-1].highHomeVel,
		theApp.m_tSysParam.tAxis[K_AXIS_A-1].lowHomeVel,theApp.m_tSysParam.tAxis[K_AXIS_A-1].homeAcc);
	if(FALSE == bReturn)
	{
		AfxMessageBox("��ͷ����A�ᣬ��λʧ�ܣ�");
		return false;
	}

	bReturn = m_pMv->HomeAxis(K_AXIS_C,-100,1,0.2,theApp.m_tSysParam.tAxis[K_AXIS_C-1].highHomeVel,
		theApp.m_tSysParam.tAxis[K_AXIS_C-1].lowHomeVel,theApp.m_tSysParam.tAxis[K_AXIS_C-1].homeAcc);
	if(FALSE == bReturn)
	{
		AfxMessageBox("��ͷ����C�ᣬ��λʧ�ܣ�");
		return false;
	}

	return true;
}
bool CCmdRun::AdjustMotorZero()
{
	if(!m_pMv->IsInitOK())
	{
		return false;
	}
	if (!theApp.m_tSysParam.AdjustMotorHomeSu)//1201
	{
		CString str;
		str.Format("���������ԭ���־��������ﵽ��λʧ��!");
		PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return false;
	}

	if(!g_pFrm->m_CmdRun.SetAdjustMotorProfile(MEDIUM_VEL))
	{
		return false;
	}
	MOTION_STATUS status;
	status = m_pMv->Move(K_AXIS_A,0,false,true);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	status = m_pMv->Move(K_AXIS_B,0,false,true);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	status = m_pMv->Move(K_AXIS_C,0,false,true);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	status = m_pMv->Move(K_AXIS_D,0,true,true);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	status = m_pMv->WaitStop(K_AXIS_A,60000);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	status = m_pMv->WaitStop(K_AXIS_B,60000);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	status = m_pMv->WaitStop(K_AXIS_C,60000);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	status = m_pMv->WaitStop(K_AXIS_D,60000);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	return true;
}
//0923 �Ľ����㷽����
bool CCmdRun::AdjustNeedle()
{
	CString str;
	if(!m_pMv->IsInitOK())
	{
		str.Format("����ͷ��ࡿ��������Ʒ���ʱ�˶�����ʼ����־�쳣��");
		PutLogIntoList(str);
		return false;
	}
	if (!theApp.m_tSysParam.AdjustMotorHomeSu)  //1201
	{
		CString str;
		str.Format("���������ԭ���־��������ͷ���ʧ��!");
		PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return false;
	}
	if(!g_pFrm->m_CmdRun.SetAdjustMotorProfile(MEDIUM_VEL))
	{
		str.Format("����ͷ��ࡿ��������Ʒ���֮ǰ�����л�ʧ�ܣ�");
		PutLogIntoList(str);
		return false;
	}
	MOTION_STATUS status;
	double dPosA = 0,dPosB = 0,dPosC = 0,dPosD = 0;
	double dCurA,dCurC,dCurB,dCurD;
	bool bMotorA,bMotorB,bMotorC,bMotorD;
	bMotorA=bMotorB=bMotorC=bMotorD=false;
	/////////////////////////////////////////////////
	if(THREE_VALVE==ProductParam.valveSelect)
	{
		dPosA = ProductParam.NeedleGap - fabs(ProductParam.tgLabelPos[0].x - ProductParam.tgLabelPos[1].x);
		dPosA+=ProductParam.offset2x;
		dPosA+=ProductParam.dComposeSecondX;
		dCurA=m_pMv->GetPos(K_AXIS_A);
		if(fabs(dCurA-dPosA)>0.001)
		{
			bMotorA=true;
		}
		else
		{
			bMotorA=false;
		}
		if(dPosA<0)
		{
			str.Format("AĿ��λ��Ϊ������������ʧ�ܣ�");
			PutLogIntoList(str);
			AddMsg(str);
			str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
			PutLogIntoList(str);
			AddMsg(str);
			return false;
		}

		dPosB=ProductParam.offset2y;
		dPosB+=ProductParam.dComposedSecondY;
		dCurB=m_pMv->GetPos(K_AXIS_B);
		if (fabs(dCurB-dPosB)>0.001)
		{
			bMotorB=true;
		}
		else
		{
			bMotorB=false;
		}
		if(dPosB<0)
		{
			str.Format("BĿ��λ��Ϊ������������ʧ�ܣ�");
			PutLogIntoList(str);
			AddMsg(str);
			str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
			PutLogIntoList(str);
			return false;
		}

		dPosC = 2*ProductParam.NeedleGap - fabs(ProductParam.tgLabelPos[0].x - ProductParam.tgLabelPos[2].x);//left and right
		dPosC+=ProductParam.offset3x;
		dPosC+=ProductParam.dComposedThirdX;
		dCurC=m_pMv->GetPos(K_AXIS_C);
		if(fabs(dCurC-dPosC)>0.001)
		{
			bMotorC=true;
		}
		else
		{
			bMotorC=false;
		}
		if(dPosC<0)
		{
			str.Format("CĿ��λ��Ϊ������������ʧ�ܣ�");
			PutLogIntoList(str);
			AddMsg(str);
			str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
			PutLogIntoList(str);
			return false;
		}
		if(dPosC>theApp.m_tSysParam.tAxis[K_AXIS_C-1].maxpos)
		{
			str.Format("CĿ��λ�ó�����г̣�������ʧ�ܣ�");
			PutLogIntoList(str);
			AddMsg(str);
			return false;
		}

		dPosD=ProductParam.offset3y;
		dPosD+=ProductParam.dComposedThirdY; 
		dCurD=m_pMv->GetPos(K_AXIS_D);
		if (fabs(dCurD-dPosD)>0.001)
		{
			bMotorD=true;
		}
		else
		{
			bMotorD=false;
		}
		if(dPosD<0)
		{
			str.Format("DĿ��λ��Ϊ������������ʧ�ܣ�");
			PutLogIntoList(str);
			AddMsg(str);
			str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
			PutLogIntoList(str);
			return false;
		}

		double dTempPos = m_pMv->GetPos(K_AXIS_A);
		//1.�������������A->C
		if(dTempPos>dPosA) 
		{
			if(bMotorA)
			{
				status = m_pMv->Move(K_AXIS_A,dPosA,false,true);
				if(MOT_STATUS_OK!=status)
				{
					str.Format("���A�˶�ʧ�ܣ�");
					PutLogIntoList(str);
					AddMsg(str);
					str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
					PutLogIntoList(str);
					return false;
				}
				Sleep(100);
			}
			if(bMotorC)
			{
				status = m_pMv->Move(K_AXIS_C,dPosC,false,true);
				if(MOT_STATUS_OK!=status)
				{
					str.Format("���C�˶�ʧ�ܣ�");
					PutLogIntoList(str);
					AddMsg(str);
					str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
					PutLogIntoList(str);
					return false;
				}
			}
		} //������� C->A
		else
		{
			if(bMotorC)
			{
				status = m_pMv->Move(K_AXIS_C,dPosC,false,true);
				if(MOT_STATUS_OK!=status)
				{
					str.Format("���C�˶�ʧ�ܣ�");
					PutLogIntoList(str);
					AddMsg(str);
					str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
					PutLogIntoList(str);
					return false;
				}
				Sleep(100);
			}
			if(bMotorA)
			{
				status = m_pMv->Move(K_AXIS_A,dPosA,false,true);
				if(MOT_STATUS_OK!=status)
				{
					str.Format("���A�˶�ʧ�ܣ�");
					PutLogIntoList(str);
					AddMsg(str);
					str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
					PutLogIntoList(str);
					return false;
				}
			}
		}
		if(bMotorB)
		{
			status = m_pMv->Move(K_AXIS_B,dPosB,false,true);
			if(MOT_STATUS_OK!=status)
			{
				str.Format("���B�˶�ʧ�ܣ�");
				PutLogIntoList(str);
				AddMsg(str);
				str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
				PutLogIntoList(str);
				return false;
			}
		}
		if(bMotorD)
		{
			status = m_pMv->Move(K_AXIS_D,dPosD,false,true);
			if(MOT_STATUS_OK!=status)
			{
				str.Format("���D�˶�ʧ�ܣ�");
				PutLogIntoList(str);
				AddMsg(str);
				str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
				PutLogIntoList(str);
				return false;
			}

		}
		if(bMotorA)
		{
			status = m_pMv->WaitStop(K_AXIS_A,120000);
			if(MOT_STATUS_OK!=status)
			{
				str.Format("�ȴ����Aֹͣʧ�ܣ�");
				PutLogIntoList(str);
				AddMsg(str);
				str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
				PutLogIntoList(str);
				return false;
			}
		}
		if(bMotorB)
		{
			status = m_pMv->WaitStop(K_AXIS_B,120000);
			if(MOT_STATUS_OK!=status)
			{
				str.Format("�ȴ����Bֹͣʧ�ܣ�");
				PutLogIntoList(str);
				AddMsg(str);
				str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
				PutLogIntoList(str);
				return false;
			}
		}				
		if(bMotorC)
		{
			status = m_pMv->WaitStop(K_AXIS_C,120000);
			if(MOT_STATUS_OK!=status)
			{
				str.Format("�ȴ����Cֹͣʧ�ܣ�");
				PutLogIntoList(str);
				AddMsg(str);
				str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
				PutLogIntoList(str);
				return false;
			}

		}
		if(bMotorD)
		{
			status = m_pMv->WaitStop(K_AXIS_D,120000);
			if(MOT_STATUS_OK!=status)
			{
				str.Format("�ȴ����Dֹͣʧ�ܣ�");
				PutLogIntoList(str);
				AddMsg(str);
				str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
				PutLogIntoList(str);
				return false;
			}
		}		

	}
	else if(TWO_VALVE==ProductParam.valveSelect)
	{
		double dRecordRelPosF2;
		dRecordRelPosF2=0;
		dPosA = ProductParam.NeedleGap - fabs(ProductParam.tgLabelPos[0].x - ProductParam.tgLabelPos[1].x);
		dRecordRelPosF2=dPosA;//��¼���λ��...
		dRecordRelPosF2=dRecordRelPosF2+ProductParam.dComposeSecondX;
		dPosA+=ProductParam.offset2x;
		dPosA+=ProductParam.dComposeSecondX;
		dCurA=m_pMv->GetPos(K_AXIS_A);
		if(fabs(dCurA-dPosA)>0.001)
		{
			bMotorA=true;
		}
		else
		{
			bMotorA=false;
		}
		if(dPosA<0)
		{
			str.Format("AĿ��λ��Ϊ������������ʧ�ܣ�");
			PutLogIntoList(str);
			AddMsg(str);
			str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
			PutLogIntoList(str);
			return false;
		}

		dPosB=ProductParam.offset2y;
		dPosB+=ProductParam.dComposedSecondY;
		dCurB=m_pMv->GetPos(K_AXIS_B);
		if (fabs(dCurB-dPosB)>0.001)
		{
			bMotorB=true;
		}
		else
		{
			bMotorB=false;
		}
		if(dPosB<0)
		{
			str.Format("BĿ��λ��Ϊ������������ʧ�ܣ�");
			PutLogIntoList(str);
			AddMsg(str);
			str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
			PutLogIntoList(str);
			return false;
		}
		dPosC = dRecordRelPosF2;
		dPosC+=ProductParam.offset3x;
		dPosC+=ProductParam.dComposedThirdX;
		dPosC+=2; //����ͼ����ʱ��ƫ�ռ�...
		dCurC=m_pMv->GetPos(K_AXIS_C);
		if(fabs(dCurC-dPosC)>0.001)
		{
			bMotorC=true;
		}
		else
		{
			bMotorC=false;
		}
		if(dPosC<0)
		{
			str.Format("CĿ��λ��Ϊ������������ʧ�ܣ�");
			AddMsg(str);
			PutLogIntoList(str);
			str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
			PutLogIntoList(str);
			return false;
		}
		if(dPosC>theApp.m_tSysParam.tAxis[K_AXIS_C-1].maxpos)
		{
			str.Format("CĿ��λ�ó�����г̣�������ʧ�ܣ�");
			PutLogIntoList(str);
			AddMsg(str);
			return false;
		}

		dPosD=ProductParam.offset3y;
		dPosD+=ProductParam.dComposedThirdY; 
		dCurD=m_pMv->GetPos(K_AXIS_D);
		if(fabs(dCurD-dPosD)>0.001)
		{
			bMotorD=true;
		}
		else
		{
			bMotorD=false;
		}
		if(dPosD<0)
		{
			str.Format("DĿ��λ��Ϊ������������ʧ�ܣ�");
			AddMsg(str);
			PutLogIntoList(str);
			str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
			PutLogIntoList(str);
			return false;
		}
		double dTempPos = m_pMv->GetPos(K_AXIS_A);
		//1.�������������A->C
		if(dTempPos>dPosA) 
		{
			if(bMotorA)
			{
				status = m_pMv->Move(K_AXIS_A,dPosA,false,true);
				if(MOT_STATUS_OK!=status)
				{
					str.Format("���A�˶�ʧ�ܣ�");
					PutLogIntoList(str);
					AddMsg(str);
					str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
					PutLogIntoList(str);
					return false;
				}
				Sleep(100);
			}
			if(bMotorC)
			{
				status = m_pMv->Move(K_AXIS_C,dPosC,false,true);
				if(MOT_STATUS_OK!=status)
				{
					str.Format("���C�˶�ʧ�ܣ�");
					AddMsg(str);
					PutLogIntoList(str);
					str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
					PutLogIntoList(str);
					return false;
				}
			}
		} //������� C->A
		else
		{
			if(bMotorC)
			{
				status = m_pMv->Move(K_AXIS_C,dPosC,false,true);
				if(MOT_STATUS_OK!=status)
				{
					str.Format("���C�˶�ʧ�ܣ�");
					PutLogIntoList(str);
					AddMsg(str);
					str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
					PutLogIntoList(str);
					return false;
				}
				Sleep(100);
			}
			if(bMotorA)
			{
				status = m_pMv->Move(K_AXIS_A,dPosA,false,true);
				if(MOT_STATUS_OK!=status)
				{
					str.Format("���A�˶�ʧ�ܣ�");
					PutLogIntoList(str);
					AddMsg(str);
					str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
					PutLogIntoList(str);
					return false;
				}
			}
		}
		if(bMotorB)
		{
			status = m_pMv->Move(K_AXIS_B,dPosB,false,true);
			if(MOT_STATUS_OK!=status)
			{
				str.Format("���B�˶�ʧ�ܣ�");
				PutLogIntoList(str);
				AddMsg(str);
				str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
				PutLogIntoList(str);
				return false;
			}
		}
		if(bMotorD)
		{
			status = m_pMv->Move(K_AXIS_D,dPosD,false,true);
			if(MOT_STATUS_OK!=status)
			{
				str.Format("���D�˶�ʧ�ܣ�");
				PutLogIntoList(str);
				AddMsg(str);
				str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
				PutLogIntoList(str);
				return false;
			}

		}
		if(bMotorA)
		{
			status = m_pMv->WaitStop(K_AXIS_A,120000);
			if(MOT_STATUS_OK!=status)
			{
				str.Format("�ȴ����Aֹͣʧ�ܣ�");
				AddMsg(str);
				PutLogIntoList(str);
				str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
				PutLogIntoList(str);
				return false;
			}
		}
		if(bMotorB)
		{
			status = m_pMv->WaitStop(K_AXIS_B,120000);
			if(MOT_STATUS_OK!=status)
			{
				str.Format("�ȴ����Bֹͣʧ�ܣ�");
				PutLogIntoList(str);
				AddMsg(str);
				//PutLogIntoList(str);
				str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
				PutLogIntoList(str);
				return false;
			}
		}				
		if(bMotorC)
		{
			status = m_pMv->WaitStop(K_AXIS_C,120000);
			if(MOT_STATUS_OK!=status)
			{
				str.Format("�ȴ����Cֹͣʧ�ܣ�");
				PutLogIntoList(str);
				AddMsg(str);
				//PutLogIntoList(str);
				str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
				PutLogIntoList(str);
				return false;
			}

		}
		if(bMotorD)
		{
			status = m_pMv->WaitStop(K_AXIS_D,120000);
			if(MOT_STATUS_OK!=status)
			{
				str.Format("�ȴ����Dֹͣʧ�ܣ�");
				PutLogIntoList(str);
				AddMsg(str);
				str.Format("��ֹ��Ӧ:��ͷ���->��Ʒ���");
				PutLogIntoList(str);
				return false;
			}
		}		

	}	
	return true;
}
bool CCmdRun::AdjustNeedleAccurate()
{
	MOTION_STATUS status;
	double dPosA = 0,dPosB = 0,dPosC = 0,dPosD = 0;
	double dTargetPos=0;
	if(!m_pMv->IsInitOK())
	{
		return false;
	}
	if(!g_pFrm->m_CmdRun.SetAdjustMotorProfile(MEDIUM_VEL))
	{
		return false;
	}

	dPosA = ProductParam.NeedleGap - fabs(ProductParam.tgLabelPos[0].x - (ProductParam.tgLabelPos[1].x+g_pFrm->m_CmdRun.ProductParam.offset2x));//left and right
	if(dPosA<0)
	{
		return false;
	}
	dPosB = ((ProductParam.tgLabelPos[1].y+g_pFrm->m_CmdRun.ProductParam.offset2y) - ProductParam.tgLabelPos[0].y)*tan(ProductParam.AdjustNeedleAngle[0]/180.0*KGB_PI);//before and behind
	if(dPosB<0)
	{
		return false;
	}
	dPosC = 2*ProductParam.NeedleGap - fabs(ProductParam.tgLabelPos[0].x - (ProductParam.tgLabelPos[2].x+g_pFrm->m_CmdRun.ProductParam.offset3x));//left and right
	if(dPosC<0)
	{
		return false;
	}
	dPosD = ((ProductParam.tgLabelPos[2].y+g_pFrm->m_CmdRun.ProductParam.offset3y) - ProductParam.tgLabelPos[0].y)*tan(ProductParam.AdjustNeedleAngle[1]/180.0*KGB_PI);//before and behind
	if(dPosD<0)
	{
		return false;
	}
	double dTempPos = m_pMv->GetPos(K_AXIS_A);
    //judge the sequence;
	if(dTempPos>dPosA)
	{
		dTargetPos=g_pFrm->m_CmdRun.ProductParam.dAdjustMotorInRunA;
		status=m_pMv->Move(K_AXIS_A,dTargetPos,true,true);
		if(MOT_STATUS_OK!=status)
		{
			return false;
		}

		dTargetPos=g_pFrm->m_CmdRun.ProductParam.dAdjustMotorInRunC;
		status=m_pMv->Move(K_AXIS_C,dTargetPos,true,true);
		if(MOT_STATUS_OK!=status)
		{
			return false;
		}
	}
	else
	{
		dTargetPos=g_pFrm->m_CmdRun.ProductParam.dAdjustMotorInRunC;
		status=m_pMv->Move(K_AXIS_C,dTargetPos,true,true);
		if(MOT_STATUS_OK!=status)
		{
			return false;
		}
		dTargetPos=g_pFrm->m_CmdRun.ProductParam.dAdjustMotorInRunA;
		status=m_pMv->Move(K_AXIS_A,dTargetPos,true,true);
		if(MOT_STATUS_OK!=status)
		{
			return false;
		}
	}

	dTargetPos=g_pFrm->m_CmdRun.ProductParam.dAdjustMotorInRunB;
	status=m_pMv->Move(K_AXIS_B,dTargetPos,false,true);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}

	dTargetPos=g_pFrm->m_CmdRun.ProductParam.dAdjustMotorInRunD;
	status=m_pMv->Move(K_AXIS_D,dTargetPos,false,true);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	status = m_pMv->WaitStop(K_AXIS_A,60000);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	status = m_pMv->WaitStop(K_AXIS_B,60000);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	status = m_pMv->WaitStop(K_AXIS_C,60000);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	status = m_pMv->WaitStop(K_AXIS_D,60000);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	return true;
}
bool CCmdRun::AdjustNeedle(double dOffsetA,double dOffsetB,double dOffsetC,double dOffsetD)
{
	if(!m_pMv->IsInitOK())
	{
		return false;
	}
	if(!g_pFrm->m_CmdRun.SetAdjustMotorProfile(MEDIUM_VEL))
	{
		return false;
	}
	MOTION_STATUS status;
	double dPosA = 0,dPosB = 0,dPosC = 0,dPosD = 0;
	dPosA = m_pMv->GetPos(K_AXIS_A) + dOffsetA;
	if(dPosA<0)
	{
		return false;
	}
	dPosB = m_pMv->GetPos(K_AXIS_B) + dOffsetB*tan(ProductParam.AdjustNeedleAngle[0]/180.0*KGB_PI);
	if(dPosB<0)
	{
		return false;
	}
	dPosC = m_pMv->GetPos(K_AXIS_C) + dOffsetC;
	if(dPosC<0)
	{
		return false;
	}
	dPosD = m_pMv->GetPos(K_AXIS_D) + dOffsetD*tan(ProductParam.AdjustNeedleAngle[1]/180.0*KGB_PI);
	if(dPosD<0)
	{
		return false;
	}
	if(dOffsetA<0)
	{
		status = m_pMv->Move(K_AXIS_A,dPosA,true,true);
		if(MOT_STATUS_OK!=status)
		{
			return false;
		}
		status = m_pMv->Move(K_AXIS_C,dPosC,true,true);
		if(MOT_STATUS_OK!=status)
		{
			return false;
		}
	}
	else
	{
		status = m_pMv->Move(K_AXIS_C,dPosC,true,true);
		if(MOT_STATUS_OK!=status)
		{
			return false;
		}
		status = m_pMv->Move(K_AXIS_A,dPosA,true,true);
		if(MOT_STATUS_OK!=status)
		{
			return false;
		}
	}
	status = m_pMv->Move(K_AXIS_B,dPosB,false,true);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	status = m_pMv->Move(K_AXIS_D,dPosD,false,true);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	status = m_pMv->WaitStop(K_AXIS_A,60000);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	status = m_pMv->WaitStop(K_AXIS_B,60000);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	status = m_pMv->WaitStop(K_AXIS_C,60000);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	status = m_pMv->WaitStop(K_AXIS_D,60000);
	if(MOT_STATUS_OK!=status)
	{
		return false;
	}
	return true;
}
bool CCmdRun::AdjustMotorToCleanPos()
{
    CString str;
	if(!m_pMv->IsInitOK())
	{
		str.Format("����ͷ��ࡿ��������ϴ���ʱ��ʼ����־�쳣��");
		PutLogIntoList(str);
		return false;
	}
	if (!theApp.m_tSysParam.AdjustMotorHomeSu)//1201
	{
		CString str;
		str.Format("���������ԭ���־��������ϴ���ʧ��!");
		PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return false;
	}

	MOTION_STATUS status;
	double dPosA = 0,dPosB = 0,dPosC = 0,dPosD = 0;
	//based in data0;
	dPosA = ProductParam.CleanGap -fabs(ProductParam.tgLabelPos[0].x - ProductParam.tgLabelPos[1].x);
	if(dPosA<0)
	{
		CString str;
		//str.Format("��ϴ������ʱ�����塾X�����Ѿ���С���޷��ﵽĿ����ϴ��࣡");
		str.Format("����ͷ��ࡿ��2������ϴ���ֵʱС����С��࣬X������쳣��");
		AddMsg(str);
		PutLogIntoList(str);
		return false;
	}


	//dPosB = (ProductParam.tgLabelPos[0].y - ProductParam.tgLabelPos[1].y)*tan(ProductParam.AdjustNeedleAngle[0]/180.0*KGB_PI);
	dPosB=ProductParam.offset2y;
	if(dPosB<0)
	{
		CString str;
		//str.Format("��ϴ������ʱ�����塾Y����ԭ��λ��ǰ������޷�����Ŀ��λ��");
		str.Format("����ͷ��ࡿ��2������ϴ���ֵʱ��Y������쳣��");
		AddMsg(str);
		PutLogIntoList(str);
		return false;
	}
	dPosC =2*ProductParam.CleanGap - fabs(ProductParam.tgLabelPos[0].x - ProductParam.tgLabelPos[2].x);
	if(dPosC<0)
	{
		CString str;
		//str.Format("��ϴ������ʱ�����塾X�����Ѿ���С���޷��ﵽĿ����ϴ��࣡");
		str.Format("����ͷ��ࡿ��3������ϴ���ֵʱС����С��࣬X������쳣��");
		AddMsg(str);
		PutLogIntoList(str);
		return false;
	}
	//dPosD = (ProductParam.tgLabelPos[2].y - ProductParam.tgLabelPos[1].y)*tan(ProductParam.AdjustNeedleAngle[1]/180.0*KGB_PI);
	dPosD=ProductParam.offset3y;	
	if(dPosD<0)
	{
		CString str;
		//str.Format("��ϴ������ʱ�����塾Y����ԭ��λ��ǰ������޷�����Ŀ��λ��");
		str.Format("����ͷ��ࡿ��3������ϴ���ֵʱ��Y������쳣��");
		AddMsg(str);
		PutLogIntoList(str);
		return false;
	}
	double dTempPos = m_pMv->GetPos(K_AXIS_A);
	//����
	if(dTempPos>dPosA)
	{
		dPosA+=ProductParam.offset2x;
		status = m_pMv->Move(K_AXIS_A,dPosA,true,true);
		if(MOT_STATUS_OK!=status)
		{
			str.Format("������ϴ���ʱA������ʧ�ܣ�");
			PutLogIntoList(str);
			return false;
		}
		dPosC+=ProductParam.offset3x;
		status = m_pMv->Move(K_AXIS_C,dPosC,true,true);
		if(MOT_STATUS_OK!=status)
		{
			str.Format("������ϴ���ʱC������ʧ�ܣ�");
			PutLogIntoList(str);
			return false;
		}
	}
	else
	{
		dPosC+=ProductParam.offset3x;
		status = m_pMv->Move(K_AXIS_C,dPosC,true,true);
		if(MOT_STATUS_OK!=status)
		{
			str.Format("������ϴ���ʱC������ʧ�ܣ�");
			PutLogIntoList(str);
			return false;
		}

		dPosA+=ProductParam.offset2x;
		status = m_pMv->Move(K_AXIS_A,dPosA,true,true);
		if(MOT_STATUS_OK!=status)
		{
			str.Format("������ϴ���ʱA������ʧ�ܣ�");
			PutLogIntoList(str);
			return false;
		}
	}
	status = m_pMv->Move(K_AXIS_B,dPosB,false,true);
	if(MOT_STATUS_OK!=status)
	{
		str.Format("������ϴ���ʱB������ʧ�ܣ�");
		PutLogIntoList(str);
		return false;
	}
	status = m_pMv->Move(K_AXIS_D,dPosD,false,true);
	if(MOT_STATUS_OK!=status)
	{
		str.Format("������ϴ���ʱD������ʧ�ܣ�");
		PutLogIntoList(str);
		return false;
	}
	status = m_pMv->WaitStop(K_AXIS_A,60000);
	if(MOT_STATUS_OK!=status)
	{
		str.Format("������ϴ���ʱA���ȴ�ֹͣʧ�ܣ�");
		PutLogIntoList(str);
		return false;
	}
	status = m_pMv->WaitStop(K_AXIS_B,60000);
	if(MOT_STATUS_OK!=status)
	{
		str.Format("������ϴ���ʱB���ȴ�ֹͣʧ�ܣ�");
		PutLogIntoList(str);
		return false;
	}
	status = m_pMv->WaitStop(K_AXIS_C,60000);
	if(MOT_STATUS_OK!=status)
	{
		str.Format("������ϴ���ʱC���ȴ�ֹͣʧ�ܣ�");
		PutLogIntoList(str);
		return false;
	}
	status = m_pMv->WaitStop(K_AXIS_D,60000);
	if(MOT_STATUS_OK!=status)
	{
		str.Format("������ϴ���ʱD���ȴ�ֹͣʧ�ܣ�");
		PutLogIntoList(str);
		return false;
	}
	return true;
}
bool CCmdRun::SetAdjustMotorProfile(VelType type)
{
	CString str;
	MOTION_STATUS status;
	int nVelType = int(type);
	status = m_pMv->SetProfile(K_AXIS_A,theApp.m_tSysParam.tAxis[K_AXIS_A-1].vel[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_A-1].acc[nVelType],
		theApp.m_tSysParam.tAxis[K_AXIS_A-1].dec[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_A-1].jerk[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_A-1].velStart[nVelType]);
	if(MOT_STATUS_OK!=status)
	{
		str.Format("A�����ò���ʧ�ܣ�");
		AddMsg(str);
		PutLogIntoList(str);
		return false;
	}
	status = m_pMv->SetProfile(K_AXIS_B,theApp.m_tSysParam.tAxis[K_AXIS_B-1].vel[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_B-1].acc[nVelType],
		theApp.m_tSysParam.tAxis[K_AXIS_B-1].acc[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_B-1].jerk[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_B-1].velStart[nVelType]);
	if(MOT_STATUS_OK!=status)
	{
		str.Format("B�����ò���ʧ�ܣ�");
		AddMsg(str);
		PutLogIntoList(str);
		return false;
	}
	status = m_pMv->SetProfile(K_AXIS_C,theApp.m_tSysParam.tAxis[K_AXIS_C-1].vel[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_C-1].acc[nVelType],
		theApp.m_tSysParam.tAxis[K_AXIS_C-1].acc[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_C-1].jerk[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_C-1].velStart[nVelType]);
	if(MOT_STATUS_OK!=status)
	{
		str.Format("C�����ò���ʧ�ܣ�");
		AddMsg(str);
		PutLogIntoList(str);
		return false;
	}
	status = m_pMv->SetProfile(K_AXIS_D,theApp.m_tSysParam.tAxis[K_AXIS_D-1].vel[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_D-1].acc[nVelType],
		theApp.m_tSysParam.tAxis[K_AXIS_D-1].acc[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_D-1].jerk[nVelType],theApp.m_tSysParam.tAxis[K_AXIS_D-1].velStart[nVelType]);
	if(MOT_STATUS_OK!=status)
	{
		str.Format("D�����ò���ʧ�ܣ�");
		AddMsg(str);
		PutLogIntoList(str);
		return false;
	}
	return true;
}

bool CCmdRun::MoveToAdjustPos(int nIndex)
{
	if(nIndex<0||nIndex>=3)
	{
		return false;
	}
	if(!theApp.m_Mv400.IsInitOK())
	{
		return false;
	}

	double dZPos = 0.0;

	g_pFrm->m_CmdRun.MoveToZSafety();
	tgPos tgNeedlePos,tgCameraPos;
	tgNeedlePos.x = g_pFrm->m_CmdRun.ProductParam.NeedlePos[0] - nIndex * g_pFrm->m_CmdRun.ProductParam.NeedleGap +
		g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.dPitchX * g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentX;
	tgNeedlePos.y = g_pFrm->m_CmdRun.ProductParam.NeedlePos[1] +
		g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.dPitchY * g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentY;
	dZPos = g_pFrm->m_CmdRun.ProductParam.NeedlePos[2+nIndex];
	tgCameraPos.x = tgNeedlePos.x + nIndex*g_pFrm->m_CmdRun.ProductParam.NeedleGap +
		(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].x - g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].x);
	tgCameraPos.y = tgNeedlePos.y +
		(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].y - g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].y);
	tgCameraPos.za = ProductParam.tgLabelPos[3].za;

	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgNeedlePos.x,tgNeedlePos.y,true))
	{
		g_pFrm->m_CmdRun.AddMsg("�ƶ�XY��ʧ�ܣ�");
		return false;
	}
	if(!g_pFrm->m_CmdRun.MoveZ(K_AXIS_ZA+nIndex,dZPos,g_pFrm->m_CmdRun.ProductParam.NeedleVel,
		g_pFrm->m_CmdRun.ProductParam.NeedleAcc,true))
	{
		g_pFrm->m_CmdRun.AddMsg("�ƶ�Z��ʧ�ܣ�");
		return false;
	}
	Sleep(100);
	theApp.m_V9Control.V9C_Dot(K_AXIS_U+nIndex,g_pFrm->m_CmdRun.ProductParam.dAutoNeedleGlueVol);
	Sleep(100);
	g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentY++;
	if(g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentY>=g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCountY)
	{
		g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentY = 0;
		g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentX++;
		if(g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentX>=g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCountX)
		{
			g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentX = 0;
		}
	}
	if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
	{
		g_pFrm->m_CmdRun.AddMsg("���õ������ʧ�ܣ�");
		return false;
	}
	g_pFrm->m_CmdRun.MoveToZSafety();
	Sleep(1000);
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgCameraPos.x,tgCameraPos.y,true))
	{
		g_pFrm->m_CmdRun.AddMsg("�ƶ������λʧ�ܣ�");
		return false;
	}
	if(!g_pFrm->m_CmdRun.MoveZ(K_AXIS_ZA,tgCameraPos.za,true))
	{
		g_pFrm->m_CmdRun.AddMsg("�ƶ������λʧ�ܣ�");
		return false;
	}
	return true;
}
bool CCmdRun::AutoAdjustNeedle(int nIndex)
{
	if(!theApp.m_Mv400.IsInitOK())
	{
		return false;
	}
	double dX,dY;
	int nCount = 0;
	tgPos tgCenter;
	double dScore = 0;
	bool bMamualAlign = false;
    //1.move axis;2.dispense;3.move camera;
	if(!g_pFrm->m_CmdRun.MoveToAdjustPos(nIndex))
	{
		return false;
	}
	Sleep(500);
	if(GREY_MODE == g_pFrm->m_CmdRun.ProductParam.searchMode)
	{
		if(!FindNeedleModel(tgCenter.x,tgCenter.y,dScore))
		{
			g_pFrm->m_CmdRun.AddMsg("����ͼ��ʧ�ܣ�");
			//return false;
			bMamualAlign = true;
		}
		else
		{
			theApp.m_tSysParam.AutoNeedleCalib.ConvertPixelToTable(tgCenter.x - (milApp.m_lBufSizeX-1)/2,
				tgCenter.y - (milApp.m_lBufSizeY-1)/2,dX,dY);
			while(fabs(dX)>g_pFrm->m_CmdRun.ProductParam.dAutoNeedleErrorX||
				fabs(dY)>g_pFrm->m_CmdRun.ProductParam.dAutoNeedleErrorY)
			{
				if(nCount>=g_pFrm->m_CmdRun.ProductParam.nAutoAdjustCount)
				{
					break;
				}
				nCount++;
				if(0 == nIndex)
				{
					g_pFrm->m_CmdRun.AdjustNeedle(dX,dY,0.0,0.0);
				}
				else if(1 == nIndex)
				{
					g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].x -= dX;
					g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].y -= dY;
				}
				else if(2 == nIndex)
				{
					g_pFrm->m_CmdRun.AdjustNeedle(0.0,0.0,dX,dY);
				}
				g_pFrm->m_CmdRun.MoveToAdjustPos(nIndex);
				Sleep(500);
				if(!FindNeedleModel(tgCenter.x,tgCenter.y,dScore))
				{
					g_pFrm->m_CmdRun.AddMsg("����ͼ��ʧ�ܣ�");
					//return false;
					bMamualAlign = true;
					break;
				}
				theApp.m_tSysParam.AutoNeedleCalib.ConvertPixelToTable(tgCenter.x - (milApp.m_lBufSizeX-1)/2,
					tgCenter.y - (milApp.m_lBufSizeY-1)/2,dX,dY);
			}
			//if(nCount>=g_pFrm->m_CmdRun.ProductParam.nAutoAdjustCount)
			//{
			//	g_pFrm->m_CmdRun.AddMsg("����ʧ�ܣ�");
			//	return false;
			//}
		}
	}
	else if(BLOB_MODE == g_pFrm->m_CmdRun.ProductParam.searchMode)
	{
		if(!FindBlobCenter(tgCenter.x,tgCenter.y))
		{
			g_pFrm->m_CmdRun.AddMsg("����ͼ��ʧ�ܣ�");
			//return false;
			bMamualAlign = true;
		}
		else
		{
			theApp.m_tSysParam.AutoNeedleCalib.ConvertPixelToTable(tgCenter.x - (milApp.m_lBufSizeX-1)/2,
				tgCenter.y - (milApp.m_lBufSizeY-1)/2,dX,dY);
			//if image process result is acceptable ;have none to do; 
			while(fabs(dX)>g_pFrm->m_CmdRun.ProductParam.dAutoNeedleErrorX||
				fabs(dY)>g_pFrm->m_CmdRun.ProductParam.dAutoNeedleErrorY)
			{
				CFunction::DoEvents();
				if(nCount>=g_pFrm->m_CmdRun.ProductParam.nAutoAdjustCount)
				{
					break;
				}
				nCount++;
				//1th needle:modify the relative lable pos;
				if(0 == nIndex)
				{
					g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].x -= dX;
					g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].y -= dY;

				}
				else if(1 == nIndex)
				{
					g_pFrm->m_CmdRun.AdjustNeedle(dX,dY,0.0,0.0);//right:tuneC D

				}
				else if(2 == nIndex)
				{

					g_pFrm->m_CmdRun.AdjustNeedle(0.0,0.0,dX,dY);//left:tune A B
				}
				g_pFrm->m_CmdRun.MoveToAdjustPos(nIndex); //move-dispense-grab
				Sleep(500);
				if(!FindBlobCenter(tgCenter.x,tgCenter.y))
				{
					g_pFrm->m_CmdRun.AddMsg("����ͼ��ʧ�ܣ�");
					//return false;
					bMamualAlign = true;
				}
				theApp.m_tSysParam.AutoNeedleCalib.ConvertPixelToTable(tgCenter.x - (milApp.m_lBufSizeX-1)/2, //Compute the result ;continue or not;
					tgCenter.y - (milApp.m_lBufSizeY-1)/2,dX,dY);
			}
			//if(nCount>=g_pFrm->m_CmdRun.ProductParam.nAutoAdjustCount)
			//{
			//	g_pFrm->m_CmdRun.AddMsg("����ʧ�ܣ�");
			//	return false;
			//}
		}
	}
	if(bMamualAlign||(nCount>=ProductParam.nAutoAdjustCount))
	{
		m_bIsSupend = true;
		if(AfxMessageBox("�ֶ�����ͼ��",MB_YESNO)!=IDYES)
		{
			m_bIsSupend = false;
			return false;
		}
		else
		{
			CDlgManualAlign manualAlign;
			if(IDOK!=manualAlign.DoModal())
			{
				m_bIsSupend = false;
				return false;
			}
			else
			{
				tgPos tgNeedlePos,tgCameraPos;
				tgNeedlePos.x = g_pFrm->m_CmdRun.ProductParam.NeedlePos[0] - nIndex*ProductParam.NeedleGap +
					g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.dPitchX * g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentX;
				tgNeedlePos.y = g_pFrm->m_CmdRun.ProductParam.NeedlePos[1] +
					g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.dPitchY * g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentY;
				tgCameraPos.x = tgNeedlePos.x + nIndex*g_pFrm->m_CmdRun.ProductParam.NeedleGap +
					(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].x - g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].x);
				tgCameraPos.y = tgNeedlePos.y +
					(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].y - g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].y);
				dX = tgCameraPos.x - m_pMv->GetPos(K_AXIS_X);
				dY = tgCameraPos.y - m_pMv->GetPos(K_AXIS_Y);
				if(0 == nIndex)
				{
					g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].x -= dX;
					g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].y -= dY;
				}
				else if(1 == nIndex)
				{
					g_pFrm->m_CmdRun.AdjustNeedle(0.0,0.0,dX,dY);
				}
				else if(2 == nIndex)
				{
					g_pFrm->m_CmdRun.AdjustNeedle(dX,dY,0.0,0.0);
				}
			}
		}
	}
	return true;
}
bool CCmdRun::AutoAdjustNeedle()
{
	CString str;
	if(!AutoAdjustNeedle(0))
	{
		str.Format("��1���㽺������ʧ�ܣ�");
		g_pFrm->m_CmdRun.AddMsg(str);
		return false;
	}
	if(!AutoAdjustNeedle(1))
	{
		str.Format("��2���㽺������ʧ�ܣ�");
		g_pFrm->m_CmdRun.AddMsg(str);
		return false;
	}
	if(!AutoAdjustNeedle(2))
	{
		str.Format("��3���㽺������ʧ�ܣ�");
		g_pFrm->m_CmdRun.AddMsg(str);
		return false;
	}
	return true;
}
bool CCmdRun::FindNeedleModel(double &dCenterX,double &dCenterY,double &dScore)
{
	if(!g_pView->m_ImgStatic.CopyImage(RGB8))
	{
		return false;
	}
	CString strFile = g_pDoc->GetModeFile(7);
	milApp.RestoreModel(strFile.GetBuffer(strFile.GetLength()));
	milApp.SetPatParam(theApp.m_tSysParam.dAutoNeedleAcceptance,theApp.m_tSysParam.dAutoNeedleAngle);
	milApp.SetModelWindow(theApp.m_tSysParam.AutoNeedleModelWin);
	milApp.SetSearchWindow(theApp.m_tSysParam.AutoNeedleROI);
	CTimeUtil::Delay(long(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay*1000));
	if(!milApp.FindModel(false))
	{
		return false;
	}
	dCenterX = milApp.m_stResult.dResultCenterX[0];
	dCenterY = milApp.m_stResult.dResultCenterY[0];
	dScore = milApp.m_stResult.dResultScore[0];
	return true;
}
bool CCmdRun::FindBlobCenter(double &dCenterX,double &dCenterY)
{
	if(!g_pView->m_ImgStatic.CopyImage(RGB8))
	{
		return false;
	}
	CString str;
	milApp.SetBlobParam(g_pFrm->m_CmdRun.ProductParam.nBlobThreshold,g_pFrm->m_CmdRun.ProductParam.nBlobAreaMin,g_pFrm->m_CmdRun.ProductParam.nBlobAreaMax,1,1,false);
	milApp.FindBlob(theApp.m_tSysParam.AutoNeedleROI);
	int nFindNum = milApp.BlobResult.nBlobNum;
	str.Format("%d",nFindNum);
	g_pFrm->m_CmdRun.AddMsg(str);
	if(nFindNum<=0)
	{
		return false;
	}
	dCenterX = milApp.BlobResult.dBlobCenterX[0];
	dCenterY = milApp.BlobResult.dBlobCenterY[0];
	return true;
}
void CCmdRun::DisableRegTable(bool bDisable)
{
	HKEY hkey;
	DWORD value=-1;
	CString subkey="Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System";
	if(bDisable)
	{
		value=1;
	}
	else
	{
		value=0;
	}
	::RegCreateKey(HKEY_CURRENT_USER,subkey,&hkey);
	::RegSetValueEx(hkey,"DisableRegistryTools",NULL,REG_DWORD,(BYTE*)&value,sizeof(DWORD));
	::RegCloseKey(hkey);
}
void CCmdRun::ComputeNeedleGap()
{
	int nValveAColNum;
	if (THREE_VALVE==ProductParam.valveSelect)
	{
		if (ProductParam.SecondDispColumn>1)
		{
			nValveAColNum=(ProductParam.SecondDispColumn+2)/3;
			ProductParam.NeedleGap=nValveAColNum*ProductParam.SecondDispColumnD;
		}
		else 
		{
			nValveAColNum=(ProductParam.ThirdDispColumn+2)/3;
			ProductParam.NeedleGap=nValveAColNum*ProductParam.ThirdDispColumnD;
		}		
	}
	else if (TWO_VALVE==ProductParam.valveSelect)
	{
		if (ProductParam.SecondDispColumn>1)
		{
			nValveAColNum=(ProductParam.SecondDispColumn+1)/2;
			ProductParam.NeedleGap=nValveAColNum*ProductParam.SecondDispColumnD;
		}
		else
		{
			nValveAColNum=(ProductParam.ThirdDispColumn+1)/2;
			ProductParam.NeedleGap=nValveAColNum*ProductParam.ThirdDispColumnD;
		}

	}
}
bool CCmdRun::WaitAutoNeedleSys(bool bX,bool bY,bool bZA,bool bZB,bool bZC,bool bA,bool bB,bool bC,bool bD)
{
	MOTION_STATUS status;
	CString str;
	if (bX)
	{
		status = m_pMv->WaitStop(K_AXIS_X,60000);
		if(MOT_STATUS_OK!=status)
		{
			str.Format("�ȴ��Զ�����xֹͣʧ�ܣ�");
			AddMsg(str);
			PutLogIntoList(str);
			return false;
		}
	}
	if (bY)
	{
		status = m_pMv->WaitStop(K_AXIS_Y,60000);
		if(MOT_STATUS_OK!=status)
		{
			str.Format("�ȴ��Զ�����yֹͣʧ�ܣ�");
			AddMsg(str);
			PutLogIntoList(str);
			return false;
		}
	}
	if (bZA)
	{
		status = m_pMv->WaitStop(K_AXIS_ZA,60000);
		if(MOT_STATUS_OK!=status)
		{
			str.Format("�ȴ��Զ�����zaֹͣʧ�ܣ�");
			AddMsg(str);
			PutLogIntoList(str);
			return false;
		}
	}
	if (bZB)
	{
		status = m_pMv->WaitStop(K_AXIS_ZB,60000);
		if(MOT_STATUS_OK!=status)
		{
			str.Format("�ȴ��Զ�����zbֹͣʧ�ܣ�");
			AddMsg(str);
			PutLogIntoList(str);
			return false;
		}
	}
	if (bZC)
	{
		status = m_pMv->WaitStop(K_AXIS_ZC,60000);
		if(MOT_STATUS_OK!=status)
		{
			str.Format("�ȴ��Զ�����zcֹͣʧ�ܣ�");
			AddMsg(str);
			PutLogIntoList(str);
			return false;
		}
	}
	if (bA)
	{
		status = m_pMv->WaitStop(K_AXIS_A,60000);
		if(MOT_STATUS_OK!=status)
		{
			str.Format("�ȴ��Զ�����aֹͣʧ�ܣ�");
			AddMsg(str);
			PutLogIntoList(str);
			return false;
		}
	}
	if (bB)
	{
		status = m_pMv->WaitStop(K_AXIS_B,60000);
		if(MOT_STATUS_OK!=status)
		{
			str.Format("�ȴ��Զ�����bֹͣʧ�ܣ�");
			AddMsg(str);
			PutLogIntoList(str);
			return false;
		}
	}
	if (bC)
	{
		status = m_pMv->WaitStop(K_AXIS_C,60000);
		if(MOT_STATUS_OK!=status)
		{
			str.Format("�ȴ��Զ�����cֹͣʧ�ܣ�");
			AddMsg(str);
			PutLogIntoList(str);
			return false;
		}
	}
	if (bD)
	{
		status = m_pMv->WaitStop(K_AXIS_D,60000);
		if(MOT_STATUS_OK!=status)
		{
			str.Format("�ȴ��Զ�����dֹͣʧ�ܣ�");
			AddMsg(str);
			PutLogIntoList(str);
			return false;
		}
	}
	return true;
}

bool CCmdRun::TargetISZoneA(int Num)
{
	if(ProductParam.FirstDispColumn>1)
	{
		for(int j=0;j<(ProductParam.FirstDispColumn+2)/3;j++)
		{
			for(int n=0;n<ProductParam.SecondDispColumn;n++)
			{
				for(int l=0;l<ProductParam.ThirdDispColumn;l++)
				{
					for(int i=0;i<ProductParam.FirstDispRow;i++)
					{
						for(int m=0;m<ProductParam.SecondDispRow;m++)
						{
							for(int k=0;k<ProductParam.ThirdDispRow;k++)
							{
								int nDisNumber = l+k*ProductParam.ThirdDispColumn+n*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
									j*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+i*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
								if(Num==nDisNumber)
								{
									return true;
								}
							}
						}
					}
				}
			}
		}
	}
	else if(ProductParam.SecondDispColumn>1)
	{
		for(int j=0;j<ProductParam.FirstDispColumn;j++)
		{
			for(int n=0;n<(ProductParam.SecondDispColumn+2)/3;n++)
			{
				for(int l=0;l<ProductParam.ThirdDispColumn;l++)
				{
					for(int i=0;i<ProductParam.FirstDispRow;i++)
					{
						for(int m=0;m<ProductParam.SecondDispRow;m++)
						{
							for(int k=0;k<ProductParam.ThirdDispRow;k++)
							{
								int nDisNumber = l+k*ProductParam.ThirdDispColumn+n*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
									j*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+i*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
								if(Num==nDisNumber)
								{
									return true;
								}
							}
						}
					}
				}
			}
		}
	}
	else
	{
		for(int j=0;j<ProductParam.FirstDispColumn;j++)
		{
			for(int n=0;n<ProductParam.SecondDispColumn;n++)
			{
				for(int l=0;l<(ProductParam.ThirdDispColumn+2)/3;l++)
				{
					for(int i=0;i<ProductParam.FirstDispRow;i++)
					{
						for(int m=0;m<ProductParam.SecondDispRow;m++)
						{
							for(int k=0;k<ProductParam.ThirdDispRow;k++)
							{
								int nDisNumber = l+k*ProductParam.ThirdDispColumn+n*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+m*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+
									j*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn+i*ProductParam.FirstDispColumn*ProductParam.SecondDispRow*ProductParam.SecondDispColumn*ProductParam.ThirdDispRow*ProductParam.ThirdDispColumn;
								if(Num==nDisNumber)
								{
									return true;
								}
							}
						}
					}
				}
			}
		}
	}
	return false;

}
BOOL CCmdRun::CheckHeightEx(int nAxisNo,double &zOffset)
{
	if(nAxisNo<K_AXIS_ZA||nAxisNo>K_AXIS_ZC)
	{
		return FALSE;
	}
	double dHeight = 0.0;
	long lStatus;
	BOOL bHeightSensor = FALSE;
	double dPosSel=0.0;
	double dNewPos;
	zOffset=0;

	if(theApp.m_tSysParam.sensorType==ContactSensorClose)
	{
		bHeightSensor = TRUE; 
	}
	else if(theApp.m_tSysParam.sensorType==ContactSensorOpen)
	{
		bHeightSensor = FALSE;
	}
	if(!MoveToZSafety())
	{
		return FALSE;
	}
	//:posing choose:
	if (nAxisNo==K_AXIS_ZA)
	{
		dPosSel=ProductParam.ContactSensorPos[0];
	}
	if (nAxisNo==K_AXIS_ZB)
	{
		dPosSel=ProductParam.ContactSensorPos[0]-(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].x-g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].x);
	}
	if (nAxisNo==K_AXIS_ZC)
	{
		dPosSel=ProductParam.ContactSensorPos[0]-(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].x-g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2].x);
	}
	
	if(!SynchronizeMoveXY(dPosSel,ProductParam.ContactSensorPos[1],ProductParam.FreeVal,ProductParam.FreeAcc,true))
	{
		return FALSE;
	}
	dHeight = (g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[nAxisNo-1]-10)<40?(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[nAxisNo-1]-10):40;
	if(!MoveZ(nAxisNo,dHeight,5,20,false))
	{
		return FALSE;
	}
	theApp.m_Mv400.GetAxisSts(nAxisNo,&lStatus);
	while(lStatus&0x400)
	{
		if(theApp.m_Mv400.GetInput(theApp.m_tSysParam.inHeight)==bHeightSensor)
		{
			theApp.m_Mv400.Stop(nAxisNo);
			break;
		}
		theApp.m_Mv400.GetAxisSts(nAxisNo,&lStatus);
	}
	Sleep(100);
	if(!MoveZ(nAxisNo,theApp.m_tSysParam.tAxis[nAxisNo-1].maxpos,
		g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel,g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc,false))
	{
		return FALSE;
	}
	BOOL bMeasureSucceed = FALSE;
	theApp.m_Mv400.GetAxisSts(nAxisNo,&lStatus);
	while(lStatus&0x400)
	{
		if(theApp.m_Mv400.GetInput(theApp.m_tSysParam.inHeight)==bHeightSensor)
		{
			theApp.m_Mv400.Stop(nAxisNo);
			break;
		}
		theApp.m_Mv400.GetAxisSts(nAxisNo,&lStatus);
	}
	if(theApp.m_Mv400.GetInput(theApp.m_tSysParam.inHeight)==(!bHeightSensor))
	{
		return bMeasureSucceed;
	}
	//��������
	double dPos = theApp.m_Mv400.GetPos(nAxisNo);
	if(!MoveZ(nAxisNo,dPos-1.0,g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel,
		g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc,true))
	{
		return FALSE;
	}
	Sleep(100);
	if(!MoveZ(nAxisNo,theApp.m_tSysParam.tAxis[nAxisNo-1].maxpos,
		g_pFrm->m_CmdRun.ProductParam.ContactMeasureVel/5,g_pFrm->m_CmdRun.ProductParam.ContactMeasureAcc/2,false))
	{
		return FALSE;
	}
	theApp.m_Mv400.GetAxisSts(nAxisNo,&lStatus);
	//����ƽ
	while(lStatus&0x400)
	{
		if(theApp.m_Mv400.GetInput(theApp.m_tSysParam.inHeight)==bHeightSensor)
		{
			theApp.m_Mv400.Stop(nAxisNo);
			Sleep(50);
			dNewPos=theApp.m_Mv400.GetPos(nAxisNo);
			zOffset=dNewPos-g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[nAxisNo-1];  //0--4:2,3,4:z����:ƫ��ֵ
			//g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[nAxisNo-1] = dNewPos;
			bMeasureSucceed = TRUE;
			break;
		}
		theApp.m_Mv400.GetAxisSts(nAxisNo,&lStatus);
	}
	if(!SetAxisProfile(MEDIUM_VEL))
	{
		return FALSE;
	}
	if(!MoveToZSafety())
	{
		return FALSE;
	}
	return bMeasureSucceed;
}
bool CCmdRun::NeedleHeightCheck()
{
	double dzOffset;
	CString str;
    
	str.Format("��ͷ�߶ȼ�鿪ʼ...");
	AddMsg(str);
	m_pMv->Move(K_AXIS_A,ProductParam.offset2x,true,true);
	m_pMv->Move(K_AXIS_C,ProductParam.offset3x,true,true);
	m_pMv->Move(K_AXIS_B,ProductParam.offset2y,true,true);
	m_pMv->Move(K_AXIS_D,ProductParam.offset3y,true,true);

	if(!CheckHeightEx(K_AXIS_ZA,dzOffset))
	{
		//Alarm(42);
		AlarmOnly(42);
		str.Format("��ͷA���ʧ��!");
		AddMsg(str);
		return false;
	}
	str.Format("��ͷA�߶ȱ仯:%0.3f mm...",dzOffset);
	PutLogIntoList(str);
	AddMsg(str);

	if(fabs(dzOffset)>ProductParam.ContactErrPermit)
	{
		AlarmOnly(43);
		str.Format("��ͷA��߸������ޣ�");
		AddMsg(str);
		PutLogIntoList(str);
		return false;
	}
	if(!CheckHeightEx(K_AXIS_ZB,dzOffset))
	{
		AlarmOnly(44);
		str.Format("��ͷB���ʧ��!");
		AddMsg(str);
		return false;
	}
	str.Format("��ͷB�߶ȱ仯:%0.3f mm...",dzOffset);
	PutLogIntoList(str);
	AddMsg(str);

	if(fabs(dzOffset)>ProductParam.ContactErrPermit)
	{
		AlarmOnly(45);
		str.Format("��ͷB��߸������ޣ�");
		AddMsg(str);
		PutLogIntoList(str);
		return false;
	}
	if(!CheckHeightEx(K_AXIS_ZC,dzOffset))
	{
		AlarmOnly(46);
		str.Format("��ͷC���ʧ��!");
		AddMsg(str);
		return false;
	}
	str.Format("��ͷC�߶ȱ仯:%0.3f mm...",dzOffset);
	PutLogIntoList(str);
	AddMsg(str);

	if(fabs(dzOffset)>ProductParam.ContactErrPermit)
	{
		AlarmOnly(47);
		str.Format("��ͷC��߸������ޣ�");
		AddMsg(str);
		PutLogIntoList(str);
		return false;
	}
	str.Format("��ͷ�߶ȼ��ɹ�...");
	AddMsg(str);
	return true;
}

CString CCmdRun::PickOutURLMsg(const CString key, const CString magbuf)
{
	CString result = _T("0");
	int pos0, pos1, pos2;

	pos0 = magbuf.Find(key);
	if (pos0==-1)
	{
		return result;
	}

	pos0 = magbuf.Find(_T(":"),pos0);
	if (pos0==-1)
	{
		return result;
	}

	pos1 = magbuf.Find(_T("|"),pos0);
	if (pos1==-1)
	{
		return result;
	}
	pos2 = pos1 - (pos0 + 1);
	return magbuf.Mid(pos0+1,pos2); 
}


int CCmdRun::ReadURLText(CString URL,CString &ServerIP,int &ServerPort,int &FCompanyCode)
{
	CInternetSession MySession;
	CString StrURL=URL;     
	CString StrInfo;
	CInternetFile* MyFile = NULL;
	CString str;

	try
	{
		MyFile=(CInternetFile*)MySession.OpenURL(StrURL);
	}
	catch (CInternetException* pEx)
	{
		MyFile = NULL;
		pEx->Delete();		
	}
	if(MyFile)
	{
		CString StrLine;

		while(MyFile->ReadString(StrLine))
		{
			StrInfo += StrLine + "\r\n";
		}
		MyFile->Close();
		delete MyFile;
	}
	else
	{
		StrInfo+= " û�в�ѯ���!\r\n";
		str.Format("��������URLʧ��...");
		PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return -1;
	}
	ServerIP = PickOutURLMsg("ServerIP",StrInfo);
	str.Format("���λ�ȡIP:");
	str.Append(StrInfo);
	PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);

	ServerPort = atoi(PickOutURLMsg("ServerPort",StrInfo));
	str.Format("���λ�ȡPORT:");
	str.Append(StrInfo);
	PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);

	FCompanyCode = atoi(PickOutURLMsg("FCompanyCode",StrInfo));
	str.Format("���λ�ȡ��˾����:");
	str.Append(StrInfo);
	PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);
	return 0;
}
CString CCmdRun::ReadProdutMag(const CString key, const CString magbuf)
{
	CString result = _T("0");
	int pos0, pos1, pos2;

	pos0 = magbuf.Find(key);
	if (pos0==-1)
	{
		return result;
	}

	pos0 = magbuf.Find(_T(":"),pos0);
	if (pos0==-1)
	{
		return result;
	}

	pos1 = magbuf.Find(_T(";"),pos0);
	if (pos1==-1)
	{
		return result;
	}

	pos2 = pos1 - (pos0 + 1);

	return magbuf.Mid(pos0+1,pos2); 
}
bool CCmdRun::FindProdutMag(const CString key, const CString magbuf)
{
	bool bRet = false;
	int pos0;

	pos0 = magbuf.Find(key);
	if (pos0!=-1)
	{
		bRet = true;
	}

	return bRet;
}
bool CCmdRun::GlueCheck()
{
	double dTemp;
	if (g_pFrm->m_CmdRun.ProductParam.bMesOffline)
	{
		return TRUE;
	}
	for(int i = 0; i < 3;i++)
	{
		dTemp = m_Glue_msg.rev_MESGlue[i];
	    if((dTemp < ProductParam.MesDataMin) || (dTemp > ProductParam.MesDataMax))
		{
			return false;
		}
	}
	return true;
}
