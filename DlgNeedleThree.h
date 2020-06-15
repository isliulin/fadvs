#include "resource.h"
#include "imgmarkgeoset.h"
#include "JogButton.h"
#pragma once




// CDlgNeedleThree dialog

class CDlgNeedleThree : public CDialog
{
	DECLARE_DYNAMIC(CDlgNeedleThree)

public:
	CDlgNeedleThree(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNeedleThree();

// Dialog Data
	enum { IDD = IDD_DLG_NEEDLETHREE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CJogButton m_adjustButton[12];
	virtual BOOL OnInitDialog();
	CImgMarkGeoSet m_picture;
	afx_msg void OnBnClickedOk();
    bool FindModelCenter(CRect ROI);
	CRect m_rectROI;
	CRect m_rectModelWin;
	int m_nImageWidth;
	int m_nImageHeight;
    double dCenterX,dCenterY;
	afx_msg void OnBnClickedBtnSearchmodelNdlthree();
	afx_msg void OnBnClickedBtnMovetocenterNdlthree();
	afx_msg void OnNMCustomdrawSliderNeedleSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnLearnmodelNdlthree();
	afx_msg void OnBnClickedBtnFocusThree();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
