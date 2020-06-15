#pragma once


// CDlgDetectCalib dialog

class CDlgDetectCalib : public CDialog
{
	DECLARE_DYNAMIC(CDlgDetectCalib)

public:
	CDlgDetectCalib(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDetectCalib();

// Dialog Data
	enum { IDD = IDD_DLG_DETECT_CALIB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_nIndex;
	tgPos tgPixelPos[3];

	CImgMarkGeoSet m_picture;
	CRect m_rectROI;
	CRect m_rectModelWin;
	long m_nImageWidth,m_nImageHeight;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnDetectCalibNext();
	afx_msg void OnBnClickedOk();
	void ComputeDetectCalibration();
	void ComputeScale();
};
