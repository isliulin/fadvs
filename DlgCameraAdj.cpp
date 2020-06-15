// DlgCameraAdj.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgCameraAdj.h"
#include <math.h>


// CDlgCameraAdj dialog

IMPLEMENT_DYNAMIC(CDlgCameraAdj, CDialog)

CDlgCameraAdj::CDlgCameraAdj(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCameraAdj::IDD, pParent)
{
	bAdjust=false;
	bOver=false;

}

CDlgCameraAdj::~CDlgCameraAdj()
{
}

void CDlgCameraAdj::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC_CAMERA_ADJUST, m_picture);
}


BEGIN_MESSAGE_MAP(CDlgCameraAdj, CDialog)
	ON_WM_TIMER()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_NEEDLE_SPEED, &CDlgCameraAdj::OnNMCustomdrawSliderNeedleSpeed)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL CDlgCameraAdj::OnInitDialog()
{
	CDialog::OnInitDialog();
	//picture init;
	for(int i=0;i<12;i++)
	{
		m_adjustButton[i].m_iIndex = i;
		m_adjustButton[i].SubclassDlgItem(IDC_BTN_X_NEG_INCAMADJUST+i,this);
		//m_adjustButton[i].SetBitmapEx(IDB_BITMAP_ADJUST);
	}
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->SetPos(1);
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->GetPos();
	CJogButton::m_dJogSpdRatio=n;
	m_picture.m_modtype = MOD_RECTANGLE;
	m_picture.m_iImgWidth = milApp.m_lBufSizeX;
	m_picture.m_iImgHeight = milApp.m_lBufSizeY;
	m_picture.m_pImgBuffer = new BYTE[m_picture.m_iImgWidth*m_picture.m_iImgHeight*3];
	m_picture.m_mod_rectangle.dx = theApp.m_tSysParam.BmpMarkRefCam.TopLeft().x + theApp.m_tSysParam.BmpMarkRefCam.Width()/2;
	m_picture.m_mod_rectangle.dy = theApp.m_tSysParam.BmpMarkRefCam.TopLeft().y + theApp.m_tSysParam.BmpMarkRefCam.Height()/2;
	m_picture.m_mod_rectangle.width = theApp.m_tSysParam.BmpMarkRefCam.Width();
	m_picture.m_mod_rectangle.height = theApp.m_tSysParam.BmpMarkRefCam.Height();
	m_picture.m_mod_scale.length = 10;//20180907
	m_picture.m_mod_scale.width = 10;
	m_picture.m_mod_scale.height = 10;
	m_picture.RoiRect =  theApp.m_tSysParam.BmpMarkROI;
	m_picture.SetDisplayMarkImage(true);
	m_picture.ImgInit(milApp.m_lBufSizeX,milApp.m_lBufSizeY,24);
	//model roi init;
	LearnWin=theApp.m_tSysParam.BmpMarkRefCam;
	ROI=theApp.m_tSysParam.BmpMarkRefCamROI;
	//load model file
	milApp.SetModelWindow(LearnWin);
	milApp.SetSearchWindow(ROI);
	CString strFile=g_pDoc->GetModeFile(10);
	milApp.RestoreModel(strFile.GetBuffer(strFile.GetLength()));
	SetTimer(1,100,NULL);
	return TRUE;  
}

