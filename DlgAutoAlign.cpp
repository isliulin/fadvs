// DlgAlignNeedle.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>
#include "TSCtrlSys.h"
#include "DlgAutoAlign.h"


// CDlgAlignNeedle dialog

IMPLEMENT_DYNAMIC(CDlgAutoAlign, CDialog)

CDlgAutoAlign::CDlgAutoAlign(CWnd* pParent /*=NULL*/)
: CDialog(CDlgAutoAlign::IDD, pParent)
{

}

CDlgAutoAlign::~CDlgAutoAlign()
{
}

void CDlgAutoAlign::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgAutoAlign, CDialog)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_AUTO_ALIGN_SPEED, &CDlgAutoAlign::OnNMCustomdrawSliderAutoAlignSpeed)
	ON_BN_CLICKED(IDOK, &CDlgAutoAlign::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgAutoAlign::OnBnClickedCancel)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_AUTO_ALIGN_NEXT, &CDlgAutoAlign::OnBnClickedBtnAutoAlignNext)
END_MESSAGE_MAP()


// CDlgAlignNeedle message handlers

BOOL CDlgAutoAlign::OnInitDialog()
{
	g_pFrm->bAutoDlgVisible = true;
	for(int i=0; i<6; i++)
	{
		m_colorButton[i].m_iIndex = i;
		m_colorButton[i].SubclassDlgItem(IDC_BTN_AUTO_ALIGN_X_NEG+i, this);
		m_colorButton[i].SetBitmapEx(IDB_BITMAP_MOVE);
		m_colorButton[i].m_pMv = &theApp.m_Mv400;
	}
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_AUTO_ALIGN_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_AUTO_ALIGN_SPEED))->SetPos(500);
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
	m_pDC = GetDlgItem(IDC_STATIC_AUTO_ALIGN_PICTURE_CONTROL)->GetDC();
	SetTimer(0,100,NULL);
	if(AfxMessageBox("是否移动到A#轨道开始取图位并移动到中心？",MB_YESNO)==IDYES)
	{
		MoveToImageCenter();
	}
	CString str;
	str.Format("标定中心.");
	SetDlgItemText(IDC_EDIT_AUTO_ALIGN_PROMPT,str);
	nStep = 0;
	return TRUE;
}

void CDlgAutoAlign::OnNMCustomdrawSliderAutoAlignSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_AUTO_ALIGN_SPEED))->GetPos();
	CColorButton::m_iSpeedNo = n;
	CString strTemp;
	strTemp.Format("X:%.4f,Y:%.4f,ZA:%.4f", theApp.m_tSysParam.tAxis[0].vel[0] * n/2000.0,
		theApp.m_tSysParam.tAxis[1].vel[0] * n/2000.0,theApp.m_tSysParam.tAxis[2].vel[0] * n/2000.0);
	SetDlgItemText(IDC_EDIT_AUTO_ALIGN_SPEED, strTemp);
	*pResult = 0;
}

void CDlgAutoAlign::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnClose();
	OnOK();
}

void CDlgAutoAlign::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnClose();
	OnCancel();
}

void CDlgAutoAlign::OnClose()
{
	g_pFrm->m_CmdRun.MoveToZSafety();
	g_pFrm->bAutoDlgVisible = false;
	KillTimer(0);
	if(m_pBitmapInfo!=NULL)
	{
		delete[] m_pBitmapInfo;
		m_pBitmapInfo = NULL;
	}
}

