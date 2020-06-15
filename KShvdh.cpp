#include "StdAfx.h"
#include "KShvdh.h"
#include "TSCtrlSys.h"
#include "Function.h"

#pragma comment(lib,"hvdh/lib/Raw2Rgb.lib")
#pragma comment(lib,"hvdh/lib/HVDAILT.lib")
#pragma comment(lib,"hvdh/lib/HVUtil.lib")

#define IMAGE_WIDTH 1280;
#define IMAGE_HEIGHT 1024;

CCriticalSection gCam_cs;

CDHCamera::CDHCamera(void)
{
	m_bOpen = FALSE;
	m_bStart = FALSE;
	m_bColor = false;
	m_pRawBuffer = NULL;
	m_pImageBuffer = NULL;
	m_pBitmapInfo = NULL;
	for(int i=0;i<256;i++)
	{
		m_pLutR[i] = (BYTE)i;
		m_pLutG[i] = (BYTE)i;
		m_pLutB[i] = (BYTE)i;
	}
}

CDHCamera::~CDHCamera(void)
{
	if(m_pImageBuffer!=NULL)
	{
		delete[] m_pImageBuffer;
		m_pImageBuffer = NULL;
	}
	if(m_pRawBuffer!=NULL)
	{
		delete[] m_pRawBuffer;
		m_pRawBuffer = NULL;
	}
}

BOOL CDHCamera::InitDH(int nIndexNo,long lGain,int nStartX,int nStartY,int nWidth,int nHeight,HV_RESOLUTION hvResolution,bool bColor)
{
	m_bOpen = FALSE;
	m_bColor = bColor;
	HVSTATUS hvStatus = STATUS_OK;
	hvStatus = HVGetDeviceTotal(&m_nTotal);
	HV_VERIFY(hvStatus);
	
	if(m_nTotal<=nIndexNo)
	{
		return FALSE;
	}
	hvStatus = BeginHVDevice(nIndexNo+1,&m_hHV);//init camera,get the handle;
	HV_VERIFY(hvStatus);
	hvStatus = HVSetResolution(m_hHV,hvResolution);//set the resolution 
	HV_VERIFY(hvStatus);
	hvStatus = HVSetSnapSpeed(m_hHV,NORMAL_SPEED);//set the grab speed
	HV_VERIFY(hvStatus);
	// �ɼ�ģʽ������CONTINUATION(����)��TRIGGER(�ⴥ��)
	hvStatus = HVSetSnapMode(m_hHV,CONTINUATION);//set the grab mode 
	HV_VERIFY(hvStatus);
	
	// ���ø�����ͨ������
	for(int i=0;i<4;i++)
	{
		hvStatus = HVAGCControl(m_hHV,(BYTE)(RED_CHANNEL+i),lGain);
		HV_VERIFY(hvStatus);
	}
	// ����ADC�ļ���:ģ��ת������
	hvStatus = HVADCControl(m_hHV,ADC_BITS,ADC_LEVEL2);
	HV_VERIFY(hvStatus);
	if(nWidth<=0)
	{
		m_nImageWidth = IMAGE_WIDTH;
	}
	else
	{
		m_nImageWidth = nWidth;
	}
	if(nHeight<=0)
	{
		m_nImageHeight = IMAGE_HEIGHT;
	}
	else
	{
		m_nImageHeight = nHeight;
	}
	//Set the ROI 
	hvStatus = HVSetOutputWindow(m_hHV,nStartX,nStartY,m_nImageWidth,m_nImageHeight);
	/*
	 *	����ԭʼͼ�񻺳�����һ�������洢�ɼ�ͼ��ԭʼ����
	 *  һ��ͼ�񻺳�����С��������ڴ�С����Ƶ��ʽȷ����
	 */
	/*
	����Bayerת����ͼ�����ݻ���
	*/
	m_pRawBuffer = new BYTE[m_nImageWidth*m_nImageHeight];
	ASSERT(m_pRawBuffer);
	m_pImageBuffer = new BYTE[m_nImageWidth*m_nImageHeight*3];
	ASSERT(m_pImageBuffer);
	/*
	 *	��ʼ������������ɼ�ͼ���ڴ�Ŀ��ƣ�
	 *	ָ���ص�����SnapThreadCallback��thisָ��
	 */
	//ָ���ص����� ��Դ�����
	hvStatus = HVOpenSnap(m_hHV,SnapThreadCallback,this);
	HV_VERIFY(hvStatus);
	if(HV_SUCCESS(hvStatus))
	{
		m_bOpen = TRUE;
	}
	m_tmDHCamera.hhv = m_hHV;
	m_tmDHCamera.nIndexNo = nIndexNo;
	return m_bOpen;
}

