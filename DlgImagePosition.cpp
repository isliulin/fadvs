// DlgImagePosition.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgImagePosition.h"


// CDlgImagePosition dialog

IMPLEMENT_DYNAMIC(CDlgImagePosition, CDialog)

CDlgImagePosition::CDlgImagePosition(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImagePosition::IDD, pParent)
{

}

CDlgImagePosition::~CDlgImagePosition()
{
}

void CDlgImagePosition::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgImagePosition, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_IMAGE_NEXT, &CDlgImagePosition::OnBnClickedBtnImageNeedleNext)
	ON_BN_CLICKED(IDOK, &CDlgImagePosition::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgImagePosition::OnBnClickedCancel)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_IMAGE_SPEED, &CDlgImagePosition::OnNMCustomdrawSliderImageSpeed)
END_MESSAGE_MAP()


// CDlgImagePosition message handlers
BOOL CDlgImagePosition::OnInitDialog()
{
	g_pFrm->bImagePosVisible = true;
	for(int i=0; i<6; i++)
	{
		m_colorButton[i].m_iIndex = i;
		m_colorButton[i].SubclassDlgItem(IDC_BTN_IMAGE_X_NEG+i, this);
		m_colorButton[i].SetBitmapEx(IDB_BITMAP_MOVE);
		m_colorButton[i].m_pMv = &theApp.m_Mv400;
	}
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_IMAGE_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_IMAGE_SPEED))->SetPos(1);
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
	nCount = 0;
	nIndex = g_pFrm->m_CmdRun.ProductParam.nImageNum;
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,TRUE);
	CString str;
	str.Format("请将相机移动到%d个图像位置",nCount);
	SetDlgItemText(IDC_EDIT_IMAGE_PROMPT,str);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	m_pDC = GetDlgItem(IDC_STATIC_IMAGE_PICTURE_CONTROL)->GetDC();
	SetTimer(0,100,NULL);
	if(AfxMessageBox("ZA轴移动到相机焦距位置？",MB_YESNO)==IDYES)
	{
		g_pFrm->m_CmdRun.SynchronizeMoveZ(theApp.m_tSysParam.dFocusZPos,true);

	}
	return TRUE;
}

void CDlgImagePosition::OnBnClickedBtnImageNeedleNext()
{
	// TODO: Add your control notification handler code here
	CString str;
	g_pFrm->m_CmdRun.ProductParam.tgImagePos[nCount].x = theApp.m_Mv400.GetPos(K_AXIS_X);
	g_pFrm->m_CmdRun.ProductParam.tgImagePos[nCount].y = theApp.m_Mv400.GetPos(K_AXIS_Y);
	g_pFrm->m_CmdRun.ProductParam.tgImagePos[nCount].za = theApp.m_Mv400.GetPos(K_AXIS_ZA);
	str.Format("tgImagePos[%d](%0.3f,%0.3f,%0.3f)",nCount,g_pFrm->m_CmdRun.ProductParam.tgImagePos[nCount].x,
		g_pFrm->m_CmdRun.ProductParam.tgImagePos[nCount].y,g_pFrm->m_CmdRun.ProductParam.tgImagePos[nCount].za);
	g_pFrm->m_CmdRun.PutLogIntoList(str);

	nCount++;
	str.Format("请将相机移动到%d个位置",nCount);
	SetDlgItemText(IDC_EDIT_IMAGE_PROMPT,str);
	if(nCount>=nIndex)
	{
		str.Format("完成",nCount);
		SetDlgItemText(IDC_EDIT_IMAGE_PROMPT,str);
		GetDlgItem(IDC_BTN_IMAGE_NEXT)->EnableWindow(FALSE);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
}
void CDlgImagePosition::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnClose();
	OnOK();
}

void CDlgImagePosition::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnClose();
	OnCancel();
}

void CDlgImagePosition::OnClose()
{
	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		return;
	}
	g_pFrm->bImagePosVisible = false;
	KillTimer(0);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,FALSE);
	Sleep(100);
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

void CDlgImagePosition::Paint()
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
}

void CDlgImagePosition::PainScale(CDC *pDC,CPoint point,int nWidth, int nHeight,int nLength)
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

void CDlgImagePosition::DrawCross(CDC *pDC,CPoint point,int nWidth, int nHeight)
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

void CDlgImagePosition::OnTimer(UINT nIDEvent)
{
	Paint();
	CString str;
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_X));
	SetDlgItemText(IDC_EDIT_IMAGE_X_POS,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_Y));
	SetDlgItemText(IDC_EDIT_IMAGE_Y_POS,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_ZA));
	SetDlgItemText(IDC_EDIT_IMAGE_ZA_POS,str);

	CDialog::OnTimer(nIDEvent);
}
void CDlgImagePosition::OnNMCustomdrawSliderImageSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_IMAGE_SPEED))->GetPos();
	CColorButton::m_iSpeedNo = n;
	CString strTemp;
	strTemp.Format("X:%.4f,Y:%.4f,ZA:%.4f", theApp.m_tSysParam.tAxis[0].vel[0] * n/2000.0,
		theApp.m_tSysParam.tAxis[1].vel[0] * n/2000.0,theApp.m_tSysParam.tAxis[2].vel[0] * n/2000.0);
	SetDlgItemText(IDC_EDIT_IMAGE_SPEED, strTemp);
	*pResult = 0;
}
