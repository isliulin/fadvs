// PagePLC.cpp : implementation file
#include "stdafx.h"
#include "TSCtrlSys.h"
#include "PagePLC.h"

// CPagePLC dialog

IMPLEMENT_DYNAMIC(CPagePLC, CPropertyPage)

CPagePLC::CPagePLC()
	: CPropertyPage(CPagePLC::IDD)
{

}

CPagePLC::~CPagePLC()
{
}

void CPagePLC::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPagePLC, CPropertyPage)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_COMMAND_RANGE(IDC_BTN_PLC0,IDC_BTN_PLC39,&CPagePLC::OnBnClickedPLC)
	ON_BN_CLICKED(IDC_CANCEL, &CPagePLC::OnCancel)
	ON_BN_CLICKED(IDC_SAVE, &CPagePLC::OnSave)
	ON_BN_CLICKED(IDC_BTN_PLC_MANUAL_LOAD, &CPagePLC::OnBnClickedBtnPlcManualLoad)
	ON_BN_CLICKED(IDC_BTN_PLC_MANUAL_UNLOAD, &CPagePLC::OnBnClickedBtnPlcManualUnload)
	ON_BN_CLICKED(IDC_BTN_PLC_PREHEAT, &CPagePLC::OnBnClickedBtnPlcPreheat)
	ON_BN_CLICKED(IDC_BTN_PLC_HEAT, &CPagePLC::OnBnClickedBtnPlcHeat)
	ON_BN_CLICKED(IDC_CHECK_PLC_INBOX_DETECT, &CPagePLC::OnBnClickedCheckPlcInboxDetect)
	ON_BN_CLICKED(IDC_CHECK_PLC_OUTBOX_DETECT, &CPagePLC::OnBnClickedCheckPlcOutboxDetect)
	ON_BN_CLICKED(IDC_CHECK_PLC_TEMP_DETECT, &CPagePLC::OnBnClickedCheckPlcTempDetect)
	ON_BN_CLICKED(IDC_BTN_LINEBACK, &CPagePLC::OnBnClickedBtnLineback)
END_MESSAGE_MAP()

// CPagePLC message handlers

