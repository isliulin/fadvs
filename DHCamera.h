#ifndef DHA513MG75_H
#define DHA513MG75_H

#include "GenICam/System.h"
#include "GenICam/Camera.h"
#include "GenICam/GigE/GigEInterface.h"
#include "GenICam/GigE/GigECamera.h"
#include "GenICam/EventSubscribe.h"
#include "Infra/Vector.h"
#include "Infra/Thread.h"
#include "Infra/Mutex.h"
#include "Infra/Guard.h"
#include "vector"


using namespace Dahua;


//����ģʽ-����
#define TRIGMODE_ON_STR			 "On"

//����ģʽ-�رգ�������ģʽ
#define TRIGMODE_OFF_STR		 "Off"

//�Զ��ع�-�رգ���ʱ��ʹ���Զ����ع�ʱ��
#define EXPOSUREAUTO_OFF_STR     "Off"

//�Զ�����-�رգ���ʱ��ʹ���Զ�������
#define GAINAUTO_OFF_STR		 "Off"

//�ⴥ���½�����Ч
#define TRIGACT_FALLINGEDGE_STR  "FallingEdge"

//�ⴥ����������Ч
#define TRIGACT_RISINGEDGE_STR   "RisingEdge"

//����ԴΪ����
#define TRIGSRC_SOFT_STR		 "Software"

//����ԴΪLINE����
#define TRIGSRC_LINE1_STR		 "Line1"

typedef enum enumWORKMODE
{
	UNKNOW_MODE = -1,
	CONMODE		= 0,	//����ģʽ
	TRIGMODE	= 1		//����ģʽ
}WORKMODE;

// ����Դ���������ⴥ������������ģʽ����Ч
typedef enum enumTRIGSRC
{
	UNKNOW_SRC		= -1,
	TRIG_SOFT		= 0,	//����Դ������
	TRIG_LINE1		= 1		//����Դ��LINE1����
}TRIGSOURCE;

// �����ʼ�����ò���
typedef struct tagCamPara
{
	int DeviceID;		//�豸���
	int Width;			//ͼ����
	int Height;			//ͼ��߶�
	int Brightness;		//����
	int Shutter;		//�ع�ʱ��
	int Contrast;		//�Աȶ�
	int Gain;			//����
	int Hue;			//ɫ��
	int Sharpness;		//���
	int Saturation;		//���Ͷ�
	int OffsetX;		//ͼ��Xƫ��
	int	OffsetY;		//ͼ��Yƫ��

	WORKMODE	WorkMode;		//�������ͣ�0Ϊ����ģʽ��1Ϊ����ģʽ
	TRIGSOURCE  TrigSource;		//����Դ������ģʽʱ��Ч��


	// struct tagCamParam���캯��
	tagCamPara(int devID)
	{
		DeviceID = devID;		//�豸���
		WorkMode = UNKNOW_MODE;	//��������,0Ϊ����ģʽ��1Ϊ����ģʽ
		TrigSource = UNKNOW_SRC;//�����������ڴ���ģʽ��ʱ�����ò���Ч��
		Brightness = -1;		//����
		Shutter = -1;			//�ع�ʱ��
		Contrast = -1;			//�Աȶ�
		Gain = -1;				//����
		Hue = -1;				//ɫ��
		Sharpness = -1;			//���
		Saturation = -1;		//���Ͷ�
		Width = -1;				//ͼ����
		Height = -1;			//ͼ��߶�
		OffsetX = -1;			//ͼ��Xƫ��
		OffsetY = -1;			//ͼ��Yƫ��		
	}

	tagCamPara()
	{
		DeviceID = -1;			//�豸���
		WorkMode = UNKNOW_MODE;	//��������,0Ϊ����ģʽ��1Ϊ����ģʽ
		TrigSource = UNKNOW_SRC;//�����������ڴ���ģʽ��ʱ�����ò���Ч��
		Brightness = -1;		//����
		Shutter = -1;			//�ع�ʱ��
		Contrast = -1;			//�Աȶ�
		Gain = -1;				//����
		Hue = -1;				//ɫ��
		Sharpness = -1;			//���
		Saturation = -1;		//���Ͷ�
		Width = -1;				//ͼ����
		Height = -1;			//ͼ��߶�
		OffsetX = -1;			//ͼ��Xƫ��
		OffsetY = -1;			//ͼ��Yƫ��
	}
}CamPara;


typedef void (__stdcall *STREAM_PROC)(unsigned char* buff, void* pUserData);
//typedef void (__stdcall *STREAM_PROC)(void* pOwner, unsigned char* pImage 					
//									  ,long lWidth, long lHeight, PGPixelFormat pixelFormat);





class DHCamera
{
public:
	DHCamera();
	virtual		~DHCamera();

	/************************************************************************/
	/* �����ʼ��                                               
	/* �������������ʼ���������
	/************************************************************************/
	bool		OpenCamera(CamPara& Para);
	bool		OpenCamera(int deviceID); 	
	bool		CloseCamera();		
	
	/************************************************************************/
	/* ���ID��״̬                                               
	/* ���IDΨһ��ʶһ̨���,initʱ����
	/* ״̬�������ӡ�δ����
	/************************************************************************/
	int         GetDeviceID();
	bool		IsConnected();									// �Ƿ������������



