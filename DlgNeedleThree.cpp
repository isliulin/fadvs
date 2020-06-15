// DlgNeedleThree.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgNeedleThree.h"
#include <math.h>


// CDlgNeedleThree dialog

IMPLEMENT_DYNAMIC(CDlgNeedleThree, CDialog)

CDlgNeedleThree::CDlgNeedleThree(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNeedleThree::IDD, pParent)
{
	m_nImageWidth = milApp.m_lNeedleBufSizeX;
	m_nImageHeight = milApp.m_lNeedleBufSizeY;
}

CDlgNeedleThree::~CDlgNeedleThree()
{
}

void CDlgNeedleThree::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC_NEEDLETHREE, m_picture);
}


BEGIN_MESSAGE_MAP(CDlgNeedleThree, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgNeedleThree::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_SEARCHMODEL_NDLTHREE, &CDlgNeedleThree::OnBnClickedBtnSearchmodelNdlthree)
	ON_BN_CLICKED(IDC_BTN_MOVETOCENTER_NDLTHREE, &CDlgNeedleThree::OnBnClickedBtnMovetocenterNdlthree)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_NEEDLE_SPEED, &CDlgNeedleThree::OnNMCustomdrawSliderNeedleSpeed)
	ON_BN_CLICKED(IDCANCEL, &CDlgNeedleThree::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_LEARNMODEL_NDLTHREE, &CDlgNeedleThree::OnBnClickedBtnLearnmodelNdlthree)
	ON_BN_CLICKED(IDC_BTN_FOCUS_THREE, &CDlgNeedleThree::OnBnClickedBtnFocusThree)
END_MESSAGE_MAP()


// CDlgNeedleThree message handlers

BOOL CDlgNeedleThree::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
    //g_pView->m_ImgStatic.ImgNeedleLive();//////////////////////////////////

	for(int i=0;i<12;i++)
	{
		m_adjustButton[i].m_iIndex = i;
		m_adjustButton[i].SubclassDlgItem(IDC_BTN_NEEDLETHREE_C_NEG_NDLTHREE+i,this);
		//m_adjustButton[i].SetBitmapEx(IDB_BITMAP_ADJUST);
	}
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->SetPos(1);
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->GetPos();
	CJogButton::m_dJogSpdRatio=n;

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
	str.Format("请手动移动X-D-Z将3#针头移至相机中心!");
	SetDlgItemText(IDC_EDIT_PROMPT,str);

	//光源控制：
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outNeedleLight,TRUE);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,FALSE);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outDetectLight,FALSE);
    ::RegisterHotKey(this->GetSafeHwnd(),2003,MOD_ALT,VK_F4);

	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		CString str;
		str.Format("手动标定针头3时Z到安全位失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return TRUE;
	}
	//Y向依据针头1，不再调整；
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2].x,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].y,true))
	{
		CString str;
		str.Format("手动标定针头3时定位到上次标定位失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return TRUE;
	}

	return TRUE;  
}

void CDlgNeedleThree::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		g_pFrm->m_CmdRun.m_bNeedleCalibrate=false;//20180814
		AfxMessageBox("运动状态不能保存当前位置！");
		return;
	}
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2].x=theApp.m_Mv400.GetPos(K_AXIS_X);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2].y=theApp.m_Mv400.GetPos(K_AXIS_Y);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2].za=theApp.m_Mv400.GetPos(K_AXIS_ZA);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2].zb=theApp.m_Mv400.GetPos(K_AXIS_ZB);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2].zc=theApp.m_Mv400.GetPos(K_AXIS_ZC);
	g_pFrm->m_CmdRun.ProductParam.offset3x=theApp.m_Mv400.GetPos(K_AXIS_C);
	g_pFrm->m_CmdRun.ProductParam.offset3y=theApp.m_Mv400.GetPos(K_AXIS_D);
	theApp.ProductParam(FALSE);
	
	if (!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		CString str;
		str.Format("手动标定针头3完成时Z到安全位失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		AfxMessageBox(str);
	}
	g_pFrm->m_CmdRun.m_bNeedleCalibrate=false;//20180814
	OnOK();
}

