#include "StdAfx.h"
#include "ComPort.h"
#include "TSCtrlSys.h"
#include "Function.h"

CEvent g_Comexit(0,1);
CEvent g_commevent(1);
CComPort::CComPort(void)
{
}

CComPort::~CComPort(void)
{
}

long CComPort::ReadPlcData(CString PLCDataID)
{
	CString Sendstr1,Sendstr2;
	CString Sendstr;
	int TempVal;
	CString AccpStr,AccpTempStr1,AccpTempStr2;
	CString  DataDT1;
	Sendstr1="%01#RDD";
	TempVal=atoi(PLCDataID.GetBuffer(0))+1;
	DataDT1.Format("%d",TempVal);
	Sendstr2=AddZero(PLCDataID,5)+AddZero(DataDT1,5);//数据DT串
	Sendstr=Sendstr1+Sendstr2+"**"+char(13);
	if (NULL != m_Port[0].m_hComm)
	{
		m_Port[0].WriteToPort(Sendstr);
		Sleep(50);
		if (m_Port[0].m_chnum > 0)
		{
			AccpStr.Format("%s",m_Port[0].m_chbuf);
			AccpStr = AccpStr.Left(m_Port[0].m_chnum);
			m_Port[0].m_chnum = 0;
			return ChangStrTOlong(AccpStr);
		}
		else return -1;
	}
	else
		return -1;
}

long CComPort::ChangStrTOlong(CString ChangStr)
{
	CString DataStr,DataStr1,DataStr2;
	CString temStr1,temStr2;
	long    AccpVal;
	if (ChangStr.GetLength()<14)//断定是否接收正常
	{
		return -1;
	}
	DataStr1=ChangStr.Mid(6,4);
	DataStr2=ChangStr.Mid(10,4);
	temStr1=DataStr1.Mid(0,2);
	temStr2=DataStr1.Mid(2,2);
	DataStr1=temStr2+temStr1;//两位对换
	temStr1=DataStr2.Mid(0,2);
	temStr2=DataStr2.Mid(2,2);
	DataStr2=temStr2+temStr1;
	DataStr=DataStr2+DataStr1;
	sscanf(DataStr,"%X",&AccpVal);//十六进制转换
	return AccpVal;
}

CString CComPort::AddZero(CString str, int StrCout)
{
	int Strleng;
	CString tempstr;
	if(str.GetLength()<StrCout)
	{
		Strleng=StrCout-str.GetLength();
		for (int i=0;i<Strleng;i++)
		{
			str=_T("0")+str;
		}
	}
	return str;
}

BOOL CComPort::WritePlcYOut(CString OutID, bool on_off)
{

	CString  stron_off,strSendPLC;
	OutID=AddZero(OutID,4);
	if (on_off)
	{
		stron_off=_T("1**");
	}
	else
		stron_off=_T("0**");
	strSendPLC=_T("%01#WCSY")+OutID+stron_off+char(13);
	m_Port[0].WriteToPort(strSendPLC);
	return false;
}

BOOL CComPort::WritePlcROut(CString OutID, bool on_off)
{
	CString  stron_off,strSendPLC;
	OutID=AddZero(OutID,4);
	if (on_off)
	{
		stron_off=_T("1**");
	}
	else
		stron_off=_T("0**");
	strSendPLC=_T("%01#WCSR")+OutID+stron_off+char(13);
	m_Port[0].WriteToPort(strSendPLC);
	return false;
}

BOOL CComPort::WritePLCData(CString DTID, long DateVal)
{
	CString DTID1,strdata,strSendPLC;
	CString  SendDa1,SendDa2,SendDa3,SendDa4;
	CString  SendData;
	int TempVal;
	TempVal=atoi(DTID.GetBuffer(0))+1;
	DTID1.Format("%d",TempVal);
	DTID=AddZero(DTID,5);
	DTID1=AddZero(DTID1,5);
	////////////////////////////
	strdata.Format(_T("%X"),DateVal);//16进制
	strdata= AddZero(strdata,8);
	SendDa1=strdata.Mid(6,2);
	SendDa2=strdata.Mid(4,2);
	SendDa3=strdata.Mid(2,2);
	SendDa4=strdata.Mid(0,2);
	SendData=SendDa1+SendDa2+SendDa3+SendDa4;
	strSendPLC=_T("%01#WDD")+DTID+DTID1+SendData+_T("**")+char(13);
	//strSendPLC=_T("%01#WDD01000010011A270011**")+char(13);
	//%01#WDD01000010011A270000**
	CommSendPlc(strSendPLC);
	return true;
}

