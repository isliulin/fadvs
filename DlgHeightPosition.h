#pragma once


// CDlgHeightPosition dialog

class CDlgHeightPosition : public CDialog
{
	DECLARE_DYNAMIC(CDlgHeightPosition)

public:
	CColorButton m_colorButton[6];
	int m_nIndex;
	int m_nCount;
	BITMAPINFO *m_pBitmapInfo;
	CDC *m_pDC;
	CDlgHeightPosition(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgHeightPosition();

// Dialog Data
	enum { IDD = IDD_DLG_HEIGHT_POSITION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnHeightPosNext();
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMCustomdrawSliderHeightPosSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	void PainScale(CDC *pDC,CPoint point,int nWidth, int nHeight,int nLength);
	void DrawCross(CDC *pDC,CPoint point,int nWidth, int nHeight);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void Close();
	void UpdateUI();
	void UpdateData();
	void Paint();
	afx_msg void OnBnClickedCancel();
};
