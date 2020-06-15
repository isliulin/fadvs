#include "StdAfx.h"
#include "EncryptOp.h"
#include "TSCtrlSys.h"
#define ENCRYPT_EXPORTS  

#define  DefRegeditInfoPass  "SOFTWARE\\TenSunCtrl"   // ע���д��·��



#pragma comment(lib,"iphlpapi")
#include <SensAPI.h>      //IsNetworkAlive
#pragma comment(lib,"Sensapi")
#include <Wininet.h>    //InternetGetConnectedState
#pragma comment(lib,"Wininet")
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi")



CEncryptOp::CEncryptOp(void)
{
	m_bLockSystem = FALSE;
	CString tmpStr = CFunction::GetDirectory();
   	m_sAllFile[0] = _T("C:\\Control\\Sysparam.dll");	
	m_sAllFile[1] = tmpStr + "\\product\\log4cpp_MD_VC80.MMF";
	m_sAllFile[2] = tmpStr + "\\GenApi_MD_VC80.mmf";
	m_sAllFile[3] = tmpStr + "\\system\\LogFile.xml";
	m_sAllFile[4] = tmpStr + "\\product\\GenApi_MD_VC100.mod";//Default\\AppData\\Local\\Microsoft\\Windows
	m_sAllFile[5] = tmpStr + "\\GCBase_MD_VC80.dll";
	//m_sAllFile[6] = _T("SOFTWARE\\TENSUN\\L1");
	//m_sAllFile[7] = _T("SOFTWARE\\TENSUN\\L2");
	m_sAllFile[6] = _T("SOFTWARE\\Microsoft\\Windows NT\\CDROW\\CurrenVersion\\TileDataModel\\NetworkLocationWizard\\66\\1\\1\\1\\1");
	m_sAllFile[7] = _T("SOFTWARE\\Microsoft\\Windows\\Shell\\Bags\\66\\Shell\\1\\1\\1\\1");
	m_sAllFile[8] = _T("SOFTWARE\\TENSUN\\License");

	SHDeleteKey(HKEY_CURRENT_USER,_T(DefRegeditInfoPass)); //ɾ���ɵ�ע���
	CreatePakcetFile();
	//m_StrDrive = GetDriverCode();

}

void CEncryptOp::Init()
{
	HRESULT hRet;
	try
	{
		hRet = CoInitializeEx(0,COINIT_MULTITHREADED);
		hRet = CoInitializeSecurity(
			NULL,
			-1,
			NULL,
			NULL,
			RPC_C_AUTHN_LEVEL_DEFAULT,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			NULL,
			EOAC_NONE,
			NULL
			);
		hRet = CoCreateInstance(
			CLSID_WbemLocator,
			0,
			CLSCTX_INPROC_SERVER,
			IID_IWbemLocator,
			(LPVOID *)&pLoc);
		hRet = pLoc->ConnectServer(
			_bstr_t(L"ROOT\\CIMV2"),
			NULL,
			NULL,
			0,
			NULL,
			0,
			0,
			&pSvc
			);
		hRet = CoSetProxyBlanket(
			pSvc,
			RPC_C_AUTHN_WINNT,
			RPC_C_AUTHZ_NONE,
			NULL,
			RPC_C_AUTHN_LEVEL_CALL,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			NULL,
			EOAC_NONE
			);
		m_StrDrive = GetDriverCode();
		m_AlarmTime = "10:00:00";
	}
	catch (CException* e)
	{
	}
}

void CEncryptOp::DeMD5()
{
	if(NULL != pSvc)
	{
		pSvc->Release();
	}
	if(NULL!=pLoc)
	{
		pLoc->Release();
	}
	CoUninitialize();
	m_Adapters.clear();
}


CEncryptOp::~CEncryptOp(void)
{
	
}

BOOL CEncryptOp::CreatePakcetFile()
{
	if(!PathIsDirectory(CFunction::GetDirectory()))//�ж�·���Ƿ����
	{
		CreateDirectory(CFunction::GetDirectory(),NULL);//�½��ļ���
	}
	if(!PathIsDirectory(CFunction::GetDirectory() + "product"))//�ж�·���Ƿ����
	{
		CreateDirectory(CFunction::GetDirectory() + "product",NULL);//�½��ļ���
	}
	if(!PathIsDirectory(CFunction::GetDirectory() + "system"))//�ж�·���Ƿ����
	{
		CreateDirectory(CFunction::GetDirectory() + "system",NULL);//�½��ļ���
	}
	if(!PathIsDirectory("C:\\Control"))//�ж�·���Ƿ����
	{
		CreateDirectory("C:\\Control",NULL);//�½��ļ���
	}

	
	for (int i = 0; i < REGFILENUM - 3; i++)
	{
		if (!PathFileExists(m_sAllFile[i]))
		{
			CFile str(m_sAllFile[i], CFile::modeReadWrite | CFile::modeCreate);
			str.Flush();
			str.Close();
		}
		//SetFileAttributes(m_sAllFile[i], FILE_ATTRIBUTE_HIDDEN);
		SetFileAttributes(m_sAllFile[i], FILE_ATTRIBUTE_NORMAL);
	}

	return TRUE;
}

