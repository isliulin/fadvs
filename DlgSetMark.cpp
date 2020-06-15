// CDlgMarkSet.cpp : implementation file
//
#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgSetMark.h"
#include <math.h>

// CDlgMarkSet dialog

IMPLEMENT_DYNAMIC(CDlgSetMark, CDialog)

CDlgSetMark::CDlgSetMark(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetMark::IDD, pParent)
{
	dCenterX = (milApp.m_lBufSizeX-1)/2;
	dCenterY = (milApp.m_lBufSizeY-1)/2;
	dAngle = 0.0;
	dScore = 0.0;

	nWidth = milApp.m_lBufSizeX;
	nHeight = milApp.m_lBufSizeY;
}

CDlgSetMark::~CDlgSetMark()
{
}

void CDlgSetMark::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_STATIC_SET_MARK_PIC,m_picture);
	DDX_Control(pDX,IDC_COMBO_SET_MARK_SELECT,m_cbSelect);
	DDX_Control(pDX,IDC_SLIDER_SET_MARK_EXPOSURE_TIME,m_sliderShutter);
	DDX_Control(pDX,IDC_SLIDER_SET_MARK_GAIN,m_sliderGain);
}


BEGIN_MESSAGE_MAP(CDlgSetMark, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_SET_MARK_SCALE, &CDlgSetMark::OnBnClickedBtnSetMarkScale)
	ON_BN_CLICKED(IDC_BTN_SET_MARK_GRABPOS, &CDlgSetMark::OnBnClickedBtnSetMarkGrabpos)
	ON_BN_CLICKED(IDC_BTN_SET_MARK_NEWPIC, &CDlgSetMark::OnBnClickedBtnSetMarkNewpic)
	ON_BN_CLICKED(IDC_BTN_SET_MARK_SAVE_LEARN_WIN, &CDlgSetMark::OnBnClickedBtnSetMarkSaveLearnWin)
	ON_BN_CLICKED(IDC_BTN_SET_MARK_SHOW_LEARN_WIN, &CDlgSetMark::OnBnClickedBtnSetMarkShowLearnWin)
	ON_BN_CLICKED(IDC_BTN_SET_MARK_SAVE_ROI, &CDlgSetMark::OnBnClickedBtnSetMarkSaveRoi)
	ON_BN_CLICKED(IDC_BTN_SET_MARK_SAVE_MODEL, &CDlgSetMark::OnBnClickedBtnSetMarkSaveModel)
	ON_BN_CLICKED(IDC_BTN_SET_MARK_SEARCH, &CDlgSetMark::OnBnClickedBtnSetMarkSearch)
	ON_BN_CLICKED(IDC_BTN_SET_MARK_MOVE_CENTER, &CDlgSetMark::OnBnClickedBtnSetMarkMoveCenter)
	ON_BN_CLICKED(IDC_BTN_SET_MARK_CLEAR, &CDlgSetMark::OnBnClickedBtnSetMarkClear)
	ON_BN_CLICKED(IDOK, &CDlgSetMark::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgSetMark::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBO_SET_MARK_SELECT, &CDlgSetMark::OnCbnSelchangeComboSetMarkSelect)
	ON_EN_KILLFOCUS(IDC_EDIT_SET_MARK_EXPOSURE_TIME,&CDlgSetMark::OnKillfocusEditSetMarkExposureTime)
	ON_EN_KILLFOCUS(IDC_EDIT_SET_MARK_GAIN,&CDlgSetMark::OnKillfocusEditSetMarkGain)
	ON_BN_CLICKED(IDC_BTN_SET_MARK_TEST, &CDlgSetMark::OnBnClickedBtnSetMarkTest)
	ON_BN_CLICKED(IDC_BTN_SET_MARK_SAVE_CENTER, &CDlgSetMark::OnBnClickedBtnSetMarkSaveCenter)
END_MESSAGE_MAP()


// CDlgMarkSet message handlers

BOOL CDlgSetMark::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString str;
	m_picture.MoveWindow(0,0,nWidth,nHeight,true);
	/*if(milApp.m_MilImage!=NULL)
	{
		milApp.DisSelectWindow(GetDlgItem(IDC_STATIC_MARK_PIC)->m_hWnd);
	}*/
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,TRUE);
	for(int i=0;i<g_pFrm->m_CmdRun.ProductParam.nImageNum;i++)
	{
		str.Format("Mark %d",i);
		m_cbSelect.AddString(str);
	}
	str.Format("¼ì²âÍ¼Ïñ");
	m_cbSelect.AddString(str);
	m_cbSelect.SetCurSel(0);
	nSelectID = m_cbSelect.GetCurSel();
	OnCbnSelchangeComboSetMarkSelect();
	InitUI();
	RefreshPosition();

	return TRUE;
}

