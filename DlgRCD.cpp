// DlgRCD.cpp : implementation file
//

#include "stdafx.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "TSCtrlSys.h"
#include "DlgRCD.h"


// CDlgRCD dialog

IMPLEMENT_DYNAMIC(CDlgRCD, CDialog)

CDlgRCD::CDlgRCD(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRCD::IDD, pParent)
{
	nLevelID = 0;
	nSelectRow = 0;
	for(int i=0;i<2;i++)
	{
		dStartPos[i] = dEndPos[i] = 0.0;
	}
}

CDlgRCD::~CDlgRCD()
{
}

void CDlgRCD::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRCD, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_RCD_NEXT, &CDlgRCD::OnBnClickedBtnRcdNext)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_RCD_SLIDER_SPEED, &CDlgRCD::OnNMCustomdrawRcdSliderSpeed)
	ON_BN_CLICKED(IDOK, &CDlgRCD::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgRCD::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgRCD message handlers

BOOL CDlgRCD::OnInitDialog()
{
	CDialog::OnInitDialog();
	g_pFrm->bIsRCDDlgVisible = true;
	for(int i=0;i<6;i++)
	{
		m_cbMotion[i].m_iIndex = i;
		m_cbMotion[i].SubclassDlgItem(IDC_BTN_RCD_X_NEG+i,this);
		m_cbMotion[i].SetBitmapEx(IDB_BITMAP_MOVE);
		m_cbMotion[i].m_pMv = &theApp.m_Mv400;
	}
	((CSliderCtrl*)GetDlgItem(IDC_RCD_SLIDER_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_RCD_SLIDER_SPEED))->SetPos(1);

	CString str;
	if(nSelectRow==1)
	{
		str.Format("请移动相机到第一行");
		SetDlgItemText(IDC_EDIT_RCD_PROMPT,str);
	}
	else if(nSelectRow==0)
	{
		str.Format("请移动相机到第一列");
		SetDlgItemText(IDC_EDIT_RCD_PROMPT,str);
	}
	else
	{
		str.Format("请移动相机到针头1的第一行");
		SetDlgItemText(IDC_EDIT_RCD_PROMPT,str);
	}
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,TRUE);
	m_pBitmapInfo = (BITMAPINFO *)(new char[sizeof(BITMAPINFOHEADER)+256*4]);
	m_pBitmapInfo->bmiHeader.biBitCount = (UINT16)(24);

	m_pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBitmapInfo->bmiHeader.biWidth = IMAGEWIDTH0;
	m_pBitmapInfo->bmiHeader.biHeight = IMAGEHEIGHT0;//160510
	m_pBitmapInfo->bmiHeader.biPlanes = 1;

	m_pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	m_pBitmapInfo->bmiHeader.biSizeImage = IMAGEWIDTH0*IMAGEHEIGHT0*3;
	m_pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biClrUsed = 0;
	m_pBitmapInfo->bmiHeader.biClrImportant = 0;

	m_pDC = GetDlgItem(IDC_STATIC_RCD_PICTURE)->GetDC();
	SetTimer(0,100,NULL);
	return TRUE;
}

void CDlgRCD::OnBnClickedBtnRcdNext()
{
	// TODO: Add your control notification handler code here
	//if(g_pFrm->m_CmdRun.MoveToCenter())
	//{
	//	Sleep(200);
	//}
	for(int i=0;i<2;i++)
	{
		dStartPos[i] = theApp.m_Mv400.GetPos(K_AXIS_X+i);
	}
	//theApp.m_Mv400.GotoSafeZPos();
	CString str;
	if(nSelectRow==1)
	{
		str.Format("请移动相机到最后一行");
	}
	else if(nSelectRow==0)
	{
		str.Format("请移动相机到最后一列");
	}
	else
	{
		str.Format("请移动相机到针头2的第一行");
	}
	SetDlgItemText(IDC_EDIT_RCD_PROMPT,str);
	GetDlgItem(IDC_BTN_RCD_NEXT)->EnableWindow(FALSE);
	GetDlgItem(IDOK)->EnableWindow(TRUE);
}

void CDlgRCD::OnNMCustomdrawRcdSliderSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_RCD_SLIDER_SPEED))->GetPos();
	CColorButton::m_iSpeedNo = n;
	CString strTemp;
	strTemp.Format("X:%.4f,Y:%.4f,Z:%.4f", theApp.m_tSysParam.tAxis[0].vel[0] * n/2000.0,
		theApp.m_tSysParam.tAxis[1].vel[0] * n/2000.0,theApp.m_tSysParam.tAxis[2].vel[0] * n/2000.0);
	SetDlgItemText(IDC_EDIT_RCD_MOTION,strTemp);
	*pResult = 0;
}

