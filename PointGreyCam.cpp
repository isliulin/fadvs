#include "stdafx.h"
#include "PointGreyCam.h"


#pragma comment(lib,"PointGrey/lib/FlyCapture2.lib")

PointGreyCam::PointGreyCam()
{
	Initialize(); //LogCam:专用记录相机异常
	m_pLog= new CLogFile(CFunction::GetDirectory() + "LogCam\\LogFile.log");
}

PointGreyCam::~PointGreyCam(void)
{
	CloseCamera();
	if(m_pCamBase)
	{
		delete m_pCamBase;
		m_pCamBase=NULL;       //2017-11-03
	}
	if (m_pGrabImage)
	{
		delete m_pGrabImage;
		m_pGrabImage=NULL;    //2017-11-03
	}
	if(m_pLog)
	{
		delete m_pLog;
	}
}

int PointGreyCam::GetCameraList(vector<int> *devices)
{
	BusManager busMgr;
	unsigned int numCameras;
	Error error;
	devices->clear();
	error = busMgr.GetNumOfCameras(&numCameras);//获取相机数量
	if (error != PGRERROR_OK || numCameras < 1)
	{
		return -1;
	}

	for(int i = 0; i < numCameras; i++)
	{

		PGRGuid guid;
		
		unsigned int sn = 0;
		error = busMgr.GetCameraSerialNumberFromIndex(i,&sn);
		if (error != PGRERROR_OK)
		{
			return -1;
		}
	
		//error = busMgr.GetCameraFromIndex(i, &guid);// 获取指定相机信息
		
		if (sn == 0)
		{
			return -1;
		}

		devices->push_back(sn);//存放到list中
	}

	return 0;
}

