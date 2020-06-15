// DlgProudSet.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgProudSet.h"
#include <math.h>


// CDlgProudSet dialog

IMPLEMENT_DYNAMIC(CDlgProudSet, CDialog)

CDlgProudSet::CDlgProudSet(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProudSet::IDD, pParent)
{
	CarmUseChange=false;
}

CDlgProudSet::~CDlgProudSet()
{
}

void CDlgProudSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_NOFLOW, m_DisFopen);
	DDX_Control(pDX, IDC_CHECK_CLEAN_NEEDLE, m_CleanNeedle);
	DDX_Control(pDX, IDC_CHECK_FIRST_TEST, m_FirstTest);
	DDX_Control(pDX, IDC_CHECK_SLOT_DETECT, m_SlotDetect);
	DDX_Control(pDX, IDC_CHECK_FULL_DISCHARGE, m_FullDischarge);
	DDX_Control(pDX, IDC_CHECK_IRREGULAR_MATRIX, m_IrregularMatrix);
	DDX_Control(pDX, IDC_COMBO_IRREGULAR_ROW, m_cbCurrentRow);
	DDX_Control(pDX, IDC_COMBO_IRREGULAR_COL, m_cbCurrentColumn);
	//DDX_Control(pDX, IDC_CHECK_SINGLE_DISP, m_SingleValve);
	DDX_Control(pDX, IDC_CHECK_FULL_CLEAN,m_FullClean);
	DDX_Control(pDX, IDC_CHECK_SWITCH_ENABLE,m_DoorSwitch);
	DDX_Control(pDX, IDC_CHECK_DISP_FINISH_FILL,m_DispFinishFill);
	DDX_Control(pDX, IDC_CHECK_CLEAN_DISCHARGE,m_CleanDischarge);
	DDX_Control(pDX, IDC_CHECK_PAUSE_CLEAN_TEST,m_PauseCleanTest);
	DDX_Control(pDX, IDC_CHECK_MEASURE_HEIGHT,m_MeasureHeight);
	DDX_Control(pDX, IDC_CHECK_ADJUST_NEEDLE,m_AutoAdjust);
	DDX_Control(pDX, IDC_CHECK_TEMP_JUDGE, m_TempJudge);
	DDX_Control(pDX, IDC_CHECK_HEAT_ENABLE, m_HeatEnable);
	DDX_Control(pDX, IDC_CHECK_MES_OFFLINE, m_MesOffLine);
	DDX_Control(pDX, IDC_CHECK_NEEDLECHECK, m_NeedleCheck);
	DDX_Control(pDX, IDC_IPADDRESS_DUP, m_IPAddress);
}
// CDlgProudSet message handlers
BEGIN_MESSAGE_MAP(CDlgProudSet, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgProudSet::OnBnClickedOk)
	ON_BN_CLICKED(IDC_FA, &CDlgProudSet::OnBnClickedFA)
	ON_BN_CLICKED(IDC_FB, &CDlgProudSet::OnBnClickedFB)
	ON_BN_CLICKED(IDC_FC, &CDlgProudSet::OnBnClickedFC)
	//ON_BN_CLICKED(IDC_FD, &CDlgProudSet::OnBnClickedFD)
	ON_EN_CHANGE(IDC_EDIT_PAUSE_POS, &CDlgProudSet::OnEnChangePausePos)
	ON_BN_CLICKED(IDC_BTN_FIRST_ROWD, &CDlgProudSet::OnBnClickedBtnFirstRowd)
	ON_BN_CLICKED(IDC_BTN_SECOND_ROWD, &CDlgProudSet::OnBnClickedBtnSecondRowd)
	ON_BN_CLICKED(IDC_BTN_FIRST_COLUMND, &CDlgProudSet::OnBnClickedBtnFirstColumnd)
	ON_BN_CLICKED(IDC_BTN_SECOND_COLUMND, &CDlgProudSet::OnBnClickedBtnSecondColumnd)
	ON_BN_CLICKED(IDC_RADIO_DOT, &CDlgProudSet::OnBnClickedRadioDot)
	ON_BN_CLICKED(IDC_RADIO_RECTANGLE, &CDlgProudSet::OnBnClickedRadioRectangle)
	ON_BN_CLICKED(IDC_RADIO_HELIX, &CDlgProudSet::OnBnClickedRadioHelix)
	ON_BN_CLICKED(IDC_RADIO_LINE_VERTICAL, &CDlgProudSet::OnBnClickedRadioLineVertical)
	ON_BN_CLICKED(IDC_RADIO_LINE_HORIZONTAL, &CDlgProudSet::OnBnClickedRadioLineHorizontal)
	ON_BN_CLICKED(IDC_RADIO_CUSTOM, &CDlgProudSet::OnBnClickedRadioCustom)
	ON_BN_CLICKED(IDC_RADIO_IMAGE_NO, &CDlgProudSet::OnBnClickedRadioImageNo)
	ON_BN_CLICKED(IDC_RADIO_IMAGE_ONE_MARK, &CDlgProudSet::OnBnClickedRadioImageOneMark)
	ON_BN_CLICKED(IDC_RADIO_IMAGE_TWO_MARK, &CDlgProudSet::OnBnClickedRadioImageTwoMark)
	ON_BN_CLICKED(IDC_BTN_CUSTOM_PARAM, &CDlgProudSet::OnBnClickedBtnCustomParam)
	ON_BN_CLICKED(IDC_BTN_THIRD_ROWD, &CDlgProudSet::OnBnClickedBtnThirdRowd)
	ON_BN_CLICKED(IDC_BTN_THIRD_COLUMND, &CDlgProudSet::OnBnClickedBtnThirdColumnd)
	ON_CBN_SELCHANGE(IDC_COMBO_IRREGULAR_ROW, &CDlgProudSet::OnCbnSelchangeComboCurrentRow)
	ON_CBN_SELCHANGE(IDC_COMBO_IRREGULAR_COL, &CDlgProudSet::OnCbnSelchangeComboCurrentColumn)
	ON_BN_CLICKED(IDC_BTN_SAVE_IRREGULAR_ROWD, &CDlgProudSet::OnBnClickedBtnSaveCurrentRowd)
	ON_BN_CLICKED(IDC_BTN_SAVE_IRREGULAR_COLUMND, &CDlgProudSet::OnBnClickedBtnSaveCurrentColumnd)
	ON_BN_CLICKED(IDC_RADIO_NO_INSPECT, &CDlgProudSet::OnBnClickedRadioNoInspect)
	ON_BN_CLICKED(IDC_RADIO_SCAN_INSPECT, &CDlgProudSet::OnBnClickedRadioScanInspect)
	ON_BN_CLICKED(IDC_RADIO_ROW_DISP, &CDlgProudSet::OnBnClickedRadioRowDisp)
	ON_BN_CLICKED(IDC_RADIO_COLUMN_DISP, &CDlgProudSet::OnBnClickedRadioColumnDisp)
	ON_BN_CLICKED(IDC_RADIO_FIRST_CLEAN_LATER_TEST, &CDlgProudSet::OnBnClickedRadioFirstCleanLaterTest)
	ON_BN_CLICKED(IDC_RADIO_FIRST_TEST_LATER_CLEAN, &CDlgProudSet::OnBnClickedRadioFirstTestLaterClean)
	ON_BN_CLICKED(IDC_RADIO_ONE_VALVE, &CDlgProudSet::OnBnClickedRadioOneValve)
	ON_BN_CLICKED(IDC_RADIO_TWO_VALVE, &CDlgProudSet::OnBnClickedRadioTwoValve)
	ON_BN_CLICKED(IDC_RADIO_THREE_VALVE, &CDlgProudSet::OnBnClickedRadioThreeValve)
	ON_BN_CLICKED(IDC_RADIO_GRAYSCALE, &CDlgProudSet::OnBnClickedRadioGrayscale)
	ON_BN_CLICKED(IDC_RADIO_GEOMETRIC, &CDlgProudSet::OnBnClickedRadioGeometric)
	ON_BN_CLICKED(IDC_RADIO_TRANSITION_INSPECT, &CDlgProudSet::OnBnClickedRadioTransitionInspect)
	ON_BN_CLICKED(IDC_RADIO_CIRCLE, &CDlgProudSet::OnBnClickedRadioCircle)
	ON_BN_CLICKED(IDC_RADIO_SECTION_INSPECT, &CDlgProudSet::OnBnClickedRadioSectionInspect)
	ON_BN_CLICKED(IDC_RADIO_MULTI, &CDlgProudSet::OnBnClickedRadioMulti)
	//ON_BN_CLICKED(IDC_BTN_NEEDLEGAP, &CDlgProudSet::OnBnClickedBtnNeedlegap)
	ON_BN_CLICKED(IDC_RADIO_COMPLEX, &CDlgProudSet::OnBnClickedRadioComplex)
	ON_BN_CLICKED(IDCANCEL, &CDlgProudSet::OnBnClickedCancel)
		
	ON_BN_CLICKED(IDC_CHECK_MES_OFFLINE, &CDlgProudSet::OnBnClickedCheckMesOffline)
	ON_BN_CLICKED(IDC_BUTTON_UDP_RESET, &CDlgProudSet::OnBnClickedButtonUdpReset)
	ON_BN_CLICKED(IDC_UDP_SAVEURL, &CDlgProudSet::OnBnClickedUdpSaveurl)
