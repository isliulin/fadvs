#pragma once

#include "DHCamera.h"
typedef enum tagPGPixelFormat
{
	ePGUnknown	= -1,				// 未知
	ePGGrey8	= 0,				// 8位灰度图像
	ePGRGB24	= 1,				// 24位彩色图像
	ePGRGBA32	= 2					// 32位彩色图像
} PGPixelFormat;


void CALLBACK CCDCallBack1(unsigned char* lpParam, void* pUserData);  //回调函数
//typedef void (WINAPI *MVGrabbedCallback)(void* pOwner, unsigned char* pImage 					
//										 ,long lWidth, long lHeight, PGPixelFormat pixelFormat);

class Ts_DhCamera
{
public:
	Ts_DhCamera(void);
	~Ts_DhCamera(void);
	
	int m_nTotal;			 //设备号
	int m_nImageWidth;		//相机宽度
	int m_nImageHeight;		//相机高度
	int m_nOffsetX;
	int m_nOffsetY;

	
	BYTE *m_pSearchBuffer;     //搜索结果图像
	BYTE *m_pImageBuffer;	//图像显示Buff
	BYTE *m_pRawBuffer;		//获取图像Buff


	BITMAPINFO *m_pBitmapInfo;  //数据头信息
	BYTE m_pLutR[256];			//颜色值
	BYTE m_pLutG[256];
	BYTE m_pLutB[256];   
	
	BOOL m_bOpen;		  //开启状态
	BOOL m_bStart;			//采集状态
	bool m_bColor;			//颜色状态

	DHCamera m_camera;     //相机对象

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
