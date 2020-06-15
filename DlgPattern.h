#pragma once


// CDlgPattern dialog

class CDlgPattern : public CDialog
{
	DECLARE_DYNAMIC(CDlgPattern)

public:
	CRect ROI;
	CRect LearnWin;

	double dCenterX;
	double dCenterY;
	double dAngle;
	double dScore;
	int nWidth;
	int nHeight;
	int nFirstRow;
	int nFirstColumn;
	int nSecondRow;
	int nSecondColumn;
	int nThirdRow;
	int nThirdColumn;
	CStatic m_picture;
	CDlgPattern(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPattern();

// Dialog Data
	enum { IDD = IDD_DLG_PATTERN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void RefreshCurrentPosition();
	void RefreshGrabImagePosition();
	bool FindPatternCenter(CRect ROI);
	void SaveParam();
	afx_msg void OnBnClickedBtnPatternScale();
	afx_msg void OnBnClickedBtnPatternNewpic();
	afx_msg void OnBnClickedBtnPatternFirstImagePos();
	afx_msg void OnBnClickedBtnPatternNextImagePos();
	afx_msg void OnBnClickedBtnPatternSearch();
	afx_msg void OnBnClickedBtnPatternMoveCenter();
	afx_msg void OnBnClickedBtnPatternSaveModel();
	afx_msg void OnBnClickedBtnPatternSaveLearnWin();
	afx_msg void OnBnClickedBtnPatternShowLearnWin();
	afx_msg void OnBnClickedBtnPatternSaveRoi();
	afx_msg void OnBnClickedBtnPatternShowRoi();
	afx_msg void OnBnClickedBtnPatternClear();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
