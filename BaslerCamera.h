#pragma once
#include <pylon/PylonIncludes.h>
#include <pylon/PylonGUI.h>
#include "CameraConfiguration.h"

using namespace Pylon;
using namespace GenApi;

class CBaslerCamera: public CImageEventHandler,
	public CConfigurationEventHandler
{
public:
	CBaslerCamera(void);
	~CBaslerCamera(void);
public:
	// static函数
	static void RunPylonAutoInitTerm();
	static int GetCameraCount();
	static DeviceInfoList_t GetCameraInfoList();

	bool InitCamera(int nIndex,int32_t nWidth,int32_t nHeight,bool bColor);
	// 根据自定义的相机UserID，获得枚举相机列表中的相机序号
	//int GetDeviceIndexFromUserID(CString UserID);
	// 根据相机序号，初始化相机
	bool SetCameraIndex(int nIndex);

	CString GetDeviceSN();
	CString GetDeviceUserID();

	bool OpenCamera();
	bool CloseCamera();
	bool DestroyDevice();

	bool SetWidth(int32_t nWidth);
	bool SetHeight(int32_t nHeight);

	int64_t GetWidth();
	int64_t GetHeight();
	int64_t GetWidthMax();
	int64_t GetHeightMax();

	bool SetOffsetX(int32_t nOffsetX);
	bool SetOffsetY(int32_t nOffsetY);
	bool SetCenterX(bool bSetValue);
	bool SetCenterY(bool bSetValue);

	bool SetPixelFormatToMono8();
	bool SetPixelFormatToBayerBG8();

	bool GrabImageStart();
	bool GrabImageStartContinuousThread();
	bool GrabOneImage(unsigned int timeoutMs = 1000);
	bool GrabOneImage(uint8_t* pImageBuffer, unsigned int timeoutMs = 1000);
	bool GrabImageStop();

	bool SetHeartbeatTimeout(int64_t time_ms);

	bool SetGain(int64_t nValue);
	bool SetExposureTime(int64_t nValue);

	bool SetToFreeRunMode();
	bool SetToSoftwareTrigerMode();
	bool SendSoftwareTrigerCommand();
	bool SetToHardwareTrigerMode();

	bool SetWhiteBalanceOnce();

	virtual void OnImageGrabbed(CInstantCamera& camera, const CGrabResultPtr& ptrGrabResult);
	virtual void OnCameraRemoved(CInstantCamera& camera);
private:
	int64_t Adjust(int64_t val, int64_t min,int64_t max,int64_t inc);
	int m_nCurrentIndex;
	CInstantCamera m_InstantCamera;
	bool m_bSaveImage;
	bool m_bNeedleImageConverter;
	CPylonImage m_targetImage;
	CImageFormatConverter m_ImageConverter;
	bool m_bFrameStartAvailable;
	bool m_bAcquisitionStartAvailable;
	char* m_chTriggerSelectorValue;
	bool m_bColor;
public:
	CGrabResultPtr m_ptrLastGrabbedImage;
};
