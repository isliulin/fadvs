#pragma once
#include "XPButton.h"


// CDlgRepair dialog

class CDlgRepair : public CDialog
{
	DECLARE_DYNAMIC(CDlgRepair)

public:
	CDlgRepair(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgRepair();

	// Dialog Data
	enum { IDD = IDD_DLG_REPAIR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	bool m_bIsFirstDraw;
	bool m_bDeviceAdjust;
	CRect m_rcPreRect;
	bool m_bDrag;
	CPoint m_ptStart;
	CXPButton *m_pBtn;
	int nButtonID;
	int nButtonNum;
	int m_nSelIndex;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnSelectDisp();
	afx_msg void OnBnClickedBtnAllDisp();
	afx_msg void OnBnClickedBtnSelectRegion();
	afx_msg void OnCbnSelchangeComboRepaireChannel();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	CRect GetARect(CPoint ptStart,CPoint ptEnd);
	bool IsInRect(UINT nID,CRect rc);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedOk();
	void SetAuthority(bool bEnable);
	afx_msg void OnBnClickedBtnMoveTo();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnSafetyPos();
	afx_msg void OnBnClickedBtnTargetpos();
	void RuleDeviceAdjust(bool bAdjust);
};
