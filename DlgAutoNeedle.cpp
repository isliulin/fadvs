// CDlgAutoNeedle.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgAutoNeedle.h"
#include <math.h>


// CDlgAutoNeedle dialog

IMPLEMENT_DYNAMIC(CDlgAutoNeedle, CDialog)

CDlgAutoNeedle::CDlgAutoNeedle(CWnd* pParent /*=NULL*/)
: CDialog(CDlgAutoNeedle::IDD, pParent)
{
	m_dScore = 0;
	m_dAngle = 0;
}

CDlgAutoNeedle::~CDlgAutoNeedle()
{
}

void CDlgAutoNeedle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_STATIC_AUTO_NEEDLE_PICTURE,m_picture);
}


BEGIN_MESSAGE_MAP(CDlgAutoNeedle, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_AUTO_NEEDLE_SAVE_POS, &CDlgAutoNeedle::OnBnClickedBtnAutoNeedleSavePos)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_AUTO_NEEDLE_SPEED, &CDlgAutoNeedle::OnNMCustomdrawSliderAutoNeedleSpeed)
	ON_BN_CLICKED(IDC_BTN_AUTO_NEEDLE_MOVE_POS, &CDlgAutoNeedle::OnBnClickedBtnAutoNeedleMovePos)
	ON_BN_CLICKED(IDOK, &CDlgAutoNeedle::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_AUTO_NEEDLE_GRAY, &CDlgAutoNeedle::OnBnClickedRadioAutoNeedleGray)
	ON_BN_CLICKED(IDC_RADIO_AUTO_NEEDLE_BLOB, &CDlgAutoNeedle::OnBnClickedRadioAutoNeedleBlob)
	ON_BN_CLICKED(IDCANCEL, &CDlgAutoNeedle::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_AUTO_NEEDLE_ADJUST_NEEDLE_ONE, &CDlgAutoNeedle::OnBnClickedBtnAutoNeedleAdjustNeedleOne)
	ON_BN_CLICKED(IDC_BTN_AUTO_NEEDLE_ADJUST_NEEDLE_TWO, &CDlgAutoNeedle::OnBnClickedBtnAutoNeedleAdjustNeedleTwo)
	ON_BN_CLICKED(IDC_BTN_AUTO_NEEDLE_ADJUST_NEEDLE_THREE, &CDlgAutoNeedle::OnBnClickedBtnAutoNeedleAdjustNeedleThree)
	ON_BN_CLICKED(IDC_BTN_AUTO_NEEDLE_ADJUST_NEEDLE, &CDlgAutoNeedle::OnBnClickedBtnAutoNeedleAdjustNeedle)
	ON_BN_CLICKED(IDC_BTN_AUTO_NEEDLE_NEEDLE_ONE, &CDlgAutoNeedle::OnBnClickedBtnAutoNeedleNeedleOne)
	ON_BN_CLICKED(IDC_BTN_AUTO_NEEDLE_NEEDLE_TWO, &CDlgAutoNeedle::OnBnClickedBtnAutoNeedleNeedleTwo)
	ON_BN_CLICKED(IDC_BTN_AUTO_NEEDLE_NEEDLE_THREE, &CDlgAutoNeedle::OnBnClickedBtnAutoNeedleNeedleThree)
	ON_BN_CLICKED(IDC_BTN_AUTO_NEEDLE_CALIB, &CDlgAutoNeedle::OnBnClickedBtnAutoNeedleCalib)
	ON_BN_CLICKED(IDC_BTN_AUTO_NEEDLE_SAVE_PARAME, &CDlgAutoNeedle::OnBnClickedBtnAutoNeedleSaveParame)
	ON_BN_CLICKED(IDC_BTN_AUTO_NEEDLE_RESET, &CDlgAutoNeedle::OnBnClickedBtnAutoNeedleReset)
	ON_BN_CLICKED(IDC_BTN_AUTO_NEEDLE_SAVE_ROI, &CDlgAutoNeedle::OnBnClickedBtnAutoNeedleSaveRoi)
	ON_BN_CLICKED(IDC_BTN_AUTO_NEEDLE_SAVE_MODEL_WIN, &CDlgAutoNeedle::OnBnClickedBtnAutoNeedleSaveModelWin)
	ON_BN_CLICKED(IDC_BTN_AUTO_NEEDLE_SAVE_MODEL, &CDlgAutoNeedle::OnBnClickedBtnAutoNeedleSaveModel)
	ON_BN_CLICKED(IDC_BTN_AUTO_NEEDLE_SEARCH, &CDlgAutoNeedle::OnBnClickedBtnAutoNeedleSearch)
END_MESSAGE_MAP()

BOOL CDlgAutoNeedle::OnInitDialog()
{
	CDialog::OnInitDialog();
	CheckRadioButton(IDC_RADIO_AUTO_NEEDLE_GRAY,IDC_RADIO_AUTO_NEEDLE_BLOB,IDC_RADIO_AUTO_NEEDLE_GRAY+(int)g_pFrm->m_CmdRun.ProductParam.searchMode);
	for(int i=0;i<10;i++)
	{
		m_colorButton[i].m_iIndex = i;
		m_colorButton[i].SubclassDlgItem(IDC_BTN_AUTO_NEEDLE_X_NEG+i,this);
		m_colorButton[i].SetBitmapEx(IDB_BITMAP_MOVE);
		m_colorButton[i].m_pMv = &theApp.m_Mv400;
	}
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_AUTO_NEEDLE_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_AUTO_NEEDLE_SPEED))->SetPos(500);
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_AUTO_NEEDLE_SPEED))->GetPos();
	CColorButton::m_iSpeedNo = n;
    
	//the display in image;
	m_picture.SetDisplayMarkImage(true);
	m_picture.m_modtype = MOD_RECTANGLE;
	m_picture.m_iImgWidth = IMAGEWIDTH0;
	m_picture.m_iImgHeight = IMAGEHEIGHT0;
	m_picture.m_pImgBuffer = new BYTE[m_picture.m_iImgWidth*m_picture.m_iImgHeight*3];
	m_picture.m_mod_rectangle.dx = theApp.m_tSysParam.AutoNeedleModelWin.TopLeft().x + theApp.m_tSysParam.AutoNeedleModelWin.Width()/2;
	m_picture.m_mod_rectangle.dy = theApp.m_tSysParam.AutoNeedleModelWin.TopLeft().y + theApp.m_tSysParam.AutoNeedleModelWin.Height()/2;
	m_picture.m_mod_rectangle.width = theApp.m_tSysParam.AutoNeedleModelWin.Width();
	m_picture.m_mod_rectangle.height = theApp.m_tSysParam.AutoNeedleModelWin.Height();
	m_picture.m_mod_scale.length = 20;
	m_picture.m_mod_scale.width = 10;
	m_picture.m_mod_scale.height = 10;
	m_picture.RoiRect =  theApp.m_tSysParam.AutoNeedleROI;
	m_picture.ImgInit(IMAGEWIDTH0,IMAGEHEIGHT0,24);
    //the rect in camera;
	milApp.SetModelWindow(theApp.m_tSysParam.AutoNeedleModelWin);
	milApp.SetSearchWindow(theApp.m_tSysParam.AutoNeedleROI);
	CString strFile = g_pDoc->GetModeFile(7);
	milApp.RestoreModel(strFile.GetBuffer(strFile.GetLength()));

	CString str;
	str.Format("%0.1f",m_dScore);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_RESULT_SCORE,str);
	str.Format("%0.1f",m_dAngle);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_RESULT_ANGLE,str);
	str.Format("%0.3f",m_tgCenter.x);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_RESULT_CENTERX,str);
	str.Format("%0.3f",m_tgCenter.y);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_RESULT_CENTERY,str);
	UpdateUI();
	SetTimer(0,100,NULL);
	//mechanical initial status;
	g_pFrm->m_CmdRun.MoveToZSafety();
	g_pFrm->m_CmdRun.AdjustNeedle();
	return TRUE;
}

