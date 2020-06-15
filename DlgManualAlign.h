#pragma once


// CDlgManualAlign dialog

class CDlgManualAlign : public CDialog
{
	DECLARE_DYNAMIC(CDlgManualAlign)

public:
	CDlgManualAlign(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgManualAlign();

// Dialog Data
	enum { IDD = IDD_DLG_MANUAL_ALIGN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CColorButton m_colorButton[4];
	BITMAPINFO *m_pBitmapInfo;
	CRect m_rectModel;
	CDC *m_pDC;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMCustomdrawSliderManualAlignSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void Paint();
	void RefreshPosition();
	void OnClose();
	void PainScale(CDC *pDC,CPoint point,int nWidth, int nHeight,int nLength);
	void DrawCross(CDC *pDC,CPoint point,int nWidth, int nHeight);
	void PainROI(CDC *pDC);
	afx_msg void OnBnClickedCancel();
};
