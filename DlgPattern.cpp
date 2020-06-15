// DlgPattern.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgPattern.h"


// CDlgPattern dialog

IMPLEMENT_DYNAMIC(CDlgPattern, CDialog)

CDlgPattern::CDlgPattern(CWnd* pParent /*=NULL*/)
: CDialog(CDlgPattern::IDD, pParent)
{
	dCenterX = (milApp.m_lBufSizeX-1)/2;
	dCenterY = (milApp.m_lBufSizeY-1)/2;
	dAngle = 0.0;
	dScore = 0.0;
	nFirstRow = 0;
	nFirstColumn = 0;
	nSecondRow = 0;
	nSecondColumn = 0;
	nThirdRow = 0;
	nThirdColumn = 0;

	nWidth = milApp.m_lBufSizeX;
	nHeight = milApp.m_lBufSizeY;
}

CDlgPattern::~CDlgPattern()
{
}

void CDlgPattern::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX,IDC_STATIC_PATTERN,m_picture);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgPattern, CDialog)
	ON_BN_CLICKED(IDC_BTN_PATTERN_SCALE, &CDlgPattern::OnBnClickedBtnPatternScale)
	ON_BN_CLICKED(IDC_BTN_PATTERN_NEWPIC, &CDlgPattern::OnBnClickedBtnPatternNewpic)
	ON_BN_CLICKED(IDC_BTN_PATTERN_FIRST_IMAGE_POS, &CDlgPattern::OnBnClickedBtnPatternFirstImagePos)
	ON_BN_CLICKED(IDC_BTN_PATTERN_NEXT_IMAGE_POS, &CDlgPattern::OnBnClickedBtnPatternNextImagePos)
	ON_BN_CLICKED(IDC_BTN_PATTERN_SEARCH, &CDlgPattern::OnBnClickedBtnPatternSearch)
	ON_BN_CLICKED(IDC_BTN_PATTERN_MOVE_CENTER, &CDlgPattern::OnBnClickedBtnPatternMoveCenter)
	ON_BN_CLICKED(IDC_BTN_PATTERN_SAVE_MODEL, &CDlgPattern::OnBnClickedBtnPatternSaveModel)
	ON_BN_CLICKED(IDC_BTN_PATTERN_SAVE_LEARN_WIN, &CDlgPattern::OnBnClickedBtnPatternSaveLearnWin)
	ON_BN_CLICKED(IDC_BTN_PATTERN_SHOW_LEARN_WIN, &CDlgPattern::OnBnClickedBtnPatternShowLearnWin)
	ON_BN_CLICKED(IDC_BTN_PATTERN_SAVE_ROI, &CDlgPattern::OnBnClickedBtnPatternSaveRoi)
	ON_BN_CLICKED(IDC_BTN_PATTERN_SHOW_ROI, &CDlgPattern::OnBnClickedBtnPatternShowRoi)
	ON_BN_CLICKED(IDC_BTN_PATTERN_CLEAR, &CDlgPattern::OnBnClickedBtnPatternClear)
	ON_BN_CLICKED(IDOK, &CDlgPattern::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgPattern::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgPattern message handlers

BOOL CDlgPattern::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString str;
	if(milApp.m_MilImage!=NULL)
	{
		m_picture.MoveWindow(0,0,nWidth,nHeight,true);
		milApp.DisSelectWindow(GetDlgItem(IDC_STATIC_PATTERN)->m_hWnd);
	}
	str.Format("%d",theApp.m_tSysParam.BmpPatternScale);
	SetDlgItemText(IDC_EDIT_PATTERN_SCALE,str);
	str.Format("%.3f",theApp.m_tSysParam.BmpPatternAcceptance);
	SetDlgItemText(IDC_EDIT_PATTERN_ACCEPTANCE,str);
	str.Format("%.1f",theApp.m_tSysParam.BmpPatternAngle);
	SetDlgItemText(IDC_EDIT_PATTERN_ANGLE,str);
	str.Format("%d",theApp.m_tSysParam.BmpPatternROI.TopLeft().x);
	SetDlgItemText(IDC_EDIT_PATTERN_ROI_LEFT,str);
	str.Format("%d",theApp.m_tSysParam.BmpPatternROI.TopLeft().y);
	SetDlgItemText(IDC_EDIT_PATTERN_ROI_TOP,str);
	str.Format("%d",theApp.m_tSysParam.BmpPatternROI.BottomRight().x);
	SetDlgItemText(IDC_EDIT_PATTERN_ROI_RIGHT,str);
	str.Format("%d",theApp.m_tSysParam.BmpPatternROI.BottomRight().y);
	SetDlgItemText(IDC_EDIT_PATTERN_ROI_BOTTOM,str);
	str.Format("%d",theApp.m_tSysParam.BmpPatternLearnWin.TopLeft().x);
	SetDlgItemText(IDC_EDIT_PATTERN_LEARNWIN_LEFT,str);
	str.Format("%d",theApp.m_tSysParam.BmpPatternLearnWin.TopLeft().y);
	SetDlgItemText(IDC_EDIT_PATTERN_LEARNWIN_TOP,str);
	str.Format("%d",theApp.m_tSysParam.BmpPatternLearnWin.BottomRight().x);
	SetDlgItemText(IDC_EDIT_PATTERN_LEARNWIN_RIGHT,str);
	str.Format("%d",theApp.m_tSysParam.BmpPatternLearnWin.BottomRight().y);
	SetDlgItemText(IDC_EDIT_PATTERN_LEARNWIN_BOTTOM,str);
	str.Format("%.1f",dScore);
	SetDlgItemText(IDC_EDIT_PATTERN_RESULT_SCORE,str);
	str.Format("%.1f",dAngle);
	SetDlgItemText(IDC_EDIT_PATTERN_RESULT_ANGLE,str);
	str.Format("%.3f",dCenterX);
	SetDlgItemText(IDC_EDIT_PATTERN_RESULT_CENTER_X,str);
	str.Format("%.3f",dCenterY);
	SetDlgItemText(IDC_EDIT_PATTERN_RESULT_CENTER_Y,str);
	ROI = theApp.m_tSysParam.BmpPatternROI;
	LearnWin = theApp.m_tSysParam.BmpPatternLearnWin;

	CString strFile = g_pDoc->GetModeFile(0);
	//milApp.SetMatchMode(g_pFrm->m_CmdRun.ProductParam.matchMode);
	milApp.SetModelWindow(LearnWin);
	milApp.SetSearchWindow(ROI);
	milApp.RestoreModel(strFile.GetBuffer(strFile.GetLength()));
	return TRUE;
}