	/************************************************************************/
	/* �����������á����Զ�ȡ                                                              
	/* ֹͣ�������Ͽ���������ӣ��ͷ���Դ
	/************************************************************************/
	bool		SetWidth(int Value);							// ����ͼ����
	bool		SetHeight(int Value);							// ����ͼ��߶�

	bool		SetOffsetX(int Value);							// ����xƫ��
	bool		SetOffsetY(int Value);							// ����Yƫ��

	bool		SetGain(int Value);								// ��������
	bool		SetBrightness(int Value);						// ��������

	bool		SetShutter(double Value);						// �����ع�ʱ��

	bool		SetContrast(int Value);							// ���öԱȶ�
	bool		SetXRevise(bool ReviseOrNot = false);			// ����X�����Ƿ�ת
	bool		SetYRevise(bool ReviseOrNot = false);			// ����Y�����Ƿ�ת
	bool		SetWorkMode(WORKMODE Mode);						// ���ù���ģʽ���������߷Ǵ���   
	bool	    SetTriggerSource(TRIGSOURCE TrigSrc);			// ���ô���Դ

	int			GetWidth();										// ��ȡͼ����
	int			GetHeight();									// ��ȡͼ��߶�
	int			GetOffsetX();									// ��ȡXƫ��
	int			GetOffsetY();									// ��ȡYƫ��
	int			GetGain();										// ��ȡ����
	int			GetBrightness();								// ��ȡ����
	int			GetShutter();									// ��ȡ�ع�ʱ��
	int			GetContrast();									// ��ȡ�Աȶ�
	WORKMODE	GetWorkMode();									// ��ȡ����ģʽ���������߷Ǵ��� 
	TRIGSOURCE  GetTriggerSource();								// ��ȡ����Դ
	bool		GetXRevise();									// ��ȡX�����Ƿ�ת
	bool		GetYRevise();									// ��ȡY�����Ƿ�ת

	bool		SetCamPropertys(CamPara& Para);					// �������� 
	bool		GetCamPropertys(CamPara& Para);					// ������ȡ 

	GenICam::EPixelType GetPixelFormat();						// ��ȡͼ���ʽ

	/************************************************************************/
	/* ��������                                                              
	/* ָʾ�����ʼ/ֹͣ�ɼ�ͼ��
	/************************************************************************/
	bool		StartCapture();									// ��ʼ����                              
	bool		StopCapture();		                            // ֹͣ����
	bool		TriggerSoft();									// ������ÿ����һ������ɼ�һ��ͼƬ������ģʽ����Ч
	


	/************************************************************************/
	/* ��ȡͼ������
	/* ��ȡ����ɼ�����ԭʼͼ�����ݣ����������û�ȡ���ص�2�ֹ�����ʽ
	/* 1���û�������SetStreamHook������Ч����ָ�룬�����ڻص�ģʽ�£�WaitPicture��ȡ����ͼ��,����false
	/* 2���û�δ���ù�SetStreamHook�������SetStreamHook(NULL)��������������ȡģʽ�£�WaitPicture�ɻ�ȡ��ͼ��
	/************************************************************************/
	bool		WaitPicture(unsigned char *pbuff,int waitTime = 200); // ��ȡһ��ͼ��
	bool		SetStreamHook(STREAM_PROC proc, void*);					  // ���ûص�������ÿ�ɼ���һ��ͼƬ�ص�����������һ��

	/************************************************************************/
	/* ͼ���ʽת�� 
	/************************************************************************/
	
	uint64_t	GetImageSize();

private:

	int  EnumCameras();
	
	void DeviceLinkNotify(const GenICam::SConnectArg& conArg);	// ����֪ͨ�ص�����

	void TryResumeConn();										// ���߻ָ�����
	
	bool IpAutoConfig();										// IP�Զ����ã�ͬ�������Σ�
	
	void SetFinish();											// ���һ��ͼ��ɼ����   

	void ClrFinish();											// ������

	void onGetFrame(const GenICam::CFrame& frame);

	//int  getDevType(std::string strName);						// ��ȡ�������
	void SetDefaultParam();										// ������������������Ĭ�ϲ���

    __int64 getTimeInUs();                                      // ��ȡ΢���ʱ��
    double  getDobDff();

private:

	CamPara			m_camPara;
	WORKMODE		m_workModeCur; 


	bool			m_bCapturingFlg;
	bool			m_bConnectFlg;

	HANDLE          m_trigFinish;
	uint32_t        m_buffSize;
	unsigned char * m_pBuff;

	int             m_waitingTime;					//����
	bool			m_bRunning;

private:
	GenICam::ICameraPtr			m_cameraSptr;		// �����������ָ��
	std::string                 m_camKey;           // ���Ψһ��ʶ
	GenICam::IStreamSourcePtr	m_streamPtr;		// ����������ָ��
	Infra::CMutex				m_buffMutex;        // buff ����������
	STREAM_PROC                 m_pStreamProc;
	void*                       m_pUserPtr;

    // ������Է�����
    GenICam::IImageFormatControlPtr m_imageParamCtrlPtr;      //ImageFormat
    GenICam::IAnalogControlPtr m_analogCtrlPtr;               //AnalogControl
    GenICam::IISPControlPtr m_ispParamCtrlPtr;                //ISPControl
    GenICam::IAcquisitionControlPtr m_aquisitionCtrlPtr;      //AcquisitionControl

	static std::vector<GenICam::ICameraPtr> g_DHCameraList;
};

#endif