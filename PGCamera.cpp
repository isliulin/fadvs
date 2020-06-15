#include "stdafx.h"
#include "PGCamera.h"

#pragma comment(lib,"PointGrey/lib/FlyCapture2.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static VideoMode g_fvmFormatVideoMode[8][8] =
{
	{
		VIDEOMODE_160x120YUV444,
			VIDEOMODE_320x240YUV422 ,
			VIDEOMODE_640x480YUV411,
			VIDEOMODE_640x480YUV422,
			VIDEOMODE_640x480RGB,
			VIDEOMODE_640x480Y8,
			VIDEOMODE_640x480Y16,
	},
	{
		VIDEOMODE_800x600YUV422 ,
			VIDEOMODE_800x600RGB,
			VIDEOMODE_800x600Y8,		
			VIDEOMODE_1024x768YUV422,
			VIDEOMODE_1024x768RGB,
			VIDEOMODE_1024x768Y8,
			VIDEOMODE_800x600Y16,
			VIDEOMODE_1024x768Y16
		},
		{
			VIDEOMODE_1280x960YUV422 ,
				VIDEOMODE_1280x960RGB,
				VIDEOMODE_1280x960Y8,		
				VIDEOMODE_1600x1200YUV422,
				VIDEOMODE_1600x1200RGB,
				VIDEOMODE_1600x1200Y8,
				VIDEOMODE_1280x960Y16,
				VIDEOMODE_1600x1200Y16
		},
		{
			VIDEOMODE_FORCE_32BITS
			},
			{
				VIDEOMODE_FORCE_32BITS,
			},
			{
				VIDEOMODE_FORCE_32BITS,
				},
				{
					VIDEOMODE_FORCE_32BITS,
				},
				{
					VIDEOMODE_FORMAT7,
					},
};

static FrameRate g_frFrameRate[] = 
{
	FRAMERATE_1_875,				// 1.875 fps. (Frames per second)
	FRAMERATE_3_75,					// 3.75 fps.
	FRAMERATE_7_5,					// 7.5 fps.
	FRAMERATE_15,					// 15 fps.
	FRAMERATE_30,					// 30 fps.
	FRAMERATE_60,					// 60 fps.
	FRAMERATE_120,					// 120 fps.
	FRAMERATE_240,					// 240 fps.
	FRAMERATE_FORMAT7               // Format7
};

static float g_fFrameRate[] = 
{
	1.875,
	3.75,
	7.5,
	15.0,
	30.0,
	60.0,
	120.0,
	240.0,
	-1
};

std::auto_ptr<BusManager> CPGCamera::m_AutoBusManager = std::auto_ptr<BusManager>(NULL);

CPGCamera::CPGCamera(PGGrabImageMode grabImageMode)
{
	m_grabImageMode = grabImageMode;
	m_pCamera = NULL;
	m_bOnline = false;
	m_pMainFrm = NULL;
	m_funcCallback = NULL;
	m_bSendCallbackEnable = true;
	m_pGrabImage = NULL;
	m_lWidth = 0;
	m_lHeight = 0;
	m_bIsGrabbing = false;
	m_bIsSnap = false;
	m_threadStatus = ePGIdle;
	m_bAcquireSuccess = false;
	m_bFreezeStop = false;
	m_hEventSnapped = NULL;
	m_triggerMode = ePGNotTrigger;

	// 线程采集图像相关
	m_pWinThread = NULL;
	m_hEventToRun = NULL;
	m_hEventExit = NULL;
	m_bTerminate = true;
	m_bHaveStopped = true;

	m_cameraType = eInterfaceUnknow;

	// 临界区
	InitializeCriticalSection(&m_criticalSection);

	m_bSendGrabTrigger = false;
	m_nBytes = 0;
	m_nCameraIndex = 0;

	m_nFrameCounter = 0;
	m_nMissCounter = 0;
	m_nPreFrameCounter = 0;
	m_nAfterFrameCounter = 0;
	m_bIsFrameCounter = TRUE;

	m_nTimeSFrameCounter = 0;
	m_nTimeSMissCounter = 0;
	m_nPreTimeSFrameCounter = 0;
	m_nAfterTimeSFrameCounter = 0;
	m_bIsTimeStampCounter = TRUE;
	m_nDeltaTimeStamp = 0;
	m_nNotStepTimeStamp = 0;
	m_nNotStepAll = 0;

	m_lSeconds = 0;
	m_nMicroSeconds = 0;
	m_nCycleSeconds = 0;
	m_nCycleCount = 0;
	m_nCycleOffset = 0;
}

CPGCamera::~CPGCamera()
{
	if(eWaitEvent == m_grabImageMode) // 事件采集方式
	{
		Freeze();
		CloseHandle(m_hEventSnapped);
		m_hEventSnapped = NULL;
	}
	else // 线程采集方式
	{
		TerminateGrabThread();
	}

	// 关闭触发
	CloseTriggerMode();

	// 断开连接
	if(m_pCamera)
	{
		m_pCamera->SetCallback(NULL,NULL);
		m_pCamera->StopCapture();
		m_pCamera->Disconnect();
		delete m_pCamera;
		m_pCamera = NULL;
	}
	m_bOnline = false;

	// 清空
	if(m_pGrabImage!=NULL)
	{
		delete[] m_pGrabImage;
		m_pGrabImage = NULL;
		m_lWidth = 0;
		m_lHeight = 0;
	}
	DeleteCriticalSection(&m_criticalSection);
}

// 获取相机的相关信息
bool CPGCamera::GetCameraInfo(CameraInfo *pCameraInfo)
{
	CString strError;
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		strError.Format("m_bOnline = %d",(int)m_bOnline);
		AfxMessageBox(strError);
		return false;
	}
	Error error;
	error = m_pCamera->GetCameraInfo(pCameraInfo);
	strError.Format("GetCameraInfo = %d",(int)error.GetType());
	//AfxMessageBox(strError);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	return true;
}

// 获取相机序列号
bool CPGCamera::GetCameraSerialNumber(unsigned int *pSerialNumber)
{
	CameraInfo cameraInfo;
	if(!GetCameraInfo(&cameraInfo))
	{
		return false;
	}
	*pSerialNumber = cameraInfo.serialNumber;
	return true;
}

// 获取相机索引
bool CPGCamera::GetCameraIndex(unsigned int *pCameraIndex)
{
	unsigned int nCnt;
	unsigned int nSerialNum;
	if(!GetCameraSerialNumber(&nSerialNum))
	{
		return false;
	}
	if(!GetBusCameraCount(&nCnt))
	{
		return false;
	}
	PGCameraInfo *pCameraInfo = new PGCameraInfo[nCnt];
	if(!EnumerateBusCameras(pCameraInfo,&nCnt))
	{
		delete[] pCameraInfo;
		pCameraInfo = NULL;
		return false;
	}
	for(int j=0;j<nCnt;j++)
	{
		if(pCameraInfo[j].m_uiSerialNum == nSerialNum)
		{
			*pCameraIndex = pCameraInfo[j].m_uiIndex;
			delete[] pCameraInfo;
			pCameraInfo = NULL;
			return true;
		}
	}
	delete[] pCameraInfo;
	pCameraInfo = NULL;
	return false;
}

// 设置回调函数参数
void CPGCamera::SetOwner(void *pOwner,PGGrabbedCallback funcCallback)
{
	m_pMainFrm = pOwner;
	m_funcCallback = funcCallback;
}

// 设置是否调用回调函数
void CPGCamera::SetSendCallbackEnable(bool bEnable)
{
	m_bSendCallbackEnable = bEnable;
}

// 获取是否调用回调函数
bool CPGCamera::GetSendCallbackEnable()
{
	return m_bSendCallbackEnable;
}

// 采集完成调用回调函数
void CPGCamera::SendCallback(PGStatus status)
{
	// 调用回调函数
	if((m_funcCallback!=NULL)&&(m_pGrabImage!=NULL))
	{
		if(ePGGrabOK == status)
		{
			m_funcCallback(m_pMainFrm,status,m_pGrabImage,m_lWidth,m_lHeight,m_pixelFormat);
		}
		else
		{
			m_funcCallback(m_pMainFrm,status,NULL,NULL,NULL,ePGUnknown);
		}
	}
}

// eWaitEvent采集模式下的内部采集触发回调函数
void CPGCamera::PGImageEventCallback(Image *pImage,const void *pCallbackData)
{
	CPGCamera *pThis = (CPGCamera *)pCallbackData;
	pThis->m_bAcquireSuccess = false;
	if(!pImage||!pImage->GetData())
	{
		Sleep(2);
		return;
	}
	if(ePGGrab == pThis->m_threadStatus) // 连续采集
	{
		if(pThis->m_bIsGrabbing)
		{
			PGStatus status = ePGGrabOK;
			pThis->m_ImageRaw.DeepCopy(pImage);
			if(!pThis->TransferData())
			{
				status = ePGGrabFailed;
			}
			else
			{
				pThis->m_bAcquireSuccess = true;
			}
			if(pThis->m_bSendCallbackEnable)
			{
				pThis->SendCallback(status);
			}
		}
	}
	else if(ePGSnap == pThis->m_threadStatus) // 单帧采集
	{
		if(pThis->m_bIsSnap)
		{
			PGStatus status = ePGGrabOK;
			pThis->m_ImageRaw.DeepCopy(pImage);
			if(!pThis->TransferData())
			{
				status = ePGGrabFailed;
			}
			else
			{
				pThis->m_bAcquireSuccess = true;
			}
			if(pThis->m_bSendCallbackEnable)
			{
				pThis->SendCallback(status);
			}
			SetEvent(pThis->m_hEventSnapped);
			pThis->m_bIsSnap = false;
		}
	}
	Sleep(2);
	return;
}

