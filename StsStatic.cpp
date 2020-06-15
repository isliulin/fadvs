// StsStatic.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "StsStatic.h"
#include "MainFrm.h"

// CStsStatic

IMPLEMENT_DYNAMIC(CStsStatic, CStatic)

CStsStatic::CStsStatic()
{
}

CStsStatic::~CStsStatic()
{
}

BEGIN_MESSAGE_MAP(CStsStatic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()
// CStsStatic message handlers
void CStsStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages
	if(GetSafeHwnd() == NULL || g_pFrm == NULL)return;

	CRect rect, rc;
	GetClientRect(&rect);
	dc.SetBkMode(TRANSPARENT);

	rc = rect;
	rc.left=rect.left+3;
	rc.top=rect.top+3;
    rc.bottom=rect.bottom-3;
	rc.right = rect.left + rect.Height()-3;
	CBrush brush;
	CBrush *pOldBrush = NULL;
	CString strTemp;
	dc.Rectangle(rect);
	switch(g_pFrm->m_CmdRun.m_tStatus)
	{
	case K_RUN_STS_NONE:
		brush.CreateSolidBrush(RGB(140,140,200));
		strTemp = "HOME...";
		break;
	case K_RUN_STS_RUN:
		brush.CreateSolidBrush(RGB(0,240,0));
		strTemp = "RUN...";
		break;
	case K_RUN_STS_PAUSE:
		brush.CreateSolidBrush(RGB(240,240,0));
		strTemp = "PAUSE...";
		break;
	case K_RUN_STS_STOP:
		brush.CreateSolidBrush(RGB(240,0,0));
		strTemp = "STOP...";
		break;
	case K_RUN_STS_EMERGENCY:
		brush.CreateSolidBrush(RGB(240,0,100));
		strTemp = "EMERGENCY";
		break;
	default:
		return;
	}
	pOldBrush = dc.SelectObject(&brush);
	dc.Ellipse(rc);
	dc.TextOut(rc.right+5, rc.CenterPoint().y-6, strTemp);
	dc.SelectObject(pOldBrush);
	brush.DeleteObject();
}
