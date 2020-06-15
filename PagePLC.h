#pragma once
#include "MyButton.h"
#define  MM_FACTOR 0.01

// CPagePLC dialog

class CPagePLC : public CPropertyPage
{
	DECLARE_DYNAMIC(CPagePLC)

public:
	CPagePLC();
	virtual ~CPagePLC();

// Dialog Data
	enum { IDD = IDD_PAGE_PLC };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	CMyButton m_btnLoaderJogP,m_btnLoaderJogN,m_btnUnloaderJogP,m_btnUnloaderJogN;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedPLC(UINT nID);
	afx_msg void OnCancel();
	afx_msg void OnSave();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnPlcManualLoad();
	afx_msg void OnBnClickedBtnPlcManualUnload();
	bool UpdateUI();
	bool UpdateBoxCheckStatus();
	bool UpdateTempCheckSwitchStatus();
	afx_msg void OnBnClickedBtnPlcPreheat();
	afx_msg void OnBnClickedBtnPlcHeat();
	bool UpdateHeatState();
	bool UpdateInputState();
	void SetAuthority();
	afx_msg void OnBnClickedBtnPlc0();
	afx_msg void OnBnClickedCheckPlcInboxDetect();
	afx_msg void OnBnClickedCheckPlcOutboxDetect();
	afx_msg void OnBnClickedCheckPlcTempDetect();
	afx_msg void OnBnClickedBtnLineback();
};
