// DlgNeedleTwo.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgNeedleTwo.h"
#include "DlgNeedleThree.h"
#include <math.h>


// CDlgNeedleTwo dialog

IMPLEMENT_DYNAMIC(CDlgNeedleTwo, CDialog)

CDlgNeedleTwo::CDlgNeedleTwo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNeedleTwo::IDD, pParent)
{
	m_nImageWidth = milApp.m_lNeedleBufSizeX;
	m_nImageHeight = milApp.m_lNeedleBufSizeY;

}

CDlgNeedleTwo::~CDlgNeedleTwo()
{
}

void CDlgNeedleTwo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC_NEEDLETWO, m_picture);
}


BEGIN_MESSAGE_MAP(CDlgNeedleTwo, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgNeedleTwo::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_LEARNMODEL_NDLTWO, &CDlgNeedleTwo::OnBnClickedBtnLearnmodelNdltwo)
	ON_BN_CLICKED(IDC_BTN_SEARCHMODEL_NDLTWO, &CDlgNeedleTwo::OnBnClickedBtnSearchmodelNdltwo)
	ON_BN_CLICKED(IDC_BTN_MOVETOCENTER_NDLTWO, &CDlgNeedleTwo::OnBnClickedBtnMovetocenterNdltwo)	
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_NEEDLE_SPEED, &CDlgNeedleTwo::OnNMCustomdrawSliderNeedleSpeed)
	ON_BN_CLICKED(IDCANCEL, &CDlgNeedleTwo::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_COMPUTESCALE_NDLTWO, &CDlgNeedleTwo::OnBnClickedBtnComputescaleNdltwo)
	ON_BN_CLICKED(IDC_BTN_FOCUS_TWO, &CDlgNeedleTwo::OnBnClickedBtnFocusTwo)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgNeedleTwo::OnBnClickedButton2)
END_MESSAGE_MAP()


// CDlgNeedleTwo message handlers

BOOL CDlgNeedleTwo::OnInitDialog()
{
	CDialog::OnInitDialog();
	for(int i=0;i<12;i++)
	{
		m_adjustButton[i].m_iIndex = i;
		m_adjustButton[i].SubclassDlgItem(IDC_BTN_NEEDLETWO_A_NEG_NDLTWO+i,this);
		//m_adjustButton[i].SetBitmapEx(IDB_BITMAP_ADJUST);
	}
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->SetPos(1);
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->GetPos();
	CJogButton::m_dJogSpdRatio=n;

	//UpdateData();

	g_pView->m_ImgStatic.ImgNeedleLive();

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
	m_picture.m_mod_scale.length = 20;
	m_picture.m_mod_scale.width = 10;
	m_picture.m_mod_scale.height = 10;
	m_picture.RoiRect =  m_rectROI;//图片上ROI框大小更新
	m_picture.SetDisplayMarkImage(false);
	m_picture.SetDisplayNeedleImage(true);/////////////////////////////////
	m_picture.yOffset=0;
	m_picture.ImgInit(m_nImageWidth,m_nImageHeight,24);

	CString strFile = g_pDoc->GetPadModelFile(11);
	milApp.SetMatchMode(g_pFrm->m_CmdRun.ProductParam.matchMode);  
	milApp.SetPadModelWindow(m_rectModelWin);
	milApp.SetPadSearchWindow(m_rectROI);
	milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));

	CString str;
	str.Format("请手动移动X-B-Z将2#针头移至相机中心！");
	SetDlgItemText(IDC_EDIT_PROMPT,str);
	//光源控制：
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outNeedleLight,TRUE);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,FALSE);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outDetectLight,FALSE);
	::RegisterHotKey(this->GetSafeHwnd(),2002,MOD_ALT,VK_F4);
   
	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		str.Format("手动标定针头2时Z到安全位失败！");
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		return TRUE;
	}
	//定位到上次标定位置//Y向依据针头1 不得再调整；
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].x,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].y,true))
	{
		str.Format("手动标定针头2时定位到上次标定位失败！");
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		return TRUE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgNeedleTwo::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		g_pFrm->m_CmdRun.m_bNeedleCalibrate=false;
		AfxMessageBox("运动状态不能保存当前位置！");
		return;
	}
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].x=theApp.m_Mv400.GetPos(K_AXIS_X);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].y=theApp.m_Mv400.GetPos(K_AXIS_Y);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].za=theApp.m_Mv400.GetPos(K_AXIS_ZA);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].zb=theApp.m_Mv400.GetPos(K_AXIS_ZB);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1].zc=theApp.m_Mv400.GetPos(K_AXIS_ZC);
	g_pFrm->m_CmdRun.ProductParam.offset2x=theApp.m_Mv400.GetPos(K_AXIS_A);
	g_pFrm->m_CmdRun.ProductParam.offset2y=theApp.m_Mv400.GetPos(K_AXIS_B);
	theApp.ProductParam(FALSE);
	
	if (!g_pFrm->m_CmdRun.MoveToZSafety())
	{
	  CString str;
	  str.Format("手动标定针头2完成时Z到安全位失败！");
	  g_pFrm->m_CmdRun.AddMsg(str);
	  g_pFrm->m_CmdRun.PutLogIntoList(str);
	  AfxMessageBox(str);
	}
	OnOK();
	CDlgNeedleThree dlg;
	dlg.DoModal();
	
}

