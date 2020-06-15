#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CDlgProudSet dialog

class CDlgProudSet : public CDialog
{
	DECLARE_DYNAMIC(CDlgProudSet)

public:
	CDlgProudSet(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgProudSet();

// Dialog Data
	enum { IDD = IDD_DLG_PROUD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

	virtual BOOL OnInitDialog();
	void updataparam(void);
	void SetAuthority();

	bool CarmUseChange;
private:
	CButton m_DisFopen;
	CButton m_CleanNeedle;
	CButton m_FirstTest;
	CButton m_SlotDetect;
	CButton m_FullDischarge;
	CButton m_IrregularMatrix;
	//CButton m_SingleValve;
	CButton m_FullClean;
	CButton m_DoorSwitch;
	CButton m_DispFinishFill;
	CButton m_CleanDischarge;
	CButton m_PauseCleanTest;
	CButton m_MeasureHeight;
	CButton m_AutoAdjust;
	CComboBox m_cbCurrentRow;
	CComboBox m_cbCurrentColumn;
	int m_nSelRow;
	int m_nSelColumn;
public:
	afx_msg void OnBnClickedFA();
	afx_msg void OnBnClickedFB();
	afx_msg void OnBnClickedFC();
	//afx_msg void OnBnClickedFD();
	afx_msg void OnEnChangePausePos();
	afx_msg void OnBnClickedBtnFirstRowd();
	afx_msg void OnBnClickedBtnSecondRowd();
	afx_msg void OnBnClickedBtnFirstColumnd();
	afx_msg void OnBnClickedBtnSecondColumnd();
	afx_msg void OnEnChangeEditDispVelXY();
	afx_msg void OnEnChangeEditDispAccXY();
	afx_msg void OnBnClickedRadioDot();
	afx_msg void OnBnClickedRadioRectangle();
	afx_msg void OnBnClickedRadioHelix();
	afx_msg void OnBnClickedRadioLineVertical();
	afx_msg void OnBnClickedRadioLineHorizontal();
	afx_msg void OnBnClickedRadioCustom();
	afx_msg void OnBnClickedRadioImageNo();
	afx_msg void OnBnClickedRadioImageOneMark();
	afx_msg void OnBnClickedRadioImageTwoMark();
	afx_msg void OnEnChangeEditDispVelZ();
	afx_msg void OnEnChangeEditDispAccZ();
	afx_msg void OnBnClickedBtnCustomParam();
	afx_msg void OnBnClickedBtnThirdRowd();
	afx_msg void OnBnClickedBtnThirdColumnd();
	afx_msg void OnCbnSelchangeComboCurrentRow();
	afx_msg void OnCbnSelchangeComboCurrentColumn();
	afx_msg void OnBnClickedBtnSaveCurrentRowd();
	afx_msg void OnBnClickedBtnSaveCurrentColumnd();
	afx_msg void OnBnClickedRadioNoInspect();
	afx_msg void OnBnClickedRadioScanInspect();
	afx_msg void OnBnClickedRadioRowDisp();
	afx_msg void OnBnClickedRadioColumnDisp();
	afx_msg void OnBnClickedRadioFirstCleanLaterTest();
	afx_msg void OnBnClickedRadioFirstTestLaterClean();
	afx_msg void OnBnClickedRadioOneValve();
	afx_msg void OnBnClickedRadioTwoValve();
	afx_msg void OnBnClickedRadioThreeValve();
	afx_msg void OnBnClickedRadioGrayscale();
	afx_msg void OnBnClickedRadioGeometric();
	afx_msg void OnBnClickedRadioTransitionInspect();
	afx_msg void OnBnClickedRadioCircle();
	afx_msg void OnBnClickedRadioSectionInspect();
	afx_msg void OnBnClickedRadioMulti();
	//afx_msg void OnBnClickedBtnNeedlegap();
	CButton m_TempJudge;
	CButton m_HeatEnable;
	CButton m_MesOffLine;
	CButton m_NeedleCheck;
	afx_msg void OnBnClickedRadioComplex();
	afx_msg void OnBnClickedCancel();	
	afx_msg void OnBnClickedCheckMesOffline();	
	afx_msg void OnBnClickedButtonUdpReset();
	afx_msg void OnBnClickedUdpSaveurl();
	CIPAddressCtrl m_IPAddress;
};
