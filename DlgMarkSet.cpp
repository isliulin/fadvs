// CDlgMarkSet.cpp : implementation file
//
#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgMarkSet.h"
#include <math.h>

// CDlgMarkSet dialog

IMPLEMENT_DYNAMIC(CDlgMarkSet, CDialog)

CDlgMarkSet::CDlgMarkSet(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMarkSet::IDD, pParent)
{
	dCenterX = (milApp.m_lBufSizeX-1)/2;
	dCenterY = (milApp.m_lBufSizeY-1)/2;
	dAngle = 0.0;
	dScore = 0.0;

	nWidth = milApp.m_lBufSizeX;
	nHeight = milApp.m_lBufSizeY;
}

CDlgMarkSet::~CDlgMarkSet()
{
}

void CDlgMarkSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_STATIC_MARK_PIC,m_picture);
	DDX_Control(pDX,IDC_COMBO_MARK_SELECT,m_cbSelect);
	DDX_Control(pDX,IDC_SLIDER_MARK_EXPOSURE_TIME,m_sliderShutter);
	DDX_Control(pDX,IDC_SLIDER_MARK_GAIN,m_sliderGain);
}


BEGIN_MESSAGE_MAP(CDlgMarkSet, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_MARK_SCALE, &CDlgMarkSet::OnBnClickedBtnMarkScale)
	ON_BN_CLICKED(IDC_BTN_MARK_GRABPOS, &CDlgMarkSet::OnBnClickedBtnMarkGrabpos)
	ON_BN_CLICKED(IDC_BTN_MARK_NEWPIC, &CDlgMarkSet::OnBnClickedBtnMarkNewpic)
	ON_BN_CLICKED(IDC_BTN_MARK_SAVE_LEARN_WIN, &CDlgMarkSet::OnBnClickedBtnMarkSaveLearnWin)
	ON_BN_CLICKED(IDC_BTN_MARK_SHOW_LEARN_WIN, &CDlgMarkSet::OnBnClickedBtnMarkShowLearnWin)
	ON_BN_CLICKED(IDC_BTN_MARK_SAVE_ROI, &CDlgMarkSet::OnBnClickedBtnMarkSaveRoi)
	ON_BN_CLICKED(IDC_BTN_MARK_SAVE_MODEL, &CDlgMarkSet::OnBnClickedBtnMarkSaveModel)
	ON_BN_CLICKED(IDC_BTN_MARK_SEARCH, &CDlgMarkSet::OnBnClickedBtnMarkSearch)
	ON_BN_CLICKED(IDC_BTN_MARK_MOVE_CENTER, &CDlgMarkSet::OnBnClickedBtnMarkMoveCenter)
	ON_BN_CLICKED(IDC_BTN_MARK_CLEAR, &CDlgMarkSet::OnBnClickedBtnMarkClear)
	ON_BN_CLICKED(IDOK, &CDlgMarkSet::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgMarkSet::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBO_MARK_SELECT, &CDlgMarkSet::OnCbnSelchangeComboMarkSelect)
	ON_EN_KILLFOCUS(IDC_EDIT_MARK_EXPOSURE_TIME,&CDlgMarkSet::OnKillfocusEditMarkExposureTime)
	ON_EN_KILLFOCUS(IDC_EDIT_MARK_GAIN,&CDlgMarkSet::OnKillfocusEditMarkGain)
	ON_BN_CLICKED(IDC_BTN_MARK_TEST, &CDlgMarkSet::OnBnClickedBtnMarkTest)
END_MESSAGE_MAP()


// CDlgMarkSet message handlers

BOOL CDlgMarkSet::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString str;
	m_picture.MoveWindow(0,0,nWidth,nHeight,true);
	/*if(milApp.m_MilImage!=NULL)
	{
		milApp.DisSelectWindow(GetDlgItem(IDC_STATIC_MARK_PIC)->m_hWnd);
	}*/
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,TRUE);
	m_cbSelect.AddString("开始图像");
	m_cbSelect.AddString("结束图像");
	m_cbSelect.SetCurSel(0);
	nSelectID = m_cbSelect.GetCurSel();
	OnCbnSelchangeComboMarkSelect();
	InitUI();
	RefreshPosition();

	return TRUE;
}

