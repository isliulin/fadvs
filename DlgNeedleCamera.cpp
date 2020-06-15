// DlgNeedleCamera.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgNeedleCamera.h"
#include "DlgNeedleOne.h"
#include "DlgDownCam.h"
// CDlgNeedleCamera dialog

IMPLEMENT_DYNAMIC(CDlgNeedleCamera, CDialog)

CDlgNeedleCamera::CDlgNeedleCamera(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNeedleCamera::IDD, pParent)
{

}

CDlgNeedleCamera::~CDlgNeedleCamera()
{
}
void CDlgNeedleCamera::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_NEEDLECAMERA, m_picture);
}


BEGIN_MESSAGE_MAP(CDlgNeedleCamera, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgNeedleCamera::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_LEARNMODEL_INCAM, &CDlgNeedleCamera::OnBnClickedBtnLearnmodelIncam)
	ON_BN_CLICKED(IDC_BTN_COMPUTESCALE_INCAM, &CDlgNeedleCamera::OnBnClickedBtnComputescaleIncam)
	ON_BN_CLICKED(IDC_BTN_SEARCHMODEL_INCAM, &CDlgNeedleCamera::OnBnClickedBtnSearchmodelIncam)
	ON_BN_CLICKED(IDC_BTN_MOVETOCENTER_INCAM, &CDlgNeedleCamera::OnBnClickedBtnMovetocenterIncam)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_NEEDLE_SPEED, &CDlgNeedleCamera::OnNMCustomdrawSliderNeedleSpeed)
	ON_BN_CLICKED(IDCANCEL, &CDlgNeedleCamera::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_PUTCROSS, &CDlgNeedleCamera::OnBnClickedBtnPutcross)
END_MESSAGE_MAP()


// CDlgNeedleCamera message handlers

BOOL CDlgNeedleCamera::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  Add extra initialization here
	//PICTURE INIT
	for(int i=0;i<12;i++)
	{
		m_adjustButton[i].m_iIndex = i;
		m_adjustButton[i].SubclassDlgItem(IDC_BTN_NEEDLE_X_NEG_INCAM+i,this);
		//m_adjustButton[i].SetBitmapEx(IDB_BITMAP_ADJUST);
	}
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->SetPos(1);
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->GetPos();
	CJogButton::m_dJogSpdRatio=n;
    g_pFrm->m_CmdRun.m_bNeedleCalibrate=true;//20180814

	m_picture.m_modtype = MOD_RECTANGLE;
	m_picture.m_iImgWidth = milApp.m_lBufSizeX;
	m_picture.m_iImgHeight = milApp.m_lBufSizeY;
	m_picture.m_pImgBuffer = new BYTE[m_picture.m_iImgWidth*m_picture.m_iImgHeight*3];
	m_picture.m_mod_rectangle.dx = theApp.m_tSysParam.BmpMarkRefCam.TopLeft().x + theApp.m_tSysParam.BmpMarkRefCam.Width()/2;
	m_picture.m_mod_rectangle.dy = theApp.m_tSysParam.BmpMarkRefCam.TopLeft().y + theApp.m_tSysParam.BmpMarkRefCam.Height()/2;
	m_picture.m_mod_rectangle.width = theApp.m_tSysParam.BmpMarkRefCam.Width();
	m_picture.m_mod_rectangle.height = theApp.m_tSysParam.BmpMarkRefCam.Height();
	m_picture.m_mod_scale.length = 20;
	m_picture.m_mod_scale.width = 10;
	m_picture.m_mod_scale.height = 10;
	m_picture.RoiRect =  theApp.m_tSysParam.BmpMarkROI;
	m_picture.SetDisplayMarkImage(true);
	m_picture.ImgInit(milApp.m_lBufSizeX,milApp.m_lBufSizeY,24);

	//load model and roi rect;
	ROI=theApp.m_tSysParam.BmpMarkRefCamROI;
	LearnWin=theApp.m_tSysParam.BmpMarkRefCam;

	milApp.SetModelWindow(LearnWin);
	milApp.SetSearchWindow(ROI);
	CString strFile=g_pDoc->GetModeFile(10);
	milApp.RestoreModel(strFile.GetBuffer(strFile.GetLength()));

	CString str;
	str.Format("请移动XYZ将相机与下相机对准！");
	SetDlgItemText(IDC_EDIT_PROMPT,str);

	//光源控制：
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outNeedleLight,FALSE);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,TRUE);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outDetectLight,FALSE);

	::RegisterHotKey(this->GetSafeHwnd(),2000,MOD_ALT,VK_F4);

	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		return TRUE;
	}
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].x,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].y,true))
	{
		return TRUE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgNeedleCamera::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		g_pFrm->m_CmdRun.m_bNeedleCalibrate=false;//20180814
		AfxMessageBox("运动状态不能保存当前位置！");
		return;
	}
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].x=theApp.m_Mv400.GetPos(K_AXIS_X);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].y=theApp.m_Mv400.GetPos(K_AXIS_Y);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].za=theApp.m_Mv400.GetPos(K_AXIS_ZA);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].zb=theApp.m_Mv400.GetPos(K_AXIS_ZB);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].zc=theApp.m_Mv400.GetPos(K_AXIS_ZC);
	theApp.ProductParam(FALSE);
	g_pFrm->m_CmdRun.MoveToZSafety();
	OnOK();
	CDlgNeedleOne dlg;
	dlg.DoModal();
}

