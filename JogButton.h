#pragma once

//#include "Controller.h"
// CJogButton

class CJogButton : public CButton
{
	DECLARE_DYNAMIC(CJogButton)

public:
	CJogButton();
	virtual ~CJogButton();

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	void SetBitmapEx(UINT ID);
	int m_iIndex;

	static int m_dJogSpdRatio;
public:
	CBitmap m_Bitmap;
	CFont	m_Font;

	//��ť��״̬
	BOOL m_bOver;		//���λ�ڰ�ť֮��ʱ��ֵΪtrue����֮Ϊflase
	BOOL m_bTracking;	//����갴��û���ͷ�ʱ��ֵΪtrue
	BOOL m_bSelected;	//��ť�������Ǹ�ֵΪtrue
	BOOL m_bFocus;		//��ťΪ��ǰ��������ʱ��ֵΪtrue
	BOOL m_bDisable;

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
};
