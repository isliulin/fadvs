#pragma once

// CDlgAutoNeedle dialog

class CDlgAutoNeedle : public CDialog
{
	DECLARE_DYNAMIC(CDlgAutoNeedle)

public:
	CDlgAutoNeedle(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAutoNeedle();

	// Dialog Data
	enum { IDD = IDD_DLG_AUTO_NEEDLE };

	CColorButton m_colorButton[10];
	int m_nSpeed;
	CImgMarkGeoSet m_picture;
	CButton m_AutoNeedlePitch;
	double m_dScore;
	double m_dAngle;
	tgPos m_tgCenter;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedBtnAutoNeedleSavePos();
	afx_msg void OnNMCustomdrawSliderAutoNeedleSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnAutoNeedleMovePos();
	void UpdateUI();
	void UpdateResult();
	void CurrentPos();
	void EncoderPos();
	void SaveData();
	void Close();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedRadioAutoNeedleGray();
	afx_msg void OnBnClickedRadioAutoNeedleBlob();
	afx_msg void OnBnClickedCancel();
	bool FindBlobCenter(double &dCenterX,double &dCenterY);
	bool FindNeedleModel(double &dCenterX,double &dCenterY,double &dScore);
	afx_msg void OnBnClickedBtnAutoNeedleAdjustNeedleOne();
	afx_msg void OnBnClickedBtnAutoNeedleAdjustNeedleTwo();
	afx_msg void OnBnClickedBtnAutoNeedleAdjustNeedleThree();
	afx_msg void OnBnClickedBtnAutoNeedleAdjustNeedle();
	afx_msg void OnBnClickedBtnAutoNeedleNeedleOne();
	afx_msg void OnBnClickedBtnAutoNeedleNeedleTwo();
	afx_msg void OnBnClickedBtnAutoNeedleNeedleThree();
	afx_msg void OnBnClickedBtnAutoNeedleCalib();
	afx_msg void OnBnClickedBtnAutoNeedleSaveParame();
	afx_msg void OnBnClickedBtnAutoNeedleReset();
	afx_msg void OnBnClickedBtnAutoNeedleSaveRoi();
	afx_msg void OnBnClickedBtnAutoNeedleSaveModelWin();
	afx_msg void OnBnClickedBtnAutoNeedleSaveModel();
	afx_msg void OnBnClickedBtnAutoNeedleSearch();
};
