// ImgMarkGeoSet.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>
#include "TSCtrlSys.h"
#include "ImgMarkGeoSet.h"


// CImgMarkGeoSet

IMPLEMENT_DYNAMIC(CImgMarkGeoSet, CStatic)

CImgMarkGeoSet::CImgMarkGeoSet()
{
	m_hDrawDib = DrawDibOpen();
	m_pBmpInfo = new BITMAPINFO[2048];
	m_pImgBuffer = NULL;
	m_pScerchImg = NULL;
	m_modtype = 0;
	m_GatherInit = FALSE;
	mBtmap = NULL;
	mBtmapBk = NULL;
	yOffset=0;
	memset(&m_mod_circle,0,sizeof(G_CIRCLE));
	memset(&m_mod_cross,0,sizeof(G_CROSS));
	memset(&m_mod_diamond,0,sizeof(G_DIAMOND));
	memset(&m_mod_edge,0,sizeof(G_EDGE_RESULT));
	memset(&m_mod_ellipse,0,sizeof(G_ELLIPSE));
	memset(&m_mod_rectangle,0,sizeof(G_RECTANGLE));
	memset(&m_mod_ring,0,sizeof(G_RING));
	memset(&m_mod_square,0,sizeof(G_MOD_SQUARE));
	memset(&m_mod_triangle,0,sizeof(G_MOD_TRIANGLE));

	m_RoiSel = FALSE;
	m_bMarkImage = false;
	m_bNeedleImage=false;
}

CImgMarkGeoSet::~CImgMarkGeoSet()
{
	if(m_hDrawDib != NULL)
	{
		DrawDibClose(m_hDrawDib);
		m_hDrawDib = NULL;
	}
	if(m_pBmpInfo != NULL)
	{
		delete []m_pBmpInfo;
		m_pBmpInfo=NULL;
	}
	if(mBtmapBk!=NULL)
	{
		delete mBtmapBk;
		mBtmapBk=NULL;
	}
	if(mBtmap!=NULL)	
	{
		delete mBtmap;
		mBtmap=NULL;
	}
}


BEGIN_MESSAGE_MAP(CImgMarkGeoSet, CStatic)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()



// CImgMarkGeoSet message handlers



void CImgMarkGeoSet::OnPaint()
{
	CPaintDC dc(this); 
	if(GetSafeHwnd() == NULL)return;

	CDC cdc;
	CBitmap bitmap;
	CRect rect,m_tImgRect;
	GetClientRect(&rect);

	cdc.CreateCompatibleDC(&dc);
	bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	CBitmap *pOldBit = cdc.SelectObject(&bitmap);
	//////////////////////////////////////////////////////////////////////////
	CBrush brush;
	brush.CreateSolidBrush(RGB(128,128,128));//背景
	CBrush *pOldBrush = cdc.SelectObject(&brush);
	cdc.SetBkMode(TRANSPARENT);
	cdc.Rectangle(rect);//绘制客户区矩形
	//计算图像显示窗体大小
	m_tImgRect = GetImgPaintRect(rect);
	//GetCmdScale();//计算图像和轨迹的比例

	CFont Font;
	Font.CreatePointFont(80, "Arial");
	CFont *pOldFont = cdc.SelectObject(&Font);
	cdc.SetTextColor(RGB(255,0,155));
	//////////////////////////////////////////////////////////////////////////
	PaintImage(&cdc, m_tImgRect);
	PaintDraw(&cdc, m_tImgRect);
	//////////////////////////////////////////////////////////////////////////
	dc.StretchBlt(0,0,rect.Width(), rect.Height(), &cdc, 0,0,rect.Width(), rect.Height(), SRCCOPY);

	//////////////////////////////////////////////////////////////////////////
	cdc.SelectObject(pOldFont);
	cdc.SelectObject(pOldBrush);
	cdc.SelectObject(pOldBit);
	bitmap.DeleteObject();
	cdc.DeleteDC();
}

void CImgMarkGeoSet::PaintImage(CDC *pDC, CRect rect)
{
	if(m_pImgBuffer == NULL) return;
	//if(m_GatherSign)

	//g_pView->m_ImgStatic.CopyBuffer(m_pImgBuffer);
	if (m_bNeedleImage)
	{
		g_pView->m_ImgStatic.CopyNeedleBuffer(m_pImgBuffer);
	}
	else
	{
		if(m_bMarkImage)
		{
			g_pView->m_ImgStatic.CopyBuffer(m_pImgBuffer);
		}
		else
		{
			g_pView->m_ImgStatic.CopyDetectBuffer(m_pImgBuffer);
		}

	}
	/* GDI+ 显示图像 */
	/*if(!m_GatherInit)
	{
	m_GatherInit = TRUE;
	if(mBtmapBk!=NULL)
	delete mBtmapBk;
	if(mBtmap!=NULL)
	delete mBtmap;
	mBtmapBk = new Bitmap(m_pBmpInfo,m_pImgBuffer);
	mBtmap = new Bitmap(m_pBmpInfo,g_pView->m_ImgStatic.m_pCamera.m_pImageBuffer);
	imageAttr.SetColorKey(Color(0,0,0),Color(0,0,0),ColorAdjustTypeDefault);
	destRect.X=0;
	destRect.Y=0;
	destRect.Width=rect.Width();
	destRect.Height=rect.Height();
	}*/
	//if(mBtmap==NULL || mBtmapBk==NULL) return;
	Graphics gr(pDC->m_hDC);
	//gr.SetSmoothingMode(SmoothingModeDefault);
	//gr.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	if(m_iCount>0 && m_GatherSign==FALSE)
	{
		//g_pView->m_ImgStatic.GetImage(m_pImgBuffer);
		g_pView->m_ImgStatic.CopyBuffer(m_pImgBuffer);
		//DrawSearch(&cdc,m_tImgRect);
		/*DrawDibDraw(m_hDrawDib, pDC->GetSafeHdc(),
		rect.left, rect.top, rect.Width(), rect.Height(),
		&m_pBmpInfo->bmiHeader,
		m_pImgBuffer,
		0,0,
		m_pBmpInfo->bmiHeader.biWidth,
		m_pBmpInfo->bmiHeader.biHeight, SRCCOPY);*/
		/*DrawDibDraw(m_hDrawDib, pDC->GetSafeHdc(),
		rect.left, rect.top, rect.Width(), rect.Height(),
		&m_pBmpInfo->bmiHeader,
		g_pView->m_ImgStatic.m_pSearchBuffer,
		0,0,
		m_pBmpInfo->bmiHeader.biWidth,
		m_pBmpInfo->bmiHeader.biHeight, SRCINVERT);*/
		gr.DrawImage(mBtmapBk,0,0,rect.Width(),rect.Height());
		gr.DrawImage(mBtmap,destRect,0,0,1280,1024,UnitPixel,&imageAttr,NULL,NULL);

		//OutString
		CString str_x,str_y,str_scale,str_count,str_score,str_time,str_angle;
		str_x.Format("     X : %0.3lf",pXPos[0]);
		str_y.Format("     Y : %0.3lf",pYPos[0]);
		str_scale.Format("比例 : %0.3lf",pScale[0]);
		str_score.Format("分数 : %0.3lf",pScore[0]);
		str_time.Format("时间 : %0.3lf ms",m_dTime*1000);
		str_angle.Format("角度 : %0.3lf",pAng[0]);
		Gr_DrawText(&gr,20,10,str_x);
		Gr_DrawText(&gr,20,30,str_y);
		Gr_DrawText(&gr,20,50,str_scale);
		Gr_DrawText(&gr,20,70,str_score);
		Gr_DrawText(&gr,20,90,str_angle);
		Gr_DrawText(&gr,20,110,str_time);
	}
	else
	{
		//gr.DrawImage(mBtmapBk,0,0,rect.Width(),rect.Height());
		DrawDibDraw(m_hDrawDib, pDC->GetSafeHdc(),
			rect.left, rect.top, rect.Width(), rect.Height(),
			&m_pBmpInfo->bmiHeader,
			m_pImgBuffer,
			0,yOffset,
			m_pBmpInfo->bmiHeader.biWidth,
			(m_pBmpInfo->bmiHeader.biHeight-yOffset), SRCCOPY);
	}
}