// ��ע����ж�������ֵ
CRegisterCode CEncryptOp::IsInstalledPro()
{

	//��ȡ������Ϣ
	GetAllOFRegisterCode();


	//�ȼ��Flag
	int nRegisterFlagNum = -1, nAllDaysFlagNum = -1, UInit = -1, nInstallNum = -1, nRCLengthNum = -1, strLimit = -1, nAllDaysNum = -1, Init = -1; 
	int CompareStrLimitdate = -1, CompareStrRegistDate = -1;//���޴���ע����
	int NeedIndex[8] = {0};
	int Index = 0;
	int Index1 = 0;
	int InitIndex[8] = {0};
	for (int i = 0; i < 8; i++)
	{
		//Flag
		if (registerCodes[i].m_bRegisterFlag)
		{
			nRegisterFlagNum++;
			if (registerCodes[i].m_nCurIdAllDays > 365 * 10 - 1)
			{
				nAllDaysFlagNum++;
			}
		}

		//Install
		if (registerCodes[i].m_bInstalled)
		{
			nInstallNum++;
		}

		//ע����
		if (registerCodes[i].m_sRegisterCode.GetLength() <= 0) //˵��û��ֵ--���޸Ļ����ļ���ʼ
		{
			nRCLengthNum++;
		}
		else if (registerCodes[i].m_sRegisterCode.GetLength() > 16)//˵����ע��׶λ��ߴ����ַ�
		{
			UInit++;
		}
		else if (!registerCodes[i].m_sRegisterCode.Compare("Init"))//˵���ǳ�ʼ���׶�
		{
			Init++;
			if (registerCodes[i].m_stLimitData.GetLength() == 10)
			{ 
				//CString LSStr = registerCodes[i].m_dtLastTime.Format(_T("%Y-%m-%d %H:%M:%S"));
				//CString LTStr = registerCodes[i].m_dtLimitTime.Format(_T("%Y-%m-%d %H:%M:%S"));

				if (registerCodes[i].m_dtLimitTime > COleDateTime::GetCurrentTime() && COleDateTime::GetCurrentTime() > registerCodes[i].m_dtLastTime)
				{
					NeedIndex[Index] = i;//����������������ڵ�����
					Index++;
				}
			}
		}
		
		//���޴�
		if (registerCodes[i].m_stLimitData.GetLength() <= 0)
		{
			strLimit++;
		}
		


		//�ϴ�ʱ�� �� ����ʱ��
		int j = i + 1;
		
		if (registerCodes[i].m_nCurIdAllDays != registerCodes[j].m_nCurIdAllDays)
		{
			nAllDaysNum++;
		}
		
		if (registerCodes[i].m_stLimitData != (registerCodes[j].m_stLimitData))
		{
			CompareStrLimitdate++;
		}
		
		if (registerCodes[i].m_sRegisterCode != (registerCodes[j].m_sRegisterCode))
		{
			CompareStrRegistDate++;
		}

		
	}                     

	//-
	int RetIndex = 0,  RetIndex1 = 0;
	if(Index != 8)
	{
		Index1 = 0;
		
		for(int i = 0; i < Index; i++)
		{

			CString LTStr = registerCodes[NeedIndex[i]].m_dtLimitTime.Format(_T("%Y-%m-%d"));
			if (!registerCodes[NeedIndex[i]].m_stLimitData.Compare(LTStr))
			{
				InitIndex[Index1] = NeedIndex[i];
				Index1++;
			}		
		}
		RetIndex1 = InitIndex[0];
		RetIndex = InitIndex[0];
		for(int i = 0; i < Index1; i++)
		{
			for(int j = i + 1; j < Index1; j++)
			{
				if(registerCodes[i].m_dtLimitTime > registerCodes[InitIndex[j]].m_dtLimitTime)
				{
					RetIndex1 = j;
				}
				if(registerCodes[RetIndex].m_dtLimitTime > registerCodes[RetIndex1].m_dtLimitTime)
				{
					RetIndex = RetIndex1;
				}

			}

		}
	}
	


	

	//�쳣
	
	//��ʼ��-----�ļ���ɾ��

	if (nRCLengthNum >= 5 && strLimit >= 5 && (UInit > -1 || Init > -1)) //6���ļ���ȡ���ִ���Ϊ�գ���ע����ȡ��ע���볤��С��5�����³�ʼ��
	{
		InitRegeditInfo(registerCodes[RetIndex]);
		return registerCodes[RetIndex];
	}
	if (Init > -1 && UInit < 7 && nInstallNum < 7) //�С�init������,//��ʼ���׶�
	{
		if  (CompareStrRegistDate > 0 || CompareStrLimitdate > 0 || nAllDaysNum > 0)
		{
			if(!registerCodes[6].m_sRegisterCode.CompareNoCase("Init") || !registerCodes[7].m_sRegisterCode.CompareNoCase("Init"))
			{
				InitRegeditInfo(registerCodes[RetIndex]);
				return registerCodes[RetIndex];
			}
		}
		
	}
	//else	//ע��׶�
	//{
	if ((nRegisterFlagNum > -1 && nAllDaysFlagNum == -1) ||  (nRegisterFlagNum == -1 && nAllDaysFlagNum > -1))
	{
		CRegisterCode retC = RetWtrite();
		return retC;
	}
	else if (CompareStrRegistDate > 0 || CompareStrLimitdate > 0 || nAllDaysNum > 0)
	{
		CRegisterCode retC = RetWtrite();
		return retC;
	}

	//}

	srand((unsigned)time(NULL));
	int No = rand()%7 + 1;
	return registerCodes[No];   
}

/// <summary>
/// ����-��д
/// </summary>
/// <returns></returns>
CRegisterCode CEncryptOp::RetWtrite()
{

	//GetAllOFRegisterCode();
	BOOL CompareFlag = FALSE;
	BOOL SetMsgFlag = FALSE;
	//CString StrDriverCode;
	CString LimitStr;
	//StrDriverCode = GetDriverCode();
	for (int i = 0; i < 8; i++)
	{
		if (registerCodes[i].m_stLimitData.GetLength() > 0)
		{
			LimitStr.Format("%s", registerCodes[i].m_stLimitData);
		}
		if (registerCodes[i].m_sRegisterCode.GetLength() == 16)
		{
			CString ret = RepairPassWord(m_StrDrive, registerCodes[i].m_sRegisterCode);
			if (!ret.Compare(LimitStr))
			{
				CompareFlag = TRUE;
			}

		}

		if (CompareFlag)
		{
			SetAllOFRegisterCode(registerCodes[i]);
			GetAllOFRegisterCode();
			SetMsgFlag = TRUE;
			CompareFlag = FALSE;
			return registerCodes[i];
		}

	}

	if (!SetMsgFlag)
	{
		CString str;
		str = CString(theApp.m_tSysParam.RegCode);
		//registerCodes[0].m_sDeviceCode = m_StrDrive;
		//registerCodes[0].m_sRegisterCode = str;
		if (CalcPassWord(m_StrDrive, str, registerCodes[0]) == 0)
		{
			RegistInfo(registerCodes[0]);
			GetAllOFRegisterCode();
			return registerCodes[0];
		}
		else
		{
			registerCodes[0].m_dtLimitTime = COleDateTime::GetCurrentTime();
			registerCodes[0].m_dtLastTime = COleDateTime::GetCurrentTime();
			m_bLockSystem = TRUE;
			InitRegeditInfo(registerCodes[0]);
		}
	}
	
	GetAllOFRegisterCode();
	return registerCodes[0];


}

/// <summary>
/// ����-��д
/// </summary>
/// <returns></returns>
CRegisterCode CEncryptOp::RetWtrite(int Index[])
{

	//GetAllOFRegisterCode();
	bool CompareFlag = false;
	bool SetMsgFlag = false;
	CString LimitStr;
	//len = sizeof(str)/sizeof(str[0]); // ���鳤��
	for (int i = 0; i < sizeof(Index)/sizeof(Index[0]); i++)
	{
		if (registerCodes[Index[i]].m_stLimitData.GetLength() > 0)
		{
			LimitStr = registerCodes[Index[i]].m_stLimitData;

			if (registerCodes[Index[i]].m_sRegisterCode.GetLength() == 16)
			{
				CString ret = RepairPassWord(m_StrDrive, registerCodes[Index[i]].m_sRegisterCode);
				if (!ret.CompareNoCase(LimitStr))
				{
					COleDateTime dt;
					BOOL ret = dt.ParseDateTime(LimitStr + " " + m_AlarmTime, LOCALE_NOUSEROVERRIDE);
					registerCodes[Index[i]].m_dtLimitTime = dt;
					CompareFlag = true;
				}

			}
		}

		if (CompareFlag)
		{
			SetAllOFRegisterCode(registerCodes[Index[i]]);
			GetAllOFRegisterCode();
			SetMsgFlag = true;
			CompareFlag = false;
			return registerCodes[Index[i]];
		}

	}

	if (!SetMsgFlag)
	{
		CString str;
		str =  CString(theApp.m_tSysParam.RegCode);
		registerCodes[Index[0]].m_sDeviceCode = m_StrDrive;
		registerCodes[Index[0]].m_sRegisterCode.Format("%s",str);
		if (CalcPassWord(m_StrDrive, str, registerCodes[Index[0]]) == 0)
		{
			RegistInfo(registerCodes[Index[0]]);
			GetAllOFRegisterCode();
			return registerCodes[Index[0]];
		}
		else
		{
			//string ret = RepairPassWord(StrDriverCode, str);
			registerCodes[Index[0]].m_dtLimitTime = COleDateTime::GetCurrentTime(); //DateTime.ParseExact(LimitDate + AlarmTime, "yyyyMMddHHmmss", System.Globalization.CultureInfo.CurrentCulture);
			registerCodes[Index[0]].m_dtLastTime = COleDateTime::GetCurrentTime();
			m_bLockSystem = true;
			return registerCodes[Index[0]];
		}
	}

	GetAllOFRegisterCode();
	return registerCodes[Index[0]];


}



