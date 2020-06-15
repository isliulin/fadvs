#include "resource.h"
#include "imgmarkgeoset.h"
#include "JogButton.h"
#pragma once




// CDlgNeedleThreeAdjust dialog

class CDlgNeedleThreeAdjust : public CDialog
{
	DECLARE_DYNAMIC(CDlgNeedleThreeAdjust)

public:
	CDlgNeedleThreeAdjust(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNeedleThreeAdjust();

// Dialog Data
	enum { IDD = IDD_DLG_NEEDLETHREE_ADJUST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CJogButton m_adjustButton[12];
	CImgMarkGeoSet m_picture;
	virtual BOOL OnInitDialog();
	int m_nImageWidth;
	int m_nImageHeight;
	int nCount;
	CRect m_rectModelWin;
	CRect m_rectROI;
	double dCenterX,dCenterY;
	bool FindModelCenter();
	bool AutoNeedleAdjust();
	void ButtonClever();
	bool bAdjust;
	bool bOver;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnCNegNdlthreeadjust();
	afx_msg void OnNMCustomdrawSliderNeedleSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnTest();
	void EnableButton(bool bEn);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
