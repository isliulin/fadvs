#pragma once
#include "resource.h"
#include "ColorButton.h"

// CDlgLabel dialog

class CDlgLabel : public CDialog
{
	DECLARE_DYNAMIC(CDlgLabel)

public:
	CDlgLabel(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLabel();

// Dialog Data
	enum { IDD = IDD_DLG_LABEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_nIndex;
	tgPos LastLablePos[6];
	BITMAPINFO *m_pBitmapInfo;
	CDC *m_pDC;
	CColorButton m_colorButton[10];
	BOOL OnInitDialog();
	afx_msg void OnNMCustomdrawSliderLabelSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void BackUpHistoryData();
	void Close();
	void UpdateUI();
	void UpdatePrompt();
	void Paint();
	void PainScale(CDC *pDC,CPoint point,int nWidth,int nHeight,int nLength);
	void DrawCross(CDC *pDC,CPoint point,int nWidth,int nHeight);
	afx_msg void OnBnClickedBtnLabelNext();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	void MoveAdjustMotorToZero();
	afx_msg void OnBnClickedBtnZtofocus();
	afx_msg void OnBnClickedBtnNeedleOne();
	afx_msg void OnBnClickedBtnNeedleTwo();
	afx_msg void OnBnClickedBtnNeedleThird();
	afx_msg void OnBnClickedBtnCemera();
	afx_msg void OnBnClickedBtnLaser();
	afx_msg void OnBnClickedBtnLaserDataread();
};
