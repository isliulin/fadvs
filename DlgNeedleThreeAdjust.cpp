// DlgNeedleThreeAdjust.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgNeedleThreeAdjust.h"
#include <math.h>


// CDlgNeedleThreeAdjust dialog

IMPLEMENT_DYNAMIC(CDlgNeedleThreeAdjust, CDialog)

CDlgNeedleThreeAdjust::CDlgNeedleThreeAdjust(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNeedleThreeAdjust::IDD, pParent)
{
	m_nImageWidth = milApp.m_lNeedleBufSizeX;
	m_nImageHeight = milApp.m_lNeedleBufSizeY;
	bAdjust=false;
	bOver=false;
	nCount = 0;
}

CDlgNeedleThreeAdjust::~CDlgNeedleThreeAdjust()
{
}

void CDlgNeedleThreeAdjust::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC_NDLTHREEADJ, m_picture);
}


BEGIN_MESSAGE_MAP(CDlgNeedleThreeAdjust, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_C_NEG_NDLTHREEADJUST, &CDlgNeedleThreeAdjust::OnBnClickedBtnCNegNdlthreeadjust)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_NEEDLE_SPEED, &CDlgNeedleThreeAdjust::OnNMCustomdrawSliderNeedleSpeed)
	ON_BN_CLICKED(IDC_BTN_TEST, &CDlgNeedleThreeAdjust::OnBnClickedBtnTest)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDlgNeedleThreeAdjust message handlers

BOOL CDlgNeedleThreeAdjust::OnInitDialog()
{
	CDialog::OnInitDialog();
	for(int i=0;i<12;i++)
	{
		m_adjustButton[i].m_iIndex = i;
		m_adjustButton[i].SubclassDlgItem(IDC_BTN_C_NEG_NDLTHREEADJUST+i,this);
		//m_adjustButton[i].SetBitmapEx(IDB_BITMAP_ADJUST);
	}
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->SetPos(1);
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->GetPos();
	CJogButton::m_dJogSpdRatio=n;
	g_pView->m_ImgStatic.m_pCameraNeedle.SetExposureTime(theApp.m_tSysParam.NeedleCameraExposure);
	g_pView->m_ImgStatic.m_pCameraNeedle.SetGain(theApp.m_tSysParam.NeedleCameraGain);
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
	m_picture.m_mod_rectangle.height = m_rectModelWin.Height();//í???é?MARK?ò′óD??üD?
	m_picture.m_mod_scale.length = 10;//20180907
	m_picture.m_mod_scale.width = 10;
	m_picture.m_mod_scale.height = 10;
	m_picture.RoiRect =  m_rectROI;//í???é?ROI?ò′óD??üD?
	m_picture.SetDisplayMarkImage(false);
	m_picture.SetDisplayNeedleImage(true);/////////////////////////////////
	m_picture.yOffset=0;
	m_picture.ImgInit(m_nImageWidth,m_nImageHeight,24);

	CString strFile = g_pDoc->GetPadModelFile(13);
	milApp.SetMatchMode(g_pFrm->m_CmdRun.ProductParam.matchMode);  
	milApp.SetPadModelWindow(m_rectModelWin);
	milApp.SetPadSearchWindow(m_rectROI);
	milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));
	SetTimer(1,50,NULL);
	GetDlgItem(IDC_BTN_TEST)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_PROMPT)->ShowWindow(SW_HIDE);//初始状态隐藏
	return TRUE;  
}

void CDlgNeedleThreeAdjust::OnTimer(UINT_PTR nIDEvent)
{
    Invalidate(false);
	UpdateWindow();
	CDialog::OnTimer(nIDEvent);
}


bool CDlgNeedleThreeAdjust::FindModelCenter()
{
	CString strFile = g_pDoc->GetPadModelFile(13);
	CString str;
	milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));
	milApp.SetPatParam(theApp.m_tSysParam.BmpNeedleMarkAcceptance,theApp.m_tSysParam.BmpNeedleMarkAngle);
	milApp.SetSearchWindow(m_rectROI);
	if (!g_pView->m_ImgStatic.CopyNeedleImage(RGB8))
	{
		str.Format("针头3对准基准位置时取像失败！");
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		return false;
	}
	if(!milApp.FindModel(true))
	{
		return false;
	}
	dCenterX = milApp.m_stResult.dResultCenterX[0];
	dCenterY = milApp.m_stResult.dResultCenterY[0];
	int left = int((dCenterX-(m_rectModelWin.Width()-1)/2)*m_picture.m_dScale);
	int top = int((dCenterY-(m_rectModelWin.Height()-1)/2)*m_picture.m_dScale);
	int width = int(m_rectModelWin.Width()*m_picture.m_dScale);
	int height = int(m_rectModelWin.Height()*m_picture.m_dScale);
	//实际逻辑功能OK，绘图有误；
	m_picture.GetDC()->Draw3dRect(left,top,width,height,RGB(255,0,0),RGB(255,0,0));
	return true;

}