// 向PLC发送输出信号，测试料架、气缸等
void CPagePLC::OnBnClickedPLC(UINT nID)
{
	CString str;
	int nBitNum = nID - IDC_BTN_PLC0;
	if(nBitNum<=15)
	{
		nBitNum += 1200;
	}
	else if(nBitNum<=31)
	{
		nBitNum = nBitNum-16+1300;  
	}
	else
	{
		nBitNum = nBitNum-32+1400;   //1403
	}
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
// 参数保存，与PLC通讯
void CPagePLC::OnSave()
{
	BOOL bRet = 0;
	long lWriteData = 0;
	double dWriteData = 0.0;
	int nWriteData = 0;
	CString str;
	double dLoadVel = 0.0,dUnloadVel = 0.0;
	double dPreheatTmp = 0.0,dHeatTmp = 0.0;

	GetDlgItemText(IDC_EDIT_PLC_LOAD_VEL,str);
	dLoadVel = atof(str);
	GetDlgItemText(IDC_EDIT_PLC_UNLOAD_VEL,str);
	dUnloadVel = atof(str);
	GetDlgItemText(IDC_EDIT_PLC_PREHEAT_SETTING,str);
	dPreheatTmp = atof(str);
	GetDlgItemText(IDC_EDIT_PLC_HEAT_SETTING,str);
	dHeatTmp = atof(str);
	if(dLoadVel<-0.001||dUnloadVel<-0.001)
	{
		AfxMessageBox("进料速度或出料速度不能小于0");
		return;
	}
	if(dLoadVel-100>0.001||dUnloadVel-100>0.001)
	{
		AfxMessageBox("进料速度或出料速度不能大于100");
		return;
	}
	if(dPreheatTmp<-0.001||dHeatTmp<-0.001)
	{
		AfxMessageBox("预热区或加热区温度设定不能小于0度");
		return;
	}
	if(dPreheatTmp-80>0.001||dHeatTmp-80>0.001)
	{
		AfxMessageBox("预热区或加热区温度设定不能超过80度");
		return;
	}
	g_pFrm->m_CmdRun.ProductParam.dPreHeatTemp = dPreheatTmp;
	g_pFrm->m_CmdRun.ProductParam.dDispHeatTemp = dHeatTmp;

	GetDlgItemText(IDC_EDIT_PLC_LOAD_VEL,str);
	dWriteData = atof(str);
	lWriteData = long(dWriteData/MM_FACTOR);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePlcData(lWriteData,"DM","1200");
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePLCData("1200",lWriteData);
	}
	if(!bRet)
	{
		AfxMessageBox("保存失败！");
		return;
	}
	Sleep(100);

	GetDlgItemText(IDC_EDIT_PLC_UNLOAD_VEL,str);
	dWriteData = atof(str);
	lWriteData = long(dWriteData/MM_FACTOR);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePlcData(lWriteData,"DM","1202");
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePLCData("1202",lWriteData);
	}
	if(!bRet)
	{
		AfxMessageBox("保存失败！");
		return;
	}
	Sleep(100);

	//流水线速度：
	GetDlgItemText(IDC_EDIT_PLC_TRANSIT_SPEED,str);
	dWriteData = atof(str);
	lWriteData = long(dWriteData/MM_FACTOR);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePlcData(lWriteData,"DM","1204");
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePLCData("1202",lWriteData);
	}
	if(!bRet)
	{
		AfxMessageBox("保存失败！");
		return;
	}

	GetDlgItemText(IDC_EDIT_PLC_LOAD_MAG_NUM,str);
	nWriteData = atoi(str);
	lWriteData = long(nWriteData/MM_FACTOR);
	str.Format("%ld",lWriteData);
	g_pFrm->m_CmdRun.AddMsg(str);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePlcData(lWriteData,"DM","1206");
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePLCData("1206",lWriteData);
	}
	if(!bRet)
	{
		AfxMessageBox("保存失败！");
		return;
	}
	Sleep(100);

	GetDlgItemText(IDC_EDIT_PLC_LOAD_MAG_SPACE,str);
	dWriteData = atof(str);
	lWriteData = long(dWriteData/MM_FACTOR);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePlcData(lWriteData,"DM","1208");
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePLCData("1208",lWriteData);
	}
	if(!bRet)
	{
		AfxMessageBox("保存失败！");
		return;
	}
	Sleep(100);

	GetDlgItemText(IDC_EDIT_PLC_UNLOAD_MAG_NUM,str);
	nWriteData = atoi(str);
	lWriteData = long(nWriteData/MM_FACTOR);
	str.Format("%ld",lWriteData);
	g_pFrm->m_CmdRun.AddMsg(str);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePlcData(lWriteData,"DM","1216");
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePLCData("1216",lWriteData);
	}
	if(!bRet)
	{
		AfxMessageBox("保存失败！");
		return;
	}
	Sleep(100);

	GetDlgItemText(IDC_EDIT_PLC_UNLOAD_MAG_SPACE,str);
	dWriteData = atof(str);
	lWriteData = long(dWriteData/MM_FACTOR);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePlcData(lWriteData,"DM","1218");
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePLCData("1218",lWriteData);
	}
	if(!bRet)
	{
		AfxMessageBox("保存失败！");
		return;
	}
	Sleep(100);

	GetDlgItemText(IDC_EDIT_PLC_DISPENSE_VEL,str);
	dWriteData = atof(str);
	lWriteData = long(dWriteData/MM_FACTOR);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePlcData(lWriteData,"DM","1210");
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePLCData("1210",lWriteData);
	}
	if(!bRet)
	{
		AfxMessageBox("保存失败！");
		return;
	}
	Sleep(100);

	GetDlgItemText(IDC_EDIT_PLC_FILL_VEL,str);
	dWriteData = atof(str);
	lWriteData = long(dWriteData/MM_FACTOR);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePlcData(lWriteData,"DM","1212");
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePLCData("1212",lWriteData);
	}
	if(!bRet)
	{
		AfxMessageBox("保存失败！");
		return;
	}
	Sleep(100);

	GetDlgItemText(IDC_EDIT_PLC_FILL_DISTANCE,str);
	dWriteData = atof(str);
	lWriteData = long(dWriteData/MM_FACTOR);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePlcData(lWriteData,"DM","1214");
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePLCData("1214",lWriteData);
	}
	if(!bRet)
	{
		AfxMessageBox("保存失败！");
		return;
	}
	Sleep(100);

	GetDlgItemText(IDC_EDIT_PLC_PUSH_COUNT,str);
	dWriteData = atoi(str);
	lWriteData = long(dWriteData/MM_FACTOR);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePlcData(lWriteData,"DM","1228");
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePLCData("1228",lWriteData);
	}
	if(!bRet)
	{
		AfxMessageBox("保存失败！");
		return;
	}
	Sleep(100);

	GetDlgItemText(IDC_EDIT_PLC_PREHEAT_SETTING,str);
	dWriteData = atof(str);
	lWriteData = long(dWriteData/MM_FACTOR);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePlcData(lWriteData,"DM","1230");
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePLCData("1230",lWriteData);
	}
	if(!bRet)
	{
		AfxMessageBox("保存失败！");
		return;
	}
	Sleep(100);

	GetDlgItemText(IDC_EDIT_PLC_HEAT_SETTING,str);
	dWriteData = atof(str);
	g_pFrm->m_CmdRun.ProductParam.dHeatSetting=dWriteData;//点胶区设定温度
	lWriteData = long(dWriteData/MM_FACTOR);
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePlcData(lWriteData,"DM","1232");
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		bRet = theApp.m_Serial.WritePLCData("1232",lWriteData);
	}
	if(!bRet)
	{
		AfxMessageBox("保存失败！");
		return;
	}
	theApp.ProductParam(FALSE);
	Sleep(100);
	AfxMessageBox("保存成功！");
}
// 取消
void CPagePLC::OnCancel()
{
	OnClose();
	CPropertyPage::OnCancel();
}

