#pragma once
#include "afxmt.h"

class CLogFile
{
public:
	CLogFile(CString filename);
	virtual ~CLogFile();
	bool bLogEnable;
	void SetLogEnable(bool bEna);
	void log (const wchar_t *fmt, ...);
	void log(CString& message);
	void logAsync(CString& message);
	void log(BYTE* message, int size);
	LONGLONG GetSize();
protected:
	int size;
	CCriticalSection cs;
	CString filename;
	void CreateBakLog();
};