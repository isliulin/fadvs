// DlgCustom.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgCustom.h"
#include "DlgAutoAlign.h"


// CDlgCustom dialog

IMPLEMENT_DYNAMIC(CDlgCustom, CDialog)

CDlgCustom::CDlgCustom(CWnd* pParent /*=NULL*/)
: CDialog(CDlgCustom::IDD, pParent)
{

}

CDlgCustom::~CDlgCustom()
{
}

void CDlgCustom::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_PATH,m_wndList);
}


BEGIN_MESSAGE_MAP(CDlgCustom, CDialog)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_CUSTOM_SPEED, &CDlgCustom::OnNMCustomdrawSliderCustomSpeed)
	ON_BN_CLICKED(IDC_BTN_CUSTOM_INSERT, &CDlgCustom::OnBnClickedBtnCustomInsert)
	ON_BN_CLICKED(IDC_BTN_CUSTOM_MODIFY, &CDlgCustom::OnBnClickedBtnCustomModify)
	ON_BN_CLICKED(IDC_BTN_CUSTOM_DELET, &CDlgCustom::OnBnClickedBtnCustomDelet)
	ON_BN_CLICKED(IDOK, &CDlgCustom::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgCustom::OnBnClickedCancel)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_CUSTOM_MOVE, &CDlgCustom::OnBnClickedBtnCustomMove)
	ON_BN_CLICKED(IDC_BTN_CUSTOM_AUTO_ALIGN, &CDlgCustom::OnBnClickedBtnCustomAutoAlign)
	ON_BN_CLICKED(IDC_BTN_CUSTOM_SAVE, &CDlgCustom::OnBnClickedBtnCustomSave)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PATH, &CDlgCustom::OnNMClickListPath)
	ON_BN_CLICKED(IDC_BTN_CUSTOM_LABEL_SAVE, &CDlgCustom::OnBnClickedBtnCustomLabelSave)
END_MESSAGE_MAP()


// CDlgCustom message handlers
BOOL CDlgCustom::OnInitDialog()
{
	CDialog::OnInitDialog();
	g_pFrm->bIsCustomVisible = true;
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,TRUE);//2016-03-13
	for(int i=0;i<6;i++)
	{
		m_colorButton[i].m_iIndex = i;
		m_colorButton[i].SubclassDlgItem(IDC_BTN_CUSTOM_X_NEG+i,this);
		m_colorButton[i].SetBitmapEx(IDB_BITMAP_MOVE);
		m_colorButton[i].m_pMv = &theApp.m_Mv400;
	}
	m_wndList.SetExtendedStyle(m_wndList.GetExtendedStyle()|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES);
	m_wndList.InsertColumn(0, "行数", 0, 40);
	m_wndList.InsertColumn(1, "参数", 0, 400);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_CUSTOM_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_CUSTOM_SPEED))->SetPos(1);
	SetTimer(0,200,NULL);
	UpateCmdListBox(g_pFrm->m_rawList.GetCount()-1);
	UpdateUI();
	return TRUE;
}

void CDlgCustom::OnClose()
{
	if(theApp.m_Mv400.IsInitOK())
	{
		if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_STOP)
		{
			g_pFrm->m_CmdRun.MoveToZSafety();
		}
	}
	g_pFrm->bIsCustomVisible = false;
	g_pFrm->m_CmdRun.ProductParam.ListElementNum = g_pFrm->m_rawList.GetCount();
	theApp.ProductParam(FALSE);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,FALSE);//2016-03-13
	KillTimer(0);
}

void CDlgCustom::OnNMCustomdrawSliderCustomSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_CUSTOM_SPEED))->GetPos();
	CColorButton::m_iSpeedNo = n;
	CString strTemp;
	strTemp.Format("X:%.4f,Y:%.4f,ZA:%.4f", theApp.m_tSysParam.tAxis[0].vel[0] * n/2000.0,
		theApp.m_tSysParam.tAxis[1].vel[0] * n/2000.0,theApp.m_tSysParam.tAxis[2].vel[0] * n/2000.0);
	SetDlgItemText(IDC_EDIT_CUSTOM_SPEED,strTemp);
	*pResult = 0;
}

