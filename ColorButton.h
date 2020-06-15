#pragma once

#include "GTS400Ex.h"
// CColorButton

class CColorButton : public CButton
{
	DECLARE_DYNAMIC(CColorButton)

public:
	CColorButton();
	virtual ~CColorButton();

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	void SetBitmapEx(UINT ID);
	int m_iIndex;

	static int m_iSpeedNo;
public:
	CBitmap m_Bitmap;
	CFont	m_Font;

	//��ť��״̬
	BOOL m_bOver;		//���λ�ڰ�ť֮��ʱ��ֵΪtrue����֮Ϊflase
	BOOL m_bTracking;	//����갴��û���ͷ�ʱ��ֵΪtrue
	BOOL m_bSelected;	//��ť�������Ǹ�ֵΪtrue
	BOOL m_bFocus;		//��ťΪ��ǰ��������ʱ��ֵΪtrue
	BOOL m_bDisable;

	GTS400Ex *m_pMv;
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);

	virtual void PreSubclassWindow();
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	BOOL m_bButtonDown;
};


