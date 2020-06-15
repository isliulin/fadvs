#pragma once


// CDlgAlignNeedle dialog

class CDlgAutoAlign : public CDialog
{
	DECLARE_DYNAMIC(CDlgAutoAlign)

public:
	CDlgAutoAlign(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAutoAlign();

	// Dialog Data
	enum { IDD = IDD_DLG_AUTO_ALIGN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CColorButton m_colorButton[6];
	int m_nSpeed;
	BITMAPINFO *m_pBitmapInfo;
	double m_dTempPos[3];
	double m_dStartPos[3];
	int nStep;
	CDC *m_pDC;

	virtual BOOL OnInitDialog();
	void Paint();
	afx_msg void OnTimer(UINT nIDEvent);
	void PainScale(CDC *pDC,CPoint point,int nWidth, int nHeight,int nLength);
	void DrawCross(CDC *pDC,CPoint point,int nWidth, int nHeight);
	void OnClose();
	afx_msg void OnNMCustomdrawSliderAutoAlignSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnAutoAlignNext();
	bool MoveToFirstDispensePos();
	bool MoveToImageCenter();
};