void CDlgMarkSet::OnBnClickedBtnMarkScale()
{
	// TODO: Add your control notification handler code here
	if(nSelectID<0||nSelectID>1)
	{
		return;
	}
	CString str,strText;
	tgPos tgTempPixel;
	double dTempA=0.0;
	double dTempB=0.0;
	double dTempC=0.0;
	double dTempD=0.0;
	CRect ROI(0,0,milApp.m_lBufSizeX-1,milApp.m_lBufSizeY-1);
	if(ROI.IsRectEmpty()||ROI.IsRectNull())
	{
		MessageBox("请设定图像的ROI区域！");
		return;
	}
	tgPos tgStartPos = g_pFrm->m_CmdRun.GetFirstCameraPos();
	tgPos tgEndPos = g_pFrm->m_CmdRun.GetLastCameraPos();
	if(nSelectID==0)
	{
		if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgStartPos.x+0.5,tgStartPos.y,true))
		{
			return;
		}
	}
	else if(nSelectID==1)
	{
		if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgEndPos.x+0.5,tgEndPos.y,true))
		{
			return;
		}
	}
	Sleep(DWORD(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay*1000));
	this->OnBnClickedBtnMarkNewpic();
	if(FindModelCenter(ROI))
	{
		tgTempPixel.x = dCenterX;
		tgTempPixel.y = dCenterY;
	}
	else
	{
		return;
	}
	if(nSelectID==0)
	{
		//theApp.m_Mv400.MoveXY(tgStartPos.x-0.5,tgStartPos.y,g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
		if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgStartPos.x-0.5,tgStartPos.y,true))
		{
			return;
		}
	}
	else if(nSelectID==1)
	{
		//theApp.m_Mv400.MoveXY(tgEndPos.x-0.5,tgEndPos.y,g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
		if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgEndPos.x-0.5,tgEndPos.y,true))
		{
			return;
		}
	}
	Sleep(DWORD(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay*1000));
	this->OnBnClickedBtnMarkNewpic();
	if(FindModelCenter(ROI))
	{
		dTempA = tgTempPixel.x - dCenterX;
		dTempB = tgTempPixel.y - dCenterY;
		strText.Format("图像系数:dCalibrationA=%.5f,dCalibrationB=%.5f",theApp.m_tSysParam.BmpMarkCalibration.dCalibrationA,theApp.m_tSysParam.BmpMarkCalibration.dCalibrationB);
		g_pFrm->m_CmdRun.AddMsg(strText);
	}
	else
	{
		return;
	}
	if(nSelectID==0)
	{
		//theApp.m_Mv400.MoveXY(tgStartPos.x-0.5,tgStartPos.y,g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
		if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgStartPos.x,tgStartPos.y+0.5,true))
		{
			return;
		}
	}
	else if(nSelectID==1)
	{
		//theApp.m_Mv400.MoveXY(tgEndPos.x-0.5,tgEndPos.y,g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
		if(g_pFrm->m_CmdRun.SynchronizeMoveXY(tgEndPos.x,tgEndPos.y+0.5,true))
		{
			return;
		}
	}
	Sleep(DWORD(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay*1000));
	this->OnBnClickedBtnMarkNewpic();
	if(FindModelCenter(ROI))
	{
		tgTempPixel.x = dCenterX;
		tgTempPixel.y = dCenterY;
	}
	else
	{
		return;
	}
	if(nSelectID==0)
	{
		//theApp.m_Mv400.MoveXY(tgStartPos.x-0.5,tgStartPos.y,g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
		if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgStartPos.x,tgStartPos.y-0.5,true))
		{
			return;
		}
	}
	else if(nSelectID==1)
	{
		//theApp.m_Mv400.MoveXY(tgEndPos.x-0.5,tgEndPos.y,g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
		if(g_pFrm->m_CmdRun.SynchronizeMoveXY(tgEndPos.x,tgEndPos.y-0.5,true))
		{
			return;
		}
	}
	Sleep(DWORD(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay*1000));
	this->OnBnClickedBtnMarkNewpic();
	if(FindModelCenter(ROI))
	{
		dTempC = tgTempPixel.x - dCenterX;
		dTempD = tgTempPixel.y - dCenterY;
		strText.Format("图像系数:dCalibrationA=%.5f,dCalibrationB=%.5f",theApp.m_tSysParam.BmpMarkCalibration.dCalibrationC,theApp.m_tSysParam.BmpMarkCalibration.dCalibrationD);
		g_pFrm->m_CmdRun.AddMsg(strText);
	}
	else
	{
		return;
	}
	if(nSelectID==0)
	{
		//theApp.m_Mv400.MoveXY(tgStartPos.x-0.5,tgStartPos.y,g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
		if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgStartPos.x,tgStartPos.y,true))
		{
			return;
		}
	}
	else if(nSelectID==1)
	{
		//theApp.m_Mv400.MoveXY(tgEndPos.x-0.5,tgEndPos.y,g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
		if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgEndPos.x,tgEndPos.y,true))
		{
			return;
		}
	}
	double det = dTempA*dTempD - dTempB*dTempC;
	if(fabs(det)<0.001)
	{
		return;
	}
	theApp.m_tSysParam.BmpMarkCalibration.dCalibrationA = dTempD/det;
	theApp.m_tSysParam.BmpMarkCalibration.dCalibrationB = dTempB/det;
	theApp.m_tSysParam.BmpMarkCalibration.dCalibrationC = dTempC/det;
	theApp.m_tSysParam.BmpMarkCalibration.dCalibrationD = dTempA/det;
	str.Format("%0.5f",theApp.m_tSysParam.BmpMarkCalibration.dCalibrationA);
	SetDlgItemText(IDC_EDIT_MARK_CALIBRATION_A,str);
	str.Format("%0.5f",theApp.m_tSysParam.BmpMarkCalibration.dCalibrationB);
	SetDlgItemText(IDC_EDIT_MARK_CALIBRATION_B,str);
	str.Format("%0.5f",theApp.m_tSysParam.BmpMarkCalibration.dCalibrationC);
	SetDlgItemText(IDC_EDIT_MARK_CALIBRATION_C,str);
	str.Format("%0.5f",theApp.m_tSysParam.BmpMarkCalibration.dCalibrationD);
	SetDlgItemText(IDC_EDIT_MARK_CALIBRATION_D,str);
}

