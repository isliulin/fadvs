#include "resource.h"
#include "ImgMarkGeoSet.h"
#include "JogButton.h"
#pragma once



// CDlgNeedleCamera dialog

class CDlgNeedleCamera : public CDialog
{
	DECLARE_DYNAMIC(CDlgNeedleCamera)

public:
	CDlgNeedleCamera(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNeedleCamera();

// Dialog Data
	enum { IDD = IDD_DLG_NEEDLECAMERA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	CImgMarkGeoSet m_picture;
	CJogButton m_adjustButton[12];
public:
	CRect ROI;
	CRect LearnWin;
	afx_msg void OnBnClickedBtnLearnmodelIncam();
	afx_msg void OnBnClickedBtnComputescaleIncam();
	bool FindModelCenter(CRect ROI);
	double dCenterX;
	double dCenterY;
	double dAngle;
	double dScore;
	afx_msg void OnBnClickedBtnSearchmodelIncam();
	afx_msg void OnBnClickedBtnMovetocenterIncam();
	afx_msg void OnNMCustomdrawSliderNeedleSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnPutcross();
};