// ��ע����ж�������ֵ
CRegisterCode CEncryptOp::IsInstalled()
{

	//��ȡ������Ϣ
	GetAllOFRegisterCode();

	int kinds = 7; //�������

	int ObjIndex[7][8];//û�в�����null����
	CStringList Strlist;
	CString strObjFlag[7][3];//��ѯ���ƣ��������

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < kinds; j++)//9-->8
		{
			strObjFlag[j][0] = "";//��ͬ������
			strObjFlag[j][1] = "";//False��ʽ��ͬ
			strObjFlag[j][2] = "";//True��ʽ��ͬ
			ObjIndex[j][i] = 0;
		}
	}
	//DateTime afterDT1 = System.DateTime.Now;
	//TimeSpan ts1 = afterDT1.Subtract(beforDT);
	//Console.WriteLine("DateTime�ܹ�����{0}ms.", ts1.TotalMilliseconds);
	//CString 
	for(int i = 0; i < 8; i ++)
	{

		if (!registerCodes[i].m_sDeviceCode.CompareNoCase(m_StrDrive))
		{
			ObjIndex[0][i] = 1;
		}

		for(int j = i + 1; j < 8;j ++)
		{         

			if (registerCodes[i].m_nCurIdAllDays == (registerCodes[j].m_nCurIdAllDays))
			{
				if (registerCodes[i].m_nCurIdAllDays >= 0)
				{
					ObjIndex[1][i] = 1;
					ObjIndex[1][j] = 1;
				}
				
			}

			if (!registerCodes[i].m_sRegisterCode.CompareNoCase(registerCodes[j].m_sRegisterCode))
			{
				if (!registerCodes[i].m_sRegisterCode.CompareNoCase(""))
				{
					ObjIndex[2][i] = 1;
					ObjIndex[2][j] = 1;
				}
				
			
			}

			if (!registerCodes[i].m_stLimitData.CompareNoCase(registerCodes[j].m_stLimitData))
			{
				if (!registerCodes[i].m_stLimitData.CompareNoCase(""))
				{
					ObjIndex[3][i] = 1;
					ObjIndex[3][j] = 1;
				}
			}

			if (registerCodes[i].m_bRegisterFlag == (registerCodes[j].m_bRegisterFlag))
			{
				if (!registerCodes[i].m_bRegisterFlag)
				{
					ObjIndex[4][i] = 1;
					ObjIndex[4][j] = 1;
				}
				else
				{
					if (registerCodes[i].m_LimitDays > 365 * 10 - 1 && registerCodes[j].m_LimitDays > 365 * 10 - 1)
					{
						ObjIndex[4][i] = 2; //2�����Ѿ�ע��--��Ҫ�ж��Ƿ���ע��
						ObjIndex[4][j] = 2;
					}

				}

			}

			if (registerCodes[i].m_bInstalled == (registerCodes[j].m_bInstalled))
			{
				ObjIndex[5][i] = 1;
				ObjIndex[5][j] = 1;
			}

			if (registerCodes[i].m_LimitDays == (registerCodes[j].m_LimitDays))
			{
				if (registerCodes[i].m_LimitDays > 0)
				{
					ObjIndex[6][i] = 1;
					ObjIndex[6][j] = 1;
				}
			}

		}
	}

	CString str;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < kinds; j++) 
		{
			str.Format("%d%d", ObjIndex[j][i], i);
			strObjFlag[j][ObjIndex[j][i]] += str;
			//strObjFlag[j][ObjIndex[j][i]] += (ObjIndex[j][i].ToString() + i.ToString());
		}
	}

	for (int m = 0; m < kinds; m++)
	{
		if (m == 4)
		{
			if (strObjFlag[m][2].GetLength() == 16)//�Ѿ�ע��
			{
				int RegIndex = _ttoi(strObjFlag[m][2].Mid(1, 1));
				return registerCodes[RegIndex];
			}
			else if (strObjFlag[m][0].GetLength() > 0 && strObjFlag[m][2].GetLength() > 0) //�����˱��޸�
			{
				int RegIndex = _ttoi(strObjFlag[m][2].Mid(1, 1));
				return registerCodes[RegIndex];
			}
		}

		//����
		if (strObjFlag[0][1].GetLength() == 0 )//���㸴�����ģ�������Ҳ�᲻һ����
		{
			registerCodes[0].m_dtLimitTime = registerCodes[0].m_dtLastTime;
			InitRegeditInfo(registerCodes[0]);
			return registerCodes[0];
		}
		else if (strObjFlag[m][0].GetLength() > 0)
		{

			int OKObjleng = strObjFlag[m][1].GetLength();
			int OkObjIndex[7];
			for (int i = 1; i < OKObjleng; i += 2)
			{
				OkObjIndex[i / 2] = _ttoi(strObjFlag[m][1].Mid(i, 1));//������һ������������ȷ�ģ����Կ�ȡ��
			}
			for (int i = 0; i < OKObjleng / 2; i++)
			{

				if (registerCodes[OkObjIndex[i]].m_dtLastTime >= registerCodes[OkObjIndex[i]].m_dtLimitTime) //ԭ�����Ѿ�����
				{
					return registerCodes[OkObjIndex[i]];
				}
				if (!registerCodes[OkObjIndex[i]].m_sRegisterCode.CompareNoCase("Init"))
				{
					if (registerCodes[OkObjIndex[i]].m_dtLimitTime > COleDateTime::GetCurrentTime() && COleDateTime::GetCurrentTime() > registerCodes[OkObjIndex[i]].m_dtLastTime) //����ʹ�õ�
					{
						str = registerCodes[OkObjIndex[i]].m_dtLimitTime.Format("%Y-%m-%d");//Format("%Y-%m-%d %H:%M:%S");
						if (!str.CompareNoCase(registerCodes[OkObjIndex[i]].m_stLimitData))
						{
							if (registerCodes[OkObjIndex[i]].m_LimitDays >= registerCodes[OkObjIndex[i]].m_nCurIdAllDays)
							{
								//ȡ��Сʱ��
								for (int j = 0; j < OKObjleng / 2; j++)
								{
									if (registerCodes[OkObjIndex[i]].m_dtLimitTime > registerCodes[OkObjIndex[j]].m_dtLimitTime)
									{
										registerCodes[OkObjIndex[i]].m_dtLimitTime = registerCodes[OkObjIndex[j]].m_dtLimitTime;
									}
								}
								InitRegeditInfo(registerCodes[OkObjIndex[i]]);
								if (registerCodes[OkObjIndex[i]].m_LimitDays < registerCodes[OkObjIndex[i]].m_nCurIdAllDays)
								{
									registerCodes[OkObjIndex[i]].m_dtLastTime = COleDateTime::GetCurrentTime();
								}
								return registerCodes[OkObjIndex[i]];
							}

						}
					}
				}
				else
				{
					registerCodes[OkObjIndex[i]] = RetWtrite(OkObjIndex);
					//registerCodes[OkObjIndex[i]] = RetWtrite();
					return registerCodes[OkObjIndex[i]];
				}
			}
		}
	}

	srand((unsigned)time(NULL));
	int No = rand()%7 + 1;
	return registerCodes[No];   
}

BOOL  CEncryptOp::InitRegeditInfo(CRegisterCode Msg)
{
	COleDateTime InitTime = COleDateTime::GetCurrentTime();


	//InitTime.ParseDateTime(CString("20-04-18 09:30:00"));
	
	CRegisterCode Init;

	COleDateTimeSpan  timespan(14, 0, 0, 0);
	Init.m_LimitDays = timespan.GetDays();

	CString MsgLimitStr = Msg.m_dtLimitTime.Format(_T("%Y-%m-%d %H:%M:%S"));
	CString MsgLastStr = Msg.m_dtLastTime.Format(_T("%Y-%m-%d %H:%M:%S"));
	if (!MsgLimitStr.Compare(MsgLastStr))//˵���ǳ�ʼ
	{
		Init.m_dtLimitTime = Init.m_dtLastTime + timespan;
	}
	else
	{
		Init.m_dtLastTime = Msg.m_dtLastTime;
		Init.m_dtLimitTime = Msg.m_dtLimitTime;
	}
	CString LimitStr = Init.m_dtLimitTime.Format(_T("%Y-%m-%d"));
	Init.m_stLimitData.Format("%s", LimitStr);
	//int year = Init.m_dtLimitTime.GetYear();
	//int day = Init.m_dtLimitTime.GetDay();
	//int Hour = Init.m_dtLimitTime.GetHour();
	
	Init.m_bInstalled = false;
	Init.m_bRegisterFlag = false;
	Init.m_sDeviceCode = m_StrDrive;
	Init.m_sRegisterCode = "Init";
	
	timespan = Init.m_dtLimitTime - Init.m_dtLastTime;
	if (timespan.m_span > 0)
	{
		Init.m_nCurIdAllDays = timespan.GetDays();//����
	}
	else
	{
		Init.m_nCurIdAllDays = 0;
	}
	
	//if (theApp.m_EncryptOp.m_bOneKeyLock)
	//{
	//	Init.m_bOneKeyLock = true;
	//}


	
	if (SetAllOFRegisterCode(Init))
	{
		GetAllOFRegisterCode();
		return true;
	}
	return FALSE;
}

