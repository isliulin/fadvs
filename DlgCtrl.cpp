// DlgCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgCtrl.h"
#include "DlgSetPosition.h"
#include "DlgHeightPosition.h"
#include "DlgValveHeat.h"

// CDlgCtrl dialog

IMPLEMENT_DYNAMIC(CDlgCtrl, CDialog)

CDlgCtrl::CDlgCtrl(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCtrl::IDD, pParent)
{
	m_iSpeed = 1;
	m_dLaserHeightVal = 0.0;
}

CDlgCtrl::~CDlgCtrl()
{
}

void CDlgCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_COMBO_IMAGE_SELECT_INDEX,m_cbImageSelect);
	DDX_Control(pDX,IDC_COMBO_TEST_INDEX,m_cbTestIndex);
	DDX_Control(pDX,IDC_COMBO_CTRL_HEIGHT_INDEX,m_cbHeightNumber);
	DDX_Control(pDX, IDC_CHECK_AUTO_DEFOAM, m_CheckAutoDefoam);
}
// CDlgCtrl message handlers
BEGIN_MESSAGE_MAP(CDlgCtrl, CDialog)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SPEED, &CDlgCtrl::OnNMCustomdrawSliderSpeed)
	ON_BN_CLICKED(IDC_SAVE_DISPENSING_POS, &CDlgCtrl::OnBnClickedSaveDispensingPos)
	ON_BN_CLICKED(IDC_MOVE_DISPENSING_POS, &CDlgCtrl::OnBnClickedMoveDispensingPos)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_MOVE_CLEAN_POS, &CDlgCtrl::OnBnClickedMoveCleanPos)
	ON_BN_CLICKED(IDC_SAVE_CLEAN_POS, &CDlgCtrl::OnBnClickedSaveCleanPos)
	ON_BN_CLICKED(IDC_CLEAN, &CDlgCtrl::OnBnClickedClean)
	ON_BN_CLICKED(IDC_MOVE_START_IMAGE_POS, &CDlgCtrl::OnBnClickedMoveStartImagePos)
	ON_BN_CLICKED(IDC_MOVE_END_IMAGE_POS, &CDlgCtrl::OnBnClickedMoveEndImagePos)
	ON_BN_CLICKED(IDC_SAVE_TEST_DISP_POS, &CDlgCtrl::OnBnClickedSaveTestDispPos)
	ON_BN_CLICKED(IDC_MOVE_TEST_DISP_POS, &CDlgCtrl::OnBnClickedMoveTestDispPos)
	ON_BN_CLICKED(IDC_TEST_DISP, &CDlgCtrl::OnBnClickedTestDisp)
	ON_BN_CLICKED(IDC_BTN_ALIGN_NEEDLE, &CDlgCtrl::OnBnClickedBtnAlignNeedle)
	ON_BN_CLICKED(IDC_BTN_MOVE_SLOT_DETECT_POS, &CDlgCtrl::OnBnClickedBtnMoveSlotDetectPos)
	ON_BN_CLICKED(IDC_BTN_SAVE_SLOT_DETECT_POS, &CDlgCtrl::OnBnClickedBtnSaveSlotDetectPos)
	ON_BN_CLICKED(IDC_BTN_MOVE_DISCHARGE_POS, &CDlgCtrl::OnBnClickedBtnMoveDischargePos)
	ON_BN_CLICKED(IDC_BTN_SAVE_DISCHARGE_POS, &CDlgCtrl::OnBnClickedBtnSaveDischargePos)
	ON_BN_CLICKED(IDC_BTN_DISCHARGE_GLUE, &CDlgCtrl::OnBnClickedBtnDischargeGlue)
	ON_BN_CLICKED(IDC_BTN_DEFOAM, &CDlgCtrl::OnBnClickedBtnDefoam)
	ON_BN_CLICKED(IDC_BTN_SAVE_CURRENT_DISP_POS, &CDlgCtrl::OnBnClickedBtnSaveCurrentDispPos)
	ON_BN_CLICKED(IDC_BTN_SAVE_MODIFY_DISP_POS, &CDlgCtrl::OnBnClickedBtnSaveModifyDispPos)
	ON_CBN_SELCHANGE(IDC_COMBO_IMAGE_SELECT_INDEX, &CDlgCtrl::OnCbnSelchangeComboImageSelectIndex)
	ON_BN_CLICKED(IDC_BTN_MOVE_IMAGE_POS, &CDlgCtrl::OnBnClickedBtnMoveImagePos)
	ON_BN_CLICKED(IDC_BTN_SET_IMAGE_POS, &CDlgCtrl::OnBnClickedBtnSetImagePos)
	ON_BN_CLICKED(IDC_BTN_SEARCH_IMAGE, &CDlgCtrl::OnBnClickedBtnSearchImage)
	ON_CBN_SELCHANGE(IDC_COMBO_TEST_INDEX, &CDlgCtrl::OnCbnSelchangeComboTestIndex)
	ON_BN_CLICKED(IDC_SINGLE_TEST_DISPENSE, &CDlgCtrl::OnBnClickedSingleTestDispense)
	ON_BN_CLICKED(IDC_SET_TEST_DISPENSE, &CDlgCtrl::OnBnClickedSetTestDispense)
	ON_BN_CLICKED(IDC_BTN_CTRL_MOVETO_HEIGHT_POS, &CDlgCtrl::OnBnClickedBtnCtrlMovetoHeightPos)
	ON_BN_CLICKED(IDC_BTN_CTRL_SET_HEIGHT_POS, &CDlgCtrl::OnBnClickedBtnCtrlSetHeightPos)
	ON_BN_CLICKED(IDC_BTN_CTRL_MEASURE_HEIGHT, &CDlgCtrl::OnBnClickedBtnCtrlMeasureHeight)
	ON_CBN_SELCHANGE(IDC_COMBO_CTRL_HEIGHT_INDEX, &CDlgCtrl::OnCbnSelchangeComboCtrlHeightIndex)
	ON_BN_CLICKED(IDC_MOVE_ADJUST_CLEAN, &CDlgCtrl::OnBnClickedMoveAdjustClean)
	ON_BN_CLICKED(IDC_MOVE_ADJUST_NEEDLE, &CDlgCtrl::OnBnClickedMoveAdjustNeedle)
	ON_BN_CLICKED(IDC_BtnModifyDischarge, &CDlgCtrl::OnBnClickedBtnmodifydischarge)
	ON_BN_CLICKED(IDC_BtnModifyClean, &CDlgCtrl::OnBnClickedBtnmodifyclean)
	ON_BN_CLICKED(IDC_BTN_MOVE_FOCUS_POS, &CDlgCtrl::OnBnClickedBtnMoveFocusPos)
	ON_BN_CLICKED(IDC_BTN_SAVE_CURRENT_FOCUS_POS, &CDlgCtrl::OnBnClickedBtnSaveCurrentFocusPos)
	ON_BN_CLICKED(IDC_BTN_SAVE_MODIFY_FOCUS, &CDlgCtrl::OnBnClickedBtnSaveModifyFocus)
	ON_BN_CLICKED(IDC_BTN_SAVE_TESTZ_EACH, &CDlgCtrl::OnBnClickedBtnSaveTestzEach)
	ON_BN_CLICKED(IDC_BTN_STOP_DISCHARGE, &CDlgCtrl::OnBnClickedBtnStopDischarge)
	ON_BN_CLICKED(IDC_BTN_ZOFFSET_APPLY, &CDlgCtrl::OnBnClickedBtnZoffsetApply)
	ON_BN_CLICKED(IDC_BTN_VALVE_HEAT, &CDlgCtrl::OnBnClickedBtnValveHeat)
	ON_BN_CLICKED(IDC_BTN_CTRL_SAVE_HEIGHT_DATA, &CDlgCtrl::OnBnClickedBtnCtrlSaveHeightData)
