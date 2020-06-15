#pragma once
#include "afxwin.h"


// CDlgPadDetect dialog

class CDlgPadDetect : public CDialog
{
	DECLARE_DYNAMIC(CDlgPadDetect)

public:
	CDlgPadDetect(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPadDetect();

// Dialog Data
	enum { IDD = IDD_DLG_PAD_DETECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CColorButton m_colorButton[6];
	CRect m_rectROI;
	CRect m_rectModelWin;
	CImgMarkGeoSet m_stPadPicture;
	CSliderCtrl m_sliderShutter;
	CSliderCtrl m_sliderGain;
	int m_nImageWidth;
	int m_nImageHeight;
	//Blob ²ÎÊý
	double dHeightMin;
	double dHeightMax;
	double dWidthMin;
	double dWidthMax;
	double dAreaMin;
	double dAreaMax;
	virtual BOOL OnInitDialog();
	void UpdateUI();
	void InitUI();
	void SaveParam();
	bool FindPad();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnChangeEditPadDetectExposureTime();
	afx_msg void OnEnChangeEditPadDetectGain();
//	afx_msg void OnBnClickedBtnPadDetectNewpic();
	afx_msg void OnBnClickedBtnPadDetectSaveLearnWin();
	afx_msg void OnBnClickedBtnPadDetectSaveRoi();
	afx_msg void OnBnClickedBtnPadDetectSaveModel();
	afx_msg void OnBnClickedBtnPadDetectTest();
	bool Detect(bool bSplice);
	bool SpliceDetect(bool *pResult,int nStartColumn,int &nEndColumn);
	afx_msg void OnBnClickedBtnPadDetectSearch();
	afx_msg void OnBnClickedBtnPadDetectScale();
	afx_msg void OnBnClickedOk();
	void Close();
	afx_msg void OnBnClickedCancel();
//	afx_msg void OnBnClickedBtnPadDetectGrabpos();
	afx_msg void OnBnClickedBtnPadDetectLight();
//	afx_msg void OnBnClickedBtnPadDetectRotate();
	//afx_msg void OnBnClickedBtnFirstpixselSave();
	void InitImagePaint();
	void DTest();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//afx_msg void OnBnClickedBtnFindblob();
	bool FindBlobCenter(CRect ROIF);
	void OverlayLine(int nColor,CPoint p1, CPoint p2);
	CComboBox m_cbPadSelect;
	int nSelectID;
	afx_msg void OnCbnSelchangeComboImgselect();
};