END_MESSAGE_MAP()
// 确定，保存参数
void CDlgProudSet::OnBnClickedOk()
{
	updataparam();
	theApp.ProductParam(FALSE);
	g_pView->m_ImgStatic.Invalidate();
	OnOK();
}
// 初始化
BOOL CDlgProudSet::OnInitDialog()
{
	CDialog::OnInitDialog();
	CheckRadioButton(IDC_RADIO_DOT,IDC_RADIO_COMPLEX,IDC_RADIO_DOT+(int)g_pFrm->m_CmdRun.ProductParam.fillMode);
	CheckRadioButton(IDC_RADIO_IMAGE_NO,IDC_RADIO_IMAGE_TWO_MARK,IDC_RADIO_IMAGE_NO+(int)g_pFrm->m_CmdRun.ProductParam.visionMode);
	CheckRadioButton(IDC_RADIO_NO_INSPECT,IDC_RADIO_SECTION_INSPECT,IDC_RADIO_NO_INSPECT+(int)g_pFrm->m_CmdRun.ProductParam.inspectMode);
	CheckRadioButton(IDC_RADIO_ROW_DISP,IDC_RADIO_COLUMN_DISP,IDC_RADIO_ROW_DISP+(int)g_pFrm->m_CmdRun.ProductParam.dispOrientation);
	CheckRadioButton(IDC_RADIO_FIRST_CLEAN_LATER_TEST,IDC_RADIO_FIRST_TEST_LATER_CLEAN,IDC_RADIO_FIRST_CLEAN_LATER_TEST+(int)g_pFrm->m_CmdRun.ProductParam.dispSequence);
	CheckRadioButton(IDC_RADIO_ONE_VALVE,IDC_RADIO_THREE_VALVE,IDC_RADIO_ONE_VALVE+(int)g_pFrm->m_CmdRun.ProductParam.valveSelect);
	CheckRadioButton(IDC_RADIO_GRAYSCALE,IDC_RADIO_GEOMETRIC,IDC_RADIO_GRAYSCALE+(int)g_pFrm->m_CmdRun.ProductParam.matchMode);
    
	// TODO:  Add extra initialization here
	CString str;
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.FirstDispRow);
	SetDlgItemText(IDC_EDIT_FIRST_ROW,str);
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.FirstDispColumn);
	SetDlgItemText(IDC_EDIT_FIRST_COLUMN,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.FirstDispRowD);
	SetDlgItemText(IDC_EDIT_FIRST_ROWD,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.FirstDispColumnD);
	SetDlgItemText(IDC_EDIT_FIRST_COLUMND,str);
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.SecondDispRow);
	SetDlgItemText(IDC_EDIT_SECOND_ROW,str);
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.SecondDispColumn);
	SetDlgItemText(IDC_EDIT_SECOND_COLUMN,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.SecondDispRowD);
	SetDlgItemText(IDC_EDIT_SECOND_ROWD,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.SecondDispColumnD);
	SetDlgItemText(IDC_EDIT_SECOND_COLUMND,str);
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.ThirdDispRow);
	SetDlgItemText(IDC_EDIT_THIRD_ROW,str);
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn);
	SetDlgItemText(IDC_EDIT_THIRD_COLUMN,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD);
	SetDlgItemText(IDC_EDIT_THIRD_ROWD,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD);
	SetDlgItemText(IDC_EDIT_THIRD_COLUMND,str);
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.MultiDispRow);
	SetDlgItemText(IDC_EDIT_MULTI_ROW_NUMBER,str);
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.MultiDispColumn);
	SetDlgItemText(IDC_EDIT_MULTI_COLUMN_NUMBER,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.MultiDispRowD);
	SetDlgItemText(IDC_EDIT_MULTI_ROW_DISTANCE,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.MultiDispColumnD);
	SetDlgItemText(IDC_EDIT_MULTI_COLUMN_DISTANCE,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dPreTime);
	SetDlgItemText(IDC_EDIT_PRE_DELAY,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dPostDisTime);
	SetDlgItemText(IDC_EDIT_POST_DELAY,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dZupHigh);
	SetDlgItemText(IDC_EDIT_LIFT_HEIGHT,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dPauseHeight);
	SetDlgItemText(IDC_EDIT_PAUSE_POS,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dPauseTime);
	SetDlgItemText(IDC_EDIT_PAUSE_TIME,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dFirstDispDelay);
	SetDlgItemText(IDC_EDIT_FIRST_DISPDELAY,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dDispRadius);
	SetDlgItemText(IDC_EDIT_DISP_RADIUS,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dDistance);
	SetDlgItemText(IDC_EDIT_DISTANCE,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay);
	SetDlgItemText(IDC_EDIT_GRABIMAGE_DELAY,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dCompAngle);
	SetDlgItemText(IDC_EDIT_COMPANGLE,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dTestDotDelay);
	SetDlgItemText(IDC_EDIT_TEST_DELAY,str);
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.TestDotNumber);
	SetDlgItemText(IDC_EDIT_TEST_NUMBER,str);
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.nLackPadCmpValue);//2016-01-16
	SetDlgItemText(IDC_EDIT_LACK_CMPVALUE,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dHeightError);
	SetDlgItemText(IDC_EDIT_HEIGHT_ERROR,str);
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.HeightNumber);
	SetDlgItemText(IDC_EDIT_HEIGHT_NUMBER,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dHeightDelay);
	SetDlgItemText(IDC_EDIT_HEIGHT_DELAY,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dHeightCompMax);
	SetDlgItemText(IDC_EDIT_HEIGHT_COMPMAX,str);
	//str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dDispenseHeight);
	//SetDlgItemText(IDC_EDIT_DISPENSE_HEIGHT,str);

	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.CircleVel);
	SetDlgItemText(IDC_EDIT_CIRCLE_VEL,str);
	GetDlgItem(IDC_EDIT_CIRCLE_VEL)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.CircleAcc);
	SetDlgItemText(IDC_EDIT_CIRCLE_ACC,str);
	GetDlgItem(IDC_EDIT_CIRCLE_ACC)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dCleanDelay);
	SetDlgItemText(IDC_EDIT_CLEAN_DELAY,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.LineLength);
	SetDlgItemText(IDC_EDIT_LINE_LENGTH,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.RectWidth);
	SetDlgItemText(IDC_EDIT_RECT_WIDTH,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.RectLength);
	SetDlgItemText(IDC_EDIT_RECT_LENGTH,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.LineVel);
	SetDlgItemText(IDC_EDIT_LINE_VEL,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.LineAcc);
	SetDlgItemText(IDC_EDIT_LINE_ACC,str);

	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.DispOffsetX);
	SetDlgItemText(IDC_EDIT_DISP_OFFSET_X,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.DispOffsetY);
	SetDlgItemText(IDC_EDIT_DISP_OFFSET_Y,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.MaxOffsetAngle);
	SetDlgItemText(IDC_EDIT_MAX_OFFSET_ANGLE,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.MaxOffsetX);
	SetDlgItemText(IDC_EDIT_MAX_ADJUST_X,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.MaxOffsetY);
	SetDlgItemText(IDC_EDIT_MAX_ADJUST_Y,str);

	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.ScanDelay);
	SetDlgItemText(IDC_EDIT_SCAN_DELAY,str);

	str.Format("%0.1f",g_pFrm->m_CmdRun.ProductParam.IrregularSlotLength);
	SetDlgItemText(IDC_EDIT_SLOT_LENGTH,str);
	str.Format("%0.1f",g_pFrm->m_CmdRun.ProductParam.IrregularSlotWidth);
	SetDlgItemText(IDC_EDIT_SLOT_WIDTH,str);
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.IrregularRow);
	SetDlgItemText(IDC_EDIT_IRREGULAR_ROW_NUM,str);
	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.IrregularColumn);
	SetDlgItemText(IDC_EDIT_IRREGULAR_COL_NUM,str);
	for(int i=1;i<g_pFrm->m_CmdRun.ProductParam.IrregularRow;i++)
	{
		str.Format("%d",i);
		m_cbCurrentRow.AddString(str);
	}
	if(g_pFrm->m_CmdRun.ProductParam.IrregularRow>1)
	{
		m_cbCurrentRow.SetCurSel(0);
		m_nSelRow = m_cbCurrentRow.GetCurSel();
		str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.IrregularRowD[m_nSelRow]);
		SetDlgItemText(IDC_EDIT_IRREGULAR_ROW_DISTANCE,str);
		m_cbCurrentRow.GetLBText(m_nSelRow,str);
		int nextRow = atoi(str)+1;
		str.Format("%d",nextRow);
		SetDlgItemText(IDC_EDIT_IRREGULAR_NEXT_ROW,str);
	}
	for(int i=1;i<g_pFrm->m_CmdRun.ProductParam.IrregularColumn;i++)
	{
		str.Format("%d",i);
		m_cbCurrentColumn.AddString(str);
	}
	if(g_pFrm->m_CmdRun.ProductParam.IrregularColumn>1)
	{
		m_cbCurrentColumn.SetCurSel(0);
		m_nSelColumn = m_cbCurrentColumn.GetCurSel();
		str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.IrregularColumnD[m_nSelColumn]);
		SetDlgItemText(IDC_EDIT_IRREGULAR_COL_DISTANCE,str);
		m_cbCurrentColumn.GetLBText(m_nSelColumn,str);
		int nextColumn = atoi(str)+1;
		str.Format("%d",nextColumn);
		SetDlgItemText(IDC_EDIT_IRREGULAR_NEXT_COL,str);
	}

	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.NeedleGap);
	SetDlgItemText(IDC_EDIT_NEEDLE_PITCH,str);

	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dComposedSecondY);
	SetDlgItemText(IDC_EDIT_COMPOSE_SECONDY,str);

	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dComposedThirdY);
	SetDlgItemText(IDC_EDIT_COMPOSE_THIRDY,str);

	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dComposeSecondX);
	SetDlgItemText(IDC_EDIT_COMPOSE_SECONDX,str);

	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dComposedThirdX);
	SetDlgItemText(IDC_EDIT_COMPOSE_THIRDX,str);

	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.dTempError);
	SetDlgItemText(IDC_EDIT_TEMP_ERROR,str);

	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.lGasketRingLife);
	SetDlgItemText(IDC_EDIT_GASKETRING_LIFE,str);

	str.Format("%ld",g_pFrm->m_CmdRun.ProductParam.ContactFrequency);
    SetDlgItemText(IDC_EDIT_CONTACT_FREQ,str);

    str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.ContactErrPermit);
	SetDlgItemText(IDC_EDIT_CONTACT_ERRPERMIT,str);

	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.MesDataMin);
	SetDlgItemText(IDC_EDIT_MESMIN,str);

	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.MesDataMax);
	SetDlgItemText(IDC_EDIT_MESMAX,str);

	if (g_pFrm->m_CmdRun.ProductParam.DispMode>0)
	{
		m_DisFopen.SetCheck(1);
	}
	else
	{
	   m_DisFopen.SetCheck(0);
	}
    if (g_pFrm->m_CmdRun.ProductParam.bMesOffline>0)
    {
		m_MesOffLine.SetCheck(1);
    }
	else
	{
		m_MesOffLine.SetCheck(0);
	}

	if(g_pFrm->m_CmdRun.ProductParam.CleanNd>0)
	{
		m_CleanNeedle.SetCheck(1);
	}
	else
	{
		m_CleanNeedle.SetCheck(0);
	}

	if(g_pFrm->m_CmdRun.ProductParam.FirstTest>0)
	{
		m_FirstTest.SetCheck(1);
	}
	else
	{
		m_FirstTest.SetCheck(0);
	}

	if(g_pFrm->m_CmdRun.ProductParam.SlotDetect>0)
	{
		m_SlotDetect.SetCheck(1);
	}
	else
	{
		m_SlotDetect.SetCheck(0);
	}

	if(g_pFrm->m_CmdRun.ProductParam.FullTestDot>0)
	{
		m_FullDischarge.SetCheck(1);
	}
	else
	{
		m_FullDischarge.SetCheck(0);
	}

	if(g_pFrm->m_CmdRun.ProductParam.IrregularMatrix>0)
	{
		m_IrregularMatrix.SetCheck(1);
	}
	else
	{
		m_IrregularMatrix.SetCheck(0);
	}

	//if(g_pFrm->m_CmdRun.ProductParam.SingleValve>0)
	//{
	//	m_SingleValve.SetCheck(1);
	//}
	//else
	//{
	//	m_SingleValve.SetCheck(0);
	//}

	if(g_pFrm->m_CmdRun.ProductParam.FullClean>0)
	{
		m_FullClean.SetCheck(1);
	}
	else
	{
		m_FullClean.SetCheck(0);
	}

	if(g_pFrm->m_CmdRun.ProductParam.DoorSwitch>0)
	{
		m_DoorSwitch.SetCheck(1);
	}
	else
	{
		m_DoorSwitch.SetCheck(0);
	}

	if(g_pFrm->m_CmdRun.ProductParam.DispFinishFill>0)
	{
		m_DispFinishFill.SetCheck(1);
	}
	else
	{
		m_DispFinishFill.SetCheck(0);
	}

	if(g_pFrm->m_CmdRun.ProductParam.CleanDischarge>0)
	{
		m_CleanDischarge.SetCheck(1);
	}
	else
	{
		m_CleanDischarge.SetCheck(0);
	}

	if(g_pFrm->m_CmdRun.ProductParam.PauseCleanTest>0)
	{
		m_PauseCleanTest.SetCheck(1);
	}
	else
	{
		m_PauseCleanTest.SetCheck(0);
	}

	if (g_pFrm->m_CmdRun.ProductParam.HeatWhenStart>0)
	{
		m_HeatEnable.SetCheck(1);
	}
	else
	{
       m_HeatEnable.SetCheck(0);
	}
	if(g_pFrm->m_CmdRun.ProductParam.TempJudgeBeforeStart>0)
	{
		m_TempJudge.SetCheck(1);
	}
	else
	{
		m_TempJudge.SetCheck(0);
	}


	if(g_pFrm->m_CmdRun.ProductParam.MeasureHeightEnable>0)
	{
		m_MeasureHeight.SetCheck(1);
	}
	else
	{
		m_MeasureHeight.SetCheck(0);
	}

	if(g_pFrm->m_CmdRun.ProductParam.AutoAdjust>0)
	{
		m_AutoAdjust.SetCheck(1);
	}
	else
	{
		m_AutoAdjust.SetCheck(0);
	}
	if(g_pFrm->m_CmdRun.ProductParam.NeedleHeightCheck>0)
	{
		m_NeedleCheck.SetCheck(1);
	}
	else
	{
		m_NeedleCheck.SetCheck(0);
	}
	/*if(1 == g_pFrm->m_CmdRun.ProductParam.bCleanRemind)	
	{
		m_ValveMonitor.SetCheck(1);
	}
	else
	{
		m_ValveMonitor.SetCheck(0);
	}*/

	for (int i=0;i<3;i++)
	{
		CButton *pbt=(CButton*)GetDlgItem(IDC_FA+i);
		pbt->SetCheck(g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]); 
	}

	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.CleanGap);
	SetDlgItemText(IDC_EDIT_NEEDLE_CLEAN_PITCH,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.ValveMonitorSpan);
	SetDlgItemText(IDC_EDIT_VALVECLEAN_TIME,str);	

	//UD和MES相关
	//str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.ServerURL);
	str=g_pFrm->m_CmdRun.ProductParam.ServerURL;
	SetDlgItemText(IDC_EDIT_UDP_URL,str);

	str=g_pFrm->m_CmdRun.ProductParam.MachineID;
	SetDlgItemText(IDC_EDIT_MACHINEID,str);

	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.ServerPort);
	SetDlgItemText(IDC_EDIT_UDP_PORT,str);

	str.Format("%d",g_pFrm->m_CmdRun.ProductParam.FCompanyCode);
	SetDlgItemText(IDC_EDIT_UDP_CPYCODE,str);

	str=g_pFrm->m_CmdRun.ProductParam.ServerIP;
	m_IPAddress.SetWindowText(str);

	
	((CButton*)GetDlgItem(IDC_CHECK_NO_COMPENSATE))->SetCheck(g_pFrm->m_CmdRun.ProductParam.nCheckIsCompensate);
		

	//str.Format("%0.3f", g_pFrm->m_CmdRun.ProductParam.CleanRemindTimeHour);
	//SetDlgItemText(IDC_EDIT_REMIND_CLEAN, str);
	SetAuthority();
	return TRUE;  // return TRUE unless you set the focus to a control
}
// 更新参数
void CDlgProudSet::updataparam(void)
{
	int nTempNumber;
	long lData;
	bool bTrackChange = false;
	CString str;
	GetDlgItemText(IDC_EDIT_FIRST_ROW,str);
	nTempNumber = atoi(str);
	if(g_pFrm->m_CmdRun.ProductParam.FirstDispRow!=nTempNumber)
	{
		bTrackChange = true;
		g_pFrm->m_CmdRun.ProductParam.FirstDispRow = nTempNumber;
	}
	GetDlgItemText(IDC_EDIT_FIRST_COLUMN,str);
	nTempNumber = atoi(str);
	if(g_pFrm->m_CmdRun.ProductParam.FirstDispColumn!=nTempNumber)
	{
		bTrackChange = true;
		g_pFrm->m_CmdRun.ProductParam.FirstDispColumn = nTempNumber;
	}
	GetDlgItemText(IDC_EDIT_FIRST_ROWD,str);
	g_pFrm->m_CmdRun.ProductParam.FirstDispRowD=atof(str);
	GetDlgItemText(IDC_EDIT_FIRST_COLUMND,str);
	g_pFrm->m_CmdRun.ProductParam.FirstDispColumnD=atof(str);
	GetDlgItemText(IDC_EDIT_SECOND_ROW,str);
	nTempNumber = atoi(str);
	if(g_pFrm->m_CmdRun.ProductParam.SecondDispRow!=nTempNumber)
	{
		bTrackChange = true;
		g_pFrm->m_CmdRun.ProductParam.SecondDispRow = nTempNumber;
	}
	GetDlgItemText(IDC_EDIT_SECOND_COLUMN,str);
	nTempNumber = atoi(str);
	if(g_pFrm->m_CmdRun.ProductParam.SecondDispColumn!=nTempNumber)
	{
		bTrackChange = true;
		g_pFrm->m_CmdRun.ProductParam.SecondDispColumn = nTempNumber;
	}
	GetDlgItemText(IDC_EDIT_SECOND_ROWD,str);
	g_pFrm->m_CmdRun.ProductParam.SecondDispRowD=atof(str);
	GetDlgItemText(IDC_EDIT_SECOND_COLUMND,str);
	g_pFrm->m_CmdRun.ProductParam.SecondDispColumnD=atof(str);
	GetDlgItemText(IDC_EDIT_THIRD_ROW,str);
	nTempNumber = atoi(str);
	if(g_pFrm->m_CmdRun.ProductParam.ThirdDispRow!=nTempNumber)
	{
		bTrackChange = true;
		g_pFrm->m_CmdRun.ProductParam.ThirdDispRow = nTempNumber;
	}
	GetDlgItemText(IDC_EDIT_THIRD_COLUMN,str);
	nTempNumber = atoi(str);
	if(g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn!=nTempNumber)
	{
		bTrackChange = true;
		g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn = nTempNumber;
	}
	GetDlgItemText(IDC_EDIT_THIRD_ROWD,str);
	g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD=atof(str);
	GetDlgItemText(IDC_EDIT_THIRD_COLUMND,str);
	g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD=atof(str);
	GetDlgItemText(IDC_EDIT_MULTI_ROW_NUMBER,str);
	g_pFrm->m_CmdRun.ProductParam.MultiDispRow=atoi(str);
	GetDlgItemText(IDC_EDIT_MULTI_COLUMN_NUMBER,str);
	g_pFrm->m_CmdRun.ProductParam.MultiDispColumn=atoi(str);
	GetDlgItemText(IDC_EDIT_MULTI_ROW_DISTANCE,str);
	g_pFrm->m_CmdRun.ProductParam.MultiDispRowD=atof(str);
	GetDlgItemText(IDC_EDIT_MULTI_COLUMN_DISTANCE,str);
	g_pFrm->m_CmdRun.ProductParam.MultiDispColumnD=atof(str);
	GetDlgItemText(IDC_EDIT_PRE_DELAY,str);
	g_pFrm->m_CmdRun.ProductParam.dPreTime=atof(str);
	GetDlgItemText(IDC_EDIT_POST_DELAY,str);
	g_pFrm->m_CmdRun.ProductParam.dPostDisTime=atof(str);
	GetDlgItemText(IDC_EDIT_LIFT_HEIGHT,str);
	g_pFrm->m_CmdRun.ProductParam.dZupHigh=atof(str);
	if(g_pFrm->m_CmdRun.ProductParam.dZupHigh<0)
	{
		g_pFrm->m_CmdRun.ProductParam.dZupHigh = 0.0;
	}
	GetDlgItemText(IDC_EDIT_PAUSE_POS,str);
	g_pFrm->m_CmdRun.ProductParam.dPauseHeight=atof(str);
	if(g_pFrm->m_CmdRun.ProductParam.dPauseHeight<0)
	{
		g_pFrm->m_CmdRun.ProductParam.dPauseHeight = 0.0;
	}
	else if(g_pFrm->m_CmdRun.ProductParam.dPauseHeight>g_pFrm->m_CmdRun.ProductParam.dZupHigh)
	{
		g_pFrm->m_CmdRun.ProductParam.dPauseHeight = g_pFrm->m_CmdRun.ProductParam.dZupHigh;
	}
	GetDlgItemText(IDC_EDIT_PAUSE_TIME,str);
	g_pFrm->m_CmdRun.ProductParam.dPauseTime=atof(str);
	GetDlgItemText(IDC_EDIT_FIRST_DISPDELAY,str);
	g_pFrm->m_CmdRun.ProductParam.dFirstDispDelay=atof(str);
	GetDlgItemText(IDC_EDIT_DISP_RADIUS,str);
	g_pFrm->m_CmdRun.ProductParam.dDispRadius=atof(str);
	GetDlgItemText(IDC_EDIT_DISTANCE,str);
	g_pFrm->m_CmdRun.ProductParam.dDistance=atof(str);
	GetDlgItemText(IDC_EDIT_GRABIMAGE_DELAY,str);
	g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay=atof(str);
	GetDlgItemText(IDC_EDIT_COMPANGLE,str);
	g_pFrm->m_CmdRun.ProductParam.dCompAngle=atof(str);
	if(g_pFrm->m_CmdRun.ProductParam.dCompAngle<0)
	{
		g_pFrm->m_CmdRun.ProductParam.dCompAngle = 0.0;
	}
	else if(g_pFrm->m_CmdRun.ProductParam.dCompAngle>=360)
	{
		g_pFrm->m_CmdRun.ProductParam.dCompAngle = 359.0;
	}

	GetDlgItemText(IDC_EDIT_CIRCLE_VEL,str);
	g_pFrm->m_CmdRun.ProductParam.CircleVel=atof(str);
	GetDlgItemText(IDC_EDIT_CIRCLE_ACC,str);
	g_pFrm->m_CmdRun.ProductParam.CircleAcc=atof(str);
	GetDlgItemText(IDC_EDIT_CLEAN_DELAY,str);
	g_pFrm->m_CmdRun.ProductParam.dCleanDelay=atof(str);
	GetDlgItemText(IDC_EDIT_TEST_DELAY,str);
	g_pFrm->m_CmdRun.ProductParam.dTestDotDelay = atof(str);
	GetDlgItemText(IDC_EDIT_TEST_NUMBER,str);
	g_pFrm->m_CmdRun.ProductParam.TestDotNumber = atoi(str);
	GetDlgItemText(IDC_EDIT_HEIGHT_ERROR,str);
	g_pFrm->m_CmdRun.ProductParam.dHeightError = atof(str);
	GetDlgItemText(IDC_EDIT_HEIGHT_NUMBER,str);
	g_pFrm->m_CmdRun.ProductParam.HeightNumber = atoi(str);
	GetDlgItemText(IDC_EDIT_HEIGHT_DELAY,str);
	g_pFrm->m_CmdRun.ProductParam.dHeightDelay = atof(str);
	GetDlgItemText(IDC_EDIT_LACK_CMPVALUE,str);
	g_pFrm->m_CmdRun.ProductParam.nLackPadCmpValue = atoi(str);//2016-01-16
	//GetDlgItemText(IDC_EDIT_DISPENSE_HEIGHT,str);
	//g_pFrm->m_CmdRun.ProductParam.dDispenseHeight = atof(str);
	//if(g_pFrm->m_CmdRun.ProductParam.dDispenseHeight<0)
	//{
		//g_pFrm->m_CmdRun.ProductParam.dDispenseHeight = 0.0;
	//}
	GetDlgItemText(IDC_EDIT_HEIGHT_COMPMAX,str);
	double dTemp =atof(str);
	if (dTemp<=0)
	{
		dTemp=0.01;
	}
	g_pFrm->m_CmdRun.ProductParam.dHeightCompMax=dTemp;//补偿极值；
	GetDlgItemText(IDC_EDIT_LINE_LENGTH,str);
	g_pFrm->m_CmdRun.ProductParam.LineLength = atof(str);
	GetDlgItemText(IDC_EDIT_RECT_WIDTH,str);
	g_pFrm->m_CmdRun.ProductParam.RectWidth = atof(str);
	GetDlgItemText(IDC_EDIT_RECT_LENGTH,str);
	g_pFrm->m_CmdRun.ProductParam.RectLength = atof(str);
	GetDlgItemText(IDC_EDIT_LINE_VEL,str);
	g_pFrm->m_CmdRun.ProductParam.LineVel = atof(str);
	GetDlgItemText(IDC_EDIT_LINE_ACC,str);
	g_pFrm->m_CmdRun.ProductParam.LineAcc = atof(str);

	GetDlgItemText(IDC_EDIT_DISP_OFFSET_X,str);
	g_pFrm->m_CmdRun.ProductParam.DispOffsetX = atof(str);
	GetDlgItemText(IDC_EDIT_DISP_OFFSET_Y,str);
	g_pFrm->m_CmdRun.ProductParam.DispOffsetY = atof(str);
	GetDlgItemText(IDC_EDIT_MAX_OFFSET_ANGLE,str);
	g_pFrm->m_CmdRun.ProductParam.MaxOffsetAngle = atof(str);
	if(g_pFrm->m_CmdRun.ProductParam.MaxOffsetAngle<0)
	{
		g_pFrm->m_CmdRun.ProductParam.MaxOffsetAngle = 0.0;
	}
	GetDlgItemText(IDC_EDIT_MAX_ADJUST_X,str);
	g_pFrm->m_CmdRun.ProductParam.MaxOffsetX = atof(str);
	if(g_pFrm->m_CmdRun.ProductParam.MaxOffsetX<0)
	{
		g_pFrm->m_CmdRun.ProductParam.MaxOffsetX = 0.0;
	}
	GetDlgItemText(IDC_EDIT_MAX_ADJUST_Y,str);
	g_pFrm->m_CmdRun.ProductParam.MaxOffsetY = atof(str);
	if(g_pFrm->m_CmdRun.ProductParam.MaxOffsetY<0)
	{
		g_pFrm->m_CmdRun.ProductParam.MaxOffsetY = 0.0;
	}

	GetDlgItemText(IDC_EDIT_SCAN_DELAY,str);
	g_pFrm->m_CmdRun.ProductParam.ScanDelay = atof(str);

	GetDlgItemText(IDC_EDIT_TEMP_ERROR,str);
	g_pFrm->m_CmdRun.ProductParam.dTempError=atof(str);

	GetDlgItemText(IDC_EDIT_GASKETRING_LIFE,str);
	g_pFrm->m_CmdRun.ProductParam.lGasketRingLife=atoi(str);

	GetDlgItemText(IDC_EDIT_SLOT_LENGTH,str);
	g_pFrm->m_CmdRun.ProductParam.IrregularSlotLength = atof(str);
	GetDlgItemText(IDC_EDIT_SLOT_WIDTH,str);
	g_pFrm->m_CmdRun.ProductParam.IrregularSlotWidth = atof(str);
	GetDlgItemText(IDC_EDIT_IRREGULAR_ROW_NUM,str);
	nTempNumber = atoi(str);
	if(g_pFrm->m_CmdRun.ProductParam.IrregularRow!=nTempNumber)
	{
		bTrackChange = true;
		g_pFrm->m_CmdRun.ProductParam.IrregularRow = nTempNumber;
	}
	GetDlgItemText(IDC_EDIT_IRREGULAR_COL_NUM,str);
	nTempNumber = atoi(str);
	if(g_pFrm->m_CmdRun.ProductParam.IrregularColumn!=nTempNumber)
	{
		bTrackChange = true;
		g_pFrm->m_CmdRun.ProductParam.IrregularColumn = nTempNumber;
	}

	GetDlgItemText(IDC_EDIT_CONTACT_FREQ,str);
    lData=atol(str);
	if(g_pFrm->m_CmdRun.ProductParam.ContactFrequency!=lData)
	{
		bTrackChange=true;
		g_pFrm->m_CmdRun.ProductParam.ContactFrequency = lData;
	}

	GetDlgItemText(IDC_EDIT_CONTACT_ERRPERMIT,str);
	dTemp=atof(str);
    if(g_pFrm->m_CmdRun.ProductParam.ContactErrPermit!=dTemp)
    {
		bTrackChange=true;
		g_pFrm->m_CmdRun.ProductParam.ContactErrPermit = dTemp;
    }

	g_pFrm->m_CmdRun.ProductParam.DispFCount=0;
	for (int i=0;i<3;i++)
	{
		CButton *pbt=(CButton*)GetDlgItem(IDC_FA+i);
		if (pbt->GetCheck())
		{
			g_pFrm->m_CmdRun.ProductParam.DispFCount++;
			g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]=true;      
		}
		else
		{
			g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]=false;
		}
	}
	if (g_pFrm->m_CmdRun.ProductParam.DispFCount==0)
	{
		g_pFrm->m_CmdRun.ProductParam.DispFCount=1;
		g_pFrm->m_CmdRun.ProductParam.FSelectVal[0]=true;
	}
	else if (g_pFrm->m_CmdRun.ProductParam.DispFCount>3)
	{
		g_pFrm->m_CmdRun.ProductParam.DispFCount=3;
	}
	// 点胶模式选择
	if (m_DisFopen.GetCheck())
	{
		g_pFrm->m_CmdRun.ProductParam.DispMode=1;//不出胶
	}
	else
	{
		g_pFrm->m_CmdRun.ProductParam.DispMode=0;
	}

	if (m_CleanNeedle.GetCheck())
	{
		g_pFrm->m_CmdRun.ProductParam.CleanNd=1;
	}
	else
	{
		g_pFrm->m_CmdRun.ProductParam.CleanNd=0;
	}

	if(m_FirstTest.GetCheck())
	{
		g_pFrm->m_CmdRun.ProductParam.FirstTest=1;
	}
	else
	{
		g_pFrm->m_CmdRun.ProductParam.FirstTest=0;
	}

	if(m_SlotDetect.GetCheck())
	{
		g_pFrm->m_CmdRun.ProductParam.SlotDetect = 1;
	}
	else
	{
		g_pFrm->m_CmdRun.ProductParam.SlotDetect = 0;
	}

	if(m_FullDischarge.GetCheck())
	{
		g_pFrm->m_CmdRun.ProductParam.FullTestDot = 1;
	}
	else
	{
		g_pFrm->m_CmdRun.ProductParam.FullTestDot = 0;
	}

	nTempNumber = m_IrregularMatrix.GetCheck();
	if(g_pFrm->m_CmdRun.ProductParam.IrregularMatrix!=nTempNumber)
	{
		bTrackChange = true;
		g_pFrm->m_CmdRun.ProductParam.IrregularMatrix = nTempNumber;
	}

	//if(m_SingleValve.GetCheck())
	//{
	//	g_pFrm->m_CmdRun.ProductParam.SingleValve = 1;
	//}
	//else
	//{
	//	g_pFrm->m_CmdRun.ProductParam.SingleValve = 0;
	//}

	if(m_FullClean.GetCheck())
	{
		g_pFrm->m_CmdRun.ProductParam.FullClean = 1;
	}
	else
	{
		g_pFrm->m_CmdRun.ProductParam.FullClean = 0;
	}

	if(m_DoorSwitch.GetCheck())
	{
		g_pFrm->m_CmdRun.ProductParam.DoorSwitch = 1;
	}
	else
	{
		g_pFrm->m_CmdRun.ProductParam.DoorSwitch = 0;
	}

	if(m_DispFinishFill.GetCheck())
	{
		g_pFrm->m_CmdRun.ProductParam.DispFinishFill = 1;
	}
	else
	{
		g_pFrm->m_CmdRun.ProductParam.DispFinishFill = 0;
	}

	if(m_CleanDischarge.GetCheck())
	{
		g_pFrm->m_CmdRun.ProductParam.CleanDischarge = 1;
	}
	else
	{
		g_pFrm->m_CmdRun.ProductParam.CleanDischarge = 0;
	}

	if(m_PauseCleanTest.GetCheck())
	{
		g_pFrm->m_CmdRun.ProductParam.PauseCleanTest = 1;
	}
	else
	{
		g_pFrm->m_CmdRun.ProductParam.PauseCleanTest = 0;
	}

	if (m_HeatEnable.GetCheck())
	{
		g_pFrm->m_CmdRun.ProductParam.HeatWhenStart=1;
	}
	else
	{
		g_pFrm->m_CmdRun.ProductParam.HeatWhenStart=0;
	}

	if(m_TempJudge.GetCheck())
	{
		g_pFrm->m_CmdRun.ProductParam.TempJudgeBeforeStart = 1;
	}
	else
	{
		g_pFrm->m_CmdRun.ProductParam.TempJudgeBeforeStart = 0;
	}

	if(m_MeasureHeight.GetCheck())
	{
		g_pFrm->m_CmdRun.ProductParam.MeasureHeightEnable = 1;
	}
	else
	{
		g_pFrm->m_CmdRun.ProductParam.MeasureHeightEnable = 0;
	}
	if(m_AutoAdjust.GetCheck())
	{
		g_pFrm->m_CmdRun.ProductParam.AutoAdjust = 1;
	}
	else
	{
		g_pFrm->m_CmdRun.ProductParam.AutoAdjust = 0;
	}
	if(bTrackChange)
	{
		g_pFrm->m_CmdRun.InitPadDetect();
		g_pFrm->m_CmdRun.ProductParam.tgTrackInfo.TrackReset();
	}

	//新增针头间距
	//GetDlgItemText(IDC_EDIT_NEEDLE_PITCH,str);
	//g_pFrm->m_CmdRun.ProductParam.NeedleGap = atof(str);
	GetDlgItemText(IDC_EDIT_NEEDLE_CLEAN_PITCH,str);
	g_pFrm->m_CmdRun.ProductParam.CleanGap=atof(str);
	if (NULL!=g_pFrm)
	{
		g_pFrm->m_CmdRun.ComputeNeedleGap();  //2017-07-19  Auto compute NEEDLE GAP;
	}
	//阀体偏移数据补偿；
	GetDlgItemText(IDC_EDIT_COMPOSE_SECONDY,str);
	g_pFrm->m_CmdRun.ProductParam.dComposedSecondY=atof(str);
	if (fabs(atof(str))>100)
	{
		g_pFrm->m_CmdRun.ProductParam.dComposedSecondY=0;
		AfxMessageBox("阀体2Y偏差数据超过了100mm，请重新设定！");
	}
	
	GetDlgItemText(IDC_EDIT_COMPOSE_THIRDY,str);
	g_pFrm->m_CmdRun.ProductParam.dComposedThirdY=atof(str);
	if (fabs(atof(str))>100)
	{
		g_pFrm->m_CmdRun.ProductParam.dComposedThirdY=0;
		AfxMessageBox("阀体3Y偏差数据超过了100mm，请重新设定！");
	}
	
	GetDlgItemText(IDC_EDIT_COMPOSE_SECONDX,str);
	g_pFrm->m_CmdRun.ProductParam.dComposeSecondX=atof(str);
	if (fabs(atof(str))>100)
	{
		g_pFrm->m_CmdRun.ProductParam.dComposeSecondX=0;
		AfxMessageBox("阀体2X偏差数据超过了100mm，请重新设定！");
	}
	
	GetDlgItemText(IDC_EDIT_COMPOSE_THIRDX,str);
	g_pFrm->m_CmdRun.ProductParam.dComposedThirdX=atof(str);
	if (fabs(atof(str))>100)
	{
		g_pFrm->m_CmdRun.ProductParam.dComposedThirdX=0;
		AfxMessageBox("阀体3X偏差数据超过了100mm，请重新设定！");
	}
	GetDlgItemText(IDC_EDIT_VALVECLEAN_TIME,str);
	g_pFrm->m_CmdRun.ProductParam.ValveMonitorSpan=atof(str);

	m_IPAddress.GetWindowText(str);
	strcpy(g_pFrm->m_CmdRun.ProductParam.ServerIP,str);
   
	GetDlgItemText(IDC_EDIT_UDP_PORT,str);
    g_pFrm->m_CmdRun.ProductParam.ServerPort=atoi(str);

	GetDlgItemText(IDC_EDIT_UDP_CPYCODE,str);
	g_pFrm->m_CmdRun.ProductParam.FCompanyCode=atoi(str);

	GetDlgItemText(IDC_EDIT_MESMIN,str);
	g_pFrm->m_CmdRun.ProductParam.MesDataMin=atof(str);

	GetDlgItemText(IDC_EDIT_MESMAX,str);
	g_pFrm->m_CmdRun.ProductParam.MesDataMax=atof(str);

	GetDlgItemText(IDC_EDIT_MACHINEID,str);
	strcpy(g_pFrm->m_CmdRun.ProductParam.MachineID,str);

	////
	/*if (1==m_ValveMonitor.GetCheck())
	{
		g_pFrm->m_CmdRun.ProductParam.bCleanRemind=1;
	}
	else
	{
		g_pFrm->m_CmdRun.ProductParam.bCleanRemind=0;
	}*/
	/*GetDlgItemText(IDC_EDIT_REMIND_CLEAN,str);
	g_pFrm->m_CmdRun.ProductParam.CleanRemindTimeHour=atof(str);*/
    if (m_MesOffLine.GetCheck()>0)
    {
		g_pFrm->m_CmdRun.ProductParam.bMesOffline=1;
    }
	else
	{
		g_pFrm->m_CmdRun.ProductParam.bMesOffline=0;
	}
	if(m_NeedleCheck.GetCheck()>0)
	{
		g_pFrm->m_CmdRun.ProductParam.NeedleHeightCheck =1;
	}
	else
	{
        g_pFrm->m_CmdRun.ProductParam.NeedleHeightCheck =0;
	}

	 g_pFrm->m_CmdRun.ProductParam.nCheckIsCompensate = ((CButton*)GetDlgItem(IDC_CHECK_NO_COMPENSATE))->GetCheck();
}
// A阀选择
void CDlgProudSet::OnBnClickedFA()
{
	// TODO: Add your control notification handler code here
	theApp.m_tSysParam.V9HomeSu=false;
	g_pFrm->m_CmdRun.V9RestOK=false;
}
// B阀选择
void CDlgProudSet::OnBnClickedFB()
{
	// TODO: Add your control notification handler code here
	theApp.m_tSysParam.V9HomeSu=false;
	g_pFrm->m_CmdRun.V9RestOK=false;
}
// C阀选择
void CDlgProudSet::OnBnClickedFC()
{
	// TODO: Add your control notification handler code here
	theApp.m_tSysParam.V9HomeSu=false;
	g_pFrm->m_CmdRun.V9RestOK=false;
}
//// D阀选择
//void CDlgProudSet::OnBnClickedFD()
//{
//	theApp.m_tSysParam.V9HomeSu=false;
//	g_pFrm->m_CmdRun.V9RestOK=false;
//	// TODO: Add your control notification handler code here
//}
// 暂停位置编辑框
void CDlgProudSet::OnEnChangePausePos()
{
	CString str;
	GetDlgItemText(IDC_EDIT_PAUSE_POS,str);
	double dPausePos = atof(str);
	GetDlgItemText(IDC_EDIT_LIFT_HEIGHT,str);
	double dZHeight = atof(str);
	if(dPausePos<0)
	{
		dPausePos = 0.0;
		str.Format("%0.3f",dPausePos);
		SetDlgItemText(IDC_EDIT_PAUSE_POS,str);
	}
	else if(dPausePos>dZHeight)
	{
		dPausePos = dZHeight;
		str.Format("%0.3f",dPausePos);
		SetDlgItemText(IDC_EDIT_PAUSE_POS,str);
	}
}