void CDlgAutoNeedle::OnTimer(UINT nIDEvent)
{
	CurrentPos();
	CDialog::OnTimer(nIDEvent);
}
//save the orgion osition;
void CDlgAutoNeedle::OnBnClickedBtnAutoNeedleSavePos()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能保存坐标!");
		return;
	}
	if(AfxMessageBox("保存新坐标?",MB_YESNO)!=IDYES)
	{
		return;
	}
	for(int i=0;i<5;i++)
	{
		if(i<3)
		{
			g_pFrm->m_CmdRun.ProductParam.NeedlePos[i] = theApp.m_Mv400.GetPos(i+1);
		}
		else
		{
			g_pFrm->m_CmdRun.ProductParam.NeedlePos[i] = g_pFrm->m_CmdRun.ProductParam.NeedlePos[2] +
				g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[i] - g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2];
		}
	}
	g_pFrm->m_CmdRun.MoveToZSafety();
	theApp.ProductParam(FALSE);
}

void CDlgAutoNeedle::OnNMCustomdrawSliderAutoNeedleSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_AUTO_NEEDLE_SPEED))->GetPos();
	CColorButton::m_iSpeedNo = n;
	CString strTemp;
	strTemp.Format("X:%.3f,Y:%.3f,ZA:%.3f,ZB:%.3f,ZC:%.3f", theApp.m_tSysParam.tAxis[0].vel[0] * n/2000.0,
		theApp.m_tSysParam.tAxis[1].vel[0] * n/2000.0,theApp.m_tSysParam.tAxis[2].vel[0] * n/2000.0,
		theApp.m_tSysParam.tAxis[3].vel[0] * n/2000.0,theApp.m_tSysParam.tAxis[4].vel[0] * n/2000.0);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_SPEED, strTemp);
	*pResult = 0;
}
//To the position ;
void CDlgAutoNeedle::OnBnClickedBtnAutoNeedleMovePos()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能移动!");
		return;
	}
	if(AfxMessageBox("移动到自动对针坐标?",MB_YESNO)!=IDYES)
	{
		return;
	}
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.NeedlePos[0],g_pFrm->m_CmdRun.ProductParam.NeedlePos[1],true))
	{
		AfxMessageBox("移动XY轴失败！");
		return;
	}
	if(!g_pFrm->m_CmdRun.SynchronizeMoveZ(g_pFrm->m_CmdRun.ProductParam.NeedlePos[2],g_pFrm->m_CmdRun.ProductParam.NeedleVel,
		g_pFrm->m_CmdRun.ProductParam.NeedleAcc,true))
	{
		AfxMessageBox("移动ZA轴失败！");
		return;
	}
	if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
	{
		AfxMessageBox("设置坐标轴速度失败！");
		return;
	}
}

