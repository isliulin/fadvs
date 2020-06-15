#include "DlgCameraAdj.h"
#include "DlgNeedleOneAdjust.h"
#include "DlgNeedleTwoAdjust.h"
#include "DlgNeedleThreeAdjust.h"
#pragma once


// CDlgNeedleAdjust dialog

class CDlgNeedleAdjust : public CDialog
{
	DECLARE_DYNAMIC(CDlgNeedleAdjust)

public:
	CDlgNeedleAdjust(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNeedleAdjust();

// Dialog Data
	enum { IDD = IDD_DLG_NEEDLEADJUST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	void EnableButton(int nIndex);
	void EnableWindow(int nIndex);
public:
	CDlgCameraAdj m_DlgCamera;
	CDlgNeedleOneAdjust m_DlgNeedleOne;
	CDlgNeedleTwoAdjust m_DlgNeedleTwo;
	CDlgNeedleThreeAdjust m_DlgNeedleThree;
	LRESULT OnMessageCamAdjust(WPARAM wparam, LPARAM lparam);
	LRESULT OnMessageNeedleOneAdjust(WPARAM wparam, LPARAM lparam);
	LRESULT OnMessageNeedleTwoAdjust(WPARAM wparam, LPARAM lparam);
	LRESULT OnMessageNeedleThreeAdjust(WPARAM wparam, LPARAM lparam);
	afx_msg void OnBnClickedBtnNeedleone();
	afx_msg void OnBnClickedBtnNeedletwo();
	afx_msg void OnBnClickedBtnNeedlethree();
	afx_msg void OnBnClickedBtnStart();
	CRect rect;
	afx_msg void OnBnClickedOver();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void ButtonClever();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	int nStep;
};