long PointGreyCam::OpenCamera(const int sn,unsigned int offsetx,unsigned int offsety,unsigned int width,unsigned int height)
{
	CString outstr;
	try
	{
		BusManager busMgr;
		PGRGuid guid;
		unsigned int numCameras;
		Error error;
		const Mode k_fmt7Mode = MODE_0;
		const PixelFormat k_fmt7PixFmt = PIXEL_FORMAT_MONO8;
		offsetX=offsetx;
		offsetY=offsety;
		Width=width;
		Height=height;

		error=busMgr.RescanBus();                                 //2017-11-15 add///
		if(error!=PGRERROR_OK)                                                  ///
		{ 
			outstr.Format("ZP OpenCamera RescanBus Error %s\n",error.GetDescription());
			m_pLog->log(outstr);                                               ///
			return -1;                                                        ///
		}                                                    //2017-11-15 add///
		error = busMgr.GetNumOfCameras(&numCameras);
		if(error != PGRERROR_OK || numCameras < 1)
		{
			outstr.Format("ZP OpenCamera GetNumOfCameras Error %s;CameraNums:%d\n",error.GetDescription(),numCameras);
			m_pLog->log(outstr); 
			return -1;
		}

		//error = busMgr.GetCameraFromSerialNumber(sn,&guid);
		error = busMgr.GetCameraFromIndex(0,&guid);
		if(error!=PGRERROR_OK)
		{
			outstr.Format("ZP OpenCamera GetCameraFromIndex Error %s\n",error.GetDescription());
			m_pLog->log(outstr); 
			return -1;         
		}
		////////////////////////////////////////
		if(!m_pCamBase->IsConnected())
		{
			error = busMgr.GetInterfaceTypeFromGuid(&guid, &m_Type);
			if (m_Type == INTERFACE_GIGE)
			{
				m_pGigECam = new GigECamera();
				error = m_pGigECam->Connect(&guid);
				m_pCamBase = m_pGigECam;
			} 
			else
			{
				m_pCam = new Camera();
				error = m_pCam->Connect(&guid);
				m_pCamBase = m_pCam;
			}

			// Power on the camera
			const unsigned int k_cameraPower = 0x610;
			const unsigned int k_powerVal = 0x80000000;
			error  = m_pCamBase->WriteRegister( k_cameraPower, k_powerVal );
			if(error != PGRERROR_OK)
			{
				outstr.Format("ZP OpenCamera WriteRegister Error %s\n",error.GetDescription());
				m_pLog->log(outstr); 
				return -1;
			}
			///////////////////////////////////////
			if (m_Type == INTERFACE_GIGE) //网口相机
			{
				GigEImageSettingsInfo imageSettingsInfo;
				error = m_pGigECam->GetGigEImageSettingsInfo(&imageSettingsInfo);
				if(error != PGRERROR_OK)
				{
					outstr.Format("ZP OpenCamera GetGigEImageSettingsInfo Error %s\n",error.GetDescription());
					m_pLog->log(outstr);
					return -1;
				}

				GigEImageSettings imageSettings;
				m_pGigECam->GetGigEImageSettings(&imageSettings);	
				if(error!=PGRERROR_OK)
				{
					outstr.Format("ZP OpenCamera GetGigEImageSettings Error %s\n",error.GetDescription());
					m_pLog->log(outstr);
					return -1;                 
				}
				imageSettings.offsetX = offsetX;
				imageSettings.offsetY = offsetY;
				imageSettings.height = Height;
				imageSettings.width = Width;
				imageSettings.pixelFormat = PIXEL_FORMAT_MONO8;

				error = m_pGigECam->SetGigEImageSettings(&imageSettings);
				if(error != PGRERROR_OK)
				{
					outstr.Format("ZP OpenCamera SetGigEImageSettings Error %s\n",error.GetDescription());
					m_pLog->log(outstr);
					return -1;
				}
			} 
			else   //USB相机和其他接口
			{
				Format7Info fmt7Info;
				bool supported;
				fmt7Info.mode = MODE_0;
				error = m_pCam->GetFormat7Info(&fmt7Info, &supported );
				if(error != PGRERROR_OK)
				{
					outstr.Format("ZP OpenCamera GetFormat7Info Error %s\n",error.GetDescription());
					m_pLog->log(outstr);
					return -1;
				}

				if ((PIXEL_FORMAT_MONO8 & fmt7Info.pixelFormatBitField) == 0)
				{
					return -1;
				}
				Format7ImageSettings fmt7ImageSettings;
				unsigned int packet;
				float percentage;
				error = m_pCam->GetFormat7Configuration(&fmt7ImageSettings,&packet,&percentage);

				fmt7ImageSettings.mode = MODE_0;
				fmt7ImageSettings.offsetX = 0;
				fmt7ImageSettings.offsetY = 0;
				fmt7ImageSettings.width = fmt7Info.maxWidth;
				fmt7ImageSettings.height = fmt7Info.maxHeight;
				fmt7ImageSettings.pixelFormat = PIXEL_FORMAT_MONO8;

				bool valid;
				Format7PacketInfo fmt7PacketInfo;

				// Validate the settings to make sure that they are valid
				error = m_pCam->ValidateFormat7Settings(&fmt7ImageSettings, &valid, &fmt7PacketInfo);
				if(error != PGRERROR_OK || !valid)
				{
					outstr.Format("ZP OpenCamera ValidateFormat7Settings Error %s\n",error.GetDescription());
					m_pLog->log(outstr);
					return -1;
				}
				// Set the settings to the camera
				error = m_pCam->SetFormat7Configuration(&fmt7ImageSettings, fmt7PacketInfo.recommendedBytesPerPacket);
				if(error != PGRERROR_OK)
				{
					outstr.Format("ZP OpenCamera SetFormat7Configuration Error %s\n",error.GetDescription());
					m_pLog->log(outstr);
					return -1;
				}

			}
			///////////////////////////////////////
			TriggerMode triggerMode;
			error = m_pCamBase->GetTriggerMode( &triggerMode );
			if(error != PGRERROR_OK)
			{
				outstr.Format("ZP OpenCamera GetTriggerMode Error %s\n",error.GetDescription());
				m_pLog->log(outstr);
				return -1;
			}
			// Set camera to trigger mode 0
			triggerMode.onOff = true;
			triggerMode.mode = 0;
			triggerMode.parameter = 0;
			triggerMode.source = 7;

			error = m_pCamBase->SetTriggerMode( &triggerMode );
			if(error != PGRERROR_OK)
			{
				outstr.Format("ZP OpenCamera SetTriggerMode Error %s\n",error.GetDescription());
				m_pLog->log(outstr);
				return -1;
			}

			FC2Config config;
			error = m_pCamBase->GetConfiguration( &config );
			config.grabTimeout = 5000; //采集时间默认5000超时
			if(error != PGRERROR_OK)
			{
				outstr.Format("ZP OpenCamera GetConfiguration Error %s\n",error.GetDescription());
				m_pLog->log(outstr);
				return -1;
			}
			error = m_pCamBase->SetConfiguration( &config );
			if(error != PGRERROR_OK)
			{
				outstr.Format("ZP OpenCamera SetConfiguration Error %s\n",error.GetDescription());
				m_pLog->log(outstr);
				return -1;
			}

			if(!m_bInGrab)   //false in default
			{
				error = m_pCamBase->StartCapture();
				if(error != PGRERROR_OK)
				{
					outstr.Format("ZP OpenCamera StartCapture Error %s\n",error.GetDescription());
					m_pLog->log(outstr);
					return -1;
				}
				else
				{
					m_bInGrab = true;
				}
			}
		}
	}
	catch (CException* e)
	{
		CString Estr;
		e->GetErrorMessage(Estr.GetBuffer(0),1024);
		//outstr.Format(L"ZP OpenCamera Try Error %s\n",Estr);
		outstr.Format("ZP OpenCamera Try Error %s\n",Estr);
		//OutputDebugString(outstr);
		m_pLog->log(outstr);
		return -1;
	}
	catch (exception &e)
	{
		//outstr.Format(L"ZP OpenCamera Try Error %s\n",e.what());
		//OutputDebugString(outstr);
		outstr.Format("ZP OpenCamera Try Error %s\n",e.what());
		m_pLog->log(outstr);
		return -1;
	}
	catch(...)
	{
		//outstr.Format(L"ZP OpenCamera Try 未知错误 Error\n");
		//OutputDebugString(outstr.GetBuffer(0));
		outstr.Format("ZP OpenCamera Try 未知错误 Error\n");
		m_pLog->log(outstr);
		return -1;
	}

	return 0;
}