void CDlgNeedleCamera::OnBnClickedBtnLearnmodelIncam()
{
	// TODO: Add your control notification handler code here
	//save the model box;
	if(!g_pView->m_ImgStatic.CopyImage(RGB8))
	{
		AfxMessageBox("取像失败！");
		return;
	}
	CString strFile;
	LearnWin.left=theApp.m_tSysParam.BmpMarkRefCam.TopLeft().x = long(m_picture.m_mod_rectangle.dx - m_picture.m_mod_rectangle.width/2);
	LearnWin.top=theApp.m_tSysParam.BmpMarkRefCam.TopLeft().y = long(m_picture.m_mod_rectangle.dy - m_picture.m_mod_rectangle.height/2);
	LearnWin.right=theApp.m_tSysParam.BmpMarkRefCam.BottomRight().x = long(theApp.m_tSysParam.BmpMarkRefCam.TopLeft().x + m_picture.m_mod_rectangle.width);
	LearnWin.bottom=theApp.m_tSysParam.BmpMarkRefCam.BottomRight().y = long(theApp.m_tSysParam.BmpMarkRefCam.TopLeft().y + m_picture.m_mod_rectangle.height);
	ROI=theApp.m_tSysParam.BmpMarkRefCamROI=m_picture.RoiRect;
	theApp.BmpParam(FALSE);
	strFile=g_pDoc->GetModeFile(10);
	milApp.LearnModel(LearnWin);
	milApp.SaveModel(strFile.GetBuffer(strFile.GetLength()));

}

void CDlgNeedleCamera::OnBnClickedBtnComputescaleIncam()
{
	// TODO: Add your control notification handler code here
	tgPos tgTempPixel;
	double dTempX,dTempY;
	double dTempA,dTempB;
	CRect ROI(0,0,milApp.m_lBufSizeX-1,milApp.m_lBufSizeY-1);
	
	// 计算系数前自己走到边界区域
	//dTempX=theApp.m_Mv400.GetPos(K_AXIS_X);
	//dTempY=theApp.m_Mv400.GetPos(K_AXIS_Y);
	//g_pFrm->m_CmdRun.SynchronizeMoveXY((dTempX-0.2),(dTempY-0.2),true);

	if(!g_pView->m_ImgStatic.CopyImage(RGB8))
	{
		AfxMessageBox("抓取图像失败！");
		return;
	}
	if (FindModelCenter(ROI))
	{
		tgTempPixel.x=dCenterX;
		tgTempPixel.y=dCenterY;
	}
	else
	{
		AfxMessageBox("查找模板失败！");
		return;
	}

	dTempX=theApp.m_Mv400.GetPos(K_AXIS_X);
	dTempY=theApp.m_Mv400.GetPos(K_AXIS_Y);
	Sleep(2000);

	g_pFrm->m_CmdRun.SynchronizeMoveXY((dTempX+0.2),(dTempY+0.2),true);
	if(!g_pView->m_ImgStatic.CopyImage(RGB8))//only for the Main Camera to use.
	{
		AfxMessageBox("抓取图像失败！");
		return;
	}
	if (FindModelCenter(ROI))
	{
		dTempA=(dCenterX-tgTempPixel.x)/0.2;
		dTempB=(dCenterY-tgTempPixel.y)/0.2;
	}
	else
	{
		AfxMessageBox("查找模板失败！");
			return;
	}
	theApp.m_tSysParam.BmpMarkRefCamCalibration.dCalibrationA =dTempA;
	theApp.m_tSysParam.BmpMarkRefCamCalibration.dCalibrationB = dTempB;
	theApp.BmpParam(FALSE);
}

