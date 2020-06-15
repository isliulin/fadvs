#pragma once

#include "DHCamera.h"
typedef enum tagPGPixelFormat
{
	ePGUnknown	= -1,				// δ֪
	ePGGrey8	= 0,				// 8λ�Ҷ�ͼ��
	ePGRGB24	= 1,				// 24λ��ɫͼ��
	ePGRGBA32	= 2					// 32λ��ɫͼ��
} PGPixelFormat;


void CALLBACK CCDCallBack1(unsigned char* lpParam, void* pUserData);  //�ص�����
//typedef void (WINAPI *MVGrabbedCallback)(void* pOwner, unsigned char* pImage 					
//										 ,long lWidth, long lHeight, PGPixelFormat pixelFormat);

class Ts_DhCamera
{
public:
	Ts_DhCamera(void);
	~Ts_DhCamera(void);
	
	int m_nTotal;			 //�豸��
	int m_nImageWidth;		//������
	int m_nImageHeight;		//����߶�
	int m_nOffsetX;
	int m_nOffsetY;

	
	BYTE *m_pSearchBuffer;     //�������ͼ��
	BYTE *m_pImageBuffer;	//ͼ����ʾBuff
	BYTE *m_pRawBuffer;		//��ȡͼ��Buff


	BITMAPINFO *m_pBitmapInfo;  //����ͷ��Ϣ
	BYTE m_pLutR[256];			//��ɫֵ
	BYTE m_pLutG[256];
	BYTE m_pLutB[256];   
	
	BOOL m_bOpen;		  //����״̬
	BOOL m_bStart;			//�ɼ�״̬
	bool m_bColor;			//��ɫ״̬

	DHCamera m_camera;     //�������

	int CCD_ID;
	int CCD_OK; 
	long m_triggerCnt;
	long m_recvPicCnt;
	int ImageOK;
	int CCD_ADD;
	int m_isStartCap;

	int Init();
	int SoftTrigger();
	int Exit();

	int GetImage(BYTE* buf);

	int StartCapture();
	int StopCapture();
	int getExposure();
	int setExposure(int exposure);//unit ms;
	int setGain(int value);
	void SetROI(int offsetX,int offsetY,int width,int height);
	void EnterContinueMode();
	bool TriggerSoft();
	void SingleGrab();
	void StopGrba();
	void CloseCamera();
	bool IsConnected();
};
