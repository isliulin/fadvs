// DlgNeedle.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgNeedle.h"
#include <math.h>
CCriticalSection gNeedle_cs;
#define KGB_PI		3.1415926


// CDlgNeedle dialog

IMPLEMENT_DYNAMIC(CDlgNeedle, CDialog)

CDlgNeedle::CDlgNeedle(CWnd* pParent /*=NULL*/)
: CDialog(CDlgNeedle::IDD, pParent)
{

}

CDlgNeedle::~CDlgNeedle()
{
}

void CDlgNeedle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgNeedle, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CDlgNeedle::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgNeedle::OnBnClickedCancel)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_NEEDLE_SPEED, &CDlgNeedle::OnNMCustomdrawSliderNeedleSpeed)
	ON_BN_CLICKED(IDC_BTN_NEEDLE_GAP, &CDlgNeedle::OnBnClickedBtnNeedleGap)
	ON_BN_CLICKED(IDC_BTN_NEEDLE_ADJUST_MOTOR_RESET, &CDlgNeedle::OnBnClickedBtnNeedleAdjustMotorReset)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_NEEDLE_ADJUST_SPEED, &CDlgNeedle::OnNMCustomdrawSliderNeedleAdjustSpeed)
	ON_BN_CLICKED(IDC_BTN_NEEDLE_ADJUST_MOTOR_ZERO, &CDlgNeedle::OnBnClickedBtnNeedleAdjustMotorZero)
	ON_BN_CLICKED(IDC_BTN_NEEDLE_LABEL, &CDlgNeedle::OnBnClickedBtnNeedleLabel)
	ON_BN_CLICKED(IDC_BTN_NEEDLE_ADJUST_NEEDLE, &CDlgNeedle::OnBnClickedBtnNeedleAdjustNeedle)
	ON_BN_CLICKED(IDC_BTN_NEEDLE_ADJUST_CLEAN, &CDlgNeedle::OnBnClickedBtnNeedleAdjustClean)
	ON_BN_CLICKED(IDC_BTN_NEEDLE_CAMERA_LABEL_POS, &CDlgNeedle::OnBnClickedBtnNeedleCameraLabelPos)
	ON_BN_CLICKED(IDC_BTN_NEEDLE_HEIGHT_LABEL_POS, &CDlgNeedle::OnBnClickedBtnNeedleHeightLabelPos)
	ON_BN_CLICKED(IDC_BTN_NEEDLE_ONE_LABEL_POS, &CDlgNeedle::OnBnClickedBtnNeedleOneLabelPos)
	ON_BN_CLICKED(IDC_BTN_NEEDLE_TWO_LABEL_POS, &CDlgNeedle::OnBnClickedBtnNeedleTwoLabelPos)
	ON_BN_CLICKED(IDC_BTN_NEEDLE_THREE_LABEL_POS, &CDlgNeedle::OnBnClickedBtnNeedleThreeLabelPos)
	ON_BN_CLICKED(IDC_BTN_NEEDLE_HEIGHT_POS, &CDlgNeedle::OnBnClickedBtnNeedleHeightPos)
	ON_BN_CLICKED(IDC_BTN_SAVEALL, &CDlgNeedle::OnBnClickedBtnSaveall)
END_MESSAGE_MAP()


// CDlgNeedle message handlers
BOOL CDlgNeedle::OnInitDialog()
{
	g_pFrm->bIsNeedleDlgVisible = true;
	//m_pCamera = NULL;
	m_pBitmapInfo = NULL;
	for(int i=0;i<10;i++)
	{
		m_colorButton[i].m_iIndex = i;
		m_colorButton[i].SubclassDlgItem(IDC_BTN_NEEDLE_X_NEG+i,this);
		m_colorButton[i].SetBitmapEx(IDB_BITMAP_MOVE);
		m_colorButton[i].m_pMv = &theApp.m_Mv400;
	}
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->SetPos(1);
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->GetPos();
	CColorButton::m_iSpeedNo = n;
	for(int i=0;i<8;i++)
	{
		m_adjustButton[i].m_iIndex = i;
		m_adjustButton[i].SubclassDlgItem(IDC_BTN_NEEDLE_A_NEG+i,this);
		m_adjustButton[i].SetBitmapEx(IDB_BITMAP_ADJUST);
	}
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_ADJUST_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_ADJUST_SPEED))->SetPos(500);
	n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_ADJUST_SPEED))->GetPos();
	CJogButton::m_dJogSpdRatio = n;
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outNeedleLight,TRUE);
	m_pDC = GetDlgItem(IDC_STATIC_NEEDLE_PICTURE)->GetDC();
	UpdateUI();
	ImgInit();
	SetAuthority();
	SetTimer(0,100,NULL);
	return TRUE;
}