BOOL CComPort::CommSendPlc(CString StrPlc)
{
	if (NULL != m_Port[0].m_hComm)
	{
		m_Port[0].WriteToPort(StrPlc);
		return true;
	}
	else
		return false;
}

double CComPort::ReadData(char *string,int nPortID,DeviceType deviceType)
{
	CString AccpStr;
	ClearCommPort(nPortID);
	if(NULL!=m_Port[nPortID].m_hComm)
	{
		if(deviceType==KEYENCE_IL)
		{
			m_Port[nPortID].WriteToPort(string);
			Sleep(50);
			if(m_Port[nPortID].m_chnum>0)
			{
				AccpStr.Format("%s",m_Port[nPortID].m_chbuf);
				AccpStr = AccpStr.Left(m_Port[nPortID].m_chnum);
				AccpStr = AccpStr.Right(m_Port[nPortID].m_chnum-AccpStr.Find(",")-1);
				m_Port[nPortID].m_chnum = 0;
				return atof(AccpStr);
			}
			else
			{
				return -1.0;
			}
		}
		else if(deviceType==PANASONIC_HL)
		{
			m_Port[nPortID].WriteToPort(string);
			Sleep(50);
			if (m_Port[nPortID].m_chnum > 0)
			{
				AccpStr.Format("%s",m_Port[nPortID].m_chbuf);

				if(g_pFrm!=NULL)		g_pFrm->m_CmdRun.AddMsg(AccpStr);

				AccpStr = AccpStr.Left(m_Port[nPortID].m_chnum);
				m_Port[nPortID].m_chnum = 0;
				AccpStr = AccpStr.Mid(7,8);

				if(g_pFrm!=NULL)		g_pFrm->m_CmdRun.AddMsg(AccpStr);
				
				return atoi(AccpStr)*0.0001;
			}
		}
		else if(deviceType==MLD_HS)
		{
			m_Port[nPortID].WriteToPort(string);
			Sleep(50);
			if (m_Port[nPortID].m_chnum > 0)
			{
				AccpStr.Format("%s",m_Port[nPortID].m_chbuf);
				if(g_pFrm!=NULL)
				{
					g_pFrm->m_CmdRun.AddMsg(AccpStr);
				}
				AccpStr = AccpStr.Left(m_Port[nPortID].m_chnum);
				m_Port[nPortID].m_chnum = 0;
				AccpStr = AccpStr.Mid(1,AccpStr.GetLength()-2);
				if(g_pFrm!=NULL)
				{
					g_pFrm->m_CmdRun.AddMsg(AccpStr);
				}
				return atof(AccpStr)-30.0;
			}
		}
	}
	return -1.0;
}

int CComPort::ReadPlcRData(CString PLCRDataID)
{
	CString Sendstr1,Sendstr2;
	CString Sendstr;
	CString AccpStr,AccpTempStr1,AccpTempStr2;
	CString  DataDT1;
	Sendstr1="%01#RCSR";
	Sendstr2=AddZero(PLCRDataID,4);//数据DT串
	Sendstr=Sendstr1+Sendstr2+"**"+char(13);
	if (NULL != m_Port[0].m_hComm)
	{
		ClearCommPort(0);
		m_Port[0].WriteToPort(Sendstr);
		Sleep(50);
		if (m_Port[0].m_chnum > 0)
		{
			AccpStr.Format("%s",m_Port[0].m_chbuf);
			g_pFrm->m_CmdRun.AddMsg(AccpStr);
			AccpStr = AccpStr.Left(m_Port[0].m_chnum);
			m_Port[0].m_chnum = 0;
			g_pFrm->m_CmdRun.AddMsg(AccpStr.Mid(6,1));
			return atoi(AccpStr.Mid(6,1));
		}
	}
	return -1;
}