void CDlgSetMark::OnBnClickedBtnSetMarkScale()
{
	// TODO: Add your control notification handler code here
	if(nSelectID<0||nSelectID>g_pFrm->m_CmdRun.ProductParam.nImageNum)
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
		MessageBox("ÇëÉè¶¨Í¼ÏñµÄROIÇøÓò£¡");
		return;
	}

	tgPos tgImagePos;
	if(nSelectID<g_pFrm->m_CmdRun.ProductParam.nImageNum)
	{
		tgImagePos = g_pFrm->m_CmdRun.ProductParam.tgImagePos[nSelectID];
	}
	else if(nSelectID==g_pFrm->m_CmdRun.ProductParam.nImageNum)
	{
		tgImagePos = g_pFrm->m_CmdRun.ProductParam.tSlotDetectPos;
	}
	g_pFrm->m_CmdRun.SynchronizeMoveXY(tgImagePos.x+0.5,tgImagePos.y,true);

	Sleep(DWORD(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay*1000));
	this->OnBnClickedBtnSetMarkNewpic();
	if(FindModelCenter(ROI))
	{
		tgTempPixel.x = dCenterX;
		tgTempPixel.y = dCenterY;
	}
	else
	{
		return;
	}
	g_pFrm->m_CmdRun.SynchronizeMoveXY(tgImagePos.x-0.5,tgImagePos.y,true);
	Sleep(DWORD(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay*1000));
	this->OnBnClickedBtnSetMarkNewpic();
	if(FindModelCenter(ROI))
	{
		dTempA = tgTempPixel.x - dCenterX;
		dTempB = tgTempPixel.y - dCenterY;
		strText.Format("Í¼ÏñÏµÊý:dCalibrationA=%.5f,dCalibrationB=%.5f",theApp.m_tSysParam.BmpMarkCalibration.dCalibrationA,theApp.m_tSysParam.BmpMarkCalibration.dCalibrationB);
		g_pFrm->m_CmdRun.AddMsg(strText);
	}
	else
	{
		return;
	}
	g_pFrm->m_CmdRun.SynchronizeMoveXY(tgImagePos.x,tgImagePos.y+0.5,true);
	Sleep(DWORD(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay*1000));
	this->OnBnClickedBtnSetMarkNewpic();
	if(FindModelCenter(ROI))
	{
		tgTempPixel.x = dCenterX;
		tgTempPixel.y = dCenterY;
	}
	else
	{
		return;
	}
	g_pFrm->m_CmdRun.SynchronizeMoveXY(tgImagePos.x,tgImagePos.y-0.5,true);
	Sleep(DWORD(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay*1000));
	this->OnBnClickedBtnSetMarkNewpic();
	if(FindModelCenter(ROI))
	{
		dTempC = tgTempPixel.x - dCenterX;
		dTempD = tgTempPixel.y - dCenterY;
		strText.Format("Í¼ÏñÏµÊý:dCalibrationA=%.5f,dCalibrationB=%.5f",theApp.m_tSysParam.BmpMarkCalibration.dCalibrationC,theApp.m_tSysParam.BmpMarkCalibration.dCalibrationD);
		g_pFrm->m_CmdRun.AddMsg(strText);
	}
	else
	{
		return;
	}
	g_pFrm->m_CmdRun.SynchronizeMoveXY(tgImagePos.x,tgImagePos.y,true);
	double det = dTempA*dTempD - dTempB*dTempC;
	if(fabs(det)<0.001)
	{
		return;
	}
	tgCameraCalibration calibration;
	calibration.dCalibrationA = dTempD/det;
	calibration.dCalibrationB = dTempB/det;
	calibration.dCalibrationC = dTempC/det;
	calibration.dCalibrationD = dTempA/det;
	if(nSelectID<g_pFrm->m_CmdRun.ProductParam.nImageNum)
	{
		theApp.m_tSysParam.BmpMarkCalibration = calibration;
	}
	else if(nSelectID==g_pFrm->m_CmdRun.ProductParam.nImageNum)
	{
		theApp.m_tSysParam.BmpSlotCalibration = calibration;
	}
	str.Format("%0.5f",calibration.dCalibrationA);
	SetDlgItemText(IDC_EDIT_SET_MARK_CALIBRATION_A,str);
	str.Format("%0.5f",calibration.dCalibrationB);
	SetDlgItemText(IDC_EDIT_SET_MARK_CALIBRATION_B,str);
	str.Format("%0.5f",calibration.dCalibrationC);
	SetDlgItemText(IDC_EDIT_SET_MARK_CALIBRATION_C,str);
	str.Format("%0.5f",calibration.dCalibrationD);
	SetDlgItemText(IDC_EDIT_SET_MARK_CALIBRATION_D,str);
	theApp.BmpParam(FALSE);
}