//实际点胶针头间距 及 清洗时间距由手动输入
void CDlgNeedle::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_NEEDLE_PITCH,str);
	g_pFrm->m_CmdRun.ProductParam.NeedleGap = atof(str);
	GetDlgItemText(IDC_EDIT_NEEDLE_VEL,str);
	g_pFrm->m_CmdRun.ProductParam.NeedleVel = atof(str);
	GetDlgItemText(IDC_EDIT_NEEDLE_ACC,str);
	g_pFrm->m_CmdRun.ProductParam.NeedleAcc = atof(str);
	GetDlgItemText(IDC_EDIT_NEEDLE_CLEAN_PITCH,str);
	g_pFrm->m_CmdRun.ProductParam.CleanGap = atof(str);//手动输入清洗时针头间距
	//补偿标定数据：
	GetDlgItemText(IDC_EDIT_OFFSET2X,str);
	g_pFrm->m_CmdRun.ProductParam.offset2x = atof(str);
	GetDlgItemText(IDC_EDIT_OFFSET2Y,str);
	g_pFrm->m_CmdRun.ProductParam.offset2y = atof(str);
	GetDlgItemText(IDC_EDIT_OFFSET3X,str);
	g_pFrm->m_CmdRun.ProductParam.offset3x = atof(str);
	GetDlgItemText(IDC_EDIT_OFFSET3Y,str);
	g_pFrm->m_CmdRun.ProductParam.offset3y = atof(str);
	theApp.ProductParam(FALSE);
	Close();
	OnOK();
}

void CDlgNeedle::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	Close();
	OnCancel();
}

void CDlgNeedle::UpdateUI()
{
	CString str;
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.NeedleGap);
	SetDlgItemText(IDC_EDIT_NEEDLE_PITCH,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.NeedleVel);
	SetDlgItemText(IDC_EDIT_NEEDLE_VEL,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.NeedleAcc);
	SetDlgItemText(IDC_EDIT_NEEDLE_ACC,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.CleanGap);
	SetDlgItemText(IDC_EDIT_NEEDLE_CLEAN_PITCH,str);
	//Load lable data on ui
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].x);
	SetDlgItemText(IDC_EDIT_LX_VALVEONE,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].x);
	SetDlgItemText(IDC_EDIT_LX_VALVETWO,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2].x);
	SetDlgItemText(IDC_EDIT_LX_VALVETHREE,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].y);
	SetDlgItemText(IDC_EDIT_LY_VALVEONE,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].y);
	SetDlgItemText(IDC_EDIT_LY_VALVETWO,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2].y);
	SetDlgItemText(IDC_EDIT_LY_VALVETHREE,str);
	//最小间距：
	double a,b,c;
	a=b=c=0;
	a=fabs(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].x-g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].x);
	b=fabs(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2].x-g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].x);
	c=max(a,b);
	str.Format("%0.3f",c);
	SetDlgItemText(IDC_EDIT_MIN_DISTANCE,str);
	//
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.offset2x);
	SetDlgItemText(IDC_EDIT_OFFSET2X,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.offset2y);
	SetDlgItemText(IDC_EDIT_OFFSET2Y,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.offset3x);
	SetDlgItemText(IDC_EDIT_OFFSET3X,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.offset3y);
	SetDlgItemText(IDC_EDIT_OFFSET3Y,str);
	
}