END_MESSAGE_MAP()
// 初始化
BOOL CDlgCtrl::OnInitDialog()
{
	CDialog::OnInitDialog();
	for(int i=0; i<10; i++)
	{
		m_colorButton[i].m_iIndex = i;
		m_colorButton[i].SubclassDlgItem(IDC_BUTTON_X_NEG+i, this);
		m_colorButton[i].SetBitmapEx(IDB_BITMAP_MOVE);
		m_colorButton[i].m_pMv = &theApp.m_Mv400;
	}
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_SPEED))->SetPos(1);
	CString str;
	for(int i=0;i<g_pFrm->m_CmdRun.ProductParam.nImageNum;i++)
	{
		str.Format("%d",i);
		m_cbImageSelect.AddString(str);
	}
	m_cbImageSelect.SetCurSel(0);
	nImagePosID = m_cbImageSelect.GetCurSel();
	for(int i=0;i<g_pFrm->m_CmdRun.ProductParam.TestDotNumber;i++)
	{
		str.Format("%d",i);
		m_cbTestIndex.AddString(str);
	}
	m_cbTestIndex.SetCurSel(0);
	m_nTestIndex = m_cbTestIndex.GetCurSel();
	for(int i=0;i<g_pFrm->m_CmdRun.ProductParam.HeightNumber;i++)
	{
		str.Format("%d",i);
		m_cbHeightNumber.AddString(str);
	}
	m_cbHeightNumber.SetCurSel(0);
	m_nHeightIndex = m_cbHeightNumber.GetCurSel();
    m_CheckAutoDefoam.SetCheck(1);
	UPDataVal();
	SetAuthority();
	SetTimer(0, 200, NULL);

	GetDlgItem(IDC_EDIT_ZOFFSET_ALL)->EnableWindow(false);
	GetDlgItem(IDC_BTN_ZOFFSET_APPLY)->EnableWindow(false);

	g_pFrm->bIsCtrlDlgVisible = true;
	return TRUE;  
}

BOOL CDlgCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_KEYDOWN)
	{
		OnKeyDown(pMsg->wParam, 1, 0);
	}
	else if(pMsg->message == WM_KEYUP)
	{
		OnKeyUp(pMsg->wParam, 1, 0);
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgCtrl::OnNMCustomdrawSliderSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_SPEED))->GetPos();
	CColorButton::m_iSpeedNo = n;
	CString strTemp;
	strTemp.Format("X:%.4f,Y:%.4f,ZA:%.4f,ZB:%.4f,ZC:%.4f", theApp.m_tSysParam.tAxis[0].vel[0] * n/2000.0,
		theApp.m_tSysParam.tAxis[1].vel[0] * n/2000.0,theApp.m_tSysParam.tAxis[2].vel[0] * n/2000.0,theApp.m_tSysParam.tAxis[3].vel[0] * n/2000.0,theApp.m_tSysParam.tAxis[4].vel[0] * n/2000.0);
	SetDlgItemText(IDC_EDIT_KEYCTRL, strTemp);

	*pResult = 0;
}
// 保存点胶位
void CDlgCtrl::OnBnClickedSaveDispensingPos()
{
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能保存点胶位!");
		return;
	}
	if (theApp.m_Mv400.IsMoveList())
	{
		AfxMessageBox("运动中，不能保存点胶位!");
		return;

	}
	if(AfxMessageBox("你确定保存新的点胶位吗？",MB_YESNO)!=IDYES)
	{
		return;
	}
	SetAuthority(false);
	for (int i=0;i<5;i++)
	{
		g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[i]=theApp.m_Mv400.GetPos(i+1);		
	}
	theApp.ProductParam(FALSE);
	UPDataVal();
	SetAuthority(true);
}
// X和Y轴移动到点胶位
void CDlgCtrl::OnBnClickedMoveDispensingPos()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能移动到A#点胶位!");
		return;
	}
	if(theApp.m_Mv400.IsInitOK())
	{
		if(!g_pFrm->m_CmdRun.MoveToZSafety())
		{
			AfxMessageBox("调节针头前Z轴回安全位失败！");
			return;
		}
		if(!g_pFrm->m_CmdRun.AdjustNeedle())
		{
			AfxMessageBox("调整电机移动失败！");
			return;
		}
	}
	SetAuthority(false);
	if (g_pFrm->m_CmdRun.ManalRun)
	{
		if(AfxMessageBox("Z轴移动到点胶位置！",MB_YESNO)==IDYES)
		{
			g_pFrm->m_CmdRun.DispensingPos(true);
		}
		else
		{
			g_pFrm->m_CmdRun.DispensingPos(false);
		}
	}
	UPDataVal();
	SetAuthority(true);
}

// 定时器
void CDlgCtrl::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	int bFresh=0;
	CButton * bTn;
	bTn=(CButton *)GetDlgItem(IDC_CHECK_SAFE_SENSOR);
	bFresh=bTn->GetCheck();
	if (1==bFresh)
	{
		UpdateSensor();
	}

	CString str;
	g_pFrm->m_CmdRun.ReadHeightSensorValue(&m_dLaserHeightVal);
	str.Format("%0.3f",m_dLaserHeightVal);
	SetDlgItemText(IDC_EDIT_CUR_LASER_VAL,str);
	GetDlgItem(IDC_BTN_MOVE_FOCUS_POS)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_BTN_SAVE_MODIFY_FOCUS)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_BTN_SAVE_CURRENT_FOCUS_POS)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	SetAuthority();
	CDialog::OnTimer(nIDEvent);
}