long PointGreyCam::CloseCamera()
{
	Error error;
	if(m_bInGrab)
	{
		error = m_pCamBase->StopCapture();//停止采集
		if (error != PGRERROR_OK)
		{
			m_bInGrab = false;
			return -1;
		}

		m_bInGrab = false;
	}

	if(m_pCamBase->IsConnected())
	{
		error = m_pCamBase->Disconnect();
		if (error != PGRERROR_OK)
		{
			return -1;
		}
	}
	return 0;
}

long PointGreyCam::Shot(Image &image)//输出图像
{
	CString outstr;
	if(!m_pCamBase->IsConnected())
	{
		//outstr.Format(L"ZP Shot not Opencamera Error \n");
		//OutputDebugString(outstr);
		outstr.Format("ZP Shot not Opencamera Error \n");
		m_pLog->log(outstr);
		return -1;
	}

	try
	{
		TriggerMode triggerMode;
		Error error;
		error = m_pCamBase->GetTriggerMode(&triggerMode);
		if(error != PGRERROR_OK)			
		{
			outstr.Format("ZP Shot GetTriggerMode Error %s\n",error.GetDescription());
			m_pLog->log(outstr);
			return -2;
		}

		if (!m_bInGrab)
		{		
			do 
			{	
				error = m_pCamBase->StartCapture(); //开始采集
				if(error != PGRERROR_OK)
				{
					outstr.Format("ZP Shot StartCapture Error %s\n",error.GetDescription());
					m_pLog->log(outstr);
					m_bInGrab = false;
					Sleep(10);
				}
				else
				{
					m_bInGrab = true;
				}
			} while (!m_bInGrab);

		}

		/*	Image rawImage; */
		if(triggerMode.onOff == true && triggerMode.mode == 0 && triggerMode.source == 7)//software
		{
			PollForTriggerReady(m_pCamBase);
			bool retVal = FireSoftwareTrigger(m_pCamBase);
			if(!retVal)
			{
				outstr.Format("ZP Shot FireSoftwareTrigger Error \n");
				m_pLog->log(outstr);
				return -1;        
			}
		}

		error = m_pCamBase->RetrieveBuffer(&rawImage);
		if(error != PGRERROR_OK)
		{
			outstr.Format("ZP Shot RetrieveBuffer Error %s\n",error.GetDescription());
			m_pLog->log(outstr);
			return -1;
		}
		//图像转换、输出
		if(!TransferData())
		{
			outstr.Format("ZP Shot TransferData Error\n");
			m_pLog->log(outstr);
			return -1;
		}
		//建议：在设置参数和关闭相机前停止采集(StopCapture)，不必每次在这个函数中停止采集
	}
	catch (CException* e)
	{
		CString Estr;
		e->GetErrorMessage(Estr.GetBuffer(0),1024);
		outstr.Format("ZP Shot Try Error %s\n",Estr);
		m_pLog->log(outstr);
		return -1;
	}
	catch (exception &e)
	{
		outstr.Format("ZP Shot Try Error %s\n",e.what());
		m_pLog->log(outstr);
		return -1;
	}
	catch(...)
	{
		outstr.Format("ZP Shot Try 未知错误 Error\n");
		m_pLog->log(outstr);
		return -1;
	}
	return 0;
}