// 根据相机GUID初始化相机
bool CPGCamera::InitFromGuid(PGRGuid *pGuid)
{
	CString strError;
	CString strRecordInfo;
	m_bOnline = false;
	Error error;
	if(m_pCamera)
	{
		delete m_pCamera;
		m_pCamera = NULL;
	}
	BusManager busMgr;
	InterfaceType interfaceType;
	error = busMgr.GetInterfaceTypeFromGuid(pGuid,&interfaceType);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	// 获取相机类型
	if(interfaceType == INTERFACE_GIGE)
	{
		m_cameraType = eGigE;
		m_pCamera = new GigECamera();
	}
	else
	{
		return false;
	}
	// 连接
	error = m_pCamera->Connect(pGuid);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	// 设置相机初始化参数
	if(interfaceType == INTERFACE_GIGE)
	{
		// 关闭心跳
		error = m_pCamera->WriteGVCPRegister(0x954,1);
		if(error!=PGRERROR_OK)
		{
			return false;
		}
		// 获取Format7下的默认参数
		error = m_pCamera->GetGigEImageSettingsInfo(&m_GigFormat7Msg);
		if(error!=PGRERROR_OK)
		{
			return false;
		}
		// 获取当前Format7下的参数
		error = m_pCamera->GetGigEImageSettings(&m_GigImageSettings);
		if(error!=PGRERROR_OK)
		{
			return false;
		}
		// 设置感兴趣区域
		m_GigImageSettings.offsetX = (m_GigFormat7Msg.maxWidth - WIDTH)/2;
		m_GigImageSettings.offsetY = (m_GigFormat7Msg.maxHeight - HEIGHT)/2;
		m_GigImageSettings.pixelFormat = PIXEL_FORMAT_MONO8;
		m_GigImageSettings.height = HEIGHT;
		m_GigImageSettings.width = WIDTH;

		error = m_pCamera->SetGigEImageSettings(&m_GigImageSettings);
		strError.Format("SetGigEImageSettings = %d",(int)error.GetType());
		AfxMessageBox(strError);
		if(error!=PGRERROR_OK)
		{
			return false;
		}

		// 设置模式
		Mode mode;
		error = m_pCamera->GetGigEImagingMode(&mode);
		strError.Format("GetGigEImagingMode = %d",(int)error.GetType());
		AfxMessageBox(strError);
		if(error!=PGRERROR_OK)
		{
			return false;
		}
		mode = MODE_0;
		error = m_pCamera->SetGigEImagingMode(mode);
		strError.Format("SetGigEImagingMode = %d",(int)error.GetType());
		AfxMessageBox(strError);
		if(error!=PGRERROR_OK)
		{
			return false;
		}

		// 设置PacketSize
		GigEProperty gigEPacket;
		gigEPacket.propType = PACKET_SIZE;
		error = m_pCamera->GetGigEProperty(&gigEPacket);
		strError.Format("GetGigEProperty = %d",(int)error.GetType());
		AfxMessageBox(strError);
		if(error!=PGRERROR_OK)
		{
			return false;
		}
		gigEPacket.isReadable = true;
		gigEPacket.isWritable = true;
		gigEPacket.value = 1400;
		error = m_pCamera->SetGigEProperty(&gigEPacket);
		strError.Format("SetGigEProperty = %d",(int)error.GetType());
		AfxMessageBox(strError);
		if(error!=PGRERROR_OK)
		{
			return false;
		}

		// 设置PacketDelay
		GigEProperty packetDelay;
		packetDelay.propType = PACKET_DELAY;
		error = m_pCamera->GetGigEProperty(&packetDelay);
		strError.Format("GetGigEProperty = %d",(int)error.GetType());
		AfxMessageBox(strError);
		if(error!=PGRERROR_OK)
		{
			return false;
		}
		packetDelay.isReadable = true;
		packetDelay.isWritable = true;
		packetDelay.value = 400*1+200;
		error = m_pCamera->SetGigEProperty(&packetDelay);
		strError.Format("SetGigEProperty = %d",(int)error.GetType());
		AfxMessageBox(strError);
		if(error!=PGRERROR_OK)
		{
			return false;
		}
	}

	// 设置Fc2config
	error = m_pCamera->GetConfiguration(&m_fc2Config);
	strError.Format("GetConfiguration = %d",(int)error.GetType());
	AfxMessageBox(strError);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	m_fc2Config.numBuffers = 30;
	m_fc2Config.grabMode = BUFFER_FRAMES;
	m_fc2Config.grabTimeout = TIMEOUT_INFINITE;
	error = m_pCamera->SetConfiguration(&m_fc2Config);
	strError.Format("SetConfiguration = %d",(int)error.GetType());
	AfxMessageBox(strError);
	if(error!=PGRERROR_OK)
	{
		return false;
	}

	// 设置0x12F8寄存器的嵌入式参数的开启，主要是FrameCounter和TimeStamp
	error = m_pCamera->GetEmbeddedImageInfo(&m_EmbeddedImageInfo);
	strError.Format("GetEmbeddedImageInfo = %d",(int)error.GetType());
	AfxMessageBox(strError);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	m_EmbeddedImageInfo.frameCounter.available = true;
	m_EmbeddedImageInfo.frameCounter.onOff = true;
	m_EmbeddedImageInfo.timestamp.available = true;
	m_EmbeddedImageInfo.timestamp.onOff = true;
	error = m_pCamera->SetEmbeddedImageInfo(&m_EmbeddedImageInfo);
	strError.Format("SetEmbeddedImageInfo = %d",(int)error.GetType());
	AfxMessageBox(strError);
	if(error!=PGRERROR_OK)
	{
		return false;
	}

	// 设置通道
	unsigned int nChannels = 0;
	error = m_pCamera->GetMemoryChannel(&nChannels);
	strError.Format("GetMemoryChannel = %d",(int)error.GetType());
	AfxMessageBox(strError);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	nChannels = 1;
	error = m_pCamera->SaveToMemoryChannel(nChannels);
	strError.Format("SaveToMemoryChannel = %d",(int)error.GetType());
	AfxMessageBox(strError);
	if(error!=PGRERROR_OK)
	{
		return false;
	}

	m_bOnline = true;

	if(GetCameraInfo(&m_pCameraInfo))
	{
		if(m_pCameraInfo.isColorCamera)
		{
			m_CameraColor = ePGColor;
		}
		else
		{
			m_CameraColor = ePGMono;
		}
	}
	else
	{
		return false;
	}

	// 关闭触发
	CloseTriggerMode();

	// 采集图像确定图像尺寸，并分配内存
	EnterCriticalSection(&m_criticalSection);
	// 开始采集
	error = m_pCamera->StartCapture(NULL,NULL);
	strError.Format("StartCapture = %d",(int)error.GetType());
	AfxMessageBox(strError);
	if(error!=PGRERROR_OK)
	{
		LeaveCriticalSection(&m_criticalSection);
		return false;
	}
	// 获取图像
	if(ePGGrabOK!=AcquireImage())
	{
		LeaveCriticalSection(&m_criticalSection);
		return false;
	}
	// 停止采集
	error = m_pCamera->StopCapture();
	strError.Format("StopCapture = %d",(int)error.GetType());
	AfxMessageBox(strError);
	if(error!=PGRERROR_OK)
	{
		LeaveCriticalSection(&m_criticalSection);
		return false;
	}
	LeaveCriticalSection(&m_criticalSection);

	// 采集初始化
	if(eWaitEvent == m_grabImageMode) // 事件采集方式
	{
		m_hEventSnapped = CreateEvent(NULL,true,false,_T(""));
	}
	else // 线程采集方式
	{
		if(!CreateGrabThread())
		{
			return false;
		}
	}
	m_bOnline = true;
	return true;
}