void CDlgNeedle::ImgInit()
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

	//m_pCamera = new CDHCamera(); //0701 1600 del :init twices results error;
	//bInitOK = m_pCamera->InitDH(0,25,0,0,IMAGEWIDTH0,IMAGEHEIGHT0,RES_MODE1,false);
	//if(bInitOK==TRUE)
	//{
		//m_pCamera->Live();
		//GetDlgItem(IDC_STATIC_NEEDLE_PICTURE)->MoveWindow(360,0,640,512,TRUE);
	//}
}
void CDlgNeedle::OnNMCustomdrawSliderNeedleSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->GetPos();
	CColorButton::m_iSpeedNo = n;
	CString strTemp;
	strTemp.Format("X:%.4f,Y:%.4f,ZA:%.4f,ZB:%.4f,ZC:%.4f", theApp.m_tSysParam.tAxis[0].vel[0] * n/2000.0,
		theApp.m_tSysParam.tAxis[1].vel[0] * n/2000.0,theApp.m_tSysParam.tAxis[2].vel[0] * n/2000.0,
		theApp.m_tSysParam.tAxis[3].vel[0] * n/2000.0,theApp.m_tSysParam.tAxis[4].vel[0] * n/2000.0);
	SetDlgItemText(IDC_EDIT_NEEDLE_SPEED, strTemp);
	*pResult = 0;
}

void CDlgNeedle::Close()
{
	g_pFrm->m_CmdRun.MoveToZSafety();
	g_pFrm->bIsNeedleDlgVisible = false;
	KillTimer(0);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outNeedleLight,FALSE);
	if(m_pBitmapInfo!=NULL)
	{
		delete[] m_pBitmapInfo;
		m_pBitmapInfo = NULL;
	}
	//if(m_pCamera!=NULL)  //0701 标定方式更改；
	//{
	//	m_pCamera->Stop();
	//	m_pCamera->Free();
	//	delete m_pCamera;
	//	m_pCamera = NULL;
	//}
	if(m_pDC!=NULL)
	{
		ReleaseDC(m_pDC);
	}
}

void CDlgNeedle::OnTimer(UINT nIDEvent)
{
	CurrentPos();
	EncoderPos();
	Paint();  
	CDialog::OnTimer(nIDEvent);
}

void CDlgNeedle::Paint()
{
	if(g_pView->m_ImgStatic.m_pCamera.m_pImageBuffer == NULL)
	{
		return;
	}

	HDRAWDIB m_hDrawDib = DrawDibOpen();
	m_pDC->SetBkMode(TRANSPARENT);
	/*DrawDibDraw(m_hDrawDib,m_pDC->GetSafeHdc(),0,0,IMAGEWIDTH0,IMAGEHEIGHT0,
		&m_pBitmapInfo->bmiHeader,m_pCamera->m_pImageBuffer,0,0,m_pBitmapInfo->bmiHeader.biWidth,
		m_pBitmapInfo->bmiHeader.biHeight,SRCCOPY);*/

	gNeedle_cs.Lock();
	DrawDibDraw(m_hDrawDib,m_pDC->GetSafeHdc(),0,0,IMAGEWIDTH0,IMAGEHEIGHT0,
		&m_pBitmapInfo->bmiHeader,g_pView->m_ImgStatic.m_pCamera.m_pImageBuffer,0,0,m_pBitmapInfo->bmiHeader.biWidth,
		m_pBitmapInfo->bmiHeader.biHeight,SRCCOPY);
	gNeedle_cs.Unlock();

	DrawCross(m_pDC,CPoint(IMAGEWIDTH0/2,IMAGEHEIGHT0/2),IMAGEWIDTH0/2,IMAGEHEIGHT0/2);
	PainScale(m_pDC,CPoint(IMAGEWIDTH0/2,IMAGEHEIGHT0/2),4,4,10);
	DrawDibClose(m_hDrawDib);
}

void CDlgNeedle::DrawCross(CDC *pDC,CPoint point,int nWidth, int nHeight)
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

void CDlgNeedle::PainScale(CDC *pDC,CPoint point,int nWidth, int nHeight,int nLength)
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



//相机测试针头间距
void CDlgNeedle::OnBnClickedBtnNeedleGap()
{
	// TODO: Add your control notification handler code here
	CString str;
	g_pFrm->m_dlgRCD.SetParam(0,2);
	g_pFrm->m_dlgRCD.DoModal();
	str.Format("%.3f",g_pFrm->m_CmdRun.ProductParam.NeedleGap);
	SetDlgItemText(IDC_EDIT_NEEDLE_PITCH,str);
	GetDlgItem(IDC_EDIT_NEEDLE_PITCH)->UpdateData(FALSE);
}