bool CDlgCameraAdj::FindModelCenter()
{
	CString strFile=g_pDoc->GetModeFile(10);//auto load one model once;
	CString str;
	milApp.RestoreModel(strFile.GetBuffer(strFile.GetLength()));
	milApp.SetPatParam(theApp.m_tSysParam.BmpNeedleMarkAcceptance,theApp.m_tSysParam.BmpNeedleMarkAngle);
	milApp.SetSearchWindow(ROI);
	//相机匹配之前复制图像到MIL缓冲区
	if(!g_pView->m_ImgStatic.CopyImage(RGB8))
	{
		str.Format("相机对准基准相机时取像失败！");
        g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
	}

	if(!milApp.FindModel(true))
	{
		return false;
	}
	dCenterX = milApp.m_stResult.dResultCenterX[0];
	dCenterY = milApp.m_stResult.dResultCenterY[0];
	int left = int((dCenterX-(LearnWin.Width()-1)/2)*m_picture.m_dScale);
	int top = int((dCenterY-(LearnWin.Height()-1)/2)*m_picture.m_dScale);
	int width = int(LearnWin.Width()*m_picture.m_dScale);
	int height = int(LearnWin.Height()*m_picture.m_dScale);
	//实际逻辑功能OK，绘图有误；
	m_picture.GetDC()->Draw3dRect(left,top,width,height,RGB(255,0,0),RGB(255,0,0));
	return true;

}

