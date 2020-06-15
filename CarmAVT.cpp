#include "stdafx.h"
#include "TSCtrlSys.h"
#include "CarmAVT.h"
#include "FGCamera.h"
#include "convert.h"
#include "Raw2Rgb.h"

#pragma comment(lib, "FGCamera.lib")
#pragma comment(lib,"hvdh/lib/Raw2Rgb.lib")
#pragma comment(lib,"hvdh/lib/HVDAILT.lib")
#pragma comment(lib,"hvdh/lib/HVUtil.lib")
//#pragma comment(lib,"Imaging/win32/debug/TIS_UDSHL09_vc9d.lib")
//#pragma comment(lib,"Imaging/win32/debug/TIS_UDSHL09_vc8d.lib")

CCarmAVT *g_pCarm = NULL;

IMPLEMENT_DYNAMIC(CCarmAVT, CWnd)

CCarmAVT::CCarmAVT()
{
	FGINIT arg;
	memset(&arg, 0, sizeof(FGINIT));
	arg.pCallback = SnapCallback;
	arg.Context = m_hWnd;

	FGInitModule(&arg);
	m_nShutter = 4000;		// 曝光时间
	m_nGain = 70;			// 增益值
	m_nPackageSize = 3500;	// 包长
	m_nBrightness = 128;	// 亮度
	m_nAOIWidth = 0;
	m_nAOIHeight = 0;
	m_nAOIX = 0;
	m_nAOIY = 0;
	m_nImageWidth = 0;
	m_nImageHeight = 0;
	m_bLivingSnap = FALSE;
	m_nSnapMode = 0;
	m_bIsColorCamera = FALSE;
	m_nFrame = 0;

	m_pBayerBuffer = NULL;
	m_pImageBuffer = NULL;
	m_pBitmapInfo = NULL;

	m_hResult = S_OK;
	m_pFile = NULL;
	m_pStream = NULL;
	m_pCompressed = NULL;
	m_bStartAVI = FALSE;
	for(int i=0;i<256;i++)
	{
		m_pLutR[i] = (BYTE)i;
		m_pLutG[i] = (BYTE)i;
		m_pLutB[i] = (BYTE)i;
	}
	g_pCarm = this;
}

CCarmAVT::~CCarmAVT()
{
	CloseCarm();
}

BEGIN_MESSAGE_MAP(CCarmAVT, CWnd)
END_MESSAGE_MAP()

