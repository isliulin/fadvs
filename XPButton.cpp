// XPButton.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "XPButton.h"


// CXPButton

IMPLEMENT_DYNAMIC(CXPButton, CButton)

CXPButton::CXPButton()
{
	m_bOver = m_bSelected = m_bTracking = FALSE;
	m_BtnStyle = TRUE;
	m_bCheck = FALSE;
	m_Font = 0;
	Set_BackColor(GetSysColor(COLOR_3DFACE));
	Set_BackColorSel(RGB(41,88,122));
	Set_TextColor(0);
	Set_TextColorSel(0);
}

CXPButton::~CXPButton()
{
	m_BoundryPen.DeleteObject();
	if(m_Font)
	{
		m_Font->DeleteObject();
		delete m_Font;
	}
	m_BackBrush.DeleteObject();
	m_BackBrushSel.DeleteObject();
}


BEGIN_MESSAGE_MAP(CXPButton, CButton)
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER,OnMouseHover)
	ON_MESSAGE(BM_SETCHECK, OnSetCheck)
	ON_MESSAGE(BM_GETCHECK, OnGetCheck)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CXPButton message handlers

void CXPButton::SetCheck(BOOL bCheck, BOOL bRepaint)
{
	m_bCheck = bCheck;

	if (bRepaint) Invalidate();
}

BOOL CXPButton::GetCheck()
{
	return m_bCheck;
}

LRESULT CXPButton::OnSetCheck(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case BST_CHECKED:
	case BST_INDETERMINATE:	// Indeterminate state is handled like checked state
		SetCheck(1);
		break;
	default:
		SetCheck(0);
		break;
	}
	return 0;
}

LRESULT CXPButton::OnGetCheck(WPARAM wParam, LPARAM lParam)
{
	return GetCheck();
}

void CXPButton::OnClicked() 
{
	m_bCheck = !m_bCheck;

	Invalidate();
}

// 添加Owner Draw属性
void CXPButton::PreSubclassWindow()
{
	UINT nBS = GetButtonStyle();
	ASSERT((nBS & SS_TYPEMASK) != BS_OWNERDRAW);
	ASSERT(nBS & BS_CHECKBOX);
	ModifyStyle(SS_TYPEMASK, BS_OWNERDRAW, SWP_FRAMECHANGED);
	CButton::PreSubclassWindow();
}

void CXPButton::OnMouseMove(UINT nFlags, CPoint point)
{
	CButton::OnMouseMove(nFlags,point);
}

LRESULT CXPButton::OnMouseLeave(WPARAM wParam,LPARAM lParam)
{
	m_bOver = FALSE;
	m_bTracking = FALSE;
	InvalidateRect(NULL,FALSE);
	return 0;
}

LRESULT CXPButton::OnMouseHover(WPARAM wParam,LPARAM lParam)
{
	m_bOver = TRUE;
	InvalidateRect(NULL);
	return 0;
}

void CXPButton::SetSelected(BOOL bSel)
{
	if(bSel)
	{
		m_bSelected = TRUE;
	}
	else
	{
		m_bSelected = FALSE;
	}
	Invalidate();
}

void CXPButton::SetBtnStyle(BOOL bStyle)
{
	m_BtnStyle = bStyle;
}

void CXPButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// 从lpDrawItemStruct获取控件的相关信息
	CRect rect = lpDrawItemStruct->rcItem;
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	int nSaveDC = pDC->SaveDC();
	UINT state = lpDrawItemStruct->itemState;
	POINT pt;
	TCHAR strText[MAX_PATH + 1];
	::GetWindowText(m_hWnd,strText,MAX_PATH);
	BOOL preSel = m_bSelected;
	if(state & ODS_SELECTED)	// 按钮按下
	//if(state & ODS_CHECKED)
	{
		m_bSelected = TRUE;
	}
	else
	{
		if(m_BtnStyle)	// 保持按下状态的按钮
		{
			m_bSelected = preSel;
		}
		else	// 不能保持按下状态的按钮
		{
			m_bSelected = FALSE;
		}
	}

	CBrush *pOldBrush;
	//if(m_bSelected)
	if(m_bCheck)
	{
		pDC->SetTextColor(m_TextColorSel);
		m_BackBrushSel.DeleteObject();
		m_BackBrushSel.CreateSolidBrush(m_BackColorSel);
		pOldBrush = pDC->SelectObject(&m_BackBrushSel);
		m_BoundryPen.DeleteObject();
		m_BoundryPen.CreatePen(PS_INSIDEFRAME|PS_SOLID,1,m_BackColorSel);
	}
	else
	{
		pDC->SetTextColor(m_TextColor);
		m_BackBrush.DeleteObject();
		m_BackBrush.CreateSolidBrush(m_BackColor);
		pOldBrush = pDC->SelectObject(&m_BackBrush);
		m_BoundryPen.DeleteObject();
		m_BoundryPen.CreatePen(PS_INSIDEFRAME|PS_SOLID,1,m_BackColor);
	}

	// 按钮的外边框，它是半径为10的圆角矩形
	pt.x = 10;
	pt.y = 10;
	CPen* hOldPen = pDC->SelectObject(&m_BoundryPen);
	pDC->RoundRect(&rect,pt);
	pDC->SelectObject(hOldPen);

	rect.DeflateRect(CSize(GetSystemMetrics(SM_CXEDGE),GetSystemMetrics(SM_CYEDGE)));
	CFont *pOldFont = NULL;
	if(m_Font)
	{
		pOldFont = pDC->SelectObject(m_Font);
	}
	// 显示按钮文本
	if(strText!=NULL)
	{
		CFont* hFont = GetFont();
		CFont* hOldFont = pDC->SelectObject(hFont);
		CSize szExtent = pDC->GetTextExtent(strText,lstrlen(strText));
		CPoint pt(rect.CenterPoint().x - szExtent.cx/2,rect.CenterPoint().y - szExtent.cy/2);
		if(state & ODS_SELECTED)
		//if(state & ODS_CHECKED)
		{
			pt.Offset(1,1);
		}
		int nMode = pDC->SetBkMode(TRANSPARENT);
		if(state & ODS_SELECTED)
		//if(state & ODS_CHECKED)
		{
			pDC->DrawState(pt,szExtent,strText,DSS_DISABLED,TRUE,0,(HBRUSH)NULL);
		}
		else
		{
			pDC->DrawState(pt,szExtent,strText,DSS_NORMAL,TRUE,0,(HBRUSH)NULL);
		}
		pDC->SelectObject(hOldFont);
		pDC->SetBkMode(nMode);
	}
	pDC->SelectObject(pOldFont);
	pDC->SelectObject(pOldBrush);
	pDC->RestoreDC(nSaveDC);
}

BOOL CXPButton::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CXPButton::Set_BackColor(COLORREF cr)
{
	m_BackColor = cr;
	if(GetSafeHwnd())
	{
		Invalidate();
	}
}
// 设置选中时的背景色
void CXPButton::Set_BackColorSel(COLORREF cr)
{
	m_BackColorSel = cr;
	if(GetSafeHwnd())
	{
		Invalidate();
	}
}

void CXPButton::SetFont(LOGFONT *pLogFont, BOOL bRedraw /* = TRUE */)
{
	if(m_Font)
	{
		m_Font->DeleteObject();
	}
	if(m_Font == NULL)
	{
		m_Font = new CFont();
	}
	if(m_Font)
	{
		if(!m_Font->CreatePointFontIndirect(pLogFont))
		{
			delete m_Font;
			m_Font = NULL;
		}
	}
}

void CXPButton::SetFont(CFont* pFont, BOOL bRedraw /* = TRUE */)
{
	LOGFONT logFont;
	pFont->GetLogFont(&logFont);
	SetFont(&logFont,bRedraw);
}

void CXPButton::SetFont(int nHeight, LPCTSTR fontName, BOOL bRedraw /* = TRUE */)
{
	if(m_Font)
	{
		m_Font->DeleteObject();
	}
	if(m_Font == NULL)
	{
		m_Font = new CFont();
	}
	if(m_Font)
	{
		if(!m_Font->CreatePointFont(nHeight,fontName))
		{
			delete m_Font;
			m_Font = NULL;
		}
	}
	if(bRedraw && GetSafeHwnd())
	{
		Invalidate();
	}
}