#include "stdafx.h"
#include <conio.h>
#include <malloc.h>
#include "MilApp.h"

#include "Mil.h"
#include "MILDyn/milblob.h"
//#include "MilEdge.h"
#include "MILDyn/milpat.h"
#include "MILDyn/milmeas.h"
#include "MilDyn/MilMod.h"
#include "TSCtrlSys.h"
#include <math.h>

//#pragma comment(lib,"mil.lib")
//#pragma comment(lib,"milblob.lib")
////#pragma comment(lib, "miledge.lib")
//#pragma comment(lib,"milim.lib")
//#pragma comment(lib,"milmeas.lib")
//#pragma comment(lib,"milpat.lib")
//#pragma comment(lib,"milmod.lib")
//#pragma warning(disable:4244)
//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

void CMilApp::OverlayRect(int nColor,CPoint startPoint,CPoint endPoint,bool bErase)
{
	if(bErase)
	{
		//MdispControl(m_MilDisplay,M_WINDOW_COLOR,M_ENABLE);
		MgraClear(M_DEFAULT,m_MilOverlayImage);
	}
	switch(nColor)
	{
	case 1:
		MgraColor(M_DEFAULT,M_RGB888(255,0,0));
		break;
	case 2:
		MgraColor(M_DEFAULT,M_RGB888(0,255,0));
		break;
	case 3:
		MgraColor(M_DEFAULT,M_RGB888(0,0,255));
		break;
	case 4:
		MgraColor(M_DEFAULT,M_RGB888(255,255,0));
		break;
	}
	MgraRect(M_DEFAULT,m_MilImage,startPoint.x,startPoint.y,endPoint.x,endPoint.y);
}

void CMilApp::OverlayDrawText(int nColor,long lOffsetX,long lOffsetY,char *pText)
{
	MgraFont(M_DEFAULT,M_FONT_DEFAULT_SMALL);
	MgraColor(M_DEFAULT,nColor);
	MgraText(M_DEFAULT,m_MilOverlayImage,lOffsetX,lOffsetY,pText);
}

void CMilApp::ClearImageBuffer(MIL_ID MilImage,int nColor)
{
	MbufClear(MilImage,nColor);
}

void CMilApp::FreeOverlay(MIL_ID MilMain)
{
	MbufControl(MilMain,M_WINDOW_DC_FREE,M_DEFAULT);
}

void CMilApp::CloseImage()
{
	if(m_MilImage>0)
	{
		MbufFree(m_MilImage);
	}
}

void CMilApp::SelectShowWindow(HWND hWnd,MIL_ID MilImage)
{
	MdispSelectWindow(m_MilDisplay,MilImage,hWnd);
}

void CMilApp::OverlayLine(int nColor,CPoint startPoint,CPoint endPoint)
{
	if(nColor==1)
	{
		MgraColor(M_DEFAULT,M_RGB888(255,0,0));
	}
	else
	{
		MgraColor(M_DEFAULT,M_RGB888(0,0,255));
	}
	MgraLine(M_DEFAULT,m_MilOverlayImage,startPoint.x,startPoint.y,endPoint.x,endPoint.y);
}

void CMilApp::SetBlobParam(int nThreshold,double dMinArea,double dMaxArea,long lOpen,long lClose,bool bFindBlack)
{
	MilBlob.m_nThreshold = nThreshold;
	MilBlob.m_lOpen = lOpen;
	MilBlob.m_lClose = lClose;
	MilBlob.m_dMinArea = dMinArea;
	MilBlob.m_dMaxArea = dMaxArea;
	MilBlob.m_bFindBlack = bFindBlack;
}

bool CMilApp::FindBlob(CRect ROIB)
{
	long lBlobNum = 0;
	MIL_ID FeatureList,Result;
	double *pCogX,*pCogY,*pMinX,*pMaxX,*pMinY,*pMaxY,*pWidth,*pLength,*pArea;
	long lOffsetX = ROIB.left;
	long lOffsetY = ROIB.top;
	long lSizeX = ROIB.Width();
	long lSizeY = ROIB.Height();
	//MgraColor(M_DEFAULT,M_RGB888(255,0,0));
	//MbufChild2d(m_MilImage,lOffsetX,lOffsetY,lSizeX,lSizeY,&m_SubImage);
	MbufChild2d(m_lPadDetectImage,lOffsetX,lOffsetY,lSizeX,lSizeY,&m_SubImage);
	if(!MilBlob.m_bFindBlack)//white blob
	{
		MimBinarize(m_SubImage,m_SubImage,M_GREATER_OR_EQUAL,MilBlob.m_nThreshold,M_NULL);
	}
	else//black blob
	{
		MimBinarize(m_SubImage,m_SubImage,M_LESS_OR_EQUAL,MilBlob.m_nThreshold,M_NULL);
	}
	MimOpen(m_SubImage,m_SubImage,MilBlob.m_lOpen,M_BINARY);
	MimClose(m_SubImage,m_SubImage,MilBlob.m_lClose,M_BINARY);
	MblobAllocFeatureList(m_MilSystem,&FeatureList);
	MblobSelectFeature(FeatureList,M_AREA);
	MblobAllocResult(m_MilSystem,&Result);
	//MblobControl(Result,M_FOREGROUND_VALUE,M_ZERO);
	MblobCalculate(m_SubImage,M_NULL,FeatureList,Result);

	MblobGetNumber(Result,&lBlobNum);
	MblobSelect(Result,M_EXCLUDE,M_AREA,M_OUT_RANGE,MilBlob.m_dMinArea,MilBlob.m_dMaxArea);
	MblobGetNumber(Result,&lBlobNum);
	BlobResult.nBlobNum = (int)lBlobNum;
	CString str;
	str.Format("BlobNum:%d",lBlobNum);
	g_pFrm->m_CmdRun.AddMsg(str);
	str.Format("Threshold:%d",MilBlob.m_nThreshold);
	g_pFrm->m_CmdRun.AddMsg(str);
	str.Format("MinArea:%.3f,MaxArea:%.3f,m_bFindBlack:%d",MilBlob.m_dMinArea,MilBlob.m_dMaxArea,(bool)MilBlob.m_bFindBlack);
	g_pFrm->m_CmdRun.AddMsg(str);
	if(lBlobNum>=5000||lBlobNum<1)  //2018-12-01 modify...
	{
		MblobFree(FeatureList);
		MblobFree(Result);
		MbufFree(m_SubImage);
		return false;
	}
	MblobFree(FeatureList);
	MblobAllocFeatureList(m_MilSystem,&FeatureList);
	MblobSelectFeature(FeatureList,M_CENTER_OF_GRAVITY);
	MblobSelectFeature(FeatureList,M_BOX_X_MAX);
	MblobSelectFeature(FeatureList,M_BOX_X_MIN);
	MblobSelectFeature(FeatureList,M_BOX_Y_MAX);
	MblobSelectFeature(FeatureList,M_BOX_Y_MIN);
	MblobSelectFeature(FeatureList,M_FERET_X);
	MblobSelectFeature(FeatureList,M_FERET_Y);
	MblobSelectFeature(FeatureList,M_AREA);
	
	MblobCalculate(m_SubImage,M_NULL,FeatureList,Result);

	pCogX = (double *)malloc(lBlobNum*sizeof(double));
	pCogY = (double *)malloc(lBlobNum*sizeof(double));
	pMinX = (double *)malloc(lBlobNum*sizeof(double));
	pMaxX = (double *)malloc(lBlobNum*sizeof(double));
	pMinY = (double *)malloc(lBlobNum*sizeof(double));
	pMaxY = (double *)malloc(lBlobNum*sizeof(double));
	pWidth = (double *)malloc(lBlobNum*sizeof(double));
	pLength = (double *)malloc(lBlobNum*sizeof(double));
	pArea = (double *)malloc(lBlobNum*sizeof(double));

	MblobGetResult(Result,M_CENTER_OF_GRAVITY_X,pCogX);
	MblobGetResult(Result,M_CENTER_OF_GRAVITY_Y,pCogY);
	MblobGetResult(Result,M_BOX_X_MIN,pMinX);
	MblobGetResult(Result,M_BOX_X_MAX,pMaxX);
	MblobGetResult(Result,M_BOX_Y_MIN,pMinY);
	MblobGetResult(Result,M_BOX_Y_MAX,pMaxY);
	MblobGetResult(Result,M_FERET_X,pWidth);
	MblobGetResult(Result,M_FERET_Y,pLength);
	MblobGetResult(Result,M_AREA,pArea);

	for(int i=0;i<lBlobNum;i++)
	{
		BlobResult.dBlobCenterX[i] = pCogX[i]+lOffsetX;
		BlobResult.dBlobCenterY[i] = pCogY[i]+lOffsetY;
		BlobResult.dBlobXMin[i] = pMinX[i]+lOffsetX;
		BlobResult.dBlobXMax[i] = pMaxX[i]+lOffsetX;
		BlobResult.dBlobYMin[i] = pMinY[i]+lOffsetY;
		BlobResult.dBlobYMax[i] = pMaxY[i]+lOffsetY;
		BlobResult.dBlobWidth[i] = pWidth[i];
		BlobResult.dBlobLength[i] = pLength[i];
		BlobResult.dBolbArea[i] = pArea[i];
	}
	free(pCogX);
	free(pCogY);
	free(pMinX);
	free(pMaxX);
	free(pMinY);
	free(pMaxY);
	free(pWidth);
	free(pLength);
	free(pArea);

	MblobFree(FeatureList);
	MblobFree(Result);
	MbufFree(m_SubImage);
	return true;
}

