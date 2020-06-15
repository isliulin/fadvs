#include "stdafx.h"
#include "TSCtrlSys.h"
#include "ImgStatic.h"
#include <math.h>
#pragma comment(lib, "vfw32.lib")
#define _USE_MATH_DEFINES
CWinThread *g_pCameraModeSwitchThread = NULL;
CCriticalSection m_csDetectCamera;

IMPLEMENT_DYNAMIC(CImgStatic, CStatic)


UINT CImgStatic::ThreadCameraModeSwitch(LPVOID lparam)
{
	static bool b = false;
	if(b) return 1;
	b = true;
	CImgStatic *p = (CImgStatic *)lparam;
    p->DetectContinueGrab();
	b=false;
	return 0;
}
CImgStatic::CImgStatic()
{
	m_rectImgRect = CRect(0,0,1,1);
	m_nImgWidth = IMAGEWIDTH0;
	m_nImgHeight = IMAGEHEIGHT0;
	m_lImgDetectWidth = IMAGEWIDTH1;
	m_lImgDetectHeight = IMAGEHEIGHT1;
	m_nImgWidthNeedle=IMAGEWIDTH2;
	m_nImgHeightNeedle=IMAGEHEIGHT2;
	m_bImgInit = FALSE;
	m_bDetectInit = false;
	m_bImgInitNeedle=false;
	m_dPatAcceptance = 0;
	m_hDrawDib = DrawDibOpen();
	m_nOutSizeX = IMAGEWIDTH0;
	m_nOutSizeY = IMAGEHEIGHT0;
	m_bMouseDown = false;

	m_nDetectPixel = 1;
	m_nImgNeedlePixel=1;
	m_pDetectCamera = NULL;
	m_pPointGreyCam = NULL;//2017-12-12
	//m_pCameraNeedle =NULL;
	m_pDetectImageBuffer = NULL;/////
	InitializeCriticalSection(&m_csDetectCamera);
}

CImgStatic::~CImgStatic()
{
	if(m_hDrawDib!=NULL)
	{
		DrawDibClose(m_hDrawDib);
		m_hDrawDib = NULL;
	}
	if(m_pBitmapInfo!=NULL)
	{
		delete[] m_pBitmapInfo;
		m_pBitmapInfo = NULL;
	}
	this->ImgFree();
}

