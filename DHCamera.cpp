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

// ͳ�ƽӿ�ʱ��
SYSTEMTIME  sysTime;                            // ϵͳʱ�䣬��ȷ�����뼶

#define BEGINTIME(operation) {GetLocalTime(&sysTime);TRACE("\n ***%s begin time:minute:%d, second:%d,  millisecond:%d***\n", operation, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);}

#define ENDTIME(operation) {GetLocalTime(&sysTime);TRACE("\n ***%s end time:minute:%d, second:%d,  millisecond:%d***\n", operation, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);}

LARGE_INTEGER   largeInteger;                   // ϵͳʱ�䣬��ȷ��΢�
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
	// ���ͨ���������ӵ��������
	g_DHCameraList.clear();
	Infra::TVector<GenICam::ICameraPtr>	tempCameras;

    BEGINTIME("discovery");
    IntStart = getTimeInUs();
	if (!GenICam::CSystem::getInstance().discovery(tempCameras, GenICam::CSystem::EInterfaceType::typeGige))
	{
		CString l_Str;
		l_Str.Format(_T("����Dahua ���ʧ��"));
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
		l_Str.Format(_T("����Ѵ�������״̬"));
		::AfxMessageBox(l_Str);
		return InitFlg;
	}

	m_camPara = Para;

	if(m_camPara.DeviceID != -1)
	{
		// ���ͨ���������ӵ��������
		if(g_DHCameraList.empty())
		{
			EnumCameras();
		}
		
		// ���ָ����DeviceID�����Ѽ�⵽����������� ��ʾδ�ҵ��ñ�ŵ����
		if((size_t)m_camPara.DeviceID >= g_DHCameraList.size())
		{
			return InitFlg;
		}

		// ͨ��DeviceID�ҵ��������
		m_cameraSptr = g_DHCameraList[m_camPara.DeviceID];


		// ����ǰ�Զ��޸ĸ����IPΪ����������ͬ����IP
		if(!IpAutoConfig())
		{
			CString l_Str;
			l_Str.Format(_T("���%d[%s] IP�Զ�����ʧ��"), m_camPara.DeviceID, m_cameraSptr->getName());
			::AfxMessageBox(l_Str);
			return InitFlg;
		}

		// �����豸
		if(!m_cameraSptr->connect())
		{
			CString l_Str;
            l_Str.Format(_T("�����%d [%s]ʧ��"),m_camPara.DeviceID, m_cameraSptr->getKey());
			::AfxMessageBox(l_Str);
			return InitFlg;
		}

        // �������Է�����
        m_imageParamCtrlPtr = GenICam::CSystem::getInstance().createImageFormatControl(m_cameraSptr);
        m_analogCtrlPtr = GenICam::CSystem::getInstance().createAnalogControl(m_cameraSptr);
        m_ispParamCtrlPtr = GenICam::CSystem::getInstance().createISPControl(m_cameraSptr);
        m_aquisitionCtrlPtr = GenICam::CSystem::getInstance().createAcquisitionControl(m_cameraSptr);

		// �������Ψһ��ʾ�����ڶ��ߺ�ָ�ʱУ��
		GenICam::CStringNode paramDeviceModelName(m_cameraSptr, "DeviceModelName");
		Dahua::Infra::CString val;
		paramDeviceModelName.getValue(val);
		m_camKey = val.c_str();

		// �������ӱ�־
		m_bConnectFlg = true;

		// ���Ķ���֪ͨ�¼�
		GenICam::IEventSubscribePtr evtSubcribePtr = GenICam::CSystem::getInstance().createEventSubscribe(m_cameraSptr);
		evtSubcribePtr->subscribeConnectArgs(GenICam::ConnectArgProc(&DHCamera::DeviceLinkNotify,this));


		if(NULL == m_trigFinish)
		{
			m_trigFinish = CreateEvent(NULL,FALSE,FALSE,NULL);
		}

		// ����ͼƬ����
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
		l_Str.Format(_T("����Ѵ�������״̬"));
		::AfxMessageBox(l_Str);
		return InitFlg;
	}


	m_camPara.DeviceID = deviceID;
	if(m_camPara.DeviceID != -1)
	{
		// ���ͨ���������ӵ��������
		if(g_DHCameraList.empty())
		{
			EnumCameras();
		}
		
		// ���ָ����DeviceID�����Ѽ�⵽����������� ��ʾδ�ҵ��ñ�ŵ����
		if((size_t)m_camPara.DeviceID >= g_DHCameraList.size())
		{
			//CString l_Str;
			//l_Str.Format(_T("δ��⵽�㹻�����������ǰ�������%d��ָ��DeviceID=%d"),DHCameraList.size(), m_camPara.DeviceID);
			//::AfxMessageBox(l_Str);
			return InitFlg;
		}

		// ͨ��DeviceID�ҵ��������
		m_cameraSptr = g_DHCameraList[m_camPara.DeviceID];


		// ����ǰ�Զ��޸ĸ����IPΪ����������ͬ����IP
		if(!IpAutoConfig())
		{
			//CString l_Str;
			//l_Str.Format(_T("���%d[%s] IP�Զ�����ʧ��"), m_camPara.DeviceID, m_cameraSptr->getKey());
			//::AfxMessageBox(l_Str);
			return InitFlg;
		}

        // �����豸
        BEGINTIME("connect");
        IntStart = getTimeInUs();
		if(!m_cameraSptr->connect())
		{
			//CString l_Str;
            //l_Str.Format(_T("�����%d[%s]ʧ��"),m_camPara.DeviceID, m_cameraSptr->getKey());
			//::AfxMessageBox(l_Str);
			return InitFlg;
		}
        IntEnd = getTimeInUs();
        TRACE("\n***connect total time: %f ms***\n", (IntEnd - IntStart) * 1000 / getDobDff());
        ENDTIME("connect");

        // �������Է�����
        m_imageParamCtrlPtr = GenICam::CSystem::getInstance().createImageFormatControl(m_cameraSptr);
        m_analogCtrlPtr = GenICam::CSystem::getInstance().createAnalogControl(m_cameraSptr);
        m_ispParamCtrlPtr = GenICam::CSystem::getInstance().createISPControl(m_cameraSptr);
        m_aquisitionCtrlPtr = GenICam::CSystem::getInstance().createAcquisitionControl(m_cameraSptr);

		// �������Ψһ��ʾ�����ڶ��ߺ�ָ�ʱУ��
		GenICam::CStringNode paramDeviceModelName(m_cameraSptr, "DeviceModelName");
		Dahua::Infra::CString val;
		paramDeviceModelName.getValue(val);
		m_camKey = val.c_str();


		// �������ӱ�־
		m_bConnectFlg = true;


		// ���Ķ���֪ͨ�¼�
		GenICam::IEventSubscribePtr evtSubcribePtr = GenICam::CSystem::getInstance().createEventSubscribe(m_cameraSptr);
		evtSubcribePtr->subscribeConnectArgs(GenICam::ConnectArgProc(&DHCamera::DeviceLinkNotify,this));


		//���ݵ�ǰ����ͺţ������ֱ��ʡ��ع�Ȳ���
		
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

	// ɫ�� ��� ���Ͷ�����δ�ṩ�ӿڣ�����Ҫ�ɲο���������ʵ�֣��⼸�����Կ�ͨ��IISPControl����ʵ�
	//Hue = -1;				//ɫ��
	//Sharpness = -1;		//���
	//Saturation = -1;		//���Ͷ�
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
			m_aquisitionCtrlPtr->triggerSource().getValueSymbol(trigSrcStr);	//����ԴΪ����
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
		//l_Str.Format(_T("����Ѵ�������״̬"));
		//::AfxMessageBox(l_Str);
		return FALSE;
	}

	// ����ͼƬ����
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
			//l_Str.Format(_T("start Grabbing ʧ��"));
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
		m_streamPtr.reset();//����������
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
		// ����ǰ�Զ��޸�Ϊ����ͬ����IP
		IpAutoConfig();
		if(!m_cameraSptr->connect())
		{
			return;
		}

