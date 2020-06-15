#include "stdafx.h"
#include "TimeUtil.h"
#include <time.h>

LONGLONG CTimeUtil::m_lPerformanceFrequency = 0;

void CTimeUtil::QueryPerformanceFrequency()
{
	LARGE_INTEGER lFrequency;
	::QueryPerformanceFrequency(&lFrequency);
	m_lPerformanceFrequency = lFrequency.QuadPart;
}

LONGLONG CTimeUtil::GetCurTime()
{
	if(0==m_lPerformanceFrequency)
	{
		QueryPerformanceFrequency();
	}

	LARGE_INTEGER lCurTime;
	QueryPerformanceCounter(&lCurTime);
	LONGLONG lTime = lCurTime.QuadPart *1000 / m_lPerformanceFrequency;
	return lTime;
}

LONGLONG CTimeUtil::GetDueTime(unsigned long lTimeInMs)
{
	if(0==m_lPerformanceFrequency)
	{
		QueryPerformanceFrequency();
	}

	LARGE_INTEGER lCurTime;
	QueryPerformanceCounter(&lCurTime);
	LONGLONG lDueTimeInMs = lTimeInMs + lCurTime.QuadPart *1000 / m_lPerformanceFrequency;
	return lDueTimeInMs;
}

bool CTimeUtil::IsTimeout(LONGLONG lDueTimeInMs)
{
	return (GetElapsedTime(lDueTimeInMs) > 0 ? true : false);
}

bool CTimeUtil::Delay(long lTimeInMs)
{
	LONGLONG lDueTime = CTimeUtil::GetDueTime(lTimeInMs);
	while(true)
	{
		MSG msgCur,msg;
		while(PeekMessage(&msgCur,NULL,NULL,NULL,PM_NOREMOVE))
		{
			if(!AfxGetThread()->PumpMessage())
			{
				//AfxGetThread()->PostThreadMessage(WM_QUIT,0,0);
				return false;
			}
			msg = msgCur;
		}
		if(!CTimeUtil::IsTimeout(lDueTime))
		{
			Sleep(1);
		}
		else
		{
			return true;
		}
	}
	return false;
}

LONGLONG CTimeUtil::GetElapsedTime(LONGLONG lStartTimeInMs)
{
	if(0==m_lPerformanceFrequency)
	{
		QueryPerformanceFrequency();
	}

	LARGE_INTEGER lCurTime;
	QueryPerformanceCounter(&lCurTime);
	LONGLONG lTimeElapsed = lCurTime.QuadPart *1000 / m_lPerformanceFrequency - lStartTimeInMs;
	return lTimeElapsed;
}

LONGLONG CTimeUtil::GetCurTimeUs()
{
	if(0==m_lPerformanceFrequency)
	{
		QueryPerformanceFrequency();
	}

	LARGE_INTEGER lCurTime;
	QueryPerformanceCounter(&lCurTime);
	LONGLONG lTime = lCurTime.QuadPart * 1000000 / m_lPerformanceFrequency;
	return lTime;
}