void CDlgMarkSet::OnBnClickedBtnMarkGrabpos()
{
	// TODO: Add your control notification handler code here
	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		return;
	}
	tgPos tgStartPos = g_pFrm->m_CmdRun.GetFirstCameraPos();
	tgPos tgEndPos = g_pFrm->m_CmdRun.GetLastCameraPos();
	if(nSelectID==0)
	{
		//theApp.m_Mv400.MoveXY(tgStartPos.x,tgStartPos.y,
		//	g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
		if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgStartPos.x,tgStartPos.y,true))
		{
			return;
		}

		//theApp.m_Mv400.Move(K_AXIS_ZA,tgStartPos.za,g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
	}
	else if(nSelectID==1)
	{
		//theApp.m_Mv400.MoveXY(tgEndPos.x,tgEndPos.y,
		//	g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
		if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgEndPos.x,tgEndPos.y,true))
		{
			return;
		}
		//theApp.m_Mv400.Move(K_AXIS_ZA,tgEndPos.za,g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
	}
	//else if(nSelectID==2)
	//{
	//	g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.tSlotDetectPos[nTrackID].x,g_pFrm->m_CmdRun.ProductParam.tSlotDetectPos[nTrackID].y,
	//		g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,true);
	//	theApp.m_Mv400.Move(K_AXIS_Z,g_pFrm->m_CmdRun.ProductParam.tSlotDetectPos[nTrackID].z,
	//		g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
	//}
	RefreshPosition();
	Sleep(DWORD(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay*1000));
	g_pFrm->m_CmdRun.AddMsg("OnBnClickedBtnMarkGrabpos");
	OnBnClickedBtnMarkNewpic();
}

