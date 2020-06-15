#pragma once
#include "PointGrey/include/FlyCapture2.h"
#include "PointGrey/include/FlyCapture2Defs.h"
#include <memory>
using namespace FlyCapture2;

#define WIDTH 808
#define HEIGHT 608

// ����ӿ�����
typedef enum tagPGCameraType 
{
	eInterfaceUnknow = 0,		    // δ֪�ӿ�
	eIEEE1394 = 1,                  // 1394�ӿ�
	eGigE,						    // GigE�ӿ�
	eUSB2,						    // USB2.0�ӿ�
	eUSB3						    // USB3.0�ӿ�
}PGCameraType;

// �ɼ�ͼ��ģʽ
typedef enum tagGrabImageMode
{
	eWaitEvent = 0,                 // �ڲ��¼�����
	eGrabThread					    // �̷߳�ʽ	
}PGGrabImageMode;

// ����ɼ��̹߳���ģʽ
typedef enum tagPGGrabThreadStatus
{
	ePGIdle = -1,					// ����
	ePGSnap = 0,					// ��֡
	ePGGrab							// ����
} PGGrabThreadStatus;

// ���ظ�ʽ
typedef enum tagPGPixelFormat
{
	ePGUnknown	= -1,				// δ֪
	ePGGrey8	= 0,				// 8λ�Ҷ�ͼ��
	ePGRGB24	= 1,				// 24λ��ɫͼ��
	ePGRGBA32	= 2					// 32λ��ɫͼ��
} PGPixelFormat;

// �������ģʽ
typedef enum tagPGTriggerMode
{
	ePGNotTrigger		= -1,		// �Ǵ���ģʽ
	ePGSoftwareTrigger	= 0,		// �������ģʽ 
	ePGHardwareTrigger	= 1			// Ӳ������ģʽ	
} PGTriggerMode;

//�����ɫ����
typedef enum tagPGCameraColor
{
	ePGColor = -1,					// ��ɫ
	ePGMono = 0					// �ڰ�

} PGCameraColor;

// �����Ϣ
typedef struct tagPGCameraInfo
{
	unsigned int m_uiIndex;         // ���
	unsigned int m_uiSerialNum;     // ���к�
	PGCameraType m_CameraType;      // �������
	IPAddress	 m_ipAddress;	    // IP��ַ
	IPAddress    m_subnetMask;      // ��������
	IPAddress    m_defaultGateway;  // Ĭ������
	char		 m_cModelName[512];	// ����ͺ�
	char		 m_cResolution[512];// ����ֱ���
	tagPGCameraInfo()
	{
		m_uiIndex = 0;
		m_uiSerialNum = 0;
		memset(m_cModelName, 0, 512);
		memset(m_cResolution, 0, 512);
	}
}PGCameraInfo, *pPGCameraInfo ;

// Format7��ʽ
typedef struct tagPGFormat7Info
{
	Mode         m_mode;			// ģʽ
	unsigned int m_uiStartX;        // ��ʼX
	unsigned int m_uiStartY;		// ��ʼY
	unsigned int m_uiWidth;         // ���
	unsigned int m_uiHeight;		// �߶�
	float        m_fBusSpeed;       // ������
	FlyCapture2::PixelFormat  m_pixelFormat;     // ���ظ�ʽ

	tagPGFormat7Info()
	{
		m_mode			= MODE_0;		
		m_uiStartX		= 0;       
		m_uiStartY		= 0;		
		m_uiWidth		= 0;        
		m_uiHeight		= 0;		
		m_fBusSpeed		= 100.0;       
		m_pixelFormat	= PIXEL_FORMAT_MONO8;   
	}
}PGFormat7Info, *pPGFormat7Info ;

// ����ɼ��Ƿ�ɹ���״̬��־
typedef enum tagPGStatus
{
	ePGGrabOK		= 0,			// �ɼ��ɹ�
	ePGGrabFailed	= 1,			// �ɼ�ʧ��
	ePGGrabTimeout	= 2,			// �ɼ���ʱ
} PGStatus;