void CMilApp::CopyBuffer(MIL_ID buffer,BYTE *pData,int type)
{
	long BufSizeX=m_lBufSizeX,BufSizeY=m_lBufSizeY;
	int _iSize = BufSizeX * BufSizeY;
	if (type==RGB24)
	{
		BYTE *m_pImageBuffer = new BYTE[_iSize];
		for(int i=0; i<_iSize; i++)
		{
			m_pImageBuffer[i]=(BYTE)(0.299*pData[3*i+0]+0.587*pData[3*i+1]+0.114*pData[3*i+2]) ;//彩色转黑白
		}
		MbufPut2d(buffer, 0, 0, m_lBufSizeX, m_lBufSizeY, m_pImageBuffer);
		delete[]m_pImageBuffer;
		m_pImageBuffer = NULL;
	}
	else if (type==RGB8)
	{
		BYTE *m_pImageBuffer = new BYTE[_iSize];
		memcpy(m_pImageBuffer,pData,_iSize*sizeof(BYTE));
		MbufPut2d(buffer, 0, 0, m_lBufSizeX, m_lBufSizeY, m_pImageBuffer);
		delete[] m_pImageBuffer;
		m_pImageBuffer = NULL;
	}
}

void CMilApp::CopyBuffer(MIL_ID buffer,BYTE *pData,long lWidth,long lHeight,int type)
{
	m_lPadBufSizeX = lWidth;
	m_lPadBufSizeY = lHeight;
	long BufSizeX=m_lPadBufSizeX,BufSizeY=m_lPadBufSizeY;
	int _iSize = BufSizeX * BufSizeY;
	if (type==RGB24)
	{
		BYTE *m_pGrayBuffer = new BYTE[_iSize];
		for(int i=0; i<_iSize; i++)
		{
			m_pGrayBuffer[i]=(BYTE)(0.299*pData[3*i+0]+0.587*pData[3*i+1]+0.114*pData[3*i+2]) ;//彩色转黑白
		}
		MbufPut2d(buffer, 0, 0, m_lPadBufSizeX, m_lPadBufSizeY, m_pGrayBuffer);
		delete[]m_pGrayBuffer;
		m_pGrayBuffer = NULL;
	}
	else if (type==RGB8)
	{
		BYTE *tempData = new BYTE[_iSize];
		memcpy(tempData,pData,_iSize*sizeof(BYTE));
		MbufPut2d(buffer, 0, 0, m_lPadBufSizeX, m_lPadBufSizeY, tempData);
		delete[]tempData;
		tempData = NULL;
	}
}

bool CMilApp::FindBlob(CRect ROIB,long *pResultNum,double *pCogX,double *pCogY)
{
	MIL_ID FeatureList,Result;

	long lOffsetX = ROIB.left;
	long lOffsetY = ROIB.top;
	long lSizeX = ROIB.Width();
	long lSizeY = ROIB.Height();

	//MgraColor(M_DEFAULT,M_RGB888(255,0,0));
	MbufChild2d(m_MilImage,lOffsetX,lOffsetY,lSizeX,lSizeY,&m_SubImage);
	if(MilBlob.m_bFindBlack)
	{
		MimBinarize(m_SubImage,m_SubImage,M_GREATER_OR_EQUAL,MilBlob.m_nThreshold,M_NULL);
	}
	else
	{
		MimBinarize(m_SubImage,m_SubImage,M_LESS_OR_EQUAL,MilBlob.m_nThreshold,M_NULL);
	}
	//MimOpen(m_SubImage,m_SubImage,MilBlob.m_lOpen,M_BINARY);
	//MimClose(m_SubImage,m_SubImage,MilBlob.m_lClose,M_BINARY);

	MblobAllocFeatureList(m_MilSystem,&FeatureList);
	MblobSelectFeature(FeatureList,M_AREA);
	MblobAllocResult(m_MilSystem,&Result);
	CString str;
	str.Format("m_nThreshold:%d",MilBlob.m_nThreshold);
	//MblobControl(Result,M_FOREGROUND_VALUE,M_ZERO);
	g_pFrm->m_CmdRun.AddMsg(str);
	MblobCalculate(m_SubImage,M_NULL,FeatureList,Result);
	MblobSelect(Result,M_EXCLUDE,M_AREA,M_OUT_RANGE,MilBlob.m_dMinArea,MilBlob.m_dMaxArea);

	long lBlobNum = 0;
	if(lBlobNum>20||lBlobNum<1)
	{
		MblobFree(FeatureList);
		MblobFree(Result);
		MblobFree(m_SubImage);
		return false;
	}

	MblobFree(FeatureList);
	MblobAllocFeatureList(m_MilSystem,&FeatureList);
	MblobSelectFeature(FeatureList,M_CENTER_OF_GRAVITY);

	MblobCalculate(m_SubImage,M_NULL,FeatureList,Result);
	MblobGetNumber(Result,&lBlobNum);
	MblobGetResult(Result,M_CENTER_OF_GRAVITY_X,pCogX);
	MblobGetResult(Result,M_CENTER_OF_GRAVITY_Y,pCogY);
	for(int i=0;i<lBlobNum;i++)
	{
		pCogX[i] = pCogX[i] + lOffsetX;
		pCogY[i] = pCogY[i] + lOffsetY;
	}
	*pResultNum = lBlobNum;
	MblobFree(FeatureList);
	MblobFree(Result);
	MbufFree(m_SubImage);
	return true;
}