void CDlgNeedle::CurrentPos()
{
	CString str;
	double dValue = 0.0;
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_X));
	SetDlgItemText(IDC_EDIT_NEEDLE_CURRENT_POS_X,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_Y));
	SetDlgItemText(IDC_EDIT_NEEDLE_CURRENT_POS_Y,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_ZA));
	SetDlgItemText(IDC_EDIT_NEEDLE_CURRENT_POS_ZA,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_ZB));
	SetDlgItemText(IDC_EDIT_NEEDLE_CURRENT_POS_ZB,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_ZC));
	SetDlgItemText(IDC_EDIT_NEEDLE_CURRENT_POS_ZC,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_A));
	SetDlgItemText(IDC_EDIT_NEEDLE_CURRENT_POS_A,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_B));
	SetDlgItemText(IDC_EDIT_NEEDLE_CURRENT_POS_B,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_C));
	SetDlgItemText(IDC_EDIT_NEEDLE_CURRENT_POS_C,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_D));
	SetDlgItemText(IDC_EDIT_NEEDLE_CURRENT_POS_D,str);

	if(g_pFrm->m_CmdRun.ReadHeightSensorValue(&dValue))
	{
		str.Format("%0.3f",dValue);
		SetDlgItemText(IDC_EDIT_NEEDLE_HIGHT_VALUE,str);
	}
	//阀体2,3转换位置：前后方向
	double dTempPos,dScale;
	dTempPos=0;
	dScale=tan(g_pFrm->m_CmdRun.ProductParam.AdjustNeedleAngle[1]/180.0*KGB_PI);
	
	dTempPos=theApp.m_Mv400.GetPos(K_AXIS_B)/dScale;
	str.Format("%0.3f",dTempPos);
	SetDlgItemText(IDC_EDIT_CONVERTPOS_FA2,str);
	dTempPos=theApp.m_Mv400.GetPos(K_AXIS_D)/dScale;
	str.Format("%0.3f",dTempPos);
	SetDlgItemText(IDC_EDIT_CONVERTPOS_FA3,str);
}

void CDlgNeedle::EncoderPos()
{
	CString str;
	str.Format("%0.3f",theApp.m_Mv400.GetEncPos(K_AXIS_X));
	SetDlgItemText(IDC_EDIT_NEEDLE_ENC_POS_X,str);
	str.Format("%0.3f",theApp.m_Mv400.GetEncPos(K_AXIS_Y));
	SetDlgItemText(IDC_EDIT_NEEDLE_ENC_POS_Y,str);
	str.Format("%0.3f",theApp.m_Mv400.GetEncPos(K_AXIS_ZA));
	SetDlgItemText(IDC_EDIT_NEEDLE_ENC_POS_ZA,str);
	str.Format("%0.3f",theApp.m_Mv400.GetEncPos(K_AXIS_ZB));
	SetDlgItemText(IDC_EDIT_NEEDLE_ENC_POS_ZB,str);
	str.Format("%0.3f",theApp.m_Mv400.GetEncPos(K_AXIS_ZC));
	SetDlgItemText(IDC_EDIT_NEEDLE_ENC_POS_ZC,str);
	str.Format("%0.3f",theApp.m_Mv400.GetEncPos(K_AXIS_A));
	SetDlgItemText(IDC_EDIT_NEEDLE_ENC_POS_A,str);
	str.Format("%0.3f",theApp.m_Mv400.GetEncPos(K_AXIS_B));
	SetDlgItemText(IDC_EDIT_NEEDLE_ENC_POS_B,str);
	str.Format("%0.3f",theApp.m_Mv400.GetEncPos(K_AXIS_C));
	SetDlgItemText(IDC_EDIT_NEEDLE_ENC_POS_C,str);
	str.Format("%0.3f",theApp.m_Mv400.GetEncPos(K_AXIS_D));
	SetDlgItemText(IDC_EDIT_NEEDLE_ENC_POS_D,str);
}

