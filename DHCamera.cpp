#include "stdafx.h"
#include "DHCamera.h"
#include <stdlib.h>
#include <Winsock2.h>
#include <process.h>
#include <stdio.h>
#include "TSCtrlSys.h"

#pragma comment (lib, "ws2_32")
#pragma comment (lib,"MVSDKmd.lib")
#pragma warning(disable: 4819)

std::vector<GenICam::ICameraPtr> DHCamera::g_DHCameraList;

// 统计接口时间
SYSTEMTIME  sysTime;                            // 系统时间，精确到毫秒级

#define BEGINTIME(operation) {GetLocalTime(&sysTime);TRACE("\n ***%s begin time:minute:%d, second:%d,  millisecond:%d***\n", operation, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);}

#define ENDTIME(operation) {GetLocalTime(&sysTime);TRACE("\n ***%s end time:minute:%d, second:%d,  millisecond:%d***\n", operation, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);}

LARGE_INTEGER   largeInteger;                   // 系统时间，精确到微妙级
__int64 IntStart;  
__int64 IntEnd;  
double DobDff;  

DHCamera::DHCamera()
	: m_workModeCur(CONMODE), m_waitingTime(200)
{
	srand((int)time(0));

	m_cameraSptr.reset();
	m_streamPtr.reset();

	m_pBuff			= NULL;
	m_trigFinish	= NULL;
	m_bCapturingFlg	= true;
	m_bConnectFlg	= false;
	m_pUserPtr		= NULL;
	m_pStreamProc	= NULL;
	
}

DHCamera::~DHCamera()
{
	CloseCamera();						 
}

int DHCamera::EnumCameras()
{
	// 检测通过网卡连接的在线相机
	g_DHCameraList.clear();
	Infra::TVector<GenICam::ICameraPtr>	tempCameras;

    BEGINTIME("discovery");
    IntStart = getTimeInUs();
	if (!GenICam::CSystem::getInstance().discovery(tempCameras, GenICam::CSystem::EInterfaceType::typeGige))
	{
		CString l_Str;
		l_Str.Format(_T("发现Dahua 相机失败"));
		::AfxMessageBox(l_Str);
	}
    IntEnd = getTimeInUs();
    TRACE("\n***discovery total time: %f ms***\n", (IntEnd - IntStart) * 1000 / getDobDff());
    ENDTIME("discovery");

	for(size_t i=0; i<tempCameras.size(); i++)
	{
		g_DHCameraList.push_back(tempCameras[i]);
	}

	return g_DHCameraList.size();
}

