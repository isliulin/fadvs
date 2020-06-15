// DlgDetectCalib.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>
#include "TSCtrlSys.h"
#include "DlgDetectCalib.h"
#include  <math.h>

// CDlgDetectCalib dialog

IMPLEMENT_DYNAMIC(CDlgDetectCalib, CDialog)

CDlgDetectCalib::CDlgDetectCalib(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDetectCalib::IDD, pParent)
{
	m_nImageWidth = IMAGEWIDTH1;
	m_nImageHeight = IMAGEHEIGHT1;
}

CDlgDetectCalib::~CDlgDetectCalib()
{
}

void CDlgDetectCalib::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_STATIC_DETECT_CALIB_PICTURE,m_picture);
}


BEGIN_MESSAGE_MAP(CDlgDetectCalib, CDialog)
	ON_BN_CLICKED(IDC_BTN_DETECT_CALIB_NEXT, &CDlgDetectCalib::OnBnClickedBtnDetectCalibNext)
	ON_BN_CLICKED(IDOK, &CDlgDetectCalib::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgDetectCalib message handlers

BOOL CDlgDetectCalib::OnInitDialog()
{
	CDialog::OnInitDialog();
   // MoveWindow(10,10,1260,984);  //主窗体位置
	m_rectModelWin = theApp.m_tSysParam.BmpPadDetectLearnWin;
	CRect rect(0,0,m_rectModelWin.Width(),m_rectModelWin.Height());
	m_rectROI = rect;
	//m_picture.MoveWindow(0,0,m_nImageWidth,m_nImageHeight,true);
	m_picture.m_modtype = MOD_RECTANGLE;
	m_picture.m_iImgWidth = m_nImageWidth;
	m_picture.m_iImgHeight = m_nImageHeight;
	m_picture.m_pImgBuffer = new BYTE[m_picture.m_iImgWidth*m_picture.m_iImgHeight];
	m_picture.m_mod_rectangle.dx = m_rectModelWin.TopLeft().x + m_rectModelWin.Width()/2;
	m_picture.m_mod_rectangle.dy = m_rectModelWin.TopLeft().y + m_rectModelWin.Height()/2;
	m_picture.m_mod_rectangle.width = m_rectModelWin.Width();
	m_picture.m_mod_rectangle.height = m_rectModelWin.Height();
	m_picture.m_mod_scale.length = 20;
	m_picture.m_mod_scale.width = 10;
	m_picture.m_mod_scale.height = 10;
	m_picture.RoiRect =  m_rectROI;
	m_picture.SetDisplayMarkImage(false);
	m_picture.ImgInit(m_nImageWidth,m_nImageHeight,8);

	//m_picture.MoveWindow(0,0,1252,920);
	//GetDlgItem(IDC_EDIT_DETECT_CALIB_PROMPT)->MoveWindow(40,925,100,25);
	//GetDlgItem(IDC_BTN_DETECT_CALIB_NEXT)->MoveWindow(180,925,40,25);
	//GetDlgItem(IDOK)->MoveWindow(260,925,40,25);
	//GetDlgItem(IDCANCEL)->MoveWindow(340,925,40,25);


	CString strFile = g_pDoc->GetPadModelFile(0);
	//milApp.SetMatchMode(g_pFrm->m_CmdRun.ProductParam.matchMode);
	milApp.SetPadModelWindow(m_rectModelWin);
	milApp.SetPadSearchWindow(m_rectROI);
	milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));
	m_nIndex = 0;
	CString str;
	str.Format("请将移动蓝框到%d模板",m_nIndex);
	SetDlgItemText(IDC_EDIT_DETECT_CALIB_PROMPT,str);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	SetTimer(0,300,NULL);
	return TRUE;
}