void CDlgAutoNeedle::UpdateUI()
{
	CString str;
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.NeedlePos[0]);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_START_POS_X,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.NeedlePos[1]);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_START_POS_Y,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.NeedlePos[2]);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_START_POS_ZA,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.NeedlePos[3]);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_START_POS_ZB,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.NeedlePos[4]);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_START_POS_ZC,str);

	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.NeedleGap);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_PITCH,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.NeedleVel);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_VEL,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.NeedleAcc);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_ACC,str);
	
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dAutoNeedleGlueVol);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_GLUE,str);
	GetDlgItem(IDC_EDIT_AUTO_NEEDLE_GLUE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dAutoNeedleErrorX);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_ERROR_X,str);
	GetDlgItem(IDC_EDIT_AUTO_NEEDLE_ERROR_X)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dAutoNeedleErrorY);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_ERROR_Y,str);
	GetDlgItem(IDC_EDIT_AUTO_NEEDLE_ERROR_Y)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	
	str.Format("%0.1f",theApp.m_tSysParam.dAutoNeedleAcceptance);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_ACCEPTANCE,str);
	GetDlgItem(IDC_EDIT_AUTO_NEEDLE_ACCEPTANCE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	
	str.Format("%0.1f",theApp.m_tSysParam.dAutoNeedleAngle);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_ANGLE,str);
	GetDlgItem(IDC_EDIT_AUTO_NEEDLE_ANGLE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.nAutoAdjustCount);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_ADJUST_COUNT,str);
	GetDlgItem(IDC_EDIT_AUTO_NEEDLE_ADJUST_COUNT)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);

	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.dPitchX);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_STEP_X_PITCH,str);
	GetDlgItem(IDC_EDIT_AUTO_NEEDLE_STEP_X_PITCH)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.dPitchY);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_STEP_Y_PITCH,str);
	GetDlgItem(IDC_EDIT_AUTO_NEEDLE_STEP_Y_PITCH)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCountX);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_STEP_X_COUNT,str);
	GetDlgItem(IDC_EDIT_AUTO_NEEDLE_STEP_X_COUNT)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCountY);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_STEP_Y_COUNT,str);
	GetDlgItem(IDC_EDIT_AUTO_NEEDLE_STEP_Y_COUNT)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);

	str.Format("%0.3f",theApp.m_tSysParam.AutoNeedleCalib.dCalibrationA);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_CALIB_A,str);
	GetDlgItem(IDC_EDIT_AUTO_NEEDLE_CALIB_A)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	
	str.Format("%0.3f",theApp.m_tSysParam.AutoNeedleCalib.dCalibrationB);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_CALIB_B,str);
	GetDlgItem(IDC_EDIT_AUTO_NEEDLE_CALIB_B)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	
	str.Format("%0.3f",theApp.m_tSysParam.AutoNeedleCalib.dCalibrationC);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_CALIB_C,str);
	GetDlgItem(IDC_EDIT_AUTO_NEEDLE_CALIB_C)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	
	str.Format("%0.3f",theApp.m_tSysParam.AutoNeedleCalib.dCalibrationD);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_CALIB_D,str);
	GetDlgItem(IDC_EDIT_AUTO_NEEDLE_CALIB_D)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);

	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.nBlobThreshold);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_THRESHOLD,str);
	GetDlgItem(IDC_EDIT_AUTO_NEEDLE_THRESHOLD)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.nBlobAreaMin);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_AREA_MIN,str);
	GetDlgItem(IDC_EDIT_AUTO_NEEDLE_AREA_MIN)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.nBlobAreaMax);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_AREA_MAX,str);
	GetDlgItem(IDC_EDIT_AUTO_NEEDLE_AREA_MAX)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.nBlobLengthMin);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_LENGTH_MIN,str);
	GetDlgItem(IDC_EDIT_AUTO_NEEDLE_LENGTH_MIN)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.nBlobLengthMax);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_LENGTH_MAX,str);
	GetDlgItem(IDC_EDIT_AUTO_NEEDLE_LENGTH_MAX)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.nBlobHeightMin);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_HEIGHT_MIN,str);
	GetDlgItem(IDC_EDIT_AUTO_NEEDLE_HEIGHT_MIN)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.nBlobHeightMax);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_HEIGHT_MAX,str);
	GetDlgItem(IDC_EDIT_AUTO_NEEDLE_HEIGHT_MAX)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
}
//pattern center and score
void CDlgAutoNeedle::UpdateResult()
{
	CString str;
	str.Format("%.3f",m_tgCenter.x);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_RESULT_CENTERX,str);
	str.Format("%.3f",m_tgCenter.y);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_RESULT_CENTERY,str);
	str.Format("%.3f",m_dScore);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_RESULT_SCORE,str);
}
//current pos of axis;
void CDlgAutoNeedle::CurrentPos()
{
	CString str;
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_X));
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_CURR_POS_X,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_Y));
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_CURR_POS_Y,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_ZA));
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_CURR_POS_ZA,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_ZB));
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_CURR_POS_ZB,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_ZC));
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_CURR_POS_ZC,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_A));
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_CURR_POS_A,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_B));
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_CURR_POS_B,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_C));
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_CURR_POS_C,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_D));
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_CURR_POS_D,str);
}
//encoder pos of axis;
void CDlgAutoNeedle::EncoderPos()
{
	CString str;
	str.Format("%0.3f",theApp.m_Mv400.GetEncPos(K_AXIS_X));
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_ENC_POS_X,str);
	str.Format("%0.3f",theApp.m_Mv400.GetEncPos(K_AXIS_Y));
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_ENC_POS_Y,str);
	str.Format("%0.3f",theApp.m_Mv400.GetEncPos(K_AXIS_ZA));
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_ENC_POS_ZA,str);
	str.Format("%0.3f",theApp.m_Mv400.GetEncPos(K_AXIS_ZB));
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_ENC_POS_ZB,str);
	str.Format("%0.3f",theApp.m_Mv400.GetEncPos(K_AXIS_ZC));
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_ENC_POS_ZC,str);
	str.Format("%0.3f",theApp.m_Mv400.GetEncPos(K_AXIS_A));
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_ENC_POS_A,str);
	str.Format("%0.3f",theApp.m_Mv400.GetEncPos(K_AXIS_B));
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_ENC_POS_B,str);
	str.Format("%0.3f",theApp.m_Mv400.GetEncPos(K_AXIS_C));
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_ENC_POS_C,str);
	str.Format("%0.3f",theApp.m_Mv400.GetEncPos(K_AXIS_D));
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_ENC_POS_D,str);
}
void CDlgAutoNeedle::SaveData()
{
	CString str;
	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_START_POS_X,str);
	g_pFrm->m_CmdRun.ProductParam.NeedlePos[0] = atof(str);
	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_START_POS_Y,str);
	g_pFrm->m_CmdRun.ProductParam.NeedlePos[1] = atof(str);
	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_START_POS_ZA,str);
	g_pFrm->m_CmdRun.ProductParam.NeedlePos[2] = atof(str);
	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_START_POS_ZB,str);
	g_pFrm->m_CmdRun.ProductParam.NeedlePos[3] = atof(str);
	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_START_POS_ZC,str);
	g_pFrm->m_CmdRun.ProductParam.NeedlePos[4] = atof(str);

	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_GLUE,str);
	g_pFrm->m_CmdRun.ProductParam.dAutoNeedleGlueVol = atof(str);
	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_ERROR_X,str);
	g_pFrm->m_CmdRun.ProductParam.dAutoNeedleErrorX = atof(str);
	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_ERROR_Y,str);
	g_pFrm->m_CmdRun.ProductParam.dAutoNeedleErrorY = atof(str);
	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_ADJUST_COUNT,str);
	g_pFrm->m_CmdRun.ProductParam.nAutoAdjustCount = atoi(str);

	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_STEP_X_PITCH,str);
	g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.dPitchX = atof(str);
	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_STEP_Y_PITCH,str);
	g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.dPitchY = atof(str);
	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_STEP_X_COUNT,str);
	g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCountX = atoi(str);
	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_STEP_Y_COUNT,str);
	g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCountY = atoi(str);

	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_THRESHOLD,str);
	g_pFrm->m_CmdRun.ProductParam.nBlobThreshold = atoi(str);
	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_LENGTH_MIN,str);
	g_pFrm->m_CmdRun.ProductParam.nBlobLengthMin = atoi(str);
	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_LENGTH_MAX,str);
	g_pFrm->m_CmdRun.ProductParam.nBlobLengthMax = atoi(str);
	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_AREA_MIN,str);
	g_pFrm->m_CmdRun.ProductParam.nBlobAreaMin = atoi(str);
	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_AREA_MAX,str);
	g_pFrm->m_CmdRun.ProductParam.nBlobAreaMax = atoi(str);
	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_HEIGHT_MIN,str);
	g_pFrm->m_CmdRun.ProductParam.nBlobHeightMin = atoi(str);
	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_HEIGHT_MAX,str);
	g_pFrm->m_CmdRun.ProductParam.nBlobHeightMax = atoi(str);
    //convert parameter:mm and pixsel 
	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_CALIB_A,str);
	theApp.m_tSysParam.AutoNeedleCalib.dCalibrationA = atof(str);
	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_CALIB_B,str);
	theApp.m_tSysParam.AutoNeedleCalib.dCalibrationB = atof(str);
	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_CALIB_C,str);
	theApp.m_tSysParam.AutoNeedleCalib.dCalibrationC = atof(str);
	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_CALIB_D,str);
	theApp.m_tSysParam.AutoNeedleCalib.dCalibrationD = atof(str);
	//pat parameter;
	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_ACCEPTANCE,str);
	theApp.m_tSysParam.dAutoNeedleAcceptance = atof(str);
	GetDlgItemText(IDC_EDIT_AUTO_NEEDLE_ANGLE,str);
	theApp.m_tSysParam.dAutoNeedleAngle = atof(str);
	theApp.ProductParam(FALSE);
	theApp.BmpParam(FALSE);
}
void CDlgAutoNeedle::Close()
{
	g_pFrm->m_CmdRun.MoveToZSafety();
}
void CDlgAutoNeedle::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	SaveData();
	Close();
	OnOK();
}