bool CDlgNeedleThreeAdjust::AutoNeedleAdjust()
{
	CString str;
	bool bAdjustOK=true;
	DWORD StartTime,EndTime;
	nCount=0;
	GetDlgItem(IDC_EDIT_PROMPT)->ShowWindow(SW_SHOW);	

	if(!theApp.m_Mv400.IsInitOK())
	{
		bOver=true;
		bAdjustOK=false;
		str.Format("板卡初始化异常，对针失败！");
		SetDlgItemText(IDC_EDIT_PROMPT,str);
		::SendMessage(g_pFrm->m_dlgNeedleAdjust.m_hWnd,WM_MSG_ADJUSTFINISH_NEEDLETHREE,0,0);
		return false;
	}

	Sleep(2500);//20180910恢复 motion done 不完全。
	if(GREY_MODE == g_pFrm->m_CmdRun.ProductParam.searchMode)
	{
		str.Format("针头3开始自动调整...");
		SetDlgItemText(IDC_EDIT_PROMPT,str);

		if(!FindModelCenter())
		{
			CString str;
			str.Format("针头3找模板失败！");
			g_pFrm->m_CmdRun.AddMsg(str);
			g_pFrm->m_CmdRun.PutLogIntoList(str);
			SetDlgItemText(IDC_EDIT_PROMPT,str);
			bOver=true;
			bAdjustOK=false;
			::SendMessage(g_pFrm->m_dlgNeedleAdjust.m_hWnd,WM_MSG_ADJUSTFINISH_NEEDLETHREE,0,0);
			return false;
		}
		else
		{
			double dX,dY;
			double dCurXPos,dCurYpos;
			theApp.m_tSysParam.BmpNeedleCalibration.ConvertPixelToMM(dCenterX - (milApp.m_lNeedleBufSizeX-1)/2,
				dCenterY - (milApp.m_lNeedleBufSizeY-1)/2,dX,dY);
			str.Format("针头3找到模板后偏移：X:%0.3f  Y:%0.3f",dX,dY);
			g_pFrm->m_CmdRun.AddMsg(str);
			g_pFrm->m_CmdRun.PutLogIntoList(str); 
            SetDlgItemText(IDC_EDIT_PROMPT,str); 

			if ((fabs(dX)>3)||(fabs(dY)>3))
			{
				str.Format("偏移值过大，请手动调整！");
				g_pFrm->m_CmdRun.AddMsg(str);
				g_pFrm->m_CmdRun.PutLogIntoList(str);
				SetDlgItemText(IDC_EDIT_PROMPT,str);
				bOver=true;
				bAdjustOK=false;
				::SendMessage(g_pFrm->m_dlgNeedleAdjust.m_hWnd,WM_MSG_ADJUSTFINISH_NEEDLETHREE,0,0);
				return false;
			}
			while(fabs(dX)>0.01||fabs(dY)>0.01)//20180723
			{
                Sleep(200);
				nCount++;
				if ((fabs(dX)>3)||(fabs(dY)>3))
				{
					str.Format("偏移值过大，请手动调整！");
					g_pFrm->m_CmdRun.AddMsg(str);
					g_pFrm->m_CmdRun.PutLogIntoList(str);
					SetDlgItemText(IDC_EDIT_PROMPT,str);
					bOver=true;
					bAdjustOK=false;
					::SendMessage(g_pFrm->m_dlgNeedleAdjust.m_hWnd,WM_MSG_ADJUSTFINISH_NEEDLETHREE,0,0);
					return false;
				}
				CFunction::DoEvents();
				//if(nCount>=g_pFrm->m_CmdRun.ProductParam.nAutoAdjustCount)
				if(nCount>=30)//20180723
				{
					str.Format("超过设定次数，请手动调整");
					SetDlgItemText(IDC_EDIT_PROMPT,str);
					bOver=true;
					bAdjustOK=false;
					break;
				}

				//for needle one;only to have modify the lable pos;
				dCurXPos=theApp.m_Mv400.GetPos(K_AXIS_C);
				dCurYpos=theApp.m_Mv400.GetPos(K_AXIS_D);
				dCurXPos-= dX;
				dCurYpos-= dY;
				theApp.m_Mv400.MoveDot(K_AXIS_C,dCurXPos,theApp.m_tSysParam.tAxis[K_AXIS_C-1].highHomeVel,theApp.m_tSysParam.tAxis[K_AXIS_C-1].homeAcc,true,true);
				theApp.m_Mv400.MoveDot(K_AXIS_D,dCurYpos,theApp.m_tSysParam.tAxis[K_AXIS_D-1].highHomeVel,theApp.m_tSysParam.tAxis[K_AXIS_D-1].homeAcc,true,true);
				CFunction::DoEvents();
				CFunction::DelayEx(4);
				CFunction::DoEvents();
				str.Format("针头3图像识别后:CD完成纠偏！");
				g_pFrm->m_CmdRun.AddMsg(str);
				g_pFrm->m_CmdRun.PutLogIntoList(str); 
				SetDlgItemText(IDC_EDIT_PROMPT,str); 

				if(!FindModelCenter())
				{
					str.Format("纠偏后找模板失败，请手动调整！");
					g_pFrm->m_CmdRun.AddMsg(str);
					g_pFrm->m_CmdRun.PutLogIntoList(str);
					SetDlgItemText(IDC_EDIT_PROMPT,str);
                    bOver=true;
					bAdjustOK=false;
					::SendMessage(g_pFrm->m_dlgNeedleAdjust.m_hWnd,WM_MSG_ADJUSTFINISH_NEEDLETHREE,0,0);
					return false;
				}
				theApp.m_tSysParam.BmpNeedleCalibration.ConvertPixelToMM(dCenterX - (milApp.m_lNeedleBufSizeX-1)/2,
					dCenterY - (milApp.m_lNeedleBufSizeY-1)/2,dX,dY);
				str.Format("针头3执行调整后实际偏移值：X:%0.3f  Y:%0.3f",dX,dY);
				g_pFrm->m_CmdRun.AddMsg(str);
				g_pFrm->m_CmdRun.PutLogIntoList(str); 
			}

		}
	}
	bOver=true;
	str.Format("针头3自动调整完毕...");
	SetDlgItemText(IDC_EDIT_PROMPT,str);
	if (bAdjustOK)
	{
		//StartTime = EndTime = GetTickCount();
		//while (true)
		//{
		//	CFunction::DoEvents();
		//	EndTime= GetTickCount();
		//	if ((EndTime-StartTime)/1000>7)//5S
		//	{
		//		break;
		//	}
		//	Sleep(250);
		//}
		g_pFrm->m_CmdRun.WaitAutoNeedleSys(true,true,true,true,true,true,true,true,true);
		::SendMessage(g_pFrm->m_dlgNeedleAdjust.m_hWnd,WM_MSG_ADJUSTFINISH_NEEDLETHREE,0,1);
	}
	else
	{
		::SendMessage(g_pFrm->m_dlgNeedleAdjust.m_hWnd,WM_MSG_ADJUSTFINISH_NEEDLETHREE,0,0);
	}
}