//2018-12-01 modify...
void CMilApp::DisSelectWindow(HWND hWnd,int nZoomFactor)
{
	if(m_MilDisplay>0)
	{
		MdispFree(m_MilDisplay);
	}
	MdispAlloc(m_MilSystem,M_DEFAULT,M_DEF_DISPLAY_FORMAT,M_DEFAULT,&m_MilDisplay);
	MdispZoom(m_MilDisplay,(long)nZoomFactor,(long)nZoomFactor);
	MdispSelectWindow(m_MilDisplay,m_MilImage,hWnd);
	//MdispControl(m_MilDisplay,M_WINDOW_OVR_WRITE,M_ENABLE);
	//MdispControl(m_MilDisplay,M_WINDOW_OVR_WRITE,M_ENABLE);
	//MdispInquire(m_MilDisplay,M_WINDOW_OVR_BUF_ID,&m_MilOverlayImage);
	//g_pFrm->m_CmdRun.AddMsg("M_WINDOW_OVR_BUF_ID");
	return;
}

CMilApp::CMilApp()
{
	ColorTable.nRed = 249;
	ColorTable.nGreen = 250;
	ColorTable.nBlue = 252;
	ColorTable.nYellow = 251;
	ColorTable.nMagenta = 253;
	ColorTable.nCyan = 254;

	m_MilDigitizer = M_NULL;
	m_MilModelID = M_NULL;
	m_MilPatResultID = M_NULL;
	m_MilBlobResult = M_NULL;
	m_MilBlobFeatureList = M_NULL;
	m_MilApplication = M_NULL;
	m_MilSystem = M_NULL;
	m_MilDisplay = M_NULL;
	m_MilImage = M_NULL;

	m_GeometricModel = M_NULL;
	m_GeometricResult = M_NULL;

	m_lBufSizeX = IMAGEWIDTH0;
	m_lBufSizeY = IMAGEHEIGHT0;

	m_lBufSizeBand = 1;
	m_emMatchMode = GRAYSCALE;

	m_winSearchRect.TopLeft().x = 0;
	m_winSearchRect.TopLeft().y = 0;
	m_winSearchRect.BottomRight().x = m_lBufSizeX-1;
	m_winSearchRect.BottomRight().y = m_lBufSizeY-1;

	m_winModelRect.TopLeft().x = 0;
	m_winModelRect.TopLeft().y = 0;
	m_winModelRect.BottomRight().x = m_lBufSizeX-1;
	m_winModelRect.BottomRight().y = m_lBufSizeY-1;

	// 颗粒检测参数初始化
	m_lPadBufSizeX = IMAGEWIDTH1;
	m_lPadBufSizeY = IMAGEHEIGHT1;
	m_lPadDetectImage = M_NULL;
	m_lPadGrayModel = M_NULL;
	m_lPadGrayResult = M_NULL;
	m_lPadGeometricModel = M_NULL;
	m_lPadGeometricResult = M_NULL;

	m_lNeedleBufSizeX=IMAGEWIDTH2;
	m_lNeedleBufSizeY=IMAGEHEIGHT2;
	m_lNeedleImage=M_NULL;
	m_lNeedleGrayModel=M_NULL;
	m_lNeedleGrayResult=M_NULL;
	m_lNeedleGeometricModel=M_NULL;
	m_lNeedleGeometricResult=M_NULL;

	m_winPadSearchRect.TopLeft().x = 0;
	m_winPadSearchRect.TopLeft().y = 0;
	m_winPadSearchRect.BottomRight().x = m_lPadBufSizeX-1;
	m_winPadSearchRect.BottomRight().y = m_lPadBufSizeY-1;

	m_winNeedleSearchRect.TopLeft().x=0;
	m_winNeedleSearchRect.TopLeft().y=0;
	m_winNeedleSearchRect.BottomRight().x=m_lNeedleBufSizeX-1;


	m_winPadModelRect.TopLeft().x = 0;
	m_winPadModelRect.TopLeft().y = 0;
	m_winPadModelRect.BottomRight().x = m_lPadBufSizeX-1;
	m_winPadModelRect.BottomRight().y = m_lPadBufSizeY-1;

	m_winNeedleModelRect.TopLeft().x=0;
	m_winNeedleModelRect.TopLeft().y=0;
	m_winNeedleModelRect.BottomRight().x=m_lNeedleBufSizeX-1;
	m_winNeedleModelRect.BottomRight().y=m_lNeedleBufSizeY-1;

	MappAllocDefault(M_SETUP,&m_MilApplication,&m_MilSystem,&m_MilDisplay,M_NULL,M_NULL);
	MbufAlloc2d(m_MilSystem,m_lBufSizeX,m_lBufSizeY,8+M_UNSIGNED,
		(m_MilDigitizer? M_IMAGE+M_DISP+M_GRAB+M_PROC : M_IMAGE+M_DISP+M_PROC),&m_MilImage);
	MbufAlloc2d(m_MilSystem,m_lPadBufSizeX,m_lPadBufSizeY,8+M_UNSIGNED,
		(m_MilDigitizer? M_IMAGE+M_DISP+M_GRAB+M_PROC : M_IMAGE+M_DISP+M_PROC),&m_lPadDetectImage);
	MbufAlloc2d(m_MilSystem,m_lNeedleBufSizeX,m_lNeedleBufSizeY,8+M_UNSIGNED,
		(m_MilDigitizer? M_IMAGE+M_DISP+M_GRAB+M_PROC : M_IMAGE+M_DISP+M_PROC),&m_lNeedleImage);
}