void CDlgAutoNeedle::OnBnClickedRadioAutoNeedleGray()
{
	// TODO: 在此添加控件通知处理程序代码
	g_pFrm->m_CmdRun.ProductParam.searchMode = GREY_MODE;
}

void CDlgAutoNeedle::OnBnClickedRadioAutoNeedleBlob()
{
	// TODO: 在此添加控件通知处理程序代码
	g_pFrm->m_CmdRun.ProductParam.searchMode = BLOB_MODE;
}

void CDlgAutoNeedle::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	Close();
	OnCancel();
}

bool CDlgAutoNeedle::FindNeedleModel(double &dCenterX,double &dCenterY,double &dScore)
{
	if(!g_pView->m_ImgStatic.CopyImage(RGB8))
	{
		return false;
	}
	milApp.SetPatParam(theApp.m_tSysParam.dAutoNeedleAcceptance,theApp.m_tSysParam.dAutoNeedleAngle);
	milApp.SetSearchWindow(theApp.m_tSysParam.AutoNeedleROI);
	CTimeUtil::Delay(long(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay*1000));
	if(!milApp.FindModel(false))
	{
		return false;
	}
	dCenterX = milApp.m_stResult.dResultCenterX[0];
	dCenterY = milApp.m_stResult.dResultCenterY[0];
	dScore = milApp.m_stResult.dResultScore[0];
	return true;
}
bool CDlgAutoNeedle::FindBlobCenter(double &dCenterX,double &dCenterY)
{
	CString str;
	if(!g_pView->m_ImgStatic.CopyImage(RGB8))
	{
		return false;
	}
	//in default ,the blob is black blob;
	milApp.SetBlobParam(g_pFrm->m_CmdRun.ProductParam.nBlobThreshold,g_pFrm->m_CmdRun.ProductParam.nBlobAreaMin,g_pFrm->m_CmdRun.ProductParam.nBlobAreaMax,1,1,false);
	milApp.FindBlob(theApp.m_tSysParam.AutoNeedleROI);
	int nFindNum = milApp.BlobResult.nBlobNum;
	str.Format("%d",nFindNum);
	g_pFrm->m_CmdRun.AddMsg(str);
	if(nFindNum<=0)
	{
		return false;
	}
	dCenterX = milApp.BlobResult.dBlobCenterX[0];
	dCenterY = milApp.BlobResult.dBlobCenterY[0];
	return true;
}
void CDlgAutoNeedle::OnBnClickedBtnAutoNeedleAdjustNeedleTwo()
{
	// TODO: Add your control notification handler code here
	if(!theApp.m_Mv400.IsInitOK())
	{
		return;
	}
	int nCount = 0;
	if(g_pFrm->m_CmdRun.MoveToAdjustPos(1))
	{
		AfxMessageBox("移动到针头1失败！");
		return;
	}
	Sleep(500);
	if(GREY_MODE == g_pFrm->m_CmdRun.ProductParam.searchMode)
	{
		if(!FindNeedleModel(m_tgCenter.x,m_tgCenter.y,m_dScore))
		{
			AfxMessageBox("查找图像失败！");
		}
		else
		{
			UpdateResult();
			double dX,dY;
			theApp.m_tSysParam.AutoNeedleCalib.ConvertPixelToTable(m_tgCenter.x - (milApp.m_lBufSizeX-1)/2,
				m_tgCenter.y - (milApp.m_lBufSizeY-1)/2,dX,dY);
			while(fabs(dX)>g_pFrm->m_CmdRun.ProductParam.dAutoNeedleErrorX||
				fabs(dY)>g_pFrm->m_CmdRun.ProductParam.dAutoNeedleErrorY)
			{
				if(nCount>=g_pFrm->m_CmdRun.ProductParam.nAutoAdjustCount)
				{
					break;
				}
				nCount++;
				g_pFrm->m_CmdRun.AdjustNeedle(0.0,0.0,dX,dY);
				g_pFrm->m_CmdRun.MoveToAdjustPos(0);
				Sleep(500);
				if(!FindNeedleModel(m_tgCenter.x,m_tgCenter.y,m_dScore))
				{
					AfxMessageBox("查找图像失败！");
					break;
				}
				UpdateResult();
				theApp.m_tSysParam.AutoNeedleCalib.ConvertPixelToTable(m_tgCenter.x - (milApp.m_lBufSizeX-1)/2,
					m_tgCenter.y - (milApp.m_lBufSizeY-1)/2,dX,dY);
			}
			if(nCount>=g_pFrm->m_CmdRun.ProductParam.nAutoAdjustCount)
			{
				AfxMessageBox("调整失败！");
			}
		}
	}
	else if(BLOB_MODE == g_pFrm->m_CmdRun.ProductParam.searchMode)
	{
		if(!FindBlobCenter(m_tgCenter.x,m_tgCenter.y))
		{
			AfxMessageBox("查找图像失败！");
		}
		else
		{
			UpdateResult();
			double dX,dY;
			theApp.m_tSysParam.AutoNeedleCalib.ConvertPixelToTable(m_tgCenter.x - (milApp.m_lBufSizeX-1)/2,
				m_tgCenter.y - (milApp.m_lBufSizeY-1)/2,dX,dY);
			while(fabs(dX)>g_pFrm->m_CmdRun.ProductParam.dAutoNeedleErrorX||
				fabs(dY)>g_pFrm->m_CmdRun.ProductParam.dAutoNeedleErrorY)
			{
				CFunction::DoEvents();
				if(nCount>=g_pFrm->m_CmdRun.ProductParam.nAutoAdjustCount)
				{
					break;
				}
				nCount++;
				g_pFrm->m_CmdRun.AdjustNeedle(dX,dY,0.0,0.0);
				g_pFrm->m_CmdRun.MoveToAdjustPos(1);
				Sleep(500);
				if(!FindBlobCenter(m_tgCenter.x,m_tgCenter.y))
				{
					AfxMessageBox("查找图像失败！");
					break;
				}
				UpdateResult();
				theApp.m_tSysParam.AutoNeedleCalib.ConvertPixelToTable(m_tgCenter.x - (milApp.m_lBufSizeX-1)/2,
					m_tgCenter.y - (milApp.m_lBufSizeY-1)/2,dX,dY);
			}
			if(nCount>=g_pFrm->m_CmdRun.ProductParam.nAutoAdjustCount)
			{
				AfxMessageBox("调整失败！");
			}
		}
	}
}

