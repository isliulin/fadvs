#include "resource.h"
#include "imgmarkgeoset.h"
#include "JogButton.h"
#pragma once




// CDlgNeedleTwo dialog

class CDlgNeedleTwo : public CDialog
{
	DECLARE_DYNAMIC(CDlgNeedleTwo)

public:
	CDlgNeedleTwo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNeedleTwo();

// Dialog Data
	enum { IDD = IDD_DLG_NEEDLETWO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CJogButton m_adjustButton[12];
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnLearnmodelNdltwo();
	bool FindModelCenter(CRect ROI);
	CImgMarkGeoSet m_picture;
	CRect m_rectROI;
	CRect m_rectModelWin; 
	int m_nImageWidth;
	int m_nImageHeight;
	double dCenterX,dCenterY;
	afx_msg void OnBnClickedBtnSearchmodelNdltwo();
	afx_msg void OnBnClickedBtnMovetocenterNdltwo();
	
	afx_msg void OnNMCustomdrawSliderNeedleSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnComputescaleNdltwo();
	afx_msg void OnBnClickedBtnFocusTwo();
	void UpdateData();
	afx_msg void OnBnClickedButton2();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
