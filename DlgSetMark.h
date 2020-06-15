#pragma once
#include "afxwin.h"
#include "ImgMarkGeoSet.h"


// DlgMarkSet dialog

class CDlgSetMark : public CDialog
{
	DECLARE_DYNAMIC(CDlgSetMark)

public:
	CDlgSetMark(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSetMark();

	CRect ROI;
	CRect LearnWin;

	double dCenterX;
	double dCenterY;
	double dAngle;
	double dScore;
	int nHeight;
	int nWidth;
	int nSelectID;
	CImgMarkGeoSet m_picture;
	CComboBox m_cbSelect;
	CSliderCtrl m_sliderShutter;
	CSliderCtrl m_sliderGain;

	// Dialog Data
	enum { IDD = IDD_DLG_SET_MARK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnSetMarkScale();
	afx_msg void OnBnClickedBtnSetMarkGrabpos();
	afx_msg void OnBnClickedBtnSetMarkNewpic();
	afx_msg void OnBnClickedBtnSetMarkSaveLearnWin();
	afx_msg void OnBnClickedBtnSetMarkShowLearnWin();
	afx_msg void OnBnClickedBtnSetMarkSaveRoi();
	afx_msg void OnBnClickedBtnSetMarkSaveModel();
	afx_msg void OnBnClickedBtnSetMarkSearch();
	afx_msg void OnBnClickedBtnSetMarkMoveCenter();
	afx_msg void OnBnClickedBtnSetMarkClear();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	void InitUI();
	void UpdateUI();
	void RefreshPosition();
	bool FindModelCenter(CRect rect);
	void SaveParam();
	afx_msg void OnCbnSelchangeComboSetMarkSelect();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKillfocusEditSetMarkExposureTime();
	afx_msg void OnKillfocusEditSetMarkGain();
	afx_msg void OnBnClickedBtnSetMarkTest();
	afx_msg void OnBnClickedBtnSetMarkSaveCenter();
};