long PointGreyCam::StartAcquire(int iRate)
{
	return -1;
}

long PointGreyCam::StopAcquire()
{
	Error error;
	CString outstr;
	if (!m_bInGrab) return 0;
	error = m_pCamBase->StopCapture();
	if (error != PGRERROR_OK)
	{
		outstr.Format("ZP Shot RetrieveBuffer Error %s\n",error.GetDescription());
		m_pLog->log(outstr);
	}
	m_bInGrab = false;
	return 0;
}


void PointGreyCam::Initialize()
{
	m_bInGrab = false;
	m_pCamBase = new Camera;
    m_pGrabImage=NULL;
}

bool PointGreyCam::PollForTriggerReady(CameraBase* pCamBase)
{
	const unsigned int k_softwareTrigger = 0x62C;
	Error error;
	unsigned int regVal = 0;
	CString outstr;

	do 
	{
		error = pCamBase->ReadRegister( k_softwareTrigger, &regVal );
		if(error != PGRERROR_OK)
		{
			outstr.Format("ZP PollForTriggerReady Error %s\n",error.GetDescription());
			m_pLog->log(outstr);
			return false;
		}
	} while ( (regVal >> 31) != 0 );

	return true;
}

bool PointGreyCam::FireSoftwareTrigger(CameraBase* pCamBase)
{
	const unsigned int k_softwareTrigger = 0x62C;
	const unsigned int k_fireVal = 0x80000000;
	Error error; 
	CString outstr;
	error = pCamBase->WriteRegister( k_softwareTrigger, k_fireVal );
	if(error != PGRERROR_OK)
	{
		outstr.Format("ZP FireSoftwareTrigger Error %s\n",error.GetDescription());
		m_pLog->log(outstr);
		return false;
	}
	return true;
}

bool PointGreyCam::GUIDStringToIntArray(const char* strGUID, unsigned int arrayValue[])
{
	int iLength = strlen(strGUID);
	if(iLength != 32)
	{
		return false;
	}

	char buffer[9];
	memset(buffer, 0, 9);

	for(int i = 0; i < 4; i++)
	{
		memcpy(buffer, &strGUID[i * 8], 8);

		unsigned int iValue;
		sscanf(buffer, "%x", &iValue);
		arrayValue[i] = iValue;
	}
	return true;
}

