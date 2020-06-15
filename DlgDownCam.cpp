// DlgDownCam.cpp : implementation file
//
#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgDownCam.h"


IMPLEMENT_DYNAMIC(CDlgDownCam, CDialog)

CDlgDownCam::CDlgDownCam(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDownCam::IDD, pParent)
{
	m_nImageWidth = milApp.m_lNeedleBufSizeX;
	m_nImageHeight = milApp.m_lNeedleBufSizeY;

}

CDlgDownCam::~CDlgDownCam()
{
}

void CDlgDownCam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_DOWMCAM, m_picture);
}


BEGIN_MESSAGE_MAP(CDlgDownCam, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgDownCam::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgDownCam::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgDownCam message handlers

BOOL CDlgDownCam::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_rectModelWin=theApp.m_tSysParam.BmpNeedleLearnWin;
	m_rectROI=theApp.m_tSysParam.BmpNeedleROI;
    g_pView->m_ImgStatic.ImgNeedleLive();
	m_picture.m_modtype = MOD_RECTANGLE;
	m_picture.m_iImgWidth = m_nImageWidth;
	m_picture.m_iImgHeight = m_nImageHeight;
	m_picture.m_pImgBuffer = new BYTE[m_picture.m_iImgWidth*m_picture.m_iImgHeight*3];
	m_picture.m_mod_rectangle.dx = m_rectModelWin.TopLeft().x + m_rectModelWin.Width()/2;
	m_picture.m_mod_rectangle.dy = m_rectModelWin.TopLeft().y + m_rectModelWin.Height()/2;
	m_picture.m_mod_rectangle.width = m_rectModelWin.Width();
	m_picture.m_mod_rectangle.height = m_rectModelWin.Height();//图片上MARK框大小更新
	m_picture.m_mod_scale.length = 10;//20 -10
	m_picture.m_mod_scale.width = 10;
	m_picture.m_mod_scale.height = 10;
	m_picture.RoiRect =  m_rectROI;//图片上ROI框大小更新
	m_picture.SetDisplayMarkImage(false);
	m_picture.SetDisplayNeedleImage(true);/////////////////////////////////
	m_picture.yOffset=0;
	m_picture.ImgInit(m_nImageWidth,m_nImageHeight,24);//the distingwish of 8 and 24 in camera;
	theApp.m_Mv400.SetOutput(17,false);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,false);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outNeedleLight,true);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgDownCam::OnBnClickedOk()
{
	theApp.m_Mv400.SetOutput(17,true);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,true);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outNeedleLight,false);
	OnOK();
}

void CDlgDownCam::OnBnClickedCancel()
{
	theApp.m_Mv400.SetOutput(17,true);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,true);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outNeedleLight,false);
	OnCancel();
}