// 初始化
BOOL CPagePLC::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CString str;
	GetDlgItem(IDC_BTN_PLC1)->ModifyStyle(0,BS_OWNERDRAW|BS_AUTOCHECKBOX,0);
	GetDlgItem(IDC_BTN_PLC2)->ModifyStyle(0,BS_OWNERDRAW|BS_AUTOCHECKBOX,0);
	GetDlgItem(IDC_BTN_PLC9)->ModifyStyle(0,BS_OWNERDRAW|BS_AUTOCHECKBOX,0);
	GetDlgItem(IDC_BTN_PLC10)->ModifyStyle(0,BS_OWNERDRAW|BS_AUTOCHECKBOX,0);
	//m_btnLoaderJogP.Attach(IDC_BTN_PLC1,this);
	//m_btnLoaderJogN.Attach(IDC_BTN_PLC2,this);
	//m_btnUnloaderJogP.Attach(IDC_BTN_PLC9,this);
	//m_btnUnloaderJogN.Attach(IDC_BTN_PLC10,this);
	m_btnLoaderJogP.SubclassDlgItem(IDC_BTN_PLC1,this);
	m_btnLoaderJogN.SubclassDlgItem(IDC_BTN_PLC2,this);
	m_btnUnloaderJogP.SubclassDlgItem(IDC_BTN_PLC9,this);
	m_btnUnloaderJogN.SubclassDlgItem(IDC_BTN_PLC10,this);
	m_btnLoaderJogP.SetUpColor(RGB(255,255,255));
	m_btnLoaderJogN.SetUpColor(RGB(255,255,255));
	m_btnUnloaderJogP.SetUpColor(RGB(255,255,255));
	m_btnUnloaderJogN.SetUpColor(RGB(255,255,255));
	m_btnLoaderJogP.SetDownColor(RGB(255,0,0));
	m_btnLoaderJogN.SetDownColor(RGB(255,0,0));
	m_btnUnloaderJogP.SetDownColor(RGB(255,0,0));
	m_btnUnloaderJogN.SetDownColor(RGB(255,0,0));
	if(theApp.m_Serial.m_Port[0].m_iComState>0)
	{
		if(!UpdateUI())
		{
			AfxMessageBox("PLC数据读取失败！");
		}
	}
	else
	{
		AfxMessageBox("PLC通讯失败！");
	}
	SetAuthority();
	SetTimer(0,500,NULL);
	return TRUE;
}