void CDlgMarkSet::OnBnClickedBtnMarkNewpic()
{
	// TODO: Add your control notification handler code here
	if(!g_pView->m_ImgStatic.CopyImage(RGB8))
	{
		AfxMessageBox("抓取图像失败！");
	}
	//milApp.DisSelectWindow(GetDlgItem(IDC_STATIC_MARK_PIC)->m_hWnd);*/
}

void CDlgMarkSet::OnBnClickedBtnMarkSaveLearnWin()
{
	// TODO: Add your control notification handler code here
	//CString str;
	//GetDlgItemText(IDC_EDIT_MARK_LEARN_WIN_LEFT,str);
	//theApp.m_tSysParam.BmpMarkLearnWin.TopLeft().x = atoi(str);
	//GetDlgItemText(IDC_EDIT_MARK_LEARN_WIN_TOP,str);
	//theApp.m_tSysParam.BmpMarkLearnWin.TopLeft().y = atoi(str);
	//GetDlgItemText(IDC_EDIT_MARK_LEARN_WIN_RIGHT,str);
	//theApp.m_tSysParam.BmpMarkLearnWin.BottomRight().x = atoi(str);
	//GetDlgItemText(IDC_EDIT_MARK_LEARN_WIN_BOTTOM,str);
	//theApp.m_tSysParam.BmpMarkLearnWin.BottomRight().y = atoi(str);
	theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].TopLeft().x = long(m_picture.m_mod_rectangle.dx - m_picture.m_mod_rectangle.width/2);
	theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].TopLeft().y = long(m_picture.m_mod_rectangle.dy - m_picture.m_mod_rectangle.height/2);
	theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].BottomRight().x = long(theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].TopLeft().x + m_picture.m_mod_rectangle.width);
	theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].BottomRight().y = long(theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].TopLeft().y + m_picture.m_mod_rectangle.height);
	UpdateUI();
	theApp.BmpParam(FALSE);
}

void CDlgMarkSet::OnBnClickedBtnMarkShowLearnWin()
{
	// TODO: Add your control notification handler code here
	//CString str;
	//GetDlgItemText(IDC_EDIT_MARK_LEARN_WIN_LEFT,str);
	//int nLeft = atoi(str);
	//GetDlgItemText(IDC_EDIT_MARK_LEARN_WIN_RIGHT,str);
	//int nRight = atoi(str);
	//GetDlgItemText(IDC_EDIT_MARK_LEARN_WIN_TOP,str);
	//int nTop = atoi(str);
	//GetDlgItemText(IDC_EDIT_MARK_LEARN_WIN_BOTTOM,str);
	//int nBottom = atoi(str);
	//
	//GetDC()->Draw3dRect(nLeft,nTop,nRight-nLeft,nBottom-nTop,RGB(0,0,255),RGB(0,0,255));
}

void CDlgMarkSet::OnBnClickedBtnMarkSaveRoi()
{
	// TODO: Add your control notification handler code here
	//CString str;
	//GetDlgItemText(IDC_EDIT_MARK_ROI_LEFT,str);
	//theApp.m_tSysParam.BmpMarkROI.TopLeft().x = atoi(str);
	//GetDlgItemText(IDC_EDIT_MARK_ROI_TOP,str);
	//theApp.m_tSysParam.BmpMarkROI.TopLeft().y = atoi(str);
	//GetDlgItemText(IDC_EDIT_MARK_ROI_RIGHT,str);
	//theApp.m_tSysParam.BmpMarkROI.BottomRight().x = atoi(str);
	//GetDlgItemText(IDC_EDIT_MARK_ROI_BOTTOM,str);
	//theApp.m_tSysParam.BmpMarkROI.BottomRight().y = atoi(str);
	theApp.m_tSysParam.BmpMarkROI = m_picture.RoiRect;
	UpdateUI();
	theApp.BmpParam(FALSE);
}

