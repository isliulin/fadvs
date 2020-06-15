#include "StdAfx.h"
#include "BaslerCamera.h"

CBaslerCamera::CBaslerCamera()
{
	m_nCurrentIndex = -1;
	m_bSaveImage = false;
	m_bNeedleImageConverter = false;
	m_bFrameStartAvailable = false;
	m_bAcquisitionStartAvailable = false;
	m_bColor = false;
	m_chTriggerSelectorValue = "FrameStart";
}

CBaslerCamera::~CBaslerCamera()
{

}

void CBaslerCamera::RunPylonAutoInitTerm()
{
	Pylon::PylonAutoInitTerm autoInitTerm;
}

int CBaslerCamera::GetCameraCount()
{
	int nTotalNum = 0;
	Pylon::DeviceInfoList_t deviceInfoList;
	try
	{
		Pylon::CTlFactory& tlFactory = CTlFactory::GetInstance();
		nTotalNum = tlFactory.EnumerateDevices(deviceInfoList);
	}
	catch (GenICam::GenericException& e)
	{
		CString str;
		str.Format(_T("InitAll() error: %s"),e.GetDescription());
		TRACE(str);
	}
	return nTotalNum;
}

DeviceInfoList_t CBaslerCamera::GetCameraInfoList()
{
	Pylon::DeviceInfoList_t deviceInfoList;
	try
	{
		Pylon::CTlFactory& tlFactory = CTlFactory::GetInstance();
		tlFactory.EnumerateDevices(deviceInfoList);
	}
	catch (GenICam::GenericException& e)
	{
		CString str;
		str.Format(_T("InitAll() error: %s"),e.GetDescription());
		TRACE(str);
	}
	return deviceInfoList;
}

bool CBaslerCamera::SetCameraIndex(int nIndexNo)
{
	bool ret = false;
	if(nIndexNo>=0 && nIndexNo<CBaslerCamera::GetCameraCount())
	{
		m_nCurrentIndex = nIndexNo;
		ret = true;
	}
	return ret;
}

CString CBaslerCamera::GetDeviceSN()
{
	CString strCameraSN = "";
	DeviceInfoList_t cameraInfoList = CBaslerCamera::GetCameraInfoList();
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		DeviceInfoList_t::iterator it = cameraInfoList.begin() + m_nCurrentIndex;
		strCameraSN = (*it).GetSerialNumber().c_str();
	}
	return strCameraSN;
}

CString CBaslerCamera::GetDeviceUserID()
{
	CString strCameraUserID = "";
	DeviceInfoList_t cameraInfoList = CBaslerCamera::GetCameraInfoList();
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		DeviceInfoList_t::iterator it = cameraInfoList.begin() + m_nCurrentIndex;
		strCameraUserID = (*it).GetUserDefinedName().c_str();
	}
	return strCameraUserID;
}

bool CBaslerCamera::OpenCamera()
{
	bool ret = false;
	DeviceInfoList_t cameraInfoList = CBaslerCamera::GetCameraInfoList();
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		if(m_InstantCamera.IsPylonDeviceAttached())
		{
			m_InstantCamera.DetachDevice();
			m_InstantCamera.DestroyDevice();
		}
		m_InstantCamera.Attach(CTlFactory::GetInstance().CreateDevice(cameraInfoList[m_nCurrentIndex]));
		m_InstantCamera.Open();

		m_InstantCamera.RegisterConfiguration( this, RegistrationMode_Append, Ownership_ExternalOwnership);
		m_InstantCamera.RegisterImageEventHandler( this, RegistrationMode_Append, Ownership_ExternalOwnership);

		CEnumerationPtr triggerSelector(m_InstantCamera.GetNodeMap().GetNode("TriggerSelector"));
		GenApi::IEnumEntry* frameStart = triggerSelector->GetEntryByName("FrameStart");
		m_bFrameStartAvailable = (frameStart && IsAvailable(frameStart));

		GenApi::IEnumEntry* acquisitionStart = triggerSelector->GetEntryByName("AcquisitionStart");
		m_bAcquisitionStartAvailable = (acquisitionStart && IsAvailable(acquisitionStart));

		ret = true;
	}
	return ret;
}

