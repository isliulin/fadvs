#pragma once

#include <vfw.h>
//#include "PGCamera.h"
#include "KShvdh.h"
#include "CarmAVT.h"
#include "PointGreyCam.h" //2017-12-12
#include "Ts_DhCamera.h"
//#include "BaslerCamera.h"

//MODEL:HV1351UM
//#include "RedrawTrack.h"
#define IMAGEHEIGHT0	512
#define IMAGEWIDTH0		640

//#define IMAGEHEIGHT1 	1944
//#define IMAGEWIDTH1 	2592

#define IMAGEHEIGHT1 	640
#define IMAGEWIDTH1 	1912

#define IMAGEHEIGHT2	512 //1024 2048
#define IMAGEWIDTH2		640  //1280 3072

#define OFFSETX         440
#define OFFSETY         684
// CImgStatic
struct tgImgPoint
{
	tgImgPoint(){x=y=0;}
	tgImgPoint(int ix, int iy){x=ix; y=iy;}
	int x, y;
};
typedef CRect tgImgRect;

enum MK_MOUSE_EVENT
{
	MK_MOUSE_NONE,
	MK_MOUSE_ZOOM,
	MK_MOUSE_MOVE
};
class CImgStatic : public CStatic
{
	DECLARE_DYNAMIC(CImgStatic)
public:
	CRect m_rectImgRect,m_rectSelRect;
	tgImgPoint m_ImgPoint,m_PatPos;
	int m_nImgWidth, m_nImgHeight,m_nImgWidthNeedle,m_nImgHeightNeedle;
	long m_lImgDetectWidth,m_lImgDetectHeight;
	CPoint m_ptCurrent;
	BOOL m_bImgInit;
	BOOL m_bImgInitNeedle;
	HDRAWDIB m_hDrawDib;
	double m_dScale;
	CPoint m_ptTrackDrawOffset;
	bool m_bDetectInit;

	CDHCamera m_pCamera;
	CDHCamera m_pCameraNeedle;
	//CPGCamera *m_pDetectCamera;//Pointer of the Pad Detected cemera;
	Ts_DhCamera *m_pDetectCamera;
    PointGreyCam * m_pPointGreyCam;  //2017-12-12  

	BYTE *m_pDetectImageBuffer;  //图像显示buffer
	CRITICAL_SECTION m_csDetectCamera;
	int m_nDetectPixel;
	int m_nImgNeedlePixel;
	double m_dPatAcceptance;

	BITMAPINFO *m_pBitmapInfo;
	int m_nOutSizeX,m_nOutSizeY;	
	double m_dDisplayCenterX[4];
	double m_dDisplayCenterY[4];

	bool m_bMouseDown;

	CImgStatic();
	~CImgStatic();

	void PaintImage(CDC *pDC, CRect rect);
	void PaintTrack(CDC *pDC, CRect rect);
	void PaintSubstrate(CDC *pDC, CRect rect);
	void DrawCross(CDC *pDC, CPoint point, int nWidth, int nHeight);
	void DrawCircle(CDC *pDC, CPoint point, int nRadius, BOOL bFull = TRUE);
	void DrawCircleEx(CDC *pDC, CPoint point, int nRadius, BOOL bFull = TRUE);
	void DrawCircleEx(CDC *pDC, CPoint point, int nRadius, CBrush brush, BOOL bFull = TRUE);
	CRect GetImgPaintRect(CRect rect);
	tgImgRect GetImgRect(CRect rect);
	tgImgPoint GetImgDot(CPoint point);
	CPoint GetPoint(tgImgPoint tPoint);
	CPoint GetPoint(double dX, double dY, double dScale, double dOffsetX, double dOffsetY, BOOL bTrack = FALSE);

	//
	BOOL ImgInit();
	void ImgFree();
	void ImgLive();
	void ImgStop();
	void ImgSetParam(long lExposureTime,long lGain);
	bool DetectContinueGrab();
	bool SetDetectCamRoi(unsigned int XOffset,unsigned int YOffset,unsigned int ImgWidth,unsigned int ImgHeight);
	bool DetectSnap();
	bool DetectStop();
	bool SetDetectCameraParam(long lExposureTime,long lGain);

	bool CopyDetectImage(BYTE *pSrcImage,long lWidth,long lHeight,PGPixelFormat pixelFormat);
	bool CopyDetectBuffer(BYTE *pSrcBuffer,long lWidth,long lHeight,PGPixelFormat pixelFormat);
	bool CopyDetectBuffer(BYTE *pDestBuffer);
	void ImgNeedleLive();
	void ImgNeedleStop();
	//
	void DetectCamReConnect();
	void DetectRelease(); //灰点释放资源
    bool DetectReset(); //灰点单独初始化流程
	bool CopyImage(int type);
	void CopyBuffer(BYTE *pBuffer);
	bool CopyNeedleImage(int type);
	void CopyNeedleBuffer(BYTE *pBuffer);
	bool FindBlobCenter(CRect ROIF,double &dCenterX,double &dCenterY);
private:
	DECLARE_MESSAGE_MAP()
	static UINT ThreadCameraModeSwitch(LPVOID lparam);
public:
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	//afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//afx_msg void OnSize(UINT nType, int cx, int cy);
	//afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//直接引用其中 Ts_DhCamera回调函数,此处禁用。
	static void WINAPI DetectCameraDisplayCallback(void* pOwner,unsigned char* pImgData, long lWidth, long lHeight, PGPixelFormat pixelFormat);
	//MV callback ;
	static void WINAPI DetectCameraMVCallback(void* pOwner, unsigned char* pImgData, long lWidth, long lHeight, PGPixelFormat pixelFormat);
	void ThreadDetectCameraModeSwitch();
	void GetPointGreayImage(unsigned char* pImgData, long lWidth, long lHeight, PGPixelFormat pixelFormat);
};