BOOL  CEncryptOp::RegistInfo(CRegisterCode strCode)
{

	COleDateTimeSpan timeSpan = strCode.m_dtLimitTime - COleDateTime::GetCurrentTime();
	int nAllDays = timeSpan.GetDays();


	if (nAllDays > 365*10)//����10��
	{
		//strCode.m_bOneKeyLock = false;
		strCode.m_bRegisterFlag = true;
		SetAllOFRegisterCode(strCode);
	}
	else
	{

		strCode.m_bRegisterFlag = false;
		strCode.m_bInstalled = true;
		strCode.m_nCurIdAllDays = nAllDays;
		strCode.m_LimitDays = nAllDays;
		SetAllOFRegisterCode(strCode);

	}

    return TRUE;
}

CString  CEncryptOp::RepairPassWord(CString strDeviceCode, CString strCode)
{
	CString tLimitData = "", retLimitData = "";
	CString tDriverCode = strDeviceCode;//���»�����
	MD5Encrypt(tDriverCode, 16, strDeviceCode);
	CString ThirdCode = strDeviceCode.Mid(0, 4);//������MD5�����λ����

	CString tStrRecevCode = strCode.Mid(0, 14);//��֤ע�����Ƿ����У��
	RegisterCodeDataCheck(tStrRecevCode);
	//����żУ��
	if(tStrRecevCode.CompareNoCase(strCode.Mid(14, 2)))//������ַ���compare����ֵ����
	{
		return retLimitData;
	}

	//�����
	strCode = strCode.Mid(0, 14);
	CString tmpCstr = tDriverCode + "134268";//134268C9A5
	char *Hexstr1 = strCode.GetBuffer(strCode.GetLength());
	char *Hexstr2 = tmpCstr.GetBuffer(tmpCstr.GetLength());

	hexstrxor(Hexstr1, Hexstr2, tStrRecevCode);//�����

	CStringList list;
	CString TmpStr = "", TmpStr1 = "";
	Y_Move(tStrRecevCode, list);
	if (!list.IsEmpty())
	{
		POSITION position;
		for(int i = 0; i < list.GetCount(); i++)
		{
			position = list.FindIndex(i);
			TmpStr = list.GetAt(position);

			TmpStr1 = TmpStr.Mid(0, 12);
			RegisterCodeDataCheck(TmpStr1);
			if (TmpStr1 != TmpStr.Mid(12, 2))
			{
				continue;
			}
			else
			{
				StringAnalysis(TmpStr.Left(12), TmpStr1, tLimitData);
				hexstrxor(tDriverCode.GetBuffer(tDriverCode.GetLength()), tLimitData.GetBuffer(tLimitData.GetLength()), retLimitData);

				if (!ThirdCode.Compare(TmpStr1))
				{
					retLimitData.Insert(4, '-');
					retLimitData.Insert(7, '-');
					return retLimitData;
				}
			}

		}
	}
	return retLimitData;
}



int  CEncryptOp::CalcPassWord(CString strDeviceCode, CString strCode, CRegisterCode &RegMsg)
{
	try
	{
		CString str;
		CString tLimitData = "", retLimitData = "";
		CString tDriverCode = strDeviceCode;//���»�����
		MD5Encrypt(tDriverCode, 16, strDeviceCode);
		CString ThirdCode = strDeviceCode.Mid(0, 4);//������MD5�����λ����

		CString tStrRecevCode = strCode.Mid(0, 14);//��֤ע�����Ƿ����У��
		RegisterCodeDataCheck(tStrRecevCode);
		//����żУ��
		if(tStrRecevCode.CompareNoCase(strCode.Mid(14, 2)))//������ַ���compare����ֵ����
		{
			str.Format("%s", "ע�����������ע����");
			//AfxMessageBox(str);
			g_pFrm->m_CmdRun.m_pLog->log(str);
			return 1;
		}

		//�����
		strCode = strCode.Mid(0, 14);
		CString tmpCstr = tDriverCode + "134268";//134268C9A5
		char *Hexstr1 = strCode.GetBuffer(strCode.GetLength());
		char *Hexstr2 = tmpCstr.GetBuffer(tmpCstr.GetLength());

		hexstrxor(Hexstr1, Hexstr2, tStrRecevCode);//�����
		CStringList list;
		CString TmpStr = "", TmpStr1 = "";
		Y_Move(tStrRecevCode, list);
		if (!list.IsEmpty())
		{
			POSITION position;
			for(int i = 0; i < list.GetCount(); i++)
			{
				position = list.FindIndex(i);
				TmpStr = list.GetAt(position);

				TmpStr1 = TmpStr.Mid(0, 12);
				RegisterCodeDataCheck(TmpStr1);
				if (TmpStr1.Compare(TmpStr.Mid(12, 2)) != 0)
				{
					continue;
				}
				else
				{
					StringAnalysis(TmpStr.Left(12), TmpStr1, tLimitData);
					hexstrxor(tDriverCode.GetBuffer(tDriverCode.GetLength()), tLimitData.GetBuffer(tLimitData.GetLength()), retLimitData);
					if (!ThirdCode.Compare(TmpStr1))
					{
						COleDateTimeSpan tSpan;
						COleDateTime dlimit;
						tSpan = COleDateTime::GetCurrentTime() - RegMsg.m_dtLastTime;
						if (tSpan.m_span > 0)
						{
							retLimitData.Insert(4, '-');
							retLimitData.Insert(7, '-');
							RegMsg.m_stLimitData = retLimitData;
							//BOOL ret = dlimit.ParseDateTime(/*"093000" + */tLimitData, LOCALE_NOUSEROVERRIDE,LANG_USER_DEFAULT );
							BOOL ret = dlimit.ParseDateTime(retLimitData + " " +  m_AlarmTime, LOCALE_NOUSEROVERRIDE);

							tSpan = dlimit - COleDateTime::GetCurrentTime();
							if (tSpan.m_span > 0)
							{
								RegMsg.m_dtLimitTime = dlimit;
								RegMsg.m_LimitDays = tSpan.GetTotalDays();

								str.Format("%s", "0-----------------------------------------0");
								g_pFrm->m_CmdRun.m_pLog->log(str);
								return 0;
							}
							else
							{
								//ʧ��
								str.Format("%s", "ע��ʧ�ܣ�����ʱ��С�ڵ�ǰʱ��");
								//AfxMessageBox(str);
								g_pFrm->m_CmdRun.m_pLog->log(str);
								return 2;
							}
						}
						else
						{
							//��ǰʱ��С���ϴγ���ر�ʱ��
							str.Format("%s", "��ǰʱ�����ϴγ���ر�ʱ��ǰ");
							//AfxMessageBox(str);
	
							g_pFrm->m_CmdRun.m_pLog->log(str);
							return 3;
						}



					}
				}

			}
		}

		//ʧ��
		str.Format("%s", "ע��������ǰʱ��С���ϴι���ʱ��");
		//AfxMessageBox(str);
		g_pFrm->m_CmdRun.m_pLog->log(str);
		return -1;
	}
	catch(int e)
	{
		return e;
	}
}