// �ص���������������������ɼ���ɺ���øûص�������
// ����˵����
//		pOwner:		�����ö����ָ�룬��SetOwner���������pOwnerһ�£�
//		status:		�ɼ�״̬ö�٣��ɹ��������ʱ
//		pImage:		ͼ������ָ��
//		lWidth:		ͼ��Ŀ�
//		lHeight:	ͼ��ĸ�
//		pixelFormat:ͼ������ظ�ʽ
// ע��
//		����ɼ�ͼ��ɹ�����pImage��lWidth��lHeight��pixelFormatΪͼ����ص���Ч���ݣ�
//		����ɼ�ͼ�������pImage��lWidth��lHeight��pixelFormatΪ��Чֵ��һ��Ϊ0��
//		����ɼ�ͼ��ʱ����pImage��lWidth��lHeight��pixelFormatΪ��Чֵ��һ��Ϊ0��

typedef void (WINAPI *PGGrabbedCallback)(void* pOwner, PGStatus status, unsigned char* pImage 					
										 ,long lWidth, long lHeight, PGPixelFormat pixelFormat);

class CPGCamera
{
public:
	// ��������ɼ�����
	GigECamera *m_pCamera;
	// ������࣬1394��GigE
	PGCameraType m_cameraType;
	// ����Ƿ�����
	bool m_bOnline;
	// �ɼ�ͼ��ʽ
	PGGrabImageMode m_grabImageMode;
	// �Ƿ����������ɼ�ͼ��
	bool m_bIsGrabbing;
	// �Ƿ����ڵ�֡�ɼ�ͼ��
	bool m_bIsSnap;
	// �ɼ��߳������Ĺ���ģʽ
	PGGrabThreadStatus m_threadStatus;
	HANDLE m_hEventSnapped;
	bool m_bAcquireSuccess;
	// �Ƿ�ΪFreezeStop
	bool m_bFreezeStop;
	// ����ɼ�ԭͼ��
	FlyCapture2::Image m_ImageRaw;
	// �ɼ�����ͼ������
	unsigned char *m_pGrabImage;
	// ͼ��Ŀ�
	long m_lWidth;
	// ͼ��ĸ�
	long m_lHeight;
	// ͼ������ظ�ʽ
	PGPixelFormat m_pixelFormat;
	// �ص�����������ָ��
	void *m_pMainFrm;
	// �ص�����
	PGGrabbedCallback m_funcCallback;
	// �Ƿ�����ص�
	bool m_bSendCallbackEnable;
	// ����ģʽ
	PGTriggerMode m_triggerMode;
	// �ٽ���������ͼ�����ݵĶ�д
	CRITICAL_SECTION m_criticalSection;
	// ���������
	int m_nCameraIndex;

	/*�̷߳�ʽ�ɼ�*/
	// �����ɼ��߳�
	CWinThread *m_pWinThread;
	// �ɼ��̴߳�����ͣʱ��������Ϣ��ʼ�ɼ�
	HANDLE m_hEventToRun;
	// ���߳�֪ͨ���߳��ѽ���ʱ����
	HANDLE m_hEventExit;
	// �˳�ʵʱ��ʾ��־
	bool m_bTerminate;
	// �Ƿ��Ѿ��˳�ʵʱ�ɼ���ʾ
	bool m_bHaveStopped;
	bool m_bSendGrabTrigger;
	// ÿ������ռ�õ��ֽ���
	int m_nBytes;

	/*�������������*/
	// ����GrabMode
	FC2Config m_fc2Config;
	GigEImageSettingsInfo m_GigFormat7Msg;
	GigEImageSettings m_GigImageSettings;
	EmbeddedImageInfo m_EmbeddedImageInfo;

	/*1394��USB2.0��USB3.0���������*/
	VideoMode m_pVideoMode;
	FrameRate m_pFrameRate;
	Format7Info m_Format7Msg;
	Format7ImageSettings m_ImageSettings;

	/*FrameCounter����*/
	ImageMetadata m_stImageMetadataCounter;
	unsigned int m_nFrameCounter;
	unsigned int m_nMissCounter;
	unsigned int m_nPreFrameCounter;
	unsigned int m_nAfterFrameCounter;
	bool m_bIsFrameCounter;

	/*TimeStamp����*/
	unsigned int m_nTimeSFrameCounter;
	unsigned int m_nTimeSMissCounter;
	unsigned int m_nPreTimeSFrameCounter;
	unsigned int m_nAfterTimeSFrameCounter;
	bool m_bIsTimeStampCounter;
	// ʱ�������
	unsigned int m_nDeltaTimeStamp;
	unsigned int m_nNotStepTimeStamp;
	unsigned int m_nNotStepAll;
	// ʱ������
	long m_lSeconds;
	unsigned int m_nMicroSeconds;
	// 1394 Cycle Time Seconds
	unsigned int m_nCycleSeconds;
	// 1394 Cycle Time Count
	unsigned int m_nCycleCount;
	// 1394 Cycle Time Offset
	unsigned int m_nCycleOffset;

