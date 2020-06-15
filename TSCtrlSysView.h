
// TSCtrlSysView.h : CTSCtrlSysView ��Ľӿ�
//
#include "ImgStatic.h"

#pragma once
class CTSCtrlSysView : public CView
{
protected: // �������л�����
	CTSCtrlSysView();
	DECLARE_DYNCREATE(CTSCtrlSysView)

// ����
public:
	CTSCtrlSysDoc* GetDocument() const;
	CImgStatic m_ImgStatic;

// ����
public:
// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);


// ʵ��
public:
	virtual ~CTSCtrlSysView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
};

#ifndef _DEBUG  // TSCtrlSysView.cpp �еĵ��԰汾
inline CTSCtrlSysDoc* CTSCtrlSysView::GetDocument() const
   { return reinterpret_cast<CTSCtrlSysDoc*>(m_pDocument); }
#endif