bool CDlgNeedleCamera::FindModelCenter(CRect ROI)
{
	//milApp.SetMatchMode(g_pFrm->m_CmdRun.ProductParam.matchMode);
	milApp.SetPatParam(theApp.m_tSysParam.BmpNeedleMarkAcceptance,theApp.m_tSysParam.BmpNeedleMarkAngle);
	milApp.SetSearchWindow(ROI);
	if(!milApp.FindModel(true))
	{
		return false;
	}
	dCenterX = milApp.m_stResult.dResultCenterX[0];
	dCenterY = milApp.m_stResult.dResultCenterY[0];
	dAngle = milApp.m_stResult.dResultAngle[0];
	dScore = milApp.m_stResult.dResultScore[0];

	int left = int((dCenterX-(LearnWin.Width()-1)/2)*m_picture.m_dScale);
	int top = int((dCenterY-(LearnWin.Height()-1)/2)*m_picture.m_dScale);
	int width = int(LearnWin.Width()*m_picture.m_dScale);
	int height = int(LearnWin.Height()*m_picture.m_dScale);
	//实际逻辑功能OK，绘图有误；
	m_picture.GetDC()->Draw3dRect(left,top,width,height,RGB(255,0,0),RGB(255,0,0));
	return true;
}


void CDlgNeedleCamera::OnBnClickedBtnSearchmodelIncam()
{
	// TODO: Add your control notification handler code here
	//the function is only for the Main Camera Use;
	if(!g_pView->m_ImgStatic.CopyImage(RGB8))
	{
		AfxMessageBox("抓取图像失败！");
	}
	ROI=m_picture.RoiRect;
	FindModelCenter(ROI);
}

void CDlgNeedleCamera::OnBnClickedBtnMovetocenterIncam()
{
	// TODO: Add your control notification handler code here
	double dCurXPos,dCurYpos;
	double dX,dY;
	CString str;
	OnBnClickedBtnSearchmodelIncam();
	dCurXPos=theApp.m_Mv400.GetPos(K_AXIS_X);
	dCurYpos=theApp.m_Mv400.GetPos(K_AXIS_Y);
	theApp.m_tSysParam.BmpMarkRefCamCalibration.ConvertPixelToMM(dCenterX-(milApp.m_lBufSizeX-1)/2,
		dCenterY-(milApp.m_lBufSizeY-1)/2,dX,dY);
	str.Format("检测到距离中心偏移值：%0.3f,%0.3f",dX,dY);
	g_pFrm->m_CmdRun.AddMsg(str);
	//保护偏移量异常 
	if ((dX>10)||(dY>10))
	{
		return;
	}
	dCurXPos-=dX;
	dCurYpos-=dY;
	if (!g_pFrm->m_CmdRun.SynchronizeMoveXY(dCurXPos,dCurYpos,true))
	{
		return;
	}
}




void CDlgNeedleCamera::OnNMCustomdrawSliderNeedleSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->GetPos();
	CJogButton::m_dJogSpdRatio=n;
	*pResult = 0;
}

void CDlgNeedleCamera::OnBnClickedCancel()
{
	g_pFrm->m_CmdRun.m_bNeedleCalibrate=false;//20180814
	OnCancel();
}

BOOL CDlgNeedleCamera::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(WM_SYSKEYDOWN==pMsg->message&&VK_F4==pMsg->wParam) //Pretranslate
		return true;

	if(WM_KEYDOWN==pMsg->message&&VK_ESCAPE==pMsg->wParam) //Pretranslate
		return true;

	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgNeedleCamera::OnBnClickedBtnPutcross()
{
	CDlgDownCam dlg;
	dlg.DoModal();
}