bool DHCamera::OpenCamera(CamPara& Para)
{
	bool InitFlg = FALSE;

	if(m_bConnectFlg)
	{
		CString l_Str;
		l_Str.Format(_T("相机已处于连接状态"));
		::AfxMessageBox(l_Str);
		return InitFlg;
	}

	m_camPara = Para;

	if(m_camPara.DeviceID != -1)
	{
		// 检测通过网卡连接的在线相机
		if(g_DHCameraList.empty())
		{
			EnumCameras();
		}
		
		// 如果指定的DeviceID超过已检测到的相机数量， 表示未找到该编号的相机
		if((size_t)m_camPara.DeviceID >= g_DHCameraList.size())
		{
			return InitFlg;
		}

		// 通过DeviceID找到相机对象
		m_cameraSptr = g_DHCameraList[m_camPara.DeviceID];


		// 连接前自动修改该相机IP为所连接网卡同网段IP
		if(!IpAutoConfig())
		{
			CString l_Str;
			l_Str.Format(_T("相机%d[%s] IP自动配置失败"), m_camPara.DeviceID, m_cameraSptr->getName());
			::AfxMessageBox(l_Str);
			return InitFlg;
		}

		// 连接设备
		if(!m_cameraSptr->connect())
		{
			CString l_Str;
            l_Str.Format(_T("打开相机%d [%s]失败"),m_camPara.DeviceID, m_cameraSptr->getKey());
			::AfxMessageBox(l_Str);
			return InitFlg;
		}

        // 创建属性访问类
        m_imageParamCtrlPtr = GenICam::CSystem::getInstance().createImageFormatControl(m_cameraSptr);
        m_analogCtrlPtr = GenICam::CSystem::getInstance().createAnalogControl(m_cameraSptr);
        m_ispParamCtrlPtr = GenICam::CSystem::getInstance().createISPControl(m_cameraSptr);
        m_aquisitionCtrlPtr = GenICam::CSystem::getInstance().createAcquisitionControl(m_cameraSptr);

		// 保存相机唯一标示，用于断线后恢复时校验
		GenICam::CStringNode paramDeviceModelName(m_cameraSptr, "DeviceModelName");
		Dahua::Infra::CString val;
		paramDeviceModelName.getValue(val);
		m_camKey = val.c_str();

		// 置已连接标志
		m_bConnectFlg = true;

		// 订阅断线通知事件
		GenICam::IEventSubscribePtr evtSubcribePtr = GenICam::CSystem::getInstance().createEventSubscribe(m_cameraSptr);
		evtSubcribePtr->subscribeConnectArgs(GenICam::ConnectArgProc(&DHCamera::DeviceLinkNotify,this));


		if(NULL == m_trigFinish)
		{
			m_trigFinish = CreateEvent(NULL,FALSE,FALSE,NULL);
		}

		// 创建图片缓存
		if(NULL != m_pBuff)
		{
			delete [] m_pBuff;
		}
		m_buffSize	= m_camPara.Width * m_camPara.Height;
		m_pBuff		= new unsigned char[m_buffSize];

		InitFlg = TRUE;
	}

	return InitFlg;
}

bool DHCamera::OpenCamera(int deviceID)
{
	bool InitFlg = FALSE;
	if(m_bConnectFlg)
	{
		CString l_Str;
		l_Str.Format(_T("相机已处于连接状态"));
		::AfxMessageBox(l_Str);
		return InitFlg;
	}


	m_camPara.DeviceID = deviceID;
	if(m_camPara.DeviceID != -1)
	{
		// 检测通过网卡连接的在线相机
		if(g_DHCameraList.empty())
		{
			EnumCameras();
		}
		
		// 如果指定的DeviceID超过已检测到的相机数量， 表示未找到该编号的相机
		if((size_t)m_camPara.DeviceID >= g_DHCameraList.size())
		{
			//CString l_Str;
			//l_Str.Format(_T("未检测到足够数量相机，当前相机总数%d，指定DeviceID=%d"),DHCameraList.size(), m_camPara.DeviceID);
			//::AfxMessageBox(l_Str);
			return InitFlg;
		}

		// 通过DeviceID找到相机对象
		m_cameraSptr = g_DHCameraList[m_camPara.DeviceID];


		// 连接前自动修改该相机IP为所连接网卡同网段IP
		if(!IpAutoConfig())
		{
			//CString l_Str;
			//l_Str.Format(_T("相机%d[%s] IP自动配置失败"), m_camPara.DeviceID, m_cameraSptr->getKey());
			//::AfxMessageBox(l_Str);
			return InitFlg;
		}

        // 连接设备
        BEGINTIME("connect");
        IntStart = getTimeInUs();
		if(!m_cameraSptr->connect())
		{
			//CString l_Str;
            //l_Str.Format(_T("打开相机%d[%s]失败"),m_camPara.DeviceID, m_cameraSptr->getKey());
			//::AfxMessageBox(l_Str);
			return InitFlg;
		}
        IntEnd = getTimeInUs();
        TRACE("\n***connect total time: %f ms***\n", (IntEnd - IntStart) * 1000 / getDobDff());
        ENDTIME("connect");

        // 创建属性访问类
        m_imageParamCtrlPtr = GenICam::CSystem::getInstance().createImageFormatControl(m_cameraSptr);
        m_analogCtrlPtr = GenICam::CSystem::getInstance().createAnalogControl(m_cameraSptr);
        m_ispParamCtrlPtr = GenICam::CSystem::getInstance().createISPControl(m_cameraSptr);
        m_aquisitionCtrlPtr = GenICam::CSystem::getInstance().createAcquisitionControl(m_cameraSptr);

		// 保存相机唯一标示，用于断线后恢复时校验
		GenICam::CStringNode paramDeviceModelName(m_cameraSptr, "DeviceModelName");
		Dahua::Infra::CString val;
		paramDeviceModelName.getValue(val);
		m_camKey = val.c_str();


		// 置已连接标志
		m_bConnectFlg = true;


		// 订阅断线通知事件
		GenICam::IEventSubscribePtr evtSubcribePtr = GenICam::CSystem::getInstance().createEventSubscribe(m_cameraSptr);
		evtSubcribePtr->subscribeConnectArgs(GenICam::ConnectArgProc(&DHCamera::DeviceLinkNotify,this));


		//根据当前相机型号，调整分辨率、曝光等参数
		
		SetWidth(1280);
		SetHeight(1024);
		m_camPara.Width = GetWidth();
		m_camPara.Height = GetHeight();
		
		SetDefaultParam();

		if(NULL == m_trigFinish)
		{
			m_trigFinish = CreateEvent(NULL,FALSE,FALSE,NULL);
		}

		InitFlg = TRUE;
	}

	return InitFlg;
}