bool CPGCamera::InitFromGuid(PGRGuid *pGuid,unsigned int nWidth,unsigned int nHeight)
{
	CString strError;
	CString strRecordInfo;
	m_bOnline = false;
	Error error;
	if(m_pCamera)
	{
		delete m_pCamera;
		m_pCamera = NULL;
	}
	BusManager busMgr;
	InterfaceType interfaceType;
	error = busMgr.GetInterfaceTypeFromGuid(pGuid,&interfaceType);
	strError.Format("GetInterfaceTypeFromGuid = %d",(int)error.GetType());
	//AfxMessageBox(strError);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	// 获取相机类型
	if(interfaceType == INTERFACE_GIGE)
	{
		m_cameraType = eGigE;
		m_pCamera = new GigECamera();
	}
	else
	{
		return false;
	}
	// 连接
	error = m_pCamera->Connect(pGuid);
	strError.Format("Connect = %d",(int)error.GetType());
	//AfxMessageBox(strError);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	// 设置相机初始化参数
	if(interfaceType == INTERFACE_GIGE)
	{
		// 关闭心跳
		error = m_pCamera->WriteGVCPRegister(0x954,1);
		strError.Format("WriteGVCPRegister = %d",(int)error.GetType());
		//AfxMessageBox(strError);
		if(error!=PGRERROR_OK)
		{
			return false;
		}
		// 获取Format7下的默认参数
		error = m_pCamera->GetGigEImageSettingsInfo(&m_GigFormat7Msg);
		strError.Format("GetGigEImageSettingsInfo = %d",(int)error.GetType());
		//AfxMessageBox(strError);
		if(error!=PGRERROR_OK)
		{
			return false;
		}
		// 获取当前Format7下的参数
		error = m_pCamera->GetGigEImageSettings(&m_GigImageSettings);
		strError.Format("GetGigEImageSettings = %d",(int)error.GetType());
		//AfxMessageBox(strError);
		if(error!=PGRERROR_OK)
		{
			return false;
		}
		// 设置感兴趣区域    //2017-06-09
		//m_GigImageSettings.offsetX = (unsigned int)(m_GigFormat7Msg.maxWidth - nWidth)/2;
		//m_GigImageSettings.offsetY = (unsigned int)(m_GigFormat7Msg.maxHeight - nHeight)/2;
		//m_GigImageSettings.pixelFormat = PIXEL_FORMAT_MONO8;
		//m_GigImageSettings.height = nHeight;
		//m_GigImageSettings.width = nWidth;

		//error = m_pCamera->SetGigEImageSettings(&m_GigImageSettings);
		//strError.Format("SetGigEImageSettings = %d,maxWidth = %d,maxHeight = %d",(int)error.GetType(),m_GigFormat7Msg.maxWidth,m_GigFormat7Msg.maxHeight);
		////AfxMessageBox(strError);
		//if(error!=PGRERROR_OK)
		//{
		//	return false;
		//}

		// 设置模式
		Mode mode;
		error = m_pCamera->GetGigEImagingMode(&mode);
		strError.Format("GetGigEImagingMode = %d",(int)error.GetType());
		//AfxMessageBox(strError);
		if(error!=PGRERROR_OK)
		{
			return false;
		}
		mode = MODE_0;
		error = m_pCamera->SetGigEImagingMode(mode);
		strError.Format("SetGigEImagingMode = %d",(int)error.GetType());
		//AfxMessageBox(strError);
		if(error!=PGRERROR_OK)
		{
			return false;
		}

		// 设置PacketSize
		GigEProperty gigEPacket;
		gigEPacket.propType = PACKET_SIZE;
		error = m_pCamera->GetGigEProperty(&gigEPacket);
		strError.Format("GetGigEProperty = %d",(int)error.GetType());
		//AfxMessageBox(strError);
		if(error!=PGRERROR_OK)
		{
			return false;
		}
		gigEPacket.isReadable = true;
		gigEPacket.isWritable = true;
		gigEPacket.value = 1400;   //20170620  1400->9000
		error = m_pCamera->SetGigEProperty(&gigEPacket);
		strError.Format("SetGigEProperty = %d",(int)error.GetType());
		//AfxMessageBox(strError);
		if(error!=PGRERROR_OK)
		{
			return false;
		}

		// 设置PacketDelay
		GigEProperty packetDelay;
		packetDelay.propType = PACKET_DELAY;
		error = m_pCamera->GetGigEProperty(&packetDelay);
		strError.Format("GetGigEProperty = %d",(int)error.GetType());
		//AfxMessageBox(strError);
		if(error!=PGRERROR_OK)
		{
			return false;
		}
		packetDelay.isReadable = true;
		packetDelay.isWritable = true;
		packetDelay.value = 600;   //20170620   400*1+200--->4800
		error = m_pCamera->SetGigEProperty(&packetDelay);
		strError.Format("SetGigEProperty = %d",(int)error.GetType());
		//AfxMessageBox(strError);
		if(error!=PGRERROR_OK)
		{
			return false;
		}
	}

	// 设置Fc2config
	error = m_pCamera->GetConfiguration(&m_fc2Config);
	strError.Format("GetConfiguration = %d",(int)error.GetType());
	//AfxMessageBox(strError);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	m_fc2Config.numBuffers = 30;
	m_fc2Config.grabMode = BUFFER_FRAMES;
	m_fc2Config.grabTimeout = TIMEOUT_INFINITE;
	error = m_pCamera->SetConfiguration(&m_fc2Config);
	strError.Format("SetConfiguration = %d",(int)error.GetType());
	//AfxMessageBox(strError);
	if(error!=PGRERROR_OK)
	{
		return false;
	}

	// 设置0x12F8寄存器的嵌入式参数的开启，主要是FrameCounter和TimeStamp
	error = m_pCamera->GetEmbeddedImageInfo(&m_EmbeddedImageInfo);
	strError.Format("GetEmbeddedImageInfo = %d",(int)error.GetType());
	//AfxMessageBox(strError);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	m_EmbeddedImageInfo.frameCounter.available = true;
	m_EmbeddedImageInfo.frameCounter.onOff = true;
	m_EmbeddedImageInfo.timestamp.available = true;
	m_EmbeddedImageInfo.timestamp.onOff = true;
	error = m_pCamera->SetEmbeddedImageInfo(&m_EmbeddedImageInfo);
	strError.Format("SetEmbeddedImageInfo = %d",(int)error.GetType());
	//AfxMessageBox(strError);
	if(error!=PGRERROR_OK)
	{
		return false;
	}

	// 设置通道
	unsigned int nChannels = 0;
	error = m_pCamera->GetMemoryChannel(&nChannels);
	strError.Format("GetMemoryChannel = %d",(int)error.GetType());
	//AfxMessageBox(strError);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	nChannels = 1;
	error = m_pCamera->SaveToMemoryChannel(nChannels);
	strError.Format("SaveToMemoryChannel = %d",(int)error.GetType());
	//AfxMessageBox(strError);
	if(error!=PGRERROR_OK)
	{
		return false;
	}

	m_bOnline = true;

	if(GetCameraInfo(&m_pCameraInfo))
	{
		if(m_pCameraInfo.isColorCamera)
		{
			m_CameraColor = ePGColor;
		}
		else
		{
			m_CameraColor = ePGMono;
		}
	}
	else
	{
		return false;
	}

	// 关闭触发
	CloseTriggerMode();

	// 采集图像确定图像尺寸，并分配内存
	EnterCriticalSection(&m_criticalSection);
	// 开始采集
	error = m_pCamera->StartCapture(NULL,NULL);
	strError.Format("StartCapture = %d",(int)error.GetType());
	//AfxMessageBox(strError);
	if(error!=PGRERROR_OK)
	{
		LeaveCriticalSection(&m_criticalSection);
		return false;
	}
	// 获取图像
	if(ePGGrabOK!=AcquireImage())
	{
		AfxMessageBox("AcquireImage Failed.");
		LeaveCriticalSection(&m_criticalSection);
		return false;
	}
	// 停止采集
	error = m_pCamera->StopCapture();
	strError.Format("StopCapture = %d",(int)error.GetType());
	//AfxMessageBox(strError);
	if(error!=PGRERROR_OK)
	{
		LeaveCriticalSection(&m_criticalSection);
		return false;
	}
	LeaveCriticalSection(&m_criticalSection);

	// 采集初始化
	if(eWaitEvent == m_grabImageMode) // 事件采集方式
	{
		m_hEventSnapped = CreateEvent(NULL,true,false,_T(""));
	}
	else // 线程采集方式
	{
		if(!CreateGrabThread())
		{
			return false;
		}
	}
	m_bOnline = true;
	return true;
}

// 根据相机序列号初始化相机
bool CPGCamera::InitFromSerial(unsigned int nSerialNum)
{
	PGRGuid guid;
	GetCameraGuidFromSerialNum(nSerialNum,&guid);
	return InitFromGuid(&guid);
}

bool CPGCamera::InitFromSerial(unsigned int nSerialNum,unsigned int nWidth,unsigned int nHeight)
{
	PGRGuid guid;
	GetCameraGuidFromSerialNum(nSerialNum,&guid);
	return InitFromGuid(&guid,nWidth,nHeight);
}

// 根据相机索引初始化相机
bool CPGCamera::InitFromIndex(unsigned int nCameraIndex)
{
	PGRGuid guid;
	m_nCameraIndex = (int)nCameraIndex;
	GetCameraGuidFromIndex(nCameraIndex,&guid);
	return InitFromGuid(&guid);
}

bool CPGCamera::InitFromIndex(unsigned int nCameraIndex,unsigned int nWidth,unsigned int nHeight)
{
	CString str;
	PGRGuid guid;
	m_nCameraIndex = nCameraIndex;
	GetCameraGuidFromIndex(nCameraIndex,&guid);
	str.Format("nWidth = %d,nHeight = %d",nWidth,nHeight);
	//AfxMessageBox(str);
	return InitFromGuid(&guid,nWidth,nHeight);
}

// 开始连续采集（支持FreeRun、触发两种模式）
// 调用该连续采集函数，没有等待立即返回
bool CPGCamera::Grab()
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	if(eWaitEvent==m_grabImageMode) // 事件采集方式
	{
		Stop();
		m_threadStatus = ePGGrab;
		m_bIsGrabbing = true;
		m_bIsSnap = false;
		Start();
	}
	else // 线程采集方式
	{
		Stop();
		Start();
		if(!m_bIsGrabbing)
		{
			m_bAcquireSuccess = false;
			m_threadStatus = ePGGrab;
			m_bIsGrabbing = true;
			SetEvent(m_hEventToRun);
		}
	}
	return true;
}

// 结束连续采集
bool CPGCamera::Freeze()
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	if(eWaitEvent==m_grabImageMode) // 事件采集方式
	{
		m_bFreezeStop = true;
		Stop();
		m_threadStatus = ePGIdle;
		m_bIsGrabbing = false;
		m_bIsSnap = true;
		m_bFreezeStop = false;
	}
	else
	{
		m_bFreezeStop = true;
		Stop();
		if(m_bIsGrabbing||!m_bHaveStopped)
		{
			m_threadStatus = ePGIdle;
			m_bIsGrabbing = false;
			SetEvent(m_hEventToRun);
			int i = 0;
			while(!m_bHaveStopped && i<500)
			{
				Sleep(2);
				i++;
			}
		}
		m_bFreezeStop = false;
	}
	return true;
}

// 获取是否处于连续采集状态
bool CPGCamera::IsGrabbing()
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	return m_bIsGrabbing;
}

// 采集单帧图像（只支持FreeRun模式）
// 如果bWaitSnapped=true，等待直到采集完成才返回
// 如果bWaitSnapped=false，调用后不等待采集完成直接返回
bool CPGCamera::Snap(bool bWaitSnapped /* = false */)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	if(eWaitEvent==m_grabImageMode) //事件采集方式
	{
		Stop();
		if(bWaitSnapped)
		{
			ResetEvent(m_hEventSnapped); // 重置单帧采集完成事件
		}
		m_threadStatus = ePGSnap;
		m_bIsGrabbing = false;
		m_bIsSnap = true;
		Start();
		if(bWaitSnapped)
		{
			// 等待单帧采集完成事件，1秒之内采集不完退出等待，返回false
			if(WAIT_TIMEOUT == WaitForSingleObject(m_hEventSnapped,1000))
			{
				return false;
			}
			return IsAcquireSuccess();
		}
	}
	else
	{
		// 如果采集线程处于连续采集状态，则返回false
		if(m_bIsGrabbing||!m_bHaveStopped)
		{
			return false;
		}
		if(!Start())
		{
			Stop();
			Start();
		}
		m_bAcquireSuccess = false;
		m_threadStatus = ePGSnap;
		if(bWaitSnapped)
		{
			ResetEvent(m_hEventSnapped); // 重置单帧采集完成事件
		}
		SetEvent(m_hEventToRun); // 发送运行采集事件
		if(bWaitSnapped)
		{
			// 等待单帧采集完成事件，1秒之内采集不完退出等待，返回false
			if(WAIT_TIMEOUT == WaitForSingleObject(m_hEventSnapped,1000))
			{
				return false;
			}
			return IsAcquireSuccess();
		}
	}
	return true;
}

// 开始
bool CPGCamera::Start()
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	Error error;
	if(eWaitEvent==m_grabImageMode) // 事件采集方式
	{
		error = m_pCamera->StartCapture(PGImageEventCallback,this);
		if(error!=PGRERROR_OK)
		{
			return false;
		}
	}
	else // 线程采集方式
	{
		// 先将回调函数设为空
		m_pCamera->SetCallback(NULL,NULL);
		error = m_pCamera->StartCapture(NULL,NULL);
		if(error!=PGRERROR_OK)
		{
			return false;
		}
	}
	return true;
}

// 停止
bool CPGCamera::Stop()
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	Error error;
	error = m_pCamera->StopCapture();
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	return true;
}