CMilApp::~CMilApp()
{
	if(m_MilBlobResult!=M_NULL)
	{
		MblobFree(m_MilBlobResult);
		m_MilBlobResult = M_NULL;
	}
	if(m_MilBlobFeatureList!=M_NULL)
	{
		MblobFree(m_MilBlobFeatureList);
		m_MilBlobFeatureList = M_NULL;
	}
	if(m_MilPatResultID!=M_NULL)
	{
		MpatFree(m_MilPatResultID);
		m_MilPatResultID = M_NULL;
	}
	if (m_MilNeedleResultID!=NULL)
	{
		MpatFree(m_MilNeedleResultID);
		m_MilNeedleResultID=NULL;

	}
	if(m_MilModelID!=M_NULL)
	{
		MpatFree(m_MilModelID);
		m_MilModelID = M_NULL;
	}
	if(m_GeometricResult!=M_NULL)
	{
		MmodFree(m_GeometricResult);
		m_GeometricResult = M_NULL;
	}
	if(m_GeometricModel!=M_NULL)
	{
		MmodFree(m_GeometricModel);
		m_GeometricModel = M_NULL;
	}
	if(m_MilImage!=M_NULL)
	{
		MbufFree(m_MilImage);
		m_MilImage = M_NULL;
	}
	// 颗粒检测参数析构
	if(m_lPadGrayResult!=M_NULL)
	{
		MpatFree(m_lPadGrayResult);
		m_lPadGrayResult = M_NULL;
	}
	if (m_lNeedleGrayResult!=NULL)
	{
		MpatFree(m_lNeedleGrayResult);
		m_lNeedleGrayResult=NULL;
	}
	if(m_lPadGrayModel!=M_NULL)
	{
		MpatFree(m_lPadGrayModel);
		m_lPadGrayModel = M_NULL;
	}
	if (m_lNeedleGrayModel!=NULL)
	{
		MpatFree(m_lNeedleGrayModel);
		m_lNeedleGrayModel = M_NULL;
	}
	if(m_lPadGeometricResult!=M_NULL)
	{
		MmodFree(m_lPadGeometricResult);
		m_lPadGeometricResult = M_NULL;
	}
	if (m_lNeedleGeometricResult!=NULL)
	{
		MmodFree(m_lNeedleGeometricResult);
		m_lNeedleGeometricResult = M_NULL;
	}
	if(m_lPadGeometricModel!=M_NULL)
	{
		MmodFree(m_lPadGeometricModel);
		m_lPadGeometricModel = M_NULL;
	}
	if (m_lNeedleGeometricModel!=NULL)
	{
		MmodFree(m_lNeedleGeometricModel);
		m_lNeedleGeometricModel = M_NULL;
	}
	if(m_lPadDetectImage!=M_NULL)
	{
		MbufFree(m_lPadDetectImage);
		m_lPadDetectImage = M_NULL;
	}
	if (m_lNeedleImage!=NULL)
	{
		MbufFree(m_lNeedleImage);
		m_lNeedleImage = M_NULL;
	}
	MappFreeDefault(m_MilApplication,m_MilSystem,m_MilDisplay,M_NULL,M_NULL);
}

void CMilApp::ClearOverlay()
{
	MbufClear(m_MilOverlayImage,m_lTransparentColor);
}

bool CMilApp::LearnModel(CRect &rect)
{
	if(GRAYSCALE == m_emMatchMode)
	{
		if(m_MilImage==NULL)
		{
			AfxMessageBox("没有图像!");
			return false;
		}
		if(M_NULL!=m_MilModelID)
		{
			FreeModel();
		}
		MpatAllocModel(m_MilSystem,m_MilImage,(long)rect.TopLeft().x,(long)rect.TopLeft().y,
			(long)rect.Width(),(long)rect.Height(),M_NORMALIZED,&m_MilModelID);
		if(M_NULL==m_MilModelID)
		{
			AfxMessageBox("模板学习失败!");
			return false;
		}
		m_winModelRect = rect;
	}
	else if(GEOMETRIC == m_emMatchMode)
	{
		if(m_MilImage==NULL)
		{
			AfxMessageBox("没有图像!");
			return false;
		}
		if(M_NULL!=m_GeometricModel)
		{
			FreeModel();
		}
		MmodAlloc(m_MilSystem,M_GEOMETRIC,M_DEFAULT,&m_GeometricModel);
		MmodDefine(m_GeometricModel,M_IMAGE,m_MilImage,(long)rect.TopLeft().x,(long)rect.TopLeft().y,
			(long)rect.Width(),(long)rect.Height());
		if(M_NULL==m_GeometricModel)
		{
			AfxMessageBox("模板学习失败!");
			return false;
		}
		m_winModelRect = rect;
	}
	return true;
}

bool CMilApp::FindModel(bool bAngleEnable)
{
	if(GRAYSCALE == m_emMatchMode)
	{
		if(M_NULL==m_MilModelID)
		{
			//AfxMessageBox("模板不存在!");
			return false;
		}
		if(M_NULL==m_MilPatResultID)
		{
			//MpatFree(m_MilPatResultID);
			//m_MilPatResultID = M_NULL;
			m_MilPatResultID = MpatAllocResult(m_MilSystem,1,M_NULL);
		}
		PreProcessModel(true,bAngleEnable);
		MpatFindModel(m_MilImage,m_MilModelID,m_MilPatResultID);
		MpatGetNumber(m_MilPatResultID,&m_stResult.nResultNum);
		if(m_stResult.nResultNum<=0)
		{
			//AfxMessageBox("查找失败!");
			return false;
		}
		else
		{
			MpatGetResult(m_MilPatResultID,M_POSITION_X,m_stResult.dResultCenterX);
			MpatGetResult(m_MilPatResultID,M_POSITION_Y,m_stResult.dResultCenterY);
			MpatGetResult(m_MilPatResultID,M_ANGLE,m_stResult.dResultAngle);
			MpatGetResult(m_MilPatResultID,M_SCORE,m_stResult.dResultScore);
		}
		if (!IsInRoiZone(m_stResult.dResultCenterX[0],m_stResult.dResultCenterY[0]))
		{
			return false;
		}
	}
	else if(GEOMETRIC == m_emMatchMode)
	{
		if(M_NULL == m_GeometricModel)
		{
			AfxMessageBox("模板不存在!");
			return false;
		}
		if(M_NULL == m_GeometricResult)
		{
			//MpatFree(m_MilPatResultID);
			//m_MilPatResultID = M_NULL;
			m_GeometricResult = MmodAllocResult(m_MilSystem,M_DEFAULT,M_NULL);
		}
		PreProcessModel(true,bAngleEnable);
		MmodFind(m_GeometricModel,m_MilImage,m_GeometricResult);
		MmodGetResult(m_GeometricResult,M_DEFAULT,M_NUMBER+M_TYPE_LONG,&m_stResult.nResultNum);
		if(m_stResult.nResultNum<=0)
		{
			AfxMessageBox("查找失败!");
			return false;
		}
		else
		{
			MmodGetResult(m_GeometricResult,M_DEFAULT,M_POSITION_X,m_stResult.dResultCenterX);
			MmodGetResult(m_GeometricResult,M_DEFAULT,M_POSITION_Y,m_stResult.dResultCenterY);
			MmodGetResult(m_GeometricResult,M_DEFAULT,M_ANGLE,m_stResult.dResultAngle);
			MmodGetResult(m_GeometricResult,M_DEFAULT,M_SCORE,m_stResult.dResultScore);
		}
		if (!IsInRoiZone(m_stResult.dResultCenterX[0],m_stResult.dResultCenterY[0]))
		{
			return false;
		}
	}
	return true;
}

