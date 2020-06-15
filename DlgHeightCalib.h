#pragma once


// CDlgHeightCalib dialog

class CDlgHeightCalib : public CDialog
{
	DECLARE_DYNAMIC(CDlgHeightCalib)

public:
	CDlgHeightCalib(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgHeightCalib();

// Dialog Data
	enum { IDD = IDD_DLG_HEIGHT_CALIB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CColorButton m_cbMotion[6];
	double dContactValue;
	double dHeightValue;
	tgPos m_tgContactPos;
	tgPos m_tgHeightPos;
	HBITMAP m_hBitmap;
	virtual BOOL OnInitDialog();
	afx_msg void OnNMCustomdrawSliderHeightCalibSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnHeightCalibNext();
	afx_msg void OnBnClickedOk();
	void UpdateContactSensorValue();
	void UpdateHeightSensorValue();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void MoveToContactSensorPos();
	void MoveToHeightSensorPos();
	void SaveData();
	afx_msg void OnBnClickedCancel();
};
