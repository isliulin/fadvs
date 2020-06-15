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

	// 相机对象
	CFGCamera m_fgCamera;
	// 是否为彩色相机
	BOOL m_bIsColorCamera;
	// 是否进行连续采集
	BOOL m_bLivingSnap;
	// 采集线程
	HANDLE m_hSnapThread;
	DWORD m_dwSnapThreadId;
	// 保存AVI线程
	HANDLE m_hStoreAVIThread;
	DWORD m_dwStoreAVIThreadId;

	// 图像分配内存区域及位图信息头空间
	UINT8* m_pImageBuffer;
	UINT8* m_pBayerBuffer;
	BITMAPINFO* m_pBitmapInfo;

	// 是否开始保存AVI
	BOOL m_bStartAVI;
	int m_nBytesPerPixel;
	PAVIFILE m_pFile;
	PAVISTREAM m_pStream;
	PAVISTREAM m_pCompressed;
	int m_nFrame;
	HRESULT m_hResult;

	// 显示文字
	CString m_strText;
	RECT m_rectOutVideo;
	int m_nCurrentImageNum;

	// 相机当前AOI宽高及起始位置
	UINT32 m_nAOIWidth;
	UINT32 m_nAOIHeight;
	UINT32 m_nAOIX;
	UINT32 m_nAOIY;
	UINT32 m_nTriggerValue;
	UINT32 m_nAOIStartX;
	UINT32 m_nAOIStartY;
	bool m_bIsBayerImage;

	// 保存图像宽高
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