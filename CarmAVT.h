#pragma once
#include "FGCamera.h"
#include "convert.h"
#include "hvdh/include/HVDAILT.H"
#include "hvdh/include/Raw2Rgb.h"

class CCarmAVT : public CWnd
{
	DECLARE_DYNAMIC(CCarmAVT)
public:
	CCarmAVT();
	virtual ~CCarmAVT();
protected:
	DECLARE_MESSAGE_MAP()
public:
	int m_nShutter;
	int m_nGain;
	int m_nPackageSize;
	int m_nBrightness;
	int m_nSnapMode;
	BYTE m_pLutR[256];
	BYTE m_pLutG[256];
	BYTE m_pLutB[256];

	// �������
	CFGCamera m_fgCamera;
	// �Ƿ�Ϊ��ɫ���
	BOOL m_bIsColorCamera;
	// �Ƿ���������ɼ�
	BOOL m_bLivingSnap;
	// �ɼ��߳�
	HANDLE m_hSnapThread;
	DWORD m_dwSnapThreadId;
	// ����AVI�߳�
	HANDLE m_hStoreAVIThread;
	DWORD m_dwStoreAVIThreadId;

	// ͼ������ڴ�����λͼ��Ϣͷ�ռ�
	UINT8* m_pImageBuffer;
	UINT8* m_pBayerBuffer;
	BITMAPINFO* m_pBitmapInfo;

	// �Ƿ�ʼ����AVI
	BOOL m_bStartAVI;
	int m_nBytesPerPixel;
	PAVIFILE m_pFile;
	PAVISTREAM m_pStream;
	PAVISTREAM m_pCompressed;
	int m_nFrame;
	HRESULT m_hResult;

	// ��ʾ����
	CString m_strText;
	RECT m_rectOutVideo;
	int m_nCurrentImageNum;

	// �����ǰAOI��߼���ʼλ��
	UINT32 m_nAOIWidth;
	UINT32 m_nAOIHeight;
	UINT32 m_nAOIX;
	UINT32 m_nAOIY;
	UINT32 m_nTriggerValue;
	UINT32 m_nAOIStartX;
	UINT32 m_nAOIStartY;
	bool m_bIsBayerImage;

	// ����ͼ����
	UINT32 m_nImageWidth;
	UINT32 m_nImageHeight;

	bool OpenCarm();
	void InitBitmap(UINT32 nImageWidth,UINT32 nImageHeight,BOOL bIsColor);
	void ShowBitmap(FGFRAME frame);
	static void WINAPI SnapCallback(void *pContext,UINT32 wParam,void *lParam);
	void CarmLive();
	void StopCarm();
	void CloseCarm();
	void SetCarmParam(int nShutter,int nBrightness,int nGain);
};