void CDlgAutoNeedle::OnBnClickedBtnAutoNeedleAdjustNeedleOne()
{
	// TODO: Add your control notification handler code here
	if(!theApp.m_Mv400.IsInitOK())
	{
		return;
	}

	int nCount = 0;

	if(g_pFrm->m_CmdRun.MoveToAdjustPos(0))
	{
		AfxMessageBox("移动到针头1失败！");
		return;
	}
	Sleep(500);
	if(GREY_MODE == g_pFrm->m_CmdRun.ProductParam.searchMode)
	{
		if(!FindNeedleModel(m_tgCenter.x,m_tgCenter.y,m_dScore))
		{
			AfxMessageBox("查找图像失败！");
		}
		else
		{
			UpdateResult();
			double dX,dY;
			theApp.m_tSysParam.AutoNeedleCalib.ConvertPixelToTable(m_tgCenter.x - (milApp.m_lBufSizeX-1)/2,
				m_tgCenter.y - (milApp.m_lBufSizeY-1)/2,dX,dY);
			while(fabs(dX)>g_pFrm->m_CmdRun.ProductParam.dAutoNeedleErrorX||
				fabs(dY)>g_pFrm->m_CmdRun.ProductParam.dAutoNeedleErrorY)
			{
				if(nCount>=g_pFrm->m_CmdRun.ProductParam.nAutoAdjustCount)
				{
					break;
				}
				nCount++;
				g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].x -= dX;
				g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].y -= dY;
				g_pFrm->m_CmdRun.MoveToAdjustPos(1);
				Sleep(500);
				if(!FindNeedleModel(m_tgCenter.x,m_tgCenter.y,m_dScore))
				{
					AfxMessageBox("查找图像失败！");
					break;
				}
				UpdateResult();
				theApp.m_tSysParam.AutoNeedleCalib.ConvertPixelToTable(m_tgCenter.x - (milApp.m_lBufSizeX-1)/2,
					m_tgCenter.y - (milApp.m_lBufSizeY-1)/2,dX,dY);
			}
			if(nCount>=g_pFrm->m_CmdRun.ProductParam.nAutoAdjustCount)
			{
				AfxMessageBox("调整失败！");
			}
		}
	}
	else if(BLOB_MODE == g_pFrm->m_CmdRun.ProductParam.searchMode)
	{
		if(!FindBlobCenter(m_tgCenter.x,m_tgCenter.y))
		{
			AfxMessageBox("查找图像失败！");
		}
		else
		{
			UpdateResult();
			double dX,dY;
			theApp.m_tSysParam.AutoNeedleCalib.ConvertPixelToTable(m_tgCenter.x - (milApp.m_lBufSizeX-1)/2,
				m_tgCenter.y - (milApp.m_lBufSizeY-1)/2,dX,dY);
			while(fabs(dX)>g_pFrm->m_CmdRun.ProductParam.dAutoNeedleErrorX||
				fabs(dY)>g_pFrm->m_CmdRun.ProductParam.dAutoNeedleErrorY)
			{
				CFunction::DoEvents();
				if(nCount>=g_pFrm->m_CmdRun.ProductParam.nAutoAdjustCount)
				{
					break;
				}
				nCount++;
				g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].x -= dX;
				g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].y -= dY;
				g_pFrm->m_CmdRun.MoveToAdjustPos(0);
				Sleep(500);
				if(!FindBlobCenter(m_tgCenter.x,m_tgCenter.y))
				{
					AfxMessageBox("查找图像失败！");
					break;
				}
				UpdateResult();
				theApp.m_tSysParam.AutoNeedleCalib.ConvertPixelToTable(m_tgCenter.x - (milApp.m_lBufSizeX-1)/2,
					m_tgCenter.y - (milApp.m_lBufSizeY-1)/2,dX,dY);
			}
			if(nCount>=g_pFrm->m_CmdRun.ProductParam.nAutoAdjustCount)
			{
				AfxMessageBox("调整失败！");
			}
		}
	}
}
//observe the correction effect;the orientation is ok or not;
void CDlgAutoNeedle::OnBnClickedBtnAutoNeedleAdjustNeedleThree()
{
	// TODO: Add your control notification handler code here
	if(!theApp.m_Mv400.IsInitOK())
	{
		return;
	}
	int nCount = 0;
	if(g_pFrm->m_CmdRun.MoveToAdjustPos(2))
	{
		AfxMessageBox("移动到针头1失败！");
		return;
	}
	Sleep(500);
	if(GREY_MODE == g_pFrm->m_CmdRun.ProductParam.searchMode)
	{
		if(!FindNeedleModel(m_tgCenter.x,m_tgCenter.y,m_dScore))
		{
			AfxMessageBox("查找图像失败！");
		}
		else
		{
			UpdateResult();
			double dX,dY;
			theApp.m_tSysParam.AutoNeedleCalib.ConvertPixelToTable(m_tgCenter.x - (milApp.m_lBufSizeX-1)/2,
				m_tgCenter.y - (milApp.m_lBufSizeY-1)/2,dX,dY);
			while(fabs(dX)>g_pFrm->m_CmdRun.ProductParam.dAutoNeedleErrorX||
				fabs(dY)>g_pFrm->m_CmdRun.ProductParam.dAutoNeedleErrorY)
			{
				if(nCount>=g_pFrm->m_CmdRun.ProductParam.nAutoAdjustCount)
				{
					break;
				}
				nCount++;
				g_pFrm->m_CmdRun.AdjustNeedle(dX,dY,0.0,0.0);
				g_pFrm->m_CmdRun.MoveToAdjustPos(2);
				Sleep(500);
				if(!FindNeedleModel(m_tgCenter.x,m_tgCenter.y,m_dScore))
				{
					AfxMessageBox("查找图像失败！");
					break;
				}
				UpdateResult();
				theApp.m_tSysParam.AutoNeedleCalib.ConvertPixelToTable(m_tgCenter.x - (milApp.m_lBufSizeX-1)/2,
					m_tgCenter.y - (milApp.m_lBufSizeY-1)/2,dX,dY);
			}
			if(nCount>=g_pFrm->m_CmdRun.ProductParam.nAutoAdjustCount)
			{
				AfxMessageBox("调整失败！");
			}
		}
	}
	else if(BLOB_MODE == g_pFrm->m_CmdRun.ProductParam.searchMode)
	{
		if(!FindBlobCenter(m_tgCenter.x,m_tgCenter.y))
		{
			AfxMessageBox("查找图像失败！");
		}
		else
		{
			UpdateResult();
			double dX,dY;
			theApp.m_tSysParam.AutoNeedleCalib.ConvertPixelToTable(m_tgCenter.x - (milApp.m_lBufSizeX-1)/2,
				m_tgCenter.y - (milApp.m_lBufSizeY-1)/2,dX,dY);
			while(fabs(dX)>g_pFrm->m_CmdRun.ProductParam.dAutoNeedleErrorX||
				fabs(dY)>g_pFrm->m_CmdRun.ProductParam.dAutoNeedleErrorY)
			{
				CFunction::DoEvents();
				if(nCount>=g_pFrm->m_CmdRun.ProductParam.nAutoAdjustCount)
				{
					break;
				}
				nCount++;
				g_pFrm->m_CmdRun.AdjustNeedle(0.0,0.0,dX,dY);//consider 3# valve;
				g_pFrm->m_CmdRun.MoveToAdjustPos(2);
				Sleep(500);
				if(!FindBlobCenter(m_tgCenter.x,m_tgCenter.y))
				{
					AfxMessageBox("查找图像失败！");
					break;
				}
				UpdateResult();
				//convert the pixsel value to mm in real world ;
				theApp.m_tSysParam.AutoNeedleCalib.ConvertPixelToTable(m_tgCenter.x - (milApp.m_lBufSizeX-1)/2,
					m_tgCenter.y - (milApp.m_lBufSizeY-1)/2,dX,dY);
			}
			//the adjust result is decided by the adjust counter;
			if(nCount>=g_pFrm->m_CmdRun.ProductParam.nAutoAdjustCount)
			{
				AfxMessageBox("调整失败！");
			}
		}
	}
}