void CImgMarkGeoSet::Gr_DrawText(Graphics *gr,int x,int y,CString str,Color cl/* =Color */)
{
	int StrLne = str.GetLength();
	StringFormat strFormat;
	WCHAR tmpch[100];
	memset(tmpch,0,sizeof(WCHAR)*100);
	if (StrLne < 100)
	{
		MultiByteToWideChar(CP_ACP,0,str,-1,tmpch,100);
	}
	else
	{
		ASSERT(FALSE);
		return;
	}
	Gdiplus::Font textF(tmpch,16,FontStyleBold, UnitPoint, NULL);
	strFormat.SetFormatFlags(StringFormatFlagsNoClip);
	SolidBrush brText(cl); 
	gr->DrawString(tmpch, 
		StrLne, 
		&textF, 
		RectF(REAL(x), 
		(REAL)(y), 
		16*StrLne, 
		16),
		&strFormat, 
		&brText);
}

CRect CImgMarkGeoSet::GetImgPaintRect(CRect rect)
{
	double dxScale = double(rect.Width())/m_iImgWidth;
	double dyScale = double(rect.Height())/m_iImgHeight;
	m_dScale = dxScale>dyScale ? dyScale : dxScale;

	CRect rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = static_cast<long>(m_dScale*m_iImgWidth);
	rc.bottom = static_cast<long>(m_dScale*m_iImgHeight);
	return rc;
}

void CImgMarkGeoSet::ImgInit(long lImageWidth,long lImageHeight,int nPixel)
{
	//	初始化BITMAPINFO 结构，此结构在保存bmp文件、显示采集图像时使用
	m_pBmpInfo->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	//	图像宽度，一般为输出窗口宽度
	m_pBmpInfo->bmiHeader.biWidth			= lImageWidth;
	//	图像宽度，一般为输出窗口高度
	m_pBmpInfo->bmiHeader.biHeight			= lImageHeight;
	/*
	*	以下设置一般相同，
	*	对于低于8位的位图，还应设置相应的位图调色板
	*/
	m_pBmpInfo->bmiHeader.biPlanes			= 1;
	m_pBmpInfo->bmiHeader.biBitCount		= nPixel;
	m_pBmpInfo->bmiHeader.biCompression		= BI_RGB;
	m_pBmpInfo->bmiHeader.biSizeImage		= 0;
	m_pBmpInfo->bmiHeader.biXPelsPerMeter	= 0;
	m_pBmpInfo->bmiHeader.biYPelsPerMeter	= 0;
	m_pBmpInfo->bmiHeader.biClrUsed			= 0;
	m_pBmpInfo->bmiHeader.biClrImportant	= 0;
	if(nPixel==8)
	{
		for(int i=0;i<256;i++)
		{
			m_pBmpInfo->bmiColors[i].rgbRed = (UINT8)i;
			m_pBmpInfo->bmiColors[i].rgbGreen = (UINT8)i;
			m_pBmpInfo->bmiColors[i].rgbBlue = (UINT8)i;
			m_pBmpInfo->bmiColors[i].rgbReserved = 0;
		}
	}

	SetTimer(911,100,NULL);
}

void CImgMarkGeoSet::PaintDraw(CDC *pDC, CRect rect)
{
	CPen grePen(PS_SOLID, 1, RGB(0, 255, 0));
	CPen blPen(PS_SOLID, 1, RGB(0, 0, 255));
	CPen *pOldPen=pDC->SelectObject(&grePen);
	CPen redPen(PS_SOLID, 1, RGB(255, 0, 0));
	//////////////////////////////////////////////////////////////////////////
	//Paint
	//if(m_nCurStep==1)
	{
		switch(m_modtype)
		{
		case MOD_CIRCLE:
			PaintCircle(pDC,rect);
			break;
		case MOD_CROSS:
			PaintCross(pDC,rect);
			break;
		case MOD_DIAMOND:
			PaintDiamond(pDC,rect);
			break;
		case MOD_ELLIPSE:
			PaintEllipse(pDC,rect);
			break;
		case MOD_RECTANGLE:
			PaintRectangle(pDC,rect);
			break;
		case MOD_RING:
			PaintRing(pDC,rect);
			break;
		case MOD_SQUARE:
			PaintSquare(pDC,rect);
			break;
		case MOD_TRIANGLE:
			PaintTriangle(pDC,rect);
			break;
		case MOD_IMAGE:
			PaintImage2(pDC,rect);
			break;
		default:
			break;
		}
		pDC->SelectObject(&blPen);
		PaintRoiRect(pDC,rect);
		pDC->SelectObject(&redPen);
		PaintScale(pDC,rect);
	}
	/*else if(m_nCurStep==2)
	{
		pDC->SelectObject(&blPen);
		PaintRoiRect(pDC,rect);
	}*/
	//////////////////////////////////////////////////////////////////////////
	pDC->SelectObject(pOldPen);
}

