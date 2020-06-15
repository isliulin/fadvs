#include "resource.h"
#include "ColorButton.h"
#include "imgmarkgeoset.h"
#include "JogButton.h"
#include "afxcmn.h"
#pragma once




// CDlgNeedleOne dialog

class CDlgNeedleOne : public CDialog
{
	DECLARE_DYNAMIC(CDlgNeedleOne)

public:
	CDlgNeedleOne(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNeedleOne();

// Dialog Data
	enum { IDD = IDD_DLG_NEEDLEONE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnMovetocenterNdleone();
public:
	CJogButton m_adjustButton[12];
	CImgMarkGeoSet m_picture;
	CRect m_rectROI;
	CRect m_rectModelWin; 
	int m_nImageWidth;
	int m_nImageHeight;
	double dCenterX,dCenterY;
	afx_msg void OnBnClickedBtnLearnmodelNdleone();
	afx_msg void OnBnClickedBtnComputescaleNdleone();
	bool FindModelCenter(CRect ROI);
	afx_msg void OnBnClickedBtnSearchmodelNdleone();
	afx_msg void OnNMCustomdrawSliderNeedleSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CSliderCtrl m_sliderShutter;
	CSliderCtrl m_sliderGain;
	afx_msg void OnEnChangeEditNeedleAcceptance();
	afx_msg void OnEnChangeEditNeedleAngle();
	afx_msg void OnBnClickedCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