bool CComPort::ReadBalance(double *pData,int nPortID,bool *bStable)
{
	CString str;
	static bool b = false;
	if(b)
	{
		return false;
	}
	b = true;//在回零中不响应回零
	CString strReceive,strResult,strSign,strValue,strUnit;
	int nLength = 20;
	*bStable = false;
	if(NULL==m_Port[nPortID].m_hComm)
	{
		b = false;
		return false;
	}
	else
	{
		if(m_Port[nPortID].m_chnum>0)
		{
			CString strTemp(m_Port[nPortID].m_chbuf);
			strReceive.Format("%s",strTemp);
			//g_pFrm->m_CmdRun.AddMsg(strReceive);
			int nStart = 0;
			int nEnd = 0;
			CString strResult;
			if((nStart=strReceive.Find("SUI"))>=0||(nStart=strReceive.Find("CU1"))>=0)
			{
				nLength = 20;
			}
			else if((nStart=strReceive.Find("SI"))>=0||(nStart=strReceive.Find("SU"))>=0||(nStart=strReceive.Find("C1"))>=0)
			{
				nLength = 19;
			}
			else if((nStart=strReceive.Find("S"))>=0)
			{
				nLength = 18;
			}
			else
			{
				m_Port[nPortID].m_chnum = 0;
				memset(m_Port[nPortID].m_chbuf,0,1024);
				b = false;
				return false;
			}
			nEnd = strReceive.Find("\r",nStart+1);
			if(nEnd<0)
			{
				m_Port[nPortID].m_chnum = 0;
				memset(m_Port[nPortID].m_chbuf,0,1024);
				b = false;
				return false;
			}
			str.Format("Length:%d",nEnd-nStart);
			g_pFrm->m_CmdRun.AddMsg(str);
			if(nEnd-nStart==nLength)
			{
				strResult = strReceive.Mid(nStart,nLength);
				g_pFrm->m_CmdRun.AddMsg(strResult);
				strSign = strReceive.Mid(nStart+6,1);
				//g_pFrm->m_CmdRun.AddMsg(strSign);
				strUnit = strReceive.Mid(nStart+17,3);
				//g_pFrm->m_CmdRun.AddMsg(strUnit);
				strValue = strReceive.Mid(nStart+7,9);
				//g_pFrm->m_CmdRun.AddMsg(strValue);
				if(strResult.Find("?")>=0||strResult.Find("^")>=0||strResult.Find("v")>=0)//天平不稳定
				{
					*bStable = false;
				}
				else
				{
					*bStable = true;
				}
				if(strUnit.Find("mg")>=0)
				{
					if(strSign == "-")
					{
						*pData = atof(strValue)*(-1);
					}
					else
					{
						*pData = atof(strValue);
					}
				}
				else if(strUnit.Find("g")>=0)
				{
					if(strSign == "-")
					{
						*pData = atof(strValue)*1000*(-1);
					}
					else
					{
						*pData = atof(strValue)*1000;
					}
				}
				else
				{
					m_Port[nPortID].m_chnum = 0;
					memset(m_Port[nPortID].m_chbuf,0,1024);
					b = false;
					return false;
				}
				m_Port[nPortID].m_chnum = 0;
				memset(m_Port[nPortID].m_chbuf,0,1024);
				b = false;
				return true;
			}
			else
			{
				m_Port[nPortID].m_chnum = 0;
				memset(m_Port[nPortID].m_chbuf,0,1024);
				b = false;
				return false;
			}
		}
	}
	b = false;
	return false;
}

bool CComPort::ReadBalance(double *pData,bool *bStable)
{
	char chSendData[4];
	chSendData[0] = 0x1B;
	chSendData[1] = 0x50;
	chSendData[2] = 0x0D;
	chSendData[3] = 0x0A;
	m_Port[1].WriteToPort(chSendData,4);

	CString strReceive,strResult,strSign,strValue,strUnit;
	if(NULL!=m_Port[1].m_hComm)
	{
		Sleep(50);
		if(m_Port[1].m_chnum>0)
		{
			CString strTemp(m_Port[1].m_chbuf);
			strReceive.Format("%s",strTemp);
			g_pFrm->m_CmdRun.AddMsg(strReceive);
			int nStart = 0;
			int nEnd = 0;
			double dScale = 1.0;
			nEnd = strReceive.Find("\r",nStart+1);
			if(nEnd>14)
			{
				nStart = nEnd-14;
			}
			if(nEnd>=0&&nEnd<14)
			{
				nStart = nEnd+2;
				nEnd = strReceive.Find("\r",nStart+1);
			}
			if(nEnd-nStart==14)
			{
				strSign = strReceive.Mid(nStart,1);
				strValue = strReceive.Mid(nStart+2,8);
				strUnit = strReceive.Mid(nStart+11,3);
				if(strUnit.Find("mg")>=0||strUnit.Find("g")>=0)
				{
					*bStable = true;
				}
				if(strUnit.Find("mg")<0&&strUnit.Find("g")>=0)
				{
					dScale = 1000;
				}
				if(strSign == "-")
				{
					*pData = atof(strValue)*(-1)*dScale;
				}
				else
				{
					*pData = atof(strValue)*dScale;
				}
				m_Port[1].m_chnum = 0;
				memset(m_Port[1].m_chbuf,0,1024);
				return true;
			}
			else
			{
				m_Port[1].m_chnum = 0;
				memset(m_Port[1].m_chbuf,0,1024);
				return false;
			}
		}
		return false;
	}
	return false;
}

