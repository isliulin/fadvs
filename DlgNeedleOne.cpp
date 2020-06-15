// DlgNeedleOne.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgNeedleOne.h"
#include "DlgNeedleTwo.h"
#include "DlgNeedleCamera.h"
#include "Function.h"
#include <math.h>


// CDlgNeedleOne dialog

IMPLEMENT_DYNAMIC(CDlgNeedleOne, CDialog)

CDlgNeedleOne::CDlgNeedleOne(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNeedleOne::IDD, pParent)
{
	m_nImageWidth = milApp.m_lNeedleBufSizeX;
	m_nImageHeight = milApp.m_lNeedleBufSizeY;

}

CDlgNeedleOne::~CDlgNeedleOne()
{
}

void CDlgNeedleOne::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC_NEEDLEONE, m_picture);
	DDX_Control(pDX, IDC_SLIDER_NEEDLE_EXPOSURE_TIME, m_sliderShutter);
	DDX_Control(pDX, IDC_SLIDER_NEEDLE_GAIN, m_sliderGain);
}


BEGIN_MESSAGE_MAP(CDlgNeedleOne, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgNeedleOne::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_MOVETOCENTER_NDLEONE, &CDlgNeedleOne::OnBnClickedBtnMovetocenterNdleone)
	ON_BN_CLICKED(IDC_BTN_LEARNMODEL_NDLEONE, &CDlgNeedleOne::OnBnClickedBtnLearnmodelNdleone)
	ON_BN_CLICKED(IDC_BTN_COMPUTESCALE_NDLEONE, &CDlgNeedleOne::OnBnClickedBtnComputescaleNdleone)
	ON_BN_CLICKED(IDC_BTN_SEARCHMODEL_NDLEONE, &CDlgNeedleOne::OnBnClickedBtnSearchmodelNdleone)
	
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_NEEDLE_SPEED, &CDlgNeedleOne::OnNMCustomdrawSliderNeedleSpeed)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT_NEEDLE_ACCEPTANCE, &CDlgNeedleOne::OnEnChangeEditNeedleAcceptance)
	ON_EN_CHANGE(IDC_EDIT_NEEDLE_ANGLE, &CDlgNeedleOne::OnEnChangeEditNeedleAngle)
	ON_BN_CLICKED(IDCANCEL, &CDlgNeedleOne::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgNeedleOne message handlers

void CDlgNeedleOne::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		g_pFrm->m_CmdRun.m_bNeedleCalibrate=false;//20180814
		AfxMessageBox("运动状态不能保存当前位置！");
		return;
	}
	double dXOffset,dXNewPos;                                            //////2017-12-08
	double dYOffset,dYNewPos;                                           //////
	dXOffset=dYOffset=0;                                               //////
	dXNewPos=theApp.m_Mv400.GetPos(K_AXIS_X);                         //////
	dXOffset=dXNewPos-g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].x;  //////
	dYNewPos=theApp.m_Mv400.GetPos(K_AXIS_Y);                        //////
	dYOffset=dYNewPos-g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].y; //////

    g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].x=theApp.m_Mv400.GetPos(K_AXIS_X);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].y=theApp.m_Mv400.GetPos(K_AXIS_Y);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].za=theApp.m_Mv400.GetPos(K_AXIS_ZA);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].zb=theApp.m_Mv400.GetPos(K_AXIS_ZB);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].zc=theApp.m_Mv400.GetPos(K_AXIS_ZC);
	///////////////////////////////////////////////////////////////////////////////////////////2017-12-08
	g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[0]+=dXOffset; //自动修正更新X偏移值     //点位1
	theApp.m_tSysParam.CleanPosition[0]+=dXOffset;                                         //点位2
	for(int i=0;i<g_pFrm->m_CmdRun.ProductParam.TestDotNumber;i++)                         //点位3
	{
		g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].x+=dXOffset;
	}
	theApp.m_tSysParam.DischargeGluePostion[0]+=dXOffset;                                  //点位4
	////
	g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[1]+=dYOffset; //自动修正更新Y偏移值     //点位1
	theApp.m_tSysParam.CleanPosition[1]+=dYOffset;                                         //点位2
	for(int i=0;i<g_pFrm->m_CmdRun.ProductParam.TestDotNumber;i++)                         //点位3
	{
		g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].y+=dYOffset;
	}
	theApp.m_tSysParam.DischargeGluePostion[1]+=dYOffset;      
	////////////////////////////////////////////////////////////////////////////////////////////2017-12-08
	theApp.ProductParam(FALSE);
	CString str;
	str.Format("Needle 1 Z Value at last:%0.3f",g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].za);
	g_pFrm->m_CmdRun.AddMsg(str);
	if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
	{
		str.Format("手动标定针头1完成时XYZ切换到中速时失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		AfxMessageBox("设置电机参数错误！");
	}
	g_pFrm->m_CmdRun.MoveToZSafety();
	OnOK();
	CDlgNeedleTwo dlg;
	dlg.DoModal();
}