bool CMilApp::FindModels()
{
	if(GRAYSCALE == m_emMatchMode)
	{
		if(M_NULL==m_MilModelID)
		{
			//AfxMessageBox("模板不存在!");
			return false;
		}
		if(M_NULL==m_MilPatResultID)
		{
			//MpatFree(m_MilPatResultID);
			//m_MilPatResultID = M_NULL;
			m_MilPatResultID = MpatAllocResult(m_MilSystem,M_DEFAULT,M_NULL);
		}
		PreProcessModel(false,true);
		MpatFindModel(m_MilImage,m_MilModelID,m_MilPatResultID);
		MpatGetNumber(m_MilPatResultID,&m_stResult.nResultNum);
		if(m_stResult.nResultNum>0)
		{
			MpatGetResult(m_MilPatResultID,M_POSITION_X,m_stResult.dResultCenterX);
			MpatGetResult(m_MilPatResultID,M_POSITION_Y,m_stResult.dResultCenterY);
			MpatGetResult(m_MilPatResultID,M_ANGLE,m_stResult.dResultAngle);
			MpatGetResult(m_MilPatResultID,M_SCORE,m_stResult.dResultScore);
		}
	}
	else if(GEOMETRIC == m_emMatchMode)
	{
		if(M_NULL == m_GeometricModel)
		{
			//AfxMessageBox("模板不存在!");
			return false;
		}
		if(M_NULL == m_GeometricResult)
		{
			//MpatFree(m_MilPatResultID);
			//m_MilPatResultID = M_NULL;
			m_GeometricResult = MmodAllocResult(m_MilSystem,M_DEFAULT,M_NULL);
		}
		PreProcessModel(false,true);
		MmodFind(m_GeometricModel,m_MilImage,m_GeometricResult);
		MmodGetResult(m_GeometricResult,M_DEFAULT,M_NUMBER+M_TYPE_LONG,&m_stResult.nResultNum);
		if(m_stResult.nResultNum>0)
		{
			MmodGetResult(m_GeometricResult,M_DEFAULT,M_POSITION_X,m_stResult.dResultCenterX);
			MmodGetResult(m_GeometricResult,M_DEFAULT,M_POSITION_Y,m_stResult.dResultCenterY);
			MmodGetResult(m_GeometricResult,M_DEFAULT,M_ANGLE,m_stResult.dResultAngle);
			MmodGetResult(m_GeometricResult,M_DEFAULT,M_SCORE,m_stResult.dResultScore);
		}
	}
	return true;
}

void CMilApp::FreeModel()
{
	if(GRAYSCALE == m_emMatchMode)
	{
		if(M_NULL!=m_MilModelID)
		{
			MpatFree(m_MilModelID);
		}
		m_MilModelID = M_NULL;
	}
	else if(GEOMETRIC == m_emMatchMode)
	{
		if(M_NULL!=m_GeometricModel)
		{
			MmodFree(m_GeometricModel);
		}
		m_GeometricModel = M_NULL;
	}
}

bool CMilApp::RestoreModel(char *szFilePath)
{
	CFileFind fileFind;
	if(!fileFind.FindFile(szFilePath))
	{
		CString str;
		str.Format("FileName:%s",szFilePath);
		g_pFrm->m_CmdRun.AddMsg(str);
		return false;
	}
	if(GRAYSCALE == m_emMatchMode)
	{
		if(M_NULL!=m_MilModelID)
		{
			FreeModel();
		}
		m_MilModelID = MpatRestore(m_MilSystem,szFilePath,M_NULL);
	}
	else if(GEOMETRIC == m_emMatchMode)
	{
		if(M_NULL!=m_GeometricModel)
		{
			FreeModel();
		}
		m_GeometricModel = MmodRestore(szFilePath,m_MilSystem,M_DEFAULT,M_NULL);
	}
	return true;
}

bool CMilApp::PreProcessModel(bool bSingle,bool bAngleEnable)
{
	if(GRAYSCALE == m_emMatchMode)
	{
		if(m_MilModelID==M_NULL)
		{
			return false;
		}
		if(bSingle)
		{
			MpatSetNumber(m_MilModelID,1);
		}
		else
		{
			MpatSetNumber(m_MilModelID,1000);
		}
		MpatSetAccuracy(m_MilModelID,M_HIGH);
		MpatSetSpeed(m_MilModelID,M_HIGH);
		MpatSetCertainty(m_MilModelID,100.0);
		MpatSetAcceptance(m_MilModelID,m_stParam.dParamScore);

		if(bAngleEnable)
		{
			MpatSetAngle(m_MilModelID,M_SEARCH_ANGLE_MODE,M_ENABLE);
			MpatSetAngle(m_MilModelID,M_SEARCH_ANGLE_ACCURACY,0.5);
			MpatSetAngle(m_MilModelID,M_SEARCH_ANGLE_DELTA_POS,m_stParam.dParamAngle/2);
			MpatSetAngle(m_MilModelID,M_SEARCH_ANGLE_DELTA_NEG,m_stParam.dParamAngle/2);
			MpatSetAngle(m_MilModelID,M_SEARCH_ANGLE,0.0);
		}

		MpatSetPosition(m_MilModelID,(long)m_winSearchRect.TopLeft().x,(long)m_winSearchRect.TopLeft().y,
			(long)m_winSearchRect.Width(),(long)m_winSearchRect.Height());

		MpatPreprocModel(M_NULL,m_MilModelID,M_DEFAULT);
	}
	else if(GEOMETRIC == m_emMatchMode)
	{
		if(m_GeometricModel == M_NULL)
		{
			return false;
		}
		if(bSingle)
		{
			MmodControl(m_GeometricModel,M_ALL,M_NUMBER,1);
		}
		else
		{
			MmodControl(m_GeometricModel,M_ALL,M_NUMBER,1000);
		}
		MmodControl(m_GeometricModel,M_CONTEXT,M_SPEED,M_HIGH);
		MmodControl(m_GeometricModel,M_CONTEXT,M_ACCURACY,M_HIGH);
		MmodControl(m_GeometricModel,M_DEFAULT,M_ACCEPTANCE,m_stParam.dParamScore);

		if(bAngleEnable)
		{
			MmodControl(m_GeometricModel,M_CONTEXT,M_SEARCH_ANGLE_RANGE,M_ENABLE);
			MmodControl(m_GeometricModel,M_DEFAULT,M_SEARCH_ANGLE_DELTA_POS,m_stParam.dParamAngle/2);
			MmodControl(m_GeometricModel,M_DEFAULT,M_SEARCH_ANGLE_DELTA_NEG,m_stParam.dParamAngle/2);
		}

		MmodControl(m_GeometricModel,M_CONTEXT,M_SEARCH_POSITION_RANGE,M_ENABLE);
		MmodControl(m_GeometricModel,M_ALL,M_POSITION_X,m_winSearchRect.CenterPoint().x);
		MmodControl(m_GeometricModel,M_ALL,M_POSITION_Y,m_winSearchRect.CenterPoint().y);
		MmodControl(m_GeometricModel,M_DEFAULT,M_POSITION_DELTA_NEG_X,m_winSearchRect.CenterPoint().x-m_winSearchRect.TopLeft().x);
		MmodControl(m_GeometricModel,M_DEFAULT,M_POSITION_DELTA_NEG_Y,m_winSearchRect.CenterPoint().y-m_winSearchRect.TopLeft().y);
		MmodControl(m_GeometricModel,M_DEFAULT,M_POSITION_DELTA_POS_X,m_winSearchRect.BottomRight().x-m_winSearchRect.CenterPoint().x);
		MmodControl(m_GeometricModel,M_DEFAULT,M_POSITION_DELTA_POS_Y,m_winSearchRect.BottomRight().y-m_winSearchRect.CenterPoint().y);

		MmodPreprocess(m_GeometricModel,M_DEFAULT);
	}
	return true;
}