bool CComPort::SetBand(int nPortID)
{
	char chSendData[16];
	chSendData[0] = 0x02;
	chSendData[1] = 0x42;
	chSendData[2] = 0x49;
	chSendData[3] = 0x54;
	chSendData[4] = 0x5F;
	chSendData[5] = 0x52;
	chSendData[6] = 0x41;
	chSendData[7] = 0x54;
	chSendData[8] = 0x45;
	chSendData[9] = 0x20;
	chSendData[10] = 0x33;
	chSendData[11] = 0x38;
	chSendData[12] = 0x2E;
	chSendData[13] = 0x34;
	chSendData[14] = 0x5F;
	chSendData[15] = 0x03;
	m_Port[nPortID].WriteToPort(chSendData,16);
	return true;
}

// 清零指令
void CComPort::BalanceZero(int nPortID)
{
	//char chSendData[3];
	//chSendData[0] = 0x5A;
	//chSendData[1] = 0x0D;
	//chSendData[2] = 0x0A;
	//m_Port[nPortID].WriteToPort(chSendData,3);
	char chSendData[4];
	chSendData[0] = 0x1B;
	chSendData[1] = 0x56;
	chSendData[2] = 0x0D;
	chSendData[3] = 0x0A;
	m_Port[nPortID].WriteToPort(chSendData,4);
}

void CComPort::BalanceTare(int nPortID)
{
	//char chSendData[3];
	//chSendData[0] = 0x54;
	//chSendData[1] = 0x0D;
	//chSendData[2] = 0x0A;
	//m_Port[nPortID].WriteToPort(chSendData,3);
	char chSendData[4];
	chSendData[0] = 0x1B;
	chSendData[1] = 0x54;
	chSendData[2] = 0x0D;
	chSendData[3] = 0x0A;
	m_Port[nPortID].WriteToPort(chSendData,4);
}

void CComPort::BalanceCal(int nPortID)
{
	char chSendData[4];
	chSendData[0] = 0x1B;
	chSendData[1] = 0x5A;
	chSendData[2] = 0x0D;
	chSendData[3] = 0x0A;
	m_Port[nPortID].WriteToPort(chSendData,4);
}

void CComPort::BalanceResultImmediateBU(int nPortID)
{
	char chSendData[4];
	chSendData[0] = 0x53;
	chSendData[1] = 0x49;
	chSendData[2] = 0x0D;
	chSendData[3] = 0x0A;
	m_Port[nPortID].WriteToPort(chSendData,4);
}

void CComPort::BalanceResultStableBU(int nPortID)
{
	char chSendData[3];
	chSendData[0] = 0x54;
	chSendData[1] = 0x0D;
	chSendData[2] = 0x0A;
	m_Port[nPortID].WriteToPort(chSendData,3);
}

void CComPort::BalanceResultImmediateCU(int nPortID)
{
	char chSendData[5];
	chSendData[0] = 0x53;
	chSendData[1] = 0x55;
	chSendData[2] = 0x49;
	chSendData[3] = 0x0D;
	chSendData[4] = 0x0A;
	m_Port[nPortID].WriteToPort(chSendData,5);
}

void CComPort::BalanceResultStableCU(int nPortID)
{
	char chSendData[4];
	chSendData[0] = 0x53;
	chSendData[1] = 0x55;
	chSendData[2] = 0x0D;
	chSendData[3] = 0x0A;
	m_Port[nPortID].WriteToPort(chSendData,4);
}

void CComPort::BalanceStartTransmissionBU(int nPortID)
{
	char chSendData[4];
	chSendData[0] = 0x43;
	chSendData[1] = 0x31;
	chSendData[2] = 0x0D;
	chSendData[3] = 0x0A;
	m_Port[nPortID].WriteToPort(chSendData,4);
}

void CComPort::BalanceFinishTransmissionBU(int nPortID)
{
	char chSendData[4];
	chSendData[0] = 0x43;
	chSendData[1] = 0x30;
	chSendData[2] = 0x0D;
	chSendData[3] = 0x0A;
	m_Port[nPortID].WriteToPort(chSendData,4);
}