void CDlgPattern::RefreshCurrentPosition()
{
	CString str;
	double dTempPosition;
	for(int i=0;i<3;i++)
	{
		dTempPosition = theApp.m_Mv400.GetPos(K_AXIS_X+i);
		str.Format("%.3f",dTempPosition);
		SetDlgItemText(IDC_EDIT_PATTERN_CURRENT_POS_X+i,str);
	}
}

void CDlgPattern::RefreshGrabImagePosition()
{
	CString str;
	double dTempPosition;
	for(int i=0;i<3;i++)
	{
		dTempPosition = theApp.m_Mv400.GetPos(K_AXIS_X+i);
		str.Format("%.3f",dTempPosition);
		SetDlgItemText(IDC_EDIT_PATTERN_IMAGE_POS_X+i,str);
	}
}

bool CDlgPattern::FindPatternCenter(CRect ROI)
{
	//milApp.SetMatchMode(g_pFrm->m_CmdRun.ProductParam.matchMode);
	milApp.SetPatParam(theApp.m_tSysParam.BmpPatternAcceptance,theApp.m_tSysParam.BmpPatternAngle);
	milApp.SetSearchWindow(ROI);
	if(!milApp.FindModel(true))
	{
		return false;
	}
	dCenterX = milApp.m_stResult.dResultCenterX[0];
	dCenterY = milApp.m_stResult.dResultCenterY[0];
	dAngle = milApp.m_stResult.dResultAngle[0];
	dScore = milApp.m_stResult.dResultScore[0];

	CString str;
	str.Format("%.3f",dCenterX);
	SetDlgItemText(IDC_EDIT_PATTERN_RESULT_CENTER_X,str);
	str.Format("%.3f",dCenterY);
	SetDlgItemText(IDC_EDIT_PATTERN_RESULT_CENTER_Y,str);
	str.Format("%.3f",dAngle);
	SetDlgItemText(IDC_EDIT_PATTERN_RESULT_ANGLE,str);
	str.Format("%.3f",dScore);
	SetDlgItemText(IDC_EDIT_PATTERN_RESULT_SCORE,str);

	GetDC()->Draw3dRect(int(dCenterX-(LearnWin.Width()-1)/2),int(dCenterY-(LearnWin.Height()-1)/2),LearnWin.Width(),LearnWin.Height(),RGB(255,0,0),RGB(255,0,0));
	return true;
}