void CDlgDetectCalib::OnBnClickedBtnDetectCalibNext()
{
	// TODO: Add your control notification handler code here
	//已知3级行距//列距：只要知道此时3点之间XY像素坐标差值，即可计算得到。
	CString str;
	m_rectROI = m_picture.RoiRect;
	str.Format("left = %d,right = %d,top = %d,bottom = %d",m_rectROI.left,m_rectROI.right,m_rectROI.top,m_rectROI.bottom);
	g_pFrm->m_CmdRun.AddMsg(str);
	milApp.SetPadSearchWindow(m_rectROI);
	if(!milApp.FindPadModels(true))
	{
		g_pFrm->m_CmdRun.AddMsg("No Find.");
		m_nIndex = 0;
		return;
	}
	if(milApp.m_stPadResult.nResultNum==1)
	{
		tgPixelPos[m_nIndex].x = milApp.m_stPadResult.dResultCenterX[0];
		tgPixelPos[m_nIndex].y = milApp.m_stPadResult.dResultCenterY[0];
		m_nIndex++;
	}
	else
	{
		str.Format("Find Multi Image = %d",milApp.m_stPadResult.nResultNum);
		g_pFrm->m_CmdRun.AddMsg(str);
		m_nIndex = 0;
	}
	if(m_nIndex>=3)
	{
		GetDlgItem(IDC_BTN_DETECT_CALIB_NEXT)->EnableWindow(FALSE);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		return;
	}
	str.Format("请将移动蓝框到%d模板",m_nIndex);
	SetDlgItemText(IDC_EDIT_DETECT_CALIB_PROMPT,str);
}

void CDlgDetectCalib::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//ComputeDetectCalibration();
	ComputeScale();
	OnOK();
}

void CDlgDetectCalib::ComputeDetectCalibration()
{
	double dTempA,dTempB,dTempC,dTempD,ret;
	dTempA = ((tgPixelPos[1].x - tgPixelPos[0].x))/g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD;
	dTempB = ((tgPixelPos[1].y - tgPixelPos[0].y))/g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD;
	dTempC = ((tgPixelPos[2].x - tgPixelPos[0].x))/g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD;
	dTempD = ((tgPixelPos[2].y - tgPixelPos[0].y))/g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD;
	double det = dTempA*dTempD - dTempB*dTempC;
	if(fabs(det)<0.001)
	{
		return;
	}
	theApp.m_tSysParam.BmpPadDetectCalibration.dCalibrationA = dTempD/det;
	theApp.m_tSysParam.BmpPadDetectCalibration.dCalibrationB = dTempB/det;
	theApp.m_tSysParam.BmpPadDetectCalibration.dCalibrationC = dTempC/det;
	theApp.m_tSysParam.BmpPadDetectCalibration.dCalibrationD = dTempA/det;

	//double dTempA,dTempB,dTempC,dTempD,ret;
	//dTempA = (tgPixelPos[1].x - tgPixelPos[0].x)/(g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD*(g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn-1));
	//dTempB = (tgPixelPos[1].y - tgPixelPos[0].y)/(g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD*(g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn-1));
	//dTempC = (tgPixelPos[2].x - tgPixelPos[0].x)/(g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD*(g_pFrm->m_CmdRun.ProductParam.ThirdDispRow-1));
	//dTempD = (tgPixelPos[2].y - tgPixelPos[0].y)/(g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD*(g_pFrm->m_CmdRun.ProductParam.ThirdDispRow-1));
	//double det = dTempA*dTempD - dTempB*dTempC;
	//if(fabs(det)<0.001)
	//{
	//	return;
	//}
	//theApp.m_tSysParam.BmpPadDetectCalibration.dCalibrationA = dTempD/det;
	//theApp.m_tSysParam.BmpPadDetectCalibration.dCalibrationB = -dTempB/det;
	//theApp.m_tSysParam.BmpPadDetectCalibration.dCalibrationC = -dTempC/det;
	//theApp.m_tSysParam.BmpPadDetectCalibration.dCalibrationD = dTempA/det;
}

void CDlgDetectCalib::ComputeScale()
{
	double dTempA,dTempB,ret;
	dTempA = ((tgPixelPos[1].x - tgPixelPos[0].x))/g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD;
	dTempB = ((tgPixelPos[2].y - tgPixelPos[1].y))/g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD;
	dTempA=fabs(dTempA);
	dTempB=fabs(dTempB);
	theApp.m_tSysParam.BmpPadDetectCalibration.dCalibrationA = dTempA;
	theApp.m_tSysParam.BmpPadDetectCalibration.dCalibrationB =dTempB;

}