void CDlgSetMark::OnBnClickedBtnSetMarkGrabpos()
{
	// TODO: Add your control notification handler code here
	if(nSelectID<0||nSelectID>g_pFrm->m_CmdRun.ProductParam.nImageNum)
	{
		return;
	}
	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		return;
	}
	if(nSelectID<g_pFrm->m_CmdRun.ProductParam.nImageNum)
	{
		if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.tgImagePos[nSelectID].x,g_pFrm->m_CmdRun.ProductParam.tgImagePos[nSelectID].y,true))
		{
			return;
		}
		theApp.m_Mv400.Move(K_AXIS_ZA,g_pFrm->m_CmdRun.ProductParam.tgImagePos[nSelectID].za,g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
		
	}
	else if(nSelectID==g_pFrm->m_CmdRun.ProductParam.nImageNum)
	{
		if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.tSlotDetectPos.x,g_pFrm->m_CmdRun.ProductParam.tSlotDetectPos.y,true))
		{
			return;
		}
	}

	RefreshPosition();
	Sleep(DWORD(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay*1000));
	g_pFrm->m_CmdRun.AddMsg("OnBnClickedBtnSetMarkGrabpos");
	OnBnClickedBtnSetMarkNewpic();
}

void CDlgSetMark::OnBnClickedBtnSetMarkNewpic()
{
	// TODO: Add your control notification handler code here
	if(!g_pView->m_ImgStatic.CopyImage(RGB8))
	{
		AfxMessageBox("×¥È¡Í¼ÏñÊ§°Ü£¡");
	}
	//milApp.DisSelectWindow(GetDlgItem(IDC_STATIC_MARK_PIC)->m_hWnd);*/
}

void CDlgSetMark::OnBnClickedBtnSetMarkSaveLearnWin()
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
	if(nSelectID<g_pFrm->m_CmdRun.ProductParam.nImageNum)
	{
		theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].TopLeft().x = long(m_picture.m_mod_rectangle.dx - m_picture.m_mod_rectangle.width/2);
		theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].TopLeft().y = long(m_picture.m_mod_rectangle.dy - m_picture.m_mod_rectangle.height/2);
		theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].BottomRight().x = long(theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].TopLeft().x + m_picture.m_mod_rectangle.width);
		theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].BottomRight().y = long(theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].TopLeft().y + m_picture.m_mod_rectangle.height);
	}
	else if(nSelectID==g_pFrm->m_CmdRun.ProductParam.nImageNum)
	{
		theApp.m_tSysParam.BmpSlotLearnWin.TopLeft().x = long(m_picture.m_mod_rectangle.dx - m_picture.m_mod_rectangle.width/2);
		theApp.m_tSysParam.BmpSlotLearnWin.TopLeft().y = long(m_picture.m_mod_rectangle.dy - m_picture.m_mod_rectangle.height/2);
		theApp.m_tSysParam.BmpSlotLearnWin.BottomRight().x = long(theApp.m_tSysParam.BmpSlotLearnWin.TopLeft().x + m_picture.m_mod_rectangle.width);
		theApp.m_tSysParam.BmpSlotLearnWin.BottomRight().y = long(theApp.m_tSysParam.BmpSlotLearnWin.TopLeft().y + m_picture.m_mod_rectangle.height);
	}
	UpdateUI();
	theApp.BmpParam(FALSE);
}

void CDlgSetMark::OnBnClickedBtnSetMarkShowLearnWin()
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

void CDlgSetMark::OnBnClickedBtnSetMarkSaveRoi()
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
	if(nSelectID<g_pFrm->m_CmdRun.ProductParam.nImageNum)
	{
		theApp.m_tSysParam.BmpMarkROI = m_picture.RoiRect;
	}
	else if(nSelectID==g_pFrm->m_CmdRun.ProductParam.nImageNum)
	{
		theApp.m_tSysParam.BmpSlotROI = m_picture.RoiRect;
	}
	UpdateUI();
	theApp.BmpParam(FALSE);
}