void CDlgCtrl::UpdateSensor()
{
	static bool b = false;
	if(b) return;
	b = true;
	CString str;
	bool bReadData = false;
	long lReadData = 0;

	bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"MR","6000");
	if(!bReadData)
	{
		b=false;
		return ;
	}
	if(lReadData==1)
	{
		DeleteObject(m_hBitmapA);
		m_hBitmapA = ::LoadBitmap(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP_SENSOR_ON));
		str.Format("针头1数据：1");
		g_pFrm->m_CmdRun.AddMsg(str);
	}
	else
	{
		DeleteObject(m_hBitmapA);
		m_hBitmapA = ::LoadBitmap(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP_SENSOR_OFF));
		str.Format("针头1数据：0");
		g_pFrm->m_CmdRun.AddMsg(str);
	}
	((CButton*)GetDlgItem(IDC_BTN_NEEDLEONE))->SetBitmap(m_hBitmapA);

	bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"MR","6001");
	if(!bReadData)
	{
		b=false;
		return ;
	}
	if(lReadData==1)
	{
		DeleteObject(m_hBitmapB);
		m_hBitmapB = ::LoadBitmap(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP_SENSOR_ON));
		str.Format("针头2数据：1");
		g_pFrm->m_CmdRun.AddMsg(str);
	}
	else
	{
		DeleteObject(m_hBitmapB);
		m_hBitmapB = ::LoadBitmap(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP_SENSOR_OFF));
		str.Format("针头2数据：0");
		g_pFrm->m_CmdRun.AddMsg(str);
	}
	((CButton*)GetDlgItem(IDC_BTN_NEEDLETWO))->SetBitmap(m_hBitmapB);

	bReadData = theApp.m_Serial.ReadPlcData(&lReadData,"MR","6002");
	if(!bReadData)
	{
		b=false;
		return ;
	}
	if(lReadData==1)
	{
		DeleteObject(m_hBitmapC);
		m_hBitmapC = ::LoadBitmap(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP_SENSOR_ON));
		str.Format("针头3数据：1");
		g_pFrm->m_CmdRun.AddMsg(str);
	}
	else
	{
		DeleteObject(m_hBitmapC);
		m_hBitmapC = ::LoadBitmap(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP_SENSOR_OFF));
		str.Format("针头3数据：0");
		g_pFrm->m_CmdRun.AddMsg(str);
	}
	((CButton*)GetDlgItem(IDC_BTN_NEEDLETHREE))->SetBitmap(m_hBitmapC);
	b=false;
}
int CDlgCtrl::UPDataVal(void)
{
	CString str;
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[0]);
	SetDlgItemText(IDC_PX1,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[1]);
	SetDlgItemText(IDC_PY1,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[2]);//20180907
	SetDlgItemText(IDC_PZA1,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[3]);//20180907
	SetDlgItemText(IDC_PZB1,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[4]);//20180907
	SetDlgItemText(IDC_PZC1,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dZDispOffsetAll);
	SetDlgItemText(IDC_EDIT_ZOFFSET_ALL,str);

	str.Format("%0.3f",theApp.m_tSysParam.CleanPosition[0]);
	SetDlgItemText(IDC_PX2,str);
	str.Format("%0.3f",theApp.m_tSysParam.CleanPosition[1]);
	SetDlgItemText(IDC_PY2,str);
	str.Format("%0.3f",theApp.m_tSysParam.CleanPosition[2]);
	SetDlgItemText(IDC_PZA2,str);
	str.Format("%0.3f",theApp.m_tSysParam.CleanPosition[3]);
	SetDlgItemText(IDC_PZB2,str);
	str.Format("%0.3f",theApp.m_tSysParam.CleanPosition[4]);
	SetDlgItemText(IDC_PZC2,str);

	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[m_nTestIndex].x);
	SetDlgItemText(IDC_PX3,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[m_nTestIndex].y);
	SetDlgItemText(IDC_PY3,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[m_nTestIndex].za);
	SetDlgItemText(IDC_PZA3,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[m_nTestIndex].zb);
	SetDlgItemText(IDC_PZB3,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[m_nTestIndex].zc);
	SetDlgItemText(IDC_PZC3,str);

	str.Format("%0.3f",theApp.m_tSysParam.DischargeGluePostion[0]);
	SetDlgItemText(IDC_PX4,str);
	str.Format("%0.3f",theApp.m_tSysParam.DischargeGluePostion[1]);
	SetDlgItemText(IDC_PY4,str);
	str.Format("%0.3f",theApp.m_tSysParam.DischargeGluePostion[2]);
	SetDlgItemText(IDC_PZA4,str);
	str.Format("%0.3f",theApp.m_tSysParam.DischargeGluePostion[3]);
	SetDlgItemText(IDC_PZB4,str);
	str.Format("%0.3f",theApp.m_tSysParam.DischargeGluePostion[4]);
	SetDlgItemText(IDC_PZC4,str);

	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgImagePos[nImagePosID].x);
	SetDlgItemText(IDC_PX5,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgImagePos[nImagePosID].y);
	SetDlgItemText(IDC_PY5,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgImagePos[nImagePosID].za);
	SetDlgItemText(IDC_PZA5,str);
	//str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgImagePos[nImagePosID].za);
	//SetDlgItemText(IDC_PZA5,str);
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.nImageNum);
	SetDlgItemText(IDC_EDIT_IMAGE_NUM,str);

	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tSlotDetectPos.x);
	SetDlgItemText(IDC_PX6,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tSlotDetectPos.y);
	SetDlgItemText(IDC_PY6,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tSlotDetectPos.za);
	SetDlgItemText(IDC_PZA6,str);

	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgMeasureHeightPos[m_nHeightIndex].x);
	SetDlgItemText(IDC_PX7,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgMeasureHeightPos[m_nHeightIndex].y);
	SetDlgItemText(IDC_PY7,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgMeasureHeightPos[m_nHeightIndex].za);
	SetDlgItemText(IDC_PZA7,str);
	//str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgMeasureHeightPos[m_nHeightIndex].za);
	//SetDlgItemText(IDC_PZA7,str);
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.HeightNumber);
	SetDlgItemText(IDC_EDIT_CTRL_HEIGHT_NUMBER,str);
	str.Format("%0.3f",theApp.m_tSysParam.dFocusZPos);
	SetDlgItemText(IDC_ZFOCUSA,str);	
	return 0;
}
// 到吸真空位
void CDlgCtrl::OnBnClickedMoveCleanPos()
{
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能移动到清洗位!");
		return;
	}
	SetAuthority(false);
	//if (!g_pFrm->m_CmdRun.IsRotatingLightClose())
	//{
	//	if (IDOK!=AfxMessageBox("旋转气缸未关闭到位，\n按 确定 忽略提示继续运动到目标位置"), MB_OKCANCEL)
	//	{
	//		//if (g_pFrm->m_CmdRun.ManalRun)
	//		//{
	//		//	g_pFrm->m_CmdRun.CleanNeedle(false);
	//		//}
	//		return;
	//	}
	//}

	if (g_pFrm->m_CmdRun.ManalRun)
	{
		//g_pFrm->m_CmdRun.SynchronizeMoveXY(theApp.m_tSysParam.CleanPosition[0],theApp.m_tSysParam.CleanPosition[1],true);
		g_pFrm->m_CmdRun.CleanNeedle(false);
		g_pFrm->m_CmdRun.AdjustMotorToCleanPos();
	}
	SetAuthority(true);
}
// 保存吸真空位
void CDlgCtrl::OnBnClickedSaveCleanPos()
{
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能保存清洗位!");
		return;
	}
	if(AfxMessageBox("你确定保存新的清洗位吗？",MB_YESNO)!=IDYES)
	{
		return;
	}
	SetAuthority(false);
	for (int i=0;i<5;i++)
	{
		if(i<2)
		{
			theApp.m_tSysParam.CleanPosition[i]=theApp.m_Mv400.GetPos(i+1);
		}
		else
		{
			theApp.m_tSysParam.CleanPosition[i]=theApp.m_Mv400.GetPos(K_AXIS_ZA) + (g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[i] - g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]);
		}
	}
	theApp.SysParam(FALSE);
	UPDataVal();
	SetAuthority(true);
}
void CDlgCtrl::OnBnClickedClean()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能移动到清洗位!");
		return;
	}
	SetAuthority(false);
	if (g_pFrm->m_CmdRun.ManalRun)
	{
		g_pFrm->m_CmdRun.CleanNeedle(true);
	}
	SetAuthority(true);
}