BOOL CDlgNeedleOne::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString str;
	for(int i=0;i<12;i++)
	{
		m_adjustButton[i].m_iIndex = i;
		m_adjustButton[i].SubclassDlgItem(IDC_BTN_NEEDLEONE_X_NEG_NDLEONE+i,this);
		//m_adjustButton[i].SetBitmapEx(IDB_BITMAP_ADJUST);
		//m_adjustButton[i].m_pMv = &theApp.m_Mv400;
	}

	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->SetPos(1);
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->GetPos();
	CJogButton::m_dJogSpdRatio = n;

	m_sliderShutter.SetRange(0,999);
	m_sliderGain.SetRange(0,63);
	m_sliderShutter.SetPos(theApp.m_tSysParam.NeedleCameraExposure);
	m_sliderGain.SetPos(theApp.m_tSysParam.NeedleCameraGain);
	g_pView->m_ImgStatic.m_pCameraNeedle.SetExposureTime(theApp.m_tSysParam.NeedleCameraExposure);
    g_pView->m_ImgStatic.m_pCameraNeedle.SetGain(theApp.m_tSysParam.NeedleCameraGain);
	g_pView->m_ImgStatic.ImgNeedleLive();

	str.Format("%0.3f",theApp.m_tSysParam.BmpNeedleMarkAcceptance);
	SetDlgItemText(IDC_EDIT_NEEDLE_ACCEPTANCE,str);
	str.Format("%0.3f",theApp.m_tSysParam.BmpNeedleMarkAngle);
	SetDlgItemText(IDC_EDIT_NEEDLE_ANGLE,str);

	//start the camera to grab:the camera of refpoint;
	//not pointer ,can not judge like detect camera;
	
	m_rectModelWin=theApp.m_tSysParam.BmpNeedleLearnWin;
	m_rectROI=theApp.m_tSysParam.BmpNeedleROI;
	m_picture.m_modtype = MOD_RECTANGLE;
	m_picture.m_iImgWidth = m_nImageWidth;
	m_picture.m_iImgHeight = m_nImageHeight;
	m_picture.m_pImgBuffer = new BYTE[m_picture.m_iImgWidth*m_picture.m_iImgHeight*3];
	m_picture.m_mod_rectangle.dx = m_rectModelWin.TopLeft().x + m_rectModelWin.Width()/2;
	m_picture.m_mod_rectangle.dy = m_rectModelWin.TopLeft().y + m_rectModelWin.Height()/2;
	m_picture.m_mod_rectangle.width = m_rectModelWin.Width();
	m_picture.m_mod_rectangle.height = m_rectModelWin.Height();//图片上MARK框大小更新
	m_picture.m_mod_scale.length = 10;//20 -10
	m_picture.m_mod_scale.width = 10;
	m_picture.m_mod_scale.height = 10;
	m_picture.RoiRect =  m_rectROI;//图片上ROI框大小更新
	m_picture.SetDisplayMarkImage(false);
	m_picture.SetDisplayNeedleImage(true);/////////////////////////////////
	m_picture.yOffset=0;
	m_picture.ImgInit(m_nImageWidth,m_nImageHeight,24);//the distingwish of 8 and 24 in camera;

	//image process set
	CString strFile = g_pDoc->GetPadModelFile(11);
	milApp.SetMatchMode(g_pFrm->m_CmdRun.ProductParam.matchMode);  //0630 patmatch
	milApp.SetPadModelWindow(m_rectModelWin);
	milApp.SetPadSearchWindow(m_rectROI);
	milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));
	str.Format("请手动移动XYZ轴将1#针头移动到相机中心！");
	SetDlgItemText(IDC_EDIT_PROMPT,str);

	//光源控制：
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outNeedleLight,TRUE);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,FALSE);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outDetectLight,FALSE);
    ::RegisterHotKey(this->GetSafeHwnd(),2001,MOD_ALT,VK_F4);
   
	//开始调整之前：ABCD 回初始点：
	if(!g_pFrm->m_CmdRun.AdjustMotorHome())
	{
		str.Format("调整马达回零失败！");
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		AfxMessageBox("调整马达复位失败，请先排除调整马达故障！");
	}
    //初步预留3mm空间，避免自动对针时针头2，针头3需要负向调整时没有空间。
	theApp.m_Mv400.MoveDot(K_AXIS_C,theApp.m_tSysParam.MotorCInitPos,theApp.m_tSysParam.tAxis[K_AXIS_C-1].highHomeVel,theApp.m_tSysParam.tAxis[K_AXIS_C-1].homeAcc,true,true);//8
	CFunction::DelayEx(2);
	theApp.m_Mv400.MoveDot(K_AXIS_A,theApp.m_tSysParam.MotorAInitPos,theApp.m_tSysParam.tAxis[K_AXIS_A-1].highHomeVel,theApp.m_tSysParam.tAxis[K_AXIS_A-1].homeAcc,true,true);//3

    //手动标定进入低速模式
	if(!g_pFrm->m_CmdRun.SetAxisProfile(LOW_VEL))
	{
		str.Format("手动标定针头1时切换到低速时失败！");
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		AfxMessageBox("设置电机参数错误！");
	}

	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		str.Format("手动标定针头1时Z到安全位失败！");
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		return TRUE;
	}
	//XY 到记忆位置
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].x,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].y,true))
	{
		str.Format("手动标定针头1时定位到上次标定位失败！");
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		return TRUE;
	}

	return TRUE; 
}

