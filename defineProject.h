#pragma once
#include "RFIDPublic.h"

#define OK			0
#define NG			(!0)


class CStopWatch 
{
public:
	CStopWatch() { ::QueryPerformanceFrequency(&m_liPerfFreq); Start(); }
	void Start() { ::QueryPerformanceCounter(&m_liPerfStart); }

	__int64 Now() const
	{
		LARGE_INTEGER liPerfNow;
		::QueryPerformanceCounter(&liPerfNow);
		return ((liPerfNow.QuadPart - m_liPerfStart.QuadPart)*1000000 / m_liPerfFreq.QuadPart);
	}

private:
	LARGE_INTEGER m_liPerfFreq;
	LARGE_INTEGER m_liPerfStart;

};

struct project331Struct 
{
	BOOL m_bStepCmd;  //331项目专用，判断有扩展指令
	int m_stepCnt;
	int m_stepStartIndex;
	unsigned char Product[ProLen];
	unsigned char *g_pComData;
	unsigned int m_dataLen;
	~project331Struct()
	{
		if(g_pComData != NULL)
			delete []g_pComData;
		g_pComData = NULL;
	}


	project331Struct()
	{
		initialize();
	}

	void initialize()
	{
		m_bStepCmd = FALSE;
		m_stepCnt = -1;
		m_stepStartIndex =0;
		::ZeroMemory(Product,sizeof(Product));
		g_pComData = NULL;
		m_dataLen=270;
	}
	void initializeProduct()
	{
		::ZeroMemory(Product,sizeof(Product));

	}
	CString GetString(VOID)
	{
		CString strSend("0014RD00000256");
		char stx = STX;
		char etx = ETX;
		strSend = stx + strSend;
		strSend = strSend + etx;
		return strSend;
	}
	short AnalyseComPortData(unsigned char * comChar,int length)
	{
		unsigned char *rfidPtr;
		RFID_TAG RF;
		int itr;


		if(comChar == NULL || length<=1) return -1;
		//if(STX == strOutput.Left(1) && ETX == strOutput.Right(1))
		if(STX == comChar[0] && ETX == comChar[length-1])
		{

			//去除数据头STX和数据尾ETX
			rfidPtr = (unsigned char *)(&RF);
			memcpy(rfidPtr,&comChar[13],length-14);

			//////////////////////////////////////////////////////////////////////
			itr = length - 14;
			for(int i=0;i<itr;i++)
			{
				unsigned char printer;
				printer = (unsigned char)(*rfidPtr);

				TRACE("%02x ",printer);
				if((i+1)%16 == 0)
					TRACE("\n");
				rfidPtr++;
			}
			TRACE("\n");	
			//////////////////////////////////////////////////////////////////////


			for(int i=0;i<ProLen;i++)
			{
				if((RF.sProductData[i].ProductData.chNGRAngle & 0x80) == 0)
				{
					Product[i] = OK;
				}
				else
				{
					Product[i] = NG;
				}
				TRACE("%d ",Product[i]);
				if((i+1)%8 == 0)
					TRACE("\n");
			}
			return 0;
		}
		return 1;//数据头或者数据尾错误
		
	}
};