void CDlgCtrl::OnBnClickedMoveStartImagePos()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能移动到开始取图位!");
		return;
	}
	if (g_pFrm->m_CmdRun.ManalRun)
	{
		g_pFrm->m_CmdRun.StartGrabImagePos();
	}
}

void CDlgCtrl::OnBnClickedMoveEndImagePos()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能移动到结束取图位!");
		return;
	}
	if (g_pFrm->m_CmdRun.ManalRun)
	{
		g_pFrm->m_CmdRun.EndGrabImagePos();
	}
}

void CDlgCtrl::OnBnClickedSaveTestDispPos()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能保存试胶位!");
		return;
	}
	if(AfxMessageBox("你确定保存新的试胶位吗？",MB_YESNO)!=IDYES)
	{
		return;
	}
	SetAuthority(false);
	g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[m_nTestIndex].x = theApp.m_Mv400.GetPos(K_AXIS_X);
	g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[m_nTestIndex].y = theApp.m_Mv400.GetPos(K_AXIS_Y);
	g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[m_nTestIndex].za = theApp.m_Mv400.GetPos(K_AXIS_ZA);
	g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[m_nTestIndex].zb = theApp.m_Mv400.GetPos(K_AXIS_ZA) + (g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[3] - g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]);
	g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[m_nTestIndex].zc = theApp.m_Mv400.GetPos(K_AXIS_ZA) + (g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[4] - g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]);
	theApp.ProductParam(FALSE);
	UPDataVal();
	SetAuthority(true);
}

void CDlgCtrl::OnBnClickedMoveTestDispPos()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能移动到试胶位!");
		return;
	}
	SetAuthority(false);	
	//if (!g_pFrm->m_CmdRun.IsRotatingLightClose())
	//{
	//	if (IDOK!=AfxMessageBox("旋转气缸未关闭到位，\n按 确定 忽略提示继续运动到目标位置"), MB_OKCANCEL)
	//	{
	//		return;
	//	}
	//}
	if (g_pFrm->m_CmdRun.ManalRun)
	{
		g_pFrm->m_CmdRun.TestDispense(m_nTestIndex,false);
	}
	SetAuthority(true);
}

void CDlgCtrl::OnBnClickedTestDisp()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能移动到试胶位!");
		return;
	}
	SetAuthority(false);
	if (g_pFrm->m_CmdRun.ManalRun)
	{
		g_pFrm->m_CmdRun.TestDispense();
	}
	SetAuthority(true);
}

void CDlgCtrl::OnBnClickedBtnAlignNeedle()
{
	SetAuthority(false);
	g_pFrm->m_dlgAlignNeedle.DoModal();
	theApp.ProductParam(FALSE);
	UPDataVal();
	SetAuthority(true);

	GetDlgItem(IDC_EDIT_ZOFFSET_ALL)->EnableWindow(true);
	GetDlgItem(IDC_BTN_ZOFFSET_APPLY)->EnableWindow(true);
}

void CDlgCtrl::OnBnClickedBtnMoveSlotDetectPos()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能移动到检测位!");
		return;
	}
	SetAuthority(false);	
	//if (!g_pFrm->m_CmdRun.IsRotatingLightClose())
	//{
	//	if (IDOK!=AfxMessageBox("旋转气缸未关闭到位，\n按 确定 忽略提示继续运动到目标位置"), MB_OKCANCEL)
	//	{
	//		return;
	//	}
	//}
	if (g_pFrm->m_CmdRun.ManalRun)
	{
		g_pFrm->m_CmdRun.SlotDetectPosition();
	}
	SetAuthority(true);
}

void CDlgCtrl::OnBnClickedBtnSaveSlotDetectPos()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能保存检测位!");
		return;
	}
	if(AfxMessageBox("你确定保存新的检测位吗？",MB_YESNO)!=IDYES)
	{
		return;
	}
	SetAuthority(false);
	g_pFrm->m_CmdRun.ProductParam.tSlotDetectPos.x = theApp.m_Mv400.GetPos(K_AXIS_X);
	g_pFrm->m_CmdRun.ProductParam.tSlotDetectPos.y = theApp.m_Mv400.GetPos(K_AXIS_Y);
	g_pFrm->m_CmdRun.ProductParam.tSlotDetectPos.za=theApp.m_Mv400.GetPos(K_AXIS_ZA);
	theApp.ProductParam(FALSE);
	UPDataVal();
	SetAuthority(true);
}

