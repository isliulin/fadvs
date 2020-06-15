// ColorButton.cpp : implementation file

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "ColorButton.h"

// CColorButton
IMPLEMENT_DYNAMIC(CColorButton, CButton)

int CColorButton::m_iSpeedNo=1;
CColorButton::CColorButton()
{
	m_bOver = FALSE;
	m_bTracking = FALSE;
	m_bSelected = FALSE;
	m_bFocus = FALSE;
	m_bDisable = FALSE;
	m_bButtonDown = FALSE;
	m_iIndex = 0;
	m_Font.CreatePointFont(75, "Arial Black", NULL);
	m_pMv = NULL;
}

CColorButton::~CColorButton()
{
	m_Font.DeleteObject();
}

BEGIN_MESSAGE_MAP(CColorButton, CButton)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CColorButton message handlers

BOOL CColorButton::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	return CButton::PreCreateWindow(cs);
}

void CColorButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//从lpDrawItemStruct获取控件的相关信息
	CRect rect=lpDrawItemStruct->rcItem;
	CDC *pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	int nSaveDC=pDC->SaveDC();

	UINT state = lpDrawItemStruct->itemState;
	CString strText;
	GetWindowText(strText);
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
	pDC->SetTextColor(RGB(200,0,250));
	pOldBitmap = cdc.SelectObject(&m_Bitmap);
	pDC->StretchBlt(0,0,rect.Width(), rect.Height(), &cdc, 48*m_iIndex, 0, 48, 48, SRCCOPY);
	pDC->TextOut(2,22, strText);
	CPen Pen(PS_SOLID, 2, RGB(255,0,50));
	CPen *pOldPen = pDC->SelectObject(&Pen);
	cdc.SelectObject(pOldBitmap);
	cdc.DeleteDC();
	//Draw OutSide
	CGdiObject *pOldBrush = pDC->SelectStockObject(NULL_BRUSH);
	if(m_bOver)	pDC->RoundRect(rect, CPoint(5,5));
	//////////////////////////////////////////////////////////////////////////
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldFont);
	pDC->SetBkMode(iOldMode);
	pDC->RestoreDC(nSaveDC);
	Pen.DeleteObject();
}

void CColorButton::OnMouseMove(UINT nFlags, CPoint point) 
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


LRESULT CColorButton::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bOver = FALSE;
	m_bTracking = FALSE;
	InvalidateRect(NULL, FALSE);
	return 0;
}

LRESULT CColorButton::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	m_bOver = TRUE;
	InvalidateRect(NULL);
	return 0;
}

void CColorButton::SetBitmapEx(UINT ID)
{
	m_Bitmap.DeleteObject();
	m_Bitmap.LoadBitmap(ID);
}
void CColorButton::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	SetButtonStyle(GetButtonStyle() |BS_OWNERDRAW |BS_BITMAP);
	CButton::PreSubclassWindow();
}

void CColorButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CButton::OnLButtonUp(nFlags, point);

	m_bButtonDown = FALSE;

	if(m_pMv != NULL)
	{
		ReleaseCapture();
		m_pMv->StopMove();
		LONGLONG llDueTime = CTimeUtil::GetDueTime(500);
		while(m_pMv->IsMove()&&!CTimeUtil::IsTimeout(llDueTime))
		{
			m_pMv->StopMove();
		}
		if(m_pMv->IsMove())
		{
			AfxMessageBox("CColorButton：电机运动停止失败！");
		}
		if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
		{
			AfxMessageBox("CColorButton：设置电机参数失败！");
		}
	}
}

void CColorButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CButton::OnLButtonDown(nFlags, point);
	if(m_pMv != NULL)
	{
		SetCapture();
		switch(GetDlgCtrlID())
		{
		case IDC_BUTTON_X_NEG:
		case IDC_BTN_RCD_X_NEG:
		case IDC_BTN_HEIGHT_X_NEG:
		case IDC_BTN_NEEDLE_X_NEG:
		case IDC_BTN_ALIGN_X_NEG:
		case IDC_BTN_CUSTOM_X_NEG:
		case IDC_BTN_AUTO_ALIGN_X_NEG:
		case IDC_BTN_MANUAL_ALIGN_X_NEG:
		case IDC_BTN_DEBUG_NEEDLE_X_NEG:
		case IDC_BTN_IMAGE_X_NEG:
		case IDC_BTN_DEBUG_HEIGHT_X_NEG:
		case IDC_BTN_SETPOS_X_NEG:
		case IDC_BTN_HEIGHT_CALIB_X_NEG:
		case IDC_BTN_HEIGHT_POS_X_NEG:
		case IDC_BTN_PAD_DETECT_X_NEG:
		case IDC_BTN_LABEL_X_NEG:
		case IDC_BTN_AUTO_NEEDLE_X_NEG:
			m_pMv->MoveBtn(K_AXIS_X, m_iSpeedNo, -1);
			break;
		case IDC_BUTTON_X_POS:
		case IDC_BTN_RCD_X_POS:
		case IDC_BTN_HEIGHT_X_POS:
		case IDC_BTN_NEEDLE_X_POS:
		case IDC_BTN_ALIGN_X_POS:
		case IDC_BTN_CUSTOM_X_POS:
		case IDC_BTN_AUTO_ALIGN_X_POS:
		case IDC_BTN_MANUAL_ALIGN_X_POS:
		case IDC_BTN_DEBUG_NEEDLE_X_POS:
		case IDC_BTN_IMAGE_X_POS:
		case IDC_BTN_DEBUG_HEIGHT_X_POS:
		case IDC_BTN_SETPOS_X_POS:
		case IDC_BTN_HEIGHT_CALIB_X_POS:
		case IDC_BTN_HEIGHT_POS_X_POS:
		case IDC_BTN_PAD_DETECT_X_POS:
		case IDC_BTN_LABEL_X_POS:
		case IDC_BTN_AUTO_NEEDLE_X_POS:
			m_pMv->MoveBtn(K_AXIS_X, m_iSpeedNo, 1);
			break;
		case IDC_BUTTON_Y_NEG:
		case  IDC_BTN_AUTO_NEEDLE_Y_NEG:  //0513
		case IDC_BTN_RCD_Y_NEG:
		case IDC_BTN_HEIGHT_Y_NEG:
		case IDC_BTN_NEEDLE_Y_NEG:
		case IDC_BTN_ALIGN_Y_NEG:
		case IDC_BTN_CUSTOM_Y_NEG:
		case IDC_BTN_AUTO_ALIGN_Y_NEG:
		case IDC_BTN_MANUAL_ALIGN_Y_NEG:
		case IDC_BTN_DEBUG_NEEDLE_Y_NEG:
		case IDC_BTN_IMAGE_Y_NEG:
		case IDC_BTN_DEBUG_HEIGHT_Y_NEG:
		case IDC_BTN_SETPOS_Y_NEG:
		case IDC_BTN_HEIGHT_CALIB_Y_NEG:
		case IDC_BTN_HEIGHT_POS_Y_NEG:
		case IDC_BTN_PAD_DETECT_Y_NEG:
		case IDC_BTN_LABEL_Y_NEG:
			m_pMv->MoveBtn(K_AXIS_Y, m_iSpeedNo, -1);
			break;
		case IDC_BUTTON_Y_POS:
		case IDC_BTN_AUTO_NEEDLE_Y_POS:
		case IDC_BTN_RCD_Y_POS:
		case IDC_BTN_HEIGHT_Y_POS:
		case IDC_BTN_NEEDLE_Y_POS:
		case IDC_BTN_ALIGN_Y_POS:
		case IDC_BTN_CUSTOM_Y_POS:
		case IDC_BTN_AUTO_ALIGN_Y_POS:
		case IDC_BTN_MANUAL_ALIGN_Y_POS:
		case IDC_BTN_DEBUG_NEEDLE_Y_POS:
		case IDC_BTN_IMAGE_Y_POS:
		case IDC_BTN_DEBUG_HEIGHT_Y_POS:
		case IDC_BTN_SETPOS_Y_POS:
		case IDC_BTN_HEIGHT_CALIB_Y_POS:
		case IDC_BTN_HEIGHT_POS_Y_POS:
		case IDC_BTN_PAD_DETECT_Y_POS:
		case IDC_BTN_LABEL_Y_POS:
			m_pMv->MoveBtn(K_AXIS_Y, m_iSpeedNo, 1);
			break;
		case IDC_BUTTON_ZA_NEG:
		case IDC_BTN_AUTO_NEEDLE_ZA_NEG:
		case IDC_BTN_RCD_Z_NEG:
		case IDC_BTN_HEIGHT_ZA_NEG:
		case IDC_BTN_NEEDLE_ZA_NEG:
		case IDC_BTN_ALIGN_ZA_NEG:
		case IDC_BTN_CUSTOM_ZA_NEG:
		case IDC_BTN_AUTO_ALIGN_ZA_NEG:
		case IDC_BTN_DEBUG_NEEDLE_Z_NEG:
		case IDC_BTN_DEBUG_HEIGHT_Z_NEG:
		case IDC_BTN_SETPOS_Z_NEG:
		case IDC_BTN_HEIGHT_CALIB_Z_NEG:
		case IDC_BTN_PAD_DETECT_Z_NEG:
		case IDC_BTN_LABEL_ZA_NEG:
		case IDC_BTN_IMAGE_ZA_NEG:
		case IDC_BTN_HEIGHT_POS_ZA_NEG:
			m_pMv->MoveBtn(K_AXIS_ZA, m_iSpeedNo, -1);
			break;
		case IDC_BUTTON_ZA_POS:
		case IDC_BTN_AUTO_NEEDLE_ZA_POS:
		case IDC_BTN_RCD_Z_POS:
		case IDC_BTN_HEIGHT_ZA_POS:
		case IDC_BTN_NEEDLE_ZA_POS:
		case IDC_BTN_ALIGN_ZA_POS:
		case IDC_BTN_CUSTOM_ZA_POS:
		case IDC_BTN_AUTO_ALIGN_ZA_POS:
		case IDC_BTN_DEBUG_NEEDLE_Z_POS:
		case IDC_BTN_DEBUG_HEIGHT_Z_POS:
		case IDC_BTN_SETPOS_Z_POS:
		case IDC_BTN_HEIGHT_CALIB_Z_POS:
		case IDC_BTN_PAD_DETECT_Z_POS:
		case IDC_BTN_LABEL_ZA_POS:
		case IDC_BTN_IMAGE_ZA_POS:
		case IDC_BTN_HEIGHT_POS_ZA_POS:
			m_pMv->MoveBtn(K_AXIS_ZA, m_iSpeedNo, 1);
			break;
		case IDC_BUTTON_ZB_NEG:
		//case IDC_BTN_HEIGHT_ZB_NEG:
		case IDC_BTN_LABEL_ZB_NEG:
		case IDC_BTN_AUTO_NEEDLE_ZB_NEG:
		case IDC_BTN_NEEDLE_ZB_NEG:
		case IDC_BTN_SETPOS_ZB_NEG:
			m_pMv->MoveBtn(K_AXIS_ZB, m_iSpeedNo, -1);
			break;
		case IDC_BTN_SETPOS_ZB_POS:
		case IDC_BUTTON_ZB_POS:
		//case IDC_BTN_HEIGHT_ZB_POS:
		case IDC_BTN_LABEL_ZB_POS:
		case IDC_BTN_AUTO_NEEDLE_ZB_POS:
		case IDC_BTN_NEEDLE_ZB_POS:
			m_pMv->MoveBtn(K_AXIS_ZB, m_iSpeedNo, 1);
			break;
		case IDC_BUTTON_ZC_NEG:
		//case IDC_BTN_HEIGHT_ZC_NEG:
		case IDC_BTN_LABEL_ZC_NEG:
		case IDC_BTN_AUTO_NEEDLE_ZC_NEG:
		case IDC_BTN_NEEDLE_ZC_NEG:
		case IDC_BTN_SETPOS_ZC_NEG:
			m_pMv->MoveBtn(K_AXIS_ZC, m_iSpeedNo, -1);
			break;
		case IDC_BTN_SETPOS_ZC_POS:
		case IDC_BUTTON_ZC_POS:
		//case IDC_BTN_HEIGHT_ZC_POS:
		case IDC_BTN_LABEL_ZC_POS:
		case IDC_BTN_AUTO_NEEDLE_ZC_POS:
		case IDC_BTN_NEEDLE_ZC_POS:
			m_pMv->MoveBtn(K_AXIS_ZC, m_iSpeedNo, 1);
			break;
		default:
			NULL;
		}
	}
}