bool CBaslerCamera::CloseCamera()
{
	bool ret = false;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		if(m_InstantCamera.IsOpen())
		{
			m_InstantCamera.Close();
		}
		ret = true;
	}
	return ret;
}

bool CBaslerCamera::DestroyDevice()
{
	bool ret = false;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		if(m_InstantCamera.IsPylonDeviceAttached())
		{
			m_InstantCamera.DetachDevice();
			m_InstantCamera.DestroyDevice();
		}
		ret = true;
	}
	return ret;
}

bool CBaslerCamera::SetWidth(int32_t nWidth)
{
	bool ret = false;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		CIntegerPtr ptrWidth(m_InstantCamera.GetNodeMap().GetNode("Width"));
		if(IsWritable(ptrWidth))
		{
			int64_t newWidth = Adjust(nWidth, ptrWidth->GetMin(), ptrWidth->GetMax(), ptrWidth->GetInc());
			ptrWidth->SetValue(newWidth);
			ret = true;
		}
	}
	return ret;
}

bool CBaslerCamera::SetHeight(int32_t nHeight)
{
	bool ret = false;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		CIntegerPtr ptrHeight(m_InstantCamera.GetNodeMap().GetNode("Height"));
		if(IsWritable(ptrHeight))
		{
			int64_t newHeight = Adjust(nHeight, ptrHeight->GetMin(), ptrHeight->GetMax(), ptrHeight->GetInc());
			ptrHeight->SetValue(newHeight);
			ret = true;
		}
	}
	return ret;
}

int64_t CBaslerCamera::GetWidth()
{
	int64_t nWidth = 0;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		CIntegerPtr ptrWidth(m_InstantCamera.GetNodeMap().GetNode("Width"));
		if(IsReadable(ptrWidth))
		{
			nWidth = ptrWidth->GetValue();
		}
	}
	return nWidth;
}

int64_t CBaslerCamera::GetHeight()
{
	int64_t nHeight = 0;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		CIntegerPtr ptrHeight(m_InstantCamera.GetNodeMap().GetNode("Height"));
		if(IsReadable(ptrHeight))
		{
			nHeight = ptrHeight->GetValue();
		}
	}
	return nHeight;
}

int64_t CBaslerCamera::GetWidthMax()
{
	int64_t nWidthMax = 0;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		CIntegerPtr ptrWidth(m_InstantCamera.GetNodeMap().GetNode("Width"));
		if(IsReadable(ptrWidth))
		{
			nWidthMax = ptrWidth->GetMax();
		}
	}
	return nWidthMax;
}

int64_t CBaslerCamera::GetHeightMax()
{
	int64_t nHeightMax = 0;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		CIntegerPtr ptrHeight(m_InstantCamera.GetNodeMap().GetNode("Height"));
		if(IsReadable(ptrHeight))
		{
			nHeightMax = ptrHeight->GetMax();
		}
	}
	return nHeightMax;
}

bool CBaslerCamera::SetOffsetX(int32_t nOffsetX)
{
	bool ret = false;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		CIntegerPtr ptrOffsetX(m_InstantCamera.GetNodeMap().GetNode("OffsetX"));
		if(IsWritable(ptrOffsetX))
		{
			int64_t newOffsetX = Adjust(nOffsetX, ptrOffsetX->GetMin(), ptrOffsetX->GetMax(), ptrOffsetX->GetInc());
			ptrOffsetX->SetValue(newOffsetX);
			ret = true;
		}
	}
	return ret;
}