void CImgMarkGeoSet::PaintCross(CDC *pDC, CRect rect)
{
	CPoint cp;
	double dx = m_mod_cross.dx*m_dScale;
	double dy = m_mod_cross.dy*m_dScale;

	CPoint tp[12];
	tp[0].x = long(dx + (m_mod_cross.horizontal/2)*m_dScale+0.5);
	tp[0].y = long(dy + (m_mod_cross.height/2)*m_dScale+0.5);
	tp[1].x = long(dy - (m_mod_cross.horizontal/2)*m_dScale+0.5);
	tp[1].y = tp[0].y;
	tp[2].x = tp[1].x;
	tp[2].y = long(dy + (m_mod_cross.vertical/2)*m_dScale+0.5);
	tp[3].x = long(dx - (m_mod_cross.width/2)*m_dScale+0.5);
	tp[3].y = tp[2].y;
	tp[4].x = tp[3].x;
	tp[4].y = long(dy - (m_mod_cross.vertical/2)*m_dScale+0.5);
	tp[5].x = tp[1].x;
	tp[5].y = tp[4].y;
	tp[6].x = tp[5].x;
	tp[6].y = long(dy - (m_mod_cross.height/2)*m_dScale+0.5);
	tp[7].x = tp[0].x;
	tp[7].y = tp[6].y;
	tp[8].x = tp[7].x;
	tp[8].y = tp[5].y;
	tp[9].x = long(dx + (m_mod_cross.width/2)*m_dScale+0.5);
	tp[9].y = tp[5].y;
	tp[10].x = tp[9].x;
	tp[10].y = tp[2].y;
	tp[11].x = tp[0].x;
	tp[11].y = tp[2].y;

	pDC->MoveTo(tp[11].x,tp[11].y);
	for(int i=0;i<12;i++)
		pDC->LineTo(tp[i].x,tp[i].y);
	DrawSquare(pDC,CPoint(tp[10].x,tp[0].y),6);
	DrawSquare(pDC,CPoint(tp[0].x,tp[10].y),6);
}

void CImgMarkGeoSet::PaintTriangle(CDC *pDC, CRect rect)
{
	double dx = m_mod_triangle.dx*m_dScale;
	double dy = m_mod_triangle.dy*m_dScale;
	CPoint tp[3];
	tp[0].x = long(dx+0.5);
	tp[0].y = long(dy - m_mod_triangle.height/2*m_dScale+0.5);
	tp[1].x = long(dx + m_mod_triangle.width/2*m_dScale+0.5);
	tp[1].y = long(dy + m_mod_triangle.height/2*m_dScale+0.5);
	tp[2].x = long(dx -  m_mod_triangle.width/2*m_dScale+0.5);
	tp[2].y = tp[1].y;
	pDC->MoveTo(tp[2].x,tp[2].y);
	for(int i=0;i<3;i++)
		pDC->LineTo(tp[i].x,tp[i].y);
	DrawSquare(pDC,CPoint(tp[1].x,tp[1].y),6);
}

void CImgMarkGeoSet::PaintSquare(CDC *pDC, CRect rect)
{
	CPoint cp;
	double dx = m_mod_square.dx*m_dScale;
	double dy = m_mod_square.dy*m_dScale;
	double len = m_mod_square.length/2*m_dScale;
	CPoint tp[4];
	tp[0].x = long(dx - m_mod_square.length/2*m_dScale+0.5);
	tp[0].y = long(dy - m_mod_square.length/2*m_dScale+0.5);
	tp[1].x = long(dx + m_mod_square.length/2*m_dScale+0.5);
	tp[1].y = tp[0].y;
	tp[2].x = tp[1].x;
	tp[2].y = long(dy + m_mod_square.length/2*m_dScale+0.5);
	tp[3].x = tp[0].x;
	tp[3].y = tp[2].y;
	pDC->MoveTo(tp[3].x,tp[3].y);
	for(int i=0;i<4;i++)
		pDC->LineTo(tp[i].x,tp[i].y);
	DrawSquare(pDC,CPoint(int(dx+len+0.5),int(dy+len+0.5)),6);
}

void CImgMarkGeoSet::PaintRing(CDC *pDC, CRect rect)
{
	double dx = m_mod_ring.dx*m_dScale;
	double dy = m_mod_ring.dy*m_dScale;
	double ir = m_mod_ring.in_ring*m_dScale;
	double or = m_mod_ring.out_ring*m_dScale;
	CPoint cp;
	cp.x = long(dx+0.5);
	cp.y = long(dy+0.5);

	DrawCircle(pDC, cp, 2);
	DrawCircle(pDC, cp, int(ir+0.5), FALSE);
	DrawCircle(pDC, cp, int(or+0.5), FALSE);
	DrawSquare(pDC,CPoint(int(dx+ir+0.5),int(dy+0.5)),6);
	DrawSquare(pDC,CPoint(int(dx+or+0.5),int(dy+0.5)),6);
}

void CImgMarkGeoSet::PaintRectangle(CDC *pDC, CRect rect)
{
	double dx = m_mod_rectangle.dx*m_dScale;
	double dy = m_mod_rectangle.dy*m_dScale;
	CPoint tp[4];
	tp[0].x = long(dx - (m_mod_rectangle.width/2)*m_dScale+0.5);
	tp[0].y = long(dy - (m_mod_rectangle.height/2)*m_dScale+0.5);
	tp[1].x = long(dx + (m_mod_rectangle.width/2)*m_dScale+0.5);
	tp[1].y = tp[0].y;
	tp[2].x = tp[1].x;
	tp[2].y = long(dy + (m_mod_rectangle.height/2)*m_dScale+0.5);
	tp[3].x = tp[0].x;
	tp[3].y = tp[2].y;
	pDC->MoveTo(tp[3].x,tp[3].y);
	for(int i=0;i<4;i++)
		pDC->LineTo(tp[i].x,tp[i].y);
	DrawSquare(pDC,CPoint(tp[2].x,tp[2].y),6);
}