// 图像数据格式转换
// 首先分配图像内存
// 需要考虑图像的宽高、格式 
// 目前支持三种PGR的像素格式：FLYCAPTURE_MONO8、FLYCAPTURE_RAW8和FLYCAPTURE_RGB8
// eGrey8：对应于PGR的FLYCAPTURE_MONO8、FLYCAPTURE_RAW8
// eRGB24：对应于FLYCAPTURE_RGB8，但需要将R-G-B排列的数据转换为B-G-R排列的数据
bool CPGCamera::TransferData()
{
	if(NULL==m_ImageRaw.GetData())
	{
		return false;
	}
	// 获取图像像素格式
	PGPixelFormat pixelFormat;
	switch(m_ImageRaw.GetPixelFormat())
	{
	case PIXEL_FORMAT_MONO8:
		{
			pixelFormat = ePGGrey8;
			m_nBytes = 1;
		}
		break;
	case PIXEL_FORMAT_RAW8:
		{
			if(m_CameraColor==ePGMono)
			{
				pixelFormat = ePGGrey8;
				m_nBytes = 1;
			}
			else
			{
				pixelFormat = ePGRGB24;
				m_nBytes = 3;
			}
		}
		break;
	case PIXEL_FORMAT_RGB8:
		{
			pixelFormat = ePGRGB24;
			m_nBytes = 3;
		}
		break;
	default:
		return false;
	}
	// 获取图像的宽高
	LONG lWidth = m_ImageRaw.GetCols();
	LONG lHeight = m_ImageRaw.GetRows();
	// 分配图像数据内存
	if(NULL==m_pGrabImage)
	{
		m_lWidth = lWidth;
		m_lHeight = lHeight;
		m_pixelFormat = pixelFormat;
		m_pGrabImage = new unsigned char[m_lWidth*m_lHeight*m_nBytes];
	}
	else
	{
		if(m_lWidth!=lWidth||m_lHeight!=lHeight||m_pixelFormat!=pixelFormat)
		{
			delete[] m_pGrabImage;
			m_pGrabImage = NULL;
			m_lWidth = lWidth;
			m_lHeight = lHeight;
			m_pixelFormat = pixelFormat;
			m_pGrabImage = new unsigned char[m_lWidth*m_lHeight*m_nBytes];
		}
	}
	// 判断内存分配是否成功
	if(NULL == m_pGrabImage)
	{
		return false;
	}
	// 拷贝图像内存
	if((m_ImageRaw.GetPixelFormat()==PIXEL_FORMAT_RAW8&&(m_CameraColor==ePGColor)))
	{
		// 拷贝图像内存
		Image convertedImage;
		m_ImageRaw.Convert(PIXEL_FORMAT_BGR,&convertedImage);//copy data;
		memcpy(m_pGrabImage,convertedImage.GetData(),m_lWidth*m_lHeight*m_nBytes);
	}
	else
	{
		//memcpy(m_pGrabImage,convertedImage.GetData(),m_lWidth*m_lHeight*m_nBytes);
		memcpy(m_pGrabImage,m_ImageRaw.GetData(),m_lWidth*m_lHeight*m_nBytes);
	}

	// 如果PGR像素格式是FLYCAPTURE_RGB8，则需要将R-G-B排列的数据转换为B-G-R排列的数据
	if(PIXEL_FORMAT_RGB8 == m_ImageRaw.GetPixelFormat())
	{
		for(int j=0;j<lHeight;j++)
		{
			for(int i=0;i<lWidth*3;i+=3)
			{
				BYTE byTemp;
				byTemp = m_pGrabImage[j*lWidth*3+i];
				m_pGrabImage[j*lWidth*3+i] = m_pGrabImage[j*lWidth*3+i+2];
				m_pGrabImage[j*lWidth*3+i+2] = byTemp;
			}
		}
	}
	return true;
}

// 判断相机是否支持指定视频格式
// GigE相机只支持Format7模式
bool CPGCamera::IsVideoModeSupported(VideoMode vmVideoMode)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}

	if(eInterfaceUnknow == m_cameraType)
	{
		return false;
	}
	if(eGigE == m_cameraType)
	{
		if(vmVideoMode!=VIDEOMODE_FORMAT7)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	Camera *pCamera = (Camera *)m_pCamera;
	if ( vmVideoMode == VIDEOMODE_640x480RGB
		|| vmVideoMode == VIDEOMODE_640x480Y8
		|| vmVideoMode == VIDEOMODE_800x600RGB
		|| vmVideoMode == VIDEOMODE_800x600Y8
		|| vmVideoMode == VIDEOMODE_1024x768RGB
		|| vmVideoMode == VIDEOMODE_1024x768Y8
		|| vmVideoMode == VIDEOMODE_1280x960RGB
		|| vmVideoMode == VIDEOMODE_1280x960Y8
		|| vmVideoMode == VIDEOMODE_1600x1200Y8
		|| vmVideoMode == VIDEOMODE_1600x1200Y8)
	{
		Error error;
		bool bSupported = false;
		for(int j = 0;j<sizeof(g_frFrameRate)/sizeof(g_frFrameRate[0])-1;j++)
		{
			error = pCamera->GetVideoModeAndFrameRateInfo(vmVideoMode,g_frFrameRate[j],&bSupported);
			if((error==PGRERROR_OK)&&bSupported)
			{
				break;
			}
		}
		return bSupported;
	}
	else if(VIDEOMODE_FORMAT7 == vmVideoMode)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// 判断相机是否支持指定帧率模式
// GigE相机只支持Format7模式
bool CPGCamera::IsFrameRateSupported(FrameRate frFrameRate)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	if(eInterfaceUnknow == m_cameraType)
	{
		return false;
	}
	if(eGigE == m_cameraType)
	{
		if(frFrameRate!=FRAMERATE_FORMAT7)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	Camera *pCamera = (Camera *)m_pCamera;
	VideoMode vmVideoMode;
	if(!GetCurVideoMode(&vmVideoMode))
	{
		return false;
	}

	Error error;
	bool bSupported = false;
	error = pCamera->GetVideoModeAndFrameRateInfo(vmVideoMode,frFrameRate,&bSupported);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	return bSupported;
}

// 设置视频模式
// 设置除Format7之外的其他视频模式
bool CPGCamera::SetCurVideoMode(VideoMode vmVideoMode)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	if(FRAMERATE_FORMAT7 == vmVideoMode)
	{
		return false;
	}
	if(eGigE == m_cameraType)
	{
		return false;
	}
	if(!IsVideoModeSupported(vmVideoMode))
	{
		return false;
	}
	bool bIsGrabbing = IsGrabbing();
	Freeze();

	// 设置寄存器
	unsigned long ulFormat,ulMode;
	if(!ConvertVideoModeToULongs(vmVideoMode,&ulFormat,&ulMode))
	{
		return false;
	}
	ulFormat = ulFormat<<29;
	ulMode = ulMode<<29;

	Error error;
	error = m_pCamera->WriteRegister(0x608,ulFormat);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	error = m_pCamera->WriteRegister(0x604,ulMode);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	if(bIsGrabbing)
	{
		Grab();
	}
	return true;
}

// 获取当前视频模式
bool CPGCamera::GetCurVideoMode(VideoMode *pVideoMode)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	if(eInterfaceUnknow == m_cameraType)
	{
		return false;
	}
	if(eGigE == m_cameraType)
	{
		*pVideoMode = VIDEOMODE_FORMAT7;
		return true;
	}
	Camera *pCamera = (Camera *)m_pCamera;
	if(NULL==pVideoMode)
	{
		return false;
	}

	Error error;
	VideoMode vmTemp;
	FrameRate frTemp;
	error = pCamera->GetVideoModeAndFrameRate(&vmTemp,&frTemp);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	*pVideoMode = vmTemp;
	return true;
}

// 设置帧率模式
// 设置除Format7之外的其它帧率模式
bool CPGCamera::SetCurFrameRate(FrameRate frFrameRate)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	if(FRAMERATE_FORMAT7 == frFrameRate)
	{
		return false;
	}
	if(eGigE == m_cameraType)
	{
		return false;
	}
	if(!IsFrameRateSupported(frFrameRate))
	{
		return false;
	}

	unsigned long ulFrameRate;
	if(!ConvertFrameRateToULong(frFrameRate,&ulFrameRate))
	{
		return false;
	}
	ulFrameRate = ulFrameRate<<29;
	Error error;
	error = m_pCamera->WriteRegister(0x600,ulFrameRate);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	return true;
}

// 获取帧率模式
bool CPGCamera::GetCurFrameRate(FrameRate *pFrameRate)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	if(NULL==pFrameRate)
	{
		return false;
	}
	if(eGigE == m_cameraType)
	{
		*pFrameRate = FRAMERATE_FORMAT7;
		return true;
	}
	Error error;
	unsigned int uiValue = 0;
	error = m_pCamera->ReadRegister(0x600,&uiValue);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	uiValue = uiValue>>29;
	*pFrameRate = g_frFrameRate[uiValue];
	return true;
}

// 获取在当前视频模式下，支持的最大帧率
bool CPGCamera::GetMaxFrameRate(FrameRate *pFrameRate)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	if(NULL==pFrameRate)
	{
		return false;
	}
	if(eInterfaceUnknow==m_cameraType||eGigE==m_cameraType)
	{
		return false;
	}
	Camera *pCamera = (Camera *)m_pCamera;
	VideoMode vmCurVideoMode;
	if(!GetCurVideoMode(&vmCurVideoMode))
	{
		return false;
	}

	// 查询可用的最大帧率
	Error error;
	bool bSupported;
	int nIndex = 0;
	int nFrameRateCount = sizeof(g_frFrameRate)/sizeof(g_frFrameRate[0]);
	for(nIndex=nFrameRateCount-1;nIndex>=0;nIndex--)
	{
		error = pCamera->GetVideoModeAndFrameRateInfo(vmCurVideoMode,g_frFrameRate[nIndex],&bSupported);
		if(bSupported)
		{
			*pFrameRate = g_frFrameRate[nIndex];
			return true;
		}
	}
	return false;
}

// 设置帧率
bool CPGCamera::SetFrameRate(float fFrameRate)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	Error error;
	Property cameraProperty;
	cameraProperty.type = FRAME_RATE;
	cameraProperty.absControl = true;
	error = m_pCamera->GetProperty(&cameraProperty);
	if(error!=PGRERROR_OK)
	{
		return false;
	}

	// 设置
	cameraProperty.absValue = fFrameRate;
	cameraProperty.absControl = true;
	cameraProperty.onOff = true;
	cameraProperty.autoManualMode = false; // 手动设置模式
	error = m_pCamera->SetProperty(&cameraProperty);
	if(error != PGRERROR_OK)
	{
		return false;
	}
	return true;
}