bool CBaslerCamera::SetOffsetY(int32_t nOffsetY)
{
	bool ret = false;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		CIntegerPtr ptrOffsetY(m_InstantCamera.GetNodeMap().GetNode("OffsetY"));
		if(IsWritable(ptrOffsetY))
		{
			int64_t newOffsetY = Adjust(nOffsetY, ptrOffsetY->GetMin(), ptrOffsetY->GetMax(), ptrOffsetY->GetInc());
			ptrOffsetY->SetValue(newOffsetY);
			ret = true;
		}
	}
	return ret;
}

bool CBaslerCamera::SetCenterX(bool bSetValue)
{
	bool ret = false;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		CIntegerPtr ptrCenterX(m_InstantCamera.GetNodeMap().GetNode("CenterX"));
		if(IsWritable(ptrCenterX))
		{
			ptrCenterX->SetValue(bSetValue);
			ret = true;
		}
	}
	return ret;
}

bool CBaslerCamera::SetCenterY(bool bSetValue)
{
	bool ret = false;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		CIntegerPtr ptrCenterY(m_InstantCamera.GetNodeMap().GetNode("CenterY"));
		if(IsWritable(ptrCenterY))
		{
			ptrCenterY->SetValue(bSetValue);
			ret = true;
		}
	}
	return ret;
}

bool CBaslerCamera::SetPixelFormatToMono8()
{
	bool ret = false;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		CEnumerationPtr ptrPixelFormat(m_InstantCamera.GetNodeMap().GetNode("PixelFormat"));

		if(IsAvailable(ptrPixelFormat->GetEntryByName("Mono8")))
		{
			ptrPixelFormat->FromString("Mono8");
			m_ImageConverter.OutputPixelFormat = PixelType_Mono8;
			m_bNeedleImageConverter = false;
			m_bColor = false;
			ret = true;
		}
	}
	return ret;
}

bool CBaslerCamera::SetPixelFormatToBayerBG8()
{
	bool ret = false;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		CEnumerationPtr ptrPixelFormat(m_InstantCamera.GetNodeMap().GetNode("PixelFormat"));

		if(IsAvailable(ptrPixelFormat->GetEntryByName("BayerBG8")))
		{
			ptrPixelFormat->FromString("BayerBG8");
			m_ImageConverter.OutputPixelFormat = PixelType_RGB8packed;
			m_bNeedleImageConverter = true;
			m_bColor = true;
			ret = true;
		}
	}
	return ret;
}

bool CBaslerCamera::GrabImageStart()
{
	bool ret = false;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		if(m_InstantCamera.IsOpen())
		{
			if(!m_InstantCamera.IsGrabbing())
			{
				m_InstantCamera.MaxNumBuffer = 10;
				CEnumerationPtr(m_InstantCamera.GetNodeMap().GetNode("AcquisitionMode"))->FromString("Continuous");
				m_InstantCamera.StartGrabbing(GrabStrategy_UpcomingImage);
			}
			ret = true;
		}
	}
	return ret;
}

bool CBaslerCamera::GrabImageStartContinuousThread()
{
	bool ret = false;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		if(m_InstantCamera.IsOpen())
		{
			if(!m_InstantCamera.IsGrabbing())
			{
				m_InstantCamera.MaxNumBuffer = 10;
				CEnumerationPtr(m_InstantCamera.GetNodeMap().GetNode("AcquisitionMode"))->FromString("Continuous");
				m_InstantCamera.StartGrabbing(GrabStrategy_UpcomingImage,GrabLoop_ProvidedByInstantCamera);
			}
			ret = true;
		}
	}
	return ret;
}