void CDlgAutoNeedle::OnBnClickedBtnAutoNeedleAdjustNeedle()
{
	// TODO: Add your control notification handler code here
	bool bReturn = false;
	OnBnClickedBtnAutoNeedleSaveRoi();
	g_pFrm->m_CmdRun.m_tStatus = K_RUN_STS_RUN;
	bReturn = g_pFrm->m_CmdRun.AutoAdjustNeedle();
	g_pFrm->m_CmdRun.m_tStatus = K_RUN_STS_STOP;
	if(bReturn)
	{
		g_pFrm->m_CmdRun.ProductParam.dAdjustMotorInRunA=theApp.m_Mv400.GetPos(K_AXIS_A);
		g_pFrm->m_CmdRun.ProductParam.dAdjustMotorInRunB=theApp.m_Mv400.GetPos(K_AXIS_B);
		g_pFrm->m_CmdRun.ProductParam.dAdjustMotorInRunC=theApp.m_Mv400.GetPos(K_AXIS_C);
		g_pFrm->m_CmdRun.ProductParam.dAdjustMotorInRunD=theApp.m_Mv400.GetPos(K_AXIS_D);
		theApp.ProductParam(FALSE);
		AfxMessageBox("调整针头成功！");
	}
	else
	{
		AfxMessageBox("调整针头失败！");
	}
}