void CDlgAutoAlign::Paint()
{
	if(g_pView->m_ImgStatic.m_pCamera.m_pImageBuffer == NULL)
	{
		return;
	}
	m_pDC->SetBkMode(TRANSPARENT);
	HDRAWDIB m_hDrawDib = DrawDibOpen();
	DrawDibDraw(m_hDrawDib,m_pDC->GetSafeHdc(),0,0,IMAGEWIDTH0,IMAGEHEIGHT0,
		&m_pBitmapInfo->bmiHeader,g_pView->m_ImgStatic.m_pCamera.m_pImageBuffer,0,0,m_pBitmapInfo->bmiHeader.biWidth,
		m_pBitmapInfo->bmiHeader.biHeight,SRCCOPY);
	DrawCross(m_pDC,CPoint(IMAGEWIDTH0/2,IMAGEHEIGHT0/2),IMAGEWIDTH0/2,IMAGEHEIGHT0/2);
	PainScale(m_pDC,CPoint(IMAGEWIDTH0/2,IMAGEHEIGHT0/2),4,4,10);
	DrawDibClose(m_hDrawDib);
}

void CDlgAutoAlign::PainScale(CDC *pDC,CPoint point,int nWidth, int nHeight,int nLength)
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

void CDlgAutoAlign::DrawCross(CDC *pDC,CPoint point,int nWidth, int nHeight)
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

void CDlgAutoAlign::OnTimer(UINT nIDEvent)
{
	Paint();
	CString str;
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_X));
	SetDlgItemText(IDC_EDIT_AUTO_ALIGN_CURRENT_X,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_Y));
	SetDlgItemText(IDC_EDIT_AUTO_ALIGN_CURRENT_Y,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_ZA));
	SetDlgItemText(IDC_EDIT_AUTO_ALIGN_CURRENT_ZA,str);
	double dHeightValue = 0.0;
	g_pFrm->m_CmdRun.ReadHeightSensorValue(&dHeightValue);
	str.Format("%0.2f",dHeightValue);
	SetDlgItemText(IDC_EDIT_AUTO_ALIGN_HEIGHT_VALUE,str);

	CDialog::OnTimer(nIDEvent);
}
void CDlgAutoAlign::OnBnClickedBtnAutoAlignNext()
{
	// TODO: Add your control notification handler code here
	if(nStep==0)
	{
		g_pFrm->m_CmdRun.ProductParam.tgCustomLabelPos.x = m_dStartPos[0] = theApp.m_Mv400.GetPos(K_AXIS_X);
		g_pFrm->m_CmdRun.ProductParam.tgCustomLabelPos.y = m_dStartPos[1] = theApp.m_Mv400.GetPos(K_AXIS_Y);
		g_pFrm->m_CmdRun.ProductParam.tgCustomLabelPos.za = m_dStartPos[2] = theApp.m_Mv400.GetPos(K_AXIS_ZA);
	}
	else
	{
		for(int i=0;i<3;i++)
		{
			m_dTempPos[i] = theApp.m_Mv400.GetPos(i+1);
		}
		tgPos tgCurrentPos;
		tgCurrentPos.x = m_dTempPos[0] - m_dStartPos[0];
		tgCurrentPos.y = m_dTempPos[1] - m_dStartPos[1];
		tgCurrentPos.za = m_dTempPos[2] - m_dStartPos[2];
		g_pFrm->m_rawList.AddTail(tgCurrentPos);
	}
	nStep++;
	CString str;
	str.Format("移动到下一坐标点或按确定退出.");
	SetDlgItemText(IDC_EDIT_AUTO_ALIGN_PROMPT,str);
}

bool CDlgAutoAlign::MoveToFirstDispensePos()
{
	bool bReturn = false;
	if(theApp.m_Mv400.IsInitOK())
	{
		bReturn = g_pFrm->m_CmdRun.StartGrabImagePos();
	}
	return bReturn;
}

bool CDlgAutoAlign::MoveToImageCenter()
{
	bool bReturn = false;
	if(theApp.m_Mv400.IsInitOK())
	{
		bReturn = g_pFrm->m_CmdRun.StartGrabImagePos();
		if(!bReturn)
		{
			return false;
		}
		Sleep(200);
		bReturn = g_pFrm->m_CmdRun.MoveToCenter();
		if(!bReturn)
		{
			return false;
		}
	}
	return bReturn;
}