bool PointGreyCam::IntArrayToGUIDString(unsigned int arrayValue[], char strGUID[])
{
	char *pcharData = strGUID;
	for(int i = 0; i < 4; i++)
	{
		unsigned int iValue = arrayValue[i];
		char *pchar = pcharData + (i * 8);

		for(int j = 0; j < 8; j++)
		{
			int iCharValue = iValue % 16;
			char charLetter;
			if(iCharValue < 10)
			{
				charLetter = iCharValue + 48;
			}
			else
			{
				charLetter = 'A' + iCharValue - 10;
			}

			pchar[7 - j] = charLetter;
			iValue = iValue / 16;
		}
	}

	return true;
}
// 设置曝光时间，单位ms
bool PointGreyCam::SetShutter(float fShutter)
{
	if(!m_pCamBase->IsConnected())
	{
		return false;
	}
	Error error;
	Property cameraProperty;
	cameraProperty.type = SHUTTER;
	cameraProperty.absControl = true;
	error = m_pGigECam->GetProperty(&cameraProperty);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	cameraProperty.absValue = fShutter;
	cameraProperty.absControl = true;
	cameraProperty.onOff = true;
	cameraProperty.autoManualMode = false;
	error = m_pGigECam->SetProperty(&cameraProperty);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	return true;
}

// 获取曝光时间，单位ms
bool PointGreyCam::GetShutter(float *pShutter)
{
	if(!m_pCamBase->IsConnected())
	{
		return false;
	}
	Error error;
	Property cameraProperty;
	cameraProperty.type = SHUTTER;
	cameraProperty.absControl = true;
	error = m_pGigECam->GetProperty(&cameraProperty);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	*pShutter = cameraProperty.absValue;
	return true;
}

// 设置增益，单位db
bool PointGreyCam::SetGain(float fGain)
{
	if(!m_pCamBase->IsConnected())
	{
		return false;
	}
	Error error;
	Property cameraProperty;
	cameraProperty.type = GAIN;
	cameraProperty.absControl = true;
	error = m_pGigECam->GetProperty(&cameraProperty);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	cameraProperty.absValue = fGain;
	cameraProperty.absControl = true;
	cameraProperty.onOff = true;
	// 手动设置模式
	cameraProperty.autoManualMode = false;
	error = m_pGigECam->SetProperty(&cameraProperty);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	return true;
}

// 获取增益，单位db
bool PointGreyCam::GetGain(float *pGain)
{
	if(!m_pCamBase->IsConnected())
	{
		return false;
	}
	Error error;
	Property cameraProperty;
	cameraProperty.type = GAIN;
	cameraProperty.absControl = true;
	error = m_pGigECam->GetProperty(&cameraProperty);
	if(error!=PGRERROR_OK)
	{
		return false;
	}
	*pGain = cameraProperty.absValue;
	return true;
}

bool PointGreyCam::TransferData()
{
	if(!m_pCamBase->IsConnected())
	{
		return false;
	}
	if(NULL==rawImage.GetData())
	{
		return false;
	}
	// 获取图像像素格式
	// 获取图像的宽高
	unsigned int nWidth = rawImage.GetCols();
	unsigned int nHeight = rawImage.GetRows();
	// 分配图像数据内存
	if(NULL==m_pGrabImage)
	{
		Width = nWidth;
		Height = nHeight;
		m_pGrabImage = new unsigned char[nWidth*nHeight*1];
	}
	else
	{
		if(nWidth!=Width||nHeight!=Height)
		{
			delete[] m_pGrabImage;
			m_pGrabImage = NULL;
			Width = nWidth;
			Height = nHeight;
			m_pGrabImage = new unsigned char[nWidth*nHeight*1];
		}
	}
	if(NULL == m_pGrabImage)
	{
		return false;
	}
	memcpy(m_pGrabImage,rawImage.GetData(),Width*Height*1);
	return true;
}

bool PointGreyCam::IsConnected()
{
	if(!m_pCamBase)
	{
		return false;
	}
	return m_pCamBase->IsConnected();
}