void CDlgAutoNeedle::OnBnClickedBtnAutoNeedleNeedleOne()
{
	// TODO: Add your control notification handler code here
	if(!theApp.m_Mv400.IsInitOK())
	{
		return;
	}

	g_pFrm->m_CmdRun.MoveToZSafety();
	tgPos tgNeedlePos,tgCameraPos;
	tgNeedlePos.x = g_pFrm->m_CmdRun.ProductParam.NeedlePos[0] +
		g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.dPitchX * g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentX;
	tgNeedlePos.y = g_pFrm->m_CmdRun.ProductParam.NeedlePos[1] +
		g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.dPitchY * g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentY;
	tgNeedlePos.za = g_pFrm->m_CmdRun.ProductParam.NeedlePos[2];
	tgCameraPos.x = tgNeedlePos.x +
		(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].x - g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].x);
	tgCameraPos.y = tgNeedlePos.y +
		(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].y - g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].y);
	tgCameraPos.za = g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].za;

	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgNeedlePos.x,tgNeedlePos.y,true))
	{
		AfxMessageBox("移动XY轴失败！");
		return;
	}
	if(!g_pFrm->m_CmdRun.MoveZ(K_AXIS_ZA,tgNeedlePos.za,g_pFrm->m_CmdRun.ProductParam.NeedleVel,
		g_pFrm->m_CmdRun.ProductParam.NeedleAcc,true))
	{
		AfxMessageBox("移动ZA轴失败！");
		return;
	}
	Sleep(100);
	theApp.m_V9Control.V9C_Dot(K_AXIS_U,g_pFrm->m_CmdRun.ProductParam.dAutoNeedleGlueVol);
	Sleep(100);
	g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentY++;
	if(g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentY>=g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCountY)
	{
		g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentY = 0;
		g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentX++;
		if(g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentX>=g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCountX)
		{
			g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentX = 0;
		}
	}
	if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
	{
		AfxMessageBox("设置电机参数失败！");
		return;
	}
	g_pFrm->m_CmdRun.MoveToZSafety();
	Sleep(1000);
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgCameraPos.x,tgCameraPos.y,true))
	{
		AfxMessageBox("移动到相机位失败！");
		return;
	}
	if(!g_pFrm->m_CmdRun.MoveZ(K_AXIS_ZA,tgCameraPos.za,true))
	{
		AfxMessageBox("移动到相机位失败！");
		return;
	}
}
//2# needle to target position 
void CDlgAutoNeedle::OnBnClickedBtnAutoNeedleNeedleTwo()
{
	// TODO: Add your control notification handler code here
	if(!theApp.m_Mv400.IsInitOK())
	{
		return;
	}

	g_pFrm->m_CmdRun.MoveToZSafety();
	tgPos tgNeedlePos,tgCameraPos;
	tgNeedlePos.x = g_pFrm->m_CmdRun.ProductParam.NeedlePos[0] - g_pFrm->m_CmdRun.ProductParam.NeedleGap +
		g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.dPitchX * g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentX;
	tgNeedlePos.y = g_pFrm->m_CmdRun.ProductParam.NeedlePos[1] +
		g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.dPitchY * g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentY;
	tgNeedlePos.zb = g_pFrm->m_CmdRun.ProductParam.NeedlePos[3];
	tgCameraPos.x = tgNeedlePos.x + g_pFrm->m_CmdRun.ProductParam.NeedleGap +
		(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].x - g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].x);
	tgCameraPos.y = tgNeedlePos.y +
		(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].y - g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].y);
	tgCameraPos.za = g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].za;

	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgNeedlePos.x,tgNeedlePos.y,true))
	{
		AfxMessageBox("移动XY轴失败！");
		return;
	}
	if(!g_pFrm->m_CmdRun.MoveZ(K_AXIS_ZB,tgNeedlePos.zb,g_pFrm->m_CmdRun.ProductParam.NeedleVel,
		g_pFrm->m_CmdRun.ProductParam.NeedleAcc,true))
	{
		AfxMessageBox("移动ZB轴失败！");
		return;
	}
	Sleep(100);
	theApp.m_V9Control.V9C_Dot(K_AXIS_V,g_pFrm->m_CmdRun.ProductParam.dAutoNeedleGlueVol);
	Sleep(100);
	g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentY++;
	if(g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentY>=g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCountY)
	{
		g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentY = 0;
		g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentX++;
		if(g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentX>=g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCountX)
		{
			g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentX = 0;
		}
	}
	if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
	{
		AfxMessageBox("设置电机参数失败！");
		return;
	}
	g_pFrm->m_CmdRun.MoveToZSafety();
	Sleep(1000);
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgCameraPos.x,tgCameraPos.y,true))
	{
		AfxMessageBox("移动到相机位失败！");
		return;
	}
	if(!g_pFrm->m_CmdRun.MoveZ(K_AXIS_ZA,tgCameraPos.za,true))
	{
		AfxMessageBox("移动到相机位失败！");
		return;
	}
}
//1:3# needle to origin  position and offset;2:dispense;3:camera grab;
//find the glue position before correct ;
void CDlgAutoNeedle::OnBnClickedBtnAutoNeedleNeedleThree()
{
	// TODO: Add your control notification handler code here
	if(!theApp.m_Mv400.IsInitOK())
	{
		return;
	}
	g_pFrm->m_CmdRun.MoveToZSafety();
	tgPos tgNeedlePos,tgCameraPos;
	tgNeedlePos.x = g_pFrm->m_CmdRun.ProductParam.NeedlePos[0] - 2*g_pFrm->m_CmdRun.ProductParam.NeedleGap +
		g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.dPitchX * g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentX;
	tgNeedlePos.y = g_pFrm->m_CmdRun.ProductParam.NeedlePos[1] +
		g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.dPitchY * g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentY;
	tgNeedlePos.zc = g_pFrm->m_CmdRun.ProductParam.NeedlePos[4];
	tgCameraPos.x = tgNeedlePos.x + 2*g_pFrm->m_CmdRun.ProductParam.NeedleGap +
		(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].x - g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].x);
	tgCameraPos.y = tgNeedlePos.y +
		(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].y - g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].y);
	tgCameraPos.za = g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].za;

	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgNeedlePos.x,tgNeedlePos.y,true))
	{
		AfxMessageBox("移动XY轴失败！");
		return;
	}
	if(!g_pFrm->m_CmdRun.MoveZ(K_AXIS_ZC,tgNeedlePos.zc,g_pFrm->m_CmdRun.ProductParam.NeedleVel,
		g_pFrm->m_CmdRun.ProductParam.NeedleAcc,true))
	{
		AfxMessageBox("移动ZC轴失败！");
		return;
	}
	Sleep(100);
	theApp.m_V9Control.V9C_Dot(K_AXIS_W,g_pFrm->m_CmdRun.ProductParam.dAutoNeedleGlueVol);
	Sleep(100);
	g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentY++;
	if(g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentY>=g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCountY)
	{
		g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentY = 0;
		g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentX++;
		if(g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentX>=g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCountX)
		{
			g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentX = 0;
		}
	}
	if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
	{
		AfxMessageBox("设置电机参数失败！");
		return;
	}
	g_pFrm->m_CmdRun.MoveToZSafety();
	Sleep(1000);
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgCameraPos.x,tgCameraPos.y,true))
	{
		AfxMessageBox("移动到相机位失败！");
		return;
	}
	if(!g_pFrm->m_CmdRun.MoveZ(K_AXIS_ZA,tgCameraPos.za,true))
	{
		AfxMessageBox("移动到相机位失败！");
		return;
	}
}
//compute the pixsel and mm transfer value;
void CDlgAutoNeedle::OnBnClickedBtnAutoNeedleCalib()
{
	// TODO: Add your control notification handler code here
	if(!theApp.m_Mv400.IsInitOK())
	{
		return;
	}
	CString strText;
	double dTempA=0.0;
	double dTempB=0.0;
	double dTempC=0.0;
	double dTempD=0.0;
	tgPos tgTempPixel;
	g_pFrm->m_CmdRun.MoveToZSafety();
	tgPos tgNeedlePos,tgCameraPos;
	//compute [dispense position] and [cemera grab position];
	tgNeedlePos.x = g_pFrm->m_CmdRun.ProductParam.NeedlePos[0] +
		g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.dPitchX * g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentX;
	tgNeedlePos.y = g_pFrm->m_CmdRun.ProductParam.NeedlePos[1] +
		g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.dPitchY * g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentY;
	tgNeedlePos.za = g_pFrm->m_CmdRun.ProductParam.NeedlePos[2];
	tgCameraPos.x = tgNeedlePos.x +
		(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].x - g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].x);
	tgCameraPos.y = tgNeedlePos.y +
		(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].y - g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].y);
	tgCameraPos.za = g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3].za;
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgNeedlePos.x,tgNeedlePos.y,true))
	{
		return;
	}
	if(!g_pFrm->m_CmdRun.MoveZ(K_AXIS_ZA,tgNeedlePos.za,g_pFrm->m_CmdRun.ProductParam.NeedleVel,g_pFrm->m_CmdRun.ProductParam.NeedleAcc,TRUE))
	{
		return;
	}
	Sleep(500);
	theApp.m_V9Control.V9C_Dot(K_AXIS_U,g_pFrm->m_CmdRun.ProductParam.dAutoNeedleGlueVol);
	Sleep(500);
	if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
	{
		return;
	}
	g_pFrm->m_CmdRun.MoveToZSafety();
	Sleep(500);
	//step1: :right pattern;
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgCameraPos.x+0.2,tgCameraPos.y,true))
	{
		return;
	}
	if(!g_pFrm->m_CmdRun.MoveZ(K_AXIS_ZA,tgCameraPos.za,true))
	{
		return;
	}
	Sleep(500);
	CTimeUtil::Delay(long(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay*1000));
	if(GREY_MODE == g_pFrm->m_CmdRun.ProductParam.searchMode)
	{
		if(!FindNeedleModel(m_tgCenter.x,m_tgCenter.y,m_dScore))
		{
			return;
		}
	}
	else
	{
		if(!FindBlobCenter(m_tgCenter.x,m_tgCenter.y))
		{
			return;
		}
	}
	tgTempPixel.x = m_tgCenter.x;
	tgTempPixel.y = m_tgCenter.y;
	//step2: :left pattern;
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgCameraPos.x-0.2,tgCameraPos.y,true))
	{
		return;
	}
	Sleep(500);
	CTimeUtil::Delay(long(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay*1000));
	if(GREY_MODE == g_pFrm->m_CmdRun.ProductParam.searchMode)
	{
		if(!FindNeedleModel(m_tgCenter.x,m_tgCenter.y,m_dScore))
		{
			return;
		}
	}
	else
	{
		if(!FindBlobCenter(m_tgCenter.x,m_tgCenter.y))
		{
			return;
		}
	}
	dTempA = (tgTempPixel.x - m_tgCenter.x)/0.4;
	dTempB = (tgTempPixel.y - m_tgCenter.y)/0.4;
	strText.Format("图像系数:dCalibrationA=%.3f,dCalibrationB=%.3f",theApp.m_tSysParam.AutoNeedleCalib.dCalibrationA,theApp.m_tSysParam.AutoNeedleCalib.dCalibrationB);
	g_pFrm->m_CmdRun.AddMsg(strText);
	
	//step3: :bottom pattern;
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgCameraPos.x,tgCameraPos.y+0.2,true))
	{
		return;
	}
	Sleep(500);
	CTimeUtil::Delay(long(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay*1000));
	if(GREY_MODE == g_pFrm->m_CmdRun.ProductParam.searchMode)
	{
		if(!FindNeedleModel(m_tgCenter.x,m_tgCenter.y,m_dScore))
		{
			return;
		}
	}
	else
	{
		if(!FindBlobCenter(m_tgCenter.x,m_tgCenter.y))
		{
			return;
		}
	}
	tgTempPixel.x = m_tgCenter.x;
	tgTempPixel.y = m_tgCenter.y;
	//step4: :top pattern;
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgCameraPos.x,tgCameraPos.y-0.2,true))
	{
		return;
	}
	Sleep(500);
	CTimeUtil::Delay(long(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay*1000));
	if(GREY_MODE == g_pFrm->m_CmdRun.ProductParam.searchMode)
	{
		if(!FindNeedleModel(m_tgCenter.x,m_tgCenter.y,m_dScore))
		{
			return;
		}
	}
	else
	{
		if(!FindBlobCenter(m_tgCenter.x,m_tgCenter.y))
		{
			return;
		}
	}
	dTempC = (tgTempPixel.x - m_tgCenter.x)/0.4;
	dTempD = (tgTempPixel.y - m_tgCenter.y)/0.4;
	strText.Format("图像系数:dCalibrationA=%.3f,dCalibrationB=%.3f",theApp.m_tSysParam.AutoNeedleCalib.dCalibrationC,theApp.m_tSysParam.AutoNeedleCalib.dCalibrationD);
	g_pFrm->m_CmdRun.AddMsg(strText);
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgCameraPos.x,tgCameraPos.y,true))
	{
		return;
	}
	Sleep(500);
	double det = dTempA*dTempD - dTempB*dTempC;
	if(fabs(det)<0.001)
	{
		return;
	}
	theApp.m_tSysParam.AutoNeedleCalib.dCalibrationA = dTempD/det;
	theApp.m_tSysParam.AutoNeedleCalib.dCalibrationB = -dTempB/det;
	theApp.m_tSysParam.AutoNeedleCalib.dCalibrationC = -dTempC/det;
	theApp.m_tSysParam.AutoNeedleCalib.dCalibrationD = dTempA/det;

	strText.Format("%0.3f",theApp.m_tSysParam.AutoNeedleCalib.dCalibrationA);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_CALIB_A,strText);
	strText.Format("%0.3f",theApp.m_tSysParam.AutoNeedleCalib.dCalibrationB);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_CALIB_B,strText);
	strText.Format("%0.3f",theApp.m_tSysParam.AutoNeedleCalib.dCalibrationC);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_CALIB_C,strText);
	strText.Format("%0.3f",theApp.m_tSysParam.AutoNeedleCalib.dCalibrationD);
	SetDlgItemText(IDC_EDIT_AUTO_NEEDLE_CALIB_D,strText);
	theApp.BmpParam(FALSE);
}