void CDlgSetMark::OnBnClickedBtnSetMarkSaveModel()
{
	// TODO: Add your control notification handler code here
	CString strFile;
	OnBnClickedBtnSetMarkNewpic();
	nSelectID = m_cbSelect.GetCurSel();
	strFile = g_pDoc->GetModeFile(nSelectID);
	CString str;
	GetDlgItemText(IDC_EDIT_SET_MARK_LEARN_WIN_LEFT,str);
	LearnWin.left = atoi(str);
	GetDlgItemText(IDC_EDIT_SET_MARK_LEARN_WIN_TOP,str);
	LearnWin.top = atoi(str);
	GetDlgItemText(IDC_EDIT_SET_MARK_LEARN_WIN_RIGHT,str);
	LearnWin.right = atoi(str);
	GetDlgItemText(IDC_EDIT_SET_MARK_LEARN_WIN_BOTTOM,str);
	LearnWin.bottom = atoi(str);
	if(nSelectID<g_pFrm->m_CmdRun.ProductParam.nImageNum)
	{
		theApp.m_tSysParam.BmpMarkLearnWin[nSelectID] = LearnWin;
	}
	else if(nSelectID==g_pFrm->m_CmdRun.ProductParam.nImageNum)
	{
		theApp.m_tSysParam.BmpSlotLearnWin = LearnWin;
	}
	milApp.LearnModel(LearnWin);
	milApp.SaveModel(strFile.GetBuffer(strFile.GetLength()));
}

void CDlgSetMark::OnBnClickedBtnSetMarkSearch()
{
	// TODO: Add your control notification handler code here
	OnBnClickedBtnSetMarkNewpic();
	CString str;
	GetDlgItemText(IDC_EDIT_SET_MARK_ROI_LEFT,str);
	ROI.left = atoi(str);
	GetDlgItemText(IDC_EDIT_SET_MARK_ROI_TOP,str);
	ROI.top = atoi(str);
	GetDlgItemText(IDC_EDIT_SET_MARK_ROI_RIGHT,str);
	ROI.right = atoi(str);
	GetDlgItemText(IDC_EDIT_SET_MARK_ROI_BOTTOM,str);
	ROI.bottom = atoi(str);
	FindModelCenter(ROI);
}

void CDlgSetMark::OnBnClickedBtnSetMarkMoveCenter()
{
	// TODO: Add your control notification handler code here
	OnBnClickedBtnSetMarkSearch();
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

void CDlgSetMark::OnBnClickedBtnSetMarkClear()
{
	// TODO: Add your control notification handler code here
	this->Invalidate();
}

void CDlgSetMark::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if(!g_pFrm->m_CmdRun.MoveToZSafety()) //first lift all Z axises;
	{
		return;
	}
	SaveParam();
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,FALSE);
	OnClose();
	OnOK();
}

void CDlgSetMark::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,FALSE);
	if(!g_pFrm->m_CmdRun.MoveToZSafety()) //first lift all Z axises;
	{
		return;
	}
	OnCancel();
}

void CDlgSetMark::RefreshPosition()
{
	CString str;
	double dTempPosition;
	for(int i=0;i<3;i++)
	{
		dTempPosition = theApp.m_Mv400.GetPos(K_AXIS_X+i);
		str.Format("%.3f",dTempPosition);
		SetDlgItemText(IDC_EDIT_SET_MARK_CURRENT_X+i,str);
	}
}

bool CDlgSetMark::FindModelCenter(CRect ROI)
{
	double dAcceptance=50.0,dFindAngle=10.0;
	if(nSelectID<g_pFrm->m_CmdRun.ProductParam.nImageNum)
	{
		dAcceptance = theApp.m_tSysParam.BmpMarkAcceptance;
		dFindAngle = theApp.m_tSysParam.BmpMarkAngle;
	}
	else if(nSelectID==g_pFrm->m_CmdRun.ProductParam.nImageNum)
	{
		dAcceptance = theApp.m_tSysParam.BmpSlotAcceptance;
		dFindAngle = theApp.m_tSysParam.BmpSlotAngle;
	}
	//milApp.SetMatchMode(g_pFrm->m_CmdRun.ProductParam.matchMode);
	milApp.SetPatParam(dAcceptance,dFindAngle);
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
	SetDlgItemText(IDC_EDIT_SET_MARK_RESULT_CENTER_X,str);
	str.Format("%.3f",dCenterY);
	SetDlgItemText(IDC_EDIT_SET_MARK_RESULT_CENTER_Y,str);
	str.Format("%.3f",dAngle);
	SetDlgItemText(IDC_EDIT_SET_MARK_RESULT_ANGLE,str);
	str.Format("%.3f",dScore);
	SetDlgItemText(IDC_EDIT_SET_MARK_RESULT_SCORE,str);

	int left = int(dCenterX-(LearnWin.Width()-1)/2);
	int top = int(dCenterY-(LearnWin.Height()-1)/2);
	int width = LearnWin.Width();
	int height = LearnWin.Height();
	GetDC()->Draw3dRect(left,top,width,height,RGB(255,0,0),RGB(255,0,0));
	return true;
}

