#pragma once


// CDlgImagePosition dialog

class CDlgImagePosition : public CDialog
{
	DECLARE_DYNAMIC(CDlgImagePosition)

public:
	CDlgImagePosition(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgImagePosition();

// Dialog Data
	enum { IDD = IDD_DLG_IMAGE_POSITION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CColorButton m_colorButton[6];
	int m_nSpeed;
	BITMAPINFO *m_pBitmapInfo;
	CDC *m_pDC;
	int nCount;
	int nIndex;
	virtual BOOL OnInitDialog();
	void Paint();
	afx_msg void OnTimer(UINT nIDEvent);
	void PainScale(CDC *pDC,CPoint point,int nWidth, int nHeight,int nLength);
	void DrawCross(CDC *pDC,CPoint point,int nWidth, int nHeight);
	void OnClose();
	afx_msg void OnBnClickedBtnImageNeedleNext();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnNMCustomdrawSliderImageSpeed(NMHDR *pNMHDR, LRESULT *pResult);
};
