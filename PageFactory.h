#pragma once
#include "afxwin.h"


// CPageFactory dialog

class CPageFactory : public CPropertyPage
{
	DECLARE_DYNAMIC(CPageFactory)

public:
	CPageFactory();
	virtual ~CPageFactory();

// Dialog Data
	enum { IDD = IDD_PAGE_FACTORY_PARAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cbVelType;
	int m_nVelType;
	CComboBox m_cbMotor;
	int m_nMotor;
	bool m_bMaxPosModify;
	bool m_bMinPosModify;
	bool m_bMaxVelModify;
	bool m_bScaleModify;
	bool m_bMaxHomeVelModify;
	bool m_bMinHomeVelModify;
	bool m_bHomeAccModify;
	bool m_bVelModify;
	bool m_bAccModify;
	bool m_bJerkModify;
	bool m_bDecModify;
	bool m_bStartVelModify;
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	afx_msg void OnCbnSelchangeComboFactoryParamMotor();
	afx_msg void OnCbnSelchangeComboFactoryParamVelType();
	void UpdateUI();
	void SaveParam();
	void InitParam();
	afx_msg void OnEnChangeEditFactoryParamMaxPos();
	afx_msg void OnEnChangeEditFactoryParamMinPos();
	afx_msg void OnEnChangeEditFactoryParamMaxVel();
	afx_msg void OnEnChangeEditFactoryParamScale();
	afx_msg void OnEnChangeEditFactoryParamMaxHomeVel();
	afx_msg void OnEnChangeEditFactoryParamMinHomeVel();
	afx_msg void OnEnChangeEditFactoryParamHomeAcc();
	afx_msg void OnEnChangeEditFactoryParamVel();
	afx_msg void OnEnChangeEditFactoryParamAcc();
	afx_msg void OnEnChangeEditFactoryParamJerk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEnChangeEditFactoryParamDec();
	afx_msg void OnEnChangeEditFactoryParamStartvel();
	CComboBox m_cbIsLog;
};