void CDlgSetMark::SaveParam()
{
	CString str;
	//GetDlgItemText(IDC_EDIT_MARK_SCALE,str);
	//theApp.m_tSysParam.BmpMarkScale = atof(str);
	if(nSelectID<g_pFrm->m_CmdRun.ProductParam.nImageNum)
	{
		GetDlgItemText(IDC_EDIT_SET_MARK_ANGLE,str);
		theApp.m_tSysParam.BmpMarkAngle = atof(str);
		GetDlgItemText(IDC_EDIT_SET_MARK_ACCEPTANCE,str);
		theApp.m_tSysParam.BmpMarkAcceptance = atof(str);

		GetDlgItemText(IDC_EDIT_SET_MARK_ROI_LEFT,str);
		theApp.m_tSysParam.BmpMarkROI.left = atol(str);
		GetDlgItemText(IDC_EDIT_SET_MARK_ROI_RIGHT,str);
		theApp.m_tSysParam.BmpMarkROI.right = atol(str);
		GetDlgItemText(IDC_EDIT_SET_MARK_ROI_TOP,str);
		theApp.m_tSysParam.BmpMarkROI.top = atol(str);
		GetDlgItemText(IDC_EDIT_SET_MARK_ROI_BOTTOM,str);
		theApp.m_tSysParam.BmpMarkROI.bottom = atol(str);

		GetDlgItemText(IDC_EDIT_SET_MARK_LEARN_WIN_LEFT,str);
		theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].left = atol(str);
		GetDlgItemText(IDC_EDIT_SET_MARK_LEARN_WIN_RIGHT,str);
		theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].right = atol(str);
		GetDlgItemText(IDC_EDIT_SET_MARK_LEARN_WIN_TOP,str);
		theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].top = atol(str);
		GetDlgItemText(IDC_EDIT_SET_MARK_LEARN_WIN_BOTTOM,str);
		theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].bottom = atol(str);

		GetDlgItemText(IDC_EDIT_SET_MARK_CALIBRATION_A,str);
		theApp.m_tSysParam.BmpMarkCalibration.dCalibrationA = atof(str);
		GetDlgItemText(IDC_EDIT_SET_MARK_CALIBRATION_B,str);
		theApp.m_tSysParam.BmpMarkCalibration.dCalibrationB = atof(str);
		GetDlgItemText(IDC_EDIT_SET_MARK_CALIBRATION_C,str);
		theApp.m_tSysParam.BmpMarkCalibration.dCalibrationC = atof(str);
		GetDlgItemText(IDC_EDIT_SET_MARK_CALIBRATION_D,str);
		theApp.m_tSysParam.BmpMarkCalibration.dCalibrationD = atof(str);
	}
	else if(nSelectID==g_pFrm->m_CmdRun.ProductParam.nImageNum)
	{
		GetDlgItemText(IDC_EDIT_SET_MARK_ANGLE,str);
		theApp.m_tSysParam.BmpSlotAngle = atof(str);
		GetDlgItemText(IDC_EDIT_SET_MARK_ACCEPTANCE,str);
		theApp.m_tSysParam.BmpSlotAcceptance = atof(str);

		GetDlgItemText(IDC_EDIT_SET_MARK_ROI_LEFT,str);
		theApp.m_tSysParam.BmpSlotROI.left = atol(str);
		GetDlgItemText(IDC_EDIT_SET_MARK_ROI_RIGHT,str);
		theApp.m_tSysParam.BmpSlotROI.right = atol(str);
		GetDlgItemText(IDC_EDIT_SET_MARK_ROI_TOP,str);
		theApp.m_tSysParam.BmpSlotROI.top = atol(str);
		GetDlgItemText(IDC_EDIT_SET_MARK_ROI_BOTTOM,str);
		theApp.m_tSysParam.BmpSlotROI.bottom = atol(str);

		GetDlgItemText(IDC_EDIT_SET_MARK_LEARN_WIN_LEFT,str);
		theApp.m_tSysParam.BmpSlotLearnWin.left = atol(str);
		GetDlgItemText(IDC_EDIT_SET_MARK_LEARN_WIN_RIGHT,str);
		theApp.m_tSysParam.BmpSlotLearnWin.right = atol(str);
		GetDlgItemText(IDC_EDIT_SET_MARK_LEARN_WIN_TOP,str);
		theApp.m_tSysParam.BmpSlotLearnWin.top = atol(str);
		GetDlgItemText(IDC_EDIT_SET_MARK_LEARN_WIN_BOTTOM,str);
		theApp.m_tSysParam.BmpSlotLearnWin.bottom = atol(str);

		GetDlgItemText(IDC_EDIT_SET_MARK_CALIBRATION_A,str);
		theApp.m_tSysParam.BmpSlotCalibration.dCalibrationA = atof(str);
		GetDlgItemText(IDC_EDIT_SET_MARK_CALIBRATION_B,str);
		theApp.m_tSysParam.BmpSlotCalibration.dCalibrationB = atof(str);
		GetDlgItemText(IDC_EDIT_SET_MARK_CALIBRATION_C,str);
		theApp.m_tSysParam.BmpSlotCalibration.dCalibrationC = atof(str);
		GetDlgItemText(IDC_EDIT_SET_MARK_CALIBRATION_D,str);
		theApp.m_tSysParam.BmpSlotCalibration.dCalibrationD = atof(str);
	}
	theApp.BmpParam(FALSE);
}