//��ȡCPUID
BOOL CEncryptOp::getCpu(CString &str)
{
	//��ȡCPU���к�
	//char szCPUID[100];
	char szTmp[100];
	unsigned long s1 = 0, s2=0;
	//int i=0;
	//FILE* wF = NULL;
	__asm
	{
		mov eax, 01h
			xor edx, edx
			cpuid
			mov s1, edx
			mov s2, eax
	}
	sprintf(szTmp, "%08X%08X", s1, s2);
	//strcpy(szCPUID, szTmp);
	//printf("CPUID is:%s\n", szCPUID);
	str = szTmp;
	return TRUE;
}
// ��ȡUUID
BOOL CEncryptOp::GetCPUUniquelyIdentifies(CString &str)
{
	
	bool bRet = false;
	IEnumWbemClassObject *pEnumerator = NULL;
	try
	{
		HRESULT hRes = pSvc->ExecQuery(bstr_t("WQL"),bstr_t("SELECT * FROM Win32_ComputerSystemProduct"),WBEM_FLAG_FORWARD_ONLY|WBEM_FLAG_RETURN_IMMEDIATELY,NULL,&pEnumerator);
		while(pEnumerator)
		{
			IWbemClassObject *pObj;
			ULONG uRet = 0;
			VARIANT vtProp;
			hRes = pEnumerator->Next(WBEM_INFINITE,1,&pObj,&uRet);
			if(0 == uRet)
			{
				break;
			}
			hRes = pObj->Get(L"UUID", 0, &vtProp, 0, 0);
			str = _com_util::ConvertBSTRToString(V_BSTR(&vtProp));
			VariantClear(&vtProp);
			pObj->Release();
		}

	}
	catch (CException* e)
	{
		if(NULL != pEnumerator)
		{
			pEnumerator->Release();
			pEnumerator = NULL;
		}
		return bRet;
	}

	if(NULL != pEnumerator)
	{
		pEnumerator->Release();
		pEnumerator = NULL;
	}
	bRet = true;
	return bRet;
}

CString CEncryptOp::GetDriverCode()
{
	CString Str = "", StrCpu = "", StrGUID = "";
	getCpu(StrCpu);
	GetCPUUniquelyIdentifies(StrGUID);
	StrCpu += StrGUID;
	MD5Encrypt(StrCpu, 16, Str);
	Str = Str.Mid(8, 8);
	return Str;
}
/// <summary>
/// CRC���У��
/// </summary>
/// <param name="cmdString">�����ַ���</param>
/// <returns></returns>
BOOL CEncryptOp::RegisterCodeDataCheck(CString &cmdString)
{
	//78DB7E17197BE048
	//CRC�Ĵ���
	int CRCCode = 0;
	string retStr = "";
	BYTE *ret = new BYTE[16];
	BYTE *ret1 = new BYTE[16];
	int len = cmdString.GetLength();
	//���ַ�����ֳ�Ϊ16�����ֽ�����Ȼ����λ��λ�������У��
	for (int i = 1; i < len / 2; i++)
	{
		CString Tre = cmdString.Mid(i * 2, 2);
		string cmdHex = Tre.GetBuffer(0);
		if (i == 1)
		{
			string cmdPrvHex = cmdString.Mid((i - 1) * 2, 2);
			hexToBytes(cmdPrvHex, ret);
			hexToBytes(cmdHex, ret1);
			ret[0] = ret[0] ^ ret1[0];
		}
		else
		{
			hexToBytes(cmdHex, ret1);
			ret[0] = ret[0] ^ ret1[0];
		}

	}
	int lem = sizeof(ret) / sizeof(int);
	bytesToHexString(ret, lem, cmdString);
	//s_temp = "0";

	delete[] ret;
	delete[] ret1;


	//string s_temp = dex2Hex(CRCCode);
	if(cmdString.GetLength() == 1)
	{
		  cmdString = "0" + cmdString;
	}
	cmdString = cmdString.MakeUpper();
	return TRUE;
}

//16�����ַ��� ת�޷����ֽ�����
void CEncryptOp::hexToBytes(const std::string& hex, BYTE* bytes) 
{
	int bytelen = hex.length() / 2;
	std::string strByte;
	unsigned int n;
	for (int i = 0; i < bytelen; i++) 
	{
		strByte = hex.substr(i * 2, 2);
		sscanf(strByte.c_str(),"%x",&n);
		bytes[i] = n;
	}
}


//�޷����ֽ�����ת16�����ַ���
BOOL CEncryptOp::bytesToHexString(const BYTE* bytes, const int length, CString &retStr) 
{
	if (bytes == NULL) 
	{
		return FALSE;
	}
	std::string buff;
	const int len = length;
	for (int j = 0; j < len; j++) {
		/*if ((bytes[j] & 0xff) < 16) {
			buff.append("0");
		}*/
		int high = bytes[j]/16, low = bytes[j]%16;
		buff += (high<10) ? ('0' + high) : ('a' + high - 10);
		buff += (low<10) ? ('0' + low) : ('a' + low - 10);
	}
	retStr = buff.c_str();
	return TRUE;
}


//��ʮ�������ַ���HexStr1��HexStr2���õ�HexStr
void CEncryptOp::hexstrxor(char * HexStr1, char * HexStr2, CString &RetStr)
{
	int i, iHexStr1Len, iHexStr2Len, iHexStrLenLow, iHexStrLenGap;
	char *HexStr = new char[16];

	//ת���ɴ�д���󳤶�, strupr�ǷǱ�׼��C��������Linux�²�֧�֣�������Ҫ�Լ�ʵ�ֻ���ʹ��glib�е�g_string_ascii_up ()
	//strupr(HexStr1);
	//strupr(HexStr2);
	iHexStr1Len = strlen( HexStr1 );
	iHexStr2Len = strlen( HexStr2 );

	//��ȡ��С�ĳ���
	iHexStrLenLow = iHexStr1Len<iHexStr2Len?iHexStr1Len:iHexStr2Len;

	//��ȡ���Ȳ�ֵ
	iHexStrLenGap = abs( iHexStr1Len-iHexStr2Len);

	//����ʮ�����Ƶ��ַ����������
	for( i=0; i<iHexStrLenLow; i++ )
	{
		*(HexStr+i) = char2int(HexStr1[i] ) ^ char2int( HexStr2[i] );
		*(HexStr+i) = int2char(*(HexStr+i));
	}
	if( iHexStr1Len>iHexStr2Len )
		memcpy( HexStr+i, HexStr1+i, iHexStrLenGap );
	else if( iHexStr1Len<iHexStr2Len )
		memcpy( HexStr+i, HexStr2+i, iHexStrLenGap );
	*( HexStr+iHexStrLenLow+iHexStrLenGap ) = 0x00;

	RetStr = CString(HexStr).MakeUpper();

}

int CEncryptOp::char2int( char input )
{
	return input > 64 ? (input - 55) : (input - 48);
}

//ASCII���н�ʮ������ת���ɶ�Ӧ���ַ�
int CEncryptOp::int2char( char input )
{
	return input > 9 ? (input + 55) : (input + 48);
}

void CEncryptOp::StringAnalysis(CString sRegisterCode, CString &RegisCode, CString &nLimitDate)
{
	//4��
	RegisCode = sRegisterCode.Mid(0, 1) + sRegisterCode.Mid(3, 1) + sRegisterCode.Mid(6, 1) + sRegisterCode.Mid(9, 1);
	nLimitDate = sRegisterCode.Mid(10, 2) + sRegisterCode.Mid(7, 2) + sRegisterCode.Mid(4, 2) + sRegisterCode.Mid(1, 2);
}

const char* CEncryptOp::Hex2ASC(const BYTE* Hex, int len)
{
	static char ASC[4096*2];
	int i;
	for(i=0;i<len;i++)
	{
		ASC[i*2] = "0123456789ABCDEF"[Hex[i] >> 4];
		ASC[i*2+1] = "0123456789ABCDEF"[Hex[i] & 0x0F];
	}
	ASC[i*2] = '\0';
	return ASC;
}