void CDlgCtrl::OnBnClickedBtnMoveDischargePos()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能移动到排胶位!");
		return;
	}
	SetAuthority(false);	
	//if (!g_pFrm->m_CmdRun.IsRotatingLightClose())
	//{
	//	if (IDOK!=AfxMessageBox("旋转气缸未关闭到位，\n按 确定 忽略提示继续运动到目标位置"), MB_OKCANCEL)
	//	{
	//		return;
	//	}
	//}
	if (g_pFrm->m_CmdRun.ManalRun)
	{
		g_pFrm->m_CmdRun.DischargePosition(false);
	}
	SetAuthority(true);
}

void CDlgCtrl::OnBnClickedBtnSaveDischargePos()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能保存排胶位!");
		return;
	}
	if(AfxMessageBox("你确定保存新的排胶位吗？",MB_YESNO)!=IDYES)
	{
		return;
	}
	SetAuthority(false);
	for (int i=0;i<5;i++)
	{
		if(i<2)
		{
			theApp.m_tSysParam.DischargeGluePostion[i]=theApp.m_Mv400.GetPos(i+1);
		}
		else
		{
			theApp.m_tSysParam.DischargeGluePostion[i]=theApp.m_Mv400.GetPos(K_AXIS_ZA) + (g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[i] - g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]);
		}
	}
	theApp.SysParam(FALSE);
	UPDataVal();
	SetAuthority(true);
}

void CDlgCtrl::OnBnClickedBtnDischargeGlue()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能移动到排胶位!");
		return;
	}
	SetAuthority(false);
	if (g_pFrm->m_CmdRun.ManalRun)
	{
		//g_pFrm->m_CmdRun.DischargePosition(true);  
		//UI 刷新，改用线程方式   2017-11-18
		g_pFrm->m_CmdRun.m_nCleanValveNum=-1;
		g_pFrm->m_CmdRun.bIsExitClean=false;
		g_pFrm->m_CmdRun.CleanThreadPlay();
		theApp.m_V9Control.V9C_SetProfile();
	}
	SetAuthority(true);
}

void CDlgCtrl::OnBnClickedBtnDefoam()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能移动到排胶位!");
		return;
	}
	SetAuthority(false);
	/*if (1==m_CheckAutoDefoam.GetCheck())
	{
		GetDlgItem(IDC_BTN_DEFOAM)->EnableWindow(FALSE);
		g_pFrm->m_CmdRun.DischargePosition(true,true);
		LONGLONG llDueDalay = CTimeUtil::GetDueTime(g_pFrm->m_CmdRun.ProductParam.dAutoDefoamTime*1000);
		while(!CTimeUtil::IsTimeout(llDueDalay))
		{
			Sleep(1);
		}

		for(int i=0;i<3;i++)
		{
			theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[i],FALSE);
		}
		int nReturn = theApp.m_V9Control.V9C_Rest(K_ALL_AXIS);
		if(nReturn>0)
		{
			g_pFrm->m_CmdRun.Alarm(nReturn+5);
		}
		else if(nReturn<0)
		{
			g_pFrm->m_CmdRun.Alarm(-nReturn+1);
		}
		g_pFrm->m_CmdRun.MoveToZSafety();
		GetDlgItem(IDC_BTN_DEFOAM)->EnableWindow(TRUE);
	}
	else
	{
		static bool bLabel = true;
		if (bLabel)
		{
			GetDlgItem(IDC_BTN_DEFOAM)->SetWindowText("停止脱泡");
			GetDlgItem(IDC_BTN_DEFOAM)->EnableWindow(FALSE);
			g_pFrm->m_CmdRun.DischargePosition(true,true);
			GetDlgItem(IDC_BTN_DEFOAM)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_BTN_DEFOAM)->SetWindowText("开始脱泡");
			GetDlgItem(IDC_BTN_DEFOAM)->EnableWindow(FALSE);
			for(int i=0;i<3;i++)
			{
				theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[i],FALSE);
			}
			int nReturn = theApp.m_V9Control.V9C_Rest(K_ALL_AXIS);
			if(nReturn>0)
			{
				g_pFrm->m_CmdRun.Alarm(nReturn+5);
			}
			else if(nReturn<0)
			{
				g_pFrm->m_CmdRun.Alarm(-nReturn+1);
			}
			g_pFrm->m_CmdRun.MoveToZSafety();
			GetDlgItem(IDC_BTN_DEFOAM)->EnableWindow(TRUE);
		}
		bLabel = !bLabel;

	}	*/
	static bool bLabel = true;
	if (bLabel)
	{
		GetDlgItem(IDC_BTN_DEFOAM)->SetWindowText("停止脱泡");
		GetDlgItem(IDC_BTN_DEFOAM)->EnableWindow(FALSE);
		g_pFrm->m_CmdRun.DischargePosition(true,true);
		GetDlgItem(IDC_BTN_DEFOAM)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BTN_DEFOAM)->SetWindowText("开始脱泡");
		GetDlgItem(IDC_BTN_DEFOAM)->EnableWindow(FALSE);
		for(int i=0;i<3;i++)
		{
			theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[i],FALSE);
		}
		int nReturn = theApp.m_V9Control.V9C_Rest(K_ALL_AXIS);
		if(nReturn>0)
		{
			g_pFrm->m_CmdRun.Alarm(nReturn+5);
		}
		else if(nReturn<0)
		{
			g_pFrm->m_CmdRun.Alarm(-nReturn+1);
		}
		g_pFrm->m_CmdRun.MoveToZSafety();
		GetDlgItem(IDC_BTN_DEFOAM)->EnableWindow(TRUE);
	}
	bLabel = !bLabel;
	SetAuthority(true);
}

void CDlgCtrl::OnBnClickedBtnSaveCurrentDispPos()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能保存A#点胶位!");
		return;
	}
	if (theApp.m_Mv400.IsMoveList())
	{
		AfxMessageBox("运动中，不能保存点胶位!");
		return;
	}
	if(AfxMessageBox("你确定保存新的A#点胶位吗？",MB_YESNO)!=IDYES)
	{
		return;
	}
	SetAuthority(false);
	for (int i=0;i<5;i++)
	{
		g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[i]=theApp.m_Mv400.GetPos(i+1);	
	}
	g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[3]=g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[2]+(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[3]-g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]);
    g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[4]=g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[2]+(g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[4]-g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]);
	theApp.ProductParam(FALSE);
	UPDataVal();
	SetAuthority(true);
}