void CDlgNeedle::OnBnClickedBtnNeedleAdjustMotorReset()
{
	// TODO: Add your control notification handler code here
	if(theApp.m_Mv400.IsInitOK())
	{
		if(!g_pFrm->m_CmdRun.MoveToZSafety())
		{
			AfxMessageBox("Z轴回安全位失败！");
			return;
		}
		if(!g_pFrm->m_CmdRun.AdjustMotorHome())
		{
			AfxMessageBox("调整电机复位失败！");
		}
	}
}

void CDlgNeedle::OnNMCustomdrawSliderNeedleAdjustSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_ADJUST_SPEED))->GetPos();
	CJogButton::m_dJogSpdRatio = n;
	CString strTemp;
	strTemp.Format("A:%.4f,B:%.4f,C:%.4f,D:%.4f", theApp.m_tSysParam.tAxis[K_AXIS_A-1].vel[0] * n/2000.0,theApp.m_tSysParam.tAxis[K_AXIS_B-1].vel[0] * n/2000.0,
		theApp.m_tSysParam.tAxis[K_AXIS_C-1].vel[0] * n/2000.0,theApp.m_tSysParam.tAxis[K_AXIS_D-1].vel[0] * n/2000.0);
	SetDlgItemText(IDC_EDIT_NEEDLE_ADJUST_SPEED, strTemp);
	*pResult = 0;
}

void CDlgNeedle::OnBnClickedBtnNeedleAdjustMotorZero()
{
	// TODO: 在此添加控件通知处理程序代码
	if(theApp.m_Mv400.IsInitOK())
	{
		if(!g_pFrm->m_CmdRun.MoveToZSafety())
		{
			AfxMessageBox("Z轴回安全位失败！");
			return;
		}
		if(!g_pFrm->m_CmdRun.AdjustMotorZero())
		{
			AfxMessageBox("调整电机回零失败！");
		}
	}
}

//记录几个偏移求值Pos;
void CDlgNeedle::OnBnClickedBtnNeedleLabel()
{
	// TODO: 在此添加控件通知处理程序代码
	g_pFrm->m_dlgLabel.DoModal();
}

void CDlgNeedle::OnBnClickedBtnNeedleAdjustNeedle()
{
	// TODO: 在此添加控件通知处理程序代码
	if(theApp.m_Mv400.IsInitOK())
	{
		if(!g_pFrm->m_CmdRun.MoveToZSafety())
		{
			AfxMessageBox("Z轴回安全位失败！");
			return;
		}
		if(!g_pFrm->m_CmdRun.AdjustNeedle())
		{
			AfxMessageBox("调整电机移动失败！");
		}
	}
}

void CDlgNeedle::OnBnClickedBtnNeedleAdjustClean()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		AfxMessageBox("Z轴回安全位失败！");
		return;
	}
	if(!g_pFrm->m_CmdRun.AdjustMotorToCleanPos())
	{
		AfxMessageBox("调整电机回零失败！");
	}
}

//相机Dispense relative Pos;
void CDlgNeedle::OnBnClickedBtnNeedleCameraLabelPos()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		AfxMessageBox("Z轴回安全位失败！");
		return;
	}
	if(!g_pFrm->m_CmdRun.AdjustMotorZero())
	{
		AfxMessageBox("调整电机移动失败！");
		return;
	}
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].x,
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].y,true))
	{
		AfxMessageBox("移动到相机标定位失败！");
		return;
	}
	if (!g_pFrm->m_CmdRun.SynchronizeMoveZ(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].za,0,0,false,true))
	{
		AfxMessageBox("移动到相机标定位失败！");
		return;

	}
	
}

//记录测高时执行位置
void CDlgNeedle::OnBnClickedBtnNeedleHeightLabelPos()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		AfxMessageBox("Z轴回安全位失败！");
		return;
	}
	if(!g_pFrm->m_CmdRun.AdjustMotorZero())
	{
		AfxMessageBox("调整电机移动失败！");
		return;
	}
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[4].x,
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[4].y,true))
	{
		AfxMessageBox("移动到测高标定位失败！");
		return;
	}

}

