// V9Param.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "V9Param.h"


// CV9Param dialog


IMPLEMENT_DYNAMIC(CV9Param, CDialog)

CV9Param::CV9Param(CWnd* pParent /*=NULL*/)
	: CDialog(CV9Param::IDD, pParent)
{
}

CV9Param::~CV9Param()
{
}

void CV9Param::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CV9Param, CDialog)
	ON_COMMAND_RANGE(IDC_BTV9MA, IDC_BTV9MA18,&CV9Param::OnBnClickedFroceM) 
	ON_BN_CLICKED(IDC_RESET_ALL, &CV9Param::OnBnClickedResetAll)
	ON_BN_CLICKED(IDC_HOME_ALL, &CV9Param::OnBnClickedHomeAll)
	ON_BN_CLICKED(IDC_DISPENSING_ALL, &CV9Param::OnBnClickedDispensingAll)
	ON_BN_CLICKED(IDC_CLEAR_ALL, &CV9Param::OnBnClickedClearAll)
	ON_BN_CLICKED(IDC_UNIFORM_PARAM, &CV9Param::OnBnClickedUniformParam)
	ON_BN_CLICKED(IDC_SAVE_ALL, &CV9Param::OnBnClickedSaveAll)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_PAMA1, IDC_PAMA18, &CV9Param::OnEnChangeEdit1)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_PAMA27, IDC_PAMA29, &CV9Param::OnEnFirstCompensation)

	ON_COMMAND_RANGE(IDC_Press1,IDC_Press3,&CV9Param::OnBnClickedPress1)

	ON_BN_CLICKED(IDCANCEL, &CV9Param::OnBnClickedCancel)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_BLOCKCLEAN, &CV9Param::OnBnClickedBtnBlockclean)
	ON_BN_CLICKED(IDC_BTN_UNION_SAVE_ALL, &CV9Param::OnBnClickedBtnUnionSaveAll)