void CDlgRCD::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	double distance = 0;
	//if(g_pFrm->m_CmdRun.MoveToCenter())
	//{
	//	Sleep(200);
	//}
	for(int i=0;i<2;i++)
	{
		dEndPos[i] = theApp.m_Mv400.GetPos(K_AXIS_X+i);
	}
	distance = sqrt((dEndPos[0]-dStartPos[0])*(dEndPos[0]-dStartPos[0])+(dEndPos[1]-dStartPos[1])*(dEndPos[1]-dStartPos[1]));
	if(nSelectRow==1)
	{
		if(nLevelID==0)
		{
			if(g_pFrm->m_CmdRun.ProductParam.FirstDispRow-1>0)
			{
				g_pFrm->m_CmdRun.ProductParam.FirstDispRowD = distance/(g_pFrm->m_CmdRun.ProductParam.FirstDispRow-1);
			}
			else
			{
				g_pFrm->m_CmdRun.ProductParam.FirstDispRowD = distance;
			}
		}
		else if(nLevelID==1)
		{
			if(g_pFrm->m_CmdRun.ProductParam.SecondDispRow-1>0)
			{
				g_pFrm->m_CmdRun.ProductParam.SecondDispRowD = distance/(g_pFrm->m_CmdRun.ProductParam.SecondDispRow-1);
			}
			else
			{
				g_pFrm->m_CmdRun.ProductParam.SecondDispRowD = distance;
			}
		}
		else if(nLevelID==2)
		{
			if(g_pFrm->m_CmdRun.ProductParam.ThirdDispRow-1>0)
			{
				g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD = distance/(g_pFrm->m_CmdRun.ProductParam.ThirdDispRow-1);
			}
			else
			{
				g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD = distance;
			}
		}
	}
	else if(nSelectRow==0)
	{
		if(nLevelID==0)
		{
			if(g_pFrm->m_CmdRun.ProductParam.FirstDispColumn-1>0)
			{
				g_pFrm->m_CmdRun.ProductParam.FirstDispColumnD = distance/(g_pFrm->m_CmdRun.ProductParam.FirstDispColumn-1);
			}
			else
			{
				g_pFrm->m_CmdRun.ProductParam.FirstDispColumnD = distance;
			}
		}
		else if(nLevelID==1)
		{
			if(g_pFrm->m_CmdRun.ProductParam.SecondDispColumn-1>0)
			{
				g_pFrm->m_CmdRun.ProductParam.SecondDispColumnD = distance/(g_pFrm->m_CmdRun.ProductParam.SecondDispColumn-1);
			}
			else
			{
				g_pFrm->m_CmdRun.ProductParam.SecondDispColumnD = distance;
			}
		}
		else if(nLevelID==2)
		{
			if(g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn-1>0)
			{
				g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD = distance/(g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn-1);
			}
			else
			{
				g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD = distance;
			}
		}
	}
	else
	{
		g_pFrm->m_CmdRun.ProductParam.NeedleGap = distance;
	}
	OnClose();
	OnOK();
}

void CDlgRCD::OnClose()
{
	g_pFrm->m_CmdRun.MoveToZSafety();
	g_pFrm->bIsRCDDlgVisible = false;
	KillTimer(0);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,FALSE);
	if(m_pBitmapInfo!=NULL)
	{
		delete[] m_pBitmapInfo;
		m_pBitmapInfo = NULL;
	}
	if(m_pDC!=NULL)
	{
		ReleaseDC(m_pDC);
	}
}

void CDlgRCD::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnClose();
	OnCancel();
}

void CDlgRCD::Paint()
{
	if(g_pView->m_ImgStatic.m_pCamera.m_pImageBuffer == NULL)
	{
		return;
	}
	m_pDC->SetBkMode(TRANSPARENT);
	HDRAWDIB m_hDrawDib = DrawDibOpen();
	DrawDibDraw(m_hDrawDib,m_pDC->GetSafeHdc(),0,0,IMAGEWIDTH0,IMAGEHEIGHT0,
		&m_pBitmapInfo->bmiHeader,g_pView->m_ImgStatic.m_pCamera.m_pImageBuffer,0,0,m_pBitmapInfo->bmiHeader.biWidth,
		m_pBitmapInfo->bmiHeader.biHeight,SRCCOPY); //160510 :w/h 异常；
	
	DrawCross(m_pDC,CPoint(IMAGEWIDTH0/2,IMAGEHEIGHT0/2),IMAGEWIDTH0/2,IMAGEHEIGHT0/2); //160510 :w/h 异常；
	PaintScale(m_pDC,CPoint(IMAGEWIDTH0/2,IMAGEHEIGHT0/2),4,4,10); //160510 :w/h 异常；
	DrawDibClose(m_hDrawDib);
}

void CDlgRCD::PaintScale(CDC *pDC,CPoint point,int nWidth, int nHeight,int nLength)
{
	CPen pen(PS_SOLID,1,RGB(255,0,0));
	CPen *pOldPen = pDC->SelectObject(&pen);
	for(int i=point.x-nLength;i>0;i=i-nLength)
	{
		pDC->MoveTo(i,point.y-nHeight/2);
		pDC->LineTo(i,point.y+nHeight/2);
		pDC->MoveTo(IMAGEWIDTH0-i,point.y-nHeight/2);
		pDC->LineTo(IMAGEWIDTH0-i,point.y+nHeight/2);
	}
	for(int i=point.y-nLength;i>0;i=i-nLength)
	{
		pDC->MoveTo(point.x-nWidth/2,i);
		pDC->LineTo(point.x+nWidth/2,i);
		pDC->MoveTo(point.x-nWidth/2,IMAGEHEIGHT0-i);
		pDC->LineTo(point.x+nWidth/2,IMAGEHEIGHT0-i);
	}
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

void CDlgRCD::DrawCross(CDC *pDC,CPoint point,int nWidth, int nHeight)
{
	CPen pen(PS_SOLID,1,RGB(255,0,0));
	CPen *pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(point.x,point.y-nHeight);
	pDC->LineTo(point.x,point.y+nHeight);
	pDC->MoveTo(point.x-nWidth,point.y);
	pDC->LineTo(point.x+nWidth,point.y);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

void CDlgRCD::OnTimer(UINT_PTR nIDEvent)
{
	Paint();
	CDialog::OnTimer(nIDEvent);
}