void CDlgAutoNeedle::OnBnClickedBtnAutoNeedleSaveParame()
{
	// TODO: Add your control notification handler code here
	SaveData();
}
//clear the step of current step;
void CDlgAutoNeedle::OnBnClickedBtnAutoNeedleReset()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentX = 0;
	g_pFrm->m_CmdRun.ProductParam.tgAutoNeedleStep.nCurrentY = 0;
}

void CDlgAutoNeedle::OnBnClickedBtnAutoNeedleSaveRoi()
{
	// TODO: Add your control notification handler code here
	theApp.m_tSysParam.AutoNeedleROI = m_picture.RoiRect;
}

void CDlgAutoNeedle::OnBnClickedBtnAutoNeedleSaveModelWin()
{
	// TODO: Add your control notification handler code here
	theApp.m_tSysParam.AutoNeedleModelWin.TopLeft().x = long(m_picture.m_mod_rectangle.dx - long(m_picture.m_mod_rectangle.width-1)/2);
	theApp.m_tSysParam.AutoNeedleModelWin.TopLeft().y = long(m_picture.m_mod_rectangle.dy - long(m_picture.m_mod_rectangle.height-1)/2);
	theApp.m_tSysParam.AutoNeedleModelWin.BottomRight().x = theApp.m_tSysParam.AutoNeedleModelWin.TopLeft().x + m_picture.m_mod_rectangle.width;
	theApp.m_tSysParam.AutoNeedleModelWin.BottomRight().y = theApp.m_tSysParam.AutoNeedleModelWin.TopLeft().y + m_picture.m_mod_rectangle.height;
}

void CDlgAutoNeedle::OnBnClickedBtnAutoNeedleSaveModel()
{
	// TODO: Add your control notification handler code here
	OnBnClickedBtnAutoNeedleSaveModelWin();
	CString strFile = g_pDoc->GetModeFile(7);
	milApp.LearnModel(theApp.m_tSysParam.AutoNeedleModelWin);
	milApp.SaveModel(strFile.GetBuffer(strFile.GetLength()));

}

void CDlgAutoNeedle::OnBnClickedBtnAutoNeedleSearch()
{
    //pattern mode
	if(GREY_MODE == g_pFrm->m_CmdRun.ProductParam.searchMode)
	{
		if(!FindNeedleModel(m_tgCenter.x,m_tgCenter.y,m_dScore))
		{
			return;
		}
	}
	else//blob mode
	{
		if(!FindBlobCenter(m_tgCenter.x,m_tgCenter.y))
		{
			return;
		}
	}
}