BOOL CEncryptOp::MD5Encrypt(CString password, int bit, CString &retMD5)
{
	HINSTANCE hDll;
	MD5_CTX ctx;
	PMD5Init MD5Init = NULL;
	PMD5Update MD5Update = NULL;
	PMD5Final MD5Final = NULL;
	char temp[100];
	::wsprintfA(temp,"%s",(LPCTSTR)password);


	if((hDll = LoadLibrary("advapi32.dll"))>0)
	{
		MD5Init = (PMD5Init)GetProcAddress(hDll,"MD5Init");
		MD5Update = (PMD5Update)GetProcAddress(hDll,"MD5Update");
		MD5Final = (PMD5Final)GetProcAddress(hDll,"MD5Final");
		MD5Init(&ctx);
		MD5Update(&ctx,(const unsigned char*)temp,password.GetLength());
		MD5Final(&ctx);
		retMD5 = Hex2ASC(ctx.digest,16);
		if(bit == 16)
		{
			retMD5 = retMD5.Right(24);
			retMD5 = retMD5.Left(16);
		}
	}

	return TRUE;
}

CString CEncryptOp::Binary2Hex(CString strBin, bool bIsUpper)
{
	CString strHex;
	//strHex.SetLength(strBin.GetLength()/4);
	CString strTemp = strBin;
	CString str;
	for(int i=0;i<strBin.GetLength()/4;i++)
	{
		str = strTemp.Right(4);
		strTemp = strTemp.Left(strTemp.GetLength()-4);
		int num = (str.GetAt(0)-'0')*8+(str.GetAt(1)-'0')*4+(str.GetAt(2)-'0')*2+(str.GetAt(3)-'0');
		if(num<10)
		{
			num += '0';
		}
		else
		{
			num += ((bIsUpper?'A':'a')-10);
		}
		strHex.AppendChar(num);
	}
	strHex.MakeReverse();
	return strHex;
}

void CEncryptOp::HexString2BinString(CString strHex, CStringList& list)
{
	int nLength = strHex.GetLength();
	int num = 0;
	for(int i=0;i<nLength;i++)
	{
		CString str;
		char ch = strHex.GetAt(i);
		if('0'<=ch&&'9'>=ch)
		{
			num = ch - '0';
		}
		else if('a'<=ch&&'f'>=ch)
		{
			num = ch - 'a' + 10;
		}
		else if('A'<=ch&&'F'>=ch)
		{
			num = ch - 'A' + 10;
		}
		for(int j=0;j<4;j++)
		{
			if(num%2)
			{
				str.AppendChar('1');
			}
			else
			{
				str.AppendChar('0');
			}
			num = num/2;
		}
		list.AddTail(str.MakeReverse());
	}
}

void CEncryptOp::Y_Move(CString strSrc, CStringList& list)
{
	CString str1,str2,str3;
	HexString2BinString(strSrc,list);
	for(int i=0;i<list.GetCount();i++)
	{
		str1+=list.GetAt(list.FindIndex(i));
	}
	int lenth = str1.GetLength();
	list.AddTail("");
	for(int i=0;i<=strSrc.GetLength();i++)
	{
		str2 = str1.Right(i);
		str3 = str2 + str1.Left(str1.GetLength()-i);
		CString temp,str;
		for(int j=0;j+4<=str3.GetLength();j+=4)
		{
			temp = str3.Right(str3.GetLength()-j);
			temp = temp.Left(4);
			str += Binary2Hex(temp, true);
		}
		list.SetAt(list.FindIndex(i),str);
		temp = list.GetAt(list.FindIndex(i));
	}
}


/// <summary>
/// д������ע����Ϣ��ע�����ļ�
/// </summary>
/// <returns></returns>
bool CEncryptOp::SetAllOFRegisterCode(CRegisterCode tmpRegMsg)
{

	try
	{
		CString InStr;
		if(tmpRegMsg.m_dtLimitTime != 0 && tmpRegMsg.m_dtLastTime != 0)
		{
			COleDateTimeSpan Msp = tmpRegMsg.m_dtLimitTime - tmpRegMsg.m_dtLastTime;
	
			if (Msp.m_span >= 0)
			{
				tmpRegMsg.m_nCurIdAllDays = Msp.GetDays();
			}
			else
			{
				tmpRegMsg.m_nCurIdAllDays = 0;
			}
		}
		for (int i = 0; i < 6; i++)
		{
			
			WriteFileLocalFile(m_sAllFile[i], tmpRegMsg);
		}

		SetOneRegisterCode(tmpRegMsg, m_sAllFile[6]);
		SetOneRegisterCode(tmpRegMsg, m_sAllFile[7]);
		SetOneRegisterCode(tmpRegMsg, m_sAllFile[8]);
		
		return true;
	}
	catch (int i)
	{
		return false;
	}
	
}

/// <summary>
/// ����һ��ע���ע������Ϣ
/// </summary>
/// <param name="RegisterCodeMsg"></param>
/// <param name="Regpath"></param>
/// <returns></returns>
bool CEncryptOp::SetOneRegisterCode(CRegisterCode RegisterCodeMsg, CString Regpath)
{
	//����
	DWORD dwDisposition;
	HKEY m_hKey;

	LONG ret = RegCreateKeyEx(HKEY_CURRENT_USER, Regpath, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &m_hKey, &dwDisposition);

	if(ret != ERROR_SUCCESS)
	{
		return FALSE;
	}

	// ��ע�����д��ע����

	CString strtemp, str;


	//LPSTR lpPath = (LPSTR)(LPCTSTR)strr1;
	////�����������д������Value                    //���� //ǿ��Ϊ�� //����        //����
	//if (ERROR_SUCCESS != (lRet = RegSetValueEx(hKey, TEXT(strr), 0, REG_SZ, (CONST BYTE *)lpPath, strr1.GetLength())))
	
	//�ַ���
	ret = RegSetValueEx(m_hKey, TEXT("RegisterCode"), NULL, REG_SZ, (LPBYTE)(LPCTSTR)RegisterCodeMsg.m_sRegisterCode, RegisterCodeMsg.m_sRegisterCode.GetLength());
	ret = RegSetValueEx(m_hKey, TEXT("DeviceCode"), NULL, REG_SZ, (LPBYTE)(LPCTSTR)RegisterCodeMsg.m_sDeviceCode, RegisterCodeMsg.m_sDeviceCode.GetLength());
	ret = RegSetValueEx(m_hKey, TEXT("LimitData"), NULL, REG_SZ, (LPBYTE)(LPCTSTR)RegisterCodeMsg.m_stLimitData, RegisterCodeMsg.m_stLimitData.GetLength());
	
	strtemp = RegisterCodeMsg.m_dtLimitTime.Format("%Y-%m-%d %H:%M:%S");
	ret = RegSetValueEx(m_hKey, TEXT("LimitTM"), NULL, REG_SZ, (LPBYTE)(LPCTSTR)strtemp, strtemp.GetLength());
	strtemp = RegisterCodeMsg.m_dtLastTime.Format("%Y-%m-%d %H:%M:%S");
	ret = RegSetValueEx(m_hKey, TEXT("LastTM"), NULL, REG_SZ, (LPBYTE)(LPCTSTR)strtemp, strtemp.GetLength());
	strtemp = RegisterCodeMsg.m_dtLockTime.Format("%Y-%m-%d %H:%M:%S");
	ret = RegSetValueEx(m_hKey, TEXT("LockData"), NULL, REG_SZ, (LPBYTE)(LPCTSTR)strtemp, strtemp.GetLength());

	//���ַ���
	//ret = RegSetValueEx(m_hKey, TEXT("Installed"), NULL, REG_DWORD, (CONST BYTE*)&(RegisterCodeMsg.m_bInstalled), sizeof(DWORD));
	//ret = RegSetValueEx(m_hKey, TEXT("Register"), NULL, REG_DWORD, (CONST BYTE*)&(RegisterCodeMsg.m_bRegisterFlag), sizeof(DWORD));

	DWORD dwVal = RegisterCodeMsg.m_bInstalled;
	ret = RegSetValueEx(m_hKey, "Installed", NULL, REG_DWORD, (CONST BYTE*)&dwVal, sizeof(DWORD));

	dwVal = RegisterCodeMsg.m_bRegisterFlag;
	ret = RegSetValueEx(m_hKey, "Register", NULL, REG_DWORD, (CONST BYTE*)&dwVal, sizeof(DWORD));

	COleDateTimeSpan span = RegisterCodeMsg.m_dtLimitTime - RegisterCodeMsg.m_dtLastTime;
	if (span.m_span >= 0)
	{
		RegisterCodeMsg.m_nCurIdAllDays = span.GetDays();
	}
	else
	{
		RegisterCodeMsg.m_nCurIdAllDays = 0;
	}
	
	ret = RegSetValueEx(m_hKey, TEXT("CurrentDays"), NULL, REG_DWORD, (CONST BYTE*)&(RegisterCodeMsg.m_nCurIdAllDays), sizeof(DWORD));
	ret = RegSetValueEx(m_hKey, TEXT("LimitDays"), NULL, REG_DWORD, (CONST BYTE*)&(RegisterCodeMsg.m_LimitDays), sizeof(DWORD));

		
	// �ر�ע���
	RegCloseKey(m_hKey);
	return TRUE;

}

