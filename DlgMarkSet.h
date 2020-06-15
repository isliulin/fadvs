#pragma once
#include "afxwin.h"
#include "ImgMarkGeoSet.h"


// DlgMarkSet dialog

class CDlgMarkSet : public CDialog
{
	DECLARE_DYNAMIC(CDlgMarkSet)

public:
	CDlgMarkSet(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMarkSet();

	CRect ROI;
	CRect LearnWin;

	double dCenterX;
	double dCenterY;
	double dAngle;
	double dScore;
	int nHeight;
	int nWidth;

	CImgMarkGeoSet m_picture;
	CComboBox m_cbSelect;
	int nSelectID;

	CSliderCtrl m_sliderShutter;
	CSliderCtrl m_sliderGain;

// Dialog Data
	enum { IDD = IDD_DLG_MARKSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnMarkScale();
	afx_msg void OnBnClickedBtnMarkGrabpos();
	afx_msg void OnBnClickedBtnMarkNewpic();
	afx_msg void OnBnClickedBtnMarkSaveLearnWin();
	afx_msg void OnBnClickedBtnMarkShowLearnWin();
	afx_msg void OnBnClickedBtnMarkSaveRoi();
	afx_msg void OnBnClickedBtnMarkSaveModel();
	afx_msg void OnBnClickedBtnMarkSearch();
	afx_msg void OnBnClickedBtnMarkMoveCenter();
	afx_msg void OnBnClickedBtnMarkClear();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	void RefreshPosition();
	bool FindModelCenter(CRect rect);
	void SaveParam();
	afx_msg void OnBnClickedBtnMarkSaveCenter();
	afx_msg void OnCbnSelchangeComboMarkSelect();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKillfocusEditMarkExposureTime();
	afx_msg void OnKillfocusEditMarkGain();

	void InitUI();
	void UpdateUI();
	afx_msg void OnBnClickedBtnMarkTest();
};