void CDlgCustom::OnBnClickedBtnCustomInsert()
{
	// TODO: Add your control notification handler code here
	int nSelIndex = m_wndList.GetItemCount();
	if(nSelIndex<0)
	{
		nSelIndex = 0;
	}
	tgPos tgCurrentPos;
	tgCurrentPos.x = theApp.m_Mv400.GetPos(K_AXIS_X) - g_pFrm->m_CmdRun.ProductParam.tgCustomLabelPos.x;
	tgCurrentPos.y = theApp.m_Mv400.GetPos(K_AXIS_Y) - g_pFrm->m_CmdRun.ProductParam.tgCustomLabelPos.y;
	tgCurrentPos.za = theApp.m_Mv400.GetPos(K_AXIS_ZA) - g_pFrm->m_CmdRun.ProductParam.tgCustomLabelPos.za;
	g_pFrm->m_rawList.AddTail(tgCurrentPos);
	UpateCmdListBox(nSelIndex);
}

void CDlgCustom::OnBnClickedBtnCustomModify()
{
	// TODO: Add your control notification handler code here
	tgPos tgTempPos;
	int nSelIndex = m_wndList.GetNextItem(-1,LVNI_SELECTED);
	if(nSelIndex<0)
	{
		nSelIndex = 0;
	}
	for(int i=m_wndList.GetItemCount()-1;i>-1;i--)
	{
		if(m_wndList.GetItemState(i,LVNI_SELECTED)==LVNI_SELECTED)
		{
			g_pFrm->m_rawList.GetAt(g_pFrm->m_rawList.FindIndex(i)).x = theApp.m_Mv400.GetPos(K_AXIS_X) - g_pFrm->m_CmdRun.ProductParam.tgCustomLabelPos.x;
			g_pFrm->m_rawList.GetAt(g_pFrm->m_rawList.FindIndex(i)).y = theApp.m_Mv400.GetPos(K_AXIS_Y) - g_pFrm->m_CmdRun.ProductParam.tgCustomLabelPos.y;
			g_pFrm->m_rawList.GetAt(g_pFrm->m_rawList.FindIndex(i)).za = theApp.m_Mv400.GetPos(K_AXIS_ZA) - g_pFrm->m_CmdRun.ProductParam.tgCustomLabelPos.za;
		}
	}
	if(nSelIndex>m_wndList.GetItemCount()-1)
	{
		nSelIndex = m_wndList.GetItemCount()-1;
	}
	UpateCmdListBox(nSelIndex);
}

void CDlgCustom::OnBnClickedBtnCustomDelet()
{
	// TODO: Add your control notification handler code here
	int nSelIndex = m_wndList.GetNextItem(-1,LVNI_SELECTED);
	if(nSelIndex<0)
	{
		nSelIndex = 0;
	}
	for(int i=m_wndList.GetItemCount()-1;i>-1;i--)
	{
		if(m_wndList.GetItemState(i,LVNI_SELECTED)==LVNI_SELECTED)
		{
			g_pFrm->m_rawList.RemoveAt(g_pFrm->m_rawList.FindIndex(i));
			m_wndList.DeleteItem(i);
		}
	}
	if(nSelIndex>m_wndList.GetItemCount()-1)
	{
		nSelIndex = m_wndList.GetItemCount()-1;
	}
	UpateCmdListBox(nSelIndex);
}

void CDlgCustom::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnClose();
	OnOK();
}

void CDlgCustom::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnClose();
	OnCancel();
}

void CDlgCustom::OnTimer(UINT_PTR nIDEvent)
{
	CString str;
	for(int i=0;i<3;i++)
	{
		double dPos = theApp.m_Mv400.GetPos(i+1);
		str.Format("%0.3f",dPos);
		SetDlgItemText(IDC_EDIT_CUSTOM_CURRENT_X_POS+i,str);
	}
}