//		TRACE("TryResumeConn: reconnect camera success\n");
	
//		TRACE("TryResumeConn: try to recovery camera configuration\n");
//		SetDefaultParam();

		// �������ӱ�־
		m_bConnectFlg = true;

//		TRACE("TryResumeConn: try to recovery stream data\n");
		// �ָ�����״̬
		StartCapture();
	}
}

bool DHCamera::IpAutoConfig()
{
	if(NULL == m_cameraSptr.get())
	{
		return false;
	}

	// ��ȡ�豸����������IP submask
	GenICam::IGigEInterfacePtr gigeInterfacePtr = GenICam::IGigEInterface::getInstance(m_cameraSptr);
	if(NULL == gigeInterfacePtr.get())
	{
		return false;
	}
	unsigned long InterfaceIpValue = ntohl(inet_addr(gigeInterfacePtr->getIpAddress().c_str()));
	unsigned long InterfaceMaskValue = ntohl(inet_addr(gigeInterfacePtr->getSubnetMask().c_str()));

	// ��ȡ�豸IP
	GenICam::IGigECameraPtr gigeDHCameraPtr = GenICam::IGigECamera::getInstance(m_cameraSptr);
	if(NULL == gigeDHCameraPtr.get())
	{
		return false;
	}
	unsigned long devIpValue = ntohl(inet_addr(gigeDHCameraPtr->getIpAddress().c_str()));

	// �������������Ƿ���ͬ����
	if((InterfaceIpValue & InterfaceMaskValue) == (devIpValue & InterfaceMaskValue))
	{
		// ����ͬ���Σ�����Ҫ�����������IP����Ϊ�ɹ�
		return true;
	}

	// ȡͬ�������IP��������IP��ͬ��
	unsigned char newIPStr[20] = {0}; //��IP��ַ
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
    // �豸�Ƿ�����
    if(!IsConnected())
	{
		return false;
	}

    // ���ô���ģʽ
    if (TRIGMODE != m_workModeCur)
    {
        SetWorkMode(TRIGMODE);
    }

    // ���ô���Դ
    if (TRIG_SOFT != GetTriggerSource())
    {
        SetTriggerSource(TRIG_SOFT);
    }

	SetEvent(m_trigFinish);

	// ��ȡ�������ԣ����������ԣ�������
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
	TRACE("onGetFrame[һ֡���ݵ��� ]:m_buffSize=%u, imageSize=%u\n", m_buffSize, imgSize);
	SetFinish(); //��ȡһ֡ͼ�����λ�¼���־��
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

	return m_ispParamCtrlPtr->brightness().setValue((int64_t)Value);//����1-100
}