	// ��ȡ�����Ϣ
	CameraInfo m_pCameraInfo;
	PGCameraColor m_CameraColor;

	CPGCamera(PGGrabImageMode grabImageMode = eGrabThread);
	~CPGCamera();

	/*���ûص�����*/
	// ���õ����ߺͻص�����
	void SetOwner(void *pOwner,PGGrabbedCallback funcCallback);
	// �����Ƿ���ûص�����
	void SetSendCallbackEnable(bool bEnable);
	// ��ȡ�Ƿ���ûص�����
	bool GetSendCallbackEnable();

	/*�����ʼ��*/
	// ����������кų�ʼ�����
	bool InitFromSerial(unsigned int nSerialNum);
	bool InitFromSerial(unsigned int nSerialNum,unsigned int nWidth,unsigned int nHeight);
	// ���������ų�ʼ�����
	bool InitFromIndex(unsigned int nCameraIndex);
	bool InitFromIndex(unsigned int nCameraIndex,unsigned int nWidth,unsigned int nHeight);
	// �������Guid��ʼ��
	bool InitFromGuid(PGRGuid *pGuid);
	bool InitFromGuid(PGRGuid *pGuid,unsigned int nWidth,unsigned int nHeight);
	// �Ƿ��������
	bool IsOnline();
	// �Ͽ��������
	bool Destroy();

	/*�ɼ�����*/
	bool Grab();
	// ���������ɼ�����Ҫ�Ѿ������ɼ��߳�
	bool Freeze();
	// ��ȡ�Ƿ��������ɼ�״̬
	bool IsGrabbing();
	// ���òɼ���ʱ
	bool SetGrabTimeout(unsigned long lTimeout);
	// �ɼ���֡ͼ��ֻ֧��FreeRunģʽ������Ҫ�Ѿ������ɼ��̡߳�
	// ���bWaitSnapped=true���ȴ�ֱ���ɼ���ɲŷ��أ�
	// ���bWaitSnapped=false�����ú󲻵ȴ��ɼ���ֱ�ӷ��أ�
	bool Snap(bool bWaitSnapped = false);
	// ͼ���Ƿ�ɼ��ɹ�
	bool IsAcquireSuccess();
	// �����ʼ�ɼ�ͼ�񣬲���Ҫ�����ɼ��߳�
	bool Start();
	// ���ֹͣ�ɼ�ͼ�񣬲���Ҫ�����ɼ��߳�
	bool Stop();
	// �ɼ���֡ͼ��
	// �òɼ��������������������ɼ�����ͬʱ���ã�
	// �òɼ�����ֱ���ɼ���ɺ��˳���
	// �òɼ������������ڲɼ��̣߳�
	bool SnapImage();

	/*��������*/
	// ��������Ĵ���ģʽ��������Ӳ������Ǵ�������
	// ���������Ϊ������Ӳ����ʱ����Ҫ�Ѿ������ɼ��̡߳�
	bool SetTriggerMode(PGTriggerMode mode,unsigned long lTimeout = 2000);
	// ��ȡ��ǰ�Ĵ���ģʽ
	PGTriggerMode GetTriggerMode();
	// �رմ���ģʽ
	bool CloseTriggerMode();
	// �Ƿ��Դ�����ʽ��������Ӳ�������ɼ�ͼ��
	bool IsTrigger();
	// ���������źŲɼ���֡ͼ�����������ɼ���ȡ��֡ͼ��
	// �����������źź󣬸ú����������ء�
	// �ɼ�һ֡ͼ��ɹ��󣬵��ûص�����
	bool SendSoftTrigger();
	// �����ɼ�һ֡ͼ��
	bool TriggerSnap();
	// ��ѯ���ȴ���ֱ��������Խ��д���
	bool CheckTriggerReady();
	// ��ѯ����Ƿ��������ɼ�
	bool CheckSoftwareTriggerPresence();