void CImgMarkGeoSet::PaintRoiRect(CDC *pDC, CRect rect)
{
	CPoint tp[4];
	tp[0].x = long(RoiRect.left*m_dScale+0.5);
	tp[0].y = long(RoiRect.top*m_dScale+0.5);
	tp[1].x = long(RoiRect.right*m_dScale+0.5);
	tp[1].y = long(RoiRect.top*m_dScale+0.5);
	tp[2].x = long(RoiRect.right*m_dScale+0.5);
	tp[2].y = long(RoiRect.bottom*m_dScale+0.5);
	tp[3].x = long(RoiRect.left*m_dScale+0.5);
	tp[3].y = long(RoiRect.bottom*m_dScale+0.5);
	pDC->MoveTo(tp[3].x,tp[3].y);
	for(int i=0;i<4;i++)
		pDC->LineTo(tp[i].x,tp[i].y);
	TRACE("ROI x:%d y:%d width:%d height:%d\n",RoiRect.left,RoiRect.top,RoiRect.Width(),RoiRect.Height());
	DrawSquare(pDC,CPoint(tp[2].x,tp[2].y),6);
}

void CImgMarkGeoSet::PaintScale(CDC *pDC, CRect rect)
{
	CPoint point;
	point.x = rect.TopLeft().x + (rect.Width()-1)/2;
	point.y = rect.TopLeft().y + (rect.Height()-1)/2;
	long width = long(m_mod_scale.width*m_dScale+0.5);
	long height = long(m_mod_scale.height*m_dScale+0.5);
	long length = long(m_mod_scale.length*m_dScale+0.5);
	pDC->MoveTo(point.x,rect.TopLeft().y);
	pDC->LineTo(point.x,rect.BottomRight().y);//VERTIVAL
	pDC->MoveTo(rect.TopLeft().x,point.y);
	pDC->LineTo(rect.BottomRight().x,point.y);//HORIZINE
	for(long i=point.x-length;i>0;i=i-length) //HIRIZONE 
	{
		pDC->MoveTo(i,point.y-height/2);
		pDC->LineTo(i,point.y+height/2);//FRONT HALF
		pDC->MoveTo(point.x*2-i,point.y-height/2);
		pDC->LineTo(point.x*2-i,point.y+height/2);//BEHIND HALF
	}
	for(long i=point.y-length;i>0;i=i-length)
	{
		pDC->MoveTo(point.x-width/2,i);
		pDC->LineTo(point.x+width/2,i);
		pDC->MoveTo(point.x-width/2,point.y*2-i);
		pDC->LineTo(point.x+width/2,point.y*2-i);
	}
}

void CImgMarkGeoSet::PaintImage2(CDC *pDC, CRect rect)
{
	//TRACE("PaintImg_x%0.3lf_y%0.3lf_w%0.3lf_h%0.3lf\n",m_mod_image.x_offset,m_mod_image.y_offset,m_mod_image.width,m_mod_image.height);
	/*CPoint cp;
	cp.x = long((m_mod_image.x_offset+(m_mod_image.width/2))*m_dScale);
	cp.y = long((m_mod_image.y_offset+(m_mod_image.height/2))*m_dScale);*/
	CPoint tp[4];
	tp[0].x = long(m_mod_image.x_offset*m_dScale+0.5);
	tp[0].y = long(m_mod_image.y_offset*m_dScale+0.5);
	tp[1].x = long((m_mod_image.x_offset+m_mod_image.width)*m_dScale+0.5);
	tp[1].y = tp[0].y;
	tp[2].x = tp[1].x;
	tp[2].y = long((m_mod_image.y_offset+m_mod_image.height)*m_dScale+0.5);
	tp[3].x = tp[0].x;
	tp[3].y = tp[2].y;
	pDC->MoveTo(tp[3].x,tp[3].y);
	for(int i=0;i<4;i++)
		pDC->LineTo(tp[i].x,tp[i].y);
	DrawSquare(pDC,CPoint(tp[2].x,tp[2].y),6);
}

void CImgMarkGeoSet::PaintEllipse(CDC *pDC, CRect rect)
{
	double dx = m_mod_ellipse.dx*m_dScale;
	double dy = m_mod_ellipse.dy*m_dScale;
	CPoint cp;
	cp.x = long(dx+0.5);
	cp.y = long(dy+0.5);
	CGdiObject *pOldGdi = pDC->SelectStockObject(NULL_BRUSH);
	pDC->Ellipse(
				int(cp.x-m_mod_ellipse.width/2.0*m_dScale+0.5),
				int(cp.y-m_mod_ellipse.height/2.0*m_dScale+0.5),
				int(cp.x+m_mod_ellipse.width/2.0*m_dScale+0.5),
				int(cp.y+m_mod_ellipse.height/2.0*m_dScale+0.5));
	pDC->SelectObject(pOldGdi);
	DrawCircle(pDC, cp, 2);
	DrawSquare(pDC,CPoint(int(dx+m_mod_ellipse.width/2.0*m_dScale+0.5),int(dy+m_mod_ellipse.height/2.0*m_dScale+0.5)),6);
}

void CImgMarkGeoSet::PaintDiamond(CDC *pDC, CRect rect)
{
	double dx = m_mod_diamond.dx*m_dScale;
	double dy = m_mod_diamond.dy*m_dScale;
	CPoint cp;
	cp.x = long(dx+0.5);
	cp.y = long(dy+0.5);
	CPoint tp[4];
	tp[0].x = cp.x;
	tp[0].y = long(dy + m_mod_diamond.height/2.0*m_dScale+0.5);
	tp[1].x = long(dx - m_mod_diamond.width/2.0*m_dScale+0.5);
	tp[1].y = cp.y;
	tp[2].x = cp.x;
	tp[2].y = long(dy - m_mod_diamond.height/2.0*m_dScale+0.5);
	tp[3].x = long(dx + m_mod_diamond.width/2.0*m_dScale+0.5);
	tp[3].y = cp.y;
	pDC->MoveTo(tp[3].x,tp[3].y);
	for(int i=0;i<4;i++)
		pDC->LineTo(tp[i].x,tp[i].y);
	DrawSquare(pDC,CPoint(int(dx+m_mod_diamond.width/2.0*m_dScale+0.5),int(dy+m_mod_diamond.height/2.0*m_dScale+0.5)),6);
}

void CImgMarkGeoSet::DrawCircle(CDC *pDC, CPoint point, int r, BOOL bFull)
{
	if(!bFull)
	{
		CGdiObject *pOldGdi = pDC->SelectStockObject(NULL_BRUSH);
		pDC->Ellipse(point.x-r, point.y-r, point.x+r, point.y+r);
		pDC->SelectObject(pOldGdi);
	}
	else
	{
		pDC->Ellipse(point.x-r, point.y-r, point.x+r, point.y+r);
	}
}

