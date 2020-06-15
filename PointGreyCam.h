#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "CameraBase.h"
#include "FlyCapture2.h"
#include "LogFile.h"
#include <vector>

using namespace FlyCapture2;
using namespace std;

class PointGreyCam
{
public:
	PointGreyCam();
	~PointGreyCam(void);

public:
	int GetCameraList(vector<int>* devices);
	long OpenCamera(const int sn,unsigned int offsetx,unsigned int offsety,unsigned int width,unsigned int height);
	long CloseCamera();                                           // �ر��豸
	long Shot(FlyCapture2::Image &image) ;                        // ����
	long StartAcquire(int iRate);       // ���� iRateΪץ�ļ��ʱ�� ��iImageType = SCI_DIBIMAGE,�ص���������bmpλͼ����iImageType = SCI_SCIIMAGE,�ص���������ImageͼƬ
	long StopAcquire();// ȡ������     
	FlyCapture2::Image rawImage; 
	unsigned char* m_pGrabImage;
	bool SetShutter(float fShutter);
    bool GetShutter(float *pShutter);
	bool SetGain(float fGain);
	bool GetGain(float *pGain);
	bool TransferData();
	bool IsConnected();
	unsigned int offsetX;
	unsigned int offsetY;
	unsigned int Width;
	unsigned int Height;
	CLogFile *m_pLog;
private:
	void Initialize();
	bool GUIDStringToIntArray(const char* strGUID, unsigned int arrayValue[]);
	bool IntArrayToGUIDString(unsigned int arrayValue[], char strGUID[]);
	DWORD WINAPI ThreadForAcqImage(LPVOID pPointer);
	bool StartWorkingThread();
	bool PollForTriggerReady(CameraBase* pCam);
	bool FireSoftwareTrigger(CameraBase* pCam);

private:
	CameraBase* m_pCamBase;
	Camera* m_pCam;
	GigECamera* m_pGigECam;
	bool m_bInGrab;
	InterfaceType m_Type;
};
