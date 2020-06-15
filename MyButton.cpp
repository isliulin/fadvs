// MyButton.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "MyButton.h"


// CMyButton

IMPLEMENT_DYNAMIC(CMyButton, CButton)

CMyButton::CMyButton()
{
	// ��ʼ��Ϊ��ɫ
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
	dc.Attach(lpDrawItemStruct->hDC);	// ��û��Ƶ��豸����DC
	VERIFY(lpDrawItemStruct->CtlType==ODT_BUTTON);
	// ���Button�ϵ�����
	// ���������»��Ƶ���ť��
	// �����ֵı�������Ϊ͸������ť�Ͻ���ʾ����
	const int nBufSize = 512;
	TCHAR chBuffer[nBufSize];
	GetWindowText(chBuffer,nBufSize);
	int nSize = strlen(chBuffer);
	DrawText(lpDrawItemStruct->hDC,chBuffer,nSize,&lpDrawItemStruct->rcItem,
		DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_TABSTOP);	// ��������
	SetBkMode(lpDrawItemStruct->hDC,TRANSPARENT);	// ����Ϊ͸��
	//if(lpDrawItemStruct->itemState&ODS_SELECTED)	// ���°�ťʱ�Ĵ���
	if(m_bCheck)
	{
		// �ػ������ؼ�
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