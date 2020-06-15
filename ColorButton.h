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

	//按钮的状态
	BOOL m_bOver;		//鼠标位于按钮之上时该值为true，反之为flase
	BOOL m_bTracking;	//在鼠标按下没有释放时该值为true
	BOOL m_bSelected;	//按钮被按下是该值为true
	BOOL m_bFocus;		//按钮为当前焦点所在时该值为true
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


