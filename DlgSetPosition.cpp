// DlgSetPosition.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgSetPosition.h"


// CDlgSetPosition dialog

IMPLEMENT_DYNAMIC(CDlgSetPosition, CDialog)

CDlgSetPosition::CDlgSetPosition(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetPosition::IDD, pParent)
{

}

CDlgSetPosition::~CDlgSetPosition()
{
}

void CDlgSetPosition::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSetPosition, CDialog)
	ON_WM_TIMER()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SETPOS_SPEED, &CDlgSetPosition::OnNMCustomdrawSliderSetposSpeed)
	ON_BN_CLICKED(IDOK, &CDlgSetPosition::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgSetPosition::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_SETPOS_NEXT, &CDlgSetPosition::OnBnClickedBtnSetposNext)
END_MESSAGE_MAP()


// CDlgSetPosition message handlers

BOOL CDlgSetPosition::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString str;
	for(int i=0;i<10;i++) //6->10
	{
		m_colorButton[i].m_iIndex = i;
		m_colorButton[i].SubclassDlgItem(IDC_BTN_SETPOS_X_NEG+i, this);
		m_colorButton[i].SetBitmapEx(IDB_BITMAP_MOVE);
		m_colorButton[i].m_pMv = &theApp.m_Mv400;
	}
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_SETPOS_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_SETPOS_SPEED))->SetPos(1);
	UpdatePos();
	m_nIndex = g_pFrm->m_CmdRun.ProductParam.TestDotNumber;
	m_nCount = 0;
	str.Format("请移动到%d个试胶位置",m_nCount);
	SetDlgItemText(IDC_EDIT_SETPOS_PROMPT,str);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	SetTimer(0,100,NULL);
	return TRUE;
}

void CDlgSetPosition::UpdatePos()
{
	CString str;
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_X));
	SetDlgItemText(IDC_EDIT_SETPOS_POS_X,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_Y));
	SetDlgItemText(IDC_EDIT_SETPOS_POS_Y,str);
	str.Format("%0.3f",theApp.m_Mv400.GetPos(K_AXIS_ZA));
	SetDlgItemText(IDC_EDIT_SETPOS_POS_Z,str);
}
void CDlgSetPosition::OnNMCustomdrawSliderSetposSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_SETPOS_SPEED))->GetPos();
	CColorButton::m_iSpeedNo = n;
	CString strTemp;
	strTemp.Format("X:%.3f,Y:%.3f,Z:%.3f", theApp.m_tSysParam.tAxis[K_AXIS_X-1].vel[0] * n/2000.0,
		theApp.m_tSysParam.tAxis[K_AXIS_Y-1].vel[0]*n/2000.0,theApp.m_tSysParam.tAxis[K_AXIS_ZA-1].vel[0] * n/2000.0);
	SetDlgItemText(IDC_EDIT_SETPOS_SPEED, strTemp);
	*pResult = 0;
}

void CDlgSetPosition::SavePos()
{
	for(int i=0;i<10;i++)
	{
		g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].x = m_tgTestPos[i].x;
		g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].y = m_tgTestPos[i].y;
		g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].za = m_tgTestPos[i].za;
		g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].zb = m_tgTestPos[i].zb;
		g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].zc = m_tgTestPos[i].zc;
	}
}

void CDlgSetPosition::OnTimer(UINT_PTR nIDEvent)
{
	UpdatePos();
	CDialog::OnTimer(nIDEvent);
}
void CDlgSetPosition::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	SavePos();
	KillTimer(0);
	OnOK();
}

void CDlgSetPosition::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	KillTimer(0);
	OnCancel();
}

void CDlgSetPosition::OnBnClickedBtnSetposNext()
{
	// TODO: Add your control notification handler code here
	if(m_nCount<0||m_nCount>=m_nIndex)
	{
		return;
	}
	//试胶位Z坐标：以Za为参考，相对关系取决于测高时相对位置；
	m_tgTestPos[m_nCount].x = theApp.m_Mv400.GetPos(K_AXIS_X);
	m_tgTestPos[m_nCount].y = theApp.m_Mv400.GetPos(K_AXIS_Y);
	m_tgTestPos[m_nCount].za = theApp.m_Mv400.GetPos(K_AXIS_ZA);
	m_tgTestPos[m_nCount].zb = theApp.m_Mv400.GetPos(K_AXIS_ZA) + (g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[3] - g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]);
	m_tgTestPos[m_nCount].zc = theApp.m_Mv400.GetPos(K_AXIS_ZA) + (g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[4] - g_pFrm->m_CmdRun.ProductParam.ContactSensorPos[2]);
	//m_tgTestPos[m_nCount].zb = theApp.m_Mv400.GetPos(K_AXIS_ZB);
	//m_tgTestPos[m_nCount].zc = theApp.m_Mv400.GetPos(K_AXIS_ZC);

	m_nCount++;
	CString str;
	str.Format("请将相机移动到%d个位置",m_nCount);
	SetDlgItemText(IDC_EDIT_SETPOS_PROMPT,str);
	if(m_nCount>=m_nIndex)
	{
		str.Format("完成");
		SetDlgItemText(IDC_EDIT_SETPOS_PROMPT,str);
		GetDlgItem(IDC_BTN_SETPOS_NEXT)->EnableWindow(FALSE);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
}
