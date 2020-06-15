
// TSCtrlSysView.h : CTSCtrlSysView 类的接口
//
#include "ImgStatic.h"

#pragma once
class CTSCtrlSysView : public CView
{
protected: // 仅从序列化创建
	CTSCtrlSysView();
	DECLARE_DYNCREATE(CTSCtrlSysView)

// 属性
public:
	CTSCtrlSysDoc* GetDocument() const;
	CImgStatic m_ImgStatic;

// 操作
public:
// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);


// 实现
public:
	virtual ~CTSCtrlSysView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
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

#ifndef _DEBUG  // TSCtrlSysView.cpp 中的调试版本
inline CTSCtrlSysDoc* CTSCtrlSysView::GetDocument() const
   { return reinterpret_cast<CTSCtrlSysDoc*>(m_pDocument); }
#endif