void CDlgSetMark::OnCbnSelchangeComboSetMarkSelect()
{
	// TODO: Add your control notification handler code here
	nSelectID = m_cbSelect.GetCurSel();
	UpdateUI();
}

void CDlgSetMark::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nValue = 0;
	CString str;
	if(pScrollBar == (CScrollBar *)&m_sliderShutter)
	{
		nValue = m_sliderShutter.GetPos();
		g_pView->m_ImgStatic.m_pCamera.SetExposureTime((long)nValue);
		theApp.m_tSysParam.CameraExposureTime = nValue;
		str.Format("%d",nValue);
		SetDlgItemText(IDC_EDIT_SET_MARK_EXPOSURE_TIME,str);
	}
	else if(pScrollBar == (CScrollBar *)&m_sliderGain)
	{
		nValue = m_sliderGain.GetPos();
		g_pView->m_ImgStatic.m_pCamera.SetGain((long)nValue);
		theApp.m_tSysParam.CameraGain = nValue;
		str.Format("%d",nValue);
		SetDlgItemText(IDC_EDIT_SET_MARK_GAIN,str);
	}
	theApp.BmpParam(FALSE);
}

void CDlgSetMark::OnKillfocusEditSetMarkExposureTime()
{
	int nValue = 0;
	CString str;
	GetDlgItemText(IDC_EDIT_SET_MARK_EXPOSURE_TIME,str);
	nValue = atoi(str);
	m_sliderShutter.SetPos(nValue);
	g_pView->m_ImgStatic.m_pCamera.SetExposureTime((long)nValue);
	theApp.m_tSysParam.CameraExposureTime = nValue;
	theApp.BmpParam(FALSE);
}

void CDlgSetMark::OnKillfocusEditSetMarkGain()
{
	int nValue = 0;
	CString str;
	GetDlgItemText(IDC_EDIT_SET_MARK_GAIN,str);
	nValue = atoi(str);
	m_sliderGain.SetPos(nValue);
	g_pView->m_ImgStatic.m_pCamera.SetGain((long)nValue);
	theApp.m_tSysParam.CameraGain = nValue;
	theApp.BmpParam(FALSE);
}

void CDlgSetMark::InitUI()
{
	CString str;
	m_sliderShutter.SetRange(0,999);
	m_sliderGain.SetRange(0,63);
	m_sliderShutter.SetPos(theApp.m_tSysParam.CameraExposureTime);
	m_sliderGain.SetPos(theApp.m_tSysParam.CameraGain);
	str.Format("%d",theApp.m_tSysParam.CameraExposureTime);
	SetDlgItemText(IDC_EDIT_SET_MARK_EXPOSURE_TIME,str);
	str.Format("%d",theApp.m_tSysParam.CameraGain);
	SetDlgItemText(IDC_EDIT_SET_MARK_GAIN,str);
}

