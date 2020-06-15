#include "StdAfx.h"
#include "TSCtrlSys.h"
#include "Ts_DhCamera.h"

#pragma comment(lib,"hvdh/lib/Raw2Rgb.lib")
#pragma comment(lib,"hvdh/lib/HVDAILT.lib")
#pragma comment(lib,"hvdh/lib/HVUtil.lib")

#define IMAGE_WIDTH 1280;
#define IMAGE_HEIGHT 1024;

//CRITICAL_SECTION  g_cs;

Ts_DhCamera::Ts_DhCamera(void)
{
	CCD_OK = 0;
	m_bOpen = FALSE;
	m_bStart = FALSE;
	m_bColor = false;
	m_isStartCap = 0;
	m_pRawBuffer = NULL;
	m_pImageBuffer = NULL;
	m_pBitmapInfo = NULL;
	m_pSearchBuffer = NULL;
	m_nImageWidth = 1280;		
	m_nImageHeight = 1024;
	m_nOffsetX=0;
	m_nOffsetY=0;

	for(int i=0;i<256;i++)
	{
		m_pLutR[i] = (BYTE)i;
		m_pLutG[i] = (BYTE)i;
		m_pLutB[i] = (BYTE)i;
	}
	//原始图像  
	if (m_pRawBuffer == NULL)
	{
		m_pRawBuffer = new BYTE[m_nImageWidth*m_nImageHeight];
	}

	if (m_pImageBuffer == NULL)
	{
		m_pImageBuffer = new BYTE[m_nImageWidth*m_nImageHeight*3];
	}
}

Ts_DhCamera::~Ts_DhCamera(void)
{
	if(m_pImageBuffer!=NULL)
	{
		delete[] m_pImageBuffer;
		m_pImageBuffer = NULL;
	}
	if(m_pRawBuffer!=NULL)
	{
		delete[] m_pRawBuffer;
		m_pRawBuffer = NULL;
	}
	if(m_pSearchBuffer!=NULL)
	{
		delete[] m_pSearchBuffer;
		m_pSearchBuffer = NULL;
	}

	DeleteCriticalSection(&g_cs);      //Del 
}

int Ts_DhCamera::Init()
{
	InitializeCriticalSection(&g_cs);  //Init

	bool isSuccess = m_camera.OpenCamera(0);
	if(isSuccess)
	{
		CCD_OK=1;	
	/*	m_nImageWidth = m_camera.GetWidth();
		m_nImageHeight = m_camera.GetHeight();*/
		SetROI(m_nOffsetX,m_nOffsetY,m_nImageWidth,m_nImageHeight);

		//原始图像  
		if (m_pRawBuffer == NULL)
		{
			m_pRawBuffer = new BYTE[m_nImageWidth*m_nImageHeight];
			ASSERT(m_pRawBuffer);
		}

		if (m_pSearchBuffer == NULL)
		{
			m_pSearchBuffer = new BYTE[m_nImageWidth*m_nImageHeight *3];
			ASSERT(m_pSearchBuffer);
		}
	}
	return (int)isSuccess;
}

int Ts_DhCamera::Exit()
{
	if(CCD_OK)
	{
		m_camera.CloseCamera();
	}

	CCD_OK = 0;
	return(1) ;
}

int Ts_DhCamera::SoftTrigger()
{
	if(CCD_OK)
	{
		bool rtn = m_camera.TriggerSoft();
		if(rtn)
		{
			m_triggerCnt++;
		}
		return rtn;
	}
	return(1);
}

int Ts_DhCamera::GetImage(BYTE* buf)  //copy image from the CCD buffer
{	
	if((m_nImageWidth !=0) && (m_nImageHeight !=0))
	{
		memcpy(buf, m_pRawBuffer, m_nImageWidth*m_nImageHeight);
	}
	return 1;		
}

int Ts_DhCamera::StartCapture()
{
	if(CCD_OK)
	{
		m_triggerCnt = 0;
		m_recvPicCnt = 0;
		return m_camera.StartCapture();
	}
	return false;
}

int Ts_DhCamera::StopCapture()
{
	return m_camera.StopCapture();
}

int Ts_DhCamera::getExposure()
{
	return m_camera.GetShutter();
}

int Ts_DhCamera::setExposure(int exposure)
{
	if (CCD_OK)
	{
		return m_camera.SetShutter(exposure*1000);
	}
	else
	{
		return 1;
	}

} 
int Ts_DhCamera::setGain(int value)
{
	if (CCD_OK)
	{
		m_camera.SetGain(value);
		return 0;
	}
	else
		return 1;

}
void Ts_DhCamera::SetROI(int offsetX,int offsetY,int width,int height)
{
	if (CCD_OK)
	{
		m_camera.SetOffsetX(offsetX);
		m_camera.SetOffsetY(offsetY);
		m_camera.SetWidth(width);
		m_camera.SetHeight(height);
	}
}