void CDlgPattern::SaveParam()
{
	CString str;
	GetDlgItemText(IDC_EDIT_PATTERN_SCALE,str);
	theApp.m_tSysParam.BmpPatternScale = atof(str);
	GetDlgItemText(IDC_EDIT_PATTERN_ANGLE,str);
	theApp.m_tSysParam.BmpPatternAngle = atof(str);
	GetDlgItemText(IDC_EDIT_PATTERN_ACCEPTANCE,str);
	theApp.m_tSysParam.BmpPatternAcceptance = atof(str);

	GetDlgItemText(IDC_EDIT_PATTERN_ROI_LEFT,str);
	theApp.m_tSysParam.BmpPatternROI.left = atol(str);
	GetDlgItemText(IDC_EDIT_PATTERN_ROI_RIGHT,str);
	theApp.m_tSysParam.BmpPatternROI.right = atol(str);
	GetDlgItemText(IDC_EDIT_PATTERN_ROI_TOP,str);
	theApp.m_tSysParam.BmpPatternROI.top = atol(str);
	GetDlgItemText(IDC_EDIT_PATTERN_ROI_BOTTOM,str);
	theApp.m_tSysParam.BmpPatternROI.bottom = atol(str);

	GetDlgItemText(IDC_EDIT_PATTERN_LEARNWIN_LEFT,str);
	theApp.m_tSysParam.BmpPatternLearnWin.left = atol(str);
	GetDlgItemText(IDC_EDIT_PATTERN_LEARNWIN_RIGHT,str);
	theApp.m_tSysParam.BmpPatternLearnWin.right = atol(str);
	GetDlgItemText(IDC_EDIT_PATTERN_LEARNWIN_TOP,str);
	theApp.m_tSysParam.BmpPatternLearnWin.top = atol(str);
	GetDlgItemText(IDC_EDIT_PATTERN_LEARNWIN_BOTTOM,str);
	theApp.m_tSysParam.BmpPatternLearnWin.bottom = atol(str);

	theApp.BmpParam(FALSE);
}