void CImgMarkGeoSet::DrawCircleEx(CDC *pDC, CPoint point, int r, BOOL bFull)
{
	CBrush brush(RGB(240,0,0));
	CBrush *pOldBrush = pDC->SelectObject(&brush);
	if(!bFull)
	{
		CGdiObject *pOldGdi = pDC->SelectStockObject(NULL_BRUSH);
		pDC->Ellipse(point.x-r, point.y-r, point.x+r, point.y+r);
		pDC->SelectObject(pOldGdi);
	}
	else
	{
		pDC->Ellipse(point.x-r, point.y-r, point.x+r, point.y+r);
	}
	pDC->SelectObject(pOldBrush);
}

void CImgMarkGeoSet::DrawSquare(CDC *pDC, CPoint point, int len, BOOL bFull/* =TRUE */)
{
	CPen pen(PS_SOLID, 1, RGB(0,255,255));
	CPen *pOldPen = pDC->SelectObject(&pen);
	CBrush brush,*pOldbr;
	brush.CreateSolidBrush(RGB(128,128,128));
	pOldbr=pDC->SelectObject(&brush);
	pDC->Rectangle(point.x-len,point.y-len,point.x+len,point.y+len);
	pDC->SelectObject(pOldbr);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

void CImgMarkGeoSet::PaintCircle(CDC *pDC, CRect rect)
{
	CPoint cp;
	cp.x = long(m_mod_circle.dx*m_dScale);
	cp.y = long(m_mod_circle.dy*m_dScale);
	int r = int(m_mod_circle.r*m_dScale);

	DrawCircle(pDC, cp, 2);
	DrawCircle(pDC, cp, r, FALSE);
	DrawSquare(pDC,CPoint(cp.x+r,cp.y),6);
}

void CImgMarkGeoSet::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_BtSel && !m_RoiSel)
	{
		switch(m_modtype)
		{
		case MOD_CIRCLE:
			//{
			//	if(m_PtSelType==1)
			//	{
			//		m_mod_circle.r = g_pView->m_ImgStatic.m_Geo.Distance(m_mod_circle.dx,m_mod_circle.dy,point.x/m_dScale,point.y/m_dScale);
			//	}
			//	else if(m_PtSelType==2)
			//	{
			//		int dx = point.x-m_StPoint.x;
			//		int dy = point.y-m_StPoint.y;
			//		m_mod_circle.dx += double(dx/m_dScale);
			//		m_mod_circle.dy += double(dy/m_dScale);
			//		m_StPoint = point;
			//	}
			//	Invalidate();
			//}
			break;
		case MOD_CROSS:
			{
				if(m_PtSelType==1)
				{
					m_mod_cross.width = fabs(m_mod_cross.dx-point.x/m_dScale)*2;
					m_mod_cross.height = fabs(m_mod_cross.dy-point.y/m_dScale)*2;
				}
				else if(m_PtSelType==2)
				{
					m_mod_cross.horizontal = fabs(m_mod_cross.dx-point.x/m_dScale)*2;
					m_mod_cross.vertical = fabs(m_mod_cross.dy-point.y/m_dScale)*2;
				}
				else if(m_PtSelType==3)
				{
					int dx = point.x-m_StPoint.x;
					int dy = point.y-m_StPoint.y;
					m_mod_cross.dx += double(dx/m_dScale);
					m_mod_cross.dy += double(dy/m_dScale);
					m_StPoint = point;
				}
				Invalidate();
			}
			break;
		case MOD_DIAMOND:
			{
				if(m_PtSelType==1)
				{
					m_mod_diamond.width = fabs(m_mod_diamond.dx-point.x/m_dScale)*2;
					m_mod_diamond.height = fabs(m_mod_diamond.dy-point.y/m_dScale)*2;
				}
				else if(m_PtSelType==2)
				{
					int dx = point.x-m_StPoint.x;
					int dy = point.y-m_StPoint.y;
					m_mod_diamond.dx += double(dx/m_dScale);
					m_mod_diamond.dy += double(dy/m_dScale);
					m_StPoint = point;
				}
				Invalidate();
			}
			break;
		case MOD_ELLIPSE:
			{
				if(m_PtSelType==1)
				{
					m_mod_ellipse.width = fabs(m_mod_ellipse.dx-point.x/m_dScale)*2;
					m_mod_ellipse.height = fabs(m_mod_ellipse.dy-point.y/m_dScale)*2;
				}
				else if(m_PtSelType==2)
				{
					int dx = point.x-m_StPoint.x;
					int dy = point.y-m_StPoint.y;
					m_mod_ellipse.dx += double(dx/m_dScale);
					m_mod_ellipse.dy += double(dy/m_dScale);
					m_StPoint = point;
				}
				Invalidate();
			}
			break;
		case MOD_RECTANGLE:
			{
				if(m_PtSelType==1)
				{
					double tdx,tdy;
					tdx = m_mod_rectangle.dx - m_mod_rectangle.width/2;
					tdy = m_mod_rectangle.dy - m_mod_rectangle.height/2;
					m_mod_rectangle.width = point.x/m_dScale - tdx;
					m_mod_rectangle.height = point.y/m_dScale - tdy;
					m_mod_rectangle.dx = tdx + m_mod_rectangle.width/2;
					m_mod_rectangle.dy = tdy + m_mod_rectangle.height/2;
				}
				else if(m_PtSelType==2)
				{
					int dx = point.x-m_StPoint.x;
					int dy = point.y-m_StPoint.y;
					m_mod_rectangle.dx += double(dx/m_dScale);
					m_mod_rectangle.dy += double(dy/m_dScale);
					m_StPoint = point;
				}
				Invalidate();
			}
			break;
		case MOD_RING:
			//{
			//	if(m_PtSelType==1)
			//	{
			//		m_mod_ring.in_ring = g_pView->m_ImgStatic.m_Geo.Distance(m_mod_ring.dx,m_mod_ring.dy,point.x/m_dScale,point.y/m_dScale);
			//	}
			//	else if(m_PtSelType==2)
			//	{
			//		m_mod_ring.out_ring = g_pView->m_ImgStatic.m_Geo.Distance(m_mod_ring.dx,m_mod_ring.dy,point.x/m_dScale,point.y/m_dScale);
			//	}
			//	else if(m_PtSelType==3)
			//	{
			//		int dx = point.x-m_StPoint.x;
			//		int dy = point.y-m_StPoint.y;
			//		m_mod_ring.dx += double(dx/m_dScale);
			//		m_mod_ring.dy += double(dy/m_dScale);
			//		m_StPoint = point;
			//	}
			//	Invalidate();
			//}
			break;
		case MOD_SQUARE:
			{
				if(m_PtSelType==1)
				{
					m_mod_square.length = fabs(point.x/m_dScale-m_mod_square.dx)*2;//g_pView->m_ImgStatic.m_Geo.Distance(m_mod_square.dx,m_mod_square.dy,point.x/m_dScale,point.y/m_dScale);
				}
				else if(m_PtSelType==2)
				{
					int dx = point.x-m_StPoint.x;
					int dy = point.y-m_StPoint.y;
					m_mod_square.dx += double(dx/m_dScale);
					m_mod_square.dy += double(dy/m_dScale);
					m_StPoint = point;
				}
				Invalidate();
			}
			break;
		case MOD_TRIANGLE:
			{
				if(m_PtSelType==1)
				{
					m_mod_triangle.width = fabs(m_mod_triangle.dx-point.x/m_dScale)*2;
					m_mod_triangle.height = fabs(m_mod_triangle.dy-point.y/m_dScale)*2;
				}
				else if(m_PtSelType==2)
				{
					int dx = point.x-m_StPoint.x;
					int dy = point.y-m_StPoint.y;
					m_mod_triangle.dx += double(dx/m_dScale);
					m_mod_triangle.dy += double(dy/m_dScale);
					m_StPoint = point;
				}
				Invalidate();
			}
			break;
		case MOD_IMAGE:
			{
				if(m_PtSelType==1)
				{
					double cx,cy;
					cx = m_mod_image.x_offset + m_mod_image.width/2;
					cy = m_mod_image.y_offset + m_mod_image.height/2;
					m_mod_image.width = fabs(m_mod_image.x_offset-point.x/m_dScale);
					m_mod_image.height = fabs(m_mod_image.y_offset-point.y/m_dScale);
					m_mod_image.x_offset = cx - m_mod_image.width/2;
					m_mod_image.y_offset = cy - m_mod_image.height/2;
				}
				else if(m_PtSelType==2)
				{
					int dx = point.x-m_StPoint.x;
					int dy = point.y-m_StPoint.y;
					m_mod_image.x_offset += double(dx/m_dScale);
					m_mod_image.y_offset += double(dy/m_dScale);
					m_StPoint = point;
				}
				Invalidate();
			}
			break;
		default:
			break;
		}
	}
	else if(m_BtSel && m_RoiSel)
	{
		if(m_PtSelType==1)
		{
			RoiRect.right = long(point.x/m_dScale);
			RoiRect.bottom = long(point.y/m_dScale);
		}
		else if(m_PtSelType==2)
		{
			int dx = point.x-m_StPoint.x;
			int dy = point.y-m_StPoint.y;
			RoiRect.left += long(dx/m_dScale);
			RoiRect.right += long(dx/m_dScale);
			RoiRect.top += long(dy/m_dScale);
			RoiRect.bottom += long(dy/m_dScale);
			m_StPoint = point;
		}
		ResetParam();
		Invalidate();
	}
	CStatic::OnMouseMove(nFlags, point);
}