void Ts_DhCamera::EnterContinueMode()
{
	m_camera.StopCapture();
	m_camera.SetWorkMode(CONMODE);
	m_camera.StartCapture(); //触发onGetFrame
	STREAM_PROC proc = (STREAM_PROC)CCDCallBack1;//函数指针强制类型转换
	m_camera.SetStreamHook(proc, (void*)this);
}

void Ts_DhCamera::SingleGrab()
{
	TRACE("SingleGrab------start...\n");
	static bool bSample=false;
	if(bSample)
	{
		return;
	}
	bSample=true;

	bool ret;
	CString str;
	ret=false;
	unsigned char * pBuffer;
	pBuffer=new unsigned char[IMAGEWIDTH1*IMAGEHEIGHT1];

	m_camera.StopCapture();
	m_camera.SetStreamHook(NULL, (void*)this); //注销
	m_camera.SetWorkMode(TRIGMODE);

	m_camera.StartCapture();
	ret=m_camera.TriggerSoft();
	if (ret)
	{
		str.Format("设置软触发成功！");
		g_pFrm->m_CmdRun.AddMsg(str);
	}
	else
	{
		str.Format("设置软触发失败！");
		g_pFrm->m_CmdRun.AddMsg(str);
		//////////////////////////////
		if (NULL!=pBuffer)
		{
			delete []pBuffer;
			pBuffer=NULL;
		}
		/////////////////////////////
		bSample=false; //2018-09-14
		return;
	}
	ret=m_camera.WaitPicture(pBuffer,2400);//ms
	if (ret)
	{
		str.Format("单帧取图成功！");
		g_pFrm->m_CmdRun.AddMsg(str);
	}
	else
	{
		str.Format("单帧取图失败！");
		g_pFrm->m_CmdRun.AddMsg(str);
		//////////////////////////////
		if (NULL!=pBuffer)
		{
			delete []pBuffer;
			pBuffer=NULL;
		}
		/////////////////////////////
		bSample=false; //2018-09-14
		return;
	}

	//////////////////////避免内存破坏///////////////////
	/*LONGLONG llDueDalay = CTimeUtil::GetDueTime(600000); 
	while(!CTimeUtil::IsTimeout(llDueDalay))
	{
		if(!g_pFrm->m_CmdRun.m_bPadDetecting) 
		{
			break;
		}
		Sleep(1);
		CFunction::DoEvents();
	}*/
	/////////////////////////////////////////////////////
	//g_pFrm->m_CmdRun.m_bPadDetecting=true;
	EnterCriticalSection(&g_cs);
	g_pView->m_ImgStatic.CopyDetectImage(pBuffer,IMAGEWIDTH1,IMAGEHEIGHT1,ePGGrey8);
	g_pView->m_ImgStatic.CopyDetectBuffer(pBuffer,IMAGEWIDTH1,IMAGEHEIGHT1,ePGGrey8);
	LeaveCriticalSection(&g_cs);
	//g_pFrm->m_CmdRun.m_bPadDetecting=false;
    Sleep(2000);
	str.Format("单帧取像成功");
	g_pFrm->m_CmdRun.AddMsg(str);

	if (NULL!=pBuffer)
	{
		delete []pBuffer;
		pBuffer=NULL;
	}
	bSample=false;
	TRACE("SingleGrab------over...\n");
}
void Ts_DhCamera::StopGrba()
{
	m_camera.StopCapture();
} 
void Ts_DhCamera::CloseCamera()
{
	m_camera.CloseCamera();
}
bool Ts_DhCamera::TriggerSoft()
{
	return m_camera.TriggerSoft();
}
bool Ts_DhCamera::IsConnected()
{
	return m_camera.IsConnected();
}
void CALLBACK CCDCallBack1(unsigned char* lpParam, void* lpUser)  //src->dst 
{
	static bool b = false;
	if(b)return;
	b=true;
	EnterCriticalSection(&g_cs);
	BYTE *pDataBuffer = (BYTE*)lpParam;  //Src
	//Ts_DhCamera *ccd = (Ts_DhCamera*)lpUser;  //Dst
	g_pView->m_ImgStatic.CopyDetectImage(pDataBuffer,IMAGEWIDTH1,IMAGEHEIGHT1,ePGGrey8);
	g_pView->m_ImgStatic.CopyDetectBuffer(pDataBuffer,IMAGEWIDTH1,IMAGEHEIGHT1,ePGGrey8);
	LeaveCriticalSection(&g_cs);
	//g_pFrm->m_CmdRun.m_bPadDetecting=false;
	b=false;
}