int DHCamera::GetDeviceID()
{
	return m_camPara.DeviceID;
}

bool DHCamera::CloseCamera()						
{
	StopCapture();

	if(NULL != m_cameraSptr.get())
	{
		m_bConnectFlg = false;
		m_cameraSptr->disConnect();
		m_cameraSptr.reset();
	}
	
	if(NULL != m_pBuff)
	{
		delete[] m_pBuff;
		m_pBuff = NULL;
	}

	return TRUE;
}

bool DHCamera::SetCamPropertys(CamPara& Para)
{
	if((NULL == m_cameraSptr.get()) || (!IsConnected()))
	{
		return FALSE;
	}


	SetHeight(Para.Height);
	SetWidth(Para.Width);
	SetOffsetX(Para.OffsetX);
	SetOffsetX(Para.OffsetY);
	SetBrightness(Para.Brightness);
	SetShutter(Para.Shutter);
	SetGain(Para.Gain);
	SetContrast(Para.Contrast);
	SetWorkMode(Para.WorkMode);     
	SetTriggerSource(Para.TrigSource);

	// 色度 锐度 饱和度这里未提供接口，如需要可参考其他属性实现，这几个属性可通过IISPControl类访问到
	//Hue = -1;				//色度
	//Sharpness = -1;		//锐度
	//Saturation = -1;		//饱和度
	return TRUE;
}


bool DHCamera::GetCamPropertys(CamPara &Para)
{
	if((NULL == m_cameraSptr.get()) || (!m_bConnectFlg))
	{
		return FALSE;
	}

	Para.DeviceID = GetDeviceID();
	Para.Brightness = GetBrightness();
	Para.Contrast = GetContrast();
	Para.Gain = GetGain();
	Para.Shutter = GetShutter();
	Para.Height = GetHeight();
	Para.Width = GetWidth();
	Para.WorkMode = GetWorkMode();
	Para.TrigSource = GetTriggerSource();
	return TRUE;
}



WORKMODE DHCamera::GetWorkMode()
{

	int Ret = FALSE;
	if((NULL == m_cameraSptr.get()) || (!m_bConnectFlg))
	{
		return UNKNOW_MODE;
	}

	if(this->m_camPara.DeviceID != -1)
	{
		

		Infra::CString trigModeStr;
		if(m_aquisitionCtrlPtr->triggerMode().isValid())
		{
			m_aquisitionCtrlPtr->triggerMode().getValueSymbol(trigModeStr);
		}
			

		if(trigModeStr == TRIGMODE_ON_STR)
		{
			m_workModeCur = TRIGMODE;
		}
		else
		{
			m_workModeCur = CONMODE;
		}
	}

	return m_workModeCur;
} 

