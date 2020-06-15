// mvFunction.h : main header file for the mvFunction DLL
//

#pragma once

class CFunction
{
public:
	CFunction();
	~CFunction();
	static CString GetVerion();

public:
	// 当前目录
	static void GetDirectory(LPCSTR  pExcutePath);
	static CString GetDirectory();
	//////////////////////////////////////////////////////////////////////////
	static void DoEvents();
	static void Delay(double dTime/*S*/);
	static void DelayEx(double dTime/*S*/);
	static void AppendEdit(CEdit *pEdit, LPCSTR pText);

	static LONGLONG GetCurrentCounter();
	static LONGLONG GetCurrentFrequency();
	static double GetCurrentTimeEx();//S

	// 文件存取
	static void HandleStruct(BOOL bReadParam, LPCSTR lpszSection, LPCSTR lpszKey, LPVOID lpStruct,  UINT uSizeStruct, LPCSTR szFileName);
	static void HandleInt(BOOL bReadParam, LPCSTR lpAppName, LPCSTR lpKeyName, INT &nValueDefault, LPCSTR lpFileName);
	static void HandleLong(BOOL bReadParam, LPCSTR lpAppName, LPCSTR lpKeyName, long &nValueDefault, LPCSTR lpFileName);
	static void HandleDWord(BOOL bReadParam, LPCSTR lpAppName, LPCSTR lpKeyName, DWORD &nValueDefault, LPCSTR lpFileName);
	static void HandleByte(BOOL bReadParam, LPCSTR lpAppName, LPCSTR lpKeyName, BYTE &nValueDefault, LPCSTR lpFileName);
	static void HandleString(BOOL bReadParam, LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpValueDefault, LPCSTR lpFileName);
	static void HandleDouble(BOOL bReadParam, LPCSTR lpAppName, LPCSTR lpKeyName, double &lpValueDefault, LPCSTR lpFileName);
	static void HandleFloat(BOOL bReadParam, LPCSTR lpAppName, LPCSTR lpKeyName, float &lpValueDefault, LPCSTR lpFileName);
};