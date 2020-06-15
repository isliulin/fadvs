// DlgAlignNeedle.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>
#include "TSCtrlSys.h"
#include "DlgAlignNeedle.h"
#include <math.h>


// CDlgAlignNeedle dialog
IMPLEMENT_DYNAMIC(CDlgAlignNeedle, CDialog)

CDlgAlignNeedle::CDlgAlignNeedle(CWnd* pParent /*=NULL*/)
: CDialog(CDlgAlignNeedle::IDD, pParent)
{
}

CDlgAlignNeedle::~CDlgAlignNeedle()
{
}

void CDlgAlignNeedle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgAlignNeedle, CDialog)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_ALIGN_SPEED, &CDlgAlignNeedle::OnNMCustomdrawSliderAlignSpeed)
	ON_BN_CLICKED(IDOK, &CDlgAlignNeedle::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgAlignNeedle::OnBnClickedCancel)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_ALIGN_NEEDLE_NEXT, &CDlgAlignNeedle::OnBnClickedBtnAlignNeedleNext)
END_MESSAGE_MAP()


// CDlgAlignNeedle message handlers

BOOL CDlgAlignNeedle::OnInitDialog()
{
	g_pFrm->bIsAlignDlgVisible = true;
	for(int i=0; i<6; i++)
	{
		m_colorButton[i].m_iIndex = i;
		m_colorButton[i].SubclassDlgItem(IDC_BTN_ALIGN_X_NEG+i, this);
		m_colorButton[i].SetBitmapEx(IDB_BITMAP_MOVE);
		m_colorButton[i].m_pMv = &theApp.m_Mv400;
	}
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_ALIGN_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_ALIGN_SPEED))->SetPos(1);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,TRUE);
	m_pBitmapInfo = (BITMAPINFO *)(new char[sizeof(BITMAPINFOHEADER)+256*4]);
	m_pBitmapInfo->bmiHeader.biBitCount = (UINT16)(24);
	m_pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBitmapInfo->bmiHeader.biWidth = IMAGEWIDTH0;
	m_pBitmapInfo->bmiHeader.biHeight = IMAGEHEIGHT0;
	m_pBitmapInfo->bmiHeader.biPlanes = 1;
	m_pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	m_pBitmapInfo->bmiHeader.biSizeImage = IMAGEWIDTH0*IMAGEHEIGHT0*3;
	m_pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biClrUsed = 0;
	m_pBitmapInfo->bmiHeader.biClrImportant = 0;
	m_pDC = GetDlgItem(IDC_STATIC_ALIGN_PICTURE_CONTROL)->GetDC();
	SetTimer(0,100,NULL);
	CString str;
	str.Format("请将相机移动到点胶位");
	SetDlgItemText(IDC_EDIT_ALIGN_NEEDLE_PROMPT,str);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	MoveToImagePosition();
	return TRUE;
}

void CDlgAlignNeedle::OnNMCustomdrawSliderAlignSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_ALIGN_SPEED))->GetPos();
	CColorButton::m_iSpeedNo = n;
	CString strTemp;
	strTemp.Format("X:%.4f,Y:%.4f,ZA:%.4f", theApp.m_tSysParam.tAxis[0].vel[0] * n/2000.0,
		theApp.m_tSysParam.tAxis[1].vel[0] * n/2000.0,theApp.m_tSysParam.tAxis[2].vel[0] * n/2000.0);
	SetDlgItemText(IDC_EDIT_ALIGN_SPEED, strTemp);
	*pResult = 0;
}

void CDlgAlignNeedle::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	double dLaserDiff;
	for(int i=0;i<3;i++)
	{
		Sleep(50);
		m_dHeightPos[i] = theApp.m_Mv400.GetPos(i+1);
		g_pFrm->m_CmdRun.ReadHeightSensorValue(&m_dHeightValue);
	}

	dLaserDiff=fabs(m_dHeightValue-g_pFrm->m_CmdRun.ProductParam.HeightStandardValue);
    //加数据差值校验最佳......

	g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[0] = m_dTempPos[0];
	g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[1] = m_dTempPos[1];

	g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[2] = g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]+dLaserDiff-0.25;  //0.25 contact sensor offset...
	g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[3] = g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[2]+(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[3]-g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]);
	g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[4] =  g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[2]+(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[4]-g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]);
	g_pFrm->m_CmdRun.ProductParam.ImageDispensePosition[0] = m_dImagePos[0];
	g_pFrm->m_CmdRun.ProductParam.ImageDispensePosition[1] = m_dImagePos[1];
	g_pFrm->m_CmdRun.ProductParam.ImageDispensePosition[2] = m_dImagePos[2];
	g_pFrm->m_CmdRun.ProductParam.HeightDispensePosition[0] = m_dHeightPos[0];
	g_pFrm->m_CmdRun.ProductParam.HeightDispensePosition[1] = m_dHeightPos[1];
	g_pFrm->m_CmdRun.ProductParam.HeightDispensePosition[2] = m_dHeightPos[2];
	OnClose();
	OnOK();
}