TRIGSOURCE DHCamera::GetTriggerSource()
{
	TRIGSOURCE trigSrc = UNKNOW_SRC;
	if((NULL == m_cameraSptr.get()) || (!m_bConnectFlg))
	{
		return trigSrc;
	}

	if(this->m_camPara.DeviceID != -1)
	{
		Infra::CString trigSrcStr;
		
		if(m_aquisitionCtrlPtr->triggerSource().isValid())
		{
			m_aquisitionCtrlPtr->triggerSource().getValueSymbol(trigSrcStr);	//触发源为软触发
			if(trigSrcStr == TRIGSRC_SOFT_STR)
			{
				trigSrc = TRIG_SOFT;
			}
			else if(trigSrcStr == TRIGSRC_LINE1_STR)
			{
				trigSrc = TRIG_LINE1;
			}
		}
	}

	return trigSrc;
}


bool DHCamera::StartCapture()
{
	if(NULL == m_cameraSptr.get())
	{
		return FALSE;
	}

	m_streamPtr = GenICam::CSystem::getInstance().createStreamSource(m_cameraSptr);
	if(NULL == m_streamPtr.get())
	{
		return FALSE;
	}

	if(!m_streamPtr->attachGrabbing(GenICam::IStreamSource::Proc(&DHCamera::onGetFrame,this)))
	{
		//CString l_Str;
		//l_Str.Format(_T("相机已处于拉流状态"));
		//::AfxMessageBox(l_Str);
		return FALSE;
	}

	// 创建图片缓存
	if(NULL != m_pBuff)
	{
		delete [] m_pBuff;
	}
	try
	{
		m_buffSize	= m_camPara.Width * m_camPara.Height;
		m_pBuff		= new unsigned char[m_buffSize];
		if(!m_streamPtr->startGrabbing())
		{
			//CString l_Str;
			//l_Str.Format(_T("start Grabbing 失败"));
			//::AfxMessageBox(l_Str);
			return FALSE;
		}
	}
	catch(...)
	{
		NULL;

	}


	return TRUE;
}


bool DHCamera::WaitPicture(unsigned char *pbuff, int waitTime)
{
	bool Ret = false;

	if(!m_bConnectFlg)
	{
		return Ret;
	}

	if(TRIGMODE == m_workModeCur)
	{
		if(WaitForSingleObject(m_trigFinish, waitTime) == WAIT_OBJECT_0)
		{
			m_buffMutex.enter();
			memcpy(pbuff,m_pBuff,m_buffSize);
			m_buffMutex.leave();
			ResetEvent(m_trigFinish);
			Ret = TRUE;
		}
	}
	else
	{
		m_waitingTime = waitTime;
		this->m_bCapturingFlg = true;
		while(m_bCapturingFlg)
		{
			//waiting
		}

		m_buffMutex.enter();
		memcpy(pbuff, m_pBuff, m_buffSize);
		m_buffMutex.leave();
		ResetEvent(m_trigFinish);
		Ret = TRUE;
	}
	return Ret;
}



void DHCamera::SetFinish()
{
	SetEvent(m_trigFinish);
}

void DHCamera::ClrFinish()
{
	ResetEvent(m_trigFinish);
}


bool DHCamera::IsConnected()
{
	if((NULL == m_cameraSptr.get()))
	{
		return FALSE;
	}

	return m_cameraSptr->isConnected();
}



bool DHCamera::SetStreamHook(STREAM_PROC proc, void* pObj)
{
	if(NULL != m_pStreamProc)
	{
		return FALSE;
	}
		
	m_pStreamProc = proc;
	m_pUserPtr = pObj;
	return TRUE;
}