int CALLBACK CDHCamera::SnapThreadCallback(HV_SNAP_INFO *pInfo)
{
	CDHCamera *thisClass = (CDHCamera *)(pInfo->pParam);
	HV_ARG_GET_LAST_STATUS hvArgFeature;
	hvArgFeature.type = HV_LAST_STATUS_TRANSFER;
	HV_RES_GET_LAST_STATUS hvResFeature;

	HVAPI_CONTROL_PARAMETER p;
	p.pInBuf = &hvArgFeature;
	p.dwInBufSize = sizeof(hvArgFeature);
	p.pOutBuf = &hvResFeature;
	p.dwOutBufSize = sizeof(hvResFeature);
	p.pBytesRet = NULL;
	p.code = ORD_GET_LAST_STATUS;
	int dwSize = sizeof(p);

	HVSTATUS hvStatus = HVCommand(thisClass->m_hHV,CMD_HVAPI_CONTROL,&p,&dwSize);
	HV_VERIFY(hvStatus);
	if(hvResFeature.status==0)
	{
			gCam_cs.Lock();//�����ڴ汣����8bit bayer data--->24 rgb data;
			ConvertBayer2Rgb(thisClass->m_pImageBuffer,thisClass->m_pRawBuffer,thisClass->m_nImageWidth,
				thisClass->m_nImageHeight,BAYER2RGB_NEIGHBOUR,thisClass->m_pLutR,thisClass->m_pLutG,thisClass->m_pLutB,true,BAYER_GB);
			gCam_cs.Unlock();//�˳��ڴ汣����	
	}
	return 1;
}

void CDHCamera::Live()
{
	if(m_nTotal<=0)
	{
		return;
	}
	if(!m_bOpen)
	{
		return;
	}
	if(m_bStart)
	{
		return;
	}
	/*
	*	��������������ɼ�ͼ���ڴ�
	*/
	HVSTATUS hvStatus = STATUS_OK;
	BYTE *pBuffer[1];
	pBuffer[0] = m_pRawBuffer;
	hvStatus = HVStartSnap(m_hHV,pBuffer,1);//image data buffer pointer->m_pRawBuffer;  //1:image data buffer num;
	HV_VERIFY(hvStatus);
	if(HV_SUCCESS(hvStatus))
	{
		m_bStart = TRUE;
	}
}

void CDHCamera::Stop()
{
	if(m_nTotal<=0)
	{
		return;
	}
	if(!m_bOpen)
	{
		return;
	}
	if(!m_bStart)
	{
		return;
	}
	HVSTATUS hvStatus = STATUS_OK;
	hvStatus = HVStopSnap(m_hHV);
	HV_VERIFY(hvStatus);
	if(HV_SUCCESS(hvStatus))
	{
		m_bStart = TRUE;
	}
	else
	{
		AfxMessageBox("CDHCamera::Stop Failed!");
	}
}

void CDHCamera::Free()
{
	if(m_nTotal<=0)
	{
		return;
	}
	if(!m_bOpen)
	{
		return;
	}
	HVSTATUS hvStatus = STATUS_OK;
	if(m_bOpen)
	{
		hvStatus = HVCloseSnap(m_hHV);
	}
	HV_VERIFY(hvStatus);
	if(HV_SUCCESS(hvStatus))
	{
		m_bOpen = FALSE;
		m_bStart = FALSE;
	}
	else
	{
		AfxMessageBox("HVCloseSnap Failed!");
	}
	// �ر�������������ͷ�����������ڲ���Դ
	hvStatus = EndHVDevice(m_hHV);
	HV_VERIFY(hvStatus);
	if(HV_SUCCESS(hvStatus))
	{

	}
	else
	{
		AfxMessageBox("EndHVDevice Failed!");
	}
}

void CDHCamera::SetExposureTime(long lValue)
{
	if(m_nTotal<=0)
	{
		return;
	}
	if(!m_bOpen)
	{
		return;
	}
	// ���ÿ����ٶ�
	HVSTATUS status = HVAECControl(m_hHV,AEC_SHUTTER_SPEED,lValue);
	HV_VERIFY(status);
	// ���ÿ��ŵ�λ
	status = HVAECControl(m_hHV,AEC_SHUTTER_UNIT,SHUTTER_MS);
	HV_VERIFY(status);
}

void CDHCamera::SetGain(long lValue)
{
	if(m_nTotal<=0)
	{
		return;
	}
	if(!m_bOpen)
	{
		return;
	}
	// ���ø�������������
	for(int i=0;i<4;i++)
	{
		HVSTATUS hvStatus = HVAGCControl(m_hHV,(BYTE)(RED_CHANNEL+i),lValue);
		HV_VERIFY(hvStatus);
	}
}