//到中心点
void CDlgNeedleOne::OnBnClickedBtnMovetocenterNdleone()
{
	// TODO: Add your control notification handler code here
	double dCurXPos,dCurYpos;
	double dX,dY;
	CString str;
	OnBnClickedBtnSearchmodelNdleone();
	dCurXPos=theApp.m_Mv400.GetPos(K_AXIS_X);
	dCurYpos=theApp.m_Mv400.GetPos(K_AXIS_Y);
	str.Format("到针头中心时纠偏前实际坐标：X:%0.3f Y:%0.3f",dCurXPos,dCurYpos);
	g_pFrm->m_CmdRun.AddMsg(str);
	g_pFrm->m_CmdRun.PutLogIntoList(str); 

	theApp.m_tSysParam.BmpNeedleCalibration.ConvertPixelToMM(dCenterX-(milApp.m_lNeedleBufSizeX-1)/2,
		dCenterY-(milApp.m_lNeedleBufSizeY-1)/2,dX,dY);
	//增加模板查找实时数据输出：
	str.Format("到中心时检测得偏移量（mm）:X:%0.3f Y:%0.3f",dX,dY);
	g_pFrm->m_CmdRun.AddMsg(str);
	g_pFrm->m_CmdRun.PutLogIntoList(str);

	if ((fabs(dX)>2)||(fabs(dY)>2))
	{
		AfxMessageBox("偏移过大，请重新计算！");
		return;
	}
	dCurXPos-=dX;
	dCurYpos-=dY;
	if (!g_pFrm->m_CmdRun.SynchronizeMoveXY(dCurXPos,dCurYpos,true))
	{
		return;
	}

	str.Format("针头1到中心时执行完毕纠偏动作！");
	g_pFrm->m_CmdRun.AddMsg(str);
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	Sleep(2000);
	dCurXPos=theApp.m_Mv400.GetPos(K_AXIS_X);
	dCurYpos=theApp.m_Mv400.GetPos(K_AXIS_Y);
	str.Format("纠偏之后实际坐标：X:%0.3f Y:%0.3f",dCurXPos,dCurYpos);
	g_pFrm->m_CmdRun.AddMsg(str);
	g_pFrm->m_CmdRun.PutLogIntoList(str);
}

//学模板
void CDlgNeedleOne::OnBnClickedBtnLearnmodelNdleone()
{
	// TODO: Add your control notification handler code here
	CString str;
	CFunction::DelayEx(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay);
	str.Format("1号针头开始学模板！");
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);
	//prepare the image:copy to buffer;
	if (!g_pView->m_ImgStatic.CopyNeedleImage(RGB8))
	{
		AfxMessageBox("1号针头相机取像失败！");
		str.Format("1号针头图像拷贝失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return;
	}
	str.Format("1号针头图像拷贝成功！");
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);
	CString strFile;
	//prepare the image rect to buffer;
	m_rectModelWin.left=theApp.m_tSysParam.BmpNeedleLearnWin.TopLeft().x = long(m_picture.m_mod_rectangle.dx - m_picture.m_mod_rectangle.width/2);
	m_rectModelWin.top=theApp.m_tSysParam.BmpNeedleLearnWin.TopLeft().y = long(m_picture.m_mod_rectangle.dy - m_picture.m_mod_rectangle.height/2);
	m_rectModelWin.right=theApp.m_tSysParam.BmpNeedleLearnWin.BottomRight().x = theApp.m_tSysParam.BmpNeedleLearnWin.TopLeft().x + m_picture.m_mod_rectangle.width;
	m_rectModelWin.bottom=theApp.m_tSysParam.BmpNeedleLearnWin.BottomRight().y = theApp.m_tSysParam.BmpNeedleLearnWin.TopLeft().y + m_picture.m_mod_rectangle.height;
	theApp.BmpParam(FALSE);
	str.Format("1号针头模板框坐标：左：%d///右：%d///上:%d///下：%d",m_rectModelWin.left,m_rectModelWin.right,m_rectModelWin.top,m_rectModelWin.bottom);
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);

	strFile=g_pDoc->GetModeFile(11);
	milApp.LearnModel(m_rectModelWin);
	//save the model file;
	milApp.SaveModel(strFile.GetBuffer(strFile.GetLength()));
	m_rectROI=m_picture.RoiRect;
	theApp.BmpParam(FALSE);
	milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));
}