bool DHCamera::StopCapture()	
{
	if(m_streamPtr)
	{
		m_streamPtr->stopGrabbing();
		m_streamPtr->detachGrabbing(GenICam::IStreamSource::Proc(&DHCamera::onGetFrame,this));			
		m_streamPtr.reset();//销毁流对象
	}
	return TRUE;
}

void DHCamera::DeviceLinkNotify(const GenICam::SConnectArg& conArg)
{
	if(GenICam::SConnectArg::EVType::offLine == conArg.m_event)
	{
		m_bConnectFlg = false;
		TRACE("DeviceLinkNotify: camera disconnected\n");
	}
	else if(GenICam::SConnectArg::EVType::onLine == conArg.m_event)
	{
		TRACE("DeviceLinkNotify: camera online, start to reconnect\n");
		TryResumeConn();
	}
}


void DHCamera::TryResumeConn()
{
	if(m_camPara.DeviceID != -1)
	{
		// 连接前自动修改为网卡同网段IP
		IpAutoConfig();
		if(!m_cameraSptr->connect())
		{
			return;
		}

//		TRACE("TryResumeConn: reconnect camera success\n");
	
//		TRACE("TryResumeConn: try to recovery camera configuration\n");
//		SetDefaultParam();

		// 置已连接标志
		m_bConnectFlg = true;

//		TRACE("TryResumeConn: try to recovery stream data\n");
		// 恢复拉流状态
		StartCapture();
	}
}

bool DHCamera::IpAutoConfig()
{
	if(NULL == m_cameraSptr.get())
	{
		return false;
	}

	// 获取设备所连接网卡IP submask
	GenICam::IGigEInterfacePtr gigeInterfacePtr = GenICam::IGigEInterface::getInstance(m_cameraSptr);
	if(NULL == gigeInterfacePtr.get())
	{
		return false;
	}
	unsigned long InterfaceIpValue = ntohl(inet_addr(gigeInterfacePtr->getIpAddress().c_str()));
	unsigned long InterfaceMaskValue = ntohl(inet_addr(gigeInterfacePtr->getSubnetMask().c_str()));

	// 获取设备IP
	GenICam::IGigECameraPtr gigeDHCameraPtr = GenICam::IGigECamera::getInstance(m_cameraSptr);
	if(NULL == gigeDHCameraPtr.get())
	{
		return false;
	}
	unsigned long devIpValue = ntohl(inet_addr(gigeDHCameraPtr->getIpAddress().c_str()));

	// 检测相机与网卡是否在同网段
	if((InterfaceIpValue & InterfaceMaskValue) == (devIpValue & InterfaceMaskValue))
	{
		// 已在同网段，不需要重新配置相机IP，认为成功
		return true;
	}

	// 取同网段随机IP（与网卡IP不同）
	unsigned char newIPStr[20] = {0}; //新IP地址
	unsigned long newIpValue = 0;
	while(1)
	{
		unsigned long newIpValue = rand() % 254 + 1; //1~254
		if(newIpValue != (InterfaceIpValue & 0xff))
		{
			newIpValue = (InterfaceIpValue & 0xFFFFFF00) + newIpValue;
			
			struct in_addr   stInAddr;
			stInAddr.s_addr	= htonl(newIpValue);
			memcpy(newIPStr, inet_ntoa(stInAddr), strlen(inet_ntoa(stInAddr)));

			break;
		}
	}

	return gigeDHCameraPtr->forceIpAddress((const char*)newIPStr, gigeInterfacePtr->getSubnetMask().c_str(), gigeInterfacePtr->getGateway().c_str() );

  }


bool DHCamera::TriggerSoft()
{
    IntStart = getTimeInUs();
    // 设备是否在线
    if(!IsConnected())
	{
		return false;
	}

    // 设置触发模式
    if (TRIGMODE != m_workModeCur)
    {
        SetWorkMode(TRIGMODE);
    }

    // 设置触发源
    if (TRIG_SOFT != GetTriggerSource())
    {
        SetTriggerSource(TRIG_SOFT);
    }

	SetEvent(m_trigFinish);

	// 获取软触发属性（命令类属性），软触发
	bool rtn = m_aquisitionCtrlPtr->triggerSoftware().execute();
	if(!rtn)
	{
		TRACE("software trigger failed\n");
	}
	else
	{
		TRACE("TriggerSoft():software trigger Success...\n");
	}
    
    IntEnd = getTimeInUs();

    TRACE("\n*** TriggerSoft [total time]: %f ms***\n", (IntEnd - IntStart) * 1000 / getDobDff());

	return rtn;
}