void CDlgNeedleThreeAdjust::OnBnClickedBtnCNegNdlthreeadjust()
{
	// TODO: Add your control notification handler code here
}

void CDlgNeedleThreeAdjust::OnNMCustomdrawSliderNeedleSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->GetPos();
	CJogButton::m_dJogSpdRatio=n;
	*pResult = 0;
}

void CDlgNeedleThreeAdjust::OnBnClickedBtnTest()
{
	// TODO: Add your control notification handler code here
	FindModelCenter();
}

void CDlgNeedleThreeAdjust::EnableButton(bool bEn)
{
	GetDlgItem(IDC_BTN_C_NEG_NDLTHREEADJUST)->EnableWindow(bEn);
	GetDlgItem(IDC_BTN_C_POS_NDLTHREEADJUST)->EnableWindow(bEn);
	GetDlgItem(IDC_BTN_D_NEG_NDLTHREEADJUST)->EnableWindow(bEn);
	GetDlgItem(IDC_BTN_D_POS_NDLTHREEADJUST)->EnableWindow(bEn);
	GetDlgItem(IDC_BTN_Z_NEG_NDLTHREEADJUST)->EnableWindow(bEn);
	GetDlgItem(IDC_BTN_Z_POS_NDLTHREEADJUST)->EnableWindow(bEn);
}
HBRUSH CDlgNeedleThreeAdjust::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if (pWnd->GetDlgCtrlID()==IDC_EDIT_PROMPT)
	{
		if (g_pFrm->m_CmdRun.m_bAutoNeedleErr)
		{
			pDC->SetTextColor(RGB(255,0,0));
		}
		else
		{
			pDC->SetTextColor(RGB(0,0,0));
		}
	}	
	return hbr;
}
