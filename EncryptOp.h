
#pragma once

#include <vector>
#include <IPHlpApi.h>
#include <Wbemidl.h>
#include <Windows.h>
#include <string>   
#include <iostream>
#include <fstream>
#include <afxtempl.h>
#include "Function.h"
#include <intrin.h>
#include <atlconv.h>
#include <afxcoll.h>
#include <cstring>
#include <atlstr.h> 





using namespace std;


#define MD5DIGESTLEN 16
#define PROTO_LIST(list) list
#define REGFILENUM 9

typedef struct  
{
	ULONG i[2];
	ULONG bif[4];
	unsigned char in[64];
	unsigned char digest[16];
}MD5_CTX;

typedef void (WINAPI* PMD5Init) PROTO_LIST((MD5_CTX*));
typedef void (WINAPI* PMD5Update) PROTO_LIST((MD5_CTX*,const unsigned char*,unsigned int));
typedef void (WINAPI* PMD5Final) PROTO_LIST((MD5_CTX*));
/////MD5
#include <string>   
#include <fstream>   

/* Type define */
typedef unsigned char byte;		
typedef unsigned int uint32; 



using std::string;
using std::ifstream;
///////

#pragma comment(lib,"wbemuuid.lib")

#define MAX_BUFF 0x100


typedef struct  _ADAPTERINFO
{
	DWORD Type;    //对应IP_ADAPTER_ADDRESSES中的 IfType
	DWORD flag;    //对应IP_ADAPTER_ADDRESSES中的 Flags
	CHAR  Name[MAX_BUFF];
	WCHAR FriendName[MAX_BUFF];
	WCHAR Description[MAX_BUFF];
	WCHAR DomainName[MAX_BUFF];
	BYTE  Ip[8];
	BYTE  Mac[8];
	WCHAR NetMask[MAX_BUFF];
	WCHAR GetWay[MAX_BUFF];
	WCHAR Dhcp[MAX_BUFF];
	BYTE  FirDns[8];
	BYTE  SecDns[8];
}ADAPTERINFO;


class CRegisterCode
{

public:
	CRegisterCode()
	{
		
		m_stLimitData = "";
		m_sDeviceCode = "";
		m_sRegisterCode = "";
		m_bRegisterFlag = false;
		m_bInstalled = false;//未有用途
		m_dtLimitTime = COleDateTime::GetCurrentTime();
		m_dtLastTime = COleDateTime::GetCurrentTime();
		m_dtLockTime = COleDateTime::GetCurrentTime();//未做使用
		m_nCurIdAllDays = -1;
		m_LimitDays = -1;
	}

	CString m_stLimitData;
	CString m_sDeviceCode;
	CString m_sRegisterCode;
	bool m_bRegisterFlag;
	bool m_bInstalled;
	int m_nCurIdAllDays;//剩余日
	int m_LimitDays;//有效期
	COleDateTime m_dtLimitTime;
	COleDateTime m_dtLastTime;
	COleDateTime m_dtLockTime; //一键锁机，未使用
	
	

	//public byte[] LockNo = new byte[16];
	
	CRegisterCode & CRegisterCode::operator = (const CRegisterCode &C)
	{
		this->m_stLimitData.Format(C.m_stLimitData);
		this->m_sDeviceCode.Format(C.m_sDeviceCode);
		this->m_sRegisterCode.Format(C.m_sRegisterCode);

		this->m_bRegisterFlag = C.m_bRegisterFlag;
		this->m_bInstalled = C.m_bInstalled;
		this->m_nCurIdAllDays = C.m_nCurIdAllDays;
		this->m_LimitDays = C.m_LimitDays;
		this->m_dtLimitTime = C.m_dtLimitTime;
		this->m_dtLastTime = C.m_dtLastTime;
		this->m_dtLockTime = C.m_dtLockTime;


		return *this;
	}



};



class  CEncryptOp
{
private: 
	IWbemLocator *pLoc;
	IWbemServices *pSvc;
public:
	CEncryptOp(void);
	~CEncryptOp(void);


	BOOL m_bLockSystem;   // 软件锁标示  T= 软件锁死 F= 软件解锁
	CString m_StrDrive;
	CString m_AlarmTime;
public:
	void Init();
	// 根据获取注册码信息
	/////////////////////////////////////////////////////
	int  CalcPassWord(CString strDeviceCode, CString strCode, CRegisterCode &RegMsg);
	BOOL RegistInfo(CRegisterCode strCode);
	BOOL InitRegeditInfo(CRegisterCode Msg);
	//BOOL InitRegeditInfo(CTime CurTime,CString strPassword);
	CRegisterCode IsInstalled();
	CRegisterCode IsInstalledPro();
	//void Serialize(CRegisterCode obj, CString FilePath);
	//void DeSerialize(CRegisterCode &obj, CString FilePath);
	BOOL CreatePakcetFile();
	CRegisterCode RetWtrite(int Index[]);

public:
	 CRegisterCode registerCodes[REGFILENUM];
	 CString m_sAllFile[REGFILENUM];
private:
	
	///////////////////////////////////////
	vector<ADAPTERINFO> m_Adapters;


public:
	BOOL getCpu(CString &str);
	BOOL GetCPUUniquelyIdentifies(CString &str);
	CString GetDriverCode();
	void KeyToMD5( CString &Password);
	BOOL RegisterCodeDataCheck(CString &cmdString);
	void hexToBytes(const std::string& hex, BYTE* bytes);
	BOOL bytesToHexString(const BYTE* bytes, const int length, CString &retStr);
	void hexstrxor(char * HexStr1, char * HexStr2, CString &RetStr);
	int char2int(char input);
	int int2char(char input);
	void StringAnalysis(CString sRegisterCode, CString &RegisCode, CString &nLimitDate);
	//////////////////////////////////
	const char* Hex2ASC(const BYTE* Hex, int len);
	BOOL MD5Encrypt(CString password, int bit, CString &retMD5);
	CString Binary2Hex(CString strBin, bool bIsUpper);
	void HexString2BinString(CString strHex, CStringList& list);
	void Y_Move(CString strSrc, CStringList& list);
	bool SetAllOFRegisterCode(CRegisterCode tmpRegMsg);
	bool SetOneRegisterCode(CRegisterCode RegisterCodeMsg, CString Regpath);
	bool GetAllOFRegisterCode();
	bool GetOneRegisterCode(CRegisterCode &RegisterCodeMsg, CString Regpath);
	CString  RepairPassWord(CString strDeviceCode, CString strCode);
	void WriteFileLocalFile(CString FileName, CRegisterCode Msg);
	void ReadFileLocalFile(CString FileName, CRegisterCode &Msg);
	CRegisterCode RetWtrite();
	void Makecode(char *pstr,int *pkey, string &ret);
	void Cutecode(char *pstr,int *pkey, string &ret);
	char MakecodeChar(char c,int key);
	char CutcodeChar(char c,int key);
	void DeMD5();

};