void CDlgCtrl::OnBnClickedBtnSaveModifyDispPos()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能保存A#点胶位!");
		return;
	}
	if (theApp.m_Mv400.IsMoveList())
	{
		AfxMessageBox("运动中，不能保存点胶位!");
		return;

	}
	CString str;
	if(AfxMessageBox("你确定保存新的A#点胶位吗？",MB_YESNO)!=IDYES)
	{
		return;
	}
	SetAuthority(false);
	GetDlgItemText(IDC_PX1,str);
	g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[0] = atof(str);
	GetDlgItemText(IDC_PY1,str);
	g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[1] = atof(str);
	GetDlgItemText(IDC_PZA1,str);
	g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[2] = atof(str);
	GetDlgItemText(IDC_PZB1,str);
	g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[3] = atof(str);
	GetDlgItemText(IDC_PZC1,str);
	g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[4] = atof(str);
	theApp.ProductParam(FALSE);
	UPDataVal();
	SetAuthority(true);
}

void CDlgCtrl::OnDlgExit()
{
	OnCancel();
}

void CDlgCtrl::OnCancel()
{
	for (int i=0;i<3;i++)
	{
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[i],FALSE);
	}
	KillTimer(2001);
	g_pFrm->m_CmdRun.MoveToZSafety();
	g_pFrm->bIsCtrlDlgVisible = false;
	CDialog::OnCancel();
}

void CDlgCtrl::OnCbnSelchangeComboImageSelectIndex()
{
	// TODO: Add your control notification handler code here
	nImagePosID = m_cbImageSelect.GetCurSel();
	UPDataVal();
}

void CDlgCtrl::OnBnClickedBtnMoveImagePos()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能移动到取图位!");
		return;
	}
	SetAuthority(false);	
	//if (!g_pFrm->m_CmdRun.IsRotatingLightClose())
	//{
	//	if (IDOK!=AfxMessageBox("旋转气缸未关闭到位，\n按 确定 忽略提示继续运动到目标位置"), MB_OKCANCEL)
	//	{
	//		return;
	//	}
	//}
	if (g_pFrm->m_CmdRun.ManalRun)
	{
		g_pFrm->m_CmdRun.StartGrabImagePos();
	}
	SetAuthority(true);
}

void CDlgCtrl::OnBnClickedBtnSetImagePos()
{
	// TODO: Add your control notification handler code here
	SetAuthority(false);
	if(g_pFrm->m_dlgImagePos.DoModal()==IDOK)
	{
		theApp.ProductParam(FALSE);
		UPDataVal();
	}
	SetAuthority(true);
}

void CDlgCtrl::OnBnClickedBtnSearchImage()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能移动到取图位!");
		return;
	}
	SetAuthority(false);
	if (g_pFrm->m_CmdRun.ManalRun)
	{
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,TRUE);
		g_pFrm->m_CmdRun.m_tStatus = K_RUN_STS_RUN;
		tgPos tgOffset;
		double dAngle = 0;
		g_pFrm->m_CmdRun.FindImageOffset(tgOffset.x,tgOffset.y,dAngle,true);
		//g_pFrm->m_CmdRun.FindTwoMark();
		g_pFrm->m_CmdRun.m_tStatus = K_RUN_STS_STOP;
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,FALSE);
	}
	SetAuthority(true);
}

void CDlgCtrl::OnCbnSelchangeComboTestIndex()
{
	// TODO: Add your control notification handler code here
	m_nTestIndex = m_cbTestIndex.GetCurSel();
	UPDataVal();
}

void CDlgCtrl::OnBnClickedSingleTestDispense()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能移动到试胶位!");
		return;
	}
	SetAuthority(false);
	if (g_pFrm->m_CmdRun.ManalRun)
	{
		g_pFrm->m_CmdRun.TestDispense(m_nTestIndex,true);
	}
	SetAuthority(true);
}

void CDlgCtrl::OnBnClickedSetTestDispense()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能设置试胶位!");
		return;
	}
	SetAuthority(false);
	if (g_pFrm->m_CmdRun.ManalRun)
	{
		CDlgSetPosition dlgSetPos;
		if(IDOK==dlgSetPos.DoModal())
		{
			theApp.ProductParam(FALSE);
			UPDataVal();
		}
	}
	SetAuthority(true);
}

void CDlgCtrl::SetAuthority()
{
	GetDlgItem(IDC_SAVE_CLEAN_POS)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_BtnModifyClean)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);

	GetDlgItem(IDC_SAVE_TEST_DISP_POS)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_SET_TEST_DISPENSE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_BTN_SAVE_TESTZ_EACH)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);

	GetDlgItem(IDC_BTN_SAVE_DISCHARGE_POS)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_BtnModifyDischarge)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);

	GetDlgItem(IDC_BTN_SET_IMAGE_POS)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_BTN_SAVE_SLOT_DETECT_POS)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);

	GetDlgItem(IDC_BtnModifyDischarge)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_BTN_CTRL_SET_HEIGHT_POS)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_BTN_CTRL_SAVE_HEIGHT_DATA)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);

	GetDlgItem(IDC_SAVE_DISPENSING_POS)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_BTN_SAVE_CURRENT_DISP_POS)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_BTN_SAVE_MODIFY_DISP_POS)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_BTN_ALIGN_NEEDLE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
}
void CDlgCtrl::OnBnClickedBtnCtrlMovetoHeightPos()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能移动到测高位!");
		return;
	}

	SetAuthority(false);
	//if (!g_pFrm->m_CmdRun.IsRotatingLightClose())
	//{
	//	if (IDOK!=AfxMessageBox("旋转气缸未关闭到位，\n按 确定 忽略提示继续运动到目标位置"), MB_OKCANCEL)
	//	{
	//		return;
	//	}
	//}
	if(g_pFrm->m_CmdRun.ManalRun)
	{
		g_pFrm->m_CmdRun.MoveToHeightPosition(m_nHeightIndex);
	}
	SetAuthority(true);
}

void CDlgCtrl::OnBnClickedBtnCtrlSetHeightPos()
{
	/*if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能测高!");
		return;
	}
	else if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_PAUSE)
	{
		AfxMessageBox("暂停状态不能测高!");
		return;
	}
	if (g_pFrm->m_CmdRun.ManalRun)
	{
		SetAuthority(false);
		CDlgHeightPosition dlgHeight;
		dlgHeight.DoModal();
		UPDataVal();
		theApp.ProductParam(FALSE);
		SetAuthority(true);
	}*/
	g_pFrm->m_CmdRun.ProductParam.tgMeasureHeightPos[m_nHeightIndex].x = theApp.m_Mv400.GetPos(K_AXIS_X);
	g_pFrm->m_CmdRun.ProductParam.tgMeasureHeightPos[m_nHeightIndex].y = theApp.m_Mv400.GetPos(K_AXIS_Y);
	g_pFrm->m_CmdRun.ProductParam.tgMeasureHeightPos[m_nHeightIndex].za = g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5].za;
	theApp.ProductParam(FALSE);
}

