// MyButton.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "MyButton.h"


// CMyButton

IMPLEMENT_DYNAMIC(CMyButton, CButton)

CMyButton::CMyButton()
{
	// 初始化为白色
	m_bCheck = FALSE;
	m_crDownColor = m_crUpColor = RGB(255,255,255);
}

CMyButton::~CMyButton()
{
}

BEGIN_MESSAGE_MAP(CMyButton, CButton)
	//ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CMyButton message handlers
BOOL CMyButton::Attach(const UINT nID, CWnd* pParent)
{
	if(!SubclassDlgItem(nID,pParent))
	{
		return FALSE;
	}
	return TRUE;
}

void CMyButton::SetDownColor(COLORREF color)
{
	m_crDownColor = color;
}

void CMyButton::SetUpColor(COLORREF color)
{
	m_crUpColor = color;
}

void CMyButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);	// 获得绘制的设备环境DC
	VERIFY(lpDrawItemStruct->CtlType==ODT_BUTTON);
	// 获得Button上的文字
	// 将文字重新绘制到按钮上
	// 将文字的背景设置为透明，按钮上仅显示文字
	const int nBufSize = 512;
	TCHAR chBuffer[nBufSize];
	GetWindowText(chBuffer,nBufSize);
	int nSize = strlen(chBuffer);
	DrawText(lpDrawItemStruct->hDC,chBuffer,nSize,&lpDrawItemStruct->rcItem,
		DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_TABSTOP);	// 绘制文字
	SetBkMode(lpDrawItemStruct->hDC,TRANSPARENT);	// 设置为透明
	//if(lpDrawItemStruct->itemState&ODS_SELECTED)	// 按下按钮时的处理
	if(m_bCheck)
	{
		// 重绘整个控件
		CBrush brush(m_crDownColor);
		dc.FillRect(&(lpDrawItemStruct->rcItem),&brush);
		DrawText(lpDrawItemStruct->hDC,chBuffer,nSize,&lpDrawItemStruct->rcItem,
			DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_TABSTOP);
		SetBkMode(lpDrawItemStruct->hDC,TRANSPARENT);
	}
	else
	{
		CBrush brush(m_crUpColor);
		dc.FillRect(&(lpDrawItemStruct->rcItem),&brush);
		DrawText(lpDrawItemStruct->hDC,chBuffer,nSize,&lpDrawItemStruct->rcItem,
			DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_TABSTOP);
		SetBkMode(lpDrawItemStruct->hDC,TRANSPARENT);
	}
	if((lpDrawItemStruct->itemState&ODS_SELECTED)&&
		(lpDrawItemStruct->itemAction&(ODA_SELECT|ODA_DRAWENTIRE)))
	{
		COLORREF fc = RGB(255-GetRValue(m_crUpColor),255-GetGValue(m_crUpColor),255-GetBValue(m_crUpColor));
		CBrush brush(fc);
		dc.FrameRect(&(lpDrawItemStruct->rcItem),&brush);
	}
	if(!(lpDrawItemStruct->itemState&ODS_SELECTED)&&(lpDrawItemStruct->itemAction&ODA_SELECT))
	{
		CBrush brush(m_crUpColor);
		dc.FrameRect(&lpDrawItemStruct->rcItem,&brush);
	}
	dc.Detach();
}

//void CMyButton::OnClicked() 
//{
//	m_bCheck = !m_bCheck;
//	Invalidate();
//}

void CMyButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bCheck = !m_bCheck;
	Invalidate();
	CButton::OnLButtonDown(nFlags,point);
}