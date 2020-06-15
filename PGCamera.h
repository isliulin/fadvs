#pragma once
#include "PointGrey/include/FlyCapture2.h"
#include "PointGrey/include/FlyCapture2Defs.h"
#include <memory>
using namespace FlyCapture2;

#define WIDTH 808
#define HEIGHT 608

// 相机接口类型
typedef enum tagPGCameraType 
{
	eInterfaceUnknow = 0,		    // 未知接口
	eIEEE1394 = 1,                  // 1394接口
	eGigE,						    // GigE接口
	eUSB2,						    // USB2.0接口
	eUSB3						    // USB3.0接口
}PGCameraType;

// 采集图像模式
typedef enum tagGrabImageMode
{
	eWaitEvent = 0,                 // 内部事件触发
	eGrabThread					    // 线程方式	
}PGGrabImageMode;

// 相机采集线程工作模式
typedef enum tagPGGrabThreadStatus
{
	ePGIdle = -1,					// 空闲
	ePGSnap = 0,					// 单帧
	ePGGrab							// 连续
} PGGrabThreadStatus;

// 像素格式
typedef enum tagPGPixelFormat
{
	ePGUnknown	= -1,				// 未知
	ePGGrey8	= 0,				// 8位灰度图像
	ePGRGB24	= 1,				// 24位彩色图像
	ePGRGBA32	= 2					// 32位彩色图像
} PGPixelFormat;

// 相机触发模式
typedef enum tagPGTriggerMode
{
	ePGNotTrigger		= -1,		// 非触发模式
	ePGSoftwareTrigger	= 0,		// 软件触发模式 
	ePGHardwareTrigger	= 1			// 硬件触发模式	
} PGTriggerMode;

//相机颜色类型
typedef enum tagPGCameraColor
{
	ePGColor = -1,					// 彩色
	ePGMono = 0					// 黑白

} PGCameraColor;

// 相机信息
typedef struct tagPGCameraInfo
{
	unsigned int m_uiIndex;         // 序号
	unsigned int m_uiSerialNum;     // 序列号
	PGCameraType m_CameraType;      // 相机类型
	IPAddress	 m_ipAddress;	    // IP地址
	IPAddress    m_subnetMask;      // 子网掩码
	IPAddress    m_defaultGateway;  // 默认网关
	char		 m_cModelName[512];	// 相机型号
	char		 m_cResolution[512];// 相机分辨率
	tagPGCameraInfo()
	{
		m_uiIndex = 0;
		m_uiSerialNum = 0;
		memset(m_cModelName, 0, 512);
		memset(m_cResolution, 0, 512);
	}
}PGCameraInfo, *pPGCameraInfo ;