void DHCamera::onGetFrame(const GenICam::CFrame& frame)
{
	m_buffSize = frame.getImageSize();
	uint32_t imgSize = frame.getImageSize();
	if(m_buffSize < imgSize)
	{
		TRACE("onGetFrame:buff size too smaller, m_buffSize=%u, imageSize=%u\n", m_buffSize, imgSize);
		return;
	}

	m_buffMutex.enter();
	if (m_pBuff == NULL)
	{
		m_pBuff = new unsigned char[imgSize];
	}
	memcpy(m_pBuff, frame.getImage(), imgSize); //event :m_pBuffer;
	if(NULL != m_pStreamProc)
	{
		m_pStreamProc(m_pBuff, m_pUserPtr); //para1: Src    //para2:Dst
	}	
	m_buffMutex.leave();

	TRACE("onGetFrame:get one image ...%"PRIu64"\n",frame.getBlockId());

	//Sleep(100);
	m_bCapturingFlg = false;
	TRACE("onGetFrame[一帧数据到达 ]:m_buffSize=%u, imageSize=%u\n", m_buffSize, imgSize);
	SetFinish(); //获取一帧图像后，置位事件标志。
}

//====================================================



bool DHCamera::SetWidth(int Value)
{
	bool ret = FALSE;
	if((NULL == m_cameraSptr.get()) || (!m_bConnectFlg) || (Value < 0))
	{
		return ret;
	}
 
	ret = m_imageParamCtrlPtr->width().setValue(Value);
	if(ret)
	{
		m_camPara.Width = Value;
	}
	return ret;
}

bool DHCamera::SetHeight(int Value)
{
	bool ret = FALSE;
	if((NULL == m_cameraSptr.get()) /*|| (!m_bConnectFlg)*/ || (Value < 0))
	{
		return ret;
	}

	ret = m_imageParamCtrlPtr->height().setValue(Value);
	if(ret)
	{
		m_camPara.Height = Value;
	}

	return ret;
}

bool DHCamera::SetOffsetX(int Value)
{
	bool ret = FALSE;
	if((NULL == m_cameraSptr.get()) || (!m_bConnectFlg) || (Value < 0))
	{
		return ret;
	}
	
	ret = m_imageParamCtrlPtr->offsetX().setValue(Value);
	return ret;
}

bool DHCamera::SetOffsetY(int Value)
{
	bool ret = FALSE;
	if((NULL == m_cameraSptr.get()) /*|| (!m_bConnectFlg)*/ || (Value < 0))
	{
		return ret;
	}

	ret = m_imageParamCtrlPtr->offsetY().setValue(Value);
	return ret;
}


bool DHCamera::SetGain(int Value)			
{
	if((NULL == m_cameraSptr.get()) /*|| (!m_bConnectFlg)*/ || (Value < 0))
	{
		return FALSE;
	}

	m_analogCtrlPtr->gainAuto().setValueBySymbol(GAINAUTO_OFF_STR);
	return m_analogCtrlPtr->gainRaw().setValue((double)Value);
}

bool DHCamera::SetBrightness(int Value)
{
	if((NULL == m_cameraSptr.get())/* || || (!m_bConnectFlg)*/ || (Value < 0))
	{
		return FALSE;
	}

	return m_ispParamCtrlPtr->brightness().setValue((int64_t)Value);//亮度1-100
}