void CImgMarkGeoSet::OnLButtonDown(UINT nFlags, CPoint point)
{
	CPoint cp;
	//if(m_nCurStep==1)
	{
		switch(m_modtype)
		{
		case MOD_CIRCLE:
			//{
			//	cp.x = long((m_mod_circle.dx+m_mod_circle.r)*m_dScale);
			//	cp.y = long(m_mod_circle.dy*m_dScale);
			//	if(g_pView->m_ImgStatic.m_Geo.Distance(cp.x,cp.y,point.x,point.y)<=6)
			//	{
			//		m_PtSelType = 1;
			//		m_BtSel = TRUE;
			//		m_StPoint = point;
			//	}
			//	else
			//	{
			//		cp.x = long(m_mod_circle.dx*m_dScale);
			//		cp.y = long(m_mod_circle.dy*m_dScale);
			//		if(g_pView->m_ImgStatic.m_Geo.Distance(cp.x,cp.y,point.x,point.y)<=(m_mod_circle.r*m_dScale))
			//		{
			//			m_PtSelType = 2;
			//			m_BtSel = TRUE;
			//			m_StPoint = point;
			//		}
			//	}
			//}
			break;
		case MOD_CROSS:
			//{
			//	cp.x = long((m_mod_cross.dx+m_mod_cross.width/2)*m_dScale);
			//	cp.y = long((m_mod_cross.dy+m_mod_cross.height/2)*m_dScale);
			//	if(g_pView->m_ImgStatic.m_Geo.Distance(cp.x,cp.y,point.x,point.y)<=6)
			//	{
			//		m_PtSelType = 1;
			//		m_BtSel = TRUE;
			//		m_StPoint = point;
			//	}
			//	else
			//	{
			//		cp.x = long((m_mod_cross.dx+m_mod_cross.horizontal/2)*m_dScale);
			//		cp.y = long((m_mod_cross.dy+m_mod_cross.vertical/2)*m_dScale);
			//		if(g_pView->m_ImgStatic.m_Geo.Distance(cp.x,cp.y,point.x,point.y)<=6)
			//		{
			//			m_PtSelType = 2;
			//			m_BtSel = TRUE;
			//			m_StPoint = point;
			//		}
			//		else
			//		{
			//			cp.x = long(m_mod_cross.dx*m_dScale);
			//			cp.y = long(m_mod_cross.dy*m_dScale);
			//			if(abs(point.x-cp.x)<=(m_mod_cross.width/2*m_dScale) && abs(point.y-cp.y)<=(m_mod_cross.height/2*m_dScale))
			//			{
			//				m_PtSelType = 3;
			//				m_BtSel = TRUE;
			//				m_StPoint = point;
			//			}
			//		}
			//	}
			//}
			break;
		case MOD_DIAMOND:
			//{
			//	cp.x = long((m_mod_diamond.dx+m_mod_diamond.width/2)*m_dScale);
			//	cp.y = long((m_mod_diamond.dy+m_mod_diamond.height/2)*m_dScale);
			//	if(g_pView->m_ImgStatic.m_Geo.Distance(cp.x,cp.y,point.x,point.y)<=6)
			//	{
			//		m_PtSelType = 1;
			//		m_BtSel = TRUE;
			//		m_StPoint = point;
			//	}
			//	else
			//	{
			//		cp.x = long(m_mod_diamond.dx*m_dScale);
			//		cp.y = long(m_mod_diamond.dy*m_dScale);
			//		if(abs(point.x-cp.x)<=(m_mod_diamond.width/2*m_dScale) && abs(point.y-cp.y)<=(m_mod_diamond.height/2*m_dScale))
			//		{
			//			m_PtSelType = 2;
			//			m_BtSel = TRUE;
			//			m_StPoint = point;
			//		}
			//	}
			//}
			break;
		case MOD_ELLIPSE:
			//{
			//	cp.x = long((m_mod_ellipse.dx+m_mod_ellipse.width/2)*m_dScale);
			//	cp.y = long((m_mod_ellipse.dy+m_mod_ellipse.height/2)*m_dScale);
			//	if(g_pView->m_ImgStatic.m_Geo.Distance(cp.x,cp.y,point.x,point.y)<=6)
			//	{
			//		m_PtSelType = 1;
			//		m_BtSel = TRUE;
			//		m_StPoint = point;
			//	}
			//	else
			//	{
			//		cp.x = long(m_mod_ellipse.dx*m_dScale);
			//		cp.y = long(m_mod_ellipse.dy*m_dScale);
			//		if(abs(point.x-cp.x)<=(m_mod_ellipse.width/2*m_dScale) && abs(point.y-cp.y)<=(m_mod_ellipse.height/2*m_dScale))
			//		{
			//			m_PtSelType = 2;
			//			m_BtSel = TRUE;
			//			m_StPoint = point;
			//		}
			//	}
			//}
			break;
		case MOD_RECTANGLE:
			{
				m_RoiSel = FALSE;
				cp.x = long((m_mod_rectangle.dx+m_mod_rectangle.width/2)*m_dScale);
				cp.y = long((m_mod_rectangle.dy+m_mod_rectangle.height/2)*m_dScale);
				if(sqrtl((cp.x-point.x)*(cp.x-point.x)+(cp.y-point.y)*(cp.y-point.y))<=6)
				{
					m_PtSelType = 1;
					m_BtSel = TRUE;
					m_StPoint = point;
				}
				else
				{
					cp.x = long(m_mod_rectangle.dx*m_dScale);
					cp.y = long(m_mod_rectangle.dy*m_dScale);
					if(abs(point.x-cp.x)<=(m_mod_rectangle.width/2*m_dScale) && abs(point.y-cp.y)<=(m_mod_rectangle.height/2*m_dScale))
					{
						m_PtSelType = 2;
						m_BtSel = TRUE;
						m_StPoint = point;
					}
					else
					{
						// ROI
						cp.x = long(RoiRect.right*m_dScale);
						cp.y = long(RoiRect.bottom*m_dScale);
						if(sqrtl((cp.x-point.x)*(cp.x-point.x)+(cp.y-point.y)*(cp.y-point.y))<=6)
						{
							m_RoiSel = TRUE;
							m_PtSelType = 1;
							m_BtSel = TRUE;
							m_StPoint = point;
						}
						else
						{
							cp.x = long((RoiRect.left+RoiRect.Width()/2.0)*m_dScale);
							cp.y = long((RoiRect.top+RoiRect.Height()/2.0)*m_dScale);
							if(abs(point.x-cp.x)<=(RoiRect.Width()/2.0*m_dScale) && abs(point.y-cp.y)<=(RoiRect.Height()/2.0*m_dScale))
							{
								m_RoiSel = TRUE;
								m_PtSelType = 2;
								m_BtSel = TRUE;
								m_StPoint = point;
							}
						}
					}
					
				}
			}
			break;
		case MOD_RING:
			//{
			//	cp.x = long((m_mod_ring.dx+m_mod_ring.in_ring)*m_dScale);
			//	cp.y = long(m_mod_ring.dy*m_dScale);
			//	if(g_pView->m_ImgStatic.m_Geo.Distance(cp.x,cp.y,point.x,point.y)<=6)
			//	{
			//		m_PtSelType = 1;
			//		m_BtSel = TRUE;
			//		m_StPoint = point;
			//	}
			//	else
			//	{
			//		cp.x = long((m_mod_ring.dx+m_mod_ring.out_ring)*m_dScale);
			//		cp.y = long(m_mod_ring.dy*m_dScale);
			//		if(g_pView->m_ImgStatic.m_Geo.Distance(cp.x,cp.y,point.x,point.y)<=6)
			//		{
			//			m_PtSelType = 2;
			//			m_BtSel = TRUE;
			//			m_StPoint = point;
			//		}
			//		else
			//		{
			//			cp.x = long(m_mod_ring.dx*m_dScale);
			//			cp.y = long(m_mod_ring.dy*m_dScale);
			//			if(g_pView->m_ImgStatic.m_Geo.Distance(cp.x,cp.y,point.x,point.y)<=(m_mod_ring.out_ring*m_dScale))
			//			{
			//				m_PtSelType = 3;
			//				m_BtSel = TRUE;
			//				m_StPoint = point;
			//			}
			//		}
			//	}
			//}
			break;
		case MOD_SQUARE:
			//{
			//	cp.x = long((m_mod_square.dx+m_mod_square.length/2)*m_dScale);
			//	cp.y = long((m_mod_square.dy+m_mod_square.length/2)*m_dScale);
			//	if(g_pView->m_ImgStatic.m_Geo.Distance(cp.x,cp.y,point.x,point.y)<=6)
			//	{
			//		m_PtSelType = 1;
			//		m_BtSel = TRUE;
			//		m_StPoint = point;
			//	}
			//	else
			//	{
			//		cp.x = long(m_mod_square.dx*m_dScale);
			//		cp.y = long(m_mod_square.dy*m_dScale);
			//		if(abs(point.x-cp.x)<=(m_mod_square.length/2*m_dScale) && abs(point.y-cp.y)<=(m_mod_square.length/2*m_dScale))
			//		{
			//			m_PtSelType = 2;
			//			m_BtSel = TRUE;
			//			m_StPoint = point;
			//		}
			//	}
			//}
			break;
		case MOD_TRIANGLE:
			//{
			//	cp.x = long((m_mod_triangle.dx+m_mod_triangle.width/2)*m_dScale);
			//	cp.y = long((m_mod_triangle.dy+m_mod_triangle.height/2)*m_dScale);
			//	if(g_pView->m_ImgStatic.m_Geo.Distance(cp.x,cp.y,point.x,point.y)<=6)
			//	{
			//		m_PtSelType = 1;
			//		m_BtSel = TRUE;
			//		m_StPoint = point;
			//	}
			//	else
			//	{
			//		cp.x = long(m_mod_triangle.dx*m_dScale);
			//		cp.y = long(m_mod_triangle.dy*m_dScale);
			//		if(abs(point.x-cp.x)<=(m_mod_triangle.width/2*m_dScale) && abs(point.y-cp.y)<=(m_mod_triangle.height/2*m_dScale))
			//		{
			//			m_PtSelType = 2;
			//			m_BtSel = TRUE;
			//			m_StPoint = point;
			//		}
			//	}
			//}
			break;
		case MOD_IMAGE:
			//{
			//	cp.x = long((m_mod_image.x_offset+m_mod_image.width)*m_dScale);
			//	cp.y = long((m_mod_image.y_offset+m_mod_image.height)*m_dScale);
			//	if(g_pView->m_ImgStatic.m_Geo.Distance(cp.x,cp.y,point.x,point.y)<=6)
			//	{
			//		m_PtSelType = 1;
			//		m_BtSel = TRUE;
			//		m_StPoint = point;
			//	}
			//	else
			//	{
			//		cp.x = long((m_mod_image.x_offset+m_mod_image.width/2)*m_dScale);
			//		cp.y = long((m_mod_image.y_offset+m_mod_image.height/2)*m_dScale);
			//		if(abs(point.x-cp.x)<=(m_mod_image.width/2*m_dScale) && abs(point.y-cp.y)<=(m_mod_image.height/2*m_dScale))
			//		{
			//			m_PtSelType = 2;
			//			m_BtSel = TRUE;
			//			m_StPoint = point;
			//		}
			//	}
			//}
			break;
		default:
			break;
		}
	}
	/*else if(m_nCurStep==2)
	{
		cp.x = long(RoiRect.right*m_dScale);
		cp.y = long(RoiRect.bottom*m_dScale);
		if(sqrtl((cp.x-point.x)*(cp.x-point.x)+(cp.y-point.y)*(cp.y-point.y))<=6)
		{
			m_PtSelType = 1;
			m_BtSel = TRUE;
			m_StPoint = point;
		}
		else
		{
			cp.x = long((RoiRect.left+RoiRect.Width()/2.0)*m_dScale);
			cp.y = long((RoiRect.top+RoiRect.Height()/2.0)*m_dScale);
			if(abs(point.x-cp.x)<=(RoiRect.Width()/2.0*m_dScale) && abs(point.y-cp.y)<=(RoiRect.Height()/2.0*m_dScale))
			{
				m_PtSelType = 2;
				m_BtSel = TRUE;
				m_StPoint = point;
			}
		}
	}
	else return;*/
	CStatic::OnLButtonDown(nFlags, point);
}