bool CMilApp::SaveModel(char *szFilePath)
{
	if(GRAYSCALE == m_emMatchMode)
	{
		if(M_NULL!=m_MilModelID)
		{
			MpatSave(szFilePath,m_MilModelID);
		}
	}
	else if(GEOMETRIC == m_emMatchMode)
	{
		if(M_NULL!=m_GeometricModel)
		{
			MmodSave(szFilePath,m_GeometricModel,M_DEFAULT);
		}
	}
	return true;
}

void CMilApp::SetPatParam(double dScore, double dAngle)
{
	m_stParam.dParamScore = dScore;
	m_stParam.dParamAngle = dAngle;
}

void CMilApp::SetSearchWindow(CRect &rect)
{
	m_winSearchRect = rect;
}

void CMilApp::SetModelWindow(CRect &rect)
{
	m_winModelRect = rect;
}

void CMilApp::SetMatchMode(MatchMode mode)
{
	m_emMatchMode = mode;
}

void CMilApp::SetPadSearchWindow(CRect &rect)
{
	m_winPadSearchRect = rect;
}

void CMilApp::SetPadModelWindow(CRect &rect)
{
	m_winPadModelRect = rect;
}

void CMilApp::SetPadSearchParam(double dScore,double dAngle)
{
	m_stPadParam.dParamScore = dScore;
	m_stPadParam.dParamAngle = dAngle;
}

bool CMilApp::SavePadModel(char *szFilePath)
{
	if(GRAYSCALE == m_emMatchMode)
	{
		if(M_NULL!=m_lPadGrayModel)
		{
			MpatSave(szFilePath,m_lPadGrayModel);
		}
	}
	else if(GEOMETRIC == m_emMatchMode)
	{
		if(M_NULL!=m_lPadGeometricModel)
		{
			MmodSave(szFilePath,m_lPadGeometricModel,M_DEFAULT);
		}
	}
	return true;
}

bool CMilApp::RestorePadModel(char *szFilePath)
{
	CString str;
	CFileFind fileFind;
	if(!fileFind.FindFile(szFilePath))
	{
		str.Format("FileName:%s",szFilePath);
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		return false;
	}
	if(GRAYSCALE == m_emMatchMode)
	{
		if(M_NULL!=m_lPadGrayModel)
		{
			FreePadModel();
		}
		m_lPadGrayModel = MpatRestore(m_MilSystem,szFilePath,M_NULL);
	}
	else if(GEOMETRIC == m_emMatchMode)
	{
		if(M_NULL!=m_lPadGeometricModel)
		{
			FreePadModel();
		}
		m_lPadGeometricModel = MmodRestore(szFilePath,m_MilSystem,M_DEFAULT,M_NULL);
	}
	return true;
}

bool CMilApp::LearnPadModel(CRect &rect)
{
	if(GRAYSCALE == m_emMatchMode)
	{
		if(m_lPadDetectImage==NULL)
		{
			AfxMessageBox("没有图像!");
			return false;
		}
		if(M_NULL!=m_lPadGrayModel)
		{
			FreePadModel();
		}
		MpatAllocModel(m_MilSystem,m_lPadDetectImage,(long)rect.TopLeft().x,(long)rect.TopLeft().y,
			(long)rect.Width(),(long)rect.Height(),M_NORMALIZED,&m_lPadGrayModel);
		if(M_NULL==m_lPadGrayModel)
		{
			AfxMessageBox("模板学习失败!");
			return false;
		}
		m_winPadModelRect = rect;
	}
	else if(GEOMETRIC == m_emMatchMode)
	{
		if(m_lPadDetectImage==NULL)
		{
			AfxMessageBox("没有图像!");
			return false;
		}
		if(M_NULL!=m_lPadGeometricModel)
		{
			FreePadModel();
		}
		MmodAlloc(m_MilSystem,M_GEOMETRIC,M_DEFAULT,&m_lPadGeometricModel);
		MmodDefine(m_lPadGeometricModel,M_IMAGE,m_lPadDetectImage,(long)rect.TopLeft().x,(long)rect.TopLeft().y,
			(long)rect.Width(),(long)rect.Height());
		if(M_NULL==m_lPadGeometricModel)
		{
			AfxMessageBox("模板学习失败!");
			return false;
		}
		m_winPadModelRect = rect;
	}
	return true;
}

bool CMilApp::LearnNeedleModel(CRect &rect)
{
	if(GRAYSCALE == m_emMatchMode)
	{
		if(m_lNeedleImage==NULL)
		{
			AfxMessageBox("没有图像!");
			return false;
		}
		if(M_NULL!=m_lNeedleGrayModel)
		{
			FreePadModel();
		}
		MpatAllocModel(m_MilSystem,m_lNeedleImage,(long)rect.TopLeft().x,(long)rect.TopLeft().y,
			(long)rect.Width(),(long)rect.Height(),M_NORMALIZED,&m_lNeedleGrayModel);
		if(M_NULL==m_lNeedleGrayModel)
		{
			AfxMessageBox("模板学习失败!");
			return false;
		}
		m_winNeedleModelRect = rect;
	}
	else if(GEOMETRIC == m_emMatchMode)
	{
		if(m_lNeedleImage==NULL)
		{
			AfxMessageBox("没有图像!");
			return false;
		}
		if(M_NULL!=m_lNeedleGeometricModel)
		{
			FreePadModel();
		}
		MmodAlloc(m_MilSystem,M_GEOMETRIC,M_DEFAULT,&m_lNeedleGeometricModel);
		MmodDefine(m_lNeedleGeometricModel,M_IMAGE,m_lNeedleImage,(long)rect.TopLeft().x,(long)rect.TopLeft().y,
			(long)rect.Width(),(long)rect.Height());
		if(M_NULL==m_lNeedleGeometricModel)
		{
			AfxMessageBox("模板学习失败!");
			return false;
		}
		m_winNeedleModelRect = rect;
	}
	return true;
}