//DlgLable记录Lable  Pos  
void CDlgNeedle::OnBnClickedBtnNeedleOneLabelPos()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		AfxMessageBox("Z轴回安全位失败！");
		return;
	}
	//因标定时在0标定，故确认前先归位；
	if(!g_pFrm->m_CmdRun.AdjustMotorZero())
	{
		AfxMessageBox("调整电机移动失败！");
		return;
	}
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].x,
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].y,true))
	{
		AfxMessageBox("移动到1#针头标定位失败！");
		return;
	}
    if (!g_pFrm->m_CmdRun.SynchronizeMoveZ(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].za,0,0,false,true))
    {
		AfxMessageBox("移动到1#针头标定位失败！");
		return;

    }

}

void CDlgNeedle::OnBnClickedBtnNeedleTwoLabelPos()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		AfxMessageBox("Z轴回安全位失败！");
		return;
	}
	if(!g_pFrm->m_CmdRun.AdjustMotorZero())
	{
		AfxMessageBox("调整电机移动失败！");
		return;
	}
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].x,
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].y,true))
	{
		AfxMessageBox("移动到2#针头标定位失败！");
		return;
	}
	if (!g_pFrm->m_CmdRun.SynchronizeMoveZ(0,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].zb,0,false,true))
	{
		AfxMessageBox("移动到2#针头标定位失败！");
		return;

	}
}

void CDlgNeedle::OnBnClickedBtnNeedleThreeLabelPos()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		AfxMessageBox("Z轴回安全位失败！");
		return;
	}
	if(!g_pFrm->m_CmdRun.AdjustMotorZero())
	{
		AfxMessageBox("调整电机移动失败！");
		return;
	}
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2].x,
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2].y,true))
	{
		AfxMessageBox("移动到3#针头标定位失败！");
		return;
	}
	if (!g_pFrm->m_CmdRun.SynchronizeMoveZ(0,0,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2].zc,false,true))
	{
		AfxMessageBox("移动到3#针头标定位失败！");
		return;

	}
}

void CDlgNeedle::OnBnClickedBtnNeedleHeightPos()
{
	// TODO: Add your control notification handler code here
	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		AfxMessageBox("Z轴回安全位失败！");
		return;
	}
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].x,
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].y,true))
	{
		AfxMessageBox("移动到测高位失败！");
		return;
	}
	if(!g_pFrm->m_CmdRun.MoveZ(K_AXIS_ZA,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].za,true))//定位到测高高度：26-34
	{
		AfxMessageBox("移动到测高位失败！");
		return;
	}
}



void CDlgNeedle::OnBnClickedBtnSaveall()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_NEEDLE_PITCH,str);
	g_pFrm->m_CmdRun.ProductParam.NeedleGap = atof(str);
	GetDlgItemText(IDC_EDIT_NEEDLE_VEL,str);
	g_pFrm->m_CmdRun.ProductParam.NeedleVel = atof(str);
	GetDlgItemText(IDC_EDIT_NEEDLE_ACC,str);
	g_pFrm->m_CmdRun.ProductParam.NeedleAcc = atof(str);
	GetDlgItemText(IDC_EDIT_NEEDLE_CLEAN_PITCH,str);
	g_pFrm->m_CmdRun.ProductParam.CleanGap = atof(str);//手动输入清洗时针头间距
	//补偿标定数据：
	GetDlgItemText(IDC_EDIT_OFFSET2X,str);
	g_pFrm->m_CmdRun.ProductParam.offset2x = atof(str);
	GetDlgItemText(IDC_EDIT_OFFSET2Y,str);
	g_pFrm->m_CmdRun.ProductParam.offset2y = atof(str);
	GetDlgItemText(IDC_EDIT_OFFSET3X,str);
	g_pFrm->m_CmdRun.ProductParam.offset3x = atof(str);
	GetDlgItemText(IDC_EDIT_OFFSET3Y,str);
	g_pFrm->m_CmdRun.ProductParam.offset3y = atof(str);
	theApp.ProductParam(FALSE);
}

void CDlgNeedle::SetAuthority()
{
	GetDlgItem(IDC_EDIT_NEEDLE_VEL)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_NEEDLE_ACC)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_NEEDLE_PITCH)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_NEEDLE_CLEAN_PITCH)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);

	GetDlgItem(IDC_EDIT_OFFSET2X)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_OFFSET2Y)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_OFFSET3X)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_OFFSET3Y)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
}