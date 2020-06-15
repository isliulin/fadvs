// DlgManualAlign.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgManualAlign.h"


// CDlgManualAlign dialog

IMPLEMENT_DYNAMIC(CDlgManualAlign, CDialog)

CDlgManualAlign::CDlgManualAlign(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgManualAlign::IDD, pParent)
{

}

CDlgManualAlign::~CDlgManualAlign()
{
}

void CDlgManualAlign::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgManualAlign, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CDlgManualAlign::OnBnClickedOk)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_MANUAL_ALIGN_SPEED, &CDlgManualAlign::OnNMCustomdrawSliderManualAlignSpeed)
	ON_BN_CLICKED(IDCANCEL, &CDlgManualAlign::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgManualAlign message handlers

BOOL CDlgManualAlign::OnInitDialog()
{
	CDialog::OnInitDialog();

	for(int i=0;i<4;i++)
	{
		m_colorButton[i].m_iIndex = i;
		m_colorButton[i].SubclassDlgItem(IDC_BTN_MANUAL_ALIGN_X_NEG+i,this);
		m_colorButton[i].SetBitmapEx(IDB_BITMAP_MOVE);
		m_colorButton[i].m_pMv = &theApp.m_Mv400;
	}
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_MANUAL_ALIGN_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_MANUAL_ALIGN_SPEED))->SetPos(1);
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
	m_rectModel = theApp.m_tSysParam.BmpMarkLearnWin[0];
	m_pDC = GetDlgItem(IDC_STATIC_MANUAL_ALIGN_PICTURE_CONTROL)->GetDC();
	SetTimer(0,200,NULL);
	return TRUE;
}

void CDlgManualAlign::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnClose();
	OnOK();
}

void CDlgManualAlign::OnNMCustomdrawSliderManualAlignSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_MANUAL_ALIGN_SPEED))->GetPos();
	CColorButton::m_iSpeedNo = n;
	CString strTemp;
	strTemp.Format("X:%.4f,Y:%.4f", theApp.m_tSysParam.tAxis[0].vel[0] * n/2000.0,
		theApp.m_tSysParam.tAxis[1].vel[0] * n/2000.0);
	SetDlgItemText(IDC_EDIT_MANUAL_ALIGN_SPEED, strTemp);
	*pResult = 0;
}

void CDlgManualAlign::RefreshPosition()
{
	CString str;

	str.Format("%.3f",theApp.m_Mv400.GetPos(K_AXIS_X));
	SetDlgItemText(IDC_EDIT_MANUAL_ALIGN_CURRENT_X,str);
	str.Format("%.3f",theApp.m_Mv400.GetPos(K_AXIS_Y));
	SetDlgItemText(IDC_EDIT_MANUAL_ALIGN_CURRENT_Y,str);
}

void CDlgManualAlign::OnTimer(UINT_PTR nIDEvent)
{
	RefreshPosition();
	Paint();
	CDialog::OnTimer(nIDEvent);
}

void CDlgManualAlign::Paint()
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
	DrawDibClose(m_hDrawDib);
	DrawCross(m_pDC,CPoint(IMAGEWIDTH0/2,IMAGEHEIGHT0/2),IMAGEWIDTH0/2,IMAGEHEIGHT0/2);
	PainScale(m_pDC,CPoint(IMAGEWIDTH0/2,IMAGEHEIGHT0/2),4,4,10);
	PainROI(m_pDC);
}

void CDlgManualAlign::PainScale(CDC *pDC,CPoint point,int nWidth, int nHeight,int nLength)
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

void CDlgManualAlign::DrawCross(CDC *pDC,CPoint point,int nWidth, int nHeight)
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

void CDlgManualAlign::PainROI(CDC *pDC)
{
	CPen pen(PS_SOLID,1,RGB(255,0,0));
	CPen *pOldPen = pDC->SelectObject(&pen);
	pDC->Draw3dRect(320-m_rectModel.Width()/2,256-m_rectModel.Height()/2,m_rectModel.Width(),m_rectModel.Height(),RGB(255,0,0),RGB(255,0,0));
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

void CDlgManualAlign::OnClose()
{
	KillTimer(0);
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
void CDlgManualAlign::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnClose();
	OnCancel();
}
