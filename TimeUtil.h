#pragma once

class CTimeUtil
{
protected:
	static LONGLONG m_lPerformanceFrequency;
	static void QueryPerformanceFrequency();
public:
	static LONGLONG GetCurTime();
	static LONGLONG GetDueTime(unsigned long lTimeInMs);
	static bool IsTimeout(LONGLONG lDueTimeInMs);
	static LONGLONG GetElapsedTime(LONGLONG lStartTimeInMs);
	static LONGLONG GetCurTimeUs();
	static bool Delay(long lTimeInMs);
};