// 获取当前帧率
bool CPGCamera::GetFrameRate(float *pFrameRate)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	Error error;
	Property cameraProperty;
	cameraProperty.type = FRAME_RATE;
	cameraProperty.absControl = true;
	error = m_pCamera->GetProperty(&cameraProperty);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	*pFrameRate = cameraProperty.absValue;
	return true;
}

// 获取可用的帧率范围
bool CPGCamera::GetFrameRateRange(float *pMinFrameRate,float *pMaxFrameRate)
{
	Error error;
	PropertyInfo cameraPropertyInfo;
	cameraPropertyInfo.type = FRAME_RATE;
	error = m_pCamera->GetPropertyInfo(&cameraPropertyInfo);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	*pMinFrameRate = cameraPropertyInfo.absMin;
	*pMaxFrameRate = cameraPropertyInfo.absMax;

	return true;
}

// 设置曝光时间，单位ms
bool CPGCamera::SetShutter(float fShutter)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	Error error;
	Property cameraProperty;
	cameraProperty.type = SHUTTER;
	cameraProperty.absControl = true;
	error = m_pCamera->GetProperty(&cameraProperty);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	// 设置
	cameraProperty.absValue = fShutter;
	cameraProperty.absControl = true;
	cameraProperty.onOff = true;
	// 手动设置模式
	cameraProperty.autoManualMode = false;
	error = m_pCamera->SetProperty(&cameraProperty);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	return true;
}

// 获取曝光时间，单位ms
bool CPGCamera::GetShutter(float *pShutter)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	Error error;
	Property cameraProperty;
	cameraProperty.type = SHUTTER;
	cameraProperty.absControl = true;
	error = m_pCamera->GetProperty(&cameraProperty);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	*pShutter = cameraProperty.absValue;
	return true;
}

// 获取可用的曝光时间范围，单位ms
bool CPGCamera::GetShutterRange(float *pMinShutter,float *pMaxShutter)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	Error error;
	PropertyInfo cameraPropertyInfo;
	cameraPropertyInfo.type = SHUTTER;
	error = m_pCamera->GetPropertyInfo(&cameraPropertyInfo);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	*pMinShutter = cameraPropertyInfo.absMin;
	*pMaxShutter = cameraPropertyInfo.absMax;
	return true;
}

// 设置增益，单位db
bool CPGCamera::SetGain(float fGain)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	Error error;
	Property cameraProperty;
	cameraProperty.type = GAIN;
	cameraProperty.absControl = true;
	error = m_pCamera->GetProperty(&cameraProperty);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	cameraProperty.absValue = fGain;
	cameraProperty.absControl = true;
	cameraProperty.onOff = true;
	// 手动设置模式
	cameraProperty.autoManualMode = false;
	error = m_pCamera->SetProperty(&cameraProperty);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	return true;
}

// 获取增益，单位db
bool CPGCamera::GetGain(float *pGain)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}

	Error error;
	Property cameraProperty;
	cameraProperty.type = GAIN;
	cameraProperty.absControl = true;
	error = m_pCamera->GetProperty(&cameraProperty);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	*pGain = cameraProperty.absValue;
	return true;
}

//ROI设置
bool CPGCamera::SetCamRoi(unsigned int XOffset,unsigned int YOffset ,unsigned int ImgWidth,unsigned int ImgHeight)
{
	Error error;
	GigEImageSettings m_GigEImageSettings;//define a parameter object for GigeCam;
	error=m_pCamera->GetGigEImageSettings(&m_GigEImageSettings);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	m_GigEImageSettings.offsetX=XOffset;
	m_GigEImageSettings.offsetY=YOffset;
	m_GigEImageSettings.height=ImgHeight;
	m_GigEImageSettings.width=ImgWidth;
	m_GigImageSettings.pixelFormat = PIXEL_FORMAT_MONO8;
	error=m_pCamera->SetGigEImageSettings(&m_GigEImageSettings);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	return true;

}
// 获取可用的增益范围，单位db
bool CPGCamera::GetGainRange(float *pMinGain,float *pMaxGain)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	Error error;
	PropertyInfo cameraPropertyInfo;
	cameraPropertyInfo.type = GAIN;
	error = m_pCamera->GetPropertyInfo(&cameraPropertyInfo);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	*pMinGain = cameraPropertyInfo.absMin;
	*pMaxGain = cameraPropertyInfo.absMax;
	return true;
}

bool CPGCamera::CheckTriggerReady()
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	const unsigned int k_softwareTrigger = 0x62C;
	Error error;
	unsigned int regVal = 0;
	do 
	{
		error = m_pCamera->ReadRegister(k_softwareTrigger,&regVal);
		if(error!=PGRERROR_OK)
		{
			return false;
		}
	} while ((regVal>>31)!=0);
	return true;
}

bool CPGCamera::CheckSoftwareTriggerPresence()
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	const unsigned int k_triggerInq = 0x530;
	Error error;
	unsigned int regVal = 0;
	error = m_pCamera->ReadRegister(k_triggerInq,&regVal);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	if((regVal&0x10000)!=0x10000)
	{
		return false;
	}
	return true;
}

// 设置相机的触发模式（软触发、硬触发或非触发）
bool CPGCamera::SetTriggerMode(PGTriggerMode mode,unsigned long lTimeout /* = 2000 */)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	if(ePGNotTrigger==mode)
	{
		return CloseTriggerMode();
	}
	Error error;
	TriggerMode triggerMode;
	error = m_pCamera->GetTriggerMode(&triggerMode);
	if(error!=PGRERROR_OK)
	{
		CloseTriggerMode();
		return false;
	}
	triggerMode.onOff = true;
	triggerMode.mode = 0;
	triggerMode.parameter = 0;
	if(ePGSoftwareTrigger==mode)
	{
		triggerMode.source = 7;
	}
	else
	{
		triggerMode.source = 0;
	}
	error = m_pCamera->SetTriggerMode(&triggerMode);
	if(error!=PGRERROR_OK)
	{
		CloseTriggerMode();
		return false;
	}
	m_triggerMode = mode;

	bool retVal = CheckTriggerReady();
	if(!retVal)
	{
		CloseTriggerMode();
		return false;
	}
	FC2Config config;
	error = m_pCamera->GetConfiguration(&config);
	if(error!=PGRERROR_OK)
	{
		CloseTriggerMode();
		return false;
	}
	// 设置采集延时
	config.grabTimeout = lTimeout;
	error = m_pCamera->SetConfiguration(&config);
	if(error!=PGRERROR_OK)
	{
		CloseTriggerMode();
		return false;
	}
	return true;
}

// 获取当前的触发模式
PGTriggerMode CPGCamera::GetTriggerMode()
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return ePGNotTrigger;
	}
	TriggerMode triggerMode;
	Error error;
	error = m_pCamera->GetTriggerMode(&triggerMode);
	if(error!=PGRERROR_OK)
	{
		return ePGNotTrigger;
	}
	if(triggerMode.onOff)
	{
		if(7==triggerMode.source)
		{
			return ePGSoftwareTrigger;
		}
		else
		{
			return ePGHardwareTrigger;
		}
	}
	return ePGNotTrigger;
}

// 关闭触发模式
bool CPGCamera::CloseTriggerMode()
{
	if(!m_pCamera)
	{
		return false;
	}
	Error error;
	TriggerMode triggerMode;
	error = m_pCamera->GetTriggerMode(&triggerMode);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	triggerMode.onOff = false;
	error = m_pCamera->SetTriggerMode(&triggerMode);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	m_triggerMode = ePGNotTrigger;
	return true;
}

// 是否以触发方式（软触发或硬触发）采集图像
bool CPGCamera::IsTrigger()
{
	if((ePGSoftwareTrigger==m_triggerMode)||(ePGHardwareTrigger==m_triggerMode))
	{
		return true;
	}
	else
	{
		return false;
	}
}

// 用触发方式采集图像：软触发
bool CPGCamera::SendSoftTrigger()
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	Error error;
	if(m_triggerMode==ePGSoftwareTrigger)
	{
		if(CheckSoftwareTriggerPresence())
		{
			if(CheckTriggerReady())
			{
				error = m_pCamera->WriteRegister(0x62C,0x80000000);
				if(error!=PGRERROR_OK)
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	return true;
}

// 获取图像的像素格式
PGPixelFormat CPGCamera::GetImagePixelFormat()
{
	return m_pixelFormat;
}

// 获取图像的宽
long CPGCamera::GetImageWidth()
{
	return m_lWidth;
}

// 获取图像的高
long CPGCamera::GetImageHeight()
{
	return m_lHeight;
}

// 获取存储图像所需的内存大小（字节）
long CPGCamera::GetImageDataSize()
{
	int nBytes = 1;
	switch(m_pixelFormat)
	{
	case ePGGrey8:
		nBytes = 1;
		break;
	case ePGRGB24:
		nBytes = 3;
		break;
	case ePGRGBA32:
		nBytes = 4;
		break;
	default:
		nBytes = 1;
	}
	return m_lWidth*m_lHeight*nBytes;
}

// 获取图像数据，要求外部已分配好内存
// 内存的大小由GetImageDataSize()获得
bool CPGCamera::GetImageData(unsigned char *pImage)
{
	if(NULL==pImage)
	{
		return false;
	}
	EnterCriticalSection(&m_criticalSection);
	if(!m_bAcquireSuccess||m_pGrabImage==NULL||
		m_lWidth<=0||m_lHeight<=0)
	{
		LeaveCriticalSection(&m_criticalSection);
		return false;
	}
	memcpy(pImage,m_pGrabImage,GetImageDataSize());
	LeaveCriticalSection(&m_criticalSection);
	return true;
}

// 将指定视频模式转为在相机寄存器中相应的索引值
bool CPGCamera::ConvertVideoModeToULongs(VideoMode vmVideoMode,unsigned long *pFormat, unsigned long *pMode)
{
	if(NULL==pFormat||NULL==pMode)
	{
		return false;
	}
	unsigned long ulFormat = 0;
	unsigned long ulMode = 0;
	for(ulFormat=0;ulFormat<8;ulFormat++)
	{
		for(ulMode=0;ulMode<8;ulMode++)
		{
			if(vmVideoMode==g_fvmFormatVideoMode[ulFormat][ulMode])
			{
				*pFormat = ulFormat;
				*pMode = ulMode;
				return true;
			}
		}
	}
	return false;
}

// 将指定帧率模式转为在相机寄存器中相应的索引值
bool CPGCamera::ConvertFrameRateToULong(FrameRate frFrameRate,unsigned long *pFrameRate)
{
	if(NULL==pFrameRate)
	{
		return false;
	}
	switch(frFrameRate)
	{
	case FRAMERATE_1_875:
		*pFrameRate = 0;
		break;
	case FRAMERATE_3_75:
		*pFrameRate = 1;
		break;
	case FRAMERATE_7_5:
		*pFrameRate = 2;
		break;
	case FRAMERATE_15:
		*pFrameRate = 3;
		break;
	case FRAMERATE_30:
		*pFrameRate = 4;
		break;
	case FRAMERATE_60:
		*pFrameRate = 5;
		break;
	case FRAMERATE_120:
		*pFrameRate = 6;
		break;
	case FRAMERATE_240:
		*pFrameRate = 7;
		break;
	case FRAMERATE_FORMAT7:
		*pFrameRate = 8;
		break;
	default:
		return false;
	}
	return true;
}

// 根据当前视频模式充值当前帧率模式
bool CPGCamera::ResetCurFrameRateByVideoMode()
{
	FrameRate frFrameRate;
	if(!GetCurFrameRate(&frFrameRate))
	{
		return false;
	}
	if(IsFrameRateSupported(frFrameRate))
	{
		return true;
	}
	if(!GetMaxFrameRate(&frFrameRate))
	{
		return false;
	}
	return SetCurFrameRate(frFrameRate);
}

// 根据当前视频模式重置曝光时间参数
bool CPGCamera::ResetCurShutterByFrameRate()
{
	float fMaxShutter;
	float fCurShutter;
	float fMinShutter;
	if(!GetShutterRange(&fMinShutter,&fMaxShutter))
	{
		return false;
	}
	if(!GetShutter(&fCurShutter))
	{
		return false;
	}
	if(fCurShutter>fMaxShutter)
	{
		SetShutter(fMaxShutter);
	}
	return true;
}

// 设置采集延时，单位：ms
bool CPGCamera::SetGrabTimeout(unsigned long lTimeout)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}

	Error error;
	FC2Config config;
	error = m_pCamera->GetConfiguration(&config);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	config.grabTimeout = lTimeout;
	error = m_pCamera->SetConfiguration(&config);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	return true;
}