/// <summary>
/// ��ȡ����ע����Ϣ
/// </summary>
/// <returns></returns>
bool CEncryptOp::GetAllOFRegisterCode()
{
	//CRegisterCode Creg;
	try
	{
		for(int i = 0; i < 6; i++)
		{
			ReadFileLocalFile(m_sAllFile[i], registerCodes[i]);
		}
		bool bgetCoede =  GetOneRegisterCode(registerCodes[6], m_sAllFile[6]);
		bool bgetCoede1 = GetOneRegisterCode(registerCodes[7], m_sAllFile[7]);

	}
	catch(int e)
	{
		return false;
	}

	return TRUE;
}



//��ȡһ��ע���ע������Ϣ
bool CEncryptOp::GetOneRegisterCode(CRegisterCode &RegisterCodeMsg, CString Regpath)
{
	try
	{
		LONG lValue = 0;
		//�ȶ�ȡ���ݵĳ���
		RegQueryValue(HKEY_CURRENT_USER, TEXT(Regpath), NULL, &lValue);
		TCHAR *pBuf = new TCHAR[lValue];
		RegQueryValue(HKEY_CURRENT_USER, TEXT(Regpath), pBuf, &lValue);

		HKEY hKey = NULL;
		DWORD dwTpye = REG_SZ;
		DWORD dwValue = MAX_PATH;//�����Сһ��Ҫ�������,��������(234)
		TCHAR data[MAX_PATH]; 
		LONG lRet = 0;
		//��ע���
		RegOpenKeyEx(HKEY_CURRENT_USER, TEXT(Regpath),0, KEY_ALL_ACCESS,&hKey);
		//��ע���                                          //valuename      //����value���� //���ݻ����� //��������С
		//if (ERROR_SUCCESS != (lRet = RegQueryValueEx(hKey, TEXT("CmSet1"), 0, &dwTpye, (LPBYTE)data, &dwValue)))
		//{
		//	AfxMessageBox(TEXT("��ȡע���ʧ��!"));
		//	return FALSE;
		//}

		CString tmpStr = "";

		lRet = RegQueryValueEx(hKey, "RegisterCode", 0, &dwTpye, (LPBYTE)data, &dwValue);
		RegisterCodeMsg.m_sRegisterCode.Format("%s", data);
		memset(data, 0, sizeof(data));
		dwValue = MAX_PATH;

		lRet = RegQueryValueEx(hKey, "DeviceCode", 0, &dwTpye, (LPBYTE)data, &dwValue);
		RegisterCodeMsg.m_sDeviceCode.Format("%s", data);
		memset(data, 0, sizeof(data));
		dwValue = MAX_PATH;

		lRet = RegQueryValueEx(hKey, TEXT("LimitData"), 0, &dwTpye, (LPBYTE)data, &dwValue);
		RegisterCodeMsg.m_stLimitData.Format("%s", data);
		memset(data, 0, sizeof(data));
		dwValue = MAX_PATH;



		lRet = RegQueryValueEx(hKey, "LastTM", 0, &dwTpye, (LPBYTE)data, &dwValue);
		tmpStr.Format("%s", data);
		if (tmpStr.GetLength() == 19)
		{
			RegisterCodeMsg.m_dtLastTime.ParseDateTime(tmpStr, VAR_CALENDAR_GREGORIAN);
		}
		
		memset(data, 0, sizeof(data));
		dwValue = MAX_PATH;

		lRet = RegQueryValueEx(hKey, TEXT("LimitTM"), 0, &dwTpye, (LPBYTE)data, &dwValue);
		tmpStr.Format("%s", data);
		if (tmpStr.GetLength() == 19)
		{
			RegisterCodeMsg.m_dtLimitTime.ParseDateTime(tmpStr, VAR_CALENDAR_GREGORIAN);
		}
		memset(data, 0, sizeof(data));
		dwValue = MAX_PATH;

		lRet = RegQueryValueEx(hKey, TEXT("LockData"), 0, &dwTpye, (LPBYTE)data, &dwValue);
		tmpStr.Format("%s", data);
		if (tmpStr.GetLength() == 19)
		{
			RegisterCodeMsg.m_dtLockTime.ParseDateTime(tmpStr, VAR_CALENDAR_GREGORIAN);
		}
		memset(data, 0, sizeof(data));
		dwValue = MAX_PATH;

		//���ַ���
		RegisterCodeMsg.m_bInstalled = true;

		lRet = RegQueryValueEx(hKey, "Installed", NULL, &dwTpye, (unsigned char*)&RegisterCodeMsg.m_bInstalled, &dwValue);
		dwValue = MAX_PATH;
		lRet = RegQueryValueEx(hKey, "Register", NULL, &dwTpye, (unsigned char*)&RegisterCodeMsg.m_bRegisterFlag, &dwValue);
		dwValue = MAX_PATH;
		lRet = RegQueryValueEx(hKey, "CurrentDays", NULL, &dwTpye, (unsigned char*)&RegisterCodeMsg.m_nCurIdAllDays, &dwValue);
		dwValue = MAX_PATH;
		lRet = RegQueryValueEx(hKey, "LimitDays", NULL, &dwTpye, (unsigned char*)&RegisterCodeMsg.m_LimitDays, &dwValue);
		dwValue = MAX_PATH;

		COleDateTimeSpan span = RegisterCodeMsg.m_dtLimitTime - RegisterCodeMsg.m_dtLastTime;
		if (span.m_span > 0)
		{
			RegisterCodeMsg.m_nCurIdAllDays = span.GetDays();
		}
		else
		{
			RegisterCodeMsg.m_nCurIdAllDays = 0;
		}
		
		delete pBuf;
		pBuf = NULL;
		// �ر�ע���
		RegCloseKey(hKey);
		return TRUE;
	
		
	}
	catch (int e)
	{
		return FALSE;
	}
}

