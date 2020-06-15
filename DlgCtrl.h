#include "ColorButton.h"
#pragma once#include "afxwin.h"



// CDlgCtrl dialog

class CDlgCtrl : public CDialog
{
	DECLARE_DYNAMIC(CDlgCtrl)

public:
	CDlgCtrl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCtrl();
 
// Dialog Data
	enum { IDD = IDD_DLG_CTRL };
	
	CColorButton m_colorButton[10];
	int		m_iSpeed;//0低速, 1中速,2高速
	int		nImagePosID;
	CComboBox m_cbImageSelect;
	double	m_dLaserHeightVal;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	int UPDataVal(void);
public:
	CComboBox m_cbTestIndex;
	int m_nTestIndex;
	CComboBox m_cbHeightNumber;
	int m_nHeightIndex;
	HBITMAP m_hBitmapA;
	HBITMAP m_hBitmapB;
	HBITMAP m_hBitmapC;
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNMCustomdrawSliderSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSaveDispensingPos();
	afx_msg void OnBnClickedMoveDispensingPos();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCancel();
	afx_msg void OnBnClickedMoveCleanPos();
	afx_msg void OnBnClickedSaveCleanPos();
	afx_msg void OnBnClickedClean();
	afx_msg void OnBnClickedMoveStartImagePos();
	afx_msg void OnBnClickedMoveEndImagePos();
	afx_msg void OnBnClickedSaveTestDispPos();
	afx_msg void OnBnClickedMoveTestDispPos();
	afx_msg void OnBnClickedTestDisp();
	afx_msg void OnBnClickedBtnAlignNeedle();
	afx_msg void OnBnClickedBtnMoveSlotDetectPos();
	afx_msg void OnBnClickedBtnSaveSlotDetectPos();
	afx_msg void OnBnClickedBtnMoveDischargePos();
	afx_msg void OnBnClickedBtnSaveDischargePos();
	afx_msg void OnBnClickedBtnDischargeGlue();
	afx_msg void OnBnClickedBtnDefoam();
	afx_msg void OnBnClickedBtnSaveCurrentDispPos();
	afx_msg void OnBnClickedBtnSaveModifyDispPos();
	void OnDlgExit();
	afx_msg void OnCbnSelchangeComboImageSelectIndex();
	afx_msg void OnBnClickedBtnMoveImagePos();
	afx_msg void OnBnClickedBtnSetImagePos();
	afx_msg void OnBnClickedBtnSearchImage();
	afx_msg void OnCbnSelchangeComboTestIndex();
	afx_msg void OnBnClickedSingleTestDispense();
	afx_msg void OnBnClickedSetTestDispense();
	void SetAuthority();
	afx_msg void OnBnClickedBtnCtrlMovetoHeightPos();
	afx_msg void OnBnClickedBtnCtrlSetHeightPos();
	afx_msg void OnBnClickedBtnCtrlMeasureHeight();
	afx_msg void OnCbnSelchangeComboCtrlHeightIndex();
	void SetAuthority(bool bEnable);
	void UpdateSensor();
	afx_msg void OnBnClickedMoveAdjustClean();
	afx_msg void OnBnClickedMoveAdjustNeedle();
	afx_msg void OnBnClickedBtnmodifydischarge();
	afx_msg void OnBnClickedBtnmodifyclean();
	afx_msg void OnBnClickedBtnMoveFocusPos();
	afx_msg void OnBnClickedBtnSaveCurrentFocusPos();
	afx_msg void OnBnClickedBtnSaveModifyFocus();
	afx_msg void OnBnClickedBtnSaveTestzEach();
	CButton m_CheckAutoDefoam;
	afx_msg void OnBnClickedBtnStopDischarge();
	afx_msg void OnBnClickedBtnZoffsetApply();
	afx_msg void OnBnClickedBtnValveHeat();
	afx_msg void OnBnClickedBtnCtrlSaveHeightData();
};