void CDlgSetMark::UpdateUI()
{
	if(nSelectID<0||nSelectID>g_pFrm->m_CmdRun.ProductParam.nImageNum)
	{
		return;
	}

	CString str;

	if(nSelectID<g_pFrm->m_CmdRun.ProductParam.nImageNum)
	{
		str.Format("%.3f",g_pFrm->m_CmdRun.ProductParam.tgImagePos[nSelectID].x);
		SetDlgItemText(IDC_EDIT_SET_MARK_IMAGE_POS_X,str);
		str.Format("%.3f",g_pFrm->m_CmdRun.ProductParam.tgImagePos[nSelectID].y);
		SetDlgItemText(IDC_EDIT_SET_MARK_IMAGE_POS_Y,str);
		str.Format("%.3f",g_pFrm->m_CmdRun.ProductParam.tgImagePos[nSelectID].za);
		SetDlgItemText(IDC_EDIT_SET_MARK_IMAGE_POS_Z,str);

		//str.Format("%.3f",theApp.m_tSysParam.BmpMarkScale);
		//SetDlgItemText(IDC_EDIT_MARK_SCALE,str);
		str.Format("%.3f",theApp.m_tSysParam.BmpMarkAcceptance);
		SetDlgItemText(IDC_EDIT_SET_MARK_ACCEPTANCE,str);
		str.Format("%.1f",theApp.m_tSysParam.BmpMarkAngle);
		SetDlgItemText(IDC_EDIT_SET_MARK_ANGLE,str);

		str.Format("%0.5f",theApp.m_tSysParam.BmpMarkCalibration.dCalibrationA);
		SetDlgItemText(IDC_EDIT_SET_MARK_CALIBRATION_A,str);
		str.Format("%0.5f",theApp.m_tSysParam.BmpMarkCalibration.dCalibrationB);
		SetDlgItemText(IDC_EDIT_SET_MARK_CALIBRATION_B,str);
		str.Format("%0.5f",theApp.m_tSysParam.BmpMarkCalibration.dCalibrationC);
		SetDlgItemText(IDC_EDIT_SET_MARK_CALIBRATION_C,str);
		str.Format("%0.5f",theApp.m_tSysParam.BmpMarkCalibration.dCalibrationD);
		SetDlgItemText(IDC_EDIT_SET_MARK_CALIBRATION_D,str);

		str.Format("%d",theApp.m_tSysParam.BmpMarkROI.TopLeft().x);
		SetDlgItemText(IDC_EDIT_SET_MARK_ROI_LEFT,str);
		str.Format("%d",theApp.m_tSysParam.BmpMarkROI.TopLeft().y);
		SetDlgItemText(IDC_EDIT_SET_MARK_ROI_TOP,str);
		str.Format("%d",theApp.m_tSysParam.BmpMarkROI.BottomRight().x);
		SetDlgItemText(IDC_EDIT_SET_MARK_ROI_RIGHT,str);
		str.Format("%d",theApp.m_tSysParam.BmpMarkROI.BottomRight().y);
		SetDlgItemText(IDC_EDIT_SET_MARK_ROI_BOTTOM,str);

		str.Format("%d",theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].TopLeft().x);
		SetDlgItemText(IDC_EDIT_SET_MARK_LEARN_WIN_LEFT,str);
		str.Format("%d",theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].TopLeft().y);
		SetDlgItemText(IDC_EDIT_SET_MARK_LEARN_WIN_TOP,str);
		str.Format("%d",theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].BottomRight().x);
		SetDlgItemText(IDC_EDIT_SET_MARK_LEARN_WIN_RIGHT,str);
		str.Format("%d",theApp.m_tSysParam.BmpMarkLearnWin[nSelectID].BottomRight().y);
		SetDlgItemText(IDC_EDIT_SET_MARK_LEARN_WIN_BOTTOM,str);

		ROI = theApp.m_tSysParam.BmpMarkROI;
		LearnWin = theApp.m_tSysParam.BmpMarkLearnWin[nSelectID];
	}
	else if(nSelectID==g_pFrm->m_CmdRun.ProductParam.nImageNum)
	{
		str.Format("%.3f",g_pFrm->m_CmdRun.ProductParam.tSlotDetectPos.x);
		SetDlgItemText(IDC_EDIT_SET_MARK_IMAGE_POS_X,str);
		str.Format("%.3f",g_pFrm->m_CmdRun.ProductParam.tSlotDetectPos.y);
		SetDlgItemText(IDC_EDIT_SET_MARK_IMAGE_POS_Y,str);

		//str.Format("%.3f",theApp.m_tSysParam.BmpMarkScale);
		//SetDlgItemText(IDC_EDIT_MARK_SCALE,str);
		str.Format("%.3f",theApp.m_tSysParam.BmpSlotAcceptance);
		SetDlgItemText(IDC_EDIT_SET_MARK_ACCEPTANCE,str);
		str.Format("%.1f",theApp.m_tSysParam.BmpSlotAngle);
		SetDlgItemText(IDC_EDIT_SET_MARK_ANGLE,str);

		str.Format("%0.5f",theApp.m_tSysParam.BmpSlotCalibration.dCalibrationA);
		SetDlgItemText(IDC_EDIT_SET_MARK_CALIBRATION_A,str);
		str.Format("%0.5f",theApp.m_tSysParam.BmpSlotCalibration.dCalibrationB);
		SetDlgItemText(IDC_EDIT_SET_MARK_CALIBRATION_B,str);
		str.Format("%0.5f",theApp.m_tSysParam.BmpSlotCalibration.dCalibrationC);
		SetDlgItemText(IDC_EDIT_SET_MARK_CALIBRATION_C,str);
		str.Format("%0.5f",theApp.m_tSysParam.BmpSlotCalibration.dCalibrationD);
		SetDlgItemText(IDC_EDIT_SET_MARK_CALIBRATION_D,str);

		str.Format("%d",theApp.m_tSysParam.BmpSlotROI.TopLeft().x);
		SetDlgItemText(IDC_EDIT_SET_MARK_ROI_LEFT,str);
		str.Format("%d",theApp.m_tSysParam.BmpSlotROI.TopLeft().y);
		SetDlgItemText(IDC_EDIT_SET_MARK_ROI_TOP,str);
		str.Format("%d",theApp.m_tSysParam.BmpSlotROI.BottomRight().x);
		SetDlgItemText(IDC_EDIT_SET_MARK_ROI_RIGHT,str);
		str.Format("%d",theApp.m_tSysParam.BmpSlotROI.BottomRight().y);
		SetDlgItemText(IDC_EDIT_SET_MARK_ROI_BOTTOM,str);

		str.Format("%d",theApp.m_tSysParam.BmpSlotLearnWin.TopLeft().x);
		SetDlgItemText(IDC_EDIT_SET_MARK_LEARN_WIN_LEFT,str);
		str.Format("%d",theApp.m_tSysParam.BmpSlotLearnWin.TopLeft().y);
		SetDlgItemText(IDC_EDIT_SET_MARK_LEARN_WIN_TOP,str);
		str.Format("%d",theApp.m_tSysParam.BmpSlotLearnWin.BottomRight().x);
		SetDlgItemText(IDC_EDIT_SET_MARK_LEARN_WIN_RIGHT,str);
		str.Format("%d",theApp.m_tSysParam.BmpSlotLearnWin.BottomRight().y);
		SetDlgItemText(IDC_EDIT_SET_MARK_LEARN_WIN_BOTTOM,str);

		ROI = theApp.m_tSysParam.BmpSlotROI;
		LearnWin = theApp.m_tSysParam.BmpSlotLearnWin;
	}

	//if(g_pView->m_ImgStatic.m_pCamera.m_pImageBuffer!=NULL)
	{
		m_picture.m_modtype = MOD_RECTANGLE;
		m_picture.m_iImgWidth = IMAGEWIDTH0;
		m_picture.m_iImgHeight = IMAGEHEIGHT0;
		m_picture.m_pImgBuffer = new BYTE[m_picture.m_iImgWidth*m_picture.m_iImgHeight*3];
		m_picture.m_mod_rectangle.dx = LearnWin.TopLeft().x + LearnWin.Width()/2;
		m_picture.m_mod_rectangle.dy = LearnWin.TopLeft().y + LearnWin.Height()/2;
		m_picture.m_mod_rectangle.width = LearnWin.Width();
		m_picture.m_mod_rectangle.height = LearnWin.Height();
		m_picture.m_mod_scale.length = 20;
		m_picture.m_mod_scale.width = 10;
		m_picture.m_mod_scale.height = 10;
		m_picture.RoiRect =  ROI;
		m_picture.SetDisplayMarkImage(true);
		m_picture.ImgInit(IMAGEWIDTH0,IMAGEHEIGHT0,24);
	}

	dScore = 0.0;
	dAngle = 0.0;
	dCenterX = 0.0;
	dCenterY = 0.0;

	str.Format("%.1f",dScore);
	SetDlgItemText(IDC_EDIT_SET_MARK_RESULT_SCORE,str);
	str.Format("%.1f",dAngle);
	SetDlgItemText(IDC_EDIT_SET_MARK_RESULT_ANGLE,str);
	str.Format("%.3f",dCenterX);
	SetDlgItemText(IDC_EDIT_SET_MARK_RESULT_CENTER_X,str);
	str.Format("%.3f",dCenterY);
	SetDlgItemText(IDC_EDIT_SET_MARK_RESULT_CENTER_Y,str);

	CString strFile = g_pDoc->GetModeFile(nSelectID);
	//milApp.SetMatchMode(g_pFrm->m_CmdRun.ProductParam.matchMode);
	milApp.SetModelWindow(LearnWin);
	milApp.SetSearchWindow(ROI);
	milApp.RestoreModel(strFile.GetBuffer(strFile.GetLength()));
}
void CDlgSetMark::OnBnClickedBtnSetMarkTest()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.RunThreadTestImage();
}

void CDlgSetMark::OnBnClickedBtnSetMarkSaveCenter()
{
	if(nSelectID<0||nSelectID>g_pFrm->m_CmdRun.ProductParam.nImageNum)
	{
		return;
	}
	double posX,posY;
	posX=theApp.m_Mv400.GetPos(K_AXIS_X);
	posY=theApp.m_Mv400.GetPos(K_AXIS_Y);
	g_pFrm->m_CmdRun.ProductParam.tgImagePos[nSelectID].x=posX;
	g_pFrm->m_CmdRun.ProductParam.tgImagePos[nSelectID].y=posY;
	theApp.ProductParam(FALSE);	
}
