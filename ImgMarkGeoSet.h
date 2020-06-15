#pragma once
#include <vfw.h>
#include <GdiPlus.h>
// CImgMarkGeoSet

using namespace Gdiplus;

#define MOD_CIRCLE		0
#define MOD_CROSS		1
#define MOD_DIAMOND		2
#define MOD_EDGE_RESULT	-1
#define MOD_ELLIPSE		3
#define MOD_IMAGE		4
#define MOD_MERGE_MODEL	-1
#define MOD_MOD_RESULT	-1
#define MOD_RECTANGLE	5
#define MOD_RING		6
#define MOD_SQUARE		7
#define MOD_TRIANGLE	8

#define ROI_RECT		0x100

struct G_CIRCLE
{
	double dx,dy;
	double r;
};

struct G_CROSS
{
	double dx,dy;
	double width;
	double height;
	double horizontal;
	double vertical;
};

struct G_SCALE
{
	double height,width,length;
};

struct G_DIAMOND
{
	double dx,dy;
	double width;
	double height;
};

struct G_EDGE_RESULT
{
	double x_offset;
	double y_offset;
	double width;
	double height;
};

struct G_ELLIPSE
{
	double dx,dy;
	double width;
	double height;
};

struct G_IMAGE
{
	double x_offset;
	double y_offset;
	double width;
	double height;
};

struct G_MERGE_MODEL
{
};

struct G_MOD_RESULT
{
};

struct G_RECTANGLE
{
	double dx,dy;
	double width;
	double height;
};

struct G_RING
{
	double dx,dy;
	double in_ring;
	double out_ring;
};

struct G_MOD_SQUARE
{
	double dx,dy;
	double length;
};

struct G_MOD_TRIANGLE
{
	double dx,dy;
	double width;
	double height;
};

class CImgMarkGeoSet : public CStatic
{
	DECLARE_DYNAMIC(CImgMarkGeoSet)

public:
	CImgMarkGeoSet();
	virtual ~CImgMarkGeoSet();

protected:
	DECLARE_MESSAGE_MAP()
public:
	BYTE	*m_pImgBuffer;
	BYTE	*m_pScerchImg; 
	int		m_DrawDelay;
	BOOL	m_bDisplay;
	double m_dScale;
	int m_iImgWidth, m_iImgHeight;
	HDRAWDIB m_hDrawDib;
	BITMAPINFO *m_pBmpInfo;
	short m_modtype;
	G_CIRCLE m_mod_circle;
	G_CROSS m_mod_cross;
	G_DIAMOND m_mod_diamond;
	G_EDGE_RESULT m_mod_edge;
	G_ELLIPSE m_mod_ellipse;
	G_RECTANGLE m_mod_rectangle;
	G_RING m_mod_ring;
	G_MOD_SQUARE m_mod_square;
	G_MOD_TRIANGLE m_mod_triangle;
	G_IMAGE m_mod_image;
	G_SCALE m_mod_scale;
	Bitmap* mBtmap;
	Bitmap* mBtmapBk;
	Graphics* graphics;
	CRect RoiRect;
public:
	BOOL m_BtSel;
	int m_PtSelType;
	int m_nCurStep;
	BOOL m_RoiSel;
	BOOL m_GatherSign;
	BOOL m_GatherInit;
	CPoint m_StPoint;
	double *pScore;
	double *pXPos;
	double *pYPos;
	double *pAng;
	double *pScale;
	double m_dTime;
	int m_iCount;
	int m_iDisplayDelay;
	RectF destRect;
	ImageAttributes imageAttr;
	bool m_bMarkImage;
	bool m_bNeedleImage;
	int yOffset;
public:
	void		PaintImage(CDC *pDC, CRect rect);
	CRect		GetImgPaintRect(CRect rect);
	void		ImgInit(long lImageWidth,long lImageHeight,int nPixel);
	void		PaintDraw(CDC *pDC, CRect rect);
	void		DrawCircle(CDC *pDC, CPoint point, int r, BOOL bFull=TRUE);
	void		DrawCircleEx(CDC *pDC, CPoint point, int r, BOOL bFull=TRUE);
	void		DrawSquare(CDC *pDC, CPoint point, int len, BOOL bFull=TRUE);

	void		PaintCircle(CDC *pDC, CRect rect);
	void		PaintCross(CDC *pDC, CRect rect);
	void		PaintDiamond(CDC *pDC, CRect rect);
	void		PaintEllipse(CDC *pDC, CRect rect);
	void		PaintRectangle(CDC *pDC, CRect rect);
	void		PaintRing(CDC *pDC, CRect rect);
	void		PaintSquare(CDC *pDC, CRect rect);
	void		PaintTriangle(CDC *pDC, CRect rect);
	void		PaintRoiRect(CDC *pDC, CRect rect);
	void		PaintImage2(CDC *pDC, CRect rect);
	void		PaintScale(CDC *pDC, CRect rect);

	void		DrawImg(BYTE *pImg,int delay);			//延时显示图像,透明重叠方式
	void		DrawImgEx(CDC *pDC, CRect rect);			//延时显示图像,透明重叠方式
	void		Gr_DrawText(Graphics *gr,int x,int y,CString str,Color cl=Color(255,0,0,255));
	void		DrawSearch(CDC *pDC, CRect rect);
	void		SetSearchResult(double *dScore,double *dXPos,double *dYPos,double *dAng,double *dScale,int iCount,double dTime);
	void		ResetParam();
	void		SetDisplayMarkImage(bool bMarkImage) { m_bMarkImage = bMarkImage;}
	void		SetDisplayNeedleImage(bool bNeedleImage) { m_bNeedleImage = bNeedleImage;} //Inline Function
public:
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};