void CDlgMarkSet::OnBnClickedBtnMarkSaveModel()
{
	// TODO: Add your control notification handler code here
	CString strFile;
	OnBnClickedBtnMarkNewpic();
	nSelectID = m_cbSelect.GetCurSel();
	strFile = g_pDoc->GetModeFile(0);
	CString str;
	GetDlgItemText(IDC_EDIT_MARK_LEARN_WIN_LEFT,str);
	LearnWin.left = atoi(str);
	GetDlgItemText(IDC_EDIT_MARK_LEARN_WIN_TOP,str);
	LearnWin.top = atoi(str);
	GetDlgItemText(IDC_EDIT_MARK_LEARN_WIN_RIGHT,str);
	LearnWin.right = atoi(str);
	GetDlgItemText(IDC_EDIT_MARK_LEARN_WIN_BOTTOM,str);
	LearnWin.bottom = atoi(str);
	theApp.m_tSysParam.BmpMarkLearnWin[nSelectID] = LearnWin;
	milApp.LearnModel(LearnWin);
	milApp.SaveModel(strFile.GetBuffer(strFile.GetLength()));
}

void CDlgMarkSet::OnBnClickedBtnMarkSearch()
{
	// TODO: Add your control notification handler code here
	OnBnClickedBtnMarkNewpic();
	CString str;
	GetDlgItemText(IDC_EDIT_MARK_ROI_LEFT,str);
	ROI.left = atoi(str);
	GetDlgItemText(IDC_EDIT_MARK_ROI_TOP,str);
	ROI.top = atoi(str);
	GetDlgItemText(IDC_EDIT_MARK_ROI_RIGHT,str);
	ROI.right = atoi(str);
	GetDlgItemText(IDC_EDIT_MARK_ROI_BOTTOM,str);
	ROI.bottom = atoi(str);
	FindModelCenter(ROI);
}

void CDlgMarkSet::OnBnClickedBtnMarkMoveCenter()
{
	// TODO: Add your control notification handler code here
	OnBnClickedBtnMarkSearch();
	double dCurrentX = theApp.m_Mv400.GetPos(K_AXIS_X);
	double dCurrentY = theApp.m_Mv400.GetPos(K_AXIS_Y);
	double dX,dY;
	theApp.m_tSysParam.BmpMarkCalibration.ConvertPixelToTable(dCenterX-(milApp.m_lBufSizeX-1)/2,
		dCenterY-(milApp.m_lBufSizeY-1)/2,dX,dY);
	dCurrentX -= dX;
	dCurrentY -= dY;
	//theApp.m_Mv400.MoveXY(dCurrentX,dCurrentY,g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(dCurrentX,dCurrentY,true))
	{
		return;
	}
}

void CDlgMarkSet::OnBnClickedBtnMarkClear()
{
	// TODO: Add your control notification handler code here
	this->Invalidate();
}

void CDlgMarkSet::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	SaveParam();
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,FALSE);
	OnOK();
}

void CDlgMarkSet::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,FALSE);
	OnCancel();
}

void CDlgMarkSet::RefreshPosition()
{
	CString str;
	double dTempPosition;
	for(int i=0;i<2;i++)
	{
		dTempPosition = theApp.m_Mv400.GetPos(K_AXIS_X+i);
		str.Format("%.3f",dTempPosition);
		SetDlgItemText(IDC_EDIT_MARK_CURRENT_X+i,str);
	}
}

bool CDlgMarkSet::FindModelCenter(CRect ROI)
{
	//milApp.SetMatchMode(g_pFrm->m_CmdRun.ProductParam.matchMode);
	milApp.SetPatParam(theApp.m_tSysParam.BmpMarkAcceptance,theApp.m_tSysParam.BmpMarkAngle);
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
	SetDlgItemText(IDC_EDIT_MARK_RESULT_CENTER_X,str);
	str.Format("%.3f",dCenterY);
	SetDlgItemText(IDC_EDIT_MARK_RESULT_CENTER_Y,str);
	str.Format("%.3f",dAngle);
	SetDlgItemText(IDC_EDIT_MARK_RESULT_ANGLE,str);
	str.Format("%.3f",dScore);
	SetDlgItemText(IDC_EDIT_MARK_RESULT_SCORE,str);

	int left = int(dCenterX-(LearnWin.Width()-1)/2);
	int top = int(dCenterY-(LearnWin.Height()-1)/2);
	int width = LearnWin.Width();
	int height = LearnWin.Height();
	GetDC()->Draw3dRect(left,top,width,height,RGB(255,0,0),RGB(255,0,0));
	return true;
}