void CDlgPattern::OnBnClickedBtnPatternScale()
{
	// TODO: Add your control notification handler code here
	CString str,strText;
	double dTempCenter=0.0;
	double dCurrentPosX = theApp.m_Mv400.GetPos(K_AXIS_X);
	double dCurrentPosY = theApp.m_Mv400.GetPos(K_AXIS_Y);

	CRect ROI(0,0,milApp.m_lBufSizeX-1,milApp.m_lBufSizeY-1);
	if(ROI.IsRectEmpty()||ROI.IsRectNull())
	{
		MessageBox("请设定图像的ROI区域！");
		return;
	}
	this->RefreshCurrentPosition();
	this->OnBnClickedBtnPatternNewpic();
	if(FindPatternCenter(ROI))
	{
		dTempCenter = dCenterX;
	}

	//theApp.m_Mv400.MoveXY(dCurrentPosX+0.5,dCurrentPosY,g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(dCurrentPosX+0.5,dCurrentPosY,true))
	{
		return;
	}
	Sleep(DWORD(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay*1000));
	this->RefreshCurrentPosition();
	this->OnBnClickedBtnPatternNewpic();
	if(FindPatternCenter(ROI))
	{
		theApp.m_tSysParam.BmpPatternScale = 0.5/(dTempCenter-dCenterX);
		strText.Format("图像系数:Scale=%.3f,CenterX1=%.3f,CenterX2=%.3f",theApp.m_tSysParam.BmpPatternScale,dTempCenter,dCenterX);
		g_pFrm->m_CmdRun.AddMsg(strText);
		dTempCenter = dCenterX;
	}

	dCurrentPosX = theApp.m_Mv400.GetPos(K_AXIS_X);
	dCurrentPosY = theApp.m_Mv400.GetPos(K_AXIS_Y);
	//theApp.m_Mv400.MoveXY(dCurrentPosX-1.0,dCurrentPosY,g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(dCurrentPosX-1.0,dCurrentPosY,true))
	{
		return;
	}
	Sleep(DWORD(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay*1000));
	this->RefreshCurrentPosition();
	this->OnBnClickedBtnPatternNewpic();
	if(FindPatternCenter(ROI))
	{
		theApp.m_tSysParam.BmpPatternScale = 1.0/(dCenterX-dTempCenter);
		strText.Format("图像系数:Scale=%.3f,CenterX2=%.3f,CenterX3=%.3f",theApp.m_tSysParam.BmpPatternScale,dTempCenter,dCenterX);
		g_pFrm->m_CmdRun.AddMsg(strText);
	}
	str.Format("%.3f",theApp.m_tSysParam.BmpPatternScale);
	SetDlgItemText(IDC_EDIT_PATTERN_SCALE,str);
	ROI = theApp.m_tSysParam.BmpPatternROI;
}

void CDlgPattern::OnBnClickedBtnPatternNewpic()
{
	// TODO: Add your control notification handler code here
	if(!g_pView->m_ImgStatic.CopyImage(RGB8))
	{
		AfxMessageBox("抓取图像失败！");
		return;
	}
	milApp.DisSelectWindow(GetDlgItem(IDC_STATIC_PATTERN)->m_hWnd);
}

void CDlgPattern::OnBnClickedBtnPatternFirstImagePos()
{
	// TODO: Add your control notification handler code here
	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		return;
	}
	tgPos tgStartPos = g_pFrm->m_CmdRun.GetFirstCameraPos();

	//theApp.m_Mv400.MoveXY(tgStartPos.x,tgStartPos.y,
	//	g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgStartPos.x,tgStartPos.y,true))
	{
		return;
	}
	if(!g_pFrm->m_CmdRun.SynchronizeMoveZ(tgStartPos.za,true))
	{
		return;
	}

	theApp.m_Mv400.WaitStop();
	nFirstRow = 0;
	nFirstColumn = 0;
	nSecondRow = 0;
	nSecondColumn = 0;
	nThirdRow = 0;
	nThirdColumn = 0;
	RefreshGrabImagePosition();
	RefreshCurrentPosition();
	Sleep(DWORD(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay*1000));
	OnBnClickedBtnPatternNewpic();
}