//计算系数
void CDlgNeedleOne::OnBnClickedBtnComputescaleNdleone()
{
	// TODO: Add your control notification handler code here
	CString str;
	tgPos tgTempPixel;
	double dTempX,dTempY;
	double dTempA,dTempB;
	CRect ROI(0,0,milApp.m_lNeedleBufSizeX-1,milApp.m_lNeedleBufSizeY-1);
	//计算系数前自行走到边界区域；
	//dTempX=theApp.m_Mv400.GetPos(K_AXIS_X);
	//dTempY=theApp.m_Mv400.GetPos(K_AXIS_Y);
	//g_pFrm->m_CmdRun.SynchronizeMoveXY((dTempX-1.0),(dTempY-1.0),true);

	str.Format("针头1计算系数前第一次取像...");
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);
	
	CFunction::DelayEx(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay);
	if(!g_pView->m_ImgStatic.CopyNeedleImage(RGB8))
	{
		AfxMessageBox("抓取图像失败！");
		str.Format("针头1计算系数时：首次拷贝图像失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return;
	}

	str.Format("针头1计算系数时：首次拷贝图像成功！");
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);
    
	str.Format("针头1计算系数时找首张图片中心...");
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);

	if (FindModelCenter(ROI))
	{
		tgTempPixel.x=dCenterX;
		tgTempPixel.y=dCenterY;
		str.Format("针头1计算系数时：图像1中心点坐标：X:%d;Y:%d",dCenterX,dCenterY);
        g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
	}
	else
	{
		AfxMessageBox("查找模板失败！");
		str.Format("针头1计算系数时：图像1找中心失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return;
	}

	dTempX=theApp.m_Mv400.GetPos(K_AXIS_X);
	dTempY=theApp.m_Mv400.GetPos(K_AXIS_Y);
	g_pFrm->m_CmdRun.SynchronizeMoveXY((dTempX+0.5),(dTempY+0.5),true);
	Sleep(2000);
	CFunction::DelayEx(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay);
	str.Format("[针头1计算系数时]：开始拷贝图像2");
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);

	if(!g_pView->m_ImgStatic.CopyNeedleImage(RGB8))//only for the Main Camera to use.
	{
		AfxMessageBox("抓取图像失败！");
		str.Format("[针头1计算系数时]：图像2拷贝失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return;
	}
	str.Format("[针头1计算系数时]：图像2拷贝成功！");
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);

    str.Format("[针头1计算系数时]：开始找图像2中心点");
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);

	if (FindModelCenter(ROI))
	{
		str.Format("图像2找中心成功：X：%d...Y:%d",dCenterX,dCenterY);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		dTempA=(dCenterX-tgTempPixel.x)/0.5;
		dTempB=(dCenterY-tgTempPixel.y)/0.5;
		if ((0==dTempA)||(0==dTempB))
		{
			str.Format("针头1系数计算异常！");
			g_pFrm->m_CmdRun.PutLogIntoList(str);
			g_pFrm->m_CmdRun.AddMsg(str);
		}
	}
	else
	{
		AfxMessageBox("查找模板失败！");
		str.Format("图像2找中心失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return;
	}
	str.Format("[针头1计算系数]成功：dTempA:%d;dTempB：%d",dTempA,dTempB);
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);
	theApp.m_tSysParam.BmpNeedleCalibration.dCalibrationA =dTempA;
	theApp.m_tSysParam.BmpNeedleCalibration.dCalibrationB = dTempB;
	theApp.BmpParam(FALSE);
}