void CDlgMarkSet::SaveParam()
{
	CString str;
	//GetDlgItemText(IDC_EDIT_MARK_SCALE,str);
	//theApp.m_tSysParam.BmpMarkScale = atof(str);
	GetDlgItemText(IDC_EDIT_MARK_ANGLE,str);
	theApp.m_tSysParam.BmpMarkAngle = atof(str);
	GetDlgItemText(IDC_EDIT_MARK_ACCEPTANCE,str);
	theApp.m_tSysParam.BmpMarkAcceptance = atof(str);

	GetDlgItemText(IDC_EDIT_MARK_ROI_LEFT,str);
	theApp.m_tSysParam.BmpMarkROI.left = atol(str);
	GetDlgItemText(IDC_EDIT_MARK_ROI_RIGHT,str);
	theApp.m_tSysParam.BmpMarkROI.right = atol(str);
	GetDlgItemText(IDC_EDIT_MARK_ROI_TOP,str);
	theApp.m_tSysParam.BmpMarkROI.top = atol(str);
	GetDlgItemText(IDC_EDIT_MARK_ROI_BOTTOM,str);
	theApp.m_tSysParam.BmpMarkROI.bottom = atol(str);

	GetDlgItemText(IDC_EDIT_MARK_LEARN_WIN_LEFT,str);
	theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].left = atol(str);
	GetDlgItemText(IDC_EDIT_MARK_LEARN_WIN_RIGHT,str);
	theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].right = atol(str);
	GetDlgItemText(IDC_EDIT_MARK_LEARN_WIN_TOP,str);
	theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].top = atol(str);
	GetDlgItemText(IDC_EDIT_MARK_LEARN_WIN_BOTTOM,str);
	theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].bottom = atol(str);

	GetDlgItemText(IDC_EDIT_MARK_CALIBRATION_A,str);
	theApp.m_tSysParam.BmpMarkCalibration.dCalibrationA = atof(str);
	GetDlgItemText(IDC_EDIT_MARK_CALIBRATION_B,str);
	theApp.m_tSysParam.BmpMarkCalibration.dCalibrationB = atof(str);
	GetDlgItemText(IDC_EDIT_MARK_CALIBRATION_C,str);
	theApp.m_tSysParam.BmpMarkCalibration.dCalibrationC = atof(str);
	GetDlgItemText(IDC_EDIT_MARK_CALIBRATION_D,str);
	theApp.m_tSysParam.BmpMarkCalibration.dCalibrationD = atof(str);
	theApp.BmpParam(FALSE);
}

void CDlgMarkSet::OnCbnSelchangeComboMarkSelect()
{
	// TODO: Add your control notification handler code here
	nSelectID = m_cbSelect.GetCurSel();
	UpdateUI();
}

void CDlgMarkSet::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nValue = 0;
	CString str;
	if(pScrollBar == (CScrollBar *)&m_sliderShutter)
	{
		nValue = m_sliderShutter.GetPos();
		g_pView->m_ImgStatic.m_pCamera.SetExposureTime((long)nValue);
		theApp.m_tSysParam.CameraExposureTime = nValue;
		str.Format("%d",nValue);
		SetDlgItemText(IDC_EDIT_MARK_EXPOSURE_TIME,str);
	}
	else if(pScrollBar == (CScrollBar *)&m_sliderGain)
	{
		nValue = m_sliderGain.GetPos();
		g_pView->m_ImgStatic.m_pCamera.SetGain((long)nValue);
		theApp.m_tSysParam.CameraGain = nValue;
		str.Format("%d",nValue);
		SetDlgItemText(IDC_EDIT_MARK_GAIN,str);
	}
	theApp.BmpParam(FALSE);
}