void CDlgProudSet::OnBnClickedBtnFirstRowd()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_FIRST_ROW,str);
	g_pFrm->m_CmdRun.ProductParam.FirstDispRow = atoi(str);
	g_pFrm->m_dlgRCD.SetParam(0,1);
	g_pFrm->m_dlgRCD.DoModal();
	str.Format("%.3f",g_pFrm->m_CmdRun.ProductParam.FirstDispRowD);
	SetDlgItemText(IDC_EDIT_FIRST_ROWD,str);
	GetDlgItem(IDC_EDIT_FIRST_ROWD)->UpdateData(FALSE);
}

void CDlgProudSet::OnBnClickedBtnSecondRowd()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_SECOND_ROW,str);
	g_pFrm->m_CmdRun.ProductParam.SecondDispRow = atoi(str);
	g_pFrm->m_dlgRCD.SetParam(1,1);
	g_pFrm->m_dlgRCD.DoModal();
	str.Format("%.3f",g_pFrm->m_CmdRun.ProductParam.SecondDispRowD);
	SetDlgItemText(IDC_EDIT_SECOND_ROWD,str);
	GetDlgItem(IDC_EDIT_SECOND_ROWD)->UpdateData(FALSE);
}

void CDlgProudSet::OnBnClickedBtnFirstColumnd()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_FIRST_COLUMN,str);
	g_pFrm->m_CmdRun.ProductParam.FirstDispColumn = atoi(str);
	g_pFrm->m_dlgRCD.SetParam(0,0);
	g_pFrm->m_dlgRCD.DoModal();
	str.Format("%.3f",g_pFrm->m_CmdRun.ProductParam.FirstDispColumnD);
	SetDlgItemText(IDC_EDIT_FIRST_COLUMND,str);
	GetDlgItem(IDC_EDIT_FIRST_COLUMND)->UpdateData(FALSE);
}