// 将当前参数保存到相机中
// lChannel：通道号
bool CPGCamera::SaveToCamera(long lChannel /* = 1 */)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}

	Error error;
	error = m_pCamera->SaveToMemoryChannel(lChannel);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	return true;
}

// 设置异步和同步总线速度
bool CPGCamera::SetBusSpeed(BusSpeed asyncBusSpeed,BusSpeed isochBusSpeed)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	Error error;
	FC2Config config;
	error = m_pCamera->GetConfiguration(&config);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	config.asyncBusSpeed = asyncBusSpeed;
	config.isochBusSpeed = isochBusSpeed;
	error = m_pCamera->SetConfiguration(&config);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	return true;
}

// 获取异步和同步总线速度
bool CPGCamera::GetBusSpeed(BusSpeed *pAsyncBusSpeed,BusSpeed *pIsochBusSpeed)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	Error error;
	FC2Config config;
	error = m_pCamera->GetConfiguration(&config);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	*pAsyncBusSpeed = config.asyncBusSpeed;
	*pIsochBusSpeed = config.isochBusSpeed;
	return true;
}

// 获取相机类型
PGCameraType CPGCamera::GetCameraType()
{
	return m_cameraType;
}

// 写寄存器
bool CPGCamera::WriteRegisiter(unsigned int nAdd,unsigned int nValue)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	Error error;
	error = m_pCamera->WriteRegister(nAdd,nValue);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	return true;
}

// 读寄存器
bool CPGCamera::ReadRegisiter(unsigned int nAdd,unsigned int *pValue)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	Error error;
	error = m_pCamera->ReadRegister(nAdd,pValue);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	return true;
}

// 相机是否连接
bool CPGCamera::IsOnline()
{
	return (m_bOnline&&m_pCamera->IsConnected());
}

// 采集线程函数
// 只有当采集模式为eGrabThread时，才会创建采集线程
UINT CPGCamera::GrabThread(LPVOID lpParam)
{
	CPGCamera *pOwner = (CPGCamera *)lpParam;
	if(NULL==pOwner)
	{
		return 0;
	}
	for(;;)
	{
		// 当收到了运行采集循环的事件，包括单帧采集和连续采集
		WaitForSingleObject(pOwner->m_hEventToRun,INFINITE);
		// 如果结束线程标志为真，则退出循环
		if(pOwner->m_bTerminate)
		{
			break;
		}
		switch(pOwner->m_threadStatus)
		{
		case ePGSnap:
			{
				// 采集一幅图像
				PGStatus status = pOwner->AcquireImage();
				if(pOwner->m_bSendCallbackEnable)
				{
					pOwner->SendCallback(status);
				}
				// 允许继续进行采集
				ResetEvent(pOwner->m_hEventToRun);
				// 通知调用线程停止等待
				SetEvent(pOwner->m_hEventSnapped);
			}
			break;
		case ePGGrab:
			{
				// 标记连续采集开始
				pOwner->m_bHaveStopped = !pOwner->m_bIsGrabbing;
				// 是否允许连续采集
				while(pOwner->m_bIsGrabbing)
				{
					// 采集一幅图像
					PGStatus status = pOwner->AcquireImage();
					if(pOwner->m_bSendCallbackEnable)
					{
						pOwner->SendCallback(status);
					}
					Sleep(2);
				}
				// 标记连续采集已经停止
				pOwner->m_bHaveStopped = true;
				// 允许继续进行采集
				ResetEvent(pOwner->m_hEventToRun);
			}
			break;
		}
	}
	// 发送采集线程结束消息
	SetEvent(pOwner->m_hEventExit);
	return 0;
}

// 创建采集线程
// 只有采集模式为eGrabThread时，才会创建采集线程
bool CPGCamera::CreateGrabThread()
{
	if(!m_bTerminate)
	{
		return false;
	}
	m_bTerminate = false;
	m_hEventSnapped = CreateEvent(NULL,true,false,_T(""));
	m_hEventToRun = CreateEvent(NULL,true,false,_T(""));
	m_hEventExit = CreateEvent(NULL,true,false,_T(""));
	//创建采集线程
	m_pWinThread = AfxBeginThread(GrabThread,this,0,0,0,NULL);
	if(NULL==m_pWinThread)
	{
		return false;
	}
	return true;
}

// 终止采集线程
// 只有采集模式为eGrabThread时，才会终止采集线程
bool CPGCamera::TerminateGrabThread()
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	if(m_bTerminate)
	{
		return true;
	}
	if(m_bIsGrabbing)
	{
		Freeze();
	}
	// 关闭采集线程
	m_bTerminate = true;
	SetEvent(m_hEventToRun);
	if(WAIT_TIMEOUT == WaitForSingleObject(m_hEventExit,1000))
	{
		Sleep(500);
	}
	if(m_pWinThread!=NULL)
	{
		m_pWinThread = NULL;
	}
	CloseHandle(m_hEventExit);
	m_hEventExit = NULL;
	CloseHandle(m_hEventSnapped);
	m_hEventSnapped = NULL;
	CloseHandle(m_hEventToRun);
	m_hEventToRun = NULL;
	return true;
}

// 从相机采集一幅图像，并进行转换
// 返回两种标志结果：相机采集是否成功，图像采集是否成功
// 当从相机采集一幅图像成功时，并成功转换相应格式后，返回ePGGrabOK
// 当从相机采集一幅图像成功时，但未成功进行格式转换，则返回ePGGrabFailed
// 当从相机采集一幅图像超时时，返回ePGGrabTimeout
// 当从相机采集一幅图像出错时，如果是调用了Freeze,则根据原来图像采集状态返回ePTGrabOK或ePTGrabFailed
PGStatus CPGCamera::AcquireImage()
{
	CString strError;
	EnterCriticalSection(&m_criticalSection);
	bool bAcquireSuccess = m_bAcquireSuccess;
	m_bAcquireSuccess = false;
	bool bIsSend = false;
	if(m_bSendGrabTrigger)
	{
		bIsSend = SendSoftTrigger();
	}
	Error error;
	// 从相机采集图像
	error = m_pCamera->RetrieveBuffer(&m_ImageRaw);
	//strError.Format("RetrieveBuffer = %d",(int)error.GetType());
	//AfxMessageBox(strError);
	// FrameCounter计数
	m_stImageMetadataCounter = m_ImageRaw.GetMetadata();
	if(m_bIsFrameCounter)
	{
		m_nPreFrameCounter = m_stImageMetadataCounter.embeddedFrameCounter;
		m_nFrameCounter = m_nFrameCounter+1;
		m_bIsFrameCounter = false;
	}
	else
	{
		m_nAfterFrameCounter = m_stImageMetadataCounter.embeddedFrameCounter;
		m_nMissCounter = m_nMissCounter+m_nAfterFrameCounter-m_nPreFrameCounter-1;
		m_nFrameCounter = m_nFrameCounter + m_nAfterFrameCounter - m_nPreFrameCounter;
		m_nPreFrameCounter = m_nAfterFrameCounter;
	}

	if(error == PGRERROR_OK)
	{
		m_bAcquireSuccess = TransferData();
		LeaveCriticalSection(&m_criticalSection);
		// 相机采集成功，图像获取是否成功由m_bAcquireSuccess决定
		//strError.Format("m_bAcquireSuccess = %d",(int)m_bAcquireSuccess);
		//AfxMessageBox(strError);
		return m_bAcquireSuccess ? ePGGrabOK : ePGGrabFailed;
	}
	else if(error == PGRERROR_TIMEOUT)
	{
		m_bAcquireSuccess = false;
		LeaveCriticalSection(&m_criticalSection);
		return ePGGrabTimeout;
	}
	else
	{
		if(m_bFreezeStop)
		{
			m_bAcquireSuccess = bAcquireSuccess;
		}
		else
		{
			m_bAcquireSuccess = false;
		}
		//strError.Format("m_bAcquireSuccess = %d",(int)m_bAcquireSuccess);
		//AfxMessageBox(strError);
		LeaveCriticalSection(&m_criticalSection);
		return m_bAcquireSuccess ? ePGGrabOK : ePGGrabFailed;
	}
}

// 重新扫描总线，当前相机连接会被中断
bool CPGCamera::RescanBus()
{
	if(m_AutoBusManager.get() == NULL)
	{
		m_AutoBusManager = std::auto_ptr<BusManager>(new BusManager);
	}
	BusManager *pBusManager = m_AutoBusManager.get();
	Error error;
	error = pBusManager->RescanBus();
	if(error != PGRERROR_OK)
	{
		return false;
	}
	Sleep(1000);
	return true;
}