void CDlgMarkSet::OnKillfocusEditMarkExposureTime()
{
	int nValue = 0;
	CString str;
	GetDlgItemText(IDC_EDIT_MARK_EXPOSURE_TIME,str);
	nValue = atoi(str);
	m_sliderShutter.SetPos(nValue);
	g_pView->m_ImgStatic.m_pCamera.SetExposureTime((long)nValue);
	theApp.m_tSysParam.CameraExposureTime = nValue;
	theApp.BmpParam(FALSE);
}

void CDlgMarkSet::OnKillfocusEditMarkGain()
{
	int nValue = 0;
	CString str;
	GetDlgItemText(IDC_EDIT_MARK_GAIN,str);
	nValue = atoi(str);
	m_sliderGain.SetPos(nValue);
	g_pView->m_ImgStatic.m_pCamera.SetGain((long)nValue);
	theApp.m_tSysParam.CameraGain = nValue;
	theApp.BmpParam(FALSE);
}

void CDlgMarkSet::InitUI()
{
	CString str;
	m_sliderShutter.SetRange(0,999);
	m_sliderGain.SetRange(0,63);
	m_sliderShutter.SetPos(theApp.m_tSysParam.CameraExposureTime);
	m_sliderGain.SetPos(theApp.m_tSysParam.CameraGain);
	str.Format("%d",theApp.m_tSysParam.CameraExposureTime);
	SetDlgItemText(IDC_EDIT_MARK_EXPOSURE_TIME,str);
	str.Format("%d",theApp.m_tSysParam.CameraGain);
	SetDlgItemText(IDC_EDIT_MARK_GAIN,str);
}