void CDlgProudSet::OnBnClickedBtnSecondColumnd()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_SECOND_COLUMN,str);
	g_pFrm->m_CmdRun.ProductParam.SecondDispColumn = atoi(str);
	g_pFrm->m_dlgRCD.SetParam(1,0);
	g_pFrm->m_dlgRCD.DoModal();
	str.Format("%.3f",g_pFrm->m_CmdRun.ProductParam.SecondDispColumnD);
	SetDlgItemText(IDC_EDIT_SECOND_COLUMND,str);
	GetDlgItem(IDC_EDIT_SECOND_COLUMND)->UpdateData(FALSE);
}

void CDlgProudSet::OnBnClickedRadioDot()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.fillMode = DOT_DISP_MODE;
	g_pFrm->m_CmdRun.V9RestOK = false;
}

void CDlgProudSet::OnBnClickedRadioLineVertical()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.fillMode = LINE_VERTICAL_DISP_MODE;
	g_pFrm->m_CmdRun.V9RestOK = false;
}

void CDlgProudSet::OnBnClickedRadioLineHorizontal()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.fillMode = LINE_HORIZONTAL_DISP_MODE;
	g_pFrm->m_CmdRun.V9RestOK = false;
}

void CDlgProudSet::OnBnClickedRadioRectangle()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.fillMode = RECTANGLE_DISP_MODE;
	g_pFrm->m_CmdRun.V9RestOK = false;
}