// 获取总线上相机的数量
bool CPGCamera::GetBusCameraCount(unsigned int *pCameraCount)
{
	if(m_AutoBusManager.get()==NULL)
	{
		m_AutoBusManager = std::auto_ptr<BusManager>(new BusManager);
	}
	BusManager *pBusManager = m_AutoBusManager.get();
	Error error;
	error = pBusManager->GetNumOfCameras(pCameraCount);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	return true;
}

// 获取总线上各相机的GUID值
// pGuid：必须分配内存；pSize：返回相机个数
bool CPGCamera::EnumerateBusCameras(PGRGuid *pGuid,unsigned int *pSize)
{
	if(m_AutoBusManager.get()==NULL)
	{
		m_AutoBusManager = std::auto_ptr<BusManager>(new BusManager);
	}
	BusManager *pBusManager = m_AutoBusManager.get();
	unsigned int uCameraNum;
	if(!GetBusCameraCount(&uCameraNum)||uCameraNum<=0)
	{
		return false;
	}
	if(!pGuid||!pSize)
	{
		return false;
	}
	*pSize = uCameraNum;
	Error error;
	for(unsigned int i=0;i<uCameraNum;i++)
	{
		PGRGuid guid;
		error = pBusManager->GetCameraFromIndex(i,&guid);
		if(error!=PGRERROR_OK)
		{
			pGuid[i] = guid;
		}
		else
		{
			pGuid[i] = guid;
		}
	}
	return true;
}

// 获取总线上各相机的序列号
// pSerialInfo：必须分配内存；pSize：返回相机个数
bool CPGCamera::EnumerateBusCameras(unsigned int *pSerialInfo,unsigned int *pSize)
{
	if(m_AutoBusManager.get()==NULL)
	{
		m_AutoBusManager = std::auto_ptr<BusManager>(new BusManager);
	}
	BusManager *pBusManager = m_AutoBusManager.get();
	unsigned int uCameraNum;
	if(!GetBusCameraCount(&uCameraNum)||uCameraNum<=0)
	{
		return false;
	}

	if(!pSerialInfo||!pSize)
	{
		return false;
	}
	*pSize = uCameraNum;
	Error error;
	for(unsigned int i=0;i<uCameraNum;i++)
	{
		unsigned int uSerialNum;
		PGRGuid guid;
		error = pBusManager->GetCameraSerialNumberFromIndex(i, &uSerialNum);
		if(error==PGRERROR_OK)
		{
			pSerialInfo[i] = uSerialNum;
		}
		else
		{
			pSerialInfo[i] = 0;
		}
	}
	return true;
}

// 获取总线上各相机的信息
// pCameraInfo：必须分配内存；pSize：返回相机个数
bool CPGCamera::EnumerateBusCameras(PGCameraInfo *pCameraInfo,unsigned int *pSize)
{
	if(m_AutoBusManager.get()==NULL)
	{
		m_AutoBusManager = std::auto_ptr<BusManager>(new BusManager);
	}
	BusManager *pBusManager = m_AutoBusManager.get();
	unsigned int uCameraNum;
	PGRGuid guid;
	GigECamera gigECamera;
	Camera camera;
	if(!GetBusCameraCount(&uCameraNum)||uCameraNum<=0)
	{
		return false;
	}
	if(!pCameraInfo||!pSize)
	{
		return false;
	}
	*pSize = uCameraNum;
	Error error;
	for(unsigned int i=0;i<uCameraNum;i++)
	{
		pCameraInfo[i].m_uiIndex = i;
		unsigned int uiSerialNum;
		error = pBusManager->GetCameraSerialNumberFromIndex(i,&uiSerialNum);
		if(error == PGRERROR_OK)
		{
			pBusManager->GetCameraFromSerialNumber(uiSerialNum,&guid);
			pCameraInfo[i].m_uiSerialNum = uiSerialNum;
			pCameraInfo[i].m_CameraType = GetCameraTypeFromSerialNum(uiSerialNum);
			if(eGigE == pCameraInfo[i].m_CameraType)
			{
				if(gigECamera.Connect(&guid)==PGRERROR_OK)
				{
					CameraInfo tmpCameraInfo;
					gigECamera.GetCameraInfo(&tmpCameraInfo);
					strcpy(pCameraInfo[i].m_cModelName,tmpCameraInfo.modelName);
					strcpy(pCameraInfo[i].m_cResolution,tmpCameraInfo.sensorResolution);
				}
				GetGigECameraIPAddressByIndex(i,pCameraInfo[i].m_ipAddress,pCameraInfo[i].m_subnetMask,pCameraInfo[i].m_defaultGateway);
			}
			else
			{
				if(camera.Connect(&guid)==PGRERROR_OK)
				{
					CameraInfo tmpCameraInfo;
					camera.GetCameraInfo(&tmpCameraInfo);
					strcpy(pCameraInfo[i].m_cModelName,tmpCameraInfo.modelName);
					strcpy(pCameraInfo[i].m_cResolution,tmpCameraInfo.sensorResolution);
				}
			}
		}
		else
		{
			pCameraInfo[i].m_uiSerialNum = 0;
			pCameraInfo[i].m_CameraType = eInterfaceUnknow;
		}
	}
	return true;
}

// 根据索引获取相机的GUID
bool CPGCamera::GetCameraGuidFromIndex(int nIndex,PGRGuid *pGuid)
{
	if(m_AutoBusManager.get()==NULL)
	{
		m_AutoBusManager = std::auto_ptr<BusManager>(new BusManager);
	}
	BusManager *pBusManager = m_AutoBusManager.get();
	unsigned int uiCameraNum;
	if(!GetBusCameraCount(&uiCameraNum)||uiCameraNum<=0)
	{
		return false;
	}
	Error error;
	error = pBusManager->GetCameraFromIndex(nIndex,pGuid);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	return true;
}

// 根据序列号获取相机的GUID
bool CPGCamera::GetCameraGuidFromSerialNum(unsigned int nSerialNum,PGRGuid *pGuid)
{
	if(m_AutoBusManager.get()==NULL)
	{
		m_AutoBusManager = std::auto_ptr<BusManager>(new BusManager);
	}
	BusManager *pBusManager = m_AutoBusManager.get();
	unsigned int uiCameraNum;
	if(!GetBusCameraCount(&uiCameraNum)||uiCameraNum<=0)
	{
		return false;
	}
	Error error;
	error = pBusManager->GetCameraFromSerialNumber(nSerialNum,pGuid);
	if(error != PGRERROR_OK)
	{
		return false;
	}
	return true;
}

// 根据GUID获取相机的类型
PGCameraType CPGCamera::GetCameraTypeFromGuid(PGRGuid *pGuid)
{
	if(m_AutoBusManager.get()==NULL)
	{
		m_AutoBusManager = std::auto_ptr<BusManager>(new BusManager);
	}
	BusManager *pBusManager = m_AutoBusManager.get();
	PGCameraType cameraType(eInterfaceUnknow);
	Error error;
	InterfaceType interfaceType;
	error = pBusManager->GetInterfaceTypeFromGuid(pGuid,&interfaceType);
	if(error!=PGRERROR_OK)
	{
		return eInterfaceUnknow;
	}
	if(INTERFACE_GIGE == interfaceType)
	{
		cameraType = eGigE;
	}
	else if(INTERFACE_IEEE1394 == interfaceType)
	{
		cameraType = eIEEE1394;
	}
	else if(INTERFACE_USB2 == interfaceType)
	{
		cameraType = eUSB2;
	}
	else if(INTERFACE_USB3 == interfaceType)
	{
		cameraType = eUSB3;
	}
	return cameraType;
}

// 根据索引获取相机的类型
PGCameraType CPGCamera::GetCameraTypeFromIndex(unsigned int nIndex)
{
	if(m_AutoBusManager.get()==NULL)
	{
		m_AutoBusManager = std::auto_ptr<BusManager>(new BusManager);
	}
	BusManager *pBusManager = m_AutoBusManager.get();
	PGRGuid guid;
	if(!GetCameraGuidFromIndex(nIndex,&guid))
	{
		return eInterfaceUnknow;
	}
	PGCameraType cameraType(eInterfaceUnknow);
	Error error;
	InterfaceType interfaceType;
	error = pBusManager->GetInterfaceTypeFromGuid(&guid,&interfaceType);
	if(INTERFACE_GIGE == interfaceType)
	{
		cameraType = eGigE;
	}
	else if(INTERFACE_IEEE1394 == interfaceType)
	{
		cameraType = eIEEE1394;
	}
	else if(INTERFACE_USB2 == interfaceType)
	{
		cameraType = eUSB2;
	}
	else if(INTERFACE_USB3 == interfaceType)
	{
		cameraType = eUSB3;
	}
	return cameraType;
}

// 根据序列号获取相机的类型
PGCameraType CPGCamera::GetCameraTypeFromSerialNum(unsigned int nSerialNum)
{
	if(m_AutoBusManager.get() == NULL)
	{
		m_AutoBusManager = std::auto_ptr<BusManager>(new BusManager);
	}
	BusManager *pBusManager = m_AutoBusManager.get();
	PGRGuid guid;
	if(!GetCameraGuidFromSerialNum(nSerialNum,&guid))
	{
		return eInterfaceUnknow;
	}
	PGCameraType cameraType(eInterfaceUnknow);
	Error error;
	InterfaceType interfaceType;
	error = pBusManager->GetInterfaceTypeFromGuid(&guid,&interfaceType);
	if(INTERFACE_GIGE == interfaceType)
	{
		cameraType = eGigE;
	}
	else if(INTERFACE_IEEE1394 == interfaceType)
	{
		cameraType = eIEEE1394;
	}
	else if(INTERFACE_USB2 == interfaceType)
	{
		cameraType = eUSB2;
	}
	else if(INTERFACE_USB3 == interfaceType)
	{
		cameraType = eUSB3;
	}
	return cameraType;
}

