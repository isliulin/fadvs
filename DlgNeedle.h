#pragma once
#include "KShvdh.h"
#include "JogButton.h"
// CDlgNeedle dialog

class CDlgNeedle : public CDialog
{
	DECLARE_DYNAMIC(CDlgNeedle)

public:
	CDlgNeedle(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNeedle();

	// Dialog Data
	enum { IDD = IDD_DLG_NEEDLE };

	CColorButton m_colorButton[10];
	CJogButton m_adjustButton[8];
	int m_nSpeed;
	//CDHCamera *m_pCamera;
	BITMAPINFO *m_pBitmapInfo;
	CDC *m_pDC;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void UpdateUI();
	void ImgInit();
	void PainScale(CDC *pDC,CPoint point,int nWidth,int nHeight,int nLength);
	void DrawCross(CDC *pDC,CPoint point,int nWidth,int nHeight);
	void Close();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnNMCustomdrawSliderNeedleSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	void Paint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedBtnNeedleGap();
	void CurrentPos();
	void EncoderPos();
	void SetAuthority();
	afx_msg void OnBnClickedBtnNeedleAdjustMotorReset();
	afx_msg void OnNMCustomdrawSliderNeedleAdjustSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnNeedleAdjustMotorZero();
	afx_msg void OnBnClickedBtnNeedleLabel();
	afx_msg void OnBnClickedBtnNeedleAdjustNeedle();
	afx_msg void OnBnClickedBtnNeedleAdjustClean();
	afx_msg void OnBnClickedBtnNeedleCameraLabelPos();
	afx_msg void OnBnClickedBtnNeedleHeightLabelPos();
	afx_msg void OnBnClickedBtnNeedleOneLabelPos();
	afx_msg void OnBnClickedBtnNeedleTwoLabelPos();
	afx_msg void OnBnClickedBtnNeedleThreeLabelPos();
	afx_msg void OnBnClickedBtnNeedleHeightPos();
	afx_msg void OnBnClickedBtnSaveall();
};
