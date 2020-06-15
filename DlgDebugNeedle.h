#pragma once


// CDlgDebugNeedle dialog

class CDlgDebugNeedle : public CDialog
{
	DECLARE_DYNAMIC(CDlgDebugNeedle)

public:
	CDlgDebugNeedle(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDebugNeedle();

// Dialog Data
	enum { IDD = IDD_DLG_DEBUG_NEEDLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CColorButton m_colorButton[6];
	int m_nSpeed;
	CDHCamera *m_pCamera;
	BITMAPINFO *m_pBitmapInfo;
	int nStep;
	CDC *m_pDC;
	virtual BOOL OnInitDialog();
	void MoveToNeedlePos(int nIndex);
	void ImgInit();
	void Paint();
	void PainScale(CDC *pDC,CPoint point,int nWidth,int nHeight,int nLength);
	void DrawCross(CDC *pDC,CPoint point,int nWidth,int nHeight);
	void OnClose();
	afx_msg void OnBnClickedBtnDebugNext();
	afx_msg void OnNMCustomdrawSliderDebugNeedleSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCancel();
	void SavePosition();
};