bool CCarmAVT::OpenCarm()
{
	UINT32 nResult = FCE_NOERROR;
	FGNODEINFO nodeInfo[2];
	UINT32 nNodeCnt;
	// 获取相机列表
	nResult = FGGetNodeList(nodeInfo,2,&nNodeCnt);
	if(nResult != FCE_NOERROR)
	{
		MessageBox("没有找到摄像机", _T("Error"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	// 连接相机
	nResult = m_fgCamera.Connect(&nodeInfo[0].Guid,(void *)1);
	if(nResult!=FCE_NOERROR)
	{
		MessageBox("连接摄像机失败", _T("Error"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	// 检测相机是不是AVT彩色相机
	char* deviceName = new char[100];
	m_fgCamera.GetDeviceName(deviceName,100);
	CString strDevName = deviceName;
	if(strDevName.Find("AVT")==0)
	{
		if(strDevName.Find("C")==strDevName.GetLength()-1)
		{
			m_bIsColorCamera = TRUE;
			m_nBytesPerPixel = 3;
		}
		else
		{
			m_bIsColorCamera = FALSE;
			m_nBytesPerPixel = 1;
		}
	}
	delete[] deviceName;

	// 设置相机输出图像格式与类型
	if(m_bIsColorCamera)
	{
		nResult = m_fgCamera.SetParameter(FGP_IMAGEFORMAT, MAKEIMAGEFORMAT(RES_SCALABLE,CM_RAW8,0));
	}
	else
	{
		nResult = m_fgCamera.SetParameter(FGP_IMAGEFORMAT, MAKEIMAGEFORMAT(RES_SCALABLE,CM_Y8,0));
	}
	if(nResult != FCE_NOERROR)
	{
		MessageBox("不支持这种图像格式", _T("Error"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	// 获取相机分辨率信息
	m_fgCamera.GetParameter(FGP_XSIZE, &m_nAOIWidth);
	m_fgCamera.GetParameter(FGP_YSIZE, &m_nAOIHeight);
	m_fgCamera.GetParameter(FGP_XPOSITION, &m_nAOIX);
	m_fgCamera.GetParameter(FGP_YPOSITION, &m_nAOIY);

	// 为图像分配合适的缓冲区，并建立合适的位图头文件
	InitBitmap(m_nAOIWidth,m_nAOIHeight,m_bIsColorCamera);
	// 为采集分配DMA通道
	m_fgCamera.SetParameter(FGP_USEIRMFORCHN,0);
	m_fgCamera.SetParameter(FGP_ISOCHANNEL,0);

	// 设置包长为最大
	FGPINFO info;
	m_fgCamera.GetParameterInfo(FGP_PACKETSIZE,&info);
	m_fgCamera.SetParameter(FGP_PACKETSIZE,info.MaxValue);
	m_fgCamera.SetParameter(FGP_DMAMODE,DMA_REPLACE);

	// 分配内存并开始采集
	nResult = m_fgCamera.OpenCapture();
	if(nResult!=FCE_NOERROR)
	{
		MessageBox("OpenCapture Error", _T("Error"), MB_OK | MB_ICONEXCLAMATION);
		m_fgCamera.Disconnect();
		return false;
	}

	nResult = m_fgCamera.StartDevice();
	if(nResult!=FCE_NOERROR)
	{
		MessageBox("StartDevice Error", _T("Error"), MB_OK | MB_ICONEXCLAMATION);
		m_fgCamera.CloseCapture();
		m_fgCamera.Disconnect();
		return false;
	}
	m_bLivingSnap = true;

	return true;
}

void CCarmAVT::ShowBitmap(FGFRAME frame)
{
	if(g_pCarm->m_nCurrentImageNum == 3)
	{
	}

	// 是否分配图像缓冲区
	if(!m_pImageBuffer)
	{
		return;
	}
	if(m_bIsColorCamera)
	{
		ConvertRawY8(m_nImageWidth, m_nImageHeight, frame.pData, m_pBayerBuffer, 0);
		// Bayer转换未完成图像数据的上下翻转，翻转图像，放在m_pImageBuffer中
		for(unsigned long i=0;i<m_nImageHeight;i++)
		{
			for(unsigned long j=0;j<m_nImageWidth*3;j+=3)
			{
				*(m_pImageBuffer+i*m_nImageWidth*3+j) = *(m_pBayerBuffer+(m_nImageHeight-i-1)*m_nImageWidth*3+j);
				*(m_pImageBuffer+i*m_nImageWidth*3+j+1) = *(m_pBayerBuffer+(m_nImageHeight-i-1)*m_nImageWidth*3+j+1);
				*(m_pImageBuffer+i*m_nImageWidth*3+j+2) = *(m_pBayerBuffer+(m_nImageHeight-i-1)*m_nImageWidth*3+j+2);
			}
		}
		//ConvertBayer2Rgb(m_pImageBuffer,m_pBayerBuffer,m_nImageWidth,m_nImageHeight,
		//	BAYER2RGB_NEIGHBOUR,m_pLutR,m_pLutG,m_pLutB,true,BAYER_GR);
	}
	else
	{
		// 直接进行图像数据拷贝
		memcpy(m_pBayerBuffer,frame.pData,frame.Length);
		for(unsigned long i=0;i<m_nImageHeight;i++)
		{
			for(unsigned long j=0;j<m_nImageWidth;j++)
			{
				*(m_pImageBuffer+i*m_nImageWidth+j) = *(m_pBayerBuffer+(m_nImageHeight-i-1)*m_nImageWidth+j);
			}
		}
	}
}

void CCarmAVT::InitBitmap(UINT32 nImageWidth,UINT32 nImageHeight,BOOL bIsColor)
{
	m_nImageWidth = nImageWidth;
	m_nImageHeight = nImageHeight;
	if(m_pImageBuffer)
	{
		delete[] m_pImageBuffer;
		m_pImageBuffer = NULL;
	}
	if(m_pBayerBuffer)
	{
		delete[] m_pBayerBuffer;
		m_pBayerBuffer = NULL;
	}
	if(m_pBitmapInfo)
	{
		delete[] m_pBitmapInfo;
		m_pBitmapInfo = NULL;
	}

	if(m_nBytesPerPixel == 1)
	{
		m_pBayerBuffer = new UINT8[m_nImageWidth*m_nImageHeight*m_nBytesPerPixel+1582];
		m_pImageBuffer = new UINT8[m_nImageWidth*m_nImageHeight*m_nBytesPerPixel+1582];
	}
	else
	{
		m_pBayerBuffer = new UINT8[m_nImageWidth*m_nImageHeight*m_nBytesPerPixel];
		m_pImageBuffer = new UINT8[m_nImageWidth*m_nImageHeight*m_nBytesPerPixel];
	}

	if(bIsColor)
	{
		m_pBitmapInfo = (BITMAPINFO*)new char[sizeof(BITMAPINFOHEADER)];
		m_pBitmapInfo->bmiHeader.biBitCount = (UINT16)24;
	}
	else
	{
		m_pBitmapInfo = (BITMAPINFO*)new char[sizeof(BITMAPINFOHEADER)+256*4];
		m_pBitmapInfo->bmiHeader.biBitCount = (UINT16)8;
	}

	m_pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBitmapInfo->bmiHeader.biWidth = m_nImageWidth;
	m_pBitmapInfo->bmiHeader.biHeight = m_nImageHeight;
	m_pBitmapInfo->bmiHeader.biPlanes = 1;

	m_pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	m_pBitmapInfo->bmiHeader.biSizeImage = m_nImageWidth*m_nImageHeight*m_nBytesPerPixel;
	m_pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biClrUsed = 0;
	m_pBitmapInfo->bmiHeader.biClrImportant = 0;

	if(!bIsColor)
	{
		for(int i=0;i<256;i++)
		{
			m_pBitmapInfo->bmiColors[i].rgbRed = (UINT8)i;
			m_pBitmapInfo->bmiColors[i].rgbGreen = (UINT8)i;
			m_pBitmapInfo->bmiColors[i].rgbBlue = (UINT8)i;
			m_pBitmapInfo->bmiColors[i].rgbReserved = 0;
		}
	}
}

void WINAPI CCarmAVT::SnapCallback(void *pContext,UINT32 wParam,void *lParam)
{
	FGFRAME frame;
	UINT32 nResult = 0;
	switch(wParam)
	{
	case WPARAM_ERROR:
		break;
	case WPARAM_FRAMESREADY:
		do 
		{
			nResult = g_pCarm->m_fgCamera.GetFrame(&frame,0);
			if(nResult!=FCE_NOERROR)
			{
				break;
			}
			if(frame.pData)
			{
				g_pCarm->ShowBitmap(frame);
				g_pCarm->m_fgCamera.PutFrame(&frame);
				//g_pView->m_ImgStatic.m_pImageBuffer = g_pCarm->m_pImageBuffer;
				//g_pView->m_ImgStatic.Invalidate();
			}
		} while (!(frame.Flags&FGF_LAST));
		break;
	default:
		break;
	}
}

void CCarmAVT::CloseCarm()
{
	m_bLivingSnap = FALSE;
	m_fgCamera.StopDevice();
	m_fgCamera.CloseCapture();
	m_fgCamera.Disconnect();
	FGExitModule();

	if(m_pImageBuffer)
	{
		delete[] m_pImageBuffer;
		m_pImageBuffer = NULL;
	}

	if(m_pBayerBuffer)
	{
		delete[] m_pBayerBuffer;
		m_pBayerBuffer = NULL;
	}

	if(m_pBitmapInfo)
	{
		delete[] m_pBitmapInfo;
		m_pBitmapInfo = NULL;
	}
}

void CCarmAVT::SetCarmParam(int nShutter,int nBrightness,int nGain)
{
	m_fgCamera.SetParameter(FGP_SHUTTER, nShutter);
	m_fgCamera.SetParameter(FGP_BRIGHTNESS, nBrightness);
	m_fgCamera.SetParameter(FGP_GAIN, nGain);
}

void CCarmAVT::StopCarm()
{
	m_fgCamera.StopDevice();
}

void CCarmAVT::CarmLive()
{
	m_fgCamera.StartDevice();
}