END_MESSAGE_MAP()
// CV9Param message handlers
void CV9Param::OnBnClickedFroceM(UINT nID)
{
	static bool RunSt=false;
	if (RunSt)
	{
		return;
	}
	RunSt=true;
	int nHomeValue = 0;
	int nReturnValue = 0;
	int BtID=nID-IDC_BTV9MA;
	int WriteV9ID=BtID/6;//V9COMM ID
	int WBtID=BtID%6;//����
	CString str;
	double tempVal,tempVal2;
	double dFirstCompVol;
	double dCompVol;
	int tempInt;
	long tempLong;
	GetDlgItemText(IDC_PAMA1+6*WriteV9ID,str);
	tempVal=atof(str);
	GetDlgItemText(IDC_PAMA1+2+6*WriteV9ID,str);
	tempLong=atol(str);
	GetDlgItemText(IDC_PAMA27+WriteV9ID,str);
	dFirstCompVol=atof(str);
	GetDlgItemText(IDC_PAMA36+WriteV9ID,str);//2018-05-03 add by zwg
	dCompVol = atof(str);
	get_DispRestD[WriteV9ID]=g_pFrm->m_CmdRun.CapRestDistance(tempVal,dCompVol,tempLong,&get_dispcount[WriteV9ID],&get_Disppost[WriteV9ID],dFirstCompVol);

	switch (WBtID)
	{
	case 0://��λ
		GetDlgItemText(IDC_PAMA1+WriteV9ID*6+3,str);
		tempVal=atof(str);
		ButtonEnable(false);
		nReturnValue = theApp.m_V9Control.V9C_Rest(WriteV9ID+6,FALSE,tempVal,get_DispRestD[WriteV9ID]);
		theApp.m_V9Control.V9C_SetProfile();
		ButtonEnable(true);
		if(nReturnValue>0)
		{
			g_pFrm->m_CmdRun.Alarm(nReturnValue+5);
		}
		else if(nReturnValue<0)
		{
			g_pFrm->m_CmdRun.Alarm(-nReturnValue+1);
		}
		break;
	case 1://���
		GetDlgItemText(IDC_PAMA1+WriteV9ID*6+3,str);
		tempVal=atof(str);
		ButtonEnable(false);
		theApp.m_V9Control.V9C_Full(WriteV9ID+6,FALSE,FALSE,tempVal,get_DispRestD[WriteV9ID]);
		theApp.m_V9Control.V9C_SetProfile();
		ButtonEnable(true);
		break;
	case 2://����
		ButtonEnable(false);
		nHomeValue = theApp.m_V9Control.V9C_Home(WriteV9ID+6);
		theApp.m_V9Control.V9C_SetProfile();
		ButtonEnable(true);
		if(nHomeValue<0)
		{
			g_pFrm->m_CmdRun.Alarm(-nHomeValue+9);
		}
		else if(nHomeValue>0)
		{
			g_pFrm->m_CmdRun.Alarm(nHomeValue+1);
		}
		break;
	case 3://�㽺
		GetDlgItemText(IDC_PAMA1+WriteV9ID*6+1,str);
		tempVal2=atof(str);
		ButtonEnable(false);
		theApp.m_V9Control.V9C_Dot(WriteV9ID+6,false,false,get_DispRestD[WriteV9ID]);
		ButtonEnable(true);
		break;
	case 4://��ϴ
		GetDlgItemText(IDC_PAMA25,str);
		tempInt=atoi(str);
		GetDlgItemText(IDC_PAMA26,str);
		tempVal2=atof(str);
		ButtonEnable(false);
		//theApp.m_V9Control.V9C_Clearn(WriteV9ID+4,tempInt,tempVal2);
		g_pFrm->m_CmdRun.m_nCleanValveNum=WriteV9ID+6;
		g_pFrm->m_CmdRun.bIsExitClean=false;
		g_pFrm->m_CmdRun.CleanThreadPlay();
		theApp.m_V9Control.V9C_SetProfile();
		ButtonEnable(true);
		break;
	case 5://�������
		/*if(0==WriteV9ID)
		{
			SaveAllParam(WriteV9ID);
			OnBnClickedUniformParam();
			OnBnClickedSaveAll();
		}*/
		//else
		{
			SaveAllParam(WriteV9ID);
			theApp.V9Param(FALSE);
		}
		UpeditData();
		break;
	default:
		break;
	}
	RunSt=false;
}
// ������λ
void CV9Param::OnBnClickedResetAll()
{
	CString str;
	GetDlgItemText(IDC_EDIT_ALL_FULL_HEIGHT,str);
	double dRestD=atof(str);
	if((dRestD<=0)||(dRestD>56)) 
	{
		AfxMessageBox("��λ�߶������쳣��");
		return;
	}
	static bool  RunSt1=false;
	if (RunSt1)
	{
		return;
	}
	RunSt1=true;
	ButtonEnable(false);
	int nReturnValue = theApp.m_V9Control.V9C_Rest(K_ALL_AXIS,1,0,dRestD);
	theApp.m_V9Control.V9C_SetProfile();
	ButtonEnable(true);
	if(nReturnValue>0)
	{
		g_pFrm->m_CmdRun.Alarm(nReturnValue+5);
	}
	else if(nReturnValue<0)
	{
		g_pFrm->m_CmdRun.Alarm(-nReturnValue+5);
	}
	RunSt1=false;
}
// ��������
void CV9Param::OnBnClickedHomeAll()
{
	// TODO: Add your control notification handler code here
	static bool RunSt3=false;
	if (RunSt3)
	{
		return;
	}
	RunSt3=true;
	ButtonEnable(false);
	int nHomeReturn = theApp.m_V9Control.V9C_Home(K_ALL_AXIS);
	theApp.m_V9Control.V9C_SetProfile();
	ButtonEnable(true);
	if(nHomeReturn<0)
	{
		g_pFrm->m_CmdRun.Alarm(-nHomeReturn+9);
	}
	else if(nHomeReturn>0)
	{
		g_pFrm->m_CmdRun.Alarm(nHomeReturn+1);
	}
	RunSt3=false;
}
// �����㽺
void CV9Param::OnBnClickedDispensingAll()
{
	// TODO: Add your control notification handler code here
	static bool RunSt4=false;
	if (RunSt4)
	{
		return;
	}
	RunSt4=true;
	ButtonEnable(false);
	theApp.m_V9Control.V9C_Dot(K_ALL_AXIS,false);
	ButtonEnable(true);
	RunSt4=false;
}
// ������ϴ
void CV9Param::OnBnClickedClearAll()
{
	// TODO: Add your control notification handler code here
	static bool RunSt5=false;
	if (RunSt5)
	{
		return;
	}
	RunSt5=true;
	ButtonEnable(false);
	//theApp.m_V9Control.V9C_Clearn(K_ALL_AXIS,g_pFrm->m_CmdRun.V9Parm[0].CleanCount,g_pFrm->m_CmdRun.V9Parm[0].CleanSpeed);
	g_pFrm->m_CmdRun.m_nCleanValveNum=-1;
	g_pFrm->m_CmdRun.bIsExitClean=false;
	g_pFrm->m_CmdRun.CleanThreadPlay();
	theApp.m_V9Control.V9C_SetProfile();
	ButtonEnable(true);
	RunSt5=false;
}
// ����һ��
void CV9Param::OnBnClickedUniformParam()
{
	// TODO: Add your control notification handler code here
	long DispCount=g_pFrm->m_CmdRun.V9Parm[0].DispCount;
	for (int i=0;i<3;i++)
	{
		if (DispCount>g_pFrm->m_CmdRun.V9Parm[i].DispCount)
		{
			DispCount=g_pFrm->m_CmdRun.V9Parm[i].DispCount;
		}
	}
	//step2
	long DispNum=g_pFrm->m_CmdRun.V9Parm[0].DispNum;
	for (int i=0;i<3;i++)
	{
		if (DispNum>g_pFrm->m_CmdRun.V9Parm[i].DispNum)
		{
			DispNum=g_pFrm->m_CmdRun.V9Parm[i].DispNum;
		}

	}
	//step3
	if (DispNum>DispCount)
	{
		DispNum=DispCount;
	}
	//step4
	g_pFrm->m_CmdRun.V9Parm[0].DispCount=DispCount;
	g_pFrm->m_CmdRun.V9Parm[0].DispNum=DispNum;
	//step5
	double dCompF2,dCompF3;
	dCompF2=g_pFrm->m_CmdRun.V9Parm[1].CompVolume;
	dCompF3=g_pFrm->m_CmdRun.V9Parm[2].CompVolume;
	g_pFrm->m_CmdRun.V9Parm[1]=g_pFrm->m_CmdRun.V9Parm[0];
	g_pFrm->m_CmdRun.V9Parm[2]=g_pFrm->m_CmdRun.V9Parm[0];
	g_pFrm->m_CmdRun.V9Parm[1].CompVolume=dCompF2;
	g_pFrm->m_CmdRun.V9Parm[2].CompVolume=dCompF3;
	UpeditData();
}
// ���½�������
void CV9Param::UpeditData(void)
{
	CString   str;
	for (int i=0;i<3;i++)
	{
		str.Format("%0.3f",g_pFrm->m_CmdRun.V9Parm[i].DispVolume);
		SetDlgItemText(IDC_PAMA1+i*6,str);
		//GetDlgItem(IDC_PAMA1+i*6)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_GUEST);//��ҵԱȨ��
		GetDlgItem(IDC_PAMA1+i*6)->EnableWindow(true);//��ҵԱȨ��
		str.Format("%0.2f",g_pFrm->m_CmdRun.V9Parm[i].dispspeed);
		SetDlgItemText(IDC_PAMA1+i*6+1,str);
		GetDlgItem(IDC_PAMA1+i*6+1)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
		str.Format("%ld",g_pFrm->m_CmdRun.V9Parm[i].DispCount);
		SetDlgItemText(IDC_PAMA1+i*6+2,str);
		GetDlgItem(IDC_PAMA1+i*6+2)->EnableWindow(true);
		str.Format("%0.1f",g_pFrm->m_CmdRun.V9Parm[i].FullSpeed);
		SetDlgItemText(IDC_PAMA1+i*6+3,str);
		GetDlgItem(IDC_PAMA1+i*6+3)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
		str.Format("%0.3f",g_pFrm->m_CmdRun.V9Parm[i].BackVolume);
		SetDlgItemText(IDC_PAMA1+i*6+4,str);
		GetDlgItem(IDC_PAMA1+i*6+4)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
		str.Format("%0.1f",g_pFrm->m_CmdRun.V9Parm[i].BackSpeed);
		SetDlgItemText(IDC_PAMA1+i*6+5,str);
		GetDlgItem(IDC_PAMA1+i*6+5)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
		str.Format("%0.3f",g_pFrm->m_CmdRun.V9Parm[i].ForwardVol);
		SetDlgItemText(IDC_PAMA19+i,str);
		GetDlgItem(IDC_PAMA19+i)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
		str.Format("%0.3f",g_pFrm->m_CmdRun.V9Parm[i].FirstCompVol);
		SetDlgItemText(IDC_PAMA27+i,str);
		GetDlgItem(IDC_PAMA27+i)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
		str.Format("%0.3f",g_pFrm->m_CmdRun.V9Parm[i].StartVel);
		SetDlgItemText(IDC_PAMA22+i,str);
		GetDlgItem(IDC_PAMA22+i)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
		str.Format("%0.3f",g_pFrm->m_CmdRun.V9Parm[i].TestVol);
		SetDlgItemText(IDC_PAMA30+i,str);
		GetDlgItem(IDC_PAMA30+i)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
		str.Format("%ld",g_pFrm->m_CmdRun.V9Parm[i].DispNum);
		SetDlgItemText(IDC_PAMA33+i,str);
		GetDlgItem(IDC_PAMA33+i)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
		str.Format("%0.3f",g_pFrm->m_CmdRun.V9Parm[i].dHeightFullALL);
		SetDlgItemText(IDC_EDIT_ALL_FULL_HEIGHT,str);
		str.Format("%0.3f",g_pFrm->m_CmdRun.V9Parm[i].CompVolume);//20180503 add by zwg
		SetDlgItemText(IDC_PAMA36+i,str);
		GetDlgItem(IDC_PAMA36+i)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);

		GetDlgItem(IDC_BTV9MA6)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
		GetDlgItem(IDC_BTV9MA12)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
		GetDlgItem(IDC_BTV9MA18)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	}
	str.Format("%d",g_pFrm->m_CmdRun.V9Parm[0].CleanCount);
	SetDlgItemText(IDC_PAMA25,str);                             //2017-11-18 Ȩ���޸�
	//GetDlgItem(IDC_PAMA25)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	str.Format("%0.1f",g_pFrm->m_CmdRun.V9Parm[0].CleanSpeed);
	SetDlgItemText(IDC_PAMA26,str);                             //2017-11-18 Ȩ���޸�
	//GetDlgItem(IDC_PAMA26)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
}
// ��ʼ��
BOOL CV9Param::OnInitDialog()
{
	CDialog::OnInitDialog();
	UpeditData();
	g_pFrm->m_CmdRun.V9RestOK=false;
	g_pFrm->IsV9SetDlgVisible = true;
	return TRUE;  // return TRUE unless you set the focus to a control
}
// �������в���
bool CV9Param::SaveAllParam(int ParamID)
{
	double mt_dispVolume;
	long mt_dispCount;
	double  mt_Fullspeed;
	double mt_ClearnCount; 
	double  mt_ClearnSpeed;
	double mt_compVol;//first comp
	double mt_compVolAll;//all comp
	double mt_ForwardVol;
	double mt_TestVol;
	long mt_dispNum;
	CString str;
	GetDlgItemText(IDC_PAMA1+ParamID*6,str);
	mt_dispVolume=atof(str);
	if(g_pFrm->m_CmdRun.V9Parm[ParamID].DispVolume!=mt_dispVolume)
	{
		str.Format("��%d �������ģ�֮ǰ��%0.3f;���£�%0.3f...",(ParamID+1),g_pFrm->m_CmdRun.V9Parm[ParamID].DispVolume,mt_dispVolume);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
	}
	g_pFrm->m_CmdRun.V9Parm[ParamID].DispVolume=mt_dispVolume;

	GetDlgItemText(IDC_PAMA1+ParamID*6+1,str);
	g_pFrm->m_CmdRun.V9Parm[ParamID].dispspeed=atof(str);//�ٶȺ�ʱ��һ��

	GetDlgItemText(IDC_PAMA1+ParamID*6+2,str);
	mt_dispCount=atol(str);
	g_pFrm->m_CmdRun.V9Parm[ParamID].DispCount=mt_dispCount;

	GetDlgItemText(IDC_PAMA1+ParamID*6+3,str);
	mt_Fullspeed=atof(str);
	g_pFrm->m_CmdRun.V9Parm[ParamID].FullSpeed=mt_Fullspeed;

	GetDlgItemText(IDC_PAMA1+ParamID*6+4,str);
	mt_ClearnCount=atof(str);
	g_pFrm->m_CmdRun.V9Parm[ParamID].BackVolume=mt_ClearnCount;

	GetDlgItemText(IDC_PAMA1+ParamID*6+5,str);
	mt_ClearnSpeed=atof(str);
	g_pFrm->m_CmdRun.V9Parm[ParamID].BackSpeed=mt_ClearnSpeed;

	GetDlgItemText(IDC_PAMA19+ParamID,str);
	mt_ForwardVol = atof(str);
	if(mt_ForwardVol<0)
	{
		mt_ForwardVol = 0;
	}
	else if(mt_ForwardVol>mt_dispVolume)
	{
		mt_ForwardVol = mt_dispVolume;
	}
	g_pFrm->m_CmdRun.V9Parm[ParamID].ForwardVol=mt_ForwardVol;

	GetDlgItemText(IDC_PAMA27+ParamID,str);
	mt_compVol = atof(str);
	g_pFrm->m_CmdRun.V9Parm[ParamID].FirstCompVol=mt_compVol;

	GetDlgItemText(IDC_PAMA22+ParamID,str);
	g_pFrm->m_CmdRun.V9Parm[ParamID].StartVel = atof(str);

	GetDlgItemText(IDC_PAMA30+ParamID,str);
	mt_TestVol = atof(str);
	if(mt_TestVol<0)
	{
		mt_TestVol = 0;
	}
	else if(mt_TestVol>mt_dispVolume)
	{
		mt_TestVol = mt_dispVolume;
	}
	g_pFrm->m_CmdRun.V9Parm[ParamID].TestVol = mt_TestVol;

	GetDlgItemText(IDC_PAMA33+ParamID,str);
	mt_dispNum = atoi(str);
	if(mt_dispCount<mt_dispNum)
	{
		mt_dispNum = mt_dispCount;
		str.Format("%ld",mt_dispNum);
		SetDlgItemText(IDC_PAMA33+ParamID,str);
	}
	g_pFrm->m_CmdRun.V9Parm[ParamID].DispNum = mt_dispNum;
    
	GetDlgItemText(IDC_PAMA36+ParamID,str);
	mt_compVolAll = atof(str);
	g_pFrm->m_CmdRun.V9Parm[ParamID].CompVolume=mt_compVolAll;//20180503 add by zwg
	g_pFrm->m_CmdRun.V9Parm[ParamID].RestDistance=g_pFrm->m_CmdRun.CapRestDistance(mt_dispVolume,mt_compVolAll,mt_dispCount,&g_pFrm->m_CmdRun.V9Parm[ParamID].DispCount,&g_pFrm->m_CmdRun.V9Parm[ParamID].DispDistance,mt_compVol);
	return true;
}
// �������в���
void CV9Param::OnBnClickedSaveAll()
{
	CString str;
	for (int i=0;i<3;i++)
	{
		//SaveAllParam(i);
		GetDlgItemText(IDC_PAMA25,str);	
		g_pFrm->m_CmdRun.V9Parm[i].CleanCount=atol(str);
		GetDlgItemText(IDC_PAMA26,str);	
		g_pFrm->m_CmdRun.V9Parm[i].CleanSpeed=atof(str);
		GetDlgItemText(IDC_EDIT_ALL_FULL_HEIGHT,str);
		g_pFrm->m_CmdRun.V9Parm[i].dHeightFullALL=atof(str);
	}
	/*g_pFrm->m_CmdRun.V9Parm[1]=g_pFrm->m_CmdRun.V9Parm[0];
    g_pFrm->m_CmdRun.V9Parm[2]=g_pFrm->m_CmdRun.V9Parm[0];*/
	UpeditData(); //update all parameter on UI;
	theApp.V9Param(FALSE);
	UpeditData();
}
// �༭��
void CV9Param::OnEnChangeEdit1(UINT nID)
{
	int BtID=nID-IDC_PAMA1;
	int WriteV9ID=BtID/6;//V9COMM ID
	int WBtID=BtID%6;//����
	double tempVal;
	long tempVal2;
	double tempDouble;
	double tempResult;
	double tempCompVol;
	CString str;
	double Maxdisp=(theApp.m_tSysParam.DisEndDistance-theApp.m_tSysParam.DispFreeDistance)*theApp.m_tSysParam.Dispratios;

	switch (WBtID)
	{
	case 0://�½���
	case 2://���̴���
		GetDlgItemText(IDC_PAMA1+6*WriteV9ID,str);
		tempVal=atof(str);
		if (WBtID==0)
		{
			tempVal2=10000000;
		}
		else
		{  
			GetDlgItemText(IDC_PAMA1+2+6*WriteV9ID,str);
			tempVal2=atol(str);
		}
		GetDlgItemText(IDC_PAMA36+WriteV9ID,str);//���ϲ���  20180503 add by zwg
		tempCompVol=atof(str);

		tempResult=g_pFrm->m_CmdRun.CapRestDistance(tempVal,tempCompVol,tempVal2,&get_dispcount[WriteV9ID],&get_Disppost[WriteV9ID]);
		if (tempResult<-2.5)
		{
			str.Format("%0.3f",Maxdisp);
			SetDlgItemText(IDC_PAMA1+0+6*WriteV9ID,str);
		}
		else
		{
			get_DispRestD[WriteV9ID]= tempResult;
			str.Format("%ld",get_dispcount[WriteV9ID]);
			SetDlgItemText(IDC_PAMA1+2+6*WriteV9ID,str);
		}
		break;
	case 1://�½��ٶ�
		GetDlgItemText(IDC_PAMA1+1+6*WriteV9ID,str);
		tempVal2=atol(str);
		if (tempVal2>theApp.m_tSysParam.DispMaxSpeed)
		{ 
			str.Format("%0.1f",theApp.m_tSysParam.DispMaxSpeed);
			SetDlgItemText(IDC_PAMA1+1+6*WriteV9ID,str);
		}
		break;
	case 3://����ٶ�
		GetDlgItemText(IDC_PAMA1+3+6*WriteV9ID,str);
		tempVal2=atol(str);
		if (tempVal2>theApp.m_tSysParam.DispMaxSpeed)
		{ 
			str.Format("%0.1f",theApp.m_tSysParam.DispMaxSpeed);
			SetDlgItemText(IDC_PAMA1+3+6*WriteV9ID,str);
		}
		break;
	case 4://��������
		GetDlgItemText(IDC_PAMA1+4+6*WriteV9ID,str);
		tempVal=atof(str);
		GetDlgItemText(IDC_PAMA1+6*WriteV9ID,str);
		tempDouble=atof(str);
		if (tempVal>tempDouble)
		{
			SetDlgItemText(IDC_PAMA1+4+6*WriteV9ID,str);
		}
		break;
	case 5://�����ٶ�
		GetDlgItemText(IDC_PAMA1+5+6*WriteV9ID,str);
		tempVal=atof(str);
		if (tempVal>theApp.m_tSysParam.DispMaxSpeed)
		{ 
			str.Format("%0.1f",theApp.m_tSysParam.DispMaxSpeed);
			SetDlgItemText(IDC_PAMA1+5+6*WriteV9ID,str);
		}
		break;
	default:
		break;
	}
	GetDlgItemText(nID,str);
	int len=str.GetLength();
	((CEdit *)GetDlgItem(nID))->SetSel(len,len+5,TRUE);
}
// ����/�ط�
void CV9Param::OnBnClickedPress1(UINT nID)
{
	int btID=nID-IDC_Press1;
	static bool ForeOut=FALSE;
	ForeOut=!ForeOut;
	if(ForeOut)
	{
		if(btID<3)
		{
			theApp.m_V9Control.V9C_Defoam(K_AXIS_U+btID);
		}
		else
		{
			theApp.m_V9Control.V9C_Defoam(K_ALL_AXIS);
		}
		theApp.m_V9Control.V9C_SetProfile();
	}
	else
	{
		if (btID<3)
		{
			theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[btID],ForeOut);
			theApp.m_V9Control.V9C_Rest(K_AXIS_U+btID);
		}
		else
		{
			for (int i=0;i<3;i++)
			{
				theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[i],ForeOut);
			}
			theApp.m_V9Control.V9C_Rest(K_ALL_AXIS);
		}
		theApp.m_V9Control.V9C_SetProfile();
	}
	// TODO: Add your control notification handler code here
}
// ȡ��
void CV9Param::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	for (int i=0;i<3;i++)
	{
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[i],false);
	}
	if(AfxMessageBox("��Ҫ:�豸�����ı䣬�Ƿ񱣴�?",MB_YESNO) == IDYES)
	{
		OnBnClickedSaveAll(); //�������С�
	}
	g_pFrm->IsV9SetDlgVisible = false;
	OnCancel();
}
// �˳�
void CV9Param::OnV9SetExit()
{
	OnBnClickedCancel();
}
// �ر�
void CV9Param::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnClose();
}