void CDlgProudSet::OnBnClickedRadioHelix()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.fillMode = HELIX_DISP_MODE;
	g_pFrm->m_CmdRun.V9RestOK = false;
}

void CDlgProudSet::OnBnClickedRadioCustom()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.fillMode = CUSTOM_DISP_MODE;
	g_pFrm->m_CmdRun.V9RestOK = false;
}

void CDlgProudSet::OnBnClickedRadioImageNo()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.visionMode = VISION_DISABLE;
}

void CDlgProudSet::OnBnClickedRadioImageOneMark()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.visionMode = VISION_ONE_MARK;
	g_pFrm->m_CmdRun.ProductParam.nImageNum = 1;
}

void CDlgProudSet::OnBnClickedRadioImageTwoMark()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.visionMode = VISION_TWO_MARK;
	g_pFrm->m_CmdRun.ProductParam.nImageNum = 2;
}

void CDlgProudSet::OnBnClickedBtnCustomParam()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_dlgCustom.DoModal();
}


void CDlgProudSet::OnBnClickedBtnThirdRowd()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_THIRD_ROW,str);
	g_pFrm->m_CmdRun.ProductParam.ThirdDispRow = atoi(str);
	g_pFrm->m_dlgRCD.SetParam(2,1);
	g_pFrm->m_dlgRCD.DoModal();
	str.Format("%.3f",g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD);
	SetDlgItemText(IDC_EDIT_THIRD_ROWD,str);
	GetDlgItem(IDC_EDIT_THIRD_ROWD)->UpdateData(FALSE);
}