	/*���ͼ������*/
	// ��ȡͼ�����ظ�ʽ
	PGPixelFormat GetImagePixelFormat();
	// ��ȡͼ��Ŀ�
	long GetImageWidth();
	// ��ȡͼ��ĸ�
	long GetImageHeight();
	// ��ȡ�洢ͼ��������ڴ��С
	long GetImageDataSize();
	// ��ȡͼ�����ݣ�Ҫ���ⲿ�ѷ�����ڴ棬�ڴ��С����GetImageDataSize()��ã�
	bool GetImageData(unsigned char *pImage);
	// ��ȡ�Ҷ�ͼ�����ͼ���ʽΪePGRGB24�򷵻�false��
	//bool GetImage(scImageGrey& image,long alignModulus = 1);
	// ��ȡRGBͼ�����ͼ���ʽΪePGGrey8�򷵻�false��
	//bool GetImage(scImageRGB& image,long alignModulus = 1);

	/*1394�����ͨ��Ƶģʽ����*/
	// ��ѯ1394����Ƿ�֧�ָ�������Ƶģʽ
	bool IsVideoModeSupported(VideoMode vmVideoMode);
	// ��ѯ1394����ڵ�ǰ��Ƶģʽ�£��Ƿ�֧�ָ�����֡��
	bool IsFrameRateSupported(FrameRate frFrameRate);
	// �趨1394�����Ƶģʽ��ֻ֧��Y8��RGBģʽ��������Ҫ���ĵ�ǰ֡��
	bool SetCurVideoMode(VideoMode vmVideoMode);
	// ��ȡ1394�����Ƶģʽ
	bool GetCurVideoMode(VideoMode *pVideoMode);
	// ����1394���֡�ʣ�������Ҫ�ı������С�ع�ʱ��
	bool SetCurFrameRate(FrameRate frFrameRate);
	// ��ȡ1394�����ǰ��֡��
	bool GetCurFrameRate(FrameRate *pFrameRate);
	// ��ȡ1394����ڵ�ǰ��Ƶģʽ�£�֧�ֵ����֡��
	bool GetMaxFrameRate(FrameRate *pFrameRate);

	/*Format7��Ƶģʽ����*/
	// ��ȡFormat7ģʽ�µĲ���
	bool GetFormat7Config(PGFormat7Info &format7Info);
	// ����Format7ģʽ�µĲ���
	bool SetFormat7Config(PGFormat7Info format7Info);

	/*�������*/
	// ֡������
	bool SetFrameRate(float fFrameRate);
	bool GetFrameRate(float *pFrameRate);
	bool GetFrameRateRange(float *pMinFrameRate,float *pMaxFrameRate);
	// �ع�ʱ������
	bool SetShutter(float fShutter);
	bool GetShutter(float *pShutter);
	bool GetShutterRange(float *pMinShutter,float *pMaxShutter);
	// ��������
	bool SetGain(float fGain);
	bool GetGain(float *pGain);
	bool GetGainRange(float *pMinGain,float *pMaxGain);
	bool SetCamRoi(unsigned int XOffset,unsigned int YOffset ,unsigned int ImgWidth,unsigned int ImgHeight);

	/*��ȡ�����Ϣ*/
	// ��ȡ����������Ϣ
	bool GetCameraInfo(CameraInfo *pCameraInfo);
	// ��ȡ��������к�
	bool GetCameraSerialNumber(unsigned int *pSerialNumber);
	// ��ȡ���������ֵ
	bool GetCameraIndex(unsigned int *pCameraIndex);

	/*�������*/
	// ���浽�����
	bool SaveToCamera(long lChannel = 1);
	// �����첽��ͬ�������ٶ�
	bool SetBusSpeed(BusSpeed asyncBusSpeed,BusSpeed isochBusSpeed);
	// ��ȡ�첽��ͬ�������ٶ�
	bool GetBusSpeed(BusSpeed *pAsyncBusSpeed,BusSpeed *pIsochBusSpeed);
	// д�Ĵ���
	bool WriteRegisiter(unsigned int nAdd,unsigned int nValue);
	// ���Ĵ���
	bool ReadRegisiter(unsigned int nAdd,unsigned int *pValue);
	// ��ȡ�������
	PGCameraType GetCameraType();
	// ����brightness��exposure��shutter��gainΪ�ֶ�״̬��һ��Ϊ�ڲ�����
	bool SetStatusManual();