void CDlgMarkSet::UpdateUI()
{
	CString str;
	tgPos tgStartPos = g_pFrm->m_CmdRun.GetFirstCameraPos();
	tgPos tgEndPos = g_pFrm->m_CmdRun.GetLastCameraPos();

	str.Format("nSelectID:%d",nSelectID);
	g_pFrm->m_CmdRun.AddMsg(str);
	if(nSelectID==0)
	{
		str.Format("%.3f",tgStartPos.x);
		SetDlgItemText(IDC_EDIT_MARK_IMAGE_POS_X,str);
		str.Format("%.3f",tgStartPos.y);
		SetDlgItemText(IDC_EDIT_MARK_IMAGE_POS_Y,str);
	}
	else if(nSelectID==1)
	{
		str.Format("%.3f",tgEndPos.x);
		SetDlgItemText(IDC_EDIT_MARK_IMAGE_POS_X,str);
		str.Format("%.3f",tgEndPos.y);
		SetDlgItemText(IDC_EDIT_MARK_IMAGE_POS_Y,str);
	}
	//else if(nSelectID==2)
	//{
	//	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tSlotDetectPos[nTrackID].x);
	//	SetDlgItemText(IDC_EDIT_MARK_IMAGE_POS_X,str);
	//	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tSlotDetectPos[nTrackID].y);
	//	SetDlgItemText(IDC_EDIT_MARK_IMAGE_POS_Y,str);
	//	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tSlotDetectPos[nTrackID].z);
	//	SetDlgItemText(IDC_EDIT_MARK_IMAGE_POS_Z,str);
	//}

	if((nSelectID==1)||(nSelectID==0))
	{
		//str.Format("%.3f",theApp.m_tSysParam.BmpMarkScale);
		//SetDlgItemText(IDC_EDIT_MARK_SCALE,str);
		str.Format("%.3f",theApp.m_tSysParam.BmpMarkAcceptance);
		SetDlgItemText(IDC_EDIT_MARK_ACCEPTANCE,str);
		str.Format("%.1f",theApp.m_tSysParam.BmpMarkAngle);
		SetDlgItemText(IDC_EDIT_MARK_ANGLE,str);

		str.Format("%0.5f",theApp.m_tSysParam.BmpMarkCalibration.dCalibrationA);
		SetDlgItemText(IDC_EDIT_MARK_CALIBRATION_A,str);
		str.Format("%0.5f",theApp.m_tSysParam.BmpMarkCalibration.dCalibrationB);
		SetDlgItemText(IDC_EDIT_MARK_CALIBRATION_B,str);
		str.Format("%0.5f",theApp.m_tSysParam.BmpMarkCalibration.dCalibrationC);
		SetDlgItemText(IDC_EDIT_MARK_CALIBRATION_C,str);
		str.Format("%0.5f",theApp.m_tSysParam.BmpMarkCalibration.dCalibrationD);
		SetDlgItemText(IDC_EDIT_MARK_CALIBRATION_D,str);

		str.Format("%d",theApp.m_tSysParam.BmpMarkROI.TopLeft().x);
		SetDlgItemText(IDC_EDIT_MARK_ROI_LEFT,str);
		str.Format("%d",theApp.m_tSysParam.BmpMarkROI.TopLeft().y);
		SetDlgItemText(IDC_EDIT_MARK_ROI_TOP,str);
		str.Format("%d",theApp.m_tSysParam.BmpMarkROI.BottomRight().x);
		SetDlgItemText(IDC_EDIT_MARK_ROI_RIGHT,str);
		str.Format("%d",theApp.m_tSysParam.BmpMarkROI.BottomRight().y);
		SetDlgItemText(IDC_EDIT_MARK_ROI_BOTTOM,str);

		str.Format("%d",theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].TopLeft().x);
		SetDlgItemText(IDC_EDIT_MARK_LEARN_WIN_LEFT,str);
		str.Format("%d",theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].TopLeft().y);
		SetDlgItemText(IDC_EDIT_MARK_LEARN_WIN_TOP,str);
		str.Format("%d",theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].BottomRight().x);
		SetDlgItemText(IDC_EDIT_MARK_LEARN_WIN_RIGHT,str);
		str.Format("%d",theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].BottomRight().y);
		SetDlgItemText(IDC_EDIT_MARK_LEARN_WIN_BOTTOM,str);

		//if(g_pView->m_ImgStatic.m_pCamera.m_pImageBuffer!=NULL)
		{
			m_picture.m_modtype = MOD_RECTANGLE;
			m_picture.m_iImgWidth = IMAGEWIDTH0;
			m_picture.m_iImgHeight = IMAGEHEIGHT0;
			m_picture.m_pImgBuffer = new BYTE[m_picture.m_iImgWidth*m_picture.m_iImgHeight*3];
			m_picture.m_mod_rectangle.dx = theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].TopLeft().x + theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].Width()/2;
			m_picture.m_mod_rectangle.dy = theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].TopLeft().y + theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].Height()/2;
			m_picture.m_mod_rectangle.width = theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].Width();
			m_picture.m_mod_rectangle.height = theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].Height();
			m_picture.m_mod_scale.length = 20;
			m_picture.m_mod_scale.width = 10;
			m_picture.m_mod_scale.height = 10;
			m_picture.RoiRect =  theApp.m_tSysParam.BmpMarkROI;
			m_picture.SetDisplayMarkImage(true);
			m_picture.ImgInit(IMAGEWIDTH0,IMAGEHEIGHT0,24);

		}

		dScore = 0.0;
		dAngle = 0.0;
		dCenterX = 0.0;
		dCenterY = 0.0;

		str.Format("%.1f",dScore);
		SetDlgItemText(IDC_EDIT_MARK_RESULT_SCORE,str);
		str.Format("%.1f",dAngle);
		SetDlgItemText(IDC_EDIT_MARK_RESULT_ANGLE,str);
		str.Format("%.3f",dCenterX);
		SetDlgItemText(IDC_EDIT_MARK_RESULT_CENTER_X,str);
		str.Format("%.3f",dCenterY);
		SetDlgItemText(IDC_EDIT_MARK_RESULT_CENTER_Y,str);

		CString strFile = g_pDoc->GetModeFile(0);
		ROI = theApp.m_tSysParam.BmpMarkROI;
		LearnWin = theApp.m_tSysParam.BmpMarkLearnWin[nSelectID];
		//milApp.SetMatchMode(g_pFrm->m_CmdRun.ProductParam.matchMode);
		milApp.SetModelWindow(LearnWin);
		milApp.SetSearchWindow(ROI);
		milApp.RestoreModel(strFile.GetBuffer(strFile.GetLength()));
	}
}
void CDlgMarkSet::OnBnClickedBtnMarkTest()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.RunThreadTestImage();
}