bool DHCamera::SetShutter(double Value)
{
	if((NULL == m_cameraSptr.get()) /*|| (!m_bConnectFlg)*/ || (Value < 0))
	{
		return FALSE;
	}

	m_aquisitionCtrlPtr->exposureAuto().setValueBySymbol(EXPOSUREAUTO_OFF_STR);
	bool rtn = m_aquisitionCtrlPtr->exposureTime().setValue(Value);//曝光时间0-65534
	if(!rtn)
	{
		TRACE("SetShutter:set shutter fail\n");
	}
	return rtn;
}

bool DHCamera::SetContrast(int Value)
{
	if((NULL == m_cameraSptr.get()) /*|| (!m_bConnectFlg)*/ || (Value < 0))
	{
		return FALSE;
	}

	bool rtn = m_ispParamCtrlPtr->contrast().setValue((int64_t)Value);	//对比度1-100
	if(!rtn)
	{
		TRACE("SetContrast:set SetContrast fail\n");
	}

	return rtn;
}


bool DHCamera::SetWorkMode(WORKMODE Mode)
{
	bool Ret = false;
	if((NULL == m_cameraSptr.get()) /*|| (!m_bConnectFlg)*/ || (Mode < 0))
	{
		return Ret;
	}

	if(this->m_camPara.DeviceID != -1)
	{
		m_workModeCur = Mode;
		if(m_workModeCur == CONMODE)
		{
			Ret = m_aquisitionCtrlPtr->triggerMode().setValueBySymbol(TRIGMODE_OFF_STR);		//连续模式
			if(!Ret)
			{
				TRACE("SetWorkMode:set triggerMode  continue  fail\n");
			}
		}
		else if(m_workModeCur == TRIGMODE)
		{
			Ret = m_aquisitionCtrlPtr->triggerMode().setValueBySymbol(TRIGMODE_ON_STR);			//触发模式
			if(!Ret)
			{
				TRACE("SetWorkMode:set triggerMode trigger Fail\n");
			}
		}
	}
	return Ret;
}


bool DHCamera::SetTriggerSource(TRIGSOURCE trigSrc)
{
	bool Ret = false;
	if((NULL == m_cameraSptr.get()) /*|| (!m_bConnectFlg)*/ || (trigSrc < 0))
	{
		return Ret;
	}

	if(this->m_camPara.DeviceID != -1)
	{
		if(TRIG_SOFT == trigSrc)
		{
			m_aquisitionCtrlPtr->triggerSource().setValueBySymbol(TRIGSRC_SOFT_STR);	//触发源为软触发
		}
		else if(TRIG_LINE1 == trigSrc)
		{
			m_aquisitionCtrlPtr->triggerSource().setValueBySymbol(TRIGSRC_LINE1_STR);	//触发源为LINE触发
		}
	}

	return Ret;
}

int	DHCamera::GetWidth()
{
	if((NULL == m_cameraSptr.get()) || (!m_bConnectFlg))
	{
		return -1;
	}

	int64_t tmpValue;
	m_imageParamCtrlPtr->width().getValue(tmpValue);
	return (int)tmpValue;
}

int	DHCamera::GetHeight()
{
	if((NULL == m_cameraSptr.get()) || (!m_bConnectFlg))
	{
		return -1;
	}

	int64_t tmpValue;
	m_imageParamCtrlPtr->height().getValue(tmpValue);
	return (int)tmpValue;
}

int	DHCamera::GetOffsetX()
{
	if((NULL == m_cameraSptr.get()) || (!m_bConnectFlg))
	{
		return -1;
	}
	
	int64_t tmpValue;
	m_imageParamCtrlPtr->offsetX().getValue(tmpValue);
	return (int)tmpValue;
}

int	DHCamera::GetOffsetY()
{
	if((NULL == m_cameraSptr.get()) || (!m_bConnectFlg))
	{
		return -1;
	}
	
	int64_t tmpValue;
	m_imageParamCtrlPtr->offsetY().getValue(tmpValue);
	return (int)tmpValue;
}