void CPagePLC::OnTimer(UINT_PTR nIDEvent)
{
	UpdateHeatState();
	UpdateInputState();
	UpdateBoxCheckStatus();//更新料盒检测状态；
	UpdateTempCheckSwitchStatus();
	CPropertyPage::OnTimer(nIDEvent);
}

void CPagePLC::OnClose()
{
	CString str;
	if(m_btnLoaderJogP.m_bCheck)
	{
		str.Format("1201");
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
	if(m_btnLoaderJogN.m_bCheck)
	{
		str.Format("1202");
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
	if(m_btnUnloaderJogP.m_bCheck)
	{
		str.Format("1209");
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
	if(m_btnUnloaderJogN.m_bCheck)
	{
		str.Format("1210");
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
	KillTimer(0);
}

void CPagePLC::OnBnClickedBtnPlcManualLoad()
{
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

void CPagePLC::OnBnClickedBtnPlcManualUnload()
{
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

bool CPagePLC::UpdateUI()
{
	bool bReadData = false;
	long lReadData = 0;
	double dReadData = 0.0;
	CString str;

	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		//////////////////////////////////////////////////////////////////////////
		// 上料下料位
		//bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","50");
		//if(!bReadData)			return false;
		//dReadData = lReadData;
		//str.Format("%f",dReadData);
		//SetDlgItemText(IDC_EDIT_LOAD_POS1,str);

		//bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","52");
		//if(!bReadData)			return false;
		//dReadData = lReadData;
		//str.Format("%f",dReadData);
		//SetDlgItemText(IDC_EDIT_LOAD_POS2,str);

		//bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","54", ".D");
		//if(!bReadData)			return false;
		//dReadData = lReadData;
		//str.Format("%f",dReadData);
		//SetDlgItemText(IDC_EDIT_LOAD_POS3,str);

		//bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","60");
		//if(!bReadData)			return false;
		//dReadData = lReadData;
		//str.Format("%f",dReadData);
		//SetDlgItemText(IDC_EDIT_UNLOAD_POS1,str);

		//bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","62");
		//if(!bReadData)			return false;
		//dReadData = lReadData;
		//str.Format("%f",dReadData);
		//SetDlgItemText(IDC_EDIT_UNLOAD_POS2,str);

		//bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","64", ".D");
		//if(!bReadData)			return false;
		//dReadData = lReadData;
		//str.Format("%f",dReadData);
		//SetDlgItemText(IDC_EDIT_UNLOAD_POS3,str);
		//////////////////////////////////////////////////////////////////////////
		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1200");
		if(!bReadData)
		{
			return false;
		}
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_PLC_LOAD_VEL,str);

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1202");
		if(!bReadData)
		{
			return false;
		}
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_PLC_UNLOAD_VEL,str);

		//流水线速度
		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1204");
		if(!bReadData)
		{
			return false;
		}
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_PLC_TRANSIT_SPEED,str);

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1206");
		if(!bReadData)
		{
			return false;
		}
		str.Format("%d",int(lReadData*MM_FACTOR));
		SetDlgItemText(IDC_EDIT_PLC_LOAD_MAG_NUM,str);

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1208");
		if(!bReadData)
		{
			return false;
		}
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_PLC_LOAD_MAG_SPACE,str);

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1216");
		if(!bReadData)
		{
			return false;
		}
		str.Format("%d",int(lReadData*MM_FACTOR));
		SetDlgItemText(IDC_EDIT_PLC_UNLOAD_MAG_NUM,str);

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1218");
		if(!bReadData)
		{
			return false;
		}
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_PLC_UNLOAD_MAG_SPACE,str);

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1210");
		if(!bReadData)
		{
			return false;
		}
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_PLC_DISPENSE_VEL,str);

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1212");
		if(!bReadData)
		{
			return false;
		}
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_PLC_FILL_VEL,str);

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1214");
		if(!bReadData)
		{
			return false;
		}
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_PLC_FILL_DISTANCE,str);

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1228");
		if(!bReadData)
		{
			return false;
		}
		dReadData = lReadData*MM_FACTOR;
		str.Format("%d",(int)dReadData);
		SetDlgItemText(IDC_EDIT_PLC_PUSH_COUNT,str);

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1230");
		if(!bReadData)
		{
			return false;
		}
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_PLC_PREHEAT_SETTING,str);

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1232");
		if(!bReadData)
		{
			return false;
		}
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_PLC_HEAT_SETTING,str);
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		lReadData = theApp.m_Serial.ReadPlcData("1200");
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_PLC_LOAD_VEL,str);

		lReadData = theApp.m_Serial.ReadPlcData("1202");
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_PLC_UNLOAD_VEL,str);

		lReadData = theApp.m_Serial.ReadPlcData("1206");
		str.Format("%d",int(lReadData*MM_FACTOR));
		SetDlgItemText(IDC_EDIT_PLC_LOAD_MAG_NUM,str);

		lReadData = theApp.m_Serial.ReadPlcData("1208");
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_PLC_LOAD_MAG_SPACE,str);

		lReadData = theApp.m_Serial.ReadPlcData("1216");
		str.Format("%d",int(lReadData*MM_FACTOR));
		SetDlgItemText(IDC_EDIT_PLC_UNLOAD_MAG_NUM,str);

		lReadData = theApp.m_Serial.ReadPlcData("1218");
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_PLC_UNLOAD_MAG_SPACE,str);

		lReadData = theApp.m_Serial.ReadPlcData("1210");
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_PLC_DISPENSE_VEL,str);

		lReadData = theApp.m_Serial.ReadPlcData("1212");
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_PLC_FILL_VEL,str);

		lReadData = theApp.m_Serial.ReadPlcData("1214");
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_PLC_FILL_DISTANCE,str);

		lReadData = theApp.m_Serial.ReadPlcData("1228");
		dReadData = lReadData*MM_FACTOR;
		str.Format("%d",(int)dReadData);
		SetDlgItemText(IDC_EDIT_PLC_PUSH_COUNT,str);

		lReadData = theApp.m_Serial.ReadPlcData("1230");
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_PLC_PREHEAT_SETTING,str);

		lReadData = theApp.m_Serial.ReadPlcData("1232");
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_PLC_HEAT_SETTING,str);
	}
	return true;
}
void CPagePLC::OnBnClickedBtnPlcPreheat()
{
	// TODO: Add your control notification handler code here
	CString str;
	str.Format("1310");
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

void CPagePLC::OnBnClickedBtnPlcHeat()
{
	// TODO: Add your control notification handler code here
	CString str;
	str.Format("1311");
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

bool CPagePLC::UpdateHeatState()
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
			((CButton*)GetDlgItem(IDC_CHECK_PLC_PREHEAT))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_PREHEAT))->SetCheck(0);
		}

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"R","1411");
		if(!bReadData)
		{
			return false;
		}
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_HEAT))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_HEAT))->SetCheck(0);
		}
		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"R","1412");
		if(!bReadData)
		{
			return false;
		}
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_PREHEAT_RESET))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_PREHEAT_RESET))->SetCheck(0);
		}
		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"R","1413");
		if(!bReadData)
		{
			return false;
		}
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_HEAT_RESET))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_HEAT_RESET))->SetCheck(0);
		}

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1240");
		if(!bReadData)
		{
			return false;
		}
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_PLC_PREHEAT_TEMP,str);
		g_pFrm->m_CmdRun.ProductParam.dRealPreHeatTemp = dReadData;

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"DM","1242");
		if(!bReadData)
		{
			return false;
		}
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_PLC_HEAT_TEMP,str);
		g_pFrm->m_CmdRun.ProductParam.dRealDispHeatTemp = dReadData;
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		lReadData = theApp.m_Serial.ReadPlcRData("1410");
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_PREHEAT))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_PREHEAT))->SetCheck(0);
		}

		lReadData = theApp.m_Serial.ReadPlcData("1411");
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_HEAT))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_HEAT))->SetCheck(0);
		}
		lReadData = theApp.m_Serial.ReadPlcData("1412");
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_PREHEAT_RESET))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_PREHEAT_RESET))->SetCheck(0);
		}
		lReadData = theApp.m_Serial.ReadPlcData("1413");
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_HEAT_RESET))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_HEAT_RESET))->SetCheck(0);
		}

		lReadData = theApp.m_Serial.ReadPlcData("1240");
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_PLC_PREHEAT_TEMP,str);
		g_pFrm->m_CmdRun.ProductParam.dRealPreHeatTemp = dReadData;

		lReadData = theApp.m_Serial.ReadPlcData("1242");
		dReadData = lReadData*MM_FACTOR;
		str.Format("%0.3f",dReadData);
		SetDlgItemText(IDC_EDIT_PLC_HEAT_TEMP,str);
		g_pFrm->m_CmdRun.ProductParam.dRealDispHeatTemp = dReadData;
	}
	return true;
}