void CDlgNeedleTwo::OnBnClickedBtnLearnmodelNdltwo()
{
	// TODO: Add your control notification handler code here
	if (!g_pView->m_ImgStatic.CopyNeedleImage(RGB8))
	{
		AfxMessageBox("参考点相机取像失败！");
		return;
	}
	CString strFile;
	m_rectModelWin.left=theApp.m_tSysParam.BmpNeedleLearnWin.TopLeft().x = long(m_picture.m_mod_rectangle.dx - m_picture.m_mod_rectangle.width/2);
	m_rectModelWin.top=theApp.m_tSysParam.BmpNeedleLearnWin.TopLeft().y = long(m_picture.m_mod_rectangle.dy - m_picture.m_mod_rectangle.height/2);
	m_rectModelWin.right=theApp.m_tSysParam.BmpNeedleLearnWin.BottomRight().x = theApp.m_tSysParam.BmpNeedleLearnWin.TopLeft().x + m_picture.m_mod_rectangle.width;
	m_rectModelWin.bottom=theApp.m_tSysParam.BmpNeedleLearnWin.BottomRight().y = theApp.m_tSysParam.BmpNeedleLearnWin.TopLeft().y + m_picture.m_mod_rectangle.height;
	theApp.BmpParam(FALSE);
	strFile=g_pDoc->GetModeFile(12);
	milApp.LearnModel(m_rectModelWin);
	milApp.SaveModel(strFile.GetBuffer(strFile.GetLength()));
	m_rectROI=m_picture.RoiRect;
	theApp.BmpParam(FALSE);
	milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));
}

bool CDlgNeedleTwo::FindModelCenter(CRect ROI)
{
	double dX,dY;
	milApp.SetPatParam(theApp.m_tSysParam.BmpNeedleMarkAcceptance,theApp.m_tSysParam.BmpNeedleMarkAngle);
	milApp.SetSearchWindow(ROI);
	if(!milApp.FindModel(true))
	{
		return false;
	}
	dCenterX = milApp.m_stResult.dResultCenterX[0];
	dCenterY = milApp.m_stResult.dResultCenterY[0];

	theApp.m_tSysParam.BmpNeedleCalibration.ConvertPixelToMM(dCenterX - (milApp.m_lNeedleBufSizeX-1)/2,
		dCenterY - (milApp.m_lNeedleBufSizeY-1)/2,dX,dY);
	//theApp.m_tSysParam.BmpStepNeedleCalibration.ConvertPixelToMM(dCenterX - (milApp.m_lNeedleBufSizeX-1)/2,
		//dCenterY - (milApp.m_lNeedleBufSizeY-1)/2,dX,dY);
	CString str;
	str.Format("针头2找到模板偏移:X:%0.3f Y: %0.3f",dX,dY);
	g_pFrm->m_CmdRun.AddMsg(str);
	g_pFrm->m_CmdRun.PutLogIntoList(str);


	int left = int((dCenterX-(m_rectModelWin.Width()-1)/2)*m_picture.m_dScale);
	int top = int((dCenterY-(m_rectModelWin.Height()-1)/2)*m_picture.m_dScale);
	int width = int(m_rectModelWin.Width()*m_picture.m_dScale);
	int height = int(m_rectModelWin.Height()*m_picture.m_dScale);
	//实际逻辑功能OK，绘图有误；
	m_picture.GetDC()->Draw3dRect(left,top,width,height,RGB(255,0,0),RGB(255,0,0));
	return true;

}
void CDlgNeedleTwo::OnBnClickedBtnSearchmodelNdltwo()
{
	// TODO: Add your control notification handler code here
	if (!g_pView->m_ImgStatic.CopyNeedleImage(RGB8))
	{
		AfxMessageBox("取像失败！");
		return;
	}
	m_rectROI=m_picture.RoiRect;
	FindModelCenter(m_rectROI);
}

