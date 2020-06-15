#pragma once
#include "afxcmn.h"


// CDlgConfigure dialog

class CDlgConfigure : public CDialog
{
	DECLARE_DYNAMIC(CDlgConfigure)

public:
	CDlgConfigure(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgConfigure();

// Dialog Data
	enum { IDD = IDD_DLG_CONFIGURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cbHeightSel;
	int m_nHeightIndex;
	CComboBox m_cbValveNum;
	int m_nValveNum;
	bool *m_bSaveConfigure;
	CComboBox m_cbPlcSel;
	CComboBox m_cbValveSel;
	int m_nPlcSel;
	int m_nValveSel;
	CComboBox m_cbHeightSensor;
	int m_nHeightSensor;
	CComboBox m_cbDetectCamera;
	int m_nDetectCamera;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboConfigureHeightSensor();
	afx_msg void OnBnClickedOk();
	void SetParam(bool* bSaveConfigure) { m_bSaveConfigure = bSaveConfigure; }
	afx_msg void OnCbnSelchangeComboConfigureValveNum();
	afx_msg void OnCbnSelchangeComboConfigurePlcType();
	afx_msg void OnCbnSelchangeComboConfigureValveType();
	afx_msg void OnCbnSelchangeComboConfigureHeightSensorType();
	afx_msg void OnCbnSelchangeComboConfigureDetectCameraType();
	CSpinButtonCtrl m_spinMarkCam;
	CSpinButtonCtrl m_spinNeedleCam;
};