void CComPort::BalanceStartTransmissionCU(int nPortID)
{
	char chSendData[5];
	chSendData[0] = 0x43;
	chSendData[1] = 0x55;
	chSendData[2] = 0x31;
	chSendData[3] = 0x0D;
	chSendData[4] = 0x0A;
	m_Port[nPortID].WriteToPort(chSendData,5);
}

void CComPort::BalanceFinishTransmissionCU(int nPortID)
{
	char chSendData[5];
	chSendData[0] = 0x43;
	chSendData[1] = 0x55;
	chSendData[2] = 0x30;
	chSendData[3] = 0x0D;
	chSendData[4] = 0x0A;
	m_Port[nPortID].WriteToPort(chSendData,5);
}

void CComPort::ClearCommPort(int nPortID)
{
	m_Port[nPortID].m_chnum = 0;
	memset(m_Port[nPortID].m_chbuf,0,1024);
}

bool CComPort::ReadPlcData(long *data,CString strType,CString strNum,CString strFormat)
{
	CString strText;
	CString strAccp;
	*data = 0;
	strText.Format("RD");
	strText = strText+" "+strType+strNum+strFormat+char(13);
	if(NULL != m_Port[0].m_hComm)
	{
		ClearCommPort(0);
		m_Port[0].WriteToPort(strText);
		Sleep(50);
		if (m_Port[0].m_chnum > 0)
		{
			strAccp.Format("%s",m_Port[0].m_chbuf);
			g_pFrm->m_CmdRun.AddMsg(strAccp);
			m_Port[0].m_chnum = 0;
			*data = atol(strAccp);
			return true;
		}
	}
	return false;
}

bool CComPort::WritePlcData(long data,CString strType,CString strNum,CString strFormat)
{
	CString strText;
	CString strData;
	if(strFormat==".H")
	{
		strData.Format("%X",data);
	}
	else
	{
		strData.Format("%ld",data);
	}
	strText.Format("WR");
	strText = strText+" "+strType+strNum+strFormat+" "+strData+char(13);
	if(!CommSendPlc(strText))
	{
		return false;
	}
	return true;
}

bool CComPort::ConnectPlc(CString &str)
{
	char ch[3];
	ch[0] = 0x43;
	ch[1] = 0x52;
	ch[2] = 0x0D;
	if(NULL != m_Port[0].m_hComm)
	{
		ClearCommPort(0);
		m_Port[0].WriteToPort(ch,3);
		Sleep(50);
		if (m_Port[0].m_chnum > 0)
		{
			str.Format("%s",m_Port[0].m_chbuf);
			g_pFrm->m_CmdRun.AddMsg(str);
			str = str.Left(2);
			if(str=="CC")
			{
				return true;
			}
		}
	}
	return false;
}

bool CComPort::DisconnectPlc(CString &str)
{
	char ch[3];
	ch[0] = 0x43;
	ch[1] = 0x51;
	ch[2] = 0x0D;
	if(NULL != m_Port[0].m_hComm)
	{
		ClearCommPort(0);
		m_Port[0].WriteToPort(ch,3);
		Sleep(50);
		if (m_Port[0].m_chnum > 0)
		{
			str.Format("%s",m_Port[0].m_chbuf);
			g_pFrm->m_CmdRun.AddMsg(str);
			str = str.Left(2);
			if(str=="CF")
			{
				return true;
			}
		}
	}
	return false;
}

////按指定格式发送数据MES
//// 格式：%B12.689#
bool CComPort::SendGlueToMES(double dGlue)
{
	CString str0,strsend;
	str0.Format("%0.3f",dGlue);
	str0=AddZero(str0,6);
	strsend=_T("%B")+str0+_T("#");
	//strSendPLC=_T("%01#WDD01000010011A270011**")+char(13);
	//%01#WDD01000010011A270000**
	if (NULL != m_Port[1].m_hComm)
	{
		m_Port[1].WriteToPort(strsend);
		CString str;
		str.Format("--->ToMes__CMD:当前胶量:%0.3f...",dGlue);
		g_pFrm->m_CmdRun.AddMsg(str);
		//g_pFrm->m_pLog->log(str);
		return true;
	}
	else
		return false;
}

bool CComPort::SendHaltToMES()
{
	CString strsend;
	strsend=_T("%DSTOP#");
	//strSendPLC=_T("%01#WDD01000010011A270011**")+char(13);
	//%01#WDD01000010011A270000**
	if (NULL != m_Port[1].m_hComm)
	{
		m_Port[1].WriteToPort(strsend);
		return true;
	}
	else
		return false;

}