void CV9Param::OnEnFirstCompensation(UINT nID)
{
	int nNum = nID - IDC_PAMA27;
	CString str;
	double dTempVal,dTempMaxVal;
	double dCompVol;
	long lDisCount;
	GetDlgItemText(nID,str);
	dTempVal = atof(str);
	switch(nNum)
	{
	case 0:
		GetDlgItemText(IDC_PAMA1,str);
		dTempMaxVal=atof(str);
		GetDlgItemText(IDC_PAMA3,str);
		lDisCount=atol(str);
		GetDlgItemText(IDC_PAMA36,str); //����
		dCompVol=atof(str);
		break;
	case 1:
		GetDlgItemText(IDC_PAMA7,str);
		dTempMaxVal=atof(str);
		GetDlgItemText(IDC_PAMA9,str);
		lDisCount=atol(str);
		GetDlgItemText(IDC_PAMA37,str); //����
		dCompVol=atof(str);
		break;
	case 2:
		GetDlgItemText(IDC_PAMA13,str);
		dTempMaxVal=atof(str);
		GetDlgItemText(IDC_PAMA15,str);
		lDisCount=atol(str);
		GetDlgItemText(IDC_PAMA38,str); //����
		dCompVol=atof(str);
		break;
	default:
		return;
	}
	if(dTempVal>dTempMaxVal)
	{
		dTempVal = dTempMaxVal;
		str.Format("%0.3f",dTempMaxVal);
		SetDlgItemText(nID,str);
	}
	get_DispRestD[nNum]=g_pFrm->m_CmdRun.CapRestDistance(dTempMaxVal,dCompVol,lDisCount,&get_dispcount[nNum],&get_Disppost[nNum],dTempVal);
	str.Format("%ld",get_dispcount[nNum]);
	SetDlgItemText(IDC_PAMA1+2+6*nNum,str);
}

