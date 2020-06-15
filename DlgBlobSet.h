#pragma once


// CDlgBlobSet dialog

class CDlgBlobSet : public CDialog
{
	DECLARE_DYNAMIC(CDlgBlobSet)

public:
	CDlgBlobSet(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgBlobSet();

// Dialog Data
	enum { IDD = IDD_DLG_BLOBSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	long lImageLeft;
	long lImageTop;
	long lImageWidth;
	long lImageHeight;
	double dHeightMin;
	double dHeightMax;
	double dWidthMin;
	double dWidthMax;
	double dAreaMin;
	double dAreaMax;
	CRect ROI;
	CPoint ptDown,C1,C2;
	CPoint oldPoint1,oldPoint2;

	bool bPatLearn;
	bool bFirstROI;
	bool bMouseDown,bMouseRect;
	bool bMoveLeft,bMoveRight,bMoveUp,bMoveDown;
	bool bGetROI;
public:
	double dMinX;
	double dMaxX;
	double dMinY;
	double dMaxY;
	double dCenterX;
	double dCenterY;
	CComboBox m_cbSelect;
	CStatic m_picture;
	int nSelectID;

	virtual BOOL OnInitDialog();

	bool FindBlobCenter(CRect ROIF);
	void RefreshPosition();
	afx_msg void OnCbnSelchangeComboBlob();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnBlobComScale();
	afx_msg void OnBnClickedBtnBlobNewPic();
	afx_msg void OnBnClickedBtnBlobSaveRoi();
	afx_msg void OnBnClickedBtnBlobSaveCenter();
	afx_msg void OnBnClickedBtnBlobTest();
	afx_msg void OnBnClickedBtnBlobMoveGrabpos();
	void OverlayLine(int nColor,CPoint p1,CPoint p2);
	afx_msg void OnBnClickedBtnBlobShowRoi();
	afx_msg void OnBnClickedBtnBlobClearRoi();
	afx_msg void OnEnChangeEditBlobRoiLeft();
	afx_msg void OnEnChangeEditBlobRoiRight();
	afx_msg void OnEnChangeEditBlobRoiTop();
	afx_msg void OnEnChangeEditBlobRoiBottom();
};
