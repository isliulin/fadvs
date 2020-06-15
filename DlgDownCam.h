#pragma once
#include "imgmarkgeoset.h"
#include "resource.h"

// CDlgDownCam dialog

class CDlgDownCam : public CDialog
{
	DECLARE_DYNAMIC(CDlgDownCam)

public:
	CDlgDownCam(CWnd* pParent = NULL);   // standard constructor

	virtual ~CDlgDownCam();
// Dialog Data
	enum { IDD = IDD_DLG_DOWN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CRect m_rectROI;
	CRect m_rectModelWin; 
	int m_nImageWidth;
	int m_nImageHeight;
	CImgMarkGeoSet m_picture;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