void CDlgCustom::UpateCmdListBox(int nIndex)
{
	CListCtrl *pList = (CListCtrl*)GetDlgItem(IDC_LIST_PATH);
	CString strTemp,strText;
	pList->DeleteAllItems();

	POSITION pos;
	tgPos tgTempPos;
	m_wndList.SetRedraw(FALSE);
	for(int i=0;i<g_pFrm->m_rawList.GetCount();i++)
	{
		pos = g_pFrm->m_rawList.FindIndex(i);
		tgTempPos = g_pFrm->m_rawList.GetAt(pos);
		strTemp.Format("%02d",i);
		pList->InsertItem(i,strTemp);
		strText = "Point(";
		strTemp.Format("%0.3f",tgTempPos.x);
		strText += strTemp;
		strText += ",";
		strTemp.Format("%0.3f",tgTempPos.y);
		strText += strTemp;
		strText += ",";
		strTemp.Format("%0.3f",tgTempPos.za);
		strText += strTemp;
		strText += ")";
		pList->SetItemText(i,1,strText);
	}

	m_wndList.SetRedraw(TRUE);

	if(nIndex<0 || nIndex > m_wndList.GetItemCount()-1)
	{
		if (m_wndList.GetItemCount()>0)
		{
			for(int i=m_wndList.GetItemCount()-1; i>-1; i--)
			{
				if(m_wndList.GetItemState(i, LVNI_SELECTED) == LVNI_SELECTED)
				{
					m_wndList.SetItemState(i, 0, LVIS_SELECTED|LVIS_FOCUSED);
					break;
				}
			}
		}
		return;
	}

	m_wndList.EnsureVisible(nIndex, FALSE);  
	for(int i=m_wndList.GetItemCount()-1; i>-1; i--)
	{
		if(m_wndList.GetItemState(i, LVNI_SELECTED) == LVNI_SELECTED)
		{
			m_wndList.SetItemState(i, 0, LVIS_SELECTED|LVIS_FOCUSED);
			break;
		}
	}
	m_wndList.SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);  
	m_wndList.SetFocus();
	CString str;
	m_wndList.SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);  
	m_wndList.SetFocus();
	m_nSelIndex = m_wndList.GetNextItem(-1,LVNI_SELECTED);
	tgPos pointPos = g_pFrm->m_rawList.GetAt(g_pFrm->m_rawList.FindIndex(m_nSelIndex));
	str.Format("%0.3f",pointPos.x);
	SetDlgItemText(IDC_EDIT_CUSTOM_X_POS,str);
	str.Format("%0.3f",pointPos.y);
	SetDlgItemText(IDC_EDIT_CUSTOM_Y_POS,str);
	str.Format("%0.3f",pointPos.za);
	SetDlgItemText(IDC_EDIT_CUSTOM_ZA_POS,str);
}

void CDlgCustom::OnBnClickedBtnCustomMove()
{
	// TODO: Add your control notification handler code here
	int nSelIndex = m_wndList.GetNextItem(-1,LVNI_SELECTED);
	if(nSelIndex<0)
	{
		nSelIndex = 0;
	}
	if(nSelIndex>m_wndList.GetItemCount()-1)
	{
		nSelIndex = m_wndList.GetItemCount()-1;
	}
	tgPos tgCurrentPos = g_pFrm->m_rawList.GetAt(g_pFrm->m_rawList.FindIndex(nSelIndex));
	if(theApp.m_Mv400.IsInitOK())
	{
		if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
		{
			AfxMessageBox("运行状态不能移动！");
			return;
		}
		if (g_pFrm->m_CmdRun.ManalRun)
		{
			if(!g_pFrm->m_CmdRun.MoveToZSafety())
			{
				return;
			}
			//theApp.m_Mv400.MoveXY(tgCurrentPos.x,tgCurrentPos.y,g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
			if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgCurrentPos.x+g_pFrm->m_CmdRun.ProductParam.tgCustomLabelPos.x,
				tgCurrentPos.y+g_pFrm->m_CmdRun.ProductParam.tgCustomLabelPos.y,true))
			{
				return;
			}
			if(AfxMessageBox("确定移动到Z轴坐标？",MB_YESNO)==IDYES)
			{
				if(!g_pFrm->m_CmdRun.SynchronizeMoveZ(tgCurrentPos.za+g_pFrm->m_CmdRun.ProductParam.tgCustomLabelPos.za,true))
				{
					return;
				}
			}
		}
	}
}