bool CPagePLC::UpdateInputState()
{
	bool bReadData = false;
	long lReadData = 0;
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"R","202");
		if(!bReadData)
		{
			return false;
		}
		if(lReadData == 1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_INPUT1))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_INPUT1))->SetCheck(0);
		}

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"R","203");
		if(!bReadData)
		{
			return false;
		}
		if(lReadData == 1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_INPUT2))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_INPUT2))->SetCheck(0);
		}
	}
	return true;
}

void CPagePLC::SetAuthority()
{
	GetDlgItem(IDC_BTN_PLC32)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_BTN_PLC33)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_PLC_LOAD_VEL)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_PLC_UNLOAD_VEL)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_PLC_LOAD_MAG_NUM)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_PLC_LOAD_MAG_SPACE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_PLC_UNLOAD_MAG_NUM)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_PLC_UNLOAD_MAG_SPACE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_PLC_DISPENSE_VEL)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_PLC_FILL_VEL)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_PLC_FILL_DISTANCE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_PLC_PREHEAT_SETTING)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_PLC_HEAT_SETTING)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	//the speed can only be modify by administor; 
	GetDlgItem(IDC_EDIT_PLC_TRANSIT_SPEED)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
}
bool CPagePLC::UpdateBoxCheckStatus()
{
	bool bReadData = false;
	long lReadData = 0;
	double dReadData = 0.0;

	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"MR","1314");
		if(!bReadData)
		{
			return false;
		}
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_INBOX_DETECT))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_INBOX_DETECT))->SetCheck(0);
		}

		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"MR","1315");
		if(!bReadData)
		{
			return false;
		}
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_OUTBOX_DETECT))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_OUTBOX_DETECT))->SetCheck(0);
		}

	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		lReadData = theApp.m_Serial.ReadPlcRData("1314");
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_INBOX_DETECT))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_INBOX_DETECT))->SetCheck(0);
		}

		lReadData = theApp.m_Serial.ReadPlcData("1315");
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_OUTBOX_DETECT))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_OUTBOX_DETECT))->SetCheck(0);
		}

	}
	return true;

}
//温度检测使能状态  2017-08-16  1112-->1700 
bool CPagePLC::UpdateTempCheckSwitchStatus()
{
	bool bReadData = false;
	long lReadData = 0;
	double dReadData = 0.0;

	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"R","1700");
		if(!bReadData)
		{
			return false;
		}
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_TEMP_DETECT))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_TEMP_DETECT))->SetCheck(0);
		}
	}
	else if(PLC_PANASONIC == theApp.m_tSysParam.PlcType)
	{
		lReadData = theApp.m_Serial.ReadPlcRData("1700");
		if(lReadData==1)
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_TEMP_DETECT))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_PLC_TEMP_DETECT))->SetCheck(0);
		}
	}
	return true;

}

void CPagePLC::OnBnClickedCheckPlcInboxDetect()
{
	// TODO: Add your control notification handler code here
	CString str;
	str.Format("1314");
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

void CPagePLC::OnBnClickedCheckPlcOutboxDetect()
{
	// TODO: Add your control notification handler code here
	CString str;
	str.Format("1315");
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
//自动运行前PLC是否判断当前温度
void CPagePLC::OnBnClickedCheckPlcTempDetect()
{
	CString str;
	str.Format("1112");
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


void CPagePLC::OnBnClickedBtnLineback()
{
	CString str;
	str.Format("%d",1408);
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