//д�������ļ�
void CEncryptOp::WriteFileLocalFile(CString FileName, CRegisterCode Msg)
{
	try
	{
		//int key[]={1, 3, 4, 2, 6, 8};//�����ַ�134268C9A5
		int key[6] ={0};
		key[0] = 1;
		key[1] = 3;
		key[2] = 4;
		key[3] = 2;
		key[4] = 6;
		key[5] = 8;



		//ofstream ofs;
		std::locale::global(std::locale(""));
		ofstream ofs(FileName,ios::out|ios::binary);    //�Զ����� д�ķ�ʽ�����ļ�student.txt
		if(!ofs.is_open())                        //�����ʧ�ܣ�outfile����0ֵ
		{
			ofs.close();
			g_pFrm->m_CmdRun.AddMsg("д��ʧ��");
			return;
		}
		//ofs.write((LPSTR)(LPCSTR)&Msg, sizeof(CRegisterCode));    //������s1����Ϣд���ļ�
	

		//4-6
		ofs.write((LPSTR)(LPCSTR)&(Msg.m_dtLastTime), sizeof(Msg.m_dtLastTime));
		ofs.write((LPSTR)(LPCSTR)&(Msg.m_dtLimitTime), sizeof(Msg.m_dtLimitTime));
		ofs.write((LPSTR)(LPCSTR)&(Msg.m_dtLockTime), sizeof(Msg.m_dtLockTime));
	
		//7-12

		ofs.write((LPSTR)(LPCSTR)&(Msg.m_bInstalled), sizeof(Msg.m_bInstalled));
		ofs.write((LPSTR)(LPCSTR)&(Msg.m_bRegisterFlag), sizeof(Msg.m_bRegisterFlag));
		ofs.write((LPSTR)(LPCSTR)&(Msg.m_LimitDays), sizeof(Msg.m_LimitDays));

	
		ofs.write((LPSTR)(LPCSTR)&(Msg.m_nCurIdAllDays), sizeof(Msg.m_nCurIdAllDays));

		//CString tmp = Msg.m_sRegisterCode;
		string strTest2;
		char *p = ((LPSTR)(LPCTSTR)Msg.m_sRegisterCode);
		Makecode(p, key, strTest2);//����
		ofs.write(strTest2.c_str(), strTest2.size() + 1);
		//ofs.write(p, sizeof(p) + 1);
		Cutecode(p, key, strTest2);//����

		p = ((LPSTR)(LPCTSTR)Msg.m_sDeviceCode);
		Makecode(p, key, strTest2);//����
		ofs.write(strTest2.c_str(), strTest2.size() + 1);
		Cutecode(p, key, strTest2);//����


		p = ((LPSTR)(LPCTSTR)Msg.m_stLimitData);
		Makecode(p, key, strTest2);//����
		ofs.write(strTest2.c_str(), strTest2.size() + 1);
		Cutecode(p, key, strTest2);//����


		//string strTest2(p);
		//std::string strTest2 = (CT2A)Msg.m_sRegisterCode;
		//
		//std::string strTest2 = (CT2A)Msg.m_sRegisterCode;
		//ofs.write(strTest2.c_str(), strTest2.size()+1);

		//strTest2 = (CT2A)Msg.m_sDeviceCode;
		//ofs.write(strTest2.c_str(), strTest2.size()+1);

		//strTest2 = (CT2A)Msg.m_stLimitData;
		//ofs.write(strTest2.c_str(), strTest2.size()+1);


		//strTest2 = (CT2A)Msg.strLockNo;
		//ofs.write(strTest2.c_str(), strTest2.size()+1);



		//ofs.write((LPSTR)(LPCSTR)&Msg, sizeof(CRegisterCode));    //������s1����Ϣд���ļ�
		//if (p != NULL)
		//{
		//	//delete p;
		//	p = NULL;
		//}
		ofs.flush();
		ofs.clear();
		ofs.close();
	}
	catch(int e)
	{
		return;
	}
	

}

void CEncryptOp::ReadFileLocalFile(CString FileName, CRegisterCode &Msg)
{
	try
	{
		//int skey[]={1, 3, 4, 2, 6, 8};//�����ַ�134268C9A5
		int key[6] ={0};
		key[0] = 1;
		key[1] = 3;
		key[2] = 4;
		key[3] = 2;
		key[4] = 6;
		key[5] = 8;
		//int *key = skey;
		//ifstream ifs;
		std::locale::global(std::locale(""));
		ifstream ifs(FileName, ios::in |ios::binary);    //�Զ����� ���ķ�ʽ���ļ�
		if(!ifs.is_open())                        //�����ʧ�ܣ�outfile����0ֵ
		{
			ifs.close();
			//AfxMessageBox("��ʧ��");
			g_pFrm->m_CmdRun.AddMsg("����ʧ��");
			return;
		}
		/*int length = sizeof(CRegisterCode) + 1;
		ifs.read((LPSTR)(LPCSTR)&Msg, length);*/

	

		//4-6
		ifs.read((LPSTR)(LPCSTR)&(Msg.m_dtLastTime), sizeof(Msg.m_dtLastTime));
		ifs.read((LPSTR)(LPCSTR)&(Msg.m_dtLimitTime), sizeof(Msg.m_dtLimitTime));
		ifs.read((LPSTR)(LPCSTR)&(Msg.m_dtLockTime), sizeof(Msg.m_dtLockTime));

		//7-12
		ifs.read((LPSTR)(LPCSTR)&(Msg.m_bInstalled), sizeof(Msg.m_bInstalled));
		ifs.read((LPSTR)(LPCSTR)&(Msg.m_bRegisterFlag), sizeof(Msg.m_bRegisterFlag));
		ifs.read((LPSTR)(LPCSTR)&(Msg.m_LimitDays), sizeof(Msg.m_LimitDays));
		ifs.read((LPSTR)(LPCSTR)&(Msg.m_nCurIdAllDays), sizeof(Msg.m_nCurIdAllDays));



		std::string strTest, st1;
		strTest.resize(1024);
		ifs.read((char * )strTest.c_str(), 1024);
		Cutecode((char *)strTest.c_str(), key, st1);//����
		Msg.m_sRegisterCode = strTest.c_str();


		string str2;
		str2 = (char*)(strTest.c_str() + strlen(strTest.c_str()) + 1);
		Cutecode((char *)str2.c_str(), key, st1);//����
		Msg.m_sDeviceCode = st1.c_str();

		string str3;
		str3 = (char*)(strTest.c_str() + strlen(str2.c_str()) + 1 + strlen(strTest.c_str()) + 1);
		Cutecode((char *)str3.c_str(), key, st1);//����
		Msg.m_stLimitData = st1.c_str();

	
		/*std::string strTest;
		strTest.reserve(1024);
		ifs.read((char * )strTest.c_str(), 1024);
		Msg.m_sRegisterCode = strTest.c_str();

		string str2;
		str2 = (char*)(strTest.c_str() + strlen(strTest.c_str()) + 1);
		Msg.m_sDeviceCode = str2.c_str();

		string str3;
		str3 = (char*)(strTest.c_str() + strlen(str2.c_str()) + 1 + strlen(strTest.c_str()) + 1);
		Msg.m_stLimitData = str3.c_str();

		string str4;
		str4 = (char*)(strTest.c_str() + strlen(str3.c_str()) + 1 + strlen(str2.c_str()) + 1 + strlen(strTest.c_str()) + 1);
		Msg.strLockNo = str4.c_str();

		strTest.empty();*/
		//�����ļ��������Ϣ
		strTest.clear();
		ifs.clear();
		ifs.close();                     //�ر��ļ�
		//return
	}
	catch(int e)
	{
		return;
	}
	
}


//�����ַ��������
char CEncryptOp::MakecodeChar(char c,int key)
{
	return c=c^key;
}
//�����ַ�����
char CEncryptOp::CutcodeChar(char c,int key)
{
	return c^key;
}


//����
void CEncryptOp::Makecode(char *pstr,int *pkey, string &ret)
{
	int len=strlen(pstr);//��ȡ����
	for(int i=0;i<len;i++)
	{
		*(pstr+i)=MakecodeChar(*(pstr+i),pkey[i%5]);
	}
	ret = string(pstr);
}
//����
void CEncryptOp::Cutecode(char *pstr,int *pkey, string &ret)
{
	int len = strlen(pstr);
	for(int i = 0; i < len; i++)
	{
		*(pstr+i) = CutcodeChar(*(pstr+i), pkey[i%5]);
	}
	ret = string(pstr);
}