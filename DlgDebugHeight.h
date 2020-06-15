#pragma once


// CDlgDebugHeight dialog

class CDlgDebugHeight : public CDialog
{
	DECLARE_DYNAMIC(CDlgDebugHeight)

public:
	CDlgDebugHeight(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDebugHeight();

// Dialog Data
	enum { IDD = IDD_DLG_DEBUG_HEIGHT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	HBITMAP m_hBitmap;
	CColorButton m_colorButton[6];
	int m_nSpeed;
	int nStep;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnDebugHeightNext();
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void InitUI();
	void MoveToContactSensorPos(int nIndex);
	void SavePos();
	void Exit();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnNMCustomdrawSliderDebugHeightSpeed(NMHDR *pNMHDR, LRESULT *pResult);
};
