
#include "JogButton.h"
#include "DlgCameraLaser.h"
#pragma once
// CDlgHeight dialog

class CDlgHeight : public CDialog
{
	DECLARE_DYNAMIC(CDlgHeight)

public:
	CDlgHeight(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgHeight();

	// Dialog Data
	enum { IDD = IDD_DLG_HEIGHT };

	CColorButton m_colorButton[10];
	int m_nSpeed;
	HBITMAP m_hBitmap;
	bool m_bMotion;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnNMCustomdrawSliderHeightSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnHeightSavePos();
	afx_msg void OnBnClickedBtnHeightZsafePos();
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void UpdateUI();
	void Close();
	void AdjustNeedleGap();
	afx_msg void OnBnClickedBtnHeightMeasureTwo();
	afx_msg void OnBnClickedBtnHeightMeasureOne();
//	afx_msg void OnBnClickedBtnHeightCalibration();
	afx_msg void OnBnClickedBtnHeightCameraPos();
	afx_msg void OnBnClickedBtnHeightMeasureThree();
	void ModifyParam();
	void RefreshPosition();
	afx_msg void OnBnClickedBtnHeightPos();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnNMCustomdrawSliderHeightAdjustSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnHeightLasersave();
	afx_msg void OnBnClickedBtnLaserposModify();
	afx_msg void OnBnClickedBtnLaserLable();
};