bool CBaslerCamera::GrabOneImage(unsigned int timeoutMs /* = 1000 */)
{
	bool ret = false;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		CGrabResultPtr pGrabResult;
		m_InstantCamera.RetrieveResult(timeoutMs,pGrabResult,TimeoutHandling_ThrowException);
		if(pGrabResult->GrabSucceeded())
		{
			ret = true;
			uint8_t* buffer = (uint8_t *)pGrabResult->GetBuffer();
			Pylon::DisplayImage(1,pGrabResult);
			static int nGrabImagesNum = 0;
			if(m_bSaveImage)
			{
				EPixelType pixelType = pGrabResult->GetPixelType();
				uint32_t width = pGrabResult->GetWidth();
				uint32_t height = pGrabResult->GetHeight();
				size_t paddingX = pGrabResult->GetPaddingX();
				EImageOrientation orientation = ImageOrientation_TopDown;
				size_t bufferSize = pGrabResult->GetImageSize();
				char strName[200];
				sprintf_s(strName, "GrabImagesNum%d-%d.bmp",m_nCurrentIndex,nGrabImagesNum);
				nGrabImagesNum++;

				CImagePersistence::Save(
					ImageFileFormat_Bmp,
					strName,
					buffer,
					bufferSize,
					pixelType,
					width,
					height,
					paddingX,
					orientation);
			}
		}
		else
		{
			CString str;
			str.Format(_T("Grab Failed."));
			TRACE(str);
		}
	}
	return ret;
}

bool CBaslerCamera::GrabOneImage(uint8_t* pImageBuffer, unsigned int timeoutMs /* = 1000 */)
{
	bool ret = false;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		CGrabResultPtr pGrabResult;
		m_InstantCamera.RetrieveResult(timeoutMs,pGrabResult,TimeoutHandling_ThrowException);
		if(pGrabResult->GrabSucceeded())
		{
			ret = true;
			if(m_ImageConverter.ImageHasDestinationFormat(pGrabResult))
			{
				uint8_t* buffer = (uint8_t *)pGrabResult->GetBuffer();
				memcpy(pImageBuffer,buffer,pGrabResult->GetPayloadSize());
			}
			else
			{
				m_ImageConverter.Convert(m_targetImage,pGrabResult);
				memcpy(pImageBuffer,m_targetImage.GetBuffer(),m_targetImage.GetAllocatedBufferSize());
			}
		}
		else
		{
			CString str;
			str.Format(_T("Grab Failed."));
			TRACE(str);
		}
	}
	return ret;
}

bool CBaslerCamera::GrabImageStop()
{
	bool ret = false;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		if(m_InstantCamera.IsOpen())
		{
			if(m_InstantCamera.IsGrabbing())
			{
				m_InstantCamera.StopGrabbing();
			}
		}
		ret = true;
	}
	return ret;
}

bool CBaslerCamera::SetHeartbeatTimeout(int64_t time_ms)
{
	bool ret = false;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		GenApi::CIntegerPtr m_pHeartbeatTimeout = NULL;
		m_pHeartbeatTimeout = m_InstantCamera.GetTLNodeMap().GetNode("HeartbeatTimeout");
		if(m_pHeartbeatTimeout.IsValid())
		{
			int64_t correctedValue = time_ms - (time_ms % m_pHeartbeatTimeout->GetInc());
			m_pHeartbeatTimeout->SetValue(correctedValue);
		}
		ret = true;
	}
	return ret;
}

int64_t CBaslerCamera::Adjust(int64_t val, int64_t min,int64_t max,int64_t inc)
{
	if(inc <= 0)
	{
		throw LOGICAL_ERROR_EXCEPTION("Unexpected increment %d",inc);
	}
	if(min > max)
	{
		throw LOGICAL_ERROR_EXCEPTION("minimum bigger than maximum.");
	}
	if(val < min)
	{
		return min;
	}
	if(val > max)
	{
		return max;
	}
	if(inc == 1)
	{
		return val;
	}
	return min + ( ((val - min) / inc) * inc);
}