	/*��������*/
	// ����ɨ�����ߣ���ǰ������ӻᱻ�ж�
	static bool RescanBus();
	// ��ȡ���������������
	static bool GetBusCameraCount(unsigned int *pCameraCount);
	// ö�������Ͽ����������Guid
	static bool EnumerateBusCameras(PGRGuid *pGuid,unsigned int *pSize);
	// ö�������Ͽ�������������к�
	static bool EnumerateBusCameras(unsigned int *pSerialInfo,unsigned int *pSize);
	// ö�������Ͽ������������Ϣ
	static bool EnumerateBusCameras(PGCameraInfo *pCameraInfo,unsigned int *pSize);
	// ��ȡ�����Guid��ͨ��������
	static bool GetCameraGuidFromIndex(int nIndex,PGRGuid *pGuid);
	// ��ȡ�����Guid��ͨ�����кţ�
	static bool GetCameraGuidFromSerialNum(unsigned int nSerialNum,PGRGuid *pGuid);
	// �ж�������ͣ�ͨ��Guid��
	static PGCameraType GetCameraTypeFromGuid(PGRGuid *pGuid);
	// �ж�������ͣ�ͨ��������
	static PGCameraType GetCameraTypeFromIndex(unsigned int nIndex);
	// �ж�������ͣ�ͨ�����кţ�
	static PGCameraType GetCameraTypeFromSerialNum(unsigned int nSerialNum);
	// ���GigE���IP��ַ��ͨ��Guid��
	static bool GetGigECameraIPAddressByGuid(PGRGuid *pGuid,IPAddress &ipAddress,
		IPAddress &subnetMask,IPAddress &defaultGateway);
	// ���GigE���IP��ַ��ͨ��������
	static bool GetGigECameraIPAddressByIndex(unsigned int nIndex,IPAddress &ipAddress,
		IPAddress &subnetMask,IPAddress &defaultGateway);
	// ���GigE���IP��ַ��ͨ�����кţ�
	static bool GetGigECameraIPAddressBySerialNum(unsigned int nSerialNum,IPAddress &ipAddress,
		IPAddress &subnetMask,IPAddress &defaultGateway);
	// ����GigE���IP��ַ��ͨ��Guid��
	static bool SetGigECameraIPAddressByGuid(PGRGuid *pGuid,IPAddress ipAddress,
		IPAddress subnetAddress,IPAddress defaultAddress);
	// ����GigE���IP��ַ��ͨ��������
	static bool SetGigECameraIPAddressByIndex(unsigned int nIndex,IPAddress ipAddress,
		IPAddress subnetAddress,IPAddress defaultGateway);
	// ����GigE���IP��ַ��ͨ�����кţ�
	static bool SetGigECameraIPAddressBySerialNum(unsigned int nSerialNum,IPAddress ipAddress,
		IPAddress subnetAddress,IPAddress defaultGateway);
	// ǿ����������GigE���IP��ַ
	static bool AutoForceAllGigECameraIP();

	/*�ڲ����ú������ɼ���أ�*/
	bool TransferData();
	// �����ɼ��̣߳�һ���ɼ�����ֻ����һ���ɼ��߳�
	bool CreateGrabThread();
	// �����ɼ��߳�
	bool TerminateGrabThread();
	// ������ɼ�һ��ͼ�񣬲�������Ӧ��ת��
	PGStatus AcquireImage();
	// �ɼ���ɵ��ûص�����
	void SendCallback(PGStatus status);
	// �ڲ��ص�����
	static void PGImageEventCallback(FlyCapture2::Image *pImage,const void *pCallbackData);
	static unsigned int GrabThread(LPVOID lpParam);

	/*�ڲ����ú�����1394��Ƶģʽ��أ�*/
	// ��ָ����ƵģʽתΪ������Ĵ�������Ӧ������ֵ
	bool ConvertVideoModeToULongs(VideoMode vmVideoMode,unsigned long *pFormat,
		unsigned long *pMode);
	// ��ָ��֡��ģʽתΪ������Ĵ�������Ӧ������ֵ
	bool ConvertFrameRateToULong(FrameRate frFrameRate,unsigned long *pFrameRate);
	// ��������ĵ�ǰ��Ƶģʽ������ǰ֡��
	bool ResetCurFrameRateByVideoMode();
	// ��������ĵ�ǰ֡�ʵ����ع�ʱ��
	bool ResetCurShutterByFrameRate();
public:
	// ���߹�����
	static std::auto_ptr<BusManager> m_AutoBusManager;
};