void CDlgPattern::OnBnClickedBtnPatternNextImagePos()
{
	// TODO: Add your control notification handler code here
	nThirdColumn++;
	if(nThirdColumn>=g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn)
	{
		nThirdColumn = 0;
		nThirdRow++;
		if(nThirdRow>=g_pFrm->m_CmdRun.ProductParam.ThirdDispRow)
		{
			nThirdRow = 0;
			nSecondColumn++;
			if(nSecondColumn>=g_pFrm->m_CmdRun.ProductParam.SecondDispColumn)
			{
				nSecondColumn = 0;
				nSecondRow++;
				if(nSecondRow>=g_pFrm->m_CmdRun.ProductParam.SecondDispRow)
				{
					nSecondRow = 0;
					nFirstColumn++;
					if(nFirstColumn>=g_pFrm->m_CmdRun.ProductParam.FirstDispColumn)
					{
						nFirstColumn = 0;
						nFirstRow++;
						if(nFirstRow>=g_pFrm->m_CmdRun.ProductParam.FirstDispRow)
						{
							nFirstRow = 0;
						}
					}
				}
			}
		}
	}
	tgPos tgCurrentPos = g_pFrm->m_CmdRun.GetPadCameraPos(nFirstRow,nFirstColumn,nSecondRow,nSecondColumn,nThirdRow,nThirdColumn);

	//theApp.m_Mv400.MoveXY(tgCurrentPos.x,tgCurrentPos.y,
	//	g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgCurrentPos.x,tgCurrentPos.y,true))
	{
		return;
	}
	if(!g_pFrm->m_CmdRun.SynchronizeMoveZ(tgCurrentPos.za,true))
	{
		return;
	}

	theApp.m_Mv400.WaitStop();
	RefreshGrabImagePosition();
	RefreshCurrentPosition();
	Sleep(DWORD(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay*1000));
	OnBnClickedBtnPatternNewpic();
}

void CDlgPattern::OnBnClickedBtnPatternSearch()
{
	// TODO: Add your control notification handler code here
	OnBnClickedBtnPatternNewpic();
	CString str;
	GetDlgItemText(IDC_EDIT_PATTERN_ROI_LEFT,str);
	ROI.left = atoi(str);
	GetDlgItemText(IDC_EDIT_PATTERN_ROI_TOP,str);
	ROI.top = atoi(str);
	GetDlgItemText(IDC_EDIT_PATTERN_ROI_RIGHT,str);
	ROI.right = atoi(str);
	GetDlgItemText(IDC_EDIT_PATTERN_ROI_BOTTOM,str);
	ROI.bottom = atoi(str);
	FindPatternCenter(ROI);
}

void CDlgPattern::OnBnClickedBtnPatternMoveCenter()
{
	// TODO: Add your control notification handler code here
	double dCurrentX = theApp.m_Mv400.GetPos(K_AXIS_X);
	double dCurrentY = theApp.m_Mv400.GetPos(K_AXIS_Y);
	double dX=(dCenterX-(milApp.m_lBufSizeX-1)/2)*theApp.m_tSysParam.BmpPatternScale;
	double dY=(dCenterY-(milApp.m_lBufSizeY-1)/2)*theApp.m_tSysParam.BmpPatternScale;
	dCurrentX -= dX;
	dCurrentY -= dY;
	//theApp.m_Mv400.MoveXY(dCurrentX,dCurrentY,g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(dCurrentX,dCurrentY,true))
	{
		return;
	}
	RefreshCurrentPosition();
}

void CDlgPattern::OnBnClickedBtnPatternSaveModel()
{
	// TODO: Add your control notification handler code here
	CString strFile;
	strFile = g_pDoc->GetModeFile(0);
	CString str;
	GetDlgItemText(IDC_EDIT_PATTERN_LEARNWIN_LEFT,str);
	LearnWin.left = atoi(str);
	GetDlgItemText(IDC_EDIT_PATTERN_LEARNWIN_TOP,str);
	LearnWin.top = atoi(str);
	GetDlgItemText(IDC_EDIT_PATTERN_LEARNWIN_RIGHT,str);
	LearnWin.right = atoi(str);
	GetDlgItemText(IDC_EDIT_PATTERN_LEARNWIN_BOTTOM,str);
	LearnWin.bottom = atoi(str);
	theApp.m_tSysParam.BmpPatternLearnWin = LearnWin;
	milApp.LearnModel(LearnWin);
	milApp.SaveModel(strFile.GetBuffer(strFile.GetLength()));
}