void CImgMarkGeoSet::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_BtSel = FALSE;
	m_RoiSel = FALSE;
	CStatic::OnLButtonUp(nFlags, point);
}
void CImgMarkGeoSet::OnTimer(UINT_PTR nIDEvent)
{
	if(m_GatherSign)
		Invalidate();
	CStatic::OnTimer(nIDEvent);
}

void CImgMarkGeoSet::DrawImgEx(CDC *pDC, CRect rect)
{
	if(m_pScerchImg == NULL) return;
	DrawDibDraw(m_hDrawDib, pDC->GetSafeHdc(),
		rect.left, rect.top, rect.Width(), rect.Height(),
		&m_pBmpInfo->bmiHeader,
		m_pScerchImg,
		0,0,
		m_pBmpInfo->bmiHeader.biWidth,
		m_pBmpInfo->bmiHeader.biHeight, SRCCOPY);
	pDC->MoveTo(10*m_dScale,10*m_dScale);
	pDC->LineTo(1200*m_dScale,1000*m_dScale);
	//Sleep(m_DrawDelay);
}

void CImgMarkGeoSet::DrawSearch(CDC *pDC, CRect rect)
{
	int dr = 10;
	if(m_iCount>0)
	{
		CPen pen(PS_SOLID, 2, RGB(0,255,0));
		CPen *pOldPen = pDC->SelectObject(&pen);
		for(int i=0;i<m_iCount;i++)
		{
			pDC->MoveTo(int((pXPos[i]-dr)*m_dScale),int((1024-pYPos[i])*m_dScale));
			pDC->LineTo(int((pXPos[i]+dr)*m_dScale),int((1024-pYPos[i])*m_dScale));
			pDC->MoveTo(int(pXPos[i]*m_dScale),int((1024-pYPos[i]+dr)*m_dScale));
			pDC->LineTo(int(pXPos[i]*m_dScale),int((1024-pYPos[i]-dr)*m_dScale));
		}
		TRACE("PAINT_DEARCH\n");
		pDC->SelectObject(pOldPen);
	}
	else
	{

	}
}