bool DHCamera::SetShutter(double Value)
{
	if((NULL == m_cameraSptr.get()) /*|| (!m_bConnectFlg)*/ || (Value < 0))
	{
		return FALSE;
	}

	m_aquisitionCtrlPtr->exposureAuto().setValueBySymbol(EXPOSUREAUTO_OFF_STR);
	bool rtn = m_aquisitionCtrlPtr->exposureTime().setValue(Value);//�ع�ʱ��0-65534
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

	bool rtn = m_ispParamCtrlPtr->contrast().setValue((int64_t)Value);	//�Աȶ�1-100
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
			Ret = m_aquisitionCtrlPtr->triggerMode().setValueBySymbol(TRIGMODE_OFF_STR);		//����ģʽ
			if(!Ret)
			{
				TRACE("SetWorkMode:set triggerMode  continue  fail\n");
			}
		}
		else if(m_workModeCur == TRIGMODE)
		{
			Ret = m_aquisitionCtrlPtr->triggerMode().setValueBySymbol(TRIGMODE_ON_STR);			//����ģʽ
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
			m_aquisitionCtrlPtr->triggerSource().setValueBySymbol(TRIGSRC_SOFT_STR);	//����ԴΪ����
		}
		else if(TRIG_LINE1 == trigSrc)
		{
			m_aquisitionCtrlPtr->triggerSource().setValueBySymbol(TRIGSRC_LINE1_STR);	//����ԴΪLINE����
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
	m_analogCtrlPtr->gainRaw().getValue(tempValue);//����0-63

	return (int)tempValue;
}

int	DHCamera::GetBrightness()
{
	if((NULL == m_cameraSptr.get()) || (!m_bConnectFlg))
	{
		return -1;
	}

	int64_t tmpValue;
	m_ispParamCtrlPtr->brightness().getValue(tmpValue);//����1-100
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
		if(m_aquisitionCtrlPtr->exposureTime().getValue(tmpValue))//�ع�ʱ��0-65534
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
	m_ispParamCtrlPtr->contrast().getValue(tmpValue);	//�Աȶ�1-100

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

bool DHCamera::SetXRevise(bool ReviseOrNot) //����X�����Ƿ�ת
{
	if((NULL == m_cameraSptr.get()) || (!m_bConnectFlg))
	{
		return false;
	}

	return m_imageParamCtrlPtr->reverseX().setValue(ReviseOrNot);
}


bool DHCamera::SetYRevise(bool ReviseOrNot) //����Y�����Ƿ�ת
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
		//TODO д���ֱ��ʡ����桢�ع�
		SetCamPropertys(m_camPara);
	}
	//50w
	else if(m_camKey.find("A5051") != std::string::npos)
	{
		//TODO д���ֱ��ʡ����桢�ع�
		SetCamPropertys(m_camPara);
	}
	//130w
	else if(m_camKey.find("A5131") != std::string::npos)
	{
		//TODO д���ֱ��ʡ����桢�ع�
		SetCamPropertys(m_camPara);
	}
	//4K����
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