void CDlgPattern::OnBnClickedBtnPatternSaveLearnWin()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_PATTERN_LEARNWIN_LEFT,str);
	theApp.m_tSysParam.BmpPatternLearnWin.TopLeft().x = atoi(str);
	GetDlgItemText(IDC_EDIT_PATTERN_LEARNWIN_TOP,str);
	theApp.m_tSysParam.BmpPatternLearnWin.TopLeft().y = atoi(str);
	GetDlgItemText(IDC_EDIT_PATTERN_LEARNWIN_RIGHT,str);
	theApp.m_tSysParam.BmpPatternLearnWin.BottomRight().x = atoi(str);
	GetDlgItemText(IDC_EDIT_PATTERN_LEARNWIN_BOTTOM,str);
	theApp.m_tSysParam.BmpPatternLearnWin.BottomRight().y = atoi(str);
	theApp.BmpParam(false);
}

void CDlgPattern::OnBnClickedBtnPatternShowLearnWin()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_PATTERN_LEARNWIN_LEFT,str);
	int nLeft = atoi(str);
	GetDlgItemText(IDC_EDIT_PATTERN_LEARNWIN_RIGHT,str);
	int nRight = atoi(str);
	GetDlgItemText(IDC_EDIT_PATTERN_LEARNWIN_TOP,str);
	int nTop = atoi(str);
	GetDlgItemText(IDC_EDIT_PATTERN_LEARNWIN_BOTTOM,str);
	int nBottom = atoi(str);

	if(theApp.m_tSysParam.nSubstrateType==0)
	{
		GetDC()->Draw3dRect(nLeft/2,nTop/2,(nRight-nLeft)/2,(nBottom-nTop)/2,RGB(0,0,255),RGB(0,0,255));
	}
	else
	{
		GetDC()->Draw3dRect(nLeft,nTop,nRight-nLeft,nBottom-nTop,RGB(0,0,255),RGB(0,0,255));
	}
}

void CDlgPattern::OnBnClickedBtnPatternSaveRoi()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_PATTERN_ROI_LEFT,str);
	theApp.m_tSysParam.BmpPatternROI.TopLeft().x = atoi(str);
	GetDlgItemText(IDC_EDIT_PATTERN_ROI_TOP,str);
	theApp.m_tSysParam.BmpPatternROI.TopLeft().y = atoi(str);
	GetDlgItemText(IDC_EDIT_PATTERN_ROI_RIGHT,str);
	theApp.m_tSysParam.BmpPatternROI.BottomRight().x = atoi(str);
	GetDlgItemText(IDC_EDIT_PATTERN_ROI_BOTTOM,str);
	theApp.m_tSysParam.BmpPatternROI.BottomRight().y = atoi(str);
	theApp.BmpParam(false);
}

void CDlgPattern::OnBnClickedBtnPatternShowRoi()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_PATTERN_ROI_LEFT,str);
	int nLeft = atoi(str);
	GetDlgItemText(IDC_EDIT_PATTERN_ROI_RIGHT,str);
	int nRight = atoi(str);
	GetDlgItemText(IDC_EDIT_PATTERN_ROI_TOP,str);
	int nTop = atoi(str);
	GetDlgItemText(IDC_EDIT_PATTERN_ROI_BOTTOM,str);
	int nBottom = atoi(str);

	GetDC()->Draw3dRect(nLeft,nTop,nRight-nLeft,nBottom-nTop,RGB(255,0,0),RGB(255,0,0));
}

void CDlgPattern::OnBnClickedBtnPatternClear()
{
	// TODO: Add your control notification handler code here
	this->Invalidate();
}

void CDlgPattern::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	SaveParam();
	OnOK();
}

void CDlgPattern::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
