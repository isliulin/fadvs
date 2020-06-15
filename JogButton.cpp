// ColorButton.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "JogButton.h"


// CColorButton

IMPLEMENT_DYNAMIC(CJogButton, CButton)

int CJogButton::m_dJogSpdRatio = 1;
CJogButton::CJogButton()
{
	m_bOver = FALSE;
	m_bTracking = FALSE;
	m_bSelected = FALSE;
	m_bFocus = FALSE;
	m_bDisable = FALSE;
	m_iIndex = 0;
	m_Font.CreatePointFont(100, "Times New Roman", NULL);

}

CJogButton::~CJogButton()
{
	m_Font.DeleteObject();
}


BEGIN_MESSAGE_MAP(CJogButton, CButton)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CColorButton message handlers

BOOL CJogButton::PreCreateWindow(CREATESTRUCT& cs)
{
	return CButton::PreCreateWindow(cs);
}

void CJogButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//从lpDrawItemStruct获取控件的相关信息
	CRect rect=lpDrawItemStruct->rcItem;
	CDC *pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	int nSaveDC=pDC->SaveDC();

	UINT state = lpDrawItemStruct->itemState;
	CString strText;
	GetWindowText(strText);    //GET窗口文字:通过Caption设定.
	int iOldMode = pDC->SetBkMode(TRANSPARENT);
	CFont *pOldFont = pDC->SelectObject(&m_Font);
	pDC->SetTextColor(RGB(15, 88, 110));

	//////////////////////////////////////////////////////////////////////////	
	//获取按钮的状态
	if (state & ODS_FOCUS)
	{
		m_bFocus = TRUE;
		m_bSelected = TRUE;
	}
	else
	{
		m_bFocus = FALSE;
		m_bSelected = FALSE;
	}

	if (state & ODS_SELECTED || state & ODS_DEFAULT)
	{
		m_bFocus = TRUE;
	}

	//////////////////////////////////////////////////////////////////////////
	CSize Size = pDC->GetTextExtent(strText);
	CDC cdc;
	cdc.CreateCompatibleDC(pDC);
	CBitmap *pOldBitmap = NULL;
	pDC->SetTextColor(RGB(0,0,0));//黑色字体 //200,0,250
	pOldBitmap = cdc.SelectObject(&m_Bitmap); //加载设定的位图***
	pDC->StretchBlt(0,0,rect.Width(), rect.Height(), &cdc, 48*m_iIndex, 0, 48, 48, SRCCOPY);
	pDC->TextOut(2,16, strText);

	/*
	if (m_bSelected&&m_bFocus)
	pDC->Draw3dRect(rect, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT));
	else
	pDC->Draw3dRect(rect, ::GetSysColor(COLOR_BTNHIGHLIGHT), ::GetSysColor(COLOR_BTNSHADOW));
	*/

	CPen Pen(PS_SOLID, 2, RGB(0,0,250));
	CPen *pOldPen = pDC->SelectObject(&Pen);

	cdc.SelectObject(pOldBitmap);
	cdc.DeleteDC();              //释放CDC

	//Draw OutSide
	CGdiObject *pOldBrush = pDC->SelectStockObject(NULL_BRUSH);
	if(/*m_bOver*/true)	pDC->RoundRect(rect, CPoint(5,5));

	//////////////////////////////////////////////////////////////////////////
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldFont);
	pDC->SetBkMode(iOldMode);
	pDC->RestoreDC(nSaveDC);
	Pen.DeleteObject();
}
void CJogButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}

	CButton::OnMouseMove(nFlags, point);
}


LRESULT CJogButton::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	m_bOver = FALSE;
	m_bTracking = FALSE;
	InvalidateRect(NULL, FALSE);
	return 0;
}

LRESULT CJogButton::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	m_bOver = TRUE;
	m_bTracking = TRUE;
	InvalidateRect(NULL);
	return 0;
}

void CJogButton::SetBitmapEx(UINT ID)
{
	m_Bitmap.DeleteObject();
	m_Bitmap.LoadBitmap(ID);
}
void CJogButton::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	SetButtonStyle(GetButtonStyle() |BS_OWNERDRAW |BS_BITMAP);
	CButton::PreSubclassWindow();
}

void CJogButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CButton::OnLButtonUp(nFlags, point);
	m_bSelected = FALSE;

	if(g_pFrm->m_CmdRun.m_pMv != NULL)
	{
		ReleaseCapture();
		g_pFrm->m_CmdRun.m_pMv->StopMove();

		LONGLONG llDueTime = CTimeUtil::GetDueTime(1000);
		while(g_pFrm->m_CmdRun.m_pMv->IsMoveAdjustMotor()&&!CTimeUtil::IsTimeout(llDueTime))
		{
			g_pFrm->m_CmdRun.m_pMv->StopMove();
		}
		if(g_pFrm->m_CmdRun.m_pMv->IsMoveAdjustMotor())
		{
			AfxMessageBox("CJogButton：调整电机运动停止失败！");
		}
		if(!g_pFrm->m_CmdRun.SetAdjustMotorProfile(MEDIUM_VEL))
		{
			AfxMessageBox("CJogButton：设置调整电机参数失败！");
		}
	}
}

void CJogButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	g_pFrm->m_CmdRun.m_bAutoNeedleErr=false;//20180908 按键时取消报警
	CButton::OnLButtonDown(nFlags, point);
	if(g_pFrm->m_CmdRun.m_pMv->IsInitOK()!=FALSE)
	{
		SetCapture();
		CString str;
		switch(GetDlgCtrlID())
		{
		case IDC_BTN_NEEDLEONE_X_NEG_NDLEONE :
		case IDC_BTN_NEEDLE_X_NEG_INCAM: 
		case IDC_BTN_X_NEG_INCAMADJUST: 
		case IDC_BTN_X_NEG_NDLONEADJUST:
		case IDC_BTN_RB_NDLTWO:
        case IDC_BTN_RB_NDLTHREE:
		case IDC_BTN_X_NEG:
			g_pFrm->m_CmdRun.m_pMv->MoveBtn(K_AXIS_X,m_dJogSpdRatio,-1);
			break;
		case IDC_BTN_LB_NDLTWO:
		case IDC_BTN_NEEDLEONE_X_POS_NDLEONE :
		case IDC_BTN_NEEDLE_X_POS_INCAM:
		case IDC_BTN_X_POS_INCAMADJUST:
		case IDC_BTN_X_POS_NDLONEADJUST:
        case IDC_BTN_LB_NDLTHREE:
		case IDC_BTN_X_POS:
			g_pFrm->m_CmdRun.m_pMv->MoveBtn(K_AXIS_X,m_dJogSpdRatio,1);
			break;
		case IDC_BTN_NEEDLEONE_Y_NEG_NDLEONE :
		case IDC_BTN_NEEDLE_Y_NEG_INCAM:
		case IDC_BTN_Y_NEG_INCAMADJUST:
		case IDC_BTN_Y_NEG_NDLONEADJUST:
		case IDC_BTN_RT_NDLTWO:
		case IDC_BTN_RT_NDLTHREE:
		case IDC_BTN_Y_NEG:
			g_pFrm->m_CmdRun.m_pMv->MoveBtn(K_AXIS_Y,m_dJogSpdRatio,-1);
			break;
		case IDC_BTN_NEEDLEONE_Y_POS_NDLEONE :
		case IDC_BTN_NEEDLE_Y_POS_INCAM:
		case IDC_BTN_Y_POS_INCAMADJUST:
		case IDC_BTN_Y_POS_NDLONEADJUST:
		case IDC_BTN_LT_NDLTWO:
		case IDC_BTN_LT_NDLTHREE:
		case IDC_BTN_Y_POS:
			g_pFrm->m_CmdRun.m_pMv->MoveBtn(K_AXIS_Y,m_dJogSpdRatio,1);
			break;
		case IDC_BTN_NEEDLEONE_Z_NEG_NDLEONE:
		case IDC_BTN_NEEDLE_Z_NEG_INCAM: 
		case IDC_BTN_Z_NEG_INCAMADJUST:
		case IDC_BTN_Z_NEG_NDLONEADJUST:
		case IDC_BTN_ZA_NEG:
			g_pFrm->m_CmdRun.m_pMv->MoveBtn(K_AXIS_ZA,m_dJogSpdRatio,-1);
			break;
		case IDC_BTN_NEEDLEONE_Z_POS_NDLEONE :
		case IDC_BTN_NEEDLE_Z_POS_INCAM:
		case IDC_BTN_Z_POS_INCAMADJUST:
		case IDC_BTN_Z_POS_NDLONEADJUST:
		case IDC_BTN_ZA_POS:
			g_pFrm->m_CmdRun.m_pMv->MoveBtn(K_AXIS_ZA,m_dJogSpdRatio,1);
			break;
		case IDC_BTN_NEEDLETWO_Z_NEG_NDLTWO:
		case IDC_BTN_Z_NEG_NDLTWOADJUST:
			g_pFrm->m_CmdRun.m_pMv->MoveBtn(K_AXIS_ZB,m_dJogSpdRatio,-1);
			break;
		case IDC_BTN_NEEDLETWO_Z_POS_NDLTWO:
		case IDC_BTN_Z_POS_NDLTWOADJUST:
			g_pFrm->m_CmdRun.m_pMv->MoveBtn(K_AXIS_ZB,m_dJogSpdRatio,1);
			break;

		case IDC_BTN_NEEDLETHREE_Z_NEG_NDLTHREE :
		case IDC_BTN_Z_NEG_NDLTHREEADJUST:
			g_pFrm->m_CmdRun.m_pMv->MoveBtn(K_AXIS_ZC,m_dJogSpdRatio,-1);
			break;
		case IDC_BTN_NEEDLETHREE_Z_POS_NDLTHREE:
		case IDC_BTN_Z_POS_NDLTHREEADJUST :
			g_pFrm->m_CmdRun.m_pMv->MoveBtn(K_AXIS_ZC,m_dJogSpdRatio,1);
			break;

		case IDC_BTN_NEEDLE_A_NEG:
		case IDC_BTN_NEEDLETWO_A_NEG_NDLTWO:
		case IDC_BTN_A_NEG_NDLTWOADJUST:
			g_pFrm->m_CmdRun.m_pMv->MoveBtn(K_AXIS_A, m_dJogSpdRatio, -1);
			break;
		case IDC_BTN_NEEDLE_A_POS:
		case IDC_BTN_NEEDLETWO_A_POS_NDLTWO:  
		case IDC_BTN_A_POS_NDLTWOADJUST:
			g_pFrm->m_CmdRun.m_pMv->MoveBtn(K_AXIS_A, m_dJogSpdRatio, 1);
			break;
		case IDC_BTN_NEEDLE_B_NEG:
		case IDC_BTN_NEEDLETWO_B_NEG_NDLTWO:
		case IDC_BTN_B_NEG_NDLTWOADJUST:
			g_pFrm->m_CmdRun.m_pMv->MoveBtn(K_AXIS_B, m_dJogSpdRatio, -1);
			break;
		case IDC_BTN_NEEDLE_B_POS:
		case IDC_BTN_NEEDLETWO_B_POS_NDLTWO:
		case IDC_BTN_B_POS_NDLTWOADJUST:
			g_pFrm->m_CmdRun.m_pMv->MoveBtn(K_AXIS_B, m_dJogSpdRatio, 1);
			break;
		case IDC_BTN_NEEDLE_C_NEG:
		case IDC_BTN_NEEDLETHREE_C_NEG_NDLTHREE :
		case IDC_BTN_C_NEG_NDLTHREEADJUST:
			g_pFrm->m_CmdRun.m_pMv->MoveBtn(K_AXIS_C, m_dJogSpdRatio, -1);
			break;
		case IDC_BTN_NEEDLE_C_POS:
		case IDC_BTN_NEEDLETHREE_C_POS_NDLTHREE:
		case IDC_BTN_C_POS_NDLTHREEADJUST:
			g_pFrm->m_CmdRun.m_pMv->MoveBtn(K_AXIS_C, m_dJogSpdRatio, 1);
			break;
		case IDC_BTN_NEEDLE_D_NEG:
		case IDC_BTN_NEEDLETHREE_D_NEG_NDLTHREE:
		case IDC_BTN_D_NEG_NDLTHREEADJUST:
			g_pFrm->m_CmdRun.m_pMv->MoveBtn(K_AXIS_D, m_dJogSpdRatio, -1);
			break;
		case IDC_BTN_NEEDLE_D_POS:
		case IDC_BTN_NEEDLETHREE_D_POS_NDLTHREE: 
		case IDC_BTN_D_POS_NDLTHREEADJUST:
			g_pFrm->m_CmdRun.m_pMv->MoveBtn(K_AXIS_D, m_dJogSpdRatio, 1);
			break;
		default:
			break;
		}
	}
}
