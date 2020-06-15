
#pragma once

/////////////////////////////////////////////////////////////////////////////
// COutputList ����
//COutPutList is based in CListBox,So have propertity like it;

class COutputList : public CListBox
{
// ����
public:
	COutputList();
// ʵ��
public:
	virtual ~COutputList();
protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnViewOutput();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

//COutputWnd is based in CDockablePane ,So have the propertity of it;
class COutputWnd : public CDockablePane
{
// ����
public:
	COutputWnd();

// ����
protected:
	CFont m_Font;
	CMFCTabCtrl	m_wndTabs;
public:
	COutputList m_wndOutputBuild;
// 	COutputList m_wndOutputDebug;
// 	COutputList m_wndOutputFind;

protected:
	void FillBuildWindow();
	void FillDebugWindow();
	void FillFindWindow();
	void AdjustHorzScroll(CListBox& wndListBox);

// ʵ��
public:
	virtual ~COutputWnd();
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
};

