#pragma once
#include "resource.h"
#include "imgmarkgeoset.h"
#include "JogButton.h"


// CDlgCameraAdj dialog

class CDlgCameraAdj : public CDialog
{
	DECLARE_DYNAMIC(CDlgCameraAdj)

public:
	CDlgCameraAdj(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCameraAdj();

// Dialog Data
	enum { IDD = IDD_DLG_CAMERA_ADJUST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CJogButton m_adjustButton[12];
	CImgMarkGeoSet m_picture;
	CRect ROI;
	CRect LearnWin;
	double dCenterX,dCenterY;
	bool FindModelCenter();
	void AutoCamAdjust();
	bool bAdjust;
	bool bOver;
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNMCustomdrawSliderNeedleSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