bool CDlgNeedleThree::FindModelCenter(CRect ROI)
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
	CString str;
	str.Format("【针头3】找到模板偏移:X:%0.3f Y: %0.3f",dX,dY);
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
void CDlgNeedleThree::OnBnClickedBtnSearchmodelNdlthree()
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

void CDlgNeedleThree::OnBnClickedBtnMovetocenterNdlthree()
{
	// TODO: Add your control notification handler code here
	double dCurXPos,dCurYpos;
	double dX,dY;
	CString str;
	OnBnClickedBtnSearchmodelNdlthree();
	dCurXPos=theApp.m_Mv400.GetPos(K_AXIS_C);
	dCurYpos=theApp.m_Mv400.GetPos(K_AXIS_D);
	theApp.m_tSysParam.BmpNeedleCalibration.ConvertPixelToMM(dCenterX-(milApp.m_lNeedleBufSizeX-1)/2,
		dCenterY-(milApp.m_lNeedleBufSizeY-1)/2,dX,dY);

	str.Format("【针头3】找模板偏移量：X:%0.3f  Y:%0.3f",dX,dY);
	g_pFrm->m_CmdRun.AddMsg(str);
	g_pFrm->m_CmdRun.PutLogIntoList(str);


	if ((fabs(dX)>2)||(fabs(dY)>2))
	{
		str.Format("偏移过大(>2)，请重新计算！");
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		return;
	}
	dCurXPos-=dX;
	dCurYpos-=dY;
	theApp.m_Mv400.MoveDot(K_AXIS_C,dCurXPos,theApp.m_tSysParam.tAxis[K_AXIS_C-1].highHomeVel,theApp.m_tSysParam.tAxis[K_AXIS_C-1].homeAcc,true,true);
	theApp.m_Mv400.MoveDot(K_AXIS_D,dCurYpos,theApp.m_tSysParam.tAxis[K_AXIS_D-1].highHomeVel,theApp.m_tSysParam.tAxis[K_AXIS_D-1].homeAcc,true,true);
}

void CDlgNeedleThree::OnNMCustomdrawSliderNeedleSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->GetPos();
	CJogButton::m_dJogSpdRatio=n;
	*pResult = 0;
}

void CDlgNeedleThree::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	//光源控制：
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outNeedleLight,FALSE);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,TRUE);
	g_pFrm->m_CmdRun.MoveToZSafety();
	g_pFrm->m_CmdRun.m_bNeedleCalibrate=false;//20180814
	OnCancel();
}

void CDlgNeedleThree::OnBnClickedBtnLearnmodelNdlthree()
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
	strFile=g_pDoc->GetModeFile(13);
	milApp.LearnModel(m_rectModelWin);
	milApp.SaveModel(strFile.GetBuffer(strFile.GetLength()));
	m_rectROI=m_picture.RoiRect;
	theApp.BmpParam(FALSE);
	milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));
}



void CDlgNeedleThree::OnBnClickedBtnFocusThree()
{
	// TODO: Add your control notification handler code here
	double dTargetPos;
	CString str;
	/*dTargetPos=g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].za;
	if (!g_pFrm->m_CmdRun.SynchronizeMoveZ(0,0,dTargetPos,false,true))
	{
		str.Format("针头3快速聚焦失败！");
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		return;
	}
	str.Format("针头3快速聚焦OK！");
	g_pFrm->m_CmdRun.AddMsg(str);
	g_pFrm->m_CmdRun.PutLogIntoList(str);*/
	dTargetPos=theApp.m_Mv400.GetPos(K_AXIS_C);
	str.Format("[][][]C轴此时坐标：%0.3f...",dTargetPos);
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);
}

BOOL CDlgNeedleThree::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(WM_SYSKEYDOWN==pMsg->message&&VK_F4==pMsg->wParam) //Pretranslate
		return true;

	if(WM_KEYDOWN==pMsg->message&&VK_ESCAPE==pMsg->wParam) //Pretranslate
		return true;

	return CDialog::PreTranslateMessage(pMsg);
}