void CImgMarkGeoSet::DrawImg(BYTE *pImg,int delay)
{
	m_bDisplay = TRUE;
	m_pScerchImg = pImg;
	m_DrawDelay = delay;
}

void CImgMarkGeoSet::SetSearchResult(double *dScore,double *dXPos,double *dYPos,double *dAng,double *dScale,int iCount,double dTime)
{
	pScore = dScore;
	pXPos = dXPos;
	pYPos = dYPos;
	pAng = dAng;
	pScale = dScale;
	m_iCount = iCount;
	m_dTime = dTime;
	m_iDisplayDelay=0;
	m_GatherInit = FALSE;
	/*for(int i=0;i<iCount;i++)
	{
		TRACE("2 dScore[%d]_%0.3lf dXPos[%d]_%0.3lf dYPos[%d]_%0.3lf dAng[%d]_%0.3lf dScale[%d]_%0.3lf \n",i,dScore[i],i,dXPos[i],i,dYPos[i],i,dAng[i],i,dScale[i]);
	}*/
}

void CImgMarkGeoSet::ResetParam()
{
	long tmp;
	if(RoiRect.left > RoiRect.right)
	{
		tmp = RoiRect.left;
		RoiRect.left = RoiRect.right;
		RoiRect.right = tmp;
	}
	if(RoiRect.top > RoiRect.bottom)
	{
		tmp = RoiRect.top;
		RoiRect.top = RoiRect.bottom;
		RoiRect.bottom = tmp;
	}
	if(RoiRect.left < 0) RoiRect.left = 0;
	if(RoiRect.right > m_iImgWidth) RoiRect.right = m_iImgWidth;
	if(RoiRect.top < 0) RoiRect.top = 0;
	if(RoiRect.bottom > m_iImgHeight)	RoiRect.bottom = m_iImgHeight;
	if(RoiRect.right<=0) RoiRect.right = m_iImgWidth;
	if(RoiRect.bottom<=0) RoiRect.bottom = m_iImgHeight;
}