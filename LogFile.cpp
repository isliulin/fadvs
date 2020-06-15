#include "stdafx.h"
#include "afxconv.h"
#include "Mmsystem.h"
#include "LogFile.h"
#include "TSCtrlSys.h"

CLogFile::CLogFile(CString filename)
{
	this->filename = filename;
	bLogEnable = true;
	CString str;
	LONGLONG llFileSize= GetSize();
	str.Format("FileSize:%ld",llFileSize);
	log(str);
	if(llFileSize>1000*1000)
	{
		CString bakFilename = this->filename + _T(".bak");
		try { CFile::Remove(bakFilename); }
		catch (CFileException *e) {e->Delete();}
		
		try { CFile::Rename(filename, bakFilename);}
		catch (CFileException *e){e->Delete();}

		try	{ CFile::Remove(filename); }
		catch (CFileException *e){e->Delete();}
	}
	str = _T("Log File Initialized!");
	log(str);
}

CLogFile::~CLogFile()
{
	CString str = _T("Log File Destructed!");
	log(str);
}

void CLogFile::SetLogEnable(bool bEna)
{
	bLogEnable = bEna;
}
void CLogFile::log(CString& message)
{
	if (!bLogEnable) return;
	if(0==theApp.m_tSysParam.iIsLog)
	{
		return;
	}
	CSingleLock lock(&cs,TRUE);

	try{
		CFile outfile;
		if(!outfile.Open(filename, CFile::modeWrite|CFile::modeNoTruncate|CFile::modeCreate))
			return;				
		outfile.SeekToEnd();

		CString logMessage;
		SYSTEMTIME SysTime;
		GetLocalTime(&SysTime);
		DWORD dT=timeGetTime();
		logMessage.Format(_T("%04d/%02d/%02d %2d:%02d:%02d:%03d\t"), SysTime.wYear,SysTime.wMonth,SysTime.wDay,
			SysTime.wHour, SysTime.wMinute, SysTime.wSecond, dT%1000);

		logMessage.Append(message);
		//WORD wUnicode = 0xFEFF;
		//outfile.Write(&wUnicode,2);
		outfile.Write(logMessage, logMessage.GetLength()*sizeof(char));
		logMessage = "\r\n";
		outfile.Write(logMessage,2);
		outfile.Close();
	}
	catch(CFileException *e){e->Delete();}
	catch(...){}
}
void CLogFile::logAsync(CString& message)
{
	if(!bLogEnable) return;
	if(0==theApp.m_tSysParam.iIsLog)
	{
		return;
	}
	//CSingleLock lock(&cs,TRUE);
	try{
		CFile outfile;
		if(!outfile.Open(filename, CFile::modeWrite|CFile::modeNoTruncate|CFile::modeCreate))
			return;				
		outfile.SeekToEnd();

		CString logMessage;
		SYSTEMTIME SysTime;
		GetLocalTime(&SysTime); 
		DWORD dT=timeGetTime();
		//logMessage.Format(_T("%04d/%02d/%02d Async:\t"), SysTime.wYear,SysTime.wMonth,SysTime.wDay); 
		logMessage.Format(_T("Async:")); 

		logMessage.Append(message);
		outfile.Write(logMessage, logMessage.GetLength()*sizeof(char));
		logMessage = "\r\n";
		outfile.Write(logMessage,2);
		outfile.Close();
	}
	catch(CFileException *e){e->Delete();}
	catch(...){}
}
void CLogFile::log(const wchar_t *fmt, ...)
{
	if (!bLogEnable) return;
	va_list arg_list;
	wchar_t temp[1024];
	va_start(arg_list,fmt);
	vswprintf_s(temp,fmt,arg_list);
	va_end(arg_list);
	//CString szTemp=temp;
	CString szTemp = static_cast<CString>(temp);
	log(szTemp);
}

void CLogFile::log(BYTE* message, int size)
{
	CString byteData;
	for (int i=0; i<size; i++)
	{
		byteData.AppendFormat(_T("%02x"),message[i]);
		if(i && i%50==0)
		{
			log(byteData);
			byteData.Empty();
		}
	}
}

LONGLONG CLogFile::GetSize()
{
	LONGLONG llFileSize = 0;
	CSingleLock lock(&cs,TRUE);
	try{
		CFile outfile;
		if(!outfile.Open(filename, CFile::modeWrite|CFile::modeNoTruncate|CFile::modeCreate))
		{
			return -1;
		}
		llFileSize = outfile.GetLength();
		outfile.Close();
	}
	catch(CFileException *e){e->Delete();}
	catch(...){}
	return llFileSize;
}