void CDlgCameraAdj::AutoCamAdjust()
{
	CString str;
	int nCount = 0;
	bool bAdjustOK=true;
	DWORD StartTime,EndTime; //20180907

	if(!theApp.m_Mv400.IsInitOK())
	{
		bOver=true;
		bAdjustOK=false;
		str.Format("板卡初始化异常，对针失败！");
		SetDlgItemText(IDC_EDIT_PROMPT,str);
		::SendMessage(g_pFrm->m_dlgNeedleAdjust.m_hWnd,WM_MSG_ADJUSTFINISH_CAM,0,0);
		return;
	}
	Sleep(2500);//20180910恢复 motion done 不完全。
	if(GREY_MODE == g_pFrm->m_CmdRun.ProductParam.searchMode)
	{
		str.Format("相机开始自动调整...");
		SetDlgItemText(IDC_EDIT_PROMPT,str);

		CString strFile=g_pDoc->GetModeFile(10);
		milApp.RestoreModel(strFile.GetBuffer(strFile.GetLength()));
		if(!FindModelCenter())
		{
			CString str;
			str.Format("相机定位找模板失败,请手动调整!");
			g_pFrm->m_CmdRun.AddMsg(str);
			g_pFrm->m_CmdRun.PutLogIntoList(str);
			SetDlgItemText(IDC_EDIT_PROMPT,str);
			bOver=true;
			bAdjustOK=false;
			::SendMessage(g_pFrm->m_dlgNeedleAdjust.m_hWnd,WM_MSG_ADJUSTFINISH_CAM,0,0);
			return;
		}
		else
		{
			double dX,dY;
			theApp.m_tSysParam.BmpMarkRefCamCalibration.ConvertPixelToMM(dCenterX - (milApp.m_lBufSizeX-1)/2,
				dCenterY - (milApp.m_lBufSizeY-1)/2,dX,dY);
			str.Format("相机找模板后偏移：X:%0.3f  Y:%0.3f",dX,dY);
			g_pFrm->m_CmdRun.AddMsg(str);
			g_pFrm->m_CmdRun.PutLogIntoList(str); 

			if ((fabs(dX)>2)||(fabs(dY)>2))
			{
				str.Format("相机偏移过大，请手动调整！");
				g_pFrm->m_CmdRun.AddMsg(str);
				g_pFrm->m_CmdRun.PutLogIntoList(str);
				SetDlgItemText(IDC_EDIT_PROMPT,str);
				bOver=true;
				bAdjustOK=false;
				::SendMessage(g_pFrm->m_dlgNeedleAdjust.m_hWnd,WM_MSG_ADJUSTFINISH_CAM,0,0);
				return;
			}
			while(fabs(dX)>0.1||fabs(dY)>0.1)
			{
				Sleep(300);
				nCount++;
				if ((fabs(dX)>2)||(fabs(dY)>2))
				{
					str.Format("检测到偏移过大，请手动调整！");
					g_pFrm->m_CmdRun.AddMsg(str);
					g_pFrm->m_CmdRun.PutLogIntoList(str);
					SetDlgItemText(IDC_EDIT_PROMPT,str);
					bOver=true;
					bAdjustOK=false;
					::SendMessage(g_pFrm->m_dlgNeedleAdjust.m_hWnd,WM_MSG_ADJUSTFINISH_CAM,0,0);
					return;
				}
				CFunction::DoEvents();
				if(nCount>=30)
				{
					str.Format("超过设定次数，请手动调整!");
					SetDlgItemText(IDC_EDIT_PROMPT,str);
					bOver=true;
					bAdjustOK=false;
					break;
				}

				//for camera ;only to have modify the lable pos;
				g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].x -= dX;
				g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].y -= dY;
				if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].x,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].y,true))
				{
					g_pFrm->m_CmdRun.AddMsg("调整过程中相机XY移动失败！");
					g_pFrm->m_CmdRun.PutLogIntoList(str);
					bOver=true;
					bAdjustOK=false;
					::SendMessage(g_pFrm->m_dlgNeedleAdjust.m_hWnd,WM_MSG_ADJUSTFINISH_CAM,0,0);
					return ;
				}
				CFunction::DoEvents();
				CFunction::DelayEx(4);
				CFunction::DoEvents();
				str.Format("相机图像识别后:XY完成纠偏！");
				g_pFrm->m_CmdRun.AddMsg(str);
				g_pFrm->m_CmdRun.PutLogIntoList(str); 
				SetDlgItemText(IDC_EDIT_PROMPT,str); 

				if(!FindModelCenter())
				{
					str.Format("相机纠偏调整后找模板失败,请手动调整！");
					g_pFrm->m_CmdRun.AddMsg(str);
					g_pFrm->m_CmdRun.PutLogIntoList(str);
					SetDlgItemText(IDC_EDIT_PROMPT,str);
					bOver=true;
					bAdjustOK=false;
					::SendMessage(g_pFrm->m_dlgNeedleAdjust.m_hWnd,WM_MSG_ADJUSTFINISH_CAM,0,0);
					//break;
					return;

				}
				theApp.m_tSysParam.BmpMarkRefCamCalibration.ConvertPixelToMM(dCenterX - (milApp.m_lBufSizeX-1)/2,
					dCenterY - (milApp.m_lBufSizeY-1)/2,dX,dY);
				str.Format("相机执行调整后实际偏移值：X:%0.3f  Y:%0.3f",dX,dY);
				g_pFrm->m_CmdRun.AddMsg(str);
				g_pFrm->m_CmdRun.PutLogIntoList(str); 
			}

		}
	}
	bOver=true;
	str.Format("相机自动调整完毕...");
	SetDlgItemText(IDC_EDIT_PROMPT,str);
	if (bAdjustOK)
	{
		//StartTime = EndTime = GetTickCount();
		//while (true)
		//{
		//	CFunction::DoEvents();
		//	EndTime= GetTickCount();
		//	if ((EndTime-StartTime)/1000>2)//2S
		//	{
		//		break;
		//	}
		//	Sleep(250);
		//}
		g_pFrm->m_CmdRun.WaitAutoNeedleSys(true,true,true,true,true,true,true,true,true);
		::SendMessage(g_pFrm->m_dlgNeedleAdjust.m_hWnd,WM_MSG_ADJUSTFINISH_CAM,0,1);
	}
	else
	{
		::SendMessage(g_pFrm->m_dlgNeedleAdjust.m_hWnd,WM_MSG_ADJUSTFINISH_CAM,0,0);
	}
}

void CDlgCameraAdj::OnTimer(UINT_PTR nIDEvent)
{
    Invalidate();
	UpdateWindow();
	CDialog::OnTimer(nIDEvent);
}
void CDlgCameraAdj::OnNMCustomdrawSliderNeedleSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->GetPos();
	CJogButton::m_dJogSpdRatio=n;
	*pResult = 0;
}


HBRUSH CDlgCameraAdj::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
