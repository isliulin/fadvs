#pragma once
#include "ImgMarkGeoSet.h"
#include  "resource.h"


// CDlgDetectBase dialog

class CDlgDetectBase : public CDialog
{
	DECLARE_DYNAMIC(CDlgDetectBase)

public:
	CDlgDetectBase(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDetectBase();

// Dialog Data
	enum { IDD = IDD_DLG_DETECT_BASEPIONT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CImgMarkGeoSet m_picture;
	long m_nImageWidth,m_nImageHeight;
	CRect m_rectROI;
	CRect m_rectModelWin;
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnDetectBasepointNext();
};