void CDlgCtrl::OnBnClickedBtnCtrlMeasureHeight()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能测高!");
		return;
	}
	else if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_PAUSE)
	{
		AfxMessageBox("暂停状态不能测高!");
		return;
	}
	if(g_pFrm->m_CmdRun.ManalRun)
	{
		SetAuthority(false);
		g_pFrm->m_CmdRun.m_tStatus = K_RUN_STS_RUN;
		double dValue = 0.0;
		g_pFrm->m_CmdRun.MeasureHeight(dValue);
		g_pFrm->m_CmdRun.m_tStatus = K_RUN_STS_STOP;
		SetAuthority(true);
	}
}

void CDlgCtrl::OnCbnSelchangeComboCtrlHeightIndex()
{
	// TODO: Add your control notification handler code here
	m_nHeightIndex = m_cbHeightNumber.GetCurSel();
	UPDataVal();
}

void CDlgCtrl::SetAuthority(bool bEnable)
{
	//GetDlgItem(IDC_MOVE_DISPENSING_POS)->EnableWindow(bEnable);
	//GetDlgItem(IDC_BTN_ALIGN_NEEDLE)->EnableWindow(bEnable);
	//GetDlgItem(IDC_SAVE_DISPENSING_POS)->EnableWindow(bEnable);
	//GetDlgItem(IDC_BTN_SAVE_CURRENT_DISP_POS)->EnableWindow(bEnable);
	//GetDlgItem(IDC_BTN_SAVE_MODIFY_DISP_POS)->EnableWindow(bEnable);

	//GetDlgItem(IDC_MOVE_CLEAN_POS)->EnableWindow(bEnable);
	//GetDlgItem(IDC_CLEAN)->EnableWindow(bEnable);
	//GetDlgItem(IDC_SAVE_CLEAN_POS)->EnableWindow(bEnable);
	//GetDlgItem(IDC_MOVE_ADJUST_CLEAN)->EnableWindow(bEnable);
	//GetDlgItem(IDC_MOVE_ADJUST_NEEDLE)->EnableWindow(bEnable);

	//GetDlgItem(IDC_MOVE_TEST_DISP_POS)->EnableWindow(bEnable);
	//GetDlgItem(IDC_SAVE_TEST_DISP_POS)->EnableWindow(bEnable);
	//GetDlgItem(IDC_SINGLE_TEST_DISPENSE)->EnableWindow(bEnable);
	//GetDlgItem(IDC_TEST_DISP)->EnableWindow(bEnable);
	//GetDlgItem(IDC_SET_TEST_DISPENSE)->EnableWindow(bEnable);

	//GetDlgItem(IDC_BTN_MOVE_DISCHARGE_POS)->EnableWindow(bEnable);
	//GetDlgItem(IDC_BTN_SAVE_DISCHARGE_POS)->EnableWindow(bEnable);
	//GetDlgItem(IDC_BTN_DISCHARGE_GLUE)->EnableWindow(bEnable);
	//GetDlgItem(IDC_BTN_DEFOAM)->EnableWindow(bEnable);

	//GetDlgItem(IDC_BTN_MOVE_IMAGE_POS)->EnableWindow(bEnable);
	//GetDlgItem(IDC_BTN_SET_IMAGE_POS)->EnableWindow(bEnable);
	//GetDlgItem(IDC_BTN_SEARCH_IMAGE)->EnableWindow(bEnable);

	//GetDlgItem(IDC_BTN_MOVE_SLOT_DETECT_POS)->EnableWindow(bEnable);
	//GetDlgItem(IDC_BTN_SAVE_SLOT_DETECT_POS)->EnableWindow(bEnable);

	//GetDlgItem(IDC_BTN_CTRL_MOVETO_HEIGHT_POS)->EnableWindow(bEnable);
	//GetDlgItem(IDC_BTN_CTRL_SET_HEIGHT_POS)->EnableWindow(bEnable);
	//GetDlgItem(IDC_BTN_CTRL_MEASURE_HEIGHT)->EnableWindow(bEnable);

	//GetDlgItem(IDC_BtnModifyClean)->EnableWindow(bEnable);
	//GetDlgItem(IDC_BtnModifyDischarge)->EnableWindow(bEnable);

	//GetDlgItem(IDC_BTN_MOVE_FOCUS_POS)->EnableWindow(bEnable);
	//GetDlgItem(IDC_BTN_SAVE_MODIFY_FOCUS)->EnableWindow(bEnable);
	//GetDlgItem(IDC_BTN_SAVE_CURRENT_FOCUS_POS)->EnableWindow(bEnable);

}

void CDlgCtrl::OnBnClickedMoveAdjustClean()
{
	// TODO: 在此添加控件通知处理程序代码
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能移动针头!");
		return;
	}
	SetAuthority(false);

	if (g_pFrm->m_CmdRun.ManalRun)
	{
		//g_pFrm->m_CmdRun.SynchronizeMoveXY(theApp.m_tSysParam.CleanPosition[0],theApp.m_tSysParam.CleanPosition[1],true);
		g_pFrm->m_CmdRun.AdjustMotorToCleanPos();
	}

	SetAuthority(true);
}

void CDlgCtrl::OnBnClickedMoveAdjustNeedle()
{
	// TODO: 在此添加控件通知处理程序代码
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能移动针头!");
		return;
	}
	SetAuthority(false);

	if (g_pFrm->m_CmdRun.ManalRun)
	{
		//g_pFrm->m_CmdRun.AdjustNeedle();
		g_pFrm->m_CmdRun.AdjustNeedleAccurate();
	}

	SetAuthority(true);
}


