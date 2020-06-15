#include "StdAfx.h"
#include "UserManage.h"
#include "TSCtrlSys.h"
CUserManage::CUserManage(void)
{
	m_CurUD.level = 0;//guest in default 
	m_CurUD.uID = 0;
}

CUserManage::~CUserManage(void)
{
	FreeUserData();
}

BOOL CUserManage::FreeUserData()
{
	m_pUserData.RemoveAll();
	return TRUE;
}

BOOL CUserManage::AddUser(USERDATA user)
{
	user.uID = m_pUserData.GetCount();
	m_pUserData.Add(user);
	return TRUE;
}

BOOL CUserManage::DelUser(USERDATA user)
{
	m_pUserData.RemoveAt(user.uID);
	return TRUE;
}

BOOL CUserManage::ChangeUser(USERDATA user)
{
	int n = m_pUserData.GetCount();
	if(user.uID > n) return FALSE;
	m_pUserData[user.uID].level = user.level;
	memcpy(m_pUserData[user.uID].Password,user.Password,sizeof(char)*20);
	memcpy(m_pUserData[user.uID].UserName,user.UserName,sizeof(char)*20);
	return TRUE;
}

int CUserManage::FindUser(USERDATA user)
{
	CString ustr,pstr,tmpustr,tmppstr;
	char uch[20];
	char pch[20];
	char cTmpUd[20];
	char cTmpPd[20];
	memcpy(uch,user.UserName,sizeof(char)*20);
	memcpy(pch,user.Password,sizeof(char)*20);
	ustr.Format("%s",uch);
	pstr.Format("%s",pch);
	int iAd = m_pUserData.GetCount();
	USERDATA TmpUd;
	for (int i=0;i<iAd;i++)
	{
		TmpUd = m_pUserData.GetAt(i);
		memcpy(cTmpUd,TmpUd.UserName,sizeof(char)*20);
		memcpy(cTmpPd,TmpUd.Password,sizeof(char)*20);
		tmpustr.Format("%s",cTmpUd);
		tmppstr.Format("%s",cTmpPd);
		if(0==tmpustr.Compare(ustr) && 0==tmppstr.Compare(pstr) && TmpUd.level == user.level)
			return TmpUd.uID;
	}
	return -1;
}

char CUserManage::Switchstr(char ch)
{
	char tch = ch;
	for (int j=0;j<8;j+=2)
	{
		unsigned short ll = 1<<j;
		if(tch&ll)
			tch = (tch&(~ll));
		else
		{
			tch = tch|(char)ll;
		}
	}
	return tch;
}

BOOL CUserManage::LoadUserData()
{
	USERDATA TmpUd;
	CString strFilePath = CFunction::GetDirectory()+"system\\_FORWORK.DAT";
	CFile readTxtFile;
	if(!readTxtFile.Open(strFilePath, CFile::modeRead | CFile::typeBinary))
	{
		for(int i=0;i<4;i++)
		{
			/*TmpUd.level = i+1;*/
			TmpUd.level=i;
			StrCpy(TmpUd.UserName,"default");
			StrCpy(TmpUd.Password,"112233");
			TmpUd.uID = i;
			m_pUserData.Add(TmpUd);
		}
	/*	TmpUd.level = 3;
		StrCpy(TmpUd.UserName,"default");
		StrCpy(TmpUd.Password,"112233");
		TmpUd.uID = 2;
		m_pUserData.Add(TmpUd);*/
		return FALSE;
	}

	int intValueOut = 0;
	int iDr = 0;
	int iUid = 0;

	if(readTxtFile.GetPosition() != readTxtFile.GetLength())
		readTxtFile.Read( &intValueOut, sizeof(int));
	if(readTxtFile.GetPosition() != readTxtFile.GetLength())
		readTxtFile.Read( &intValueOut, sizeof(int));
	iDr = (intValueOut%10);
	while(readTxtFile.GetPosition() != readTxtFile.GetLength())
	{
		TmpUd.uID = iUid;
		readTxtFile.Read( &intValueOut, sizeof(int));
		TmpUd.level = intValueOut>>iDr;
		for(int i=0;i<20;i++)
		{
			if(readTxtFile.GetPosition() == readTxtFile.GetLength()) break;
			readTxtFile.Read( &intValueOut, sizeof(int));
			TmpUd.UserName[i] = Un_int2char(intValueOut,iDr);
		}
		for(int i=0;i<20;i++)
		{
			if(readTxtFile.GetPosition() == readTxtFile.GetLength()) break;
			readTxtFile.Read( &intValueOut, sizeof(int));
			TmpUd.Password[i] = Un_int2char(intValueOut,iDr);
		}
		m_pUserData.Add(TmpUd);
		iUid++;
	}
	readTxtFile.Close();
	return TRUE;
}

BOOL CUserManage::SaveUserData()
{
	srand((int)time(0));
	int iHead = rand();
	iHead = rand();
	int iNone = rand();
	int iDr = iHead%10;
	int iNd = m_pUserData.GetCount();
	int iTmp = 0;

	CString strFilePath = CFunction::GetDirectory() + "system\\_FORWORK.DAT";
	//strFilePath.Format();
	CFile WriteTxtFile( strFilePath, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary );
	WriteTxtFile.Write( &iNone, sizeof(int));
	WriteTxtFile.Write( &iHead, sizeof(int));
	for(int i =0;i<iNd;i++)
	{
		USERDATA TmpUser = m_pUserData.GetAt(i);
		iTmp = TmpUser.level<<iDr;
		WriteTxtFile.Write( &iTmp, sizeof(int));
		for(int j=0;j<20;j++)
		{
			iTmp = Ln_char2int(TmpUser.UserName[j],iDr);
			WriteTxtFile.Write( &iTmp, sizeof(int));
		}
		for(int j=0;j<20;j++)
		{
			iTmp = Ln_char2int(TmpUser.Password[j],iDr);
			WriteTxtFile.Write( &iTmp, sizeof(int));
		}
	}
	WriteTxtFile.Close();

	return TRUE;
}

char CUserManage::Getrandomstr()
{
	char ch;
	int t = rand();
	ch = (char)t&0xFF;
	return ch;
}

int CUserManage::Ln_char2int(char ch,int dr)
{
	if(ch == 0) return 0xFFFF;

	char tch = Switchstr(ch);
	int t1,t2;
	t2 = rand();
	t2 = t2 & 0xFF;
	t2 = t2 >> (8-dr);
	t1 = (tch<<dr) + t2;
	return t1;
}

char CUserManage::Un_int2char(int ich,int dr)
{
	char tch;
	if(ich == 0xFFFF)
	{
		tch = 0;
		return tch;
	}
	int ti = ich>>dr;
	ti = ti&0xFF;
	tch = (char)ti;
	return Switchstr(tch);
}