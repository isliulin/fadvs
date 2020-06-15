#pragma once
#include "camera.h"
#include "hvdh/include/HVDAILT.H"
#include "hvdh/include/Raw2Rgb.h"

struct STRUCT_DHCAMERA
{
	HHV hhv;
	int nIndexNo;//0,1,2...
};

class CDHCamera
{
public:
	CDHCamera(void);
	~CDHCamera(void);
public:
	int m_nTotal;
	int m_nImageWidth;
	int m_nImageHeight;
	BYTE *m_pImageBuffer;
	BYTE *m_pRawBuffer;
	HHV m_hHV;
	BITMAPINFO *m_pBitmapInfo;
	BYTE m_pLutR[256];
	BYTE m_pLutG[256];
	BYTE m_pLutB[256];
	BOOL m_bOpen;
	BOOL m_bStart;
	bool m_bColor;
	STRUCT_DHCAMERA m_tmDHCamera;

	BOOL InitDH(int nIndexNo,long lGain=25,int nStartX=0,int nStartY=0,int nWidth=-1,int nHeight=-1,HV_RESOLUTION hvResolution=RES_MODE0,bool bColor=true);
	static int CALLBACK SnapThreadCallback(HV_SNAP_INFO *pInfo);
	void Live();
	void Stop();
	void Free();
	void SetExposureTime(long lValue);
	void SetGain(long lValue);
};
