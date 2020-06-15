// DlgMaterialCode.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgMaterialCode.h"


// CDlgMaterialCode dialog

IMPLEMENT_DYNAMIC(CDlgMaterialCode, CDialog)

CDlgMaterialCode::CDlgMaterialCode(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMaterialCode::IDD, pParent)
{

}

CDlgMaterialCode::~CDlgMaterialCode()
{
}

void CDlgMaterialCode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgMaterialCode, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgMaterialCode::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgMaterialCode::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_SEND, &CDlgMaterialCode::OnBnClickedBtnSend)
END_MESSAGE_MAP()

// CDlgMaterialCode message handlers

void CDlgMaterialCode::OnBnClickedOk()
{
	OnOK();
}

void CDlgMaterialCode::OnBnClickedCancel()
{
	OnCancel();
}

void CDlgMaterialCode::OnBnClickedBtnSend()
{
	g_pFrm->m_CmdRun.ProductParam.UDPSendMsgFlag = true;
	g_pFrm->m_CmdRun.bMesDataUpdate=false;   ////数据更新状态
	g_pFrm->m_CmdRun.bMaterialCodeNew=true;  ////记录是否扫描流程单
	GetDlgItemText(IDC_EDIT_MATERIALCODE,g_pFrm->m_CmdRun.ProductParam.UDPSendMsg);
	if(!g_pFrm->m_wndPosInfo.UDPSendMsg())
	{
		AfxMessageBox("MES数据发送异常，请重新发送或检查网络!");
	}	
	//::SendMessage(g_pFrm->m_wndPosInfo.m_hWnd,WM_MSG_GLUE_EVENT,0,1);	
}

BOOL CDlgMaterialCode::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_ESCAPE)
	{
		// 如果消息是键盘按下事件，且是Esc键，执行以下代码（什么都不做，你可以自己添加想要的代码）
		return TRUE;
	}
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		// 如果消息是键盘按下事件，且是Entert键，执行以下代码（什么都不做，你可以自己添加想要的代码）
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}