//找模板中心
bool CDlgNeedleOne::FindModelCenter(CRect ROI)
{
	CString str;
	double dX,dY;
	milApp.SetPatParam(theApp.m_tSysParam.BmpNeedleMarkAcceptance,theApp.m_tSysParam.BmpNeedleMarkAngle);
	milApp.SetSearchWindow(ROI);
	if(!milApp.FindModel(true))
	{
		CString str;
		str.Format("针头1找模板失败");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return false;
	}
	dCenterX = milApp.m_stResult.dResultCenterX[0];
	dCenterY = milApp.m_stResult.dResultCenterY[0];
	str.Format("针头1找模板成功：中心X:%d;Y:%d",dCenterX,dCenterY);
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);

	theApp.m_tSysParam.BmpNeedleCalibration.ConvertPixelToMM(dCenterX - (milApp.m_lNeedleBufSizeX-1)/2,
		dCenterY - (milApp.m_lNeedleBufSizeY-1)/2,dX,dY);
	str.Format("针头1找到模板偏移:X:%0.3f Y: %0.3f",dX,dY);
	g_pFrm->m_CmdRun.AddMsg(str);
	g_pFrm->m_CmdRun.PutLogIntoList(str);

	//绘制模板边框；
	int left = int((dCenterX-(m_rectModelWin.Width()-1)/2)*m_picture.m_dScale);
	int top = int((dCenterY-(m_rectModelWin.Height()-1)/2)*m_picture.m_dScale);
	int width = int(m_rectModelWin.Width()*m_picture.m_dScale);
	int height = int(m_rectModelWin.Height()*m_picture.m_dScale);
	m_picture.GetDC()->Draw3dRect(left,top,width,height,RGB(255,0,0),RGB(255,0,0));
	return true;

}
//找模板
void CDlgNeedleOne::OnBnClickedBtnSearchmodelNdleone()
{
	// TODO: Add your control notification handler code here
	if (!g_pView->m_ImgStatic.CopyNeedleImage(RGB8))
	{
		AfxMessageBox("取像失败！");
			return;
	}
	m_rectROI=m_picture.RoiRect;
	//set model parameter:
	CString strFile = g_pDoc->GetPadModelFile(11);
	milApp.SetMatchMode(g_pFrm->m_CmdRun.ProductParam.matchMode); 
	milApp.SetPadModelWindow(m_rectModelWin);
	milApp.SetPadSearchWindow(m_rectROI);
	milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));
	FindModelCenter(m_rectROI);

}



void CDlgNeedleOne::OnNMCustomdrawSliderNeedleSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->GetPos();
	CJogButton::m_dJogSpdRatio = n;
	*pResult = 0;
}

void CDlgNeedleOne::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	int nValue = 0;
	if(pScrollBar == (CScrollBar *)&m_sliderShutter)
	{
		nValue = m_sliderShutter.GetPos();
		g_pView->m_ImgStatic.m_pCameraNeedle.SetExposureTime((long)nValue);
		theApp.m_tSysParam.NeedleCameraExposure = nValue;
	}
	else if(pScrollBar == (CScrollBar *)&m_sliderGain)
	{
		nValue = m_sliderGain.GetPos();
		g_pView->m_ImgStatic.m_pCameraNeedle.SetGain((long)nValue);
		theApp.m_tSysParam.NeedleCameraGain = nValue;
	}
	theApp.BmpParam(FALSE);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

//AUTO SAVE
void CDlgNeedleOne::OnEnChangeEditNeedleAcceptance()
{
 CString str;
 GetDlgItemText(IDC_EDIT_NEEDLE_ACCEPTANCE,str);
 theApp.m_tSysParam.BmpNeedleMarkAcceptance=atof(str);
 theApp.BmpParam(FALSE);	
}

void CDlgNeedleOne::OnEnChangeEditNeedleAngle()
{
	CString str;
	GetDlgItemText(IDC_EDIT_NEEDLE_ANGLE,str);
	theApp.m_tSysParam.BmpNeedleMarkAngle=atof(str);
	theApp.BmpParam(FALSE);
}

void CDlgNeedleOne::OnBnClickedCancel()
{
	//光源控制：
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outNeedleLight,FALSE);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,TRUE);

	if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
	{
		AfxMessageBox("设置电机参数错误！");
	}
	g_pFrm->m_CmdRun.MoveToZSafety();
	g_pFrm->m_CmdRun.m_bNeedleCalibrate=false;//20180814
	OnCancel();
}

BOOL CDlgNeedleOne::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(WM_SYSKEYDOWN==pMsg->message&&VK_F4==pMsg->wParam) //Pretranslate
		return true;

	if(WM_KEYDOWN==pMsg->message&&VK_ESCAPE==pMsg->wParam) //Pretranslate
		return true;

	return CDialog::PreTranslateMessage(pMsg);
}