bool CBaslerCamera::SetExposureTime(int64_t nValue)
{
	bool ret = false;
	try
	{
		if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
		{
			INodeMap& nodeMap = m_InstantCamera.GetNodeMap();
			CEnumerationPtr ptrExposureMode(nodeMap.GetNode("ExposureMode"));
			if(IsWritable(ptrExposureMode))
			{
				ptrExposureMode->FromString("Timed");
			}

			CIntegerPtr ptrExposureTimeRaw(nodeMap.GetNode("ExposureTimeRaw"));
			if(ptrExposureTimeRaw.IsValid())
			{
				int64_t newExposureTimeRaw = Adjust(nValue, ptrExposureTimeRaw->GetMin(), ptrExposureTimeRaw->GetMax(), ptrExposureTimeRaw->GetInc());
				ptrExposureTimeRaw->SetValue(newExposureTimeRaw);
				ret = true;
			}
		}
	}
	catch(GenICam::GenericException &e)
	{
		CString str;
		str.Format(_T("SetExposureTimeRaw() error: %s"),e.GetDescription());
		TRACE(str);
	}
	return ret;
}

bool CBaslerCamera::SetGain(int64_t nValue)
{
	bool ret = false;
	try
	{
		if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
		{
			INodeMap& nodeMap = m_InstantCamera.GetNodeMap();
			CEnumerationPtr ptrGainAuto(nodeMap.GetNode("GainAuto"));
			if(IsWritable(ptrGainAuto))
			{
				ptrGainAuto->FromString("Off");
			}

			CIntegerPtr ptrGainRaw(nodeMap.GetNode("GainRaw"));
			if(ptrGainRaw.IsValid())
			{
				int64_t newGainRaw = Adjust(nValue, ptrGainRaw->GetMin(), ptrGainRaw->GetMax(), ptrGainRaw->GetInc());
				ptrGainRaw->SetValue(newGainRaw);
				ret = true;
			}
		}
	}
	catch(GenICam::GenericException &e)
	{
		CString str;
		str.Format(_T("SetGainRaw() error: %s"),e.GetDescription());
		TRACE(str);
	}
	return ret;
}

bool CBaslerCamera::SetToFreeRunMode()
{
	bool ret = false;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		CEnumerationPtr triggerSelector(m_InstantCamera.GetNodeMap().GetNode("TriggerSelector"));
		CEnumerationPtr triggerMode(m_InstantCamera.GetNodeMap().GetNode("TriggerMode"));

		if(m_bAcquisitionStartAvailable)
		{
			triggerSelector->FromString("AcquisitionStart");
			triggerMode->FromString("Off");
		}

		if(m_bFrameStartAvailable)
		{
			triggerSelector->FromString("FrameStart");
			triggerMode->FromString("Off");
		}
		ret = true;
	}
	return ret;
}

bool CBaslerCamera::SetToSoftwareTrigerMode()
{
	bool ret = false;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		CEnumerationPtr triggerSelector(m_InstantCamera.GetNodeMap().GetNode("TriggerSelector"));
		CEnumerationPtr triggerMode(m_InstantCamera.GetNodeMap().GetNode("TriggerMode"));
		if(m_bAcquisitionStartAvailable && !m_bFrameStartAvailable)
		{
			triggerSelector->FromString("AcquisitionStart");
			triggerMode->FromString("On");
			m_chTriggerSelectorValue = "AcquisitionStart";
		}
		else
		{
			if(m_bAcquisitionStartAvailable)
			{
				triggerSelector->FromString("AcquisitionStart");
				triggerMode->FromString("Off");
			}
			assert(m_bFrameStartAvailable);
			triggerSelector->FromString("FrameStart");
			triggerMode->FromString("On");
		}
		triggerSelector->FromString(m_chTriggerSelectorValue);
		CEnumerationPtr(m_InstantCamera.GetNodeMap().GetNode("TriggerSource"))->FromString("Software");
		ret = true;
	}
	return ret;
}

bool CBaslerCamera::SendSoftwareTrigerCommand()
{
	bool ret = false;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		if(m_InstantCamera.WaitForFrameTriggerReady(500,TimeoutHandling_ThrowException))
		{
			m_InstantCamera.ExecuteSoftwareTrigger();
		}
		ret = true;
	}
	return ret;
}