void CDlgProudSet::OnBnClickedBtnThirdColumnd()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_THIRD_COLUMN,str);
	g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn = atoi(str);
	g_pFrm->m_dlgRCD.SetParam(2,0);
	g_pFrm->m_dlgRCD.DoModal();
	str.Format("%.3f",g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD);
	SetDlgItemText(IDC_EDIT_THIRD_COLUMND,str);
	GetDlgItem(IDC_EDIT_THIRD_COLUMND)->UpdateData(FALSE);
}

void CDlgProudSet::SetAuthority()
{
	GetDlgItem(IDC_EDIT_FIRST_ROW)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_FIRST_COLUMN)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_FIRST_ROWD)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_FIRST_COLUMND)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_BTN_FIRST_ROWD)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_BTN_FIRST_COLUMND)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);

	GetDlgItem(IDC_EDIT_SECOND_ROW)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_SECOND_COLUMN)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_SECOND_ROWD)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_SECOND_COLUMND)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_BTN_SECOND_ROWD)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_BTN_SECOND_COLUMND)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);

	GetDlgItem(IDC_EDIT_THIRD_ROW)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_THIRD_COLUMN)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_THIRD_ROWD)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_THIRD_COLUMND)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_BTN_THIRD_ROWD)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_BTN_THIRD_COLUMND)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);

	GetDlgItem(IDC_EDIT_IRREGULAR_ROW_NUM)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_IRREGULAR_COL_NUM)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_IRREGULAR_ROW_DISTANCE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_IRREGULAR_COL_DISTANCE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_IRREGULAR_NEXT_ROW)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_IRREGULAR_NEXT_COL)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_SLOT_LENGTH)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_SLOT_WIDTH)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_COMBO_IRREGULAR_ROW)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_COMBO_IRREGULAR_COL)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);

	GetDlgItem(IDC_EDIT_PRE_DELAY)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_POST_DELAY)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_PAUSE_TIME)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_PAUSE_POS)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_TEMP_ERROR)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);

	GetDlgItem(IDC_EDIT_LIFT_HEIGHT)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_GRABIMAGE_DELAY)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_CLEAN_DELAY)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_FIRST_DISPDELAY)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_SCAN_DELAY)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_DISP_OFFSET_X)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_DISP_OFFSET_Y)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_COMPANGLE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_DISP_RADIUS)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_DISTANCE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_CIRCLE_VEL)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_CIRCLE_ACC)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_RECT_LENGTH)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_RECT_WIDTH)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_LINE_VEL)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_LINE_ACC)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_LINE_LENGTH)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_TEST_DELAY)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_TEST_NUMBER)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_MAX_OFFSET_ANGLE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_HEIGHT_NUMBER)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_HEIGHT_ERROR)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_HEIGHT_DELAY)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_LACK_CMPVALUE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	//GetDlgItem(IDC_EDIT_DISPENSE_HEIGHT)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_MAX_ADJUST_X)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_MAX_ADJUST_Y)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_GASKETRING_LIFE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_HEIGHT_COMPMAX)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_ADMIN);

	//GetDlgItem(IDC_CHECK_NOFLOW)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_GUEST);
	GetDlgItem(IDC_CHECK_CLEAN_NEEDLE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_CHECK_FIRST_TEST)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_GUEST);
	GetDlgItem(IDC_CHECK_SLOT_DETECT)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_CHECK_FULL_DISCHARGE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_CHECK_IRREGULAR_MATRIX)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	//GetDlgItem(IDC_CHECK_SINGLE_DISP)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_CHECK_FULL_CLEAN)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_CHECK_SWITCH_ENABLE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_CHECK_DISP_FINISH_FILL)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_CHECK_CLEAN_DISCHARGE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_CHECK_MEASURE_HEIGHT)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_CHECK_PAD_DETECT)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_CHECK_PAUSE_CLEAN_TEST)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_CHECK_ADJUST_NEEDLE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_CHECK_TEMP_JUDGE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_CHECK_HEAT_ENABLE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_CHECK_MES_OFFLINE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_CHECK_NEEDLECHECK)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);

	GetDlgItem(IDC_RADIO_DOT)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_RADIO_LINE_VERTICAL)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_RADIO_LINE_HORIZONTAL)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_RADIO_CUSTOM)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_RADIO_CIRCLE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_RADIO_MULTI)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_RADIO_COMPLEX)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);

	//GetDlgItem(IDC_RADIO_RECTANGLE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_RADIO_HELIX)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);

	GetDlgItem(IDC_RADIO_IMAGE_NO)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_RADIO_IMAGE_ONE_MARK)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_RADIO_IMAGE_TWO_MARK)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);

	GetDlgItem(IDC_RADIO_NO_INSPECT)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	//GetDlgItem(IDC_RADIO_SCAN_INSPECT)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_RADIO_TRANSITION_INSPECT)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	//GetDlgItem(IDC_RADIO_SECTION_INSPECT)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);


	GetDlgItem(IDC_RADIO_ROW_DISP)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_RADIO_COLUMN_DISP)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);

	GetDlgItem(IDC_RADIO_FIRST_CLEAN_LATER_TEST)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_RADIO_FIRST_TEST_LATER_CLEAN)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);



	GetDlgItem(IDC_FA)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_FB)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_FC)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);

	GetDlgItem(IDC_EDIT_MULTI_ROW_NUMBER)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_MULTI_COLUMN_NUMBER)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_MULTI_ROW_DISTANCE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_MULTI_COLUMN_DISTANCE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);

	GetDlgItem(IDC_EDIT_NEEDLE_CLEAN_PITCH)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_COMPOSE_SECONDY)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_COMPOSE_THIRDY)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_COMPOSE_SECONDX)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_COMPOSE_THIRDX)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_VALVECLEAN_TIME)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	//GetDlgItem(IDC_EDIT_REMIND_CLEAN)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	//GetDlgItem(IDC_EDIT_REMIND_CLEAN)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);

	GetDlgItem(IDC_EDIT_CONTACT_FREQ)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_CONTACT_ERRPERMIT)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_UDP_CPYCODE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_MACHINEID)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	
	GetDlgItem(IDC_EDIT_UDP_URL)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_UDP_PORT)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_IPADDRESS_DUP)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_MESMIN)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_EDIT_MESMAX)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	GetDlgItem(IDC_CHECK_NO_COMPENSATE)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
}