void CV9Param::ButtonEnable(bool bEnable)
{
	GetDlgItem(IDC_BTV9MA)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTV9MA2)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTV9MA3)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTV9MA4)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTV9MA5)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTV9MA6)->EnableWindow(bEnable&&theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
    GetDlgItem(IDC_Press1)->EnableWindow(bEnable);

	GetDlgItem(IDC_BTV9MA7)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTV9MA8)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTV9MA9)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTV9MA10)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTV9MA11)->EnableWindow(bEnable);
	GetDlgItem(IDC_Press2)->EnableWindow(bEnable);

	GetDlgItem(IDC_BTV9MA13)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTV9MA14)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTV9MA15)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTV9MA16)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTV9MA17)->EnableWindow(bEnable);
	GetDlgItem(IDC_Press3)->EnableWindow(bEnable);

	GetDlgItem(IDC_RESET_ALL)->EnableWindow(bEnable);//Reset combind
	GetDlgItem(IDC_HOME_ALL)->EnableWindow(bEnable);//Home combind
	GetDlgItem(IDC_DISPENSING_ALL)->EnableWindow(bEnable);//Dispense combind
	GetDlgItem(IDC_CLEAR_ALL)->EnableWindow(bEnable);//Clear combined

	GetDlgItem(IDC_SAVE_ALL)->EnableWindow(bEnable);//mutex assure UI fluent;
	GetDlgItem(IDC_BTV9MA12)->EnableWindow(bEnable&&theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);//mutex assure UI fluent;
	GetDlgItem(IDC_BTV9MA18)->EnableWindow(bEnable&&theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);//mutex assure UI fluent;
	GetDlgItem(IDC_BTN_UNION_SAVE_ALL)->EnableWindow(bEnable);

}
void CV9Param::OnBnClickedBtnBlockclean()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.CleanSuspend();
	g_pFrm->m_CmdRun.bIsExitClean=true;
	g_pFrm->m_CmdRun.m_pMv->WaitStop(K_AXIS_U,10000);
	g_pFrm->m_CmdRun.m_pMv->WaitStop(K_AXIS_W,10000);
	g_pFrm->m_CmdRun.m_pMv->WaitStop(K_AXIS_V,10000);

	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[0],FALSE);//����
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[1],FALSE);//����	
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[2],FALSE);//����	
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[0],TRUE);//����
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[1],TRUE);//����
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[2],TRUE);//����
}


