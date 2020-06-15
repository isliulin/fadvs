//#if !defined(AFX_MILINIAPP_H__3384A548_D21B_11D7_97FA_00B0D08F9CDC__INCLUDED_)
//#define AFX_MILINIAPP_H__3384A548_D21B_11D7_97FA_00B0D08F9CDC__INCLUDED_
//
//#if _MSC_VER > 1000
#pragma once
//#endif // _MSC_VER > 1000
#include "Mil.h"
#include "paramdef.h"
#define IMAGE_THRESHOLD_VALUE 26L 

/* Minimum and maximum area of blobs. */
#define MIN_BLOB_AREA         50L 
#define MAX_BLOB_AREA         50000L

/* Radius of the smallest particles to keep. */
#define MIN_BLOB_RADIUS       3L

/* Minimum hole compactness corresponding to a washer. */
#define MIN_COMPACTNESS       1.5

#define RGB24 1
#define RGB8 2

#define	SEARCH_MAX_NUM	1000
#define	BLOB_MAX_NUM	30

struct M_BLOB
{
	int m_nThreshold;
	long m_lOpen;
	long m_lClose;
	double m_dMinArea;
	double m_dMaxArea;
	bool m_bFindBlack;
};
struct M_BLOBResult
{
	int nBlobNum;
	double dBlobCenterX[BLOB_MAX_NUM];
	double dBlobCenterY[BLOB_MAX_NUM];
	double dBlobXMin[BLOB_MAX_NUM];
	double dBlobXMax[BLOB_MAX_NUM];
	double dBlobYMin[BLOB_MAX_NUM];
	double dBlobYMax[BLOB_MAX_NUM];
	double dBlobLength[BLOB_MAX_NUM];
	double dBlobWidth[BLOB_MAX_NUM];
	double dBolbArea[BLOB_MAX_NUM];
};
struct SEARCH_PARAM
{
	double dParamScore;
	double dParamAngle;
};
struct SEARCH_RESULT
{
	long nResultNum;
	double dResultCenterX[SEARCH_MAX_NUM];
	double dResultCenterY[SEARCH_MAX_NUM];
	double dResultScore[SEARCH_MAX_NUM];
	double dResultAngle[SEARCH_MAX_NUM];
};
struct COLORTABLE
{
	int nRed;
	int nGreen;
	int nBlue;
	int nYellow;
	int nMagenta;
	int nCyan;
};

class CMilApp
{
public:
	M_BLOB MilBlob;
	M_BLOBResult BlobResult;
	SEARCH_PARAM m_stParam;
	SEARCH_RESULT m_stResult;
	COLORTABLE ColorTable;

	long m_lBufSizeX;
	long m_lBufSizeY;
	long m_lSubBufSizeX;
	long m_lSubBufSizeY;
	int m_nDigitizer;
	long m_lBufSizeBand;
	long m_lChannel;
	long m_lCurrentChannel;
	long m_lTransparentColor;
	int m_nImageType;
	int m_nZoomValue;

	long m_lTotalBlobs;
	long m_lBlobsHoles;
	long m_lBlobsRoughHoles;
	long m_lImageSizeX;
	long m_lImageSizeY;

	MIL_ID m_MilImage;
	MIL_ID m_MilBinImage;
	MIL_ID m_MilDisplay;
	MIL_ID m_MilApplication;
	MIL_ID m_MilDigitizer;
	MIL_ID m_MilSystem;
	MIL_ID m_SubImage;
	MIL_ID m_MilOverlayImage;
	MIL_ID m_MilBlobResult;
	MIL_ID m_MilBlobFeatureList;
	MIL_ID m_FeatureList;
	MIL_ID m_BlobResult;

	MIL_ID m_MilModelID;
	MIL_ID m_MilPatResultID;
	MIL_ID m_MilNeedleResultID;

	CRect m_winSearchRect;
	CRect m_winModelRect;

	MatchMode m_emMatchMode;

	// 几何匹配
	MIL_ID m_GeometricModel;
	MIL_ID m_GeometricResult;

	// 颗粒检测
	long m_lPadBufSizeX;
	long m_lPadBufSizeY;
	MIL_ID m_lPadDetectImage;
	MIL_ID m_lPadGrayModel;
	MIL_ID m_lPadGrayResult;
	MIL_ID m_lPadGeometricModel;
	MIL_ID m_lPadGeometricResult;
	CRect m_winPadSearchRect;
	CRect m_winPadModelRect;
	SEARCH_PARAM m_stPadParam;
	SEARCH_RESULT m_stPadResult;

	//自动对针头用
	long m_lNeedleBufSizeX;
	long m_lNeedleBufSizeY;
	MIL_ID m_lNeedleImage;
	MIL_ID m_lNeedleGrayModel;
	MIL_ID m_lNeedleGrayResult;
	MIL_ID m_lNeedleGeometricModel;
	MIL_ID m_lNeedleGeometricResult;
	CRect m_winNeedleSearchRect;
	CRect m_winNeedleModelRect;
	SEARCH_PARAM m_stNeedleParam;
	SEARCH_RESULT m_stNeedleResult;


	CMilApp();
	~CMilApp();
	void OverlayDrawText(int nColor,long lOffsetX,long lOffsetY,char *pText);
	void OverlayRect(int nColor,CPoint startPoint,CPoint endPoint,bool bErase=true);
	void OverlayLine(int nColor,CPoint startPoint,CPoint endPoint);
	void ClearOverlay();
	void InitOverlay();
	void DigGrab(CString fileName);
	bool FindBlob(CRect ROIB);
	void CloseImage();
	void DisSelectWindow(HWND hWnd,int nZoomFactor=1);
	void SetBlobParam(int nThreshold,double dMinArea,double dMaxArea,long lOpen,long lClose,bool bFindBlack);
	void SelectShowWindow(HWND hWnd,MIL_ID image);
	void FreeOverlay(MIL_ID MilMain);
	void ClearImageBuffer(MIL_ID image,int nColor);
	void CopyBuffer(MIL_ID buffer,BYTE *pData,int type);
	void CopyBuffer(MIL_ID buffer,BYTE *pData,long lWidth,long lHeight,int type);
	bool FindBlob(CRect ROIB,long *pResultNum,double *pCogX,double *pCogY);

	bool LearnModel(CRect &rect);
	bool FindModel(bool bAngleEnable);
	bool FindModels();
	bool RestoreModel(char *szFilePath);
	bool PreProcessModel(bool bSingle,bool bAngleEnable);
	bool SaveModel(char *szFilePath);
	void FreeModel();
	void SetPatParam(double dScore,double dAngle);
	void SetSearchWindow(CRect &rect);
	void SetModelWindow(CRect &rect);
	void SetMatchMode(MatchMode mode);
	//
	void SetPadSearchWindow(CRect &rect);
	void SetPadModelWindow(CRect &rect);
	void SetPadSearchParam(double dScore,double dAngle);
	bool LearnPadModel(CRect &rect);
	bool LearnNeedleModel(CRect &rect);
	bool SavePadModel(char *szFilePath);
	bool RestorePadModel(char *szFilePath);
	bool PreProcessPadModel(bool bSingle);
	bool FindPadModels(bool bSingle = false);
	void FreePadModel();
	bool OpenFile(char *szFilePath,bool bDetectImage);
	bool SaveFile(char *szFilePath,bool bDetectImage);
	bool GetDetectBuffer(BYTE *pBuffer);
	bool IsInRoiZone(double x,double y);
};

//#endif // !defined(AFX_MILINIAPP_H__3384A548_D21B_11D7_97FA_00B0D08F9CDC__INCLUDED_)