void CDlgProudSet::OnCbnSelchangeComboCurrentRow()
{
	// TODO: Add your control notification handler code here
	CString str;
	m_nSelRow = m_cbCurrentRow.GetCurSel();
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.IrregularRowD[m_nSelRow]);
	SetDlgItemText(IDC_EDIT_IRREGULAR_ROW_DISTANCE,str);
	m_cbCurrentRow.GetLBText(m_nSelRow,str);
	int nextRow = atoi(str)+1;
	str.Format("%d",nextRow);
	SetDlgItemText(IDC_EDIT_IRREGULAR_NEXT_ROW,str);
}

void CDlgProudSet::OnCbnSelchangeComboCurrentColumn()
{
	// TODO: Add your control notification handler code here
	CString str;
	m_nSelColumn = m_cbCurrentColumn.GetCurSel();
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.IrregularColumnD[m_nSelColumn]);
	SetDlgItemText(IDC_EDIT_IRREGULAR_COL_DISTANCE,str);
	m_cbCurrentColumn.GetLBText(m_nSelColumn,str);
	int nextColumn = atoi(str)+1;
	str.Format("%d",nextColumn);
	SetDlgItemText(IDC_EDIT_IRREGULAR_NEXT_COL,str);
}

void CDlgProudSet::OnBnClickedBtnSaveCurrentRowd()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_IRREGULAR_ROW_DISTANCE,str);
	g_pFrm->m_CmdRun.ProductParam.IrregularRowD[m_nSelRow] = atof(str);
}