void CV9Param::OnBnClickedBtnUnionSaveAll()
{
	CString str;
	long DispCount;//single
	long DispNum;  //disp 
	/////////////////////////////////////////////////////////
	for (int i=0;i<3;i++)
	{
		GetDlgItemText(IDC_PAMA25,str);	
		g_pFrm->m_CmdRun.V9Parm[i].CleanCount=atol(str);
		GetDlgItemText(IDC_PAMA26,str);	
		g_pFrm->m_CmdRun.V9Parm[i].CleanSpeed=atof(str);
	}
	////////////////////////////////////////////////////////
	double dCompF1,dCompF2,dCompF3;
	GetDlgItemText(IDC_PAMA36,str);
	dCompF1=atof(str);
	GetDlgItemText(IDC_PAMA37,str);
	dCompF2=atof(str);
	GetDlgItemText(IDC_PAMA38,str);
	dCompF3=atof(str);
	SaveAllParam(0);
	//dCompF2=g_pFrm->m_CmdRun.V9Parm[1].CompVolume;
	//dCompF3=g_pFrm->m_CmdRun.V9Parm[2].CompVolume;
	if(g_pFrm->m_CmdRun.V9Parm[1].DispVolume!=g_pFrm->m_CmdRun.V9Parm[0].DispVolume)
	{
		str.Format("��2 �������ģ�֮ǰ��%0.3f;���£�%0.3f...",g_pFrm->m_CmdRun.V9Parm[1].DispVolume,g_pFrm->m_CmdRun.V9Parm[0].DispVolume);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
	}
	if(g_pFrm->m_CmdRun.V9Parm[2].DispVolume!=g_pFrm->m_CmdRun.V9Parm[0].DispVolume)
	{
		str.Format("��3 �������ģ�֮ǰ��%0.3f;���£�%0.3f...",g_pFrm->m_CmdRun.V9Parm[2].DispVolume,g_pFrm->m_CmdRun.V9Parm[0].DispVolume);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
	}
	g_pFrm->m_CmdRun.V9Parm[1]=g_pFrm->m_CmdRun.V9Parm[0];
	g_pFrm->m_CmdRun.V9Parm[2]=g_pFrm->m_CmdRun.V9Parm[0];
	g_pFrm->m_CmdRun.V9Parm[1].CompVolume=dCompF2;
	g_pFrm->m_CmdRun.V9Parm[2].CompVolume=dCompF3;


	UpeditData();
	for (int i=0;i<3;i++)
	{
		SaveAllParam(i);
	}

	////ȡ��ֵ
	//step1:
	DispCount=g_pFrm->m_CmdRun.V9Parm[0].DispCount;
	for (int i=0;i<3;i++)
	{
		if (DispCount>g_pFrm->m_CmdRun.V9Parm[i].DispCount)
		{
			DispCount=g_pFrm->m_CmdRun.V9Parm[i].DispCount;
		}
	}
	//step2
	DispNum=g_pFrm->m_CmdRun.V9Parm[0].DispNum;
	for (int i=0;i<3;i++)
	{
		if (DispNum>g_pFrm->m_CmdRun.V9Parm[i].DispNum)
		{
			DispNum=g_pFrm->m_CmdRun.V9Parm[i].DispNum;
		}

	}
	//step3
	if (DispNum>DispCount)
	{
		DispNum=DispCount;
	}
	//step4
	for (int i=0;i<3;i++)
	{
		g_pFrm->m_CmdRun.V9Parm[i].DispCount=DispCount;
		g_pFrm->m_CmdRun.V9Parm[i].DispNum=DispNum;
	}

	UpeditData(); //update all parameter on UI;
	theApp.V9Param(FALSE);
	UpeditData();
}