bool CMilApp::PreProcessPadModel(bool bSingle)
{
	CString strText;
	if(GRAYSCALE == m_emMatchMode)
	{
		if(m_lPadGrayModel==M_NULL)
		{
			return false;
		}
		if(bSingle)
		{
			MpatSetNumber(m_lPadGrayModel,1);
		}
		else
		{
			MpatSetNumber(m_lPadGrayModel,1000);
		}
		MpatSetAccuracy(m_lPadGrayModel,M_HIGH);
		MpatSetSpeed(m_lPadGrayModel,M_HIGH);
		MpatSetCertainty(m_lPadGrayModel,100.0);
		MpatSetAcceptance(m_lPadGrayModel,m_stPadParam.dParamScore);

		MpatSetAngle(m_lPadGrayModel,M_SEARCH_ANGLE_MODE,M_ENABLE);
		MpatSetAngle(m_lPadGrayModel,M_SEARCH_ANGLE_ACCURACY,0.5);
		MpatSetAngle(m_lPadGrayModel,M_SEARCH_ANGLE_DELTA_POS,m_stPadParam.dParamAngle/2);
		MpatSetAngle(m_lPadGrayModel,M_SEARCH_ANGLE_DELTA_NEG,m_stPadParam.dParamAngle/2);
		MpatSetAngle(m_lPadGrayModel,M_SEARCH_ANGLE,0.0);

		MpatSetPosition(m_lPadGrayModel,(long)m_winPadSearchRect.TopLeft().x,(long)m_winPadSearchRect.TopLeft().y,
			(long)m_winPadSearchRect.Width(),(long)m_winPadSearchRect.Height());

		MpatPreprocModel(M_NULL,m_lPadGrayModel,M_DEFAULT);
	}
	else if(GEOMETRIC == m_emMatchMode)
	{
		if(m_lPadGeometricModel == M_NULL)
		{
			return false;
		}
		if(bSingle)
		{
			MmodControl(m_lPadGeometricModel,M_ALL,M_NUMBER,1);
			strText.Format("Number = 1");
			g_pFrm->m_CmdRun.AddMsg(strText);
		}
		else
		{
			MmodControl(m_lPadGeometricModel,M_ALL,M_NUMBER,1000);
			strText.Format("Number = 1000");
			g_pFrm->m_CmdRun.AddMsg(strText);
		}
		MmodControl(m_lPadGeometricModel,M_CONTEXT,M_SPEED,M_HIGH);
		strText.Format("Speed = High");
		g_pFrm->m_CmdRun.AddMsg(strText);
		MmodControl(m_lPadGeometricModel,M_CONTEXT,M_ACCURACY,M_HIGH);
		strText.Format("Accuracy = High");
		g_pFrm->m_CmdRun.AddMsg(strText);
		MmodControl(m_lPadGeometricModel,M_DEFAULT,M_ACCEPTANCE,m_stPadParam.dParamScore);
		strText.Format("Acceptance = %0.1f",m_stPadParam.dParamScore);
		g_pFrm->m_CmdRun.AddMsg(strText);

		MmodControl(m_lPadGeometricModel,M_CONTEXT,M_SEARCH_ANGLE_RANGE,M_ENABLE);
		strText.Format("AngleRange = 1");
		g_pFrm->m_CmdRun.AddMsg(strText);
		MmodControl(m_lPadGeometricModel,M_DEFAULT,M_SEARCH_ANGLE_DELTA_POS,m_stPadParam.dParamAngle/2);
		strText.Format("AngleDeltaPos = %0.1f",m_stPadParam.dParamAngle/2);
		g_pFrm->m_CmdRun.AddMsg(strText);
		MmodControl(m_lPadGeometricModel,M_DEFAULT,M_SEARCH_ANGLE_DELTA_NEG,m_stPadParam.dParamAngle/2);
		strText.Format("AngleDeltaNeg = %0.1f",m_stPadParam.dParamAngle/2);
		g_pFrm->m_CmdRun.AddMsg(strText);

		MmodControl(m_lPadGeometricModel,M_CONTEXT,M_SEARCH_POSITION_RANGE,M_ENABLE);
		strText.Format("PosRange = 1");
		g_pFrm->m_CmdRun.AddMsg(strText);
		MmodControl(m_lPadGeometricModel,M_ALL,M_POSITION_X,m_winPadSearchRect.CenterPoint().x);
		strText.Format("PosX = %d",m_winPadSearchRect.CenterPoint().x);
		g_pFrm->m_CmdRun.AddMsg(strText);
		MmodControl(m_lPadGeometricModel,M_ALL,M_POSITION_Y,m_winPadSearchRect.CenterPoint().y);
		strText.Format("PosY = %d",m_winPadSearchRect.CenterPoint().y);
		g_pFrm->m_CmdRun.AddMsg(strText);
		strText.Format("Left = %ld,right = %ld,top = %ld,bottom = %ld,centerX = %ld,centerY=%ld",m_winPadSearchRect.TopLeft().x,m_winPadSearchRect.BottomRight().x,m_winPadSearchRect.TopLeft().y,
			m_winPadSearchRect.BottomRight().y,m_winPadSearchRect.CenterPoint().x,m_winPadSearchRect.CenterPoint().y);
		g_pFrm->m_CmdRun.AddMsg(strText);
		strText.Format("PosDeltaNegX = %d,PosDeltaNegY = %d,PosDeltaPosX = %d,PosDeltaPosY = %d",m_winPadSearchRect.CenterPoint().x-m_winPadSearchRect.TopLeft().x,
			m_winPadSearchRect.CenterPoint().y-m_winPadSearchRect.TopLeft().y,m_winPadSearchRect.BottomRight().x-m_winPadSearchRect.CenterPoint().x,m_winPadSearchRect.BottomRight().y-m_winPadSearchRect.CenterPoint().y);
		g_pFrm->m_CmdRun.AddMsg(strText);
		MmodControl(m_lPadGeometricModel,M_DEFAULT,M_POSITION_DELTA_NEG_X,m_winPadSearchRect.CenterPoint().x-m_winPadSearchRect.TopLeft().x);
		strText.Format("PosDeltaNegX = %d",m_winPadSearchRect.CenterPoint().x-m_winPadSearchRect.TopLeft().x);
		g_pFrm->m_CmdRun.AddMsg(strText);
		MmodControl(m_lPadGeometricModel,M_DEFAULT,M_POSITION_DELTA_NEG_Y,m_winPadSearchRect.CenterPoint().y-m_winPadSearchRect.TopLeft().y);
		strText.Format("PosDeltaNegY = %d",m_winPadSearchRect.CenterPoint().y-m_winPadSearchRect.TopLeft().y);
		g_pFrm->m_CmdRun.AddMsg(strText);
		MmodControl(m_lPadGeometricModel,M_DEFAULT,M_POSITION_DELTA_POS_X,m_winPadSearchRect.BottomRight().x-m_winPadSearchRect.CenterPoint().x);
		strText.Format("PosDeltaPosX = %d",m_winPadSearchRect.BottomRight().x-m_winPadSearchRect.CenterPoint().x);
		g_pFrm->m_CmdRun.AddMsg(strText);
		MmodControl(m_lPadGeometricModel,M_DEFAULT,M_POSITION_DELTA_POS_Y,m_winPadSearchRect.BottomRight().y-m_winPadSearchRect.CenterPoint().y);
		strText.Format("PosDeltaPosY = %d",m_winPadSearchRect.BottomRight().y-m_winPadSearchRect.CenterPoint().y);
		g_pFrm->m_CmdRun.AddMsg(strText);

		MmodPreprocess(m_lPadGeometricModel,M_DEFAULT);
	}
	return true;
}