void CDlgAlignNeedle::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnClose();
	OnCancel();
}

void CDlgAlignNeedle::OnClose()
{
	g_pFrm->m_CmdRun.MoveToZSafety();
	g_pFrm->bIsAlignDlgVisible = false;
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

void CDlgAlignNeedle::Paint()
{
	if(g_pView->m_ImgStatic.m_pCamera.m_pImageBuffer == NULL)
	{
		return;
	}
	m_pDC->SetBkMode(TRANSPARENT);
	HDRAWDIB m_hDrawDib = DrawDibOpen();
	DrawDibDraw(m_hDrawDib,m_pDC->GetSafeHdc(),0,0,IMAGEWIDTH0,IMAGEWIDTH0,
		&m_pBitmapInfo->bmiHeader,g_pView->m_ImgStatic.m_pCamera.m_pImageBuffer,0,0,m_pBitmapInfo->bmiHeader.biWidth,
		m_pBitmapInfo->bmiHeader.biHeight,SRCCOPY);
	DrawDibClose(m_hDrawDib);
	DrawCross(m_pDC,CPoint(IMAGEWIDTH0/2,IMAGEWIDTH0/2),IMAGEWIDTH0/2,IMAGEWIDTH0/2);
	PainScale(m_pDC,CPoint(IMAGEWIDTH0/2,IMAGEWIDTH0/2),4,4,10);
}

void CDlgAlignNeedle::PainScale(CDC *pDC,CPoint point,int nWidth, int nHeight,int nLength)
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

void CDlgAlignNeedle::DrawCross(CDC *pDC,CPoint point,int nWidth, int nHeight)
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

void CDlgAlignNeedle::OnTimer(UINT nIDEvent)
{
	Paint();
	CString str;
	double dHeightValue;
	g_pFrm->m_CmdRun.ReadHeightSensorValue(&dHeightValue);
	str.Format("%0.3f",dHeightValue);
	SetDlgItemText(IDC_EDIT_ALIGN_HEIGHT_CURRENT_VALUE,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_X));
	SetDlgItemText(IDC_EDIT_ALIGN_X_POS,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_Y));
	SetDlgItemText(IDC_EDIT_ALIGN_Y_POS,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_ZA));
	SetDlgItemText(IDC_EDIT_ALIGN_ZA_POS,str);

	CDialog::OnTimer(nIDEvent);
}
void CDlgAlignNeedle::OnBnClickedBtnAlignNeedleNext()
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_BTN_ALIGN_ZA_NEG)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_ALIGN_ZA_POS)->EnableWindow(FALSE);

	for(int i=0;i<3;i++)
	{
		m_dImagePos[i] = theApp.m_Mv400.GetPos(i+1);
	}
	m_dTempPos[0] = theApp.m_Mv400.GetPos(K_AXIS_X) -     //tgLabelPos[0]，tgLabelPos[1]，tgLabelPos[2]，tgLabelPos[3]：以下Camera基准
		(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].x - g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].x);
	m_dTempPos[1] = theApp.m_Mv400.GetPos(K_AXIS_Y) -
		(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].y - g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].y);
	g_pFrm->m_CmdRun.MoveToZSafety();
	//Move to laser test position;
	g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.HeightDispensePosition[0],g_pFrm->m_CmdRun.ProductParam.HeightDispensePosition[1],true);
	////与测高时z相对静止.兼容激光固定版   2017-12-09
	if(!g_pFrm->m_CmdRun.SynchronizeMoveZ(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].za,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].zb,
		                                  g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].zc,false,true))
	{
		return ;
	}
	CString str;
	str.Format("激光测高感应器准备读取数据...");
	SetDlgItemText(IDC_EDIT_ALIGN_NEEDLE_PROMPT,str);
	GetDlgItem(IDC_BTN_ALIGN_NEEDLE_NEXT)->EnableWindow(FALSE);
	GetDlgItem(IDOK)->EnableWindow(TRUE);
}

bool CDlgAlignNeedle::MoveToImagePosition()
{
	if(theApp.m_Mv400.IsInitOK())
	{
		if(!g_pFrm->m_CmdRun.MoveToZSafety())
		{
			return false;
		}
		if(AfxMessageBox("相机移动到点胶位置？",MB_YESNO)==IDYES)
		{
			if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.ImageDispensePosition[0],g_pFrm->m_CmdRun.ProductParam.ImageDispensePosition[1],TRUE))
			{
				return false;
			}
			if(!g_pFrm->m_CmdRun.MoveZ(K_AXIS_ZA,g_pFrm->m_CmdRun.ProductParam.ImageDispensePosition[2],TRUE))
			{
				return false;
			}
		}
	}
	return true;
}