bool CBaslerCamera::SetToHardwareTrigerMode()
{
	bool ret = false;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		INodeMap& control = m_InstantCamera.GetNodeMap();
		CEnumerationPtr triggerSelector(control.GetNode("TriggerSelector"));
		CEnumerationPtr triggerMode(control.GetNode("TriggerNode"));
		if(m_bAcquisitionStartAvailable && !m_bFrameStartAvailable)
		{
			triggerSelector->FromString("AcquisitionStart");
			triggerMode->FromString("On");
			m_chTriggerSelectorValue = "AcquisitionStart";
		}
		else
		{
			if(m_bAcquisitionStartAvailable)
			{
				triggerSelector->FromString("AcquisitionStart");
				triggerMode->FromString("Off");
			}
			assert(m_bFrameStartAvailable);
			triggerSelector->FromString("FrameStart");
			triggerMode->FromString("On");
		}
		triggerSelector->FromString(m_chTriggerSelectorValue);
		CEnumerationPtr(control.GetNode("TriggerSource"))->FromString("Line1");
		CEnumerationPtr(control.GetNode("TriggerActivation"))->FromString("RisingEdge");
		CFloatPtr(control.GetNode("TriggerDelayAbs"))->SetValue(100);
		CEnumerationPtr(control.GetNode("LineSelector"))->FromString("Line1");
		CFloatPtr(control.GetNode("LineDebouncerTimeAbs"))->SetValue(20);
		ret = true;
	}
	return ret;
}

bool CBaslerCamera::SetWhiteBalanceOnce()
{
	bool ret = false;
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		INodeMap& control = m_InstantCamera.GetNodeMap();
		CEnumerationPtr ptrWhiteBalanceAuto(control.GetNode("BalanceWhiteAuto"));
		if(m_bColor && IsAvailable(ptrWhiteBalanceAuto->GetEntryByName("Once")))
		{
			ptrWhiteBalanceAuto->FromString("Once");
			do 
			{
			} while (ptrWhiteBalanceAuto->ToString() == "Once");
		}
		ret = true;
	}
	return ret;
}

void CBaslerCamera::OnImageGrabbed(CInstantCamera& camera, const CGrabResultPtr& ptrGrabResult)
{
	if(m_nCurrentIndex>=0 && m_nCurrentIndex<CBaslerCamera::GetCameraCount())
	{
		m_ptrLastGrabbedImage = ptrGrabResult;
		Pylon::DisplayImage(m_nCurrentIndex,ptrGrabResult);
	}
}

void CBaslerCamera::OnCameraRemoved(CInstantCamera& camera)
{
	CString str;
	str.Format("camera removed, SN = %s",camera.GetDeviceInfo().GetSerialNumber().c_str());
	TRACE(str);
}

bool CBaslerCamera::InitCamera(int nIndex,int32_t nWidth,int32_t nHeight,bool bColor)
{
	bool ret = false;
	ret = SetCameraIndex(nIndex);
	if(!ret)
	{
		return ret;
	}
	ret = OpenCamera();
	if(!ret)
	{
		return ret;
	}
	ret = SetHeartbeatTimeout(6000);
	if(!ret)
	{
		return ret;
	}
	ret = SetOffsetX(0);
	if(!ret)
	{
		return ret;
	}
	ret = SetOffsetY(0);
	if(!ret)
	{
		return ret;
	}
	if(nWidth>GetWidthMax()||nHeight>GetHeightMax())
	{
		return false;
	}
	ret = SetWidth(nWidth);
	if(!ret)
	{
		return ret;
	}
	ret = SetHeight(nHeight);
	if(!ret)
	{
		return ret;
	}

	if(m_bColor)
	{
		ret = SetPixelFormatToMono8();
	}
	else
	{
		ret = SetPixelFormatToBayerBG8();
	}
	return ret;
}