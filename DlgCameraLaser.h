#include "ImgMarkGeoSet.h"
#include "JogButton.h"
#include "afxwin.h"
#include  "resource.h"
#pragma once



// CDlgCameraLaser dialog

class CDlgCameraLaser : public CDialog
{
	DECLARE_DYNAMIC(CDlgCameraLaser)

public:
	CDlgCameraLaser(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCameraLaser();

// Dialog Data
	enum { IDD = IDD_DLG_CAMERALASER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CJogButton m_Button[6];
	CImgMarkGeoSet m_picture;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnNMCustomdrawSliderNeedleSpeed(NMHDR *pNMHDR, LRESULT *pResult);
};