int	DHCamera::GetGain()
{
	if((NULL == m_cameraSptr.get()) || (!m_bConnectFlg))
	{
		return -1;
	}

	double tempValue;
	m_analogCtrlPtr->gainRaw().getValue(tempValue);//增益0-63

	return (int)tempValue;
}

int	DHCamera::GetBrightness()
{
	if((NULL == m_cameraSptr.get()) || (!m_bConnectFlg))
	{
		return -1;
	}

	int64_t tmpValue;
	m_ispParamCtrlPtr->brightness().getValue(tmpValue);//亮度1-100
	return (int)tmpValue;
}

int	DHCamera::GetShutter()
{
	if((NULL == m_cameraSptr.get()) || (!m_bConnectFlg))
	{
		return -1;
	}

	double tmpValue;
	if(m_aquisitionCtrlPtr->exposureTime().isValid())
	{
		if(m_aquisitionCtrlPtr->exposureTime().getValue(tmpValue))//曝光时间0-65534
			TRACE("GetShutter:exposureTime = %f\n", tmpValue);
	}


	return (int)tmpValue;
}

int	DHCamera::GetContrast()
{
	if((NULL == m_cameraSptr.get()) || (!m_bConnectFlg))
	{
		return -1;
	}

	int64_t tmpValue;
	m_ispParamCtrlPtr->contrast().getValue(tmpValue);	//对比度1-100

	return (int)tmpValue;
}

bool DHCamera::GetXRevise()
{
	if((NULL == m_cameraSptr.get()) || (!m_bConnectFlg))
	{
		return false;
	}

	bool xRev;
	m_imageParamCtrlPtr->reverseX().getValue(xRev);
	return xRev;
}

bool DHCamera::GetYRevise()
{
	if((NULL == m_cameraSptr.get()) || (!m_bConnectFlg))
	{
		return false;
	}

	bool yRev;
	m_imageParamCtrlPtr->reverseY().getValue(yRev);

	return yRev;
}

bool DHCamera::SetXRevise(bool ReviseOrNot) //设置X方向是否翻转
{
	if((NULL == m_cameraSptr.get()) || (!m_bConnectFlg))
	{
		return false;
	}

	return m_imageParamCtrlPtr->reverseX().setValue(ReviseOrNot);
}


bool DHCamera::SetYRevise(bool ReviseOrNot) //设置Y方向是否翻转
{
	if((NULL == m_cameraSptr.get()) || (!m_bConnectFlg))
	{
		return false;
	}

	return m_imageParamCtrlPtr->reverseY().setValue(ReviseOrNot);
}


void DHCamera::SetDefaultParam()
{
	if((NULL == m_cameraSptr.get()) || (!m_bConnectFlg))
	{
		return;
	}

	m_camPara.WorkMode = CONMODE;
	m_camPara.TrigSource = TRIG_SOFT;

	//30w
	if(m_camKey.find("A5031") != std::string::npos)
	{
		//TODO 写死分辨率、增益、曝光
		SetCamPropertys(m_camPara);
	}
	//50w
	else if(m_camKey.find("A5051") != std::string::npos)
	{
		//TODO 写死分辨率、增益、曝光
		SetCamPropertys(m_camPara);
	}
	//130w
	else if(m_camKey.find("A5131") != std::string::npos)
	{
		//TODO 写死分辨率、增益、曝光
		SetCamPropertys(m_camPara);
	}
	//4K线阵
	else if(m_camKey.find("L5043") != std::string::npos)
	{
		SetCamPropertys(m_camPara);
	}
/*	//500w
	else if(m_camKey.find("A5501") != std::string::npos)
	{
		
	} 
*/
}

__int64 DHCamera::getTimeInUs()
{
    QueryPerformanceCounter(&largeInteger);
    return largeInteger.QuadPart;
}

double DHCamera::getDobDff()
{
    QueryPerformanceFrequency(&largeInteger);
    return largeInteger.QuadPart;
}






uint64_t DHCamera::GetImageSize()
{
	return m_buffSize;
}