// Format7格式
typedef struct tagPGFormat7Info
{
	Mode         m_mode;			// 模式
	unsigned int m_uiStartX;        // 起始X
	unsigned int m_uiStartY;		// 起始Y
	unsigned int m_uiWidth;         // 宽度
	unsigned int m_uiHeight;		// 高度
	float        m_fBusSpeed;       // 数据率
	FlyCapture2::PixelFormat  m_pixelFormat;     // 像素格式

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

// 相机采集是否成功的状态标志
typedef enum tagPGStatus
{
	ePGGrabOK		= 0,			// 采集成功
	ePGGrabFailed	= 1,			// 采集失败
	ePGGrabTimeout	= 2,			// 采集超时
} PGStatus;

// 回调函数类型声明，当相机采集完成后调用该回调函数；
// 参数说明：
//		pOwner:		主调用对象的指针，与SetOwner的输入参数pOwner一致；
//		status:		采集状态枚举：成功、出错或超时
//		pImage:		图像数据指针
//		lWidth:		图像的宽
//		lHeight:	图像的高
//		pixelFormat:图像的像素格式
// 注：
//		如果采集图像成功，则pImage、lWidth、lHeight和pixelFormat为图像相关的有效数据；
//		如果采集图像出错，则pImage、lWidth、lHeight和pixelFormat为无效值，一般为0；
//		如果采集图像超时，则pImage、lWidth、lHeight和pixelFormat为无效值，一般为0；

typedef void (WINAPI *PGGrabbedCallback)(void* pOwner, PGStatus status, unsigned char* pImage 					
										 ,long lWidth, long lHeight, PGPixelFormat pixelFormat);

class CPGCamera
{
public:
	// 网络相机采集基类
	GigECamera *m_pCamera;
	// 相机种类，1394或GigE
	PGCameraType m_cameraType;
	// 相机是否连接
	bool m_bOnline;
	// 采集图像方式
	PGGrabImageMode m_grabImageMode;
	// 是否正在连续采集图像
	bool m_bIsGrabbing;
	// 是否正在单帧采集图像
	bool m_bIsSnap;
	// 采集线程所处的工作模式
	PGGrabThreadStatus m_threadStatus;
	HANDLE m_hEventSnapped;
	bool m_bAcquireSuccess;
	// 是否为FreezeStop
	bool m_bFreezeStop;
	// 相机采集原图像
	FlyCapture2::Image m_ImageRaw;
	// 采集到的图像数据
	unsigned char *m_pGrabImage;
	// 图像的宽
	long m_lWidth;
	// 图像的高
	long m_lHeight;
	// 图像的像素格式
	PGPixelFormat m_pixelFormat;
	// 回调函数主窗口指针
	void *m_pMainFrm;
	// 回调函数
	PGGrabbedCallback m_funcCallback;
	// 是否允许回调
	bool m_bSendCallbackEnable;
	// 触发模式
	PGTriggerMode m_triggerMode;
	// 临界区，控制图像数据的读写
	CRITICAL_SECTION m_criticalSection;
	// 相机索引号
	int m_nCameraIndex;

	/*线程方式采集*/
	// 连续采集线程
	CWinThread *m_pWinThread;
	// 采集线程处于暂停时，发出消息开始采集
	HANDLE m_hEventToRun;
	// 子线程通知主线程已结束时间句柄
	HANDLE m_hEventExit;
	// 退出实时显示标志
	bool m_bTerminate;
	// 是否已经退出实时采集显示
	bool m_bHaveStopped;
	bool m_bSendGrabTrigger;
	// 每个像素占用的字节数
	int m_nBytes;

	/*网口相机的设置*/
	// 设置GrabMode
	FC2Config m_fc2Config;
	GigEImageSettingsInfo m_GigFormat7Msg;
	GigEImageSettings m_GigImageSettings;
	EmbeddedImageInfo m_EmbeddedImageInfo;

	/*1394，USB2.0或USB3.0相机的设置*/
	VideoMode m_pVideoMode;
	FrameRate m_pFrameRate;
	Format7Info m_Format7Msg;
	Format7ImageSettings m_ImageSettings;

	/*FrameCounter计数*/
	ImageMetadata m_stImageMetadataCounter;
	unsigned int m_nFrameCounter;
	unsigned int m_nMissCounter;
	unsigned int m_nPreFrameCounter;
	unsigned int m_nAfterFrameCounter;
	bool m_bIsFrameCounter;

	/*TimeStamp计数*/
	unsigned int m_nTimeSFrameCounter;
	unsigned int m_nTimeSMissCounter;
	unsigned int m_nPreTimeSFrameCounter;
	unsigned int m_nAfterTimeSFrameCounter;
	bool m_bIsTimeStampCounter;
	// 时间戳增量
	unsigned int m_nDeltaTimeStamp;
	unsigned int m_nNotStepTimeStamp;
	unsigned int m_nNotStepAll;
	// 时间戳结果
	long m_lSeconds;
	unsigned int m_nMicroSeconds;
	// 1394 Cycle Time Seconds
	unsigned int m_nCycleSeconds;
	// 1394 Cycle Time Count
	unsigned int m_nCycleCount;
	// 1394 Cycle Time Offset
	unsigned int m_nCycleOffset;

	// 获取相机信息
	CameraInfo m_pCameraInfo;
	PGCameraColor m_CameraColor;

	CPGCamera(PGGrabImageMode grabImageMode = eGrabThread);
	~CPGCamera();

	/*设置回调函数*/
	// 设置调用者和回调函数
	void SetOwner(void *pOwner,PGGrabbedCallback funcCallback);
	// 设置是否调用回调函数
	void SetSendCallbackEnable(bool bEnable);
	// 获取是否调用回调函数
	bool GetSendCallbackEnable();

	/*相机初始化*/
	// 根据相机序列号初始化相机
	bool InitFromSerial(unsigned int nSerialNum);
	bool InitFromSerial(unsigned int nSerialNum,unsigned int nWidth,unsigned int nHeight);
	// 根据索引号初始化相机
	bool InitFromIndex(unsigned int nCameraIndex);
	bool InitFromIndex(unsigned int nCameraIndex,unsigned int nWidth,unsigned int nHeight);
	// 根据相机Guid初始化
	bool InitFromGuid(PGRGuid *pGuid);
	bool InitFromGuid(PGRGuid *pGuid,unsigned int nWidth,unsigned int nHeight);
	// 是否连接相机
	bool IsOnline();
	// 断开相机连接
	bool Destroy();

	/*采集操作*/
	bool Grab();
	// 结束连续采集，需要已经创建采集线程
	bool Freeze();
	// 获取是否处于连续采集状态
	bool IsGrabbing();
	// 设置采集延时
	bool SetGrabTimeout(unsigned long lTimeout);
	// 采集单帧图像（只支持FreeRun模式）。需要已经创建采集线程。
	// 如果bWaitSnapped=true，等待直到采集完成才返回；
	// 如果bWaitSnapped=false，调用后不等待采集完直接返回；
	bool Snap(bool bWaitSnapped = false);
	// 图像是否采集成功
	bool IsAcquireSuccess();
	// 相机开始采集图像，不需要创建采集线程
	bool Start();
	// 相机停止采集图像，不需要创建采集线程
	bool Stop();
	// 采集单帧图像。
	// 该采集函数不能与上述其他采集函数同时调用；
	// 该采集函数直到采集完成后退出；
	// 该采集函数不依赖于采集线程；
	bool SnapImage();

	/*触发设置*/
	// 设置相机的触发模式（软触发、硬触发或非触发）。
	// 当设置相机为软触发或硬触发时，需要已经创建采集线程。
	bool SetTriggerMode(PGTriggerMode mode,unsigned long lTimeout = 2000);
	// 获取当前的触发模式
	PGTriggerMode GetTriggerMode();
	// 关闭触发模式
	bool CloseTriggerMode();
	// 是否以触发方式（软触发或硬触发）采集图像
	bool IsTrigger();
	// 发送软触发信号采集单帧图像，利用连续采集获取单帧图像
	// 发送完软触发信号后，该函数立即返回。
	// 采集一帧图像成功后，调用回调函数
	bool SendSoftTrigger();
	// 触发采集一帧图像
	bool TriggerSnap();
	// 查询并等待，直到相机可以进行触发
	bool CheckTriggerReady();
	// 查询相机是否有软触发采集
	bool CheckSoftwareTriggerPresence();

	/*相机图像属性*/
	// 获取图像像素格式
	PGPixelFormat GetImagePixelFormat();
	// 获取图像的宽
	long GetImageWidth();
	// 获取图像的高
	long GetImageHeight();
	// 获取存储图像所需的内存大小
	long GetImageDataSize();
	// 获取图像数据，要求外部已分配好内存，内存大小可由GetImageDataSize()获得；
	bool GetImageData(unsigned char *pImage);
	// 获取灰度图像，如果图像格式为ePGRGB24则返回false；
	//bool GetImage(scImageGrey& image,long alignModulus = 1);
	// 获取RGB图像，如果图像格式为ePGGrey8则返回false；
	//bool GetImage(scImageRGB& image,long alignModulus = 1);

	/*1394相机普通视频模式设置*/
	// 查询1394相机是否支持给定的视频模式
	bool IsVideoModeSupported(VideoMode vmVideoMode);
	// 查询1394相机在当前视频模式下，是否支持给定的帧率
	bool IsFrameRateSupported(FrameRate frFrameRate);
	// 设定1394相机视频模式：只支持Y8和RGB模式；可能需要更改当前帧率
	bool SetCurVideoMode(VideoMode vmVideoMode);
	// 获取1394相机视频模式
	bool GetCurVideoMode(VideoMode *pVideoMode);
	// 设置1394相机帧率，可能需要改变最大最小曝光时间
	bool SetCurFrameRate(FrameRate frFrameRate);
	// 获取1394相机当前的帧率
	bool GetCurFrameRate(FrameRate *pFrameRate);
	// 获取1394相机在当前视频模式下，支持的最大帧率
	bool GetMaxFrameRate(FrameRate *pFrameRate);

	/*Format7视频模式设置*/
	// 获取Format7模式下的参数
	bool GetFormat7Config(PGFormat7Info &format7Info);
	// 设置Format7模式下的参数
	bool SetFormat7Config(PGFormat7Info format7Info);

	/*相机属性*/
	// 帧率设置
	bool SetFrameRate(float fFrameRate);
	bool GetFrameRate(float *pFrameRate);
	bool GetFrameRateRange(float *pMinFrameRate,float *pMaxFrameRate);
	// 曝光时间设置
	bool SetShutter(float fShutter);
	bool GetShutter(float *pShutter);
	bool GetShutterRange(float *pMinShutter,float *pMaxShutter);
	// 增益设置
	bool SetGain(float fGain);
	bool GetGain(float *pGain);
	bool GetGainRange(float *pMinGain,float *pMaxGain);
	bool SetCamRoi(unsigned int XOffset,unsigned int YOffset ,unsigned int ImgWidth,unsigned int ImgHeight);

	/*获取相机信息*/
	// 获取相机的相关信息
	bool GetCameraInfo(CameraInfo *pCameraInfo);
	// 获取相机的序列号
	bool GetCameraSerialNumber(unsigned int *pSerialNumber);
	// 获取相机的索引值
	bool GetCameraIndex(unsigned int *pCameraIndex);

	/*相机参数*/
	// 保存到相机中
	bool SaveToCamera(long lChannel = 1);
	// 设置异步和同步总线速度
	bool SetBusSpeed(BusSpeed asyncBusSpeed,BusSpeed isochBusSpeed);
	// 获取异步和同步总线速度
	bool GetBusSpeed(BusSpeed *pAsyncBusSpeed,BusSpeed *pIsochBusSpeed);
	// 写寄存器
	bool WriteRegisiter(unsigned int nAdd,unsigned int nValue);
	// 读寄存器
	bool ReadRegisiter(unsigned int nAdd,unsigned int *pValue);
	// 获取相机种类
	PGCameraType GetCameraType();
	// 设置brightness、exposure、shutter和gain为手动状态，一般为内部调用
	bool SetStatusManual();

	/*公共函数*/
	// 重新扫描总线，当前相机连接会被中断
	static bool RescanBus();
	// 获取总线上相机的数量
	static bool GetBusCameraCount(unsigned int *pCameraCount);
	// 枚举总线上可用相机及其Guid
	static bool EnumerateBusCameras(PGRGuid *pGuid,unsigned int *pSize);
	// 枚举总线上可用相机及其序列号
	static bool EnumerateBusCameras(unsigned int *pSerialInfo,unsigned int *pSize);
	// 枚举总线上可用相机及其信息
	static bool EnumerateBusCameras(PGCameraInfo *pCameraInfo,unsigned int *pSize);
	// 获取相机的Guid（通过索引）
	static bool GetCameraGuidFromIndex(int nIndex,PGRGuid *pGuid);
	// 获取相机的Guid（通过序列号）
	static bool GetCameraGuidFromSerialNum(unsigned int nSerialNum,PGRGuid *pGuid);
	// 判断相机类型（通过Guid）
	static PGCameraType GetCameraTypeFromGuid(PGRGuid *pGuid);
	// 判断相机类型（通过索引）
	static PGCameraType GetCameraTypeFromIndex(unsigned int nIndex);
	// 判断相机类型（通过序列号）
	static PGCameraType GetCameraTypeFromSerialNum(unsigned int nSerialNum);
	// 获得GigE相机IP地址（通过Guid）
	static bool GetGigECameraIPAddressByGuid(PGRGuid *pGuid,IPAddress &ipAddress,
		IPAddress &subnetMask,IPAddress &defaultGateway);
	// 获得GigE相机IP地址（通过索引）
	static bool GetGigECameraIPAddressByIndex(unsigned int nIndex,IPAddress &ipAddress,
		IPAddress &subnetMask,IPAddress &defaultGateway);
	// 获得GigE相机IP地址（通过序列号）
	static bool GetGigECameraIPAddressBySerialNum(unsigned int nSerialNum,IPAddress &ipAddress,
		IPAddress &subnetMask,IPAddress &defaultGateway);
	// 设置GigE相机IP地址（通过Guid）
	static bool SetGigECameraIPAddressByGuid(PGRGuid *pGuid,IPAddress ipAddress,
		IPAddress subnetAddress,IPAddress defaultAddress);
	// 设置GigE相机IP地址（通过索引）
	static bool SetGigECameraIPAddressByIndex(unsigned int nIndex,IPAddress ipAddress,
		IPAddress subnetAddress,IPAddress defaultGateway);
	// 设置GigE相机IP地址（通过序列号）
	static bool SetGigECameraIPAddressBySerialNum(unsigned int nSerialNum,IPAddress ipAddress,
		IPAddress subnetAddress,IPAddress defaultGateway);
	// 强制设置所有GigE相机IP地址
	static bool AutoForceAllGigECameraIP();

	/*内部调用函数（采集相关）*/
	bool TransferData();
	// 创建采集线程，一个采集对象只能有一个采集线程
	bool CreateGrabThread();
	// 结束采集线程
	bool TerminateGrabThread();
	// 从相机采集一幅图像，并进行相应的转换
	PGStatus AcquireImage();
	// 采集完成调用回调函数
	void SendCallback(PGStatus status);
	// 内部回调函数
	static void PGImageEventCallback(FlyCapture2::Image *pImage,const void *pCallbackData);
	static unsigned int GrabThread(LPVOID lpParam);

	/*内部调用函数（1394视频模式相关）*/
	// 将指定视频模式转为在相机寄存器中相应的索引值
	bool ConvertVideoModeToULongs(VideoMode vmVideoMode,unsigned long *pFormat,
		unsigned long *pMode);
	// 将指定帧率模式转为在相机寄存器中相应的索引值
	bool ConvertFrameRateToULong(FrameRate frFrameRate,unsigned long *pFrameRate);
	// 根据相机的当前视频模式调整当前帧率
	bool ResetCurFrameRateByVideoMode();
	// 根据相机的当前帧率调整曝光时间
	bool ResetCurShutterByFrameRate();
public:
	// 总线管理类
	static std::auto_ptr<BusManager> m_AutoBusManager;
};