void CDlgCustom::OnBnClickedBtnCustomAutoAlign()
{
	// TODO: Add your control notification handler code here
	int nSelIndex = m_wndList.GetItemCount();
	if(nSelIndex<0)
	{
		nSelIndex = 0;
	}
	g_pFrm->m_dlgAutoAlign.DoModal();
	UpateCmdListBox(nSelIndex);
	UpdateUI();
}

void CDlgCustom::OnBnClickedBtnCustomSave()
{
	// TODO: Add your control notification handler code here
	CString str;
	tgPos tgTempPos;
	GetDlgItemText(IDC_EDIT_CUSTOM_X_POS,str);
	tgTempPos.x = atof(str);
	GetDlgItemText(IDC_EDIT_CUSTOM_Y_POS,str);
	tgTempPos.y = atof(str);
	GetDlgItemText(IDC_EDIT_CUSTOM_ZA_POS,str);
	tgTempPos.za = atof(str);
	int nSelIndex = m_wndList.GetNextItem(-1,LVNI_SELECTED);
	if(nSelIndex<0)
	{
		nSelIndex = 0;
	}
	for(int i=m_wndList.GetItemCount()-1;i>-1;i--)
	{
		if(m_wndList.GetItemState(i,LVNI_SELECTED)==LVNI_SELECTED)
		{
			g_pFrm->m_rawList.GetAt(g_pFrm->m_rawList.FindIndex(i)).x = tgTempPos.x;
			g_pFrm->m_rawList.GetAt(g_pFrm->m_rawList.FindIndex(i)).y = tgTempPos.y;
			g_pFrm->m_rawList.GetAt(g_pFrm->m_rawList.FindIndex(i)).za = tgTempPos.za;
		}
	}
	if(nSelIndex>m_wndList.GetItemCount()-1)
	{
		nSelIndex = m_wndList.GetItemCount()-1;
	}
	UpateCmdListBox(nSelIndex);
}

void CDlgCustom::OnNMClickListPath(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	CString str;
	if (0 >= g_pFrm->m_rawList.GetCount())
	{
		return;
	}
	m_nSelIndex = m_wndList.GetNextItem(-1,LVNI_SELECTED);
	if(m_nSelIndex<0)
	{
		m_nSelIndex = 0;
		m_wndList.SetItemState(m_nSelIndex, LVIS_SELECTED, LVIS_SELECTED);  
		m_wndList.SetFocus();
	}
	if(m_nSelIndex>m_wndList.GetItemCount()-1)
	{
		m_nSelIndex = m_wndList.GetItemCount()-1;
		m_wndList.SetItemState(m_nSelIndex, LVIS_SELECTED, LVIS_SELECTED);  
		m_wndList.SetFocus();
	}
	tgPos pointPos = g_pFrm->m_rawList.GetAt(g_pFrm->m_rawList.FindIndex(m_nSelIndex));
	str.Format("%0.3f",pointPos.x);
	SetDlgItemText(IDC_EDIT_CUSTOM_X_POS,str);
	str.Format("%0.3f",pointPos.y);
	SetDlgItemText(IDC_EDIT_CUSTOM_Y_POS,str);
	str.Format("%0.3f",pointPos.za);
	SetDlgItemText(IDC_EDIT_CUSTOM_ZA_POS,str);
	*pResult = 0;
}

void CDlgCustom::UpdateUI()
{
	CString str;
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgCustomLabelPos.x);
	SetDlgItemText(IDC_EDIT_CUSTOM_LABEL_X,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgCustomLabelPos.y);
	SetDlgItemText(IDC_EDIT_CUSTOM_LABEL_Y,str);
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.tgCustomLabelPos.za);
	SetDlgItemText(IDC_EDIT_CUSTOM_LABEL_ZA,str);
}

void CDlgCustom::OnBnClickedBtnCustomLabelSave()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.ProductParam.tgCustomLabelPos.x = theApp.m_Mv400.GetPos(K_AXIS_X);
	g_pFrm->m_CmdRun.ProductParam.tgCustomLabelPos.y = theApp.m_Mv400.GetPos(K_AXIS_Y);
	g_pFrm->m_CmdRun.ProductParam.tgCustomLabelPos.za = theApp.m_Mv400.GetPos(K_AXIS_ZA);
	UpdateUI();
}
