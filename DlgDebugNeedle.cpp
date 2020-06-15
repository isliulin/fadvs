// DlgDebugNeedle.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgDebugNeedle.h"


// CDlgDebugNeedle dialog

IMPLEMENT_DYNAMIC(CDlgDebugNeedle, CDialog)

CDlgDebugNeedle::CDlgDebugNeedle(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDebugNeedle::IDD, pParent)
{

}

CDlgDebugNeedle::~CDlgDebugNeedle()
{
}

void CDlgDebugNeedle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgDebugNeedle, CDialog)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_DEBUG_NEXT, &CDlgDebugNeedle::OnBnClickedBtnDebugNext)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_DEBUG_NEEDLE_SPEED, &CDlgDebugNeedle::OnNMCustomdrawSliderDebugNeedleSpeed)
	ON_BN_CLICKED(IDOK, &CDlgDebugNeedle::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgDebugNeedle::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgDebugNeedle message handlers

BOOL CDlgDebugNeedle::OnInitDialog()
{
	g_pFrm->bIsDebugNeedleVisible = true;
	m_pCamera = NULL;
	m_pBitmapInfo = NULL;
	for(int i=0;i<6;i++)
	{
		m_colorButton[i].m_iIndex = i;
		m_colorButton[i].SubclassDlgItem(IDC_BTN_DEBUG_NEEDLE_X_NEG+i,this);
		m_colorButton[i].SetBitmapEx(IDB_BITMAP_MOVE);
		m_colorButton[i].m_pMv = &theApp.m_Mv400;
	}
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_DEBUG_NEEDLE_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_DEBUG_NEEDLE_SPEED))->SetPos(1);
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_DEBUG_NEEDLE_SPEED))->GetPos();
	CColorButton::m_iSpeedNo = n;
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outNeedleLight,TRUE);
	nStep = 0;
	m_pDC = GetDlgItem(IDC_STATIC_DEBUG_NEEDLE_PICTURE)->GetDC();
	ImgInit();
	SetTimer(0,100,NULL);
	MoveToNeedlePos(1);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	return TRUE;
}

void CDlgDebugNeedle::ImgInit()
{
	BOOL bInitOK = TRUE;
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

	m_pCamera = new CDHCamera();
	bInitOK = m_pCamera->InitDH(0,25,0,0,IMAGEWIDTH0,IMAGEHEIGHT0,RES_MODE1,false);
	if(bInitOK==TRUE)
	{
		m_pCamera->Live();
	}
}

void CDlgDebugNeedle::OnBnClickedBtnDebugNext()
{
	// TODO: Add your control notification handler code here
	if(nStep==0)
	{
		SavePosition();
		Sleep(100);
		MoveToNeedlePos(0);
		nStep++;
	}
	else if(nStep==1)
	{
		MoveToNeedlePos(2);
		nStep++;
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_DEBUG_NEXT)->EnableWindow(FALSE);
	}
}

void CDlgDebugNeedle::OnNMCustomdrawSliderDebugNeedleSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_DEBUG_NEEDLE_SPEED))->GetPos();
	CColorButton::m_iSpeedNo = n;
	CString strTemp;
	strTemp.Format("X:%.4f,Y:%.4f,Z:%.4f", theApp.m_tSysParam.tAxis[0].vel[0] * n/2000.0,
		theApp.m_tSysParam.tAxis[1].vel[0] * n/2000.0,theApp.m_tSysParam.tAxis[2].vel[0] * n/2000.0);
	SetDlgItemText(IDC_EDIT_DEBUG_NEEDLE_SPEED, strTemp);
}

void CDlgDebugNeedle::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnClose();
	OnOK();
}

void CDlgDebugNeedle::OnTimer(UINT nIDEvent)
{
	Paint();
	CDialog::OnTimer(nIDEvent);
}

void CDlgDebugNeedle::Paint()
{
	if(m_pCamera->m_pImageBuffer==NULL)
	{
		return;
	}
	HDRAWDIB m_hDrawDib = DrawDibOpen();
	m_pDC->SetBkMode(TRANSPARENT);
	DrawDibDraw(m_hDrawDib,m_pDC->GetSafeHdc(),0,0,IMAGEWIDTH0,IMAGEHEIGHT0,
		&m_pBitmapInfo->bmiHeader,m_pCamera->m_pImageBuffer,0,0,m_pBitmapInfo->bmiHeader.biWidth,
		m_pBitmapInfo->bmiHeader.biHeight,SRCCOPY);
	DrawCross(m_pDC,CPoint(IMAGEWIDTH0/2,IMAGEHEIGHT0/2),IMAGEWIDTH0/2,IMAGEHEIGHT0/2);
	PainScale(m_pDC,CPoint(IMAGEWIDTH0/2,IMAGEHEIGHT0/2),4,4,10);
	DrawDibClose(m_hDrawDib);
}

void CDlgDebugNeedle::DrawCross(CDC *pDC,CPoint point,int nWidth, int nHeight)
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

void CDlgDebugNeedle::PainScale(CDC *pDC,CPoint point,int nWidth, int nHeight,int nLength)
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

void CDlgDebugNeedle::MoveToNeedlePos(int nIndex)
{
	if(theApp.m_Mv400.IsInitOK())
	{
		if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_STOP)
		{
			if(!g_pFrm->m_CmdRun.MoveToZSafety())
			{
				return;
			}
			Sleep(100);
			if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[nIndex].x,
				g_pFrm->m_CmdRun.ProductParam.tgLabelPos[nIndex].y,true))
			{
				return;
			}
			if(!g_pFrm->m_CmdRun.SynchronizeMoveZ(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[nIndex].za,true))
			{
				return;
			}
		}
	}
}

void CDlgDebugNeedle::OnClose()
{
	g_pFrm->m_CmdRun.MoveToZSafety();
	g_pFrm->bIsDebugNeedleVisible = false;
	KillTimer(0);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outNeedleLight,FALSE);
	if(m_pBitmapInfo!=NULL)
	{
		delete[] m_pBitmapInfo;
		m_pBitmapInfo = NULL;
	}
	if(m_pCamera!=NULL)
	{
		m_pCamera->Stop();
		m_pCamera->Free();
		delete m_pCamera;
		m_pCamera = NULL;
	}
	if(m_pDC!=NULL)
	{
		ReleaseDC(m_pDC);
	}
}

void CDlgDebugNeedle::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnClose();
	OnCancel();
}

void CDlgDebugNeedle::SavePosition()
{
	g_pFrm->m_CmdRun.ProductParam.NeedlePos[0] = theApp.m_Mv400.GetPos(K_AXIS_X);
	g_pFrm->m_CmdRun.ProductParam.NeedlePos[1] = theApp.m_Mv400.GetPos(K_AXIS_Y);
	theApp.ProductParam(FALSE);
}