// 根据GUID设置GigE相机的IP、子网掩码和默认网关
bool CPGCamera::SetGigECameraIPAddressByGuid(PGRGuid *pGuid,IPAddress ipAddress,
											 IPAddress subnetAddress,IPAddress defaultAddress)
{
	CameraInfo cameraInfo[128];
	memset(cameraInfo,0,128*sizeof(CameraInfo));
	unsigned int uiCameraCnt;
	Error error;
	uiCameraCnt = 128;
	error = BusManager::DiscoverGigECameras(cameraInfo,&uiCameraCnt);
	if(error != PGRERROR_OK || uiCameraCnt<=0)
	{
		return false;
	}
	for(int j=0;j<uiCameraCnt;j++)
	{
		PGRGuid tmpGuid;
		if(GetCameraGuidFromSerialNum(cameraInfo[j].serialNumber,&tmpGuid))
		{
			if(tmpGuid==*pGuid)
			{
				MACAddress macAddress;
				macAddress = cameraInfo[j].macAddress;
				error = BusManager::ForceIPAddressToCamera(macAddress,ipAddress,
					subnetAddress,defaultAddress);
				if(error == PGRERROR_OK)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}
	return false;
}

// 根据序列号设置GigE相机的IP、子网掩码和默认网关
bool CPGCamera::SetGigECameraIPAddressBySerialNum(unsigned int nSerialNum,IPAddress ipAddress,
												  IPAddress subnetAddress,IPAddress defaultGateway)
{
	PGRGuid guid;
	if(!GetCameraGuidFromSerialNum(nSerialNum,&guid))
	{
		return false;
	}
	return SetGigECameraIPAddressByGuid(&guid,ipAddress,subnetAddress,defaultGateway);
}

// 根据索引号设置GigE相机的IP、子网掩码和默认网关
bool CPGCamera::SetGigECameraIPAddressByIndex(unsigned int nIndex,IPAddress ipAddress,
											  IPAddress subnetAddress,IPAddress defaultGateway)
{
	PGRGuid guid;
	if(!GetCameraGuidFromIndex(nIndex,&guid))
	{
		return false;
	}
	return SetGigECameraIPAddressByGuid(&guid,ipAddress,subnetAddress,defaultGateway);
}

// 根据GUID获取GigE相机的IP、子网掩码和默认网关
bool CPGCamera::GetGigECameraIPAddressByGuid(PGRGuid *pGuid,IPAddress &ipAddress,
											 IPAddress &subnetMask,IPAddress &defaultGateway)
{
	CameraInfo cameraInfo[128];
	memset(cameraInfo,0,128*sizeof(CameraInfo));
	unsigned int uiCameraCnt;
	Error error;
	uiCameraCnt = 128;
	error = BusManager::DiscoverGigECameras(cameraInfo,&uiCameraCnt);
	if(error != PGRERROR_OK||uiCameraCnt <= 0)
	{
		return false;
	}
	for(int j=0;j<uiCameraCnt;j++)
	{
		PGRGuid tmpGuid;
		if(GetCameraGuidFromSerialNum(cameraInfo[j].serialNumber,&tmpGuid))
		{
			if(tmpGuid == *pGuid)
			{
				ipAddress = cameraInfo[j].ipAddress;
				subnetMask = cameraInfo[j].subnetMask;
				defaultGateway = cameraInfo[j].defaultGateway;
				return true;
			}
		}
	}
	return false;
}

// 根据索引获取GigE相机的IP、子网掩码和默认网关
bool CPGCamera::GetGigECameraIPAddressByIndex(unsigned int nIndex,IPAddress &ipAddress,
											  IPAddress &subnetMask,IPAddress &defaultGateway)
{
	PGRGuid guid;
	if(!GetCameraGuidFromIndex(nIndex,&guid))
	{
		return false;
	}
	return GetGigECameraIPAddressByGuid(&guid,ipAddress,subnetMask,defaultGateway);
}

// 根据序列号获取GigE相机的IP、子网掩码和默认网关
bool CPGCamera::GetGigECameraIPAddressBySerialNum(unsigned int nSerialNum,IPAddress &ipAddress,
												  IPAddress &subnetMask,IPAddress &defaultGateway)
{
	PGRGuid guid;
	if(!GetCameraGuidFromSerialNum(nSerialNum,&guid))
	{
		return false;
	}
	return GetGigECameraIPAddressByGuid(&guid,ipAddress,subnetMask,defaultGateway);
}

// 重新设置总线上GigECamera相机IP
bool CPGCamera::AutoForceAllGigECameraIP()
{
	if(m_AutoBusManager.get() == NULL)
	{
		m_AutoBusManager = std::auto_ptr<BusManager>(new BusManager);
	}
	BusManager *pBusManager = m_AutoBusManager.get();
	Error error;
	// 强制设置IP
	error = pBusManager->ForceAllIPAddressesAutomatically();
	if(error != PGRERROR_OK)
	{
		return false;
	}
	// 设置完成后，等待5s
	Sleep(5000);
	// 重新扫描总线
	error = pBusManager->RescanBus();
	if(error != PGRERROR_OK)
	{
		return false;
	}
	return true;
}

// 获取Format7参数
bool CPGCamera::GetFormat7Config(PGFormat7Info &format7Info)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	if(eInterfaceUnknow == m_cameraType)
	{
		return false;
	}
	if(eGigE == m_cameraType)
	{
		GigECamera *pCamera = (GigECamera *)m_pCamera;
		Mode mode;
		Error error;
		error = pCamera->GetGigEImagingMode(&mode);
		if(error!=PGRERROR_OK)
		{
			return false;
		}
		format7Info.m_mode = mode;

		GigEImageSettings imageSettings;
		error = pCamera->GetGigEImageSettings(&imageSettings);
		if(error != PGRERROR_OK)
		{
			return false;
		}
		format7Info.m_uiStartX = imageSettings.offsetX;
		format7Info.m_uiStartY = imageSettings.offsetY;
		format7Info.m_uiWidth = imageSettings.width;
		format7Info.m_uiHeight = imageSettings.height;
		format7Info.m_pixelFormat = imageSettings.pixelFormat;
	}
	else // IEEE1394或USB
	{
		Camera *pCamera = (Camera *)m_pCamera;
		Error error;
		Format7ImageSettings formatSettings;
		unsigned int uiPacketSize;
		float fBufSpeed;
		error = pCamera->GetFormat7Configuration(&formatSettings,&uiPacketSize,&fBufSpeed);
		if(error != PGRERROR_OK)
		{
			return false;
		}
		format7Info.m_fBusSpeed = fBufSpeed;
		format7Info.m_uiStartX = formatSettings.offsetX;
		format7Info.m_uiStartY = formatSettings.offsetY;
		format7Info.m_uiWidth = formatSettings.width;
		format7Info.m_uiHeight = formatSettings.height;
		format7Info.m_mode = formatSettings.mode;
		format7Info.m_pixelFormat = formatSettings.pixelFormat;
	}
	return true;
}

// 设置Format7参数
bool CPGCamera::SetFormat7Config(PGFormat7Info format7Info)
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	if(eInterfaceUnknow == m_cameraType)
	{
		return false;
	}
	if(eGigE == m_cameraType)
	{
		GigECamera *pCamera = (GigECamera *)m_pCamera;
		Error error;
		Mode mode = format7Info.m_mode;
		bool bSupported;
		error = pCamera->QueryGigEImagingMode(mode,&bSupported);
		if(error != PGRERROR_OK||!bSupported)
		{
			return false;
		}
		error = pCamera->SetGigEImagingMode(mode);
		if(error!=PGRERROR_OK)
		{
			return false;
		}
		GigEImageSettings imageSettings;
		imageSettings.offsetX = format7Info.m_uiStartX;
		imageSettings.offsetY = format7Info.m_uiStartY;
		imageSettings.width = format7Info.m_uiWidth;
		imageSettings.height = format7Info.m_uiHeight;
		imageSettings.pixelFormat = format7Info.m_pixelFormat;
		error = pCamera->SetGigEImageSettings(&imageSettings);
		if(error != PGRERROR_OK)
		{
			return false;
		}
	}
	else
	{
		Camera *pCamera = (Camera *)m_pCamera;
		Error error;
		Format7ImageSettings formatSettings;
		formatSettings.offsetX = format7Info.m_uiStartX;
		formatSettings.offsetY = format7Info.m_uiStartY;
		formatSettings.width = format7Info.m_uiWidth;
		formatSettings.height = format7Info.m_uiHeight;
		formatSettings.pixelFormat = format7Info.m_pixelFormat;
		formatSettings.mode = format7Info.m_mode;
		error = pCamera->SetFormat7Configuration(&formatSettings,format7Info.m_fBusSpeed);
		if(error != PGRERROR_OK)
		{
			return false;
		}
	}
	return true;
}

// 采集单帧图像
// 该采集函数不能与其他采集函数同时调用
// 该采集函数直到采集完成后退出
bool CPGCamera::SnapImage()
{
	//AfxMessageBox("SnapImage");
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	m_pCamera->SetCallback(NULL,NULL); // 需要先将回调函数设为空
	m_pCamera->StartCapture(NULL,NULL);
	PGStatus status = AcquireImage();
	if(m_bSendCallbackEnable)
	{
		SendCallback(status);
	}
	m_pCamera->StopCapture();
	//m_pCamera->SetCallback(NULL,NULL);
	return IsAcquireSuccess();
}

// 利用软触发方式，采集单帧图像
// 发送完软触发信号后，直到采集单帧图像完成后，该函数才返回
// 调用该函数，需要将相机设置为软触发模式
// 该函数不依赖于采集线程
bool CPGCamera::TriggerSnap()
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	m_pCamera->StopCapture();
	m_pCamera->SetCallback(NULL,NULL);
	m_pCamera->StartCapture(NULL,NULL);
	if(!SendSoftTrigger())
	{
		return false;
	}

	PGStatus status = AcquireImage();
	if(m_bSendCallbackEnable)
	{
		SendCallback(status);
	}
	m_pCamera->StopCapture();
	return IsAcquireSuccess();
}

// 图像是否采集成功
bool CPGCamera::IsAcquireSuccess()
{
	return m_bAcquireSuccess;
}

// 设置状态，设置brightness、exposure、shutter、gain、framerate为手动状态
bool CPGCamera::SetStatusManual()
{
	if(!m_bOnline||!m_pCamera->IsConnected())
	{
		return false;
	}
	Error error;
	unsigned int uiValue = 0xC2000000;
	// 设置Brightness为Abs_Value手动方式
	error = m_pCamera->WriteRegister(0x800,uiValue);
	if(error != PGRERROR_OK)
	{
		return false;
	}
	// 设置exposure为Abs_Value手动方式
	error = m_pCamera->WriteRegister(0x804,uiValue);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	// 设置shutter时间为Abs_Value手动方式
	error = m_pCamera->WriteRegister(0x81C,uiValue);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	// 设置gain为Abs_Value手动模式
	error = m_pCamera->WriteRegister(0x820,uiValue);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	return true;
}

bool CPGCamera::Destroy()
{
	TerminateGrabThread();
	// 清空
	if(m_pGrabImage!=NULL)
	{
		delete[] m_pGrabImage;
		m_pGrabImage = NULL;
		m_lWidth = 0;
		m_lHeight = 0;
	}
	m_bOnline = false;
	return true;
}