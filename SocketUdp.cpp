// SocketUdp.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "SocketUdp.h"
#include "DlgPosInfo.h"
//���Ӷ�Ӧdlg���ͷ�ļ�

//#include "SocketUDP_ClientAndServer.h"
//#include "SocketUDP_ClientAndServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSocketUdp

CSocketUdp::CSocketUdp()
{
}

CSocketUdp::~CSocketUdp()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CSocketUdp, CSocket)
	//{{AFX_MSG_MAP(CSocketUdp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CSocketUdp member functions
//�Զ����¼���������Ի���Ĺ���
void CSocketUdp::SetParent(CDialog *pWnd)
{
	m_pWnd = pWnd;
}

//���е�Socket�¼�����������Ի�������������ش������£�
void CSocketUdp::OnClose(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if (0==nErrorCode)
	{
		//((CDlgPosInfo*)m_pWnd)->OnSocketClose();
	}
}


void CSocketUdp::OnReceive(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if(0==nErrorCode)
	{
		((CDlgPosInfo*)m_pWnd)->OnReceive();
	}
}