void CDlgCtrl::OnBnClickedBtnmodifydischarge()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能保存排胶位!");
		return;
	}
	CString str;
	if(AfxMessageBox("你确定保存新的排胶位吗？",MB_YESNO)!=IDYES)
	{
		return;
	}
	SetAuthority(false);
	GetDlgItemText(IDC_PX4,str);
	theApp.m_tSysParam.DischargeGluePostion[0]=atof(str);
	GetDlgItemText(IDC_PY4,str);
	theApp.m_tSysParam.DischargeGluePostion[1]=atof(str);
	GetDlgItemText(IDC_PZA4,str);
	theApp.m_tSysParam.DischargeGluePostion[2]=atof(str);
	GetDlgItemText(IDC_PZB4,str);
	theApp.m_tSysParam.DischargeGluePostion[3]=atof(str);
	GetDlgItemText(IDC_PZC4,str);
	theApp.m_tSysParam.DischargeGluePostion[4]=atof(str);
	theApp.ProductParam(FALSE);
	UPDataVal();
	SetAuthority(true);

}

void CDlgCtrl::OnBnClickedBtnmodifyclean()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能保存清洗位!");
		return;
	}
	CString str;
	if(AfxMessageBox("你确定保存新的清洗位吗？",MB_YESNO)!=IDYES)
	{
		return;
	}
	SetAuthority(false);
	GetDlgItemText(IDC_PX2,str);
	theApp.m_tSysParam.CleanPosition[0]=atof(str);
	GetDlgItemText(IDC_PY2,str);
	theApp.m_tSysParam.CleanPosition[1]=atof(str);
	GetDlgItemText(IDC_PZA2,str);
	theApp.m_tSysParam.CleanPosition[2]=atof(str);
	GetDlgItemText(IDC_PZB2,str);
	theApp.m_tSysParam.CleanPosition[3]=atof(str);
	GetDlgItemText(IDC_PZC2,str);
	theApp.m_tSysParam.CleanPosition[4]=atof(str);
	theApp.ProductParam(FALSE);
	UPDataVal();
	SetAuthority(true);
}

void CDlgCtrl::OnBnClickedBtnMoveFocusPos()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能移动到相机焦距位!");
		return;
	}
	SetAuthority(false);	
	if (g_pFrm->m_CmdRun.ManalRun)
	{
		g_pFrm->m_CmdRun.SynchronizeMoveZ(theApp.m_tSysParam.dFocusZPos,true);
	}
	SetAuthority(true);
}

void CDlgCtrl::OnBnClickedBtnSaveCurrentFocusPos()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能保存相机焦距位!");
		return;
	}
	if(AfxMessageBox("你确定保存新的相机焦距位吗？",MB_YESNO)!=IDYES)
	{
		return;
	}
	SetAuthority(false);
	theApp.m_tSysParam.dFocusZPos=theApp.m_Mv400.GetPos(K_AXIS_ZA);
	theApp.SysParam(FALSE);
	theApp.ProductParam(FALSE);
	UPDataVal();
	SetAuthority(true);
}

void CDlgCtrl::OnBnClickedBtnSaveModifyFocus()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能保存相机焦距位!");
		return;
	}
	CString str;
	if(AfxMessageBox("你确定保存新的相机焦距位吗？",MB_YESNO)!=IDYES)
	{
		return;
	}
	SetAuthority(false);
	GetDlgItemText(IDC_ZFOCUSA,str);
	theApp.m_tSysParam.dFocusZPos=atof(str);
	theApp.ProductParam(FALSE);
	theApp.SysParam(FALSE);
	UPDataVal();
	SetAuthority(true);
}

void CDlgCtrl::OnBnClickedBtnSaveTestzEach()
{
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		AfxMessageBox("运动状态不能保存试胶位!");
		return;
	}
	if(AfxMessageBox("你确定分开保存新的试胶位吗？",MB_YESNO)!=IDYES)
	{
		return;
	}
	SetAuthority(false);
	g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[m_nTestIndex].x = theApp.m_Mv400.GetPos(K_AXIS_X);
	g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[m_nTestIndex].y = theApp.m_Mv400.GetPos(K_AXIS_Y);
	g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[m_nTestIndex].za = theApp.m_Mv400.GetPos(K_AXIS_ZA);
	g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[m_nTestIndex].zb = theApp.m_Mv400.GetPos(K_AXIS_ZB);
	g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[m_nTestIndex].zc = theApp.m_Mv400.GetPos(K_AXIS_ZC);
	theApp.ProductParam(FALSE);
	UPDataVal();
	SetAuthority(true);

}

void CDlgCtrl::OnBnClickedBtnStopDischarge()
{
	g_pFrm->m_CmdRun.CleanSuspend();
	g_pFrm->m_CmdRun.bIsExitClean=true;
	g_pFrm->m_CmdRun.m_pMv->WaitStop(K_AXIS_U,10000);
	g_pFrm->m_CmdRun.m_pMv->WaitStop(K_AXIS_W,10000);
	g_pFrm->m_CmdRun.m_pMv->WaitStop(K_AXIS_V,10000);

	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[0],FALSE);//关气
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[1],FALSE);//关气	
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[2],FALSE);//关气	
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[0],TRUE);//开阀
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[1],TRUE);//开阀
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[2],TRUE);//开阀
}

void CDlgCtrl::OnBnClickedBtnZoffsetApply()
{
	CString str;
	double dValue;
	GetDlgItemText(IDC_EDIT_ZOFFSET_ALL,str);
	dValue=atof(str);

	GetDlgItem(IDC_EDIT_ZOFFSET_ALL)->EnableWindow(false);
	GetDlgItem(IDC_BTN_ZOFFSET_APPLY)->EnableWindow(false);
	if(fabs(dValue)>10)
	{
		g_pFrm->m_CmdRun.ProductParam.dZDispOffsetAll=-10;
		theApp.ProductParam(FALSE);
		AfxMessageBox("数据超限!");
		return;
	}

	g_pFrm->m_CmdRun.ProductParam.dZDispOffsetAll=atof(str);
	theApp.ProductParam(FALSE);
	UPDataVal();
	////
	g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[2]+=g_pFrm->m_CmdRun.ProductParam.dZDispOffsetAll;
	g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[3]+=g_pFrm->m_CmdRun.ProductParam.dZDispOffsetAll;
	g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[4]+=g_pFrm->m_CmdRun.ProductParam.dZDispOffsetAll;
	theApp.ProductParam(FALSE);
	UPDataVal();	
}

void CDlgCtrl::OnBnClickedBtnValveHeat()
{
	CDlgValveHeat dlg;
	dlg.DoModal();
}

void CDlgCtrl::OnBnClickedBtnCtrlSaveHeightData()
{
	double dHeightValue;
	dHeightValue=0;

	if(g_pFrm->m_CmdRun.ReadHeightSensorValue(&dHeightValue))
		g_pFrm->m_CmdRun.ProductParam.dLaserDataBase[m_nHeightIndex]=dHeightValue;
	theApp.ProductParam(FALSE);
}