bool CMilApp::FindPadModels(bool bSingle)
{
	CString strInfo;
	if(GRAYSCALE == m_emMatchMode)
	{
		if(M_NULL==m_lPadGrayModel)
		{
			//AfxMessageBox("模板不存在!");
			strInfo.Format("[FindPadModels]模板不存在...");
			g_pFrm->m_CmdRun.AddMsg(strInfo);
			return false;
		}
		if(M_NULL==m_lPadGrayResult)
		{
			//MpatFree(m_MilPatResultID);
			//m_MilPatResultID = M_NULL;
			m_lPadGrayResult = MpatAllocResult(m_MilSystem,M_DEFAULT,M_NULL);
		}
		PreProcessPadModel(bSingle);
		MpatFindModel(m_lPadDetectImage,m_lPadGrayModel,m_lPadGrayResult);
		MpatGetNumber(m_lPadGrayResult,&m_stPadResult.nResultNum);
		if(m_stPadResult.nResultNum<0||m_stPadResult.nResultNum>SEARCH_MAX_NUM) //2017-04-11 by zwg(&&->||)
		{
			strInfo.Format("[FindPadModels]数目：%d...",m_stPadResult.nResultNum);
			g_pFrm->m_CmdRun.AddMsg(strInfo);
			return false;
		}
		if(m_stPadResult.nResultNum>0&&m_stPadResult.nResultNum<=SEARCH_MAX_NUM)
		{
			MpatGetResult(m_lPadGrayResult,M_POSITION_X,m_stPadResult.dResultCenterX);
			MpatGetResult(m_lPadGrayResult,M_POSITION_Y,m_stPadResult.dResultCenterY);
			MpatGetResult(m_lPadGrayResult,M_ANGLE,m_stPadResult.dResultAngle);
			MpatGetResult(m_lPadGrayResult,M_SCORE,m_stPadResult.dResultScore);
		}
	}
	else if(GEOMETRIC == m_emMatchMode)
	{
		if(M_NULL == m_lPadGeometricModel)
		{
			//AfxMessageBox("模板不存在!");
			return false;
		}
		if(M_NULL == m_lPadGeometricResult)
		{
			//MpatFree(m_MilPatResultID);
			//m_MilPatResultID = M_NULL;
			m_lPadGeometricResult = MmodAllocResult(m_MilSystem,M_DEFAULT,M_NULL);
		}
		PreProcessPadModel(bSingle);
		MmodFind(m_lPadGeometricModel,m_lPadDetectImage,m_lPadGeometricResult);
		MmodGetResult(m_lPadGeometricResult,M_DEFAULT,M_NUMBER+M_TYPE_LONG,&m_stPadResult.nResultNum);
		if(m_stPadResult.nResultNum>0||m_stPadResult.nResultNum<=SEARCH_MAX_NUM)
		{
			MmodGetResult(m_lPadGeometricResult,M_DEFAULT,M_POSITION_X,m_stPadResult.dResultCenterX);
			MmodGetResult(m_lPadGeometricResult,M_DEFAULT,M_POSITION_Y,m_stPadResult.dResultCenterY);
			MmodGetResult(m_lPadGeometricResult,M_DEFAULT,M_ANGLE,m_stPadResult.dResultAngle);
			MmodGetResult(m_lPadGeometricResult,M_DEFAULT,M_SCORE,m_stPadResult.dResultScore);
		}
	}
	return true;
}

void CMilApp::FreePadModel()
{
	if(GRAYSCALE == m_emMatchMode)
	{
		if(M_NULL!=m_lPadGrayModel)
		{
			MpatFree(m_lPadGrayModel);
		}
		m_lPadGrayModel = M_NULL;
	}
	else if(GEOMETRIC == m_emMatchMode)
	{
		if(M_NULL!=m_lPadGeometricModel)
		{
			MmodFree(m_lPadGeometricModel);
		}
		m_lPadGeometricModel = M_NULL;
	}
}

bool CMilApp::OpenFile(char *szFilePath,bool bDetectImage)
{
	if(bDetectImage)
	{
		MbufImport(szFilePath,M_DEFAULT,M_LOAD,m_MilSystem,&m_lPadDetectImage);
	}
	else
	{
		MbufImport(szFilePath,M_DEFAULT,M_LOAD,m_MilSystem,&m_MilImage);
	}
	return true;
}

bool CMilApp::SaveFile(char *szFilePath,bool bDetectImage)
{
	if(bDetectImage)
	{
		MbufExport(szFilePath,M_BMP,m_lPadDetectImage);
	}
	else
	{
		MbufExport(szFilePath,M_BMP,m_MilImage);
	}
	return true;
}

bool CMilApp::GetDetectBuffer(BYTE *pBuffer)
{
	MbufGet(m_lPadDetectImage,pBuffer);
	return true;
}
bool CMilApp::IsInRoiZone(double x,double y)
{
	if ((m_winModelRect.Width()>m_winSearchRect.Width())||(m_winModelRect.Height()>m_winSearchRect.Height()))
	{
		CString str;
		str.Format("模板框与ROI框尺寸关系异常！");
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		return false;
	}
	double dWidth=fabs((double)((m_winSearchRect.Width()-m_winModelRect.Width())/2));
	double dHeight=fabs((double)((m_winSearchRect.Height()-m_winModelRect.Height())/2));
	double dCenterX=(m_winSearchRect.left+m_winSearchRect.right)/2;
	double dCenterY=(m_winSearchRect.top+m_winSearchRect.bottom)/2;
	return ((fabs(dCenterX-x)<=dWidth)&&(fabs(dCenterY-y)<=dHeight));
}