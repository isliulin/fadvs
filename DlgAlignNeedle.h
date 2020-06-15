#pragma once


// CDlgAlignNeedle dialog

class CDlgAlignNeedle : public CDialog
{
	DECLARE_DYNAMIC(CDlgAlignNeedle)

public:
	CDlgAlignNeedle(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAlignNeedle();

	// Dialog Data
	enum { IDD = IDD_DLG_ALIGN_NEEDLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CColorButton m_colorButton[6];
	int m_nSpeed;
	BITMAPINFO *m_pBitmapInfo;
	double m_dTempPos[2];
	double m_dImagePos[3];
	double m_dHeightPos[3];//测高有有效工作距离
	CDC *m_pDC;
	double m_dHeightValue;

	virtual BOOL OnInitDialog();
	void Paint();
	afx_msg void OnTimer(UINT nIDEvent);
	void PainScale(CDC *pDC,CPoint point,int nWidth, int nHeight,int nLength);
	void DrawCross(CDC *pDC,CPoint point,int nWidth, int nHeight);
	void OnClose();
	afx_msg void OnNMCustomdrawSliderAlignSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnAlignNeedleNext();
	bool MoveToImagePosition();
};
