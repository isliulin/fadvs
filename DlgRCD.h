#pragma once


// CDlgRCD dialog

class CDlgRCD : public CDialog
{
	DECLARE_DYNAMIC(CDlgRCD)

public:
	CDlgRCD(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgRCD();

	// Dialog Data
	enum { IDD = IDD_DLG_RCD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int nLevelID;
	int nSelectRow;
	CColorButton m_cbMotion[6];
	double dStartPos[2];
	double dEndPos[2];
	BITMAPINFO *m_pBitmapInfo;
	CDC *m_pDC;
	void SetParam(int nLevel,int selectRow) { nLevelID = nLevel; nSelectRow = selectRow;};
	BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnRcdNext();
	afx_msg void OnNMCustomdrawRcdSliderSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	void OnClose();
	afx_msg void OnBnClickedCancel();
	void Paint();
	void PaintScale(CDC *pDC,CPoint point,int nWidth, int nHeight,int nLength);
	void DrawCross(CDC *pDC,CPoint point,int nWidth, int nHeight);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