void CDlgNeedleTwo::OnBnClickedBtnMovetocenterNdltwo()
{
	// TODO: Add your control notification handler code here
	double dCurXPos,dCurYpos;
	double dX,dY;
	CString str;

	OnBnClickedBtnSearchmodelNdltwo();
	dCurXPos=theApp.m_Mv400.GetPos(K_AXIS_A);//161223 
	//dCurXPos=theApp.m_Mv400.GetPos(K_AXIS_X);
	dCurYpos=theApp.m_Mv400.GetPos(K_AXIS_B);
	theApp.m_tSysParam.BmpNeedleCalibration.ConvertPixelToMM(dCenterX-(milApp.m_lNeedleBufSizeX-1)/2,
		dCenterY-(milApp.m_lNeedleBufSizeY-1)/2,dX,dY);
	
	str.Format("【针头2】到中心时偏移量（mm）:X:%0.3f Y:%0.3f",dX,dY);
	g_pFrm->m_CmdRun.AddMsg(str);
	g_pFrm->m_CmdRun.PutLogIntoList(str);

	if ((fabs(dX)>2)||(fabs(dY)>2))
	{
		AfxMessageBox("偏移过大，请重新计算！");
		return;
	}
	dCurXPos-=dX;//coordine:X real and pixsel
	dCurYpos-=dY;//coordine:Y real and pixsel  the sign of debug0822
	//theApp.m_Mv400.Move(K_AXIS_A,dCurXPos,true,true); //20161223  
	//theApp.m_Mv400.Move(K_AXIS_B,dCurYpos,true,true);
    //修正纠偏函数 
	theApp.m_Mv400.MoveDot(K_AXIS_A,dCurXPos,theApp.m_tSysParam.tAxis[K_AXIS_A-1].highHomeVel,theApp.m_tSysParam.tAxis[K_AXIS_A-1].homeAcc,true,true);
	theApp.m_Mv400.MoveDot(K_AXIS_B,dCurYpos,theApp.m_tSysParam.tAxis[K_AXIS_B-1].highHomeVel,theApp.m_tSysParam.tAxis[K_AXIS_B-1].homeAcc,true,true);

	str.Format("【针头2】到中心时马达动作完毕！");
	g_pFrm->m_CmdRun.AddMsg(str);
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	Sleep(2000);
	dCurXPos=theApp.m_Mv400.GetPos(K_AXIS_A);
	dCurYpos=theApp.m_Mv400.GetPos(K_AXIS_B);
	str.Format("【针头2】纠偏之后实际坐标：A:%0.3f  B:%0.3f",dCurXPos,dCurYpos);
	g_pFrm->m_CmdRun.AddMsg(str);
	g_pFrm->m_CmdRun.PutLogIntoList(str);
}





void CDlgNeedleTwo::OnNMCustomdrawSliderNeedleSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->GetPos();
	CJogButton::m_dJogSpdRatio=n;
	*pResult = 0;
}

void CDlgNeedleTwo::OnBnClickedCancel()
{
	//光源控制：
	g_pFrm->m_CmdRun.m_bNeedleCalibrate=false;//20180814
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outNeedleLight,FALSE);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,TRUE);
	g_pFrm->m_CmdRun.MoveToZSafety();
	OnCancel();
}