void CDlgProudSet::OnBnClickedBtnSaveCurrentColumnd()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_IRREGULAR_COL_DISTANCE,str);
	g_pFrm->m_CmdRun.ProductParam.IrregularColumnD[m_nSelColumn] = atof(str);
}

void CDlgProudSet::OnBnClickedRadioNoInspect()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.inspectMode = NO_INSPECT;
}

void CDlgProudSet::OnBnClickedRadioScanInspect()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.inspectMode = SCAN_INSPECT;
}

void CDlgProudSet::OnBnClickedRadioRowDisp()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.dispOrientation = ROW_DISP;
}

void CDlgProudSet::OnBnClickedRadioColumnDisp()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.dispOrientation = COLUMN_DISP;
}

void CDlgProudSet::OnBnClickedRadioFirstCleanLaterTest()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.dispSequence = CLEAN_LATER_TEST;
}

void CDlgProudSet::OnBnClickedRadioFirstTestLaterClean()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.dispSequence = TEST_LATER_CLEAN;
}

void CDlgProudSet::OnBnClickedRadioOneValve()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.valveSelect = ONE_VALVE;
	g_pFrm->m_CmdRun.ProductParam.FSelectVal[0] = true;
	g_pFrm->m_CmdRun.ProductParam.FSelectVal[1] = false;
	g_pFrm->m_CmdRun.ProductParam.FSelectVal[2] = false;
	theApp.m_tSysParam.V9HomeSu=false;
	g_pFrm->m_CmdRun.V9RestOK=false;
	for (int i=0;i<3;i++)
	{
		CButton *pbt=(CButton*)GetDlgItem(IDC_FA+i);
		pbt->SetCheck(g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]); 
	}
}

void CDlgProudSet::OnBnClickedRadioTwoValve()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.valveSelect = TWO_VALVE;
	g_pFrm->m_CmdRun.ProductParam.FSelectVal[0] = true;
	g_pFrm->m_CmdRun.ProductParam.FSelectVal[1] = true;
	g_pFrm->m_CmdRun.ProductParam.FSelectVal[2] = false;
	theApp.m_tSysParam.V9HomeSu=false;
	g_pFrm->m_CmdRun.V9RestOK=false;
	for (int i=0;i<3;i++)
	{
		CButton *pbt=(CButton*)GetDlgItem(IDC_FA+i);
		pbt->SetCheck(g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]); 
	}
}

void CDlgProudSet::OnBnClickedRadioThreeValve()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.valveSelect = THREE_VALVE;
	g_pFrm->m_CmdRun.ProductParam.FSelectVal[0] = true;
	g_pFrm->m_CmdRun.ProductParam.FSelectVal[1] = true;
	g_pFrm->m_CmdRun.ProductParam.FSelectVal[2] = true;
	theApp.m_tSysParam.V9HomeSu=false;
	g_pFrm->m_CmdRun.V9RestOK=false;
	for (int i=0;i<3;i++)
	{
		CButton *pbt=(CButton*)GetDlgItem(IDC_FA+i);
		pbt->SetCheck(g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]); 
	}
}

void CDlgProudSet::OnBnClickedRadioGrayscale()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.matchMode = GRAYSCALE;
	milApp.SetMatchMode(g_pFrm->m_CmdRun.ProductParam.matchMode);
}

void CDlgProudSet::OnBnClickedRadioGeometric()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.matchMode = GEOMETRIC;
	milApp.SetMatchMode(g_pFrm->m_CmdRun.ProductParam.matchMode);
}

void CDlgProudSet::OnBnClickedRadioTransitionInspect()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.inspectMode = TRANSITION_INSPECT;
}

void CDlgProudSet::OnBnClickedRadioCircle()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.fillMode = CIRCLE_DISP_MODE;
	g_pFrm->m_CmdRun.V9RestOK = false;
}

void CDlgProudSet::OnBnClickedRadioSectionInspect()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.inspectMode = SESSION_INDPECT;
}

void CDlgProudSet::OnBnClickedRadioMulti()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.fillMode = MULTI_DISP_MODE;
	g_pFrm->m_CmdRun.V9RestOK = false;
}

//void CDlgProudSet::OnBnClickedBtnNeedlegap()
//{
//	// TODO: Add your control notification handler code here
//	CString str;
//	g_pFrm->m_dlgRCD.SetParam(0,2);
//	g_pFrm->m_dlgRCD.DoModal();
//	str.Format("%.3f",g_pFrm->m_CmdRun.ProductParam.NeedleGap);
//	GetDlgItem(IDC_EDIT_NEEDLE_PITCH)->UpdateData(FALSE);
//}


void CDlgProudSet::OnBnClickedRadioComplex()
{
	g_pFrm->m_CmdRun.ProductParam.fillMode = COMPLEX_DISP_MODE;
	g_pFrm->m_CmdRun.V9RestOK = false;
}


void CDlgProudSet::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
void CDlgProudSet::OnBnClickedCheckMesOffline()
{
	g_pFrm->m_CmdRun.ProductParam.UDPOffline = (bool)((CButton*)GetDlgItem(IDC_CHECK_MES_OFFLINE))->GetCheck();
	
}



void CDlgProudSet::OnBnClickedButtonUdpReset()
{
	// TODO: 在此添加控件通知处理程序代码
	//BYTE IPByte[4];
	//CString str;
	//GetDlgItemText(IDC_EDIT_UDP_PORT,str);
	//g_pFrm->m_CmdRun.ProductParam.ServerPort = atoi(str);//读取端口号


	//((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_DUP))->GetAddress(IPByte[0],IPByte[1],IPByte[2],IPByte[3]);
	//g_pFrm->m_CmdRun.ProductParam.ServerIP.Format("%d%s%d%s%d%s%d",IPByte[0],".",IPByte[1],".",IPByte[2],".",IPByte[3]);
	g_pFrm->m_CmdRun.ProductParam.UDPRestConnet = true;
}

void CDlgProudSet::OnBnClickedUdpSaveurl()
{
	// TODO: 在此添加控件通知处理程序代码//保存网址
	CString str;
	GetDlgItemText(IDC_EDIT_UDP_URL,str);
	strcpy(g_pFrm->m_CmdRun.ProductParam.ServerURL,str);

	CString strIP;
    int Port;
	int CompanyCode;

	theApp.ProductParam(FALSE);
	int ret = g_pFrm->m_CmdRun.ReadURLText(g_pFrm->m_CmdRun.ProductParam.ServerURL,strIP,Port,CompanyCode);
	if(ret == -1)
	{
		g_pFrm->m_CmdRun.AddMsg("保存网址后--获取IP、端口和公司代码失败！");
		return;
	}
	strcpy(g_pFrm->m_CmdRun.ProductParam.ServerIP,strIP);
	g_pFrm->m_CmdRun.ProductParam.ServerPort=Port;
	g_pFrm->m_CmdRun.ProductParam.FCompanyCode=CompanyCode;
    theApp.ProductParam(FALSE);
}
