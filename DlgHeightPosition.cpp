// DlgHeightPosition.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgHeightPosition.h"


// CDlgHeightPosition dialog

IMPLEMENT_DYNAMIC(CDlgHeightPosition, CDialog)

CDlgHeightPosition::CDlgHeightPosition(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgHeightPosition::IDD, pParent)
{

}

CDlgHeightPosition::~CDlgHeightPosition()
{
}

void CDlgHeightPosition::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgHeightPosition, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_HEIGHT_POS_NEXT, &CDlgHeightPosition::OnBnClickedBtnHeightPosNext)
	ON_BN_CLICKED(IDOK, &CDlgHeightPosition::OnBnClickedOk)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_HEIGHT_POS_SPEED, &CDlgHeightPosition::OnNMCustomdrawSliderHeightPosSpeed)
	ON_BN_CLICKED(IDCANCEL, &CDlgHeightPosition::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgHeightPosition message handlers
BOOL CDlgHeightPosition::OnInitDialog()
{
	for(int i=0; i<6; i++)
	{
		m_colorButton[i].m_iIndex = i;
		m_colorButton[i].SubclassDlgItem(IDC_BTN_HEIGHT_POS_X_NEG+i, this);
		m_colorButton[i].SetBitmapEx(IDB_BITMAP_MOVE);
		m_colorButton[i].m_pMv = &theApp.m_Mv400;
	}
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_HEIGHT_POS_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_HEIGHT_POS_SPEED))->SetPos(1);
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
	m_nIndex = g_pFrm->m_CmdRun.ProductParam.HeightNumber;
	m_nCount = 0;
	CString str;
	str.Format("请将相机移动到%d个位置",m_nCount);
	SetDlgItemText(IDC_EDIT_HEIGHT_POS_PROMPT,str);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	m_pDC = GetDlgItem(IDC_STATIC_HEIGHT_POS_PICTURE_CONTROL)->GetDC();
	SetTimer(0,100,NULL);
	return TRUE;
}

void CDlgHeightPosition::OnBnClickedBtnHeightPosNext()
{
	//g_pFrm->m_CmdRun.ProductParam.tgMeasureHeightPos[m_nCount].x = theApp.m_Mv400.GetPos(K_AXIS_X) -
		//(g_pFrm->m_CmdRun.ProductParam.CameraOnContactor[0] - g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].x);
	//g_pFrm->m_CmdRun.ProductParam.tgMeasureHeightPos[m_nCount].y = theApp.m_Mv400.GetPos(K_AXIS_Y) -
		//(g_pFrm->m_CmdRun.ProductParam.CameraOnContactor[1] - g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].y);
	double dHeightValue;
	g_pFrm->m_CmdRun.ProductParam.tgMeasureHeightPos[m_nCount].x = theApp.m_Mv400.GetPos(K_AXIS_X);
	g_pFrm->m_CmdRun.ProductParam.tgMeasureHeightPos[m_nCount].y = theApp.m_Mv400.GetPos(K_AXIS_Y);
	//g_pFrm->m_CmdRun.ProductParam.tgMeasureHeightPos[m_nCount].za = g_pFrm->m_CmdRun.ProductParam.HeightDispensePosition[2];
	g_pFrm->m_CmdRun.ProductParam.tgMeasureHeightPos[m_nCount].za = g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].za;
	if(g_pFrm->m_CmdRun.ReadHeightSensorValue(&dHeightValue))
	    g_pFrm->m_CmdRun.ProductParam.dLaserDataBase[m_nCount]=dHeightValue;
	else
        g_pFrm->m_CmdRun.ProductParam.dLaserDataBase[m_nCount]=0;

	m_nCount++;
	CString str;
	str.Format("请将相机移动到%d个位置",m_nCount);
	SetDlgItemText(IDC_EDIT_HEIGHT_POS_PROMPT,str);
	if(m_nCount>=m_nIndex)
	{
		str.Format("完成",m_nCount);
		SetDlgItemText(IDC_EDIT_HEIGHT_POS_PROMPT,str);
		GetDlgItem(IDC_BTN_HEIGHT_POS_NEXT)->EnableWindow(FALSE);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
}

void CDlgHeightPosition::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	Close();
	OnOK();
}

void CDlgHeightPosition::OnNMCustomdrawSliderHeightPosSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_HEIGHT_POS_SPEED))->GetPos();
	CColorButton::m_iSpeedNo = n;
	CString strTemp;
	strTemp.Format("X:%.4f,Y:%.4f,ZA:%.4f", theApp.m_tSysParam.tAxis[0].vel[0] * n/2000.0,
		theApp.m_tSysParam.tAxis[1].vel[0] * n/2000.0,theApp.m_tSysParam.tAxis[2].vel[0] * n/2000.0);
	SetDlgItemText(IDC_EDIT_HEIGHT_POS_SPEED,strTemp);
	*pResult = 0;
}

void CDlgHeightPosition::OnTimer(UINT_PTR nIDEvent)
{
	Paint();
	UpdateUI();
	UpdateData();
	CDialog::OnTimer(nIDEvent);
}

void CDlgHeightPosition::DrawCross(CDC *pDC,CPoint point,int nWidth, int nHeight)
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

void CDlgHeightPosition::PainScale(CDC *pDC,CPoint point,int nWidth, int nHeight,int nLength)
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

void CDlgHeightPosition::Paint()
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

void CDlgHeightPosition::Close()
{
	g_pFrm->m_CmdRun.MoveToZSafety();
	KillTimer(0);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,FALSE);
	//if(m_bNeedleCameraInit&&m_pNeedleCamera!=NULL)
	//{
	//	m_pNeedleCamera->Stop();
	//	m_bNeedleCameraInit = FALSE;
	//}
	//else if(m_pNeedleCamera!=NULL)
	//{
	//	m_pNeedleCamera->Free();
	//	delete m_pNeedleCamera;
	//	m_pNeedleCamera = NULL;
	//}
	Sleep(100);
	delete[] m_pBitmapInfo;
	m_pBitmapInfo = NULL;
	if(m_pDC!=NULL)
	{
		ReleaseDC(m_pDC);
	}
}

void CDlgHeightPosition::UpdateUI()
{
	CString str;
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_X));
	SetDlgItemText(IDC_EDIT_HEIGHT_POS_CURRENT_X,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_Y));
	SetDlgItemText(IDC_EDIT_HEIGHT_POS_CURRENT_Y,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_ZA));
	SetDlgItemText(IDC_EDIT_HEIGHT_POS_CURRENT_ZA,str);
}
void CDlgHeightPosition::UpdateData()
{
	double dHeightValue;
	CString str;
	if(g_pFrm->m_CmdRun.ReadHeightSensorValue(&dHeightValue))
	{
		str.Format("实时激光数据:%0.3f mm...",dHeightValue);
		g_pFrm->m_CmdRun.AddMsg(str);
	}

}
void CDlgHeightPosition::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	Close();
	OnCancel();
}