//计算步进对应的系数 
void CDlgNeedleTwo::OnBnClickedBtnComputescaleNdltwo()
{
	CString str;
	tgPos tgTempPixel;
	double dTempX,dTempY;
	double dTempA,dTempB;
	CRect ROI(0,0,milApp.m_lNeedleBufSizeX-1,milApp.m_lNeedleBufSizeY-1);
	str.Format("【针头2】计算系数前第一次取像...");
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);

	CFunction::DelayEx(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay);
	if(!g_pView->m_ImgStatic.CopyNeedleImage(RGB8))
	{
		AfxMessageBox("抓取图像失败！");
		str.Format("【针头2】计算系数时：首次拷贝图像失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return;
	}

	str.Format("【针头2】计算系数时：首次拷贝图像成功！");
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);

	//str.Format("针头2计算系数时找首张图片中心...");
	//g_pFrm->m_CmdRun.PutLogIntoList(str);
	//g_pFrm->m_CmdRun.AddMsg(str);

	if (FindModelCenter(ROI))
	{
		tgTempPixel.x=dCenterX;
		tgTempPixel.y=dCenterY;
		str.Format("【针头2】计算系数时：图像中心点坐标：X:%d;Y:%d",dCenterX,dCenterY);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
	}
	else
	{
		AfxMessageBox("查找模板失败！");
		str.Format("【针头2】计算系数时：找模板失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return;
	}

	dTempX=theApp.m_Mv400.GetPos(K_AXIS_A);
	dTempY=theApp.m_Mv400.GetPos(K_AXIS_B);
	str.Format("移动到第二点前，马达AB位置：%0.3f,%0.3f",dTempX,dTempY);
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);

	//g_pFrm->m_CmdRun.SynchronizeMoveXY((dTempX+0.5),(dTempY+0.5),true);
	dTempX+=0.2;
	dTempY+=0.2;
	theApp.m_Mv400.MoveDot(K_AXIS_A,dTempX,theApp.m_tSysParam.tAxis[K_AXIS_A-1].highHomeVel,theApp.m_tSysParam.tAxis[K_AXIS_A-1].homeAcc,true,true);
	theApp.m_Mv400.MoveDot(K_AXIS_B,dTempY,theApp.m_tSysParam.tAxis[K_AXIS_B-1].highHomeVel,theApp.m_tSysParam.tAxis[K_AXIS_B-1].homeAcc,true,true);

	//theApp.m_Mv400.Move(K_AXIS_A,(dTempX+0.2),true,true);
    //theApp.m_Mv400.Move(K_AXIS_B,(dTempY+0.2),true,true);
	Sleep(5000);
	CFunction::DelayEx(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay);
	str.Format("【针头2计算系数时】：开始拷贝图像2");
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);

	if(!g_pView->m_ImgStatic.CopyNeedleImage(RGB8))//only for the Main Camera to use.
	{
		AfxMessageBox("抓取图像失败！");
		str.Format("【针头2计算系数时】：图像2拷贝失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return;
	}
	str.Format("【针头2】计算系数时：图像2拷贝成功！");
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);

	str.Format("【针头2】计算系数时：开始找图像2中心点");
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);

	if (FindModelCenter(ROI))
	{
		str.Format("图像2找中心成功：X：%d...Y:%d",dCenterX,dCenterY);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		dTempA=(dCenterX-tgTempPixel.x)/0.2;
		dTempB=(dCenterY-tgTempPixel.y)/0.2;
		if ((0==dTempA)||(0==dTempB))
		{
			str.Format("【针头2】系数计算异常！");
			g_pFrm->m_CmdRun.PutLogIntoList(str);
			g_pFrm->m_CmdRun.AddMsg(str);
		}
	}
	else
	{
		AfxMessageBox("计算系数时查找模板失败！");
		str.Format("图像2找中心失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return;
	}
	str.Format("【针头2】计算系数成功：dTempA:%d;dTempB：%d",dTempA,dTempB);
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);
	theApp.m_tSysParam.BmpStepNeedleCalibration.dCalibrationA =dTempA;
	theApp.m_tSysParam.BmpStepNeedleCalibration.dCalibrationB = dTempB;
	theApp.BmpParam(FALSE);
	UpdateData();
}

void CDlgNeedleTwo::OnBnClickedBtnFocusTwo()
{
	// TODO: Add your control notification handler code here
   double dTargetPos;
   CString str;
   dTargetPos=g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].za;
   if (!g_pFrm->m_CmdRun.SynchronizeMoveZ(0,dTargetPos,0,false,true))
   {
	   str.Format("针头2快速聚焦失败！");
	   g_pFrm->m_CmdRun.AddMsg(str);
	   g_pFrm->m_CmdRun.PutLogIntoList(str);
	   return;
   }
   str.Format("针头2快速聚焦OK！");
   g_pFrm->m_CmdRun.AddMsg(str);
   g_pFrm->m_CmdRun.PutLogIntoList(str);


}

void CDlgNeedleTwo::UpdateData()
{
	CString str;
	str.Format("%0.3f",theApp.m_tSysParam.BmpStepNeedleCalibration.dCalibrationA);
	SetDlgItemText(IDC_EDIT_SCALEA,str);
	str.Format("%0.3f",theApp.m_tSysParam.BmpStepNeedleCalibration.dCalibrationB);
	SetDlgItemText(IDC_EDIT_SCALEB,str);
}
void CDlgNeedleTwo::OnBnClickedButton2()
{
	CString str;
	double data;
	GetDlgItemText(IDC_EDIT_SCALEA,str);
	data=atof(str);
	theApp.m_tSysParam.BmpStepNeedleCalibration.dCalibrationA=data;
	GetDlgItemText(IDC_EDIT_SCALEB,str);
	data=atof(str);
	theApp.m_tSysParam.BmpStepNeedleCalibration.dCalibrationB=data;
	theApp.BmpParam(FALSE);
}

BOOL CDlgNeedleTwo::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(WM_SYSKEYDOWN==pMsg->message&&VK_F4==pMsg->wParam) //Pretranslate
		return true;

	if(WM_KEYDOWN==pMsg->message&&VK_ESCAPE==pMsg->wParam) //Pretranslate
		return true;

	return CDialog::PreTranslateMessage(pMsg);
}