BEGIN_MESSAGE_MAP(CImgStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void CImgStatic::PaintImage(CDC *pDC, CRect rect)
{
	//if(m_pImageBuffer == NULL)
	//{
	//	return;
	//}
	//DrawDibDraw(m_hDrawDib,pDC->GetSafeHdc(),rect.left,rect.top,rect.Width(),rect.Height(),
	//	&m_pBmpInfo->bmiHeader,m_pImageBuffer,0,0,m_pBmpInfo->bmiHeader.biWidth,
	//	m_pBmpInfo->bmiHeader.biHeight,SRCCOPY);
	if(m_pCamera.m_pImageBuffer == NULL)
	{
		return;
	}

	DrawDibDraw(m_hDrawDib,pDC->GetSafeHdc(),rect.left,rect.top,rect.Width(),rect.Height(),
		&m_pBitmapInfo->bmiHeader,m_pCamera.m_pImageBuffer,0,0,m_pBitmapInfo->bmiHeader.biWidth,
		m_pBitmapInfo->bmiHeader.biHeight,SRCCOPY);

}

void CImgStatic::PaintSubstrate(CDC *pDC, CRect rect)
{
	if(g_pFrm==NULL)
	{
		return;
	}
	try
	{	
		CString strTemp;
		pDC->Draw3dRect(m_rectSelRect,RGB(240,150,0),RGB(240,150,0));
		CPoint point = GetPoint(m_PatPos);
		DrawCross(pDC, point, 15,15);
		strTemp.Format("%.2f(%d,%d)", m_dPatAcceptance, point.x, point.y);
		pDC->SetTextColor(RGB(0,0,255));
		pDC->TextOut(point.x+10, point.y-20, strTemp);
		pDC->SetTextColor(RGB(255,0,0));

		CPen pen(PS_SOLID,1,RGB(250,0,0));
		CPen *pOldPen = pDC->SelectObject(&pen);
		DrawCross(pDC, rect.CenterPoint(), rect.Width()/2, rect.Height()/2);
		//画0.5mm刻度
		double dmm = 0.5/theApp.m_tSysParam.dPixelScaleX * m_dScale;
		CPoint t;
		int x = rect.CenterPoint().x;
		int y = rect.CenterPoint().y;
		int i = 0;
		while(1)
		{
			t.x = static_cast<int>(x - dmm*i);
			t.y = static_cast<int>(y - dmm*i);

			if(t.x <=0 && t.y<=0)break;
			pDC->MoveTo(t.x, y-8);
			pDC->LineTo(t.x, y+8);
			pDC->MoveTo(x-8, t.y);
			pDC->LineTo(x+8, t.y);

			t.x = static_cast<int>(x + dmm*i);
			t.y = static_cast<int>(y + dmm*i);

			pDC->MoveTo(t.x, y-8);
			pDC->LineTo(t.x, y+8);
			pDC->MoveTo(x-8, t.y);
			pDC->LineTo(x+8, t.y);

			i++;
		}

		CRect rc,r;
		GetClientRect(&r);
		if(r.Width()>rect.Width())
		{
			rc.left = r.left+rect.Width();
			rc.top = r.top;
			rc.right = r.right;
			rc.bottom = r.bottom;
		}
		else
		{
			rc.left = rect.left;
			rc.right = rect.right;
			rc.top = rect.top;
			rc.bottom = rect.bottom;
		}
		pDC->SelectObject(pOldPen);
		pen.DeleteObject();
		pen.CreatePen(PS_SOLID,3,RGB(204,85,51));
		pOldPen = pDC->SelectObject(&pen);
		if(g_pFrm->m_CmdRun.ProductParam.IrregularMatrix)
		{
			double dTotalRowDistance = 0.0;
			double dTotalColumnDistance = 0.0;
			for(int j = 0;j<g_pFrm->m_CmdRun.ProductParam.IrregularRow;j++)
			{
				if(j<g_pFrm->m_CmdRun.ProductParam.IrregularRow-1)
				{
					dTotalRowDistance += g_pFrm->m_CmdRun.ProductParam.IrregularRowD[j];
				}
				else
				{
					dTotalRowDistance += g_pFrm->m_CmdRun.ProductParam.IrregularRowD[0];
				}
			}
			for(int j = 0;j<g_pFrm->m_CmdRun.ProductParam.IrregularColumn;j++)
			{
				if(j<g_pFrm->m_CmdRun.ProductParam.IrregularColumn-1)
				{
					dTotalColumnDistance += g_pFrm->m_CmdRun.ProductParam.IrregularColumnD[j];
				}
				else
				{
					dTotalColumnDistance += g_pFrm->m_CmdRun.ProductParam.IrregularColumnD[0];
				}
			}
			double dScaleX = rc.Width()/(1.2*dTotalColumnDistance);//长1mm的相素
			double dScaleY = rc.Height()/(2.3*dTotalRowDistance);//高1mm的相素
			int nX0 = int(rc.right-(0.1*dTotalColumnDistance+g_pFrm->m_CmdRun.m_tOffset.x)*dScaleX);
			int nY0=int((0.1*dTotalRowDistance+g_pFrm->m_CmdRun.m_tOffset.y)*dScaleY+rc.top);
			double dCurrentColumnD = 0.0;
			double dCurrentRowD = 0.0;
			for(int j=0;j<=g_pFrm->m_CmdRun.ProductParam.IrregularColumn;j++)
			{
				pDC->MoveTo(int(nX0-dCurrentColumnD*dScaleX),nY0);
				pDC->LineTo(int(nX0-dCurrentColumnD*dScaleX),int(nY0+dTotalRowDistance*dScaleY));
				if(j<g_pFrm->m_CmdRun.ProductParam.IrregularColumn-1)
				{
					dCurrentColumnD += g_pFrm->m_CmdRun.ProductParam.IrregularColumnD[j];
				}
				else if(j>=g_pFrm->m_CmdRun.ProductParam.IrregularColumn-1)
				{
					dCurrentColumnD += g_pFrm->m_CmdRun.ProductParam.IrregularColumnD[0];
				}
			}
			for(int j=0;j<=g_pFrm->m_CmdRun.ProductParam.IrregularRow;j++)
			{
				pDC->MoveTo(nX0,int(nY0+dCurrentRowD*dScaleY));
				pDC->LineTo(int(nX0-dTotalColumnDistance*dScaleX),int(nY0+dCurrentRowD*dScaleY));
				if(j<g_pFrm->m_CmdRun.ProductParam.IrregularRow-1)
				{
					dCurrentRowD += g_pFrm->m_CmdRun.ProductParam.IrregularRowD[j];
				}
				else if(j>=g_pFrm->m_CmdRun.ProductParam.IrregularRow-1)
				{
					dCurrentRowD += g_pFrm->m_CmdRun.ProductParam.IrregularRowD[0];
				}
			}
			// 点胶坐标
			nX0 = int(nX0 - g_pFrm->m_CmdRun.ProductParam.IrregularColumnD[0] * dScaleX / 2);
			nY0 = int(nY0 + g_pFrm->m_CmdRun.ProductParam.IrregularRowD[0] * dScaleY / 2);
			CPoint ptCurrent;
			ptCurrent.x = long((theApp.m_Mv400.GetPos(K_AXIS_X)-g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[0])*dScaleX+nX0);
			ptCurrent.y = long((theApp.m_Mv400.GetPos(K_AXIS_Y)-g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[1])*dScaleY+nY0);
			pDC->SelectObject(pOldPen);
			pen.DeleteObject();
			pen.CreatePen(PS_SOLID, 1, RGB(255,255,0));
			pOldPen = pDC->SelectObject(&pen);
			DrawCross(pDC,ptCurrent,15,15);
			// 已点胶位置画标志
			g_pFrm->m_CmdRun.m_bRefresh = true;
			if (g_pFrm->m_CmdRun.PDposting!=NULL)
			{
				int nCount = g_pFrm->m_CmdRun.ProductParam.IrregularRow*g_pFrm->m_CmdRun.ProductParam.IrregularColumn;
				for (int i=0;i<nCount;i++)
				{
					if(g_pFrm->m_CmdRun.PDposting[i].bIsCompleted)
					{
						CPoint ptTemp;
						ptTemp.x=long((g_pFrm->m_CmdRun.PDposting[i].Dx-g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[0])*dScaleX+nX0);
						ptTemp.y=long((g_pFrm->m_CmdRun.PDposting[i].Dy-g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[1])*dScaleY+nY0);
						DrawCircleEx(pDC,ptTemp,5,1);
					}
				}
			}
			g_pFrm->m_CmdRun.m_bRefresh = false;
		}
		else
		{
			double dScaleX = rc.Width()/(1.2*g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*g_pFrm->m_CmdRun.ProductParam.FirstDispColumnD);//长1mm的相素
			double dScaleY = rc.Height()/(2.3*g_pFrm->m_CmdRun.ProductParam.FirstDispRow*g_pFrm->m_CmdRun.ProductParam.FirstDispRowD);//高1mm的相素
			int nX0 = int(rc.right-(0.1*g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*g_pFrm->m_CmdRun.ProductParam.FirstDispColumnD+g_pFrm->m_CmdRun.m_tOffset.x)*dScaleX);
			int nY0=int((0.1*g_pFrm->m_CmdRun.ProductParam.FirstDispRow*g_pFrm->m_CmdRun.ProductParam.FirstDispRowD+g_pFrm->m_CmdRun.m_tOffset.y)*dScaleY+rc.top);
			for(int m=0;m<g_pFrm->m_CmdRun.ProductParam.FirstDispRow;m++)
			{
				for(int n=0;n<g_pFrm->m_CmdRun.ProductParam.FirstDispColumn;n++)
				{
					for(int i=0;i<g_pFrm->m_CmdRun.ProductParam.SecondDispRow;i++)
					{
						for(int j=0;j<g_pFrm->m_CmdRun.ProductParam.SecondDispColumn;j++)
						{
							for(int k=0;k<=g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn;k++)
							{
								pDC->MoveTo(int(nX0-n*g_pFrm->m_CmdRun.ProductParam.FirstDispColumnD*dScaleX-j*g_pFrm->m_CmdRun.ProductParam.SecondDispColumnD*dScaleX-k*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD*dScaleX),
									int(nY0+m*g_pFrm->m_CmdRun.ProductParam.FirstDispRowD*dScaleY+i*g_pFrm->m_CmdRun.ProductParam.SecondDispRowD*dScaleY));
								pDC->LineTo(int(nX0-n*g_pFrm->m_CmdRun.ProductParam.FirstDispColumnD*dScaleX-j*g_pFrm->m_CmdRun.ProductParam.SecondDispColumnD*dScaleX-k*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD*dScaleX),
									int(nY0+m*g_pFrm->m_CmdRun.ProductParam.FirstDispRowD*dScaleY+i*g_pFrm->m_CmdRun.ProductParam.SecondDispRowD*dScaleY+g_pFrm->m_CmdRun.ProductParam.ThirdDispRow*g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD*dScaleY));
							}
							for(int l=0;l<=g_pFrm->m_CmdRun.ProductParam.ThirdDispRow;l++)
							{
								pDC->MoveTo(int(nX0-n*g_pFrm->m_CmdRun.ProductParam.FirstDispColumnD*dScaleX-j*g_pFrm->m_CmdRun.ProductParam.SecondDispColumnD*dScaleX),
									int(nY0+m*g_pFrm->m_CmdRun.ProductParam.FirstDispRowD*dScaleY+i*g_pFrm->m_CmdRun.ProductParam.SecondDispRowD*dScaleY+l*g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD*dScaleY));
								pDC->LineTo(int(nX0-n*g_pFrm->m_CmdRun.ProductParam.FirstDispColumnD*dScaleX-j*g_pFrm->m_CmdRun.ProductParam.SecondDispColumnD*dScaleX-g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD*dScaleX),
									int(nY0+m*g_pFrm->m_CmdRun.ProductParam.FirstDispRowD*dScaleY+i*g_pFrm->m_CmdRun.ProductParam.SecondDispRowD*dScaleY+l*g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD*dScaleY));
							}
						}
					}
				}
			}

			// 点胶坐标
			nX0 = int(nX0 - g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD * dScaleX / 2);
			nY0 = int(nY0 + g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD * dScaleY / 2);
			CPoint ptCurrent;
			ptCurrent.x = long((theApp.m_Mv400.GetPos(K_AXIS_X)-g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[0])*dScaleX*(-1)+nX0);
			ptCurrent.y = long((theApp.m_Mv400.GetPos(K_AXIS_Y)-g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[1])*dScaleY+nY0);
			pDC->SelectObject(pOldPen);
			pen.DeleteObject();
			pen.CreatePen(PS_SOLID, 1, RGB(255,255,0));
			pOldPen = pDC->SelectObject(&pen);
			DrawCross(pDC,ptCurrent,15,15);

			// 已点胶位置画标志
			g_pFrm->m_CmdRun.m_bRefresh = true;
			if (g_pFrm->m_CmdRun.PDposting!=NULL)
			{
				int nCount = g_pFrm->m_CmdRun.ProductParam.FirstDispRow*g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*g_pFrm->m_CmdRun.ProductParam.SecondDispRow*
					g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*g_pFrm->m_CmdRun.ProductParam.ThirdDispRow*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn;
				for (int i=0;i<nCount;i++)
				{
					if(g_pFrm->m_CmdRun.PDposting[i].bIsCompleted)
					{
						CPoint ptTemp;
						ptTemp.x=long((g_pFrm->m_CmdRun.PDposting[i].Dx-g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[0])*dScaleX*(-1)+nX0);
						ptTemp.y=long((g_pFrm->m_CmdRun.PDposting[i].Dy-g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[1])*dScaleY+nY0);
						DrawCircleEx(pDC,ptTemp,5,CBrush(RGB(0,0,240)),1);
					}
					else if(!g_pFrm->m_CmdRun.PDposting[i].bIsHasPad)
					{
						CPoint ptTemp;
						ptTemp.x=long((g_pFrm->m_CmdRun.PDposting[i].Dx-g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[0])*dScaleX*(-1)+nX0);
						ptTemp.y=long((g_pFrm->m_CmdRun.PDposting[i].Dy-g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[1])*dScaleY+nY0);
						DrawCircleEx(pDC,ptTemp,5,CBrush(RGB(240,0,0)),1);
					}
				}
			}
			g_pFrm->m_CmdRun.m_bRefresh = false;
		}
		pDC->SelectObject(pOldPen);
		pen.DeleteObject();
		////////
	}
	catch (...)
	{
		CString str;
		str.Format("PaintSubstrate()存在异常...");
		if (NULL!=g_pFrm)
		{
			g_pFrm->m_CmdRun.PutLogIntoList(str);
		}
		AfxMessageBox("PaintSubstrate()存在内存异常...");
	}
}

void CImgStatic::PaintTrack(CDC *pDC, CRect rect)
{
	if(g_pFrm==NULL)
	{
		return;
	}

	CPen pen(PS_SOLID,1,RGB(100,255,50));
	CPen *pOldPen = pDC->SelectObject(&pen);
	CBrush brush;
	brush.CreateSolidBrush(RGB(150,255,50));
	CBrush *pOldBrush = pDC->SelectObject(&brush);
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	pen.DeleteObject();
	brush.DeleteObject();
}

void CImgStatic::DrawCross(CDC *pDC, CPoint point, int nWidth, int nHeight)
{
	pDC->MoveTo(point.x,point.y-nHeight);
	pDC->LineTo(point.x,point.y+nHeight);
	pDC->MoveTo(point.x-nWidth,point.y);
	pDC->LineTo(point.x+nWidth,point.y);
}

void CImgStatic::DrawCircle(CDC *pDC, CPoint point, int nRadius, BOOL bFull /* = TRUE */)
{
	if(!bFull)
	{
		CGdiObject *pOldGdi = pDC->SelectStockObject(NULL_BRUSH);
		pDC->Ellipse(point.x-nRadius, point.y-nRadius, point.x+nRadius, point.y+nRadius);
		pDC->SelectObject(pOldGdi);
	}
	else
	{
		pDC->Ellipse(point.x-nRadius, point.y-nRadius, point.x+nRadius, point.y+nRadius);
	}
}

void CImgStatic::DrawCircleEx(CDC *pDC, CPoint point, int nRadius, BOOL bFull /* = TRUE */)
{
	CBrush brush(RGB(240,0,0));
	CBrush *pOldBrush = pDC->SelectObject(&brush);
	if(!bFull)
	{
		CGdiObject *pOldGdi = pDC->SelectStockObject(NULL_BRUSH);
		pDC->Ellipse(point.x-nRadius, point.y-nRadius, point.x+nRadius, point.y+nRadius);
		pDC->SelectObject(pOldGdi);
	}
	else
	{
		pDC->Ellipse(point.x-nRadius, point.y-nRadius, point.x+nRadius, point.y+nRadius);
	}
	pDC->SelectObject(pOldBrush);
	brush.DeleteObject();
}

void CImgStatic::DrawCircleEx(CDC *pDC, CPoint point, int nRadius, CBrush brush, BOOL bFull /* = TRUE */)
{
	CBrush *pOldBrush = pDC->SelectObject(&brush);
	if(!bFull)
	{
		CGdiObject *pOldGdi = pDC->SelectStockObject(NULL_BRUSH);
		pDC->Ellipse(point.x-nRadius, point.y-nRadius, point.x+nRadius, point.y+nRadius);
		pDC->SelectObject(pOldGdi);
	}
	else
	{
		pDC->Ellipse(point.x-nRadius, point.y-nRadius, point.x+nRadius, point.y+nRadius);
	}
	pDC->SelectObject(pOldBrush);
	brush.DeleteObject();
}

CRect CImgStatic::GetImgPaintRect(CRect rect)
{
	double dxScale = double(rect.Width())/m_nImgWidth;
	double dyScale = double(rect.Height())/m_nImgHeight;
	m_dScale = dxScale>dyScale ? dyScale : dxScale;

	CRect rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = static_cast<long>(m_dScale*m_nImgWidth);
	rc.bottom = static_cast<long>(m_dScale*m_nImgHeight);
	return rc;
}

tgImgRect CImgStatic::GetImgRect(CRect rect)
{
	tgImgRect rc;
	tgImgPoint pt;
	int nTemp;
	pt = GetImgDot(rect.TopLeft());
	rc.left = pt.x;
	rc.top = pt.y;
	pt = GetImgDot(rect.BottomRight());
	rc.right = pt.x;
	rc.bottom = pt.y;

	if(rc.left > rc.right)
	{
		nTemp = rc.left;
		rc.left = rc.right;
		rc.right = nTemp;
	}

	if(rc.top > rc.bottom)
	{
		nTemp = rc.top;
		rc.top = rc.bottom;
		rc.bottom = nTemp;
	}
	if(rc.right>m_nImgWidth-1)
	{
		rc.right = m_nImgWidth-1;
	}
	if(rc.bottom>m_nImgHeight-1)
	{
		rc.bottom = m_nImgHeight-1;
	}
	return rc;
}

tgImgPoint CImgStatic::GetImgDot(CPoint point)
{
	tgImgPoint pt;
	pt.x = static_cast<int>(point.x/m_dScale);
	pt.y = static_cast<int>(point.y/m_dScale);
	return pt;
}

CPoint CImgStatic::GetPoint(tgImgPoint tPoint)
{
	CPoint point;
	point.x = static_cast<long>(tPoint.x*m_dScale);
	point.y = static_cast<long>(tPoint.y*m_dScale);
	return point;
}

CPoint CImgStatic::GetPoint(double dX, double dY, double dScale, double dOffsetX, double dOffsetY, BOOL bTrack /* = FALSE */)
{
	CPoint point;
	point.x = static_cast<long>((dX+dOffsetX)*dScale);
	point.y = static_cast<long>((dY+dOffsetY)*dScale);
	if(bTrack)
	{
		point.x += m_ptTrackDrawOffset.x;
		point.y += m_ptTrackDrawOffset.y;
	}
	return point;
}

BOOL CImgStatic::ImgInit()
{
	//// AVT彩色相机
	//m_nOutSizeX=m_nImgWidth; m_nOutSizeY=m_nImgHeight;
	//m_bImgInit = TRUE;
	//m_pBmpInfo = (BITMAPINFO*)(new char[sizeof(BITMAPINFOHEADER) + 256*4]);
	//m_pBmpInfo->bmiHeader.biBitCount=(UINT16)(24);

	//m_pBmpInfo->bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
	//m_pBmpInfo->bmiHeader.biWidth  = m_nOutSizeX;
	//m_pBmpInfo->bmiHeader.biHeight = m_nOutSizeY;
	//m_pBmpInfo->bmiHeader.biPlanes = 1;

	//m_pBmpInfo->bmiHeader.biCompression  = BI_RGB;
	//m_pBmpInfo->bmiHeader.biSizeImage    = m_nOutSizeX * m_nOutSizeY * 3;
	//m_pBmpInfo->bmiHeader.biXPelsPerMeter= 0;
	//m_pBmpInfo->bmiHeader.biYPelsPerMeter= 0;
	//m_pBmpInfo->bmiHeader.biClrUsed      = 0;
	//m_pBmpInfo->bmiHeader.biClrImportant = 0;	

	//m_bImgInit=m_CarmAVT.OpenCarm();

	// DH黑白相机
	CString str;
	m_bImgInit = TRUE;
	m_nOutSizeX = m_nImgWidth = IMAGEWIDTH0;
	m_nOutSizeY = m_nImgHeight = IMAGEHEIGHT0;
	//m_bImgInit = m_pCamera.InitDH(1, 25, 0,0,m_nOutSizeX,m_nOutSizeY,RES_MODE1,false);//Init the Mark Cemera;
	m_bImgInit = m_pCamera.InitDH(theApp.m_tSysParam.nCamMarkNo, 25, 0,0,m_nOutSizeX,m_nOutSizeY,RES_MODE1,false);//Init the Mark Cemera:RES_DODE1 640X512
	if(!m_bImgInit)
	{
		str.Format("[][][]定位相机初始化失败...");
		g_pFrm->m_pLog->log(str);
	}
	m_bImgInit=m_pCameraNeedle.InitDH(theApp.m_tSysParam.nCamNeedleNo, 25, 0,0,m_nImgWidthNeedle,m_nImgHeightNeedle,RES_MODE1,false);//Init the Needle Cemera;
	if(!m_bImgInit)
	{
		str.Format("[][][]对针头相机初始化失败...");
		g_pFrm->m_pLog->log(str);
	}
	//m_pBitmapInfo = (BITMAPINFO*)(new char[sizeof(BITMAPINFOHEADER) + 256*4]);
	//m_pBitmapInfo->bmiHeader.biBitCount=(UINT16)(24);
	//m_pBitmapInfo->bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
	//m_pBitmapInfo->bmiHeader.biWidth  = m_nImgWidth;
	//m_pBitmapInfo->bmiHeader.biHeight = m_nImgHeight;
	//m_pBitmapInfo->bmiHeader.biPlanes = 1;
	//m_pBitmapInfo->bmiHeader.biCompression  = BI_RGB;
	//m_pBitmapInfo->bmiHeader.biSizeImage    = m_nImgWidth * m_nImgHeight * 3;
	//m_pBitmapInfo->bmiHeader.biXPelsPerMeter= 0;
	//m_pBitmapInfo->bmiHeader.biYPelsPerMeter= 0;
	//m_pBitmapInfo->bmiHeader.biClrUsed      = 0;
	//m_pBitmapInfo->bmiHeader.biClrImportant = 0;

	m_pBitmapInfo = (BITMAPINFO*)(new char[sizeof(BITMAPINFOHEADER) + 256*4]);
	m_pBitmapInfo->bmiHeader.biBitCount=(UINT16)24;
	m_pBitmapInfo->bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
	m_pBitmapInfo->bmiHeader.biWidth  = m_nImgWidth;
	m_pBitmapInfo->bmiHeader.biHeight = m_nImgHeight;
	m_pBitmapInfo->bmiHeader.biPlanes = 1;
	m_pBitmapInfo->bmiHeader.biCompression  = BI_RGB;
	m_pBitmapInfo->bmiHeader.biSizeImage    = m_nImgWidth * m_nImgHeight * 3;
	m_pBitmapInfo->bmiHeader.biXPelsPerMeter= 0;
	m_pBitmapInfo->bmiHeader.biYPelsPerMeter= 0;
	m_pBitmapInfo->bmiHeader.biClrUsed      = 0;
	m_pBitmapInfo->bmiHeader.biClrImportant = 0;

	if(theApp.m_tSysParam.detectCameraType==DETECT_CAMERA_MV)
	{
		m_pDetectCamera=new Ts_DhCamera;
		if(NULL==m_pDetectCamera)
		{
			CString str;
			str.Format("ImgInit().大华相机实例建立失败...");
			g_pFrm->m_CmdRun.PutLogIntoList(str);
			return false;
		}
		m_pDetectCamera->m_nOffsetX=OFFSETX; 
		m_pDetectCamera->m_nOffsetY=OFFSETY; 
		m_pDetectCamera->m_nImageHeight = m_lImgDetectHeight;
		m_pDetectCamera->m_nImageWidth = m_lImgDetectWidth;
		m_bDetectInit=m_pDetectCamera->Init();
		if (m_bDetectInit)
		{
			if(!SetDetectCameraParam(theApp.m_tSysParam.CameraDetectExposureTime,theApp.m_tSysParam.CameraDetectGain))
			{
				AfxMessageBox("DaHua相机设置曝光时间或增益失败！");
			}
			DetectContinueGrab();
			g_pFrm->m_CmdRun.AddMsg("DaHua相机初始化ok...");
		}
		else
		{
			AfxMessageBox("DaHua相机初始化失败！");
		}
	}
	else
	{
		m_pPointGreyCam=new PointGreyCam;
		if (NULL==m_pPointGreyCam)
		{
			AfxMessageBox("ImgInit().PointGrey实例建立失败.");
			return false;
		}
		long ret=m_pPointGreyCam->OpenCamera(0,OFFSETX,OFFSETY,IMAGEWIDTH1,IMAGEHEIGHT1);
		if (0==ret)
		{
			g_pFrm->m_CmdRun.AddMsg("PointGrey相机初始化ok...");
			m_bDetectInit=true;
		}
		else
		{
			m_bDetectInit=false;
		}
		if(m_bDetectInit)
		{
			if(!SetDetectCameraParam(theApp.m_tSysParam.CameraDetectExposureTime,theApp.m_tSysParam.CameraDetectGain))
			{
				AfxMessageBox("PointGrey相机设置曝光时间或增益失败！");
			}
		}
		else
		{
			AfxMessageBox("PointGrey相机初始化失败！");
		}
	}
	return m_bImgInit;
}

void CImgStatic::ImgFree()
{
	ImgStop();
	m_pCameraNeedle.Stop();
	DetectStop();
	m_pCamera.Free();
	m_pCameraNeedle.Free();
	if(m_pDetectCamera!=NULL)   //MV_CAMERA
	{
		delete m_pDetectCamera;
		m_pDetectCamera = NULL;
	}
	if (m_pPointGreyCam!=NULL)  //PG_CAMERA
	{
		delete m_pPointGreyCam;
		m_pPointGreyCam=NULL;
	}
	this->DestroyWindow();
}

void CImgStatic::ImgStop()
{
	//m_CarmAVT.StopCarm();
	m_pCamera.Stop();
}

void CImgStatic::ImgLive()
{
	//m_CarmAVT.CarmLive();
	m_pCamera.Live();
}

bool CImgStatic::CopyImage(int type)
{
	//if(m_pRawBuffer != NULL)
	//{
	//	KS_CopyBuffer(m_KSBuffer,m_pImageBuffer,KS_RGB24);
	//}
	//else if(m_pImageBuffer != NULL)
	//{
	//	KS_CopyBuffer(m_KSBuffer,m_pImageBuffer,KS_RGB8);
	//}
	//milApp.CopyBuffer(milApp.m_MilImage,m_pImageBuffer,RGB24);
	bool bReturn = false;
	if(type==RGB24)
	{
		if(m_pCamera.m_pImageBuffer!=NULL)
		{
			milApp.CopyBuffer(milApp.m_MilImage,m_pCamera.m_pImageBuffer,type);
			bReturn = true;
		}
	}
	else
	{
		if(m_pCamera.m_pRawBuffer!=NULL)
		{
			milApp.CopyBuffer(milApp.m_MilImage,m_pCamera.m_pRawBuffer,type);
			bReturn = true;
		}
	}
	return bReturn;
}

void CImgStatic::CopyBuffer(BYTE *pBuffer)
{
	if(m_pCamera.m_pImageBuffer!=NULL)
	{
		memcpy(pBuffer,m_pCamera.m_pImageBuffer,m_pCamera.m_nImageWidth*m_pCamera.m_nImageHeight*3);
	}
}
void CImgStatic::OnPaint()
{
	CPaintDC dc(this);
	if(GetSafeHwnd()==NULL)
	{
		return;
	}
	CDC cdc;
	CBitmap bitmap;
	CRect rect;
	GetClientRect(&rect);//获取图形控件自身客户区的大小；

	cdc.CreateCompatibleDC(&dc);
	bitmap.CreateCompatibleBitmap(&dc,rect.Width(),rect.Height());
	CBitmap *pOldBitmap = cdc.SelectObject(&bitmap);

	CBrush brush;
	brush.CreateSolidBrush(RGB(163, 176, 188));
	CBrush *pOldBrush = cdc.SelectObject(&brush); 
	cdc.SetBkMode(TRANSPARENT);
	cdc.Rectangle(rect);
	// 计算图像显示窗体大小
	m_rectImgRect = GetImgPaintRect(rect);
	CFont font;
	font.CreatePointFont(80,"Arial");
	CFont *pOldFont = cdc.SelectObject(&font);
	cdc.SetTextColor(RGB(255,0,155));

	PaintImage(&cdc,m_rectImgRect);
	//PaintShape(&cdc,m_rectImgRect);
	PaintSubstrate(&cdc,m_rectImgRect);
	//PaintTrack(&cdc,m_rectImgRect);

	dc.StretchBlt(0,0,rect.Width(),rect.Height(),&cdc,0,0,rect.Width(),rect.Height(),SRCCOPY);

	cdc.SelectObject(pOldFont);
	cdc.SelectObject(pOldBrush);
	cdc.SelectObject(pOldBitmap);
	bitmap.DeleteObject();
	brush.DeleteObject();
	font.DeleteObject();
	cdc.DeleteDC();
}

void CImgStatic::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bMouseDown = false;
	ReleaseCapture();
	CStatic::OnLButtonUp(nFlags,point);
}

void CImgStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	CPoint oldPoint;
	oldPoint = point;
	m_rectSelRect.left = m_rectSelRect.right = point.x;
	m_rectSelRect.top = m_rectSelRect.bottom = point.y;
	m_bMouseDown = true;
	Invalidate();
	SetCapture();
	CStatic::OnLButtonDown(nFlags,point);
}

void CImgStatic::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint oldPoint;
	oldPoint = point;
	if(m_bMouseDown)
	{
		//m_rectSelRect.right = point.x;
		//m_rectSelRect.bottom = point.y;
		CPoint t = GetPoint(tgImgPoint(m_nImgWidth/2, m_nImgHeight/2));
		//TRACE(_T("\nCPoint t.x = %d, t.y = %d "), t.x, t.y);
		int nx = abs(point.x - t.x);
		int ny = abs(point.y - t.y);
		m_rectSelRect.left = t.x-nx;
		m_rectSelRect.top = t.y-ny;
		m_rectSelRect.right = t.x+nx;
		m_rectSelRect.bottom = t.y+ny;
	}
	CStatic::OnMouseMove(nFlags,point);
}

bool CImgStatic::FindBlobCenter(CRect ROIF,double &dCenterX,double &dCenterY)
{
	CPoint tempPoint1,tempPoint2;
	double dHight,dWidth;
	milApp.SetBlobParam(theApp.m_tSysParam.StandardLen,theApp.m_tSysParam.BmpAreaMin,100000,1,1,false);
	double dMinX = 10000.0;
	double dMinY = 10000.0;
	double dMaxX = 0.0;
	double dMaxY = 0.0;
	dCenterX = 0.0;
	dCenterY = 0.0;
	milApp.FindBlob(ROIF);
	int nFindNum = milApp.BlobResult.nBlobNum;
	if(nFindNum>0&&nFindNum<20)
	{
		for(int i=0;i<nFindNum;i++)
		{
			dWidth = milApp.BlobResult.dBlobXMax[i] - milApp.BlobResult.dBlobXMin[i];
			dHight = milApp.BlobResult.dBlobYMax[i] - milApp.BlobResult.dBlobYMin[i];
			if(dWidth>theApp.m_tSysParam.BmpWidthMax||dWidth<theApp.m_tSysParam.BmpWidthMin)
			{
				continue;
			}
			if(dHight>theApp.m_tSysParam.BmpHeighMax||dHight<theApp.m_tSysParam.BmpHeighMin)
			{
				continue;
			}
			if(milApp.BlobResult.dBlobXMin[i]<dMinX)
			{
				dMinX = milApp.BlobResult.dBlobXMin[i];
			}
			if(milApp.BlobResult.dBlobXMax[i]>dMaxX)
			{
				dMaxX = milApp.BlobResult.dBlobXMax[i];
			}
			if(milApp.BlobResult.dBlobYMin[i]<dMinY)
			{
				dMinY = milApp.BlobResult.dBlobYMin[i];
			}
			if(milApp.BlobResult.dBlobYMax[i]>dMaxY)
			{
				dMaxY = milApp.BlobResult.dBlobYMax[i];
			}
		}
		dCenterX = (dMinX + dMaxX)/2;
		dCenterY = (dMinY + dMaxY)/2;
		if(dCenterX>m_nImgWidth||dCenterY>m_nImgHeight)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	return false;
}

void CImgStatic::ImgSetParam(long lExposureTime,long lGain)
{
	m_pCamera.SetExposureTime(lExposureTime);
	m_pCamera.SetGain(lGain);
}

bool CImgStatic::SetDetectCameraParam(long lExposureTime,long lGain)
{
	if(!m_bDetectInit)     
		return false;
	if (theApp.m_tSysParam.detectCameraType==DETECT_CAMERA_MV)
	{
		if(m_pDetectCamera==NULL)
		{
			return false;
		}
		if(!m_pDetectCamera->setExposure(float(lExposureTime)))
		{
			return false;
		}
		if(!m_pDetectCamera->setGain(float(lGain)))
		{
			return false;
		}
	}
	else
	{
		if(m_pPointGreyCam==NULL)
		{
			return false;
		}
		if(!m_pPointGreyCam->SetShutter(float(lExposureTime)))
		{
			return false;
		}
		if(!m_pPointGreyCam->SetGain(float(lGain)))
		{
			return false;
		}
	}	
	return true;
}

bool CImgStatic::SetDetectCamRoi(unsigned int XOffset,unsigned int YOffset,unsigned int ImgWidth,unsigned int ImgHeight)
{
	if(!m_bDetectInit)     //2017-08-16
		return false;

	if(m_pDetectCamera==NULL)
	{
		return false;
	}
	m_pDetectCamera->SetROI(XOffset,YOffset,ImgWidth,ImgHeight);
	return true;
}

bool CImgStatic::DetectContinueGrab()
{
	if(!m_bDetectInit)     //2017-08-16
		return false;
	m_pDetectCamera->EnterContinueMode();
	return true;
}
//PG/MV CAMERA---2017-12-12
bool CImgStatic::DetectSnap()
{
	if (theApp.m_tSysParam.detectCameraType==DETECT_CAMERA_MV)
	{
		if(!m_bDetectInit)     
			return false;
		if (NULL==m_pDetectCamera)
		{
			return false;
		}
		m_pDetectCamera->SingleGrab();
		return true;
	}
	else
	{
		static bool bRun=false; 
		long ret;
		if (bRun)
		{
			return false;
		}
		bRun=true;
		if(m_pPointGreyCam == NULL)
		{
			AfxMessageBox("DetectSnap().m_pCamera = NULL");
			bRun=false;
			return false;
		}
		if (!m_bDetectInit)
		{
			bRun=false;
			return false;
		}
		try
		{
			ret=m_pPointGreyCam->Shot(m_pPointGreyCam->rawImage);
		}
		catch(...)
		{
			if (NULL!=g_pFrm)
			{
				CString str;
				str.Format("ImgStatic L922:DetectSnap().m_pPointGreyCam->Shot(m_pPointGreyCam->rawImage)触发异常.");
				g_pFrm->m_CmdRun.AddMsg(str);
				g_pFrm->m_CmdRun.PutLogIntoList(str);
			}
		}
		if (0!=ret)
		{
			bRun=false;
			return false;
		}
		// if grab success--->copy data:1.camera->mil buffer; 2.camera->image display buffer
		try
		{
			GetPointGreayImage(m_pPointGreyCam->m_pGrabImage,IMAGEWIDTH1,IMAGEHEIGHT1,ePGGrey8);
		}
		catch(...)
		{
			if (NULL!=g_pFrm)
			{
				CString str;
				str.Format("ImgStatic L942:DetectSnap().GetPointGreayImage(m_pPointGreyCam->m_pGrabImage,IMAGEWIDTH1,IMAGEHEIGHT1,ePGGrey8)出现异常...");
				g_pFrm->m_CmdRun.AddMsg(str);
				g_pFrm->m_CmdRun.PutLogIntoList(str);
			}
		}
		bRun=false;
		return true;
	}	
}
//PG/MV CAMERA---2017-12-12
bool CImgStatic::DetectStop()
{
	if(!m_bDetectInit)     
		return false;
	if (theApp.m_tSysParam.detectCameraType==DETECT_CAMERA_MV)
	{
		if (NULL!=m_pDetectCamera)
		{
			m_pDetectCamera->StopCapture();
		}
	}
	else
	{
		if(NULL!=m_pPointGreyCam)
		{
			m_pPointGreyCam->StopAcquire();
		}
	}
	return true;
}

//camera-> MIL
bool CImgStatic::CopyDetectImage(BYTE *pScrImage,long lWidth,long lHeight,PGPixelFormat pixelFormat)
{
	if (!m_bDetectInit)
	{
		return false;
	}
	if(pixelFormat==ePGGrey8) //拷贝到mil缓存
	{
		milApp.CopyBuffer(milApp.m_lPadDetectImage,pScrImage,lWidth,lHeight,RGB8);
	}
	else
	{
		milApp.CopyBuffer(milApp.m_lPadDetectImage,pScrImage,lWidth,lHeight,RGB24);
	}
	return true;
}
//camera->buffer(display)
bool CImgStatic::CopyDetectBuffer(BYTE *pScrBuffer,long lWidth,long lHeight,PGPixelFormat pixelFormat)
{
	if(!m_bDetectInit)
	{
		return false;
	}
	int nPixel = 1;
	if(pixelFormat!=ePGGrey8)
	{
		nPixel = 3;
	}
	if(m_lImgDetectWidth!=lWidth||m_lImgDetectHeight!=lHeight||m_nDetectPixel!=nPixel)
	{
		if(m_pDetectImageBuffer!=NULL)
		{
			delete[] m_pDetectImageBuffer;
			m_pDetectImageBuffer = NULL;
		}
		m_lImgDetectWidth = lWidth;
		m_lImgDetectHeight = lHeight;
		m_nDetectPixel = nPixel;
	}
	if(m_pDetectImageBuffer==NULL)
	{
		m_pDetectImageBuffer = new BYTE[m_lImgDetectWidth*m_lImgDetectHeight*m_nDetectPixel];
	}
	//Mono8 data: data-copy sequence is contrast...
	for(int i=0;i<m_lImgDetectHeight;i++)
	{
		memcpy(m_pDetectImageBuffer+i*m_lImgDetectWidth*m_nDetectPixel,pScrBuffer+(m_lImgDetectHeight-1-i)*m_lImgDetectWidth*m_nDetectPixel,m_lImgDetectWidth*m_nDetectPixel*sizeof(BYTE));
	}
	return true;
}

bool CImgStatic::CopyDetectBuffer(BYTE *pDestBuffer)
{
	if((m_pDetectImageBuffer!=NULL)&&(pDestBuffer!=NULL))
	{
		//EnterCriticalSection(&g_cs);//2017-12-12  del
		memcpy(pDestBuffer,m_pDetectImageBuffer,m_lImgDetectWidth*m_lImgDetectHeight*m_nDetectPixel*sizeof(BYTE));
		//LeaveCriticalSection(&g_cs);//2017-12-12  del
	}
	return true;
}
//对针头相机
void CImgStatic::ImgNeedleLive()
{
	m_pCameraNeedle.Live();
}
void CImgStatic::ImgNeedleStop()
{
	m_pCameraNeedle.Stop();
}
bool CImgStatic::CopyNeedleImage(int type)
{
	bool bReturn = false;
	if(type==RGB24)
	{
		if(m_pCameraNeedle.m_pImageBuffer!=NULL)
		{
			milApp.CopyBuffer(milApp.m_MilImage,m_pCameraNeedle.m_pImageBuffer,type);
			bReturn = true;
		}
	}
	else
	{
		if(m_pCameraNeedle.m_pRawBuffer!=NULL)
		{
			milApp.CopyBuffer(milApp.m_MilImage,m_pCameraNeedle.m_pRawBuffer,type);
			bReturn = true;
		}
	}
	return bReturn;
}

void CImgStatic::CopyNeedleBuffer(BYTE *pBuffer)
{
	if(m_pCameraNeedle.m_pImageBuffer!=NULL)
	{
		memcpy(pBuffer,m_pCameraNeedle.m_pImageBuffer,m_pCameraNeedle.m_nImageWidth*m_pCameraNeedle.m_nImageHeight*3);
	}
}
//PG相机回调函数；
void CImgStatic::DetectCameraDisplayCallback(void* pOwner,unsigned char* pImgData, long lWidth, long lHeight, PGPixelFormat pixelFormat)
{
	CImgStatic *pImgStatic = (CImgStatic*)pOwner;

	if (!pImgData)
	{
		return;	
	}
	EnterCriticalSection(&pImgStatic->m_csDetectCamera);
	pImgStatic->CopyDetectImage(pImgData,int(lWidth),int(lHeight),pixelFormat);
	pImgStatic->CopyDetectBuffer(pImgData,int(lWidth),int(lHeight),pixelFormat);
	LeaveCriticalSection(&pImgStatic->m_csDetectCamera);
}
//MV相机回调函数:LIVE MODE
void CImgStatic::DetectCameraMVCallback(void* pOwner, unsigned char* pImgData, long lWidth, long lHeight, PGPixelFormat pixelFormat)
{
	CImgStatic *pImgStatic = (CImgStatic*)pOwner;
	if (!pImgData)
	{
		return;	
	}
	EnterCriticalSection(&pImgStatic->m_csDetectCamera);
	pImgStatic->CopyDetectImage(pImgData,int(lWidth),int(lHeight),pixelFormat);
	pImgStatic->CopyDetectBuffer(pImgData,int(lWidth),int(lHeight),pixelFormat);
	LeaveCriticalSection(&pImgStatic->m_csDetectCamera);
}
void CImgStatic::ThreadDetectCameraModeSwitch()
{		
	g_pCameraModeSwitchThread=AfxBeginThread(CImgStatic::ThreadCameraModeSwitch,this);
	if (NULL==g_pCameraModeSwitchThread)
	{
		AfxMessageBox("DetectCamera模式切换线程创建失败！");
		return;
	}
}

//抓取PG相机图像:1.CAMERA->MIL;  2.CAMERA->IMAGE DISPLAY BUFFER
void CImgStatic::GetPointGreayImage(unsigned char* pImgData, long lWidth, long lHeight, PGPixelFormat pixelFormat)
{
	if (NULL==pImgData)
	{
		CString str;
		str.Format("PG相机采集图像失败:DetectCameraDisplayCallback pImgData为空");
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		return;	
	}
	//EnterCriticalSection(&g_cs);//2017-12-12  del
	CopyDetectImage(pImgData,int(lWidth),int(lHeight),pixelFormat);
	CopyDetectBuffer(pImgData,int(lWidth),int(lHeight),pixelFormat);
	//LeaveCriticalSection(&g_cs);//2017-12-12  del
}
void CImgStatic::DetectRelease()
{ 
	DetectStop();
	if (m_pPointGreyCam!=NULL)
	{
		delete m_pPointGreyCam;
		m_pPointGreyCam=NULL;
	}
}
bool CImgStatic::DetectReset()
{
	m_pPointGreyCam=new PointGreyCam;
	if (NULL==m_pPointGreyCam)
	{
		return false;
	}
	long ret=m_pPointGreyCam->OpenCamera(0,OFFSETX,OFFSETY,IMAGEWIDTH1,IMAGEHEIGHT1);
	if (0==ret)
	{
		m_bDetectInit=true;
	}
	else
	{
		m_bDetectInit=false;
	}
	if(m_bDetectInit)
	{
		if(!SetDetectCameraParam(theApp.m_tSysParam.CameraDetectExposureTime,theApp.m_tSysParam.CameraDetectGain))
		{
			if (NULL!=g_pFrm)
			{
				g_pFrm->m_CmdRun.AddMsg("Detect相机设置曝光时间或增益失败！");
			}
		}
	}
	else
	{
		if (NULL!=g_pFrm)
		{
			g_pFrm->m_CmdRun.AddMsg("Detect相机初始化失败！");
		}
	}
	return m_bImgInit;
}

//PG camera connect auto
void CImgStatic::DetectCamReConnect()
{
	if(theApp.m_tSysParam.detectCameraType==DETECT_CAMERA_PG)
	{
		DetectRelease();
		DetectReset();
	}
}




